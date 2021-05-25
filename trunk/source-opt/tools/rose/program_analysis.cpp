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


#include "program_analysis.h"
#include <assert.h>
#include <algorithm>
#include <map>
#include <boost/range/adaptor/reversed.hpp>

#include "SgGraphTemplate.h"
#include "codegen.h"
#include "ir_types.h"
#include "common.h"
#include "mars_ir_v2.h"

using std::list;
using std::map;
using std::numeric_limits;
using std::set;
using std::tuple;
using std::vector;
#define _DEBUG_GET_RANGE_ 0      //  0
#define _DEBUG_PRINT_EXP_ 0      //  0
#define _DEBUG_VAR_INDUCT_ 0     //  1
#define _DEBUG_RANGE_UNION_ 0    //  1
#define _DEBUG_SUBSTITUTE_ 0     //  1
#define _DEBUG_CHECK_PV_ 0       //  1
#define _DEBUG_EXACT_FLAG_ 0     //  1
#define _DEBUG_CONTROL_CHECH_ 0  //  1
#define _DEBUG_HELPER_NEW_ 0     //  1
#define _DEBUG_RANGE_REFINE_ 0   //  1
#define _DEBUG_FUNC_PATH_ 0      //  1
#define _DEBUG_MERGE_RANGE_V2_ 0
#define _DEBUG_COPYACCESS_ 1

#define _DEBUG_var_ref_intermediate 0

namespace MarsProgramAnalysis {

map<DUMMY, CMarsRangeExpr> cache_get_range_var;
map<DUMMY, CMarsRangeExpr> cache_get_range_exp;
map<DUMMY, tuple<bool, int64_t>> cache_consts;
map<DUMMY, int> cache_IsNonNegative;
map<DUMMY, int> cache_IsNonPositive;
map<DUMMY, bool> cache_IsMatchedFuncAndCall;

//  ZP: 20161002
int g_exact_flag_cache_enabled = 0;
map<void *, map<void *, map<void *, pair<bool, bool>>>> g_exact_flag_cache;

void get_control_exact_flag_in_scope(CMarsAST_IF *ast, void *array, void *pos,
                                     void *range_scope, bool *is_write_exact,
                                     bool *is_read_exact);
void get_control_exact_flag_in_scope_v2(CMarsAST_IF *ast, void *array,
                                        void *pos, void *range_scope,
                                        const t_func_call_path &fn_path,
                                        bool *w_exact, bool *r_exact);
static bool evaluate_constant_comparsion(int comp_opcode, int64_t c_op0,
                                         int64_t c_op1) {
  switch (comp_opcode) {
  case V_SgLessOrEqualOp:
    return c_op0 <= c_op1;
  case V_SgLessThanOp:
    return c_op0 < c_op1;
  case V_SgGreaterOrEqualOp:
    return c_op0 >= c_op1;
  case V_SgEqualityOp:
    return c_op0 == c_op1;
  case V_SgNotEqualOp:
    return c_op0 != c_op1;
  case V_SgGreaterThanOp:
    return c_op0 > c_op1;
  default:
    return false;
  }
  return false;
}

static int is_control_inexact_for_loop(CMarsAST_IF *ast, void *loop,
                                       void *range_scope) {
  if (!ast->IsForStatement(loop))
    return true;
  CMarsRangeExpr range = CMarsVariable(loop, ast).get_range();
  if (!range.is_exact()) {
    return true;
  }
  range.refine_range_in_scope(range_scope, loop);
  CMarsExpression me_lb;
  CMarsExpression me_ub;
  if (!range.get_simple_bound(&me_lb, &me_ub)) {
    return true;
  } else {
    //  CMarsExpression length = me_ub - me_lb + 1;
    if (!(me_ub >= me_lb))
      return true;
  }
  void *curr_pos = loop;
  //  check the break and continue statements
  vector<void *> vec_contn;
  vector<void *> vec_break;
  vector<void *> vec_goto;
  ast->GetNodesByType(curr_pos, "preorder", "SgContinueStmt", &vec_contn);
  ast->GetNodesByType(curr_pos, "preorder", "SgBreakStmt", &vec_break);
  ast->GetNodesByType(curr_pos, "preorder", "SgGotoStatement", &vec_goto);
  if (!vec_goto.empty()) {
    return true;
  } else {
    for (auto contn : vec_contn) {
      void *related_loop = ast->TraceUpToLoopScope(contn);
      if (related_loop == curr_pos) {
        return true;
      }
    }
    for (auto br_stmt : vec_break) {
      if (ast->IsScopeBreak(br_stmt, curr_pos)) {
        return true;
      }
    }
  }
  return false;
}
#if 0
CTempASTStmt::CTempASTStmt(void *temp_expr, CMarsAST_IF *ast, void *pos) {
  m_ast = ast;
  m_temp_stmt = nullptr;

  if (!ast->is_floating_node(temp_expr))
    return;
  if (ast->is_floating_node(pos))
    assert(0);

  m_temp_stmt = m_ast->CreateStmt(V_SgExprStatement, temp_expr);
  void *sg_pos_stmt = pos;
  if (ast->IsArgumentInitName(pos)) {
    assert(0);
  } else if (ast->IsGlobalInitName(pos)) {
    assert(0);
  } else if (!ast->IsStatement(pos)) {
    sg_pos_stmt = m_ast->GetEnclosingNode("Statement", pos);
  }
  m_ast->InsertAfterStmt(m_temp_stmt, sg_pos_stmt);
  m_reset_defuse = 0;
}

void CTempASTStmt::reset_defuse() {
  m_ast->init_defuse();
  m_reset_defuse = 1;
}

CTempASTStmt::~CTempASTStmt() {
  if (m_temp_stmt) {
    m_ast->RemoveStmt(m_temp_stmt);

    //  the defuse need to be reset again after removing the statement
    if (m_reset_defuse)
      m_ast->init_defuse();
  }
}
#endif
/****************************************************************/
//  class CMarsVariable
/****************************************************************/
map<void *, string> CMarsVariable::s_map_tmpvar_name;

CMarsVariable::CMarsVariable(void *var, CMarsAST_IF *ast,
                             void *pos /*=nullptr*/, void *scope) {
  m_var = var;
  m_ast = ast;
  m_scope = nullptr;
  if (scope != nullptr) {
    m_scope = scope;
  }
  if (pos == nullptr) {
    m_pos = var;
  } else {
    m_pos = pos;
  }
  assert(ast);
  //  assert(!ast->is_floating_node(m_pos));
}

CMarsVariable::CMarsVariable(CMarsAST_IF *ast, CMarsRangeExpr mr) {
  static int s_tmp_val_total_idx = 0;
  string s_name = "__tmp" + my_itoa(s_tmp_val_total_idx++);

  vector<void *> v_empty;
  m_var = ast->CreateFuncCall(s_name, ast->GetTypeByString("long"), v_empty,
                              ast->GetGlobal(0));
  CMarsVariable::s_map_tmpvar_name.insert(pair<void *, string>(m_var, s_name));

  m_ast = ast;
  m_scope = ast->GetProject();
  m_pos = ast->GetProject();

  assert(ast);
  assert(!ast->is_floating_node(m_pos));

  set_range(mr);
}

string CMarsVariable::unparse() const {
  if (m_ast->IsForStatement(m_var) != 0) {
    void *iter = m_ast->GetLoopIterator(m_var);
    return m_ast->UnparseToString(iter != nullptr ? iter : m_var);
  }

  return m_ast->UnparseToString(m_var);
}

//  0 iterator, 1 local, 2 expression, 3 argument
string CMarsVariable::GetTypeString() {
  assert(m_ast);

  if (m_ast->IsArgumentInitName(m_var) != 0) {
    return "argument";
  }
  if (m_ast->IsForStatement(m_var) != 0) {
    return "iterator";
  }
  if (m_ast->IsLocalInitName(m_var) != 0) {
    return "local";
  }
  //    if (m_ast->IsForwardInitName(m_var))  return "forward";  //  ROSE has
  //    bug to determine forward variable declaration
  if (m_ast->IsGlobalInitName(m_var) != 0) {
    return "global";
  }
  if (m_ast->IsExpression(m_var) != 0) {
    return "expression";
  }

  return "unknown";
}
#if KEEP_UNUSED
int CMarsVariable::GetType() {
  string str = GetTypeString();

  if ("iterator" == str)
    return 0;
  if ("local" == str)
    return 1;
  if ("global" == str)
    return 2;
  if ("expression" == str)
    return 3;
  if ("argument" == str)
    return 4;

  return -1;
}
#endif

//  ZP: 2015-07-25: in this version, we only want to find the constant boundary
CMarsRangeExpr CMarsVariable::get_range() const {
#if _DEBUG_GET_RANGE_
  cout << "  -- get_range() var start -- " << m_ast->GetASTTypeString(m_var)
       << " " << m_ast->UnparseToString(m_var) << endl;
#endif

  DEFINE_START_END;
  STEMP(start);
  CMarsRangeExpr mr(get_variable_range(m_var, m_ast, m_pos, m_scope));
  ACCTM(CMarsVariable___get_range, start, end);

#if _DEBUG_GET_RANGE_
  cout << "  -- get_range() var end   -- " << m_ast->GetASTTypeString(m_var)
       << " " << m_ast->UnparseToString(m_var) << " : " << mr.print() << endl;
#endif
  return mr;
}

//  Utility function: used for temporary CMarsExpression/CMarsRangeExpr objects
void CMarsVariable::set_range(CMarsRangeExpr mr) {
#if PROJDEBUG
  assert(is_tmp_var(m_var));
#endif

  set_variable_range(m_var, mr);
}

#if KEEP_UNUSED
void CMarsVariable::clear_range() {  //  clean memory
#if PROJDEBUG
  assert(is_tmp_var(m_var));
#endif

  clear_variable_range(m_var);
}

//  Variable ranges
int CMarsVariable::IsConstantRange() {
  CMarsRangeExpr mr = get_variable_range(m_var, m_ast, m_pos, m_scope);

  return mr.IsConstantBound();
}
int64_t CMarsVariable::GetLBAbs() {
  CMarsRangeExpr mr = get_variable_range(m_var, m_ast, m_pos, m_scope);
  return mr.get_const_lb();
}
int64_t CMarsVariable::GetUBAbs() {
  CMarsRangeExpr mr = get_variable_range(m_var, m_ast, m_pos, m_scope);
  return mr.get_const_ub();
}
int CMarsVariable::GetBoundAbs(int64_t &lb, int64_t &ub) {
  CMarsRangeExpr mr = get_variable_range(m_var, m_ast, m_pos, m_scope);
  lb = mr.get_const_lb();
  ub = mr.get_const_ub();
  return mr.IsConstantBound();
}

int CMarsVariable::GetBoundExpr(CMarsExpression &lb, CMarsExpression &ub) {
  return 0;
}
#endif

string CMarsVariable::print_var() const {
  string str;

  //  if (is_tmp_var(m_var))
  //      return get_tmp_var_name(m_var);

  if (m_ast->IsForStatement(m_var) != 0) {
    void *iter = m_ast->GetLoopIterator(m_var);
    str += m_ast->UnparseToString(iter != nullptr ? iter : m_var);
  } else {
    str += m_ast->UnparseToString(m_var);
  }
  return str;
}

#if _DEBUG_GET_RANGE_
string CMarsVariable::print() const {
  string str;
  //  if (!is_tmp_var(m_var)) {  //  not tmp var
  str = m_ast->GetASTTypeString(m_var) + " " + print_var() + " : " +
        get_range().print();
  //  }
  //  else
  //  {
  //    str = "TMPVAR " + print_var() + " : " +
  //                 get_range().print();
  //  }
  return str;
}
#endif

void CMarsRangeExpr::get_vars(vector<void *> *reduced_all_vars) const {
  size_t i;
  vector<void *> all_vars;
  for (i = 0; i < m_lb.size(); i++) {
    m_lb[i].get_vars(&all_vars);
  }
  for (i = 0; i < m_ub.size(); i++) {
    m_ub[i].get_vars(&all_vars);
  }

  std::unordered_set<void *> flag;
  for (i = 0; i < all_vars.size(); i++) {
    if (flag.count(all_vars[i]) <= 0) {
      flag.insert(all_vars[i]);
      reduced_all_vars->push_back(all_vars[i]);
    }
  }
}

#if KEEP_UNUSED
int CMarsRangeExpr::refine_range_in_scope_one_variable(
    void *range_scope, vector<void *> &loops_in_scope) {
  return 0;
}
#endif

void CMarsRangeExpr::refine_range_get_remain_substitutes(
    void *range_scope, void *pos, const t_func_call_path &fn_path,
    vector<void *> *vec_sub_vars, vector<CMarsExpression> *vec_sub_lbs,
    vector<CMarsExpression> *vec_sub_ubs, vector<bool> *vec_sub_is_exact) {
  CMarsAST_IF *ast = get_ast();
#if PROJDEBUG
  assert(ast->is_located_in_path(range_scope, fn_path));
#endif
  vector<void *> m_pv;
  this->get_vars(&m_pv);

  //  3. Single assignement tracing: 20160202 ZP
  //  Note:
  //  * In the current implementation, expression substitution in
  //  CMarsExpression stoped if the new term is not movable to the position of
  //  the old term.
  //  * However, in range analysis, movability is not necessary for the term,
  //  unless the tracing is within the scope of the range_scope.
  //  * So an additional tracing is needed to get the exact range at the scope
  {
    for (size_t i = 0; i < m_pv.size(); i++) {
      void *curr_term = m_pv[i];
#if 1
      //  assert range
      //  else if (ast->IsInitName(curr_term) && has_variable_range(curr_term) )
      if ((ast->IsForStatement(curr_term) == 0) &&
          has_variable_range(ast, curr_term, pos, range_scope, true)) {
        if (ast->any_var_located_in_scope(curr_term, range_scope) != 0) {
          CMarsRangeExpr var_range =
              CMarsVariable(curr_term, ast, pos, range_scope).get_range();
          if (var_range.has_bound() == 0) {
            continue;
          }
          CMarsExpression me_lb = var_range.get_lb();
          CMarsExpression me_ub = var_range.get_ub();
          if ((me_lb.is_movable_to(pos) == 0) ||
              (me_ub.is_movable_to(pos) == 0)) {
            continue;
          }
          vec_sub_vars->push_back(curr_term);
          vec_sub_lbs->push_back(me_lb);
          vec_sub_ubs->push_back(me_ub);
          vec_sub_is_exact->push_back(var_range.is_exact());
        }
        continue;
      }
#endif

      //  VarRef: can be function args or other references
      if ((ast->IsVarRefExp(curr_term) != 0) ||
          (ast->IsInitName(curr_term) != 0)) {
        void *curr_name = ast->GetVariableInitializedName(curr_term);
        if (curr_name == nullptr) {
          curr_name = curr_term;
        }

        //  ZP: 20161106
        void *pos_tmp = pos;
        if (ast->is_floating_node(curr_term) == 0) {
          pos_tmp = curr_term;
        }
        void *new_pos = pos_tmp;
        void *sg_exp = ast->GetSingleAssignExpr_path(
            curr_name, fn_path, &new_pos, pos_tmp, 0, range_scope);
        if (sg_exp != nullptr) {
          if (has_variable_range(m_ast, sg_exp, pos, range_scope, true)) {
            if (ast->is_located_in_scope(sg_exp, range_scope, fn_path) != 0) {
              CMarsRangeExpr var_range =
                  CMarsVariable(sg_exp, ast, pos_tmp, range_scope).get_range();
              if (var_range.has_bound() == 0) {
                continue;
              }
              CMarsExpression me_lb = var_range.get_lb();
              CMarsExpression me_ub = var_range.get_ub();
              if (sg_exp != curr_term) {
                if ((me_lb.is_movable_to(new_pos) == 0) ||
                    (me_ub.is_movable_to(new_pos) == 0)) {
                  continue;
                }
                vec_sub_vars->push_back(curr_term);
                vec_sub_lbs->push_back(me_lb);
                vec_sub_ubs->push_back(me_ub);
                vec_sub_is_exact->push_back(var_range.is_exact());
              }
            }
            continue;
          }

          if (sg_exp != curr_term &&
              (ast->is_movable_test(sg_exp, new_pos) != 0)) {
            vec_sub_vars->push_back(curr_term);
            CMarsExpression me_exp(sg_exp, ast);
            vec_sub_lbs->push_back(me_exp);
            vec_sub_ubs->push_back(me_exp);
            vec_sub_is_exact->push_back(1);
          }
        }
      }
    }

    for (size_t i = 0; i < m_pv.size(); i++) {
      void *curr_term = m_pv[i];

      //  for-loop
      if (ast->IsForStatement(curr_term) != 0) {
        void *sg_loop = curr_term;
        if (ast->is_located_in_scope(sg_loop, range_scope, fn_path) != 0) {
          CMarsRangeExpr var_range =
              CMarsVariable(sg_loop, ast, pos, range_scope).get_range();
          if (var_range.has_bound() == 0) {
            continue;
          }
          CMarsExpression me_lb = var_range.get_lb();
          CMarsExpression me_ub = var_range.get_ub();

          if ((me_lb.is_movable_to(pos) == 0) ||
              (me_ub.is_movable_to(pos) == 0)) {
            continue;
          }
          vec_sub_vars->push_back(sg_loop);
          vec_sub_lbs->push_back(me_lb);
          vec_sub_ubs->push_back(me_ub);
          vec_sub_is_exact->push_back(var_range.is_exact());
        }
      } else if ((ast->IsVarRefExp(curr_term) != 0) ||
                 (ast->IsInitName(curr_term) != 0)) {
        //  VarRef: can be function args or other references
        void *pos_tmp = pos;
        if (ast->is_floating_node(curr_term) == 0) {
          pos_tmp = curr_term;
        }

        void *curr_name = ast->GetVariableInitializedName(curr_term);
        if (curr_name == nullptr) {
          curr_name = curr_term;
        }
        void *new_pos = pos_tmp;
        void *sg_exp = ast->GetSingleAssignExpr_path(
            curr_name, fn_path, &new_pos, pos_tmp, 0, range_scope);
        if (sg_exp != nullptr) {
          if (ast->IsVarRefExp(sg_exp) != 0) {
            void *new_name = ast->GetVariableInitializedName(sg_exp);
            void *sg_loop = ast->GetLoopFromIteratorByPos(new_name, pos, 1);
            if ((sg_loop != nullptr) &&
                (ast->is_located_in_scope(sg_loop, range_scope, fn_path) !=
                 0)) {
              CMarsRangeExpr var_range =
                  CMarsVariable(sg_loop, ast, pos, range_scope).get_range();
              if (var_range.has_bound() == 0) {
                continue;
              }
              CMarsExpression me_lb = var_range.get_lb();
              CMarsExpression me_ub = var_range.get_ub();
              sg_exp = sg_loop;

              if (sg_exp != curr_term) {
                if ((me_lb.is_movable_to(new_pos) == 0) ||
                    (me_ub.is_movable_to(new_pos) == 0)) {
                  continue;
                }
                vec_sub_vars->push_back(curr_term);
                vec_sub_lbs->push_back(me_lb);
                vec_sub_ubs->push_back(me_ub);
                vec_sub_is_exact->push_back(var_range.is_exact());
              }
              continue;
            }
          }
        }
      }
    }
  }
}

int CMarsRangeExpr::refine_range_in_scope_v2_exact(
    void *range_scope, void *pos, const t_func_call_path &fn_path) {
  if (!is_exact() || (any_var_located_in_scope(range_scope) == 0)) {
    return 0;
  }
#if _DEBUG_RANGE_REFINE_
  CMarsRangeExpr mr_org(*this);
#endif

  //  1. Get all the possible substitutions
  //  a) all the loops in the scope
  //  b) all the single assignements (cross-function) with in the scope
  int hit_once = 0;
  //  handling exact substitution
  while (is_exact()) {
    vector<void *> vec_sub_vars;
    vector<CMarsExpression> vec_sub_lbs;
    vector<CMarsExpression> vec_sub_ubs;
    vector<bool> vec_sub_is_exact;
    refine_range_get_remain_substitutes(range_scope, pos, fn_path,
                                        &vec_sub_vars, &vec_sub_lbs,
                                        &vec_sub_ubs, &vec_sub_is_exact);

    vector<int> idx_remain;
    int hit = 0;
    for (size_t i = 0; i < vec_sub_vars.size(); i++) {
      //  1.1 try to perform exact check, pick one which can be substituted
      //  exactly
      CMarsRangeExpr range_tmp(*this);
      range_tmp.substitute(vec_sub_vars[i], vec_sub_lbs[i], vec_sub_ubs[i],
                           vec_sub_is_exact[i]);

      //  1.2 perform the substitution if it is exact
      if (range_tmp.is_exact()) {
        *this = range_tmp;
        hit_once = 1;
        hit = 1;
      }
    }
    if (hit == 0) {
      break;
    }
  }

#if _DEBUG_RANGE_REFINE_
  if (hit_once) {
    cout << "  -- refine: " << mr_org.print_e() << " -> " << print_e()
         << " scope=" << ast->UnparseToString(range_scope).substr(0, 20)
         << endl;
    cout << "     path: " << ast->print_func_path(fn_path) << endl;
  }
#endif
  //  youxiang 20170130 update position
  update_position(fn_path, pos);
  return hit_once;
}

void CMarsRangeExpr::update_position(const t_func_call_path &fn_path,
                                     void *pos) {
  assert(m_ast);
  auto fn_path_iter = fn_path.begin();
  void *curr_pos = pos;
  while (fn_path_iter != fn_path.end()) {
    void *func_decl = fn_path_iter->first;
    void *func_call = fn_path_iter->second;
    if ((func_decl != nullptr) && (func_call != nullptr)) {
      if (any_var_located_in_scope(func_decl) == 0) {
        curr_pos = func_call;
      } else {
        break;
      }
    }
    fn_path_iter++;
  }
  void *final_pos = curr_pos;
  do {
    if (any_var_located_in_scope(curr_pos) == 0) {
      if (m_ast->IsBasicBlock(curr_pos) == 0) {
        final_pos = curr_pos;
      }
      curr_pos = m_ast->GetParent(curr_pos);
    } else {
      break;
    }
  } while (curr_pos != nullptr && !m_ast->IsFunctionDefinition(curr_pos));
  if ((final_pos != nullptr) && final_pos != pos) {
    set_pos(final_pos);
  }
}

void CMarsRangeExpr::update_control_flag(void *range_scope, void *pos,
                                         void *array, bool is_write,
                                         const t_func_call_path &fn_path) {
  if (!is_exact()) {
    return;
  }

  assert(get_ast());
  CMarsAST_IF *ast = get_ast();

  //  handling control exact flags
  bool is_control_exact = true;
  {
    bool tmp_w_exact = true;
    bool tmp_r_exact = true;
    get_control_exact_flag_in_scope_v2(ast, array, pos, range_scope, fn_path,
                                       &tmp_w_exact, &tmp_r_exact);
    is_control_exact = is_write ? tmp_w_exact : tmp_r_exact;
  }
  if (!is_control_exact) {
    set_exact(false);
  }
}

//  A more powerful functionality for range refinement
//  1. support cross-function refinement
//  2. support multiple calls
//  3. support loop and assert substitution
//  4. support exact flag with control flows
void CMarsRangeExpr::refine_range_in_scope_v2(void *range_scope, void *pos,
                                              void *array, bool is_write,
                                              const t_func_call_path &fn_path) {
#if _DEBUG_RANGE_REFINE_
  CMarsRangeExpr mr_org(*this);
#endif
  CMarsAST_IF *ast = get_ast();
  if (ast == nullptr) {
    return;
  }

  //  handling control exact flags
  update_control_flag(range_scope, pos, array, is_write, fn_path);

  //  handling exact substitution
  refine_range_in_scope_v2_exact(range_scope, pos, fn_path);

  //
  //  1. Get all the possible substitutions
  //  a) all the loops in the scope
  //  b) all the single assignements (cross-function) with in the scope
  vector<void *> vec_loops;
  ast->get_loop_nest_in_scope(pos, range_scope, fn_path, &vec_loops);

  vector<void *> m_iv = vec_loops;

  bool hit_once = false;
  //  handling remain substitution
  while (true) {
    vector<void *> vec_sub_vars;
    vector<CMarsExpression> vec_sub_lbs;
    vector<CMarsExpression> vec_sub_ubs;
    vector<bool> vec_sub_is_exact;
    refine_range_get_remain_substitutes(range_scope, pos, fn_path,
                                        &vec_sub_vars, &vec_sub_lbs,
                                        &vec_sub_ubs, &vec_sub_is_exact);
    if (vec_sub_vars.empty()) {
      break;
    }

    bool hit = false;
    for (size_t i = 0; i < vec_sub_vars.size(); i++) {
      if (substitute(vec_sub_vars[i], vec_sub_lbs[i], vec_sub_ubs[i],
                     vec_sub_is_exact[i])) {
        hit_once = true;
        hit = true;
      }
    }
    if (!hit) {
      break;
    }
  }

  //  youxiang 20170219 update position
  update_position(fn_path, pos);
  if (hit_once) {
#if _DEBUG_RANGE_REFINE_
    cout << "  -- refine: " << mr_org.print_e() << " -> " << print_e()
         << " scope=" << ast->UnparseToString(range_scope).substr(0, 20)
         << endl;
    cout << "     path: " << ast->print_func_path(fn_path) << endl;
#endif
  }
}

void CMarsRangeExpr::refine_range_in_scope_v2(void *range_scope, void *pos,
                                              const t_func_call_path &fn_path) {
#if _DEBUG_RANGE_REFINE_
  CMarsRangeExpr mr_org(*this);
#endif
  CMarsAST_IF *ast = get_ast();
  if (ast == nullptr) {
    return;
  }
  CMarsRangeExpr infinite(ast);

  //  handling exact substitution
  refine_range_in_scope_v2_exact(range_scope, pos, fn_path);

  //
  //  1. Get all the possible substitutions
  //  a) all the loops in the scope
  //  b) all the single assignements (cross-function) with in the scope
  vector<void *> vec_loops;
  ast->get_loop_nest_in_scope(pos, range_scope, fn_path, &vec_loops);

  vector<void *> m_iv = vec_loops;

  bool hit_once = false;
  //  handling remain substitution
  while (true) {
    vector<void *> vec_sub_vars;
    vector<CMarsExpression> vec_sub_lbs;
    vector<CMarsExpression> vec_sub_ubs;
    vector<bool> vec_sub_is_exact;
    refine_range_get_remain_substitutes(range_scope, pos, fn_path,
                                        &vec_sub_vars, &vec_sub_lbs,
                                        &vec_sub_ubs, &vec_sub_is_exact);
    if (vec_sub_vars.empty()) {
      break;
    }

    bool hit = false;
    for (size_t i = 0; i < vec_sub_vars.size(); i++) {
      if (substitute(vec_sub_vars[i], vec_sub_lbs[i], vec_sub_ubs[i],
                     vec_sub_is_exact[i])) {
        hit_once = true;
        hit = true;
      }
    }
    if (!hit) {
      break;
    }
  }

  //  youxiang 20170219 update position
  auto fn_path_iter = fn_path.begin();
  void *curr_pos = pos;
  while (fn_path_iter != fn_path.end()) {
    void *func_decl = fn_path_iter->first;
    void *func_call = fn_path_iter->second;
    if ((func_decl != nullptr) && (func_call != nullptr)) {
      if (any_var_located_in_scope(func_decl) == 0) {
        curr_pos = func_call;
      } else {
        break;
      }
    }
    fn_path_iter++;
  }
  if (curr_pos != pos) {
    set_pos(curr_pos);
  }

  if (hit_once) {
#if _DEBUG_RANGE_REFINE_
    cout << "  -- refine: " << mr_org.print_e() << " -> " << print_e()
         << " scope=" << ast->UnparseToString(range_scope).substr(0, 20)
         << endl;
    cout << "     path: " << ast->print_func_path(fn_path) << endl;
#endif
  }
}

void CMarsRangeExpr::refine_range_in_scope(void *range_scope, void *pos) {
  CMarsAST_IF *m_ast = get_ast();
  if (m_ast == nullptr) {
    return;
  }
  if (range_scope == nullptr) {
    range_scope = m_ast->GetProject();
  }

  CMarsRangeExpr infinite(m_ast);

  //  1. Get all the possible substitutions
  //  a) all the loops in the scope
  //  b) all the single assignements (cross-function) with in the scope
  vector<void *> vec_loops;
  m_ast->get_loop_nest_in_scope(pos, range_scope, &vec_loops);
  void *m_scope = range_scope;
  vector<void *> m_iv = vec_loops;

  //  2. get all the variables to replace
  vector<void *> vec_sub_vars;
  vector<CMarsExpression> vec_sub_lbs;
  vector<CMarsExpression> vec_sub_ubs;
  vector<bool> vec_sub_is_exact;

  {
    for (size_t i = 0; i < m_iv.size(); i++) {
      CMarsRangeExpr var_range =
          CMarsVariable(m_iv[i], m_ast, pos, range_scope).get_range();
      //  CYX : 20160705 avoid the potential assertion failure
      if (var_range.has_bound() == 0) {
        continue;
      }
      //  ZP : 20160301 //  FIXME: refine the loop bounds
      //  var_range.refine_range_in_scope(range_scope, pos); //  this does not
      //  work becuase of infinite recursion

      CMarsExpression me_lb = var_range.get_lb();
      CMarsExpression me_ub = var_range.get_ub();
      vec_sub_vars.push_back(m_iv[i]);
      vec_sub_lbs.push_back(me_lb);
      vec_sub_ubs.push_back(me_ub);
      vec_sub_is_exact.push_back(var_range.is_exact());
    }
  }

  //  ///////////////////////////  /
  //  ZP: 20160302
  //  2.1 substitute all the assert ranges
  {
    while (true) {
      vector<void *> t_pv;
      this->get_vars(&t_pv);

      bool hit = false;
      for (size_t i = 0; i < t_pv.size(); i++) {
        void *curr_term = t_pv[i];

        if ((m_ast->IsInitName(curr_term) != 0) &&
            has_variable_range(m_ast, curr_term, pos, range_scope, true)) {
          if (this->substitute(curr_term, pos) != 0) {
            hit = true;
            break;
          }
        }
      }
      if (!hit) {
        break;
      }
    }
  }

  vector<void *> m_pv;
  this->get_vars(&m_pv);

  //  3. Single assignement tracing: 20160202 ZP
  //  Note:
  //  * In the current implementation, expression substitution in
  //  CMarsExpression stoped if the new term is not movable to the position of
  //  the old term.
  //  * However, in range analysis, movability is not necessary for the term,
  //  unless the tracing is within the scope of the m_scope.
  //  * So an additional tracing is needed to get the exact range at the scope
  {
    for (size_t i = 0; i < m_pv.size(); i++) {
      void *curr_term = m_pv[i];
      //  if (has_variable_range(curr_term)) continue;
      if ((m_ast->IsVarRefExp(curr_term) != 0) ||
          (m_ast->IsInitName(curr_term) != 0)) {
        void *m_pos = pos;
        void *curr_name = m_ast->GetVariableInitializedName(curr_term);
        if (curr_name == nullptr) {
          curr_name = curr_term;
        }
        void *sg_exp = m_ast->GetSingleAssignExpr(curr_name, m_pos, 0, m_scope);
        if (sg_exp != nullptr) {
          if (m_ast->IsVarRefExp(sg_exp) != 0) {
            void *new_name = m_ast->GetVariableInitializedName(sg_exp);
            void *sg_loop = m_ast->GetLoopFromIteratorByPos(new_name, m_pos, 1);
            if ((sg_loop != nullptr) &&
                (m_ast->is_located_in_scope(sg_loop, m_scope) != 0)) {
              // CMarsRangeExpr var_range =
              //  CMarsVariable(sg_loop, m_ast, pos, range_scope).get_range();
              // CMarsExpression me_lb = var_range.get_lb();
              // CMarsExpression me_ub = var_range.get_ub();
              sg_exp = sg_loop;
            }
          }
          //  ZP: 20160223, aggressive at range analysis
          //  if (m_ast->is_located_in_scope(sg_exp, m_scope))
          CMarsExpression me_exp(sg_exp, m_ast, nullptr, m_scope);
          this->substitute(curr_term, me_exp, me_exp, 1);
        }
      }
    }
  }

  //  ////////////////////////////////////////////////  ]
  //  ZP: substitute variable ranges
  //  Carefully consideration is done for exact flag calculation
  //  Note:
  //     Exact flag propagated to indicate that the range is
  //     actually accessed for all the data elements in the range.
  //     This analysis is very useful and necessary for write access,
  //     to determine if the result after the write access will still
  //     determined by the original value or not
  //  Algorithm:
  //     The analysis performs a conservative propagation using the following
  //     rules We assume we replace "i" in "mr_in" with "lb_i" and "ub_i", and
  //     we got "mr_out"
  //     1. if the range lb/ub of i is not exact, mr_out is not exact
  //     2. if mr_in is inexact, mr_out is inexact
  //     3. if mr_in is not simple form (one lb, one ub), mr_out is inexact
  //     4. if the coeffs of i in mr_in bounds is not constant, mr_out is
  //     inexact
  //     5. [formula I]: if (lb[i], ub[i]) and (lb[i+1], ub[i+1) overlap,
  //     mr_out is inexact
  //     6. The order of the substituted vars matters a lot on the result of
  //     [formula I]
  //     7. The exact check will be done by trying the possible substitution
  //     order
  //        in a greedy way ( to be verify if it impacts the accuracy)
  vector<int> idx_remain;
  for (size_t i = 0; i < vec_sub_vars.size(); i++) {
    idx_remain.push_back(i);
  }
  {
    while (true) {
      //  1. select one var for exact reduce
      size_t i;
      for (i = 0; i < idx_remain.size(); i++) {
        //  1.1 try to perform exact check, pick one which can be substituted
        //  exactly
        CMarsRangeExpr range_tmp(*this);
        int j = idx_remain[i];
        range_tmp.substitute(vec_sub_vars[j], vec_sub_lbs[j], vec_sub_ubs[j],
                             vec_sub_is_exact[j]);
#if _DEBUG_EXACT_FLAG_
        {
          CMarsVariable mv(vec_sub_vars[j], m_ast);
          cout << "--Try Exact (" << mv.print() << "): " << this->print_e()
               << " --> " << range_tmp.print_e() << endl;
        }
#endif
        //  1.2 perform the substitution if it is exact
        if (range_tmp.is_exact()) {
          *this = range_tmp;
          break;
        }
      }

      //  2. prepare for the next round if an exact substitutin is found
      if (i != idx_remain.size()) {
        //  2.1 remove idx_remain[i] from idx_remain
        {
          vector<int> idx_remain_new;
          for (size_t j = 0; j < idx_remain.size(); j++) {
            if (j != i) {
              idx_remain_new.push_back(idx_remain[j]);
            }
          }
          idx_remain = idx_remain_new;
        }
      } else {
        break;
      }
    }
  }

  {
    for (size_t i = 0; i < idx_remain.size(); i++) {
      int j = idx_remain[i];
      //  range.substitute(vec_sub_vars[i], vec_sub_lbs[i], vec_sub_ubs[i]);
      this->substitute(vec_sub_vars[j], vec_sub_lbs[j], vec_sub_ubs[j],
                       vec_sub_is_exact[j]);
    }
  }
}

#if KEEP_UNUSED
CMarsRangeExpr CMarsExpression::get_range_in_scope(void *range_scope) {
  if (range_scope == nullptr)
    range_scope = m_ast->GetProject();

  vector<void *> vec_loops;
  void *pos = (m_pos) ? m_pos : m_expr;
  m_ast->get_loop_nest_in_scope(pos, range_scope, &vec_loops);
  void *m_scope = range_scope;

  vector<void *> m_iv = vec_loops;
  //  2. substitute iv with ranges
  CMarsRangeExpr range(*this, *this);
  {
    for (size_t i = 0; i < m_iv.size(); i++) {
      CMarsRangeExpr var_range =
          CMarsVariable(m_iv[i], m_ast, pos, range_scope).get_range();

      CMarsExpression me_lb = var_range.get_lb();
      CMarsExpression me_ub = var_range.get_ub();

      range.substitute(m_iv[i], me_lb, me_ub);
    }
  }

  //  3. Single assignement tracing: 20160202 ZP
  //  Note:
  //  * In the current implementation, expression substitution in
  //  CMarsExpression stoped if the new term is not movable to the position of
  //  the old term.
  //  * However, in range analysis, movability is not necessary for the term,
  //  unless the tracing is within the scope of the m_scope.
  //  * So an additional tracing is needed to get the exact range at the scope
  vector<void *> m_pv;
  range.get_vars(&m_pv);
  {
    for (size_t i = 0; i < m_pv.size(); i++) {
      void *curr_term = m_pv[i];
      if (m_ast->IsVarRefExp(curr_term)) {
        void *curr_name = m_ast->GetVariableInitializedName(curr_term);
        void *sg_exp = m_ast->GetSingleAssignExpr(
            curr_name, m_pos, 0,
            range_scope);  //  support for scope is not added
        if (sg_exp) {
          if (m_ast->IsVarRefExp(sg_exp)) {
            void *new_name = m_ast->GetVariableInitializedName(sg_exp);
            void *sg_loop = m_ast->GetLoopFromIteratorByPos(new_name, m_pos, 1);
            if (sg_loop && m_ast->is_located_in_scope(sg_loop, m_scope)) {
              CMarsRangeExpr var_range =
                  CMarsVariable(sg_loop, m_ast, pos, range_scope).get_range();
              CMarsExpression me_lb = var_range.get_lb();
              CMarsExpression me_ub = var_range.get_ub();
              sg_exp = sg_loop;
              range.substitute(curr_term, me_lb, me_ub);

              continue;
            }
          }

          if (m_ast->is_located_in_scope(sg_exp, m_scope) &&
              m_ast->is_movable_test(sg_exp, m_pos)) {
            CMarsExpression me_exp(sg_exp, m_ast, nullptr);
            range.substitute(curr_term, me_exp, me_exp);
          }
        }
      }
    }
  }

  return range;
}
#endif

/************************************************************************/
//  class CMarsExpression
/************************************************************************/

const CMarsExpression pos_INF = CMarsExpression(0, 0, 0, 0, 0, 1);
const CMarsExpression neg_INF = CMarsExpression(0, 0, 0, 0, 0, 2);
const CMarsExpression full_INF = CMarsExpression(0, 0, 0, 0, 0, 3);

void *CMarsExpression::get_expr_from_coeff(bool final_expr) const {
  if (is_INF() != 0) {
    return nullptr;
  }
  assert(m_ast);
  void *expr_for_display = nullptr;
  if (m_const != 0) {
    int64_t val = m_const;
    if (m_ast->IsIntegerType(m_int_type)) {
      expr_for_display = m_ast->CreateConst(&val, m_int_type);
    } else {
      expr_for_display = m_ast->CreateConst(m_const);
    }
  }
  map<void *, CMarsExpression>::const_iterator it;
  vector<void *> terms;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    if ((it->second != 0) != 0) {
      void *var = it->first;
      if (m_ast->IsInitName(var) != 0) {
        var = m_ast->CreateVariableRef(var);
      } else if (m_ast->IsForStatement(var) != 0) {
        void *iter = m_ast->GetLoopIterator(var);
        if (iter == nullptr) {
          return nullptr;
        }
        var = m_ast->CreateVariableRef(iter);
      } else {
        void *var_org = var;

        DEFINE_START_END;
        STEMP(start);
        if (m_ast->IsInitializer(var) != 0) {
          var = m_ast->CreateVariableRef(m_ast->GetParent(var));
        } else {
          var = m_ast->CopyExp(var_org);
        }
        ACCTM(CopyExp, start, end);

        //  ZP: 20170314
        copy_variable_range(var_org, var);
      }
      void *term;
      if ((it->second == 1) != 0) {
        term = var;
      } else if ((it->second == -1) != 0) {
        term = m_ast->CreateExp(V_SgMinusOp, var);
      } else if (it->second.IsConstant() != 0) {
        void *coeff_exp = it->second.get_expr_from_coeff();
        term = m_ast->CreateExp(V_SgMultiplyOp, var, coeff_exp);
      } else {
        term = m_ast->CreateExp(V_SgMultiplyOp, var,
                                it->second.get_expr_from_coeff(final_expr));
      }
      terms.push_back(term);
    }
  }

  //  bug 1757: Cycle and resource result changes slightly and randomly
  //  so, we need to make sure the order of the terms
  //  the following sorts the terms by its unparse info
  // TODO(youxiang): if there is any run-time issue, we can consider to add a
  // vector to
  //  record the insert order to the map and iterator on the map based on the
  //  insert order.

  sort(terms.begin(), terms.end(), [&](void *lhs, void *rhs) {
    string file1 = m_ast->_p(lhs);
    string file2 = m_ast->_p(rhs);
    return file1 < file2;
  });
  for (auto term : terms) {
    if (expr_for_display != nullptr) {
      expr_for_display = m_ast->CreateExp(V_SgAddOp, expr_for_display, term);
    } else {
      expr_for_display = term;
    }
  }

  if (expr_for_display == nullptr) {
    expr_for_display = m_ast->CreateConst(0);
  }

  return expr_for_display;
}

string CMarsExpression::unparse() const {
  if (is_INF() != 0) {
    if (is_pINF() != 0) {
      return "+na";
    }
    if (is_nINF() != 0) {
      return "-na";
    }
    return "na";
  }
  if (IsConstant() != 0) {
    std::stringstream ss;
    switch (m_int_type) {
    case V_SgTypeBool:
      ss << static_cast<bool>(GetConstant());
      break;
    case V_SgTypeChar:
    case V_SgTypeSignedChar:
      ss << static_cast<int16_t>(GetConstant());
      break;
    case V_SgTypeUnsignedChar:
      ss << static_cast<uint16_t>(GetConstant());
      break;
    case V_SgTypeShort:
    case V_SgTypeSignedShort:
      ss << static_cast<int16_t>(GetConstant());
      break;
    case V_SgTypeUnsignedShort:
      ss << static_cast<uint16_t>(GetConstant());
      break;
    case V_SgTypeInt:
    case V_SgTypeSignedInt:
      ss << static_cast<int>(GetConstant());
      break;
    case V_SgTypeUnsignedInt:
      ss << static_cast<unsigned int>(GetConstant());
      break;
    case V_SgTypeLong:
    case V_SgTypeSignedLong:
      ss << static_cast<int64_t>(GetConstant());
      break;
    case V_SgTypeUnsignedLong:
      ss << static_cast<uint64_t>(GetConstant());
      break;
    default:
      return my_itoa(GetConstant());
    }
    return ss.str();
  }
  void *expr_for_display;

#if 1
  assert(m_ast);
  expr_for_display = get_expr_from_coeff();
#else
  expr_for_display = m_expr;
#endif

  string res = m_ast->UnparseToString(expr_for_display);
  if ((expr_for_display != nullptr) &&
      (m_ast->GetParent(expr_for_display) == nullptr)) {
    m_ast->DeleteNode(expr_for_display);
  }
  return res;
}

void CMarsExpression::print_debug() {
#if _DEBUG_PRINT_EXP_
  {
    if (is_INF()) {
      cout << "[ME debug] infinite=" << m_finite << endl;
    } else if (!IsConstant()) {
      string str_has_pos = (m_pos != m_expr) ? "pos=1" : "pos=0";
      cout << "[ME debug] " << m_ast << " " << str_has_pos << " " << m_pos
           << " " << print_coeff(1) << endl;
      //   m_expr << " " << print() << endl;  //  may introduce recursion
    }
  }
#endif

  //    if (m_expr == 0)
  //        m_expr = m_expr;
}
string CMarsExpression::print_coeff(int level) const {
  if (is_INF() != 0) {
    if (is_fINF() != 0) {
      return string("INF");
    }
    if (is_pINF() != 0) {
      return string("+INF");
    }
    if (is_nINF() != 0) {
      return string("-INF");
    }
    return "UNKNOWN";
  }

  if (IsConstant() != 0) {
    return my_itoa(m_const);
  }

  string str = "{";
  if (level >= 0) {
    str += unparse();
    str += ":";
  }
  vector<string> vec_strs;
  map<void *, CMarsExpression>::const_iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    void *sg_var = it->first;
    string sub_str;
    //    if (CMarsVariable::is_tmp_var(sg_var))
    //    {
    //        str += CMarsVariable::get_tmp_var_name(sg_var) + "->" +
    //            it->second.print_coeff(level) + ",";
    //        continue;
    //    }

    if (level > 0) {
      sub_str += m_ast->GetASTTypeString(sg_var) + " ";
    }

    //  if (level == 0)
    //  simply the display
    if ((m_ast->IsForStatement(sg_var) != 0) &&
        (m_ast->GetLoopIterator(sg_var) != nullptr)) {
      sg_var = m_ast->GetLoopIterator(sg_var);
    }
    sub_str += m_ast->UnparseToString(sg_var) + "->" +
               it->second.print_coeff(level) + ",";
    vec_strs.push_back(sub_str);
  }
  std::sort(vec_strs.begin(), vec_strs.end());
  for (auto one_str : vec_strs) {
    str += one_str;
  }
  str += my_itoa(m_const);

  str += "}";

  return str;
}
string CMarsExpression::print(int level) const {
  if (IsConstant() != 0) {
    return my_itoa(m_const);
  }

  string str = print_coeff(level);

  if (level == 1) {
    CMarsRangeExpr mr = get_range();
    str += " : " + mr.print();
  } else if (level >= 2) {
    CMarsRangeExpr mr = get_range();
    str += " : " + mr.print_const_bound();
  }

  return str;
}

CMarsExpression::CMarsExpression(CMarsAST_IF *ast, int64_t val) : m_ast(ast) {
  //  m_expr = ast->CreateConst(val);
  m_expr = nullptr;
  m_coeff.clear();
  m_const = val;
  m_pos = nullptr;
  m_scope = nullptr;
  if (val == CMarsRangeExpr::MARS_INT64_MAX) {
    m_finite = 1;
  } else if (val == CMarsRangeExpr::MARS_INT64_MIN) {
    m_finite = 2;
  } else {
    m_finite = 0;
  }
  m_int_type = V_SgTypeLong;
  print_debug();
}

CMarsExpression::CMarsExpression(CMarsAST_IF *ast, int val) : m_ast(ast) {
  //  m_expr = ast->CreateConst(val);
  m_expr = nullptr;
  m_coeff.clear();
  m_const = val;
  m_pos = nullptr;
  m_scope = nullptr;
  m_finite = 0;
  m_int_type = V_SgTypeInt;
  print_debug();
}

CMarsExpression::CMarsExpression(void *expr, map<void *, CMarsExpression> coeff,
                                 CMarsAST_IF *ast, int64_t const_i, void *pos,
                                 void *scope, int int_type, int finite) {
  m_expr = expr;
  m_coeff = coeff;
  m_ast = ast;
  m_const = const_i;
  m_pos = pos;
  m_scope = scope;
  m_finite = finite;
  m_int_type = int_type;
  reduce();
  //  if (!m_expr)
  //  m_expr = get_expr_from_coeff();
  print_debug();
  //  assert(m_ast->IsIntegerType(m_int_type));
}

void CMarsExpression::get_vars(vector<void *> *vars) const {
  // reduce();
  for (auto it = m_coeff.begin(); it != m_coeff.end(); it++) {
    vars->push_back(it->first);
  }
}
vector<void *> CMarsExpression::get_vars() const {
  vector<void *> vars;
  // reduce();
  for (auto it = m_coeff.begin(); it != m_coeff.end(); it++) {
    if (it->second != 0) {
      vars.push_back(it->first);
    }
  }
  return vars;
}

int CMarsExpression::any_var_located_in_scope(void *scope) {
  reduce();
  map<void *, CMarsExpression>::iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    void *term = it->first;
    if (m_ast->IsInitializer(term)) {
      term = m_ast->GetParent(term);
    }
    if (m_ast->any_var_located_in_scope(term, scope))
      return 1;
    if (it->second.any_var_located_in_scope(scope) != 0) {
      return 1;
    }
  }

  return 0;
}

int CMarsExpression::any_var_located_in_scope_except_loop_iterator(
    void *scope) {
  reduce();
  map<void *, CMarsExpression>::iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    if (m_ast->IsForStatement(it->first))
      continue;
    if (m_ast->any_var_located_in_scope(it->first, scope))
      return 1;
  }
  return 0;
}

//  Ignore for-loops
int CMarsExpression::is_movable_to(void *pos) {
  reduce();
  map<void *, CMarsExpression>::iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    if (m_ast->IsForStatement(it->first) != 0) {
      if (m_ast->is_located_in_scope_simple(pos, it->first) != 0) {
        continue;
      }
      return 0;
    }

    int move = m_ast->is_movable_test(it->first, pos, m_pos);

#if PROJDEBUG

#endif

    if (move == 0) {
      return 0;
    }
    if (it->second.is_movable_to(pos) == 0) {
      return 0;
    }
  }

  return 1;
}

CMarsExpression operator+(const CMarsExpression &op1,
                          const CMarsExpression &op2) {
  //  handling infinite
  {
    if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
      return full_INF;
    }
    if ((op1.is_nINF() != 0) || (op2.is_nINF() != 0)) {
      return neg_INF;
    }
    if ((op1.is_pINF() != 0) || (op2.is_pINF() != 0)) {
      return pos_INF;
    }
  }
  if ((op1 == 0) != 0) {
    return op2;
  }
  if ((op2 == 0) != 0) {
    return op1;
  }
  DEFINE_START_END;
  STEMP(start);

  CMarsAST_IF *out_ast;
  int64_t out_const;
  void *out_expr = nullptr;
  map<void *, CMarsExpression> out_coeff;
  void *out_pos = nullptr;

  assert(op1.m_ast == op2.m_ast);
  assert(op1.m_ast);
  CMarsAST_IF *ast = op1.m_ast;
  out_ast = ast;
  void *out_scope = ast->GetCommonPosition(op1.m_scope, op2.m_scope);
  int out_int_type = op1.m_int_type;
  if (op1.IsConstant() != 0) {
    out_int_type = op2.m_int_type;
  } else if (op2.IsConstant() == 0) {
    out_int_type =
        ast->GetBinaryResultType(op1.m_int_type, op2.m_int_type, V_SgAddOp);
    if (out_int_type == 0) {
      return full_INF;
    }
  }
  if (out_scope == nullptr) {
    out_scope = op1.m_scope != nullptr ? op1.m_scope
                                       : op2.m_scope;  //  FIXME: if not proper
  }

  if ((op1.m_expr != nullptr) && (ast->is_floating_node(op1.m_expr) == 0) &&
      (op1.m_expr != nullptr) && (ast->is_floating_node(op2.m_expr) == 0) &&
      ast->GetParent(op1.m_expr) == ast->GetParent(op2.m_expr) &&
      (ast->IsAddOp(ast->GetParent(op1.m_expr)) != 0)) {
    out_expr = ast->GetParent(op1.m_expr);
    out_pos = out_expr;
  } else {
    out_pos = ast->GetCommonPosition(op1.m_pos, op2.m_pos);
    if (out_pos == nullptr) {
      out_pos = op1.m_pos != nullptr ? op1.m_pos
                                     : op2.m_pos;  //  FIXME: if not proper
    }
  }
  if ((op1.IsConstant() != 0) && (op2.IsConstant() != 0)) {
    CMarsExpression ret(op1);
    ret.m_const += op2.m_const;
    ret.m_pos = out_pos;
    ret.m_scope = out_scope;
    ret.m_int_type = out_int_type;
    return ret;
  }

  out_const = op1.m_const + op2.m_const;
  out_coeff = op1.m_coeff;
  map<void *, CMarsExpression>::iterator it;
  map<void *, CMarsExpression> map2 = op2.m_coeff;
  for (it = map2.begin(); it != map2.end(); it++) {
    if (out_coeff.find(it->first) == out_coeff.end()) {
      out_coeff[it->first] = it->second;
    } else {
      out_coeff[it->first] = out_coeff[it->first] + it->second;
    }
  }
  ACCTM(get_range___isNormal___mr_lb_ub___add, start, end);
  return CMarsExpression(out_expr, out_coeff, out_ast, out_const, out_pos,
                         out_scope, out_int_type, 0);
}

CMarsExpression operator-(const CMarsExpression &op1,
                          const CMarsExpression &op2) {
  //  handling infinite
  {
    if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
      return full_INF;
    }
    if ((op1.is_nINF() != 0) || (op2.is_pINF() != 0)) {
      return neg_INF;
    }
    if ((op1.is_pINF() != 0) || (op2.is_nINF() != 0)) {
      return pos_INF;
    }
  }
  if ((op1 == 0) != 0) {
    return -op2;
  }
  if ((op2 == 0) != 0) {
    return op1;
  }
  DEFINE_START_END;
  STEMP(start);

  CMarsAST_IF *out_ast;
  int64_t out_const;
  void *out_expr = nullptr;
  map<void *, CMarsExpression> out_coeff;
  void *out_pos = nullptr;

  assert(op1.m_ast == op2.m_ast);
  assert(op1.m_ast);
  CMarsAST_IF *ast = op1.m_ast;
  out_ast = ast;
  void *out_scope = ast->GetCommonPosition(op1.m_scope, op2.m_scope);
  int out_int_type = op1.m_int_type;
  if (op1.IsConstant() != 0) {
    out_int_type = op2.m_int_type;
  } else if (op2.IsConstant() == 0) {
    out_int_type =
        ast->GetBinaryResultType(op1.m_int_type, op2.m_int_type, V_SgAddOp);
    if (out_int_type == 0) {
      return full_INF;
    }
  }
  if (out_scope == nullptr) {
    out_scope = op1.m_scope != nullptr ? op1.m_scope
                                       : op2.m_scope;  //  FIXME: if not proper
  }

  if ((op1.m_expr != nullptr) && (ast->is_floating_node(op1.m_expr) == 0) &&
      (op1.m_expr != nullptr) && (ast->is_floating_node(op2.m_expr) == 0) &&
      ast->GetParent(op1.m_expr) == ast->GetParent(op2.m_expr) &&
      (ast->IsSubtractOp(ast->GetParent(op1.m_expr)) != 0)) {
    out_expr = ast->GetParent(op1.m_expr);
    out_pos = out_expr;
  } else {
    out_pos = ast->GetCommonPosition(op1.m_pos, op2.m_pos);
    if (out_pos == nullptr) {
      out_pos = op1.m_pos != nullptr ? op1.m_pos
                                     : op2.m_pos;  //  FIXME: if not proper
    }
  }
  if ((op1.IsConstant() != 0) && (op2.IsConstant() != 0)) {
    CMarsExpression ret(op1);
    ret.m_const -= op2.m_const;
    ret.m_pos = out_pos;
    ret.m_scope = out_scope;
    ret.m_int_type = out_int_type;
    return ret;
  }

  out_const = op1.m_const - op2.m_const;
  out_coeff = op1.m_coeff;
  map<void *, CMarsExpression>::iterator it;
  map<void *, CMarsExpression> map2 = op2.m_coeff;
  for (it = map2.begin(); it != map2.end(); it++) {
    if (out_coeff.find(it->first) == out_coeff.end()) {
      out_coeff[it->first] = -it->second;
    } else {
      out_coeff[it->first] = out_coeff[it->first] - it->second;
    }
  }
  ACCTM(get_range___isNormal___mr_lb_ub___add, start, end);
  return CMarsExpression(out_expr, out_coeff, out_ast, out_const, out_pos,
                         out_scope, out_int_type, 0);
}

CMarsExpression operator+(const CMarsExpression &op1, int64_t op2) {
  if (op2 == 0) {
    return op1;
  }
  CMarsExpression ret(op1);
  ret.m_expr = nullptr;
  ret.m_const += op2;
  return ret;
}

CMarsExpression operator-(const CMarsExpression &op1, int64_t op2) {
  if (op2 == 0) {
    return op1;
  }
  CMarsExpression ret(op1);
  ret.m_const -= op2;
  ret.m_expr = nullptr;
  return ret;
}

int handle_infinite_in_mul_div(const CMarsExpression &op1,
                               const CMarsExpression &op2,
                               CMarsExpression *ret) {
  //  first priority
  if (op1.is_zero() != 0) {
    *ret = op1;
  } else if (op2.is_zero() != 0) {
    *ret = op2;  //  for div by zero case, the ret is assumed to be zero

    //  exclude simple condition first
  } else if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
    *ret = full_INF;

    //  in case we can judge the sign
  } else if ((op1.is_nINF() != 0) && (op2.IsNonNegative() != 0)) {
    *ret = neg_INF;
  } else if ((op1.is_nINF() != 0) && (op2.IsNonPositive() != 0)) {
    *ret = pos_INF;
  } else if ((op1.is_pINF() != 0) && (op2.IsNonNegative() != 0)) {
    *ret = pos_INF;
  } else if ((op1.is_pINF() != 0) && (op2.IsNonPositive() != 0)) {
    *ret = neg_INF;
  } else if ((op2.is_nINF() != 0) && (op1.IsNonNegative() != 0)) {
    *ret = neg_INF;
  } else if ((op2.is_nINF() != 0) && (op1.IsNonPositive() != 0)) {
    *ret = pos_INF;
  } else if ((op2.is_pINF() != 0) && (op1.IsNonNegative() != 0)) {
    *ret = pos_INF;
  } else if ((op2.is_pINF() != 0) && (op1.IsNonPositive() != 0)) {
    *ret = neg_INF;

    //  otherwise, full_INF
  } else if ((op1.is_INF() != 0) || (op2.is_INF() != 0)) {
    *ret = full_INF;
  } else {
    return 0;
  }

  return 1;  //  the case is handled already
}

int handle_infinite_in_shift(const CMarsExpression &op1,
                             const CMarsExpression &op2, CMarsExpression *ret) {
  //  first priority
  if ((op1.is_zero() != 0) || (op2.is_zero() != 0)) {
    *ret = op1;

    //  exclude simple condition first
  } else if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
    *ret = full_INF;

    //  in case we can judge the sign
  } else if (op1.is_nINF() != 0) {
    *ret = neg_INF;
  } else if (op1.is_pINF() != 0) {
    *ret = pos_INF;
  } else {
    return 0;
  }

  return 1;  //  the case is handled already
}

CMarsExpression operator*(const CMarsExpression &op1,
                          const CMarsExpression &op2) {
  //  handling infinite
  {
    CMarsExpression ret;
    if (handle_infinite_in_mul_div(op1, op2, &ret) != 0) {
      return ret;
    }
  }
  DEFINE_START_END;
  STEMP(start);

  CMarsAST_IF *out_ast;
  int64_t out_const;
  void *out_expr = nullptr;
  map<void *, CMarsExpression> out_coeff;
  void *out_pos = nullptr;

  assert(op1.m_ast == op2.m_ast);
  assert(op1.m_ast);
  CMarsAST_IF *ast = op1.m_ast;
  if (((op1 == 0) != 0) || ((op2 == 0) != 0)) {
    return CMarsExpression(ast, 0);
  }

  out_ast = ast;
  void *out_scope = ast->GetCommonPosition(op1.m_scope, op2.m_scope);
  int out_int_type = op1.m_int_type;
  if (op1.IsConstant() != 0) {
    out_int_type = op2.m_int_type;
  } else if (op2.IsConstant() == 0) {
    out_int_type =
        ast->GetBinaryResultType(op1.m_int_type, op2.m_int_type, V_SgAddOp);
    if (out_int_type == 0) {
      return full_INF;
    }
  }
  if (out_scope == nullptr) {
    out_scope = op1.m_scope != nullptr ? op1.m_scope
                                       : op2.m_scope;  //  FIXME: if not proper
  }

  if ((op1.m_expr != nullptr) && (ast->is_floating_node(op1.m_expr) == 0) &&
      (op2.m_expr != nullptr) && (ast->is_floating_node(op2.m_expr) == 0) &&
      ast->GetParent(op1.m_expr) == ast->GetParent(op2.m_expr) &&
      (ast->IsMultiplyOp(ast->GetParent(op1.m_expr)) != 0)) {
    out_expr = ast->GetParent(op1.m_expr);
    out_pos = out_expr;
  } else {
    out_pos = ast->GetCommonPosition(op1.m_pos, op2.m_pos);
    if (out_pos == nullptr) {
      out_pos = op1.m_pos != nullptr ? op1.m_pos
                                     : op2.m_pos;  //  FIXME: if not proper
    }
  }
  if ((op1.IsConstant() != 0) && (op2.IsConstant() != 0)) {
    CMarsExpression ret(op1);
    ret.m_const *= op2.m_const;
    ret.m_pos = out_pos;
    ret.m_scope = out_scope;
    ret.m_int_type = out_int_type;
    return ret;
  }

  if ((op1.IsConstant() == 0) && (op2.IsConstant() == 0)) {
    //  non-linear term generated;
    if (out_expr == nullptr) {
      void *op1_exp = op1.get_expr_from_coeff();
      void *op2_exp = op2.get_expr_from_coeff();
      out_expr =
          ast->CreateExp(V_SgMultiplyOp, op1_exp, op2_exp, 0, nullptr, false);
    }
    out_coeff.clear();
    out_coeff[out_expr] = CMarsExpression(ast, 1);
    out_const = 0;

  } else {
    int64_t mult_op1;
    CMarsExpression me_mult_op1;
    CMarsExpression me_mult_op2;
    map<void *, CMarsExpression> mult_op2;

    if (op1.IsConstant() != 0) {
      me_mult_op1 = op1;
      mult_op1 = op1.GetConstant();
      mult_op2 = op2.m_coeff;
    } else if (op2.IsConstant() != 0) {
      me_mult_op1 = op2;
      mult_op1 = op2.GetConstant();
      mult_op2 = op1.m_coeff;
    } else {
      assert(0);
    }
    map<void *, CMarsExpression>::iterator it;
    out_coeff.clear();
    for (it = mult_op2.begin(); it != mult_op2.end(); it++) {
      if (mult_op1 == 1) {
        out_coeff[it->first] = it->second;
      } else {
        out_coeff[it->first] = it->second * me_mult_op1;
      }
    }
    out_const = op1.m_const * op2.m_const;
  }
  ACCTM(get_range___isNormal___mr_lb_ub___mul, start, end);

  return CMarsExpression(out_expr, out_coeff, out_ast, out_const, out_pos,
                         out_scope, out_int_type, 0);
}

CMarsExpression operator*(const CMarsExpression &op1, int64_t op2) {
  return op1 * CMarsExpression(op1.get_ast(), op2);
}

CMarsExpression operator<<(const CMarsExpression &op1,
                           const CMarsExpression &op2) {
  //  handling infinite
  {
    CMarsExpression ret;
    if (handle_infinite_in_shift(op1, op2, &ret) != 0) {
      return ret;
    }
  }

  CMarsAST_IF *out_ast;
  int64_t out_const;
  void *out_expr = nullptr;
  map<void *, CMarsExpression> out_coeff;
  void *out_pos = nullptr;

  assert(op1.m_ast == op2.m_ast);
  assert(op1.m_ast);
  CMarsAST_IF *ast = op1.m_ast;
  out_ast = ast;
  void *out_scope = ast->GetCommonPosition(op1.m_scope, op2.m_scope);
  int out_int_type = op1.m_int_type;
  if (out_scope == nullptr) {
    out_scope = op1.m_scope != nullptr ? op1.m_scope
                                       : op2.m_scope;  //  FIXME: if not proper
  }

  if ((op1.m_expr != nullptr) && (ast->is_floating_node(op1.m_expr) == 0) &&
      (op2.m_expr != nullptr) && (ast->is_floating_node(op2.m_expr) == 0) &&
      ast->GetParent(op1.m_expr) == ast->GetParent(op2.m_expr) &&
      (ast->IsMultiplyOp(ast->GetParent(op1.m_expr)) != 0)) {
    out_expr = ast->GetParent(op1.m_expr);
    out_pos = out_expr;
  } else {
    out_pos = ast->GetCommonPosition(op1.m_pos, op2.m_pos);
    if (out_pos == nullptr) {
      out_pos = op1.m_pos != nullptr ? op1.m_pos
                                     : op2.m_pos;  //  FIXME: if not proper
    }
  }

  if ((op1.IsConstant() != 0) && (op2.IsConstant() != 0)) {
    CMarsExpression ret(op1);
    ret.m_const <<= op2.m_const;
    ret.m_pos = out_pos;
    ret.m_scope = out_scope;
    return ret;
  }

  if (op2.IsConstant() == 0) {
    //  non-linear term generated;
    if (out_expr == nullptr) {
      void *op1_exp = op1.get_expr_from_coeff();
      void *op2_exp = op2.get_expr_from_coeff();
      out_expr =
          ast->CreateExp(V_SgLshiftOp, op1_exp, op2_exp, 0, nullptr, false);
    }
    out_coeff.clear();
    out_coeff[out_expr] = CMarsExpression(ast, 1);
    out_const = 0;
  } else {
    int64_t mult_op1;
    map<void *, CMarsExpression> mult_op2;
    if (op2.IsConstant() != 0) {
      mult_op1 = 1 << op2.GetConstant();
      mult_op2 = op1.m_coeff;
    } else {
      assert(0);
    }

    map<void *, CMarsExpression>::iterator it;
    out_coeff.clear();
    for (it = mult_op2.begin(); it != mult_op2.end(); it++) {
      if (mult_op1 == 0) {
        out_coeff[it->first] = CMarsExpression(ast, 0);
      } else if (mult_op1 == 1) {
        out_coeff[it->first] = it->second;
      } else {
        out_coeff[it->first] = it->second * mult_op1;
      }
    }
    out_const = op1.m_const * mult_op1;
  }

  return CMarsExpression(out_expr, out_coeff, out_ast, out_const, out_pos,
                         out_scope, out_int_type, 0);
}

int divisible(int64_t op1, int64_t op2) {
  return op2 && static_cast<int>(op1 % op2 == 0);
}

#define _DEBUG_DIVIDABILITY_ 0
int divisible(const CMarsExpression &op1, int64_t op2, bool check_range) {
#if _DEBUG_DIVIDABILITY_
  cout << "Dividability: " << op1.print_s() << ", " << op2 << endl;
#endif

  //  1. Note that infinite is supposed to be divisible
  if (op1.is_INF() != 0) {
    return 1;
  }

  {
    //  2. check whether all the component are divisible
    int all_divisible = 1;
    if (divisible(op1.GetConstant(), op2) == 0) {
      all_divisible = 0;
    }
    if (all_divisible != 0) {
      map<void *, CMarsExpression> coeff = op1.m_coeff;
      map<void *, CMarsExpression>::iterator it;
      for (it = coeff.begin(); it != coeff.end(); it++) {
        if (divisible(it->second, op2, check_range) == 0) {
          all_divisible = 0;
          break;
        }
      }
    }
    if (all_divisible != 0) {
#if _DEBUG_DIVIDABILITY_
      cout << "Dividability: Yes" << endl;
#endif
      return 1;
    }
  }
  //  3. check whether the range is constant
  if (check_range && op1.has_range()) {
    CMarsRangeExpr mr = op1.get_range();
    if ((mr.IsConstant() != 0) && (divisible(mr.get_const_lb(), op2) != 0)) {
#if _DEBUG_DIVIDABILITY_
      cout << "Dividability: Yes" << endl;
#endif
      return 1;
    }
  }

#if _DEBUG_DIVIDABILITY_
  cout << "Dividability: No" << endl;
#endif
  return 0;
}

static void *get_original_binary_expression(CMarsAST_IF *ast, int opcode,
                                            void *operand) {
  void *parent = ast->GetParent(operand);
  while (ast->IsExpression(parent)) {
    int op = ast->GetExpressionOp(parent);
    if (op == V_SgModOp) {
      return parent;
    }
    if (ast->IsFunctionCall(parent)) {
      void *left_exp, *right_exp;
      left_exp = right_exp = nullptr;
      ast->IsAPIntOp(parent, &left_exp, &right_exp, &op);
      if (op == V_SgModOp) {
        return parent;
      }
    }
    parent = ast->GetParent(parent);
  }
  return nullptr;
}
//  /////////////////////////////////////////////  /
//  Note: in general this function will try to do the devision, if not
//  feasible, non-linear terms will be generated
//  1. if op2 is not constant, generate non-linear term
//  2. if op1 is also a constant, direct divide (FIXME: the integer division
//  is performed here,
//    which may cause MISMATCH with original double division, to be fixed)
//  3. if op1 is not a constant, each of the coefficients of op1 is divided by
//  op2, all the non-divisible terms are
//    added together to form a new term for the division: (3+2x+3y+4z)/3 -> 1
//    + y + (2x+4y)/3
CMarsExpression operator/(const CMarsExpression &op1,
                          const CMarsExpression &op2) {
  //  handling infinite
  {
    CMarsExpression ret;
    if (handle_infinite_in_mul_div(op1, op2, &ret) != 0) {
      return ret;
    }
  }

  CMarsAST_IF *out_ast;
  int64_t out_const;
  void *out_expr = nullptr;
  map<void *, CMarsExpression> out_coeff;
  void *out_pos = nullptr;

  assert(op1.m_ast == op2.m_ast);
  assert(op1.m_ast);
  CMarsAST_IF *ast = op1.m_ast;
  out_ast = ast;
  void *out_scope = ast->GetCommonPosition(op1.m_scope, op2.m_scope);
  int out_int_type = op1.m_int_type;
  if (op1.IsConstant() != 0) {
    out_int_type = op2.m_int_type;
  } else if (op2.IsConstant() == 0) {
    out_int_type =
        ast->GetBinaryResultType(op1.m_int_type, op2.m_int_type, V_SgAddOp);
    if (out_int_type == 0) {
      return full_INF;
    }
  }
  if (out_scope == nullptr) {
    out_scope = op1.m_scope != nullptr ? op1.m_scope
                                       : op2.m_scope;  //  FIXME: if not proper
  }

  if ((op1.m_expr != nullptr) && (ast->is_floating_node(op1.m_expr) == 0) &&
      (op2.m_expr != nullptr) && (ast->is_floating_node(op2.m_expr) == 0) &&
      ast->GetParent(op1.m_expr) == ast->GetParent(op2.m_expr)) {
    out_expr = get_original_binary_expression(ast, V_SgDivideOp, op1.m_expr);
    out_pos = out_expr;
  } else {
    out_pos = ast->GetCommonPosition(op1.m_pos, op2.m_pos);
    if (out_pos == nullptr) {
      out_pos = op1.m_pos != nullptr ? op1.m_pos
                                     : op2.m_pos;  //  FIXME: if not proper
    }
  }

  if ((op1.IsConstant() != 0) && (op2.IsConstant() != 0)) {
    CMarsExpression ret(op1);
    ret.m_const /= op2.GetConstant();
    ret.m_pos = out_pos;
    ret.m_scope = out_scope;
    ret.m_int_type = out_int_type;
    return ret;
  }
  CMarsExpression op1_minus_constant = op1 - op1.GetConstant();
  if (op2.IsConstant() == 0 ||
      (op2.GetConstant() != 0 &&
       !divisible(op1_minus_constant, op2.GetConstant(), false))) {
    //  non-linear term generated;
    if (out_expr == nullptr) {
      //  /////////////////  /
      //  add by ZP 20150727
      //  assert(0);
      //  /////////////////  /

      void *op1_exp = op1.get_expr_from_coeff();
      void *op2_exp = op2.get_expr_from_coeff();
      out_expr =
          ast->CreateExp(V_SgDivideOp, op1_exp, op2_exp, 0, nullptr, false);
    }

    out_coeff.clear();
    out_coeff[out_expr] = CMarsExpression(ast, 1);
    out_const = 0;
  } else {
    map<void *, CMarsExpression> div_op1 = op1.m_coeff;
    int64_t div_op2 = op2.GetConstant();
    if (div_op2 == 0) {
      return full_INF;
    }
    map<void *, CMarsExpression>::iterator it;
    out_coeff.clear();
    for (it = div_op1.begin(); it != div_op1.end(); it++) {
      //  ///////////////////////////////////  /
      out_coeff[it->first] = it->second / div_op2;
    }
    out_const = op1.m_const / div_op2;
    //  ///////////////////////////////////  /
  }

  return CMarsExpression(out_expr, out_coeff, out_ast, out_const, out_pos,
                         out_scope, out_int_type, 0);
}

CMarsExpression operator/(const CMarsExpression &op1, int64_t op2) {
  return op1 / CMarsExpression(op1.get_ast(), op2);
}
//  /////////////////////////////////////////////  /
//  Note: in general this function will try to do the mod, if not feasible,
//  non-linear terms will be generated
//  1. if op2 is not constant, generate non-linear term
//  2. if op1 is also a constant, direct mod
//  3. if op1 is not a constant, each of the coefficients of op1 is divided by
//  op2, all the non-divisible terms are
//    added together to form a new term for the mod: (4+2x+3y+4z)%3 ->
//    (2x+4y + 1) % 3 according to distributive rule
//    (a+b) mod c=(a mod c+ b mod c) mod c

CMarsExpression operator%(const CMarsExpression &op1,
                          const CMarsExpression &op2) {
  //  handling infinite
  {
    CMarsExpression ret;
    if (handle_infinite_in_mul_div(op1, op2, &ret) != 0) {
      return ret;
    }
  }

  CMarsAST_IF *out_ast;
  int64_t out_const;
  void *out_expr = nullptr;
  map<void *, CMarsExpression> out_coeff;
  void *out_pos = nullptr;

  assert(op1.m_ast == op2.m_ast);
  assert(op1.m_ast);
  CMarsAST_IF *ast = op1.m_ast;
  out_ast = ast;
  void *out_scope = ast->GetCommonPosition(op1.m_scope, op2.m_scope);
  int out_int_type = op1.m_int_type;
  if (op1.IsConstant() != 0) {
    out_int_type = op2.m_int_type;
  } else if (op2.IsConstant() == 0) {
    out_int_type =
        ast->GetBinaryResultType(op1.m_int_type, op2.m_int_type, V_SgAddOp);
    if (out_int_type == 0) {
      return full_INF;
    }
  }
  if (out_scope == nullptr) {
    out_scope = op1.m_scope != nullptr ? op1.m_scope
                                       : op2.m_scope;  //  FIXME: if not proper
  }

  if ((op1.m_expr != nullptr) && (ast->is_floating_node(op1.m_expr) == 0) &&
      (op2.m_expr != nullptr) && (ast->is_floating_node(op2.m_expr) == 0) &&
      ast->GetParent(op1.m_expr) == ast->GetParent(op2.m_expr)) {
    out_expr = get_original_binary_expression(ast, V_SgModOp, op1.m_expr);
    out_pos = out_expr;
  } else {
    out_pos = ast->GetCommonPosition(op1.m_pos, op2.m_pos);
    if (out_pos == nullptr) {
      out_pos = op1.m_pos != nullptr ? op1.m_pos
                                     : op2.m_pos;  //  FIXME: if not proper
    }
  }

  if ((op1.IsConstant() != 0) && (op2.IsConstant() != 0)) {
    CMarsExpression ret(op1);
    auto val = op2.GetConstant();
    if (val != 0) {
      ret.m_const %= val;
      ret.m_pos = out_pos;
      ret.m_scope = out_scope;
      ret.m_int_type = out_int_type;
      return ret;
    } else {
      return full_INF;
    }
  }
  CMarsExpression op1_minus_constant = op1 - op1.GetConstant();
  if (op2.IsConstant() == 0 ||
      (op2.GetConstant() != 0 &&
       !divisible(op1_minus_constant, op2.GetConstant(), false))) {
    //  non-linear term generated;
    if (out_expr == nullptr) {
      //  /////////////////  /
      //  add by ZP 20150727
      //  assert(0);
      //  /////////////////  /

      void *op1_exp = op1.get_expr_from_coeff();
      void *op2_exp = op2.get_expr_from_coeff();
      out_expr = ast->CreateExp(V_SgModOp, op1_exp, op2_exp, 0, nullptr, false);
    }

    out_coeff.clear();
    out_coeff[out_expr] = CMarsExpression(ast, 1);
    out_const = 0;
  } else {
    map<void *, CMarsExpression> mod_op1 = op1.m_coeff;
    int64_t mod_op2 = op2.GetConstant();
    if (mod_op2 == 0) {
      return full_INF;
    }
    map<void *, CMarsExpression>::iterator it;
    out_coeff.clear();
    out_const = op1.m_const % mod_op2;
    //  ///////////////////////////////////  /
  }

  return CMarsExpression(out_expr, out_coeff, out_ast, out_const, out_pos,
                         out_scope, out_int_type, 0);
}

CMarsExpression operator%(const CMarsExpression &op1, int64_t op2) {
  return op1 % CMarsExpression(op1.get_ast(), op2);
}

CMarsExpression operator>>(const CMarsExpression &op1,
                           const CMarsExpression &op2) {
  //  handling infinite
  {
    CMarsExpression ret;
    if (handle_infinite_in_shift(op1, op2, &ret) != 0) {
      return ret;
    }
  }

  CMarsAST_IF *out_ast;
  int64_t out_const;
  void *out_expr = nullptr;
  map<void *, CMarsExpression> out_coeff;
  void *out_pos = nullptr;

  assert(op1.m_ast == op2.m_ast);
  assert(op1.m_ast);
  CMarsAST_IF *ast = op1.m_ast;
  out_ast = ast;
  void *out_scope = ast->GetCommonPosition(op1.m_scope, op2.m_scope);
  if (out_scope == nullptr) {
    out_scope = op1.m_scope != nullptr ? op1.m_scope
                                       : op2.m_scope;  //  FIXME: if not proper
  }
  int out_int_type = op1.m_int_type;
  if ((op1.m_expr != nullptr) && (ast->is_floating_node(op1.m_expr) == 0) &&
      (op2.m_expr != nullptr) && (ast->is_floating_node(op2.m_expr) == 0) &&
      ast->GetParent(op1.m_expr) == ast->GetParent(op2.m_expr)) {
    out_expr = get_original_binary_expression(ast, V_SgRshiftOp, op1.m_expr);
    out_pos = out_expr;
  } else {
    out_pos = ast->GetCommonPosition(op1.m_pos, op2.m_pos);
    if (out_pos == nullptr) {
      out_pos = op1.m_pos != nullptr ? op1.m_pos
                                     : op2.m_pos;  //  FIXME: if not proper
    }
  }

  //  /////////////////////////////////////  /
  //  ZP: 20150815
  if ((op1.IsConstant() != 0) && (op2.IsConstant() != 0)) {
    CMarsExpression ret(op1);
    ret.m_const >>= op2.m_const;
    ret.m_pos = out_pos;
    ret.m_scope = out_scope;
    return ret;
  }
  CMarsExpression op1_minus_constant = op1 - op1.GetConstant();
  //  /////////////////////////////////////  /
  if (op2.IsConstant() == 0 ||
      !divisible(op1, 1L << op2.GetConstant(), false)) {
    //  if (1) //  ZP: 20150727
    //  non-linear term generated;
    if (out_expr == nullptr) {
      //  /////////////////  /
      //  add by ZP 20150727
      //  assert(0);
      //  /////////////////  /

      void *op1_exp = op1.get_expr_from_coeff();
      void *op2_exp = op2.get_expr_from_coeff();
      out_expr =
          ast->CreateExp(V_SgRshiftOp, op1_exp, op2_exp, 0, nullptr, false);
    }

    out_coeff.clear();
    out_coeff[out_expr] = CMarsExpression(ast, 1);
    out_const = 0;
    //  } else if (op1.IsConstant()) {
    //    out_coeff.clear();
    //    out_const = op1.m_const / op2.m_const;
  } else {
    map<void *, CMarsExpression> div_op1 = op1.m_coeff;
    int64_t div_op2 = 1L << op2.GetConstant();

    map<void *, CMarsExpression>::iterator it;
    out_coeff.clear();
    for (it = div_op1.begin(); it != div_op1.end(); it++) {
      out_coeff[it->first] = it->second / div_op2;
    }
    out_const = op1.m_const / div_op2;
    //  ///////////////////////////////////  /
  }

  return CMarsExpression(out_expr, out_coeff, out_ast, out_const, out_pos,
                         out_scope, out_int_type, 0);
}

int operator==(const CMarsExpression &op1, const CMarsExpression &op2) {
  {
    if ((op1.is_INF() != 0) || (op2.is_INF() != 0)) {
      return static_cast<int>(op1.m_finite == op2.m_finite);
    }
  }

  CMarsAST_IF *ast = op1.m_ast;
  assert(op1.m_ast == op2.m_ast);
  //  if (op1.m_expr != op2.m_expr) return 0;

  if (op2.m_coeff.empty() && op2.m_const == 0) {
    if (!op1.m_coeff.empty()) {
      return 0;
    }
    if (op1.m_const != 0) {
      return 0;
    }
    return 1;
  }
  CMarsExpression sub = op1 - op2;

  return (sub == CMarsExpression(ast, 0));
}
int operator!=(const CMarsExpression &op1, const CMarsExpression &op2) {
  return static_cast<int>(static_cast<int>((op1 == op2)) == 0);
}
int operator==(const CMarsExpression &op1, int64_t op2) {
  if (op1.is_INF() != 0) {
    return 0;
  }
  if (!op1.m_coeff.empty()) {
    return 0;
  }
  if (op1.m_const != op2) {
    return 0;
  }
  return 1;
}
int operator!=(const CMarsExpression &op1, int64_t op2) {
  return static_cast<int>(static_cast<int>((op1 == op2)) == 0);
}

int operator>=(const CMarsExpression &op1, const CMarsExpression &op2) {
  if (op1.is_INF() && op2.is_INF() && (op1 == op2) != 0) {
    return 1;
  }
  if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
    //  undetermined
    return 0;
  }
  if ((op1.is_pINF() != 0) || (op2.is_nINF() != 0)) {
    return 1;
  }
  if ((op1.is_nINF() != 0) || (op2.is_pINF() != 0)) {
    return 0;
  }
  CMarsExpression diff = op1 - op2;
  if (diff.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() >= op2.GetConstant());
  }
  if ((diff.IsSignned() != 0) && diff.has_range()) {
    if (diff.IsNonNegative() != 0) {
      return 1;
    }
    return 0;
  }
  if (op1.has_range() && op2.has_range()) {
    CMarsRangeExpr mr1 = op1.get_range();
    CMarsRangeExpr mr2 = op2.get_range();
    return static_cast<int>(mr1.get_const_lb() >= mr2.get_const_ub());
  }
  return 0;
}

int operator>(const CMarsExpression &op1, const CMarsExpression &op2) {
  if (op1.is_INF() && op2.is_INF() && (op1 == op2) != 0) {
    return 0;
  }
  if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
    //  undetermined
    return 0;
  }
  if ((op1.is_pINF() != 0) || (op2.is_nINF() != 0)) {
    return 1;
  }
  if ((op1.is_nINF() != 0) || (op2.is_pINF() != 0)) {
    return 0;
  }
  CMarsExpression diff = op1 - op2 - 1;
  if (diff.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() > op2.GetConstant());
  }
  if ((diff.IsSignned() != 0) && diff.has_range()) {
    if (diff.IsNonNegative() != 0) {
      return 1;
    }
    return 0;
  }
  if (op1.has_range() && op2.has_range()) {
    CMarsRangeExpr mr1 = op1.get_range();
    CMarsRangeExpr mr2 = op2.get_range();
    return static_cast<int>(mr1.get_const_lb() > mr2.get_const_ub());
  }
  return 0;
}

int operator<=(const CMarsExpression &op1, const CMarsExpression &op2) {
  if (op1.is_INF() && op2.is_INF() && (op1 == op2) != 0) {
    return 1;
  }
  if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
    //  undetermined
    return 0;
  }
  if ((op1.is_pINF() != 0) || (op2.is_nINF() != 0)) {
    return 0;
  }
  if ((op1.is_nINF() != 0) || (op2.is_pINF() != 0)) {
    return 1;
  }
  CMarsExpression diff = op1 - op2;
  if (diff.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() <= op2.GetConstant());
  }
  if ((diff.IsSignned() != 0) && diff.has_range()) {
    if (diff.IsNonPositive() != 0) {
      return 1;
    }
    return 0;
  }
  if (op1.has_range() && op2.has_range()) {
    CMarsRangeExpr mr1 = op1.get_range();
    CMarsRangeExpr mr2 = op2.get_range();
    return static_cast<int>(mr1.get_const_ub() <= mr2.get_const_lb());
  }
  return 0;
}

int operator<(const CMarsExpression &op1, const CMarsExpression &op2) {
  if (op1.is_INF() && op2.is_INF() && (op1 == op2) != 0) {
    return 0;
  }
  if ((op1.is_fINF() != 0) || (op2.is_fINF() != 0)) {
    //  undetermined
    return 0;
  }
  if ((op1.is_pINF() != 0) || (op2.is_nINF() != 0)) {
    return 0;
  }
  if ((op1.is_nINF() != 0) || (op2.is_pINF() != 0)) {
    return 1;
  }
  CMarsExpression diff = op1 - op2 + 1;
  if (diff.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() < op2.GetConstant());
  }
  if ((diff.IsSignned() != 0) && diff.has_range()) {
    if (diff.IsNonPositive() != 0) {
      return 1;
    }
    return 0;
  }
  if (op1.has_range() && op2.has_range()) {
    CMarsRangeExpr mr1 = op1.get_range();
    CMarsRangeExpr mr2 = op2.get_range();
    return static_cast<int>(mr1.get_const_ub() < mr2.get_const_lb());
  }
  return 0;
}

int operator>=(const CMarsExpression &op1, int64_t op2) {
  if (op1.is_fINF() != 0) {
    //  undetermined
    return 0;
  }
  if (op1.is_pINF() != 0) {
    return 1;
  }
  if (op1.is_nINF() != 0) {
    return 0;
  }
  if (op1.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() >= op2);
  }
  if (op1.has_range()) {
    CMarsRangeExpr mr = op1.get_range();
    return static_cast<int>(mr.get_const_lb() >= op2);
  }
  return 0;
}

int operator>(const CMarsExpression &op1, int64_t op2) {
  if (op1.is_fINF() != 0) {
    //  undetermined
    return 0;
  }
  if (op1.is_pINF() != 0) {
    return 1;
  }
  if (op1.is_nINF() != 0) {
    return 0;
  }
  if (op1.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() > op2);
  }
  if (op1.has_range()) {
    CMarsRangeExpr mr = op1.get_range();
    return static_cast<int>(mr.get_const_lb() > op2);
  }
  return 0;
}

int operator<=(const CMarsExpression &op1, int64_t op2) {
  if (op1.is_fINF() != 0) {
    //  undetermined
    return 0;
  }
  if (op1.is_pINF() != 0) {
    return 0;
  }
  if (op1.is_nINF() != 0) {
    return 1;
  }
  if (op1.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() <= op2);
  }
  if (op1.has_range()) {
    CMarsRangeExpr mr = op1.get_range();
    return static_cast<int>(mr.get_const_ub() <= op2);
  }
  return 0;
}

int operator<(const CMarsExpression &op1, int64_t op2) {
  if ((op1 == op2) != 0) {
    return 0;
  }
  if (op1.is_fINF() != 0) {
    //  undetermined
    return 0;
  }
  if (op1.is_pINF() != 0) {
    return 0;
  }
  if (op1.is_nINF() != 0) {
    return 1;
  }

  if (op1.IsConstant() != 0) {
    return static_cast<int>(op1.GetConstant() < op2);
  }
  if (op1.has_range()) {
    CMarsRangeExpr mr = op1.get_range();
    return static_cast<int>(mr.get_const_ub() < op2);
  }
  return 0;
}

CMarsExpression operator-(const CMarsExpression &op) {
  {
    if (op.is_fINF() != 0) {
      return full_INF;
    }
    if (op.is_nINF() != 0) {
      return pos_INF;
    }
    if (op.is_pINF() != 0) {
      return neg_INF;
    }
  }

  CMarsExpression ret = op;
  CMarsAST_IF *ast = op.m_ast;
  void *sg_node = op.m_expr != nullptr ? ast->GetParent(op.m_expr) : nullptr;
  SgMinusOp *sg_minus = isSgMinusOp(static_cast<SgNode *>(sg_node));
  if (sg_minus != nullptr) {
    ret.m_expr = sg_minus;
  } else {
#if 0
    void *op_exp = op.m_expr;
    if (ast->IsForStatement(op_exp))
      op_exp = ast->GetLoopIterator(op_exp);
    if (!ast->IsInitName(op_exp))
      op_exp = ast->CopyExp(op_exp);

    ret.m_expr = ast->CreateExp(V_SgMinusOp, op_exp);
#else
    ret.m_expr = nullptr;
#endif
  }

  map<void *, CMarsExpression>::iterator it;
  for (it = ret.m_coeff.begin(); it != ret.m_coeff.end(); it++) {
    it->second = -(it->second);
  }
  ret.m_const = -op.m_const;
  ret.m_pos = op.m_pos;
  ret.m_scope = op.m_scope;
  ret.m_int_type = op.m_int_type;
  return ret;
}

//  bool CMarsExpression::hasOnlyIteratorsInScope(void *scope) {
//   vector<void *> v_ref;
//   m_ast->GetNodesByType(scope, "preorder", "SgVarRefExp", &v_ref, false,
//   true);
//
//   for (auto entry : m_coeff) {
//     if (!(m_ast->IsVarRefExp(entry.first) ||
//           m_ast->IsForStatement(entry.first) ||
//           isSgInitializedName(static_cast<SgNode *>(entry.first)) ||)
//         !entry.second.hasOnlyIteratorsInScope(scope) ||
//         (m_ast->IsInScope(entry.first, scope) &&
//          !m_ast->IsForStatement(entry.first) &&
//          m_ast->has_write_in_scope_fast(
//              m_ast->GetVariableInitializedName(entry.first), scope)) ||
//         (std::find(v_ref.begin(), v_ref.end(), entry.first) != v_ref.end()
//         &&
//          !m_ast->IsForStatement(entry.first) &&
//          m_ast->has_write_in_scope_fast(
//              m_ast->GetVariableInitializedName(entry.first),
//              m_ast->TraceUpToFuncDecl(entry.first)))) {
//       return false;
//     }
//   }
//   return true;
//  }

void CMarsExpression::analysis() {
  m_int_type = V_SgTypeVoid;
  if (m_ast->IsExpression(m_expr) != 0) {
    void *base_type = m_ast->GetBaseType(m_ast->GetTypeByExp(m_expr), true);
    if (m_ast->IsIntegerType(base_type) != 0) {
      m_int_type = (static_cast<SgType *>(base_type)->variantT());
    } else if (m_ast->IsXilinxAPIntType(base_type) != 0) {
      m_int_type = m_ast->GetNearestIntegerType(base_type);
    } else if (m_ast->IsEnumType(base_type)) {
      m_int_type = V_SgTypeInt;
    }
  } else {
    void *var = m_expr;
    if (m_ast->IsForStatement(var) != 0) {
      var = m_ast->GetLoopIterator(var);
    }
    if (m_ast->IsInitName(var) != 0) {
      void *base_type = m_ast->GetBaseType(m_ast->GetTypebyVar(var), true);
      if (m_ast->IsIntegerType(base_type) != 0) {
        m_int_type = (static_cast<SgType *>(base_type)->variantT());
      } else if (m_ast->IsXilinxAPIntType(base_type) != 0) {
        m_int_type = m_ast->GetNearestIntegerType(base_type);
      } else if (m_ast->IsEnumType(base_type)) {
        m_int_type = V_SgTypeInt;
      }
    }
  }
  //  m_pos = m_expr;
  //  void * m_expr; //  The corresponding AST node, nullptr if no
  //  correspondence map<void *, CMarsExpression> m_coeff; CMarsAST_IF *
  //  m_ast;
  m_float_pos = m_ast->is_floating_node(m_pos);
  SgExpression *sgExp = isSgExpression(static_cast<SgNode *>(m_expr));
  //  assert(!sgExp || m_ast->IsIntegerTypeRecur(m_ast->GetTypeByExp(sgExp)));
  //  0. VarRefExp
  //  1. UnaryOp (ok)
  //  2. BinaryOp
  //  3. ValueExp (ok)
  //  4. Others (Function Call, etc)
  m_coeff.clear();
  m_coeff[m_expr] = CMarsExpression(m_ast, 1);
  m_const = 0;
  if (m_ast->IsIntegerType(m_int_type) == 0) {
    return;
  }
  int op = sgExp != nullptr ? sgExp->variantT() : 0;
  void *left_exp;
  void *right_exp;
  left_exp = right_exp = nullptr;
  if (sgExp != nullptr) {
    if (isSgBinaryOp(sgExp) != nullptr) {
      m_ast->GetExpOperand(sgExp, &left_exp, &right_exp);
    } else {
      m_ast->IsAPIntOp(sgExp, &left_exp, &right_exp, &op);
    }
  }
  if (isSgVarRefExp(sgExp) != nullptr) {
    m_coeff.clear();
    //  to be induct later
    //  m_coeff[m_ast->GetVariableInitializedName(sgExp)] = CMarsExpression(1,
    //  m_ast);
    m_coeff[sgExp] = CMarsExpression(m_ast, 1);
    m_const = 0;
  } else if (isSgSizeOfOp(sgExp) != nullptr) {
    if (void *val = m_ast->get_sizeof_value(sgExp)) {
      CMarsExpression me_op(val, m_ast, m_pos, m_scope);
      m_coeff = (me_op).m_coeff;
      m_const = (me_op).m_const;
      m_ast->DeleteNode(val);
    }
  } else if (isSgUnaryOp(sgExp) != nullptr) {
    if (isSgMinusOp(sgExp) != nullptr) {
      CMarsExpression me_op(m_ast->GetExpUniOperand(sgExp), m_ast, m_pos,
                            m_scope);
      m_coeff = (-me_op).m_coeff;
      m_const = (-me_op).m_const;
    } else if (isSgUnaryAddOp(sgExp) != nullptr) {
      CMarsExpression me_op(m_ast->GetExpUniOperand(sgExp), m_ast, m_pos,
                            m_scope);
      m_coeff = (me_op).m_coeff;
      m_const = (me_op).m_const;
    } else if (isSgCastExp(sgExp) != nullptr) {
      void *prev_exp = m_ast->GetExpUniOperand(sgExp);
      void *curr_type = m_ast->GetBaseType(m_ast->GetTypeByExp(sgExp), false);
      void *prev_type =
          m_ast->GetBaseType(m_ast->GetTypeByExp(prev_exp), false);
      if (m_ast->IsSafeIntConversion(prev_exp, m_pos, m_scope, prev_type,
                                     curr_type)) {
        CMarsExpression me_op(prev_exp, m_ast, m_pos, m_scope);
        CMarsRangeExpr me_op_range = me_op.get_range();
        if (isSgTypeBool(static_cast<SgNode *>(curr_type)) != nullptr) {
          m_coeff.clear();
          m_const = static_cast<int64_t>(((me_op_range.is_const_lb() != 0) &&
                                          me_op_range.get_const_lb() > 0) ||
                                         ((me_op_range.is_const_ub() != 0) &&
                                          me_op_range.get_const_ub() < 0));
        } else {
          m_coeff = (me_op).m_coeff;
          m_const = (me_op).m_const;
        }
        m_int_type = me_op.m_int_type;
      } else {
        int64_t value;
        if (m_ast->IsConstantInt(sgExp, &value, true, m_scope)) {
          m_coeff.clear();
          m_const = value;
        }
      }
    } else {
      //  do default as a non-linear term
    }
  } else if (m_ast->IsBinaryOp(op) != 0) {
    if (V_SgAddOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      CMarsExpression ret = (me_left + me_right);
      m_coeff = ret.m_coeff;
      m_const = ret.m_const;
    } else if (V_SgSubtractOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      CMarsExpression ret = (me_left - me_right);
      m_coeff = ret.m_coeff;
      m_const = ret.m_const;
    } else if (V_SgMultiplyOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      CMarsExpression ret = (me_left * me_right);
      m_coeff = ret.m_coeff;
      m_const = ret.m_const;
    } else if (V_SgDivideOp == op) {
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_right.IsConstant()) {
        CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
        CMarsExpression ret = (me_left / me_right);
        m_coeff = ret.m_coeff;
        m_const = ret.m_const;
      }
    } else if (V_SgModOp == op) {
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_right.IsConstant()) {
        CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
        CMarsExpression ret = (me_left % me_right);
        m_coeff = ret.m_coeff;
        m_const = ret.m_const;
      }
    } else if (V_SgLshiftOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      CMarsExpression ret = (me_left << me_right);
      m_coeff = ret.m_coeff;
      m_const = ret.m_const;
    } else if (V_SgRshiftOp == op) {
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_right.IsConstant()) {
        CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
        CMarsExpression ret = (me_left >> me_right);
        m_coeff = ret.m_coeff;
        m_const = ret.m_const;
      }
    } else if (V_SgAndOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_left.IsConstant() && me_right.IsConstant()) {
        m_const = me_left.GetConstant() && me_right.GetConstant();
        m_coeff.clear();
      }
    } else if (V_SgOrOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_left.IsConstant() && me_right.IsConstant()) {
        m_const = me_left.GetConstant() || me_right.GetConstant();
        m_coeff.clear();
      }
    } else if (V_SgBitAndOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_left.IsConstant() && me_right.IsConstant()) {
        m_const = me_left.GetConstant() & me_right.GetConstant();
        m_coeff.clear();
      }
    } else if (V_SgBitOrOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_left.IsConstant() && me_right.IsConstant()) {
        m_const = me_left.GetConstant() | me_right.GetConstant();
        m_coeff.clear();
      }
    } else if (V_SgBitXorOp == op) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_left.IsConstant() && me_right.IsConstant()) {
        m_const = me_left.GetConstant() ^ me_right.GetConstant();
        m_coeff.clear();
      }
    } else if (m_ast->IsCompareOp(op)) {
      CMarsExpression me_left(left_exp, m_ast, m_pos, m_scope);
      CMarsExpression me_right(right_exp, m_ast, m_pos, m_scope);
      if (me_left.IsConstant() && me_right.IsConstant()) {
        m_const = evaluate_constant_comparsion(op, me_left.GetConstant(),
                                               me_right.GetConstant());
        m_coeff.clear();
      }
    }
  } else if (isSgValueExp(sgExp) != nullptr) {
    int64_t value = 0;
    int known_type =
        static_cast<int>(m_ast->IsConstantInt(sgExp, &value, true, m_scope));

    if (known_type != 0) {
      m_coeff.clear();
      m_const = value;
    }
  } else if ((isSgAssignInitializer(sgExp) != nullptr) &&
             !has_variable_range(m_ast, sgExp, m_pos, m_scope, true)) {
    CMarsExpression me_op(m_ast->GetInitializerOperand(sgExp), m_ast, m_pos,
                          m_scope);
    m_coeff = me_op.m_coeff;
    m_const = me_op.m_const;
  } else if (isSgFunctionCallExp(sgExp) != nullptr) {
    int op = 0;
    void *var_exp;
    void *dummy_op2;
    var_exp = dummy_op2 = nullptr;
    if (m_ast->IsAPIntOp(sgExp, &var_exp, &dummy_op2, &op)) {
      if (m_ast->IsImplicitConversionOp(op) != 0) {
        //  FIXME: check whether it is safe conversion
        CMarsExpression me_op(var_exp, m_ast, m_pos, m_scope);
        //  check whether there is a function call as atom
        //  because copied function call will cause unparse crash later
        bool contains_call = false;
        for (auto atom : me_op.m_coeff) {
          if (m_ast->IsFunctionCall(atom.first) != 0) {
            contains_call = true;
            break;
          }
        }
        if (!contains_call) {
          m_coeff = me_op.m_coeff;
          m_const = me_op.m_const;
        }
      }
    }
  } else if (isSgConditionalExp(sgExp) != nullptr) {
    void *cond_exp;
    void *true_exp;
    void *false_exp;
    m_ast->IsConditionalExp(sgExp, &cond_exp, &true_exp, &false_exp);
    CMarsExpression me_cond(cond_exp, m_ast, m_pos, m_scope);
    if (me_cond.IsConstant() != 0) {
      if (me_cond.GetConstant() != 0) {
        CMarsExpression me_op(true_exp, m_ast, m_pos, m_scope);
        m_coeff = me_op.m_coeff;
        m_const = me_op.m_const;
      } else {
        CMarsExpression me_op(false_exp, m_ast, m_pos, m_scope);
        m_coeff = me_op.m_coeff;
        m_const = me_op.m_const;
      }
    }
  }

  DEFINE_START_END;
  STEMP(start);
  reduce();  //  remove zero terms
  TIMEP("          reduce", start, end);
  STEMP(start);
  induct();
  TIMEP("          induct", start, end);
  STEMP(start);
  reduce();  //  remove zero terms
  TIMEP("          reduce", start, end);
}

//  remove the zero terms
void CMarsExpression::reduce() {
  map<void *, CMarsExpression> new_coeff;
  new_coeff.clear();
  map<void *, CMarsExpression>::iterator it;
  map<void *, CMarsExpression>::iterator itt;
  bool reduced = false;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    void *var = it->first;
    CMarsExpression coeff = it->second;
    if ((coeff != 0)) {
      bool one_reduced = false;
      for (itt = new_coeff.begin(); itt != new_coeff.end(); ++itt) {
        void *other_var = itt->first;
        if (m_ast->is_identical_expr(var, other_var, m_pos, m_pos)) {
          coeff = itt->second + coeff;
          if ((coeff != 0) != 0) {
            new_coeff[other_var] = coeff;
          } else {
            new_coeff.erase(other_var);
          }
          one_reduced = true;
          break;
        }
      }
      if (!one_reduced) {
        new_coeff[var] = coeff;
      }
      reduced |= one_reduced;
    } else {
      reduced = true;
    }
  }
  m_coeff = new_coeff;
  //  if (reduced)
  //  m_expr = get_expr_from_coeff();
  //  do not do it any more, because it breaks the uniquness
  //  moved to display in unparse()
  //    const int REDUCE_EXPR_AS_WELL = 0;
  //    if (REDUCE_EXPR_AS_WELL)
  //    {
  //        assert(m_ast);
  //        m_expr = nullptr;
  //        if (m_const != 0) m_expr = m_ast->CreateConst(m_const);
  //        for (it = m_coeff.begin(); it != m_coeff.end(); it++)
  //        {
  //            if (it->second != 0) {
  //                void * var = it->first;
  //                if (m_ast->IsInitName(var))
  //                    var = m_ast->CreateVariableRef(var);
  //                else if (m_ast->IsForStatement(var))
  //                    var =
  //                    m_ast->CreateVariableRef(m_ast->GetLoopIterator(var));
  //
  //                void * term;
  //                if (it->second == 1)
  //                {
  //                    term = var;
  //                }
  //                else if (it->second == -1)
  //                {
  //                    term = m_ast->CreateExp(V_SgMinusOp, var);
  //                }
  //                else
  //                {
  //                    term = m_ast->CreateExp(V_SgMultiplyOp,
  //                            var,
  //                            it->second.get_pointer());
  //                }
  //                if (m_expr)
  //                    m_expr = m_ast->CreateExp(V_SgAddOp,
  //                        m_expr,
  //                        term);
  //                else
  //                    m_expr = term;
  //            }
  //        }
  //
  //        if (m_expr==nullptr) m_expr = m_ast->CreateConst(0);
  //    }
}

/*void CMarsExpression::AddValue(CMarsExpression exp) {
  m_const += exp.m_const;

  map<void *, CMarsExpression>::iterator it;
  map<void *, CMarsExpression> map1 = exp.m_coeff;
  for (it = map1.begin(); it != map1.end(); it++) {
    if (m_coeff.find(it->first) == m_coeff.end()) {
      m_coeff[it->first] = it->second;
    } else {
      m_coeff[it->first] = m_coeff[it->first] + it->second;
    }
  }

  reduce();
}*/

int CMarsRangeExpr::is_movable_to(void *pos) {
  size_t i;
  for (i = 0; i < get_lb_set().size(); i++) {
    CMarsExpression me = get_lb(i);
    if (me.is_movable_to(pos) == 0) {
      return 0;
    }
  }
  for (i = 0; i < get_ub_set().size(); i++) {
    CMarsExpression me = get_ub(i);
    if (me.is_movable_to(pos) == 0) {
      return 0;
    }
  }
  return 1;
}

int CMarsRangeExpr::any_var_located_in_scope(void *scope) {
  size_t i;
  for (i = 0; i < get_lb_set().size(); i++) {
    CMarsExpression me = get_lb(i);
    if (me.any_var_located_in_scope(scope) != 0) {
      return 1;
    }
  }
  for (i = 0; i < get_ub_set().size(); i++) {
    CMarsExpression me = get_ub(i);
    if (me.any_var_located_in_scope(scope) != 0) {
      return 1;
    }
  }
  return 0;
}

int CMarsRangeExpr::any_var_located_in_scope_except_loop_iterator(void *scope) {
  size_t i;
  for (i = 0; i < get_lb_set().size(); i++) {
    CMarsExpression me = get_lb(i);
    if (me.any_var_located_in_scope_except_loop_iterator(scope) != 0) {
      return 1;
    }
  }
  for (i = 0; i < get_ub_set().size(); i++) {
    CMarsExpression me = get_ub(i);
    if (me.any_var_located_in_scope_except_loop_iterator(scope) != 0) {
      return 1;
    }
  }
  return 0;
}
int CMarsExpression::substitute(void *sg_var, int64_t value) {
  return substitute(sg_var, CMarsExpression(m_ast, value));
}

int CMarsExpression::substitute(void *sg_var, const CMarsExpression &expr) {
  if (((expr.get_coeff(sg_var) != 0) != 0) ||
      m_coeff.find(sg_var) == m_coeff.end()) {
    return 0;
  }
  //  void * sg_expr = expr.get_pointer();
  //  void * sg_pos  = expr.get_pos();
  CMarsExpression me_new = expr;  //  (sg_expr, m_ast, sg_pos);
  me_new = me_new * m_coeff[sg_var];

  m_coeff.erase(sg_var);
  void *orig_pos = get_pos();
  *this = *this + me_new;
  if ((m_ast != nullptr) && (m_ast->is_floating_node(orig_pos) == 0)) {
    set_pos(orig_pos);
  }
  //  m_expr = get_expr_from_coeff();
  m_expr = nullptr;

#if 0
  {
    static int count = 0;
    cout << "  count = " << count << endl;
    cout << "  var_in " << CMarsVariable(sg_var, m_ast).unparse() << ":"
         << m_ast->GetASTTypeString(sg_var) << endl;
    cout << "  coeff  " << print_coeff(1) << endl;

    count++;
  }
#endif

#if 0  //  _DEBUG_SUBSTITUTE_
  string old_str = print_coeff();
  cout << "Expr Subs " << old_str << " : {"
    << CMarsVariable(sg_var, m_ast).unparse() << " -> " << expr.print_s()
    << "} -> " << print_coeff() << endl;
#endif
  return 1;
}

void CMarsExpression::induct() {
  map<void *, CMarsExpression>::iterator it;

#if _DEBUG_VAR_INDUCT_
  static int count = 0;
#endif
  DEFINE_START_END;

  int redo = 1;
  while (redo != 0) {
    STEMP(start);
    reduce();
    ACCTM(induct___reduce, start, end);
    redo = 0;
    for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
      void *sg_var = it->first;  //  these are expression
      void *sg_name = (m_ast->IsVarRefExp(sg_var)) != 0
                          ? m_ast->GetVariableInitializedName(sg_var)
                          : (m_ast->IsInitName(sg_var) != 0 ? sg_var : nullptr);

      if (sg_name != nullptr) {
        CMarsExpression me_coeff = it->second;
        STEMP(start);
        if (m_float_pos == 0) {
          //  this replacement will not accrss function, because of movability
          void *sg_loop = m_ast->GetLoopFromIteratorByPos(sg_name, m_pos, 0);
          ACCTM(induct___loop, start, end);

#if _DEBUG_VAR_INDUCT_
          cout << "induct loop count = " << count << " "
               << m_ast->GetASTTypeString(sg_var) << " "
               << m_ast->UnparseToString(sg_var) << " in "
               << m_ast->UnparseToString(m_ast->GetParent(sg_var)) << " "
               << sg_loop << endl;
#endif

          if (sg_loop != nullptr) {
            m_coeff[sg_loop] = me_coeff;
            m_coeff[sg_var] = CMarsExpression(m_ast, 0);
            redo = 1;
            break;
          }
        }
        void *sg_exp = nullptr;
        bool is_const_var =
            (m_ast->IsConstType(m_ast->GetTypebyVar(sg_name)) != 0) &&
            (m_ast->IsArgumentInitName(sg_name) == 0);
        //  ZP: FIXED: 20150824

        STEMP(start);
        int64_t sg_int_value = 0;
        if (m_ast->IsConstantInt(sg_name, &sg_int_value, true, m_scope)) {
          sg_exp = m_ast->CreateConst(sg_int_value);
        } else if (is_const_var) {
          sg_exp = m_ast->GetInitializerOperand(sg_name);
        } else if (m_ast->is_floating_node(sg_var) != 0) {
          //  CTempASTStmt tmp(sg_var, m_ast, m_pos);
          //  sg_exp = m_ast->GetSingleAssignExpr(sg_name, sg_var, 1);
          if (m_float_pos == 0) {
            sg_exp = m_ast->GetSingleAssignExpr(sg_name, m_pos, 0, m_scope);
          }
        } else {
          sg_exp = m_ast->GetSingleAssignExpr(sg_name, sg_var, 0, m_scope);
        }
        ACCTM(induct___sg_exp, start, end);

#if _DEBUG_VAR_INDUCT_
        {
          string is_sub = (sg_exp && sg_exp != sg_var) ? "subs" : "nosubs";
          cout << "induct var count = " << count << " "
               << m_ast->GetASTTypeString(it->first) << " "
               << m_ast->UnparseToString(it->first) << " in "
               << m_ast->UnparseToString(m_ast->GetParent(it->first)) << " -> "
               << ((sg_exp) ? m_ast->GetASTTypeString(sg_exp)
                            : string("nullptr"))
               << " "
               << ((sg_exp) ? m_ast->UnparseToString(sg_exp)
                            : string("nullptr"))
               << " " << sg_exp << " " << is_sub << endl;

          count++;
        }
#endif

        STEMP(start);
        if ((sg_exp != nullptr) && sg_exp != sg_var) {
          CMarsExpression val(sg_exp, m_ast, sg_exp, m_scope);
          if (sg_exp == sg_name &&
              (m_ast->is_movable_test(sg_exp, m_pos) != 0)) {
            m_coeff[sg_name] = me_coeff;
            m_coeff[sg_var] = CMarsExpression(m_ast, 0);
          } else if ((val.IsConstant() == 0) &&
                     has_variable_range(m_ast, sg_exp, m_pos, m_scope, true) &&
                     (m_ast->is_movable_test(sg_exp, m_pos) != 0)) {
            m_coeff[sg_exp] = me_coeff;
            m_coeff[sg_var] = CMarsExpression(m_ast, 0);
          } else {
            //  youxiang 20161208 cannot cross function during const
            //  propagation for scalar interface
            void *sg_func = m_ast->TraceUpToFuncDecl(sg_exp);
            if (sg_func != nullptr) {
              if (((val.IsConstant() == 0) ||
                   !m_ast->is_in_same_function(sg_exp, m_pos)) &&
                  (m_ast->is_movable_test(sg_exp, m_pos) == 0)) {
                //  do nothing, the atom is the var_ref
                continue;
              }
              substitute(sg_var, val);

            } else {
              //  youxiang 20180313 relax for constant global variable
              if (val.IsConstant() != 0) {
                substitute(sg_var, val);
              } else {
                continue;
              }
            }
          }
          redo = 1;
          break;
        }
        ACCTM(induct___sg_exp_post, start, end);
      }
    }
  }
}

//  #define _DEBUG_PARSE_VAR_BOUND_SIMPLE_

//  Assume the expr is GreaterOrEqualto ZERO: (x+y+...)>=0
//  1. The bound of var whose coeff is 1 -> lb
//  2. The bound of var whose coeff is -1 -> ub
//  3. otherwise skip term
void CMarsExpression::ParseVarBoundSimple(map<void *, void *> *lb,
                                          map<void *, void *> *ub) {
  map<void *, CMarsExpression>::iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    void *var = it->first;
    if ((it->second == 1) != 0) {
      //  expr>=0  -> var >= var-expr
      CMarsExpression me = CMarsExpression(var, m_ast, m_pos, m_scope) - *this;
      (*lb)[var] = me.get_pointer() != nullptr ? me.get_pointer()
                                               : me.get_expr_from_coeff();

#ifdef _DEBUG_PARSE_VAR_BOUND_SIMPLE_
      cout << CMarsExpression(var, m_ast, m_pos).print() << "+" << this->print()
           << "=" << me.print() << endl;
#endif
    } else if ((it->second == -1) != 0) {
      //  expr>=0  -> var <= var+expr
      CMarsExpression me = CMarsExpression(var, m_ast, m_pos, m_scope) + *this;
      (*ub)[var] = me.get_pointer() != nullptr ? me.get_pointer()
                                               : me.get_expr_from_coeff();

#ifdef _DEBUG_PARSE_VAR_BOUND_SIMPLE_
      cout << CMarsExpression(var, m_ast, m_pos).print() << "+" << this->print()
           << "=" << me.print() << endl;
#endif
    }
  }

#ifdef _DEBUG_PARSE_VAR_BOUND_SIMPLE_
  cout << "[CMarsExpression::ParseVarBoundSimple] " << print() << endl;
  map<void *, void *>::iterator itt;
  for (itt = lb->begin(); itt != lb->end(); itt++) {
    cout << "  lb[" + m_ast->UnparseToString(itt->first) + "] "
         << m_ast->GetASTTypeString(itt->second) << " "
         << m_ast->UnparseToString(itt->second) << endl;
  }
  for (itt = ub->begin(); itt != ub->end(); itt++) {
    cout << "  ub[" + m_ast->UnparseToString(itt->first) + "] "
         << m_ast->GetASTTypeString(itt->second) << " "
         << m_ast->UnparseToString(itt->second) << endl;
  }
#endif
}

//  1. All the coefficients are constants
//  2. All the variables have atom ranges (loop iterator, assert, etc.), and
//  constant bounds
int CMarsExpression::has_coeff_range_simple() const {
  map<void *, CMarsExpression>::const_iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    void *var = it->first;
    CMarsExpression coeff = it->second;
    if (coeff.IsConstant() == 0) {
      return 0;
    }

    if (m_ast->IsForStatement(it->first) != 0) {
      void *loop_step = m_ast->GetLoopIncrementExpr(var);
      int64_t value = 0;
      int ret = m_ast->GetLoopStepValueFromExpr(loop_step, &value);
      if (ret == 0) {
        return 0;
      }
    }

    CMarsRangeExpr mr_var(
        CMarsVariable(var, m_ast, m_pos, m_scope).get_range());
    mr_var.refine_range_in_scope(nullptr, m_pos);
    if (mr_var.is_const_bound() == 0) {
      return 0;
    }
  }
  return 1;
}

static bool simple_triple_compare(const vector<int64_t> &one,
                                  const vector<int64_t> &other) {
  return std::abs(one[0]) < std::abs(other[0]);
}

//  vector<step, lb, ub> of all the terms
//  if assert false here please check by call has_coeff_range_simple
vector<vector<int64_t>> CMarsExpression::get_coeff_range_simple() {
  vector<vector<int64_t>> vec_triple;

  vector<int64_t> term;
  term.push_back(1);
  term.push_back(m_const);
  term.push_back(m_const);
  vec_triple.push_back(term);

  map<void *, CMarsExpression>::const_iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    void *var = it->first;
    CMarsExpression coeff = it->second;
    if (coeff.IsConstant() == 0) {
      assert(0);
    }
    int64_t step = coeff.GetConstant();
    //  int64_t n_coeff = step;

    //  /////////////////////////////////////////  /
    //  ZP: 20151118 removed, because this step is later used to divide the
    //  coefficient
    //     so in this part there is no difference between incr 1 and incr n
    //  if (m_ast->IsForStatement(it->first))
    //  {
    //    void * loop_step = m_ast->GetLoopIncrementExpr(var);
    //    int64_t value =0;
    //    int64_t ret = m_ast->GetLoopStepValueFromExpr(loop_step, &value) ;
    //    if (!ret) assert(0);
    //    step *= std::abs(value);
    //  }
    //  //////////////////////////////////////////  /

    CMarsRangeExpr mr_var(
        CMarsVariable(var, m_ast, m_pos, m_scope).get_range());
    mr_var.refine_range_in_scope(nullptr, m_pos);
    if (mr_var.is_const_bound() == 0) {
      assert(0);
    }
    int64_t n_lb = mr_var.get_const_lb();
    int64_t n_ub = mr_var.get_const_ub();

    //  if (n_coeff> 0)
    //  {
    //    n_lb = n_coeff * n_lb;
    //    n_ub = n_coeff * n_ub;
    //  }
    //  else
    //  {
    //    n_lb = n_coeff * n_ub;
    //    n_ub = n_coeff * n_lb;
    //  }

    term.clear();
    term.push_back(step);
    term.push_back(n_lb);
    term.push_back(n_ub);
    vec_triple.push_back(term);
  }

  std::sort(vec_triple.begin(), vec_triple.end(), simple_triple_compare);

  return vec_triple;
}

#if 0
bool CMarsVariable::operator<(const CMarsVariable &var) const {
  if ((int64_t)this->m_var < (int64_t)var.m_var) {
    return true;
  } else if ((int64_t)this->m_var > (int64_t)var.m_var) {
    return false;
  }

  if ((int64_t)this->m_ast < (int64_t)var.m_ast) {
    return true;
  } else if ((int64_t)this->m_ast > (int64_t)var.m_ast) {
    return false;
  }

  if ((int64_t)this->m_pos < (int64_t)var.m_pos) {
    return true;
  } else if ((int64_t)this->m_pos > (int64_t)var.m_pos) {
    return false;
  }

  if ((int64_t)this->m_scope < (int64_t)var.m_scope) {
    return true;
  } else if ((int64_t)this->m_scope > (int64_t)var.m_scope) {
    return false;
  }

  return false;
}
#endif

//  General function: get expression range from variable range
CMarsRangeExpr CMarsExpression::get_range() const {
#if 0
  if (!has_range()) {
    return CMarsRangeExpr(*this, *this, true);
  }
#endif
  if (_DEBUG_GET_RANGE_) {
    cout << "  -- get_range() exp start --" << unparse() << " "
         << print_coeff(1) << endl;
  }

#if 0  //  not cache the result of get_range()
  static map<DUMMY, CMarsRangeExpr> exprRangesCache;
  DUMMY dummy(this);

  if (exprRangesCache.find(dummy) != exprRangesCache.end()) {
    return exprRangesCache[dummy];
  } else {
    exprRangesCache.insert(
        pair<DUMMY, CMarsRangeExpr>(dummy, CMarsRangeExpr(m_ast)));
  }

  auto &mr = exprRangesCache[dummy];
#else
  CMarsRangeExpr mr(m_ast);
#endif

  DEFINE_START_END;
  STEMP(start);

  if (is_INF() != 0) {
    mr = CMarsRangeExpr(*this, *this);
    ACCTM(get_range___is_INF, start, end);
  } else if (IsConstant() != 0) {
    mr = CMarsRangeExpr(GetConstant(), m_ast);
    ACCTM(get_range___isConstant, start, end);
  } else if (IsNormal() != 0) {
    //  mr += var_range * coeff
    //  mr_lb += var.lb * coeff
    CMarsExpression mr_lb(get_ast(), m_const);
    CMarsExpression mr_ub(get_ast(), m_const);
    ACCTM(get_range___isNormal___construct_mr_lb_ub, start, end);

    bool is_exact = true;
    map<void *, CMarsExpression>::const_iterator it;
    for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
#if 1  //  cache ranges for variables
      CMarsRangeExpr varRangeExpr;
      CMarsVariable cMarsVar(it->first, m_ast, m_pos, m_scope);
      DUMMY dummy(&cMarsVar);
      if (cache_get_range_var.find(dummy) != cache_get_range_var.end()) {
        varRangeExpr = cache_get_range_var[dummy];
      } else {
        STEMP(start);
        varRangeExpr = cMarsVar.get_range();
        ACCTM(get_range___isNormal___mr_var, start, end);
        cache_get_range_var.insert(
            pair<DUMMY, CMarsRangeExpr>(dummy, varRangeExpr));
      }
#else
      CMarsVariable cMarsVar(it->first, m_ast, m_pos, m_scope);
      CMarsRangeExpr varRangeExpr = cMarsVar.get_range();
#endif
      CMarsRangeExpr mr_var(varRangeExpr);
      //  CMarsRangeExpr mr_var1 (CMarsExpression(it->first, m_ast, m_pos),
      //      CMarsExpression(it->first, m_ast, m_pos)
      //        ) ;

      assert(it->second.IsConstant());
      int64_t n_coeff = it->second.GetConstant();
      auto me_coeff = it->second;
      STEMP(start);
      CMarsExpression curr_lb =
          mr_var.get_lb_set().size() == 1 ? mr_var.get_lb_set()[0] : neg_INF;
      CMarsExpression curr_ub =
          mr_var.get_ub_set().size() == 1 ? mr_var.get_ub_set()[0] : pos_INF;
      ACCTM(get_range___isNormal___curr_lb_ub, start, end);
      is_exact = mr_var.is_exact();
      if (((curr_lb == neg_INF) != 0) || ((curr_ub == pos_INF) != 0) ||
          mr_var.get_lb_set().size() > 1 || mr_var.get_ub_set().size() > 1) {
        is_exact = false;
      }

      int64_t op2 = n_coeff;
      STEMP(start);
      if (0 == op2) {
        //  do nothing
      } else if (op2 > 0) {
        mr_lb = mr_lb + curr_lb * me_coeff;
        mr_ub = mr_ub + curr_ub * me_coeff;
        if (op2 > 1 && ((curr_lb == curr_ub) == 0)) {
          is_exact = false;
        }
      } else if (op2 < 0) {
        mr_lb = mr_lb + curr_ub * me_coeff;
        mr_ub = mr_ub + curr_lb * me_coeff;
        if (op2 < -1 && ((curr_lb == curr_ub) == 0)) {
          is_exact = false;
        }
      }
      ACCTM(get_range___isNormal___mr_lb_ub, start, end);
    }
    //  YXC: 2019-05-23 handle possible overflow
    if ((mr_lb > mr_ub) != 0) {
      std::swap(mr_lb, mr_ub);
    }
    STEMP(start);
    mr = CMarsRangeExpr(mr_lb, mr_ub, is_exact);
    if (mr.get_ast() == nullptr) {
      mr.set_ast(get_ast());
    }
    if (IsAtomicWithNonDeterminedRange() != 0) {
      if (has_variable_range(m_ast, *this)) {
        CMarsRangeExpr ret = get_variable_range(m_ast, *this);
        return ret;
      }
    }
    ACCTM(get_range___isNormal___mr, start, end);
  } else {
    mr = CMarsRangeExpr(*this, *this);
#if KEEP_UNREACHABLE
    assert(0);
    bool is_exact = false;
    mr = CMarsRangeExpr(m_const, m_ast, is_exact);

    //  Basic Algorithm (Peng Zhang, 20150725):
    //  [a..b] + [c..d] = [(a+c)..(b+d)]
    //  -[a..b] = [-b..-a]
    //  [a..b] * c = [min(a*c, b*c)..max(a*c, b*c)]
    //  [a..b] * [c..d] = [min(a*[c..d], b*[c..d])..max(...)]
    //  mr = sum(mr_var[i]*mr_coeff[i] + m_const);

    map<void *, CMarsExpression>::const_iterator it;
    for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
      //  make into atomic:
      CMarsRangeExpr mr_var = CMarsVariable(it->first, m_ast).get_range();
      CMarsRangeExpr mr_coeff = it->second.get_range();

      mr = mr + mr_var * mr_coeff;
    }
    ACCTM(get_range___else, start, end);
#endif
  }

  if (_DEBUG_GET_RANGE_) {
    cout << "  -- get_range() exp end   --"
         << m_ast->GetASTTypeString(get_expr_from_coeff()) << " "
         << print_coeff(1) << " : " << mr.print_e() << endl;
  }

  return mr;
}

//  General function: get expression range from variable range
bool CMarsExpression::has_range() const {
  if (is_INF() != 0) {
    return false;
  }
  if (IsConstant() != 0) {
    return true;
  }
  if ((IsNormal() != 0) && (IsAtomicWithNonDeterminedRange() == 0)) {
    return true;
  }
  return has_variable_range(m_ast, *this);
}

int CMarsExpression::IsNonNegative() const {
  if (is_INF() != 0) {
    if (is_pINF() != 0) {
      return 1;
    }
    { return 0; }
  }
  if (IsConstant() != 0) {
    return static_cast<int>(GetConstant() >= 0);
  }

  if (m_ast->IsUnsignedType(m_int_type) != 0) {
    return 1;
  }
#define CACHE 1
#if CACHE
  DUMMY dummy(this);

  if (cache_IsNonNegative.find(dummy) != cache_IsNonNegative.end()) {
    return cache_IsNonNegative[dummy];
  }
#endif

  DEFINE_START_END;

  STEMP(start);
  CMarsRangeExpr mr = get_range();
  ACCTM(get_range, start, end);

  STEMP(start);
  int ret = mr.IsNonNegative();
  ACCTM(IsNonNegative, start, end);

#if CACHE
  cache_IsNonNegative.insert(pair<DUMMY, int>(dummy, ret));
#endif
#undef CACHE

  return ret;
}

int CMarsExpression::IsNonPositive() const {
  return (-(*this)).IsNonNegative();
#if 0
  if (is_INF()) {
    if (is_fINF() || is_nINF())
      return 1;
    else
      return 0;
  }
  if (IsConstant()) {
    return GetConstant() <= 0;
  }

#define CACHE 1
#if CACHE
  DUMMY dummy(this);

  if (cache_IsNonPositive.find(dummy) != cache_IsNonPositive.end()) {
    return cache_IsNonPositive[dummy];
  }
#endif

  DEFINE_START_END;

  STEMP(start);
  CMarsRangeExpr mr = get_range();
  ACCTM(get_range, start, end);

  STEMP(start);
  int ret = mr.IsNonPositive();
  ACCTM(IsNonPositive, start, end);

#if CACHE
  cache_IsNonPositive.insert(pair<DUMMY, int>(dummy, ret));
#endif
#undef CACHE

  return ret;
#endif
}

bool CMarsExpression::IsLoopInvariant(void *for_loop) const {
  if (IsConstant() != 0) {
    return true;
  }
  for (map<void *, CMarsExpression>::const_iterator it = m_coeff.begin();
       it != m_coeff.end(); ++it) {
    if (!it->second.IsLoopInvariant(for_loop)) {
      return false;
    }
    void *var = it->first;
    if (m_ast->IsForStatement(var) != 0) {
      if (var == for_loop || (m_ast->IsInScope(var, for_loop) != 0)) {
        return false;
      }
      continue;
    }
    if (!m_ast->IsLoopInvariant(for_loop, var, m_pos)) {
      return false;
    }
  }
  return true;
}

bool CMarsExpression::IsStandardForm(void *for_loop,
                                     map<void *, int64_t> *iterators,
                                     map<void *, int64_t> *invariants) const {
  if (IsNormal() == 0) {
    return false;
  }
  map<void *, CMarsExpression>::const_iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    void *var = it->first;
    int64_t coeff = it->second.GetConstant();
    if (m_ast->IsForStatement(var) != 0) {
      if (var == for_loop || (m_ast->IsInScope(var, for_loop) != 0)) {
        (*iterators)[var] = coeff;
      } else {
        (*invariants)[var] = coeff;
      }
      continue;
    }
    if (m_ast->IsLoopInvariant(for_loop, var, m_pos)) {
      (*invariants)[var] = coeff;
    } else {
      return false;
    }
  }
  return true;
}

/**********************************************************************/
//  class CMarsRangeExpr
/**********************************************************************/
int CMarsRangeExpr::IsNonNegative() const {
  return static_cast<int>(get_const_lb() >= 0);
}
//  int CMarsRangeExpr::IsNonPositive() const { return get_const_ub() <= 0; }

CMarsRangeExpr::CMarsRangeExpr(int64_t lb, int64_t ub, CMarsAST_IF *ast,
                               bool is_exact /*=0*/) {
  m_is_exact = is_exact;
  m_ast = ast;
  assert(m_ast);
  if (lb > ub) {
    set_empty();
  } else {
    set_lb(CMarsExpression(ast, lb));
    set_ub(CMarsExpression(ast, ub));
    reduce();
  }
}
CMarsRangeExpr::CMarsRangeExpr(int64_t ublb, CMarsAST_IF *ast,
                               bool is_exact /*=1*/) {
  m_is_exact = is_exact;
  m_ast = ast;
  assert(m_ast);
  set_lb(CMarsExpression(ast, ublb));
  set_ub(CMarsExpression(ast, ublb));
  reduce();
}

CMarsRangeExpr::CMarsRangeExpr(CMarsAST_IF *ast, void *type) {
  m_is_exact = false;
  m_ast = ast;
  if ((ast == nullptr) || (type == nullptr)) {
    return;
  }
  void *base_type = ast->GetBaseType(type, true);
  if (ast->IsIntegerType(base_type) == 0) {
    return;
  }
  SgType *sg_type = isSgType(static_cast<SgNode *>(base_type));
#if 0
  if (isSgTypeChar((sg_type)) || isSgTypeSignedChar(sg_type)) {
    set_lb(CMarsExpression((int64_t)numeric_limits<char>::min(), ast));
    set_ub(CMarsExpression((int64_t)numeric_limits<char>::max(), ast));
  } else if (isSgTypeUnsignedChar((sg_type))) {
    set_lb(CMarsExpression((int64_t) numeric_limits<unsigned char>::min(),
          ast));
    set_ub(CMarsExpression((int64_t) numeric_limits<unsigned char>::max(),
          ast));
  } else if (isSgTypeShort((sg_type)) || isSgTypeSignedShort(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<int16_t>::min(), ast));
    set_ub(CMarsExpression((int64_t) numeric_limits<int16_t>::max(), ast));
  } else if (isSgTypeUnsignedShort(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<uint16_t>::min(), ast));
    set_ub(CMarsExpression((int64_t) numeric_limits<uint16_t>::max(), ast));
  } else if (isSgTypeInt((sg_type)) || isSgTypeSignedInt(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<int>::min(), ast));
    set_ub(CMarsExpression((int64_t) numeric_limits<int>::max(), ast));
  } else if (isSgTypeUnsignedInt(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<unsigned int>::min(), ast));
    set_ub(CMarsExpression((int64_t) numeric_limits<unsigned int>::max(), ast));
  } else if (isSgTypeLong((sg_type)) || isSgTypeSignedLong(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<int64_t>::min(), ast));
    set_ub(CMarsExpression((int64_t) numeric_limits<int64_t>::max(), ast));
  } else if (isSgTypeUnsignedLong(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<unsigned int64_t>::min(),
          ast));
    //  FIXME:
    set_ub(CMarsExpression((int64_t) numeric_limits<int64_t>::max(), ast));
  } else if (isSgTypeLongLong((sg_type)) || isSgTypeLongLong(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<int64_t>::min(), ast));
    set_ub(CMarsExpression((int64_t) numeric_limits<int64_t>::max(), ast));
  } else if (isSgTypeUnsignedLongLong(sg_type)) {
    set_lb(CMarsExpression((int64_t) numeric_limits<uint64_t>::min(), ast));
    //  FIXME:
    set_ub(CMarsExpression((int64_t) numeric_limits<int64_t>::max(), ast));
  }
#else
  if ((isSgTypeUnsignedChar(sg_type) != nullptr) ||
      (isSgTypeUnsignedLongLong(sg_type) != nullptr) ||
      (isSgTypeUnsignedLong(sg_type) != nullptr) ||
      (isSgTypeUnsignedInt(sg_type) != nullptr) ||
      (isSgTypeUnsignedShort(sg_type) != nullptr) ||
      (isSgTypeUnsignedChar(sg_type) != nullptr)) {
    set_lb(CMarsExpression(
        ast, static_cast<int64_t>(numeric_limits<uint64_t>::min())));
  }
#endif
}
//  Simple means: no more than 1 LB or UB
int CMarsRangeExpr::get_simple_bound(CMarsExpression *lb,
                                     CMarsExpression *ub) const {
  if (is_empty() != 0) {
    return 0;
  }
#if 0
    if (get_lb_set().size() != 1) return 0;
    if (get_ub_set().size() != 1) return 0;

    *lb = m_lb[0];
    *ub = m_ub[0];
#else
  //  youxiang 20160930 support pInf & nInf
  if (get_lb_set().size() > 1) {
    return 0;
  }
  if (get_ub_set().size() > 1) {
    return 0;
  }
  *lb = get_lb_set().size() == 1 ? m_lb[0] : neg_INF;
  *ub = get_ub_set().size() == 1 ? m_ub[0] : pos_INF;
#endif
  return 1;
}

//  Basic Algorithm (Peng Zhang, 20150725):
//  [a..b] + [c..d] = [(a+c)..(b+d)]
//  -[a..b] = [-b..-a]
//  [a..b] * c = [min(a*c, b*c)..max(a*c, b*c)]
//  [a..b] * [c..d] = [min(a*[c..d], b*[c..d])..max(...)]
//
//  Notes:
//  1. Definition: na = infinite
//  2. CMarsRangeExpr() -> na..na
//  3. CMarsRangeExpr(0) -> 0..0
//  4. [na..na] */+/- any = [na..na]
CMarsRangeExpr operator-(const CMarsRangeExpr &op) {
  CMarsRangeExpr mr;
  size_t i;

  for (i = 0; i < op.m_lb.size(); i++) {
    mr.append_ub(-op.m_lb[i]);
  }
  for (i = 0; i < op.m_ub.size(); i++) {
    mr.append_lb(-op.m_ub[i]);
  }
  mr.reduce();
  mr.set_exact(op.is_exact());
  return mr;
}

CMarsRangeExpr operator+(const CMarsRangeExpr &op1,
                         const CMarsExpression &op2) {
  if (op1.is_empty() != 0) {
    return op1;
  }

  CMarsRangeExpr mr;
  size_t i;
  for (i = 0; i < op1.m_lb.size(); i++) {
    mr.append_lb((op1.m_lb)[i] + op2);
  }
  for (i = 0; i < op1.m_ub.size(); i++) {
    mr.append_ub((op1.m_ub)[i] + op2);
  }
  mr.reduce();
  bool is_exact = op1.is_exact();
  mr.set_exact(is_exact);
  return mr;
}

// return true only if
// 1. op1 and op2 are the same exact
// 2. both op1 and op2 are simple bound
// 3. both lb and ub of op1 and op2 are equal
bool operator==(const CMarsRangeExpr &op1, const CMarsRangeExpr &op2) {
  if (op1.is_exact() != op2.is_exact()) {
    return false;
  }
  if (op1.is_empty() && op2.is_empty()) {
    return true;
  }
  CMarsExpression lb1, lb2, ub1, ub2;
  if (op1.get_simple_bound(&lb1, &ub1) == 0 ||
      op2.get_simple_bound(&lb2, &ub2) == 0)
    return false;
  return lb1 == lb2 && ub1 == ub2;
}

//  [a..b] + [c..d] = [(a+c)..(b+d)]
CMarsRangeExpr operator+(const CMarsRangeExpr &op1, const CMarsRangeExpr &op2) {
  if (op1.is_empty() != 0) {
    return op1;
  }
  if (op2.is_empty() != 0) {
    return op2;
  }

  CMarsAST_IF *ast = op1.get_ast() != nullptr ? op1.get_ast() : op2.get_ast();

  CMarsRangeExpr mr(ast);
  size_t i;
  size_t j;

  for (i = 0; i < op1.m_lb.size(); i++) {
    for (j = 0; j < op2.m_lb.size(); j++) {
      mr.append_lb((op1.m_lb)[i] + (op2.m_lb)[j]);
    }
  }
  for (i = 0; i < op1.m_ub.size(); i++) {
    for (j = 0; j < op2.m_ub.size(); j++) {
      mr.append_ub(op1.m_ub[i] + op2.m_ub[j]);
    }
  }

  bool is_exact = static_cast<int>(
      (static_cast<int>(op1.is_exact() != 0) != 0) && (op2.is_exact()) != 0);
  mr.set_exact(is_exact);
  mr.reduce();
  return mr;
}
CMarsRangeExpr operator-(const CMarsRangeExpr &op1, const CMarsRangeExpr &op2) {
  return op1 + (-op2);
}

#if KEEP_UNUSED
//  Note: min for ub (umin) is represened as a set of ub_set, but min for lb
//  (lmin) is not stored. So if relative magnitute is not determined,
//  the abs const bound will be used
CMarsExpression lmin(const CMarsExpression &op1, const CMarsExpression &op2) {
  assert(op1.get_ast() == op2.get_ast());
  CMarsAST_IF *ast = (op1.get_ast()) ? op1.get_ast() : op2.get_ast();
  if (!op1.get_range().has_lb())
    return neg_INF;  //  CMarsExpression(CMarsRangeExpr::MARS_INT64_MIN, ast);
  if (!op2.get_range().has_lb())
    return neg_INF;  //  CMarsExpression(CMarsRangeExpr::MARS_INT64_MIN, ast);

  //  CMarsExpression diff = op1 - op2;
  if (op1 >= op2) {
    return op2;
  } else if (op2 >= op1) {
    return op1;
  } else {  //  can not tell which one is smaller
#if PROJDEBUG
    cout << "[lmin] Magnitude not determined: " << op1.print() << " "
         << op2.print() << endl;
#endif
    return CMarsExpression(std::min(op1.get_range().get_const_lb(),
                                    op2.get_range().get_const_lb()),
                           ast);
  }
}

CMarsExpression umax(const CMarsExpression &op1, const CMarsExpression &op2) {
  assert(op1.get_ast() == op2.get_ast());
  CMarsAST_IF *ast = (op1.get_ast()) ? op1.get_ast() : op2.get_ast();
  if (!op1.get_range().has_ub())
    return pos_INF;  //  CMarsExpression(CMarsRangeExpr::MARS_INT64_MAX, ast);
  if (!op2.get_range().has_ub())
    return pos_INF;  //  CMarsExpression(CMarsRangeExpr::MARS_INT64_MAX, ast);

  //  CMarsExpression diff = op1 - op2;
  if (op1 >= op2) {
    return op1;
  } else if (op2 >= op1) {
    return op2;
  } else {  //  can not tell which one smaller
#if PROJDEBUG
    cout << "[umax] Magnitude not determined: " << op1.print() << " "
         << op2.print() << endl;
#endif
    return CMarsExpression(std::max(op1.get_range().get_const_ub(),
                                    op2.get_range().get_const_ub()),
                           ast);
  }
}
#endif

#define _OLD_MIN_MAX_
#ifdef _OLD_MIN_MAX_
#if KEEP_UNREACHABLE
CMarsRangeExpr min(const CMarsExpression &op1, const CMarsExpression &op2) {
  CMarsRangeExpr mr;

  //  CMarsExpression diff = op1 - op2;
  if (op1 >= op2) {
    mr.set_lb(op2);
  } else if (op2 >= op1) {
    mr.set_lb(op1);
  } else {  //  can not tell which one smaller
    if (op1.IsNonNegative() && op2.IsNonNegative()) {
      CMarsExpression me_lb = op1;
      mr.set_lb(CMarsExpression(0, me_lb.get_ast()));
    }
  }

  return mr;
}
CMarsRangeExpr max(const CMarsExpression &op1, const CMarsExpression &op2) {
  CMarsRangeExpr mr;
  //  CMarsExpression diff = op1 - op2;
  if (op1 >= op2 {
    mr.set_ub(op1);
  } else if (op2 >= op1) {
    mr.set_ub(op2);
  } else {  //  can not tell which one smaller
    if (op1.IsNonPositive() && op2.IsNonPositive()) {
      CMarsExpression me_ub = op1;
      mr.set_ub(CMarsExpression(0, me_ub.get_ast()));
    }
  }

  return mr;
}
#endif

#if KEEP_UNUSED
CMarsRangeExpr min(const CMarsRangeExpr &op1, const CMarsRangeExpr &op2) {
  CMarsRangeExpr mr;
  if (op1.has_lb() && op2.has_lb()) {
    mr = min(op1.get_lb(), op2.get_lb());
  }
  return mr;
}
CMarsRangeExpr max(const CMarsRangeExpr &op1, const CMarsRangeExpr &op2) {
  CMarsRangeExpr mr;
  if (op1.has_ub() && op2.has_ub()) {
    mr = max(op1.get_ub(), op2.get_ub());
  }
  return mr;
}
#endif

int check_exact_range_mult(const CMarsRangeExpr &r, int64_t n) {
  return static_cast<int>(
      (n == 1 || n == -1 || n == 0 || r.is_single_access()) &&
      (r.is_exact()) != 0);
}

CMarsRangeExpr operator*(const CMarsRangeExpr &op1, int64_t op2) {
  CMarsRangeExpr mr;
  size_t i;
  if (op1.is_empty() != 0) {
    return op1;
  }
  if (0 == op2) {
    mr = CMarsRangeExpr(0, op1.get_ast());
  } else if (op2 > 0) {
    for (i = 0; i < op1.m_lb.size(); i++) {
      mr.append_lb(op1.m_lb[i] * op2);
    }
    for (i = 0; i < op1.m_ub.size(); i++) {
      mr.append_ub(op1.m_ub[i] * op2);
    }
  } else if (op2 < 0) {
    for (i = 0; i < op1.m_lb.size(); i++) {
      mr.append_ub(op1.m_lb[i] * op2);
    }
    for (i = 0; i < op1.m_ub.size(); i++) {
      mr.append_lb(op1.m_ub[i] * op2);
    }
  }
  bool is_exact = check_exact_range_mult(op1, op2);
  mr.set_exact(is_exact);
  return mr;
}

#if KEEP_UNREACHABLE
CMarsRangeExpr operator*(const CMarsRangeExpr &op1,
                         const CMarsExpression &op2) {
  CMarsRangeExpr mr;

  if (op1.has_lb() && op1.has_ub()) {
    CMarsRangeExpr lb = min(op1.get_lb() * op2, op1.get_ub() * op2);
    CMarsRangeExpr ub = max(op1.get_lb() * op2, op1.get_ub() * op2);
    if (lb.has_lb())
      mr.set_lb(lb.get_lb());
    if (ub.has_ub())
      mr.set_ub(ub.get_ub());
  } else if (op1.has_lb()) {  //  op1.get_ub = +infinite
    if (op2.IsNonNegative()) {
      CMarsExpression lb = op1.get_lb() * op2;
      mr.set_lb(lb);
    } else if ((-op2).IsNonNegative()) {
      CMarsExpression ub = op1.get_lb() * op2;
      mr.set_ub(ub);
    }
  } else if (op1.has_ub()) {  //  op1.get_lb = -infinite
    if (op2.IsNonNegative()) {
      CMarsExpression ub = op1.get_ub() * op2;
      mr.set_ub(ub);
    } else if ((-op2).IsNonNegative()) {
      CMarsExpression lb = op1.get_ub() * op2;
      mr.set_lb(lb);
    }
  }

  bool is_exact = true;
  if (!op2.IsConstant()) {
    is_exact = false;
  } else {
    int64_t nop2 = op2.GetConstant();
    if (!check_exact_range_mult(op1, nop2))
      is_exact = false;
  }
  mr.set_exact(is_exact);
  return mr;
}
#endif
#endif

#if 0
vector<CMarsExpression> operator*(const vector<CMarsExpression> &op1,
                                  const vector<CMarsExpression> &op2) {
  vector<CMarsExpression> ret;
  for (size_t i = 0; i < op1.size(); i++) {
    for (size_t j = 0; j < op2.size(); j++) {
      ret.push_back(op1[i] * op2[j]);
    }
  }
  return ret;
}

vector<CMarsExpression> operator+(const vector<CMarsExpression> &op1,
                                  const vector<CMarsExpression> &op2) {
  vector<CMarsExpression> ret;

  ret.insert(ret.end(), op1.begin(), op1.end());
  ret.insert(ret.end(), op2.begin(), op2.end());

  return ret;
}
#endif

#if KEEP_UNREACHABLE
//  [a..b] * [c..d] = [min(a*[c..d], b*[c..d])..max(...)]
CMarsRangeExpr operator*(const CMarsRangeExpr &op1, const CMarsRangeExpr &op2) {
  CMarsRangeExpr mr;

  //  [a0,a1..b0,b1] * [c0,c1..d0,d1]
  //  the real bounds comes at orignal bound
  //  mr = [min(ab[i] * cd[j]), max(ab[i] * cd[j])]

#if 0
    vector<CMarsExpression> lb_cand;
    vector<CMarsExpression> ub_cand;

    vector<CMarsExpression> & op1_lb = op1.get_lb_set();
    vector<CMarsExpression> & op1_ub = op1.get_ub_set();
    vector<CMarsExpression> & op2_lb = op2.get_lb_set();
    vector<CMarsExpression> & op2_ub = op2.get_ub_set();

    if (op2.has_lb() && op2.has_ub()) {
        vector<CMarsExpression> bound1_cand;
        vector<CMarsExpression> bound2_cand;
        //  bound1 = (lb1, ub1);
        //  //  if op2.NonNeg, no ub1; if op2.NonPos, no lb1
        //  bound2 = (lb2, ub2);
        if (!op2.IsNonPositive())
            bound1_cand.insert(bound1_cand.end(), op1.lb.begin(), op1.lb.end());
        if (!op2.IsNonNegative())
            bound1_cand.insert(bound1_cand.end(), op1.ub.begin(), op1.ub.end());
        if (!op1.IsNonPositive())
            bound2_cand.insert(bound2_cand.end(), op2.lb.begin(), op2.lb.end());
        if (!op1.IsNonNegative())
            bound2_cand.insert(bound2_cand.end(), op2.ub.begin(), op2.ub.end());
    } else if (op2.has_lb()) {  //  op2.get_ub = +infinite
        if (op1.has_lb() && op1.get_lb().IsNonNegative()) {
            //  if op1 >= 0
            CMarsRangeExpr lb = op1 * op2.get_lb();
            if (lb.has_lb()) mr.set_lb(lb.get_lb());
        } else if (op1.has_ub() && (-op1.get_ub()).IsNonNegative()) {
            //  if op1 <= 0
            CMarsRangeExpr ub = op1 * op2.get_lb();
            if (ub.has_ub()) mr.set_ub(ub.get_ub());
        } else {
            //  mr = [-na..na]
        }
    } else if (op2.has_ub()) {
        if (op1.has_lb() && op1.get_lb().IsNonNegative()) {
            CMarsRangeExpr ub = (op1 * op2.get_ub());
            if (ub.has_ub()) mr.set_ub(ub.get_ub());
        } else if (op1.has_ub() && (-op1.get_ub()).IsNonNegative()) {
            CMarsRangeExpr lb = (op1 * op2.get_ub());
            if (lb.has_lb()) mr.set_lb(lb.get_lb());
        } else {
            //  mr = [-na..na]
        }
    } else {
        //  mr = [-na..na]
    }
#endif

  //  ////////////////////////  /
  //  [{lb1}..{ub1}] * [{lb2}..{ub2}]
  //  step1 => {[lb1..ub1]*[lb2..ub2]}   //  it is conservative, because
  //  conditions
  //  in [{lb1..} -> [lb1.. is conditional, but
  //  the condition is ignored in the
  //  multiply
  //  step2 => {lb1*lb2, lb1*ub2, ub1*lb2, ub1*ub2}
  //  it is also conservative, because
  //  the relative order between lb and ub
  //  is ignored, which may be not detectable
  //  by the following symbolic operation
  //  step3 => lb_exp = lmin{lb1*lb2, ...}

  //  ////////////////////////  /
  //  Note
  //  for step 1, a reduce() in all the bound is recommended, because it
  //  reduces the for step 2, conservative sign check is recommended, to
  //  determine the relative order

  CMarsRangeExpr r1 = op1;
  CMarsRangeExpr r2 = op2;

  r1.reduce();
  r2.reduce();

  vector<CMarsExpression> lb_cand;
  vector<CMarsExpression> ub_cand;

  if (r1.IsConstant()) {
    CMarsExpression i_r1 = r1.GetConstant();
    return r2 * i_r1;
  }
  if (r2.IsConstant()) {
    CMarsExpression i_r2 = r2.GetConstant();
    return r1 * i_r2;
  }

  if (r1.IsNonNegative() && r2.IsNonNegative()) {
    lb_cand = r1.m_lb * r2.m_lb;  //  vector * vector -> vector
    ub_cand = r1.m_ub * r2.m_ub;
  } else if (r1.IsNonNegative() && r2.IsNonPositive()) {
    lb_cand = r1.m_ub * r2.m_lb;
    ub_cand = r1.m_lb * r2.m_ub;
  } else if (r1.IsNonPositive() && r2.IsNonNegative()) {
    lb_cand = r1.m_lb * r2.m_ub;
    ub_cand = r1.m_ub * r2.m_lb;
  } else if (r1.IsNonPositive() && r2.IsNonPositive()) {
    lb_cand = r1.m_ub * r2.m_ub;
    ub_cand = r1.m_lb * r2.m_lb;
  } else if (r1.IsNonNegative()) {
    lb_cand = r1.m_lb * r2.m_lb;  //  vector * vector -> vector
    ub_cand = r1.m_ub * r2.m_ub;
    lb_cand = lb_cand + r1.m_ub * r2.m_lb;
    ub_cand = ub_cand + r1.m_lb * r2.m_ub;
  } else if (r1.IsNonPositive()) {
    lb_cand = r1.m_lb * r2.m_ub;
    ub_cand = r1.m_ub * r2.m_lb;
    lb_cand = lb_cand + r1.m_ub * r2.m_ub;
    ub_cand = ub_cand + r1.m_lb * r2.m_lb;
  } else if (r2.IsNonNegative()) {
    lb_cand = r1.m_lb * r2.m_lb;  //  vector * vector -> vector
    ub_cand = r1.m_ub * r2.m_ub;
    lb_cand = lb_cand + r1.m_lb * r2.m_ub;
    ub_cand = ub_cand + r1.m_ub * r2.m_lb;
  } else if (r2.IsNonPositive()) {
    lb_cand = r1.m_ub * r2.m_lb;
    ub_cand = r1.m_lb * r2.m_ub;
    lb_cand = lb_cand + r1.m_ub * r2.m_ub;
    ub_cand = ub_cand + r1.m_lb * r2.m_lb;
  } else {
    lb_cand = r1.m_lb * r2.m_lb;  //  vector * vector -> vector
    ub_cand = r1.m_ub * r2.m_ub;
    lb_cand = lb_cand + r1.m_lb * r2.m_ub;
    ub_cand = ub_cand + r1.m_ub * r2.m_lb;
    lb_cand = lb_cand + r1.m_ub * r2.m_lb;
    ub_cand = ub_cand + r1.m_lb * r2.m_ub;
    lb_cand = lb_cand + r1.m_ub * r2.m_ub;
    ub_cand = ub_cand + r1.m_lb * r2.m_lb;
  }

  mr = CMarsRangeExpr(lb_cand, ub_cand);
  mr.reduce();
  return mr;

#if 0
  //  ZP: to be continued
  assert(0);

  vector<CMarsExpression> bound1;
  bound1.insert(bound1.begin(), op1.m_lb.begin(), op1.m_lb.end());
  bound1.insert(bound1.begin(), op1.m_ub.begin(), op1.m_ub.end());

  vector<CMarsExpression> bound2;
  bound2.insert(bound2.begin(), op2.m_lb.begin(), op2.m_lb.end());
  bound2.insert(bound2.begin(), op2.m_ub.begin(), op2.m_ub.end());

  size_t i, j;
  if (bound1.size() && bound2.size()) {
    CMarsExpression me_min;
    for (i = 0; i < bound1.size(); i++) {
      for (j = 0; j < bound2.size(); j++) {
        if (i == 0 && j == 0)
          me_min = bound1[i] * bound2[i];
        else
          me_min = lmin(bound1[i] * bound2[j], me_min);
      }
    }
  }
  CMarsExpression me_max;
  {
    for (i = 0; i < bound1.size(); i++) {
      for (j = 0; j < bound2.size(); j++) {
        if (i == 0 && j == 0)
          me_max = bound1[i] * bound2[j];
        else
          me_max = umax(bound1[i] * bound2[j], me_max);
      }
    }
  }

  return mr;

  //  old version

  if (op2.has_lb() && op2.has_ub()) {
    CMarsRangeExpr lb = min(op1 * op2.get_lb(), op1 * op2.get_ub());
    CMarsRangeExpr ub = max(op1 * op2.get_lb(), op1 * op2.get_ub());
    if (lb.has_lb())
      mr.set_lb(lb.get_lb());
    if (ub.has_ub())
      mr.set_ub(ub.get_ub());
  } else if (op2.has_lb()) {  //  op2.get_ub = +infinite
    if (op1.has_lb() && op1.get_lb().IsNonNegative()) {
      //  if op1 >= 0
      CMarsRangeExpr lb = op1 * op2.get_lb();
      if (lb.has_lb())
        mr.set_lb(lb.get_lb());
    } else if (op1.has_ub() && (-op1.get_ub()).IsNonNegative()) {
      //  if op1 <= 0
      CMarsRangeExpr ub = op1 * op2.get_lb();
      if (ub.has_ub())
        mr.set_ub(ub.get_ub());
    } else {
      //  mr = [-na..na]
    }
  } else if (op2.has_ub()) {
    if (op1.has_lb() && op1.get_lb().IsNonNegative()) {
      CMarsRangeExpr ub = (op1 * op2.get_ub());
      if (ub.has_ub())
        mr.set_ub(ub.get_ub());
    } else if (op1.has_ub() && (-op1.get_ub()).IsNonNegative()) {
      CMarsRangeExpr lb = (op1 * op2.get_ub());
      if (lb.has_lb())
        mr.set_lb(lb.get_lb());
    } else {
      //  mr = [-na..na]
    }
  } else {
    //  mr = [-na..na]
  }

  return mr;
#endif
}
#endif

CMarsRangeExpr operator/(const CMarsRangeExpr &op1,
                         const CMarsExpression &op2) {
  size_t i;
  CMarsRangeExpr range;
  for (i = 0; i < op1.m_lb.size(); i++) {
    range.AppendLB(op1.m_lb[i] / op2);
    range.AppendUB(op1.m_lb[i] / op2);
  }

  for (i = 0; i < op1.m_ub.size(); i++) {
    range.AppendLB(op1.m_ub[i] / op2);
    range.AppendUB(op1.m_ub[i] / op2);
  }

  bool is_exact = true;
  if (op1.is_exact() == 0) {
    is_exact = false;
  }
  if (op2.IsConstant() == 0) {
    is_exact = false;
  }

  range.set_exact(is_exact);
  range.reduce();

  return range;
}

//  ZP : 20150828
//  This marcro affacts the precision of the range analysis
//  the larger the more temporary results are stored, hence, less conservative
//  Note: this value can not be large, in those case, it may cause
//  ROSE hang on code generation
#define MAX_RANGE_RECORD_SLOT 5

void CMarsRangeExpr::reduce() {
  size_t i;
  size_t j;
  vector<bool> to_remove;
  int is_n_inf = 0;
  int is_p_inf = 0;
  to_remove.resize(m_lb.size());
  for (i = 0; i < m_lb.size(); i++) {
    to_remove[i] = false;
  }
  for (i = 0; i < m_lb.size(); i++) {
    if (to_remove[i]) {
      continue;
    }
    if (m_lb[i].is_nINF() != 0) {
      is_n_inf = 1;
      break;
    }
    for (j = i + 1; j < m_lb.size(); j++) {
      if (m_lb[j].is_nINF() != 0) {
        is_n_inf = 1;
        break;
      }
      //  CMarsExpression diff = m_lb[i] - m_lb[j];
      if ((m_lb[i] >= m_lb[j]) != 0) {
        void *new_pos = m_lb[j].IsConstant() != 0 ? nullptr : m_lb[j].get_pos();
        CMarsAST_IF *ast = m_lb[j].get_ast();
        if (new_pos != nullptr) {
          new_pos = ast->GetCommonPosition(new_pos, m_lb[i].get_pos());
          if (new_pos != nullptr) {
            m_lb[j].set_pos(new_pos);
          } else {
            continue;
          }
        }
        to_remove[i] = true;
      } else if ((m_lb[i] <= m_lb[j]) != 0) {
        void *new_pos = m_lb[i].IsConstant() != 0 ? nullptr : m_lb[i].get_pos();
        CMarsAST_IF *ast = m_lb[i].get_ast();
        if (new_pos != nullptr) {
          new_pos = ast->GetCommonPosition(new_pos, m_lb[j].get_pos());
          if (new_pos != nullptr) {
            m_lb[i].set_pos(new_pos);
          } else {
            continue;
          }
        }
        to_remove[j] = true;
      }

      //  FIXME: ZP 20150825: very dangerous
      //  if (m_lb[i].unparse() == m_lb[j].unparse()) to_remove[i] = 1;
    }
  }
  vector<CMarsExpression> new_lb;
  for (i = 0; i < m_lb.size(); i++) {
    if (!to_remove[i] && (is_n_inf == 0)) {
      new_lb.push_back(m_lb[i]);
    }
  }

  //  //////////////////////////////////////////  /
  //  FIXME: ZP 20150829:to control the complexy
  if (new_lb.size() > MAX_RANGE_RECORD_SLOT) {
    new_lb.clear();  //  set to unbounded
  }
  //  //////////////////////////////////////////  /
  m_lb = new_lb;
  to_remove.resize(m_ub.size());
  for (i = 0; i < m_ub.size(); i++) {
    to_remove[i] = false;
  }
  for (i = 0; i < m_ub.size(); i++) {
    if (to_remove[i]) {
      continue;
    }
    if (m_ub[i].is_pINF() != 0) {
      is_p_inf = 1;
      break;
    }
    for (j = i + 1; j < m_ub.size(); j++) {
      if (m_ub[j].is_pINF() != 0) {
        is_p_inf = 1;
        break;
      }
      //  CMarsExpression diff = m_ub[i] - m_ub[j];
      if ((m_ub[i] >= m_ub[j]) != 0) {
        void *new_pos = m_ub[i].IsConstant() != 0 ? nullptr : m_ub[i].get_pos();
        CMarsAST_IF *ast = m_ub[i].get_ast();
        if (new_pos != nullptr) {
          new_pos = ast->GetCommonPosition(new_pos, m_ub[j].get_pos());
          if (new_pos != nullptr) {
            m_ub[i].set_pos(new_pos);
          } else {
            continue;
          }
        }
        to_remove[j] = true;

      } else if ((m_ub[i] <= m_ub[j]) != 0) {
        void *new_pos = m_ub[j].IsConstant() != 0 ? nullptr : m_ub[j].get_pos();
        CMarsAST_IF *ast = m_ub[j].get_ast();
        if (new_pos != nullptr) {
          new_pos = ast->GetCommonPosition(new_pos, m_ub[i].get_pos());
          if (new_pos != nullptr) {
            m_ub[j].set_pos(new_pos);
          } else {
            continue;
          }
        }
        to_remove[i] = true;
      }
      //
      //  FIXME: ZP 20150825: very dangerous
      //  if (m_ub[i].unparse() == m_ub[j].unparse()) to_remove[i] = 1;
    }
  }
  vector<CMarsExpression> new_ub;
  for (i = 0; i < m_ub.size(); i++) {
    if (!to_remove[i] && (is_p_inf == 0)) {
      new_ub.push_back(m_ub[i]);
    }
  }

  //  //////////////////////////////////////////  /
  //  FIXME: ZP 20150829:to control the complexy
  if (new_ub.size() > MAX_RANGE_RECORD_SLOT) {
    new_ub.clear();  //  set to unbounded
  }
  //  //////////////////////////////////////////  /

  m_ub = new_ub;
}

int CMarsRangeExpr::substitute(void *var, void *pos) {
  CMarsRangeExpr var_range = CMarsVariable(var, get_ast(), pos).get_range();
  return substitute(var, var_range.get_lb(), var_range.get_ub(),
                    var_range.is_exact());
}

int is_range_connected(const CMarsExpression &lb0, const CMarsExpression &ub0,
                       const CMarsExpression &lb1, const CMarsExpression &ub1) {
  //  1. check if lb1 is in [lb0-1, ub0+1]

  {
    //  CMarsExpression diff_lb = lb1 - (lb0 - 1);
    //  CMarsExpression diff_ub = lb1 - (ub0 + 1);
    if (((lb1 <= (lb0 - 1)) != 0) && ((lb1 >= (ub0 + 1)) != 0)) {
      return 1;
    }
    if (((lb1 >= (lb0 - 1)) != 0) && ((lb1 <= (ub0 + 1)) != 0)) {
      return 1;
    }
  }
  //  2. check if ub1 is in [lb0-1, ub0+1]
  {
    //  CMarsExpression diff_lb = ub1 - (lb0 - 1);
    //  CMarsExpression diff_ub = ub1 - (ub0 + 1);
    if (((ub1 <= (lb0 - 1)) != 0) && ((ub1 >= (ub0 + 1)) != 0)) {
      return 1;
    }
    if (((ub1 >= (lb0 - 1)) != 0) && ((ub1 <= (ub0 + 1)) != 0)) {
      return 1;
    }
  }
  //  3. check if lb0 is in [lb1-1, ub1+1]
  {
    //  CMarsExpression diff_lb = lb0 - (lb1 - 1);
    //  CMarsExpression diff_ub = lb0 - (ub1 + 1);
    if (((lb0 <= (lb1 - 1)) != 0) && ((lb0 >= ub1 + 1) != 0)) {
      return 1;
    }
    if (((lb0 >= (lb1 - 1)) != 0) && ((lb0 <= (ub1 + 1)) != 0)) {
      return 1;
    }
  }
  //  4. check if ub0 is in [lb1-1, ub1+1]
  {
    //  CMarsExpression diff_lb = ub0 - (lb1 - 1);
    //  CMarsExpression diff_ub = ub0 - (ub1 + 1);
    if (((ub0 <= (lb1 - 1)) != 0) && ((ub0 >= (ub1 + 1)) != 0)) {
      return 1;
    }
    if (((ub0 >= (lb1 - 1)) != 0) && ((ub0 <= (ub1 + 1)) != 0)) {
      return 1;
    }
  }
  return 0;
}

int CMarsRangeExpr::has_var(void *var) {
  size_t i;
  for (i = 0; i < get_lb_set().size(); i++) {
    if ((get_lb(i).get_coeff(var) != 0) != 0) {
      return 1;
    }
  }
  for (i = 0; i < get_ub_set().size(); i++) {
    if ((get_ub(i).get_coeff(var) != 0) != 0) {
      return 1;
    }
  }
  return 0;
}

//  ////////////////////////////////////////////////  ]
//  ZP: substitute variable ranges
//  Carefully consideration is done for exact flag calculation
//  Note:
//     Exact flag propagated to indicate that the range is
//     actually accessed for all the data elements in the range.
//     This analysis is very useful and necessary for write access,
//     to determine if the result after the write access will still
//     determined by the original value or not
//  Algorithm:
//     The analysis performs a conservative propagation using the following
//     rules We assume we replace "i" in "mr_in" with "lb_i" and "ub_i", and
//     we got "mr_out"
//     1. if the range lb/ub of i is not exact, mr_out is not exact
//     2. if mr_in is inexact, mr_out is inexact
//     3. if mr_in is not simple form (one lb, one ub), mr_out is inexact
//     4. if the coeffs of i in mr_in bounds is not constant, mr_out is
//     inexact
//     5. [formula I]: if (lb[i], ub[i]) and (lb[i+1], ub[i+1) overlap, mr_out
//     is inexact
//     6. The order of the substituted vars matters a lot on the result of
//     [formula I]
//     7. The exact check will be done by trying the possible substitution
//     order
//        in a greedy way ( to be verify if it impacts the accuracy)
//  is_exact indicates whether the range of the replaced variable is exact or
//  not
int CMarsRangeExpr::substitute(void *var, const CMarsExpression &lb,
                               const CMarsExpression &ub,
                               bool is_exact /*=0*/) {
  size_t i;

  //  ZP: 20160211
  {
    if (has_var(var) == 0) {
      return 0;
    }
    if (is_empty() != 0) {
      return 0;
    }
  }
  if (lb.is_pINF() && ub.is_nINF()) {
    set_empty(get_ast());
    set_exact(is_exact);
    return 1;
  }
  int ret = 0;
  //  ZP: 20160205
  //  Calculate exact flag before substitution
  {
    if (!is_exact) {
      m_is_exact = false;
    }
    //  1. Check lb/ub number
    if ((m_is_exact) && ((lb - ub == 0) == 0)) {
      reduce();
      if (m_lb.size() != 1 || m_ub.size() != 1) {
        m_is_exact = false;
      }
    }
    if (((lb - ub == 0) == 0) && (m_is_exact)) {
      CMarsExpression me_lb = m_lb[0];
      CMarsExpression me_ub = m_ub[0];
      //  2. get the coeff, cl and cu
      int64_t cl;
      int64_t cu;
      if (m_is_exact) {
        CMarsExpression tmp = me_lb.get_coeff(var);
        if (tmp.IsConstant() == 0) {
          m_is_exact = false;
        } else {
          cl = tmp.GetConstant();
        }
        tmp = me_ub.get_coeff(var);
        if (tmp.IsConstant() == 0) {
          m_is_exact = false;
        } else {
          cu = tmp.GetConstant();
        }
      }

      if (m_is_exact) {
        CMarsExpression me_lb_1 = me_lb + cl;
        CMarsExpression me_ub_1 = me_ub + cu;
        m_is_exact = is_range_connected(me_lb, me_ub, me_lb_1, me_ub_1);
      }
    }
  }

  //  Add all the candidates into the bounds
  {
    vector<CMarsExpression> new_lb;
    for (i = 0; i < m_lb.size(); i++) {
      CMarsExpression me = m_lb[i];
      CMarsExpression me1 = me;
      CMarsExpression coeff = me.get_coeff(var);
      if (coeff.IsConstant() != 0) {
        if (coeff.GetConstant() > 0) {
          ret |= me.substitute(var, lb);
          new_lb.push_back(me);
        } else {
          ret |= me1.substitute(var, ub);
          new_lb.push_back(me1);
        }
      } else {
        //  if (!coeff.IsNonPositive())
        {
          ret |= me.substitute(var, lb);
          new_lb.push_back(me);
        }
        //  if (!coeff.IsNonNegative())
        {
          ret |= me1.substitute(var, ub);
          new_lb.push_back(me1);
        }
      }
    }
    m_lb = new_lb;
  }

  {
    vector<CMarsExpression> new_ub;
    for (i = 0; i < m_ub.size(); i++) {
      CMarsExpression me = m_ub[i];
      CMarsExpression me1 = me;
      CMarsExpression coeff = me.get_coeff(var);
      if (coeff.IsConstant() != 0) {
        if (coeff.GetConstant() > 0) {
          ret |= me.substitute(var, ub);
          new_ub.push_back(me);
        } else {
          ret |= me1.substitute(var, lb);
          new_ub.push_back(me1);
        }
      } else {
        //  if (!coeff.IsNonPositive())
        {
          ret |= me.substitute(var, ub);
          new_ub.push_back(me);
        }
        //  if (!coeff.IsNonNegative())
        {
          ret |= me1.substitute(var, lb);
          new_ub.push_back(me1);
        }
      }
    }
    m_ub = new_ub;
  }
  if (is_empty() != 0) {
    //  Youxiang: 20190523 overflow may happen
    std::swap(m_lb, m_ub);
  }

#if _DEBUG_SUBSTITUTE_
  string str_old = print_e();
#endif

#if 0  //  _DEBUG_SUBSTITUTE_
  cout << "Range Subt " << str_old << " : {"
       << CMarsVariable(var, get_ast()).unparse() << " -> " << lb.print_s()
       << ".." << ub.print_s() << "} -> " << print() << endl;
#endif

  reduce();

#if _DEBUG_SUBSTITUTE_
  cout << "Range Subs " << str_old
       << " : {"
       //  << CMarsVariable(var, get_ast()).unparse() << " -> " << lb.print_s()
       << m_ast->_p(var) << " -> " << lb.print_s() << ".." << ub.print_s()
       << "} -> " << print_e() << endl;
#endif
  return ret;
}

int CMarsRangeExpr::is_const_lb() const {
  if (m_lb.empty()) {
    return 0;
  }

  int has_a_const = 0;

  for (size_t i = 0; i < m_lb.size(); i++) {
    CMarsExpression lb = m_lb[i];

    if (lb.is_pINF() != 0) {
      return 0;
    }
    if (lb.is_nINF() != 0) {
      continue;
    }

    if (lb.IsConstant() != 0) {
      has_a_const = 1;
      continue;
    }
    if (!lb.has_range()) {
      return 0;
    }
    CMarsRangeExpr lb_range = lb.get_range();

    if (lb_range.is_empty() != 0) {
      continue;
    }
    if (lb_range == *this) {
      return 0;
    }
    if (lb_range.is_const_lb() == 0) {
      return 0;
    }

    has_a_const = 1;
  }

  return has_a_const;
}

int64_t CMarsRangeExpr::get_const_lb() const {
  if (m_lb.empty()) {
    return MARS_INT64_MIN;
  }
  int64_t ret = MARS_INT64_MAX;
  for (size_t i = 0; i < m_lb.size(); i++) {
    CMarsExpression lb = m_lb[i];

    if (lb.is_pINF() != 0) {
      continue;
    }
    if (lb.is_nINF() != 0) {
      return MARS_INT64_MIN;
    }
    if (lb.IsConstant() != 0) {
      ret = std::min(ret, lb.GetConstant());
    } else if (!lb.has_range()) {
      return MARS_INT64_MIN;
    } else {
      int64_t lblb = lb.get_range().get_const_lb();
      ret = std::min(ret, lblb);
    }
  }
  return ret;
}

int CMarsRangeExpr::is_const_ub() const {
  if (m_ub.empty()) {
    return 0;
  }

  int has_a_const = 0;

  for (size_t i = 0; i < m_ub.size(); i++) {
    CMarsExpression ub = m_ub[i];

    if (ub.is_nINF() != 0) {
      return 0;
    }
    if (ub.is_pINF() != 0) {
      continue;
    }

    if (ub.IsConstant() != 0) {
      has_a_const = 1;
      continue;
    }
    if (!ub.has_range()) {
      return 0;
    }
    CMarsRangeExpr ub_range = ub.get_range();

    if (ub_range.is_empty() != 0) {
      continue;
    }
    if (ub_range == *this) {
      return 0;
    }
    if (ub_range.is_const_ub() == 0) {
      return 0;
    }

    has_a_const = 1;
  }

  return has_a_const;
}

//  int CMarsRangeExpr::is_const_ub() const {
//
//  if (m_ub.size() <=0 ) return 0;
//
//  int has_a_const = 0;
//  for (size_t i = 0; i < m_ub.size(); i++) {
//    CMarsExpression ub = m_ub[i];
//
//    if (ub.is_nINF())
//      return 0;
//    else if (ub.is_pINF())
//      continue;
//    else if (ub.IsConstant())
//    { has_a_const = 1; continue; }
//    else if (!ub.has_range())
//      return 0;
//    else {
//      if (!ub.get_range().is_const_ub())
//        return 0;
//      else has_a_const = 1;
//    }
//  }
//  return has_a_const;
//  }

int64_t CMarsRangeExpr::get_const_ub() const {
  if (m_ub.empty()) {
    return MARS_INT64_MAX;
  }
  int64_t ret = MARS_INT64_MIN;
  for (size_t i = 0; i < m_ub.size(); i++) {
    CMarsExpression ub = m_ub[i];

    if (ub.is_nINF() != 0) {
      continue;
    }
    if (ub.is_pINF() != 0) {
      return MARS_INT64_MAX;
    }
    if (ub.IsConstant() != 0) {
      ret = std::max(ret, ub.GetConstant());
    } else if (!ub.has_range()) {
      return MARS_INT64_MAX;
    } else {
      int64_t ubub = ub.get_range().get_const_ub();
      ret = std::max(ret, ubub);
    }
  }
  return ret;
}

#if 0
CMarsExpression CMarsRangeExpr::GetConstant() {
  CMarsAST_IF *ast;
  if (has_lb())
    ast = m_lb[0].get_ast();
  if (has_ub())
    ast = m_ub[0].get_ast();
  else
    return full_INF;

  //  FIXME: ZP
  return CMarsExpression(ast, get_const_lb());
}
#endif

//  These operations are done in a conservative way
CMarsRangeExpr CMarsRangeExpr::Union(const CMarsRangeExpr &range) const {
  //  #if _DEBUG_RANGE_UNION_
  //  cout << "--RangeUnion: " << print_e() << " U " << range.print_e() <<
  //  endl; #endif

  //  assert(get_ast());
  CMarsRangeExpr ret(get_ast());

  int is_exact;

  //  handling empty
  if (is_empty() != 0) {
    ret = range;
  } else if (range.is_empty() != 0) {
    ret = *this;
  } else {
    CMarsRangeExpr merged_range;
    if (is_cover(range, &merged_range) != 0) {
      return merged_range;
    }
    if (range.is_cover(*this, &merged_range) != 0) {
      return merged_range;
    }
    size_t i;
    ret = *this;

    is_exact =
        static_cast<int>((static_cast<int>(this->is_exact() != 0) != 0) &&
                         (range.is_exact()) != 0);
    if (is_exact) {
      if (get_lb_set().size() != 1 || range.get_lb_set().size() != 1) {
        is_exact = false;
      }
      if (get_ub_set().size() != 1 || range.get_ub_set().size() != 1) {
        is_exact = false;
      }
    }
    if (is_exact) {
      CMarsExpression lb1 = get_lb_set()[0];
      CMarsExpression ub1 = get_ub_set()[0];
      CMarsExpression lb2 = range.get_lb_set()[0];
      CMarsExpression ub2 = range.get_ub_set()[0];

      is_exact = is_range_connected(lb1, ub1, lb2, ub2);
    }

    if ((has_lb() == 0) || (range.has_lb() == 0)) {
      ret.AppendLB(neg_INF);
    } else {
      for (i = 0; i < range.get_lb_set().size(); i++) {
        //            if (ret.get_lb(0).get_pointer() == (void*)0x15de5d0)
        //                printf("2");
        ret.AppendLB(range.get_lb(i));
      }
    }

    if ((has_ub() == 0) || (range.has_ub() == 0)) {
      ret.AppendUB(pos_INF);
    } else {
      for (i = 0; i < range.get_ub_set().size(); i++) {
        ret.AppendUB(range.get_ub(i));
      }
    }

    ret.set_exact(is_exact);
  }

  ret.reduce();
  if (ret.is_exact() != 0) {
    if (ret.get_lb_set().size() != 1) {
      ret.set_exact(false);
    }
    if (ret.get_ub_set().size() != 1) {
      ret.set_exact(false);
    }
  }

  //  #if _DEBUG_RANGE_UNION_
  //  cout << "  ret=" << ret.print_e() << endl;
  //  #endif
  return ret;
}

int CMarsRangeExpr::is_cover(const CMarsRangeExpr &mr,
                             CMarsRangeExpr *merged_range) const {
  if (is_exact() == 0) {
    return 0;
  }
  if (is_empty() != 0) {
    return 0;
  }
  if (mr.is_empty() != 0) {
    *merged_range = *this;
    return 1;
  }
  if ((mr.has_lb() == 0) || (mr.has_ub() == 0)) {
    return 0;
  }
  size_t i;
  size_t j;

  CMarsAST_IF *ast = get_ast();
  for (i = 0; i < this->m_lb.size(); i++) {
    CMarsExpression lb = m_lb[i];
    void *new_pos = lb.IsConstant() != 0 ? nullptr : lb.get_pos();
    for (j = 0; j < mr.m_lb.size(); j++) {
      //  CMarsExpression diff = lb - mr.m_lb[j];
      if ((lb <= mr.m_lb[j]) == 0) {
        return 0;
      }
      if (new_pos != nullptr) {
        new_pos = ast->GetCommonPosition(new_pos, mr.m_lb[j].get_pos());
        if (new_pos == nullptr) {
          return 0;
        }
      }
    }
    if (new_pos != nullptr) {
      lb.set_pos(new_pos);
    }
    merged_range->AppendLB(lb);
  }

  for (i = 0; i < this->m_ub.size(); i++) {
    CMarsExpression ub = m_ub[i];
    void *new_pos = ub.IsConstant() != 0 ? nullptr : ub.get_pos();
    for (j = 0; j < mr.m_ub.size(); j++) {
      if ((ub >= mr.m_ub[j]) == 0) {
        return 0;
      }
      if (new_pos != nullptr) {
        new_pos = ast->GetCommonPosition(new_pos, mr.m_ub[j].get_pos());
        if (new_pos == nullptr) {
          return 0;
        }
      }
    }
    if (new_pos != nullptr) {
      ub.set_pos(new_pos);
    }
    merged_range->AppendUB(ub);
  }
  merged_range->set_exact(is_exact());
  return 1;
}

//  Note: this intersection is performed in a conservative way, so if
CMarsRangeExpr CMarsRangeExpr::Intersect(const CMarsRangeExpr &range) const {
  if ((is_empty() != 0) || (range.has_bound() == 0)) {
    return *this;
  }
  if ((range.is_empty() != 0) || (has_bound() == 0)) {
    return range;
  }
  if (is_single_access()) {
    CMarsExpression lb;
    CMarsExpression ub;
    int simple_bound = range.get_simple_bound(&lb, &ub);
    if ((simple_bound != 0) &&
        (((get_lb(0) == lb) != 0) || ((get_lb(0) == ub) != 0))) {
      return *this;
    }
  }
  if (range.is_single_access()) {
    CMarsExpression lb;
    CMarsExpression ub;
    int simple_bound = get_simple_bound(&lb, &ub);
    if ((simple_bound != 0) &&
        (((range.get_lb(0) == lb) != 0) || ((range.get_lb(0) == ub) != 0))) {
      return range;
    }
  }
  int is_exact;
  CMarsRangeExpr ret;
  if (has_lb() == 0) {
    ret.m_lb = range.get_lb_set();
  } else if (range.has_lb() == 0) {
    ret.m_lb = get_lb_set();
  }

  if (has_ub() == 0) {
    ret.m_ub = range.get_ub_set();
  } else if (range.has_ub() == 0) {
    ret.m_ub = get_ub_set();
  }
  vector<CMarsExpression> lb1 = get_lb_set();
  vector<CMarsExpression> ub1 = get_ub_set();
  vector<CMarsExpression> lb2 = range.get_lb_set();
  vector<CMarsExpression> ub2 = range.get_ub_set();

  size_t i;
  size_t j;
  set<int> to_erased_1;
  set<int> to_erased_2;
  for (i = 0; i < lb2.size(); i++) {
    CMarsExpression e2 = lb2[i];
    bool valid = false;
    for (j = 0; j < lb1.size(); j++) {
      CMarsExpression e1 = lb1[j];
      //  CMarsExpression r = e1 - e2;
      if ((e1 >= e2) == 0) {
        valid = false;
        break;
      }
      { valid = true; }
    }
    if (valid) {
      to_erased_2.insert(i);
    }
  }
  for (i = 0; i < lb1.size(); i++) {
    CMarsExpression e1 = lb1[i];
    bool valid = false;
    for (j = 0; j < lb2.size(); j++) {
      if (to_erased_2.count(j) > 0) {
        continue;
      }
      CMarsExpression e2 = lb2[j];
      //  CMarsExpression r = e2 - e1;
      if ((e2 >= e1) == 0) {
        valid = false;
        break;
      }
      { valid = true; }
    }
    if (valid) {
      to_erased_1.insert(i);
    }
  }
  for (i = 0; i < lb1.size(); i++) {
    if (to_erased_1.count(i) > 0) {
      continue;
    }
    ret.AppendLB(lb1[i]);
  }

  for (i = 0; i < lb2.size(); i++) {
    if (to_erased_2.count(i) > 0) {
      continue;
    }
    ret.AppendLB(lb2[i]);
  }

  to_erased_1.clear();
  to_erased_2.clear();
  for (i = 0; i < ub2.size(); i++) {
    CMarsExpression e2 = ub2[i];
    bool valid = false;
    for (j = 0; j < ub1.size(); j++) {
      CMarsExpression e1 = ub1[j];
      //  CMarsExpression r = e1 - e2;
      if ((e1 <= e2) == 0) {
        valid = false;
        break;
      }
      { valid = true; }
    }
    if (valid) {
      to_erased_2.insert(i);
    }
  }
  for (i = 0; i < ub1.size(); i++) {
    bool valid = false;
    CMarsExpression e1 = ub1[i];
    for (j = 0; j < ub2.size(); j++) {
      if (to_erased_2.count(j) > 0) {
        continue;
      }
      CMarsExpression e2 = ub2[j];
      //  CMarsExpression r = e2 - e1;
      if ((e2 <= e1) == 0) {
        valid = false;
        break;
      }
      { valid = true; }
    }
    if (valid) {
      to_erased_1.insert(i);
    }
  }

  for (i = 0; i < ub1.size(); i++) {
    if (to_erased_1.count(i) > 0) {
      continue;
    }
    ret.AppendUB(ub1[i]);
  }

  for (i = 0; i < ub2.size(); i++) {
    if (to_erased_2.count(i) > 0) {
      continue;
    }
    ret.AppendUB(ub2[i]);
  }

  is_exact = static_cast<int>((static_cast<int>(this->is_exact() != 0) != 0) &&
                              (range.is_exact()) != 0);
  ret.set_exact(is_exact);
  return ret;
}

void CMarsRangeExpr::set_pos(void *pos) {
  for (auto &lb : m_lb) {
    if (lb.is_INF())
      continue;
    lb.set_pos(pos);
  }
  for (auto &ub : m_ub) {
    if (ub.is_INF())
      continue;
    ub.set_pos(pos);
  }
}

/*******************************************************************/
//  class CMarsAccess
/*******************************************************************/

//  get array, exp_list, order
void CMarsAccess::analysis_exp() {
  static int count = 0;
  assert(m_ref);
  assert(!m_ast->is_floating_node(m_pos));

  vector<void *> vec_indexes;
  //  m_ast->parse_pntr_ref(m_ref, &m_array, &vec_indexes);
  int pointer_dim = 0;
  void *sg_pntr = nullptr;
  void *array_ref = nullptr;

#if 0
  static int s_cnt = 0;
  cout << "-- s_cnt=" << s_cnt << endl;
  if (s_cnt == 9)
      cout << " " << endl;
  s_cnt++;
#endif

  m_ast->parse_array_ref_from_pntr(m_ref, &array_ref, m_pos);
  if (array_ref == nullptr) {
    return;
  }
  m_ast->parse_pntr_ref_by_array_ref(array_ref, &m_array, &sg_pntr,
                                     &vec_indexes, &pointer_dim, m_pos);

  for (size_t i = 0; i < vec_indexes.size(); i++) {
    //  CMarsExpression me(vec_indexes[i], m_ast, m_ref);
    CMarsExpression me(vec_indexes[i], m_ast, m_pos, m_scope);
    m_expr_list.push_back(me);

    count++;
  }
}

//  get iv, pv according to scope
void CMarsAccess::analysis_var() {
  size_t i;
  assert(m_ref);
  m_iv.clear();
  m_pv.clear();

  if (m_scope == nullptr) {
    m_scope = m_ast->GetProject();
  }

  vector<void *> vec_loops;

  void *pos = (m_pos) != nullptr ? m_pos : m_ref;
  m_ast->get_loop_nest_in_scope(pos, m_scope, &vec_loops);

  //  iterator variable is set as all the loops between the ref and the scope
  m_iv = vec_loops;

  //  all the other variables are in the m_pv
  map<void *, int> map_all_vars;
  for (i = 0; i < m_expr_list.size(); i++) {
    vector<void *> vars;
    m_expr_list[i].get_vars(&vars);
    for (size_t j = 0; j < vars.size(); j++) {
      map_all_vars[vars[j]] = 1;
    }
  }

  map<void *, int> map_iv;
  for (i = 0; i < m_iv.size(); i++) {
    map_iv[m_iv[i]] = 1;
  }

  map<void *, int>::iterator it;
  for (it = map_all_vars.begin(); it != map_all_vars.end(); it++) {
    if (map_iv.find(it->first) == map_iv.end()) {
      m_pv.push_back(it->first);
    }
  }
}
string CMarsAccess::print_s() {
  string str;
  size_t i;
  str += m_ast->UnparseToString(m_ref);
  str += ": ";

  //    str += m_ast->UnparseToString(m_array);
  //    vector<CMarsRangeExpr> ranges = GetRangeExpr();
  //    for (i = 0; i < ranges.size(); i++)
  //    {
  //        str += "[" + ranges[i].print() + "]";
  //    }

  str += " iv=";
  for (i = 0; i < m_iv.size(); i++) {
    if (i > 0) {
      str += ",";
    }
    CMarsVariable var(m_iv[i], m_ast);
    str += var.print_var();
  }

  void *scope = m_scope;
  if (m_ast->GetLoopBody(m_ast->GetParent(m_scope)) == m_scope) {
    scope = m_ast->GetParent(m_scope);
  }

  str += " scope=" + CMarsVariable(scope, m_ast).unparse();

  return str;
}

string CMarsAccess::print() {
  string str;
  size_t i;
  str += m_ast->UnparseToString(m_ref);
  str += ": ";

  str += m_ast->UnparseToString(m_array);
  vector<CMarsRangeExpr> ranges = GetRangeExpr();
  for (i = 0; i < ranges.size(); i++) {
    str += "[" + ranges[i].print() + "]";
  }

  str += " iv=";
  for (i = 0; i < m_iv.size(); i++) {
    if (i > 0) {
      str += ",";
    }
    CMarsVariable var(m_iv[i], m_ast, m_pos);
    str += var.print_var();
  }

  str += " pv=";
  for (i = 0; i < m_pv.size(); i++) {
    if (i > 0) {
      str += ",";
    }
    CMarsVariable var(m_pv[i], m_ast, m_pos);
    str += var.print_var();
  }

  str += " scope=" + m_ast->UnparseToString(m_scope);

  return str;
}

vector<CMarsRangeExpr> CMarsAccess::GetRangeExpr() {
  vector<CMarsRangeExpr> ret;
  for (size_t i = 0; i < m_expr_list.size(); i++) {
    ret.push_back(GetRangeExpr(i));
  }
  return ret;
}

//  1. detect if pv is modified in the scope (if so, set range to na)
//  2. substitute iv with ranges
CMarsRangeExpr CMarsAccess::GetRangeExpr(int idx) {
  CMarsExpression me(m_expr_list[idx]);
  CMarsRangeExpr range(me, me,
                       1);  //  by default it is exact, because me == me

  range.refine_range_in_scope(m_scope, m_pos);
  return range;

#if 0

  assert(idx >= 0 && idx < m_expr_list.size());
#if _DEBUG_CHECK_PV_
  {
    cout << "  --check pv[" << m_ast->_p(m_ref) << "] --" << endl;
  }
#endif
  CMarsRangeExpr infinite;

  //  1. detect if pv is modified in the scope (if so, set range to na)
  {
    for (size_t i = 0; i < m_pv.size(); i++) {
#if _DEBUG_CHECK_PV_
      {
        static int count = 0;
        cout << "  --check pv: count=" << count << " " << m_ast->_p(m_pv[i])
             << endl;
        count++;
      }
#endif
      if (m_ast->IsForStatement(m_pv[i]))
        continue;
      if (m_ast->is_floating_node(m_pv[i]) ||
          m_ast->is_located_in_scope(m_pv[i], m_scope)) {
        //                return infinite;
      }
    }
  }

  //  2. get all the variables to replace
  vector<void*>           vec_sub_vars;
  vector<CMarsExpression> vec_sub_lbs;
  vector<CMarsExpression> vec_sub_ubs;
  vector<int>             vec_sub_is_exact;

  vector<void *> vec_loops;
  m_ast->get_loop_nest_in_scope(m_pos, m_scope, &vec_loops);
  m_iv = vec_loops;
  {
    for (size_t i = 0; i < m_iv.size(); i++) {
      CMarsRangeExpr var_range = CMarsVariable(m_iv[i], m_ast).get_range();
      CMarsExpression me_lb = var_range.get_lb();
      CMarsExpression me_ub = var_range.get_ub();
      vec_sub_vars.push_back(m_iv[i]);
      vec_sub_lbs.push_back(me_lb);
      vec_sub_ubs.push_back(me_ub);
      vec_sub_is_exact.push_back(var_range.is_exact());
    }
  }

  //  3. Single assignement tracing: 20160202 ZP
  //  Note:
  //  * In the current implementation, expression substitution in
  //  CMarsExpression stoped if the new term is not movable to the position
  //  of the old term.
  //  * However, in range analysis, movability is not necessary for the term,
  //  unless the tracing is within the scope of the m_scope.
  //  * So an additional tracing is needed to get the exact range at the scope
  {
    for (size_t i = 0; i < m_pv.size(); i++) {
        void * curr_term = m_pv[i];
        if (m_ast->IsVarRefExp(curr_term)) {
            void * curr_name = m_ast->GetVariableInitializedName(curr_term);
            void * sg_exp = m_ast->GetSingleAssignExpr(curr_name, m_pos, 0,
                m_scope);  //  support for scope is not added
            if (sg_exp) {
                if (m_ast->IsVarRefExp(sg_exp)) {
                    void * new_name = m_ast->GetVariableInitializedName(sg_exp);
                    void *sg_loop = m_ast->GetLoopFromIteratorByPos(new_name,
                        m_pos, 1);
                    if (sg_loop && m_ast->is_located_in_scope(sg_loop,
                          m_scope)) {
                        CMarsRangeExpr var_range = CMarsVariable(sg_loop,
                            m_ast).get_range();
                        CMarsExpression me_lb = var_range.get_lb();
                        CMarsExpression me_ub = var_range.get_ub();
                        sg_exp = sg_loop;
                        //  range.substitute(curr_term, me_lb, me_ub);

                        //  vec_sub_vars.push_back(curr_term);
                        //  vec_sub_lbs.push_back(me_lb);
                        //  vec_sub_ubs.push_back(me_ub);
                        //  vec_sub_is_exact.push_back(var_range.is_exact());

                        //  {
                        //    int t;
                        //    for (t = 0; t < m_iv.size(); t++) {
                        //        if (m_iv[t] == sg_loop) break;
                        //    }
                        //    assert(t != m_iv.size());
                        //  }


                        //  continue;
                    }
                }

                if (m_ast->is_located_in_scope(sg_exp, m_scope)) {
                    CMarsExpression me_exp(sg_exp, m_ast, nullptr);
                    range.substitute(curr_term, me_exp, me_exp, 1);
                    //  vec_sub_vars.push_back(curr_term);
                    //  vec_sub_lbs.push_back(me_exp);
                    //  vec_sub_ubs.push_back(me_exp);
                    //  vec_sub_is_exact.push_back(1);
                }
            }
        }
    }
  }

  //  ////////////////////////////////////////////////  ]
  //  ZP: substitute variable ranges
  //  Carefully consideration is done for exact flag calculation
  //  Note:
  //     Exact flag propagated to indicate that the range is
  //     actually accessed for all the data elements in the range.
  //     This analysis is very useful and necessary for write access,
  //     to determine if the result after the write access will still
  //     determined by the original value or not
  //  Algorithm:
  //     The analysis performs a conservative propagation using the following
  //     rules. We assume we replace "i" in "mr_in" with "lb_i" and "ub_i",
  //     and we got "mr_out"
  //     1. if the range lb/ub of i is not exact, mr_out is not exact
  //     2. if mr_in is inexact, mr_out is inexact
  //     3. if mr_in is not simple form (one lb, one ub), mr_out is inexact
  //     4. if the coeffs of i in mr_in bounds is not constant, mr_out is
  //        inexact
  //     5. [formula I]: if (lb[i], ub[i]) and (lb[i+1], ub[i+1) overlap,
  //     mr_out is inexact
  //     6. The order of the substituted vars matters a lot on the result of
  //     [formula I]
  //     7. The exact check will be done by trying the possible substitution
  //     order in a greedy way ( to be verify if it impacts the accuracy)


  bool is_exact = false;
  vector<int> idx_remain;
  for (size_t i = 0; i < vec_sub_vars.size(); i++) idx_remain.push_back(i);
  {
    while (1) {
        //  1. select one var for exact reduce
        size_t i;
        for (i = 0; i < idx_remain.size(); i++) {
            //  1.1 try to perform exact check, pick one which
            //  can be substituted exactly
            CMarsRangeExpr range_tmp(range);
            int j = idx_remain[i];
            range_tmp.substitute(vec_sub_vars[j], vec_sub_lbs[j],
                vec_sub_ubs[j], vec_sub_is_exact[j]);
#if _DEBUG_EXACT_FLAG_
            {
                CMarsVariable mv(vec_sub_vars[j], m_ast);
                cout << "--Try Exact ("<< mv.print() <<"): " <<
                    range.print_e() << " --> " << range_tmp.print_e() << endl;
            }
#endif
            //  1.2 perform the substitution if it is exact
            if (range_tmp.is_exact()) {
                range = range_tmp;
                break;
            }
        }

        //  2. prepare for the next round if an exact substitutin is found
        if (i != idx_remain.size()) {
            //  2.1 remove idx_remain[i] from idx_remain
            {
                vector<int> idx_remain_new;
                for (size_t j = 0; j < idx_remain.size(); j++) {
                    if (j != i) idx_remain_new.push_back(idx_remain[j]);
                }
                idx_remain = idx_remain_new;
            }
            //  2.2 if it is the last one to remove, then exact = true;`
            if (idx_remain.size() == 0) {
                is_exact = true;
            }
        } else {
            is_exact = false;
            break;
        }
    }
  }

  {
      for (size_t i = 0; i < idx_remain.size(); i++) {
        int j = idx_remain[i];
        //  range.substitute(vec_sub_vars[i], vec_sub_lbs[i], vec_sub_ubs[i]);
        range.substitute(vec_sub_vars[j], vec_sub_lbs[j], vec_sub_ubs[j],
            vec_sub_is_exact[j]);
      }
  }

  return range;
#endif
}

#if KEEP_UNUSED
//  1. detect if pv is modified in the scope (if so, set range to na)
//  2. substitute iv with ranges
CMarsRangeExpr CMarsAccess::GetRangeExpr_old(int idx) {
  assert(idx >= 0 && (size_t)idx < m_expr_list.size());
#if _DEBUG_CHECK_PV_
  { cout << "  --check pv[" << m_ast->_p(m_ref) << "] --" << endl; }
#endif

  CMarsRangeExpr infinite(m_ast);

  //  1. detect if pv is modified in the scope (if so, set range to na)
  {
    for (size_t i = 0; i < m_pv.size(); i++) {
#if _DEBUG_CHECK_PV_
      {
        static int count = 0;
        cout << "  --check pv: count=" << count << " " << m_ast->_p(m_pv[i])
             << endl;
        count++;
      }
#endif
      if (m_ast->IsForStatement(m_pv[i]))
        continue;
      if (m_ast->is_floating_node(m_pv[i]) ||
          m_ast->is_located_in_scope(m_pv[i], m_scope)) {
        //                return infinite;
      }
    }
  }

  //  2. substitute iv with ranges
  CMarsExpression me(m_expr_list[idx]);
  CMarsRangeExpr range(me, me);
  {
    for (size_t i = 0; i < m_iv.size(); i++) {
      CMarsRangeExpr var_range = CMarsVariable(m_iv[i], m_ast).get_range();

      //  ZP: FIX: 20150824: also check if all the variables in the range
      //  bounds are in the scope
      CMarsExpression me_lb = var_range.get_lb();
      CMarsExpression me_ub = var_range.get_ub();

      //            if (me_lb.any_var_located_in_scope(m_scope)) return
      //            infinite;
      //            if (me_ub.any_var_located_in_scope(m_scope)) return
      //            infinite;

      range.substitute(m_iv[i], me_lb, me_ub);
    }
  }

  //  3. Single assignement tracing: 20160202 ZP
  //  Note:
  //  * In the current implementation, expression substitution in
  //  CMarsExpression stoped if the new term is not movable to the position of
  //  the old term.
  //  * However, in range analysis, movability is not necessary for the term,
  //  unless the tracing is within the scope of the m_scope.
  //  * So an additional tracing is needed to get the exact range at the scope
  {
    for (size_t i = 0; i < m_pv.size(); i++) {
      void *curr_term = m_pv[i];
      if (m_ast->IsVarRefExp(curr_term)) {
        void *curr_name = m_ast->GetVariableInitializedName(curr_term);
        void *sg_exp = m_ast->GetSingleAssignExpr(
            curr_name, m_pos, 0, m_scope);  //  support for scope is not added
        if (sg_exp) {
          if (m_ast->IsVarRefExp(sg_exp)) {
            void *new_name = m_ast->GetVariableInitializedName(sg_exp);
            void *sg_loop = m_ast->GetLoopFromIteratorByPos(new_name, m_pos, 1);
            if (sg_loop && m_ast->is_located_in_scope(sg_loop, m_scope)) {
              CMarsRangeExpr var_range =
                  CMarsVariable(sg_loop, m_ast).get_range();
              CMarsExpression me_lb = var_range.get_lb();
              CMarsExpression me_ub = var_range.get_ub();
              sg_exp = sg_loop;
              range.substitute(curr_term, me_lb, me_ub);

              continue;
            }
          }

          if (m_ast->is_located_in_scope(sg_exp, m_scope) &&
              m_ast->is_movable_test(sg_exp, m_pos)) {
            CMarsExpression me_exp(sg_exp, m_ast, nullptr);
            range.substitute(curr_term, me_exp, me_exp);
          }
        }
      }
    }
  }

  return range;
}
#endif

//  Simple Access Type: Single Variable Continous access
//  For each dimension: the index is like "i+1" or "i-2"
//  1. each dimension has at most one non-zero variable
//  2. the coefficient of the variable is constant 1 or -1
//  3. each variable exist in at most one dimension
bool CMarsAccess::is_simple_type_v1() {
  auto indexes = GetIndexes();
  return is_simple_type_v1(indexes);
}

bool CMarsAccess::is_simple_type_v1(const vector<CMarsExpression> &indexes) {
  auto is_type = true;
  set<void *> vars_hit;
  for (auto &idx : indexes) {
    //  1. check if no more than one term
    vector<void *> vec_vars;
    idx.get_vars(&vec_vars);
    if (!(vec_vars.size() <= 1)) {
      is_type = false;
    }

    //  2. check if the coefficent is one
    if (vec_vars.size() == 1) {
      CMarsExpression coeff = idx.get_coeff(vec_vars[0]);
      if (coeff.IsConstant() == 0) {
        is_type = false;
      } else if (1 != coeff.get_const() && -1 != coeff.get_const()) {
        is_type = false;
      }

      if (vars_hit.count(vec_vars[0]) != 0U) {
        return false;
      }

      vars_hit.insert(vec_vars[0]);
    }
  }

  return is_type;
}

//  for each array dim: tuple<var, coeff, const>
vector<tuple<void *, int64_t, int64_t>>
CMarsAccess::simple_type_v1_get_dim_to_var() {
  vector<tuple<void *, int64_t, int64_t>> ret;
  assert(is_simple_type_v1());

  auto indexes = GetIndexes();

  for (auto &idx : indexes) {
    auto vec_vars = idx.get_vars();
    if (vec_vars.size() == 1) {
      tuple<void *, int64_t, int64_t> dim_info(
          vec_vars[0], idx.get_const_coeff(vec_vars[0]), idx.get_const());
      ret.push_back(dim_info);
    } else if (vec_vars.empty()) {
      tuple<void *, int64_t, int64_t> dim_info(nullptr, 0, idx.get_const());
      ret.push_back(dim_info);
    } else {
      assert(false);
    }
  }

  return ret;
}

//  for each variable: array_dim, coeff, const
//  array_dim = -1 if the variable does not exist in any dimension
//  is_compact: do not list the unrelated loops
vector<tuple<int, int64_t, int64_t>>
CMarsAccess::simple_type_v1_get_var_to_dim(vector<void *> vec_vars,
                                           int is_compact) {
  assert(is_simple_type_v1());

  vector<tuple<int, int64_t, int64_t>> ret;
  for (auto var : vec_vars) {
    auto indexes = GetIndexes();
    bool hit = false;
    for (size_t i = 0; i < indexes.size(); i++) {
      auto idx = indexes[i];
      if ((idx.get_coeff(var) != 0) != 0) {
        ret.push_back(tuple<int, int64_t, int64_t>(i, idx.get_const_coeff(var),
                                                   idx.get_const()));
        hit = true;
        break;
      }
    }
    if (!hit && (is_compact == 0)) {
      ret.push_back(tuple<int, int64_t, int64_t>(-1, 0, 0));
    }
  }

  return ret;
}

class CMarsRangeExprSet;

void GetAccessRangeInScope_v2(void *array, void *ref_scope, void *range_scope,
                              CMarsAST_IF *ast, vector<CMarsRangeExpr> *r_range,
                              vector<CMarsRangeExpr> *w_range,
                              CMarsRangeExpr *r_flatten_range,
                              CMarsRangeExpr *w_flatten_range,
                              int64_t *access_size, bool check_access_bound);

void GetAccessRangeInScope_v2(void *array, void *ref_scope, void *range_scope,
                              CMarsAST_IF *ast,
                              const list<t_func_call_path> &vec_paths,
                              vector<CMarsRangeExpr> *r_range,
                              vector<CMarsRangeExpr> *w_range,
                              CMarsRangeExpr *r_flatten_range,
                              CMarsRangeExpr *w_flatten_range,
                              int64_t *access_size, bool check_access_bound);

void GetAccessRangeInScope_in_path(void *array, void *ref_scope,
                                   t_func_call_path fn_path, void *range_scope,
                                   CMarsAST_IF *ast,
                                   vector<CMarsRangeExprSet> *r_range,
                                   vector<CMarsRangeExprSet> *w_range,
                                   bool eval_access_size);

CMarsArrayRangeInScope::CMarsArrayRangeInScope(
    void *array, CMarsAST_IF *ast, const list<t_func_call_path> &vec_call_path,
    void *ref_scope, void *range_scope, bool eval_access_size,
    bool check_access_bound) {
  assert(ast->IsInitName(array));
  m_array = array;
  m_ast = ast;
  m_scope = ref_scope;
  m_eval_access_size = eval_access_size;
  m_access_size = 0;
  m_check_access_bound = check_access_bound;
  //  20161002: ZP
  g_exact_flag_cache_enabled = 1;
  g_exact_flag_cache.clear();

  DEFINE_START_END;
  STEMP(start);
  //  m_expr_list, m_iv and m_pv is empty
  GetAccessRangeInScope_v2(
      m_array, m_scope, range_scope, m_ast, vec_call_path, &m_range_r,
      &m_range_w, &m_flatten_range_r, &m_flatten_range_w,
      eval_access_size ? &m_access_size : nullptr, m_check_access_bound);
  TIMEP("GetAccessRangeInScope_v2", start, end);

  g_exact_flag_cache_enabled = 0;
  g_exact_flag_cache.clear();
}

#if KEEP_UNUSED
string CMarsArrayRangeInScope::print() {
  string str;
  str = m_ast->UnparseToString(m_array);
  str += ":";
  str += print_s();
  return str;
}
#endif
string CMarsArrayRangeInScope::print_s() {
  size_t i;
  string str;

  size_t dim = m_range_w.size();
  assert(dim == m_range_r.size());

  if (has_read() != 0) {
    str += " R";
    for (i = 0; i < dim; i++) {
      str += "[" + m_range_r[i].print_e() + "]";
    }
  }

  if (has_write() != 0) {
    str += " W";
    for (i = 0; i < dim; i++) {
      str += "[" + m_range_w[i].print_e() + "]";
    }
  }

  return str;
}

int CMarsArrayRangeInScope::has_read() {
  int dim = m_range_r.size();

  for (int i = 0; i < dim; i++) {
    if (m_range_r[i].is_empty() != 0) {
      return 0;
    }
  }

  return 1;
}

int CMarsArrayRangeInScope::has_write() {
  int dim = m_range_w.size();

  for (int i = 0; i < dim; i++) {
    if (m_range_w[i].is_empty() != 0) {
      return 0;
    }
  }

  return 1;
}

/*******************************************************************/
//  Function GetDependence
/*******************************************************************/

int sign(int64_t a) {
  if (a > 0) {
    return 1;
  }
  if (a < 0) {
    return -1;
  }
  return 0;
}

//  Range based dependence analysis
int GetDependence_v2(CMarsAccess access1, CMarsAccess access2,
                     CMarsDepDistSet *dist_set,
                     CMarsResultFlags *results_flags) {
  //  if the scope is not the common innermost loop:
  //  a) If the range is fully covered => 0
  //  b) else => return *

  assert(access1.get_ast() == access2.get_ast());

  void *scope = access1.get_scope();
  assert(scope == access2.get_scope());

  vector<CMarsRangeExpr> ranges1 = access1.GetRangeExpr();
  vector<CMarsRangeExpr> ranges2 = access2.GetRangeExpr();

  assert(ranges1.size() == ranges2.size());
  int dim = ranges1.size();

  //  Note: in this version we just check if the first access can fully cover
  //  the second one. So we just have two following cases:
  //  1. The first reference can fully cover the second at the loop level:
  //  distset= {0}
  //  2. Can not: distset = na..na
  int is_meet = 1;
  for (int i = 0; i < dim; i++) {
    CMarsRangeExpr merged_range;
    if ((ranges1[i].is_exact() != 0) &&
        (ranges1[i].is_cover(ranges2[i], &merged_range) != 0)) {
      //  do nothing
    } else {
      is_meet = 0;
    }
  }

  if (is_meet != 0) {
    results_flags->set_flag("valid", 1);
    *dist_set = CMarsDepDistSet(0, 0);
  } else {
    results_flags->set_flag("valid", 0);
    *dist_set = CMarsDepDistSet(CMarsRangeExpr::MARS_INT64_MIN,
                                CMarsRangeExpr::MARS_INT64_MAX);
  }

  return 1;
}

//  FIXME: read/write is not cared in the current version
//  return status:
//  "exact": whether the result is exact (non-conservative)
//  "valid": whether the result is valid (not valid if not analyzable)
//  NOTE: When calculating the dependence across a specific for-loop,
//       Pleaes use its body to set the scope for the accesses
int GetDependence(CMarsAccess access1, CMarsAccess access2,
                  CMarsDepDistSet *dist_set, CMarsResultFlags *results_flags) {
  results_flags->set_flag("valid", 0);
  results_flags->set_flag("exact", 0);

  assert(access1.get_ast() == access2.get_ast());
  CMarsAST_IF *ast = access1.get_ast();

  //  FIXME: in this version we only support two accesses with the same scope
  if (access1.get_scope() != access2.get_scope()) {
    return 0;
  }

  void *scope = access1.get_scope();

  //  FIXME: only support for-loop as scope
  //  if (!ast->IsForStatement(scope))
  //  return 0;
  //  if (!ast->IsForStatement(scope))
  if ((scope != nullptr) && scope != ast->GetProject()) {
    void *for_loop = ast->TraceUpToForStatement(ast->GetParent(scope));
    if ((for_loop == nullptr) || scope != ast->GetLoopBody(for_loop)) {
      return GetDependence_v2(access1, access2, dist_set, results_flags);
    }

    scope = for_loop;
  }
  //  assert(ast->IsForStatement(scope));

  //  Chech array and dim
  void *array1 = access1.GetArray();
  void *array2 = access2.GetArray();
  assert(array1 == array2);
  vector<CMarsExpression> indexes1 = access1.GetIndexes();
  vector<CMarsExpression> indexes2 = access2.GetIndexes();
  assert(indexes1.size() == indexes2.size());

  //  for each dimension
  int64_t lb = CMarsRangeExpr::MARS_INT64_MIN;
  int64_t ub = CMarsRangeExpr::MARS_INT64_MAX;
  for (size_t i = 0; i < indexes1.size(); i++) {
    size_t j;
    CMarsExpression index1 = indexes1[i];
    CMarsExpression index2 = indexes2[i];
    //  A*p1+B*i1+C*x1+c1 = D*p2+E*i2+F*x2+c2
    //  p: parameters
    //  i: scope, current loop
    //  x: inner loops
    vector<void *> iv1 = access1.m_iv;
    vector<void *> iv2 = access2.m_iv;
    vector<void *> pv1 = access1.m_pv;
    vector<void *> pv2 = access2.m_pv;

    //  FIXME: only support B=E
    CMarsExpression me_B = index1.get_coeff(scope);
    CMarsExpression me_E = index2.get_coeff(scope);
    assert(me_B == me_E);

    //  B*(i2-i1) = A*p1-D*p2 + (C*x1 - F*x2) + c1-c2
    CMarsExpression me_term21 = index1;
    CMarsRangeExpr me_range21(me_term21, me_term21);
    for (j = 0; j < iv1.size(); j++) {
      me_range21.substitute(iv1[j], nullptr);
    }

    CMarsExpression me_term22 = index2;
    CMarsRangeExpr me_range22(me_term22, me_term22);
    for (j = 0; j < iv2.size(); j++) {
      me_range22.substitute(iv2[j], nullptr);
    }

    CMarsRangeExpr diff = me_range21 - me_range22;
    CMarsExpression zero(ast, 0);
    diff.substitute(scope, zero, zero);

    int64_t diff_lb = diff.get_const_lb();
    int64_t diff_ub = diff.get_const_ub();

    if (((me_B == 0) != 0) && sign(diff_lb) * sign(diff_ub) <= 0) {
      //  no constraints for dist
    } else if ((me_B == 0) != 0) {
      //  no dependence
      results_flags->set_flag("valid", 1);
      results_flags->set_flag("exact", 1);
      dist_set->set_empty();
      return 1;
    } else if ((diff.is_const_bound() != 0) && (me_B.IsConstant() != 0)) {
      int64_t b0 = me_B.GetConstant();
      int64_t b = b0;
      if (b < 0) {
        b = -b;
      }

      int64_t actual_lb = (diff_lb / b) * b;
      if (actual_lb < diff_lb) {
        actual_lb += b;
      }
      int64_t actual_ub = (diff_ub / b) * b;
      if (actual_ub > diff_ub) {
        actual_ub -= b;
      }

      if (b0 > 0) {
        lb = std::max(actual_lb / b0, lb);
        ub = std::min(actual_ub / b0, ub);
      } else {
        lb = std::max(actual_ub / b0, lb);
        ub = std::min(actual_lb / b0, ub);
      }
    } else {
      CMarsRangeExpr dist = diff / me_B;
      //  FIXME: only handle the cases that dist is bounded
      if (dist.has_lb() != 0) {
        lb = std::max(dist.get_const_lb(), lb);
      }
      if (dist.has_ub() != 0) {
        ub = std::min(dist.get_const_ub(), ub);
      }
    }
  }

  //  check if the distance is in the range
  {
    CMarsRangeExpr var_range = CMarsVariable(scope, ast).get_range();
    int64_t var_lb = var_range.get_const_lb();
    int64_t var_ub = var_range.get_const_ub();

    int64_t max_dist = var_ub - var_lb;
    int64_t min_dist = -max_dist;

    if (lb > max_dist || ub < min_dist) {
      results_flags->set_flag("valid", 1);
      results_flags->set_flag("exact", 1);
      dist_set->set_empty();
      return 1;
    }
  }

  if (lb <= ub) {
    CMarsDepDistSet dep_dist(lb, ub);
    *dist_set = dep_dist;
  }

  results_flags->set_flag("valid", 1);
  return 1;
}

class CMarsRangeExprSet {
 public:
  CMarsRangeExprSet() {
    m_is_set_to_inexact = false;
    m_ast = nullptr;
    m_access_size = 0;
  }

  void Collect(CMarsRangeExpr mr, int64_t access_size) {
    if (mr.is_empty() != 0) {
      return;
    }
    m_range_set.push_back(mr);
    assert(mr.get_ast());
    if (numeric_limits<int64_t>::max() - access_size > m_access_size) {
      m_access_size += access_size;
    } else {
      m_access_size = numeric_limits<int64_t>::max();
    }
  }
  CMarsRangeExpr Union();
  void Assign(CMarsRangeExpr mr, int64_t access_size) {
    m_range_set.clear();
    m_range_set.push_back(mr);
    assert(mr.get_ast());
    if (numeric_limits<int64_t>::max() - access_size > m_access_size) {
      m_access_size += access_size;
    } else {
      m_access_size = numeric_limits<int64_t>::max();
    }
  }
  void set_inexact() { m_is_set_to_inexact = true; }
  void set_ast(CMarsAST_IF *ast) { m_ast = ast; }

  void merge(const CMarsRangeExprSet &other);
  string print() const;
  int is_set_empty() const { return static_cast<int>(m_range_set.empty()); }

  bool is_full_access(int64_t size) const;

  //  Extended for range analysis v2
  void Collect(CMarsRangeExpr mr, void *array, void *ref, int is_write,
               t_func_call_path fn_path, int64_t access_size) {
    if (mr.is_empty() != 0) {
      return;
    }
    Collect(mr, access_size);
    m_array_set.push_back(array);
    m_ref_set.push_back(ref);
#if PROJDEBUG
    assert(ref && !m_ast->is_floating_node(ref));
#endif
    m_iswrite_set.push_back(is_write);
    m_fnpath_set.push_back(fn_path);
  }
  void Collect_with_offset_with_pos(const CMarsRangeExprSet &mrs,
                                    const CMarsRangeExpr &offset,
                                    int set_inexact,
                                    int64_t outside_access_size) {
    for (size_t i = 0; i < mrs.m_range_set.size(); i++) {
      CMarsRangeExpr mr = mrs.m_range_set[i] + offset;
      if (set_inexact) {
        mr.set_exact(false);
      }
      mr.set_pos(mrs.m_ref_set[i]);
      mr.update_position(mrs.m_fnpath_set[i], mrs.m_ref_set[i]);
      int64_t total_access_size = 0;
      int64_t curr_access_size = mrs.get_access_size();
      if ((outside_access_size != 0) && (curr_access_size != 0)) {
        if (numeric_limits<int64_t>::max() / outside_access_size >
            curr_access_size) {
          total_access_size = outside_access_size * curr_access_size;
        } else {
          total_access_size = numeric_limits<int64_t>::max();
        }
      }
      Collect(mr, mrs.m_array_set[i], mrs.m_ref_set[i], mrs.m_iswrite_set[i],
              mrs.m_fnpath_set[i], total_access_size);
    }
  }
  void Assign(CMarsRangeExpr mr, void *array, void *ref, int is_write,
              t_func_call_path fn_path, int64_t access_size) {
    Assign(mr, access_size);
    m_array_set.clear();
    m_array_set.push_back(array);
    m_ref_set.clear();
    m_ref_set.push_back(ref);
#if PROJDEBUG
    assert(ref && !m_ast->is_floating_node(ref));
#endif
    m_iswrite_set.clear();
    m_iswrite_set.push_back(is_write);
    m_fnpath_set.clear();
    m_fnpath_set.push_back(fn_path);
  }
  CMarsRangeExpr Union(void *range_scope);
  CMarsRangeExpr Union_v2(void *range_scope);

  int refine_range_exact(int idx, void *range_scope);
  void erase_range(int idx);
  void update_control_flag(int idx, void *range_scope);
  int remove_covered_range(void *range_scope);

  int refine_range_exact(void *range_scope);
  int merge_range_exact(void *range_scope);

  int64_t get_access_size() const { return m_access_size; }

  //  ZP: 20170318
  int is_empty_conservative() {
    for (auto mr : m_range_set) {
      if (mr.is_empty() == 0) {
        return 0;
      }
    }
    return 1;
  }

#if KEEP_UNUSED
  void expand_set_by_path(void *range_scope);
#endif
  void refine_range_in_scope(void *range_scope, void *pos);
  void check_size();
  void refine_range(int idx, void *range_scope);

  CMarsRangeExprSet(const CMarsRangeExprSet &range_set)
      : m_range_set(range_set.m_range_set),
        m_is_set_to_inexact(range_set.m_is_set_to_inexact),
        m_ast(range_set.m_ast), m_array_set(range_set.m_array_set),
        m_ref_set(range_set.m_ref_set), m_iswrite_set(range_set.m_iswrite_set),
        m_fnpath_set(range_set.m_fnpath_set),
        m_access_size(range_set.m_access_size) {}

 protected:
  vector<CMarsRangeExpr> m_range_set;
  int m_is_set_to_inexact;  //  provide a way to set inexact easily
  CMarsAST_IF *m_ast;

  vector<void *> m_array_set;
  vector<void *> m_ref_set;
  vector<int> m_iswrite_set;
  vector<t_func_call_path> m_fnpath_set;
  int64_t m_access_size;
};

void CMarsRangeExprSet::merge(const CMarsRangeExprSet &other) {
  for (int i = 0; i != other.m_range_set.size(); ++i) {
    void *ref = other.m_ref_set[i];
    bool duplicated = false;
    for (int j = 0; j != m_range_set.size(); ++j) {
      if (ref == m_ref_set[j]) {
        if (m_ast->is_located_in_path_v2(
                ref, m_ast->GetCommonPath(other.m_fnpath_set[i],
                                          m_fnpath_set[j])) != 0) {
          duplicated = true;
          break;
        }
      }
    }
    if (duplicated) {
      continue;
    }
    Collect(other.m_range_set[i], other.m_array_set[i], ref,
            other.m_iswrite_set[i], other.m_fnpath_set[i],
            other.get_access_size());
  }
}

#if KEEP_UNUSED
void CMarsRangeExprSet::expand_set_by_path(void *range_scope) {
#if 0
    vector<CMarsRangeExpr>   range_set   = m_range_set;
    vector<void *>           array_set   = m_array_set;
    vector<void *>           ref_set     = m_ref_set;
    vector<int>              iswrite_set = m_iswrite_set;

    assert(range_set.size() == array_set.size());
    assert(range_set.size() == ref_set.size());
    assert(range_set.size() == iswrite_set.size());

    m_range_set.clear();
    m_array_set.clear();
    m_ref_set.clear();
    m_iswrite_set.clear();

    set<void *> ref_record;

    for (size_t i = 0; i < range_set.size(); i++) {
        if (range_set[i].is_empty()) continue;

        CMarsAST_IF * ast = range_set[i].get_ast();
        list<t_func_call_path> paths;
        ast->get_call_paths(ref_set[i], range_scope, &paths);

        //  if (ref_record.count(ref_set[i])) continue;

        ref_record.insert(ref_set[i]);

#if _DEBUG_FUNC_PATH_
        if (paths.size() != 1) {
            cout << "#path=" << paths.size()
              << " scope='" << ast->UnparseToString(range_scope).substr(0, 20)
              << "' ref='"   << ast->UnparseToString(ref_set[i]).substr(0, 20)
              << "' array='"   << ast->UnparseToString(array_set[i]).substr(0,
                  20) << "'" << endl;
        }
#endif

        for (size_t j =0; j < paths.size(); j++) {
            //  GetAccessRangeInScope_in_path();
            m_range_set.push_back(range_set[i]);
            m_array_set.push_back(array_set[i]);
            m_ref_set.push_back(ref_set[i]);
            m_iswrite_set.push_back(iswrite_set[i]);
            m_fnpath_set.push_back(paths[j]);
        }
    }
#else

  list<t_func_call_path> vec_paths;

  //  get all the paths
  {
    vector<void *> ref_set = m_ref_set;
    vector<CMarsRangeExpr> range_set = m_range_set;
    for (size_t i = 0; i < ref_set.size(); i++) {
      CMarsAST_IF *ast = range_set[i].get_ast();
      list<t_func_call_path> paths;
      ast->get_call_paths(ref_set[i], range_scope, &paths);

      for (auto path_it = paths.begin(); path_it != paths.end(); path_it++) {
        auto vec_paths_it = vec_paths.begin();
        //  add to list if not found
        while (*vec_paths_it != *path_it && vec_paths_it != vec_paths.end())
          ++vec_paths_it;
        if (vec_paths_it == vec_paths.end()) {
          vec_paths.push_back(*path_it);
        }
      }
    }
  }

  for (auto path : vec_paths) {
  }
#endif
}
#endif
#if KEEP_UNUSED
void CMarsRangeExprSet::refine_range_in_scope(void *range_scope, void *pos) {
  for (size_t i = 0; i < m_range_set.size(); i++) {
    m_range_set[i].refine_range_in_scope(range_scope, pos);
  }
}
#endif

void CMarsRangeExprSet::check_size() {
  assert(m_range_set.size() == m_ref_set.size());
  assert(m_range_set.size() == m_array_set.size());
  assert(m_range_set.size() == m_iswrite_set.size());
  assert(m_range_set.size() == m_fnpath_set.size());
}

CMarsRangeExpr CMarsRangeExprSet::Union(void *range_scope) {
  //  assert(m_range_set.size() == m_ref_set.size());
  //  assert(m_range_set.size() == m_array_set.size());
  //  assert(m_range_set.size() == m_iswrite_set.size());
  //  expand_set_by_path(range_scope);

  assert(m_range_set.size() == m_ref_set.size());
  assert(m_range_set.size() == m_array_set.size());
  assert(m_range_set.size() == m_iswrite_set.size());
  assert(m_range_set.size() == m_fnpath_set.size());

  for (size_t i = 0; i < m_range_set.size(); i++) {
    if (m_range_set[i].is_empty() != 0) {
      continue;
    }
    m_range_set[i].refine_range_in_scope_v2(range_scope, m_ref_set[i],
                                            m_array_set[i], m_iswrite_set[i],
                                            m_fnpath_set[i]);
  }

  return Union();
}

int CMarsRangeExprSet::merge_range_exact(void *range_scope) {
  int is_changed = 0;
  if (m_range_set.size() <= 1) {
    return 0;
  }
  int num_exact = 0;
  for (auto &range : m_range_set) {
    if (range.is_exact() != 0) {
      num_exact++;
    }
  }
  if (num_exact <= 0) {
    return 0;
  }
  if (m_array_set.empty()) {
    return 0;
  }

  DEFINE_START_END;
  STEMP(start);
  for (size_t i = 0; i != m_range_set.size(); ++i) {
    CMarsRangeExpr mr_ret = m_range_set[i];
    if (mr_ret.is_exact() == 0) {
      continue;
    }
    void *first_array = m_array_set[i];
    void *first_ref = m_ref_set[i];
    void *new_pos = first_ref;
    int first_iswrite = m_iswrite_set[i];
    t_func_call_path first_fnpath = m_fnpath_set[i];
    t_func_call_path comm_fnpath = first_fnpath;
    set<int> removed_idx;
    while (true) {
      int hit = 0;
      for (size_t j = 0; j != m_range_set.size(); ++j) {
        if (i == j || removed_idx.count(j) > 0) {
          continue;
        }
        CMarsRangeExpr one_range = m_range_set[j];
        CMarsRangeExpr tmp = mr_ret.Union(one_range);

#if _DEBUG_MERGE_RANGE_V2_
        cout << " -- merge " << tmp.is_exact() << ": " << one_range.print_e()
             << " -> " << tmp.print_e() << endl;
#endif
        if (tmp.is_exact() != 0) {
          t_func_call_path second_fnpath = m_fnpath_set[j];
          mr_ret = tmp;
          new_pos = m_ast->GetCommonPosition(new_pos, comm_fnpath, m_ref_set[j],
                                             second_fnpath);
          comm_fnpath = m_ast->GetCommonPath(comm_fnpath, second_fnpath);
          removed_idx.insert(j);
          hit = 1;
          is_changed = 1;
          break;
        }
      }
      if (hit != 0) {
        continue;
      }
      { break; }
    }
    if (is_changed != 0) {
      removed_idx.insert(i);
      vector<int> vec_idx(removed_idx.begin(), removed_idx.end());
      for (size_t j = vec_idx.size(); j > 0; --j) {
        erase_range(vec_idx[j - 1]);
      }
      if (comm_fnpath.front() != pair<void *, void *>(nullptr, nullptr) &&
          comm_fnpath.front() != pair<void *, void *>(nullptr, new_pos)) {
        if (comm_fnpath.front().second == new_pos) {
          comm_fnpath.pop_front();
        }
        comm_fnpath.push_front(std::make_pair(nullptr, new_pos));
      }
      Collect(mr_ret, first_array, new_pos, first_iswrite, comm_fnpath, 0);
      break;
    }
  }
  TIMEP("    CMarsRangeExprSet::merge_range_exact", start, end);
  return is_changed;
}

int CMarsRangeExprSet::refine_range_exact(void *range_scope) {
  int is_changed = 0;
  DEFINE_START_END;
  STEMP(start);
  for (size_t i = 0; i < m_range_set.size(); i++) {
    is_changed |= refine_range_exact(i, range_scope);
  }
  TIMEP("    CMarsRangeExprSet::refine_range_exact", start, end);
  return is_changed;
}
int CMarsRangeExprSet::refine_range_exact(int idx, void *range_scope) {
  int i = idx;
  return m_range_set[i].refine_range_in_scope_v2_exact(
      range_scope, m_ref_set[i], m_fnpath_set[i]);
}

void CMarsRangeExprSet::refine_range(int idx, void *range_scope) {
  int i = idx;
  m_range_set[i].refine_range_in_scope_v2(range_scope, m_ref_set[i],
                                          m_array_set[i], m_iswrite_set[i],
                                          m_fnpath_set[i]);
}

void CMarsRangeExprSet::erase_range(int idx) {
  m_range_set.erase(m_range_set.begin() + idx);
  m_ref_set.erase(m_ref_set.begin() + idx);
  m_array_set.erase(m_array_set.begin() + idx);
  m_iswrite_set.erase(m_iswrite_set.begin() + idx);
  m_fnpath_set.erase(m_fnpath_set.begin() + idx);
}

void CMarsRangeExprSet::update_control_flag(int idx, void *range_scope) {
  int i = idx;
  m_range_set[i].update_control_flag(range_scope, m_ref_set[i], m_array_set[i],
                                     m_iswrite_set[i], m_fnpath_set[i]);
}

int CMarsRangeExprSet::remove_covered_range(void *range_scope) {
  for (size_t i = 0; i < m_range_set.size(); i++) {
    update_control_flag(i, range_scope);
  }

  int is_changed = 0;
  //  Remove redundant constant ranges
  CMarsRangeExprSet tmp_set = *this;
  for (size_t i = 0; i < m_range_set.size(); i++) {
    tmp_set.refine_range(i, range_scope);
  }

  vector<int> is_erase;
  is_erase.resize(m_range_set.size());
  for (size_t i = 0; i < m_range_set.size(); i++) {
    is_erase[i] = 0;
  }
  for (size_t i = 0; i < m_range_set.size(); i++) {
    if (is_erase[i] != 0) {
      continue;
    }
    CMarsRangeExpr curr_mr = tmp_set.m_range_set[i];
    for (size_t j = 0; j < m_range_set.size(); j++) {
      if (j == i) {
        continue;
      }
      CMarsRangeExpr one_mr = tmp_set.m_range_set[j];
      CMarsRangeExpr merged_range;
      if ((curr_mr.is_const_bound() != 0) &&
          (curr_mr.is_cover(one_mr, &merged_range) != 0)) {
        is_erase[j] = 1;
        tmp_set.m_range_set[i] = merged_range;
        curr_mr = merged_range;
      }
    }
  }
  CMarsRangeExprSet new_set;
  new_set.set_ast(m_ast);
  int size = m_range_set.size();
  check_size();
  bool first = true;
  for (size_t i = 0; i < size; i++) {
    if (is_erase[i] == 0) {
      new_set.Collect(m_range_set[i], m_array_set[i], m_ref_set[i],
                      m_iswrite_set[i], m_fnpath_set[i],
                      first ? get_access_size() : 0);
      first = false;
    }
  }
  *this = new_set;

  return is_changed;
}

//  ///////////////////////////////////////////////////////////////  /
//  Algorithm
//  1. Do exact merge first
//  2. Do exact substitute second
//  3. Do inexact remaining later
CMarsRangeExpr CMarsRangeExprSet::Union_v2(void *range_scope) {
  assert(m_ast);
  CMarsRangeExpr mr_ret(m_ast);

  if (m_range_set.empty()) {
    assert(m_ast);
    mr_ret.set_empty(m_ast);
    return mr_ret;
  }

  mr_ret.set_empty(m_range_set[0].get_ast());

#if 1
  remove_covered_range(range_scope);

  //  int is_changed = 1;
  //  while(is_changed) {
  //    is_changed = 0;
  //    is_changed |= refine_range_exact(range_scope);
  //    is_changed |= merge_range_exact(range_scope);
  //  }
  int is_changed = 0;
  is_changed |= refine_range_exact(range_scope);
  is_changed |= merge_range_exact(range_scope);
  if (is_changed != 0) {
    is_changed = 0;
    is_changed |= refine_range_exact(range_scope);
    is_changed |= merge_range_exact(range_scope);
  }
  if (is_changed != 0) {
    is_changed = 0;
    is_changed |= refine_range_exact(range_scope);
    is_changed |= merge_range_exact(range_scope);
  }
  if (is_changed != 0) {
    is_changed = 0;
    is_changed |= refine_range_exact(range_scope);
    is_changed |= merge_range_exact(range_scope);
  }
  if (is_changed != 0) {
    refine_range_exact(range_scope);
    merge_range_exact(range_scope);
  }

  return Union(range_scope);  //  handling non-exact

#else
  for (size_t i = 0; i < m_range_set.size(); i++)
    refine_range_exact(i, range_scope);

  int scheme = 1;
  list<int> remain_idx;
  list<int>::iterator it;
  if (scheme) {
    for (size_t i = 0; i < m_range_set.size(); i++) {
      //  check if the current one is covered by another
      CMarsRangeExpr curr_mr = m_range_set[i];
      size_t j;
      for (j = 0; j < m_range_set.size(); j++) {
        CMarsRangeExpr one_mr = m_range_set[j];
        if (i == j)
          continue;

        if (one_mr.is_cover(curr_mr) && curr_mr.is_cover(one_mr)) {
          //  if equal range, only keep the leftmost one
          if (i > j)
            break;
        } else if (one_mr.is_cover(curr_mr)) {
          break;
        }
      }
      if (j == m_range_set.size()) {
        remain_idx.push_back(i);
      }
    }
  }

  for (size_t i = 0; i < m_range_set.size(); i++)
    refine_range(i, range_scope);

  while (1) {
    int hit = 0;
    for (it = remain_idx.begin(); it != remain_idx.end(); it++) {
      CMarsRangeExpr one_range = m_range_set[*it];
      CMarsRangeExpr tmp = mr_ret.Union(one_range);
      tmp.set_ast(m_ast);
      if (tmp.is_exact()) {
        mr_ret = tmp;
        remain_idx.erase(it);
        hit = 1;
        break;
      }
    }
    if (hit)
      continue;
    else
      break;
  }

  for (it = remain_idx.begin(); it != remain_idx.end(); it++) {
    CMarsRangeExpr one_range = m_range_set[*it];
    mr_ret = mr_ret.Union(one_range);
    mr_ret.set_ast(m_ast);
  }

#endif
  return mr_ret;
}

CMarsRangeExpr CMarsRangeExprSet::Union() {
  assert(m_ast);
  CMarsRangeExpr mr_ret(m_ast);

  if (m_is_set_to_inexact) {
    mr_ret.set_exact(false);
  }

  if (m_range_set.empty()) {
    assert(m_ast);
    mr_ret.set_empty(m_ast);
    return mr_ret;
  }
  if (m_range_set.size() == 1) {
    mr_ret = m_range_set[0];
    return mr_ret;
  }

  mr_ret.set_empty(m_range_set[0].get_ast());

  if (m_is_set_to_inexact) {
    mr_ret.set_exact(false);
  }

#if 0
    //  Range Union in the order of exact merge first
    if (m_range_set.size() > 0)  {
        mr_ret = m_range_set[0];
        string str_control = m_is_set_to_inexact? "0" : "1";

#if _DEBUG_RANGE_UNION_
  cout << "--RangeUnion (ctrl_exact=" << str_control << "): "
    << m_range_set[0].print_e();
#endif
    }

    for (size_t i = 1; i < m_range_set.size(); i++) {
        mr_ret = mr_ret.Union(m_range_set[i]);
#if _DEBUG_RANGE_UNION_
  cout <<  " U " << m_range_set[i].print_e();
#endif
    }

#if _DEBUG_RANGE_UNION_
  cout << endl;
#endif

    if (m_is_set_to_inexact)
        mr_ret.set_exact(false);

#if _DEBUG_RANGE_UNION_
  cout << "  ret=" << mr_ret.print_e() << endl;
#endif

#else

  list<int> remain_idx;
  list<int>::iterator it;
  if (false) {
    for (size_t i = 0; i < m_range_set.size(); i++) {
      remain_idx.push_back(i);
    }
  } else {
    set<int> erase_idx;
    for (size_t i = 0; i < m_range_set.size(); i++) {
      //  check if the current one is covered by another
      CMarsRangeExpr curr_mr = m_range_set[i];
      size_t j;
      for (j = 0; j < m_range_set.size(); j++) {
        if (erase_idx.count(j) > 0) {
          continue;
        }
        CMarsRangeExpr one_mr = m_range_set[j];
        if (i == j) {
          continue;
        }
        CMarsRangeExpr merged_range;
        if (one_mr.is_cover(curr_mr, &merged_range) != 0) {
          m_range_set[j] = merged_range;
          break;
        }
      }
      if (j == m_range_set.size()) {
        remain_idx.push_back(i);
      } else {
        erase_idx.insert(i);
      }
    }
  }
  if (!m_is_set_to_inexact) {
    while (true) {
      int hit = 0;
      for (it = remain_idx.begin(); it != remain_idx.end(); it++) {
        CMarsRangeExpr one_range = m_range_set[*it];
        CMarsRangeExpr tmp = mr_ret.Union(one_range);
        tmp.set_ast(m_ast);
        if (tmp.is_exact() != 0) {
          mr_ret = tmp;
          remain_idx.erase(it);
          hit = 1;
          break;
        }
      }
      if (hit != 0) {
        continue;
      }
      { break; }
    }
  }

  for (it = remain_idx.begin(); it != remain_idx.end(); it++) {
    CMarsRangeExpr one_range = m_range_set[*it];
    mr_ret = mr_ret.Union(one_range);
    mr_ret.set_ast(m_ast);
  }

#endif

  if (m_is_set_to_inexact) {
    mr_ret.set_exact(false);
  }

  return mr_ret;
}

bool CMarsRangeExprSet::is_full_access(int64_t size) const {
  //  FIXME: currently, we handle each dimension range independently
  //  but we have not gurantee their independence
  for (size_t i = 0; i != m_range_set.size(); ++i) {
    if (m_range_set[i].is_empty() != 0) {
      continue;
    }
    if (m_range_set[i].is_full_access(size)) {
      return true;
    }
  }
  return false;
}

/*************************************************************************/
//  Misc Functions
/*************************************************************************/
void GetFlattenRangeFromNDRange(void *array, CMarsAST_IF *ast,
                                const vector<CMarsRangeExpr> &r_range,
                                const vector<CMarsRangeExpr> &w_range,
                                const vector<CMarsRangeExprSet> &r_range_set,
                                const vector<CMarsRangeExprSet> &w_range_set,
                                CMarsRangeExpr *r_flatten_range,
                                CMarsRangeExpr *w_flatten_range) {
  int dim = r_range.size();

  if (dim == 1) {
    *r_flatten_range = r_range[0];
    *w_flatten_range = w_range[0];
    return;
  }
  vector<size_t> sg_sizes;
  {
    void *sg_base_type;
    ast->get_type_dimension(ast->GetTypebyVar(array), &sg_base_type, &sg_sizes,
                            array);
  }
  assert((size_t)dim == sg_sizes.size());
  vector<int> accumate_sizes(dim, 1);
  for (int i = 1; i < dim; ++i) {
    accumate_sizes[i] = sg_sizes[i - 1] * accumate_sizes[i - 1];
  }
  bool full_read_access;
  bool full_write_access;
  full_read_access = full_write_access = true;

  bool single_read_access;
  bool single_write_access;
  single_read_access = single_write_access = true;
  *r_flatten_range = CMarsRangeExpr(0, 0, ast, 1);
  *w_flatten_range = CMarsRangeExpr(0, 0, ast, 1);
  bool r_range_complete;
  bool w_range_complete;
  r_range_complete = w_range_complete = false;
  for (int i = 0; i < dim; ++i) {
    int reverse_dim = dim - 1 - i;
    if (single_read_access) {
      if (!r_range[i].is_single_access()) {
        single_read_access = false;
      }
      CMarsRangeExpr flatten_range = r_range[i] * accumate_sizes[reverse_dim];
      flatten_range.set_exact(r_range[i].is_exact());
      *r_flatten_range = *r_flatten_range + flatten_range;
    } else {
      if (full_read_access &&
          !r_range_set[i].is_full_access(sg_sizes[reverse_dim])) {
        full_read_access = false;
      }
      if (!r_range_complete) {
        *r_flatten_range =
            *r_flatten_range +
            CMarsRangeExpr(0, accumate_sizes[reverse_dim + 1] - 1, ast, 1);
        r_range_complete = true;
      }
    }

    if (single_write_access) {
      if (!w_range[i].is_single_access()) {
        single_write_access = false;
      }
      CMarsRangeExpr flatten_range = w_range[i] * accumate_sizes[reverse_dim];
      flatten_range.set_exact(w_range[i].is_exact());
      *w_flatten_range = *w_flatten_range + flatten_range;
    } else {
      if (full_write_access &&
          !w_range_set[i].is_full_access(sg_sizes[reverse_dim])) {
        full_write_access = false;
      }
      if (!w_range_complete) {
        *w_flatten_range =
            *w_flatten_range +
            CMarsRangeExpr(0, accumate_sizes[reverse_dim + 1] - 1, ast, 1);
        w_range_complete = true;
      }
    }
  }
  if (!full_read_access) {
    r_flatten_range->set_exact(false);
  }
  if (!full_write_access) {
    w_flatten_range->set_exact(false);
  }
}

static bool is_full_access(shared_ptr<vec_list_rinfo> range_set, int dim,
                           int dim_size) {
  //  FIXME: currently, we handle each dimension range independently
  //  but we have not gurantee their independence
  for (auto &one_rinfo : range_set->at(dim)) {
    if (one_rinfo.range.is_empty() != 0) {
      continue;
    }
    if (one_rinfo.range.is_full_access(dim_size)) {
      return true;
    }
  }
  return false;
}

void GetFlattenRangeFromNDRange(void *array, CMarsAST_IF *ast,
                                const vector<CMarsRangeExpr> &r_range,
                                const vector<CMarsRangeExpr> &w_range,
                                shared_ptr<vec_list_rinfo> r_range_set,
                                shared_ptr<vec_list_rinfo> w_range_set,
                                CMarsRangeExpr *r_flatten_range,
                                CMarsRangeExpr *w_flatten_range) {
  int dim = r_range.size();

  if (dim == 1) {
    *r_flatten_range = r_range[0];
    *w_flatten_range = w_range[0];
    return;
  }
  vector<size_t> sg_sizes;
  {
    void *sg_base_type;
    ast->get_type_dimension(ast->GetTypebyVar(array), &sg_base_type, &sg_sizes,
                            array);
  }
  assert((size_t)dim == sg_sizes.size());
  vector<int> accumate_sizes(dim, 1);
  for (int i = 1; i < dim; ++i) {
    accumate_sizes[i] = sg_sizes[i - 1] * accumate_sizes[i - 1];
  }
  bool full_read_access;
  bool full_write_access;
  full_read_access = full_write_access = true;

  bool single_read_access;
  bool single_write_access;
  single_read_access = single_write_access = true;
  *r_flatten_range = CMarsRangeExpr(0, 0, ast, 1);
  *w_flatten_range = CMarsRangeExpr(0, 0, ast, 1);
  bool r_range_complete;
  bool w_range_complete;
  r_range_complete = w_range_complete = false;
  for (int i = 0; i < dim; ++i) {
    int reverse_dim = dim - 1 - i;
    if (single_read_access) {
      if (!r_range[i].is_single_access()) {
        single_read_access = false;
      }
      CMarsRangeExpr flatten_range = r_range[i] * accumate_sizes[reverse_dim];
      flatten_range.set_exact(r_range[i].is_exact());
      *r_flatten_range = *r_flatten_range + flatten_range;
    } else {
      if (full_read_access &&
          !is_full_access(r_range_set, i, sg_sizes[reverse_dim])) {
        full_read_access = false;
      }
      if (!r_range_complete) {
        *r_flatten_range =
            *r_flatten_range +
            CMarsRangeExpr(0, accumate_sizes[reverse_dim + 1] - 1, ast, 1);
        r_range_complete = true;
      }
    }

    if (single_write_access) {
      if (!w_range[i].is_single_access()) {
        single_write_access = false;
      }
      CMarsRangeExpr flatten_range = w_range[i] * accumate_sizes[reverse_dim];
      flatten_range.set_exact(w_range[i].is_exact());
      *w_flatten_range = *w_flatten_range + flatten_range;
    } else {
      if (full_write_access &&
          !is_full_access(w_range_set, i, sg_sizes[reverse_dim])) {
        full_write_access = false;
      }
      if (!w_range_complete) {
        *w_flatten_range =
            *w_flatten_range +
            CMarsRangeExpr(0, accumate_sizes[reverse_dim + 1] - 1, ast, 1);
        w_range_complete = true;
      }
    }
  }
  if (!full_read_access) {
    r_flatten_range->set_exact(false);
  }
  if (!full_write_access) {
    w_flatten_range->set_exact(false);
  }
}

#if KEEP_UNUSED
//  input: [Notice] the original AST node, because def/use result is used in
//  analysis
void *
copy_and_induct_all_var_refs_in_scope(CMarsAST_IF *ast, void *ref_scope,
                                      void *boundary_scope,
                                      vector<void *> *inner_terms = nullptr) {
  map<void *, void *> map_var2exp;  //  original var_ref -> copied_exp (only
                                    //  contains outside var and iter var)
  if (!ast->analysis_induct_map_in_scope(ref_scope, boundary_scope,
                                         &map_var2exp, inner_terms))
    return nullptr;

  return ast->copy_and_replace_var_ref(ref_scope, map_var2exp);
}
#endif

int is_pntr_in_body(CMarsAST_IF *ast, void *pntr_stmt, void *loop_if) {
  if (ast->IsIfStatement(loop_if) != 0) {
    void *true_body = ast->GetIfStmtTrueBody(loop_if);
    void *false_body = ast->GetIfStmtFalseBody(loop_if);

    int in_true_body = (true_body) != nullptr
                           ? ast->is_located_in_scope(pntr_stmt, true_body)
                           : 0;
    int in_false_body = (false_body) != nullptr
                            ? ast->is_located_in_scope(pntr_stmt, false_body)
                            : 0;

    return static_cast<int>((in_true_body != 0) || (in_false_body != 0));
  }
  if (ast->IsLoopStatement(loop_if) != 0) {
    void *loop_body = ast->GetLoopBody(loop_if);
    int in_body = ast->is_located_in_scope(pntr_stmt, loop_body);
    return in_body;
  }
  if (ast->IsSwitchStatement(loop_if) != 0) {
    void *cond_stmt = ast->GetSwitchStmtSelector(loop_if);
    //  not in condition -> in the body
    return static_cast<int>(
        static_cast<int>(ast->is_located_in_scope(pntr_stmt, cond_stmt)) == 0);
  }

  assert(false);
  return 0;
}

//  1. Control statements
//  2. Expression inductable
//  3. Node interface
int CheckAccessSeparabilityInScope(CMarsAST_IF *ast, void *array,
                                   void *ref_scope, void *boundary_scope,
                                   CMerlinMessage *msg) {
  if (ast->GetVariableName(array) == "__PRETTY_FUNCTION__") {
    return 1;
  }

  //  assert(ast->IsFunctionDeclaration(ref_scope));
  string str_array = ast->_up(array);
  void *func_decl = ast->TraceUpToFuncDecl(ref_scope);
  string str_array_loc = getUserCodeFileLocation(ast, array, true);
  string str_func_loc = getUserCodeFileLocation(ast, func_decl, true);
  string str_func = ast->GetFuncName(func_decl);
  string str_array_info = "\'" + str_array + "\' " + str_array_loc;
  string str_func_info = "\'" + str_func + "\' " + str_func_loc;

  //  0. Get basic information from AST: pntrs and loops/ifs
  //  0.1 Find all the pntr references (accross functions)
  vector<void *> vec_pntr;
  {
    int allow_artifical_ref = 0;  //  this flag is only used for range analysis
    int ret = ast->GetPntrReferenceOfArray(array, ref_scope, &vec_pntr,
                                           allow_artifical_ref);
    if (ret == 0) {
      string str = "Reference parsing fails for variable " + str_array_info +
                   " in function " + str_func_info;
      if (msg != nullptr) {
        *msg = CMerlinMessage(-1, str);
      }
      return 0;
    }

    int access_times = 0;
    bool has_write = false;
    set<void *> set_callee;
    for (auto pntr : vec_pntr) {
      //  skip partial access as function call argument
      if (ast->GetFuncCallParamIndex(pntr) != -1 &&
          ast->get_pntr_init_dim(pntr) != ast->get_pntr_pntr_dim(pntr)) {
        void *call = ast->TraceUpToFuncCall(pntr);
        void *callee = ast->GetFuncDeclByCall(call);
        if (callee != nullptr) {
          if (set_callee.count(callee) > 0) {
            ++access_times;
          } else {
            set_callee.insert(callee);
          }
          continue;
        }
      }
      ++access_times;
      has_write |= ast->is_write_conservative(pntr);
    }
    if (has_write && access_times > 1) {
      string str = "Access to variable " + str_array_info +
                   " has possible data dependence in function " + str_func_info;
      if (msg != nullptr) {
        *msg = CMerlinMessage(-1, str);
      }
      return 0;
    }
  }

  //  {
  //    string str = "";
  //    for (auto pntr : vec_pntr) {
  //      vector<void *> indexes;
  //      ast->get_index_from_pntr_ref(pntr, &indexes);
  //      for (auto index : indexes) {
  //        CMarsExpression exp(index, ast, pntr);
  //        void *scope = ref_scope;
  //        if (ast->IsFunctionDeclaration(scope)) {
  //          scope = ast->GetFuncBody(ref_scope);
  //        }
  //        if (!exp.hasOnlyIteratorsInScope(scope)) {
  //          str += "Access for variable '" + str_array + "' in function '" +
  //                 str_func +
  //                 "' constains variables that are not loop iterators.\n";
  //          if (msg)
  //            *msg = CMerlinMessage(-1, str);
  //          return 0;
  //        }
  //      }
  //    }
  //  }

  //  0.2 Find all the loops and if-conditions (accross functions)
  map<void *, vector<void *>> map_pntr2loopif;
  for (auto pntr : vec_pntr) {
    string str_pntr = ast->_up(pntr);
    vector<void *> loop_or_if;

    void *pntr_stmt = ast->TraceUpToStatement(pntr);
    int ret = ast->get_surrounding_control_stmt(pntr_stmt, boundary_scope,
                                                &loop_or_if);
    if (ret == 0) {
      string str =
          "Can not get control statement list for variable reference '" +
          str_pntr + "' " + getUserCodeFileLocation(ast, pntr, true) +
          " in function " + str_func_info;
      if (msg != nullptr) {
        *msg = CMerlinMessage(-1, str);
      }
      return 0;
    }

    //  remove last level control if pntr is in the expression of the if/cond
    //  or loop body
    if (!loop_or_if.empty()) {
      void *last_layer = loop_or_if[loop_or_if.size() - 1];
      if (is_pntr_in_body(ast, pntr_stmt, last_layer) == 0) {
        loop_or_if.erase(loop_or_if.begin() + loop_or_if.size() - 1);
      }
    }

    map_pntr2loopif.insert(pair<void *, vector<void *>>(pntr, loop_or_if));
  }

  //  1. Control statement limitations
  //  1.1 no surrounding while/do_while
  //  1.2 no goto
  //  1.3 no direct break/continue in for-loop
  //  1.4 no surrounding switch
  //  1.5 no non-canonical loop
  //  1.6 no iterator assignment in the loop body
  //  1.7 no iterator used in a pntr outside loop
  //  1.8 else branch

  //  1.1,1.4,1.5 while/dowhile, switch, non-canonical for-loop
  {
    for (auto pntr : vec_pntr) {
      for (auto ctrl_stmt : map_pntr2loopif[pntr]) {
        string str_pntr = ast->_up(pntr);
        int is_hit = 0;
        string stmt_type;
        if (ast->IsWhileStatement(ctrl_stmt) != 0) {
          is_hit = 1;
          stmt_type = "while";
        } else if (ast->IsDoWhileStatement(ctrl_stmt) != 0) {
          is_hit = 1;
          stmt_type = "do-while";
        } else if (ast->IsSwitchStatement(ctrl_stmt) != 0) {
          is_hit = 1;
          stmt_type = "switch";
        }
        if (is_hit != 0) {
          string str = "Can not have " + stmt_type +
                       " statment surrounding variable reference '" + str_pntr +
                       "' " + getUserCodeFileLocation(ast, pntr, true) +
                       " in function " + str_func_info + " : " +
                       ast->_up(ctrl_stmt);
          if (msg != nullptr) {
            *msg = CMerlinMessage(-1, str);
          }
          return 0;
        }

        if (ast->IsForStatement(ctrl_stmt) != 0) {
          void *ivar;
          if (ast->IsCanonicalForLoop(ctrl_stmt, &ivar) == 0) {
            string str =
                "Can not have non-canonical loop surrounding variable "
                "reference '" +
                str_pntr + "' " + getUserCodeFileLocation(ast, pntr, true) +
                " in function " + str_func_info + " : " + ast->_up(ctrl_stmt);
            if (msg != nullptr) {
              *msg = CMerlinMessage(-1, str);
            }
            return 0;
          }
        }
      }
    }
  }
  //  1.2 no goto
  {
    vector<void *> v_goto;
    ast->GetNodesByType(nullptr, "preorder", "SgGotoStatement", &v_goto);
    for (auto goto_stmt : v_goto) {
      void *label_stmt = ast->GetGotoLabel(goto_stmt);

      if (ast->is_located_in_scope(goto_stmt, boundary_scope) != 0) {
        string stmt_type = "goto";
        string str = "Can not have " + stmt_type + " statment " +
                     getUserCodeFileLocation(ast, goto_stmt, true) +
                     " in function " + str_func_info;
        if (msg != nullptr) {
          *msg = CMerlinMessage(-1, str);
        }
        return 0;
      }
      if (ast->is_located_in_scope(label_stmt, boundary_scope) != 0) {
        string stmt_type = "goto";
        string str = "Can not have " + stmt_type + " statment " +
                     getUserCodeFileLocation(ast, goto_stmt, true) +
                     " whose target label is in function " + str_func_info;
        if (msg != nullptr) {
          *msg = CMerlinMessage(-1, str);
        }
        return 0;
      }
    }
  }
  //  1.3 no direct break/continue in for-loop
  {
    for (auto pntr : vec_pntr) {
      for (auto ctrl_stmt : map_pntr2loopif[pntr]) {
        string str_pntr = ast->_up(pntr);
        if (ast->IsForStatement(ctrl_stmt) != 0) {
          vector<void *> v_break;
          ast->GetNodesByType(nullptr, "preorder", "SgBreakStmt", &v_break);
          for (auto one_break : v_break) {
            if (ast->IsScopeBreak(one_break, ctrl_stmt)) {
              string stmt_type = "break";
              string str =
                  "Can not have " + stmt_type +
                  " statment in a loop surrounding variable reference '" +
                  str_pntr + "' " + getUserCodeFileLocation(ast, pntr, true) +
                  " in function " + str_func_info + " : " + ast->_up(one_break);
              if (msg != nullptr) {
                *msg = CMerlinMessage(-1, str);
              }
              return 0;
            }
          }

          vector<void *> v_cont;
          ast->GetNodesByType(nullptr, "preorder", "SgContinueStmt", &v_cont);
          for (auto one_cont : v_cont) {
            if (ast->TraceUpToForStatement(one_cont) == ctrl_stmt) {
              string stmt_type = "continue";
              string str =
                  "Can not have " + stmt_type +
                  " statment in a loop surrounding variable reference '" +
                  str_pntr + getUserCodeFileLocation(ast, pntr, true) +
                  " in function " + str_func_info + " : " + ast->_up(one_cont);
              if (msg != nullptr) {
                *msg = CMerlinMessage(-1, str);
              }
              return 0;
            }
          }
        }
      }
    }
  }

  //  1.6 no iterator assignment in the loop body
  {
    for (auto pntr : vec_pntr) {
      for (auto ctrl_stmt : map_pntr2loopif[pntr]) {
        if (ast->IsForStatement(ctrl_stmt) != 0) {
          void *loop = ctrl_stmt;
          void *iter = ast->GetLoopIterator(loop);
          void *body = ast->GetLoopBody(loop);
          string str_pntr = ast->_up(pntr);

          if (ast->has_write_in_scope(iter, body) != 0) {
            string stmt_type = "continue";
            string str =
                "Can not modify loop iterator in its loop body for "
                "variable reference '" +
                str_pntr + "' " + getUserCodeFileLocation(ast, pntr, true) +
                " in function '" + str_func_info + ", Loop: " + ast->_up(loop) +
                " " + getUserCodeFileLocation(ast, loop, true);
            if (msg != nullptr) {
              *msg = CMerlinMessage(-1, str);
            }
            return 0;
          }
        }
      }
    }
  }

  //  2. no iterator used in a pntr outside loop
  //  -> reduce to non-inductable variables
  {
    //  2.1 get all the related var refs for the loops/ifs, and pntrs
    set<void *> related_var_ref;
    for (auto pntr : vec_pntr) {
      for (auto ctrl_stmt : map_pntr2loopif[pntr]) {
        vector<void *> v_ref;
        ast->GetNodesByType(ctrl_stmt, "preorder", "SgVarRefExp", &v_ref);

        //  a. remove iterator ref
        //  b. remove refs in the body
        for (auto ref : v_ref) {
          if (ast->IsForStatement(ctrl_stmt) != 0) {
            if (ast->GetVariableInitializedName(ref) ==
                ast->GetLoopIterator(ctrl_stmt)) {
              continue;
            }
            void *body = ast->GetLoopBody(ctrl_stmt);
            if ((body != nullptr) &&
                (ast->is_located_in_scope(ref, body) != 0)) {
              continue;
            }
          } else if (ast->IsIfStatement(ctrl_stmt) != 0) {
            void *body = ast->GetIfStmtTrueBody(ctrl_stmt);
            if ((body != nullptr) &&
                (ast->is_located_in_scope(ref, body) != 0)) {
              continue;
            }
            body = ast->GetIfStmtFalseBody(ctrl_stmt);
            if ((body != nullptr) &&
                (ast->is_located_in_scope(ref, body) != 0)) {
              continue;
            }
          }

          bool found_in_pntr = false;
          for (auto pntr : vec_pntr) {
            void *array_ref;
            if ((ast->parse_array_ref_from_pntr(pntr, &array_ref, pntr) != 0) &&
                array_ref == ref) {
              found_in_pntr = true;
            }
          }
          if (found_in_pntr) {
            continue;
          }

          related_var_ref.insert(ref);
        }
      }

      vector<void *> indexes;
      ast->get_index_from_pntr_ref(pntr, &indexes);
      for (auto index : indexes) {
        vector<void *> v_ref;
        ast->GetNodesByType(index, "preorder", "SgVarRefExp", &v_ref);
        for (auto ref : v_ref) {
          related_var_ref.insert(ref);
        }
      }
    }
    //  2.2 check if for each ref, it can be expressed by boundary variable
    //  and loop iterators
    for (auto one_ref : related_var_ref) {
      map<void *, void *> map_induct_in_scope;
      vector<void *> v_terminal_refs;
      if ((ast->IsArrayType(ast->GetTypeByExp(one_ref)) != 0) ||
          (ast->IsPointerType(ast->GetTypeByExp(one_ref)) != 0)) {
        string str = "Cannot support indirect variable access for variable " +
                     str_array_info + " in function " + str_func_info + "";
        if (msg != nullptr) {
          *msg = CMerlinMessage(-1, str);
        }
        return 0;
      }
      if (ast->analysis_induct_map_in_scope(one_ref, boundary_scope,
                                            &map_induct_in_scope,
                                            &v_terminal_refs) == 0) {
        string str_one_ref = ast->_up(one_ref);
        string str_p = ast->_up(ast->_pa(one_ref));
        string str_ref = str_one_ref + "@" + str_p;

        string str = "Failed to induct variable " + str_array_info +
                     " reference " +
                     getUserCodeFileLocation(ast, one_ref, true) +
                     " in function " + str_func_info;
        if (msg != nullptr) {
          *msg = CMerlinMessage(-1, str);
        }
        return 0;
      }
      for (auto pair : map_induct_in_scope) {
        void *expr = pair.second;
        vector<void *> v_ref;
        ast->GetNodesByType_int(expr, "preorder", V_SgVarRefExp, &v_ref);
        for (auto ref : v_ref) {
          if ((ast->IsArrayType(ast->GetTypeByExp(ref)) != 0) ||
              (ast->IsPointerType(ast->GetTypeByExp(ref)) != 0)) {
            string str =
                "Cannot support indirect variable access for variable " +
                str_array_info + " in function " + str_func_info;
            if (msg != nullptr) {
              *msg = CMerlinMessage(-1, str);
            }
            return 0;
          }
        }
      }

      for (auto one_terminal_ref : v_terminal_refs) {
        //  a. skip boundary arguments
        //  The detailed test on the accesses of boundary argument is to be
        //  performed later
        //  void * var_name =
        //  ast->GetVariableInitializedName(one_terminal_ref); if (-1 !=
        //  ast->GetFuncParamIndex(var_name, boundary_scope))
        //  continue;
        void *decl = ast->GetVariableDecl(one_terminal_ref);
        void *boundary = boundary_scope;
        void *name = ast->GetVariableInitializedName(one_terminal_ref);
        if (ast->IsFunctionDeclaration(boundary) != 0)
          boundary = ast->GetFuncBody(boundary);

        //  ZP: 20170627
        if ((ast->is_located_in_scope(decl, boundary) == 0) &&
            (name == array || (ast->IsScalarType(name) != 0))) {
          continue;
        }

        cout << "-- Not expressible: " << ast->_up(one_terminal_ref) << " in "
             << ast->_p(ast->_pa(one_terminal_ref)) << endl;

        string str_ref = "'" + ast->_up(one_terminal_ref) + "' in '" +
                         ast->_up(ast->_pa(one_terminal_ref)) + "'";
        string str = "Variable " + str_array_info + " reference " +
                     getUserCodeFileLocation(ast, one_terminal_ref, true) +
                     +" in function " + str_func_info +
                     " cannot be expressed by boundary scalar " +
                     "variable of the stream scope or loop iterators within "
                     "the spawning scope\n";
        if (msg != nullptr) {
          *msg = CMerlinMessage(-1, str);
        }
        return 0;
      }
    }
  }

  //  3. Node interface
  //  3.1 not support both read and write
  {
    int has_write = ast->has_write_in_scope(array, ref_scope);
    int has_read = ast->has_read_in_scope(array, ref_scope);
    if ((has_write != 0) && (has_read != 0)) {
      string str = "Variable " + str_array_info + " in function " +
                   str_func_info + "\n" +
                   " cannot have both read and write accesses\n";
      if (msg != nullptr) {
        *msg = CMerlinMessage(-1, str);
      }
      return 0;
    }
  }

  return 1;
}

//  Description:
//    Generate a new stmt(or basicblock) to recover
// TODO(youxiang): vec_params is not used now
void *CopyAccessInScope(CMarsAST_IF *ast, void *array, void *ref_scope,
                        void *boundary_scope, int *is_write,
                        map<void *, void *> *map_spref2snref,
                        vector<void *> *vec_params) {
  DEFINE_START_END;
  *is_write = 0;

  //  1. Find all the references (accross functions)
  vector<void *> vec_access;
  int allow_artifical_ref = 0;  //  this flag is only used for range analysis
  STEMP(start);
  int ret = ast->GetPntrReferenceOfArray(array, ref_scope, &vec_access,
                                         allow_artifical_ref);
  TIMEP("CopyAccessInScope___GetPntrReferenceOfArray", start, end);
  if (ret == 0) {
    return nullptr;
  }

  STEMP(start);
  //  1.0 handling scalar
  {
    void *bb = ast->CreateBasicBlock();
    void *sg_type = ast->GetTypebyVar(array);
    if (ast->IsScalarType(sg_type) != 0) {
      *is_write = 0;
      int is_read = 0;

      for (auto ref : vec_access) {
        if (ast->is_write_conservative(ref) != 0) {
          *is_write = 1;
        }
      }

      if (!vec_access.empty() && *is_write == 0) {
        is_read = 1;
      }

      assert(!*is_write || !is_read);

      void *new_stmt = nullptr;
      void *var_ref = ast->CreateVariableRef(array);
      if (*is_write != 0) {
        new_stmt = ast->CreateStmt(
            V_SgExprStatement,
            ast->CreateExp(V_SgAssignOp, var_ref, ast->CreateConst(0)));
      } else {
        new_stmt = ast->CreateStmt(V_SgExprStatement, var_ref);
      }
      ast->AppendChild(bb, new_stmt);

      //  ZP: 20170609: since scalar refinement is not done in snode yet, we
      //  create the ref to init_name map for scalar here it is to be replaced
      //  outside by true references in CreateStreamPortFromNode()
      {
        if (map_spref2snref != nullptr) {
          map_spref2snref->insert(pair<void *, void *>(var_ref, array));
        }
      }

      TIMEP("CopyAccessInScope___1", start, end);
      return bb;
    }
  }
  TIMEP("CopyAccessInScope___1", start, end);

#if _DEBUG_COPYACCESS_
  {
    for (auto pntr : vec_access) {
      cout << "-- Find a ref: " << ast->_up(pntr) << endl;
    }
  }
#endif

  STEMP(start);
  //  2. Find all the loops and if-conditions (accross functions)
  map<void *, vector<void *>> map_ref2loop_or_if;  //  ref -> loops and ifs
  for (auto pntr : vec_access) {
    vector<void *> loop_or_if;

    void *pntr_stmt = ast->TraceUpToStatement(pntr);
    int ret = ast->get_surrounding_control_stmt(pntr_stmt, boundary_scope,
                                                &loop_or_if);
    if (ret == 0) {
      TIMEP("CopyAccessInScope___2", start, end);
      return nullptr;
    }

    //  remove last level control if pntr is in the expression of the if/cond
    //  or loop body
    if (!loop_or_if.empty()) {
      void *last_layer = loop_or_if[loop_or_if.size() - 1];
      if (is_pntr_in_body(ast, pntr_stmt, last_layer) == 0) {
        loop_or_if.erase(loop_or_if.begin() + loop_or_if.size() - 1);
      }
    }

    //  Only considering full dim access
    if (ast->get_pntr_init_dim(pntr) != ast->get_pntr_pntr_dim(pntr)) {
      continue;
    }

    auto one_pair = pair<void *, vector<void *>>(pntr, loop_or_if);
    map_ref2loop_or_if.insert(one_pair);
  }
  TIMEP("CopyAccessInScope___2", start, end);

  STEMP(start);
  //  2.1 get the pragmas outside the
  map<void *, vector<vector<void *>>> map_ref2pragmas;  //  ref -> loops and ifs
  for (auto it : map_ref2loop_or_if) {
    void *pntr = it.first;

    vector<vector<void *>> v_pragmas;  //  [layer][pragma]
    for (auto sg_layer : it.second) {
      vector<void *> pragmas;
      if (ast->IsForStatement(sg_layer) != 0) {
        for (auto child : ast->GetChildStmts(sg_layer)) {
          if (ast->IsPragmaDecl(child) != 0) {
            string pragma_str = ast->GetPragmaString(child);
            vector<string> split = str_split(pragma_str, " ");
            string pragma_key = split[0];
            boost::algorithm::to_upper(pragma_key);
            if (pragma_key == ACCEL_PRAGMA) {
              pragmas.push_back(child);
            }
          }
        }
      }
      v_pragmas.push_back(pragmas);
    }
    map_ref2pragmas.insert(
        pair<void *, vector<vector<void *>>>(pntr, v_pragmas));
  }
  TIMEP("CopyAccessInScope___2.1", start, end);

  STEMP(start);
  //  2.2 get whether true or false body for the if-statements
  map<void *, vector<int>> map_ref2ifbody;  //  ref -> vector<whether true body>
  for (auto it : map_ref2loop_or_if) {
    void *pntr = it.first;

    vector<int> v_ifbody;  //  [layer][pragma]
    for (auto sg_layer : it.second) {
      vector<void *> pragmas;
      if (ast->IsIfStatement(sg_layer) != 0) {
        void *true_body = ast->GetIfStmtTrueBody(sg_layer);
        void *false_body = ast->GetIfStmtFalseBody(sg_layer);

        int in_true_body = (true_body) != nullptr
                               ? ast->is_located_in_scope(pntr, true_body)
                               : 0;
        int in_false_body = (false_body) != nullptr
                                ? ast->is_located_in_scope(pntr, false_body)
                                : 0;

        assert((in_true_body && in_false_body) == 0);
        assert((in_true_body || in_false_body) == 1);

        v_ifbody.push_back(in_true_body);
      } else {
        v_ifbody.push_back(1);
      }
    }
    map_ref2ifbody[pntr] = v_ifbody;
  }
  TIMEP("CopyAccessInScope___2.2", start, end);

  STEMP(start);
  //  3.0 analysis the induction replacements in the scope
  map<void *, void *> map_induct_in_scope;
  vector<void *> v_inner_refs;
  map<void *, void *> map_newpntr2oldpntr;
  if (ast->analysis_induct_map_in_scope(ref_scope, boundary_scope,
                                        &map_induct_in_scope,
                                        &v_inner_refs) == 0) {
    assert(0);  // TODO(youxiang): handling the case that induct is not feasible
  }
#if _DEBUG_var_ref_intermediate
  for (auto one_induct : map_induct_in_scope) {
    cout << " -- induct " << ast->_p(one_induct.first) << ": "
         << ast->_p(one_induct.second) << endl;
  }
#endif
  ACCTM(CopyAccessInScope___3_0, start, end);

  STEMP(start);
  //  3. Refine all the var refs in the refs, loops and if-conditions
  //    Refine: induct the local variables into loop iters or boundary
  //    variable Replace: use the new loop iterators for the ref of old loop
  //    iterators
  // TODO(youxiang):
  //  Algorithm:
  //    a) Update from outer to inner
  //    b) Create new ifs and loops, and establish iter2iter map from outer to
  //    inner c) For each layer, first refine, and then replace d) The output
  //    (new ifs/loops) is stored in map_ref2loop_or_if_after_refine
  map<void *, vector<void *>>
      map_ref2loop_or_if_after_refine;  //  = map_ref2loop_or_if;
  map<void *, void *> map_ref2decl_pos;
  map<void *, int> map_iswrite;
  void *ret_stmt = ast->CreateBasicBlock();

  {
    CNameConflictSolver conflict_solver;

    for (auto it : map_ref2loop_or_if) {
      void *pntr = it.first;
      vector<void *> new_loop_or_if;

      void *ref =
          ast->CreateStmt(V_SgExprStatement, ast->CreateVariableRef(array));
      void *access_bb = ast->CreateBasicBlock();
      ast->AppendChild(access_bb, ref);
      ast->AppendChild(ret_stmt, access_bb);
      void *decl_insert_pos = ref;  //  used as a mark for adding declarations
      map<void *, void *>
          map_iter2expr;  //  old_iter -> new_iter_ref for each reference
      for (auto sg_layer : it.second) {
        //  void * new_layer = ast->CopyStmt(sg_layer);

        //  3.1. refine the var refs in the loop or if
        //  new_layer = copy_and_induct_all_var_refs_in_scope(ast, new_layer,
        //  boundary_scope); moved outside above

        int iter_decl_inside = 0;
        void *old_iter = nullptr;

        //  3.2.1 Create iter to iter map after loop copy
        if (ast->IsForStatement(sg_layer) != 0) {
          old_iter = ast->GetLoopIterator(sg_layer);
          assert(old_iter);
          void *new_iter = nullptr;
          if (ast->is_located_in_scope(old_iter,
                                       sg_layer) !=
              0) {  //  for (int i = 0; ...
            iter_decl_inside = 1;

            //  ///////////////////////////////////  /
            //            //  move this code later: iter_decl_inside
            //            new_iter = ast->GetLoopIterator(new_layer);
            //            map_iter2expr.insert(pair<void*,void*>(old_iter,
            //                  ast->CreateVariableRef(new_iter)));
            //  ///////////////////////////////////  /
          } else {
            static int index = 0;
            new_iter = ast->CreateVariableDecl(
                ast->GetTypeByString("int"),
                conflict_solver.get_and_assign(
                    "_" + ast->_up(old_iter) + "_" +
                    std::to_string(++index)),  //  add "_" prefix for the
                                               //  generated iterator
                //  conflict_solver.get_and_assign("_i"),
                nullptr, access_bb);
            ast->InsertStmt(new_iter, decl_insert_pos);
            map_iter2expr.insert(pair<void *, void *>(
                old_iter, ast->CreateVariableRef(new_iter)));
          }
        } else if (ast->IsIfStatement(sg_layer) != 0) {
        } else {
          // TODO(youxiang): support while/do_while loop, etc.
          assert(0);
        }

#if _DEBUG_var_ref_intermediate
        {
          vector<void *> v_ref;
          ast->GetNodesByType(sg_layer, "preorder", "SgVarRefExp", &v_ref);
          map<void *, void *> m_ref2var;
          for (auto ref : v_ref)
            m_ref2var[ref] = ast->GetVariableInitializedName(ref);
          cout << "sg_layer -- " << ast->_p(sg_layer) << ":" << endl;
          for (auto ref : v_ref)
            cout << "  " << ast->_p(ref) << "->" << ast->_p(m_ref2var[ref])
                 << endl;
          m_ref2var[nullptr] = nullptr;
        }
#endif

        //  3.2.2 replace the old ref with new iterator
        //  Note: apply induction first to enable more candidates to replace
        map<void *, void *> map_replace =
            map_induct_in_scope;  //  ref to expr map
        void *new_layer = ast->copy_and_replace_var_ref(sg_layer, map_replace);
        ast->get_ref_map_from_iter_map(new_layer, map_iter2expr, &map_replace);
        new_layer =
            ast->copy_and_replace_var_ref(new_layer, map_replace, nullptr, 1);

        //  moved from above: iter_decl_inside
        if (iter_decl_inside != 0) {
          void *new_iter = ast->GetLoopIterator(new_layer);
          assert(new_iter);
          map_iter2expr.insert(
              pair<void *, void *>(old_iter, ast->CreateVariableRef(new_iter)));
        }

        new_loop_or_if.push_back(new_layer);
      }

      int is_write = ast->is_write_conservative(pntr);

      //  Note: apply induction first to enable more candidates to replace
      map<void *, void *> map_replace =
          map_induct_in_scope;  //  ref to expr map
      void *new_pntr = ast->copy_and_replace_var_ref(pntr, map_replace);
      ast->get_ref_map_from_iter_map(new_pntr, map_iter2expr, &map_replace);
      new_pntr =
          ast->copy_and_replace_var_ref(new_pntr, map_replace, nullptr, 1);
      map_iswrite[new_pntr] = is_write;
      map_newpntr2oldpntr[new_pntr] = pntr;

      if (map_spref2snref != nullptr) {
        (*map_spref2snref)[new_pntr] = pntr;
      }

      map_ref2loop_or_if_after_refine.insert(
          pair<void *, vector<void *>>(new_pntr, new_loop_or_if));
      map_ref2decl_pos[new_pntr] = decl_insert_pos;
    }
  }
  ACCTM(CopyAccessInScope___3, start, end);

  STEMP(start);
  //  4. Generate the accesses and copy pragmas
  {
    for (auto it : map_ref2loop_or_if_after_refine) {
      void *pntr = it.first;
      void *new_stmt = nullptr;
      void *decl_insert_pos = map_ref2decl_pos[pntr];
      vector<vector<void *>> v_pragmas =
          map_ref2pragmas[map_newpntr2oldpntr[pntr]];
      vector<int> v_ifbody = map_ref2ifbody[map_newpntr2oldpntr[pntr]];
      if (1 == map_iswrite[pntr]) {  //  write
        *is_write = 1;

        new_stmt = ast->CreateStmt(
            V_SgExprStatement,
            ast->CreateExp(V_SgAssignOp, pntr, ast->CreateConst(0)));
      } else {  //  read
        new_stmt = ast->CreateStmt(V_SgExprStatement, (pntr));
      }

      vector<void *> control = it.second;
      assert(control.size() == v_pragmas.size());  //  both are layer number
      size_t i = control.size() - 1;
      for (auto one_ctrl : boost::adaptors::reverse(control)) {
        void *new_ctrl = one_ctrl;
        void *new_body = nullptr;
        vector<void *> pragmas = v_pragmas[i];
        int is_true_body = v_ifbody[i];
        if (ast->IsForStatement(new_ctrl) != 0) {
          new_body = ast->GetLoopBody(new_ctrl);
          //  ast->ReplaceStmt(ast->GetLoopBody(new_ctrl), new_stmt);
        } else if (ast->IsIfStatement(new_ctrl) != 0) {
#if 1
          //  ZP: 20170601 more concise code
          if (is_true_body == 0) {
            void *sg_cond = ast->GetIfStmtCondition(new_ctrl);
            void *sg_exp = ast->GetExprFromStmt(sg_cond);
            void *new_exp =
                ast->CreateExp(V_SgNotOp, sg_exp);  //  no copy, direct use
            //  ast->ReplaceExp(sg_exp, new_exp);
            new_ctrl =
                ast->CreateIfStmt(new_exp, ast->CreateBasicBlock(), nullptr);
          }
          new_body = ast->GetIfStmtTrueBody(new_ctrl);
          ast->SetIfStmtFalseBody(new_ctrl, nullptr);
#else
          if (is_true_body) {
            new_body = ast->GetIfStmtTrueBody(new_ctrl);

            if (ast->GetIfStmtFalseBody(new_ctrl))
              //  ast->SetIfStmtFalseBody(new_ctrl,
              //  ast->CreateStmt(V_SgNullStatement));
              ast->SetIfStmtFalseBody(new_ctrl, nullptr);
          } else {
            new_body = ast->GetIfStmtFalseBody(new_ctrl);

            if (ast->GetIfStmtTrueBody(new_ctrl))
              ast->SetIfStmtTrueBody(new_ctrl, ast->CreateBasicBlock());
          }
#endif

          //  ast->ReplaceStmt(ast->GetIfStmtTrueBody(new_ctrl), new_stmt);
        } else {
          assert(false);  // TODO(youxiang): check availability before
        }

        //  ZP: 20170601
        {
          void *bb = ast->CreateBasicBlock();
          void *bb1 = ast->CreateBasicBlock();
          ast->AppendChild(bb, bb1);
          ast->ReplaceStmt(new_body, bb);
          new_body = bb1;
        }

        ast->ReplaceStmt(new_body, new_stmt);

        for (auto one_pragma : pragmas) {
          //  ZP: 20170601
          //  ast->PrependChild(new_ctrl, ast->CopyChild(one_pragma));
          ast->InsertStmt(ast->CopyChild(one_pragma), new_stmt);
        }

        new_stmt = new_ctrl;
        i--;
      }
      ast->InsertStmt(new_stmt, decl_insert_pos);
      ast->RemoveChild(decl_insert_pos);
    }
  }
  TIMEP("CopyAccessInScope___4", start, end);

#if _DEBUG_var_ref_intermediate
  {
    vector<void *> v_ref;
    ast->GetNodesByType(ret_stmt, "preorder", "SgVarRefExp", &v_ref);
    map<void *, void *> m_ref2var;
    for (auto ref : v_ref)
      m_ref2var[ref] = ast->GetVariableInitializedName(ref);
    //  cout << "ret_stmt -- " << ast->_p(ret_stmt) << ":" << endl;
    cout << "ret_stmt -- " << endl;
    for (auto ref : v_ref)
      cout << "  " << ast->_p(ref) << "->" << ast->_p(m_ref2var[ref]) << endl;
    m_ref2var[nullptr] = nullptr;
  }
#endif

#if 0

  if (map_spref2snref) {
    cout << "-- ref_map:" << endl;
    for (auto iter : *map_spref2snref) {
      cout << "  " << ast->_p(iter.first) << "->" << ast->_p(iter.second)
        << endl;
    }
  }
#endif

  return ret_stmt;
}

bool ArrayRangeAnalysis::child_of(void *subnode, void *node) {
  return subnode != node && m_ast->IsInScope(subnode, node);
}

bool ArrayRangeAnalysis::child_of_if_true_body(void *node, void *ifstmt) {
  void *curr = node;
  while (curr && m_ast->GetParent(curr) != ifstmt) {
    curr = m_ast->GetParent(curr);
  }
  return curr == m_ast->GetIfStmtTrueBody(ifstmt);
}

void *ArrayRangeAnalysis::get_interesting_upper(void *node, void *range_scope) {
  void *prev = node;
  node = m_ast->GetParent(node);
  while (prev != range_scope) {
    if (m_ast->IsIfStatement(node)) {
      if (prev == m_ast->GetIfStmtCondition(node)) {
        if (node == range_scope) {
          return nullptr;
        }
      } else {
        return node;
      }
    } else if (m_ast->IsLoopStatement(node)) {
      if (prev == m_ast->GetLoopInit(node) ||
          prev == m_ast->GetLoopTest(node)) {
        if (node == range_scope) {
          return nullptr;
        }
      } else {
        return node;
      }
    } else if (m_ast->IsFunctionDeclaration(node)) {
      return node;
    } else if (m_ast->IsSwitchStatement(node)) {
      if (prev == m_ast->GetSwitchStmtSelector(node)) {
        if (node == range_scope) {
          return nullptr;
        }
      } else {
        return node;
      }
    }
    prev = node;
    node = m_ast->GetParent(node);
  }
  return range_scope;
}

shared_ptr<MyAST>
ArrayRangeAnalysis::create_tree(void *range_scope,
                                const map2vec_list_rinfo &range_for) {
  unordered_map<void *, shared_ptr<MyAST>> cache;

  auto append_in_tree = [&](void *upper, void *curr) {
    auto utree = cache[upper];
    auto ctree = cache[curr];
    if (m_ast->IsLoopStatement(upper)) {
      utree->body.push_back(ctree);
    } else if (m_ast->IsIfStatement(upper)) {
      if (child_of_if_true_body(curr, upper)) {
        utree->body.push_back(ctree);
      } else {
        utree->orelse.push_back(ctree);
      }
    } else if (m_ast->IsFunctionDeclaration(upper)) {
      utree->body.push_back(ctree);
    } else if (m_ast->IsSwitchStatement(upper)) {
      utree->body.push_back(ctree);
    } else {
      utree->body.push_back(ctree);
    }
  };

  auto create_and_append_in_tree = [&](void *upper, void *curr) {
    auto ctree = cache[curr];
    if (m_ast->IsLoopStatement(upper)) {
      cache[upper] = std::make_shared<MyAST>("loop", upper);
      cache[upper]->body.push_back(ctree);
      int64_t tc, ub;
      if (m_ast->get_loop_trip_count(upper, &tc, &ub)) {
        cache[upper]->trip_count = ub;
      } else {
        cache[upper]->trip_count = std::numeric_limits<size_t>::max();
      }
    } else if (m_ast->IsIfStatement(upper)) {
      cache[upper] = std::make_shared<MyAST>("if", upper);
      if (child_of_if_true_body(curr, upper)) {
        cache[upper]->body.push_back(ctree);
        if (cache[upper]->true_branch_is_exact_condition == -1) {
          cache[upper]->true_branch_is_exact_condition = is_exact_condition(
              m_ast, m_ast->GetIfStmtCondition(upper), range_scope, true);
          cache[upper]->true_body = m_ast->GetIfStmtTrueBody(upper);
        }
      } else {
        cache[upper]->orelse.push_back(ctree);
        if (cache[upper]->false_branch_is_exact_condition == -1) {
          cache[upper]->false_branch_is_exact_condition = is_exact_condition(
              m_ast, m_ast->GetIfStmtCondition(upper), range_scope, false);
          cache[upper]->false_body = m_ast->GetIfStmtFalseBody(upper);
        }
      }
    } else if (m_ast->IsFunctionDeclaration(upper)) {
      cache[upper] = std::make_shared<MyAST>("function", upper);
      cache[upper]->body.push_back(ctree);
    } else if (m_ast->IsSwitchStatement(upper)) {
      cache[upper] = std::make_shared<MyAST>("switch", upper);
      cache[upper]->body.push_back(ctree);
    } else {
      // The root node of create_tree() can be SgNode other than above types
      // E.g. SgBasicBlock, SgExprStatement
      cache[upper] = std::make_shared<MyAST>("unknown", upper);
      cache[upper]->body.push_back(ctree);
    }
  };

  cache[nullptr] = std::make_shared<MyAST>("root", nullptr);
  for (auto kv : range_for) {
    void *ref = kv.first;
    cache[ref] = std::make_shared<MyAST>("ref", ref,
                                         std::make_shared<Result>(kv.second));
  }

  for (auto kv : range_for) {
    void *curr = kv.first;
    void *upper = get_interesting_upper(curr, range_scope);
    if (upper == nullptr) {
      append_in_tree(nullptr, curr);
    } else {
      while (curr != range_scope) {
        if (cache.count(upper) > 0) {
          append_in_tree(upper, curr);
          break;
        } else {
          create_and_append_in_tree(upper, curr);
        }
        curr = upper;
        upper = get_interesting_upper(curr, range_scope);
      }
      if (curr == range_scope) {
        append_in_tree(nullptr, curr);
      }
    }
  }
  return cache[nullptr];
}

shared_ptr<Result> ArrayRangeAnalysis::postorder(shared_ptr<MyAST> tree,
                                                 const void *range_scope,
                                                 int dim_size) {
  if (tree->type == "ref") {
    return tree->result;
  } else if (tree->type == "function") {
    auto r = std::make_shared<Result>(dim_size);
    for (auto &ptr : tree->body) {
      auto r2 = postorder(ptr, range_scope, dim_size);
      r->merge(r2);
    }
    for (int dim = 0; dim < dim_size; ++dim) {
      union_group_range_exact(r->vl_rinfo, range_scope, tree->sgnode, dim);
    }
    return r;
  } else if (tree->type == "loop") {
    auto r = std::make_shared<Result>(dim_size);
    for (auto &ptr : tree->body) {
      auto r2 = postorder(ptr, range_scope, dim_size);
      r->merge(r2);
    }
    for (int dim = 0; dim < dim_size; ++dim) {
      union_group_range_exact(r->vl_rinfo, range_scope, tree->sgnode, dim,
                              tree->trip_count);
    }
    // 1. For non-canonical loop, set each Range to inexact
    // 2. For canonical loop, if trip count unknown, set each Range to inexact
    //    E.g. for (i = 0; i < m; i++) ...
    check_exact_flag_for_loop(tree->sgnode, range_scope, r->vl_rinfo);
    return r;
  } else if (tree->type == "switch") {
    auto r = std::make_shared<Result>(dim_size);
    for (auto &ptr : tree->body) {
      auto r2 = postorder(ptr, range_scope, dim_size);
      r->merge(r2);
    }
    for (int dim = 0; dim < dim_size; ++dim) {
      // TODO(youxiang) enhance switch support. Currently, we will consertively
      // set into
      // non exact
      set_non_exact_flag(r->vl_rinfo, dim);
      union_group_range_exact(r->vl_rinfo, range_scope, tree->sgnode, dim);
    }
    return r;
  } else if (tree->type == "if") {
    auto rtrue = std::make_shared<Result>(dim_size);
    auto rfalse = std::make_shared<Result>(dim_size);
    for (auto &ptr : tree->body) {
      auto r2 = postorder(ptr, range_scope, dim_size);
      rtrue->merge(r2);
    }
    for (int dim = 0; dim < dim_size; ++dim) {
      union_group_range_exact(rtrue->vl_rinfo, range_scope, tree->true_body,
                              dim);
    }
    for (auto &ptr : tree->orelse) {
      auto r2 = postorder(ptr, range_scope, dim_size);
      rfalse->merge(r2);
    }
    for (int dim = 0; dim < dim_size; ++dim) {
      union_group_range_exact(rfalse->vl_rinfo, range_scope, tree->false_body,
                              dim);
    }

    for (int dim = 0; dim < dim_size; ++dim) {
      if (equal_range(rtrue->vl_rinfo->at(dim), rfalse->vl_rinfo->at(dim))) {
        union_group_range_exact(rtrue->vl_rinfo, range_scope, tree->sgnode,
                                dim);
        // FIXME: lift the position out of if-statement but it may
        // lose inner condition
        for (auto &one_range : rtrue->vl_rinfo->at(dim)) {
          one_range.sgnode = tree->sgnode;
          one_range.range.set_pos(const_cast<void *>(tree->sgnode));
        }
      } else {
        if (tree->true_branch_is_exact_condition == 0 ||
            tree->false_branch_is_exact_condition == 0) {
          rtrue->extend_1d(rfalse, dim);
          // set each range to inexact
          set_non_exact_flag(rtrue->vl_rinfo, dim);
          union_group_range_exact(rtrue->vl_rinfo, range_scope, tree->sgnode,
                                  dim);
        } else {
          rtrue->extend_1d(rfalse, dim);
          union_group_range_exact(rtrue->vl_rinfo, range_scope, tree->sgnode,
                                  dim);
        }
      }
    }
    return rtrue;
  } else {
    // The range_scope node of create_tree() can be SgNode other than above
    // types E.g. SgBasicBlock, SgExprStatement
    auto r = std::make_shared<Result>(dim_size);
    for (auto &ptr : tree->body) {
      auto r2 = postorder(ptr, range_scope, dim_size);
      r->merge(r2);
    }
    for (int dim = 0; dim < dim_size; ++dim) {
      union_group_range_exact(r->vl_rinfo, range_scope, tree->sgnode, dim);
    }
    return r;
  }
}

range ArrayRangeAnalysis::try_merge(shared_ptr<list_range> ranges) {
  if (ranges->empty()) {
    range res(m_ast);
    res.set_empty();
    return res;
  }
  auto it = ranges->begin();
  range final_range = *it;
  it++;
  while (it != ranges->end()) {
    final_range = final_range.Union(*it++);
  }
  return final_range;
}

size_t ArrayRangeAnalysis::merge_access_size(size_t one_access_size,
                                             size_t other_access_size) {
  if (numeric_limits<size_t>::max() - one_access_size > other_access_size)
    return one_access_size + other_access_size;
  return numeric_limits<size_t>::max();
}

void ArrayRangeAnalysis::try_merge(shared_ptr<vec_list_rinfo> ranges, int dim) {
  if (ranges->at(dim).size() == 1) {
    return;
  }
  // merge *it, *it2 on condition 'require_exact' into 'merged'
  // if success, ranges.erase(it), ranges.erase(it2);
  //             ranges.push_front(merged)
  //             then try_merge(ranges, require_exact)
  auto &one_list_range = ranges->at(dim);
  if (one_list_range.empty())
    return;
  auto it = one_list_range.begin();
  while (it != one_list_range.end()) {
    auto it2 = it;
    ++it2;
    range curr_range = (*it).range;
    auto curr_pos = (*it).sgnode;
    auto curr_access_size = (*it).acc_size;
    bool has_merge = false;
    while (it2 != one_list_range.end()) {
      range &other_range = (*it2).range;
      auto other_pos = (*it2).sgnode;
      size_t access_size = (*it2).acc_size;
      if (m_ast->InSameUnconditionalScope(const_cast<void *>(curr_pos),
                                          const_cast<void *>(other_pos))) {
        if (curr_range.is_exact() || other_range.is_exact()) {
          // try exact merge
          range tmp = curr_range.Union(other_range);
          if (tmp.is_exact() == 1) {
            curr_range = tmp;
            curr_pos = m_ast->GetCommonPosition(const_cast<void *>(curr_pos),
                                                const_cast<void *>(other_pos));
            has_merge = true;
            if (m_eval_access_size)
              curr_access_size =
                  merge_access_size(curr_access_size, access_size);
            it2 = one_list_range.erase(it2);
            continue;
          }
        } else {
          // try non exact merge (completely duplicated)
          if (curr_range == other_range) {
            curr_pos = m_ast->GetCommonPosition(const_cast<void *>(curr_pos),
                                                const_cast<void *>(other_pos));
            if (m_eval_access_size)
              curr_access_size =
                  merge_access_size(curr_access_size, access_size);
            it2 = one_list_range.erase(it2);
            continue;
          }
        }
      }

      it2++;
    }
    if (has_merge) {
      (*it).sgnode = curr_pos;
      curr_range.set_pos(const_cast<void *>(curr_pos));
      (*it).range = curr_range;
      (*it).acc_size = curr_access_size;
    }
    it++;
  }
}

void ArrayRangeAnalysis::union_group_range_exact(
    shared_ptr<vec_list_rinfo> rinfos, const void *range_scope,
    const void *upper, int dim, size_t upper_trip_count) {
  try_merge(rinfos, dim);
  if (upper != nullptr) {
    // in sub tree, apply exact refine, search just one level up for symbol
    // value, then
    for (auto &one_range : rinfos->at(dim)) {
      if (m_eval_access_size && one_range.acc_size != 0) {
        if (one_range.acc_size != numeric_limits<size_t>::max() &&
            upper_trip_count <
                numeric_limits<size_t>::max() / one_range.acc_size) {
          one_range.acc_size *= upper_trip_count;
        } else {
          one_range.acc_size = numeric_limits<size_t>::max();
        }
      }
      if (one_range.range.is_exact()) {
        one_range.range.refine_range_in_scope_v2_exact(
            const_cast<void *>(upper), const_cast<void *>(one_range.sgnode),
            one_range.func_path);
      } else {
        one_range.range.refine_range_in_scope_v2(
            const_cast<void *>(upper), const_cast<void *>(one_range.sgnode),
            one_range.func_path);
      }
      if (m_ast->IsBasicBlock(const_cast<void *>(upper)) == 0) {
        bool should_lift_position = false;
        if (!should_lift_position) {
          vector<void *> vec_vars;
          one_range.range.get_vars(&vec_vars);
          bool contains_loop_iterator = false;
          for (auto var : vec_vars) {
            if (m_ast->IsForStatement(var) &&
                m_ast->IsInScope(var, const_cast<void *>(range_scope))) {
              contains_loop_iterator = true;
              break;
            }
          }
          if (!contains_loop_iterator) {
            should_lift_position = true;
          }
        }
        if (should_lift_position) {
          one_range.sgnode = upper;
        }
      }
    }
    try_merge(rinfos, dim);
  }
}

range ArrayRangeAnalysis::union_group_range(shared_ptr<vec_list_rinfo> rinfos,
                                            const void *range_scope, int dim) {
  // in root tree, apply non exact refine
  auto ranges = std::make_shared<list_range>();
  for (auto &one_range : rinfos->at(dim)) {
    one_range.range.refine_range_in_scope_v2(
        const_cast<void *>(range_scope), const_cast<void *>(one_range.sgnode),
        one_range.func_path);
    ranges->push_back(one_range.range);
  }
  return try_merge(ranges);
}

// youxiang, be sure to do just *exact* merge
shared_ptr<Result>
ArrayRangeAnalysis::Union(const map2vec_list_rinfo &range_for,
                          void *range_scope, int dim_size) {
  if (range_for.empty()) {
    return std::make_shared<Result>(dim_size);
  } else {
    auto tree = create_tree(range_scope, range_for);
    return postorder(tree, range_scope, dim_size);
  }
}

std::pair<std::shared_ptr<Result>, std::shared_ptr<Result>>
ArrayRangeAnalysis::DFS_analyze(void *range_scope, void *array,
                                void *ref_scope) {
  if (m_cache.count({range_scope, array}) > 0) {
    return m_cache[{range_scope, array}];
  }
  m_cache[{range_scope, array}] = {};  // mark visited
  size_t dim;
  int data_unit_size;
  int dim_fake;
  bool all_dimension_size_known;
  size_t total_flatten_size;
  vector<size_t> sg_sizes;
  get_array_dimension_size(array, &dim, &data_unit_size, &dim_fake,
                           &all_dimension_size_known, &total_flatten_size,
                           &sg_sizes);
  size_t edim = (dim > 0 ? dim : dim_fake);
  void *func_of_array = m_ast->TraceUpToFuncDecl(array);
  map2vec_list_rinfo base_ranges_read;
  map2vec_list_rinfo base_ranges_write;
  if (m_ast->TraceUpToFuncDecl(range_scope) != func_of_array) {
    vector<void *> calls;
    m_ast->GetNodesByType_int(range_scope, "postorder", V_SgFunctionCallExp,
                              &calls);
    for (auto call : calls) {
      void *func = m_ast->GetFuncDeclByCall(call);
      if (func == nullptr)
        continue;
      auto func_name = m_ast->GetFuncName(func);
      if (func_name.find("memcpy_wide_bus_") == 0)
        continue;
      DFS_analyze(func, array, ref_scope);

      auto new_read_range = std::make_shared<vec_list_rinfo>();
      if (m_cache[{func, array}].first != nullptr) {
        *new_read_range = *m_cache[{func, array}].first->vl_rinfo;
      } else {
        new_read_range->resize(edim);
      }
      auto new_write_range = std::make_shared<vec_list_rinfo>();
      if (m_cache[{func, array}].second != nullptr) {
        *new_write_range = *m_cache[{func, array}].second->vl_rinfo;
      } else {
        new_write_range->resize(edim);
      }
      // do necessary replacement with call argument
      substitute_parameter_atom_in_range(array, new_read_range, new_write_range,
                                         func, call, range_scope);
      // concat the call path
      for (auto &one_dim_rinfo : *new_read_range) {
        for (auto &one_rinfo : one_dim_rinfo) {
          if (!one_rinfo.func_path.empty())
            one_rinfo.func_path.pop_back();
          one_rinfo.func_path.emplace_back(func, call);
        }
      }
      for (auto &one_dim_rinfo : *new_write_range) {
        for (auto &one_rinfo : one_dim_rinfo) {
          if (!one_rinfo.func_path.empty())
            one_rinfo.func_path.pop_back();
          one_rinfo.func_path.emplace_back(func, call);
        }
      }
      // initialize the call path
      for (auto &one_dim_rinfo : *new_read_range) {
        for (auto &one_rinfo : one_dim_rinfo) {
          one_rinfo.func_path.emplace_back(range_scope, nullptr);
        }
      }
      for (auto &one_dim_rinfo : *new_write_range) {
        for (auto &one_rinfo : one_dim_rinfo) {
          one_rinfo.func_path.emplace_back(range_scope, nullptr);
        }
      }
      base_ranges_read[call] = new_read_range;
      base_ranges_write[call] = new_write_range;
    }
  }
  // for global variables, its references may appear everywere
  vector<void *> refs = m_ast->GetAllRefInScope(array, range_scope);
  for (auto &ref : refs) {
    auto final_read_range = std::make_shared<vec_list_rinfo>(edim);
    auto final_write_range = std::make_shared<vec_list_rinfo>(edim);
    void *new_scope = nullptr;
    void *new_range_scope = nullptr;
    void *new_array = nullptr;
    void *new_func_call = nullptr;
    int new_pointer_dim = 0;
    analyze_ref(ref, range_scope, ref_scope, dim, all_dimension_size_known,
                total_flatten_size, data_unit_size, sg_sizes, final_read_range,
                final_write_range, &new_scope, &new_range_scope, &new_array,
                &new_func_call, &new_pointer_dim);
    if (new_array != nullptr) {
      DFS_analyze(new_range_scope, new_array, new_scope);
      auto new_read_range = std::make_shared<vec_list_rinfo>();
      if (m_cache[{new_range_scope, new_array}].first != nullptr) {
        *new_read_range =
            *m_cache[{new_range_scope, new_array}].first->vl_rinfo;
      } else if (dim - new_pointer_dim > 0) {
        new_read_range->resize(dim - new_pointer_dim);
      }
      auto new_write_range = std::make_shared<vec_list_rinfo>();
      if (m_cache[{new_range_scope, new_array}].second != nullptr) {
        *new_write_range =
            *m_cache[{new_range_scope, new_array}].second->vl_rinfo;
      } else if (dim - new_pointer_dim > 0) {
        new_write_range->resize(dim - new_pointer_dim);
      }
      if (new_func_call != nullptr) {
        // do necessary replacement with call argument
        substitute_parameter_atom_in_range(new_array, new_read_range,
                                           new_write_range, new_range_scope,
                                           new_func_call, range_scope);
        // concat the call path
        for (auto &one_dim_rinfo : *new_read_range) {
          for (auto &one_rinfo : one_dim_rinfo) {
            if (!one_rinfo.func_path.empty())
              one_rinfo.func_path.pop_back();
            one_rinfo.func_path.emplace_back(new_range_scope, new_func_call);
          }
        }
        for (auto &one_dim_rinfo : *new_write_range) {
          for (auto &one_rinfo : one_dim_rinfo) {
            if (!one_rinfo.func_path.empty())
              one_rinfo.func_path.pop_back();
            one_rinfo.func_path.emplace_back(new_range_scope, new_func_call);
          }
        }
      }
      propagate_range(ref, range_scope, *new_read_range, *new_write_range, dim,
                      new_pointer_dim, final_read_range, final_write_range);
    }
    // initialize the call path
    for (auto &one_dim_rinfo : *final_read_range) {
      for (auto &one_rinfo : one_dim_rinfo) {
        if (one_rinfo.func_path.empty())
          one_rinfo.func_path.emplace_back(nullptr, ref);
        one_rinfo.func_path.emplace_back(range_scope, nullptr);
      }
    }
    for (auto &one_dim_rinfo : *final_write_range) {
      for (auto &one_rinfo : one_dim_rinfo) {
        if (one_rinfo.func_path.empty())
          one_rinfo.func_path.emplace_back(nullptr, ref);
        one_rinfo.func_path.emplace_back(range_scope, nullptr);
      }
    }

    base_ranges_read[ref] = final_read_range;
    base_ranges_write[ref] = final_write_range;
  }
  auto res_r = Union(base_ranges_read, range_scope, edim);
  auto res_w = Union(base_ranges_write, range_scope, edim);
  m_cache[{range_scope, array}] = {res_r, res_w};
  return {res_r, res_w};
}

string ArrayRangeAnalysis::print_s() {
  size_t i;
  string str;

  size_t dim = m_range_w.size();
  assert(dim == m_range_r.size());

  if (has_read() != 0) {
    str += " R";
    for (i = 0; i < dim; i++) {
      str += "[" + m_range_r[i].print_e() + "]";
    }
  }

  if (has_write() != 0) {
    str += " W";
    for (i = 0; i < dim; i++) {
      str += "[" + m_range_w[i].print_e() + "]";
    }
  }

  return str;
}

int ArrayRangeAnalysis::has_read() {
  int dim = m_range_r.size();

  for (int i = 0; i < dim; i++) {
    if (m_range_r[i].is_empty() != 0) {
      return 0;
    }
  }

  return 1;
}

int ArrayRangeAnalysis::has_write() {
  int dim = m_range_w.size();

  for (int i = 0; i < dim; i++) {
    if (m_range_w[i].is_empty() != 0) {
      return 0;
    }
  }

  return 1;
}

void ArrayRangeAnalysis::analyze(void *array, void *range_scope,
                                 void *ref_scope) {
  if (m_ast->IsFunctionDeclaration(range_scope)) {
    range_scope = m_ast->GetFuncBody(range_scope);
  }
  if (m_ast->IsProject(ref_scope) || ref_scope == nullptr) {
    if (void *scope = m_ast->TraceUpToBasicBlock(array)) {
      ref_scope = scope;
    } else if (void *scope = m_ast->TraceUpToFuncDecl(array)) {
      ref_scope = scope;
    }
  }
  shared_ptr<Result> read_result = nullptr;
  shared_ptr<Result> write_result = nullptr;
  if (m_ast->IsProject(range_scope) || range_scope == nullptr) {
    CMarsIrV2 mars_ir;
    mars_ir.build_mars_ir(m_ast, m_ast->GetProject());
    for (auto kernel : mars_ir.get_top_kernels()) {
      auto pa = DFS_analyze(kernel, array, ref_scope);
      if (read_result)
        read_result->merge(pa.first);
      else
        read_result = pa.first;
      if (write_result)
        write_result->merge(pa.second);
      else
        write_result = pa.second;
    }
  } else {
    auto pa = DFS_analyze(range_scope, array, ref_scope);
    read_result = pa.first;
    write_result = pa.second;
  }
  size_t m_read_access_size = 0;
  if (read_result) {
    for (size_t dim = 0; dim < read_result->vl_rinfo->size(); ++dim) {
      m_range_r.push_back(
          union_group_range(read_result->vl_rinfo, range_scope, dim));
      if (m_eval_access_size) {
        size_t curr_access_size = 0;
        for (auto one_rinfo : read_result->vl_rinfo->at(dim)) {
          curr_access_size =
              merge_access_size(one_rinfo.acc_size, curr_access_size);
        }
        m_read_access_size = max(m_read_access_size, curr_access_size);
      }
    }
  }

  size_t m_write_access_size = 0;
  if (write_result) {
    for (size_t dim = 0; dim < write_result->vl_rinfo->size(); ++dim) {
      m_range_w.push_back(
          union_group_range(write_result->vl_rinfo, range_scope, dim));
      if (m_eval_access_size) {
        size_t curr_access_size = 0;
        for (auto one_rinfo : write_result->vl_rinfo->at(dim)) {
          curr_access_size =
              merge_access_size(one_rinfo.acc_size, curr_access_size);
        }
        m_write_access_size = max(m_write_access_size, curr_access_size);
      }
    }
  }
  m_access_size = max(m_read_access_size, m_write_access_size);
  intersect_with_access_range_info(array, ref_scope, range_scope);

  if (read_result != nullptr && write_result != nullptr) {
    // get flatten range
    GetFlattenRangeFromNDRange(array, m_ast, m_range_r, m_range_w,
                               read_result->vl_rinfo, write_result->vl_rinfo,
                               &m_flatten_range_r, &m_flatten_range_w);
  }
}

void ArrayRangeAnalysis::analyze_ref(
    void *curr_ref, void *range_scope, void *ref_scope, int dim,
    bool all_dimension_size_known, size_t total_flatten_size,
    int data_unit_size, const vector<size_t> &sg_sizes,
    shared_ptr<vec_list_rinfo> r_range, shared_ptr<vec_list_rinfo> w_range,
    void **new_scope, void **new_range_scope, void **new_array,
    void **new_func_call, int *new_pointer_dim) {
  void *array;
  void *sg_pntr;
  vector<void *> sg_indexes;
  int pointer_dim;
  m_ast->parse_pntr_ref_by_array_ref(curr_ref, &array, &sg_pntr, &sg_indexes,
                                     &pointer_dim);
  CMarsAccess ac(sg_pntr, m_ast, range_scope);
  bool is_in_ref_scope = m_ast->IsInScope(curr_ref, ref_scope);
  *new_pointer_dim = -1;
  //  1. handling the accesses
  //  1.1 element reference
  //  1.2 argument (with offset)
  //  1.3 full assignment (with offset)
  //  1.4 partial assignment (with offset) [ZP: supported, 20150821]
  //  1.5 memcpy (with offset)             [ZP: supported, 20150821]
  //  1.6 complex than assignment with offset => full range

  CMarsRangeExpr infinite(m_ast);
  if (static_cast<size_t>(pointer_dim) == dim) {
    //  1. full pntr
    //  /////////////////////////////////////////  /
    //  ZP: 20170531
    //  fixing for scalar
    if (dim == 0 && is_in_ref_scope) {
      CMarsRangeExpr mr(CMarsExpression(m_ast, 0), CMarsExpression(m_ast, 0));
      if (m_ast->has_write_conservative(sg_pntr) != 0) {
        (*w_range)[0].push_back(RangeInfo(sg_pntr, mr));
      }
      if (m_ast->has_read_conservative(sg_pntr) != 0) {
        (*r_range)[0].push_back(RangeInfo(sg_pntr, mr));
      }
    }
    //  /////////////////////////////////////////  /
    bool isRead = true;
    {
      int arg_idx = m_ast->GetFuncCallParamIndex(sg_pntr);
      if (-1 != arg_idx && dim != 0) {
        void *func_call = m_ast->TraceUpToFuncCall(sg_pntr);
        assert(m_ast->IsFunctionCall(func_call));
        void *func_decl = m_ast->GetFuncDeclByCall(func_call);
        if (m_ast->IsMerlinInternalIntrinsic(
                m_ast->GetFuncNameByCall(func_call, false))) {
          goto SKIP_FULL_PNTR_FUNC;
        }
        if (m_ast->IsFunctionDeclaration(func_decl) != 0) {
          void *func_para = m_ast->GetFuncParam(func_decl, arg_idx);
          void *func_para_type = m_ast->GetTypebyVar(func_para);
          if (m_ast->IsReferenceType(func_para_type) == 0) {
            goto SKIP_FULL_PNTR_FUNC;
          }
          vector<void *> vec_nodes;
          isRead = false;
          for (auto ref : m_ast->GetAllRefInScope(func_para, func_decl)) {
            if (m_ast->IsAddressOfOp(m_ast->GetParent(ref)) == 0) {
              isRead = true;
              continue;
            }
            void *parent = m_ast->GetParent(m_ast->GetParent(ref));
            void *var_name;
            void *value_exp;
            if (m_ast->parse_assign(parent, &var_name, &value_exp) == 0) {
              goto ERROR_OUT;
            }
            vec_nodes.push_back(var_name);
          }

          if (vec_nodes.empty()) {
            goto SKIP_FULL_PNTR_FUNC;
          } else if (vec_nodes.size() > 1) {
            goto ERROR_OUT;
          }
          *new_array = vec_nodes[0];
          *new_pointer_dim = pointer_dim - 1;
          *new_scope = is_in_ref_scope ? func_decl : ref_scope;
          *new_func_call = func_call;
          *new_range_scope = func_decl;
        } else {
          //  undefined system call
        }
      }
    SKIP_FULL_PNTR_FUNC : {}
    }
    if (is_in_ref_scope) {
      for (size_t j = 0; j < dim; j++) {
        CMarsRangeExpr mr(ac.GetIndex(j), ac.GetIndex(j));
        if (m_ast->has_write_conservative(sg_pntr) != 0) {
          (*w_range)[j].push_back(RangeInfo(sg_pntr, mr));
        }
        if ((m_ast->has_read_conservative(sg_pntr) != 0) && isRead) {
          (*r_range)[j].push_back(RangeInfo(sg_pntr, mr));
        }
      }
    }
    return;
  } else if (pointer_dim != -1) {
    //  2. pointer assignement or function argument
    //  2.0 point assignment lhs, e.g. int * array = ...;
    void *var_name;
    void *value_exp;
    int ret =
        m_ast->parse_assign(m_ast->GetParent(sg_pntr), &var_name, &value_exp);
    //  FIXME: typically the array is current the alias of another array
    //       this reference is ignored in range analysis
    if (ret != 0) {
      if (var_name == array) {
        //  ZP: 20161125: only support single assigned alias
        vector<void *> vec_def = m_ast->GetVarDefbyPosition(var_name, sg_pntr);
        if (vec_def.size() == 1 && (m_ast->IsLocalInitName(vec_def[0]) != 0)) {
        } else {
          for (size_t j = 0; j < dim; j++) {
            (*w_range)[j].push_back(RangeInfo(sg_pntr, infinite));
            (*r_range)[j].push_back(RangeInfo(sg_pntr, infinite));
          }
        }
        return;
      }

      //  2.1 point assignment rhs, e.g. int * alias = array;
      if (value_exp == sg_pntr && (m_ast->IsInitName(var_name) != 0)) {
        *new_array = var_name;
        *new_pointer_dim = pointer_dim;

        //  ZP: 20160223
        *new_scope = is_in_ref_scope ? m_ast->GetScope(var_name) : ref_scope;
        *new_range_scope = range_scope;

        //  ZP: 20161125: propagate only if new_array is single assigned
        //  alias
        vector<void *> vec_def =
            m_ast->GetVarDefbyPosition(*new_array, sg_pntr);
        if (vec_def.size() == 1 && (m_ast->IsLocalInitName(vec_def[0]) != 0)) {
        } else {
          for (size_t j = 0; j < dim; j++) {
            (*w_range)[j].push_back(RangeInfo(sg_pntr, infinite));
            (*r_range)[j].push_back(RangeInfo(sg_pntr, infinite));
          }
        }
        return;
      }
    }

    //  2.2 function argument, e.g. sub_func(a); //  sub_func(int *b)
    {
      int arg_idx = m_ast->GetFuncCallParamIndex(sg_pntr);
      if (-1 != arg_idx) {
        void *func_call = m_ast->TraceUpToFuncCall(sg_pntr);
        assert(m_ast->IsFunctionCall(func_call));
        void *func_decl = m_ast->GetFuncDeclByCall(func_call);
        // skip intrinsic function call
        if (m_ast->IsMerlinInternalIntrinsic(
                m_ast->GetFuncNameByCall(func_call, false))) {
          return;
        }
        if (m_ast->IsMemCpy(func_call)) {
          // handle memcpy
          if (!is_in_ref_scope)
            return;
          void *sg_array1, *sg_array2;
          void *sg_pntr1, *sg_pntr2;
          vector<void *> sg_idx1, sg_idx2;
          int pointer_dim1, pointer_dim2;
          void *sg_length;
          int ret = m_ast->parse_memcpy(
              func_call, &sg_length, &sg_array1, &sg_pntr1, &sg_idx1,
              &pointer_dim1, &sg_array2, &sg_pntr2, &sg_idx2, &pointer_dim2);
          int full_dim1 = static_cast<int>(static_cast<size_t>(pointer_dim1) ==
                                           sg_idx1.size() - 1);
          int full_dim2 = static_cast<int>(static_cast<size_t>(pointer_dim2) ==
                                           sg_idx2.size() - 1);
          int is_read = static_cast<int>(sg_array2 == array);
          int is_write = static_cast<int>(sg_array1 == array);
          assert(is_read || is_write);
          auto &curr_range = is_read != 0 ? r_range : w_range;
          if ((ret != 0) && (full_dim1 != 0) && (full_dim2 != 0) &&
              data_unit_size != 0) {
            vector<void *> curr_idx = (is_write) != 0 ? sg_idx1 : sg_idx2;
            //  FIXME: handle cross-bound access, e.g.
            //  int a[5][10];
            //  memcpy(&a[0][0], b, sizeof (int) * 50);
            CMarsExpression me_length =
                CMarsExpression(sg_length, m_ast, curr_ref, range_scope);
            if (me_length.IsConstant() && all_dimension_size_known) {
              size_t num_elements = me_length.GetConstant() / data_unit_size;
              if (num_elements == total_flatten_size) {
                for (size_t j = 0; j < dim; j++) {
                  CMarsRangeExpr curr_mr(0, sg_sizes[j] - 1, m_ast, 1);
                  (*curr_range)[j].push_back(
                      RangeInfo(sg_pntr, curr_mr, num_elements));

#if _DEBUG_RANGE_UNION_
                  // cout << "--RangeUnion: " << curr_mr.print_e() <<
                  // endl;
#endif
                }
                return;
              }
            }

            for (size_t j = 0; j < dim; j++) {
              CMarsExpression me_start(curr_idx[j], m_ast, curr_ref,
                                       range_scope);
              CMarsExpression me_end(curr_idx[j], m_ast, curr_ref, range_scope);
              if (j == dim - 1)
                me_end = (me_end + me_length / data_unit_size) - 1;

              //  need an function to refine a range
              //  Input: CMarsRangeExpr, a scope for range
              //  Output: a new CMarsRangeExpr, where the iterators in the
              //  scope are substituted
              CMarsRangeExpr curr_mr(me_start, me_end, 1);
              size_t access_size = numeric_limits<size_t>::max();
              if (me_length.has_range()) {
                access_size =
                    me_length.get_range().get_const_ub() / data_unit_size;
              }
              (*curr_range)[j].push_back(
                  RangeInfo(sg_pntr, curr_mr, access_size));
#if _DEBUG_RANGE_UNION_
              // cout << "--RangeUnion: " << curr_mr.print_e() << endl;
#endif
            }
            return;
          }
        } else if (m_ast->GetFuncNameByCall(func_call).find(
                       "memcpy_wide_bus_read") == 0 ||
                   m_ast->GetFuncNameByCall(func_call).find(
                       "memcpy_wide_bus_write") == 0) {
          // handle wide bus memcpy
          if (!is_in_ref_scope)
            return;
          int is_write =
              static_cast<int>(m_ast->GetFuncNameByCall(func_call).find(
                                   "memcpy_wide_bus_write") == 0);
          int num_offset_arg = 0;
          void *buf_arg = nullptr;
          if (is_write != 0) {
            buf_arg = m_ast->GetFuncCallParam(func_call, 1);
          } else {
            buf_arg = m_ast->GetFuncCallParam(func_call, 0);
          }
          assert(buf_arg);
          void *basic_type;
          vector<size_t> dims;
          m_ast->get_type_dimension(m_ast->GetTypeByExp(buf_arg), &basic_type,
                                    &dims, buf_arg);
          if (dims.size() > 1) {
            num_offset_arg = dims.size();
          }
          void *sg_length =
              m_ast->GetFuncCallParam(func_call, 3 + num_offset_arg);
          if ((arg_idx == 1 + num_offset_arg && (is_write == 0)) ||
              (arg_idx == 0 && (is_write != 0))) {
            void *sg_pntr = m_ast->GetFuncCallParam(func_call, arg_idx);
            //  FIXME: ZP: 20151023 assume only the pointer itself
            void *sg_tmp = sg_pntr;
            m_ast->remove_cast(&sg_tmp);
            assert(m_ast->IsVarRefExp(sg_tmp) != 0);
            void *sg_offset =
                m_ast->GetFuncCallParam(func_call, 2 + num_offset_arg);
            CMarsExpression me_start(sg_offset, m_ast, sg_pntr, range_scope);
            CMarsExpression me_end(sg_offset, m_ast, sg_pntr, range_scope);
            CMarsExpression me_length(sg_length, m_ast, sg_pntr, range_scope);
            me_start = me_start / data_unit_size;
            me_end = me_end + me_length + (data_unit_size - 1);
            me_end = (me_end / data_unit_size) - 1;
            //  ZP: FIXME: 20160303
            //  exact flag need to be updated carefully, according to the
            //  alignment
            auto curr_range = is_write != 0 ? w_range : r_range;
            CMarsRangeExpr curr_mr(me_start, me_end, 1);
            (*curr_range)[0].push_back(RangeInfo(sg_pntr, curr_mr));
            //  FIXME: handle unaligned access Sean 12/02/2016
          } else {
            auto curr_range = is_write != 0 ? r_range : w_range;
            //  FIXME: handle cross-bound access, e.g.
            //  int a[5][10];
            //  memcpy(&a[0][0], b, sizeof (int) * 50);
            CMarsExpression me_length =
                CMarsExpression(sg_length, m_ast, curr_ref, range_scope);
            if (me_length.IsConstant() && all_dimension_size_known) {
              size_t num_elements = me_length.GetConstant() / data_unit_size;
              if (num_elements == total_flatten_size) {
                for (size_t j = 0; j < dim; j++) {
                  CMarsRangeExpr curr_mr(0, sg_sizes[j] - 1, m_ast, 1);
                  (*curr_range)[j].push_back(
                      RangeInfo(sg_pntr, curr_mr, num_elements));

#if _DEBUG_RANGE_UNION_
                  // cout << "--RangeUnion: " << curr_mr.print_e() <<
                  // endl;
#endif
                }
                return;
              }
            }

            for (size_t j = 0; j < dim; j++) {
              CMarsExpression me_start(sg_indexes[j], m_ast, sg_pntr,
                                       range_scope);
              CMarsExpression me_end(sg_indexes[j], m_ast, sg_pntr,
                                     range_scope);
              if (j == dim - 1) {
                CMarsExpression me_length =
                    CMarsExpression(sg_length, m_ast, sg_pntr, range_scope);
                me_end = me_end + me_length / data_unit_size;
                me_end = me_end - 1;
              }
              //  need an function to refine a range
              //  Input: CMarsRangeExpr, a scope for range
              //  Output: a new CMarsRangeExpr, where the iterators in the
              //  scope are substituted
              CMarsRangeExpr curr_mr(me_start, me_end, 1);
              size_t access_size = numeric_limits<size_t>::max();
              if (me_length.has_range()) {
                access_size =
                    me_length.get_range().get_const_ub() / data_unit_size;
              }
              (*curr_range)[j].push_back(
                  RangeInfo(sg_pntr, curr_mr, access_size));

#if _DEBUG_RANGE_UNION_
              // cout << "--RangeUnion: " << curr_mr.print_e() << endl;
#endif
            }
          }
          return;
        } else if (m_ast->IsFunctionDeclaration(func_decl) != 0) {
          // handle normal sub function
          *new_array = m_ast->GetFuncParam(func_decl, arg_idx);
          *new_pointer_dim = pointer_dim;
          *new_func_call = func_call;
          *new_range_scope = func_decl;
          *new_scope = is_in_ref_scope ? func_decl : ref_scope;
          return;
        } else {
          //  undefined system call
          cerr << "found undefined system/user call" << m_ast->_p(func_call)
               << "\n";
        }
      }
    }
  }
ERROR_OUT : {
  //  3. all the other cases, perform it conservatively
  for (size_t j = 0; j < dim; j++) {
    (*w_range)[j].push_back(RangeInfo(sg_pntr, infinite));
    (*r_range)[j].push_back(RangeInfo(sg_pntr, infinite));
  }
}
}

void ArrayRangeAnalysis::propagate_range(void *curr_ref, void *range_scope,
                                         const vec_list_rinfo &r1,
                                         const vec_list_rinfo &w1, int dim,
                                         int new_pointer_dim,
                                         shared_ptr<vec_list_rinfo> r_range,
                                         shared_ptr<vec_list_rinfo> w_range) {
  void *pos = curr_ref;
  void *sg_array;
  void *sg_pntr;
  vector<void *> sg_indexes;
  int pointer_dim;
  m_ast->parse_pntr_ref_by_array_ref(curr_ref, &sg_array, &sg_pntr, &sg_indexes,
                                     &pointer_dim);
  CMarsAccess ac(sg_pntr, m_ast, range_scope);
  CMarsRangeExpr infinite(m_ast);
  //  ZP: 20160203
  //  assert(dim - pointer_dim == r1->size());
  //  assert(dim - pointer_dim == w1->size());
  //  the left dims are determined, and the right dims are unioned
  //  ZP: 20160203
  if (dim - new_pointer_dim == w1.size()) {
    int is_empty = 0;
    for (auto one_set : w1) {
      if (is_empty_list_range(one_set)) {
        is_empty = 1;
        break;
      }
    }
    if (is_empty == 0) {
      for (size_t j = 0; j < new_pointer_dim; j++) {
        CMarsExpression me_offset =
            CMarsExpression(sg_indexes[j], m_ast, pos, range_scope);
        CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
        (*w_range)[j].push_back(RangeInfo(sg_pntr, mr_offset));
#if _DEBUG_RANGE_UNION_
        //  cout << "--RangeUnion: (pntr=" <<
        //  m_ast->UnparseToString(sg_pntr)
        //     << " dim=" << my_itoa(j) << ") "
        //     << mr_offset.print_e() << endl;
#endif
      }
    }
    if (is_empty == 0) {
      for (size_t j = new_pointer_dim; j < dim; j++) {
        size_t j0 = j - new_pointer_dim;
        //  ZP: 20160204
        CMarsExpression me_offset =
            CMarsExpression(sg_indexes[j], m_ast, pos, range_scope);
        CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
        get_range_with_offset(w1[j0], mr_offset, &(*w_range)[j]);
#if _DEBUG_RANGE_UNION_
        // cout << "--RangeUnion: (pntr=" <<
        // m_ast->UnparseToString(sg_pntr)
        //    << "dim=" << my_itoa(j) << ") " << w1[j0].Union().print_e()
        //    << " + " << mr_offset.print_e() << endl;
#endif
      }
    }
  } else {
    for (size_t j = 0; j < dim; j++) {
      (*w_range)[j].push_back(RangeInfo(curr_ref, infinite));
    }
  }
  //  ZP: 20160203
  if (dim - new_pointer_dim == r1.size()) {
    int is_empty = 0;
    for (auto one_set : r1) {
      if (is_empty_list_range(one_set)) {
        is_empty = 1;
        break;
      }
    }
    if (is_empty == 0) {
      for (int j = 0; j < new_pointer_dim; j++) {
        CMarsExpression me_offset =
            CMarsExpression(sg_indexes[j], m_ast, pos, range_scope);
        CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
        (*r_range)[j].push_back(RangeInfo(sg_pntr, mr_offset));
#if _DEBUG_RANGE_UNION_
        // cout << "--RangeUnion: (pntr=" <<
        // m_ast->UnparseToString(sg_pntr)
        //      << " dim=" << my_itoa(j) << ") "
        //      << zero_range_set.Union().print_e() << " + "
        //      << mr_offset.print_e() << endl;
#endif
      }
    }
    if (is_empty == 0) {
      for (size_t j = new_pointer_dim; j < dim; j++) {
        size_t j0 = j - new_pointer_dim;
        //  ZP: 20160204
        CMarsExpression me_offset =
            CMarsExpression(sg_indexes[j], m_ast, pos, range_scope);
        CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
        get_range_with_offset(r1[j0], mr_offset, &(*r_range)[j]);
#if _DEBUG_RANGE_UNION_
        // cout << "--RangeUnion: " << r1[j0].Union().print_e() << " + "
        //     << mr_offset.print_e() << endl;
#endif
      }
    }
  } else {
    for (size_t j = 0; j < dim; j++) {
      (*r_range)[j].push_back(RangeInfo(curr_ref, infinite));
    }
  }
  return;
}

void ArrayRangeAnalysis::get_range_with_offset(const list_rinfo &old_range,
                                               const CMarsRangeExpr &offset,
                                               list_rinfo *new_range) {
  for (auto &one_old_range : old_range) {
    RangeInfo new_rinfo(one_old_range.sgnode, one_old_range.range + offset,
                        one_old_range.acc_size);
    new_rinfo.func_path = one_old_range.func_path;
    new_range->push_back(new_rinfo);
  }
  return;
}

void ArrayRangeAnalysis::substitute_parameter_atom_in_range(
    void *array, shared_ptr<vec_list_rinfo> read_range,
    shared_ptr<vec_list_rinfo> write_range, void *func_decl, void *func_call,
    void *range_scope) {
  int arg_size = m_ast->GetFuncParamNum(func_decl);
  unordered_map<void *, CMarsExpression> param_to_argument_expr;
  for (int i = 0; i < arg_size; ++i) {
    void *param = m_ast->GetFuncParam(func_decl, i);
    void *var_type = m_ast->GetTypebyVar(param);
    if (m_ast->IsScalarType(var_type) &&
        has_variable_range(m_ast, param, param, func_decl, true) == 0) {
      void *argument = m_ast->GetFuncCallParam(func_call, i);
      param_to_argument_expr[param] =
          CMarsExpression(argument, m_ast, argument, range_scope);
    }
  }
  for (auto &one_dim_range : *read_range) {
    for (auto &one_range_info : one_dim_range) {
      for (const auto &one_pair : param_to_argument_expr) {
        one_range_info.range.substitute(one_pair.first, one_pair.second,
                                        one_pair.second, 1);
      }
      if (one_range_info.sgnode == nullptr ||
          one_range_info.sgnode == func_decl) {
        one_range_info.sgnode = func_call;
        one_range_info.range.set_pos(func_call);
      }
    }
  }
  for (auto &one_dim_range : *write_range) {
    for (auto &one_range_info : one_dim_range) {
      for (const auto &one_pair : param_to_argument_expr) {
        one_range_info.range.substitute(one_pair.first, one_pair.second,
                                        one_pair.second, 1);
      }
      if (one_range_info.sgnode == nullptr ||
          one_range_info.sgnode == func_decl) {
        one_range_info.sgnode = func_call;
        one_range_info.range.set_pos(func_call);
      }
    }
  }

  use_access_range_info(array, func_decl, func_decl, read_range, write_range);
}

void ArrayRangeAnalysis::get_array_dimension_size(
    void *array, size_t *dim_size, int *data_unit_size, int *dim_fake,
    bool *all_dimension_size_known, size_t *total_flatten_size,
    vector<size_t> *sg_sizes) {
  *all_dimension_size_known = true;
  *total_flatten_size = 1;
  {
    void *sg_base_type;
    m_ast->get_type_dimension(m_ast->GetTypebyVar(array), &sg_base_type,
                              sg_sizes, array);
    std::reverse(sg_sizes->begin(), sg_sizes->end());
    *dim_size = sg_sizes->size();
    for (auto dim : *sg_sizes) {
      if (m_ast->IsDeterminedSize(dim)) {
        *total_flatten_size *= dim;
      } else {
        *all_dimension_size_known = false;
        break;
      }
    }
    *data_unit_size = m_ast->get_type_unit_size(sg_base_type, array);
    //  /////////////////////////////////////////  /
    //  ZP: 20170531
    //  fixing for scalar
    if (*dim_size == 0) {
      *dim_fake = 1;
    }
  }
}

bool ArrayRangeAnalysis::equal_range(const list_rinfo &one_rinfos,
                                     const list_rinfo &other_rinfos) {
  for (auto &one_rinfo : one_rinfos) {
    bool matched = false;
    for (auto &other_rinfo : other_rinfos) {
      if (one_rinfo.range == other_rinfo.range) {
        matched = true;
        break;
      }
    }
    if (!matched)
      return false;
  }
  for (auto &one_rinfo : other_rinfos) {
    bool matched = false;
    for (auto &other_rinfo : one_rinfos) {
      if (one_rinfo.range == other_rinfo.range) {
        matched = true;
        break;
      }
    }
    if (!matched)
      return false;
  }

  return true;
}

void ArrayRangeAnalysis::check_exact_flag_for_loop(
    const void *loop, const void *range_scope,
    shared_ptr<vec_list_rinfo> range_group) {
  if (is_control_inexact_for_loop(m_ast, const_cast<void *>(loop),
                                  const_cast<void *>(range_scope))) {
    for (auto &one_dim_range : *range_group) {
      for (auto &one_rinfo : one_dim_range) {
        one_rinfo.range.set_exact(false);
      }
    }
  }
}

void ArrayRangeAnalysis::set_non_exact_flag(
    shared_ptr<vec_list_rinfo> range_group, int dim) {
  for (auto &one_rinfo : range_group->at(dim)) {
    one_rinfo.range.set_exact(false);
  }
}

bool ArrayRangeAnalysis::is_empty_list_range(const list_rinfo &ranges) {
  for (auto &range : ranges) {
    if (!range.range.is_empty())
      return false;
  }
  return true;
}

bool ArrayRangeAnalysis::check_local_or_infinite_range(
    shared_ptr<vec_list_rinfo> ranges, int dim, void *range_scope) {
  for (auto &one_range : ranges->at(dim)) {
    if (one_range.range.any_var_located_in_scope_except_loop_iterator(
            range_scope) != 0 ||
        one_range.range.is_infinite()) {
      one_range.range.refine_range_in_scope_v2(
          range_scope, const_cast<void *>(one_range.sgnode),
          one_range.func_path);
      one_range.sgnode = range_scope;
      one_range.range.set_pos(range_scope);
      if (one_range.range.any_var_located_in_scope_except_loop_iterator(
              range_scope) != 0 ||
          one_range.range.is_infinite()) {
        return true;
      }
    }
  }
  return false;
}

void ArrayRangeAnalysis::use_access_range_info(
    void *array, void *ref_scope, void *range_scope,
    shared_ptr<vec_list_rinfo> read_ranges,
    shared_ptr<vec_list_rinfo> write_ranges) {
  //  YX: 20170307 check whether current range can be refined later
  //  if yes, go ahead; otherwise, replace range inferred from range
  //  intrinsic
  bool access_range_intrinsic_valid = false;
  vector<CMarsRangeExpr> access_range;
  void *access_range_intrinsic =
      GetAccessRangeFromIntrinsic(array, ref_scope, range_scope, m_ast,
                                  &access_range, &access_range_intrinsic_valid);
  if (access_range_intrinsic_valid && (access_range_intrinsic != nullptr)) {
    for (size_t dim = 0; dim < access_range.size(); dim++) {
      if (dim < read_ranges->size()) {
        if (check_local_or_infinite_range(read_ranges, dim, range_scope)) {
          size_t access_size = 0;
          if (m_eval_access_size) {
            for (auto &one_rinfo : read_ranges->at(dim)) {
              access_size = merge_access_size(one_rinfo.acc_size, access_size);
            }
          }
          read_ranges->at(dim).clear();
          read_ranges->at(dim).push_back(
              RangeInfo(range_scope, access_range[dim], access_size));
        }
      }
      if (dim < write_ranges->size()) {
        if (check_local_or_infinite_range(write_ranges, dim, range_scope)) {
          size_t access_size = 0;
          if (m_eval_access_size) {
            for (auto &one_rinfo : write_ranges->at(dim)) {
              access_size = merge_access_size(one_rinfo.acc_size, access_size);
            }
          }
          write_ranges->at(dim).clear();
          write_ranges->at(dim).push_back(
              RangeInfo(range_scope, access_range[dim], access_size));
        }
      }
    }
  }
}

void ArrayRangeAnalysis::intersect_with_access_range_info(void *array,
                                                          void *ref_scope,
                                                          void *range_scope) {
  bool access_range_intrinsic_valid = false;
  vector<CMarsRangeExpr> access_range;
  GetAccessRangeFromIntrinsic(array, ref_scope, range_scope, m_ast,
                              &access_range, &access_range_intrinsic_valid);
  vector<size_t> sg_sizes;
  {
    void *sg_base_type;
    m_ast->get_type_dimension(m_ast->GetTypebyVar(array), &sg_base_type,
                              &sg_sizes, array);
    std::reverse(sg_sizes.begin(), sg_sizes.end());
  }
  for (int i = 0; i < 2; ++i) {
    auto &vec_range = i == 0 ? m_range_r : m_range_w;
    int dim_size = vec_range.size();
    for (size_t dim = 0; dim < dim_size; dim++) {
      //  / ZP: 20161204
      CMarsRangeExpr infinite(m_ast);
      auto one_range = vec_range[dim];
      if ((one_range.any_var_located_in_scope(range_scope) != 0)) {
#if PROJDEBUG
        cerr << "Unrecognized access bounds for variable '" + m_ast->_p(array) +
                    "\' because of containing variable in scope :"
             << endl;
        cerr << m_ast->_p(range_scope) << endl;
        cerr << "range : " << endl;
        cerr << one_range.print_e() << endl;
#endif
        one_range = infinite;
      }

      if (access_range_intrinsic_valid) {
        if (dim < access_range.size()) {
          if (!m_check_access_bound && (one_range.is_exact() == 0)) {
            one_range = one_range.Intersect(access_range[dim]);
          } else {
            if (one_range.is_infinite() != 0) {
              one_range = access_range[dim];
            }
          }
        }
      }
      if (static_cast<unsigned int>(!sg_sizes.empty()) != 0U) {
        if (dim < sg_sizes.size()) {
          size_t dim_size = sg_sizes[dim];
          if (m_ast->IsDeterminedSize(dim_size)) {
            CMarsRangeExpr dim_range(0, dim_size - 1, m_ast, 0);
            if (!m_check_access_bound && (one_range.is_exact() == 0)) {
              one_range = one_range.Intersect(dim_range);
            } else {
              if (one_range.is_infinite() != 0) {
                one_range = dim_range;
              }
            }
          }
        }
      }
      vec_range[dim] = one_range;
    }
  }
}

ArrayRangeAnalysis::ArrayRangeAnalysis(void *array, CMarsAST_IF *ast,
                                       void *ref_scope, void *range_scope,
                                       bool eval_access_size,
                                       bool check_access_bound) {
  m_ast = ast;
  m_eval_access_size = eval_access_size;
  m_check_access_bound = check_access_bound;
  analyze(array, range_scope, ref_scope);
}

void GetAccessRangeInScope_v2(void *array, void *ref_scope, void *range_scope,
                              CMarsAST_IF *ast, vector<CMarsRangeExpr> *r_range,
                              vector<CMarsRangeExpr> *w_range,
                              CMarsRangeExpr *r_flatten_range,
                              CMarsRangeExpr *w_flatten_range,
                              int64_t *access_size, bool check_access_bound) {
  DEFINE_START_END;
  STEMP(start);
  if (ast->IsFunctionDeclaration(range_scope) != 0) {
    range_scope = ast->GetFuncBody(range_scope);
  }
  if (ast->IsFunctionDeclaration(ref_scope) != 0) {
    ref_scope = ast->GetFuncBody(ref_scope);
  }
  //  0. Get all the function paths
  list<t_func_call_path> vec_paths;
  ast->get_all_func_path_in_scope(array, range_scope, &vec_paths);
  TIMEP("  get_all_func_path_in_scope", start, end);

  GetAccessRangeInScope_v2(array, ref_scope, range_scope, ast, vec_paths,
                           r_range, w_range, r_flatten_range, w_flatten_range,
                           access_size, check_access_bound);
}

void GetAccessRangeInScope_v2(void *array, void *ref_scope, void *range_scope,
                              CMarsAST_IF *ast,
                              const list<t_func_call_path> &vec_paths,
                              vector<CMarsRangeExpr> *r_range,
                              vector<CMarsRangeExpr> *w_range,
                              CMarsRangeExpr *r_flatten_range,
                              CMarsRangeExpr *w_flatten_range,
                              int64_t *access_size, bool check_access_bound) {
  DEFINE_START_END;

  STEMP(start);
  if (ast->IsFunctionDeclaration(range_scope) != 0) {
    range_scope = ast->GetFuncBody(range_scope);
  }
  if (ast->IsFunctionDeclaration(ref_scope) != 0) {
    ref_scope = ast->GetFuncBody(ref_scope);
  }
  size_t dim = ast->get_dim_num_from_var(array);
  vector<size_t> sg_sizes;
  {
    void *sg_base_type;
    ast->get_type_dimension(ast->GetTypebyVar(array), &sg_base_type, &sg_sizes,
                            array);
    std::reverse(sg_sizes.begin(), sg_sizes.end());
  }
  TIMEP("  get_dim_num_from_var", start, end);

  //  /////////////////////////////////////////  /
  //  ZP: 20170531
  //  fixing for scalar
  if (dim == 0) {
    dim = 1;
  }
  //  //////////////////////////////  /

  //  1. Get base range for each path
  vector<CMarsRangeExprSet> r_range_set;
  vector<CMarsRangeExprSet> w_range_set;
  {
    w_range_set.resize(dim);
    r_range_set.resize(dim);
    for (size_t j = 0; j < dim; j++) {
      w_range_set[j].set_ast(ast);
      r_range_set[j].set_ast(ast);
    }

    STEMP(start);
    for (auto path : vec_paths) {
      //  cout << ast->print_func_path(path) << endl;

      vector<CMarsRangeExprSet> r_one_range_set;
      vector<CMarsRangeExprSet> w_one_range_set;
      w_one_range_set.resize(dim);
      r_one_range_set.resize(dim);
      for (size_t j = 0; j < dim; j++) {
        w_one_range_set[j].set_ast(ast);
        r_one_range_set[j].set_ast(ast);
      }

      //  cout << ast->print_func_path(path) << endl;
      GetAccessRangeInScope_in_path(array, ref_scope, path, range_scope, ast,
                                    &r_one_range_set, &w_one_range_set,
                                    access_size != nullptr);

      for (size_t j = 0; j < dim; j++) {
        w_range_set[j].merge(w_one_range_set[j]);
        r_range_set[j].merge(r_one_range_set[j]);
      }
    }
    TIMEP("  GetAccessRangeInScope_in_path", start, end);
  }
  assert(r_range_set.size() == w_range_set.size());

  //  2. calculate the access size
  if (access_size != nullptr) {
    int64_t max_access_size = 0;
    for (size_t i = 0; i < dim; i++) {
      int64_t curr_access_size = std::max(r_range_set[i].get_access_size(),
                                          w_range_set[i].get_access_size());
      max_access_size = std::max(curr_access_size, max_access_size);
    }
    *access_size = max_access_size;
    ALGOP("RAN", "access size: " << *access_size);
  }

  //  3. Merge the base range according to the exactness
  r_range->clear();
  w_range->clear();
  for (size_t i = 0; i < dim; i++) {
    STEMP(start);
    CMarsRangeExpr r_range_union = r_range_set[i].Union_v2(range_scope);
    CMarsRangeExpr w_range_union = w_range_set[i].Union_v2(range_scope);
    TIMEP("  CMarsRangeExprSet::Union_v2", start, end);
    ALGOP("RAN", "dim: " << i);
    ALGOP("RAN", "r_range: " << r_range_union.print_e());
    ALGOP("RAN", "w_range: " << w_range_union.print_e());

    bool access_range_intrinsic_valid = false;
    vector<CMarsRangeExpr> access_range;
    STEMP(start);
    GetAccessRangeFromIntrinsic(array, ref_scope, range_scope, ast,
                                &access_range, &access_range_intrinsic_valid);
    TIMEP("  GetAccessRangeFromIntrinsic", start, end);
#ifdef PRINTLOG
    for (auto &range : access_range) {
      ALGOP("RAN", "access range from intrinsic: " + range.print_e());
    }
#endif

    //  / ZP: 20161204
    CMarsRangeExpr infinite(ast);
    STEMP(start);
    if ((r_range_union.any_var_located_in_scope(range_scope) != 0) &&
        (r_range_union.is_movable_to(range_scope) == 0)) {
#if PROJDEBUG
      cerr << "Unrecognized access bounds for variable '" + ast->_p(array) +
                  "\' because of containing variable in scope :"
           << endl;
      cerr << ast->_p(range_scope) << endl;
      cerr << "range : " << endl;
      cerr << r_range_union.print_e() << endl;
#endif
      r_range_union = infinite;
    }

    if (access_range_intrinsic_valid) {
      if (i < access_range.size()) {
        if (!check_access_bound && (r_range_union.is_exact() == 0)) {
          r_range_union = r_range_union.Intersect(access_range[i]);
        } else {
          if (r_range_union.is_infinite() != 0) {
            r_range_union = access_range[i];
          }
        }
      }
    }
    if (static_cast<unsigned int>(!sg_sizes.empty()) != 0U) {
      if (i < sg_sizes.size()) {
        size_t dim_size = sg_sizes[i];
        if (ast->IsDeterminedSize(dim_size)) {
          CMarsRangeExpr dim_range(0, dim_size - 1, ast, 0);
          if (!check_access_bound && (r_range_union.is_exact() == 0)) {
            r_range_union = r_range_union.Intersect(dim_range);
          } else {
            if (r_range_union.is_infinite() != 0) {
              r_range_union = dim_range;
            }
          }
        }
      }
    }
    r_range->push_back(r_range_union);
    TIMEP("  deal_with_r_range_union", start, end);
    ALGOP("RAN", "refined_r_range: " << r_range_union.print_e());

    STEMP(start);
    if ((w_range_union.any_var_located_in_scope(range_scope) != 0) &&
        (w_range_union.is_movable_to(range_scope) == 0)) {
#if PROJDEBUG
      cerr << "Unrecognized access bounds for variable '" + ast->_p(array) +
                  "\' because of containing variable in scope :"
           << endl;
      cerr << ast->_p(range_scope) << endl;
      cerr << "range :" << endl;
      cerr << w_range_union.print_e() << endl;
#endif
      w_range_union = infinite;
    }

    if (access_range_intrinsic_valid) {
      if (i < access_range.size()) {
        if (!check_access_bound && (w_range_union.is_exact() == 0)) {
          w_range_union = w_range_union.Intersect(access_range[i]);
        } else {
          if (w_range_union.is_infinite() != 0) {
            w_range_union = access_range[i];
          }
        }
      }
    }
    if (static_cast<unsigned int>(!sg_sizes.empty()) != 0U) {
      if (i < sg_sizes.size()) {
        size_t dim_size = sg_sizes[i];
        if (ast->IsDeterminedSize(dim_size)) {
          CMarsRangeExpr dim_range(0, dim_size - 1, ast, 0);
          if (!check_access_bound && (w_range_union.is_exact() == 0)) {
            w_range_union = w_range_union.Intersect(dim_range);
          } else {
            if (w_range_union.is_infinite() != 0) {
              w_range_union = dim_range;
            }
          }
        }
      }
    }

    w_range->push_back(w_range_union);
    TIMEP("  deal_with_w_range_union", start, end);
    ALGOP("RAN", "refined_w_range: " << w_range_union.print_e());
  }

  //  //////////////////////  /
  //  4. Get flattened range from multi-dimensional range
  STEMP(start);
  GetFlattenRangeFromNDRange(array, ast, *r_range, *w_range, r_range_set,
                             w_range_set, r_flatten_range, w_flatten_range);
  TIMEP("  GetFlattenRangeFromNDRange", start, end);
  ALGOP("RAN", "flattened_r_range: " << r_flatten_range->print_e());
  ALGOP("RAN", "flattened_w_range: " << w_flatten_range->print_e());
}

int is_exact_condition(CMarsAST_IF *ast, void *cond_stmt, void *scope,
                       bool true_branch) {
  //  1. check whether the condition is simple condition
  //  2. check whether the condition may be possible to be true
  void *cond_exp = ast->GetExprFromStmt(cond_stmt);
  vector<void *> cond_vec;
  if (true_branch) {
    ast->GetExprListFromAndOp(cond_exp, &cond_vec);
  } else {
    ast->GetExprListFromOrOp(cond_exp, &cond_vec);
  }
  int i = 0;
  for (auto &cond : cond_vec) {
    i++;
    if (ast->IsCompareOp(cond) == 0) {
      return 0;
    }
    void *op0;
    void *op1;
    ast->GetExpOperand(cond, &op0, &op1);
    CMarsExpression ce0(op0, ast);
    CMarsExpression ce1(op1, ast);
    CMarsExpression ce_diff = ce0 - ce1;
    if (ce_diff.IsConstant()) {
      if (!evaluate_constant_comparsion(ast->GetCompareOp(cond),
                                        ce_diff.GetConstant(), 0) &&
          (true_branch || i == cond_vec.size())) {
        return 0;
      }
      if (evaluate_constant_comparsion(ast->GetCompareOp(cond),
                                       ce_diff.GetConstant(), 0) &&
          (!true_branch || i == cond_vec.size())) {
        return 1;
      }
      continue;
    }
    auto vars = ce_diff.get_vars();
    // check whether it is simple comparison between loop iterator and
    // constant i.e.  if (i > 10)
    int iter_cnt_in_condition = 0;
    for (auto &var : vars) {
      if (ast->IsForStatement(var) != 0) {
        if (ast->IsInScope(var, scope) == 0) {
          return 0;
        }
        auto coeff = ce_diff.get_coeff(var);
        if (coeff.IsConstant() != 0) {
          int64_t const_coeff = coeff.GetConstant();
          if (const_coeff == 1 || const_coeff == -1) {
            iter_cnt_in_condition++;
            continue;
          }
        }
      }
      return 0;
    }
    if (iter_cnt_in_condition > 1) {
      return 0;
    }
  }
  return 1;
}

//  For each if statement, 1. both if and else-branch have exact range.
//  And 2. the ranges has the same lb and ub
//  For each switch statement, return inexact currently
//  NOTE: the input "is_exact" flag should be initialized (e.g. as 1),
//  this function only try to change it to zero
void exact_check_by_if_stmt(CMarsAST_IF *ast, void *sg_array, void *range_scope,
                            void *sg_pntr, bool is_write, bool *is_exact) {
  //    int has_read = 0, has_write=0;
  //    if (ast->has_write_conservative(sg_pntr)) has_write = 1;
  //    if (ast->has_read_conservative (sg_pntr)) has_read = 1;
  //

#if 0  //  _DEBUG_HELPER_NEW_
    cout << "--check_if : " <<
        ast->UnparseToString(sg_pntr).substr(0, 30) << " in " <<
        ast->UnparseToString(range_scope).substr(0, 30) << endl;
#endif
  if (ast->IsInScope(sg_pntr, range_scope) == 0) {
    return;
  }

  void *curr_pos = sg_pntr;
  void *prev_pos = nullptr;
  //  is_exact[0] = is_exact[1] = 1;
  while ((*is_exact) && (curr_pos != nullptr)) {
    if (ast->IsFunctionDeclaration(curr_pos) != 0) {
      return;
    }

    if (ast->IsIfStatement(curr_pos) != 0) {
      void *if_body = ast->GetIfStmtTrueBody(curr_pos);
      void *else_body = ast->GetIfStmtFalseBody(curr_pos);
      void *if_cond = ast->GetIfStmtCondition(curr_pos);

      if (ast->is_located_in_scope(sg_pntr, if_cond) != 0) {
        return;
      }
      bool true_branch = prev_pos == if_body;
      if (is_exact_condition(ast, if_cond, range_scope, true_branch) == 0) {
        if (else_body == nullptr ||
            (true_branch &&
             ast->GetAllRefInScope(sg_array, else_body).empty()) ||
            (!true_branch &&
             ast->GetAllRefInScope(sg_array, if_body).empty())) {
          *is_exact = false;
        } else {
          vector<CMarsRangeExpr> r_range1;
          vector<CMarsRangeExpr> w_range1;
          CMarsRangeExpr r_flatten_range1;
          CMarsRangeExpr w_flatten_range1;
          GetAccessRangeInScope_v2(sg_array, if_body, if_body, ast, &r_range1,
                                   &w_range1, &r_flatten_range1,
                                   &w_flatten_range1, nullptr, false);

          vector<CMarsRangeExpr> r_range2;
          vector<CMarsRangeExpr> w_range2;
          CMarsRangeExpr r_flatten_range2;
          CMarsRangeExpr w_flatten_range2;
          GetAccessRangeInScope_v2(sg_array, else_body, else_body, ast,
                                   &r_range2, &w_range2, &r_flatten_range2,
                                   &w_flatten_range2, nullptr, false);

          for (int n = 0; n < 2; n++) {
            for (size_t i = 0; i < r_range1.size(); i++) {
              if (n != is_write) {
                continue;
              }

              CMarsRangeExpr range1 = (n == 0) ? r_range1[i] : w_range1[i];
              CMarsRangeExpr range2 = (n == 0) ? r_range2[i] : w_range2[i];

              bool exact = true;
              if ((range1.is_exact() == 0) || (range2.is_exact() == 0)) {
                exact = false;
              }
              if (exact) {
                if (range1.get_lb_set().size() != 1) {
                  exact = false;
                }
                if (range1.get_ub_set().size() != 1) {
                  exact = false;
                }
                if (range2.get_lb_set().size() != 1) {
                  exact = false;
                }
                if (range2.get_ub_set().size() != 1) {
                  exact = false;
                }
              }
              if (exact) {
                CMarsExpression me_lb1 = range1.get_lb_set()[0];
                CMarsExpression me_ub1 = range1.get_ub_set()[0];
                CMarsExpression me_lb2 = range2.get_lb_set()[0];
                CMarsExpression me_ub2 = range2.get_ub_set()[0];
                if ((me_lb1 - me_lb2 == 0) == 0) {
                  exact = false;
                }
                if ((me_ub1 - me_ub2 == 0) == 0) {
                  exact = false;
                }
              }
              if (*is_exact) {
                *is_exact = exact;
                if (!exact)
                  return;
              }
            }
          }
        }
      }
    }
    if (ast->IsSwitchStatement(curr_pos) != 0) {
      *is_exact = false;
    }

    if (curr_pos == range_scope) {
      return;
    }
    prev_pos = curr_pos;
    curr_pos = ast->GetParent(curr_pos);
    //  assert(curr_pos);
  }
}

#if KEEP_UNUSED
int is_control_inexact_condition_stmt(CMarsAST_IF *ast, void *sg_array,
                                      void *loop, void *range_scope,
                                      int is_write) {
  bool is_exact = true;
  exact_check_by_if_stmt(ast, sg_array, range_scope, loop, is_write, &is_exact);
  if (!is_exact)
    return 1;
  else
    return 0;
}
#endif

//  For each for-statement,
//  1. ub-lb need to be positive
//  2. no continue/break statement
//  NOTE: the input "is_exact" flag should be initialized (e.g. as 1),
//  this function only try to change it to zero
void exact_check_by_for_stmt(CMarsAST_IF *ast, void *sg_array,
                             void *range_scope, void *sg_pntr, bool *is_exact) {
  //  int has_read = 0, has_write=0;
  //  if (ast->has_write_conservative(sg_pntr)) has_write = 1;
  //  if (ast->has_read_conservative (sg_pntr)) has_read = 1;

  //  check if loop bound conditions are all met

  void *curr_pos = sg_pntr;
  bool loop_met = true;
  while (loop_met && (curr_pos != nullptr)) {
    //  if (curr_pos == range_scope ||
    //  ast->IsFunctionDeclaration(curr_pos)) break;
    if (ast->IsFunctionDeclaration(curr_pos) != 0) {
      break;
    }

    if ((ast->IsForStatement(curr_pos) != 0) &&
        (ast->is_located_in_scope(sg_pntr, ast->GetLoopBody(curr_pos)) != 0)) {
      CMarsRangeExpr range = CMarsVariable(curr_pos, ast, sg_pntr).get_range();
      range.set_ast(ast);
      range.refine_range_in_scope(range_scope, sg_pntr);
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      if (range.get_simple_bound(&me_lb, &me_ub) == 0) {
        loop_met = false;
      } else {
        CMarsExpression length = me_ub - me_lb + 1;
        if ((me_ub >= me_lb) == 0) {
          loop_met = false;
        }
      }

      if (loop_met) {
        //  check the break and continue statements
        vector<void *> vec_contn;
        vector<void *> vec_break;
        vector<void *> vec_goto;
        ast->GetNodesByType(curr_pos, "preorder", "SgContinueStmt", &vec_contn);
        ast->GetNodesByType(curr_pos, "preorder", "SgBreakStmt", &vec_break);
        ast->GetNodesByType(curr_pos, "preorder", "SgGotoStatement", &vec_goto);
        if (!vec_goto.empty()) {
          loop_met = false;
        } else {
          for (auto contn : vec_contn) {
            void *related_loop = ast->TraceUpToLoopScope(contn);
            if (related_loop == curr_pos) {
              loop_met = false;
              break;
            }
          }
          for (auto br : vec_break) {
            if (ast->IsScopeBreak(br, curr_pos)) {
              loop_met = false;
              break;
            }
          }
        }
      }
    }

    if (curr_pos == range_scope) {
      break;
    }
    curr_pos = ast->GetParent(curr_pos);
  }

  if (!loop_met) {
    *is_exact = false;
  }
}

//  For each while-statement between sg_pntr and range_scope, report
//  inexact NOTE: the input "is_exact" flag should be initialized (e.g. as
//  1), this function only try to change it to zero
void exact_check_by_while_stmt(CMarsAST_IF *ast, void *sg_array,
                               void *range_scope, void *sg_pntr,
                               bool *is_exact) {
  //  int has_read = 0, has_write=0;
  //  if (ast->has_write_conservative(sg_pntr)) has_write = 1;
  //  if (ast->has_read_conservative (sg_pntr)) has_read = 1;

  void *curr_pos = sg_pntr;
  bool loop_met = true;
  while (loop_met && (curr_pos != nullptr)) {
    if (ast->IsFunctionDeclaration(curr_pos) != 0) {
      break;
    }

    if (((ast->IsWhileStatement(curr_pos) != 0) ||
         (ast->IsDoWhileStatement(curr_pos) != 0)) &&
        (ast->is_located_in_scope(sg_pntr, ast->GetLoopBody(curr_pos)) != 0)) {
      loop_met = false;
    }
    if (curr_pos == range_scope) {
      break;
    }
    curr_pos = ast->GetParent(curr_pos);
  }

  if (loop_met == 0) {
    *is_exact = false;
  }
}

void get_control_exact_flag_in_scope_v2(CMarsAST_IF *ast, void *array,
                                        void *pos, void *range_scope,
                                        const t_func_call_path &fn_path,
                                        bool *w_exact, bool *r_exact) {
  if (pos == nullptr) {
    return;
  }

  t_func_call_path::const_iterator it;
#if _DEBUG_FUNC_PATH_
  cout << "---Path L=" << fn_path.size() << endl;
#endif
  //  if (fn_path.size() == 0)
  //  {
  //    get_control_exact_flag_in_scope(ast, array, pos, range_scope,
  //            w_exact, r_exact);
  //  }
  //  else
  int hit = 0;
  for (it = fn_path.begin(); it != fn_path.end(); it++) {
    t_func_call_path::const_iterator it1 = it;
    it1++;
    if (it1 == fn_path.end()) {
      break;
    }
    //  pos -> decl0 .. call0 -> decl1 .. call1 -> scope
    void *sg_decl = it1->first;
    void *sg_pos = it->second;
    if (sg_decl == nullptr) {
      sg_decl = range_scope;
    }

    if ((hit == 0) && (ast->is_located_in_scope_simple(pos, sg_decl) == 0)) {
      continue;
    }

    if (hit == 0) {
      sg_pos = pos;
      hit = 1;
    }

#if _DEBUG_FUNC_PATH_
    cout << "  '" << ast->UnparseToString(it->second).substr(0, 20) << "' .. '"
         << ast->UnparseToString(it1->first).substr(0, 20) << "'" << endl;
#endif
    get_control_exact_flag_in_scope(ast, array, sg_pos, sg_decl, w_exact,
                                    r_exact);
  }
}

//  map<void*, map<void*, map<void*, pair<int, int> > > >
//  g_exact_flag_cache;

//  NOTE: the input "is_exact" flag should be initialized (e.g. as 1),
//  this function only try to change it to zero
void get_control_exact_flag_in_scope(CMarsAST_IF *ast, void *array, void *pos,
                                     void *range_scope, bool *is_write_exact,
                                     bool *is_read_exact) {
  //  ZP: 20161002
  if (g_exact_flag_cache_enabled != 0) {
    if (g_exact_flag_cache.find(array) != g_exact_flag_cache.end()) {
      void *pos_scope = ast->TraceUpToScope(pos);
      if (g_exact_flag_cache[array].find(pos_scope) !=
          g_exact_flag_cache[array].end()) {
        if (g_exact_flag_cache[array][pos_scope].find(range_scope) !=
            g_exact_flag_cache[array][pos_scope].end()) {
          if (g_exact_flag_cache[array][pos_scope][range_scope].first == 0) {
            *is_write_exact = false;
          }
          if (g_exact_flag_cache[array][pos_scope][range_scope].second == 0) {
            *is_read_exact = false;
          }
          return;
        }
      }
    }
  }

  void *sg_array = array;
  void *sg_pntr = pos;
  bool is_exact_by_control = true;
  bool is_exact_by_control_write = true;
  bool is_exact_by_control_read = true;
  if (true) {
#if _DEBUG_CONTROL_CHECH_
    if (i == 0)
      cout << "--GetRangeInScope: " << ast->_p(sg_pntr) << " in "
           << ast->_p(range_scope) << endl;
#endif

    //  these two functions are not across-func
    if (*is_read_exact) {
      exact_check_by_if_stmt(
          ast, sg_array, range_scope, sg_pntr, 0,
          &is_exact_by_control_read);  //  is_exact_by_control);
      if (!is_exact_by_control_read) {
        *is_read_exact = false;
      }
    }
    if (*is_write_exact) {
      exact_check_by_if_stmt(
          ast, sg_array, range_scope, sg_pntr, 1,
          &is_exact_by_control_write);  //  is_exact_by_control);
      if (!is_exact_by_control_write) {
        *is_write_exact = false;
      }
    }

    //  if (!(is_exact_by_control_write && is_exact_by_control_read))
    //  is_exact_by_control = false;
    if ((*is_read_exact) || (*is_write_exact)) {
      exact_check_by_for_stmt(ast, sg_array, range_scope, sg_pntr,
                              &is_exact_by_control);
      if (!is_exact_by_control) {
        *is_read_exact = *is_write_exact = false;
      }
    }
    if ((*is_read_exact) || (*is_write_exact)) {
      exact_check_by_while_stmt(ast, sg_array, range_scope, sg_pntr,
                                &is_exact_by_control);
      if (!is_exact_by_control) {
        *is_read_exact = *is_write_exact = false;
      }
    }

    //  if (!is_exact_by_control) is_exact_by_control_write = false;
    //  if (!is_exact_by_control) is_exact_by_control_read  = false;

    //  cout << "    +" << is_exact_by_control_write << " " <<
    //                  is_exact_by_control_read  << endl;
    //  bool w_exact = true;
    //  bool r_exact = true;
  }
  //  ///////////////  /
  //
  //  if (!is_exact_by_control_write) *is_write_exact = false;
  //  if (!is_exact_by_control_read)  *is_read_exact  = 0;

  //  ZP: 20161002
  if (g_exact_flag_cache_enabled != 0) {
    if (g_exact_flag_cache.find(array) == g_exact_flag_cache.end()) {
      map<void *, map<void *, pair<bool, bool>>> empty_2d;
      g_exact_flag_cache[array] = empty_2d;
    }

    void *pos_scope = ast->TraceUpToScope(pos);
    if (g_exact_flag_cache[array].find(pos_scope) ==
        g_exact_flag_cache[array].end()) {
      map<void *, pair<bool, bool>> empty_1d;
      g_exact_flag_cache[array][pos_scope] = empty_1d;
    }

    {
      pair<bool, bool> new_pair(*is_write_exact, *is_read_exact);
      g_exact_flag_cache[array][pos_scope][range_scope] = new_pair;
    }
  }
}

#if _DEBUG_HELPER_NEW_
string simplify_scope_str(CMarsAST_IF *ast, void *scope);

string simplify_scope_str(CMarsAST_IF *ast, void *scope) {
  int length = 30;
  //  if (ast->IsFunctionCall(ast->GetExprFromStmt(scope)))
  //    return ast->GetFuncNameByCall(ast->GetExprFromStmt(scope)) +
  //    "(...);
  //    ";
  //  if (ast->IsFunctionDeclaration((scope)))
  //    return ast->GetFuncName(scope) + "(...){...} ";
  //  if (ast->IsBasicBlock((scope)))
  //    return ast->UnparseToString(scope).substr(0, length) + "...";
  //  else
  return ast->UnparseToString(scope).substr(0, length) + "...";
}
#endif

//  Notes:
//  1. Consider cross-function (all the sub-function calls are analyzed)
//  2.
void GetAccessRangeInScope_in_path(void *array, void *ref_scope,
                                   t_func_call_path fn_path, void *range_scope,
                                   CMarsAST_IF *ast,
                                   vector<CMarsRangeExprSet> *r_range,
                                   vector<CMarsRangeExprSet> *w_range,
                                   bool eval_access_size) {
  //  ZP: 2016-02-24
  if (ast->IsFunctionDeclaration(range_scope) != 0) {
    range_scope = ast->GetFuncBody(range_scope);
  }
  if (ast->IsFunctionDeclaration(ref_scope) != 0) {
    ref_scope = ast->GetFuncBody(ref_scope);
  }
#if _DEBUG_HELPER_NEW_
  // string str_ref_sc = simplify_scope_str(ast, ref_scope);
  // string str_rng_sc = simplify_scope_str(ast, range_scope);
  // cout << "--helper_old: array=" << ast->UnparseToString(array)
  //      << " refscope=" << str_ref_sc << " rangescope=" << str_rng_sc <<
  //      endl;
#endif
  CMarsRangeExpr infinite(ast);
  //  1. get all the references
  //  //////////////////  /
  //  ZP: 20160303
  //  Currently only support a fine-grained ref_scope as input
  vector<void *> vec_refs;
  ast->get_ref_in_scope(array, ref_scope, fn_path, &vec_refs, true);
  //  //////////////////  /
  size_t dim;
  int data_unit_size = 0;
  vector<size_t> sg_sizes;
  bool all_dimension_size_known = true;
  size_t total_flatten_size = 1;
  {
    void *sg_base_type;
    ast->get_type_dimension(ast->GetTypebyVar(array), &sg_base_type, &sg_sizes,
                            array);
    std::reverse(sg_sizes.begin(), sg_sizes.end());
    dim = sg_sizes.size();
    for (auto dim : sg_sizes) {
      if (ast->IsDeterminedSize(dim)) {
        total_flatten_size *= dim;
      } else {
        all_dimension_size_known = false;
        break;
      }
    }
    data_unit_size = ast->get_type_unit_size(sg_base_type, array);
    if (w_range->size() != dim) {
      w_range->resize(dim);
      r_range->resize(dim);
    }
    for (size_t j = 0; j < dim; j++) {
      //  CMarsRangeExpr mr; mr.set_empty(ast);
      //  (*w_range)[j].Collect(mr, nullptr, nullptr, 0);
      //  (*r_range)[j].Collect(mr, nullptr, nullptr, 0);
      (*w_range)[j].set_ast(ast);
      (*r_range)[j].set_ast(ast);
    }
    //  /////////////////////////////////////////  /
    //  ZP: 20170531
    //  fixing for scalar
    if (dim == 0) {
      size_t dim_fake = 1;
      if (w_range->size() != dim_fake) {
        w_range->resize(dim_fake);
        r_range->resize(dim_fake);
      }
      for (size_t j = 0; j < dim_fake; j++) {
        //  CMarsRangeExpr mr; mr.set_empty(ast);
        //  (*w_range)[j].Collect(mr, nullptr, nullptr, 0);
        //  (*r_range)[j].Collect(mr, nullptr, nullptr, 0);
        (*w_range)[j].set_ast(ast);
        (*r_range)[j].set_ast(ast);
      }
    }
  }
  //  2. handling the accesses
  //  2.1 element reference
  //  2.2 argument (with offset)
  //  2.3 full assignment (with offset)
  //  2.4 partial assignment (with offset) [ZP: supported, 20150821]
  //  2.5 memcpy (with offset)             [ZP: supported, 20150821]
  //  2.6 complex than assignment with offset => full range
  for (size_t i = 0; i < vec_refs.size(); i++) {
    void *curr_ref = vec_refs[i];
    void *pos = curr_ref;
    void *sg_array;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim;
    ast->parse_pntr_ref_by_array_ref(curr_ref, &sg_array, &sg_pntr, &sg_indexes,
                                     &pointer_dim);
    //  ///////////////  /
    //  ZP: 2016-02-09
    //  int is_exact_by_control = 1;
    bool is_exact_by_control_write = true;
    bool is_exact_by_control_read = true;
    CMarsAccess ac(sg_pntr, ast, range_scope);
    void *new_array = nullptr;
    void *new_scope = nullptr;
    t_func_call_path new_path = fn_path;
    void *new_range_scope = nullptr;
    int new_pointer_dim = -1;
    int64_t access_size = 1;
    if (eval_access_size) {
      access_size =
          ast->get_average_loop_trip_count(sg_pntr, range_scope, fn_path);
    }
    if (static_cast<size_t>(pointer_dim) == dim) {
      //  1. full pntr
      //  /////////////////////////////////////////  /
      //  ZP: 20170531
      //  fixing for scalar
      if (dim == 0) {
        CMarsRangeExpr mr(CMarsExpression(ast, 0), CMarsExpression(ast, 0));
        if (ast->has_write_conservative(sg_pntr) != 0) {
          (*w_range)[0].Collect(mr, array, sg_pntr, 1, fn_path, access_size);
        }
        if (ast->has_read_conservative(sg_pntr) != 0) {
          (*r_range)[0].Collect(mr, array, sg_pntr, 0, fn_path, access_size);
        }
      }
      //  /////////////////////////////////////////  /
      bool isRead = true;
      {
        int arg_idx = ast->GetFuncCallParamIndex(sg_pntr);
        if (-1 != arg_idx && dim != 0) {
          void *func_call = ast->TraceUpToFuncCall(sg_pntr);
          assert(ast->IsFunctionCall(func_call));
          void *func_decl = ast->GetFuncDeclByCall(func_call);
          if (ast->IsMerlinInternalIntrinsic(
                  ast->GetFuncNameByCall(func_call, false))) {
            goto SKIP_FULL_PNTR_FUNC;
          }
          if (ast->IsFunctionDeclaration(func_decl) != 0) {
            new_scope = ast->GetFuncBody(func_decl);
            new_range_scope = func_call;
            pos = func_call;
            void *func_para = ast->GetFuncParam(func_decl, arg_idx);
            void *func_para_type = ast->GetTypebyVar(func_para);
            if (ast->IsReferenceType(func_para_type) == 0) {
              goto SKIP_FULL_PNTR_FUNC;
            }
            vector<void *> vec_nodes;
            isRead = false;
            for (auto ref : ast->GetAllRefInScope(func_para, new_scope)) {
              if (ast->IsAddressOfOp(ast->GetParent(ref)) == 0) {
                isRead = true;
                continue;
              }
              void *parent = ast->GetParent(ast->GetParent(ref));
              void *var_name;
              void *value_exp;
              if (ast->parse_assign(parent, &var_name, &value_exp) == 0) {
                goto ERROR_OUT;
              }
              vec_nodes.push_back(var_name);
            }

            if (vec_nodes.empty()) {
              goto SKIP_FULL_PNTR_FUNC;
            } else if (vec_nodes.size() > 1) {
              goto ERROR_OUT;
            }
            new_array = vec_nodes[0];
            new_pointer_dim = pointer_dim - 1;

            if (ast->is_included_in_path(std::make_pair(func_decl, func_call),
                                         fn_path) == 0) {
              goto SKIP_FULL_PNTR_FUNC;
            }
          } else {
            //  undefined system call
          }
        }
      SKIP_FULL_PNTR_FUNC : {}
      }
      for (size_t j = 0; j < dim; j++) {
        CMarsRangeExpr mr(ac.GetIndex(j), ac.GetIndex(j));
        if (ast->has_write_conservative(sg_pntr) != 0) {
          (*w_range)[j].Collect(mr, array, sg_pntr, 1, fn_path, access_size);
        }
        if ((ast->has_read_conservative(sg_pntr) != 0) && isRead) {
          (*r_range)[j].Collect(mr, array, sg_pntr, 0, fn_path, access_size);
        }
#if _DEBUG_RANGE_UNION_
//        cout << "--RangeUnion W: " << (*w_range)[j].print() << endl;
//        cout << "--RangeUnion R: " << (*r_range)[j].print() << endl;
#endif
      }
      if (new_array == nullptr) {
        continue;
      }
    } else if (pointer_dim != -1) {
      //  2. pointer assignement or function argument
      //  2.0 point assignment lhs, e.g. int * array = ...;
      void *var_name;
      void *value_exp;
      int ret =
          ast->parse_assign(ast->GetParent(sg_pntr), &var_name, &value_exp);
      //  FIXME: typically the array is current the alias of another array
      //       this reference is ignored in range analysis
      if (ret != 0) {
        if (var_name == array) {
          //  ZP: 20161125: only support single assigned alias
          vector<void *> vec_def = ast->GetVarDefbyPosition(var_name, sg_pntr);
          if (vec_def.size() == 1 && (ast->IsLocalInitName(vec_def[0]) != 0)) {
          } else {
            for (size_t j = 0; j < dim; j++) {
              (*w_range)[j].Assign(infinite, array, sg_pntr, 1, fn_path,
                                   access_size);
              (*r_range)[j].Assign(infinite, array, sg_pntr, 0, fn_path,
                                   access_size);
            }
          }
          continue;
        }

        //  2.1 point assignment rhs, e.g. int * alias = array;
        if (value_exp == sg_pntr && (ast->IsInitName(var_name) != 0)) {
          new_array = var_name;
          new_pointer_dim = pointer_dim;

          //  ZP: 20160223
          new_scope = ast->GetScope(var_name);
          new_range_scope = range_scope;

          //  ZP: 20161125: propagate only if new_array is single assigned
          //  alias
          vector<void *> vec_def = ast->GetVarDefbyPosition(new_array, sg_pntr);
          if (vec_def.size() == 1 && (ast->IsLocalInitName(vec_def[0]) != 0)) {
          } else {
            for (size_t j = 0; j < dim; j++) {
              (*w_range)[j].Assign(infinite, array, sg_pntr, 1, fn_path,
                                   access_size);
              (*r_range)[j].Assign(infinite, array, sg_pntr, 0, fn_path,
                                   access_size);
            }
            continue;
          }
        }
      }

      //  2.2 function argument, e.g. sub_func(a); //  sub_func(int *b)
      {
        int arg_idx = ast->GetFuncCallParamIndex(sg_pntr);
        if (-1 != arg_idx) {
          void *func_call = ast->TraceUpToFuncCall(sg_pntr);
          assert(ast->IsFunctionCall(func_call));
          void *func_decl = ast->GetFuncDeclByCall(func_call);
          if (ast->IsMerlinInternalIntrinsic(
                  ast->GetFuncNameByCall(func_call, false))) {
            continue;
          }
          if (ast->IsMemCpy(func_call)) {
            void *sg_array1, *sg_array2;
            void *sg_pntr1, *sg_pntr2;
            vector<void *> sg_idx1, sg_idx2;
            int pointer_dim1, pointer_dim2;
            void *sg_length;
            int ret = ast->parse_memcpy(
                func_call, &sg_length, &sg_array1, &sg_pntr1, &sg_idx1,
                &pointer_dim1, &sg_array2, &sg_pntr2, &sg_idx2, &pointer_dim2);
            int full_dim1 = static_cast<int>(
                static_cast<size_t>(pointer_dim1) == sg_idx1.size() - 1);
            int full_dim2 = static_cast<int>(
                static_cast<size_t>(pointer_dim2) == sg_idx2.size() - 1);
            if ((ret != 0) && (full_dim1 != 0) && (full_dim2 != 0) &&
                data_unit_size != 0) {
              int is_read = static_cast<int>(sg_array2 == array);
              int is_write = static_cast<int>(sg_array1 == array);
              assert(is_read || is_write);
              vector<void *> curr_idx = (is_write) != 0 ? sg_idx1 : sg_idx2;
              auto &curr_range = is_read != 0 ? *r_range : *w_range;
              //  FIXME: handle cross-bound access, e.g.
              //  int a[5][10];
              //  memcpy(&a[0][0], b, sizeof (int) * 50);
              CMarsExpression me_length =
                  CMarsExpression(sg_length, ast, curr_ref, range_scope);
              if (me_length.IsConstant() && all_dimension_size_known) {
                size_t num_elements = me_length.GetConstant() / data_unit_size;
                if (num_elements == total_flatten_size) {
                  for (size_t j = 0; j < dim; j++) {
                    CMarsRangeExpr curr_mr(0, sg_sizes[j] - 1, ast, 1);
                    curr_range[j].Collect(curr_mr, array, sg_pntr, is_write,
                                          fn_path, access_size);

#if _DEBUG_RANGE_UNION_
                    // cout << "--RangeUnion: " << curr_mr.print_e() <<
                    // endl;
#endif
                  }
                  continue;
                }
              }

              for (size_t j = 0; j < dim; j++) {
                CMarsExpression me_start(curr_idx[j], ast, curr_ref,
                                         range_scope);
                CMarsExpression me_end(curr_idx[j], ast, curr_ref, range_scope);
                if (j == dim - 1)
                  me_end = (me_end + me_length / data_unit_size) - 1;

                //  need an function to refine a range
                //  Input: CMarsRangeExpr, a scope for range
                //  Output: a new CMarsRangeExpr, where the iterators in
                //  the scope are substituted
                CMarsRangeExpr curr_mr(me_start, me_end, 1);
                curr_range[j].Collect(curr_mr, array, sg_pntr, is_write,
                                      fn_path, access_size);

#if _DEBUG_RANGE_UNION_
                // cout << "--RangeUnion: " << curr_mr.print_e() << endl;
#endif
              }
              continue;
            }
          } else if (ast->GetFuncNameByCall(func_call).find(
                         "memcpy_wide_bus_read") == 0 ||
                     ast->GetFuncNameByCall(func_call).find(
                         "memcpy_wide_bus_write") == 0) {
            int is_write =
                static_cast<int>(ast->GetFuncNameByCall(func_call).find(
                                     "memcpy_wide_bus_write") == 0);
            int num_offset_arg = 0;
            void *buf_arg = nullptr;
            if (is_write != 0) {
              buf_arg = ast->GetFuncCallParam(func_call, 1);
            } else {
              buf_arg = ast->GetFuncCallParam(func_call, 0);
            }
            assert(buf_arg);
            void *basic_type;
            vector<size_t> dims;
            ast->get_type_dimension(ast->GetTypeByExp(buf_arg), &basic_type,
                                    &dims, buf_arg);
            if (dims.size() > 1) {
              num_offset_arg = dims.size();
            }
            void *sg_length =
                ast->GetFuncCallParam(func_call, 3 + num_offset_arg);
            if ((arg_idx == 1 + num_offset_arg && (is_write == 0)) ||
                (arg_idx == 0 && (is_write != 0))) {
              void *sg_pntr = ast->GetFuncCallParam(func_call, arg_idx);
              //  FIXME: ZP: 20151023 assume only the pointer itself
              void *sg_tmp = sg_pntr;
              ast->remove_cast(&sg_tmp);
              assert(ast->IsVarRefExp(sg_tmp) != 0);
              void *sg_offset =
                  ast->GetFuncCallParam(func_call, 2 + num_offset_arg);
              CMarsExpression me_start(sg_offset, ast, sg_pntr, range_scope);
              CMarsExpression me_end(sg_offset, ast, sg_pntr, range_scope);
              CMarsExpression me_length(sg_length, ast, sg_pntr, range_scope);
              me_start = me_start / data_unit_size;
              me_end = me_end + me_length + (data_unit_size - 1);
              me_end = (me_end / data_unit_size) - 1;
              //  ZP: FIXME: 20160303
              //  exact flag need to be updated carefully, according to
              //  the alignment
              auto &curr_range = is_write != 0 ? *w_range : *r_range;
              CMarsRangeExpr curr_mr(me_start, me_end, 1);
              curr_range[0].Collect(curr_mr, array, sg_pntr, is_write, fn_path,
                                    access_size);
#if _DEBUG_RANGE_UNION_
              // cout << "--RangeUnion: " << curr_mr.print_e() << endl;
#endif
              //  FIXME: handle unaligned access Sean 12/02/2016
            } else {
              auto &curr_range = is_write != 0 ? *r_range : *w_range;
              //  FIXME: handle cross-bound access, e.g.
              //  int a[5][10];
              //  memcpy(&a[0][0], b, sizeof (int) * 50);
              CMarsExpression me_length =
                  CMarsExpression(sg_length, ast, curr_ref, range_scope);
              if (me_length.IsConstant() && all_dimension_size_known) {
                size_t num_elements = me_length.GetConstant() / data_unit_size;
                if (num_elements == total_flatten_size) {
                  for (size_t j = 0; j < dim; j++) {
                    CMarsRangeExpr curr_mr(0, sg_sizes[j] - 1, ast, 1);
                    curr_range[j].Collect(curr_mr, array, sg_pntr, is_write,
                                          fn_path, access_size);

#if _DEBUG_RANGE_UNION_
                    // cout << "--RangeUnion: " << curr_mr.print_e() <<
                    // endl;
#endif
                  }
                  continue;
                }
              }

              for (size_t j = 0; j < dim; j++) {
                CMarsExpression me_start(sg_indexes[j], ast, sg_pntr,
                                         range_scope);
                CMarsExpression me_end(sg_indexes[j], ast, sg_pntr,
                                       range_scope);
                if (j == dim - 1) {
                  CMarsExpression me_length =
                      CMarsExpression(sg_length, ast, sg_pntr, range_scope);
                  me_end = me_end + me_length / data_unit_size;
                  me_end = me_end - 1;
                }
                //  need an function to refine a range
                //  Input: CMarsRangeExpr, a scope for range
                //  Output: a new CMarsRangeExpr, where the iterators in
                //  the scope are substituted
                CMarsRangeExpr curr_mr(me_start, me_end, 1);
                curr_range[j].Collect(curr_mr, array, sg_pntr, is_write,
                                      fn_path, access_size);
#if _DEBUG_RANGE_UNION_
                // cout << "--RangeUnion: " << curr_mr.print_e() << endl;
#endif
              }
            }
            continue;
          } else if (ast->IsFunctionDeclaration(func_decl) != 0) {
            new_array = ast->GetFuncParam(func_decl, arg_idx);
            new_pointer_dim = pointer_dim;
            new_scope = ast->GetFuncBody(func_decl);
            new_range_scope = func_call;
            pos = func_call;
            if (ast->is_included_in_path(std::make_pair(func_decl, func_call),
                                         fn_path) == 0) {
              continue;
            }
          } else {
            //  undefined system call
          }
        }
      }
    }
    if (new_array != nullptr) {
      vector<CMarsRangeExprSet> r1, w1;
      GetAccessRangeInScope_in_path(new_array, new_scope, new_path,
                                    new_range_scope, ast, &r1, &w1,
                                    eval_access_size);
      //  ZP: 20160203
      //  assert(dim - pointer_dim == r1.size());
      //  assert(dim - pointer_dim == w1.size());
      //  the left dims are determined, and the right dims are unioned
      //  ZP: 20160203
      if (dim - new_pointer_dim == w1.size()) {
        int is_empty = 0;
        for (auto one_set : w1) {
          if (one_set.is_empty_conservative() != 0)
            is_empty = 1;
        }
        if (is_empty == 0) {
          for (size_t j = 0; j < new_pointer_dim; j++) {
            CMarsExpression me_offset =
                CMarsExpression(sg_indexes[j], ast, pos, range_scope);
            CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
            CMarsRangeExprSet zero_range_set;
            zero_range_set.set_ast(ast);
            CMarsRangeExpr curr_mr(0, 0, ast, 1);
            int is_write = 1;
            zero_range_set.Collect(curr_mr, array, sg_pntr, is_write, fn_path,
                                   access_size);
            (*w_range)[j].Collect_with_offset_with_pos(
                zero_range_set, mr_offset, !is_exact_by_control_write, 0);
#if _DEBUG_RANGE_UNION_
            //  cout << "--RangeUnion: (pntr=" <<
            //  ast->UnparseToString(sg_pntr)
            //     << " dim=" << my_itoa(j) << ") "
            //     << zero_range_set.Union().print_e() << " + "
            //     << mr_offset.print_e() << endl;
#endif
          }
        }
        if (is_empty == 0) {
          for (size_t j = new_pointer_dim; j < dim; j++) {
            size_t j0 = j - new_pointer_dim;
            //  ZP: 20160204
            CMarsExpression me_offset =
                CMarsExpression(sg_indexes[j], ast, pos, range_scope);
            CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
            (*w_range)[j].Collect_with_offset_with_pos(
                w1[j0], mr_offset, !is_exact_by_control_write, access_size);
#if _DEBUG_RANGE_UNION_
            // cout << "--RangeUnion: (pntr=" <<
            // ast->UnparseToString(sg_pntr)
            //    << "dim=" << my_itoa(j) << ") " <<
            //    w1[j0].Union().print_e()
            //    << " + " << mr_offset.print_e() << endl;
#endif
          }
        }
      } else {
        for (size_t j = 0; j < dim; j++) {
          (*w_range)[j].Assign(infinite, array, sg_pntr, 1, fn_path,
                               access_size);
        }
      }
      //  ZP: 20160203
      if (dim - new_pointer_dim == r1.size()) {
        int is_empty = 0;
        for (auto one_set : r1) {
          if (one_set.is_empty_conservative() != 0) {
            is_empty = 1;
          }
        }
        if (is_empty == 0) {
          for (int j = 0; j < new_pointer_dim; j++) {
            CMarsExpression me_offset =
                CMarsExpression(sg_indexes[j], ast, pos, range_scope);
            CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
            CMarsRangeExprSet zero_range_set;
            zero_range_set.set_ast(ast);
            CMarsRangeExpr curr_mr(0, 0, ast, 1);
            int is_write = 0;
            zero_range_set.Collect(curr_mr, array, sg_pntr, is_write, fn_path,
                                   access_size);
            (*r_range)[j].Collect_with_offset_with_pos(
                zero_range_set, mr_offset, !is_exact_by_control_write, 0);
#if _DEBUG_RANGE_UNION_
            // cout << "--RangeUnion: (pntr=" <<
            // ast->UnparseToString(sg_pntr)
            //      << " dim=" << my_itoa(j) << ") "
            //      << zero_range_set.Union().print_e() << " + "
            //      << mr_offset.print_e() << endl;
#endif
          }
        }
        if (is_empty == 0) {
          for (size_t j = new_pointer_dim; j < dim; j++) {
            size_t j0 = j - new_pointer_dim;
            //  ZP: 20160204
            CMarsExpression me_offset =
                CMarsExpression(sg_indexes[j], ast, pos, range_scope);
            CMarsRangeExpr mr_offset(me_offset, me_offset, 1);
            (*r_range)[j].Collect_with_offset_with_pos(
                r1[j0], mr_offset, !is_exact_by_control_read, access_size);
#if _DEBUG_RANGE_UNION_
            // cout << "--RangeUnion: " << r1[j0].Union().print_e() << " +
            // "
            //     << mr_offset.print_e() << endl;
#endif
          }
        }
      } else {
        for (size_t j = 0; j < dim; j++) {
          (*r_range)[j].Assign(infinite, array, sg_pntr, 0, fn_path,
                               access_size);
        }
      }
      continue;
    }
  ERROR_OUT : {
    //  3. all the other cases, perform it conservatively
    for (size_t j = 0; j < dim; j++) {
      (*w_range)[j].Assign(infinite, array, sg_pntr, 1, fn_path, access_size);
      (*r_range)[j].Assign(infinite, array, sg_pntr, 0, fn_path, access_size);
    }
  }
  }
  //  YX: 20170307 check whether current range can be refined later
  //  if yes, go ahead; otherwise, replace range inferred from range
  //  intrinsic
  bool access_range_intrinsic_valid = false;
  vector<CMarsRangeExpr> access_range;
  void *access_range_intrinsic =
      GetAccessRangeFromIntrinsic(array, ref_scope, range_scope, ast,
                                  &access_range, &access_range_intrinsic_valid);
  if (access_range_intrinsic_valid && (access_range_intrinsic != nullptr)) {
    for (size_t j = 0; j < dim; j++) {
      if (j < access_range.size()) {
        CMarsRangeExprSet tmp_r_set = (*r_range)[j];
        int64_t r_access_size = tmp_r_set.get_access_size();
        CMarsRangeExpr tmp_r_range = tmp_r_set.Union_v2(range_scope);
        if (((tmp_r_range.any_var_located_in_scope(range_scope) != 0) &&
             (tmp_r_range.is_movable_to(range_scope) == 0)) ||
            (tmp_r_range.is_infinite() != 0)) {
          (*r_range)[j].Assign(access_range[j], array, access_range_intrinsic,
                               0, fn_path, r_access_size);
        }
        CMarsRangeExprSet tmp_w_set = (*w_range)[j];
        int64_t w_access_size = tmp_w_set.get_access_size();
        CMarsRangeExpr tmp_w_range = tmp_w_set.Union_v2(range_scope);
        if (((tmp_w_range.any_var_located_in_scope(range_scope) != 0) &&
             (tmp_w_range.is_movable_to(range_scope) == 0)) ||
            (tmp_w_range.is_infinite() != 0)) {
          (*w_range)[j].Assign(access_range[j], array, access_range_intrinsic,
                               1, fn_path, w_access_size);
        }
      }
    }
  }

  if (!sg_sizes.empty()) {
    for (size_t j = 0; j < dim; j++) {
      if (j < sg_sizes.size()) {
        size_t dim_size = sg_sizes[j];
        if (!ast->IsDeterminedSize(dim_size))
          continue;

        CMarsRangeExprSet tmp_r_set = (*r_range)[j];
        int64_t r_access_size = tmp_r_set.get_access_size();
        CMarsRangeExpr tmp_r_range = tmp_r_set.Union_v2(range_scope);
        if (((tmp_r_range.any_var_located_in_scope(range_scope) != 0) &&
             (tmp_r_range.is_movable_to(range_scope) == 0)) ||
            (tmp_r_range.is_infinite() != 0)) {
          (*r_range)[j].Assign(CMarsRangeExpr(0, dim_size - 1, ast, 0), array,
                               array, 0, fn_path, r_access_size);
        }
        CMarsRangeExprSet tmp_w_set = (*w_range)[j];
        int64_t w_access_size = tmp_w_set.get_access_size();
        CMarsRangeExpr tmp_w_range = tmp_w_set.Union_v2(range_scope);
        if (((tmp_w_range.any_var_located_in_scope(range_scope) != 0) &&
             (tmp_w_range.is_movable_to(range_scope) == 0)) ||
            (tmp_w_range.is_infinite() != 0)) {
          (*w_range)[j].Assign(CMarsRangeExpr(0, dim_size - 1, ast, 0), array,
                               array, 1, fn_path, w_access_size);
        }
      }
    }
  }
}

vector<CMarsRangeExpr> RangeUnioninVector(const vector<CMarsRangeExpr> &vec1,
                                          const vector<CMarsRangeExpr> &vec2) {
  vector<CMarsRangeExpr> ret;

  if (vec1.empty()) {
    return vec2;
  }

  assert(vec1.size() == vec2.size());
  for (size_t i = 0; i < vec1.size(); i++) {
    CMarsRangeExpr mr = vec1[i].Union(vec2[i]);
    ret.push_back(mr);
  }

  return ret;
}

void *GetAccessRangeFromIntrinsic(void *array, void *ref_scope,
                                  void *range_scope, CMarsAST_IF *ast,
                                  vector<CMarsRangeExpr> *range, bool *valid) {
  *valid = false;
  void *curr_scope = ref_scope;
  if (ast->IsFunctionDeclaration(ref_scope)) {
    curr_scope = ast->GetFuncBody(ref_scope);
  }
  while (ast->IsFunctionDeclaration(curr_scope) == 0) {
    if ((curr_scope != nullptr) && (ast->IsBasicBlock(curr_scope) == 0)) {
      curr_scope = ast->GetParent(curr_scope);
      continue;
    }
    vector<void *> vec_refs;
    ast->get_ref_in_scope(array, curr_scope, &vec_refs);

    void *access_range_intrinsic = nullptr;
    for (auto ref : vec_refs) {
      int arg_idx = ast->GetFuncCallParamIndex(ref);
      if (arg_idx == -1) {
        continue;
      }

      void *func_call = ast->TraceUpToFuncCall(ref);

      string func_name = ast->GetFuncNameByCall(func_call);
      if (func_name.find("__merlin_access_range") != 0) {
        continue;
      }

      void *stmt = ast->TraceUpToStatement(ref);
      //  check whether access range intrinsic is in the same scope
      if (ast->GetChildStmtIdx(curr_scope, stmt) >=
          ast->GetChildStmtNum(curr_scope)) {
        continue;
      }
      if (*valid) {
        //  multiple access range intrinsic
        *valid = false;
        access_range_intrinsic = nullptr;
        range->clear();
        return nullptr;
      }
      int arg_num = ast->GetFuncCallParamNum(func_call);

      assert(((arg_num - 1) % 2) == 0);

      for (int i = 1; i < arg_num; i += 2) {
        void *lb = ast->GetFuncCallParam(func_call, i);
        void *ub = ast->GetFuncCallParam(func_call, i + 1);
        CMarsRangeExpr mr(CMarsExpression(lb, ast, func_call, range_scope),
                          CMarsExpression(ub, ast, func_call, range_scope),
                          0 /*non-exact*/);
        range->push_back(mr);
      }
      *valid = true;
      access_range_intrinsic = func_call;
    }

    if (*valid) {
      return access_range_intrinsic;
    }
    if (curr_scope != nullptr) {
      curr_scope = ast->GetParent(curr_scope);
    } else {
      break;
    }
  }
  return nullptr;
}

CMarsExpression::CMarsExpression(void *expr, CMarsAST_IF *ast, void *pos,
                                 void *scope)
    : m_ast(ast) {
  DEFINE_START_END;
  m_expr = expr;

  STEMP(start);
  m_coeff.clear();
  TIMEP("        m_coeff.clear", start, end);

  m_const = 0;
  if (pos == nullptr) {
    m_pos = expr;
  } else {
    m_pos = pos;
  }
  if (scope != nullptr) {
    m_scope = scope;
  } else {
    m_scope = nullptr;
  }
  m_finite = 0;
  m_float_pos = 1;

  STEMP(start);
  analysis();  //  parse the program to generate m_coeff
  print_debug();
  TIMEP("        analysis", start, end);
  //  assert(m_ast->IsIntegerType(m_int_type));
}
CMarsExpression::CMarsExpression(const CMarsExpression &op) {
  m_ast = op.m_ast;
  m_expr = op.m_expr;
  m_coeff = op.m_coeff;
  m_const = op.m_const;
  m_pos = op.m_pos;
  m_scope = op.m_scope;
  m_float_pos = op.m_float_pos;
  m_finite = op.m_finite;
  m_int_type = op.m_int_type;
  //  assert(m_ast->IsIntegerType(m_int_type));
  //        print_debug();
}

CMarsExpression::CMarsExpression() {
  m_ast = nullptr;
  m_expr = nullptr;
  m_coeff.clear();
  m_const = 0;
  m_pos = nullptr;
  m_scope = nullptr;
  m_float_pos = 1;
  m_finite = 3;
  m_int_type = V_SgTypeVoid;
  //        print_debug();
}

CMarsExpression::CMarsExpression(CMarsVariable mv) {
  m_ast = mv.m_ast;
  void *expr = mv.m_var;
  void *pos = mv.m_pos;
  void *scope = mv.m_scope;

  m_expr = expr;
  m_coeff.clear();
  m_const = 0;
  if (pos == nullptr) {
    m_pos = expr;
  } else {
    m_pos = pos;
  }
  if (scope != nullptr) {
    m_scope = scope;
  } else {
    m_scope = nullptr;
  }
  m_finite = 0;
  m_float_pos = 1;
  analysis();  //  parse the program to generate m_coeff
  print_debug();
  //  assert(m_ast->IsIntegerType(m_int_type));
}

void CMarsExpression::set_pos(void *new_pos) {
  assert(!m_ast->is_floating_node(new_pos));
  m_pos = new_pos;
  m_float_pos = m_ast->is_floating_node(new_pos);
}
//  zero-order
int CMarsExpression::IsConstant() const {
  if (is_INF() != 0) {
    return 0;
  }
  { return static_cast<int>(m_coeff.empty()); }
}

#if KEEP_UNUSED
int CMarsExpression::IsAtomic() {
  reduce();
  if (m_coeff.size() == 1) {
    if (m_coeff.begin()->second.IsConstant()) {
      return 1;
    }
  }
  return 0;
}
#endif

int CMarsExpression::IsAtomicWithNonDeterminedRange() const {
  for (auto it : m_coeff) {
    if ((it.second.IsConstant() == 0) ||
        !has_variable_range(m_ast, it.first, m_pos, m_scope, false)) {
      return 1;
    }
  }
  return 0;
}

int CMarsExpression::IsNormal() const {  //  all the coefficient are constant
  map<void *, CMarsExpression>::const_iterator it;
  for (it = m_coeff.begin(); it != m_coeff.end(); it++) {
    if (it->second.IsConstant() == 0) {
      return 0;
    }
  }
  return 1;
}

CMarsExpression CMarsExpression::get_coeff(void *var) const {
  if (m_coeff.find(var) != m_coeff.end()) {
    return m_coeff.find(var)->second;
  }
  return CMarsExpression(get_ast(), 0);
}
int64_t CMarsExpression::get_const_coeff(void *var) {
  if (m_coeff.find(var) != m_coeff.end()) {
    assert(m_coeff[var].IsConstant());
    return m_coeff[var].get_const();
  }
  return 0;
}

void CMarsExpression::operator=(const CMarsExpression &op) {
  m_expr = op.m_expr;
  m_coeff = op.m_coeff;
  m_const = op.m_const;
  m_ast = op.m_ast;
  m_pos = op.m_pos;
  m_float_pos = op.m_float_pos;
  m_finite = op.m_finite;
  if (m_int_type == V_SgTypeVoid) {
    //  keep original type to keep type cast info
    m_int_type = op.m_int_type;
  }
}

CMarsRangeExpr::CMarsRangeExpr(const CMarsExpression &lb,
                               const CMarsExpression &ub, bool is_exact) {
  if (!lb.is_INF() && !ub.is_INF()) {
    if (lb > ub) {
      set_empty(lb.get_ast());
      m_is_exact = is_exact;
      return;
    }
  }
  m_lb.push_back(lb);
  m_ub.push_back(ub);
  //  printf("--- lb=%s, ub=%s \n", lb.print_s().c_str(),
  //  ub.print_s().c_str());
  if ((lb - ub == 0) != 0) {
    is_exact = true;
  }
  reduce();
  m_is_exact = is_exact;
  m_ast = lb.get_ast();
  CMarsAST_IF *ast = ub.get_ast();
  if (m_ast == nullptr) {
    m_ast = ast;
  } else if (ast != nullptr) {
    assert(m_ast == ast);
  }
  //  assert(get_ast());
}

#if 0
CMarsRangeExpr::CMarsRangeExpr(vector<CMarsExpression> lb,
                               vector<CMarsExpression> ub, bool is_exact) {
  m_lb = lb;
  m_ub = ub;
  reduce();
  m_is_exact = is_exact;
  m_ast = nullptr;
  for (auto &me : lb) {
    CMarsAST_IF *ast = me.get_ast();
    if (!m_ast)
      m_ast = ast;
    else if (ast)
      assert(m_ast == ast);
  }
  for (auto &me : ub) {
    CMarsAST_IF *ast = me.get_ast();
    if (!m_ast)
      m_ast = ast;
    else if (ast)
      assert(m_ast == ast);
  }
  //  assert(get_ast());
}
#endif

//  union operation
void CMarsRangeExpr::AppendLB(const CMarsExpression &lb) {
  m_lb.push_back(lb);
  CMarsAST_IF *ast = lb.get_ast();
  if (m_ast == nullptr) {
    m_ast = ast;
  } else if (ast != nullptr) {
    assert(m_ast == ast);
  }
  reduce();
}
void CMarsRangeExpr::AppendUB(const CMarsExpression &ub) {
  m_ub.push_back(ub);
  CMarsAST_IF *ast = ub.get_ast();
  if (m_ast == nullptr) {
    m_ast = ast;
  } else if (ast != nullptr) {
    assert(m_ast == ast);
  }
  reduce();
}
void CMarsRangeExpr::set_lb(const CMarsExpression &lb) {
  m_lb.clear();
  m_lb.push_back(lb);
  CMarsAST_IF *ast = lb.get_ast();
  if (m_ast == nullptr) {
    m_ast = ast;
  } else if (ast != nullptr) {
    assert(m_ast == ast);
  }
  reduce();
}
void CMarsRangeExpr::set_ub(const CMarsExpression &ub) {
  m_ub.clear();
  m_ub.push_back(ub);
  CMarsAST_IF *ast = ub.get_ast();
  if (m_ast == nullptr) {
    m_ast = ast;
  } else if (ast != nullptr) {
    assert(m_ast == ast);
  }
  reduce();
}
//  FIXME: to be deprecated, using the set based one
CMarsExpression CMarsRangeExpr::get_lb(int idx) const {
  if (has_lb() != 0) {
    return m_lb[idx];
  }
  return neg_INF;
}
CMarsExpression CMarsRangeExpr::get_ub(int idx) const {
  if (has_ub() != 0) {
    return m_ub[idx];
  }
  return pos_INF;
}

//  conservative
int CMarsRangeExpr::is_empty() const {
  //  Youxiang: 20180810 lower bound is neg_INF if lbs is empty
  //                   upper bound is pos_INF is ubs is empty
  //  1. lbs and ubs are both non-empty
  //  2. for every lb, ub, lb-ub-1 >= 0
  for (size_t i = 0; i < m_lb.size(); i++) {
    for (size_t j = 0; j < m_ub.size(); j++) {
      CMarsExpression lb = m_lb[i];
      CMarsExpression ub = m_ub[j];
      if ((lb.is_pINF() != 0) || (ub.is_nINF() != 0)) {
        return 1;
      }
      if ((lb.IsConstant() != 0) && (ub.IsConstant() != 0)) {
        if (lb.GetConstant() > ub.GetConstant()) {
          return 1;
        }
      } else {
        //  CMarsExpression diff = lb - ub - 1;
        if ((lb > ub) != 0) {
          return 1;
        }
      }
    }
  }

  return 0;
}

void CMarsRangeExpr::set_empty(CMarsAST_IF *ast) {
  // if (ast == nullptr) {
  //   ast = get_ast();
  // }
  *this = CMarsRangeExpr(pos_INF, neg_INF);
  m_ast = ast;
  //  CMarsRangeExpr(CMarsExpression(MARS_INT64_MAX, ast),
  //                CMarsExpression(MARS_INT64_MIN, ast));
  m_is_exact = true;
}

string CMarsRangeExpr::print_const_bound() const {
  if (is_empty() != 0) {
    return "empty";
  }
  string str_lb = is_const_lb() != 0 ? my_itoa(get_const_lb()) : "neg_inf";
  string str_ub = is_const_ub() != 0 ? my_itoa(get_const_ub()) : "pos_inf";
  return str_lb + ".." + str_ub;
}

string CMarsRangeExpr::print() const {
  if (is_empty() != 0) {
    return "empty";
  }
  return print(m_lb, 1) + ".." + print(m_ub, 0);
}
string CMarsRangeExpr::print_e() const {
  if (is_empty() != 0) {
    return "empty";
  }
  return print() + ((m_is_exact) != 0 ? " exact" : " inexact");
}

bool CMarsRangeExpr::length_is_greater_or_equal(int64_t len) const {
  //  1. lbs and ubs are both non-empty
  //  2. for every lb, ub, lb-ub-1 >= 0
  for (size_t i = 0; i < m_lb.size(); i++) {
    for (size_t j = 0; j < m_ub.size(); j++) {
      CMarsExpression lb = m_lb[i];
      CMarsExpression ub = m_ub[j];
      //  CMarsExpression diff = ub - lb + 1 - len;
      if ((ub - lb >= len - 1) == 0) {
        return false;
      }
    }
  }
  return true;
}

bool CMarsRangeExpr::IsConstantLength(int64_t *len) const {
  //  1. lbs and ubs are both non-empty
  //  2. for every lb, ub, lb-ub-1 >= 0
  bool flag = false;
  for (size_t i = 0; i < m_lb.size(); i++) {
    for (size_t j = 0; j < m_ub.size(); j++) {
      CMarsExpression lb = m_lb[i];
      CMarsExpression ub = m_ub[j];
      CMarsExpression diff = ub - lb + 1;
      if (diff.IsConstant() == 0) {
        return false;
      }
      if (!flag) {
        *len = diff.GetConstant();
        flag = true;
      } else if (*len != diff.GetConstant()) {
        return false;
      }
    }
  }
  return true;
}

bool CMarsRangeExpr::is_single_access() const {
  int64_t len = -1;
  if (!IsConstantLength(&len)) {
    return false;
  }
  if (len != 1) {
    return false;
  }
  return true;
}

bool CMarsRangeExpr::is_full_access(int64_t size) const {
  if (!m_is_exact) {
    return false;
  }
  if (IsConstantBound() == 0) {
    return false;
  }
  if (get_const_lb() != 0 || get_const_ub() != size - 1) {
    return false;
  }
  return true;
}

string CMarsRangeExpr::print(const vector<CMarsExpression> &bound,
                             int is_lb) const {
  string str;  //  = has_lb() ? get_lb().print() : "na";
  if (!bound.empty()) {
    if (bound.size() > 1) {
      string curr_str = bound[0].print_s();
      for (size_t i = 1; i < bound.size(); i++) {
        string str_0 = is_lb != 0 ? "min(" : "max(";
        curr_str = str_0 + curr_str + "," + bound[i].print_s() + ")";
      }
      str += curr_str;
    } else {
      str = bound[0].print_s();
    }
  } else {
    str = "na";
  }

  return str;
}

CMarsAST_IF *CMarsRangeExpr::get_ast() const {
  //  FIXME
  if (m_ast != nullptr) {
    return m_ast;
  }
  if (has_lb() != 0) {
    return m_lb[0].get_ast();
  }
  if (has_ub() != 0) {
    return m_ub[0].get_ast();
  }
  return nullptr;
}

#if KEEP_UNUSED
vector<int64_t> CMarsDepDistSet::get_set() {
  vector<int64_t> ret;
  for (int64_t i = m_lb; i <= m_ub; i++) {
    ret.push_back(i);
  }
  return ret;
}
#endif

int CMarsDepDistSet::is_empty() {
  if (m_lb == CMarsRangeExpr::MARS_INT64_MAX) {
    return 1;
  }
  if (m_ub == CMarsRangeExpr::MARS_INT64_MIN) {
    return 1;
  }

  return static_cast<int>(m_lb > m_ub);
}

#if KEEP_UNUSED
int64_t CMarsDepDistSet::size() {
  if (is_empty())
    return 0;
  if (!has_lb() || !has_ub())
    return CMarsRangeExpr::MARS_INT64_MAX;

  return m_ub - m_lb + 1;
}
#endif
string CMarsDepDistSet::print() {
  string str;
  if (is_empty() != 0) {
    return "empty";
  }

  string str_lb;
  if (m_lb == CMarsRangeExpr::MARS_INT64_MIN) {
    str_lb = "*";
  } else {
    str_lb = my_itoa(m_lb);
  }

  string str_ub;
  if (m_ub == CMarsRangeExpr::MARS_INT64_MAX) {
    str_ub = "*";
  } else {
    str_ub = my_itoa(m_ub);
  }

  if (str_ub == str_lb) {
    str = str_ub;
  } else {
    str += str_lb + ".." + str_ub;
  }
  return str;
}

CMarsAccess::CMarsAccess(void *ref, CMarsAST_IF *ast, void *scope, void *pos)
    : m_scope(scope) {
  m_ref = ref;
  m_ast = ast;
  if (pos == nullptr) {
    m_pos = ref;
  } else {
    m_pos = pos;
  }
  analysis_exp();  //  get array, exp_list, order
  analysis_var();  //  get iv, pv
}

void CMarsAccess::set_scope(void *scope) {
  assert(m_ast);
  m_scope = scope;  //  CMarsScope(scope);
  analysis_var();   //  get iv, pv
}

#if 0
void CMarsAccess::set_pos(void *pos) {
  assert(false);  //  use the construction function instead
  m_pos = pos;
  analysis_var();  //  get iv, pv
}
#endif

DUMMY::DUMMY(CMarsAST_IF *ast, void *expr, void *pos, void *scope) {
  v.push_back((int64_t)ast);
  v.push_back((int64_t)expr);
  v.push_back((int64_t)pos);
  v.push_back((int64_t)scope);
}

DUMMY::DUMMY(const CMarsExpression *exp) {
  v.push_back((int64_t)exp->m_ast);
  v.push_back((int64_t)exp->m_expr);
  v.push_back((int64_t)exp->m_pos);
  v.push_back((int64_t)exp->m_scope);
  //  v.push_back((int64_t) exp->m_float_pos);
  for (auto &ele : exp->m_coeff) {
    v.push_back((int64_t)ele.first);
    v.push_back(static_cast<int64_t>(ele.second.m_const));
  }
  v.push_back(static_cast<int64_t>(exp->m_const));
  v.push_back(static_cast<int64_t>(exp->m_finite));
}

DUMMY::DUMMY(const CMarsVariable *var) {
  v.push_back((int64_t)var->m_ast);
  if ((var->m_ast->IsVarRefExp(var->m_var) != 0) &&
      (var->m_ast->is_floating_node(var->m_var) != 0)) {
    v.push_back((int64_t)var->m_ast->GetVariableInitializedName(var->m_var));
  } else {
    v.push_back((int64_t)var->m_var);
  }
  v.push_back((int64_t)var->m_pos);
  v.push_back((int64_t)var->m_scope);
}

bool DUMMY::operator<(const DUMMY &var) const {
  if (this->v.size() != var.v.size()) {
    return true;
  }

  for (size_t i = 0; i < this->v.size(); ++i) {
    if (this->v[i] < var.v[i]) {
      return true;
    }
    if (this->v[i] > var.v[i]) {
      return false;
    }
  }

  return false;
}

#if KEEP_UNUSED
string DUMMY::toString() {
  std::stringstream ss;
  for (auto ele : this->v) {
    ss << ele << "  ";
  }
  return ss.str();
}
#endif
}  //  namespace MarsProgramAnalysis

void invalidateCache() {
  DEFINE_START_END;
  STEMP(start);
  cache_analysis_induct_map_in_scope.clear();
  MarsProgramAnalysis::cache_get_range_var.clear();
  MarsProgramAnalysis::cache_get_range_exp.clear();
  MarsProgramAnalysis::cache_consts.clear();
  MarsProgramAnalysis::cache_IsNonNegative.clear();
  MarsProgramAnalysis::cache_IsNonPositive.clear();
  MarsProgramAnalysis::cache_IsMatchedFuncAndCall.clear();
  ACCTM(invalidateCache, start, end);
}
