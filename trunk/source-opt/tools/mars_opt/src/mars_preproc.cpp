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


#include <fstream>
#include <iostream>
#include <regex>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"

#include "tldm_annotate.h"

#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "program_analysis.h"
using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;

#define USED_CODE_IN_COVERAGE_TEST 0
//  extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
void GetTLDMInfo_withPointer4(void *sg_node, void *pArg) {
  CSageCodeGen sggen;
  string str = sggen.GetPragmaString(sg_node);

  if (!str.empty()) {
    void *sg_pragma_decl = sg_node;  //  sggen.GetParent(sg_node);
    vector<pair<void *, string>> *pPragmaList =
        static_cast<vector<pair<void *, string>> *>(pArg);
    pPragmaList->push_back(pair<void *, string>(sg_pragma_decl, str));
  }
}

#if USED_CODE_IN_COVERAGE_TEST
vector<string> get_port_link_attribute(CSageCodeGen *codegen,
                                       void *sg_func_decl, void *sg_arg) {
  vector<string> null_attribute;
  printf("Enter HasLink!\n");
  vector<string> port_attribute;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(sg_func_decl, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  size_t i;
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    port_attribute.clear();
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    if (("ACCEL" != sFilter) || ("interface" != sCmd))
      continue;
    string string_arg = codegen->UnparseToString(sg_arg);
    printf("get attribute pragma = %s\n", vecTldmPragmas[i].second.c_str());
    //  printf("get attribute sg_arg = %s\n", string_arg.c_str());

    string interface_type;
    string variable_name;
    string link_depth;
    string max_depth;
    string buffer_id;
    string bank;
    if (mapParams["type"].first.size() > 0) {
      interface_type = mapParams["type"].first[0];
      port_attribute.push_back("bus_e1");
    } else {
      interface_type = "globalmemory";
      port_attribute.push_back("bus_e1");
    }
    if (mapParams["variable"].first.size() > 0) {
      variable_name = mapParams["variable"].first[0];
      port_attribute.push_back(variable_name);
    } else {
      return null_attribute;
    }
    if (mapParams["depth"].first.size() > 0) {
      link_depth = mapParams["depth"].first[0];
      port_attribute.push_back(link_depth);
    } else {
      return null_attribute;
    }
    if (mapParams["max_depth"].first.size() > 0) {
      max_depth = mapParams["max_depth"].first[0];
      port_attribute.push_back(max_depth);
    } else {
      port_attribute.push_back(link_depth);
    }
    if (mapParams["buffer_id"].first.size() > 0) {
      buffer_id = mapParams["buffer_id"].first[0];
      port_attribute.push_back(buffer_id);
    } else {
      port_attribute.push_back("");
    }
    if (mapParams["bank"].first.size() > 0) {
      bank = mapParams["bank"].first[0];
      port_attribute.push_back(bank);
    } else {
      port_attribute.push_back("NULL");
    }
    if (variable_name == string_arg && interface_type == "globalmemory") {
      //  printf("get atrribute of link for %s!\n", string_arg.c_str());
      //  printf("type = %s\n", interface_type.c_str());
      //  printf("variable = %s\n", variable_name.c_str());
      //  printf("depth = %s\n", link_depth.c_str());
      //  printf("max_depth = %s\n", max_depth.c_str());
      //  printf("buffer_id = %s\n", buffer_id.c_str());
      return port_attribute;
    }
  }
  return null_attribute;
}

vector<string> get_port_attribute(CSageCodeGen *codegen, void *pTopFunc,
                                  void *param, void *func_call,
                                  vector<pair<void *, string>> vecTldmPragmas) {
  vector<string> null_attribute;
  printf("Enter get_port_attribute...\n");
  vector<string> port_attribute;
  int is_sharememory = 0;
  int is_fifo = 0;
  int is_variable = 0;
  for (size_t j = 0; j < vecTldmPragmas.size(); j++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[j].second, &sFilter, &sCmd,
                            &mapParams);
    if (("ACCEL" != sFilter) || ("interface" != sCmd))
      continue;
    //  printf("\n----------pragma = %s\n",vecTldmPragmas[j].second.c_str());
    void *pTaskPragma = vecTldmPragmas[j].first;
    vector<void *> pFuncCall = codegen->TraceDownToFuncCall(pTaskPragma);
    //  printf("----------func_call_in = %s\n",
    //  codegen->UnparseToString(func_call).c_str());
    //  printf("----------func_call_det = %s\n",
    //  codegen->UnparseToString(pFuncCall).c_str());
    is_variable = 0;
    is_sharememory = 0;
    is_fifo = 0;
    for (size_t i = 0; i < pFuncCall.size(); i++) {
      if (pFuncCall[i] == func_call) {
        port_attribute.clear();
        string pragma = vecTldmPragmas[j].second;
        string param_s = codegen->UnparseToString(param);

        //  printf("----------pragma = %s\n",vecTldmPragmas[j].second.c_str());
        //  printf("----------matched func_call = %s\n",
        //  codegen->UnparseToString(pFuncCall[i]).c_str());
        //  printf("----------param = %s\n", param_s.c_str());
        //  printf("----------interface variale = %s\n",
        //  mapParams["variable"].first[0].c_str());

        string interface_type;
        if (mapParams["type"].first.size() > 0) {
          interface_type = mapParams["type"].first[0];
          if (interface_type == "sharememory") {
            is_sharememory = 1;
          } else if (interface_type == "fifo") {
            is_fifo = 1;
            interface_type = "fifo_v1";
          }
          port_attribute.push_back(interface_type);
        } else {
          //  printf("\n[mars_opt] ERROR: interface type is not specified:
          //  %s\n\n",
          //       vecTldmPragmas[j].second.c_str());
          //  string msg = "Finding an invalid interface pragma \"" +
          //  vecTldmPragmas[j].second; msg += "\" whose interface type is not
          //  specified"; dump_critical_message("PROCS", "ERROR", msg, 301, 1);
          //  exit(1);
        }
        string interface_variable;
        if (mapParams["variable"].first.size() > 0) {
          interface_variable = mapParams["variable"].first[0];
          port_attribute.push_back(interface_variable);
          if (param_s == interface_variable) {
            is_variable = 1;
          }
        } else {
          //  printf(
          //    "\n[mars_opt] ERROR: interface variable is not specified:
          //    %s\n\n", vecTldmPragmas[j].second.c_str());
          string msg = "Finding an invalid interface pragma \"" +
                       vecTldmPragmas[j].second;
          msg += "\" whose interface variable is not specified";
          dump_critical_message("PROCS", "ERROR", msg, 302, 1);
          exit(1);
        }
        string interface_depth;
        if (mapParams["depth"].first.size() > 0) {
          interface_depth = mapParams["depth"].first[0];
          port_attribute.push_back(interface_depth);
        } else {
          string msg = "Finding an invalid interface pragma \"" +
                       vecTldmPragmas[j].second;
          msg += "\" whose interface depth is not specified";
          dump_critical_message("PROCS", "WARNING", msg, 201, 1);
          //  printf("\n[mars_opt] WARNING: interface depth is not specified:
          //  %s\n\n",
          //       vecTldmPragmas[j].second.c_str());
          //  exit(1);
        }
        if (is_sharememory && is_variable)
          break;
        if (is_fifo && is_variable)
          break;
      }
    }
    if (is_sharememory && is_variable)
      break;
    if (is_fifo && is_variable)
      break;
  }

  if (is_sharememory && is_variable) {
    return port_attribute;
  } else if (is_fifo && is_variable) {
    return port_attribute;
  } else {
    return null_attribute;
  }
  return null_attribute;
}

int mars_gen_preproc(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options) {
  size_t i;
  //  FIXME temprory move kernrl pragma to task pragma, will delete later
  vector<pair<void *, string>> vecTldmPragmas_tmp;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas_tmp);
  for (i = 0; i < vecTldmPragmas_tmp.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    string sOrgPragma = vecTldmPragmas_tmp[i].second;
    tldm_pragma_parse_whole(vecTldmPragmas_tmp[i].second, &sFilter, &sCmd,
                            &mapParams);
    if (!is_cmost_pragma(sFilter) || ("kernel" != sCmd))
      continue;
    void *org_pragma = vecTldmPragmas_tmp[i].first;
    void *next_stmt = codegen->GetNextStmt(org_pragma);
    void *call = codegen->GetFuncCallInStmt(next_stmt);
    string kernel_name = codegen->GetFuncNameByCall(call);
    string pragma_task = "ACCEL task name=" + kernel_name;
    string die_name = "NULL";
    if (mapParams["die"].first.size() > 0) {
      pragma_task += " die=" + mapParams["die"].first[0];
      die_name = mapParams["die"].first[0];
    }
    //  sOrgPragma.erase(0, 12);
    //  sOrgPragma = "ACCEL task " + sOrgPragma;
    //  printf("org pragma = %s\n", sOrgPragma.c_str());
    void *new_pragma =
        codegen->CreatePragma(pragma_task, codegen->GetScope(org_pragma));
    codegen->ReplaceStmt(org_pragma, new_pragma);
  }

  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    string sOrgPragma = vecTldmPragmas[i].second;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    //  if (!is_cmost_pragma(sFilter)) return 0;
    if (!is_cmost_pragma(sFilter) || ("task" != sCmd))
      continue;

    //  string sComponentName;
    //  if (mapParams["name"].first.size() > 0)
    //  {
    //    sComponentName = mapParams["name"].first[0];
    //  }
    //  else
    //  {
    //    printf("\n[mars_opt] ERROR: task name is not specified: %s\n\n",
    //    vecTldmPragmas[i].second.c_str());
    //    exit(1);
    //  }

    string sAdditional = "";
    {
      map<string, pair<vector<string>, vector<string>>>::iterator it;
      for (it = mapParams.begin(); it != mapParams.end(); it++) {
        //  if (it->first != "name")

        string attr = it->first;

        sAdditional += " " + attr;
        if (!it->second.first.empty())
          sAdditional += "=\"" + it->second.first[0] + "\" ";

#if 0
        {
          sAdditional += " " + it->first + "=\"";
          for (j = 0; j < it->second.first.size(); j++) {
            if (j != 0)
              sAdditional += ",";
            sAdditional += it->second.first[j];
          }
          //  for (j = 0; j < it->second.second.size(); j++) {
          //    if (j != 0)
          //        sAdditional += ";";
          //    else
          //        sAdditional += ":";
          //    sAdditional += it->second.second[j];
          //  }
          sAdditional += "\" ";
        }
#endif
      }
    }
    //  sAdditional = "asdf";

    void *pTaskPragma = vecTldmPragmas[i].first;

    //  one_region_to_basicblock(codegen, sComponentName, pTaskBegin, "task",
    //  sAdditional);

    //  1 Check function call
    //  1.1 Check if the following statement is a functionc call (without return
    //  value)
    void *pFuncCall = codegen->find_kernel_call(pTaskPragma);
    void *task_stmt = codegen->TraceUpToStatement(pFuncCall);
    if (!pFuncCall) {
      string sFileName = codegen->GetFileInfo_filename(pFuncCall);
      printf("\n[mars_opt] ERROR: Statement after the task pragma is not a "
             "proper function call: \n%s, line %d:\n%s\n%s\n\n",
             sFileName.c_str(), codegen->GetFileInfo_line(pTaskPragma),
             codegen->UnparseToString(pTaskPragma).c_str(),
             codegen->UnparseToString(pFuncCall).c_str());
      exit(1);
    }

    //  1.2 check function name
    string sFuncName = codegen->GetFuncNameByCall(pFuncCall);
    //  if (sFuncName != sComponentName + "_kernel")
    {
        //  ZP: 20151012
        //  string sFileName = codegen->GetFileInfo_filename(pFuncCall);
        //  string sExample = "E.g. for task \"abc\", the kernel function name
        //  should be \"abc_kernel\"";
        //  printf("\n[mars_opt] ERROR: Function name does not match with task
        //  pragma: \n%s\n\n%s, line %d:\n%s\n%s\n\n",
        //     sExample.c_str(),
        //     sFileName.c_str(), codegen->GetFileInfo_line(pTaskPragma),
        //     codegen->UnparseToString(pTaskPragma).c_str(),
        //     codegen->UnparseToString(pFuncCall).c_str());
        //  exit(1);
    }

    //  2. Process the pragma
    //  2.1 Insert graph_begin/end
    {
      void *pGraphBegin = codegen->CreatePragma("cmost graph_begin",
                                                codegen->GetScope(pFuncCall));
      void *pGraphEnd = codegen->CreatePragma("cmost graph_end",
                                              codegen->GetScope(pFuncCall));
      codegen->InsertStmt(pGraphBegin, task_stmt);
      codegen->InsertAfterStmt(pGraphEnd, task_stmt);
    }

    //  2.2 Insert task_begin/end
    {
      void *pTaskBegin = codegen->CreatePragma("cmost task_begin" + sAdditional,
                                               codegen->GetScope(pFuncCall));
      void *pTaskEnd =
          codegen->CreatePragma("cmost task_end", codegen->GetScope(pFuncCall));
      codegen->InsertStmt(pTaskBegin, task_stmt);
      codegen->InsertAfterStmt(pTaskEnd, task_stmt);
    }

    //  2.3 Comment original task pragma
    {
      string sOrgPragma = codegen->UnparseToString(pTaskPragma);
      codegen->AddDirectives("//  " + sOrgPragma, task_stmt);
      codegen->RemoveStmt(pTaskPragma);
    }
  }

  return 1;
}

//  1. for each task pragma: the body is only a function call, and the function
//  name is matched with the task name
//  2. The kernel function name matches the file name of the kernel function
//  declaration
//  3. The arguments can only be:
//    a) scalars: char, short, int, float (double is not supported)
//    b) pointers: int *, float *, double * (all the others are not supported)
//    c) dimension: only 1-D, pointer or array
//    d) the pointer arguments must be defined in cmost_malloc();
int mars_gen_check_task_interface_format(CSageCodeGen *codegen, void *pTopFunc,
                                         CInputOptions options) {
  printf("Enter mars_gen_check_task_interface_format\n");
  //  ZP: 2015-10-21
  return 1;
#if 0
  const int CONTINUE_ON_WARNING = 1;


  map<string, void *> mapTasks;
  get_tasks_from_pragmas(codegen, pTopFunc, options, mapTasks);
  map<string, void *>::iterator it;

  //  used in step 3.d)
  vector<void *> vec_func_calls_all;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgFunctionCallExp",
      &vec_func_calls_all);
  vector<void *> vec_func_calls_cmost_malloc;
  for (int j = 0; j < vec_func_calls_all.size(); j++) {
    void *one_call = vec_func_calls_all[j];
    string func_name = codegen->GetFuncNameByCall(one_call);
    if (func_name.find("cmost_malloc") != string::npos) {
      vec_func_calls_cmost_malloc.push_back(one_call);
      //    cout << " Find : " << codegen->UnparseToString(one_call) << endl;
    }
  }

  for (it = mapTasks.begin(); it != mapTasks.end(); it++) {
    string sTaskName = it->first;
    void *task_bb = it->second;

    assert(codegen->IsBasicBlock(task_bb));

    //  1. for each task pragma: the body is only a function call, and the
    //  function name is matched with the task namef
    if (codegen->GetChildStmtNum(task_bb) != 1) {
      printf("\n[mars_opt] ERROR: Task pragma is not followed by the kernel "
          "function call (task=%s).\n\n",
          sTaskName.c_str());
      printf("    Note: this may be caused by \n");
      printf("    1. The missing of the declaration of kernel function in the "
          "calling position\n");
      printf(
          "       (Please refer to the example code to add the declaration)\n");
      printf("    2. Task pragma is not followed by a function call\n");

      string sFileName = codegen->GetFileInfo_filename(task_bb);
      int line = codegen->GetFileInfo_line(task_bb);
      printf("\n%s, line %d:\nKernel Function: %s\n\n", sFileName.c_str(), line,
          sTaskName.c_str());

      if (!CONTINUE_ON_WARNING)
        exit(1);
    }
    void *task_body = codegen->GetChildStmt(task_bb, 0);
    void *func_call = codegen->GetExprFromStmt(task_body);
    if (!func_call || !codegen->IsFunctionCall(func_call)) {
      printf("\n[mars_opt] ERROR: Task pragma is not followed by the kernel "
          "function call (task=%s).\n\n",
          sTaskName.c_str());
      printf("    Note: this may be caused by \n");
      printf("    1. The missing of the declaration of kernel function in the "
          "calling position\n");
      printf(
          "       (Please refer to the example code to add the declaration)\n");
      printf("    2. Task pragma is not followed by a function call\n");

      string sFileName = codegen->GetFileInfo_filename(task_bb);
      int line = codegen->GetFileInfo_line(task_bb);
      printf("\n%s, line %d:\nKernel Function: %s\n\n", sFileName.c_str(), line,
          sTaskName.c_str());

      if (!CONTINUE_ON_WARNING)
        exit(1);
    }

    //  ZP: 20150513, it has been checked in previous pass (mars_gen_preproc)
    //  string func_call_name = codegen->GetFuncNameByCall(func_call);
    //  if (func_call_name != sTaskName + "_kernel")
    //  {
    //    printf("\n[mars_opt] ERROR: Task function name does not match the task
    //    name (task=%s).\n\n", sTaskName.c_str());
    //    exit(1);
    //  }

    //  2. The kernel function name matches the file name of the kernel function
    //  declaration
    void *func_decl = codegen->GetFuncDeclByCall(func_call);
    if (!func_decl) {
      printf("\n[mars_opt] ERROR: Kernel function body is not found "
          "(task=%s).\n\n",
          sTaskName.c_str());
      exit(1);
      if (!CONTINUE_ON_WARNING)
        exit(1);
    }

    string func_decl_path_name = codegen->GetFileInfo_filename(func_decl);
    vector<string> segment = str_split(func_decl_path_name, '/');
    string sFuncDeclName = codegen->GetFuncName(func_decl);
    if (segment[segment.size() - 1] != sFuncDeclName + ".c" &&
        segment[segment.size() - 1] != sFuncDeclName + ".cpp") {
      printf("\n[mars_opt] ERROR: Kernel source file name does not match "
          "kernel function name.\n");
      printf("    E.g. for kernel function \"abc_kernel(...)\", its source "
          "file should be \"abc_kernel.c\"\n");
      printf("    Please also note all the sub-functions called by the kernel "
          "function should be in \"abc_kernel.c\"\n");

      string sFileName = codegen->GetFileInfo_filename(func_decl);
      int line = codegen->GetFileInfo_line(func_decl);
      printf("\n%s, line %d:\nKernel Function: %s\n\n", sFileName.c_str(), line,
          sFuncDeclName.c_str());
      if (!CONTINUE_ON_WARNING)
        exit(1);
    }

    //  3. The arguments can only be:
    //    a) scalars: char, short, int, float (double is not supported)
    //    b) pointers: int *, float *, double * (all the others are not
    //    supported)
    //    c) dimension: only 1-D, pointer or array
    //    d) the pointer arguments must be defined in cmost_malloc();
    for (int i = 0; i < codegen->GetFuncParamNum(func_decl); i++) {
      void *curr_arg = codegen->GetFuncParam(func_decl, i);
      void *curr_param = codegen->GetFuncCallParam(func_call, i);
      if (codegen->IsCastExp(curr_param))
        curr_param = codegen->GetExpUniOperand(curr_param);
#if PROJDEBUG
      cout << "[INFO] Port " << sFuncDeclName << ": "
        //                << ((SgNode*)curr_arg)->class_name()  << " "
        << codegen->UnparseToString(codegen->GetTypebyVar(curr_arg)) << " "
        << codegen->UnparseToString(curr_arg) << endl;
#endif
      void *sg_type = codegen->GetTypebyVar(curr_arg);
      int argument_err = 0;
      if (codegen->IsPointerType(sg_type) || codegen->IsArrayType(sg_type)) {
        void *basic_type;
        vector<size_t> vec_dimsize;
        int dim_num = codegen->get_type_dimension(sg_type, basic_type,
            vec_dimsize, curr_arg);

        string sTypeName = codegen->UnparseToString(basic_type);

        if (dim_num != 1) {
          argument_err = 1;
        } else if (sTypeName != "float" && sTypeName != "int" &&
            sTypeName != "double") {
          argument_err = 2;
        } else {
          //  try to match the cmost_malloc functions
          //  step 3.d)
          int find_the_malloc = 0;
          for (int j = 0; j < vec_func_calls_cmost_malloc.size(); j++) {
            void *one_call = vec_func_calls_cmost_malloc[j];
            //                        string func_name =
            //                        codegen->GetFuncNameByCall(one_call);
            //                        if (func_name.find("cmost_malloc") == -1)
            //                        continue;
            void *first_arg = codegen->GetFuncCallParam(one_call, 0);
            if (codegen->IsCastExp(first_arg)) {
              first_arg = codegen->GetExpUniOperand(first_arg);
            }
            if (codegen->IsAddressOfOp(first_arg)) {
              first_arg = codegen->GetExpUniOperand(first_arg);
            }

            //  ZP: FIXME: We only use the name compare here,
            //  which is not stable
            //  To be change to variable trace
            if (codegen->UnparseToString(first_arg) ==
                codegen->UnparseToString(curr_param)) {
#if PROJDEBUG
              cout << "++" << codegen->UnparseToString(curr_param) << "++"
                << codegen->UnparseToString(first_arg) << "++"
                << codegen->UnparseToString(one_call) << endl;
#endif
              void *sg_name = codegen->GetVariableInitializedName(curr_param);

              //  ////////////////////////////////////////////////////  /
              sg_name = codegen->TraceVariableInitializedName(sg_name);
              //  ////////////////////////////////////////////////////  /

              if (sg_name &&
                  codegen->GetVariableInitializedName(first_arg) == sg_name) {
                find_the_malloc = 1;
                cout << "  Find cmost_malloc() " << endl;
              }
            }
          }  //  end of vec_func_call loop

          if (find_the_malloc == 0) {
            printf("\n[mars_opt] ERROR: Missing cmost_malloc() call for the "
                "Kernel pointer arguments.\n");
            printf("    E.g. void acc_kernel(int * port_var);\n");
            printf("         int * port_var; \n");
            printf("         cmost_malloc(&port_var, SIZE_OF_BYTE);\n");
            printf("         acc_kernel(port_var)\n");

            string sFileName = codegen->GetFileInfo_filename(func_call);
            int line = codegen->GetFileInfo_line(func_call);
            printf(
              "\n%s, line %d:\n  Kernel Function: %s\n  Argument: %s %s\n\n",
              sFileName.c_str(), line, sFuncDeclName.c_str(),
              codegen->UnparseToString(
                codegen->GetTypebyVar(curr_arg)).c_str(),
              codegen->UnparseToString(curr_arg).c_str());

            if (!CONTINUE_ON_WARNING)
              exit(1);
          }
        }
      } else {
        string sTypeName = codegen->UnparseToString(sg_type);
        if (sTypeName != "float" && sTypeName != "int")
          argument_err = 3;
      }

      if (argument_err != 0) {
        printf("\n[mars_opt] ERROR: Kernel Argument can only be scalar "
            "(<=32bit) or 1-D array/pointer (32/64bit).\n");
        printf("    E.g. int a[100]; int * a; float a; are allowed\n");
        printf(
            "         int a[100][100]; char * a; double a; are not allowed\n");

        string sFileName = codegen->GetFileInfo_filename(func_decl);
        int line = codegen->GetFileInfo_line(func_decl);
        printf("\n%s, line %d:\n  Kernel Function: %s\n  Argument: %s %s\n\n",
            sFileName.c_str(), line, sFuncDeclName.c_str(),
            codegen->UnparseToString(codegen->GetTypebyVar(curr_arg)).c_str(),
            codegen->UnparseToString(curr_arg).c_str());

        if (argument_err == 3) {
          printf("Note: scalar \"double\" is not supported. A workaround is "
              "changing to \"double *\"\n");
        }
        if (!CONTINUE_ON_WARNING)
          exit(1);
      }
    }
  }
#endif
}

void collect_tldm_info(CSageCodeGen *codegen, void *pTopFunc,
                       const vector<pair<void *, string>> &vecTldmPragmas,
                       map<void *, string> *mapTaskName,
                       map<void *, string> *mapTaskSyncMode,
                       map<void *, string> *mapTaskDevice,
                       map<void *, string> *mapTask2Additional,
                       map<void *, string> *mapTask2GraphName,
                       map<void *, void *> *mapTask2Graph,
                       map<void *, string> *mapGraph2GraphName) {
  int curr_task_id = -1;
  map<string, int> mapGraph2ID;
  int curr_graph_id = 0;
  for (size_t i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    //  if (!is_cmost_pragma(sFilter)) return 0;
    //  if (("tldm" != sFilter && "cmost" != sFilter) || ("task_block" != sCmd))
    //  continue;
    if (!is_cmost_pragma(sFilter) || ("task_block" != sCmd))
      continue;

    curr_task_id++;
    string sTaskName;
    if (mapParams["name"].first.size() > 0) {
      sTaskName = mapParams["name"].first[0];
    } else {
      static int task_id = 0;
      sTaskName = "unnamed_task" + my_itoa(task_id++);
    }
    void *sg_pragma = vecTldmPragmas[i].first;
    void *task_bb = codegen->GetNextStmt(sg_pragma);
    assert(codegen->IsBasicBlock(task_bb));

    string additional, graph_name = "undefined";
    void *graph_bb = get_surrounding_graph_from_task(
        task_bb, codegen, sTaskName, graph_name, additional);

    //  ///////////////////////////////////////////////////////////////////////
    //  / ZP: 2014-01-24
    if (graph_bb) {
      (*mapTask2Additional)[graph_bb] = additional;
    }
    //  ///////////////////////////////////////////////////////////////////////
    //  /

    if (mapGraph2ID.find(graph_name) == mapGraph2ID.end()) {
      mapGraph2ID[graph_name] = curr_graph_id++;
    }

    //  printf("Han sTaskName = %s, graph_name = %s\n", sTaskName.c_str(),
    //  graph_name.c_str());
    assert(codegen->IsBasicBlock(graph_bb));
    (*mapTask2Graph)[task_bb] = graph_bb;
    (*mapTaskName)[task_bb] = sTaskName;
    (*mapTask2GraphName)[task_bb] = graph_name;

    (*mapGraph2GraphName)[graph_bb] = graph_name;
    graph_names.insert(graph_name);

    //  get additional annotation for task
    string sAdditional;
    {
      printf("device = %s\n", (*mapTaskDevice)[task_bb].c_str());
      map<string, pair<vector<string>, vector<string>>>::iterator it;
      for (it = mapParams.begin(); it != mapParams.end(); it++) {
        if (it->first != "name") {
          if (it->first == "async" || it->first == "nonblocking" ||
              it->first == "non-blocking") {
            (*mapTaskSyncMode)[task_bb] = "1";
            sAdditional += " async=\"1\" ";
            continue;
          }

          if (it->first == "device") {
            (*mapTaskDevice)[task_bb] = it->second.first[0];
            //  sAdditional += " device=\"" + (*mapTaskDevice)[task_bb] + "\" ";
            printf("device = %s\n", (*mapTaskDevice)[task_bb].c_str());
            continue;
          }

          sAdditional += " " + it->first + "=\"";
          for (size_t j = 0; j < it->second.first.size(); j++) {
            if (j != 0)
              sAdditional += ",";
            sAdditional += it->second.first[j];
          }
          for (size_t j = 0; j < it->second.second.size(); j++) {
            if (j != 0)
              sAdditional += ";";
            else
              sAdditional += ":";
            sAdditional += it->second.second[j];
          }
          sAdditional += "\" ";
        }
      }
    }

    //  ///////////////////////////////////////////////////////////////////////
    //  /
    assert(mapGraph2ID.end() != mapGraph2ID.find(graph_name));
    (*mapTask2Additional)[task_bb] = sAdditional;
  }
}

void traverse_tldm_graph_to_generate_tldm_ir(
    CSageCodeGen *codegen, const map<void *, string> &mapGraph2GraphName,
    const map<void *, string> &mapTask2Additional) {
  for (auto it_g = mapGraph2GraphName.begin(); it_g != mapGraph2GraphName.end();
       it_g++) {
    void *sg_graph_bb = it_g->first;
    string sGraphName = it_g->second;

    vector<void *> vec_exps_graph;
    vec_exps_graph.clear();

    //  2.5 write the data structure into TLDM_IR
    tldm_polyhedral_info poly_info_graph;
    poly_info_graph.name = sGraphName;
    poly_info_graph.type = "graph";
    poly_info_graph.order_vector = "1";
    poly_info_graph.condition_vector = "1";
    poly_info_graph.iterator_vector = "";
    poly_info_graph.iterator_range = "";
    poly_info_graph.parameter_vector = "";
    poly_info_graph.parameter_range = "";

    //  store access info into poly_info_graph.properties
    poly_info_graph.properties["name"] = poly_info_graph.name;
    poly_info_graph.properties["order_vector"] = poly_info_graph.order_vector;
    poly_info_graph.properties["condition_vector"] = "1";
    poly_info_graph.properties["type"] = "graph";
    poly_info_graph.properties["iterator_vector"] =
        poly_info_graph.iterator_vector;
    poly_info_graph.properties["iterator_range"] =
        poly_info_graph.iterator_range;
    poly_info_graph.properties["parameter_vector"] =
        poly_info_graph.parameter_vector;
    poly_info_graph.properties["parameter_range"] =
        poly_info_graph.parameter_range;

    //  ZP 2014-01-24: for tldm text format output
    {
      if (mapTask2Additional.end() != mapTask2Additional.find(sg_graph_bb)) {
        poly_info_graph.properties["additional_tunning_string"] =
            mapTask2Additional.find(sg_graph_bb)->second;
      }
    }

    //  create graph task
    create_tldm_task(sGraphName, poly_info_graph);
  }
}

tldm_polyhedral_info generate_poly_info_for_task(
    CSageCodeGen *codegen, void *sg_task_bb, const string &sTaskName,
    const string &str_graph_name, const string &sKernelName,
    const map<void *, string> &mapTaskSyncMode,
    const map<void *, string> &mapTaskDevice,
    const map<void *, string> &mapTask2Additional) {
  tldm_polyhedral_info poly_info_task;
  poly_info_task.name = sTaskName;
  poly_info_task.order_vector = "1";
  poly_info_task.condition_vector = "1";
  poly_info_task.iterator_vector = "";
  poly_info_task.iterator_range = "";
  poly_info_task.type = "task";
  poly_info_task.parameter_vector = "";
  poly_info_task.parameter_range = "";
  poly_info_task.properties["name"] = poly_info_task.name;
  poly_info_task.properties["order_vector"] = poly_info_task.order_vector;
  poly_info_task.properties["condition_vector"] =
      poly_info_task.condition_vector;
  poly_info_task.properties["type"] = "task";
  poly_info_task.properties["iterator_vector"] = poly_info_task.iterator_vector;
  poly_info_task.properties["iterator_range"] = poly_info_task.iterator_range;
  poly_info_task.properties["parameter_vector"] =
      poly_info_task.parameter_vector;
  poly_info_task.properties["parameter_range"] = poly_info_task.parameter_range;
  poly_info_task.properties["graph_name"] = str_graph_name;
  poly_info_task.properties["kernel_name"] = sKernelName;
  poly_info_task.properties["async"] = mapTaskSyncMode.find(sg_task_bb)->second;
  poly_info_task.properties["device"] = mapTaskDevice.find(sg_task_bb)->second;
  {
    if (mapTask2Additional.end() != mapTask2Additional.find(sg_task_bb)) {
      poly_info_task.properties["additional_tunning_string"] =
          mapTask2Additional.find(sg_task_bb)->second;
    }
  }
  return poly_info_task;
}

tldm_polyhedral_info generate_access_tldm_polly_info(
    CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir, void *sg_task_bb,
    const string &sTaskName, int parsed_array_dim, int pointer_dim,
    const string &parsed_data_type, void *sg_arg, void *actual_arg, int arg_idx,
    void *sg_call, void *sg_func_decl,
    const vector<pair<void *, string>> &vecTldmPragmas,
    tldm_polyhedral_info poly_info_task) {
  tldm_polyhedral_info access_poly_info;
  void *arg_type = codegen->GetTypebyVar(sg_arg);
  string sDataType = codegen->UnparseToString(arg_type);
  string sArray = codegen->GetVariableName(sg_arg);
  access_poly_info.properties["port_type"] =
      (codegen->IsPointerType(arg_type) == 0 &&
       codegen->IsArrayType(arg_type) == 0 &&
       codegen->IsRestrictType(arg_type) == 0)
          ? "param"
          : "bus_e1";
  vector<string> port_attribute_s = get_port_attribute(
      codegen, pTopFunc, actual_arg, sg_call, vecTldmPragmas);
  if (mars_ir->HasLink(codegen, sg_func_decl, sg_arg, 1)) {
    //  printf("Detect link port.\n");
    port_attribute_s = get_port_link_attribute(codegen, sg_func_decl, sg_arg);
  }
  //  printf("port attribute size=%d\n", port_attribute_s.size());
  if (port_attribute_s.size() > 0 && (codegen->IsPointerType(arg_type) != 0 ||
                                      codegen->IsArrayType(arg_type) != 0)) {
    //  printf("----------org_arg=%s----------%s,%s,%s\n\n",codegen->UnparseToString(actual_arg).c_str(),port_attribute_s[0].c_str(),port_attribute_s[1].c_str(),port_attribute_s[2].c_str());
    access_poly_info.properties["port_type"] = port_attribute_s[0];
  }
  //  put all the additional annotation into TLDM-IR
  get_map_additionals(codegen, sArray, pTopFunc, access_poly_info.properties);

  //  ///////////////////////////////////////////////////////////////////////
  //  / added by zhangpeng for SW generation
  access_poly_info.properties["func_name"] = sTaskName;
  access_poly_info.properties["func_arg_idx"] = my_itoa(arg_idx);
  string str_func_arg_name = sArray;
  //  if (USE_LOWERCASE_NAME) str_func_arg_name =
  //  get_lower_case(str_func_arg_name);
  //  mapAdditional["func_arg_name"] = str_func_arg_name;
  access_poly_info.properties["func_arg_name"] = str_func_arg_name;
  //  ///////////////////////////////////////////////////////////////////////
  //  /

  access_poly_info.name = sArray;
  access_poly_info.order_vector = "1";  //  accesses[j].sOrderVec;
  access_poly_info.type = "access";
  access_poly_info.iterator_vector = "";  //  accesses[j].iter_list;
  access_poly_info.iterator_range = "";   //  accesses[j].up_list;
  access_poly_info.condition_vector = "1";
  access_poly_info.parameter_vector = "";
  access_poly_info.parameter_range = "";
  access_poly_info.access_pattern = "r:";
  access_poly_info.properties["name"] = access_poly_info.name;
  access_poly_info.properties["order_vector"] = access_poly_info.order_vector;
  access_poly_info.properties["condition_vector"] =
      access_poly_info.condition_vector;
  access_poly_info.properties["type"] = "access";
  access_poly_info.properties["iterator_vector"] =
      access_poly_info.iterator_vector;
  access_poly_info.properties["iterator_range"] =
      access_poly_info.iterator_range;
  access_poly_info.properties["parameter_vector"] =
      access_poly_info.parameter_vector;
  access_poly_info.properties["parameter_range"] =
      access_poly_info.parameter_range;
  access_poly_info.properties["access_pattern"] =
      access_poly_info.access_pattern;
  access_poly_info.properties["ref_name"] = sArray + "_0";
  access_poly_info.properties["channel_offset"] = "";

  //  CMarsRangeExpr var_range = CMarsVariable(sg_arg,
  //  &codegen).get_range(); CMarsExpression me_lb = var_range.get_lb();
  //  CMarsExpression me_ub = var_range.get_ub();

  //  if(mapArray2DimStr.count(access_poly_info.name) > 0)
  if (0) {  //  (mapArray2DimStr.count(access_poly_info.name) > 0)
    access_poly_info.properties["array_dim"] =
        mapArray2DimStr[access_poly_info.name];
  } else {
    //  access_poly_info.properties["array_dim"] = "";

    //  ZP: 20151012
    access_poly_info.properties["array_dim"] = my_itoa(parsed_array_dim);
  }
  if (port_attribute_s.size() > 2) {
    //  printf("----------org_arg=%s----------%s,%s,%s\n\n",codegen->UnparseToString(actual_arg).c_str(),port_attribute_s[0].c_str(),port_attribute_s[1].c_str(),port_attribute_s[2].c_str());
    access_poly_info.properties["array_dim"] = port_attribute_s[2];
  }
  if (port_attribute_s.size() > 3) {
    access_poly_info.properties["max_depth"] = port_attribute_s[3];
  } else {
    access_poly_info.properties["max_depth"] = my_itoa(parsed_array_dim);
  }
  if (port_attribute_s.size() > 4) {
    access_poly_info.properties["buffer_id"] = port_attribute_s[4];
  } else {
    access_poly_info.properties["buffer_id"] = "";
  }
  if (port_attribute_s.size() > 5) {
    access_poly_info.properties["bank"] = port_attribute_s[5];
  } else {
    access_poly_info.properties["bank"] = "NULL";
  }
  //  printf("array_dim =
  //  %s\n",access_poly_info.properties["array_dim"].c_str());
  //  printf("max_depth =
  //  %s\n",access_poly_info.properties["max_depth"].c_str());
  //  printf("func_arg_name =
  //  %s\n",access_poly_info.properties["func_arg_name"].c_str());

  string data_size_in_byte = "-1";
  string data_type;
  int data_bitwidth;

  //  ZP: 20160201
  int data_bitwidth_kernel;
  {
    data_bitwidth_kernel =
        codegen->get_type_size(codegen->GetBaseType(arg_type), sg_arg) * 8;

    //  SgType * sg_org_type =
    //  (SgType*)codegen->GetOrigTypeByTypedef(arg_type);
    //  string sDataType = codegen->UnparseToString(sg_org_type);
    //  data_bitwidth_kernel =
    //  codegen->get_bitwidth_from_type_str(sDataType);
    //  assert(data_bitwidth_kernel != 0);
  }

  if (mapArray2TypeName.count(access_poly_info.name) > 0) {
    string org_type = mapArray2TypeName[access_poly_info.name];
    int org_bitwidth = codegen->get_bitwidth_from_type_str(org_type);

    data_size_in_byte =
        my_itoa(codegen->get_sizeof_arbitrary_precision_integer(org_bitwidth)) +
        " * (" + access_poly_info.properties["array_dim"] + "L)";

    data_type = org_type;
    data_bitwidth = org_bitwidth;
    if (data_type.find("class ap_") != string::npos ||
        data_type.find("class ::ap_") != string::npos) {
      data_type = "interface";
    }
  } else {
    data_size_in_byte =
        my_itoa(parsed_array_dim *
                (codegen->get_sizeof_arbitrary_precision_integer(
                    codegen->get_bitwidth_from_type_str(parsed_data_type))));

    data_type = parsed_data_type;
    data_bitwidth = codegen->get_bitwidth_from_type_str(data_type);
    if (data_type.find("class ap_") != string::npos ||
        data_type.find("class ::ap_") != string::npos) {
      data_type = "interface";
    }
  }

  access_poly_info.properties["data_bitwidth"] = my_itoa(data_bitwidth);
  access_poly_info.properties["data_bitwidth_kernel"] =
      data_bitwidth_kernel != 0 ? my_itoa(data_bitwidth_kernel)
                                : ("sizeof(" + sDataType + ")");

  //  ////////////////////  /
  //  FIXME: ZP 20160602
  if (data_type.find("&") != string::npos) {
    string task_name = poly_info_task.properties["name"];
    string kernel_name = poly_info_task.properties["kernel_name"];
    string port_name = codegen->UnparseToString(actual_arg);

    string sFileName = codegen->get_file(actual_arg);
    string sFileLine = my_itoa(codegen->get_line(actual_arg));

    string msg = "Finding an unsupported interface variable type "
                 "(reference type) : \n    type=" +
                 data_type + "\n    task=" + task_name +
                 " kernel=" + kernel_name + " argument=" + port_name +
                 " \n    at " + sFileName + ":" + sFileLine + "\n";
    dump_critical_message("PROCS", "ERROR", msg, 303, 1);

#if PROJDEBUG
    cout << "[ERROR@Outline]" << msg << endl;
#endif
    exit(1);
  }
  //  ////////////////////  /

  access_poly_info.properties["data_type"] = data_type;
  access_poly_info.properties["data_size_in_byte"] = data_size_in_byte;

  access_poly_info.properties["actual_argument"] =
      codegen->UnparseToString(actual_arg);
  //  codegen->UnparseToString(arg); //  ZP: 20161128, not know why used
  //  formal argument here

  //  //  ZP: 20161128
  //  if (codegen->IsVarRefExp(actual_arg))
  //  {
  //    void * wr_init = codegen->GetVariableInitializedName(actual_arg);
  //    if (codegen->IsArgumentInitName(wr_init))
  //    {
  //        void * wr_pragma =
  //        mars_ir_v2.get_unique_wrapper_pragma(wr_init);

  //        access_poly_info.properties["actual_argument"]  =
  //            mars_ir_v2.get_pragma_attribute(wr_pragma, "variable");
  //    }
  //  }

  access_poly_info.properties["formal_argument"] =
      codegen->UnparseToString(sg_arg);
  //  2. read_range and write_range
  if (pointer_dim != 0) {
    if ("bus_e1" == access_poly_info.properties["port_type"] ||
        "fifo_v1" == access_poly_info.properties["port_type"]) {
#if PROJDEBUG
      cout << "port_type: " << access_poly_info.properties["port_type"] << endl;
#endif
      void *sg_param = codegen->GetFuncCallParam(sg_call, arg_idx);
      int arg_idx = codegen->GetFuncCallParamIndex(sg_param);

      //  if (codegen->IsCastExp(sg_param)) sg_param =
      //  codegen->GetExpUniOperand(sg_param);
      codegen->remove_cast(&sg_param);
      //  assert(codegen->IsVarRefExp(sg_param));

      void *sg_scope = sg_task_bb;
      //  ZP: 20160622
      {
        assert(arg_idx != -1);
        void *decl = codegen->GetFuncDeclByCall(sg_call);
        sg_param = codegen->GetFuncParam(decl, arg_idx);
        //  YX: 20170313
        sg_scope = decl;
      }

      //  if(codegen->IsVarRefExp(sg_param))
      {
        void *sg_array =
            sg_param;  //  codegen->GetVariableInitializedName(sg_param);
        assert(sg_array);

        ArrayRangeAnalysis mar(sg_array, codegen, sg_scope, sg_scope, false,
                               true);

#if PROJDEBUG
        if (1)
          cout << mar.print() << endl;
#endif
        //  ZP: 20151108
        if (parsed_array_dim == 0 &&
            !mars_ir->HasLink(codegen, sg_func_decl, sg_arg, 1)) {
          string task_name = poly_info_task.properties["name"];
          string kernel_name = poly_info_task.properties["kernel_name"];
          string port_name = codegen->UnparseToString(actual_arg);

          string sFileName = codegen->get_file(actual_arg);
          string sFileLine = my_itoa(codegen->get_line(actual_arg));

          string msg = "Finding an array whose dimension size is "
                       "undetermined: \n    task=" +
                       task_name + " kernel=" + kernel_name +
                       " argument=" + port_name + " \n    at " + sFileName +
                       ":" + sFileLine + "\n";
          msg += "    Hint: please add an assertion for the non-constant "
                 "variables in the malloc/new statements";
          dump_critical_message("PROCS", "ERROR", msg, 301, 1);

#if PROJDEBUG
          cout << "[ERROR@Outline]" << msg << endl;
#endif
          exit(1);
        }

        int empty_access = 1;

        if (mar.has_read()) {
          assert(mar.GetRangeExprRead().size() == 1);
          vector<CMarsRangeExpr> vec_mr = mar.GetRangeExprRead();
          assert(vec_mr.size() == 1);  //  FIXME: assume 1-D only

          CMarsRangeExpr mr = vec_mr[0];
          string range = mr.print();
          CMarsExpression me_lower;
          CMarsExpression me_upper;
          int is_simple = mr.get_simple_bound(&me_lower, &me_upper);

          if (!mr.has_lb() || !mr.has_ub()) {
            access_poly_info.properties["read_range_valid"] = "1";
            access_poly_info.properties["read_range_start"] = "0";
            access_poly_info.properties["read_range_length"] =
                access_poly_info.properties["array_dim"];
          } else if (range != "empty" && is_simple) {
            CMarsExpression me_length(me_upper - me_lower + 1);

            access_poly_info.properties["read_range_valid"] = "1";
            access_poly_info.properties["read_range_start"] =
                me_lower.print_s();
            access_poly_info.properties["read_range_length"] =
                me_length.print_s();
          } else if (range != "empty") {
            string range = mr.print();
            vector<string> v_range = str_split(range, "..");
            string str_start = v_range[0];
            string str_len = "(" + v_range[1] + ")-(" + v_range[0] + ")+1";

            access_poly_info.properties["read_range_valid"] = "1";
            access_poly_info.properties["read_range_start"] = str_start;
            access_poly_info.properties["read_range_length"] = str_len;
          }

          //  ZP: 20151104
          if (access_poly_info.properties["read_range_valid"] == "1") {
            empty_access = 0;

            int lbound =
                my_atoi(access_poly_info.properties["read_range_start"]);
            int ubound =
                lbound +
                my_atoi(access_poly_info.properties["read_range_length"]) - 1;
            int dim = parsed_array_dim;
            if (lbound < 0 || ubound >= dim) {
              string msg =
                  "Finding a possible out-of-bound access: \n  range=[" +
                  range + "] task=" + sTaskName +
                  " argument=" + codegen->UnparseToString(actual_arg) + "\n";
              dump_critical_message("PROCS", "WARNING", msg, 201, 1);
            }
          }
        }

        if (mar.has_write()) {
          assert(mar.GetRangeExprWrite().size() == 1);
          vector<CMarsRangeExpr> vec_mr = mar.GetRangeExprWrite();
          assert(vec_mr.size() == 1);  //  FIXME: assume 1-D only

          CMarsRangeExpr mr = vec_mr[0];
          string range = mr.print();
          CMarsExpression me_lower;
          CMarsExpression me_upper;
          int is_simple = mr.get_simple_bound(&me_lower, &me_upper);

          if (!mr.has_lb() || !mr.has_ub()) {
            access_poly_info.properties["write_range_valid"] = "1";
            access_poly_info.properties["write_range_start"] = "0";
            access_poly_info.properties["write_range_length"] =
                access_poly_info.properties["array_dim"];
          } else if (range != "empty" && is_simple) {
            CMarsExpression me_length(me_upper - me_lower + 1);

            access_poly_info.properties["write_range_valid"] = "1";
            access_poly_info.properties["write_range_start"] =
                me_lower.print_s();
            access_poly_info.properties["write_range_length"] =
                me_length.print_s();
          } else if (range != "empty") {
            vector<string> v_range = str_split(range, "..");
            string str_start = v_range[0];
            string str_len = "(" + v_range[1] + ")-(" + v_range[0] + ")+1";

            access_poly_info.properties["write_range_valid"] = "1";
            access_poly_info.properties["write_range_start"] = str_start;
            access_poly_info.properties["write_range_length"] = str_len;
          }

          //  ///////////  /
          //  ZP: 20160623
          if (access_poly_info.properties["write_range_valid"] == "1" &&
              !mr.is_exact()) {
            access_poly_info.properties["read_range_valid"] =
                access_poly_info.properties["write_range_valid"];
            access_poly_info.properties["read_range_start"] =
                access_poly_info.properties["write_range_start"];
            access_poly_info.properties["read_range_length"] =
                access_poly_info.properties["write_range_length"];
          }

          //
          //  ZP: 20151104
          if (access_poly_info.properties["write_range_valid"] == "1") {
            empty_access = 0;
            int lbound =
                my_atoi(access_poly_info.properties["write_range_start"]);
            int ubound =
                lbound +
                my_atoi(access_poly_info.properties["write_range_length"]) - 1;
            int dim = parsed_array_dim;
            if (lbound < 0 || ubound >= dim) {
              string msg = "Possible out-of-bound access: \n  range=[" + range +
                           "] task=" + sTaskName +
                           " argument=" + codegen->UnparseToString(actual_arg) +
                           "\n";
              dump_critical_message("PROCS", "WARNING", msg, 201, 1);
            }
          }
        }

        if (empty_access == 1) {
          string msg =
              "Finding an unused interface variable: \n task=" + sTaskName +
              " argument=" + codegen->UnparseToString(actual_arg) + "\n";
          dump_critical_message("PROCS", "WARNING", msg, 202, 1);
        }
      }
      //  else {
      //    //  access_poly_info.properties["read_range_valid"] = "1";
      //    //  access_poly_info.properties["read_range_start"] = "0";
      //    //  access_poly_info.properties["read_range_length"] =
      //    access_poly_info.properties["array_dim"];
      //  }
    }
  }
  return access_poly_info;
}

int tldm_extraction_mars_gen(CSageCodeGen *codegen, void *pTopFunc,
                             CInputOptions options) {
  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(codegen, pTopFunc);
  vector<pair<void *, string>> vecTldmPragmas;
  map<void *, string> mapTaskName;
  map<void *, string> mapTaskSyncMode;
  map<void *, string> mapTaskDevice;
  map<void *, string> mapTask2Additional;
  map<void *, string> mapTask2GraphName;
  map<void *, void *> mapTask2Graph;
  map<void *, string> mapGraph2GraphName;
  //  ///////////////////////////////////////////////////////////////////////  /
  //  1. Extract TLDM pragmas from AST
  //  ///////////////////////////////////////////////////////////////////////  /
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  collect_tldm_info(codegen, pTopFunc, vecTldmPragmas, &mapTaskName,
                    &mapTaskSyncMode, &mapTaskDevice, &mapTask2Additional,
                    &mapTask2GraphName, &mapTask2Graph, &mapGraph2GraphName);
  //  /////////////////////////////////////////////////////////////////////  /
  //  2. Traverse TLDM graphs and generate TLDM-IR for graphs
  //  /////////////////////////////////////////////////////////////////////  /
  traverse_tldm_graph_to_generate_tldm_ir(codegen, mapGraph2GraphName,
                                          mapTask2Additional);
  //  /////////////////////////////////////////////////////////////////////  /
  //  3. Traverse TLDM tasks and generate TLDM-IR for the tasks
  //  /////////////////////////////////////////////////////////////////////  /
  map<string, string> mapVar2Range;
  for (auto it = mapTask2Graph.begin(); it != mapTask2Graph.end(); it++) {
    void *sg_task_bb = it->first;
    void *sg_graph_bb = it->second;
    string sTaskName = mapTaskName[sg_task_bb];
    string sKernelName;
    string str_graph_name = mapTask2GraphName[sg_task_bb];

    vector<void *> vec_exps;
    codegen->GetNodesByType(sg_task_bb, "preorder", "SgVarRefExp", &vec_exps);

    //  get all variable names of the kernel invariant variables used in the
    //  task
    set<string> var_defined;
    for (size_t idx = 0; idx < vec_exps.size(); idx++) {
      if (mapVar2Range.count(codegen->UnparseToString(vec_exps[idx])) > 0) {
        var_defined.insert(codegen->UnparseToString(vec_exps[idx]));
      }
    }

    vector<void *> vec_exps_graph;

    //  setup a map from the kernel invariant variable name to one of its
    //  reference in the graph
    map<string, void *> mapName2var;
    codegen->GetNodesByType(sg_graph_bb, "preorder", "SgVarRefExp",
                            &vec_exps_graph);
    for (size_t idx = 0; idx < vec_exps_graph.size(); idx++) {
      //  if(var_defined.count(codegen->UnparseToString(vec_exps[idx])) > 0)
      //  continue;
      if (mapVar2Range.count(codegen->UnparseToString(vec_exps_graph[idx])) >
          0) {
        mapName2var[codegen->UnparseToString(vec_exps_graph[idx])] =
            vec_exps_graph[idx];
      }
    }

    //  /////////////////////////////////////////////////  /
    //  3.2 order vector extraction for the task
    //  /////////////////////////////////////////////////  /
    string order_vec = "1";

#if PROJDEBUG
    std::cout << "Analyzing task domain: " << mapTaskName[sg_task_bb] << endl;
#endif
    execute_seq[sg_graph_bb].insert(sg_task_bb);

    void *sg_new_decl;
    void *sg_stmt = codegen->GetChildStmt(sg_task_bb, 0);
    assert(codegen->IsExprStatement(sg_stmt));
    void *sg_call = codegen->GetExprFromStmt(sg_stmt);
    assert(codegen->IsFunctionCall(sg_call));
    sg_new_decl = codegen->GetFuncDeclByCall(sg_call, 1);
    assert(sg_new_decl);  //  acc function needs to be declared

    vector<string> iterators, lower, upper;
    //  int ret = codegen->analyze_iteration_domain(sg_graph_bb, sg_task_bb,
    //  iterators, lower, upper);
    map<string, vector<poly_access_pattern>> mapVar2Matrix;
    vector<string> orderOfPortVar;
    vector<string> params;
    //  codegen->get_io_matrix_of_function(sg_new_decl, mapVar2Matrix,
    //  iterators, params, orderOfPortVar);

    //  string iter_list = ""; string up_list = ""; string param_list = "";
    //  vector<string> vecPragmas;
    //  vecPragmas.push_back("tldm iteration_domain iterator=\"" + iter_list +
    //  "\" max_range=\"" + up_list + "\"\n");
    //  string sAdditional = mapTask2Additional[sg_task_bb];
    //  vecPragmas.push_back("tldm tuning iterator=\"" + iter_list + "\"
    //  order=\""+ iter_list +"\" graph_name=\"" + str_graph_name +
    //  "\""+sAdditional+"\n");

    sKernelName = codegen->GetFuncName(sg_new_decl);

    //  /////////////////////////////////////////////////  /
    //  7 Write task and access patterns into TLDM-IR
    //  /////////////////////////////////////////////////  /
    tldm_polyhedral_info poly_info_task = generate_poly_info_for_task(
        codegen, sg_task_bb, sTaskName, str_graph_name, sKernelName,
        mapTaskSyncMode, mapTaskDevice, mapTask2Additional);

    CTldmTaskAnn *curr_task = create_tldm_task(sTaskName, poly_info_task);

    //  ZP : 20151115: fix for constant argument
    {
      for (i = 0; i < (size_t)codegen->GetFuncCallParamNum(sg_call); i++) {
        static int kernel_arg_index = 0;
        void *actual_arg = codegen->GetFuncCallParam(sg_call, i);
        codegen->remove_cast(&actual_arg);
        void *arg_type = codegen->GetTypeByExp(actual_arg);

        //  ZP: 20161129
        //  if (codegen->IsScalarType(arg_type) &&
        //        !codegen->IsVarRefExp(actual_arg))
        if (codegen->IsValueExp(actual_arg)) {
          void *sg_scope = codegen->GetScope(actual_arg);
          void *new_decl = codegen->CreateVariableDecl(
              arg_type, "__kernel_arg" + my_itoa(kernel_arg_index++),
              codegen->CopyExp(actual_arg), sg_scope);
          void *new_actual_arg = codegen->CreateVariableRef(new_decl);

          //  GetPreviousStmt has bug with pragma
          //  void * sg_graph_pragma = codegen->GetPreviousStmt(sg_graph_bb);
          void *sg_parent = codegen->GetParent(sg_graph_bb);
          void *sg_graph_pragma = codegen->GetChildStmt(
              sg_parent, codegen->GetChildStmtIdx(sg_parent, sg_graph_bb) - 1);
          codegen->InsertStmt(new_decl, sg_graph_pragma);
          codegen->ReplaceExp(actual_arg, new_actual_arg);

          actual_arg = new_actual_arg;
        }
      }
    }

    {
      CMarsIr mars_ir;
      mars_ir.get_mars_ir(codegen, pTopFunc, options);
      void *sg_func_decl = sg_new_decl;
      for (i = 0; i < (size_t)codegen->GetFuncParamNum(sg_func_decl); i++) {
        void *arg = codegen->GetFuncParam(sg_func_decl, i);
        void *actual_arg = codegen->GetFuncCallParam(sg_call, i);

        //  //  ZP: 20151013 fixed
        //  if (codegen->IsCastExp(actual_arg))
        //  {
        //    actual_arg = codegen->GetExpUniOperand(actual_arg);
        //  }
        codegen->remove_cast(&actual_arg);

        //  ZP: 20151012
        size_t parsed_array_dim;
        int pointer_dim = -1;
        string parsed_data_type;  //  scalar or int *, ...
        {
          void *array_ref = NULL;
          void *base_type;
          vector<size_t> nSizes;
          //  codegen->parse_array_ref_from_pntr(actual_arg, &array_ref);
          codegen->get_type_dimension_by_ref(actual_arg, &array_ref, &base_type,
                                             &nSizes);

          if (array_ref) {
            //  void * sg_array;
            //  void * sg_pntr;
            //  vector<void*> sg_indexes;
            //  int pointer_dim;
            //  codegen->parse_pntr_ref_by_array_ref(array_ref, &sg_array,
            //  sg_pntr, sg_indexes, pointer_dim);

            //  void * sg_type = codegen->GetTypebyVarRef(array_ref);
            //  codegen->get_type_dimension(sg_type, &base_type, &nSizes,
            //  actual_arg);
            //
            pointer_dim = nSizes.size();

            //  if (pointer_dim == nSizes.size())
            if (pointer_dim == 0) {
              parsed_array_dim = 1;
              parsed_data_type = "scalar";
            } else if (pointer_dim == 1) {
              //  else if (pointer_dim == nSizes.size() - 1)
              parsed_array_dim = nSizes[0];
              //  youxiang 20161103
              //  parsed_data_type = codegen->UnparseToString(
              //        codegen->GetOrigTypeByTypedef(base_type)) +
              //    " *";
              parsed_data_type = codegen->UnparseToString(
                  codegen->GetOrigTypeByTypedef(base_type));

            } else {
              string sFileName = codegen->GetFileInfo_filename(sg_call);
              printf("\n[mars_opt] ERROR: Multi-dimensional array/pointer is "
                     "not supported on kernel interface: \n%s, line "
                     "%d:\n%s\n%s\n\n",
                     sFileName.c_str(), codegen->GetFileInfo_line(sg_call),
                     codegen->UnparseToString(actual_arg).c_str(),
                     codegen->UnparseToString(sg_call).c_str());
              assert(0);
              exit(1);
            }
          } else {
            void *base_type1;
            vector<size_t> nSizes1;
            SgInitializedName *sg_arg =
                isSgInitializedName(static_cast<SgNode *>(arg));
            codegen->get_type_dimension(sg_arg->get_type(), &base_type1,
                                        &nSizes1, sg_arg);
            parsed_data_type = codegen->GetStringByType(
                codegen->GetOrigTypeByTypedef(base_type1));
            parsed_array_dim = 1;
            if (nSizes1.size() == 1) {
              parsed_array_dim = nSizes1[0];
            } else if (nSizes1.size() > 1) {
              string sFileName = codegen->GetFileInfo_filename(sg_call);
              printf("\n[mars_opt] ERROR: Multi-dimensional array/pointer is "
                     "not supported on kernel interface: \n%s, line "
                     "%d:\n%s\n%s\n\n",
                     sFileName.c_str(), codegen->GetFileInfo_line(sg_call),
                     codegen->UnparseToString(actual_arg).c_str(),
                     codegen->UnparseToString(sg_call).c_str());
              assert(0);
              exit(1);
            }
            //  if (nSizes1.size() != 0 && !mars_ir.HasLink(codegen,
            //  sg_func_decl, sg_arg, 1)) {

            //    string sFileName = codegen->GetFileInfo_filename(actual_arg);
            //    string sFileLine = my_itoa(codegen->get_line(actual_arg));
            //    string msg = string("Finding an unsupported kernel interface
            //  with ") +         " \n    task='" + sTaskName + "'
            //  kernel='"
            //  + sKernelName
            //  + "' argument='" +         codegen->UnparseToString(actual_arg)
            //  +
            //  "' \n at " +         sFileName + ":" + sFileLine + "\n"; msg
            //  += "\n    The possible reasons: ";     msg += "\n        1) The
            //  unmatched pointer dimensions
            //  between the actual "         "and formal parameters,";     msg
            //  += "2) The struct or class object, ";     msg += "3) The pointer
            //  operations, ";     msg += "4) The multi-dimensional pointers, ";
            //  msg
            //  += "5) The addressOf operations '&'. ";
            //    dump_critical_message("PROCS", "ERROR", msg, 302, 1);

            //    //  ZP: FIXME: the type analysis of actual argument and formal
            //    //  parameter mismatch.
            //    //  assert(0);
            //    exit(1);
            //  }
          }

          if (parsed_data_type == "scalar") {
            SgInitializedName *sg_arg =
                isSgInitializedName(static_cast<SgNode *>(arg));
            parsed_data_type = codegen->UnparseToString(
                codegen->GetOrigTypeByTypedef(sg_arg->get_type()));
          }

          printf("Func=%s, Arg=%s: dim=%ld, type=%s \n",
                 codegen->UnparseToString(sg_call).c_str(),
                 codegen->UnparseToString(actual_arg).c_str(), parsed_array_dim,
                 parsed_data_type.c_str());
        }

        SgInitializedName *sg_arg =
            isSgInitializedName(static_cast<SgNode *>(arg));
        string sDataType = codegen->UnparseToString(sg_arg->get_type());
        string sArray = codegen->UnparseToString(sg_arg);

        //  /////////////////////////////////  /
        tldm_polyhedral_info access_poly_info = generate_access_tldm_polly_info(
            codegen, pTopFunc, &mars_ir, sg_task_bb, sTaskName,
            parsed_array_dim, pointer_dim, parsed_data_type, sg_arg, actual_arg,
            i, sg_call, sg_func_decl, vecTldmPragmas, poly_info_task);
        create_and_append_tldm_port(curr_task, sArray, access_poly_info);
      }
    }

    {
      string task_name = poly_info_task.properties["name"];
      string kernel_name = poly_info_task.properties["kernel_name"];
      string task_sync_type = poly_info_task.properties["async"];
      if (task_sync_type == "1")
        task_sync_type = "non-blocking";
      else
        task_sync_type = "blocking";
      string msg = "Generating a " + task_sync_type + " task '" + task_name +
                   string("' ") + "for kernel '" + kernel_name + string("' ") +
                   getUserCodeFileLocation(codegen, sg_stmt, true) + "\n";

#if PROJDEBUG
      dump_critical_message("PROCS", "INFO", msg, 101, 1);
#endif
    }
  }

  //     sorted_execute_seq.clear();
  //     for(map<void*, set<void*, TaskOrderLT> >::iterator mi =
  //     execute_seq.begin(); mi != execute_seq.end(); mi ++)
  //     {
  //         sorted_execute_seq.insert(mi->first);
  //     }
  //     for(set<void*, GraphOrderLT>::iterator si = sorted_execute_seq.begin();
  //     si != sorted_execute_seq.end(); si ++)
  //         graph_execute_seq.push_back(mapGraph2GraphName[*si]);

  dump_task_pragma();
  dep_graph_gen();
  io_cfg_gen();
  system("echo 'task_pragma.c*' >> kernel.list");
  system("echo 'dep_pragma.c*' >> kernel.list");
  return 1;
}

int preprocessing(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &options) {
  if ("" != options.get_option_key_value("-a", "naive_hls"))
    return 0;
  //  if ("naive_hls" == options.get_option("-a"))
  //  return 0;
  codegen->fix_defuse_issues();
  return 1;
}

int cpu_ref_add_timer(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options) {
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    string sOrgPragma = vecTldmPragmas[i].second;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    if (!is_cmost_pragma(sFilter) || ("task" != sCmd))
      continue;

    void *sg_stmt = vecTldmPragmas[i].first;
    void *sg_task_func = codegen->GetNextStmt(sg_stmt);
    //            cout << codegen->UnparseToString(sg_stmt) << endl;

    //  add by Han
    vector<void *> vec_param;
    vec_param.push_back(codegen->CreateConst(0));
    void *func_start_timer = codegen->CreateStmt(
        V_SgExprStatement,
        codegen->CreateFuncCall("timer_begin", codegen->GetTypeByString("void"),
                                vec_param, codegen->GetScope(sg_task_func)));

    vector<void *> vec_param1;
    vec_param1.push_back(codegen->CreateConst(0));
    void *func_acc_timer = codegen->CreateStmt(
        V_SgExprStatement,
        codegen->CreateFuncCall("timer_total", codegen->GetTypeByString("void"),
                                vec_param1, codegen->GetScope(sg_task_func)));

    //        void * func_start_timer = codegen->CreateStmt(V_SgExprStatement,
    //        codegen->CreateFuncCall(
    //                "start_timer", codegen->GetTypeByString("void"),
    //                vec_param, codegen->GetScope(sg_task_func)
    //                ));
    //        void * func_acc_timer = codegen->CreateStmt(V_SgExprStatement,
    //        codegen->CreateFuncCall(
    //                "acc_comp_timer", codegen->GetTypeByString("void"),
    //                vec_param1, codegen->GetScope(sg_task_func)
    //                ));

    codegen->InsertStmt(func_start_timer, sg_task_func);
    codegen->InsertAfterStmt(func_acc_timer, sg_task_func);
    codegen->AddHeader("#include \"__merlin_timer.h\"\n", sg_task_func);
  }

  vector<void *> vecCalls;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgFunctionDeclaration",
                          &vecCalls);
  for (size_t j = 0; j < vecCalls.size(); j++) {
    string func_name = codegen->GetFuncName(vecCalls[j]);
    if (func_name == "main") {
#if 0
      codegen->CopyFuncDecl(vecCalls[j], "user_main");
      codegen->RemoveChild(vecCalls[j]);
#else
      codegen->SetFuncName(vecCalls[j], "user_main");

      //  add a wrapper
      //  user_main_wrapper(int argc, char ** argv);

      vector<void *> arg_list;
      SgInitializedName *sg_param = SageBuilder::buildInitializedName(
          "argc", static_cast<SgType *>(codegen->GetTypeByString("int")));
      arg_list.push_back(sg_param);
      sg_param = SageBuilder::buildInitializedName(
          "argv", static_cast<SgType *>(codegen->GetTypeByString("char * *")));
      arg_list.push_back(sg_param);

      //  3. create funciton declaration
      void *sg_decl_new;
      {
        sg_decl_new = codegen->CreateFuncDecl(
            "int", "user_main_wrapper", arg_list,
            codegen->GetGlobal(vecCalls[j]), true, nullptr);

        void *sg_body_new = codegen->GetFuncBody(sg_decl_new);

        vector<void *> param_list;
        if (codegen->GetFuncParamNum(vecCalls[j]) == 2) {
          param_list.push_back(codegen->CreateVariableRef(arg_list[0]));
          param_list.push_back(codegen->CreateVariableRef(arg_list[1]));
        }
        void *expr = codegen->CreateFuncCall(
            "user_main", codegen->GetTypeByString("int"), param_list,
            codegen->GetGlobal(vecCalls[j]));
        void *stmt = codegen->CreateStmt(V_SgExprStatement, expr);
        codegen->AppendChild(sg_body_new, stmt);
      }

      codegen->AppendChild(codegen->GetGlobal(vecCalls[j]), sg_decl_new);
#endif
    }
  }
  return 1;
}
#endif
