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
#include "bsuNode.h"
using MarsProgramAnalysis::CMarsExpression;
#define USED_CODE_IN_COVERAGE_TEST 0
//  FIXME: order vector string: when if statement exists, still incorrect
//  (Apr.3, 2015, Yuxin), but will not lead to assert
bool CMirNode::order_vector_gen(CSageCodeGen *codegen, void *sg_node_scope,
                                void *sg_node) {
  vector<void *> iterators;
  vector<SgExpression *> lower;
  vector<SgExpression *> upper;
  vector<int> ub_limit_vec;
  size_t i;

  bool ret =
      analyze_iteration_domain_order(codegen, sg_node_scope, sg_node,
                                     &iterators, &lower, &upper, &ub_limit_vec);
  if (!ret) {
    cout << "Non-canonical loop\n";
    return false;
  }

  for (i = 0; i < iterators.size(); i++) {
    iterator_table.push_back(iterators[i]);

    if (true) {
      CMarsExpression me(lower[i], codegen);
      if (me.IsConstant() != 0) {
        range_lower_table[i] = me.GetConstant();
      } else if (lower[i]->unparseToString() == "0") {
        range_lower_table[i] = 0;
        //  FIXME
      } else {
#if PROJDEBUG
        cout << "[MARS-IR-WARN] " << codegen->get_file(lower[i]) << ", line "
             << codegen->get_line(lower[i])
             << ": Unsupported for-loop lower range in iteration domain "
                "analysis: "
             << lower[i]->unparseToString() << endl;
#endif
        is_affine = false;
        if (i == lower.size() - 1) {
          is_curr_affine = false;
        }
      }
    } else {
#if PROJDEBUG
      cout
          << "[MARS-IR-WARN] " << codegen->get_file(lower[i]) << ", line "
          << codegen->get_line(lower[i])
          << ": Unsupported for-loop lower range in iteration domain analysis: "
          << lower[i]->unparseToString() << endl;
#endif
      is_affine = false;
      return false;
    }

    if (true) {
      CMarsExpression me(upper[i], codegen);
      if (me.IsConstant() != 0) {
        range_upper_table[i] = me.GetConstant();
      } else if (upper[i]->unparseToString() == "0") {
        range_upper_table[i] = 0;
      } else {
#if PROJDEBUG
        cout << "[MARS-IR-WARN] " << codegen->get_file(upper[i]) << ", line "
             << codegen->get_line(upper[i])
             << ": Unsupported for-loop upper range in iteration domain "
                "analysis: "
             << upper[i]->unparseToString() << endl;
#endif
        is_affine = false;
        if (i == upper.size() - 1) {
          is_curr_affine = false;
        }
      }
    } else {
#if PROJDEBUG
      cout
          << "[MARS-IR-WARN] " << codegen->get_file(upper[i]) << ", line "
          << codegen->get_line(upper[i])
          << ": Unsupported for-loop upper range in iteration domain analysis: "
          << upper[i]->unparseToString() << endl;
#endif
      is_affine = false;
      return false;
    }
    upper_err_table[i] = ub_limit_vec[i];
  }
  return true;
}

bool CMirNode::analyze_iteration_domain_order(CSageCodeGen *codegen,
                                              void *sg_scope_, void *sg_stmt_,
                                              vector<void *> *iterators,
                                              vector<SgExpression *> *lower,
                                              vector<SgExpression *> *upper,
                                              vector<int> *ub_limit_vec) {
  size_t i;
  void *curr_loop = codegen->GetEnclosingNode("ForLoop", sg_stmt_);

  //  1. get loops
  vector<void *> sg_loops;
  while (curr_loop != nullptr) {
    sg_loops.push_back(curr_loop);
    curr_loop = codegen->GetEnclosingNode("ForLoop", curr_loop);
  }
  iterators_num = sg_loops.size();
  std::reverse(sg_loops.begin(), sg_loops.end());
  loops_outside = sg_loops;
  //  3. fill information
  for (i = 0; i < sg_loops.size(); i++) {
    void *sg_loop = sg_loops[i];
    void *ivar = nullptr;
    void *lb = nullptr;
    void *ub = nullptr;
    void *step = nullptr;
    void *loop_body = nullptr;
    void *cond_op;
    int ub_limit = 0;
    int is_canonical = codegen->ParseOneForLoop(
        sg_loop, &ivar, &lb, &ub, &step, &cond_op, &loop_body, &ub_limit);
    if (is_canonical == 0) {
#if PROJDEBUG
      cout << "[codegen] WARNING: unsupported for-loop in iteration domain "
              "analysis: \n"
           << (static_cast<SgNode *>(sg_loop)->unparseToString()) << endl;
#endif
      return false;
    }
    if (ivar == nullptr)
      return false;
    iterators->push_back(ivar);
    lower->push_back(static_cast<SgExpression *>(lb));
    upper->push_back(static_cast<SgExpression *>(ub));
    ub_limit_vec->push_back(ub_limit);
  }

  return true;
}

bool CMirNode::is_complete_unroll(bool check_skip_attribute) {
  for (auto pragma : vec_pragma) {
    if ((pragma.is_parallel() || pragma.is_hls_unroll()) &&
        (!pragma.get_attribute(CMOST_complete).empty() ||
         pragma.get_attribute(CMOST_parallel_factor).empty())) {
      if (!check_skip_attribute || pragma.get_attribute(CMOST_SKIP).empty()) {
        return true;
      }
    }
  }
  return false;
}

bool CMirNode::is_autoFG() {
  for (auto pragma : vec_pragma) {
    if (!pragma.get_attribute(CMOST_auto).empty()) {
      return true;
    }
  }
  return false;
}

#if USED_CODE_IN_COVERAGE_TEST
void CMirNode::print_parent() {
  if (parent_node) {
    cout << "Parent: " << parent_node->ref->unparseToString() << endl;
  }
}

bool CMirNode::ParseLoopBound(SgExpression *bound) {
  if (isSgBinaryOp(bound))
    return false;

  if (isSgVarRefExp(bound))
    return false;
  return true;
}

SgForStatement *CMirNode::get_parent_for() {
  SgForStatement *ref_for =
      SageInterface::getEnclosingNode<SgForStatement>(ref);
  return ref_for;
}
#endif

void CMirNode::init() {
  ref = nullptr;
  parent_node = nullptr;
  has_loop = false;
  iterators_num = -1;
  is_function = false;
  is_fine_grain = true;
  is_pipeline = false;
  is_parallel = false;
  is_innermost = true;
  is_affine = true;
  is_curr_affine = true;
  is_kernel = false;
  is_task = false;
  is_while = false;
  arraycount = 0;
  iterator_table.clear();  //  clean index expression
  vec_pragma.clear();
}

CMirNode::CMirNode(SgBasicBlock *body, bool function) {
  init();
  ref = body;
  is_function = function;
}

CMirNode::~CMirNode() {
  full_access_table_v2.clear();
  pragma_table.clear();
  child_node_set.clear();
  child_node.clear();
  predecessor_set.clear();
  predecessors.clear();
}

void parse_sub_func_access(CSageCodeGen *codegen, void *func_decl,
                           void *arr_orig, void *arr_local_arg,
                           map<void *, set<void *>> *node_access_table) {
  vector<void *> vec_tmp;
  codegen->GetNodesByType_int(func_decl, "preorder", V_SgVarRefExp, &vec_tmp);
  for (size_t i = 0; i < vec_tmp.size(); i++) {
    void *ref = vec_tmp[i];
    void *sg_array = nullptr;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim;
    codegen->parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr, &sg_indexes,
                                         &pointer_dim);
    void *sg_type = codegen->GetTypebyVar(sg_array);
    if ((codegen->IsPointerType(sg_type) == 0) &&
        (codegen->IsArrayType(sg_type) == 0)) {
      continue;
    }
    //  handle pointer alias
    while (codegen->IsArgumentInitName(sg_array) == 0) {
      auto vec_source = codegen->get_alias_source(sg_array, ref);
      if (vec_source.size() != 1) {
        break;
      }
      void *source = vec_source[0];
      void *var_ref = nullptr;
      codegen->parse_array_ref_from_pntr(source, &var_ref);
      if (var_ref == nullptr) {
        break;
      }
      sg_array = codegen->GetVariableInitializedName(var_ref);
    }
    //  Not the reference of the argument we want to collect
    if ((codegen->IsArgumentInitName(sg_array) != 0) &&
        sg_array != arr_local_arg) {
      continue;
    }

    void *target_arr = nullptr;
    if (sg_array == arr_local_arg) {
      target_arr = arr_orig;
    } else {
      target_arr = sg_array;  //  Declared locally or globally
    }

    if (codegen->IsVarRefExp(sg_pntr) == 0) {
      (*node_access_table)[target_arr].insert(ref);
    }

    //  Recursively
    if (codegen->GetEnclosingNode("FunctionCallExp", ref) != nullptr) {
      if (pointer_dim < codegen->get_dim_num_from_var(sg_array)) {
        void *sg_call = codegen->GetEnclosingNode("FunctionCallExp", ref);
        int param_index = codegen->GetFuncCallParamIndexRecur(ref);
        if (param_index == -1) {
          continue;
        }
        void *sg_decl = codegen->GetFuncDeclByCall(sg_call, 1);
        if (sg_decl == nullptr) {
          continue;
        }
        if (codegen->IsFromInputFile(sg_decl) == 0) {
          continue;
        }
        void *sg_arg = codegen->GetFuncParam(sg_decl, param_index);
        parse_sub_func_access(codegen, sg_decl, target_arr, sg_arg,
                              node_access_table);
      }
    }
  }
}

//  Recursively
void collect_accesses_in_scope(CSageCodeGen *codegen, void *scope,
                               map<void *, set<void *>> *node_access_table,
                               bool recursively) {
  vector<void *> vec_tmp;
  codegen->GetNodesByType_int(scope, "preorder", V_SgVarRefExp, &vec_tmp);
  for (size_t i = 0; i < vec_tmp.size(); i++) {
    void *ref = vec_tmp[i];
    void *array_init = codegen->GetVariableInitializedName(ref);
    void *sg_type = codegen->GetTypebyVar(array_init);
    if ((codegen->IsPointerType(sg_type) == 0) &&
        (codegen->IsArrayType(sg_type) == 0)) {
      continue;
    }
    void *sg_array = nullptr;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim;
    codegen->parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr, &sg_indexes,
                                         &pointer_dim, nullptr, true, -1, 1);
    if (codegen->IsVarRefExp(sg_pntr) == 0) {
      (*node_access_table)[array_init].insert(ref);
    }
    //  Relate the accesses in the subfunctions
    if ((codegen->GetEnclosingNode("FunctionCallExp", ref) != nullptr) &&
        recursively) {
      //  If there is partial access in the function call for the array
      //  then there might be accesses to the rest of the dimensions
      //  in the subfunction calls.
      if (pointer_dim < codegen->get_dim_num_from_var(sg_array)) {
        void *sg_call = codegen->GetEnclosingNode("FunctionCallExp", ref);
        int param_index = codegen->GetFuncCallParamIndexRecur(ref);
        if (param_index == -1) {
          continue;
        }
        void *sg_decl = codegen->GetFuncDeclByCall(sg_call, 1);
        if (sg_decl == nullptr) {
          continue;
        }
        if (codegen->IsFromInputFile(sg_decl) == 0) {
          continue;
        }
        void *sg_arg = codegen->GetFuncParam(sg_decl, param_index);
        parse_sub_func_access(codegen, sg_decl, sg_array, sg_arg,
                              node_access_table);
      }
    }
  }
}

void CMirNode::set_full_access_table_v2(CSageCodeGen *codegen,
                                        bool recursively) {
  full_access_table_v2.clear();
  collect_accesses_in_scope(codegen, ref, &full_access_table_v2, recursively);
}

int CMirNode::has_opt_pragmas() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_pipeline() || pragma.is_parallel()) {
      return 1;
    }
  }
  return 0;
}
int CMirNode::has_pipeline() {
  if (is_complete_unroll()) {
    return 0;
  }
  for (auto pragma : vec_pragma) {
    if (pragma.is_pipeline()) {
      return 1;
    }
  }
  return 0;
}
int CMirNode::has_parallel() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_parallel()) {
      string f_str = pragma.get_attribute(CMOST_parallel_factor);
      if (!f_str.empty()) {
        int factor = my_atoi(f_str);
        if (factor == 1) {
          return 0;
        }
      }
      return 1;
    }
  }
  return 0;
}
int CMirNode::has_reduction() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_reduction()) {
      return 1;
    }
  }
  return 0;
}
int CMirNode::has_line_buffer() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_line_buffer()) {
      return 1;
    }
  }
  return 0;
}
#if USED_CODE_IN_COVERAGE_TEST
int CMirNode::has_tiling() {
  for (auto pragma : vec_pragma)
    if (pragma.is_loop_tiling())
      return true;
  return false;
}
int CMirNode::has_flatten_off() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_loop_flatten() && !pragma.get_attribute(CMOST_OFF).empty()) {
      return 1;
    }
  }
  return 0;
}
#endif
int CMirNode::has_spawn() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_spawn()) {
      return 1;
    }
  }
  return 0;
}
int CMirNode::has_partition(bool *turn_on) {
  for (auto pragma : vec_pragma) {
    if (pragma.is_partition()) {
      *turn_on = pragma.get_attribute(CMOST_OFF).empty();
      return 1;
    }
  }
  return 0;
}
int CMirNode::is_partial_unroll(int *factor) {
  for (auto pragma : vec_pragma) {
    if (!pragma.is_parallel() && !pragma.is_hls_unroll())
      continue;
    if (pragma.get_attribute(CMOST_parallel_factor).empty())
      continue;
    *factor = my_atoi(pragma.get_attribute(CMOST_parallel_factor));
    if (*factor > 1) {
      return 1;
    }
  }
  return 0;
}

string CMirNode::get_attribute(string attr) {
  for (auto pragma : vec_pragma) {
    string ret = pragma.get_attribute(attr);
    if (!ret.empty()) {
      return ret;
    }
  }
  return "";
}

void CMirNode::remove_attribute(string attr) {
  for (auto &pragma : vec_pragma) {
    pragma.remove_attribute(attr);
  }
}

#if USED_CODE_IN_COVERAGE_TEST
CAnalPragma CMirNode::get_pipeline_pragma() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_pipeline())
      return pragma;
  }
  assert(0);
}
#endif

CAnalPragma CMirNode::get_parallel_pragma() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_parallel()) {
      return pragma;
    }
  }
  assert(0);
}

#if USED_CODE_IN_COVERAGE_TEST
CAnalPragma CMirNode::get_tiling_pragma() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_loop_tiling())
      return pragma;
  }
  assert(0);
}
#endif

void CMirNode::add_one_child(CMirNode *child) {
  if (child_node_set.count(child) > 0) {
    return;
  }
  child_node_set.insert(child);
  child_node.push_back(child);
}

void CMirNode::add_one_predecessor(CMirNode *caller) {
  if (predecessor_set.count(caller) > 0) {
    return;
  }
  predecessor_set.insert(caller);
  predecessors.push_back(caller);
}

#if USED_CODE_IN_COVERAGE_TEST
CMirNode *CMirNode::get_child(size_t index) {
  assert(0);
  if (child_node.size() <= index)
    return nullptr;
  return child_node[index];
}
#endif
vector<CMirNode *> CMirNode::get_all_child(bool recursive) {
  if (!recursive) {
    return child_node;
  }
  vector<CMirNode *> ret_vec;
  for (size_t i = 0; i != child_node.size(); ++i) {
    ret_vec.push_back(child_node[i]);
  }
  for (size_t i = 0; i != child_node.size(); ++i) {
    vector<CMirNode *> sub_child = child_node[i]->get_all_child(recursive);
    for (size_t ii = 0; ii != sub_child.size(); ++ii) {
      ret_vec.push_back(sub_child[ii]);
    }
  }
  return ret_vec;
}

void CMirNode::get_opt_vars(CSageCodeGen *codegen, string opt_str,
                            vector<void *> *vars) {
  void *sg_scope = ref;

  set<string> all_vars_set;
  vector<string> all_vars_vec;
  vector<CAnalPragma> vec_pragma = get_all_pragmas();
  for (auto ana_pragma : vec_pragma) {
    string var_str = ana_pragma.get_attribute(CMOST_variable);
    if (var_str.empty()) {
      continue;
    }
    if (opt_str == "reduction") {
      if (ana_pragma.is_reduction()) {
        if (all_vars_set.count(var_str) <= 0) {
          all_vars_set.insert(var_str);
          all_vars_vec.push_back(var_str);
        }
      }
    } else if (opt_str == "line_buffer") {
      if (ana_pragma.is_line_buffer()) {
        if (all_vars_set.count(var_str) <= 0) {
          all_vars_set.insert(var_str);
          all_vars_vec.push_back(var_str);
        }
      }
    } else if (opt_str == "false_dependence") {
      if (ana_pragma.is_false_dep()) {
        if (all_vars_set.count(var_str) <= 0) {
          all_vars_set.insert(var_str);
          all_vars_vec.push_back(var_str);
        }
      }
    }
  }

  for (auto var_str : all_vars_vec) {
    void *target_var = nullptr;
    target_var = codegen->getInitializedNameFromName(sg_scope, var_str, true);
    if (target_var != nullptr) {
      vars->push_back(target_var);
    }
  }
}

void CMirNode::collectVendorOptPragmas(CSageCodeGen *codegen,
                                       std::vector<void *> *HLS_pragmas) {
  vector<void *> pragmaStmts;
  codegen->GetNodesByType(ref, "preorder", "SgPragmaDeclaration", &pragmaStmts);
  void *loop_scope = codegen->GetParent(ref);
  for (auto pragma : pragmaStmts) {
    void *curr_loop_scope = codegen->TraceUpToLoopScope(pragma);
    if (is_function && curr_loop_scope)
      continue;
    if (!is_function && curr_loop_scope != loop_scope)
      continue;
    SgPragmaDeclaration *decl =
        isSgPragmaDeclaration(static_cast<SgNode *>(pragma));
    CAnalPragma ana_pragma(codegen);
    bool errorOut = false;
    if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false, true)) {
      if (ana_pragma.is_hls_fg_opt()) {
        HLS_pragmas->push_back(decl);
      }
    }
  }
}

int CMirNode::has_hls_fg_opt() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_hls_fg_opt())
      return true;
  }
  return false;
}

int CMirNode::has_hls_unroll() {
  for (auto pragma : vec_pragma) {
    if (pragma.is_hls_unroll())
      return true;
  }
  return false;
}
