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


//  module           :   Array Dimension Interchange
//  description      :   Interchange the parallel dimensions to lower position
//  (right) author           :   Peng Zhang
//                      (pengzhang@falcon-computing.com)
//  ******************************************************************************************//
#include <iostream>
#include <string>
//  #include <boost/algorithm/string.hpp>
#include "file_parser.h"
#include "math.h"
#include <boost/algorithm/string/replace.hpp>

#include "codegen.h"
#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "postwrap_process.h"
#include "rose.h"

using MarsProgramAnalysis::CMarsExpression;

class CAutoDimInterchange {
 public:
  CAutoDimInterchange(CMarsAST_IF *ast_in, CMarsIr *mars_ir_in,
                      CMarsIrV2 *mars_ir_v2_in, const CInputOptions &options_in)
      : ast(ast_in), mars_ir(mars_ir_in), mars_ir_v2(mars_ir_v2_in),
        options(options_in) {}

  int run(void *kernel);

 protected:
  //  Major steps:
  int auto_dim_interchange_collect_parallel_dim_access(
      void *kernel, map<void *, set<int>> *p_parallel_access);  //  output
  int auto_dim_interchange_collect_associated_arrays(
      void *kernel, const map<void *, set<int>> &parallel_access,  //  input
      map<set<void *>, set<int>> *p_parallel_array_dims);          //  ouput
  int auto_dim_interchange_filter_associated_arrays(
      void *kernel,
      map<set<void *>, set<int>> *p_parallel_array_dims,  //  input/output
      map<void *, vector<size_t>> *p_common_dims,  // output //  left to right
      int pre_check);
  int auto_dim_interchange_apply_dim_extension_and_offset_split(
      void *kernel, map<set<void *>, set<int>> *p_parallel_array_dims,  // i/o
      const map<void *, vector<size_t>> &common_dims);                  // input
  int auto_dim_interchange_calculate_common_dims(
      void *kernel,
      map<set<void *>, set<int>> *p_parallel_array_dims,  //  input/output
      map<void *, vector<size_t>> *p_common_dims,  // output   //  left to right
      int pre_check);
  int auto_dim_interchange_apply_dim_interchange(
      void *kernel,
      const map<set<void *>, set<int>> &parallel_array_dims,  // input
      const map<void *, vector<size_t>> &common_dims);        // input

  //  Sub functions:
  int alias_induction_in_scopes(void *sg_name, void *scope,
                                set<void *> *p_boundaries,
                                set<void *> *p_visited_func);
  template <typename T>
  vector<T> dim_switch(vector<T> input, set<int> dims_to_move_right);
  void add_addressof_and_simplify(void **new_pntr, int64_t n);
  int check_pntr_nonzero_idx_condition(void *array, void *pntr,
                                       map<void *, vector<size_t>> common,
                                       set<int> dim_set,
                                       string *p_invalid_reason);
  int get_parallel_variables_from_paralel_loops(void *init_name, void *scope,
                                                set<void *> *p_para_iter);

  int apply_add_offset_variable(
      void *kernel, const set<void *> &alias_set,
      int max_para_dim,                            //  input
      map<void *, vector<void *>> *p_offset_vars,  //  output
      string *p_invalid_reason);
  int apply_shift_offset_to_variable(
      void *kernel,
      int max_para_dim,                                //  input
      const map<void *, vector<void *>> &offset_vars,  //  input
      string *p_invalid_reason);
  int apply_dim_extension(
      void *kernel, const map<void *, vector<size_t>> &common_dims,  //  input
      const map<void *, vector<void *>> &offset_vars,                //  input
      string *p_invalid_reason);

  int get_offset_var_set_recur(void *array, int max_para_dim,
                               set<void *> *p_offset_var_set,
                               bool has_offset_propagate,
                               string *p_invalid_reason);
  int apply_shift_offset_one_rvalue(void *rvalue, int max_para_dim,
                                    vector<void *> offset_inits,
                                    string *p_invalid_reason);
  bool invalid_rvalue_for_dim_ext(void *rvalue, int adjusted_array_dim,
                                  string *p_invalid_reason);
  bool is_in_range_intrisic(void *ref);
  void update_access_range_intrinsic(void *sg_init);

 protected:
  CMarsAST_IF *ast;
  CMarsIr *mars_ir;
  CMarsIrV2 *mars_ir_v2;
  CInputOptions options;
};

bool CAutoDimInterchange::is_in_range_intrisic(void *ref) {
  void *call = ast->TraceUpToFuncCall(ref);
  return (call != nullptr) &&
         ast->IsMerlinInternalIntrinsic(ast->GetFuncNameByCall(call));
}

void CAutoDimInterchange::update_access_range_intrinsic(void *sg_init) {
  if (ast->IsArgumentInitName(sg_init) == 0) {
    return;
  }

  void *func_decl = ast->TraceUpToFuncDecl(sg_init);
  if (func_decl == nullptr) {
    return;
  }
  void *func_body = ast->GetFuncBody(func_decl);
  if (func_body == nullptr) {
    return;
  }

  void *var_init = sg_init;

  int continue_flag;
  vector<size_t> dims =
      mars_ir_v2->get_array_dimension(var_init, &continue_flag);
  if (!dims.empty()) {
    bool all_dim_valid = true;
    for (auto dim : dims) {
      if (dim == 0U) {
        all_dim_valid = false;
        break;
      }
    }
    void *access_range_intrinsic_call = nullptr;
    if (all_dim_valid) {
      string access_range_intrinsic_name = "__merlin_access_range";
      vector<void *> arg_list;
      arg_list.push_back(ast->CreateVariableRef(var_init));
      for (auto dim : dims) {
        arg_list.push_back(ast->CreateConst(0));
        arg_list.push_back(ast->CreateConst(dim - 1));
      }
      access_range_intrinsic_call = ast->CreateFuncCall(
          access_range_intrinsic_name, ast->GetTypeByString("void"), arg_list,
          func_body);
    }

    vector<void *> vec_refs;
    ast->get_ref_in_scope(var_init, func_decl, &vec_refs);
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

      if (access_range_intrinsic_call != nullptr) {
        ast->ReplaceExp(func_call, ast->CopyExp(access_range_intrinsic_call));
      } else {
        ast->RemoveStmt(ast->TraceUpToStatement(func_call));
      }
    }
  }
}

//  1.1 Find all the parallelized iterator variables
int CAutoDimInterchange::get_parallel_variables_from_paralel_loops(
    void *init_name, void *scope, set<void *> *p_para_iter) {
  if (init_name == nullptr) {
    return 0;
  }

  vector<void *> v_ref;
  ast->get_ref_in_scope(init_name, scope, &v_ref);

  for (auto ref : v_ref) {
    if (is_in_range_intrisic(ref)) {
      continue;
    }
    int arg = ast->GetFuncCallParamIndex(ref);
    if (-1 != arg) {
      void *sg_call = ast->TraceUpToFuncCall(ref);
      if (sg_call == nullptr) {
        continue;
      }
      void *sg_decl = ast->GetFuncDeclByCall(sg_call);
      if (sg_decl == nullptr) {
        continue;
      }

      void *sg_param = ast->GetFuncParam(sg_decl, arg);
      if (sg_param == nullptr) {
        continue;
      }

      void *func_recur = nullptr;
      if (ast->IsRecursiveFunction(sg_decl, &func_recur) != 0) {
        cout << " ERROR: found a recursive function: " << ast->_p(func_recur)
             << endl;
        return 0;
      }

      p_para_iter->insert(sg_param);

      int ret = get_parallel_variables_from_paralel_loops(sg_param, sg_decl,
                                                          p_para_iter);
      if (ret == 0) {
        return 0;
      }
    }
  }

  return 1;
}

int CAutoDimInterchange::auto_dim_interchange_collect_parallel_dim_access(
    void *kernel, map<void *, set<int>> *p_parallel_access) {
  //  1. Find all fully parallelized loops
  set<void *> para_loops;
  {
    vector<CMirNode *> vec_nodes;
    mars_ir->get_topological_order_nodes(&vec_nodes);

    for (auto one_node : vec_nodes) {
      if (!one_node->is_function && one_node->is_complete_unroll()) {
        void *sg_loop = ast->GetParent(one_node->ref);

        if (ast->IsForStatement(sg_loop) == 0) {
          continue;
        }
        void *sg_iter = ast->GetLoopIterator(sg_loop);
        if (sg_iter == nullptr) {
          continue;
        }

        cout << " -- Find a parallel loop: " << ast->_p(sg_loop) << endl;
        para_loops.insert(sg_loop);
      }
    }
  }
  if (para_loops.empty()) {
    return 0;  //  do not need to do anything in this kernel
  }

  //  1.1 Find all the parallelized iterator variables
  //  a. from function call
  //  b. from CMarsExpression induction
  set<void *> para_iters;
  for (auto one_para_loop : para_loops) {
    void *init = ast->GetLoopIterator(one_para_loop);
    if (init == nullptr) {
      continue;
    }

    para_iters.insert(one_para_loop);  //  if it is a loop add the loop,
                                       //  otherwise the init_var
    int ret = get_parallel_variables_from_paralel_loops(init, one_para_loop,
                                                        &para_iters);
    if (ret == 0) {
      return 0;
    }
  }

  //  2. Check all the reference to find the dimensions that is a fully
  //  parallized loop iterator
  vector<void *> v_var_ref;
  bool is_compatible = false;
  bool is_cross_func = true;
  ast->GetNodesByType(kernel, "preorder", "SgVarRefExp", &v_var_ref,
                      is_compatible, is_cross_func);

  for (auto one_ref : v_var_ref) {
    void *array = nullptr;
    void *pntr = nullptr;
    vector<void *> indexes;
    int pointer_dim = -1;
    bool simplify_idx = true;
    int adjusted_array_dim = -1;
    ast->parse_pntr_ref_by_array_ref(one_ref, &array, &pntr, &indexes,
                                     &pointer_dim, one_ref, simplify_idx,
                                     adjusted_array_dim);

    if ((array == nullptr) || (pntr == nullptr)) {
      continue;
    }
    int init_dim =
        ast->get_dim_num_from_var(array);  //  ast->get_pntr_init_dim(pntr);
    //  shall we handle for one-dimension array ?
    if (init_dim <= 0) {
      continue;
    }

    set<int> parallel_dims;

    if (p_parallel_access->find(pntr) != p_parallel_access->end()) {
      parallel_dims = (*p_parallel_access)[pntr];
    }

#ifdef PROJDEBUG
    cout << "   -- Find an access '" << ast->_up(pntr)
         << "'(type_dim=" << my_itoa(init_dim) << "): ";
    for (auto idx : indexes) {
      cout << ast->_up(idx) << ",";
    }
    cout << "pointer_dim=" << my_itoa(pointer_dim) << endl;
#endif
    //  NOTE: ZP: count from right can help avoid the dim alignment issue
    //  between partial pointer alias
    int n_idx = init_dim - 1;            //  index count from right to left
    assert(indexes.size() == init_dim);  //  this is guaranteed in the impl of
                                         //  parse_pntr_xxx() above;
    for (auto idx : indexes) {           //  from left to right
      if (ast->IsVarRefExp(idx) != 0) {
        bool is_cross_func = true;
        void *loop =
            ast->GetLoopFromIteratorByPos(ast->GetVariableInitializedName(idx),
                                          idx, static_cast<int>(is_cross_func));

        void *init = ast->GetVariableInitializedName(idx);

        if (para_loops.find(loop) != para_loops.end() ||
            para_iters.find(init) != para_iters.end()) {
          if (parallel_dims.find(n_idx) == parallel_dims.end()) {
            parallel_dims.insert(n_idx);
          }
#ifdef PROJDEBUG
          cout << "     -- Find a parallel dim '" << ast->_up(array) << "'("
               << my_itoa(n_idx) << ")': " << ast->_p(idx) << endl;
#endif
        }
      }
      n_idx--;
    }

    if (!parallel_dims.empty()) {
      if (p_parallel_access->find(pntr) == p_parallel_access->end()) {
        p_parallel_access->insert(pair<void *, set<int>>(pntr, parallel_dims));
      } else {
        //  when will we run here??
        (*p_parallel_access)[pntr] = parallel_dims;
      }
    }
  }

  return 1;
}

//  For each array, expand it to all its associated variables (function and/or
//  pointer alias) â 1h
//     a.    Skip transform if unknown alias detected (incomplete alias
//     detection)
//  when will the function return zero?
int CAutoDimInterchange::auto_dim_interchange_collect_associated_arrays(
    void *kernel, const map<void *, set<int>> &parallel_access,
    map<set<void *>, set<int>> *p_parallel_array_dims) {
  for (auto it : parallel_access) {
    void *pntr = it.first;
    set<int> dims = it.second;

    void *array = ast->get_array_from_pntr(pntr);
    if (array == nullptr) {
      continue;
    }

    set<void *> v_alias;
    set<void *> visited_func;
    int ret = alias_induction_in_scopes(array, ast->GetProject(), &v_alias,
                                        &visited_func);
    if (ret == 0) {
      continue;
    }

    //  alias combining algorithm:
    //  combining the sets that has overlap with current set,
    //  and then insert the current set into the combined new set
    map<set<void *>, set<int>> new_map;
    set<void *> new_alias_set;
    set<int> new_dim_set;
    for (auto &&[one_alias_set, one_dim_set] : *p_parallel_array_dims) {
      //  check if intersecton v_alias and alias_set is empty
      bool is_intersect = false;
      for (auto it : v_alias) {
        if (one_alias_set.end() != one_alias_set.find(it)) {
          is_intersect = true;
          break;
        }
      }

      //  merge to new_ set if intersect, else copy the set into new_map
      if (is_intersect) {
        new_alias_set.insert(one_alias_set.begin(), one_alias_set.end());
        new_dim_set.insert(one_dim_set.begin(), one_dim_set.end());
      } else {
        new_map.insert(pair<set<void *>, set<int>>(one_alias_set, one_dim_set));
      }
    }
    new_alias_set.insert(v_alias.begin(), v_alias.end());
    new_dim_set.insert(dims.begin(), dims.end());
    new_map.insert(pair<set<void *>, set<int>>(new_alias_set, new_dim_set));
    *p_parallel_array_dims = new_map;
  }

#if PROJDEBUG
  cout << " -- Alias analysis: (alias_set -> dim_set)" << endl;
  for (auto &&[alias_set, dim_set] : *p_parallel_array_dims) {
    cout << "   -- ";
    for (auto array : alias_set) {
      cout << ast->_up(array) << ",";
    }
    cout << " -> ";
    for (auto dim : dim_set) {
      cout << my_itoa(dim) << ",";
    }
    cout << endl;
  }
#endif

  return 1;
}

//  Step 3.5 Calculate common_dims;
//  In pre_check stage: the following limitation checkers are disabled
int CAutoDimInterchange::auto_dim_interchange_calculate_common_dims(
    void *kernel,
    map<set<void *>, set<int>> *p_parallel_array_dims,  //  input/output
    map<void *, vector<size_t>> *p_common_dims,         //  output
    int pre_check) {
  map<set<void *>, set<int>> output;

  p_common_dims->clear();

  cout << " -- Calculate Common Dimension: " << endl;

  for (auto &&[alias_set, dim_set] : *p_parallel_array_dims) {
    //  string base_type_info = "";
    vector<size_t> common_dim_sizes;
    common_dim_sizes.clear();  //  left to right
    int invalid_set = 0;
    string invalid_reason = "unknown reason";

    int max_para_dim = -1;
    for (auto i : dim_set) {
      if (i > max_para_dim) {
        max_para_dim = i;
      }
    }

    for (auto array : alias_set) {
      //  c. Dim number/size not match
      {
        void *sg_type = ast->GetTypebyVar(array);
        void *base_type;
        vector<size_t> one_dim_size;
        int ret = ast->get_type_dimension(sg_type, &base_type,
                                          &one_dim_size);  //  right to left

        if (ret == 0) {
          goto end_of_checker_c;
        }

        // TODO(youxiang): also need to compare the partial pointer partial
        // array dims
        //  process only full array variables (pointers alias are excluded in
        //  the checker)
        for (auto t : one_dim_size) {
          if ((static_cast<int>(t)) < 0) {
            invalid_reason = "Unknown dimension size '" + ast->_up(array) +
                             ":" + ast->_up(sg_type) + "'";
            invalid_set = 1;
            break;
          }
          //  if (t == 0) goto end_of_checker_c; //  exclude pointers
        }

        //  dim match algorithm
        //  1) match the common length (min of common_dim_sizes and
        //  one_dim_size)
        //    1.1) for pointer, only the highest dim can be zero
        //  2) if one_dim_size is longer than common_dim_sizes, update
        //  common_dim_sizes
        {
          int dim_mismatch = 0;
          int i = 0;

          //  Function: update common_dim_sizes with info from one_dim_size and
          //  check mismatch
          if (dim_mismatch == 0) {
#define ignore_leftmost_dim_of_pointer 1
            for (i = 0; i < common_dim_sizes.size() && i <= max_para_dim; i++) {
              if (i >= one_dim_size.size()) {
                break;
              }

#if ignore_leftmost_dim_of_pointer
              if (i == one_dim_size.size() - 1 && one_dim_size[i] == 0) {
                continue;
              }
#endif
              if (one_dim_size[i] != common_dim_sizes[i]) {
                dim_mismatch = 1;
                break;
              }
            }
            if (dim_mismatch == 0) {
              for (; i < one_dim_size.size() && i <= max_para_dim; i++) {
#if ignore_leftmost_dim_of_pointer
                if (i == one_dim_size.size() - 1 && one_dim_size[i] == 0) {
                  continue;
                }
#endif
                common_dim_sizes.push_back(one_dim_size[i]);  //  right to left
              }
            }
          }

          if (dim_mismatch != 0) {
            string one_dim_info = ast->_up(array) + ":";
            std::reverse(
                one_dim_size.begin(),
                one_dim_size.end());  //  show it in left to right order
            for (auto t : one_dim_size) {
              one_dim_info += my_itoa(t) + ",";
            }
            string common_dim_info = "common:";
            vector<size_t> tt = common_dim_sizes;
            std::reverse(tt.begin(),
                         tt.end());  //  show it in left to right order
            for (auto t : tt) {
              common_dim_info += my_itoa(t) + ",";
            }
            invalid_reason = "Dimension size mismatch '" + common_dim_info +
                             "' vs '" + one_dim_info + "'";
            invalid_set = 1;
            break;
          }
        }
      end_of_checker_c : {}
      }
    }

    int has_pointer_dim = 0;
    if (invalid_set == 0) {
      for (auto t : common_dim_sizes) {
        if (t == 0) {
          has_pointer_dim = 1;
        }
      }
    }

    if (has_pointer_dim != 0) {
      string common_dim_info;
      vector<size_t> tt = common_dim_sizes;
      std::reverse(tt.begin(), tt.end());  //  show it in left to right order
      for (auto t : tt) {
        common_dim_info += my_itoa(t) + ",";
      }
      invalid_reason = "Common dimension has zero: '" + common_dim_info + "'";
      invalid_set = 1;
    }

    if (invalid_set == 0) {
      output.insert(pair<set<void *>, set<int>>(alias_set, dim_set));

      std::reverse(common_dim_sizes.begin(),
                   common_dim_sizes.end());  //  store it in left to right order
      for (auto array : alias_set) {
        p_common_dims->insert(
            pair<void *, vector<size_t>>(array, common_dim_sizes));
      }

      cout << "   -- ";
      for (auto array : alias_set) {
        cout << ast->_up(array) << ",";
      }
      cout << ": parallel_dim=";
      for (auto dim : dim_set) {
        cout << my_itoa(dim) << ",";
      }

      cout << " common_type_dim=";
      for (auto dim : common_dim_sizes) {
        cout << my_itoa(dim) << ",";
      }
      cout << endl;

    } else {
      cout << "   ++ ";
      for (auto array : alias_set) {
        cout << ast->_up(array) << ",";
      }
      cout << ": parallel_dim=";
      for (auto dim : dim_set) {
        cout << my_itoa(dim) << ",";
      }
      cout << " (skipped due to " << invalid_reason << ")" << endl;
    }
  }

  *p_parallel_array_dims = output;
  return 1;
}

int CAutoDimInterchange::get_offset_var_set_recur(void *array, int max_para_dim,
                                                  set<void *> *p_offset_var_set,
                                                  bool has_offset_propagate,
                                                  string *p_invalid_reason) {
  if (p_offset_var_set->find(array) != p_offset_var_set->end()) {
    return 1;
  }

  vector<void *> v_ref;
  ast->get_ref_in_scope(array, nullptr, &v_ref);
  for (auto one_ref : v_ref) {
    if (is_in_range_intrisic(one_ref)) {
      continue;
    }
    void *one_pntr;
    void *sg_array;
    vector<void *> indexes;
    int pointer_dim;
    ast->parse_pntr_ref_by_array_ref(one_ref, &sg_array, &one_pntr, &indexes,
                                     &pointer_dim, one_ref);

    if ((one_pntr == nullptr) || sg_array != array) {
      *p_invalid_reason = "Assert -- Null pntr found '" + ast->_p(array) +
                          "' line=" + my_itoa(ast->get_line(one_ref));
      return 0;
    }

    if (ast->get_pntr_init_dim(one_pntr) == ast->get_pntr_pntr_dim(one_pntr)) {
      continue;
    }

    void *parent = ast->GetParent(one_pntr);
    if ((parent != nullptr) && (ast->IsAssignOp(parent) != 0) &&
        one_pntr == ast->GetExpLeftOperand(parent)) {
      continue;
    }

    //  Lvalue: Detect alias_var in four cases
    //  a. Function arg
    //  b. assignOp
    //  c. initial assign
    void *alias_var = ast->get_alias_array_from_assignment_expression(one_pntr);

    //  d. exp stmt (useful in test)
    if (ast->IsExprStatement(ast->GetParent(one_pntr)) != 0) {
      continue;
    }

    if (alias_var == nullptr) {
      *p_invalid_reason = "Pntr position is unrecognized '" +
                          ast->_p(one_pntr) +
                          "' line=" + my_itoa(ast->get_line(one_ref));
      return 0;
    }
    int ret = get_offset_var_set_recur(
        alias_var, max_para_dim, p_offset_var_set, true, p_invalid_reason);
    if (ret == 0) {
      return 0;
    }
  }

  if (has_offset_propagate) {
    if (p_offset_var_set->find(array) == p_offset_var_set->end()) {
      p_offset_var_set->insert(array);
    }
  }

  return 1;
}

//  input
//    int a[16][32];
//    a[i][j];
//  output
//    int a[16][32];
//    int a_offset0 = 0;
//    int a_offset1 = 0;
//    a[i+a_offset0][j+a_offset1];
int CAutoDimInterchange::apply_add_offset_variable(
    void *kernel, const set<void *> &alias_set, int max_para_dim,  //  input
    map<void *, vector<void *>> *p_offset_vars,                    //  output
    string *p_invalid_reason) {
  p_offset_vars->clear();

  //  1. find the arrays to add offset variables
  //  Condition:
  //  If a variable (A) is the alias of the other array, variable A needs to
  //  have offset variables a. A has partial pntr as initilizer b. A's reference
  //  is assigned by partial pntr c. A is the function parameter that has
  //  partial pntr as actual argument
  {
    set<void *> offset_var_set;
    for (auto array : alias_set) {
      int ret = get_offset_var_set_recur(array, max_para_dim, &offset_var_set,
                                         false, p_invalid_reason);
      if (ret == 0) {
        return 0;
      }
    }
    for (auto var : offset_var_set) {
      vector<void *> empty;
      p_offset_vars->insert(pair<void *, vector<void *>>(var, empty));
    }
  }

  //  2. add offset variable declarations and references
  //  map<void*, vector<void*> > offset_vars,//  output
  {
    for (auto &&[alias_var, offset_inits] : *p_offset_vars) {
      //  2.1. add offset vars init_name and initialization values
      vector<void *> one_offset_vars;
      {
        if (ast->IsLocalInitName(alias_var) != 0) {
          void *alias_var_decl = ast->TraceUpToStatement(alias_var);

          void *insert_pos = ast->CreateBasicBlock();

          if (ast->IsBasicBlock(ast->TraceUpToScope(alias_var_decl)) == 0) {
            *p_invalid_reason = "Insert position invalid: " +
                                ast->_p(ast->TraceUpToScope(alias_var_decl));
            return 0;
          }

          ast->InsertStmt(insert_pos, alias_var_decl);

          void *alias_scope = ast->GetScope(alias_var);
          for (int i = 0; i <= max_para_dim; i++) {
            string offset_var_name =
                "__" + ast->_up(alias_var) + "_offset" + my_itoa(i);
            offset_var_name =
                ast->GetSafeNewVarName(offset_var_name, alias_scope);

            void *decl = ast->CreateVariableDecl(
                "int", offset_var_name, ast->CreateConst(0), alias_scope);
            ast->InsertStmt(decl, insert_pos);

            one_offset_vars.push_back(ast->GetVariableInitializedName(decl));
          }
          ast->RemoveStmt(insert_pos);
        } else if (ast->IsArgumentInitName(alias_var) != 0) {
          void *func_decl = ast->TraceUpToFuncDecl(alias_var);
          one_offset_vars.resize(1 + max_para_dim);
          for (int i = 0; i <= max_para_dim; i++) {
            string offset_var_name = "__" + ast->_up(alias_var) + "_offset" +
                                     my_itoa(max_para_dim - i);  //  my_itoa(i);
            offset_var_name =
                ast->GetSafeNewVarName(offset_var_name, func_decl);

            void *new_arg = ast->CreateVariable(ast->GetTypeByString("int"),
                                                offset_var_name);
            ast->insert_param_to_decl(func_decl, new_arg, true);
            //  from right
            one_offset_vars[max_para_dim - i] =
                new_arg;  //  .push_back(new_arg);

            vector<void *> v_calls;
            ast->GetFuncCallsFromDecl(func_decl, nullptr, &v_calls);
            for (auto call : v_calls) {
              ast->insert_argument_to_call(call, ast->CreateConst(0));
            }
          }

        } else {
          cout << " ERROR: unrecognized init name: " << ast->_p(alias_var)
               << endl;
          return 0;
        }
      }
      offset_inits = one_offset_vars;

      string var_info;
      for (auto tt : one_offset_vars) {
        var_info += ast->_up(tt) + ",";
      }
      cout << "   -- add offset variable: " << ast->_p(alias_var)
           << " max_para_dim=" << max_para_dim << endl;

      //  2.2 update alias_var pntr indexes
      {
        vector<void *> v_ref;
        ast->get_ref_in_scope(alias_var, nullptr, &v_ref);
        std::reverse(v_ref.begin(), v_ref.end());
        for (auto one_ref : v_ref) {
          if (is_in_range_intrisic(one_ref)) {
            continue;
          }
          void *one_pntr;
          void *sg_array;
          vector<void *> indexes;
          int pointer_dim;
          ast->parse_pntr_ref_by_array_ref(one_ref, &sg_array, &one_pntr,
                                           &indexes, &pointer_dim, one_ref);
          int array_dim = ast->get_pntr_init_dim(one_pntr);

          int new_pointer_dim = pointer_dim;
          int dim_ext = (max_para_dim + 1) - array_dim;
          if (dim_ext < 0) {
            dim_ext = 0;
          }
          for (int i = 0; i < dim_ext; i++) {
            indexes.insert(indexes.begin(), ast->CreateConst(0));
#define _EXTEND_PNTR_DIM_IN_ADVANCE_ 1
#if _EXTEND_PNTR_DIM_IN_ADVANCE_
            new_pointer_dim++;
#endif
          }

          //  FIX: skip adding offset for left single_var value reference
          {
            void *parent = ast->GetParent(one_pntr);
            if ((parent != nullptr) && (ast->IsAssignOp(parent) != 0)) {
              if (one_pntr == ast->GetExpLeftOperand(parent)) {
                if (ast->IsVarRefExp(one_pntr) != 0) {
                  continue;
                }
              }
            }
          }

          string old_pntr_info = ast->_up(one_pntr) + "--";
          {} {
            for (auto t : indexes) {
              old_pntr_info += ast->_up(t) + ",";
            }
          }
          old_pntr_info += "(pd=" + my_itoa(pointer_dim) + ")";

          if ((one_pntr == nullptr) || sg_array != alias_var) {
            return 0;
          }

          vector<void *> new_indexes_cp;
          for (auto idx : indexes) {
            new_indexes_cp.push_back(ast->CopyExp(idx));
          }

          for (int i = 0; i <= max_para_dim; i++) {
            //  process from right dims to left
            auto idx = indexes[indexes.size() - 1 - i];
            void *new_idx = nullptr;

            int64_t value;
            if (ast->IsConstantInt(idx, &value, false, nullptr) && value == 0) {
              new_idx = ast->CreateVariableRef(one_offset_vars[i]);
            } else {
              new_idx =
                  ast->CreateExp(V_SgAddOp, ast->CopyExp(idx),
                                 ast->CreateVariableRef(one_offset_vars[i]));
            }
            new_indexes_cp[indexes.size() - 1 - i] = new_idx;
          }

          void *new_pntr_var_ref = ast->CreateVariableRef(sg_array);
          void *new_pntr =
              ast->CreateArrayRef(new_pntr_var_ref, new_indexes_cp);

          add_addressof_and_simplify(
              &new_pntr,
              static_cast<int64_t>(indexes.size() - new_pointer_dim));
          ast->ReplaceExp(one_pntr, new_pntr);

          {
            void *one_pntr;
            void *sg_array;
            vector<void *> d_indexes;
            int pointer_dim;
            bool simplify_idx = true;
            int adjusted_array_dim = max_para_dim + 1;
            ast->parse_pntr_ref_by_array_ref(new_pntr_var_ref, &sg_array,
                                             &one_pntr, &d_indexes,
                                             &pointer_dim, new_pntr_var_ref,
                                             simplify_idx, adjusted_array_dim);

            string new_pntr_info = ast->_up(new_pntr);  //  + "\n ";
            //  {
            //  for (auto t : d_indexes) new_pntr_info += ast->_up(t) + ",";
            //  }
            new_pntr_info += "(pd=" + my_itoa(pointer_dim) + ")";

            cout << "     -- insert offset into pntr: " << old_pntr_info
                 << endl;

            cout << "        -> " << new_pntr_info << endl;
          }
        }
      }
    }
  }

  return 1;
}

//  input
//    int * a = b[i][j];
//  output
//    int * a = b[0][0];
//    a_offset0 = j;
//    a_offset1 = i;
int CAutoDimInterchange::apply_shift_offset_one_rvalue(
    void *rvalue, int max_para_dim, vector<void *> offset_inits,
    string *p_invalid_reason) {
  if (rvalue == nullptr) {
    cout << "ASSERT FAIL: " << __FILE__ << ":" << __LINE__ << endl;
    return 0;
  }

  void *sg_array;
  vector<void *> indexes;
  int pointer_dim;
  void *one_pntr;
  {
    CMarsExpression me(rvalue, ast);
    if ((me == 0) != 0) {
      return 1;
    }
    void *var_ref = nullptr;
    int simplify_idx = 0;
    int adjusted_array_dim = max_para_dim + 1;
    if (ast->parse_array_ref_from_pntr(rvalue, &var_ref, rvalue,
                                       simplify_idx != 0,
                                       adjusted_array_dim) == 0) {
      *p_invalid_reason = "Invalid partial pointer rvalue: " + ast->_p(rvalue);
      //  cout << "ASSERT FAIL: " << __FILE__ << ":" << __LINE__ << endl;
      return 0;
    }
    ast->parse_pntr_ref_by_array_ref(var_ref, &sg_array, &one_pntr, &indexes,
                                     &pointer_dim, rvalue, simplify_idx != 0,
                                     adjusted_array_dim);
    if (one_pntr != rvalue) {
      cout << "ASSERT FAIL: " << __FILE__ << ":" << __LINE__ << endl;
      return 0;
    }
  }

  //  1. set index to zero for the pntr
  vector<void *> offset_values;
  string step1_info;
  {
    vector<void *> new_indexes_cp;
    for (size_t i = 0; i < indexes.size(); i++) {
      if (i < indexes.size() - (max_para_dim + 1)) {
        new_indexes_cp.push_back(ast->CopyExp(indexes[i]));
      } else {
        offset_values.push_back(ast->CopyExp(indexes[i]));
        new_indexes_cp.push_back(ast->CreateConst(0));
      }
    }

    void *new_pntr_var_ref = ast->CreateVariableRef(sg_array);
    void *new_pntr = ast->CreateArrayRef(new_pntr_var_ref, new_indexes_cp);

    add_addressof_and_simplify(
        &new_pntr, static_cast<int64_t>(indexes.size() - pointer_dim));
    step1_info = ast->_up(one_pntr) + " -> ";
    ast->ReplaceExp(one_pntr, new_pntr);
    step1_info += ast->_up(new_pntr);
    one_pntr = new_pntr;
    rvalue = nullptr;  //  force to use one_pntr below
  }

  //  2. set the offset value to the lvalue offset variables
  string step2_info;
  if (offset_inits.empty()) {
    return 1;
  }

  if (-1 == ast->GetFuncCallParamIndex(one_pntr)) {
    void *insert_pos = ast->TraceUpToStatement(one_pntr);
    vector<void *> alias_offset_vars = offset_inits;

    if (ast->IsBasicBlock(ast->TraceUpToScope(insert_pos)) == 0) {
      *p_invalid_reason = "Insert position invalid: " +
                          ast->_p(ast->TraceUpToScope(insert_pos));
      return 0;
    }

    for (int i = 0; i <= max_para_dim; i++) {
      void *offset_init = alias_offset_vars[i];
      void *sg_offset_value =
          (i < max_para_dim + 1 - offset_values.size())
              ? ast->CreateConst(0)
              : ast->CopyExp(offset_values[max_para_dim - i]);

      void *assignop = ast->CreateExp(
          V_SgAssignOp, ast->CreateVariableRef(offset_init), sg_offset_value);
      void *stmt = ast->CreateStmt(V_SgExprStatement, assignop);

      ast->InsertAfterStmt(stmt, insert_pos);
      step2_info += "       " + ast->_up(stmt) + "\n";
    }
  } else {  //  function argument
    void *func_call = ast->TraceUpToFuncCall(one_pntr);

    vector<void *> alias_offset_vars = offset_inits;

    for (int i = 0; i <= max_para_dim; i++) {
      void *offset_init = alias_offset_vars[i];
      void *sg_offset_value =
          (i < max_para_dim + 1 - offset_values.size())
              ? ast->CreateConst(0)
              : ast->CopyExp(offset_values[max_para_dim - i]);

      void *offset_param;  //  from offset_init
      {
        int arg_idx = ast->GetFuncParamIndex(offset_init);
        offset_param = ast->GetFuncCallParam(func_call, arg_idx);
      }
      if (offset_param == nullptr) {
        *p_invalid_reason = "Insert position invalid: " + ast->_p(func_call);
        return 0;
      }

      ast->ReplaceExp(offset_param, sg_offset_value);
    }
    step2_info += "       " + ast->_up(func_call) + "\n";
  }

  cout << "   -- shift offset: " << step1_info << endl;
  cout << step2_info;
  return 1;
}

//  input
//    int * a = b[i][j];
//  output
//    int * a = b[0][0];
//    a_offset0 = j;
//    a_offset1 = i;
int CAutoDimInterchange::apply_shift_offset_to_variable(
    void *kernel,
    int max_para_dim,                                //  input
    const map<void *, vector<void *>> &offset_vars,  //  input
    string *p_invalid_reason) {
  for (auto &&[array, offset_inits] : offset_vars) {
    //  Find all the rvalues which is assigned to array
    //  a. if array has initilizer, shift the initilizeOprand
    //  b. if array is a local, shift all the rvalues that write to array
    //  c. if array is a func param, shift all the actual argument to offset
    //  args

    //  a. if array has initilizer, shift the initilizeOprand
    if (ast->GetInitializer(array) != nullptr) {
      void *rvalue_exp = ast->GetInitializerOperand(ast->GetInitializer(array));
      if (rvalue_exp != nullptr) {
        int ret = apply_shift_offset_one_rvalue(rvalue_exp, max_para_dim,
                                                offset_inits, p_invalid_reason);
        if (ret == 0) {
          return 0;
        }
      }
    }

    //  b. if array is a local, shift all the rvalues that write to array
    {
      vector<void *> v_ref;
      ast->get_ref_in_scope(array, nullptr, &v_ref);
      std::reverse(v_ref.begin(), v_ref.end());
      for (auto one_ref : v_ref) {
        if (is_in_range_intrisic(one_ref)) {
          continue;
        }
        void *parent = ast->GetParent(one_ref);
        if ((ast->IsAssignOp(parent) != 0) &&
            one_ref == ast->GetExpLeftOperand(parent)) {
          void *rvalue_exp = ast->GetExpRightOperand(parent);
          int ret = apply_shift_offset_one_rvalue(
              rvalue_exp, max_para_dim, offset_inits, p_invalid_reason);
          if (ret == 0) {
            return 0;
          }
        }
      }
    }

    //  c. if array is a func param, shift all the actual argument to offset
    //  args
    if (ast->IsArgumentInitName(array) != 0) {
      int arg_idx = ast->GetFuncParamIndex(array);
      void *decl = ast->TraceUpToFuncDecl(array);

      vector<void *> calls;
      ast->GetFuncCallsFromDecl(decl, nullptr, &calls);

      for (auto one_call : calls) {
        void *rvalue_exp = ast->GetFuncCallParam(one_call, arg_idx);

        int ret = apply_shift_offset_one_rvalue(rvalue_exp, max_para_dim,
                                                offset_inits, p_invalid_reason);
        if (ret == 0) {
          return 0;
        }
      }
    }
  }

  return 1;
}

bool CAutoDimInterchange::invalid_rvalue_for_dim_ext(void *rvalue,
                                                     int adjusted_array_dim,
                                                     string *p_invalid_reason) {
  void *var_ref = nullptr;
  bool simplify_idx = false;
  //  int adjusted_array_dim = -1;
  if (ast->parse_array_ref_from_pntr(rvalue, &var_ref, rvalue, simplify_idx,
                                     adjusted_array_dim) == 0) {
    *p_invalid_reason = "Invalid partial pointer rvalue: " + ast->_p(rvalue);
    return true;
  }
  return false;
}
//  input
//    int (* a)[16] = b[i][j];
//    ... = a[i][j];
//    a = ;
//  output
//    int (* a)[32][16] = b[i] + j;
//    ... = a[0][i][j];
//    a = ...;
int CAutoDimInterchange::apply_dim_extension(
    void *kernel, const map<void *, vector<size_t>> &common_dims,  //  input
    const map<void *, vector<void *>> &offset_vars,                //  input
    string *p_invalid_reason) {
  for (auto &&[array, offset_inits] : offset_vars) {
    string type_update_info;
    string rvalue_update_info;
    if (common_dims.find(array) == common_dims.end()) {
      cout << "ASSERT FAIL: " << __FILE__ << ":" << __LINE__ << endl;
      return 0;
    }

    vector<size_t> common = common_dims.find(array)->second;
    size_t new_array_dim = common.size();
    size_t array_dim = ast->get_dim_num_from_var(array);

    if (array_dim >= new_array_dim) {
      continue;
    }

    int64_t n_dim_ext = static_cast<int64_t>(new_array_dim - array_dim);

    //  1. Handle all the rvalues which is assigned to array
    //  a. if array has initilizer, shift the initilizeOprand
    //  b. if array is a local, shift all the rvalues that write to array
    //  c. if array is a func param, shift all the actual argument to offset
    //  args

    //  a. if array has initilizer, shift the initilizeOprand
    if (ast->GetInitializer(array) != nullptr) {
      void *rvalue_exp = ast->GetInitializerOperand(ast->GetInitializer(array));
      if (rvalue_exp != nullptr) {
        int adjusted_array_dim = new_array_dim;
        if (invalid_rvalue_for_dim_ext(rvalue_exp, adjusted_array_dim,
                                       p_invalid_reason)) {
          return 0;
        }
        void *new_rvalue_exp = ast->CopyExp(rvalue_exp);
        add_addressof_and_simplify(&new_rvalue_exp, n_dim_ext);
        rvalue_update_info += "        " + ast->_p(rvalue_exp) + " -> ";
        ast->ReplaceExp(rvalue_exp, new_rvalue_exp);
        rvalue_update_info += ast->_p(new_rvalue_exp) + "\n";
      }
    }

    //  b. if array is a local, shift all the rvalues that write to array
    {
      vector<void *> v_ref;
      ast->get_ref_in_scope(array, nullptr, &v_ref);
      std::reverse(v_ref.begin(), v_ref.end());
      for (auto one_ref : v_ref) {
        if (is_in_range_intrisic(one_ref)) {
          continue;
        }
        void *parent = ast->GetParent(one_ref);
        if ((ast->IsAssignOp(parent) != 0) &&
            one_ref == ast->GetExpLeftOperand(parent)) {
          void *rvalue_exp = ast->GetExpRightOperand(parent);
          int adjusted_array_dim = new_array_dim;
          if (invalid_rvalue_for_dim_ext(rvalue_exp, adjusted_array_dim,
                                         p_invalid_reason)) {
            return 0;
          }
          void *new_rvalue_exp = ast->CopyExp(rvalue_exp);
          add_addressof_and_simplify(&new_rvalue_exp, n_dim_ext);
          rvalue_update_info += "        " + ast->_p(rvalue_exp) + " -> ";
          ast->ReplaceExp(rvalue_exp, new_rvalue_exp);
          rvalue_update_info += ast->_p(new_rvalue_exp) + "\n";
        }
      }
    }

    //  c. if array is a func param, shift all the actual argument to offset
    //  args
    if (ast->IsArgumentInitName(array) != 0) {
      int arg_idx = ast->GetFuncParamIndex(array);
      void *decl = ast->TraceUpToFuncDecl(array);

      vector<void *> calls;
      ast->GetFuncCallsFromDecl(decl, nullptr, &calls);

      for (auto one_call : calls) {
        void *rvalue_exp = ast->GetFuncCallParam(one_call, arg_idx);
        int adjusted_array_dim = new_array_dim;
        if (invalid_rvalue_for_dim_ext(rvalue_exp, adjusted_array_dim,
                                       p_invalid_reason)) {
          return 0;
        }

        void *new_rvalue_exp = ast->CopyExp(rvalue_exp);
        add_addressof_and_simplify(&new_rvalue_exp, n_dim_ext);
        rvalue_update_info += "        " + ast->_p(rvalue_exp) + " -> ";
        ast->ReplaceExp(rvalue_exp, new_rvalue_exp);
        rvalue_update_info += ast->_p(new_rvalue_exp) + "\n";
      }
    }

    //  3. Change the type of the var
#if 1
    {
      vector<size_t> dim_org;  //  from left to right dim
      void *base_type = nullptr;
      ast->get_type_dimension_new(ast->GetTypebyVar(array), &base_type,
                                  &dim_org);  //  left to right

      vector<size_t> dim_new = common;
      if (dim_org[0] == 0) {
        dim_new[0] = 0;  //  convert back to pointer
      }

      //  Update the type
      {
        void *new_type =
            ast->CreateArrayType(base_type, dim_new);  //  left to right
        string str_old_type = ast->_up(ast->GetTypebyVar(array));
        ast->SetTypetoVar(array, new_type);
        string str_new_type = ast->_up(new_type);
        type_update_info += str_old_type + " -> " + str_new_type;
      }

      //  handling forwarding declaration
      if (ast->IsArgumentInitName(array) != 0) {
        void *one_decl = ast->TraceUpToFuncDecl(array);
        if (one_decl != nullptr) {
          vector<void *> vec_decls = ast->GetAllFuncDecl(one_decl);
          for (auto decl : vec_decls) {
            if (decl == one_decl) {
              continue;
            }
            void *new_decl =
                ast->CloneFuncDecl(one_decl, ast->GetGlobal(decl), false);
            ast->ReplaceStmt(decl, new_decl);
          }
        }
      }
    }
#endif

    //  2. Update the pntr of the var
    {
      vector<void *> v_ref;
      ast->get_ref_in_scope(array, nullptr, &v_ref);
      std::reverse(v_ref.begin(), v_ref.end());
      for (auto one_ref : v_ref) {
        if (is_in_range_intrisic(one_ref)) {
          continue;
        }
        void *parent = ast->GetParent(one_ref);
        //  excluding lvalue pntr
        if (!((ast->IsAssignOp(parent) != 0) &&
              one_ref == ast->GetExpLeftOperand(parent))) {
          void *rvalue_exp;
          vector<void *> indices;
          int pointer_dim;
          {
            void *d_pntr;
            void *d_array;
            bool simplify_idx = true;
            int adjusted_array_dim = -1;
            ast->parse_pntr_ref_by_array_ref(one_ref, &d_array, &d_pntr,
                                             &indices, &pointer_dim, one_ref,
                                             simplify_idx, adjusted_array_dim);
            if ((d_pntr == nullptr) || (d_array == nullptr)) {
              cout << "ASSERT FAIL: " << __FILE__ << ":" << __LINE__ << endl;
              return 0;
            }
            rvalue_exp = d_pntr;
          }

          void *new_rvalue_exp = ast->CopyExp(rvalue_exp);
          int new_array_dim = indices.size();  //  type transformation is done

          int new_pointer_dim = pointer_dim;

          //  reset low dim offsets to zero, it is legal on ExprStatements
          if (true) {
            if (ast->IsExprStatement(ast->GetParent(rvalue_exp)) != 0) {
              if (!(indices.size() >=
                    new_pointer_dim)) {  //  ensured if type has been extended
                cout << "ASSERT FAIL: " << __FILE__ << ":" << __LINE__ << endl;
                //  return 0; //  skip do this special handling
              } else {
                vector<void *> new_indexes_cp;
                for (auto idx : indices) {
                  new_indexes_cp.push_back(ast->CopyExp(idx));
                }
                for (size_t i = new_pointer_dim + 1; i < indices.size(); i++) {
                  new_indexes_cp[i] = ast->CreateConst(0);
                }
                void *new_pntr_var_ref = ast->CreateVariableRef(array);
                void *new_pntr =
                    ast->CreateArrayRef(new_pntr_var_ref, new_indexes_cp);
                new_rvalue_exp = new_pntr;
              }
              //  new_pointer_dim += n_dim_ext;
              add_addressof_and_simplify(
                  &new_rvalue_exp,
                  static_cast<int64_t>(new_array_dim - new_pointer_dim));
            }
          }

          rvalue_update_info +=
              "        " + ast->_p(rvalue_exp) + " \n          -> ";
          "(pd=" + my_itoa(pointer_dim) + ") -> ";
#if _EXTEND_PNTR_DIM_IN_ADVANCE_
#else
          new_pointer_dim += n_dim_ext;
          add_addressof_and_simplify(new_rvalue_exp, -n_dim_ext);
#endif
          ast->ReplaceExp(rvalue_exp, new_rvalue_exp);
          rvalue_update_info += ast->_p(new_rvalue_exp) +
                                "(pd=" + my_itoa(new_pointer_dim) + ")\n";
        }
      }
    }

    cout << "   -- dim extension: " << ast->_up(array) << ": "
         << type_update_info << endl;
    cout << rvalue_update_info;
  }

  return 1;
}

int CAutoDimInterchange::
    auto_dim_interchange_apply_dim_extension_and_offset_split(
        void *kernel,
        map<set<void *>, set<int>> *p_parallel_array_dims,  //  input/out
        const map<void *, vector<size_t>> &common_dims) {   //  input
  cout << " -- Apply Offset Split and Dimension Extension: " << endl;

  map<set<void *>, set<int>> output;
  vector<set<void *>> vec_alias_set;
  for (auto &&[alias_set, dim_set] : *p_parallel_array_dims) {
    vec_alias_set.push_back(alias_set);
  }
  sort(vec_alias_set.begin(), vec_alias_set.end(), [&](auto a, auto b) {
    string s_a;
    for (auto v : a) {
      s_a += ast->_up(v);
    }
    string s_b;
    for (auto v : b) {
      s_b += ast->_up(v);
    }
    return s_a < s_b;
  });

  for (auto alias_set : vec_alias_set) {
    auto dim_set = (*p_parallel_array_dims)[alias_set];
    string invalid_reason = "Unknown reason";
    int invalid_set = 0;
    int max_para_dim = -1;
    for (auto i : dim_set) {
      if (i > max_para_dim) {
        max_para_dim = i;
      }
    }

    map<void *, vector<void *>> offset_vars;
    if (invalid_set == 0) {
      invalid_set = static_cast<int>(static_cast<int>(apply_add_offset_variable(
                                         kernel, alias_set, max_para_dim,
                                         &offset_vars, &invalid_reason)) == 0);
    }
    if (invalid_set == 0) {
      invalid_set = static_cast<int>(
          static_cast<int>(apply_shift_offset_to_variable(
              kernel, max_para_dim, offset_vars, &invalid_reason)) == 0);
    }
    ast->init_defuse();
    if (invalid_set == 0) {
      invalid_set = static_cast<int>(
          static_cast<int>(apply_dim_extension(kernel, common_dims, offset_vars,
                                               &invalid_reason)) == 0);
    }

    if (invalid_set == 0) {
      output.insert(pair<set<void *>, set<int>>(alias_set, dim_set));
      //  cout << "   -- " ;
      //  for (auto array : alias_set) cout << ast->_up(array) << ",";
      //  cout << ": parallel_dim=" ;
      //  for (auto dim: dim_set) cout << my_itoa(dim) << ",";
      //  cout << endl;
    } else {
      cout << "   ++ ";
      for (auto array : alias_set) {
        cout << ast->_up(array) << ",";
      }
      cout << ": parallel_dim=";
      for (auto dim : dim_set) {
        cout << my_itoa(dim) << ",";
      }
      cout << " (skipped due to " << invalid_reason << ")" << endl;
    }
  }

  *p_parallel_array_dims = output;

  return 1;
}

int CAutoDimInterchange::check_pntr_nonzero_idx_condition(
    void *array, void *pntr, map<void *, vector<size_t>> common_dims,
    set<int> dim_set, string *p_invalid_reason) {
  int64_t n_dim_ext = 0;
  if (array != nullptr) {  // consider dim ext if there is associated alias
    assert(common_dims.count(array) && "Comment below should be enabled");
    //  MER763 Min Gao: suppose not going to happen
    //  if (common_dims.find(array) == common_dims.end()) {
    //  invalid_reason = "Assert -- can not find 'array' in common_dim '" +
    //                   ast->_p(array) +
    //                   "' line=" + my_itoa(ast->get_line(array));
    //  return 0;
    //  }
    vector<size_t> common = common_dims[array];

    int array_dim = ast->get_dim_num_from_var(array);
    n_dim_ext = common.size() - array_dim;
    if (n_dim_ext < 0) {
      n_dim_ext = 0;
    }
  }
  int pntr_dim = ast->get_pntr_pntr_dim(pntr);
  pntr_dim -= n_dim_ext;

  if (pntr_dim < 0) {
    *p_invalid_reason = "Not enough pointer dim for dimension extension '" +
                        ast->_p(pntr) +
                        "' line=" + my_itoa(ast->get_line(pntr));
    return 0;
  }

  void *sg_var_ref;
  ast->parse_array_ref_from_pntr(pntr, &sg_var_ref);
  if (sg_var_ref == nullptr) {
    return 0;
  }
  void *d_pntr;
  void *d_array;
  vector<void *> indices;
  int pointer_dim;
  ast->parse_pntr_ref_by_array_ref(sg_var_ref, &d_array, &d_pntr, &indices,
                                   &pointer_dim, pntr);
  if ((d_pntr == nullptr) || (d_array == nullptr)) {
    return 0;
  }

  int array_dim = ast->get_dim_num_from_var(d_array);

  for (int i = static_cast<int>(indices.size()); i < array_dim; i++) {
    indices.push_back(nullptr);
  }

  vector<int> simple_indexes;
  for (auto idx : indices) {
    int64_t value = -1;
    if (nullptr == idx ||
        (ast->IsConstantInt(idx, &value, true, nullptr) && value == 0)) {
      simple_indexes.push_back(0);
    } else {
      simple_indexes.push_back(1);
    }
  }

  set<int> dim_set_from_left;
  for (auto t : dim_set) {
    dim_set_from_left.insert(simple_indexes.size() - 1 - t);
  }
  vector<int> dim_new = dim_switch<int>(simple_indexes, dim_set_from_left);

  for (size_t i = pntr_dim + 1; i < dim_new.size(); i++) {
    if (dim_new[i] != 0) {
      *p_invalid_reason = "Transformed pntr has non-zero offset  '" +
                          ast->_p(pntr) +
                          "' line=" + my_itoa(ast->get_line(pntr));
      return 0;
    }
  }

  return 1;
}

//  Function:
//  - Filtering out the situations that cannot(not need) apply dim interchange:
//  - 4h
//  Filter conditions
//  1. Exclude recursive pointer alias
//  2. Exclude the situations that recursive check has uncertain results
//    (See details in IsRecrusivePointerAlias())
//    - Only the follow partial pntr ref is considereds (otherwise non-confident
//    result):
//      a. Function argument
//      b. Initializer
//      c. assignOp with single ref lvalue in an SgExprStatement
//    - Excluded cases includes (but not limited to):
//      a. p++, p+=
//      b. pa = pb = pc;
//      c. pa[0] = pb;
//  2. Exclude if common dimension sizes are not the same across all the
//  arrays/alias in the alias_set
//  3. Exclude if pntr in a function call without definition
//  4. Exclude if pntr Index has side effect
//  5. Exclude negative pointer_dim pntr, e.g. &array
//  6. Exclude global variable and kernel interface variable in alias set
//  7. Exclude data type (base type) mismatch and pointer casting
//  8. Exclude if no necessary to transform (all parallel dims are at the right
//  already)
//
int CAutoDimInterchange::auto_dim_interchange_filter_associated_arrays(
    void *kernel,
    map<set<void *>, set<int>> *p_parallel_array_dims,  //  input/output
    map<void *, vector<size_t>> *p_common_dims,         //  output
    int pre_check) {
  if (pre_check != 0) {
    cout << " -- Update Filtering - Stage 1/2: (alias_set -> dim_set)" << endl;
  } else {
    cout << " -- Update Filtering - Stage 2/2: (alias_set -> dim_set)" << endl;
  }

  set<void *> kernel_top_set;
  //  0. collect all the kernel functions
  for (auto one_node : mars_ir_v2->get_top_kernels()) {
    if (kernel_top_set.find(one_node) == kernel_top_set.end()) {
      kernel_top_set.insert(one_node);
    }
  }

  map<set<void *>, set<int>> output;
  for (auto &&[alias_set, dim_set] : *p_parallel_array_dims) {
    //  void * base_type = nullptr;
    //  string base_type_info = "";
    int invalid_set = 0;
    string invalid_reason = "unknown reason";

    //  Filters:
    //  map<void*,vector<void*> > alias_map;
    //  if ( !invalid_set )
    //  for (auto && [alias_set, dim_set]: parallel_array_dims)
    {
      int max_para_dim = -1;
      for (auto i : dim_set) {
        if (i > max_para_dim) {
          max_para_dim = i;
        }
      }

      for (auto array : alias_set) {
        vector<void *> v_ref;
        ast->get_ref_in_scope(array, nullptr, &v_ref);
        int array_dim = ast->get_dim_num_from_var(array);
        assert(array_dim != 0 && "Comment below should be enabled");
        //  if (array_dim == 0) {
        //  cout << "Assert -- Found an unexpected scalar variable '" +
        //              ast->_p(array) +
        //              "' line=" + my_itoa(ast->get_line(array));
        //  return 0; //  exit the transformation
        //  }

        bool confidence = true;
        if (ast->IsRecursivePointerAlias(array, &confidence) != 0) {
          invalid_reason = "Detected recursive pointer alias '" +
                           ast->_p(array) +
                           "' line=" + my_itoa(ast->get_line(array));
          invalid_set = 1;
          break;
        }
        if (!confidence) {
          invalid_reason = "IsRecursive check has no full confidence '" +
                           ast->_p(array) +
                           "' line=" + my_itoa(ast->get_line(array));
          invalid_set = 1;
          break;
        }

        if (pre_check == 0) {
          void *sg_type = ast->GetTypebyVar(array);
          void *base_type;
          vector<size_t> one_dim_size;
          ast->get_type_dimension(sg_type, &base_type,
                                  &one_dim_size);  //  right to left

          vector<size_t> common;
          if (p_common_dims->find(array) != p_common_dims->end()) {
            common = (*p_common_dims)[array];
          }

          assert(one_dim_size.size() >= common.size() &&
                 "Comment below should be enabled");
          //  MER763 Min Gao: suppose not to happen?
          //  if (one_dim_size.size() < common.size()) {
          //  string one_dim_info = ast->_up(array) + ":";
          //  std::reverse(one_dim_size.begin(),
          //               one_dim_size.end()); //  show it in left to right
          //               order
          //  for (auto t : one_dim_size)
          //    one_dim_info += my_itoa(t) + ",";
          //  string common_dim_info = "common:";
          //  for (auto t : common)
          //    common_dim_info += my_itoa(t) + ",";

          //  invalid_reason = "No enough dimension to interchange '" +
          //                   common_dim_info + "' vs '" + one_dim_info + "'";
          //  invalid_set = 1;
          //  break;
          //  }
        }

        for (auto one_ref : v_ref) {
          if (is_in_range_intrisic(one_ref)) {
            continue;
          }
          void *one_pntr;
          void *sg_array;
          vector<void *> indexes;
          int pointer_dim;
          bool simplify_idx = true;
          int adjusted_array_dim = -1;
          ast->parse_pntr_ref_by_array_ref(one_ref, &sg_array, &one_pntr,
                                           &indexes, &pointer_dim, one_ref,
                                           simplify_idx, adjusted_array_dim);

          if ((one_pntr == nullptr) || sg_array != array) {
            invalid_reason = "Assert -- Null pntr found '" + ast->_p(array) +
                             "' line=" + my_itoa(ast->get_line(one_ref));
            invalid_set = 1;
            break;
          }

          if (ast->get_dim_num_from_var(sg_array) !=
              ast->get_pntr_pntr_dim(one_pntr)) {
            if (-1 != ast->GetFuncCallParamIndex(one_pntr)) {
              void *sg_call = ast->TraceUpToFuncCall(one_pntr);
              int fail = 0;
              if (sg_call == nullptr) {
                fail = 1;
              } else {
                void *sg_decl = ast->GetFuncDeclByCall(sg_call);
                if (sg_decl == nullptr) {
                  fail = 1;
                } else {
                  void *sg_body = ast->GetFuncBody(sg_decl);
                  if (sg_body == nullptr) {
                    fail = 1;
                  }
                }
              }

              if (fail != 0) {
                invalid_reason =
                    "Surrounding function call has no definition '" +
                    ast->_p(one_pntr) +
                    "' line=" + my_itoa(ast->get_line(one_ref));
                invalid_set = 1;
                break;
              }
            }
          }

          //  5. Disable if any swapping index has side effect
          {
            for (auto idx : indexes) {
              if (ast->has_side_effect(idx) == 0) {
                continue;  //  ZP: save time
              }

              {
                invalid_reason = "Index has side effect '" + ast->_p(one_pntr) +
                                 "' line=" + my_itoa(ast->get_line(one_ref));
                invalid_set = 1;
                break;
              }
            }

            if (invalid_set == 0) {
              int has_side_effect = 0;
              vector<void *> v_ref;
              ast->GetNodesByType_int(one_pntr, "preorder", V_SgVarRefExp,
                                      &v_ref);
              set<void *> s_var;
              for (auto ref : v_ref) {
                void *init = ast->GetVariableInitializedName(ref);
                if (ref == one_ref) {
                  continue;
                }
                if (s_var.find(init) == s_var.end()) {
                  s_var.insert(init);
                }
              }
              for (auto one_var : s_var) {
                if (ast->has_side_effect_v2(one_pntr, one_var) != 0) {
                  has_side_effect = 1;
                }
              }

              if (has_side_effect != 0) {
                invalid_reason = "Index has side effect '" + ast->_p(one_pntr) +
                                 "' line=" + my_itoa(ast->get_line(one_ref));
                invalid_set = 1;
                break;
              }
            }

            if (invalid_set != 0) {
              break;
            }
          }

          if (pointer_dim == array_dim) {
            continue;  //  skip checking full-dim
          }

          void *pa = ast->GetParent(one_pntr);
          if (-1 == ast->GetFuncCallParamIndex(one_pntr) &&
              ((ast->IsAssignOp(pa)) == 0) &&
              ((ast->IsExprStatement(pa)) == 0) &&
              !((ast->IsAssignInitializer(pa) != 0) &&
                one_pntr == ast->GetInitializerOperand(pa))) {
            invalid_reason = "Pntr position is unrecognized '" +
                             ast->_p(one_pntr) +
                             "' line=" + my_itoa(ast->get_line(one_ref));
            invalid_set = 1;
            break;
          }

          //  if (ast->is_write_conservative(one_pntr)) //  write reference
          //  {
          //  if (pointer_dim != 0)
          //  {
          //    invalid_reason = "Write to a partial pntr '"+ast->_p(one_pntr) +
          //      "' line=" + my_itoa(ast->get_line(one_ref));
          //    invalid_set = 1;
          //    break;
          //  }
          //  }

          //  test dimension extension condition
          void *alias =
              ast->get_alias_array_from_assignment_expression(one_pntr);
          {
            if (alias != nullptr) {  //  consider dim extension if there is
                                     //  associated alias
              if (p_common_dims->find(alias) == p_common_dims->end()) {
                invalid_reason =
                    "Assert -- can not find 'alias' in common_dim '" +
                    ast->_p(alias) + "' line=" + my_itoa(ast->get_line(alias));
                invalid_set = 1;
                break;
              }
            }
            int pntr_dim = ast->get_pntr_pntr_dim(one_pntr);
            if (pntr_dim < 0) {
              invalid_reason = "Not support negative dim pntr '" +
                               ast->_p(one_pntr) +
                               "' line=" + my_itoa(ast->get_line(one_pntr));
              invalid_set = 1;
              break;
            }
          }

          if (pre_check == 0) {
            //  4. Disable if the offset of the low dims below pointer_dim
            //  (after dim extension) are not all zero

            bool ret = check_pntr_nonzero_idx_condition(alias, one_pntr,
                                                        *p_common_dims, dim_set,
                                                        &invalid_reason) != 0;
            invalid_set = static_cast<int>(!ret);
          }

          if (invalid_set != 0) {
            break;
          }
        }
        if (invalid_set != 0) {
          break;
        }
      }
    }

    //  a. Associate to any kernel interface (ddr variable)
    //  b. The simple parallel access at all at right side (not need to change)
    //  d.2 Data type mismatch & point casting
    if (invalid_set == 0) {
      for (auto array : alias_set) {
        vector<void *> v_ref;
        ast->get_ref_in_scope(array, nullptr, &v_ref);

        //  a. Associate to any kernel interface (ddr variable)
        if (ast->IsGlobalInitName(array) != 0) {
          invalid_reason = "global variable '" + ast->_up(array) + "'";
          invalid_set = 1;
          break;
        }
        if ((ast->IsArgumentInitName(array) != 0) &&
            kernel_top_set.find(ast->TraceUpToFuncDecl(array)) !=
                kernel_top_set.end()) {
          invalid_set = 1;
          invalid_reason =
              "kernel interface variable '" + ast->_up(array) + "'";
          break;
        }

        //  b. The simple parallel access at all at right side (not need to
        //  change)
        {
          int max = -1;
          for (auto t : dim_set) {
            if (t > max) {
              max = t;
            }
          }
          int has_hole = 0;
          for (int i = 0; i < max; i++) {
            if (dim_set.find(i) == dim_set.end()) {
              has_hole = 1;
            }
          }
          if (has_hole == 0) {
            string dim_list;
            for (auto t : dim_set) {
              dim_list += my_itoa(t) + ",";
            }
            invalid_reason = "no need to interchange dims '" + dim_list + "'";
            invalid_set = 1;
            break;
          }  //  no hole in dim, so no need to interchange
        }

        //  c. Dim number/size not match
        //  ( moved to calculate common_dims )

        //  d.1 Data type mismatch & point casting
        //  void * one_type = ast->GetBaseType(ast->GetTypebyVar(array));

        //  if (!base_type) {
        //  base_type = one_type;
        //  base_type_info = ast->_up(array) + ":" + ast->_up(one_type);
        //  }
        //  else if (base_type != one_type) {

        //  invalid_set = 1;
        //  string curr_base_type_info = ast->_up(array) + ":" +
        //  ast->_up(one_type); invalid_reason = "base type mismatch
        //  '"+base_type_info+"' vs '" +curr_base_type_info+"'"; break;
        //  }

        for (auto one_ref : v_ref) {
          if (is_in_range_intrisic(one_ref)) {
            continue;
          }
          void *one_pntr = ast->get_pntr_from_var_ref(one_ref);

          if (one_pntr == nullptr) {
            continue;
          }

          int pntr_dim = ast->get_pntr_pntr_dim(one_pntr);
          int init_dim = ast->get_pntr_init_dim(one_pntr);

          if (pntr_dim == init_dim) {
            continue;
          }

          //  d.2 Data type mismatch & point casting
          if ((ast->IsCastExp(one_pntr) != 0) ||
              (ast->IsCastExp(ast->_pa(one_pntr)) != 0)) {
            void *the_pntr = one_pntr;
            if (ast->IsCastExp(the_pntr) == 0) {
              the_pntr = ast->_pa(the_pntr);
            }
            invalid_reason =
                "pointer casting expression '" + ast->_up(the_pntr) + "'";
            invalid_set = 1;
            break;
          }
        }
        if (invalid_set != 0) {
          break;
        }
      }
    }

    if (invalid_set == 0) {
      output.insert(pair<set<void *>, set<int>>(alias_set, dim_set));
      cout << "   -- ";
      for (auto array : alias_set) {
        cout << ast->_up(array) << ",";
      }
      cout << ": parallel_dim=";
      for (auto dim : dim_set) {
        cout << my_itoa(dim) << ",";
      }

      cout << endl;

    } else {
      cout << "   ++ ";
      for (auto array : alias_set) {
        cout << ast->_up(array) << ",";
      }
      cout << ": parallel_dim=";
      for (auto dim : dim_set) {
        cout << my_itoa(dim) << ",";
      }
      cout << " (skipped due to " << invalid_reason << ")" << endl;
    }
  }
  *p_parallel_array_dims = output;

  return 1;
}

template <typename T>
vector<T> CAutoDimInterchange::dim_switch(vector<T> input,
                                          set<int> dims_to_move_right) {
  vector<T> ret;

  //  push left dims first
  for (size_t t = 0; t < input.size(); t++) {
    if (dims_to_move_right.find(t) == dims_to_move_right.end()) {
      ret.push_back(input[t]);
    }
  }

  //  push right dims then
  for (size_t t = 0; t < input.size(); t++) {
    if (dims_to_move_right.find(t) != dims_to_move_right.end()) {
      ret.push_back(input[t]);
    }
  }

  return ret;
}

//  Function: add n AddressOf to new_pntr
//  assume the pntr is full pointer dim
void CAutoDimInterchange::add_addressof_and_simplify(void **new_pntr,
                                                     int64_t n) {
  int reducible = 1;
  if (n > 0) {
    for (int64_t t = 0; t < n; t++) {
      void *left = ast->GetExpLeftOperand(*new_pntr);
      void *right = ast->GetExpRightOperand(*new_pntr);
      if ((left == nullptr) || (right == nullptr) ||
          (ast->IsPntrArrRefExp(*new_pntr) == 0)) {
        reducible = 0;
      }

      int64_t value = -1;
      if ((reducible != 0) &&
          ast->IsConstantInt(right, &value, true, nullptr) && value == 0) {
        *new_pntr = left;  //  remove one layer
        continue;
      }

      if (reducible != 0) {
        *new_pntr = ast->CreateExp(V_SgAddOp, left, right);
      } else {
        *new_pntr = ast->CreateExp(V_SgAddressOfOp, *new_pntr);
      }
      reducible = 0;
    }
  }
  assert(n >= 0 && "Comment below should be enabled");
  //  Min Gao MER763 commented first to observe it n < 0 is really happening
  //  reducible = 1;
  //  if (n < 0) {

  //  for (int t = 0; t < -n; t++) {
  //    void *left = ast->GetExpLeftOperand(*new_pntr);
  //    void *right = ast->GetExpRightOperand(*new_pntr);
  //    if (ast->IsAddressOfOp(*new_pntr)) {
  //      *new_pntr = ast->GetExpUniOperand(*new_pntr);
  //    } else if (ast->IsAddOp(*new_pntr)) {
  //      *new_pntr = ast->CreateExp(V_SgPntrArrRefExp, left, right);
  //    } else {
  //      *new_pntr =
  //          ast->CreateExp(V_SgPntrArrRefExp, *new_pntr, ast->CreateConst(0));
  //    }
  //  }
  //  }
}

int CAutoDimInterchange::auto_dim_interchange_apply_dim_interchange(
    void *kernel, const map<set<void *>, set<int>> &parallel_array_dims,
    const map<void *, vector<size_t>> &common_dims) {
  cout << " -- Apply Dim Transform: " << endl;
  for (auto &&[alias_set, dim_set] : parallel_array_dims) {
    if (alias_set.empty()) {
      continue;
    }
    void *one_array = nullptr;
    for (auto t : alias_set) {
      one_array = t;
      break;
    }  //  find the first one

    cout << "   -- ";
    for (auto array : alias_set) {
      cout << ast->_up(array) << ",";
    }
    cout << " -> dims_to_move_right=";
    for (auto dim : dim_set) {
      cout << my_itoa(dim) << ",";
    }
    string common_dim_str;
    for (auto dim : common_dims.at(one_array)) {
      common_dim_str += my_itoa(dim) + ",";
    }
    cout << " (common_dim=" << common_dim_str << ")" << endl;
    for (auto array : alias_set) {
      //  1. type conversion
      vector<size_t> dim_org;  //  from left to right dim
      void *base_type = nullptr;
      ast->get_type_dimension_new(ast->GetTypebyVar(array), &base_type,
                                  &dim_org);  //  left to right
      if (base_type == nullptr) {
        return 0;
      }
      if (dim_org.empty()) {
        return 0;
      }

      if (common_dims.find(array) == common_dims.end()) {
        continue;
      }
      vector<size_t> common = common_dims.at(array);
      size_t array_dim = common.size();

      int64_t n_dim_ext = array_dim - dim_org.size();
      if (array_dim < dim_org.size()) {
        n_dim_ext = 0;
      }

      vector<size_t> dim_org_ext;  //  = common;
      if (dim_org.size() > array_dim) {
        for (size_t i = 0; i < dim_org.size() - array_dim; i++) {
          dim_org_ext.push_back(dim_org[i]);
        }
      }
      for (size_t i = 0; i < array_dim; i++) {
        dim_org_ext.push_back(common[i]);
      }
      array_dim = dim_org_ext.size();

      set<int> dim_set_from_left;
      for (auto t : dim_set) {
        dim_set_from_left.insert(array_dim - 1 - t);
      }
      vector<size_t> dim_new =
          dim_switch<size_t>(dim_org_ext, dim_set_from_left);
      if (dim_org[0] == 0) {
        dim_new[0] = 0;  //  convert back to pointer
      }

      //  Update the type
      {
        void *new_type =
            ast->CreateArrayType(base_type, dim_new);  //  left to right
        string str_old_type = ast->_up(ast->GetTypebyVar(array));
        ast->SetTypetoVar(array, new_type);
        string str_new_type = ast->_up(new_type);

        cout << "     -- Interchange an array type " << ast->_up(array) << ": "
             << str_old_type << " -> " << str_new_type << endl;
      }

      //  handling forwarding declaration
      int arg_idx = ast->GetFuncParamIndex(array);
      if (-1 != arg_idx) {
        void *one_decl = ast->TraceUpToFuncDecl(array);
        if (one_decl != nullptr) {
          vector<void *> vec_decls = ast->GetAllFuncDecl(one_decl);
          for (auto decl : vec_decls) {
            if (decl == one_decl) {
              continue;
            }
            void *new_decl =
                ast->CloneFuncDecl(one_decl, ast->GetGlobal(decl), false);
            ast->ReplaceStmt(decl, new_decl);
          }
        }
      }

      assert(n_dim_ext <= 0 && "Commnet below should be enabled");
      //  Min MER-763 comment out as this should already been processed in step
      //  5
      //  //  1.1: Initialization list: need to add & if dimention extension
      //  if (n_dim_ext > 0) {
      //  set<void *> v_exp_to_addressof;

      //  {
      //    void *initer = ast->GetInitializer(array);
      //    if (initer && ast->GetInitializerOperand(initer)) {
      //      void *init_exp = ast->GetInitializerOperand(initer);
      //      v_exp_to_addressof.insert(init_exp);
      //    }
      //  }

      //  {
      //    vector<void *> v_refs;
      //    ast->get_ref_in_scope(array, ast->GetScope(array), &v_refs);
      //    for (auto ref : v_refs) {
      //      if (is_in_range_intrisic(ref))
      //        continue;
      //      void *sg_pntr = ast->get_pntr_from_var_ref(ref);
      //      if (!sg_pntr)
      //        continue;

      //      void *sg_parent = ast->_pa(sg_pntr);
      //      if (!sg_parent)
      //        continue;

      //      void *left, *right;
      //      ast->parse_assign(sg_parent, &left, &right);

      //      if (left && right && left == array) {
      //        if (v_exp_to_addressof.find(right) == v_exp_to_addressof.end())
      //          v_exp_to_addressof.insert(right);
      //      }
      //    }
      //  }

      //  for (auto rvalue_exp : v_exp_to_addressof) {
      //    void *new_init_exp = ast->CopyExp(rvalue_exp);

      //    add_addressof_and_simplify(&new_init_exp,
      //    static_cast<int64_t>(n_dim_ext));

      //    string str_old_decl = ast->_up(ast->TraceUpToStatement(rvalue_exp));

      //    ast->ReplaceExp(rvalue_exp, new_init_exp);

      //    string str_new_decl =
      //    ast->_up(ast->TraceUpToStatement(new_init_exp));

      //    cout << "     -- Update Assignment: " << str_old_decl << " -> "
      //         << str_new_decl << endl;
      //  }
      //  }

      //  2. reference conversion

      void *sg_scope = ast->GetScope(array);
      vector<void *> v_ref;
      ast->get_ref_in_scope(
          array, sg_scope, &v_ref,
          true);  // TODO(youxiang): intrinsic is ignored currently

      //  ZP: Note: smart solving pntr of pntr issue
      //  1. In pntr traverse, index will be always visited after the var_ref
      //  2. change the traverse order from preorder to postorder in above call
      //  can not change the visit order (because the var_ref and idx are
      //  brother relation instead of parent/child relation But, simply reverse
      //  the listt works
      std::reverse(v_ref.begin(), v_ref.end());

      for (auto one_ref : v_ref) {
        if (is_in_range_intrisic(one_ref)) {
          continue;
        }
        void *one_pntr;
        void *sg_array;
        vector<void *> indexes;
        int pointer_dim;
        //  1. Get dimension indexes
        //  Note: indexes has been extended (at left) since type hass changed
        bool simplify_idx = false;
        ast->parse_pntr_ref_by_array_ref(one_ref, &sg_array, &one_pntr,
                                         &indexes, &pointer_dim, one_ref,
                                         simplify_idx);
        if ((one_pntr == nullptr) || sg_array != array) {
          continue;
        }

        //  Skip lvalue of single-var pntr
        {
          void *sg_assign = ast->GetParent(one_pntr);
          if (sg_assign == nullptr) {
            goto end_of_skip_lvalue;
          }

          if (pointer_dim + n_dim_ext == ast->get_dim_num_from_var(sg_array)) {
            goto end_of_skip_lvalue;
          }

          if (ast->IsAssignOp(sg_assign) == 0) {
            goto end_of_skip_lvalue;
          }
          if (one_pntr != ast->GetExpLeftOperand(sg_assign)) {
            goto end_of_skip_lvalue;
          }

          continue;  //  do not transform the lvalue;
        end_of_skip_lvalue : {}
        }

        assert(n_dim_ext == 0 && "Comment below should be enabled");
        //  Min Gao MER763 commented out, supposed to be done in step 5.
        //  //  Note: the index has been shifted due to type change,
        //  //  Action: right shifting by n_dim_ext
        //  if (n_dim_ext)
        //  for (int t = (int)indexes.size() - 1; t >= 0; t--) {
        //    if (t >= n_dim_ext)
        //      indexes[t] = indexes[t - n_dim_ext];
        //    else
        //      indexes[t] = ast->CreateConst(0);
        //  }

        string index_str;
        for (auto idx : indexes) {
          index_str += ast->_up(idx) + ",";
        }

        //  2. Interchange dimention
        vector<void *> new_indexes =
            dim_switch<void *>(indexes, dim_set_from_left);
        string str_one_pntr = ast->_up(one_pntr);

        //  3. Create new pntr
        vector<void *> new_indexes_cp;
        for (auto idx : new_indexes) {
          CMarsExpression me(idx, ast, one_pntr);
          new_indexes_cp.push_back(me.get_expr_from_coeff());
        }
        void *new_pntr_var_ref = ast->CreateVariableRef(array);
        void *new_pntr = ast->CreateArrayRef(new_pntr_var_ref, new_indexes_cp);

        //  4. Recover partial pointer
        int64_t new_pointer_dim = pointer_dim + n_dim_ext;

        add_addressof_and_simplify(
            &new_pntr, static_cast<int64_t>(array_dim - new_pointer_dim));

        //  5. Replace Pntr
        ast->ReplaceExp(one_pntr, new_pntr);

        //  6. output display
        void *d_array;
        void *d_pntr;
        vector<void *> d_indexes;
        int d_pointer_dim;
        ast->parse_pntr_ref_by_array_ref(
            new_pntr_var_ref, &d_array, &d_pntr, &d_indexes, &d_pointer_dim,
            new_pntr_var_ref);  //  d_pointer_dim is not calc correctly

        string new_index_str;
        for (auto idx : d_indexes) {
          new_index_str += ast->_up(idx) + ",";
        }

        cout << "     -- Interchange a pntr " << str_one_pntr;
        cout << ":(" << index_str << ")pd=" << my_itoa(pointer_dim);
        cout << " -> " + ast->_up(d_pntr) + ":(" << new_index_str << ")pd=";
        cout << my_itoa(d_pointer_dim) << endl;
      }
    }
    for (auto array : alias_set) {
      if (ast->IsArgumentInitName(array) != 0) {
        update_access_range_intrinsic(array);
      }
    }
  }

  return 1;
}

//  Alias Analysis:
//  Get all the array init_names that is associated with the current sg_name
//  1. The local arrays
//  2. The kernel boundary interfaces
//  3. If a sg_name partial pntr used as lvalue, add rvalue pntr into recursion
//  4. If a sg_name partial pntr used as rvalue, add lvalue pntr into recursion
//  Note:
//  0. Suppose only handle C99 code with no structure
//  1. Only support array/pointer, return 0 for scalar
//  2. if alias analysis fails, return 0
int CAutoDimInterchange::alias_induction_in_scopes(
    void *sg_name, void *sg_scope, set<void *> *p_boundaries,
    set<void *> *p_visited_func) {
  // TODO(youxiang): if a partial pntr is not a function parameter, an
  //  assignement/initializer right value, or a
  //      assignment left value, we need to return 0;

  if (ast->get_dim_num_from_var(sg_name) <= 0) {
    return 0;
  }

  if (p_boundaries->find(sg_name) == p_boundaries->end()) {
    p_boundaries->insert(sg_name);
  }

  void *recur_func;
  if ((ast->IsFunctionDeclaration(sg_scope) != 0) &&
      (ast->IsRecursiveFunction(sg_scope, &recur_func) != 0)) {
    cout << " ERROR: find a recursive function: " << ast->_p(sg_scope) << endl;
    return 0;
  }

  {
    void *initer = ast->GetInitializer(sg_name);
    if (initer != nullptr) {
      void *initer_exp = ast->GetInitializerOperand(initer);

      if (initer_exp != nullptr) {
        vector<void *> indices;
        void *array;
        ast->parse_pntr_ref_new(initer_exp, &array, &indices);

        if (array != nullptr) {
          if (p_boundaries->find(array) == p_boundaries->end()) {
            p_boundaries->insert(array);
            int ret = alias_induction_in_scopes(array, sg_scope, p_boundaries,
                                                p_visited_func);
            if (ret == 0) {
              return 0;
            }
          }
        }
      }
    }
  }

  //  2.0 TODO: point assignment lhs, e.g. int * array = ...;
  //  2.1 TODO: point assignment rhs, e.g. int * alias = array;
  //  Note: to try/apply ROSE alias analysis
  //  option 1: PtrAnal (ZP: tested and working. But seems not useful, not know
  //  how to use the result)
  //  - rose-develop/src/midend/programAnalysis/pointerAnal/
  //  -
  //  rose-develop/tests/nonsmoke/functional/roseTests/programAnalysisTests/PtrAnalTest.C
  //  -
  //  rose-develop/tests/nonsmoke/functional/roseTests/programAnalysisTests/testPtr1.C
  //  -
  //  rose-develop/tests/nonsmoke/functional/roseTests/programAnalysisTests/PtrAnalTest.out1
  //  option 2: DataFlowAnalysis (seems good, seen the result in the wikibook
  //  page, but not working yet)
  //  - source:
  //  rose-develop/src/midend/programAnalysis/genericDataflow/simpleAnalyses
  //  - test: rose-develop/src/midend/programAnalysis/pointerAnal/
  //  - test:
  //  rose-develop/tests/nonsmoke/functional/roseTests/programAnalysisTests/generalDataFlowAnalysisTests/pointerAliasAnalysis*
  //  - https://  en.wikibooks.org/wiki/ROSE_Compiler_Framework/Pointer_Analysis
#if 1  //  a temporary solution of alias analysis, to be replaced by a standard
       //  one
  {
    //  detecting and handling alias (not cross function)
    {
      vector<void *> v_access;
#if 0
      int valid = ast->GetPntrReferenceOfArray(sg_name, sg_scope, v_access);
#else
      vector<void *> v_ref;
      ast->get_ref_in_scope(sg_name, nullptr, &v_ref);
      for (auto one_ref : v_ref) {
        if (is_in_range_intrisic(one_ref)) {
          continue;
        }
        void *one_pntr = ast->get_pntr_from_var_ref(one_ref);

        if (one_pntr == nullptr) {
          continue;
        }
        if (ast->get_pntr_pntr_dim(one_pntr) ==
            ast->get_pntr_init_dim(one_pntr)) {
          continue;
        }

        v_access.push_back(one_pntr);
      }
#endif

      //  handling rvalue as one_access
      for (auto one_access : v_access) {
        void *sg_alias =
            ast->get_alias_array_from_assignment_expression(one_access);
        if (sg_alias == nullptr) {
          continue;
        }

        void *array = sg_alias;
        if ((array != nullptr) &&
            (ast->is_located_in_scope(array, sg_scope) != 0)) {
          if (p_boundaries->find(array) == p_boundaries->end()) {
            p_boundaries->insert(array);
            int ret = alias_induction_in_scopes(array, sg_scope, p_boundaries,
                                                p_visited_func);
            if (ret == 0) {
              return 0;
            }
          }
        }
      }

      //  handling missing rvalue as one_access by
      //  get_alias_array_from_assignment_expression
      for (auto one_access : v_access) {
        void *sg_parent = ast->GetParent(one_access);
        if (sg_parent == nullptr) {
          continue;
        }
        if (ast->IsAssignOp(sg_parent) == 0) {
          continue;
        }

        if (one_access != ast->GetExpRightOperand(sg_parent)) {
          continue;
        }

        void *sg_lvalue = ast->GetExpLeftOperand(sg_parent);
        if (sg_lvalue == nullptr) {
          continue;
        }

        vector<void *> indices;
        void *sg_array;
        ast->parse_pntr_ref_new(sg_lvalue, &sg_array, &indices);

        void *array = sg_array;
        if ((array != nullptr) &&
            (ast->is_located_in_scope(array, sg_scope) != 0)) {
          if (p_boundaries->find(array) == p_boundaries->end()) {
            p_boundaries->insert(array);
            int ret = alias_induction_in_scopes(array, sg_scope, p_boundaries,
                                                p_visited_func);
            if (ret == 0) {
              return 0;
            }
          }
        }
      }
      //  handling lvalue of an assignment
      for (auto one_access : v_access) {
        void *sg_parent = ast->GetParent(one_access);
        if (sg_parent == nullptr) {
          continue;
        }

        void *sg_alias_init = nullptr;

        if (ast->IsAssignOp(sg_parent) != 0) {
          void *sg_rvalue = nullptr;
          if (one_access != ast->GetExpLeftOperand(sg_parent)) {
            continue;
          }
          sg_rvalue = ast->GetExpRightOperand(sg_parent);
          if (sg_rvalue == nullptr) {
            continue;
          }
          vector<void *> indices;
          ast->parse_pntr_ref_new(sg_rvalue, &sg_alias_init, &indices);
        }

        void *array = sg_alias_init;
        if ((array != nullptr) &&
            (ast->is_located_in_scope(array, sg_scope) != 0)) {
          if (p_boundaries->find(array) == p_boundaries->end()) {
            p_boundaries->insert(array);
            int ret = alias_induction_in_scopes(array, sg_scope, p_boundaries,
                                                p_visited_func);
            if (ret == 0) {
              return 0;
            }
          }
        }
      }
    }
  }
#endif

  //  2.2 function argument, e.g. sub_func(a); //  sub_func(int *b)
  if (-1 != ast->GetFuncParamIndex(sg_name)) {
    int arg_idx = ast->GetFuncParamIndex(sg_name);
    void *func_decl = ast->TraceUpToFuncDecl(sg_name);

    if ((func_decl != nullptr) && (ast->GetFuncBody(func_decl) == nullptr)) {
      return 0;  //  a func with no define
    }

    if ((func_decl != nullptr) &&
        (ast->is_located_in_scope(func_decl, sg_scope) != 0)) {
      //  && visited_func.find(func_decl) == visited_func.end())
      vector<void *> calls;
      ast->GetFuncCallsFromDecl(func_decl, nullptr, &calls);

      for (auto one_call : calls) {
        if (ast->is_located_in_scope(one_call, sg_scope) == 0) {
          continue;
        }
        void *actual_param = ast->GetFuncCallParam(one_call, arg_idx);
        vector<void *> indices;
        void *array;
        ast->parse_pntr_ref_new(actual_param, &array, &indices);
        if ((array != nullptr) &&
            (ast->is_located_in_scope(array, sg_scope) != 0)) {
          if (p_boundaries->find(array) == p_boundaries->end()) {
            p_boundaries->insert(array);

            int ret = alias_induction_in_scopes(array, sg_scope, p_boundaries,
                                                p_visited_func);
            if (ret == 0) {
              return 0;
            }
          }
        }
      }
      //  visited_func.insert(func_decl);
    }
  }

  return 1;
}

//  Main function
int CAutoDimInterchange::run(void *kernel) {
  int ret;
  cout << "[AutoDimInterchange] processing " << ast->_up(kernel) << " ..."
       << endl;

  //  Step 1. and 2. Find simple parallel access
  map<void *, set<int>> parallel_access;  //  pntr -> parallel dim indexes
  ret = auto_dim_interchange_collect_parallel_dim_access(kernel,
                                                         &parallel_access);
  if (ret == 0) {
    return 0;  //  do nothing
  }

  //  Step 3. Find associated arrays/pointers init names
  map<set<void *>, set<int>>
      parallel_array_dims;  //  set of init_names -> parallel dim indexes (from
                            //  right)
  ret = auto_dim_interchange_collect_associated_arrays(kernel, parallel_access,
                                                       &parallel_array_dims);
  if (ret == 0) {
    return 0;  //  do nothing
  }

  //  Step 3.5 Calculate common_dims;
  map<void *, vector<size_t>> common_dims;  //  array -> common_dims
  int pre_check = 1;
  ret = auto_dim_interchange_calculate_common_dims(kernel, &parallel_array_dims,
                                                   &common_dims, pre_check);
  if (ret == 0) {
    return 0;  //  do nothing
  }

  //  Step 4.1 Filter associated arrays/pointers init names
  ret = auto_dim_interchange_filter_associated_arrays(
      kernel, &parallel_array_dims, &common_dims, pre_check);
  if (ret == 0) {
    return 0;  //  do nothing
  }

  //  Step 5.1 Array dim extension and low_dim offset splitting
  ret = auto_dim_interchange_apply_dim_extension_and_offset_split(
      kernel, &parallel_array_dims, common_dims);
  if (ret == 0) {
    return 0;
  }

  ast->init_defuse();

  pre_check = 0;
  //  Step 4.2 Filter associated arrays/pointers init names
  ret = auto_dim_interchange_filter_associated_arrays(
      kernel, &parallel_array_dims, &common_dims, pre_check);
  if (ret == 0) {
    return 0;  //  do nothing
  }

  //  Step 5. and 6. Apply dimention extension and interchange
  ret = auto_dim_interchange_apply_dim_interchange(kernel, parallel_array_dims,
                                                   common_dims);
  if (ret == 0) {
    return 0;
  }

  return 1;
}

//  Goal
//  - Interchange the parallel array dims into low dimensions to achieve
//    desired II in pipelining/parallelization
//  Function
//  1. Detect the arrays and their dimensions that has parallel loop iterators
//  2. Consider cross-function and pointer alias
//  3. Preprocess 1: Extend the alias with less dimension to cover the
//  interchange
//  4. Preprocess 2: Split the access index offsets into separate variables to
//  solve
//     the issue of offset at low dimensions
//  5. Array dimension interchange transformation: type and ref pntr
//  Assumption:
//   - Input code is compatible with C99 standard (not checked in the code!)
//  Limitations
//  1. Exclude recursive pointer alias
//  2. Exclude the situations that recursive check has uncertain results
//     (See details in IsRecrusivePointerAlias())
//     - Only the follow partial pntr ref cases are considered (otherwise
//     non-confident result):
//       a. Function argument
//       b. Initializer
//       c. assignOp with single ref lvalue in an SgExprStatement
//     - Excluded cases include (but not limited to):
//       a. p++, p+=
//       b. pa = pb = pc;
//       c. pa[0] = pb;
//  2. Exclude if common dimension sizes are not the same across all the
//  arrays/alias in the alias_set
//  3. Exclude if pntr in a function call without definition
//  4. Exclude if pntr Index has side effect
//  5. Exclude negative pointer_dim pntr, e.g. &array
//  6. Exclude global variable and kernel interface variable in alias set
//  7. Exclude data type (base type) mismatch and pointer casting
//  8. Exclude if no necessary to transform (all parallel dims are at the right
//  already)
int auto_dim_interchange_top(CSageCodeGen *codegen, void *pTopFunc,
                             const CInputOptions &options) {
  printf("Merlin Pass [Auto Array Dimension Interchange] started ... \n");

  CMarsIr mars_ir;
  try {
    mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  } catch (std::exception &e) {
    cout << "[MARS_IR] Exception: Recursive function is detected" << endl;
    return 0;
  }

  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(codegen, pTopFunc);
  CAutoDimInterchange auto_dim_interchange(codegen, &mars_ir, &mars_ir_v2,
                                           options);

  auto kernels = mars_ir_v2.get_top_kernels();

  for (auto kernel : kernels) {
    auto_dim_interchange.run(kernel);
  }

  return 1;
}
