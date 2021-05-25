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


//  ******************************************************************************************//
//  module           :   function inline
//  description      :   inline all the functions in one kernel
//  input            :   code which go through preprocess, have no static
//  variable and memcpy output           :   all kernel functions with out sub
//  functions limitation       :   not support recursive function;
//                      not support function return not in the end of the
//                      function
//  author           :   Han
//  ******************************************************************************************//
#include <iostream>
#include <string>
//  #include <boost/algorithm/string.hpp>
#include "file_parser.h"
#include "math.h"
#include <boost/algorithm/string/replace.hpp>

#include "codegen.h"
#include "function_inline.h"
#include "ir_types.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "postwrap_process.h"
#include "rose.h"

class CAutoInline {
 public:
  CAutoInline(CMarsAST_IF *ast_in, CMarsIrV2 *mars_ir_in,
              const CInputOptions &options_in, string mode)
      : ast(ast_in), mars_ir(mars_ir_in), options(options_in),
        process_mode(mode) {
    string tool_version = options.get_option_key_value("-a", "tool_version");
    hls_flow = false;
    if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
      hls_flow = true;
    }
  }

  int run(void *kernel);

 protected:
  int auto_inline_collect_partition_info(void *kernel,
                                         map<void *, int> *p_partition_info);
  int alias_induction_in_scopes(void *sg_name, void *scope,
                                set<void *> *p_boundaries,
                                set<void *> *p_visited_func);
  int auto_inline_filter_func_by_partition(vector<void *> *p_func_decls,
                                           map<void *, int> *p_partition_info,
                                           void *sg_scope);
  bool func_has_inline(void *func_decl);
  void get_global_pragmas(vector<void *> *p_pragmas);

 protected:
  CMarsAST_IF *ast;
  CMarsIrV2 *mars_ir;
  CInputOptions options;
  string process_mode;
  bool hls_flow;
};

//  Check if any argument has partitioned dim
int CAutoInline::auto_inline_filter_func_by_partition(
    vector<void *> *p_func_decls, map<void *, int> *p_partition_info,
    void *sg_scope) {
  //  0. collect global variable set

  vector<void *> filter_decls;

  for (auto one_func : *p_func_decls) {
    if (ast->GetFuncBody(one_func) == nullptr) {
      continue;
    }

    vector<void *> init_names;
    for (auto one_arg : ast->GetFuncParams(one_func)) {
      init_names.push_back(one_arg);
    }

#if 1  //  consider global variable as well
    {
      set<void *> s_global;
      map<void *, set<void *>> var2refs;
      bool errorOut = false;
      collect_used_global_variable(one_func, &s_global, &var2refs, ast,
                                   &errorOut, hls_flow);  //  cross_func
      for (auto one_init : s_global) {
        init_names.push_back(one_init);
      }
    }
#endif
    vector<void *> v_calls;
    ast->GetFuncCallsFromDecl(one_func, nullptr, &v_calls);

    for (auto one_arg : init_names) {
      if (this->process_mode == "off") {
        continue;
      }

      if (this->process_mode != "aggressive") {
        int arg_idx = ast->GetFuncParamIndex(one_arg);
        if (-1 != arg_idx) {
          int is_single_var_ref_in_call = 1;
          for (auto one_call : v_calls) {
            void *one_arg = ast->GetFuncCallParam(one_call, arg_idx);
            void *real_arg = one_arg;
            while (ast->IsCastExp(real_arg) != 0) {
              real_arg = ast->RemoveCast(real_arg);
            }
            if ((one_arg == nullptr) ||
                (ast->IsVarRefExp(real_arg) == 0)) {  //  single variable
              is_single_var_ref_in_call = 0;
              break;
            }
          }

          if (/*this->process_mode == "on" &&*/ is_single_var_ref_in_call !=
              0) {
            continue;
          }
        }
      }

      int curr_dim = ast->get_dim_num_from_var(one_arg);
      if (curr_dim <= 0) {
        continue;
      }

      set<void *> alias_sources;
      set<void *> visited_func;
      int ret = alias_induction_in_scopes(one_arg, sg_scope, &alias_sources,
                                          &visited_func);
      if (ret == 0) {
        continue;
      }

      {
        cout << " -- [Filter] Alias of '" << ast->_up(one_arg) << "': ";
        for (auto t : alias_sources) {
          cout << ast->_up(t) << ",";
        }
        cout << endl;
      }

      //  to see if any alias source is partitioned
      int hit = 0;
      for (auto one_source : alias_sources) {
        if (p_partition_info->find(one_source) == p_partition_info->end()) {
          continue;
        }

        int part_dim =
            (*p_partition_info)[one_source];  //  part_dim starts from zero

        //  if part_dim == curr_dim, partition does not hit on the arg
        if (part_dim >= curr_dim) {
          continue;
        }

        hit = 1;
        break;
      }

      if (hit != 0) {
        filter_decls.push_back(one_func);
        break;  //  ignore the remain arguments
      }
    }
  }

  *p_func_decls = filter_decls;

  return 1;
}

bool CAutoInline::func_has_inline(void *func_decl) {
  if ((func_decl == nullptr) || (ast->GetFuncBody(func_decl) == nullptr)) {
    return false;
  }

  vector<void *> pragmas;
  int is_compatible = 1;
  ast->GetNodesByType(func_decl, "preorder", "SgPragmaDeclaration", &pragmas,
                      is_compatible != 0);
  for (auto one_pragma : pragmas) {
    bool allow_hls = true;
    string pragma_name =
        mars_ir->get_pragma_attribute(one_pragma, "inline", allow_hls);
    boost::algorithm::to_lower(pragma_name);
    if (pragma_name == "inline") {
      return true;
    }
  }
  return false;
}

//  Function
//  1. Add "HLS inline" pragma to the body of the function decl that has
//  partitioned arguments
//  Process
//  1. for each local array, if it is partitioned, find all the accesses
//  2. for each cross-func acceses, check if there is arguments that has low dim
//  partitioned outside the function decl
//  3. for the func decls that meet 2's check, add "HLS inline" if there no
//  exisiting "HLS inline" inside the func
int CAutoInline::run(void *kernel) {
  cout << "[AutoInline] processing " << ast->_up(kernel) << " ..." << endl;

  //  1. Find all the local arrays that has partitioning
  map<void *, int>
      partition_info;  //  array_name -> largest partition dim index (from left)
  auto_inline_collect_partition_info(kernel, &partition_info);

  //  2. Filtering functions with partitioning condition
  vector<void *> func_decls;
  ast->GetNodesByType(ast->GetFuncBody(kernel), "preorder",
                      "SgFunctionDeclaration", &func_decls, true, true);
  auto_inline_filter_func_by_partition(&func_decls, &partition_info, kernel);

  //  3. Add "HLS inline" if there no exisiting "HLS inline" inside the func
  for (auto one_func : func_decls) {
    if (func_has_inline(one_func) || ast->func_has_dataflow(one_func)) {
      continue;
    }
    bool dataflow_sub_func = false;
    vector<void *> vec_calls;
    ast->GetFuncCallsFromDecl(one_func, nullptr, &vec_calls);
    for (auto call : vec_calls) {
      void *caller = ast->TraceUpToFuncDecl(call);
      if (caller != nullptr && ast->func_has_dataflow(caller)) {
        dataflow_sub_func = true;
        break;
      }
    }
    if (dataflow_sub_func)
      continue;

    void *func_body = ast->GetFuncBody(one_func);
    if (func_body != nullptr) {
      void *new_pragma = ast->CreatePragma("HLS inline", func_body);
      ast->PrependChild(func_body, new_pragma);
      cout << " -- Insert inline at " << ast->_up(one_func) << endl;
    }
  }

  return 1;
}

//  Get all the array init_names that is associated with the current sg_name
//  1. the local arrays
//  2. the kernel boundary interfaces
//  Note:
//  0. Suppose only handle C99 code with no structure
//  1. Only support array/pointer, return 0 for scalar
//  2. if alias analysis fails, return 0
// TODO(Han) :clean up API, visted_func is not used.
int CAutoInline::alias_induction_in_scopes(void *sg_name, void *sg_scope,
                                           set<void *> *p_boundaries,
                                           set<void *> *p_visited_func) {
  if (ast->get_dim_num_from_var(sg_name) <= 0) {
    return 0;
  }

  if (p_boundaries->find(sg_name) == p_boundaries->end()) {
    p_boundaries->insert(sg_name);
  }

  //  2.0 TODO: point assignment lhs, e.g. int * array = ...;
  //  2.1 TODO: point assignment rhs, e.g. int * alias = array;
  //  Note: to try/apply ROSE alias analysis
  //  option 1: PtrAnal (ZP: tested and working. But seems not useful, not
  //  know how to use the result)
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
  //  - https://
  //  en.wikibooks.org/wiki/ROSE_Compiler_Framework/Pointer_Analysis
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
    }
  }
#endif

  //  2.2 function argument, e.g. sub_func(a); //  sub_func(int *b)
  if (-1 != ast->GetFuncParamIndex(sg_name)) {
    int arg_idx = ast->GetFuncParamIndex(sg_name);
    void *func_decl = ast->TraceUpToFuncDecl(sg_name);
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

void CAutoInline::get_global_pragmas(vector<void *> *p_pragmas) {
  for (auto one_global : ast->GetGlobals()) {
    for (auto stmt : ast->GetChildStmts(one_global)) {
      if (ast->IsPragmaDecl(stmt) != 0) {
        p_pragmas->push_back(stmt);
      }
    }
  }
}

int CAutoInline::auto_inline_collect_partition_info(
    void *kernel, map<void *, int> *p_partition_info) {
  //  1. search for partitioning pragmas
  //  2. for each pragma, trace to array init_name, and fill the output
  {
    vector<void *> pragmas;
    int is_compatible = 1;
    int is_cross_func = 1;
    ast->GetNodesByType(kernel, "preorder", "SgPragmaDeclaration", &pragmas,
                        is_compatible != 0, is_cross_func != 0);
    get_global_pragmas(&pragmas);
    for (auto one_pragma : pragmas) {
      bool allow_hls = true;
      string pragma_name = mars_ir->get_pragma_attribute(
          one_pragma, "array_partition", allow_hls);
      boost::algorithm::to_lower(pragma_name);
      if ("array_partition" != pragma_name) {
        continue;
      }

      //  cout << "Pragma: " << ast->_p(one_pragma) << endl;

      string str_var =
          mars_ir->get_pragma_attribute(one_pragma, "variable", allow_hls);
      string str_dim =
          mars_ir->get_pragma_attribute(one_pragma, "dim", allow_hls);
      int part_idx = (str_dim.empty()) ? 0 : my_atoi(str_dim);

      void *sg_pos = ast->GetScope(one_pragma);
      void *sg_name = ast->find_variable_by_name(str_var, sg_pos);

      if (sg_name == nullptr) {
        cout << "ERROR: unrecognized variable name '" << str_var
             << "' in pragma: " << endl;
        cout << "  " << ast->_p(one_pragma) << endl;
        continue;
      }

      int array_dim = ast->get_dim_num_from_var(sg_name);
      int part_idx_from_right = array_dim - part_idx;

      if (part_idx_from_right < 0 || part_idx <= 0) {
        cout << "ERROR: invalid dim index in array_partition pragma: "
                "partition_dim="
             << my_itoa(part_idx) << " array_dim=" << my_itoa(array_dim)
             << endl;
        cout << "  " << ast->_p(one_pragma) << endl;
        continue;
      }

      set<void *> end_variables;
      set<void *> visited_func;
      int ret = alias_induction_in_scopes(sg_name, kernel, &end_variables,
                                          &visited_func);
      if (ret == 0) {
        continue;
      }

      {
        cout << " -- [Partition] Alias of '" << ast->_up(sg_name) << "': ";
        for (auto t : end_variables) {
          cout << ast->_up(t) << ",";
        }
        cout << endl;
      }

      for (auto one_local_array : end_variables) {
        if (p_partition_info->find(one_local_array) ==
            p_partition_info->end()) {
          p_partition_info->insert(
              pair<void *, int>(one_local_array, part_idx_from_right));
          cout << " -- Set partition info: " << ast->_up(one_local_array)
               << " right_dim=" << my_itoa(part_idx_from_right) << endl;
        } else {
          int pre_idx = (*p_partition_info)[one_local_array];
          if (part_idx_from_right < pre_idx) {
            (*p_partition_info)[one_local_array] = part_idx_from_right;
            cout << " -- Update partition info: " << ast->_up(one_local_array)
                 << " right_dim=" << my_itoa(part_idx_from_right) << endl;
          }
        }
      }
    }
  }

  return 1;
}

//  Function
//  1. Add "HLS inline" pragma to the body of the function decl that has
//  partitioned arguments
//  Algorithm
//  1. for each local array, if it is partitioned, find all the accesses
//  2. for each cross-func acceses, check if there is arguments that has low dim
//  partitioned outside the function decl
//  3. for the func decls that meet 2's check, add "HLS inline" if there no
//  exisiting "HLS inline" inside the func
int auto_func_inline_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  printf("Merlin Pass [Auto Function Inlining] started ... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  string process_mode = options.get_option_key_value("-a", "auto_func_inline");
  CAutoInline auto_inline(codegen, &mars_ir, options, process_mode);

  auto kernels = mars_ir.get_top_kernels();

  for (auto kernel : kernels) {
    auto_inline.run(kernel);
  }

  return 0;
}

//  #endif //  HAS_AUTO_FUNC_INLINE
