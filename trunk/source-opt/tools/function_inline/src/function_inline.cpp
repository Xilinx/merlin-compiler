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
#include "function_inline.h"
#include <iostream>
#include <string>
#include <unordered_set>
#include <boost/algorithm/string.hpp>

#include "math.h"
#include "codegen.h"
#include "ir_types.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "postwrap_process.h"
#include "rose.h"
//  #include "inliner.h"
// using namespace std;
//  using namespace SageInterface;
//  using namespace SageBuilder;
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);

map<string, int> used_func;
static int count_return = 0;

//  map<pair<string, string>, void*> global_var;

int is_host_function(CSageCodeGen *codegen, void *func) {
  //  check whether there is task pragma in the function body
  void *func_body = codegen->GetFuncBody(func);
  //  remove unused function declaration without body
  if (func_body == nullptr) {
    return 0;
  }
  vector<void *> vec_pragmas;
  codegen->GetNodesByType(func_body, "preorder", "SgPragmaDeclaration",
                          &vec_pragmas);
  for (auto pragma : vec_pragmas) {
    SgPragmaDeclaration *decl =
        isSgPragmaDeclaration(static_cast<SgNode *>(pragma));
    assert(decl);
    CAnalPragma ana_pragma(codegen);
    bool invalid;
    if (ana_pragma.PragmasFrontendProcessing(decl, &invalid, false)) {
      if (ana_pragma.is_task()) {
        return 1;
      }
    }
  }
  return 0;
}

int func_inline(CSageCodeGen *codegen, void *func_call, enum effort curr_flow,
                bool transform, bool report) {
  //  1.get function body
  void *func_call_stmt = codegen->TraceUpToStatement(func_call);
  string func_name = codegen->GetFuncNameByCall(func_call);
  if (codegen->IsMerlinInternalIntrinsic(func_name)) {
    return 1;
  }
  //  void * func_call_stmt = codegen->TraceUpToStatement(func_call);
  void *call_parent = codegen->GetParent(func_call);

  void *func_decl = codegen->GetFuncDeclByCall(func_call);
  if (func_decl == nullptr) {
    return 0;
  }
  void *func_body_old = codegen->GetFuncBody(func_decl);
  if (transform) {
    //  inline sub functions
    vector<void *> sub_func_calls;
    codegen->GetNodesByType(func_body_old, "preorder", "SgFunctionCallExp",
                            &sub_func_calls);
    int ret = 0;
    for (size_t i = sub_func_calls.size(); i > 0; --i) {
      ret |= func_inline(codegen, sub_func_calls[i - 1], curr_flow, transform,
                         report);
    }
  }
  //  youxiang 20161027
  func_body_old = codegen->GetFuncBody(func_decl);
  void *caller_decl = codegen->TraceUpToFuncDecl(func_call);

  //    vector<void *> sub_functions;
  //    codegen->GetNodesByType(func_body_old, "preorder",  "SgFunctionCallExp",
  //    &sub_functions); for (size_t i=0; i < sub_functions.size(); i++) {
  //        string sub_func_name = codegen->GetFuncNameByCall(sub_functions[i]);
  //        if(sub_func_name == func_name) {
  //            string msg = "Do not support recursive function : " + func_name;
  //            dump_critical_message("FUCIN", "ERROR", msg, 302);
  //            throw std::exception();
  //        }
  //    }

  void *return_type = codegen->GetFuncReturnType(func_decl);

  void *func_call_scope = codegen->GetScope(func_call);

  void *return_var_decl = nullptr;
  if (codegen->IsStatement(call_parent) == 0) {
    //  ZP: 20160703
    if ("void" == codegen->UnparseToString(return_type)) {
      if (report) {
        string sFileName = codegen->get_file(func_call);
        string sFileLine = my_itoa(codegen->get_line(func_call));

        string call_info =
            "    Call: " + codegen->_up(func_call) + " at " + sFileName + "\n";
        call_info += "    Decl: " + codegen->_up(func_decl) + " at " +
                     codegen->get_file(func_decl) + "\n";
        string msg =
            "Cannot support a void function called as an expression \n" +
            call_info;
        if (curr_flow <= LOW) {
          dump_critical_message(FUCIN_WARNING_1(call_info, false));
        } else {
          msg +=
              ". The design flow will switch to low effort mode automatically";
          dump_critical_message(FUCIN_WARNING_1(call_info, true));
        }
      }
      return 0;
    }
  }
  vector<void *> return_stmts;
  vector<void *> all_stmts;

  codegen->GetNodesByType_compatible(func_body_old, "SgStatement", &all_stmts);
  int all_size = all_stmts.size();
  void *last_stmt = all_stmts[all_size - 1];
  //  printf("last stmt = %s\n", codegen->UnparseToString(last_stmt).c_str());
  codegen->GetNodesByType(func_body_old, "preorder", "SgReturnStmt",
                          &return_stmts);

  for (auto &ret_stmt : return_stmts) {
    if (last_stmt != ret_stmt) {
      if (report) {
        string return_string = codegen->UnparseToString(ret_stmt);
        string function_string = codegen->UnparseToString(func_decl);
        string call_info = "    Function: " + codegen->_up(func_decl) + " at " +
                           codegen->get_file(func_decl) + "\n";
        call_info += "    Statement:" + codegen->_up(ret_stmt) + " at " +
                     codegen->get_file(ret_stmt) + "\n";
        string msg = "Cannot inline a function with \"return\" in the middle "
                     "of function body \n" +
                     call_info;
        if (curr_flow <= LOW) {
          dump_critical_message(FUCIN_WARNING_2(call_info, false));
        } else {
#if 0
          dump_critical_message("FUCIN", "ERROR", msg, 301);
          throw std::exception();
#else
          msg +=
              ". The design flow will switch to low effort mode automatically";
          dump_critical_message(FUCIN_WARNING_2(call_info, true));
        }
#endif
        }
        return 0;
      }
    }

    //  youxiang 20160929 check goto statement
    vector<void *> goto_stmts;
    codegen->GetNodesByType(func_body_old, "preorder", "SgGotoStatement",
                            &goto_stmts);
    if (!goto_stmts.empty()) {
      if (report) {
        void *goto_stmt = goto_stmts[0];
        string goto_string = codegen->UnparseToString(goto_stmt);
        string function_string = codegen->UnparseToString(func_decl);
        string call_info = "    Function: " + codegen->_up(func_decl) + " at " +
                           codegen->get_file(func_decl) + "\n";
        call_info += "    Statement:" + codegen->_up(goto_stmt) + " at " +
                     codegen->get_file(goto_stmt) + "\n";
        string msg =
            "Cannot inline a function with \"goto\" statement \n" + call_info;
        if (curr_flow <= LOW) {
          dump_critical_message(FUCIN_WARNING_3(call_info, false));
        } else {
          msg +=
              ". The design flow will switch to low effort mode automatically";
          dump_critical_message(FUCIN_WARNING_3(call_info, true));
        }
      }
      return 0;
    }

    if (!transform) {
      return 1;
    }
#if 0
    //  //////////////////////////////  /
    //  ZP: 20160920
    //  2.2 including global constant variable
    //  youxiang: 20161125
    //  do not check global variable which will be converted into arguments
    //  later
    {
      CSageCodeGen *ast = &codegen;

      vector<void *> v_ref;
      ast->GetNodesByType(func_body_old, "preorder", "SgVarRefExp", &v_ref);
      void *curr_kernel = ast->TraceUpToFuncDecl(func_call);
      for (auto ref : v_ref) {
        void *sg_name = ast->GetVariableInitializedName(ref);
        if (ast->IsGlobalInitName(sg_name)) {
          void *type = ast->GetTypebyVar(sg_name);
          void *decl = ast->GetVariableDecl(sg_name);
          //  youxiang 20160923 do not touch compiler internal global variable,
          //  such as
          //  __PRETTY_FUNCTION__
          //
          if (decl && !ast->IsFromInputFile(decl))
            continue;
          if (decl && ast->is_in_same_file(decl, curr_kernel))
            continue;
          string s_var = ast->_up(sg_name);
          string s_file = ast->get_file(curr_kernel);
          if (!ast->IsConstType(type)) {
            if (report) {
              s_var = (decl) ? ast->_up(decl) : ast->_up(sg_name);
              string msg =
                  "Do not support non-constant global variable in kernel: \n";
              msg += "    Global variable: " + s_var + "\n";
              msg += "    Kernel function: " + ast->_up(func_decl) + "\n";
              msg += "    File location:   " +
                     ast->GetFileInfo_filename(func_decl, 1) + "\n";
              dump_critical_message("FUCIN", "ERROR", msg, 304);
#if PROJDEBUG
              cout << endl;
              cout << "ERROR: " << msg << endl;
              cout << endl;
#endif
            }
            return 0;
          }
#if 0
        //  move the constant to local
        if (decl) {
          void * curr_kernel_body = ast->GetFuncBody(curr_kernel);

          if (global_var.find(s_var) == global_var.end()) {
            //  ////////////////////////////  /
            //  NOTE: does not work, because the newly added variable
            //  can not be found
            //  void * find = ast->find_variable_by_name(s_var,
            //  curr_kernel_body);
            //  if (!ast->is_in_same_file(find, curr_kernel_body))
            {
              void * decl_cp = ast->CopyVarDeclStmt(decl, curr_kernel_body);
              ast->PrependChild(curr_kernel_body, decl_cp);
              global_var[s_var] = ast->GetVariableInitializedName(decl_cp);
            }
          }

          void * new_var = global_var[s_var];
          ast->replace_variable_references_in_scope(sg_name, new_var,
              func_body);
        }
#else
        //  youxiang 20161005
        //  forward declaration in currrent file
        if (decl && transform) {
          void *curr_scope = ast->GetGlobal(curr_kernel);

          //  if (global_var.find(make_pair(s_file, s_var)) == global_var.end())
          {
            {
              void *extern_decl = ast->CreateVariableDecl(
                  ast->GetTypebyVar(sg_name), s_var, nullptr, curr_scope);
              ast->InsertStmt(extern_decl, curr_kernel);
              ast->SetExtern(extern_decl);
              //  global_var[make_pair(s_file, s_var)] =
              //  ast->GetVariableInitializedName(extern_decl);
            }
          }

          //  void * new_var = global_var[make_pair(s_file, s_var)];
          //  ast->replace_variable_references_in_scope(sg_name, new_var,
          //  func_body);
        }

#endif
        }
      }
    }
#endif

    if ("void" != codegen->UnparseToString(return_type)) {
      string return_var_name =
          "return_" + my_itoa(count_return) + "_" + func_name;
      count_return++;
      codegen->get_valid_local_name(caller_decl, &return_var_name);
      return_var_decl = codegen->CreateVariableDecl(
          return_type, return_var_name, nullptr, func_call_scope);
      codegen->InsertStmt(return_var_decl, func_call_stmt);
    }

    void *func_body = codegen->CopyStmt(func_body_old);

    codegen->GetNodesByType(func_body, "preorder", "SgReturnStmt",
                            &return_stmts);
    //  1 update return statement
    for (auto &ret_stmt : return_stmts) {
      if (codegen->IsDummyReturnStmt(ret_stmt) != 0) {
        codegen->RemoveStmt(ret_stmt);
        continue;
      }
      void *return_exp = codegen->GetReturnExp(ret_stmt);
      void *return_ref = codegen->CreateVariableRef(return_var_decl);
      void *new_stmt =
          codegen->CreateStmt(V_SgAssignStatement, return_ref, return_exp);
      codegen->ReplaceStmt(ret_stmt, new_stmt);
    }
    //  2.replace the other variable name with a new one
    vector<void *> all_init_name;
    codegen->GetNodesByType(func_body, "preorder", "SgInitializedName",
                            &all_init_name);
    for (auto &var_init : all_init_name) {
      void *variable_decl = codegen->GetVariableDecl(var_init);
      void *variable_type = codegen->GetTypebyVar(var_init);
      string variable_name = codegen->UnparseToString(var_init);
      string new_variable_name = variable_name + "__" + func_name;
      codegen->get_valid_local_name(caller_decl, &new_variable_name);
      void *initializer = codegen->GetInitializer(var_init);
      void *new_variable = codegen->CreateVariableDecl(
          variable_type, new_variable_name,
          initializer != nullptr ? codegen->CopyExp(initializer) : nullptr,
          func_body);
      if (codegen->is_register(variable_decl) != 0) {
        codegen->set_register(new_variable);
      }
      vector<void *> all_ref;
      all_ref = codegen->GetAllRefInScope(var_init, func_body);
      for (size_t j = 0; j < all_ref.size(); j++) {
        void *new_variable_ref = codegen->CreateVariableRef(new_variable);
        codegen->ReplaceExp(all_ref[j], new_variable_ref);
      }
      if (codegen->IsVariableDecl(variable_decl) != 0) {
        codegen->ReplaceStmt(variable_decl, new_variable);
      }
      if (codegen->IsConstType(variable_type) != 0) {
        codegen->SetVariableName(new_variable, variable_name);
      }
    }

    //  ZP: 21060704
    //  2.1 remove Label statement
    vector<void *> label_stmts;
    codegen->GetNodesByType(func_body, "preorder", "SgLabelStatement",
                            &label_stmts);
    for (auto &label_stmt : label_stmts) {
      codegen->RemoveLabel(label_stmt);
    }

    //  Youxiang: 20170412
    //  2.2 remove function declaration statement
    vector<void *> func_stmts;
    codegen->GetNodesByType(func_body, "preorder", "SgFunctionDeclaration",
                            &func_stmts);
    for (auto func_stmt : func_stmts) {
      codegen->RemoveStmt(func_stmt);
    }

    //  //////////////////////////////  /

    //  3.extract func argument, replace these argument in function body with a
    //  new name
    int func_call_param_num = codegen->GetFuncCallParamNum(func_call);
    int func_decl_param_num = codegen->GetFuncParamNum(func_decl);
    assert(func_call_param_num == func_decl_param_num);
    for (int i = 0; i < func_decl_param_num; i++) {
      void *func_decl_param = codegen->GetFuncParam(func_decl, i);
      void *func_call_param = codegen->GetFuncCallParam(func_call, i);
      void *param_type = codegen->GetTypebyVar(func_decl_param);
      string type_0 = codegen->GetStringByType(param_type);
      //  printf("param type=%s\n",type_0.c_str());
      vector<void *> all_ref;
      all_ref = codegen->GetAllRefInScope(func_decl_param, func_body);
      //  printf("reference number=%d\n",all_ref.size());
      void *new_variable = nullptr;
      if ((codegen->IsArrayType(param_type) != 0) ||
          (codegen->IsPointerType(param_type) != 0) ||
          (codegen->IsReferenceType(param_type) != 0)) {
        //  printf("copy expression =
        //  %s\n",codegen->UnparseToString(new_variable_ref).c_str());

        //  //////////////////////////////////////  /
        //  ZP: 20160920 : fix lvalue problem
        CSageCodeGen *ast = codegen;
        if (ast->IsPointerType(param_type) != 0) {
          //  youxiang: 20160930 : bug672
          //  void * arg_type = ast->GetTypeByExp(func_call_param);
          //  if (ast->IsArrayType(arg_type))
          {
            vector<void *> tmp;
            if (ast->has_lvalue_ref_in_scope(func_decl, func_decl_param,
                                             &tmp) != 0) {
              string variable_name = codegen->UnparseToString(func_decl_param);
              string new_variable_name = variable_name + "__" + func_name;
              codegen->get_valid_local_name(caller_decl, &new_variable_name);
              new_variable = codegen->CreateVariableDecl(
                  param_type, new_variable_name, nullptr, func_body);
              void *new_variable_ref = codegen->CreateVariableRef(new_variable);
              void *init_stmt =
                  codegen->CreateStmt(V_SgAssignStatement, new_variable_ref,
                                      codegen->CopyExp(func_call_param));
              codegen->PrependChild(func_body, init_stmt);
              codegen->PrependChild(func_body, new_variable);
            }
          }
        }
        //  //////////////////////////////////////  /

      } else if (codegen->IsScalarType(param_type) != 0) {
        string variable_name = codegen->UnparseToString(func_decl_param);
        //  printf("param name=%s\n",variable_name.c_str());
        string new_variable_name = variable_name + "__" + func_name;
        //  printf("new param name=%s\n",new_variable_name.c_str());
        codegen->get_valid_local_name(caller_decl, &new_variable_name);
        new_variable = codegen->CreateVariableDecl(
            param_type, new_variable_name, codegen->CopyExp(func_call_param),
            func_body);
        codegen->PrependChild(func_body, new_variable);
      } else {
        assert(0 && "unsupported type");
      }
      for (size_t j = 0; j < all_ref.size(); j++) {
        codegen->ReplaceExp(all_ref[j],
                            new_variable != nullptr
                                ? codegen->CreateVariableRef(new_variable)
                                : codegen->CopyExp(func_call_param));
      }
    }

    //  4. if use function call in assignment, replace function call with the
    //  new assignment
    //  youxiang 20161005 function call as condition expr in if-statement
    //  if(codegen->IsExprStatement(call_parent))
    if (return_var_decl == nullptr) {
      void *basic_block = codegen->TraceUpToBasicBlock(func_call_stmt);
      if ((basic_block != nullptr) &&
          codegen->GetChildStmtNum(basic_block) == 1 &&
          codegen->GetChildStmt(basic_block, 0) == func_call_stmt) {
        codegen->ReplaceStmt(basic_block, func_body);
      } else {
        codegen->InsertStmt(func_body, func_call_stmt);
        codegen->RemoveStmt(func_call_stmt);
      }
    } else {
      codegen->InsertStmt(func_body, func_call_stmt);
      void *return_ref = codegen->CreateVariableRef(return_var_decl);
      codegen->ReplaceExp(func_call, return_ref);
    }
    codegen->clean_empty_aggregate_initializer(func_body);
    return 1;
  }

  int process_function_call_in_loop_statement(
      CSageCodeGen * codegen, void *func, set<void *> &visited) {
    if (visited.count(func) > 0) {
      return 0;
    }
    visited.insert(func);
    void *func_body = codegen->GetFuncBody(func);
    if (func_body == nullptr) {
      return 0;
    }
    int ret = 0;
    vector<void *> sub_calls;
    codegen->GetNodesByType(func_body, "preorder", "SgFunctionCallExp",
                            &sub_calls);
    for (auto call : sub_calls) {
      void *callee = codegen->GetFuncDeclByCall(call);
      if (callee == nullptr) {
        continue;
      }
      ret |= process_function_call_in_loop_statement(codegen, callee, visited);
    }

    //  Youxiang: 20170321 add missing brace
    codegen->add_missing_brace(func_body);

    //  for-statement
    //  for (; test; incr) { body;}
    //  =>
    //  for (; true; ) { if (!test) break; body; incr;}
    vector<void *> vec_for_stmt;
    codegen->GetNodesByType(func_body, "preorder", "SgForStatement",
                            &vec_for_stmt);
    for (auto for_stmt : vec_for_stmt) {
      void *test_stmt = codegen->GetLoopTest(for_stmt);
      void *incr_expr = codegen->GetLoopIncrementExpr(for_stmt);
      void *loop_body = codegen->GetLoopBody(for_stmt);
      vector<void *> vec_func_call;
      codegen->GetNodesByType(test_stmt, "preorder", "SgFunctionCallExp",
                              &vec_func_call);
      if (static_cast<unsigned int>(!vec_func_call.empty()) != 0U) {
        void *true_body = codegen->CreateBasicBlock();
        void *break_stmt = codegen->CreateStmt(V_SgBreakStmt);
        codegen->PrependChild(true_body, break_stmt);
        void *test_expr = codegen->GetExprFromStmt(test_stmt);
        void *not_test_expr =
            codegen->CreateExp(V_SgNotOp, codegen->CopyExp(test_expr));
        void *if_stmt =
            codegen->CreateIfStmt(not_test_expr, true_body, nullptr);
        codegen->PrependChild(loop_body, if_stmt);
        codegen->ReplaceExp(test_expr, codegen->CreateConst(1));
        ret = 1;
      }
      codegen->GetNodesByType(incr_expr, "preorder", "SgFunctionCallExp",
                              &vec_func_call);
      if (static_cast<unsigned int>(!vec_func_call.empty()) != 0U) {
        void *new_incr_stmt =
            codegen->CreateStmt(V_SgExprStatement, codegen->CopyExp(incr_expr));
        codegen->AppendChild(loop_body, new_incr_stmt);
        codegen->ReplaceExp(incr_expr, codegen->CreateExp(V_SgNullExpression));
        ret = 1;
      }
    }
    //  do-while
    //  do {body;} while (e);
    //  =>
    //  do { body; if (!e) break; } while (1);
    vector<void *> vec_dowhile_stmt;
    codegen->GetNodesByType(func_body, "preorder", "SgDoWhileStmt",
                            &vec_dowhile_stmt);
    for (auto dowhile_stmt : vec_dowhile_stmt) {
      void *test_stmt = codegen->GetLoopTest(dowhile_stmt);
      void *loop_body = codegen->GetLoopBody(dowhile_stmt);
      vector<void *> vec_func_call;
      codegen->GetNodesByType(test_stmt, "preorder", "SgFunctionCallExp",
                              &vec_func_call);
      if (static_cast<unsigned int>(!vec_func_call.empty()) != 0U) {
        void *true_body = codegen->CreateBasicBlock();
        void *break_stmt = codegen->CreateStmt(V_SgBreakStmt);
        codegen->PrependChild(true_body, break_stmt);
        void *test_expr = codegen->GetExprFromStmt(test_stmt);
        void *not_test_expr =
            codegen->CreateExp(V_SgNotOp, codegen->CopyExp(test_expr));
        void *if_stmt =
            codegen->CreateIfStmt(not_test_expr, true_body, nullptr);
        codegen->AppendChild(loop_body, if_stmt);
        codegen->ReplaceExp(test_expr, codegen->CreateConst(1));
        ret = 1;
      }
    }
    //  while
    //  while (e) { body; }
    //  while (1) { if (!e) break; body; }
    vector<void *> vec_while_stmt;
    codegen->GetNodesByType(func_body, "preorder", "SgWhileStmt",
                            &vec_while_stmt);
    for (auto while_stmt : vec_while_stmt) {
      void *test_stmt = codegen->GetLoopTest(while_stmt);
      void *loop_body = codegen->GetLoopBody(while_stmt);
      vector<void *> vec_func_call;
      codegen->GetNodesByType(test_stmt, "preorder", "SgFunctionCallExp",
                              &vec_func_call);
      if (static_cast<unsigned int>(!vec_func_call.empty()) != 0U) {
        void *true_body = codegen->CreateBasicBlock();
        void *break_stmt = codegen->CreateStmt(V_SgBreakStmt);
        codegen->PrependChild(true_body, break_stmt);
        void *test_expr = codegen->GetExprFromStmt(test_stmt);
        void *not_test_expr =
            codegen->CreateExp(V_SgNotOp, codegen->CopyExp(test_expr));
        void *if_stmt =
            codegen->CreateIfStmt(not_test_expr, true_body, nullptr);
        codegen->PrependChild(loop_body, if_stmt);
        codegen->ReplaceExp(test_expr, codegen->CreateConst(1));
        ret = 1;
      }
    }
    return ret;
  }

  int process_function_call_in_loop_statement(
      CSageCodeGen * codegen, void *pTopFunc, const CInputOptions &options) {
    CMarsIrV2 mars_ir;
    mars_ir.build_mars_ir(codegen, pTopFunc);
    set<void *> visited;
    int ret = 0;
    for (auto kernel : mars_ir.get_top_kernels()) {
      ret |= process_function_call_in_loop_statement(codegen, kernel, visited);
    }
    return ret;
  }

  int func_inline_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options) {
    //  20161006 youxiang
    //  processing  function call in condition statement
    process_function_call_in_loop_statement(codegen, pTopFunc, options);
    CMarsIrV2 mars_ir;
    mars_ir.build_mars_ir(codegen, pTopFunc);
    bool errorOut = false;
    enum effort curr_flow;
    string effort_level = options.get_option_key_value("-a", "effort");
    if (effort_level == "low") {
      curr_flow = LOW;
    } else if (effort_level == "standard") {
      curr_flow = STANDARD;
    } else if (effort_level == "high") {
      curr_flow = HIGH;
    } else {
      curr_flow = MEDIUM;
    }
    for (auto sg_kernel : mars_ir.get_top_kernels()) {
      string kernel_name = codegen->GetFuncName(sg_kernel);
      //  printf("Enter func_inline_top %s\n",name.c_str());

      //  /////////////////////  /
      //  ZP: 20160920
      //  Check Global variable
      {
        CSageCodeGen *ast = codegen;
        void *func_decl = sg_kernel;
        vector<void *> v_ref;
        ast->GetNodesByType(sg_kernel, "preorder", "SgVarRefExp", &v_ref);
        for (auto ref : v_ref) {
          void *sg_name = ast->GetVariableInitializedName(ref);
          if (ast->IsGlobalInitName(sg_name) != 0) {
            void *type = ast->GetTypebyVar(sg_name);
            void *decl = ast->GetVariableDecl(sg_name);
            string s_var = ast->_up(sg_name);

            if (ast->IsConstType(type) == 0) {
              s_var = (decl) != nullptr ? ast->_up(decl) : ast->_up(sg_name);
              string global_info = "    Global variable: " + s_var + "\n";
              global_info +=
                  "    Kernel function: " + ast->_up(func_decl) + "\n";
              global_info += "    File location:   " +
                             ast->GetFileInfo_filename(func_decl, 1) + "\n";
              string msg =
                  "Do not support non-constant global variable in kernel: \n" +
                  global_info;
              dump_critical_message(FUCIN_ERROR_1(global_info));
#if PROJDEBUG
              cout << endl;
              cout << "ERROR: " << msg << endl;
              cout << endl;
#endif
              throw std::exception();
              continue;
            }
          }
        }
      }
      //  /////////////////////////  /
      vector<void *> vecCalls;
      codegen->GetNodesByType(sg_kernel, "preorder", "SgFunctionCallExp",
                              &vecCalls);

      for (size_t i = vecCalls.size(); i > 0; i--) {
        func_inline(codegen, vecCalls[i - 1], curr_flow, true, true);
      }

      //  delete original function declaration
      //  for(map<string, int>::iterator mi = used_func.begin(); mi !=
      //  used_func.end(); mi++) {
      //    string get_name = mi->first;
      //    //  printf("used function name %s\n",get_name.c_str());
      //    void * old_decl = codegen->GetFuncDeclByName(get_name);
      //    codegen->RemoveStmt(old_decl);
      //  }

      //  ////////////////////////////////////////  /
      //  remove redundant global variable extern
      //  ////////////////////////////////////////  /
      {
        //  moved to the end of this function
      }

      //  ////////////////////////////////////////  /
      //  remove redundant functions in the kernel file
      //  ////////////////////////////////////////  /
      //  void *sg_global = codegen->GetGlobal(sg_kernel);
      codegen->reset_func_decl_cache();
      codegen->reset_func_call_cache();
      vector<void *> all_funcs;
      codegen->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                              &all_funcs);
      set<void *> all_sub_funcs;
      for (auto decl : all_funcs) {
        if (sg_kernel == decl) {
          continue;
        }
        //  1. do not touch function declaration in header files or outside
        //  input files
        //  2. try to remove unused function declaration without body
        if ((codegen->IsFromInputFile(decl) == 0) ||
            codegen->isWithInHeaderFile(decl)) {
          continue;
        }
        all_sub_funcs.insert(decl);
      }

      //  youxiang: 20160927 bug678
      {
        bool LocalChanged;
        vector<void *> vec_calls;
        vector<void *> vec_func_refs;
        codegen->GetNodesByType(nullptr, "preorder", "SgFunctionCallExp",
                                &vec_calls);
        codegen->GetNodesByType(nullptr, "preorder", "SgFunctionRefExp",
                                &vec_func_refs);
        map<void *, set<void *>> call2func;
        map<void *, set<void *>> func2calls;
        map<void *, set<void *>> ref2func;
        map<void *, set<void *>> func2refs;
        set<void *> sub_funcs;
        for (auto func : all_sub_funcs) {
          if (mars_ir.is_kernel(func) || codegen->GetFuncName(func) == "main" ||
              (is_host_function(codegen, func) != 0)) {
            continue;
          }
          sub_funcs.insert(func);
          for (auto call : vec_calls) {
            void *callee_decl = codegen->GetFuncDeclByCall(call);
            if (callee_decl == nullptr) {
              call2func[call].insert(nullptr);
            } else if (codegen->isSameFunction(callee_decl, func)) {
              func2calls[func].insert(call);
              call2func[call].insert(func);
            }
          }
          for (auto func_ref : vec_func_refs) {
            void *callee_decl = codegen->GetFuncDeclByRef(func_ref);
            if (callee_decl == nullptr) {
              ref2func[func_ref].insert(nullptr);
            } else if (codegen->isSameFunction(callee_decl, func)) {
              func2refs[func].insert(func_ref);
              ref2func[func_ref].insert(func);
            }
          }
        }
        do {
          LocalChanged = false;
          for (auto func : sub_funcs) {
            bool not_dead =
                !func2calls[func].empty() || !func2refs[func].empty();
            if (not_dead) {
              continue;
            }
            LocalChanged = true;
            sub_funcs.erase(func);
            vector<void *> sub_calls;
            vector<void *> sub_func_refs;
            codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp",
                                    &sub_calls);
            codegen->GetNodesByType(func, "preorder", "SgFunctionRefExp",
                                    &sub_func_refs);
            for (auto call : sub_calls) {
              for (auto sub_func : call2func[call]) {
                if (sub_func == nullptr) {
                  continue;
                }
                func2calls[sub_func].erase(call);
              }
              call2func.erase(call);
            }
            for (auto func_ref : sub_func_refs) {
              for (auto sub_func : ref2func[func_ref]) {
                if (sub_func == nullptr) {
                  continue;
                }
                func2refs[sub_func].erase(func_ref);
              }
              ref2func.erase(func_ref);
            }
            string orig_func_name = codegen->GetFuncName(func);
            vector<void *> fp_list;
            void *dummy_decl = codegen->CreateFuncDecl(
                "void", "__merlin_dummy_" + orig_func_name, fp_list,
                codegen->GetGlobal(func), false, nullptr);
            codegen->SetStatic(dummy_decl);
            string file_name = codegen->get_file(func);
            if (file_name.find("__merlinwrapper_") == std::string::npos) {
              codegen->ReplaceStmt(func, dummy_decl);
            }
            break;
          }
        } while (!sub_funcs.empty() && LocalChanged);
      }

      //  1. check whether the remained functions are used only by top kernel
      //  function
      for (auto func : all_sub_funcs) {
        if (codegen->get_file(func) != codegen->get_file(sg_kernel)) {
          continue;
        }
        set<void *> bound_set;
        mars_ir.get_function_bound(func, &bound_set);
        assert(!bound_set.empty());
        bool is_host = false;
        for (auto bound_func : bound_set) {
          if (codegen->GetFuncName(bound_func) == "main") {
            is_host = true;
          }
        }
        if (is_host) {
          string func_info =
              "  Host function: " + codegen->GetFuncName(func) + "\n";
          func_info +=
              "  Kernel function: " + codegen->GetFuncName(sg_kernel) + "\n";
          func_info += "  Common file name: " + codegen->get_file(func);
          string msg = "Host function and kernel function can not be ";
          msg += "defined in the same file, please separate them into";
          msg += "different files. \n" + func_info;
          //  string msg = "Finding function \'" + codegen->GetFuncName(func) +
          //  "\' ("
          //              + codegen->get_file(func) + ") used by host or another
          //              kernel \'"
          //              + codegen->GetFuncName(bound_func) + "\' (" +
          //              codegen->get_file(bound_func) + ") with kernel \'"
          //              + codegen->get_file(sg_kernel) +
          //              "\' in the same file. Please seperate it manually";
          dump_critical_message(FUCIN_ERROR_2(func_info));
          errorOut = true;
        }
      }
      //  2. check whether all the noinline functions are in the same kernel
      //  files
      //
      SetVector<void *> noinlined_func;
      SetVector<void *> noinlined_call;
      codegen->GetSubFuncDeclsRecursively(sg_kernel, &noinlined_func,
                                          &noinlined_call);
      for (auto func : noinlined_func) {
        if (codegen->get_file(func) != codegen->get_file(sg_kernel)) {
          string callee_info = "\'" + codegen->GetFuncName(func) + "\' (" +
                               codegen->get_file(func) + ") ";
          string kernel_info = "\'" + codegen->GetFuncName(sg_kernel) + "\' (" +
                               codegen->get_file(sg_kernel) + ")";
          string msg = "Finding function " + callee_info + " used by kernel " +
                       kernel_info +
                       " but in the different file. Please merge it manually";
          dump_critical_message(FUCIN_ERROR_3(callee_info, kernel_info));
          errorOut = true;
        }
      }
    }

    //  ////////////////////////////////////////  /
    //  remove redundant global variable extern
    //  ZP: 20161028
    //  ////////////////////////////////////////  /
    {
      CSageCodeGen *ast = codegen;
      printf("Removing redundant global variables\n");
      set<void *> hit;
      for (auto kernel : mars_ir.get_top_kernels()) {
#if PROJDEBUG
        cout << "kernel - " << ast->_up(kernel) << endl;
#endif

        void *global = ast->GetGlobal(kernel);
        if (hit.count(global) == 0U) {
          auto v_decls = ast->GetChildStmts(global);
          for (auto decl : v_decls) {
            if ((ast->IsVariableDecl(decl) != 0) &&
                (ast->IsFromInputFile(decl) != 0)) {
              if (ast->IsExtern(decl)) {
#if PROJDEBUG
                cout << "Global extern: " << ast->_up(decl) << endl;
#endif

                void *init = ast->GetVariableInitializedName(decl);
                vector<void *> v_gv_refs;
                ast->get_ref_in_scope_recursive(init, kernel, &v_gv_refs);
                if (v_gv_refs.empty()) {
#if PROJDEBUG
                  cout << "Remove: " << ast->_up(decl) << endl;
#endif
                  vector<void *> fp_list;
                  void *dummy_global = ast->CreateFuncDecl(
                      "void", "__merlin_dummy_" + ast->GetVariableName(init),
                      fp_list, ast->GetGlobal(init), false, nullptr);
                  ast->SetStatic(dummy_global);
                  string file_name = codegen->get_file(decl);
                  if (file_name.find("__merlinwrapper_") == std::string::npos) {
                    codegen->ReplaceStmt(decl, dummy_global);
                  }
                  //  ast->RemoveStmt(decl);
                }
              }
            }
          }

          hit.insert(global);
        }
      }
    }

    if (errorOut) {
      throw std::exception();
    }
    return 0;
  }

#define HAS_INLINE_BY_PRAGMA 1
#if HAS_INLINE_BY_PRAGMA

  int detect_inline_pragma(void *func_decl_call, CMarsIrV2 *mars_ir) {
    CMarsAST_IF *ast = mars_ir->get_ast();
    if (func_decl_call == nullptr) {
      return 0;
    }

    void *curr_stmt = ast->TraceUpToStatement(func_decl_call);
    if (curr_stmt == nullptr) {
      return 0;
    }
    void *pre_stmt = ast->GetPreviousStmt(curr_stmt);
    if (pre_stmt == nullptr) {
      return 0;
    }

    if (ast->IsPragmaDecl(pre_stmt) != 0) {
      string s_value = mars_ir->get_pragma_attribute(pre_stmt, "inline");
      boost::algorithm::to_lower(s_value);
      if (s_value == "inline") {
        return 1;
      }
    }

    return 0;
  }

  int inline_one_func_call_with_rose(void *call, CMarsAST_IF *ast) {
    cout << "Inlining: " << ast->_p(call) << endl;

    if (ast->IsFunctionCall(call) == 0) {
      return 0;
    }

    //  void * org_body = nullptr;
    //  void* new_bb = ast->CreateBasicBlock( org_body );
    bool ret = doInline(static_cast<SgFunctionCallExp *>(call));
    return static_cast<int>(ret);

#if 0  //  backup code for pragma insertion solution
  static set<void*> inlined_func;

  void * func_decl = ast->GetFuncDeclByCall(call);
  if (!func_decl) return 0;
  void * func_body = ast->GetFuncBody(func_decl);
  if (!func_body) return 0;

  if (inlined_func.find(func_decl) == inlined_func.end()) {
    void * new_pragma = ast->CreatePragma("HLS inline", func_body);
    ast->PrependChild(func_body, new_pragma);

    inlined_func.insert(func_decl);
  }
#endif

    return 1;
  }

  int inline_one_func_recur_with_rose(
      void *func_decl, CMarsAST_IF *ast, const CInputOptions &options,
      CMarsIrV2 *mars_ir, set<void *> &visited_func) {
    if (func_decl == nullptr) {
      return 0;
    }

    void *rec_func = nullptr;
    if (ast->IsRecursiveFunction(func_decl, &rec_func) != 0) {
      return 0;
    }

    vector<void *> sub_func_calls;
    ast->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                        &sub_func_calls);

    for (auto one_call : sub_func_calls) {
      void *one_decl = ast->GetFuncDeclByCall(one_call);

      if ((one_decl != nullptr) &&
          visited_func.find(one_decl) == visited_func.end() &&
          (ast->IsRecursiveFunction(one_decl, &rec_func) == 0)) {
        //  inline sub-hierarchy first!
        inline_one_func_recur_with_rose(one_decl, ast, options, mars_ir,
                                        visited_func);

        //  2. detect and transform from func call pragmas
        if (detect_inline_pragma(one_call, mars_ir) != 0) {
          inline_one_func_call_with_rose(one_call, ast);
        }
      }
    }

    //  inline sub-hierarchy first!
    //  1. detect and transform from func decl pragmas
    if ((detect_inline_pragma(func_decl, mars_ir) != 0) &&
        visited_func.find(func_decl) == visited_func.end()) {
      vector<void *> all_calls;
      ast->GetFuncCallsFromDecl(func_decl, nullptr, &all_calls);

      for (auto one_call : all_calls) {
        if (visited_func.find(one_call) == visited_func.end()) {
          inline_one_func_call_with_rose(one_call, ast);
        }
      }

      visited_func.insert(func_decl);
    }

    return 1;
  }

  //  ////////////////////  /
  //  Notice: ZP May 2018
  //  1. This function calls doInline to perform the transform, so stability is
  //  dependent on ROSE implementation.
  //  2. Be very careful if want to enable the pragma to user, because it may
  //  get mismatched/missing if previous
  //    transformation is done on the function decls or calls
  int func_inline_by_pragma_top(CSageCodeGen * codegen, void *pTopFunc,
                                const CInputOptions &options) {
    printf("Merlin Pass [Function Inlining by Pragma] started ... \n");

    CMarsIrV2 mars_ir;
    mars_ir.build_mars_ir(codegen, pTopFunc);

    auto kernels = mars_ir.get_top_kernels();

    set<void *> visited_func;
    for (auto kernel : kernels) {
      inline_one_func_recur_with_rose(kernel, codegen, options, &mars_ir,
                                      visited_func);
    }

    return 0;
  }

  static int cp_fn_id;
  static std::unordered_set<void *> inlined_func_HLS;
  static std::unordered_map<void *, void *> cp_fn_map;

  bool only_used_once(void *func_decl, CMarsAST_IF *ast, CMarsIrV2 *mars_ir) {
    vector<void *> vec_calls;
    ast->GetFuncCallsFromDecl(func_decl, ast->GetGlobal(func_decl), &vec_calls);
    int cnt = 0;
    for (auto &one_call : vec_calls) {
      if (detect_inline_pragma(one_call, mars_ir) != 0) {
        cnt++;
      }
    }
    return cnt == vec_calls.size();
  }

  int remove_accel_inline_pragma(void *fn_call_decl, CMarsAST_IF *ast,
                                 CMarsIrV2 *mars_ir) {
    void *curr_stmt = ast->TraceUpToStatement(fn_call_decl);
    if (curr_stmt == nullptr) {
      return 0;
    }
    void *pre_stmt = ast->GetPreviousStmt(curr_stmt);
    if (pre_stmt == nullptr) {
      return 0;
    }

    if (ast->IsPragmaDecl(pre_stmt) != 0) {
      string s_value = mars_ir->get_pragma_attribute(pre_stmt, "inline");
      boost::algorithm::to_lower(s_value);
      if (s_value != "inline") {
        return 0;
      }
      ast->RemoveStmt(pre_stmt);
      return 1;
    }
    return 0;
  }

  int inline_one_func_call_with_HLS(void *call, CMarsAST_IF *ast,
                                    CMarsIrV2 *mars_ir) {
    cout << "Inlining: " << ast->_p(call) << endl;

    //  create a function decl copy
    void *func_decl = ast->GetFuncDeclByCall(call);
    if (func_decl == nullptr) {
      return 0;
    }
    void *func_body = ast->GetFuncBody(func_decl);
    if (func_body == nullptr) {
      return 0;
    }

    if (inlined_func_HLS.count(func_decl)) {
      remove_accel_inline_pragma(call, ast, mars_ir);
      return 0;
    }

    if (cp_fn_map.count(func_decl) == 0U) {
      if (!only_used_once(func_decl, ast, mars_ir)) {
        //  get a new name;
        string fn_name = ast->GetFuncName(func_decl);
        string new_fn_name = fn_name + "_cp_";
        ast->get_valid_global_name(ast->GetGlobal(func_decl), &new_fn_name);

        void *new_fn_decl = ast->CloneFuncDecl(
            func_decl, ast->GetGlobal(func_decl), new_fn_name);
        void *new_fn_body = ast->GetFuncBody(new_fn_decl);
        if (new_fn_body == nullptr) {
          return 0;
        }

        if (inlined_func_HLS.count(func_decl) == 0U) {
          void *new_pragma = ast->CreatePragma("HLS inline", new_fn_body);
          ast->PrependChild(new_fn_body, new_pragma);

          inlined_func_HLS.insert(new_fn_decl);
        }

        //  insert it to somewhere
        ast->InsertAfterStmt(new_fn_decl, func_decl);

        cp_fn_map[func_decl] = new_fn_decl;
      } else {  //  used once only, inline decl directly
        remove_accel_inline_pragma(call, ast, mars_ir);
        if (inlined_func_HLS.count(func_decl) == 0U) {
          void *new_pragma = ast->CreatePragma("HLS inline", func_body);
          ast->PrependChild(func_body, new_pragma);
          inlined_func_HLS.insert(func_decl);
          return 1;
        }
        return 0;
      }
    }

    remove_accel_inline_pragma(call, ast, mars_ir);
    void *cp_fn_decl = cp_fn_map[func_decl];

    //  replace call to copied fn decl
    vector<void *> fn_params = ast->GetFuncCallParamList(call);
    vector<void *> cp_params;
    for (auto fn_param : fn_params) {
      void *cp_param = ast->CopyExp(fn_param);
      cp_params.push_back(cp_param);
    }
    void *new_call = ast->CreateFuncCall(cp_fn_decl, cp_params);
    ast->ReplaceExp(call, new_call);

    return 1;
  }

  int convert_accel_pragma_to_HLS_pragma(void *func_decl, CMarsAST_IF *ast,
                                         CMarsIrV2 *mars_ir) {
    if (func_decl == nullptr) {
      return 0;
    }
    remove_accel_inline_pragma(func_decl, ast, mars_ir);

    void *func_body = ast->GetFuncBody(func_decl);
    if (func_body == nullptr) {
      return 0;
    }
    if (inlined_func_HLS.count(func_decl) == 0U) {
      void *new_pragma = ast->CreatePragma("HLS inline", func_body);
      ast->PrependChild(func_body, new_pragma);
      inlined_func_HLS.insert(func_decl);
      return 1;
    }
    return 0;
  }

  int inline_one_func_recur_with_HLS(void *func_decl, CMarsAST_IF *ast,
                                     const CInputOptions &options,
                                     CMarsIrV2 *mars_ir) {
    if (func_decl == nullptr) {
      return 0;
    }

    void *rec_func = nullptr;
    if (ast->IsRecursiveFunction(func_decl, &rec_func) != 0) {
      return 0;
    }

    vector<void *> sub_func_calls;
    ast->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                        &sub_func_calls);

    for (auto one_call : sub_func_calls) {
      void *one_decl = ast->GetFuncDeclByCall(one_call);

      if ((one_decl != nullptr) &&
          (ast->IsRecursiveFunction(one_decl, &rec_func) == 0)) {
        //  inline sub-hierarchy first!
        inline_one_func_recur_with_HLS(one_decl, ast, options, mars_ir);

        //  create a fn decl copy with HLS inline pragma, rename the decl,
        //  make callsite be the new created fn
        if (detect_inline_pragma(one_call, mars_ir) != 0) {
          inline_one_func_call_with_HLS(one_call, ast, mars_ir);
        }
      }
    }

    if (detect_inline_pragma(func_decl, mars_ir) != 0) {
      //  erase current pragma, insert #pragma HLS inline into
      //  the first line of the function body
      convert_accel_pragma_to_HLS_pragma(func_decl, ast, mars_ir);
    }

    return 1;
  }

  int func_inline_by_pragma_HLS_top(CSageCodeGen * codegen, void *pTopFunc,
                                    const CInputOptions &options) {
    printf("Merlin Pass [Function Inlining by Pragma to HLS Pragma] started "
           "... \n");

    CMarsIrV2 mars_ir;
    mars_ir.build_mars_ir(codegen, pTopFunc);

    auto kernels = mars_ir.get_top_kernels();

    cp_fn_id = 0;
    for (auto kernel : kernels) {
      inline_one_func_recur_with_HLS(kernel, codegen, options, &mars_ir);
    }

    return 0;
  }

#endif  //  HAS_INLINE_BY_PRAGMA
