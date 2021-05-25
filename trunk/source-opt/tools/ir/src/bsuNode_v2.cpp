// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#include "bsuNode_v2.h"
#include "program_analysis.h"

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsArrayRangeInScope;
using MarsProgramAnalysis::CMarsDepDistSet;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsResultFlags;
using MarsProgramAnalysis::CMarsVariable;
using MarsProgramAnalysis::full_INF;
using std::list;
#define USED_CODE_IN_COVERAGE_TEST 0

CMarsNode::CMarsNode(CSageCodeGen *codegen, const vector<void *> &stmts,
                     const vector<void *> &loops,
                     const map<void *, vector<pair<void *, bool>>> &loop2conds,
                     const vector<pair<void *, bool>> &conds,
                     const vector<int> &orders,
                     const t_func_call_path &call_path, void *kernel)
    : mLoops(loops), mLoop2Conds(loop2conds), mInnermostConds(conds),
      mStmts(stmts), mCallPath(call_path), m_ast(codegen), mUserKernel(kernel) {
  mOrders.resize((orders.size() - 1) * 2 + 1, 0);
  mSchedule_depth.resize(loops.size() * 2 + 2);
  for (size_t i = 0; i != orders.size() - 1; ++i) {
    mOrders[i * 2] = orders[i];
  }
  mOrders[orders.size() * 2 - 2] = orders.back();
  update_reference();
}

void CMarsNode::update_reference() {
  //  1. analyze variable reference
  SetVector<void *> all_refs;
  for (auto &stmt : mStmts) {
    vector<void *> sub_refs;
    m_ast->GetNodesByType_int(stmt, "preorder", V_SgVarRefExp, &sub_refs);
    all_refs.insert(sub_refs.begin(), sub_refs.end());
  }
  for (auto &loop : mLoops) {
    void *loop_init = m_ast->GetLoopInit(loop);
    if (loop_init != nullptr) {
      vector<void *> sub_refs;
      m_ast->GetNodesByType_int(loop_init, "preorder", V_SgVarRefExp,
                                &sub_refs);
      all_refs.insert(sub_refs.begin(), sub_refs.end());
    }
    void *loop_test = m_ast->GetLoopTest(loop);
    if (loop_test != nullptr) {
      vector<void *> sub_refs;
      m_ast->GetNodesByType_int(loop_test, "preorder", V_SgVarRefExp,
                                &sub_refs);
      all_refs.insert(sub_refs.begin(), sub_refs.end());
    }
    void *loop_incr = m_ast->GetLoopIncrementExpr(loop);
    if (loop_incr != nullptr) {
      vector<void *> sub_refs;
      m_ast->GetNodesByType_int(loop_incr, "preorder", V_SgVarRefExp,
                                &sub_refs);
      all_refs.insert(sub_refs.begin(), sub_refs.end());
    }
  }
  for (auto &it : mLoop2Conds) {
    vector<pair<void *, bool>> conds = it.second;
    for (auto &jt : conds) {
      void *if_stmt = jt.first;
      void *cond = m_ast->GetIfStmtCondition(if_stmt);
      if (cond != nullptr) {
        vector<void *> sub_refs;
        m_ast->GetNodesByType_int(cond, "preorder", V_SgVarRefExp, &sub_refs);
        all_refs.insert(sub_refs.begin(), sub_refs.end());
      }
    }
  }
  //  reset
  mPorts.clear();
  mPort2AccessType.clear();
  mPort2Pntr.clear();
  mPort2Refs.clear();
  //  calculate
  for (auto &var_ref : all_refs) {
    void *sg_pntr = nullptr;
    void *var_init = nullptr;
    int pointer_dim = 0;
    vector<void *> sg_indexes;
    m_ast->parse_pntr_ref_by_array_ref(var_ref, &var_init, &sg_pntr,
                                       &sg_indexes, &pointer_dim, var_ref);
    //  filter loop iterators
    //  if (m_ast->GetLoopFromIteratorByPos(var_init, var_ref, 1) != nullptr)
    if (m_ast->GetLoopFromIteratorByPos(var_init, var_ref) != nullptr) {
      continue;
    }
    void *top_var_init = get_top_instance(var_init);
    if (top_var_init != nullptr) {
      var_init = top_var_init;
    }
    mPorts.insert(var_init);
    if (m_ast->is_altera_channel_write(sg_pntr) != 0) {
      mPort2AccessType[var_init] =
          static_cast<access_type>(mPort2AccessType[var_init] | WRITE);
    } else if (m_ast->is_altera_channel_read(sg_pntr) != 0) {
      mPort2AccessType[var_init] =
          static_cast<access_type>(mPort2AccessType[var_init] | READ);
    } else {
      mPort2AccessType[var_init] = static_cast<access_type>(
          mPort2AccessType[var_init] |
          (m_ast->has_write_conservative(sg_pntr) != 0 ? WRITE : 0) |
          (m_ast->has_read_conservative(sg_pntr) != 0 ? READ : 0));
    }
    mPort2Pntr[var_init].insert(sg_pntr);
    mPort2Refs[var_init].insert(var_ref);
  }
}

void *CMarsNode::get_top_instance(void *var_init) {
  auto call_iter = mCallPath.begin();
  while (true) {
    if (m_ast->IsArgumentInitName(var_init) == 0) {
      return var_init;
    }
    void *func_decl = m_ast->TraceUpToFuncDecl(var_init);
    if (func_decl == nullptr) {
      return var_init;
    }
    if (m_ast->isSameFunction(func_decl, mUserKernel)) {
      return var_init;
    }
    int idx = m_ast->GetFuncParamIndex(var_init);
    assert(-1 != idx);
    while (call_iter != mCallPath.end()) {
      void *callee = call_iter->first;
      if (m_ast->isSameFunction(func_decl, callee)) {
        break;
      }
      ++call_iter;
    }
    if (call_iter == mCallPath.end()) {
      break;
    }
    void *actual_exp = m_ast->GetFuncCallParam(call_iter->second, idx);
    m_ast->remove_cast(&actual_exp);
    if (m_ast->IsVarRefExp(actual_exp) == 0) {
      return var_init;
    }
    var_init = m_ast->GetVariableInitializedName(actual_exp);
    ++call_iter;
  }
  return nullptr;
}
void CMarsNode::print() {
  string indent;
  for (size_t i = 0; i != mLoops.size(); ++i) {
    cout << indent << "(sync point, pipeline stage) = (" << mOrders[i * 2]
         << "," << mOrders[i * 2 + 1] << ") " << m_ast->_p(mLoops[i]) << endl;
    indent += "  ";
  }
  cout << indent << "(sync point = " << mOrders.back() << ") {" << endl;
  indent += "  ";
  cout << indent << "Ports: ";
  bool first = true;
  for (auto i = mPorts.begin(); i != mPorts.end(); ++i) {
    void *port = *i;
    access_type at = mPort2AccessType[port];
    string at_str =
        (at == READ) ? "readonly" : (at == WRITE) ? "writeonly" : "readwrite";
    cout << (first ? "" : ", ");
    cout << "(" << m_ast->_p(*i) << ", " << at_str << ")";
    first = false;
  }
  cout << endl;
  for (size_t i = 0; i != mStmts.size(); ++i) {
    cout << indent << m_ast->_p(mStmts[i]) << endl;
  }
  indent = indent.substr(0, indent.size() - 2);
  cout << indent << "}" << endl;
}

void CMarsLoop::analyze_pragma(void *pragma_decl) {
  SgPragmaDeclaration *decl =
      isSgPragmaDeclaration(static_cast<SgNode *>(pragma_decl));
  if (decl == nullptr) {
    return;
  }
  bool errorOut = false;
  CAnalPragma ana_pragma(m_ast);
  if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut)) {
    pragma_table.insert(pragma_decl);
    vec_pragma.push_back(ana_pragma);
  }
}

vector<void *> CMarsNode::get_inner_loops() const {
  vector<void *> inner_loops;
  set<void *> all_decls;
  vector<void *> vec_calls;
  if (!mLoops.empty()) {
    vector<void *> sub_loops;
    m_ast->GetNodesByType_int(m_ast->GetLoopBody(mLoops.back()), "preorder",
                              V_SgForStatement, &sub_loops);
    inner_loops.insert(inner_loops.begin(), sub_loops.begin(), sub_loops.end());
    m_ast->GetNodesByType_int(m_ast->GetLoopBody(mLoops.back()), "preorder",
                              V_SgFunctionCallExp, &vec_calls);
  } else {
    for (auto &stmt : mStmts) {
      vector<void *> sub_loops;
      m_ast->GetNodesByType_int(stmt, "preorder", V_SgForStatement, &sub_loops);
      inner_loops.insert(inner_loops.end(), sub_loops.begin(), sub_loops.end());
      vector<void *> sub_calls;
      m_ast->GetNodesByType_int(stmt, "preorder", V_SgFunctionCallExp,
                                &sub_calls);
      vec_calls.insert(vec_calls.end(), sub_calls.begin(), sub_calls.end());
    }
  }
  for (auto one_call : vec_calls) {
    void *func_decl = m_ast->GetFuncDeclByCall(one_call, 1);
    if (func_decl == nullptr) {
      continue;
    }
    if (all_decls.count(func_decl) > 0) {
      continue;
    }
    all_decls.insert(func_decl);
    SetVector<void *> sub_decls;
    SetVector<void *> sub_calls;
    m_ast->GetSubFuncDeclsRecursively(func_decl, &sub_decls, &sub_calls);
    all_decls.insert(sub_decls.begin(), sub_decls.end());
  }
  for (auto func_decl : all_decls) {
    vector<void *> sub_loops;
    m_ast->GetNodesByType_int(func_decl, "preorder", V_SgForStatement,
                              &sub_loops);
    inner_loops.insert(inner_loops.end(), sub_loops.begin(), sub_loops.end());
  }
  return inner_loops;
}

vector<void *> CMarsNode::get_inner_loops(void *ref) {
  vector<void *> inner_loops = get_inner_loops();
  vector<void *> inner_loops_in_scope;
  CMarsAST_IF *ast = get_ast();
  for (auto inner_loop : inner_loops) {
    if (ast->is_located_in_scope(ref, inner_loop) != 0) {
      inner_loops_in_scope.push_back(inner_loop);
    }
  }
  return inner_loops_in_scope;
}

t_func_call_path CMarsNode::get_call_path_in_scope(void *scope) {
  CMarsAST_IF *ast = get_ast();
  auto call_iter = mCallPath.begin();
  while (call_iter != mCallPath.end()) {
    void *func_call = call_iter->second;
    if (func_call == nullptr) {
      call_iter++;
      continue;
    }
    if (ast->is_located_in_scope(func_call, scope) == 0) {
      break;
    }
    call_iter++;
  }
  t_func_call_path range_call_path(mCallPath.begin(), call_iter);
  range_call_path.insert(range_call_path.begin(),
                         std::make_pair(nullptr, nullptr));
  range_call_path.insert(range_call_path.end(), std::make_pair(scope, nullptr));
  return range_call_path;
}

vector<CMarsRangeExpr> CMarsNode::get_port_access_union(void *port, void *scope,
                                                        bool read) {
  auto refs = get_port_refs(port);
  CMarsAST_IF *ast = get_ast();

  vector<CMarsRangeExpr> range;
  if (ast->IsScalarType(ast->GetTypebyVar(port)) != 0) {
    return range;
  }
  void *range_scope = scope;
  if (range_scope == nullptr) {
    range_scope = ast->GetFuncBody(get_user_kernel());
  }
  list<t_func_call_path> vec_call_paths;
  vec_call_paths.push_back(get_call_path_in_scope(range_scope));
  vector<void *> vec_pos(refs.begin(), refs.end());
  void *common_scope = ast->GetCommonScope(vec_pos);
  if (common_scope != nullptr) {
    vector<void *> vec_refs;
    vector<void *> var_refs;
    ast->GetNodesByType_int(common_scope, "preorder", V_SgVarRefExp, &var_refs);
    for (auto ref : var_refs) {
      void *var_init = ast->GetVariableInitializedName(ref);
      void *top_var_init = get_top_instance(var_init);
      if (top_var_init == port) {
        vec_refs.push_back(ref);
      }
    }
    set<void *> all_refs(vec_refs.begin(), vec_refs.end());
    if (all_refs == refs) {
      CMarsArrayRangeInScope acc(port, ast, vec_call_paths, common_scope,
                                 range_scope, false, false);
      if (read && (acc.has_read() != 0)) {
        return acc.GetRangeExprRead();
      }
      if (!read && (acc.has_write() != 0)) {
        return acc.GetRangeExprWrite();
      }
      return range;
    }
  }
  for (auto ref : refs) {
    void *sg_pntr = nullptr;
    void *var_init = nullptr;
    int pointer_dim = 0;
    vector<void *> sg_indexes;
    ast->parse_pntr_ref_by_array_ref(ref, &var_init, &sg_pntr, &sg_indexes,
                                     &pointer_dim, ref);
    if (read && (ast->has_read_conservative(sg_pntr) == 0)) {
      continue;
    }
    if (!read && (ast->has_write_conservative(sg_pntr) == 0)) {
      continue;
    }

    CMarsArrayRangeInScope acc(port, ast, vec_call_paths, ref, range_scope,
                               false, false);
    vector<CMarsRangeExpr> one_range;
    if (read && (acc.has_read() != 0)) {
      one_range = acc.GetRangeExprRead();
    } else if (!read && (acc.has_write() != 0)) {
      one_range = acc.GetRangeExprWrite();
    } else {
      continue;
    }

    range = RangeUnioninVector(range, one_range);
  }
  return range;
}

bool CMarsNode::is_after(CMarsNode *other) {
  if (this == other) {
    return false;
  }
  if (get_user_kernel() != other->get_user_kernel()) {
    return false;
  }
  vector<int> orders = other->get_orders();
  int min_depth = std::min(orders.size(), mOrders.size());
  for (int i = 0; i != min_depth; ++i) {
    if (mOrders[i] > orders[i]) {
      return true;
    }
    if (mOrders[i] < orders[i]) {
      return false;
    }
  }
  assert(0 && "cannot determine their location relation");
  return false;
}

#if USED_CODE_IN_COVERAGE_TEST
string CMarsNode::unparse() {
  void *node = (mLoops.size() > 0) ? mLoops[mLoops.size() - 1] : mStmts[0];
  return string("Line ") + my_itoa(get_ast()->get_line(node)) + ":" +
         get_ast()->_p(node);
}
#endif

vector<void *> CMarsNode::get_common_loops(CMarsNode *other) {
  int min_loop_depth = std::min(other->get_loop_depth(), get_loop_depth());
  vector<void *> res;
  for (int i = 0; i != min_loop_depth; ++i) {
    if (get_loop(i) == other->get_loop(i)) {
      res.push_back(get_loop(i));
    } else {
      return res;
    }
  }
  return res;
}

bool CMarsNode::has_common_loops(CMarsNode *other) {
  int min_loop_depth = std::min(other->get_loop_depth(), get_loop_depth());
  for (int i = 0; i != min_loop_depth; ++i) {
    if (get_loop(i) == other->get_loop(i)) {
      return true;
    }
  }
  return false;
}

bool CMarsNode::is_real_shared_port(CMarsNode *other, void *port) {
  t_func_call_path call_path = mCallPath;
  t_func_call_path other_call_path = other->get_call_path();
  if (m_ast->IsGlobalInitName(port) != 0) {
    return true;
  }
  void *func_decl = m_ast->TraceUpToFuncDecl(port);
  void *kernel = get_user_kernel();
  if (m_ast->isSameFunction(func_decl, kernel)) {
    return true;
  }
  auto r_call_iter = call_path.rbegin();
  auto r_call_iter_other = other_call_path.rbegin();
  while (r_call_iter != call_path.rend() &&
         r_call_iter_other != other_call_path.rend()) {
    if (*r_call_iter != *r_call_iter_other) {
      break;
    }
    void *callee = r_call_iter->first;
    if (m_ast->isSameFunction(callee, func_decl)) {
      return true;
    }
    r_call_iter++;
    r_call_iter_other++;
  }
  return false;
}

set<void *> CMarsNode::get_common_ports(CMarsNode *other) {
  auto ports = other->get_ports();
  set<void *> set_ports(ports.begin(), ports.end());
  set<void *> set_curr_ports(mPorts.begin(), mPorts.end());
  set<void *> shared_ports;
  std::set_intersection(set_curr_ports.begin(), set_curr_ports.end(),
                        set_ports.begin(), set_ports.end(),
                        std::inserter(shared_ports, shared_ports.begin()));
  set<void *> res;
  for (auto &port : shared_ports) {
    bool write0 = port_is_write(port);
    bool write1 = other->port_is_write(port);
    //  both are read
    if (!write0 && !write1) {
      continue;
    }
    //  check whether the port is actually local port according to call path
    if (!is_real_shared_port(other, port)) {
      continue;
    }
#if 0
        //  FIXME: we only handle scalar variables
        auto type = m_ast->GetTypebyVar(port);
        if (!m_ast->IsScalarType(type)) {
            res.insert(port);
            continue;
        }
        auto ref0 = get_port_references(port);
        auto ref1 = other->get_port_references(port);
        bool included = false;
        for (auto &ref : ref0) {
            if (m_ast->is_altera_channel_read(ref) ||
                    m_ast->is_altera_channel_write(ref)) {
                included = true;
                break;
            }
            if (!m_ast->has_read_conservative(ref)) continue;
            //  check whether read ref defs are included in other node
            vector<void*> vec_def = m_ast->GetVarDefbyPosition(port, ref);
            if (vec_def.size() == 0) {
                included = true;
                break;
            }
            for (auto &def : vec_def) {
                vector<void*> vec_refs;
                m_ast->GetNodesByType_int(
                    def, "preorder", V_SgVarRefExp, &vec_refs);
                for (auto &def_ref : vec_refs) {
                    if (m_ast->GetVariableInitializedName(def_ref) == port &&
                            ref0.count(def_ref) <= 0 &&
                            ref1.count(def_ref) > 0) {
                        included = true;
                        break;
                    }
                }
                if (included) break;
            }
        }
        if (included) {
            res.insert(port);
            continue;
        }
        for (auto &ref : ref1) {
            if (!m_ast->has_read_conservative(ref)) continue;
            vector<void*> vec_def = m_ast->GetVarDefbyPosition(port, ref);
            if (vec_def.size() == 0) {
                included = true;
                break;
            }
            for (auto &def : vec_def) {
                vector<void*> vec_refs;
                m_ast->GetNodesByType_int(
                    def, "preorder", V_SgVarRefExp, &vec_refs);
                for (auto &def_ref : vec_refs) {
                    if (m_ast->GetVariableInitializedName(def_ref) == port &&
                            ref0.count(def_ref) > 0 &&
                            ref1.count(def_ref) <= 0) {
                        included = true;
                        break;
                    }
                }
                if (included) break;
            }
        }
        if (included) {
            res.insert(port);
            continue;
        }
#endif
    res.insert(port);
  }
  return res;
}

void CMarsNode::get_dependency(CMarsNode *other, map<void *, dep_type> *res) {
  set<void *> shared_ports = get_common_ports(other);
  bool after = is_after(other);
  bool feed_back = has_common_loops(other);
  for (set<void *>::iterator i = shared_ports.begin(); i != shared_ports.end();
       ++i) {
    void *port = *i;
    access_type one_access = get_port_access_type(port);
    access_type other_access = other->get_port_access_type(port);
    int dep_res = 1 << (after ? ((one_access << 2) | other_access)
                              : ((other_access << 2) | one_access));
    if (feed_back) {
      //  feedback dependency?
      dep_res |= 1 << (after ? ((other_access << 2) | one_access)
                             : ((one_access << 2) | other_access));
    }
    (*res)[port] = static_cast<dep_type>(dep_res);
  }
}

#if 0
//  no use
bool CMarsNode::has_direct_write_dependency(CMarsNode *other) {
  map<void *, dep_type> res;
  get_dependency(other, &res);
  for (map<void *, dep_type>::iterator i = res.begin(); i != res.end(); ++i) {
    dep_type dep = i->second;
    if (IS_WRITE_DEP(dep))
      return true;
  }
  return false;
}
#endif

bool CMarsNode::is_subNode(void *kernel_top, const vector<int> &order_prefix) {
  if (!m_ast->isSameFunction(kernel_top, mUserKernel)) {
    return false;
  }

#if USED_CODE_IN_COVERAGE_TEST
  //  Currently the use of order_prefix is always null
  for (size_t i = 0; i != order_prefix.size(); ++i) {
    if (i == mOrders.size())
      return false;
    if (mOrders[i] != order_prefix[i])
      return false;
  }
#endif
  return true;
}

void CMarsNode::insert_statement(void *new_stmt, void *old_stmt, bool before) {
  size_t old_size = mStmts.size();
  size_t i = 0;
  while (i < old_size && mStmts[i] != old_stmt) {
    ++i;
  }
  if (i == old_size) {
    //  old_stmt should be in the complex statements, such
    //  as if-stat
    return;
  }
  mStmts.resize(old_size + 1);
  //  assert(i < old_size && "cannot find the old statment");
  if (!before) {
    ++i;
  }
  size_t j = old_size - 1;
  while (j >= i) {
    mStmts[j + 1] = mStmts[j];
    if (j == 0) {
      break;
    }
    --j;
  }
  mStmts[i] = new_stmt;
}

#if USED_CODE_IN_COVERAGE_TEST
void get_dependency(const vector<CMarsNode *> &one_group,
                    const vector<CMarsNode *> &other_group,
                    map<void *, dep_type> *res) {
  for (size_t i = 0; i != one_group.size(); ++i)
    for (size_t j = 0; j != other_group.size(); ++j) {
      map<void *, dep_type> sub_res;
      one_group[i]->get_dependency(other_group[j], &sub_res);
      for (map<void *, dep_type>::iterator k = sub_res.begin();
           k != sub_res.end(); ++k) {
        void *port = k->first;
        map<void *, dep_type>::iterator m = res->find(port);
        if (m == res->end()) {
          (*res)[port] = k->second;
        } else {
          (*res)[port] = (dep_type)(k->second | m->second);
        }
      }
    }
  return;
}
#endif

void CMarsNode::code_generation(void *insert_before) {
  void *body = m_ast->CreateBasicBlock();
  if (nullptr == insert_before) {
    insert_before = mStmts[0];
  }
  for (size_t i = 0; i != mStmts.size(); ++i) {
    void *stmt = mStmts[i];
    m_ast->AppendChild(body, m_ast->CopyStmt(stmt));
  }
  for (size_t i = 0; i != mInnermostConds.size(); ++i) {
    void *if_stmt = mInnermostConds[i].first;
    bool flag = mInnermostConds[i].second;
    void *cond = m_ast->GetExprFromStmt(m_ast->GetIfStmtCondition(if_stmt));
    if (cond != nullptr) {
      cond = m_ast->CopyExp(cond);
      if (!flag) {
        cond = m_ast->CreateExp(V_SgNotOp, cond);
      }
      void *new_if_stmt = m_ast->CreateIfStmt(cond, body, nullptr);
      body = m_ast->CreateBasicBlock();
      m_ast->AppendChild(body, new_if_stmt);
    }
  }
  void *last_stmt = nullptr;
  if (!mLoops.empty()) {
    for (int i = static_cast<int>(mLoops.size() - 1); i >= 0; --i) {
      void *loop = mLoops[i];
      vector<pair<void *, bool>> conds = mLoop2Conds[loop];
      void *sg_init = m_ast->GetLoopInit(loop);
      void *sg_test = m_ast->GetLoopTest(loop);
      void *sg_incr = m_ast->GetLoopIncrementExpr(loop);
      void *new_loop = m_ast->CreateForLoop(m_ast->CopyStmt(sg_init),
                                            m_ast->CopyStmt(sg_test),
                                            m_ast->CopyExp(sg_incr), body);
      last_stmt = new_loop;
      for (int j = static_cast<int>(conds.size() - 1); j >= 0; --j) {
        void *if_stmt = conds[j].first;
        bool flag = conds[j].second;
        void *cond = m_ast->GetExprFromStmt(m_ast->GetIfStmtCondition(if_stmt));
        if (cond != nullptr) {
          cond = m_ast->CopyExp(cond);
          if (!flag) {
            cond = m_ast->CreateExp(V_SgNotOp, cond);
          }
          body = m_ast->CreateBasicBlock();
          m_ast->AppendChild(body, last_stmt);
          last_stmt = m_ast->CreateIfStmt(cond, body, nullptr);
        }
      }
      if (i > 0) {
        body = m_ast->CreateBasicBlock();
        m_ast->AppendChild(body, last_stmt);
      }
    }
  } else {
    //  fake a dummy loop to be friendly with outline
    string iter = "i";
    void *func_decl = m_ast->TraceUpToFuncDecl(insert_before);
    void *func_body = m_ast->GetFuncBody(func_decl);
    m_ast->get_valid_local_name(func_decl, &iter);
    void *iter_decl =
        m_ast->CreateVariableDecl("int", iter, nullptr, func_body);
    m_ast->PrependChild(func_body, iter_decl);
    void *iter_var = m_ast->GetVariableInitializedName(iter_decl);
    last_stmt =
        m_ast->CreateStmt(V_SgForStatement, iter_var, m_ast->CreateConst(0),
                          m_ast->CreateConst(1), body, nullptr);
  }
  if (last_stmt != nullptr) {
    m_ast->InsertStmt(last_stmt, insert_before);
    //  renaming all the iterators
    map<void *, void *> old_iter2new_iter;
    void *kernel_func = m_ast->TraceUpToFuncDecl(insert_before);
    void *func_body = m_ast->GetFuncBody(kernel_func);
    for (auto &loop : mLoops) {
      void *loop_iter = m_ast->GetLoopIterator(loop);
      if (loop_iter != nullptr) {
        string new_iter_str = m_ast->GetVariableName(loop_iter);
        m_ast->get_valid_local_name(kernel_func, &new_iter_str);
        void *new_iter = m_ast->CreateVariableDecl(
            m_ast->GetTypebyVar(loop_iter), new_iter_str, nullptr, func_body);
        m_ast->PrependChild(func_body, new_iter);
        old_iter2new_iter[loop_iter] = new_iter;
      }
    }
    vector<void *> vec_refs;
    m_ast->GetNodesByType_int(last_stmt, "preorder", V_SgVarRefExp, &vec_refs);
    for (auto &ref : vec_refs) {
      void *var_init = m_ast->GetVariableInitializedName(ref);
      if (old_iter2new_iter.count(var_init) > 0) {
        void *new_var = old_iter2new_iter[var_init];
        m_ast->ReplaceExp(ref, m_ast->CreateVariableRef(new_var));
      }
    }
  }
}

void CMarsNode::remove_all_statements(bool keep_assert_stmt) {
  vector<void *> vec_stmts;
  for (auto &stmt : mStmts) {
    if (keep_assert_stmt) {
      vector<void *> vec_call;
      m_ast->GetNodesByType_int(stmt, "preorder", V_SgFunctionCallExp,
                                &vec_call);
      bool contain_assert = false;
      for (auto &call : vec_call) {
        if (m_ast->IsAssertFailCall(call)) {
          contain_assert = true;
          break;
        }
      }
      if (contain_assert) {
        vec_stmts.push_back(stmt);
        continue;
      }
    }
    if ((m_ast->IsPragmaDecl(stmt) != 0) ||
        (m_ast->IsVariableDecl(stmt) != 0)) {
      vec_stmts.push_back(stmt);
      continue;
    }
    m_ast->RemoveStmt(stmt);
  }
  mStmts = vec_stmts;
}
#if 0
vector<CMarsRangeExpr> CMarsNode::get_port_access_union(void * port) {
    auto refs = get_port_refs(port);
    CMarsAST_IF *ast = get_ast();

    vector<CMarsRangeExpr> range;
    if (ast->IsScalarType(ast->GetTypebyVar(port)))
      return range;
    vector<void*> vec_pos(refs.begin(), refs.end());
    void *common_scope = ast->GetCommonScope(vec_pos);
    if (common_scope) {
      vector<void*> vec_refs;
      ast->get_ref_in_scope(port, common_scope, &vec_refs);
      set<void*> all_refs(vec_refs.begin(), vec_refs.end());
      if (all_refs == refs) {
        CMarsArrayRangeInScope acc(
            port, ast, common_scope, get_user_kernel(), false, false);
        if (acc.has_read())
          range = RangeUnioninVector(range, acc.GetRangeExprRead());
        if (acc.has_write())
          range = RangeUnioninVector(range, acc.GetRangeExprWrite());
        return range;
      }
    }
    for (auto ref : refs) {
        CMarsArrayRangeInScope acc(ref, ast, get_user_kernel(), false, false);
        vector<CMarsRangeExpr> one_range = acc.has_read() ?
              acc.GetRangeExprRead() : acc.GetRangeExprWrite();

        range = RangeUnioninVector(range, one_range);
    }
    return range;
}
#endif

int get_sync_level_from_schedule(CMarsScheduleVector sch0,
                                 CMarsScheduleVector sch1) {
  int sync_level = 0;
  for (size_t i = 0; i < sch0.size() && i < sch1.size(); i++) {
    if (sch0[i] != sch1[i]) {
      break;
    }
    sync_level++;
  }
  return sync_level;
}

#if USED_CODE_IN_COVERAGE_TEST
//  Assumptions:
//  a) if statement, not else
//  b) the condition is i==0 or i==N-1, i==...
//  Input: pos can be an referece or a statement
bool CMarsNode::condition_is_simple(void *pos) {
  CMarsAST_IF *ast = get_ast();
  void *curr_pos = pos;
  void *pre_pos = nullptr;

  while (curr_pos) {
    if (ast->IsFunctionDeclaration(curr_pos))
      break;

    if (!ast->IsIfStatement(curr_pos))
      continue;

    //  a) if statement, not else
    if (pre_pos != ast->GetIfStmtTrueBody(curr_pos))
      return 0;

    //  b) the condition is i==0 or i==N-1
    void *cond = ast->GetIfStmtCondition(curr_pos);
    void *cond_exp = ast->GetExprFromStmt(cond);
    vector<void *> conds;
    ast->GetExprListFromAndOp(cond_exp, &conds);
    for (auto &one_cond : conds) {
      if (!ast->IsEqualOp(one_cond) && !ast->IsNonEqualOp(one_cond))
        return 0;
      void *var = ast->GetExpLeftOperand(one_cond);
      if (!ast->IsVarRefExp(var))
        return 0;
      void *loop = ast->GetLoopFromIteratorRef(var);
      if (!ast->IsForStatement(loop))
        return 0;
    }
    pre_pos = curr_pos;
    curr_pos = ast->GetParent(curr_pos);
  }

  return 1;
}

//  for-loop -> position (0: before the loop, 1: after the loop, 2: in the
//  middle iterations, 3: unknown pos)
map<void *, pair<int, bool>> CMarsNode::get_simple_condition(void *pos) {
  CMarsAST_IF *ast = get_ast();
  map<void *, pair<int, bool>> res;

  void *curr_pos = pos;
  void *pre_pos = nullptr;

  while (curr_pos) {
    if (ast->IsFunctionDeclaration(curr_pos))
      break;

    if (!ast->IsIfStatement(curr_pos)) {
      pre_pos = curr_pos;
      curr_pos = ast->GetParent(curr_pos);
      continue;
    }

    //  a) if statement, not else
    if (pre_pos != ast->GetIfStmtTrueBody(curr_pos) &&
        pre_pos != ast->GetIfStmtFalseBody(curr_pos))
      assert(false);
    bool true_branch = pre_pos == ast->GetIfStmtTrueBody(curr_pos);
    //  b) the condition is i==0 or i==N-1
    void *cond = ast->GetIfStmtCondition(curr_pos);
    void *cond_exp = ast->GetExprFromStmt(cond);
    vector<void *> conds;
    if (true_branch)
      ast->GetExprListFromAndOp(cond_exp, &conds);
    else
      ast->GetExprListFromOrOp(cond_exp, &conds);
    for (auto &one_cond : conds) {
#if PROJDEBUG
      cout << "cond: " << ast->_p(one_cond) << endl;
#endif
      if (!ast->IsEqualOp(one_cond) && !ast->IsNonEqualOp(one_cond))
        assert(false);
      bool equal = ast->IsEqualOp(one_cond);
      void *var = ast->GetExpLeftOperand(one_cond);
      if (!ast->IsVarRefExp(var))
        assert(false);
      void *loop = ast->GetLoopFromIteratorRef(var);
      if (!ast->IsForStatement(loop))
        assert(false);

      void *value = ast->GetExpRightOperand(one_cond);

      CMarsExpression me_value(value, ast);
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      {
        CMarsRangeExpr mr = CMarsVariable(loop, ast).get_range();
        int ret = mr.get_simple_bound(me_lb, me_ub);
        assert(ret);
      }

      int cond_pos;
      if (me_lb - me_value == 0)
        cond_pos = 0;
      else if (me_ub - me_value == 0)
        cond_pos = 1;
      else if ((me_value - me_lb).IsNonNegative() &&
               (me_ub - me_value).IsNonNegative())
        cond_pos = 2;
      else
        cond_pos = 3;

      res[loop] = pair<int, bool>(cond_pos, !(equal ^ true_branch));
    }
    pre_pos = curr_pos;
    curr_pos = ast->GetParent(curr_pos);
  }

  return res;
}
#endif

CMarsExpression
CMarsNode::get_iteration_domain_size_by_port_and_level(void *port,
                                                       int sync_level) {
  //  Add by Yuxin, as a work around for comm_token
  //  auto type = m_ast->GetTypebyVar(port);
  //  if (m_ast->IsScalarType(type)) return 1;

  auto refs = get_port_references(port);
  assert(refs.size() ==
         1);  //  FIXME: support only one reference for channel ports
  void *ref = *(refs.begin());
  CMarsAST_IF *ast = get_ast();

  //  1. get the for-loops
  auto loops = get_loops();
  CMarsExpression ret_size(ast, 1);

#if 0
    //  2. get if-conditions: assumptions on the conditions
    //  a) if statement, not else
    //  b) the condition is i==0 or i==N-1
    //  auto loop_cond = get_simple_condition(ref);


    for (size_t i =(1+sync_level)/2; i < loops.size(); i++) {
        auto loop = loops[i];
        if (loop_cond.find(loop) == loop_cond.end()) {
            CMarsRangeExpr mr = CMarsVariable(loop, ast).get_range();
            CMarsExpression me_lb;
            CMarsExpression me_ub;
            int ret = mr.get_simple_bound(me_lb, me_ub);
            assert(ret);
            CMarsExpression me_tripcount = me_ub - me_lb + 1;
            assert(me_tripcount.IsConstant());

            int loop_tripcount = me_tripcount.get_const();
            ret_size *= loop_tripcount;
        } else {
            auto cond = loop_cond[loop];
            if (cond.second) continue;
            CMarsRangeExpr mr = CMarsVariable(loop, ast).get_range();
            CMarsExpression me_lb;
            CMarsExpression me_ub;
            int ret = mr.get_simple_bound(me_lb, me_ub);
            assert(ret);
            CMarsExpression me_tripcount = me_ub - me_lb;
            assert(me_tripcount.IsConstant());

            int loop_tripcount = me_tripcount.get_const();
            ret_size *= loop_tripcount;
        }
    }
#else
  //  exclude outer common loops whose level is less than or equal
  //  to sync_level / 2
  for (size_t i = sync_level / 2; i < loops.size(); i++) {
    auto loop = loops[i];
    CMarsRangeExpr mr = CMarsVariable(loop, ast, ref).get_range();
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    if (mr.is_empty() != 0) {
      return CMarsExpression(ast, 0);
    }
    int ret = mr.get_simple_bound(&me_lb, &me_ub);
    if (ret == 0) {
      return full_INF;
    }
    assert(ret);
    CMarsExpression me_tripcount = me_ub - me_lb + 1;
    ret_size = ret_size * me_tripcount;
  }
#endif

  return ret_size;
}

#if USED_CODE_IN_COVERAGE_TEST
CMarsExpression CMarsNode::get_iteration_domain_size_by_port_and_level(
    void *port, int sync_level, int parallel_level) {
  //  Add by Yuxin, as a work around for comm_token
  //  auto type = m_ast->GetTypebyVar(port);
  //  if (m_ast->IsScalarType(type)) return 1;

  auto refs = get_port_references(port);
  assert(refs.size() ==
         1);  //  FIXME: support only one reference for channel ports
  void *ref = *(refs.begin());
  CMarsAST_IF *ast = get_ast();

  //  1. get the for-loops
  auto loops = get_loops();
#if 0
    //  2. get if-conditions: assumptions on the conditions
    //  a) if statement, not else
    //  b) the condition is i==0 or i==N-1
    auto loop_cond = get_simple_condition(ref);

    int64_t ret_size = 1;

    for (size_t i =(1+sync_level)/2; i < loops.size() - parallel_level; i++) {
        auto loop = loops[i];
        if (loop_cond.find(loop) == loop_cond.end()) {
            CMarsRangeExpr mr = CMarsVariable(loop, ast).get_range();
            CMarsExpression me_lb;
            CMarsExpression me_ub;
            int ret = mr.get_simple_bound(me_lb, me_ub);
            assert(ret);
            CMarsExpression me_tripcount = me_ub - me_lb + 1;
            assert(me_tripcount.IsConstant());

        //    int loop_tripcount = me_tripcount.get_const();
            int64_t loop_tripcount = me_tripcount.get_const();
            ret_size *= loop_tripcount;
        } else {
            auto cond = loop_cond[loop];
            if (cond.second) continue;
            CMarsRangeExpr mr = CMarsVariable(loop, ast).get_range();
            CMarsExpression me_lb;
            CMarsExpression me_ub;
            int ret = mr.get_simple_bound(me_lb, me_ub);
            assert(ret);
            CMarsExpression me_tripcount = me_ub - me_lb;
            assert(me_tripcount.IsConstant());

            //  int loop_tripcount = me_tripcount.get_const();
            int64_t loop_tripcount = me_tripcount.get_const();
            ret_size *= loop_tripcount;
        }
    }
#else
  //  int64_t ret_size = 1;
  CMarsExpression ret_size(ast, 1);
  CMarsExpression ret_0(ast, 0);
  CMarsExpression ret_minus_1(ast, -1);

  //  exclude outer common loops whose level is less than or equal
  //  to sync_level / 2
  for (size_t i = sync_level / 2; i < loops.size() - parallel_level; i++) {
    auto loop = loops[i];
    CMarsRangeExpr mr = CMarsVariable(loop, ast, ref).get_range();
    if (mr.is_empty())
      return ret_0;
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    int ret = mr.get_simple_bound(me_lb, me_ub);
    if (!ret)
      return ret_minus_1;
    //    assert(ret);
    CMarsExpression me_tripcount = me_ub - me_lb + 1;
    //  Yuxin 2016.05.19
    //   if (!me_tripcount.IsConstant())
    //     return -1;
    //  assert(me_tripcount.IsConstant());

    //  int64_t loop_tripcount = me_tripcount.get_const();
    //    ret_size *= loop_tripcount;
    ret_size = ret_size * me_tripcount;
  }
#endif

  return ret_size;
}
#endif

string CMarsEdge::print() {
  string str;
  CMarsAST_IF *ast = get_ast();

  CMarsNode *node0 = GetNode(0);
  CMarsNode *node1 = GetNode(1);

  void *port = GetPort();

  set<void *> refs0 = node0->get_port_references(port);
  set<void *> refs1 = node1->get_port_references(port);

  string str_r0 = "{";
  assert(refs0.size());
  for (auto &r0 : refs0) {
    str_r0 += ast->UnparseToString(r0) + ",";
  }
  str_r0[str_r0.length() - 1] = '}';

  string str_r1 = "{";
  assert(refs1.size());
  for (auto &r1 : refs1) {
    str_r1 += ast->UnparseToString(r1) + ",";
  }
  str_r1[str_r1.length() - 1] = '}';

  str += str_r0 + "->" + str_r1 + "\n";

  for (auto &term : m_properties) {
    str += term.first + "=" + term.second + " ";
  }

  return str;
}

void CMarsEdge::update_dependence() {
  clear_dep_cache();
  mDepVector = new CMarsDepVector;

  CMarsNode *node0 = GetNode(0);
  CMarsNode *node1 = GetNode(1);
  void *port = GetPort();

  //  1. get common for-loops
  vector<void *> common_loops;
  { common_loops = mNodes[0]->get_common_loops(mNodes[1]); }

  //  2. calculate the dependence on the level
  //  a) if dep is *, return 1 on the location level
  //  b) if dep is a range [m, n], return the larger one "n" on the loop level
  //  c) check all the reference and get the largest dependence distance

  for (size_t i = 0; i < common_loops.size(); i++) {
    void *scope = common_loops[i];

    set<void *> refs0 = node0->get_port_references(port);
    set<void *> refs1 = node1->get_port_references(port);

    int dist = INT_MIN;
    int is_unbounded = 0;
    for (set<void *>::iterator r0 = refs0.begin(); r0 != refs0.end(); r0++) {
      for (set<void *>::iterator r1 = refs1.begin(); r1 != refs1.end(); r1++) {
        void *ref0 = *r0;
        void *ref1 = *r1;

        {
          CMarsAccess access1(ref0, m_ast, nullptr);
          CMarsAccess access2(ref1, m_ast, nullptr);
          access1.set_scope(scope);
          access2.set_scope(scope);

          CMarsDepDistSet dist_set;
          CMarsResultFlags results_flags;
          MarsProgramAnalysis::GetDependence(access1, access2, &dist_set,
                                             &results_flags);

          if (dist_set.get_ub() > dist) {
            dist = dist_set.get_ub();
          }
          if (!((dist_set.has_ub() != 0) && (dist_set.has_lb() != 0))) {
            is_unbounded = 1;
          }
        }
      }
    }
    //  bounded
    if (is_unbounded == 0) {
      mDepVector->push_back(0);     //  location level
      mDepVector->push_back(dist);  //  location level
    } else {
      int pos0 = static_cast<int>(node1->is_after(node0));
      int pos1 = static_cast<int>(node0->is_after(node1));
      int curr_dist = ((pos0 != 0) && (pos1 != 0)) ? 0 : pos0 != 0 ? 1 : -1;
      mDepVector->push_back(curr_dist);  //  location level
      dist = curr_dist;
    }

    if (dist != 0) {
      break;
    }
  }

  if (mDepVector->is_zero() != 0) {
    int pos = static_cast<int>(node1->is_after(node0));
    if (pos != 0) {
      mDepVector->push_back(1);
    } else if (node1 == node0) {
      mDepVector->push_back(0);
    } else {
      mDepVector->push_back(-1);
    }
  }
}

void *CMarsNode::get_scope() {
  void *stmt = mStmts[0];
  return m_ast->TraceUpToScope(m_ast->GetParent(stmt));
}

bool CMarsNode::is_dead() const {
  if (mStmts.empty()) {
    return true;
  }
  if (mPorts.empty()) {
    return true;
  }
  //  if only contains assert statements, it is dead; otherwise
  //  it is not
  for (auto stmt : mStmts) {
    vector<void *> vec_call;
    m_ast->GetNodesByType_int(stmt, "preorder", V_SgFunctionCallExp, &vec_call);
    bool contain_assert = false;
    for (auto &call : vec_call) {
      if (m_ast->IsAssertFailCall(call)) {
        contain_assert = true;
        break;
      }
    }
    if (!contain_assert) {
      return false;
    }
  }
  return true;
}

int VEC_2L_PLUS_ONE::is_zero() {
  vector<int>::iterator it;
  for (it = begin(); it != end(); it++) {
    if (*it != 0) {
      return 0;
    }
  }
  return 1;
}
void CMarsNode::remove_port(void *port) {
  mPorts.erase(port);
  mPort2Pntr.erase(port);
  mPort2Refs.erase(port);
}

void CMarsNode::set_ports(const set<void *> &ports) {
  mPorts.clear();
  mPorts.insert(ports.begin(), ports.end());
}

int CMarsNode::get_loop_level(void *loop) const {
  for (size_t i = 0; i < mLoops.size(); i++) {
    if (mLoops[i] == loop) {
      return i;
    }
  }
  return -1;
}

void *CMarsNode::get_loop(int depth) const {
  if (depth >= get_loop_depth()) {
    return nullptr;
  }
  return mLoops[depth];
}

bool CMarsNode::contains(void *loop) const {
  for (size_t i = 0; i != mLoops.size(); ++i) {
    if (mLoops[i] == loop) {
      return true;
    }
  }
  return false;
}
int CMarsNode::get_order(size_t depth) const {
  if (depth >= mOrders.size()) {
    return -1;
  }
  return mOrders[depth];
}

void CMarsNode::set_schedule_depth(size_t level, int depth) {
  if (level < mSchedule_depth.size()) {
    mSchedule_depth[level] = depth;
  }
}

int CMarsNode::get_schedule_depth(size_t level) const {
  if (level >= mSchedule_depth.size()) {
    return -1;
  }
  return mSchedule_depth[level];
}

#if USED_CODE_IN_COVERAGE_TEST
int CMarsNode::get_schedule(size_t level) const {
  if (level >= mSchedule.size())
    return -1;
  return mSchedule[level];
}

int CMarsNode::get_loop2schedule(int level) {
  if (mLoop2Schedule.find(level) != mLoop2Schedule.end())
    return mLoop2Schedule[level];
  else
    return -1;
}
#endif

void CMarsNode::schedule_map2vec() {
  for (size_t i = 0; i < mLoop2Schedule.size(); i++) {
    if (mLoop2Schedule.find(i) != mLoop2Schedule.end()) {
      mSchedule.push_back(mLoop2Schedule[i]);
    }
  }
}

access_type CMarsNode::get_port_access_type(void *port) const {
  map<void *, access_type>::const_iterator i = mPort2AccessType.find(port);
  if (i == mPort2AccessType.end()) {
    return NO_ACCESS;
  }
  return i->second;
}
void CMarsEdge::clear_dep_cache() {
  delete mDepVector;

  mDepVector = nullptr;
}

CMarsEdge::CMarsEdge(CMarsAST_IF *ast, CMarsNode *node0, CMarsNode *node1,
                     void *port) {
  m_ast = ast;
  mNodes.push_back(node0);
  mNodes.push_back(node1);
  mPort = port;
  mDepVector = nullptr;
  m_comm_type = COMMTYPE_SHARED_MEM_DDR;
  m_ssst = false;
}

CMarsNode *CMarsEdge::GetNode(int index) {
  assert(0 <= index);
  assert(index < 2);
  assert(mNodes.size() == 2);
  return mNodes[index];
}
CMarsDepVector CMarsEdge::GetDependence() {
  if (mDepVector == nullptr) {
    update_dependence();
  }

  return *mDepVector;
}

#if USED_CODE_IN_COVERAGE_TEST
int CMarsLoop::has_opt_pragmas() {
  for (auto pragma : vec_pragma)
    if (pragma.is_pipeline() || pragma.is_parallel())
      return true;
  return false;
}
int CMarsLoop::has_pipeline() {
  for (auto pragma : vec_pragma)
    if (pragma.is_pipeline())
      return true;
  return false;
}
#endif

int CMarsLoop::has_parallel() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_parallel()) {
      return 1;
    }
  }
  return 0;
}

int CMarsLoop::is_complete_unroll() {
  for (auto pragma : vec_pragma) {
    if ((pragma.is_parallel()) &&
        !pragma.get_attribute(CMOST_complete).empty()) {
      return 1;
    }
  }
  return 0;
}

int CMarsLoop::is_partial_unroll(int *factor) {
  for (auto pragma : vec_pragma) {
    if (!pragma.is_parallel() ||
        pragma.get_attribute(CMOST_parallel_factor).empty()) {
      continue;
    }
    *factor = my_atoi(pragma.get_attribute(CMOST_parallel_factor));
    return 1;
  }
  return 0;
}

void CMarsLoop::set_trip_count(int64_t trip_count, int64_t max_trip_count) {
  m_trip_count = trip_count;
  m_max_trip_count = max_trip_count;
}
