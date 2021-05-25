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


#include <iostream>
#include <string>

#include "codegen.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "reduction.h"
#include "rose.h"

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
#define USED_CODE_IN_COVERAGE_TEST 0
#if USED_CODE_IN_COVERAGE_TEST
void Reduction_altera(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options);
#endif
int reduction_top(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &options) {
#if USED_CODE_IN_COVERAGE_TEST
  //  No more in use
  if (options.get_option_key_value("-a", "altera") == "altera") {
    Reduction_altera(codegen, pTopFunc, options);
    return 0;
  }
#endif
  reduction_run(codegen, pTopFunc, options);
  return 0;
}

#if USED_CODE_IN_COVERAGE_TEST

void *simplify_index_in_exp(CSageCodeGen *codegen, void *pntr) {
  CMarsAccess ma(pntr, codegen, nullptr);
  void *array = ma.GetArray();
  vector<void *> new_indices;
  vector<CMarsExpression> old_indices = ma.GetIndexes();
  for (auto &me : old_indices) {
    new_indices.push_back(me.get_expr_from_coeff());
  }
  return codegen->CreateArrayRef(codegen->CreateVariableRef(array),
                                 new_indices);
}

void *replace_variables_in_scope(CSageCodeGen *codegen, void *sg_scope,
                                 const map<void *, void *> &mapIter) {
  void *new_scope = nullptr;
  if (codegen->IsStatement(sg_scope))
    new_scope = codegen->CopyStmt(sg_scope);
  else if (codegen->IsExpression(sg_scope))
    new_scope = codegen->CopyExp(sg_scope);
  else
    return nullptr;

  vector<void *> vec_refs;
  codegen->GetNodesByType(new_scope, "preorder", "SgVarRefExp", &vec_refs);
  for (auto &ref : vec_refs) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    auto new_var_iter = mapIter.find(var_init);
    if (new_var_iter == mapIter.end())
      continue;
    void *new_ref = nullptr;
    if (codegen->IsInitName(new_var_iter->second))
      new_ref = codegen->CreateVariableRef((*new_var_iter).second);
    else if (codegen->IsExpression(new_var_iter->second))
      new_ref = codegen->CopyExp((*new_var_iter).second);
    if (new_ref)
      codegen->ReplaceExp(ref, new_ref);
    if (codegen->IsVarRefExp(new_scope))
      return new_ref;
  }
  return new_scope;
}

void get_unrelated_loops(CSageCodeGen *codegen, void *sg_ref, void *sg_pos,
                         const vector<void *> &all_loops,
                         vector<void *> *unrelated_loops) {
  for (auto loop : all_loops) {
    void *iter = codegen->GetLoopIterator(loop);
    vector<void *> refs;
    codegen->get_ref_in_scope(iter, sg_ref, &refs);
    if (refs.size() == 0)
      unrelated_loops->push_back(loop);
  }
}

void get_unrelated_loops(CSageCodeGen *codegen, vector<void *> sg_ref,
                         void *sg_pos, const vector<void *> &all_loops,
                         vector<void *> *unrelated_loops) {
  get_unrelated_loops(codegen, sg_ref[0], sg_pos, all_loops, unrelated_loops);
}

//  A simple version of block-based reduction
//
//  Process;
//  1. Find the "reduce" pragma, the statement follow the pragma is to be
//  reduced
//  2. Find the scope of parallelization (the outermost parallelized loop)
//  3. Find the external variables (the reduction results)
//  4. Find the assignments to the external variables
//  5. copy the declaration of the external varialbes to internal, just before
//  the parallel scope
//  6. create a map between exteranl reference and private reference
//  6. for the reduce stmt, replace the external variable reference with new
//  internal variables
//  7. copy the  reduce stmt to just after the parallel loop
//  8. replace the private references in the copied reduce stmt with the new
//  interval variables
//
//  Assumptions:
//  1. feasibility of reduction is guaranteed by the users
//  2. write to external variable can only be "ext_val = private_val" or
//  "ext_val
//  += private_val"
//  3. the non-scalar reduction variables are of array type (not pointer)
//  4. Initialization of the reduction variables are constant
//  5. parallelized loops are perfectly nested
//  6. the index of the pntr's of the array reduciton referneces need be the
//  same
void Reduction_altera(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options) {
  cout << "Hello Reduction for Altera" << endl;

  vector<void *> vec_pragmas;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
                          &vec_pragmas);

  vector<void *> vec_reduce_stmt;
  for (auto pragma : vec_pragmas) {
    string str_pragma = codegen->GetPragmaString(pragma);
    if (str_pragma.find("ACCEL reduction") == 0 ||
        str_pragma.find("ACCEL REDUCTION") == 0) {
      cout << "Pragma : " << str_pragma << endl;

      void *sg_reduce_stmt = codegen->GetNextStmt(pragma);
      assert(sg_reduce_stmt);

      vec_reduce_stmt.push_back(sg_reduce_stmt);
    }
  }

  for (auto reduce_stmt : vec_reduce_stmt) {
    //  1. preparation
    //  1.1. Find the "reduction" pragma, the statement follow the pragma is to
    //  be reduced 1.2. Find the scope of parallelization (the outermost
    //  parallelized loop) 1.3. Find the external variables (the reduction
    //  results) 1.4. Find the assignments to the external variables 1.5. create
    //  a map between exteranl reference and private reference
    cout << "reduce: " << codegen->_p(reduce_stmt) << endl;
    void *func_decl = codegen->TraceUpToFuncDecl(reduce_stmt);
    void *func_body = codegen->GetFuncBody(func_decl);
    vector<void *> parallel_loops;  //  inverse order: inner loop first
    {
      void *sg_curr = codegen->TraceUpToForStatement(reduce_stmt);
      while (sg_curr) {
        void *sg_before = codegen->GetPreviousStmt(sg_curr);
        if (!codegen->IsPragmaDecl(sg_before))
          break;
        cout << "before: " << codegen->_p(sg_before) << endl;
        CAnalPragma ana_pragma(codegen);
        bool errorOut;
        if (!ana_pragma.PragmasFrontendProcessing(sg_before, &errorOut,
                                                  false) ||
            !ana_pragma.is_parallel() ||
            ana_pragma.get_attribute(CMOST_complete) == "")
          break;

        parallel_loops.push_back(sg_curr);

        cout << "Add a loop: " << codegen->_p(sg_curr) << endl;

        sg_curr = codegen->TraceUpToForStatement(codegen->GetParent(sg_curr));
      }
    }

    if (!parallel_loops.size())
      return;

    void *outermost_loop = parallel_loops[parallel_loops.size() - 1];
    void *insert_after_pos = outermost_loop;
    void *insert_before_pos = codegen->GetPreviousStmt(outermost_loop);

    map<void *, void *> mapE2P;    //  external to private
    map<void *, void *> mapE2ref;  //  external to private
    {
      //  vector<void*> vec_refs;
      //  codegen->GetNodesByType(reduce_stmt, "preorder",  "SgVarRefExp",
      //  &vec_refs);

      //  find assign statement
      //  assumption: external_var = (+=) private

      vector<void *> vec_assign;
      codegen->GetNodesByType(reduce_stmt, "preorder", "SgAssignOp",
                              &vec_assign);
      if (vec_assign.size() == 0) {
        codegen->GetNodesByType(reduce_stmt, "preorder", "SgPlusAssignOp",
                                &vec_assign);
      }

      for (auto assign : vec_assign) {
        void *left = codegen->GetExpLeftOperand(assign);
        void *right = codegen->GetExpRightOperand(assign);
        right = codegen->RemoveCast(right);

        vector<void *> indexes;
        void *sg_array;
        codegen->parse_pntr_ref_new(left, &sg_array, &indexes);
        void *sg_private = nullptr;

        //  ZP: FIXME: This replacement is NOT complete
        //            not only the right of assign, but also the references in
        //            the if-condition needs to be replaced In general, no pntr
        //            is allowed in private variables
        if (!codegen->IsVarRefExp(right)) {
          //  FIXME: to do moveable test
          string tmp_var = "__rdc_tmp";
          codegen->get_valid_local_name(func_decl, &tmp_var);
          void *tmp_decl = codegen->CreateVariableDecl(
              codegen->GetTypeByExp(right), tmp_var, nullptr, func_body);
          codegen->PrependChild(func_body, tmp_decl, true);
          void *new_assign_stmt = codegen->CreateStmt(
              V_SgAssignStatement, codegen->CreateVariableRef(tmp_decl),
              codegen->CopyExp(right));
          codegen->InsertStmt(new_assign_stmt, reduce_stmt);
          sg_private = codegen->GetVariableInitializedName(tmp_decl);
          codegen->ReplaceExp(right, codegen->CreateVariableRef(tmp_decl));
        } else {
          sg_private = codegen->GetVariableInitializedName(right);
        }
        mapE2P[sg_array] = sg_private;
        mapE2ref[sg_array] = simplify_index_in_exp(codegen, left);
        codegen->ReplaceExp(left, mapE2ref[sg_array]);

        cout << "MAP: " << codegen->UnparseToString(sg_array) << ":"
             << codegen->_p(mapE2ref[sg_array]) << "->"
             << codegen->UnparseToString(sg_private) << endl;
      }
    }

    map<void *, void *> mapE2init;  //  external to init expression
    {
      void *sg_scope = codegen->TraceUpToFuncDecl(reduce_stmt);

      vector<void *> vec_assign;
      codegen->GetNodesByType(sg_scope, "preorder", "SgAssignOp", &vec_assign);
      if (vec_assign.size() == 0) {
        codegen->GetNodesByType(sg_scope, "preorder", "SgPlusAssignOp",
                                &vec_assign);
      }

      {
        codegen->GetNodesByType(sg_scope, "preorder", "SgAssignInitializer",
                                &vec_assign);
      }

      for (auto assign : vec_assign) {
        if (codegen->is_located_in_scope_simple(assign, reduce_stmt))
          continue;
        cout << "find init \n";
        void *left = nullptr;
        void *right = nullptr;

        void *sg_array = nullptr;

        int is_initializer = codegen->IsAssignInitializer(assign);

        if (!is_initializer) {
          left = codegen->GetExpLeftOperand(assign);
          right = codegen->GetExpRightOperand(assign);
          vector<void *> indexes;
          codegen->parse_pntr_ref_new(left, &sg_array, &indexes);
        } else {
          sg_array =
              codegen->TraceUpByTypeCompatible(assign, V_SgInitializedName);
          if (!sg_array)
            continue;
          right = codegen->GetInitializerOperand(assign);
        }
        right = codegen->RemoveCast(right);

        if (mapE2P.find(sg_array) != mapE2P.end()) {
          mapE2init[sg_array] = codegen->CopyExp(right);

#if PROJDEBUG
          cout << "Find an init: " << codegen->UnparseToString(sg_array)
               << " = " << codegen->_p(right) << endl;
#endif
        }
      }
    }

    //  2. create declaration
    //  2.1 copy the declaration of the external varialbes to internal, just
    //  before the parallel scope
    map<void *, void *> mapE2new;
    map<void *, void *> mapE2new_ref;
    {
      void *insert_scope = codegen->GetScope(insert_before_pos);

      for (auto sg_pair : mapE2P) {
        void *sg_array = sg_pair.first;
#if PROJDEBUG
        cout << "add declaration:  " << codegen->UnparseToString(sg_array)
             << endl;
#endif

        void *sg_type = codegen->GetTypebyVar(sg_array);
        string str_var = codegen->UnparseToString(sg_array);
        void *new_decl = codegen->CreateVariableDecl(sg_type, str_var + "_rdc",
                                                     nullptr, insert_scope);
        codegen->InsertStmt(new_decl, insert_before_pos);

        mapE2new[sg_array] = codegen->GetVariableInitializedName(new_decl);

        {
          void *sg_ref = codegen->CopyExp(mapE2ref[sg_array]);
          mapE2new_ref[sg_array] =
              replace_variables_in_scope(codegen, sg_ref, mapE2new);
        }
      }
    }

    //  3. create initilization
    {
      for (auto sg_pair : mapE2P) {
        void *sg_array = sg_pair.first;
        void *sg_ref = mapE2ref[sg_array];

        if (!sg_ref)
          continue;
        //  if (mapE2init.find(sg_array) == mapE2init.end()) continue;
        //  void * sg_init_value = mapE2init[sg_array];
        void *sg_init_value = nullptr;
        if (mapE2init.find(sg_array) != mapE2init.end() &&
            codegen->IsValueExp(
                mapE2init[sg_array]))  // ZP: 20170106 //  only induct the
                                       //  constant initial value
          sg_init_value = codegen->CopyExp(mapE2init[sg_array]);
        else
          sg_init_value = codegen->CreateConst(0);

        //  3.1 get indexes
        vector<void *> indexes;
        void *sg_array1;
        codegen->parse_pntr_ref_new(sg_ref, &sg_array1, &indexes);
        assert(sg_array == sg_array1);

        //  3.2 find the loops that do not exists in the index
        vector<void *> unrelated_loops;
        get_unrelated_loops(codegen, sg_ref, reduce_stmt, parallel_loops,
                            &unrelated_loops);

        //  3.3 create and insert the loop
        void *new_body = nullptr;
        map<void *, void *> mapIter;
        {
          copy_and_insert_for_loop_excluding_loops(codegen, outermost_loop,
                                                   unrelated_loops, mapIter,
                                                   new_body, insert_before_pos);
        }

        //  3.4 replace the refernece expression
        void *sg_new_ref = mapE2new_ref[sg_array];
        sg_new_ref = replace_variables_in_scope(codegen, sg_new_ref, mapIter);

        //  3.5 create and insert the init assignement
        void *sg_init_stmt = codegen->CreateStmt(
            V_SgExprStatement,
            codegen->CreateExp(V_SgAssignOp, sg_new_ref,
                               codegen->CopyExp(sg_init_value)));
        if (!new_body)
          codegen->InsertStmt(sg_init_stmt, insert_before_pos);
        else
          codegen->AppendChild(new_body, sg_init_stmt);
      }
    }

    //
    //
    //  4. final reduction
    //  4.1 copy the reduce stmt to just after the parallel loop
    //  4.1 replace the private references in the copied reduce stmt with the
    //  new interval variables
    {
      map<void *, void *> mapReduceVar;
      vector<void *> vec_refs;

      for (auto pair : mapE2P) {
        void *sg_private = pair.second;
        void *sg_array = pair.first;
        void *sg_ref = mapE2new_ref[sg_array];

        mapReduceVar[sg_private] = sg_ref;

        vec_refs.push_back(sg_ref);
      }

      void *new_reduce_stmt =
          replace_variables_in_scope(codegen, reduce_stmt, mapReduceVar);

      //  4.2 find the loops that do not exists in the index
      vector<void *> unrelated_loops;
      get_unrelated_loops(codegen, vec_refs, reduce_stmt, parallel_loops,
                          &unrelated_loops);

      //  4.3 create and insert the loop
      void *new_body = nullptr;
      map<void *, void *> mapIter;
      {
        copy_and_insert_for_loop_excluding_loops(
            codegen, outermost_loop, unrelated_loops, mapIter, new_body,
            insert_after_pos, false);
      }

      codegen->AppendChild(new_body, new_reduce_stmt);
    }

    //  5. update reduce stmt
    //  5.1 replace the external variable reference with new internal variables
    {
      void *new_reduce_stmt =
          replace_variables_in_scope(codegen, reduce_stmt, mapE2new);
      codegen->ReplaceStmt(reduce_stmt, new_reduce_stmt);
    }
  }
}
#endif

void reduction_run(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options) {
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  mars_ir.update_loop_node_info(codegen);

  string str_scheme = options.get_option_key_value("-a", "scheme");

  std::cout << "==============================================" << std::endl;
  std::cout << "-----=# Reduction Optimization Start#=----\n";
  std::cout << "==============================================" << std::endl;

  if (options.get_option_key_value("-a", "naive_hls") == "naive_hls") {
    cout << "[MARS-REDUCTION-MSG] Naive HLS mode.\n";
    return;
  }

  set<void *> rdc_buf;
  bool Changed = true;
  while (Changed) {
    Changed = false;

    vector<CMirNode *> vec_nodes;
    mars_ir.get_topological_order_nodes(&vec_nodes);

    for (size_t j = 0; j < vec_nodes.size(); j++) {
      CMirNode *new_node = vec_nodes[j];
      if (new_node->is_while) {
        continue;
      }
      //  Check syntax for reduction
      //  IF pipeline pragma is recursively inserted, only reduction
      //  in the innermost level will be applied
      if (new_node->is_fine_grain && (new_node->has_pipeline() != 0) &&
          (new_node->has_parallel() == 0)) {
        Changed |= insert_reduction(codegen, pTopFunc, &mars_ir, new_node, j,
                                    str_scheme, &rdc_buf);
      }
      if (Changed) {
        mars_ir.clear();
        codegen->reset_func_decl_cache();
        codegen->reset_func_call_cache();
        mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
        mars_ir.update_loop_node_info(codegen);
        codegen->init_defuse_range_analysis();
        break;
      }
    }
  }
  for (auto it : mars_ir.ir_list) {
    CMirNode *fNode = it.get_top_node();
    codegen->clean_empty_aggregate_initializer(fNode->ref);
  }
  remove_reduction_pragma(codegen, pTopFunc, rdc_buf);

  std::cout << "============================================" << std::endl;
  std::cout << "-----=# Reduction Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;
}

//  Handling reduction transformation for a loop nest
bool insert_reduction(CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir,
                      CMirNode *bNode, int node_num, string str_scheme,
                      set<void *> *rdc_buf) {
  Rose_STL_Container<SgNode *> op_list =
      NodeQuery::querySubTree(bNode->ref, V_SgPlusAssignOp);
  bool Changed = false;

  //  ZP: 2015.11.29 reduction function
  {
    vector<void *> all_calls;
    codegen->GetNodesByType(bNode->ref, "preorder", "SgFunctionCallExp",
                            &all_calls);
    for (size_t i = 0; i < all_calls.size(); i++) {
      void *curr_call = all_calls[i];
      void *sg_func_decl = codegen->GetFuncDeclByCall(curr_call);
      int is_function_reduction =
          static_cast<int>(mars_ir->is_reduction(codegen, sg_func_decl));

      if (is_function_reduction != 0) {
        op_list.push_back(static_cast<SgNode *>(all_calls[i]));
      }
    }
  }

  //  remove += in for loop increment
  {
    Rose_STL_Container<SgNode *> op_list1;
    for (size_t i = 0; i < op_list.size(); i++) {
      SgNode *curr_op = op_list[i];
      //  ZP: TODO: please replace this Sage function with codegen functions:
      //  TraceUpToForStatement
      SgForStatement *forLoop =
          SageInterface::getEnclosingNode<SgForStatement>(curr_op);
      SgExpression *step_exp = forLoop->get_increment();
      if (step_exp == curr_op) {
        cout << "Plus assign operation " << curr_op->unparseToString()
             << " is found in for increment expression.\n";
        continue;
      }

      op_list1.push_back(curr_op);
    }
    op_list = op_list1;
  }

  int n_size = op_list.size();
  if (n_size > 1) {
    for (int i = 0; i < n_size; i++) {
      void *curr_op = codegen->TraceUpToStatement(op_list[i]);
      if (codegen->IsTransformation(curr_op)) {
        return false;
      }
    }
    if (g_debug_mode != 0) {
      void *loop = codegen->TraceUpToForStatement(bNode->ref);
      string str_ref = codegen->UnparseToString(loop);
      if (str_ref.length() > 20) {
        str_ref = str_ref.substr(0, 20) + " ...";
      }
      //            string msg = "Disabling reduction in loop '" + str_ref + "'
      //  (" + sFile +
      //                ":" + my_itoa(nVarLine) +
      //                ") because of multiple reduction operations:\n";
      string loop_info =
          "'" + str_ref + "' " + getUserCodeFileLocation(codegen, loop, true);
      string op_info = " ";
      for (int i = 0; i < n_size; i++) {
        void *curr_op = codegen->TraceUpToStatement(op_list[i]);
        str_ref = codegen->UnparseToString(curr_op);
        if (str_ref.length() > 20) {
          str_ref = str_ref.substr(0, 20) + " ...";
        }
        //        msg +=
        op_info += "   " + str_ref + " " +
                   getUserCodeFileLocation(codegen, curr_op, true) + " \n";
      }
      //        msg +=
      //            "   Hint: try to group these operations into a reduction
      //  function\n";     cout << msg << endl; dump_critical_message("REDUC",
      //  "WARNING", msg, 201);
      dump_critical_message(REDUC_WARNING_1(loop_info, op_info));
    }
    return false;
  }

  Rose_STL_Container<SgNode *>::iterator it = op_list.begin();
  for (it = op_list.begin(); it != op_list.end(); ++it) {
    SgExpression *curr_op = isSgPlusAssignOp(*it);
    SgFunctionCallExp *curr_call = isSgFunctionCallExp(*it);
    void *sg_func_decl = codegen->GetFuncDeclByCall(curr_call);
    int is_function_reduction =
        static_cast<int>(mars_ir->is_reduction(codegen, sg_func_decl));

    //  ZP: reduction function
    if ((curr_op == nullptr) && (is_function_reduction == 0)) {
      continue;
    }
    if (curr_op == nullptr) {
      curr_op = curr_call;
    }

    //  1. Do basic check and get the basic information for the C loop/Node
    SgForStatement *forLoop =
        SageInterface::getEnclosingNode<SgForStatement>(curr_op);
    CMirNode *cNode;
    {
      //  Check if there is a loop body for the enclosing loop
      {
        SgBasicBlock *loop_body = isSgBasicBlock(forLoop->get_loop_body());
        cNode = mars_ir->get_node(loop_body);
        if (cNode == nullptr) {
          continue;
        }
      }
    }

    //  2. Get arrays to reduce
    vector<void *> pntr_to_reduce;         //  init name
    SgExpression *arr_pntr_ref = nullptr;  //  the reduction pntr
    {
      SgInitializedName *arr_var;
      void *var_ref;
      if (isSgPlusAssignOp(curr_op) != nullptr) {
        arr_pntr_ref = isSgPlusAssignOp(*it)->get_lhs_operand();
        string arrName = codegen->GetSgPntrArrName(arr_pntr_ref);

        if (codegen->get_var_from_pntr_ref(arr_pntr_ref, &var_ref) == 0) {
          arr_var = isSgInitializedName(static_cast<SgNode *>(
              codegen->GetVariableInitializedName(arr_pntr_ref)));
        } else {
          arr_var = isSgInitializedName(static_cast<SgNode *>(
              codegen->GetVariableInitializedName(var_ref)));
        }

        cout << codegen->_p(arr_pntr_ref) << endl;
        if (rdc_buf->find(arr_pntr_ref) != rdc_buf->end()) {
          cout << "Skip new generated buffer\n";
          continue;
        }

        if (arr_var == nullptr) {
          continue;
        }
        //  FIXME
        int ref_num = get_ref_num(codegen, bNode->ref, arr_var);
        if (ref_num > 1) {
#if DEBUG
          cout << "[MARS-REDUCTION-MARN] Multiple accesses to array '"
               << arr_var->unparseToString()
               << "'. Reduction optimization quit." << endl;
#endif
          continue;
        }

        pntr_to_reduce.push_back(arr_pntr_ref);
      } else if (isSgFunctionCallExp(curr_op) != nullptr) {
        int n_param = codegen->GetFuncCallParamNum(curr_op);
        assert(n_param % 2 == 0);
        int n_var_reduce = n_param / 2;
        for (int t = 0; t < n_var_reduce; t++) {
          pntr_to_reduce.push_back(codegen->GetFuncCallParam(curr_op, t));
        }
        arr_pntr_ref = curr_op;
      }
    }
    if (arr_pntr_ref == nullptr)
      continue;

    //  /////////////////////////////////////////////  /

    //  ZP: FIXME: currently, skip feasibility and profitability check for
    //  function reduction

    if ((is_function_reduction != 0) ||
        anal_reduction(codegen, bNode, cNode, arr_pntr_ref)) {
      int lift_level = 0;
      lift_level = lift_location(codegen, bNode, cNode, pntr_to_reduce);
#if DEBUG
      cout << "Reduction lifting can be applied for " << lift_level << " level "
           << endl;
#endif

      if (lift_level > 0) {
        void *forLoop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
        if (!check_loop_initializer(codegen, forLoop, lift_level)) {
          reportLoopLowerBound(codegen, forLoop);
          continue;
        }
      }
      //  Yuxin: If there is reduction optimization, then comment all the label
      //  statements.
      remove_labels(codegen, pTopFunc);

      if (!build_reduction(codegen, bNode, cNode, curr_op, lift_level,
                           pntr_to_reduce, str_scheme, rdc_buf)) {
        reportNoBenefit(codegen, curr_op);
        cout << "No benefit from reduction. Quit reduction for "
             << codegen->_up(arr_pntr_ref) << endl;
        continue;
      }
      cout << "[MARS-REDUCTION-MSG] Reduction is automatically applied" << endl;
      Changed |= true;
    } else {
      cout << "[MARS-REDUCTION-MSG] Reduction is not applied to "
           << codegen->_up(arr_pntr_ref) << endl;
    }
  }
  return Changed;
}

//  bNode-pipeline node; cNode: current node
bool anal_reduction(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *cNode,
                    SgExpression *arr_ref) {
  cout << "Start reduction analysis.\n";
  if (arr_ref == nullptr)
    return false;
  SgForStatement *forLoop = isSgForStatement(bNode->ref->get_scope());
  SgInitializedName *curr_iter = isSgInitializedName(
      static_cast<SgNode *>(codegen->GetLoopIterator(forLoop)));
  if (curr_iter == nullptr) {
    reportUnCanonicalLoop(codegen, forLoop);
    return false;
  }
  string curr_iter_str = curr_iter->unparseToString();

  //  Conditions for applying reduciton
  //  1. There is no other access except the reduction in the scope of b-loop
  //  2. There is at least one loop iterator between b-loop and c-loop which
  //  does not exists int all the index dimensions

  if (isSgPntrArrRefExp(arr_ref) != nullptr) {
    void *var_ref;
    codegen->get_var_from_pntr_ref(arr_ref, &var_ref);
    void *arr_init = codegen->GetVariableInitializedName(var_ref);

    if (arr_init == nullptr) {
      return false;
    }
    int ref_num = get_ref_num(codegen, bNode->ref, arr_init);
    if (ref_num > 1) {
      cout << "[MARS-REDUCTION-WARN] " << ref_num
           << " accesses to the array exists in this loop. Reduction "
              "process quit "
           << endl;
      return false;
    }
    CMarsAccess access1(arr_ref, codegen, cNode->ref);
    vector<CMarsExpression> indexes1 = access1.GetIndexes();

    SgForStatement *reduc_for =
        SageInterface::getEnclosingNode<SgForStatement>(cNode->ref);
    SgForStatement *pipe_for =
        SageInterface::getEnclosingNode<SgForStatement>(bNode->ref);

    int auto_block = 1;
    map<void *, int>::iterator it;
    while (codegen->IsInScope(reduc_for, pipe_for) != 0) {
      int reduc_tag = 1;
      for (size_t i = 0; i < indexes1.size(); i++) {
        map<void *, CMarsExpression> *m_coeff_1 = indexes1[i].get_coeff();
        map<void *, CMarsExpression>::iterator it;

        for (it = m_coeff_1->begin(); it != m_coeff_1->end(); it++) {
          cout << "[print] "
               << "[dim=" << i << "]" << codegen->_p(it->first) << endl;
        }

        if (m_coeff_1->find(reduc_for) != m_coeff_1->end()) {
          reduc_tag = 0;
          if (reduc_for != pipe_for) {
            auto_block = 0;
          }
          break;
        }

        if (reduc_tag != 0) {
          for (it = m_coeff_1->begin(); it != m_coeff_1->end(); it++) {
            cout << "[print] "
                 << "m_coeff: " << codegen->_p(it->first) << endl;
            if (codegen->IsIncludeVar(it->first,
                                      codegen->GetLoopIterator(reduc_for))) {
              reduc_tag = 0;
              if (reduc_for != pipe_for) {
                auto_block = 0;
              }
              break;
            }
          }
        }
        if (reduc_tag == 0) {
          break;
        }
      }
      if (((reduc_tag != 0) && (auto_block == 0)) ||
          ((reduc_tag != 0) && bNode->is_fine_grain)) {
        return true;
      }

      if ((auto_block != 0) && (reduc_for == pipe_for)) {
        cout << "[MARS-REDUCTION-MSG] Automatic block reduction will be "
                "inferred ref: "
             << codegen->_up(arr_ref) << endl;
        return false;
      }
      if (reduc_for == pipe_for) {
        cout << "[MARS-REDUCTION-MSG] Reduction is not required." << endl;
        return false;
      }
      reduc_for = SageInterface::getEnclosingNode<SgForStatement>(reduc_for);
    }
  } else if (isSgVarRefExp(arr_ref) != nullptr) {
    cout << "[print] reduction to variable " << codegen->_up(arr_ref) << endl;
    void *arr_init = codegen->GetVariableInitializedName(arr_ref);
    int ref_num = get_ref_num(codegen, bNode->ref, arr_init);
    if (ref_num > 1) {
      cout << "[MARS-REDUCTION-WARN] " << ref_num
           << " accesses to the array exists in this loop. Reduction process "
              "quit "
           << endl;
      return false;
    }
    return true;
  }
  return false;
}

int lift_location_internal(CSageCodeGen *codegen, CMirNode *bNode,
                           CMirNode *cNode, SgExpression *arr_ref) {
  SgForStatement *curr_for_0 =
      SageInterface::getEnclosingNode<SgForStatement>(bNode->ref);
  SgInitializedName *arr_var;
  void *var_ref;
  if (codegen->get_var_from_pntr_ref(arr_ref, &var_ref) == 0) {
    arr_var = isSgInitializedName(
        static_cast<SgNode *>(codegen->GetVariableInitializedName(arr_ref)));
  } else {
    arr_var = isSgInitializedName(
        static_cast<SgNode *>(codegen->GetVariableInitializedName(var_ref)));
  }

  int lift_level = 0;
  if (isSgPntrArrRefExp(arr_ref) != nullptr) {
    CMarsAccess access1(arr_ref, codegen, cNode->ref);
    vector<CMarsExpression> indexes1 = access1.GetIndexes();
    SgForStatement *lift_for =
        SageInterface::getEnclosingNode<SgForStatement>(bNode->ref);
    lift_for = SageInterface::getEnclosingNode<SgForStatement>(lift_for);

    while (lift_for != nullptr) {
      if (codegen->IsInScope(arr_var, lift_for) != 0) {
        return lift_level;
      }
      for (size_t i = 0; i < indexes1.size(); i++) {
        map<void *, CMarsExpression> *m_coeff_1 = indexes1[i].get_coeff();
        map<void *, CMarsExpression>::iterator it;
        if (m_coeff_1->find(lift_for) != m_coeff_1->end()) {
          return lift_level;
        }
        for (it = m_coeff_1->begin(); it != m_coeff_1->end(); it++) {
          if (codegen->IsIncludeVar(it->first,
                                    codegen->GetLoopIterator(lift_for))) {
            return lift_level;
          }
        }
      }

      void *ref_body = lift_for->get_loop_body();
      SgStatementPtrList stmt_lst;
      codegen->GetChildStmtList(ref_body, &stmt_lst);
      //  FIXME: for lifting Yuxin Dec 05 2015
      for (size_t i = 0; i < stmt_lst.size(); i++) {
        if (isSgVariableDeclaration(stmt_lst[i]) != nullptr) {
          SgInitializedName *init_name = isSgInitializedName(
              isSgVariableDeclaration(stmt_lst[i])->get_variables()[0]);
          if (init_name == arr_var) {
            return lift_level;
          }
        }

        Rose_STL_Container<SgNode *> varList =
            NodeQuery::querySubTree(stmt_lst[i], V_SgVarRefExp);
        Rose_STL_Container<SgNode *>::iterator var;

        for (var = varList.begin(); var != varList.end(); var++) {
          SgExpression *ref0 = isSgExpression(*var);
          SgInitializedName *arr_var0;
          void *var_ref;
          if (codegen->get_var_from_pntr_ref(ref0, &var_ref) == 0) {
            arr_var0 = isSgInitializedName(static_cast<SgNode *>(
                codegen->GetVariableInitializedName(ref0)));
          } else {
            arr_var0 = isSgInitializedName(static_cast<SgNode *>(
                codegen->GetVariableInitializedName(var_ref)));
          }

          if (arr_var0 == arr_var &&
              (codegen->IsInScope(ref0, curr_for_0) == 0)) {
            cout << "Multiple references to the variable exist in the outside "
                    "loop. Cannot be lifted anymore.\n";
            return lift_level;
          }
        }
      }

      lift_level++;
      lift_for = SageInterface::getEnclosingNode<SgForStatement>(lift_for);
    }
    return lift_level;
  }
  if (isSgVarRefExp(arr_ref) != nullptr) {
    SgForStatement *lift_for =
        SageInterface::getEnclosingNode<SgForStatement>(bNode->ref);
    lift_for = SageInterface::getEnclosingNode<SgForStatement>(lift_for);

    while (lift_for != nullptr) {
      if (codegen->IsInScope(arr_var, lift_for) != 0) {
        return lift_level;
      }

      void *ref_body = lift_for->get_loop_body();
      SgStatementPtrList stmt_lst;
      codegen->GetChildStmtList(ref_body, &stmt_lst);
      for (size_t i = 0; i < stmt_lst.size(); i++) {
        if (isSgVariableDeclaration(stmt_lst[i]) != nullptr) {
          SgInitializedName *init_name = isSgInitializedName(
              isSgVariableDeclaration(stmt_lst[i])->get_variables()[0]);
          if (init_name == arr_var) {
            return lift_level;
          }
        }

        {
          Rose_STL_Container<SgNode *> varList =
              NodeQuery::querySubTree(stmt_lst[i], V_SgVarRefExp);
          Rose_STL_Container<SgNode *>::iterator var;

          for (var = varList.begin(); var != varList.end(); var++) {
            SgExpression *ref0 = isSgExpression(*var);
            SgInitializedName *arr_var0;
            void *var_ref;
            if (codegen->get_var_from_pntr_ref(ref0, &var_ref) == 0) {
              arr_var0 = isSgInitializedName(static_cast<SgNode *>(
                  codegen->GetVariableInitializedName(ref0)));
            } else {
              arr_var0 = isSgInitializedName(static_cast<SgNode *>(
                  codegen->GetVariableInitializedName(var_ref)));
            }

            if (arr_var0 == arr_var &&
                (codegen->IsInScope(ref0, curr_for_0) == 0)) {
              return lift_level;
            }
            //                cout << "Multiple statements exist in the outside
            //  loop.
            //  Cannot be lifted anymore.\n";
          }
        }
      }

      lift_level++;
      lift_for = SageInterface::getEnclosingNode<SgForStatement>(lift_for);
    }
    return lift_level;
  }
  return 0;
}

int lift_location(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *cNode,
                  vector<void *> pntr_to_reduce) {
  int ret = 9999;
  for (size_t i = 0; i < pntr_to_reduce.size(); i++) {
    SgExpression *arr_pntr_ref =
        isSgExpression(static_cast<SgNode *>(pntr_to_reduce[i]));
    int lift_level =
        lift_location_internal(codegen, bNode, cNode, arr_pntr_ref);
    if (lift_level < ret) {
      ret = lift_level;
    }
  }

  return ret;
}

//  /////////////////////////////////////////////  /
//  ZP: 20151203: Note:
//  There are two ways to handle the reduction loop inside the pipeline loop
//  Scheme0: interleaved mode, the reduction loop is parallelized by allocating
//           additional dimension in the reduction buffer; and then the
//           reduction
//           dimensions are reduced layer-by-layer in the postproc loop nests
//  Scheme1: block mode, additional local reduction buffers are allocated inside
//           the pipeline loop; local reduction buffers only contains
//           the parallel dimensions; and their value are reduced to the global
//           reduce buffer at each iteration of the pipeline loop; only one
//           postproc step is needed to reduce the iterations for the pipeline
//           loop
//  Comparison: from current finding, scheme1 is better than scheme0 in case
//           of massive parallelism: less BRAM and LUT utilization, and better
//           timing because of smaller fanout
//  Note: scheme 1 is disabled if the pipeline loop is not a reduction loop
//  #define REDUCE_SCHEME 0
//  #define REDUCE_SCHEME 1
bool build_reduction(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *cNode,
                     SgExpression *curr_op, int lift_level,
                     vector<void *> pntr_to_reduce, string str_scheme,
                     set<void *> *rdc_buf) {
  int REDUCE_SCHEME = 1;
  if (!str_scheme.empty()) {
    REDUCE_SCHEME = my_atoi(str_scheme);
  }
  assert(REDUCE_SCHEME == 0 || REDUCE_SCHEME == 1);

  int i;
  map<int, SgInitializedName *> curr_iter_map;
  //  SgBasicBlock *sg_bb = isSgBasicBlock(static_cast<SgNode
  //  *>(codegen->CreateBasicBlock()));

  //  Note: preparation for building
  SgForStatement *forLoop = isSgForStatement(bNode->ref->get_scope());
  SgInitializedName *curr_iter = isSgInitializedName(
      static_cast<SgNode *>(codegen->GetLoopIterator(forLoop)));
  if (curr_iter == nullptr) {
    reportUnCanonicalLoop(codegen, forLoop);
    return false;
  }
  //  Note: find reduction insert location
  SgForStatement *insertLoop;
  insertLoop = SageInterface::getEnclosingNode<SgForStatement>(bNode->ref);
  for (i = 0; i < lift_level; i++) {
    insertLoop = SageInterface::getEnclosingNode<SgForStatement>(insertLoop);
  }
  assert(insertLoop);

  //  ////////////////////////////////////////////////  /
  //  ZP: Support the Multi-level reduction (Scheme 0)
  //  1. Get the parallel loops and reduction loops
  //  2. Create declearation and reference of multiple level for multiple
  //  references
  //  3. Initialize the reduction buffer with original init value or zero
  //  4. Replace the references in the reduciton loop (add dependence false)
  //  5. Create post processing loop nests
  //  5.1 Each post loop reduce the partial results from level i to leven i+1

  //  ////////////////////////////////////////////////  /
  //  ZP: Support the block-based reduction (Scheme 1)
  //  1. the same as Scheme 0
  //  2. Create decleration and reference of local reduction buffers
  //  3. Initialize the local reduction buffers using input values
  //  4. Replace the references
  //  5. simple post reduciton (only one layer)
  //

  //  ////////////////////////////////////////  /
  //  1. Get Basic Loop Information
  //  ////////////////////////////////////////  /
  map<void *, int> loop_is_reduct;
  map<void *, int> loop_is_parallel;
  vector<void *> vec_reduct_loops;
  vector<void *> vec_parallel_loops;
  vector<void *> loop_nest;
  void *pipeline_loop = isSgForStatement(bNode->ref->get_scope());
  void *innermost_loop = isSgForStatement(cNode->ref->get_scope());
  {
    void *curr_loop = innermost_loop;
    while (curr_loop != nullptr) {
      void *iter = codegen->GetLoopIterator(curr_loop);
      if (iter == nullptr) {
        reportUnCanonicalLoop(codegen, curr_loop);
        return false;
      }
      //  for each loop, check if the iterator is in all the references
      int is_parallel = 1;
      for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
        CMarsAccess ma(pntr_to_reduce[l], codegen, cNode->ref);
        vector<CMarsExpression> indexes = ma.GetIndexes();
        int one_parallel = 0;
        for (size_t t = 0; t < indexes.size(); t++) {
          if ((indexes[t].get_coeff(curr_loop) != 0) != 0) {
            one_parallel = 1;
            break;
          }
        }
        if (one_parallel == 0) {
          is_parallel = 0;
          break;
        }
      }

      if (is_parallel != 0) {
        loop_is_parallel[curr_loop] = 1;
        vec_parallel_loops.push_back(curr_loop);
      } else {
        loop_is_reduct[curr_loop] = 1;
        vec_reduct_loops.push_back(curr_loop);
      }
      cout << "TEST:\n";
      cout << codegen->_p(curr_loop) << endl;
      loop_nest.push_back(curr_loop);
      if (curr_loop == pipeline_loop) {
        break;
      }
      curr_loop = codegen->TraceUpToForStatement(codegen->GetParent(curr_loop));
    }

    std::reverse(vec_reduct_loops.begin(), vec_reduct_loops.end());
    std::reverse(vec_parallel_loops.begin(), vec_parallel_loops.end());
    std::reverse(loop_nest.begin(), loop_nest.end());

    {
      cout << codegen->_p(curr_op) << endl;
      for (size_t t = 0; t < loop_nest.size(); t++) {
        if (loop_is_parallel.find(loop_nest[t]) != loop_is_parallel.end()) {
          cout << "Parallel loop: " << codegen->_p(loop_nest[t]) << endl;
        }
        if (loop_is_reduct.find(loop_nest[t]) != loop_is_reduct.end()) {
          cout << "Reduction loop: " << codegen->_p(loop_nest[t]) << endl;
        }
      }
    }
  }
  void *parent_loop =
      codegen->TraceUpToForStatement(codegen->GetParent(loop_nest[0]));
  int reduct_level = loop_is_reduct.size();

  vector<size_t>
      loop_dim_sizes;  //  from bnode (pipeline loop) to cnode (innermost loop)
  {
    size_t t0;
    for (t0 = 0; t0 < loop_nest.size(); t0++) {
      int i = static_cast<int>(bNode->iterators_num - 1 +
                               t0);  //  from outer to inner
      int lower_range = cNode->range_lower_table[i];
      int upper_range = cNode->range_upper_table[i];
      int upper_err = cNode->upper_err_table[i];
      int dim_size = upper_range + upper_err - lower_range + 1;
      if (lower_range > 0) {
        dim_size = upper_range + upper_err + 1;
      }

      cout << "dim size: " << dim_size << endl;
      loop_dim_sizes.push_back(dim_size);
    }
  }
  int scheme_1_enabled = static_cast<int>(
      REDUCE_SCHEME == 1 && reduct_level > 1 &&
      loop_is_reduct.find(loop_nest[0]) != loop_is_reduct.end());

  //  FIXME: Only apply for interleaved reduction, Jan 07 2016 Yuxin
  //  In interleaved reduction, when lifting level<1, reduction will stop
  if ((scheme_1_enabled == 0) && lift_level <= 0) {
    //    cout << "No benefit from interleaved reduction. Quit reduction"
    //    <<endl;
    return false;
  }
  //  Note: find all the pragmas
  vector<vector<void *>> vec_pragmas;
  {
    int t0 = 0;
    //  vec_pragmas.resize(loop_nest.size());
    SgForStatement *inner_loop = isSgForStatement(cNode->ref->get_scope());
    for (int i = static_cast<int>(cNode->iterators_num - 1);
         i >= static_cast<int>(bNode->iterators_num - 1); i--) {
      SgInitializedName *inner_iter = isSgInitializedName(
          static_cast<SgNode *>(codegen->GetLoopIterator(inner_loop)));
      if (inner_iter == nullptr) {
        reportUnCanonicalLoop(codegen, inner_loop);
        return false;
      }
      if (scheme_1_enabled != 0) {
        int t0 = static_cast<int>(loop_nest.size() - 1 -
                                  (cNode->iterators_num - 1 - i));

        void *loop = loop_nest[t0];
        if (loop_is_reduct.find(loop) != loop_is_reduct.end() && t0 != 0) {
          inner_loop =
              SageInterface::getEnclosingNode<SgForStatement>(inner_loop);
          continue;
        }
      }

      {
        vector<void *> curr_pragmas;
        SgStatementPtrList stmt_lst;

        codegen->GetChildStmtList(inner_loop->get_loop_body(), &stmt_lst);
        for (size_t inst = 0; inst < stmt_lst.size(); inst++) {
          if (isSgPragmaDeclaration(stmt_lst[inst]) != nullptr) {
            SgPragmaDeclaration *p_decl = isSgPragmaDeclaration(stmt_lst[inst]);

            curr_pragmas.push_back(p_decl);
          }
        }
        inner_loop =
            SageInterface::getEnclosingNode<SgForStatement>(inner_loop);
        t0++;
        vec_pragmas.push_back(curr_pragmas);
      }
    }
    std::reverse(vec_pragmas.begin(), vec_pragmas.end());
  }

  //  display
  if (g_debug_mode != 0) {
    string str_scheme = "interleaved";
    if (scheme_1_enabled != 0) {
      str_scheme = "block-based";
    }

    string str_ref = codegen->UnparseToString(curr_op);
    if (str_ref.length() > 20) {
      str_ref = str_ref.substr(0, 20) + " ...";
    }
    string msg = "Applying " + str_scheme + " reduction on operation '" +
                 str_ref + "' " +
                 getUserCodeFileLocation(codegen, curr_op, true) + "\n";
    cout << msg << endl;
    string op_info =
        "'" + str_ref + "' " + getUserCodeFileLocation(codegen, curr_op, true);
    dump_critical_message(REDUC_INFO_1(str_scheme, op_info));
  }

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 2.1 Create the declaration for the reduction buffers
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  //  Note: insert new buffer declaration
  vector<void *> new_decl;
  vector<vector<void *>>
      new_decl_ln;  //  the multi-layer reduction variables (for scheme 0)
  vector<void *>
      new_decl_local;  //  the local reduction variables (for scheme 1)
  void *insert_pos_local = nullptr;
  new_decl_ln.resize(reduct_level - 1);
  //  FIXME: to support multiple reduction variables
  for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
    SgInitializedName *target_arr;
    SgExpression *arr_ref =
        isSgExpression(static_cast<SgNode *>(pntr_to_reduce[l]));
    {
      void *var_ref;
      if (codegen->get_var_from_pntr_ref(arr_ref, &var_ref) == 0) {
        target_arr = isSgInitializedName(static_cast<SgNode *>(
            codegen->GetVariableInitializedName(arr_ref)));
      } else {
        target_arr = isSgInitializedName(static_cast<SgNode *>(
            codegen->GetVariableInitializedName(var_ref)));
      }
    }

    SgArrayType *arr_type;
    SgType *arr_base_type = target_arr->get_type()->findBaseType();
    if (scheme_1_enabled == 1) {
      //  ignore the reduction loop dimensions inside pipeline loop

      size_t t0;
      vector<size_t> dim_sizes;
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        if (t0 == 0 ||
            loop_is_reduct.find(loop_nest[t0]) == loop_is_reduct.end()) {
          dim_sizes.push_back(loop_dim_sizes[t0]);
        }
      }
      arr_type = isSgArrayType(static_cast<SgNode *>(
          codegen->CreateArrayType(arr_base_type, dim_sizes)));
    } else {
      arr_type = isSgArrayType(static_cast<SgNode *>(
          codegen->CreateArrayType(arr_base_type, loop_dim_sizes)));
    }

    void *array_type_local;  //  scheme 1
    {
      //  local buffer only has parallel dimensions
      size_t t0;
      vector<size_t> dim_sizes;
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        if (t0 != 0 &&
            loop_is_parallel.find(loop_nest[t0]) != loop_is_parallel.end()) {
          dim_sizes.push_back(loop_dim_sizes[t0]);
        }
      }
      if (dim_sizes.empty()) {
        array_type_local = arr_base_type;
      } else {
        array_type_local = isSgArrayType(static_cast<SgNode *>(
            codegen->CreateArrayType(arr_base_type, dim_sizes)));
      }
    }

    vector<void *> array_type_ln;  //  for each level (scheme 0)
    array_type_ln.resize(reduct_level - 1);
    for (int level = 0; level < reduct_level - 1; level++) {
      void *array_type = arr_base_type;
      size_t t0;
      vector<size_t> dim_sizes;
      int skip = level + 1;
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        int dim_size = loop_dim_sizes[t0];

        if (loop_is_reduct.find(loop_nest[t0]) != loop_is_reduct.end()) {
          if (skip == 0) {
            dim_sizes.push_back(dim_size);
          } else {
            skip--;
          }
        }
      }
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        int dim_size = loop_dim_sizes[t0];

        if (loop_is_parallel.find(loop_nest[t0]) != loop_is_parallel.end()) {
          dim_sizes.push_back(dim_size);
        }
      }
      array_type = codegen->CreateArrayType(array_type, dim_sizes);
      array_type_ln[level] = array_type;
    }

    SgScopeStatement *indexScope = insertLoop->get_scope();
    void *func_decl = codegen->GetEnclosingNode("Function", insertLoop);
    void *sg_location = insertLoop;
    void *sg_prev = codegen->GetParent(insertLoop);
    if (codegen->IsLabelStatement(sg_prev) != 0) {
      sg_location = sg_prev;
    }

    string bufname_str = "_" + codegen->UnparseToString(target_arr) + "_rdc";
    codegen->get_valid_local_name(func_decl, &bufname_str);
    SgVariableDeclaration *new_buf =
        static_cast<SgVariableDeclaration *>(codegen->CreateVariableDecl(
            arr_type, bufname_str, nullptr, indexScope));
    //        cout << "[debug2]:" << codegen->_p(new_buf) <<endl;   //  WARN:
    //        adding
    //  this line will cause crash in EDG
    codegen->InsertStmt(new_buf, sg_location);
    //    codegen->AppendChild(sg_bb, new_buf);
    new_decl.push_back(new_buf);

    if (scheme_1_enabled != 0) {
      void *sg_body = codegen->GetLoopBody(loop_nest[0]);
      assert(sg_body);
      string bufname_local = bufname_str + "_t";
      void *new_buf = codegen->CreateVariableDecl(
          array_type_local, bufname_local, nullptr, sg_body);
      if (insert_pos_local != nullptr) {
        codegen->InsertAfterStmt(new_buf, insert_pos_local);
        insert_pos_local = new_buf;
      } else {
        codegen->PrependChild(sg_body, new_buf, true);
        insert_pos_local = new_buf;
      }
      new_decl_local.push_back(new_buf);
    }
  }

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 2.2 Create the references for the reduction buffers
  //     And replace the reference in the main loop
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  //  Note: replace old reference with new reference
  //  Main Body: a[x][y] -> a_rdc[i][j][k]
  vector<void *> vec_new_refs;
  vector<void *> vec_old_refs;  //  make a copy for later use, after replacement
  SgForStatement *c_loop = isSgForStatement(cNode->ref->get_scope());
  assert(c_loop);
  for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
    SgExpression *arr_ref_c = static_cast<SgExpression *>(
        codegen->rebuild_pntr_ref(pntr_to_reduce[l]));
    //  Revised by Yuxin, Jan 23 2016, arr_ref_c will be modified during code
    //  generation
    //  Should keep a copy of it, instead of the original node
    vec_old_refs.push_back(codegen->CopyExp(arr_ref_c));
    void *new_ref = codegen->CreateVariableRef(new_decl[l]);
    string str_var_name = codegen->UnparseToString(new_ref);

    vector<void *> vec_index;
    for (size_t t0 = 0; t0 < loop_nest.size(); t0++) {
      //  for reduction buffer:
      //  1. Only skip loops if scheme_1 is applied
      //  2. Only skip internal reduction loops
      if ((scheme_1_enabled == 0) || t0 == 0 ||
          loop_is_reduct.find(loop_nest[t0]) == loop_is_reduct.end()) {
        vec_index.push_back(codegen->CreateVariableRef(
            codegen->GetLoopIterator(loop_nest[t0])));
      }
    }
    new_ref = codegen->CreateArrayRef(new_ref, vec_index);

    //  if (!scheme_1_enabled)
    //  {
    //    codegen->ReplaceExp(arr_ref, new_ref);
    //    string str_pragma = "HLS dependence variable="+str_var_name+" array
    //    inter false";
    //    void * inner_loop_body = cNode->ref;
    //    assert(codegen->IsBasicBlock(inner_loop_body));
    //    void * sg_pragma = codegen->CreatePragma(str_pragma, inner_loop_body);
    //    codegen->PrependChild(inner_loop_body, sg_pragma);
    //  }
    vec_new_refs.push_back(codegen->CopyExp(new_ref));
  }

  vector<vector<void *>> vec_new_refs_ln;
  vector<void *> vec_new_refs_local;

  if (scheme_1_enabled != 0) {
    //  new_buf_local contains the parallel dimensions
    for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
      vector<void *> index;
      size_t t0;
      for (t0 = 0; t0 < vec_parallel_loops.size(); t0++) {
        index.push_back(codegen->CreateVariableRef(
            codegen->GetLoopIterator(vec_parallel_loops[t0])));
      }

      void *array_ref = codegen->CreateVariableRef(new_decl_local[l]);
      void *new_ref_local = codegen->CreateArrayRef(array_ref, index);
      vec_new_refs_local.push_back(new_ref_local);
    }
  }

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 3 Create Initialization for reduction buffers
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 3.1 copy and create the for loops for init and post processing
  //  ////////////////////////////////////////////////////////////////////////////////
  //  / map <orig_for_loop, copied_for_loop>
  map<SgForStatement *, SgForStatement *> for_map;
  map<SgForStatement *, SgForStatement *> for_init_map;
  SgBasicBlock *init_loop_body = static_cast<SgBasicBlock *>(
      codegen->CreateBasicBlock());  //  the new loop which
  //  contains the init, main
  //  and post loops
  {
    //  Note: build assignment from new reference to old reference

    SgForStatement *copyLoop =
        SageInterface::getEnclosingNode<SgForStatement>(cNode->ref);

    //  Build the copied for-loops (two copies: init and post)
    for (int i = cNode->iterators_num; i > bNode->iterators_num - 1; i--) {
      if (scheme_1_enabled != 0) {
        int t0 =
            static_cast<int>(loop_nest.size() - 1 - (cNode->iterators_num - i));

        void *loop = loop_nest[t0];
        if (loop_is_reduct.find(loop) != loop_is_reduct.end() && t0 != 0) {
          copyLoop = SageInterface::getEnclosingNode<SgForStatement>(copyLoop);
          continue;
        }
      }
      SgStatement *init = static_cast<SgStatement *>(
          codegen->CopyStmt(copyLoop->get_init_stmt()[0]));
      ROSE_ASSERT(init);
      SgStatement *cond = static_cast<SgStatement *>(
          codegen->CopyStmt(isSgExprStatement(copyLoop->get_test())));
      ROSE_ASSERT(cond);
      SgExpression *incr = static_cast<SgExpression *>(
          codegen->CopyExp(copyLoop->get_increment()));
      ROSE_ASSERT(incr);

      SgForStatement *initLoop =
          static_cast<SgForStatement *>(codegen->CreateForLoop(
              init, cond, incr, codegen->CreateBasicBlock(), copyLoop));

      for_init_map[copyLoop] =
          isSgForStatement(static_cast<SgStatement *>(initLoop));
      copyLoop = SageInterface::getEnclosingNode<SgForStatement>(copyLoop);
    }
  }
  //  3.1.1 Copy pragmas from main loop to init loop
  {
    int l = 0;

    for (auto for_it : for_init_map) {
      SgForStatement *initLoop = for_it.second;

      //        if (for_it == for_init_map.begin()) {
      if (l == 0) {
        void *sg_location = insertLoop;
        void *sg_prev = codegen->GetParent(insertLoop);
        if (codegen->IsLabelStatement(sg_prev) != 0) {
          sg_location = sg_prev;
        }
        codegen->InsertStmt(initLoop, sg_location);
      } else {
        codegen->AppendChild_v1(initLoop, init_loop_body);
      }

      init_loop_body = isSgBasicBlock(initLoop->get_loop_body());
      for (size_t t0 = 0; t0 < vec_pragmas[l].size(); t0++) {
        codegen->AppendChild(init_loop_body,
                             codegen->CopyStmt(vec_pragmas[l][t0]));
      }
      l++;
    }
  }
  //    codegen->InsertStmt(sg_bb, for_init_map.begin()->second);

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 3.2 Build initialization statement
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  //  3.2.1 Build the loop nest for local initilization
  void *new_init_local_body = nullptr;
  {
    if (scheme_1_enabled != 0) {
      //  a.1 get init insert position
      //  a.2 create body
      void *sg_local_init = codegen->CreateBasicBlock();

      //  a.3 create loops
      void *for_loop_nest;
      void *curr_loop = nullptr;
      string str_pragma;
      str_pragma = "ACCEL PARALLEL COMPLETE";
      for (size_t t = 0; t < loop_nest.size(); t++) {
        //  only care parallel loops
        if (loop_is_parallel.find(loop_nest[t]) == loop_is_parallel.end()) {
          continue;
        }

        void *loop_t = codegen->copy_loop_for_c_syntax(loop_nest[t]);
        if (curr_loop != nullptr) {
          void *bb_t = codegen->CreateBasicBlock();
          void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
          codegen->AppendChild(bb_t, sg_pragma);
          codegen->AppendChild(bb_t, loop_t);
          codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
        } else {
          for_loop_nest = loop_t;
        }
        curr_loop = loop_t;
      }

      void *new_body = codegen->CreateBasicBlock();
      if (curr_loop != nullptr) {
        void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
        codegen->AppendChild(new_body, sg_pragma);
        codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);
        //  a.4 insert into AST
        //  inserted later
        codegen->AppendChild(sg_local_init, for_loop_nest);
      } else {
        //  Edit by Yuxin Jan 08, the nested {{...}} will triger crash in
        //  LivenessAnalysis
        sg_local_init = new_body;
      }

      assert(insert_pos_local);
      codegen->InsertAfterStmt(sg_local_init, insert_pos_local);
      new_init_local_body = new_body;
    }
  }

  //  init statement: a_rdc[i][j][k] = 0;          //  for +=
  //                 a_rdc[i][j][k] = a[..][..];  //  for reduction function
  for (size_t l = 0; l < vec_new_refs.size(); l++) {
    void *init_value_exp;
    if (isSgPlusAssignOp(curr_op) != nullptr) {
      SgStatement *init_expr = static_cast<SgStatement *>(codegen->CreateStmt(
          V_SgAssignStatement, codegen->CopyExp(vec_new_refs[l]),
          codegen->CreateConst(0)));
      codegen->AppendChild_v1(init_expr, init_loop_body);
      init_value_exp = static_cast<SgExpression *>(codegen->CreateConst(0));
    } else if (isSgFunctionCallExp(curr_op) != nullptr) {
      SgStatement *init_expr = static_cast<SgStatement *>(codegen->CreateStmt(
          V_SgAssignStatement, codegen->CopyExp(vec_new_refs[l]),
          codegen->CopyExp(vec_old_refs[l])));
      init_value_exp = codegen->CopyExp(vec_old_refs[l]);
      codegen->AppendChild_v1(init_expr, init_loop_body);
    } else {
      assert(0);
    }

    if (scheme_1_enabled != 0) {
      void *assign_stmt = codegen->CreateStmt(
          V_SgExprStatement,
          codegen->CreateExp(V_SgAssignOp,
                             codegen->CopyExp(vec_new_refs_local[l]),
                             codegen->CopyExp(init_value_exp)));
      codegen->AppendChild(new_init_local_body, assign_stmt);
    }
  }

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 4 Build main loop with replacement
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /
  {
    for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
      SgExpression *old_pntr = static_cast<SgExpression *>(pntr_to_reduce[l]);
      void *new_ref =
          (scheme_1_enabled) != 0 ? vec_new_refs_local[l] : vec_new_refs[l];
      void *new_arr_decl =
          (scheme_1_enabled) != 0 ? new_decl_local[l] : new_decl[l];
      void *array_ref = codegen->CreateVariableRef(new_arr_decl);
      string str_var_name = codegen->UnparseToString(array_ref);

      codegen->ReplaceExp(old_pntr, new_ref);

      void *inner_loop_body = cNode->ref;
      assert(codegen->IsBasicBlock(inner_loop_body));

      string str_pragma =
          "HLS dependence variable=" + str_var_name + " array inter false";
      void *sg_pragma = codegen->CreatePragma(str_pragma, inner_loop_body);
      codegen->PrependChild(inner_loop_body, sg_pragma);
    }
  }

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 5 Build post process loop(s)
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  //  Build post processing loop body: old_ref += new_ref;
  //    Note: build assignment for old reference

  assert(reduct_level >= 1);
  vector<void *> vec_postproc_bodies;
  {
    int num_post_steps = scheme_1_enabled != 0 ? 2 : reduct_level;
    for (int level = 0; level < num_post_steps; level++) {
      void *post_bb = codegen->CreateBasicBlock();
      vector<void *> vec_reduce_from;
      vector<void *> vec_reduce_to;

      if (scheme_1_enabled != 0) {
        if (level == 0) {
          vec_reduce_from = vec_new_refs_local;
          vec_reduce_to = vec_new_refs;
        } else {
          vec_reduce_from = vec_new_refs;
          vec_reduce_to = vec_old_refs;
        }
      } else {
        if (level == 0) {
          vec_reduce_from = vec_new_refs;
        } else {
          vec_reduce_from = vec_new_refs_ln[level - 1];
        }
        if (level == reduct_level - 1) {
          vec_reduce_to = vec_old_refs;
        } else {
          vec_reduce_to = vec_new_refs_ln[level];
        }
      }

      assert(vec_reduce_from.size() == vec_reduce_to.size());

      //  post process statement: a[x][y] += a_rdc[i][j][k]
      if (isSgPlusAssignOp(curr_op) != nullptr) {
        assert(vec_reduce_from.size() == 1);
        for (size_t l = 0; l < vec_reduce_from.size(); l++) {
          SgExpression *old_ref = static_cast<SgExpression *>(
              codegen->CopyExp(static_cast<SgExpression *>(vec_reduce_to[l])));
          SgStatement *assign_expr =
              static_cast<SgStatement *>(codegen->CreateStmt(
                  V_SgExprStatement,
                  codegen->CreateExp(V_SgPlusAssignOp, old_ref,
                                     codegen->CopyExp(vec_reduce_from[l]))));
          codegen->AppendChild(post_bb, assign_expr);
        }
      } else if (isSgFunctionCallExp(curr_op) != nullptr) {
        void *sg_old_call_c = codegen->CopyExp(curr_op);
        assert((size_t)codegen->GetFuncCallParamNum(sg_old_call_c) ==
               vec_reduce_from.size() * 2);
        int size = vec_new_refs.size();
        for (size_t l = 0; l < vec_reduce_from.size(); l++) {
          void *sg_param = codegen->GetFuncCallParam(sg_old_call_c, l);
          codegen->ReplaceExp(sg_param, codegen->CopyExp(vec_reduce_to[l]));
          void *sg_param_1 = codegen->GetFuncCallParam(
              sg_old_call_c, static_cast<int>(l + size));
          codegen->ReplaceExp(sg_param_1, codegen->CopyExp(vec_reduce_from[l]));
        }
        void *stmt = codegen->CreateStmt(V_SgExprStatement, sg_old_call_c);
        codegen->AppendChild(post_bb, stmt);
      } else {
        assert(0);
      }

      vec_postproc_bodies.push_back(post_bb);
    }
  }

  //  Note build post proc for loops
  vector<void *> vec_post_loops;
  {
    vector<void *> vec_loop_reordered;
    {
      size_t t0;
      for (t0 = 0; t0 < vec_reduct_loops.size(); t0++) {
        vec_loop_reordered.push_back(vec_reduct_loops[t0]);
      }
      for (t0 = 0; t0 < vec_parallel_loops.size(); t0++) {
        vec_loop_reordered.push_back(vec_parallel_loops[t0]);
      }
    }

    if (scheme_1_enabled != 0) {
      //  two level of post reduction
      //  a.1 reduce from a_rdc_t -> a_rdc; only parallel loops
      //  a.2 reduce from a_rdc -> old_ref; pipeline + parallel loops
      for (int level = 0; level < 2; level++) {
        void *for_loop_nest =
            (level == 0) ? nullptr
                         : codegen->copy_loop_for_c_syntax(loop_nest[0]);

        void *curr_loop = for_loop_nest;
        string str_pragma = "ACCEL PIPELINE";
        bool parallel_tag = false;
        for (size_t t = level; t < loop_nest.size(); t++) {
          //  only care parallel loops
          if (loop_is_parallel.find(loop_nest[t]) == loop_is_parallel.end()) {
            continue;
          }
          parallel_tag = true;
          void *loop_t = codegen->copy_loop_for_c_syntax(loop_nest[t]);
          if (curr_loop != nullptr) {
            void *bb_t = codegen->CreateBasicBlock();
            void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
            codegen->AppendChild(bb_t, sg_pragma);
            codegen->AppendChild(bb_t, loop_t);
            codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
          } else {
            for_loop_nest = loop_t;
          }
          curr_loop = loop_t;
        }
        void *new_body = codegen->CreateBasicBlock();
        //  Yuxin edit for bug 161 & 181

        if ((curr_loop != nullptr) && parallel_tag) {
          string str_pragma = "ACCEL PARALLEL COMPLETE";
          void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
          codegen->AppendChild(new_body, sg_pragma);
        } else if ((curr_loop != nullptr) && !parallel_tag) {
          string str_pragma = "ACCEL PIPELINE";
          void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
          codegen->AppendChild(new_body, sg_pragma);
        }

        if (level == 0) {
          for (size_t l = 0; l < new_decl.size(); l++) {
            string str_var_name = codegen->GetVariableName(
                codegen->GetVariableInitializedName(new_decl[l]));
            string str_pragma = "HLS dependence variable=" + str_var_name +
                                " array inter false";
            void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
            codegen->AppendChild(new_body, sg_pragma);
          }
        }
        codegen->AppendChild(new_body, vec_postproc_bodies[level]);
        if (curr_loop != nullptr) {
          codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);
        } else {
          for_loop_nest = new_body;  //  no loop, fake the body as the loop
        }

        if (level == 0) {
          codegen->AppendChild(codegen->GetLoopBody(pipeline_loop),
                               for_loop_nest);
        } else {
          vec_post_loops.push_back(for_loop_nest);
        }
      }
    } else {
      //  the last level reduction keeps the original loop order
      //  all the high-level reduction will use the optimized order (reduction
      //  loop outer)
      int level = 0;  //  used after the for loop
      for (level = 0; level < reduct_level - 1; level++) {
#if USED_CODE_IN_COVERAGE_TEST
        //  loop interchange: reduction outer and parallel inner
        void *for_loop_nest =
            codegen->copy_loop_for_c_syntax(vec_loop_reordered[level]);
        void *curr_loop = for_loop_nest;
        string str_pragma = "ACCEL PIPELINE";
        for (size_t t = level + 1; t < vec_loop_reordered.size(); t++) {
          void *loop_t = codegen->copy_loop_for_c_syntax(vec_loop_reordered[t]);
          void *bb_t = codegen->CreateBasicBlock();
          void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
          codegen->AppendChild(bb_t, sg_pragma);
          codegen->AppendChild(bb_t, loop_t);
          codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
          curr_loop = loop_t;
          str_pragma = "ACCEL PARALLEL COMPLETE";
        }
        void *new_body = codegen->CreateBasicBlock();
        void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
        codegen->AppendChild(new_body, sg_pragma);
        codegen->AppendChild(new_body,
                             codegen->CopyStmt(vec_postproc_bodies[level]));
        codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);

        vec_post_loops.push_back(for_loop_nest);
#endif
      }
      //  Last level reduction:
      {
        //  only the last reduction level is in the loop nest
        void *last_reduct_loop = vec_reduct_loops[vec_reduct_loops.size() - 1];

        void *for_loop_nest = nullptr;
        void *curr_loop = nullptr;
        string str_pragma = "ACCEL PIPELINE";
        for (size_t t = 0; t < loop_nest.size(); t++) {
          void *one_loop = loop_nest[t];
          if (one_loop == last_reduct_loop ||
              loop_is_parallel.find(one_loop) != loop_is_parallel.end()) {
            void *loop_t = codegen->copy_loop_for_c_syntax(one_loop);
            if (curr_loop == nullptr) {
              curr_loop = loop_t;
              for_loop_nest = curr_loop;
            } else {
              void *bb_t = codegen->CreateBasicBlock();
              void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
              codegen->AppendChild(bb_t, sg_pragma);
              codegen->AppendChild(bb_t, loop_t);
              codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
              curr_loop = loop_t;
              str_pragma = "ACCEL PARALLEL COMPLETE";
            }
          }
        }

        void *new_body = codegen->CreateBasicBlock();
        void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
        codegen->AppendChild(new_body, sg_pragma);
        codegen->AppendChild(new_body,
                             codegen->CopyStmt(vec_postproc_bodies[level]));
        codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);

        vec_post_loops.push_back(for_loop_nest);
      }
    }
  }

  //         codegen->AppendChild(sg_bb,
  //  (SgStatement*)codegen->CopyStmt(for_init_map.begin()->second));
  //        SgLabelStatement *sg_label_0 =
  //            isSgLabelStatement((SgNode
  //  *)codegen->GetPreviousStmt(insertLoop));         if (sg_label_0) {
  //            codegen->AppendChild(sg_bb,
  //  (SgStatement*)codegen->CopyStmt(sg_label_0));
  //        }
  //        codegen->AppendChild(sg_bb,
  //  (SgStatement*)codegen->CopyStmt(insertLoop));

  for (size_t t = 0; t < vec_post_loops.size(); t++) {
    codegen->InsertAfterStmt(vec_post_loops[t], insertLoop);
    vector<void *> vec_assign;
    codegen->GetNodesByType(vec_post_loops[t], "preorder", "SgPlusAssignOp",
                            &vec_assign);
    for (auto it : vec_assign) {
      void *arr_pntr_ref =
          isSgPlusAssignOp(static_cast<SgNode *>(it))->get_lhs_operand();
      rdc_buf->insert(arr_pntr_ref);
    }
    //    codegen->AppendChild(sg_bb, vec_post_loops[t]);
  }
  /*
          size_t stmt_idx = -1;
          if (codegen->GetEnclosingNode("ForLoop",
     for_init_map.begin()->second)) { void *sg_loop =
     codegen->GetEnclosingNode("ForLoop", for_init_map.begin()->second);
     stmt_idx = codegen->GetChildStmtIdx(sg_loop, for_init_map.begin()->second);
          }
              if (scheme_1_enabled && !stmt_idx) {
                  SgStatementPtrList &stmt_lst =
                  isSgBasicBlock(static_cast<SgNode
     *>(sg_bb)->getStatementList()); for (int i = 0; i < stmt_lst.size(); i++) {
                  codegen->InsertStmt(stmt_lst[i],
     for_init_map.begin()->second);
          //  insertStatementBefore(stmt_lst[i], for_init_map.begin()->second);
          }
          } else
          codegen->InsertStmt(sg_bb, for_init_map.begin()->second);*/

#if 0
        codegen->RemoveStmt(for_init_map.begin()->second);
        if (sg_label_0) {
            cout << "Remove label: " << codegen->_p(sg_label_0) << endl;
            codegen->RemoveStmt(sg_label_0);
        }
        codegen->RemoveStmt(insertLoop);
#endif
  cout << "parent: " << codegen->_p(parent_loop) << endl;
  //    codegen->RemoveStmt(for_map.begin()->second);

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 6 Handling reduction functions
  //  ////////////////////////////////////////////////////////////////////////////////
  //  / ZP: 20151129 add HLS inline pragma, otherwise there is problem on
  //  parallel
  {
    if (codegen->IsFunctionCall(curr_op) != 0) {
      void *func_decl = codegen->GetFuncDeclByCall(curr_op);
      void *func_body = codegen->GetFuncBody(func_decl);
      if (func_body != nullptr) {
        void *pragma = codegen->CreatePragma("HLS inline", func_body);
        codegen->PrependChild(func_body, pragma);
      }
    }
  }
  return true;
}

int get_ref_num(CSageCodeGen *codegen, void *sg_bb, void *arr_init) {
  int ref_num = 0;
  vector<void *> vec_tmp;
  codegen->GetNodesByType_compatible(sg_bb, "SgVarRef", &vec_tmp);
  for (size_t i = 0; i < vec_tmp.size(); i++) {
    void *ref = vec_tmp[i];
    void *target_init = codegen->GetVariableInitializedName(ref);
    if (target_init == arr_init) {
      ref_num++;
    }
  }
  return ref_num;
}

void reportNoBenefit(CSageCodeGen *codegen, void *curr_op) {
  string str_ref = codegen->UnparseToString(curr_op);
  if (str_ref.length() > 20) {
    str_ref = str_ref.substr(0, 20) + " ...";
  }
  string op_info =
      "'" + str_ref + "' " + getUserCodeFileLocation(codegen, curr_op, true);
  dump_critical_message(REDUC_WARNING_2(op_info));
}

void reportUnCanonicalLoop(CSageCodeGen *codegen, void *loop) {
  string str_ref = codegen->UnparseToString(loop);
  if (str_ref.length() > 20) {
    str_ref = str_ref.substr(0, 20) + " ...";
  }
#if 0
        string msg = "Stoping reduction analysis in loop '" + str_ref + "' (" +
            sFile + ":" + my_itoa(nVarLine) +
            ") because it is not canonical loop.\n";
        cout << msg << endl;
#endif
  string op_info =
      "'" + str_ref + "' " + getUserCodeFileLocation(codegen, loop, true);
  dump_critical_message(REDUC_WARNING_3(op_info));
}

void remove_reduction_pragma(CSageCodeGen *codegen, void *pTopFunc,
                             set<void *> rdc_buf) {
  //  parsing reduction pragma to remove them
  Rose_STL_Container<SgNode *> nodeList = NodeQuery::querySubTree(
      static_cast<SgProject *>(pTopFunc), V_SgPragmaDeclaration);

  Rose_STL_Container<SgNode *>::iterator nodeListIterator = nodeList.begin();
  for (nodeListIterator = nodeList.begin(); nodeListIterator != nodeList.end();
       ++nodeListIterator) {
    SgPragmaDeclaration *decl = isSgPragmaDeclaration(*nodeListIterator);
    ROSE_ASSERT(decl);
    CAnalPragma ana_pragma(codegen);
    bool errorOut;
    if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut)) {
      if (ana_pragma.is_reduction()) {
        string var_str = ana_pragma.get_attribute(CMOST_variable);
        void *target_var =
            codegen->getInitializedNameFromName(decl, var_str, true);
        if (target_var == nullptr || var_str == "") {
          codegen->RemoveStmt(decl);
        } else if ((target_var != nullptr) &&
                   rdc_buf.find(target_var) != rdc_buf.end()) {
          //  Applied reduction for target variable
          codegen->RemoveStmt(decl);
        }
      }
    }
  }
}

void remove_labels(CSageCodeGen *codegen, void *pTopFunc) {
  //  Comment all the label statements before for loops
  Rose_STL_Container<SgNode *> nodeList = NodeQuery::querySubTree(
      static_cast<SgProject *>(pTopFunc), V_SgForStatement);

  Rose_STL_Container<SgNode *>::iterator nodeListIterator = nodeList.begin();

  for (nodeListIterator = nodeList.begin(); nodeListIterator != nodeList.end();
       ++nodeListIterator) {
    SgForStatement *sg_loop = isSgForStatement(*nodeListIterator);
    if (sg_loop != nullptr) {
      void *sg_stmt = codegen->GetParent(sg_loop);
      if (codegen->IsLabelStatement(sg_stmt) != 0) {
        string label_str = codegen->UnparseToString(sg_stmt);
        codegen->AddComment("Original label: " + label_str, sg_stmt);
        codegen->RemoveLabel(sg_stmt);
      }
    }
  }
}
//  Add by Yuxin to check the form like for(i=jj;i....)
bool check_loop_initializer(CSageCodeGen *codegen, void *sg_scope,
                            int lift_level) {
  SgForStatement *insertLoop = static_cast<SgForStatement *>(sg_scope);
  SgForStatement *lift_for = static_cast<SgForStatement *>(
      codegen->GetEnclosingNode("ForLoop", sg_scope));

  for (int i = 0; i < lift_level; i++) {
    SgInitializedName *lift_iter = isSgInitializedName(
        static_cast<SgNode *>(codegen->GetLoopIterator(lift_for)));
    if (lift_iter == nullptr) {
      reportUnCanonicalLoop(codegen, lift_for);
      return false;
    }

    SgStatement *init_stmt = insertLoop->get_init_stmt()[0];
    SgExpression *init_exp = nullptr;
    if (isSgExprStatement(init_stmt) != nullptr) {
      SgBinaryOp *init_assign_op =
          isSgBinaryOp(isSgExprStatement(init_stmt)->get_expression());
      if (init_assign_op == nullptr) {
        return false;
      }
      init_exp = isSgExpression(init_assign_op->get_rhs_operand_i());
    } else if (isSgVariableDeclaration(init_stmt) != nullptr) {
      SgVariableDeclaration *init_decl_stmt =
          isSgVariableDeclaration(init_stmt);
      SgInitializedName *init_name =
          isSgInitializedName(init_decl_stmt->get_variables()[0]);
      if (init_name == nullptr) {
        return false;
      }
      SgAssignInitializer *init_initializer =
          isSgAssignInitializer(init_name->get_initializer());
      if (init_initializer == nullptr) {
        return false;
      }
      init_exp = isSgExpression(init_initializer->get_operand());
    }
    if (init_exp == nullptr) {
      return false;
    }

    Rose_STL_Container<SgNode *> varList =
        NodeQuery::querySubTree(init_exp, V_SgVarRefExp);
    for (Rose_STL_Container<SgNode *>::iterator var = varList.begin();
         var != varList.end(); var++) {
      SgExpression *ref = isSgExpression(*var);
      if (ref == nullptr) {
        continue;
      }
      SgInitializedName *arr_var = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(ref));

      if (arr_var == lift_iter) {
        cout << "The loop's lower bound is related to the outter loop "
                "iterators.Quit reduction optimization."
             << endl;
        return false;
      }
    }

    SgStatement *cond_exp = isSgExprStatement(insertLoop->get_test());

    Rose_STL_Container<SgNode *> varList_c =
        NodeQuery::querySubTree(cond_exp, V_SgVarRefExp);
    for (Rose_STL_Container<SgNode *>::iterator var = varList_c.begin();
         var != varList_c.end(); var++) {
      SgExpression *ref = isSgExpression(*var);
      if (ref == nullptr) {
        continue;
      }
      SgInitializedName *arr_var = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(ref));
      if (arr_var == lift_iter) {
        cout << "The loop's upper bound is related to the outter loop "
                "iterators.Quit reduction optimization."
             << endl;
        return false;
      }
    }

    lift_for = SageInterface::getEnclosingNode<SgForStatement>(lift_for);
  }
  return true;
}

void reportLoopLowerBound(CSageCodeGen *codegen, void *loop) {
  string str_ref = codegen->UnparseToString(loop);
  if (str_ref.length() > 20) {
    str_ref = str_ref.substr(0, 20) + " ...";
  }
#if 0
        string msg = "Stoping reduction analysis in loop '" + str_ref + "' (" +
            sFile + ":" + my_itoa(nVarLine) + "):\nbecause it uses the "
            "outer loop's iterator in "
            "loop bound expression.\n";
        cout << msg << endl;
#endif
  string op_info =
      "'" + str_ref + "' " + getUserCodeFileLocation(codegen, loop, true);
  dump_critical_message(REDUC_WARNING_4(op_info));
}
