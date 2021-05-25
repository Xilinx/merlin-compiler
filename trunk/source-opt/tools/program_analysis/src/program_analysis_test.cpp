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


#include <list>
#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

#include "array_delinearize.h"
#include "program_analysis.h"

#include "CallGraph.h"

#include "common.h"

#include "history.h"
#include "locations.h"
#include "id_update.h"
#include "pool_visit.h"

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsDepDistSet;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsResultFlags;
using MarsProgramAnalysis::CMarsVariable;
using std::list;
int loop_flatten_for_pipeline(CSageCodeGen *codegen, void *pTopFunc,
                              const CInputOptions &options, CMarsIr *mars_ir);
int index_switch_transform(CSageCodeGen *codegen, void *input_array,
                           void *input_loop, vector<int> switch_factor,
                           bool read_only = true);

int program_analysis_test_var_type(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  printf("**** Test Variable Type **** \n");
  vector<void *> vecVars;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgInitializedName", &vecVars);

  for (size_t j = 0; j < vecVars.size(); j++) {
    void *sg_var = vecVars[j];
    CMarsVariable mr_var(sg_var, codegen);

    string var_name = mr_var.unparse();
    if (var_name.empty() || var_name.find("__builtin_") == 0) {
      continue;
    }

    string add_info;
    if (mr_var.GetTypeString() == "argument") {
      string func_name;

      void *sg_func_decl = codegen->GetParent(codegen->GetParent(sg_var));
      assert(codegen->IsFunctionDeclaration(sg_func_decl));
      func_name = codegen->GetFuncName(sg_func_decl);
      if (func_name.find("__builtin_") == 0) {
        continue;
      }

      add_info = codegen->UnparseToString(sg_func_decl);
    } else {
      add_info = codegen->UnparseToString(codegen->GetParent(sg_var));
    }

    printf("[Var %s] type=%s : %s\n", mr_var.unparse().c_str(),
           codegen->GetVariableTypeName(sg_var).c_str(), add_info.c_str());
  }
  return 1;
}

int program_analysis_test_var_ref(CSageCodeGen *codegen, void *pTopFunc,
                                  const CInputOptions &options) {
  printf("**** Test Variable Reference **** \n");
  vector<void *> vecRefs;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgVarRefExp", &vecRefs);

  for (size_t j = 0; j < vecRefs.size(); j++) {
    void *sg_ref = vecRefs[j];
    void *sg_name = codegen->GetVariableInitializedName(sg_ref);
    void *sg_decl = codegen->GetVariableDecl(sg_name);
    string str_decl = codegen->UnparseToString(sg_decl);

    if (codegen->IsArgumentInitName(sg_name) != 0) {
      str_decl = codegen->UnparseToString(codegen->GetParent(sg_decl));
    }

    printf("[Ref %s] %s %s \n", codegen->UnparseToString(sg_name).c_str(),
           codegen->UnparseToString(codegen->TraceUpToAccess(sg_ref)).c_str(),
           str_decl.c_str());
  }
  return 1;
}

int program_analysis_test_var_range(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &options) {
  printf("**** Test Loop Canonicalization **** \n");

  vector<void *> vecLoops;
  codegen->GetNodesByType(pTopFunc, "postorder", "SgForStatement", &vecLoops);

  int ret_c = 0;
  for (size_t j = 0; j < vecLoops.size(); j++) {
    void *sg_loop = vecLoops[j];
    ret_c |= codegen->CanonicalizeForLoop(&sg_loop, true);
  }
  if (ret_c != 0) {
    codegen->init_defuse_range_analysis();
    vecLoops.clear();
    codegen->GetNodesByType(pTopFunc, "postorder", "SgForStatement", &vecLoops);
  }
  for (size_t j = 0; j < vecLoops.size(); j++) {
    void *sg_loop = vecLoops[j];
    ret_c |= codegen->StandardizeForLoop(&sg_loop);
  }

  if (ret_c != 0) {
    codegen->reset_func_decl_cache();
    codegen->reset_func_call_cache();
    codegen->init_defuse_range_analysis();
  }

  cout << endl << "**** Test Expression Range ****" << endl;
  vector<void *> vecExpr;
  codegen->GetNodesByType_compatible(pTopFunc, "SgExprStatement", &vecExpr);
  for (size_t i = 0; i < vecExpr.size(); i++) {
    void *sg_expr = codegen->GetExprFromStmt(vecExpr[i]);

    if (((codegen->IsFromInputFile(sg_expr) == 0) &&
         !codegen->IsTransformation(sg_expr)) ||
        codegen->isWithInHeaderFile(sg_expr)) {
      continue;
    }
    if (codegen->IsForStatement(codegen->GetParent(vecExpr[i])) != 0) {
      continue;
    }
    if (isSgForInitStatement(
            static_cast<SgNode *>(codegen->GetParent(vecExpr[i]))) != nullptr) {
      continue;
    }
    if (isSgForInitStatement(static_cast<SgNode *>(
            codegen->GetParent(codegen->GetParent(vecExpr[i])))) != nullptr) {
      continue;
    }
    if (isSgAssignOp(static_cast<SgNode *>(sg_expr)) != nullptr) {
      continue;
    }
    if (codegen->IsIntegerTypeRecur(codegen->GetTypeByExp(sg_expr)) == 0) {
      continue;
    }

    CMarsExpression me_expr(sg_expr, codegen);
    cout << "[Exp " << codegen->_p(sg_expr) << "] " << me_expr.print(1)
         << ", constant bound: " << me_expr.print(2) << endl;
  }

  cout << endl << "**** Test Scope Range ****" << endl;
  vector<void *> scope_set;  //  1. function body; 2. for loop
  vector<void *> array_set;  //  1. array; 2. pointers; 3. function arg
  {
    size_t j;
    scope_set = codegen->GetAllFuncDecl();
    vector<void *> vec_loop;
    codegen->GetNodesByType(nullptr, "preorder", "SgForStatement", &vec_loop);
    for (j = 0; j < vec_loop.size(); j++) {
      if (((codegen->IsFromInputFile(vec_loop[j]) == 0) &&
           !codegen->IsTransformation(vec_loop[j])) ||
          codegen->isWithInHeaderFile(vec_loop[j])) {
        continue;
      }
      void *loop_body = codegen->GetLoopBody(vec_loop[j]);
      if (loop_body != nullptr) {
        scope_set.push_back(loop_body);
      }
      scope_set.push_back(vec_loop[j]);
    }

    vector<void *> vec_decl;
    //  codegen->GetNodesByType(nullptr, "preorder",  "SgVariableDeclaration",
    //  &vec_decl);
    codegen->GetNodesByType(nullptr, "preorder", "SgInitializedName",
                            &vec_decl);

    for (j = 0; j < vec_decl.size(); j++) {
      void *sg_name =
          vec_decl[j];  //  codegen->GetVariableInitializedName(vec_decl[j]);
      string str_name = codegen->GetVariableName(sg_name);
      //  if (str_name == "" || str_name.find("__builtin_") == 0 ) continue;
      if (str_name.empty() || str_name.find("__") == 0) {
        continue;
      }
      void *sg_type = codegen->GetTypebyVar(sg_name);
      //  if (codegen->is_floating_node(sg_type)) continue; //  built-in
      //  variables
      if (((codegen->IsFromInputFile(sg_name) == 0) &&
           !codegen->IsTransformation(sg_name)) ||
          codegen->isWithInHeaderFile(sg_name)) {
        continue;
      }
      void *basic_type = nullptr;
      vector<size_t> vec_sizes;
      codegen->get_type_dimension(sg_type, &basic_type, &vec_sizes, sg_name);
      if (basic_type == nullptr || vec_sizes.empty()) {
        continue;
      }

      array_set.push_back(sg_name);
    }
  }

  {
    size_t j;
    cout << endl;
    cout << "Scopes: " << endl;
    for (j = 0; j < scope_set.size(); j++) {
      cout << "  " << codegen->GetASTTypeString(scope_set[j]) << " "
           << codegen->UnparseToString(scope_set[j]) << endl;
    }
    cout << endl;
    cout << "Arrays: " << endl;
    for (j = 0; j < array_set.size(); j++) {
      cout << "  "
           << codegen->UnparseToString(codegen->GetTypebyVar(array_set[j]), 20)
           << " " << codegen->UnparseToString(array_set[j]) << endl;
    }
  }

  {
    cout << endl;
    cout << "Ranges: " << endl;
    size_t i;
    size_t j;
    for (i = 0; i < scope_set.size(); i++) {
      //  if (i != 0) continue;
      cout << "  " << codegen->GetASTTypeString(scope_set[i]) << " "
           << codegen->UnparseToString(scope_set[i]) << endl;
      for (j = 0; j < array_set.size(); j++) {
        void *scope = scope_set[i];
        assert(scope);

        //  if (j != 0) continue;
        ArrayRangeAnalysis range(array_set[j], codegen, scope, scope, false,
                                 false);
        string str_range = range.print_s();

        if ((range.has_read() != 0) || (range.has_write() != 0)) {
          cout << "    "
               << codegen->UnparseToString(codegen->GetTypebyVar(array_set[j]),
                                           20)
               << " " << codegen->UnparseToString(array_set[j]) << " : "
               << str_range << endl;
        }

        //  Yuxin: Jul 19 2018
        //  Add read and write range intersect/union test
        if ((range.has_read() != 0) && (range.has_write() != 0)) {
          vector<CMarsRangeExpr> mr_read_range = range.GetRangeExprRead();
          vector<CMarsRangeExpr> mr_write_range = range.GetRangeExprWrite();
          if (mr_read_range.size() != mr_write_range.size()) {
            continue;
          }
          int dim_size = mr_read_range.size();
          for (int i = 0; i < dim_size; ++i) {
            CMarsRangeExpr read_range = mr_read_range[i];
            CMarsRangeExpr write_range = mr_write_range[i];
            CMarsRangeExpr mr_intersect = read_range.Intersect(write_range);
            cout << "    "
                 << "intersect: " << mr_intersect.print_e() << endl;

            CMarsRangeExpr mr_union = read_range.Union(write_range);
            cout << "    "
                 << "union: " << mr_union.print_e() << endl;
          }
        }
      }
    }
  }

  return 1;
}

int program_analysis_test_var_range_with_assert(CSageCodeGen *codegen,
                                                void *pTopFunc,
                                                const CInputOptions &options) {
  printf("**** Test Loop Canonicalization **** \n");
  vector<void *> vecLoops;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgForStatement", &vecLoops);

  for (size_t j = 0; j < vecLoops.size(); j++) {
    void *sg_loop = vecLoops[j];

    SgInitializedName *sg_iv;
    SgExpression *sg_lower;
    SgExpression *sg_upper;
    SgExpression *sg_stride;
    bool isIncremental;

    //  non-side-effect
    int ret_c = codegen->CanonicalizeForLoop(&sg_loop, true);
    if (ret_c != 0) {
      codegen->init_defuse_range_analysis();
    }

    int is_canonical = static_cast<int>(SageInterface::isCanonicalForLoop(
        static_cast<SgNode *>(sg_loop), &sg_iv, &sg_lower, &sg_upper,
        &sg_stride, nullptr, &isIncremental));

    int lower = 0;
    int upper = 0;
    if (is_canonical != 0) {
      printf("[Loop %s] %s [%d..%d]\n", codegen->UnparseToString(sg_iv).c_str(),
             codegen->UnparseToString(sg_loop).c_str(), lower, upper);
    } else {
      printf("[Loop non-canonical] %s \n",
             codegen->UnparseToString(sg_loop).c_str());
    }
  }

  printf("\n**** Test Variable Range **** \n");

  codegen->init_defuse();
  codegen->reset_range_analysis();

  printf("\n**** Test Expression Range **** \n");

  //  print out the information for every ExprStatement
  vector<void *> vecExpr;
  codegen->GetNodesByType_compatible(pTopFunc, "SgExprStatement", &vecExpr);

  for (size_t i = 0; i < vecExpr.size(); i++) {
    void *sg_expr = codegen->GetExprFromStmt(vecExpr[i]);
    if (codegen->IsIntegerTypeRecur(codegen->GetTypeByExp(sg_expr)) == 0) {
      continue;
    }
    CMarsExpression me_expr(sg_expr, codegen);
    cout << "[Exp " << codegen->_p(sg_expr) << "] " << me_expr.print(1) << endl;
    //  me_expr.print_s() << endl;
    CMarsRangeExpr range = me_expr.get_range();
    CMarsRangeExpr range_refine = range;
    range_refine.refine_range_in_scope(nullptr, sg_expr);
    if (range.has_bound() != 0) {
      cout << "[Range " << range.print()
           << " (min(" + my_itoa_hex(range_refine.get_const_lb()) +
                  ")...(max(" + my_itoa_hex(range_refine.get_const_ub()) + "))"
           << "]" << endl;
    }
  }
  vector<void *> vec_loops;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgForStatement", &vec_loops);
  for (auto loop : vec_loops) {
    int64_t TC = 0;
    int64_t TC_ub = 0;
    codegen->get_loop_trip_count(loop, &TC, &TC_ub);
    if (TC_ub > 0)
      cout << codegen->_p(loop) << ": TC = " << TC << ", TC_ub = " << TC_ub
           << endl;
    else
      cout << codegen->_p(loop) << ": TC = ? "
           << ", TC_ub = ?" << endl;
  }
  return 1;
}

int program_analysis_test_var_range_with_bound(CSageCodeGen *codegen,
                                               void *pTopFunc,
                                               const CInputOptions &options) {
  printf("**** Test variable range with bound  **** \n");

  vector<void *> vecLoops;
  codegen->GetNodesByType(pTopFunc, "postorder", "SgForStatement", &vecLoops);

  vector<void *> scope_set;  //  1. function body; 2. for loop
  vector<void *> array_set;  //  1. array; 2. pointers; 3. function arg
  {
    size_t j;
    scope_set = codegen->GetAllFuncDecl();
    vector<void *> vec_loop;
    codegen->GetNodesByType(nullptr, "preorder", "SgForStatement", &vec_loop);
    for (j = 0; j < vec_loop.size(); j++) {
      void *loop_body = codegen->GetLoopBody(vec_loop[j]);
      if (loop_body != nullptr) {
        scope_set.push_back(loop_body);
      }
      scope_set.push_back(vec_loop[j]);
    }

    vector<void *> vec_decl;
    //  codegen->GetNodesByType(nullptr, "preorder",  "SgVariableDeclaration",
    //  &vec_decl);
    codegen->GetNodesByType(nullptr, "preorder", "SgInitializedName",
                            &vec_decl);

    for (j = 0; j < vec_decl.size(); j++) {
      void *sg_name =
          vec_decl[j];  //  codegen->GetVariableInitializedName(vec_decl[j]);
      string str_name = codegen->GetVariableName(sg_name);
      //  if (str_name == "" || str_name.find("__builtin_") == 0 ) continue;
      if (str_name.empty() || str_name.find("__") == 0) {
        continue;
      }
      void *sg_type = codegen->GetTypebyVar(sg_name);
      //  if (codegen->is_floating_node(sg_type)) continue; //  built-in
      //  variables
      void *basic_type = nullptr;
      vector<size_t> vec_sizes;
      codegen->get_type_dimension(sg_type, &basic_type, &vec_sizes, sg_name);
      if (basic_type == nullptr || vec_sizes.empty()) {
        continue;
      }

      array_set.push_back(sg_name);
    }
  }

  {
    size_t j;
    cout << endl;
    cout << "Scopes: " << endl;
    for (j = 0; j < scope_set.size(); j++) {
      cout << "  " << codegen->GetASTTypeString(scope_set[j]) << " "
           << codegen->UnparseToString(scope_set[j]) << endl;
    }
    cout << endl;
    cout << "Arrays: " << endl;
    for (j = 0; j < array_set.size(); j++) {
      cout << "  "
           << codegen->UnparseToString(codegen->GetTypebyVar(array_set[j]), 20)
           << " " << codegen->UnparseToString(array_set[j]) << endl;
    }
  }

  {
    cout << endl;
    cout << "Ranges: " << endl;
    size_t i;
    size_t j;
    for (i = 0; i < scope_set.size(); i++) {
      //  if (i != 0) continue;
      cout << "  " << codegen->GetASTTypeString(scope_set[i]) << " "
           << codegen->UnparseToString(scope_set[i]) << endl;
      for (j = 0; j < array_set.size(); j++) {
        void *scope = scope_set[i];
        assert(scope);

        //  if (j != 0) continue;
        ArrayRangeAnalysis range(array_set[j], codegen, scope, scope, false,
                                 true);
        string str_range = range.print_s();

        if ((range.has_read() != 0) || (range.has_write() != 0)) {
          cout << "    "
               << codegen->UnparseToString(codegen->GetTypebyVar(array_set[j]),
                                           20)
               << " " << codegen->UnparseToString(array_set[j]) << " : "
               << str_range << endl;
        }

        //  Yuxin: Jul 19 2018
        //  Add read and write range intersect/union test
        if ((range.has_read() != 0) && (range.has_write() != 0)) {
          vector<CMarsRangeExpr> mr_read_range = range.GetRangeExprRead();
          vector<CMarsRangeExpr> mr_write_range = range.GetRangeExprWrite();
          if (mr_read_range.size() != mr_write_range.size()) {
            continue;
          }
          int dim_size = mr_read_range.size();
          for (int i = 0; i < dim_size; ++i) {
            CMarsRangeExpr read_range = mr_read_range[i];
            CMarsRangeExpr write_range = mr_write_range[i];
            CMarsRangeExpr mr_intersect = read_range.Intersect(write_range);
            cout << "    "
                 << "intersect: " << mr_intersect.print_e() << endl;

            CMarsRangeExpr mr_union = read_range.Union(write_range);
            cout << "    "
                 << "union: " << mr_union.print_e() << endl;
          }
        }
      }
    }
  }

  return 1;
}

#define __VERBOSE__ 0
int program_analysis_test_expr_gen(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  printf("**** Test Expression Generation **** \n");
  vector<void *> vecExpr;

#if __VERBOSE__
  codegen->GetNodesByType_compatible(pTopFunc, "SgExpression", &vecExpr);
#else
  codegen->GetNodesByType_compatible(pTopFunc, "SgExprStatement", &vecExpr);
#endif

  for (size_t j = 0; j < vecExpr.size(); j++) {
#if __VERBOSE__
    void *sg_expr = vecExpr[j];
#else
    void *sg_expr = codegen->GetExprFromStmt(vecExpr[j]);
    void *sg_stmt = vecExpr[j];
    void *sg_parent = codegen->GetParent(sg_stmt);
    if ((codegen->IsForStatement(sg_parent) != 0) &&
        sg_stmt == codegen->GetLoopTest(sg_parent)) {
      continue;
    }
    if (codegen->IsForInitStatement(sg_parent) != 0) {
      continue;
    }
#endif
    void *for_loop =
        codegen->TraceUpByTypeCompatible(sg_stmt, V_SgForStatement);
    // for display
    if (true) {
      vector<void *> vecExpr1;
      codegen->GetNodesByType_compatible(sg_expr, "SgExpression", &vecExpr1);
      for (size_t i = 0; i < vecExpr1.size(); i++) {
        void *expr = vecExpr1[i];
        if (codegen->IsIntegerTypeRecur(codegen->GetTypeByExp(expr)) == 0) {
          continue;
        }
        CMarsExpression me_expr(expr, codegen);

        vector<void *> vars;
        me_expr.get_vars(&vars);
        string var1;
        if (!vars.empty()) {
          var1 = codegen->UnparseToString(codegen->GetParent(vars[0]));
        } else {
          var1 = "";
        }

        cout << "  [Exp " << codegen->_p(expr) << "] "
             << " @" << codegen->get_line(expr) << " " << me_expr.print(1)
             << endl;
        cout << (((for_loop == nullptr) || me_expr.IsLoopInvariant(for_loop))
                     ? "  loop invariant"
                     : "  loop variant")
             << endl;
      }
    }
    if (codegen->IsIntegerTypeRecur(codegen->GetTypeByExp(sg_expr)) == 0) {
      continue;
    }

    int nLine = codegen->GetFileInfo_line(sg_expr);
    CMarsExpression me_expr(sg_expr, codegen);

    cout << "[Exp " << my_itoa(nLine) << " " << codegen->_p(sg_expr) << "] "
         << me_expr.print(1) << endl;
    cout << (((for_loop == nullptr) || me_expr.IsLoopInvariant(for_loop))
                 ? "loop invariant"
                 : "loop variant")
         << endl;
  }

  //  test constant expression generation
  {
    float a = 1.2;
    void *float_a = codegen->CreateConst(&a, V_SgTypeFloat);
    cout << codegen->_up(float_a) << " (expected " << a << ")" << endl;

    double b = 1.2;
    void *double_a = codegen->CreateConst(&b, V_SgTypeDouble);
    cout << codegen->_up(double_a) << " (expected " << b << ")" << endl;

    string s = "Hello World";
    void *string_a = codegen->CreateConst(&s, V_SgTypeString);
    cout << codegen->_up(string_a) << " (expected " << s << ")" << endl;

    char c = 'Z';
    void *char_c = codegen->CreateConst(&c, V_SgTypeChar);
    cout << codegen->_up(char_c) << " (expected " << c << ")" << endl;

    unsigned char uc = 'Z';
    void *char_uc = codegen->CreateConst(&c, V_SgTypeUnsignedChar);
    cout << codegen->_up(char_uc) << " (expected " << uc << ")" << endl;

    int64_t Li = 12345678900000L;
    void *long_int_a = codegen->CreateConst(&Li, V_SgTypeLong);
    cout << codegen->_up(long_int_a) << " (expected " << Li << ")" << endl;
#if 0
    int64_t LLi = 12345678900000LL;
    void *long_long_int_a = codegen->CreateConst(&LLi, V_SgTypeLongLong);
    cout << codegen->_up(long_long_int_a)
      << " (expected " << LLi << ")" << endl;
#endif
  }
  {
    //  test unary op mode
    vector<void *> vec_exp;
    codegen->GetNodesByType(nullptr, "preorder", "SgPlusPlusOp", &vec_exp);
    for (auto exp : vec_exp) {
      cout << codegen->_up(exp) << " with unary op mode "
           << codegen->GetUnaryOpMode(exp) << endl;
    }
    codegen->GetNodesByType(nullptr, "preorder", "SgMinusMinusOp", &vec_exp);
    for (auto exp : vec_exp) {
      cout << codegen->_up(exp) << " with unary op mode "
           << codegen->GetUnaryOpMode(exp) << endl;
    }
  }
  return 1;
}

int program_analysis_test_call_graph(CSageCodeGen *codegen, void *pTopFunc,
                                     const CInputOptions &options) {
  printf("**** Test Call Graph **** \n");

  CallGraphBuilder CGBuilder(static_cast<SgProject *>(codegen->GetProject()));
  CGBuilder.buildCallGraph(builtinFilter());

  SgIncidenceDirectedGraph *graph = CGBuilder.getGraph();

  boost::unordered_map<SgFunctionDeclaration *, SgGraphNode *> nodemap =
      CGBuilder.getGraphNodesMapping();

  //  to make it ordered for testing comparison
  map<SgFunctionDeclaration *, SgGraphNode *> new_map;
  {
    boost::unordered_map<SgFunctionDeclaration *, SgGraphNode *>::iterator it;
    for (it = nodemap.begin(); it != nodemap.end(); it++) {
      new_map[it->first] = it->second;
    }
  }

  {
    map<SgFunctionDeclaration *, SgGraphNode *>::iterator it;

    for (it = new_map.begin(); it != new_map.end(); it++) {
      void *sg_func = it->first;
      SgGraphNode *gn = it->second;
      assert(gn->get_SgNode() == sg_func);

      cout << "[Func] " << codegen->GetASTTypeString(sg_func) << " "
           << codegen->UnparseToString(sg_func) << " : ";

      vector<SgGraphNode *> vec_gn;
      vector<SgGraphNode *>::iterator itt;

      vec_gn.clear();
      graph->getPredecessors(gn, vec_gn);
      cout << "CALL:";
      for (itt = vec_gn.begin(); itt != vec_gn.end(); itt++) {
        cout << codegen->GetFuncName((*itt)->get_SgNode()) << " ";
      }
      vec_gn.clear();
      graph->getSuccessors(gn, vec_gn);
      cout << " SUB:";
      for (itt = vec_gn.begin(); itt != vec_gn.end(); itt++) {
        cout << codegen->GetFuncName((*itt)->get_SgNode()) << " ";
      }
      cout << endl;
    }
  }

  return 1;
}

int program_analysis_test_liveness(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  printf("**** Test UseDef and Liveness **** \n");
  vector<void *> vecExpr;

  void *pMainFunc = codegen->GetFuncDeclByName("main");
  //  void * pMainFunc = codegen->GetFuncDeclByName("KMeansClassified__call");
  //  void * pMainFunc = codegen->GetFuncDeclByName("KMeansClassified_c");
  if (pMainFunc == nullptr) {
    printf("Main function is required.\n");
    return 0;
  }

  codegen->set_function_for_liveness(pMainFunc);
  codegen->GetNodesByType_compatible(codegen->GetFuncBody(pMainFunc),
                                     "SgStatement", &vecExpr);

  for (size_t j = 0; j < vecExpr.size(); j++) {
    void *sg_expr = vecExpr[j];
    if (sg_expr == codegen->GetFuncBody(pMainFunc)) {
      continue;
    }

    int nLine = codegen->GetFileInfo_line(codegen->TraceUpToStatement(sg_expr));
    printf("[Stmt %d %s] %s -- \n", nLine,
           codegen->GetASTTypeString(sg_expr).c_str(),
           codegen->UnparseToString(sg_expr).c_str());

    vector<void *> liveIn;
    vector<void *> liveOut;
    if (codegen->IsForStatement(sg_expr) != 0) {
      codegen->get_liveness_for_loop(sg_expr, &liveIn, &liveOut);
    } else {
      codegen->get_liveness_in(sg_expr, &liveIn);
      codegen->get_liveness_out(sg_expr, &liveOut);
    }
    vector<void *>::iterator it;

    std::vector<std::pair<void *, void *>> defs = codegen->GetAllDefbyPosition(
        sg_expr);  //  defuse->getDefMultiMapFor((SgNode*)sg_expr);
    std::vector<std::pair<void *, void *>> uses = codegen->GetAllUsebyPosition(
        sg_expr);  //  defuse->getUseMultiMapFor((SgNode*)sg_expr);
    std::vector<std::pair<void *, void *>>::iterator itt;

    printf("  DEF: \n");
    std::sort(defs.begin(), defs.end(), [&](auto a, auto b) {
      return codegen->_up(a.first) + codegen->_up(a.second) <
             codegen->_up(b.first) + codegen->_up(b.second);
    });
    for (itt = defs.begin(); itt != defs.end(); itt++) {
      printf("  %s %s %s\n", codegen->UnparseToString((itt)->first).c_str(),
             codegen->GetASTTypeString((itt)->second).c_str(),
             codegen->UnparseToString((itt)->second).c_str());
    }
    printf("  USE: \n");
    std::sort(uses.begin(), uses.end(), [&](auto a, auto b) {
      return codegen->_up(a.first) + codegen->_up(a.second) <
             codegen->_up(b.first) + codegen->_up(b.second);
    });
    for (itt = uses.begin(); itt != uses.end(); itt++) {
      void *pa = codegen->GetParent(itt->second);
      printf("  %s %s in %s\n", codegen->UnparseToString((itt)->first).c_str(),
             codegen->GetASTTypeString((itt)->second).c_str(),
             codegen->UnparseToString(pa).c_str());
    }
    printf("  LIVE in : ");
    std::sort(liveIn.begin(), liveIn.end(), [&](auto a, auto b) {
      return codegen->_up(a) < codegen->_up(b);
    });
    for (it = liveIn.begin(); it != liveIn.end(); it++) {
      printf(" %s,", codegen->UnparseToString(*it).c_str());
    }
    printf("\n");
    printf("  LIVE out: ");
    std::sort(liveOut.begin(), liveOut.end(), [&](auto a, auto b) {
      return codegen->_up(a) < codegen->_up(b);
    });
    for (it = liveOut.begin(); it != liveOut.end(); it++) {
      printf(" %s,", codegen->UnparseToString(*it).c_str());
    }
    printf(" \n");
  }
  return 1;
}

int program_analysis_test_access(CSageCodeGen *codegen, void *pTopFunc,
                                 const CInputOptions &options) {
  printf("**** Test Loop and Access **** \n");
  vector<void *> vecExpr;

  codegen->GetNodesByType_compatible(pTopFunc, "SgExprStatement", &vecExpr);

  for (size_t j = 0; j < vecExpr.size(); j++) {
    void *sg_expr = codegen->GetExprFromStmt(vecExpr[j]);
    void *sg_stmt = vecExpr[j];
    void *sg_parent = codegen->GetParent(sg_stmt);
    if ((codegen->IsForStatement(sg_parent) != 0) &&
        sg_stmt == codegen->GetLoopTest(sg_parent)) {
      continue;
    }
    if (codegen->IsForInitStatement(sg_parent) != 0) {
      continue;
    }
    if (codegen->IsFunctionCall(sg_expr) != 0) {
      continue;
    }
    if (codegen->IsAssignOp(sg_expr) != 0) {
      continue;
    }

    vector<void *> vec_loops_;
    vec_loops_.push_back(codegen->GetProject());
    codegen->get_loop_nest_in_scope(sg_expr, nullptr, &vec_loops_);

    for (size_t i = 0; i < vec_loops_.size(); i++) {
      void *sg_scope = vec_loops_[i];

      void *body = codegen->GetLoopBody(sg_scope);
      if (body != nullptr) {
        sg_scope = body;
      }

      CMarsAccess access(sg_expr, codegen, sg_scope);
      cout << access.print() << endl;
    }
  }
  return 1;
}
void program_analysis_test_type_by_ref(CSageCodeGen *codegen, void *pTopFunc,
                                       const CInputOptions &options) {
  size_t i;
  size_t j;
  printf("**** Test Array/Point Type by Reference**** \n");
  vector<void *> vecExpr;

  codegen->GetNodesByType_compatible(pTopFunc, "SgExpression", &vecExpr);

  vector<void *> vec_refs;
  for (j = 0; j < vecExpr.size(); j++) {
    void *sg_expr = vecExpr[j];
    if (sg_expr != codegen->TraceUpToRootExpression(sg_expr)) {
      continue;
    }
    if (codegen->is_floating_node(sg_expr) != 0) {
      continue;
    }

    CMarsExpression me(sg_expr, codegen, sg_expr);
    if (me.IsConstant() != 0) {
      continue;
    }

    void *sg_array;
    void *basic_type;
    vector<size_t> vecSizes;

    codegen->get_type_dimension_by_ref(sg_expr, &sg_array, &basic_type,
                                       &vecSizes);

    if ("__" == codegen->_up(sg_expr).substr(0, 2)) {
      continue;
    }

    if (sg_array != nullptr) {
      cout << codegen->UnparseToString(sg_expr) << ": "
           << codegen->UnparseToString(sg_array) << " "
           << codegen->UnparseToString(basic_type) << " ";
      for (i = 0; i < vecSizes.size(); i++) {
        cout << "[" << vecSizes[i] << "]";
      }
      cout << endl;
    } else {
      cout << codegen->UnparseToString(sg_expr) << ": " << endl;
    }
  }
}

void program_analysis_test_dependence(CSageCodeGen *codegen, void *pTopFunc,
                                      const CInputOptions &options) {
  size_t i;
  size_t j;
  size_t k;
  size_t l;
  printf("**** Test Dependence **** \n");
  vector<void *> vecExpr;

  codegen->GetNodesByType_compatible(pTopFunc, "SgExprStatement", &vecExpr);

  vector<void *> vec_refs;
  for (j = 0; j < vecExpr.size(); j++) {
    void *sg_expr = codegen->GetExprFromStmt(vecExpr[j]);
    void *sg_stmt = vecExpr[j];
    void *sg_parent = codegen->GetParent(sg_stmt);
    if ((codegen->IsForStatement(sg_parent) != 0) &&
        sg_stmt == codegen->GetLoopTest(sg_parent)) {
      continue;
    }
    if (codegen->IsForInitStatement(sg_parent) != 0) {
      continue;
    }
    if (codegen->IsFunctionCall(sg_expr) != 0) {
      continue;
    }

    vec_refs.push_back(sg_expr);

    CMarsAccess access(sg_expr, codegen, nullptr);
    cout << access.print() << endl;
  }

  //  for each pair of access 1, 2
  //  for each common loop
  for (i = 0; i < vec_refs.size(); i++) {
    for (j = 0; j < vec_refs.size(); j++) {
      if (i == j) {
        continue;
      }
      CMarsAccess access1(vec_refs[i], codegen, nullptr);
      CMarsAccess access2(vec_refs[j], codegen, nullptr);

      if (access1.get_array() != access2.get_array()) {
        continue;
      }

      vector<void *> vec_loops1;
      vector<void *> vec_loops2;
      {
        codegen->get_loop_nest_in_scope(access1.get_ref(), nullptr,
                                        &vec_loops1);
        codegen->get_loop_nest_in_scope(access2.get_ref(), nullptr,
                                        &vec_loops2);
      }

      //  find the common loops
      for (k = 0; k < vec_loops1.size(); k++) {
        for (l = 0; l < vec_loops2.size(); l++) {
          if (vec_loops1[k] == vec_loops2[l]) {
            void *loop = vec_loops1[k];

            void *body = codegen->GetLoopBody(loop);
            if (body != nullptr) {
              loop = body;
            }

            access1.set_scope(loop);
            access2.set_scope(loop);

            CMarsDepDistSet dist_set;
            CMarsResultFlags results_flags;
            int ret =
                GetDependence(access1, access2, &dist_set, &results_flags);

            cout << "[GetDist " << ret << " : " << access1.print_s() << " -> "
                 << access2.print_s() << " ] " << dist_set.print() << endl;
          }
        }
      }
    }
  }
}
void program_analysis_test_movable(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  size_t i;
  size_t j;
  size_t k;
  printf("**** Test Expression Movability**** \n");

  void *pMain = codegen->GetFuncDeclByName("main");
  void *pMainBody = codegen->GetFuncBody(pMain);

  vector<void *> vec_expr;
  vector<void *> vec_pos;
  for (i = 0; i < static_cast<size_t>(codegen->GetChildStmtNum(pMainBody));
       i++) {
    void *stmt = codegen->GetChildStmt(pMainBody, i);

    if (codegen->IsBasicBlock(stmt) != 0) {
      for (j = 0; j < static_cast<size_t>(codegen->GetChildStmtNum(stmt));
           j++) {
        void *sub_stmt = codegen->GetChildStmt(stmt, j);
        if (codegen->IsExprStatement(sub_stmt) != 0) {
          vec_expr.push_back(codegen->GetExprFromStmt(sub_stmt));
        }
      }
    } else {
      string str = codegen->UnparseToString(stmt);
      if (str.find("pos") == 0) {
        vec_pos.push_back(stmt);
      }
    }
  }

  void *curr_exp;
  void *move_from;
  void *move_to;

  //  1. test default source move_from = nullptr
  printf("==== test default source \n");
  for (i = 0; i < vec_expr.size(); i++) {
    curr_exp = vec_expr[i];
    for (j = 0; j < vec_pos.size(); j++) {
      move_to = vec_pos[j];
      int is_movable = codegen->is_movable_test(curr_exp, move_to);

      printf("%d: %s (nullptr->%s)\n", is_movable,
             codegen->UnparseToString(curr_exp).c_str(),
             codegen->UnparseToString(move_to).c_str());
    }
  }

  //  2. test normal expression
  printf("==== test normal expression\n");
  {
    for (i = 0; i < vec_expr.size(); i++) {
      curr_exp = vec_expr[i];
      for (j = 0; j < vec_pos.size(); j++) {
        for (k = 0; k < vec_pos.size(); k++) {
          move_from = vec_pos[j];
          move_to = vec_pos[k];
          int is_side_effect = codegen->has_side_effect(curr_exp);
          int is_movable =
              codegen->is_movable_test(curr_exp, move_to, move_from);

          if ((is_side_effect == 0) || (j == 0 && k == 0)) {
            printf("%d: %s (%s->%s)\n", is_movable,
                   codegen->UnparseToString(curr_exp).c_str(),
                   codegen->UnparseToString(move_from).c_str(),
                   codegen->UnparseToString(move_to).c_str());
          }
        }
      }
    }
  }

  //  3. test floating expression
  printf("==== test floating expression\n");
  {
    for (i = 0; i < vec_expr.size(); i++) {
      curr_exp = codegen->CopyExp(vec_expr[i]);
      for (j = 0; j < vec_pos.size(); j++) {
        for (k = 0; k < vec_pos.size(); k++) {
          move_from = vec_pos[j];
          move_to = vec_pos[k];
          int is_side_effect = codegen->has_side_effect(curr_exp);
          int is_movable =
              codegen->is_movable_test(curr_exp, move_to, move_from);

          if ((is_side_effect == 0) || (j == 0 && k == 0)) {
            printf("%d: %s (%s->%s)\n", is_movable,
                   codegen->UnparseToString(curr_exp).c_str(),
                   codegen->UnparseToString(move_from).c_str(),
                   codegen->UnparseToString(move_to).c_str());
          }
        }
      }
    }
  }
}

#if 0
int program_analysis_test_delinear(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  size_t i, j;
  printf("**** Test Array Delinearization**** \n");

  void *pMain = codegen->GetFuncDeclByName("main");
  void *pMainBody = codegen->GetFuncBody(pMain);

  vector<void *> vec_array;
  for (i = 0; i < (size_t)codegen->GetChildStmtNum(pMainBody); i++) {
    void *stmt = codegen->GetChildStmt(pMainBody, i);

    if (codegen->IsBasicBlock(stmt)) {
      for (j = 0; j < (size_t)codegen->GetChildStmtNum(stmt); j++) {
        void *sub_stmt = codegen->GetChildStmt(stmt, j);
        if (codegen->IsExprStatement(sub_stmt)) {
          void *sub_exp = codegen->GetExprFromStmt(sub_stmt);
          if (codegen->IsVarRefExp(sub_exp)) {
            vec_array.push_back(codegen->GetVariableInitializedName(sub_exp));
          }
        }
      }
    }
  }

  for (i = 0; i < vec_array.size(); i++) {
    vector<size_t> dim_split_steps;
    map<void *, size_t> mapAlias2BStep;
    analysis_delinearize(codegen, vec_array[i], 0, dim_split_steps,
                         mapAlias2BStep, false);
    apply_delinearize(codegen, vec_array[i], 0, dim_split_steps,
                      mapAlias2BStep);

    int nLine = codegen->GetFileInfo_line(codegen->_pa(vec_array[i]));
    cout << nLine << " : " << codegen->_p(codegen->_pa(vec_array[i])) << endl;
    cout << "  old_step: ";
    for (j = 0; j < dim_split_steps.size(); j++)
      cout << " " << dim_split_steps[j];
    cout << endl;
  }

  cout << endl;
  return 1;
}
#endif

int program_analysis_test_index_switch(CSageCodeGen *codegen, void *pTopFunc,
                                       const CInputOptions &options) {
  size_t i;
  size_t j;
  printf("**** Test Array Index Switch**** \n");

  void *pMain = codegen->GetFuncDeclByName("main");
  void *pMainBody = codegen->GetFuncBody(pMain);

  vector<void *> vec_array;
  for (i = 0; i < static_cast<size_t>(codegen->GetChildStmtNum(pMainBody));
       i++) {
    void *stmt = codegen->GetChildStmt(pMainBody, i);

    if (codegen->IsBasicBlock(stmt) != 0) {
      for (j = 0; j < static_cast<size_t>(codegen->GetChildStmtNum(stmt));
           j++) {
        void *sub_stmt = codegen->GetChildStmt(stmt, j);
        if (codegen->IsExprStatement(sub_stmt) != 0) {
          void *sub_exp = codegen->GetExprFromStmt(sub_stmt);
          if (codegen->IsVarRefExp(sub_exp) != 0) {
            vec_array.push_back(codegen->GetVariableInitializedName(sub_exp));
          }
        }
      }
    }
  }

  vector<void *> vecLoops;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgForStatement", &vecLoops);

  //  FIXME
  vector<int> vec_factor;
  vec_factor.push_back(4);
  vec_factor.push_back(1);
  vec_factor.push_back(2);

  for (i = 0; i < vecLoops.size(); i++) {
    void *loop = vecLoops[i];
    if (codegen->is_innermost_for_loop(loop) == 0) {
      continue;
    }

    for (j = 0; j < vec_array.size(); j++) {
      index_switch_transform(codegen, vec_array[j], loop, vec_factor);
    }
  }
  return 1;
}

int program_analysis_test_flatten(CSageCodeGen *codegen, void *pTopFunc,
                                  const CInputOptions &options) {
  printf("**** Test Loop Flatten **** \n");

  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);

  loop_flatten_for_pipeline(codegen, pTopFunc, options, &mars_ir);
  return 1;
}

int program_analysis_test_is_write(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  printf("**** Test Is Write **** \n");
  void *pMain = codegen->GetFuncDeclByName("main");
  void *pMainBody = codegen->GetFuncBody(pMain);

  vector<void *> vec_ref;
  codegen->GetNodesByType(pMainBody, "preorder", "SgVarRefExp", &vec_ref);

  for (size_t i = 0; i < vec_ref.size(); i++) {
    void *ref = vec_ref[i];
    if ("a" != codegen->UnparseToString(ref) &&
        "b" != codegen->UnparseToString(ref)) {
      continue;
    }

    int ret = codegen->array_ref_is_write_conservative(ref);

    cout << ret << " : " << codegen->UnparseToString(ref) << " @ "
         << codegen->_p(codegen->TraceUpToStatement(ref)) << endl;
  }

  return 1;
}

int program_analysis_test_loop_order(CSageCodeGen *codegen, void *pTopFunc,
                                     const CInputOptions &options) {
  printf("**** Test Loop Order Vector **** \n");
  void *pMain = codegen->GetFuncDeclByName("main");
  void *pMainBody = codegen->GetFuncBody(pMain);

  vector<void *> vec_loops;
  codegen->GetNodesByType(pMainBody, "preorder", "SgForStatement", &vec_loops);

  for (size_t i = 0; i < vec_loops.size(); i++) {
    void *loop = vec_loops[i];

    vector<int> order = codegen->get_loop_order_vector_in_function(loop);

    string sFileName = getUserCodeFileLocation(codegen, loop);

    cout << sFileName << ": (";
    for (size_t j = 0; j < order.size(); j++) {
      cout << order[j] << " ";
    }
    cout << ")" << codegen->_p(loop) << endl;
  }

  return 1;
}

int program_analysis_test_recursive_function(CSageCodeGen *codegen,
                                             void *pTopFunc,
                                             const CInputOptions &options) {
  vector<void *> vec_funcs;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgFunctionDeclaration",
                          &vec_funcs);

  for (auto &func : vec_funcs) {
    string func_name = codegen->GetFuncName(func);
    if (func_name.find("__") == 0) {
      continue;
    }
    int nLine = codegen->get_line(func);

    cout << nLine << ": (";
    void *rec_func = nullptr;
    cout << (codegen->IsRecursiveFunction(func, &rec_func) != 0 ? "recursive"
                                                                : "normal");
    cout << ")" << codegen->_p(func) << endl;
  }

  return 1;
}

int program_analysis_test_recursive_type(CSageCodeGen *codegen, void *pTopFunc,
                                         const CInputOptions &options) {
  vector<void *> vec_types;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgClassDeclaration",
                          &vec_types);

  for (auto type : vec_types) {
    SgClassDeclaration *sg_class =
        isSgClassDeclaration(static_cast<SgNode *>(type));
    if (sg_class == nullptr) {
      continue;
    }
    string class_name = codegen->GetTypeName(sg_class, false);
    SgType *sg_type = sg_class->get_type();
    int nLine = codegen->get_line(sg_class);
    cout << nLine << ": (";
    void *rec_type = nullptr;
    cout << (codegen->IsRecursiveType(sg_type, &rec_type) != 0 ? "recursive"
                                                               : "normal");
    cout << ") " << class_name << endl;
  }

  return 1;
}

#if 0
int program_analysis_test_struct_type(CSageCodeGen *codegen, void *pTopFunc,
                                      const CInputOptions &options) {
  printf("**** Test Struct Type **** \n");
  vector<void *> vec_class;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgClassDeclaration",
                         &vec_class);

  for (size_t j = 0; j < vec_class.size(); j++) {
    SgClassDeclaration *sg_var =
      isSgClassDeclaration(static_cast<SgNode *>(vec_class[j]));
    if (!sg_var || !sg_var->get_isUnNamed())
      continue;
    sg_var->set_name("__merlin_struct_" + my_itoa(j));
    sg_var->set_isUnNamed(false);
  }
  return 1;
}

int program_analysis_test_statement_update(CSageCodeGen *codegen,
                                           void *pTopFunc,
                                           const CInputOptions &options) {
  printf("**** Test Statement Update **** \n");
  vector<void *> vec_class;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgClassDeclaration",
                         &vec_class);

  for (size_t j = 0; j < vec_class.size(); j++) {
    codegen->RemoveStmt(vec_class[j], true);
  }

  vector<void *> vec_variable;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgVariableDeclaration",
                         &vec_variable);

  for (size_t j = 0; j < vec_variable.size(); j++) {
    void *orig_variable = vec_variable[j];
    void *orig_var_init = codegen->GetVariableInitializedName(orig_variable);
    void *scope = codegen->TraceUpToScope(orig_variable);
    void *new_variable = codegen->CreateVariableDecl(
        codegen->GetTypebyVar(orig_var_init),
        codegen->GetVariableName(orig_var_init) + "_new", nullptr, scope);
    codegen->ReplaceStmt(orig_variable, new_variable);
  }

  vector<void *> vec_pragma;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
                         &vec_pragma);

  for (size_t j = 0; j < vec_pragma.size(); j++) {
    codegen->RemoveStmt(vec_pragma[j]);
  }

  return 1;
}
#endif

int program_analysis_test_split_assign(CSageCodeGen *codegen, void *pTopFunc,
                                       const CInputOptions &options) {
  printf("**** Test split assign expression **** \n");

  codegen->SplitMultiAssign(pTopFunc);

  return 1;
}

int program_analysis_test_split_exp(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &options) {
  printf("**** Test split expression **** \n");

  vector<void *> vec_exp;
  codegen->GetNodesByType(nullptr, "postorder", "SgOrOp", &vec_exp);
  for (auto exp : vec_exp) {
    codegen->splitExpression(exp);
  }

  codegen->GetNodesByType(nullptr, "postorder", "SgAndOp", &vec_exp);
  for (auto exp : vec_exp) {
    codegen->splitExpression(exp);
  }

  codegen->GetNodesByType(nullptr, "preorder", "SgConditionalExp", &vec_exp);
  for (auto exp : vec_exp) {
    codegen->splitExpression(exp);
  }

  return 1;
}

int program_analysis_test_call_path(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &options) {
  cout << endl << "**** Test call path ****" << endl;
  vector<void *> vec_scopes;
  codegen->GetNodesByType_compatible(pTopFunc, "SgScopeStatement", &vec_scopes);
  for (auto scope : vec_scopes) {
    list<t_func_call_path> list_call_path;
    codegen->get_all_func_path_in_scope(scope, &list_call_path);
    for (auto call_path : list_call_path) {
      cout << codegen->print_func_path(call_path);
    }
  }
  return 1;
}

void program_analysis_test_ast_dump(CMarsAST_IF *cg, void *pTopFunc,
                                    const CInputOptions &options) {
  for (auto global : cg->GetGlobals()) {
    cerr << "first statement: ";
    cerr << cg->_p(cg->GetFirstInsertPosInGlobal(global)) << endl;
  }
  SgProject &root = *static_cast<SgProject *>(cg->GetProject());
  cg->TraversePre(&root,
                  [&](SgNode *node) { cerr << cg->_p(node, 80) << "\n"; });
}

void program_analysis_test_ast_collect(CSageCodeGen *codegen, void *pTopFunc,
                                       const CInputOptions &options) {
  cout << "Test GetNodesByType()" << endl;
  vector<string> vec_ast_type{"SgExprStatement", "SgInitializer",
                              "SgDeclarationStatement"};
  for (auto ast_type : vec_ast_type) {
    vector<void *> vec_exps;
    codegen->GetNodesByType(nullptr, "preorder", ast_type, &vec_exps, true);
    cout << "number of " << ast_type << " " << vec_exps.size() << endl;
  }

  vector<int> vec_ast_type_int{V_SgExpression,
                               V_SgExprStatement,
                               V_SgStatement,
                               V_SgInitializer,
                               V_SgFunctionCallExp,
                               V_SgFunctionDeclaration,
                               V_SgTemplateFunctionDeclaration,
                               V_SgClassDeclaration,
                               V_SgTemplateClassDeclaration,
                               V_SgPragmaDeclaration,
                               V_SgScopeStatement,
                               V_SgInitializedName,
                               V_SgCompoundAssignOp,
                               V_SgDeclarationStatement};
  vector<string> vec_ast_type_str{"SgExpression",
                                  "SgExprStatement",
                                  "SgStatement",
                                  "SgInitializer",
                                  "SgFunctionCallExp",
                                  "SgFunctionDeclaration",
                                  "SgTemplateFunctionDeclaration",
                                  "SgClassDeclaration",
                                  "SgTemplateClassDeclaration",
                                  "SgPragmaDeclaration",
                                  "SgScopeStatement",
                                  "SgInitializedName",
                                  "SgCompoundAssignOp",
                                  "SgDeclarationStatement"};
  for (int i = 0; i < static_cast<int>(vec_ast_type_int.size()); ++i) {
    vector<void *> vec_exps;
    codegen->GetNodesByType_int(nullptr, "postorder", vec_ast_type_int[i],
                                &vec_exps, true);
    cout << "number of " << vec_ast_type_str[i] << " " << vec_exps.size()
         << endl;
  }
}

void program_analysis_traverse_all_nodes(CSageCodeGen *codegen, void *pTopFunc,
                                         const CInputOptions &options) {
  cout << "Test traverse_all_nodes()" << endl;
  vector<SgNode *> nodes;
  traverse_all_nodes(pTopFunc, options, &nodes);
  cout << "traverse success" << endl;
}

void program_analysis_split_inlined_agg_def(CSageCodeGen *codegen,
                                            void *pTopFunc,
                                            const CInputOptions &options) {
  cout << "Test split_inliend_agg_def()" << endl;
  codegen->splitInlinedAggregatedDefinition(pTopFunc);
}

void program_analysis_sizeof_ap_int(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &options) {
  cout << "Test sizeof ap_int" << endl;
#define cout_sizeof(x)                                                         \
  cout << "sizeof(ap_int<" << (x)                                              \
       << "> = " << codegen->get_sizeof_arbitrary_precision_integer(x)         \
       << endl;
  cout_sizeof(1);
  cout_sizeof(7);
  cout_sizeof(11);
  cout_sizeof(15);
  cout_sizeof(20);
  cout_sizeof(34);
  cout_sizeof(55);
  cout_sizeof(100);
  cout_sizeof(120);
  cout_sizeof(200);
  cout_sizeof(266);
  cout_sizeof(312);
  cout_sizeof(500);
}

void program_analysis_test_msg_report(CSageCodeGen *codegen, void *pTopFunc,
                                      const CInputOptions &options) {
  //  test exist_test
  vector<string> tmp;
  loadHistories("none_exist_passes.list", &tmp);

  string historyListLoc = options.get_option_key_value("-a", "history_list");

  //  load histories
  vector<string> historyNames;
  vector<map<string, HistoryMarker>> historyList =
      loadHistories(historyListLoc, &historyNames);
  for (auto &m : historyList) {
    for (auto &marker : m) {
      auto ret = marker.second.toString("");
      cout << ret << endl;
    }
  }

  vector<void *> nodes;
  codegen->GetNodesByType_int(nullptr, "postorder", V_SgFunctionDeclaration,
                              &nodes, true);
  for (auto n : nodes) {
    auto node = static_cast<SgNode *>(n);
    markIncludeMacro(codegen, node, "aaa", 0);
    auto filename = codegen->GetFileInfo_filename(node);
    auto line = codegen->GetFileInfo_line(node);
    FileLocation g(filename, line);
    FileLocation h(filename, line);
    if (g == h) {
    } else {
      cout << "FileLocation test error" << endl;
    }
    string topo_loc = getTopoLocation(codegen, node);
    if (topo_loc.empty()) {
      continue;
    }
    auto p = getFileLocation(codegen, topo_loc, 0);
    cout << "FileLocation:" << endl;
    auto p_fname = p.substr(p.find_last_of('/'), p.size());
    cout << p_fname << endl;
  }
}

void program_analysis_test_type_transform(CSageCodeGen *codegen, void *pTopFunc,
                                          const CInputOptions &options) {
  void *remove_modifier_func = codegen->GetFuncDeclByName("remove_modifier");
  void *replace_vars_ref = codegen->GetFuncDeclByName("replace_vars_ref");
  if (remove_modifier_func != nullptr) {
    vector<void *> vec_vars;
    codegen->GetNodesByType(remove_modifier_func, "preorder",
                            "SgInitializedName", &vec_vars);
    for (auto var : vec_vars) {
      void *type = codegen->GetTypebyVar(var);
      void *new_type = codegen->RemoveConstVolatileType(type, var);
      if (new_type != type) {
        cout << "remove modifier from " << codegen->_up(type) << " to "
             << codegen->_up(new_type) << endl;
        codegen->SetTypetoVar(var, new_type);
      }
    }
  }
  if (replace_vars_ref != nullptr) {
    vector<void *> vec_vars;
    codegen->GetNodesByType(replace_vars_ref, "preorder", "SgInitializedName",
                            &vec_vars);
    for (auto var : vec_vars) {
      void *type = codegen->GetTypebyVar(var);
      cout << "replace var refs from " << codegen->_up(type) << " to ";
      codegen->replace_var_refs_in_type(type, var);
      cout << codegen->_up(type) << endl;
    }
  }

  //  create type from string
  {
    cout << "test creating type from string " << endl;
    //  FIXME: cannot support multiple array/pointer, such as, int [10][10], int
    //  (*)[10]
    vector<string> vec_ty_str{"int [10]",   "int *",       "const double *",
                              "ap_int<12>", "ap_uint<10>", "::st",
                              "st",         "st2"};
    for (auto ty_str : vec_ty_str) {
      void *arr_ty = codegen->GetTypeByString(ty_str);
      if (arr_ty != nullptr) {
        cout << codegen->_up(arr_ty) << " (expected " << ty_str << ")" << endl;
      }
    }
  }
}

void program_analysis_test_original_node(CSageCodeGen *codegen, void *pTopFunc,
                                         const CInputOptions &options) {
  vector<void *> vec_nodes;
  codegen->GetNodesByType_compatible(nullptr, "SgStatement", &vec_nodes);
  for (auto node : vec_nodes) {
    if (!codegen->IsCompilerGenerated(node) || codegen->_up(node).empty()) {
      continue;
    }
    void *orig_node = codegen->GetOriginalNode(node);
    cout << codegen->_p(node) << " original location "
         << codegen->get_file(orig_node) << ":" << codegen->get_line(orig_node)
         << endl;
  }
}

int program_analysis_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  printf("Hello Program Analysis ... \n");

  //  In the normal flow, the fixing is done in a standalone pass
  //  Here we do not have the assumption, so do it explicitly
  codegen->fix_defuse_issues();

  for (int i = 0; i < options.get_option_num("-a"); i++) {
    string test_flag = options.get_option("-a", i);

    if ("test_var_type" == test_flag) {
      program_analysis_test_var_type(codegen, pTopFunc, options);
    }

    if ("test_var_ref" == test_flag) {
      program_analysis_test_var_ref(codegen, pTopFunc, options);
    }

    if ("test_range" == test_flag) {
      program_analysis_test_var_range(codegen, pTopFunc, options);
    }
    if ("test_assert_range" == test_flag) {
      program_analysis_test_var_range_with_assert(codegen, pTopFunc, options);
    }
    if ("test_bound_range" == test_flag) {
      program_analysis_test_var_range_with_bound(codegen, pTopFunc, options);
    }
    if ("test_exp" == test_flag) {
      program_analysis_test_expr_gen(codegen, pTopFunc, options);
    }

    if ("test_liveness" == test_flag) {
      program_analysis_test_liveness(codegen, pTopFunc, options);
    }

    if ("test_call_graph" == test_flag) {
      program_analysis_test_call_graph(codegen, pTopFunc, options);
    }

    if ("test_access" == test_flag) {
      program_analysis_test_access(codegen, pTopFunc, options);
    }

    if ("test_dependence" == test_flag) {
      program_analysis_test_dependence(codegen, pTopFunc, options);
    }

    if ("test_type_by_ref" == test_flag) {
      program_analysis_test_type_by_ref(codegen, pTopFunc, options);
    }

    if ("test_movable" == test_flag) {
      program_analysis_test_movable(codegen, pTopFunc, options);
    }

    if ("test_index_switch" == test_flag) {
      program_analysis_test_index_switch(codegen, pTopFunc, options);
    }

    if ("test_flatten" == test_flag) {
      program_analysis_test_flatten(codegen, pTopFunc, options);
    }

    if ("test_is_write" == test_flag) {
      program_analysis_test_is_write(codegen, pTopFunc, options);
    }

    if ("test_loop_order" == test_flag) {
      program_analysis_test_loop_order(codegen, pTopFunc, options);
    }
    if ("test_recursive_func" == test_flag) {
      program_analysis_test_recursive_function(codegen, pTopFunc, options);
    }
    if ("test_recursive_type" == test_flag) {
      program_analysis_test_recursive_type(codegen, pTopFunc, options);
    }
#if 0
    if ("test_struct_type" == test_flag) {
      program_analysis_test_struct_type(codegen, pTopFunc, options);
    }
    if ("test_statement_update" == test_flag) {
      program_analysis_test_statement_update(codegen, pTopFunc, options);
    }
#endif
    if ("test_split_assign" == test_flag) {
      program_analysis_test_split_assign(codegen, pTopFunc, options);
    }

    if ("test_split_exp" == test_flag) {
      program_analysis_test_split_exp(codegen, pTopFunc, options);
    }

    if ("test_call_path" == test_flag) {
      program_analysis_test_call_path(codegen, pTopFunc, options);
    }

    if ("ast_dump" == test_flag) {
      program_analysis_test_ast_dump(codegen, pTopFunc, options);
    }
    if ("test_ast_collect" == test_flag) {
      program_analysis_test_ast_collect(codegen, pTopFunc, options);
    }
    if ("test_msg_report" == test_flag) {
      program_analysis_test_msg_report(codegen, pTopFunc, options);
    }
    if ("test_type_transform" == test_flag) {
      program_analysis_test_type_transform(codegen, pTopFunc, options);
    }
    if ("test_original_node" == test_flag) {
      program_analysis_test_original_node(codegen, pTopFunc, options);
    }
    if ("test_traverse_all_nodes" == test_flag) {
      program_analysis_traverse_all_nodes(codegen, pTopFunc, options);
    }
    if ("test_split_inlined_agg_def" == test_flag) {
      program_analysis_split_inlined_agg_def(codegen, pTopFunc, options);
    }
    if ("test_sizeof_ap_int" == test_flag) {
      program_analysis_sizeof_ap_int(codegen, pTopFunc, options);
    }
    printf("\n");
  }
  return 0;
}
