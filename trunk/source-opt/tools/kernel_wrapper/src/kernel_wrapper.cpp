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
//  module           :   kernel wrapper
//  description      :   generate a wrapper for specified kernel
//  input            :
//  output           :
//  limitation       :
//  author           :   Han
//  ******************************************************************************************//
#include <iostream>
#include <string>
#include "math.h"
#include <boost/algorithm/string.hpp>

#include "codegen.h"
#include "ir_types.h"
#include "kernel_wrapper.h"
#include "mars_ir.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "postwrap_process.h"
#include "rose.h"
#define KERNEL_FILE_LIST "kernel.list"  //  also defined in kernel_separate.cpp

//  #include "inliner.h"
// using namespace std;
using std::exception;
//  using namespace SageInterface;
//  using namespace SageBuilder;
// using namespace MarsProgramAnalysis;
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);

//  -----------------------------------------------------------------------------
//  copy include diretives
//  -----------------------------------------------------------------------------
void copy_include_directive(CSageCodeGen *codegen, void *new_global,
                            void *orig_global) {
  int num_childs = codegen->GetChildStmtNum(orig_global);
  string file_name = codegen->get_file(orig_global);
  for (int i = 0; i != num_childs; ++i) {
    void *stmt = codegen->GetChildStmt(orig_global, i);
    if (codegen->get_file(stmt) != file_name &&
        !codegen->IsTransformation(stmt)) {
      continue;
    }
    if ((codegen->IsTypedefDecl(stmt) != 0) ||
        (codegen->IsClassDecl(stmt) != 0) || (codegen->IsEnumDecl(stmt) != 0)) {
      codegen->AppendChild(new_global, codegen->CopyStmt(stmt));
      continue;
    }
    //  discard other directive
    SgLocatedNode *sg_located_node =
        isSgLocatedNode(static_cast<SgNode *>(stmt));
    AttachedPreprocessingInfoType *old_info_list =
        sg_located_node->get_attachedPreprocessingInfoPtr();
    if (old_info_list == nullptr) {
      continue;
    }
    bool need_directive = false;
    for (auto info : *old_info_list) {
      if (info->getTypeOfDirective() ==
              PreprocessingInfo::CpreprocessorIncludeDeclaration ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::CpreprocessorDefineDeclaration ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::CpreprocessorUndefDeclaration ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::ClinkageSpecificationStart ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::ClinkageSpecificationEnd) {
        need_directive = true;
      }
    }
    if (!need_directive) {
      continue;
    }
    AttachedPreprocessingInfoType *new_info = new AttachedPreprocessingInfoType;
    for (auto info : *old_info_list) {
      if (info->getTypeOfDirective() ==
              PreprocessingInfo::CpreprocessorIncludeDeclaration ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::CpreprocessorDefineDeclaration ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::CpreprocessorUndefDeclaration ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::ClinkageSpecificationStart ||
          info->getTypeOfDirective() ==
              PreprocessingInfo::ClinkageSpecificationEnd) {
        new_info->push_back(info);
      }
    }
    vector<string> empty;
    void *dummy_decl =
        codegen->CreateFuncDecl("void", "__merlin_dummy_" + my_itoa(i), empty,
                                empty, new_global, false, nullptr);
    codegen->SetStatic(dummy_decl);
    codegen->AppendChild(new_global, dummy_decl);
    isSgLocatedNode(static_cast<SgNode *>(dummy_decl))
        ->set_attachedPreprocessingInfoPtr(new_info);
  }
}

int kernel_wrapper_top(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options) {
  printf("Hello kernel_wrapper...\n");
  int err_ret = 0;
  int count_kernel = 0;
  string kernel_die;
  void *head_file;
  string tool_type;
  if ("aocl" == options.get_option_key_value("-a", "impl_tool")) {
    tool_type = "aocl";
  } else if ("sdaccel" == options.get_option_key_value("-a", "impl_tool")) {
    tool_type = "sdaccel";
  } else {
    tool_type = "aocl";
  }
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;

  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  //  check kernel pragma illegality
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    void *kernel_pragma = vecTldmPragmas[i].first;
    string pragma_str = vecTldmPragmas[i].second;
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(pragma_str, &sFilter, &sCmd, &mapParams);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);
    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if ("KERNEL" !=
        sCmd) {  //  Only care kernel pragam here, preprocessing has been done
                 //  to switch other forms into "kernel" pragma
      continue;
    }
    void *next_stmt = codegen->GetNextStmt(kernel_pragma);
    if (codegen->IsFunctionDeclaration(next_stmt) == 0) {
      string msg = "Did not get a function declaration right after pragma:\n";
      msg += "#pragma " + pragma_str + " " +
             getUserCodeFileLocation(codegen, kernel_pragma, true);
      dump_critical_message("KWRAP", "ERROR", msg, 301, 1);
      err_ret = 1;
    }
  }

  global_variable_conversion_checker(codegen, pTopFunc, options);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    void *kernel_pragma = vecTldmPragmas[i].first;
    string pragma_str = vecTldmPragmas[i].second;
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(pragma_str, &sFilter, &sCmd, &mapParams);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);
    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if ("KERNEL" !=
        sCmd) {  //  Only care kernel pragma here, preprocessing has been done
                 //  to switch other forms into "kernel" pragma
      continue;
    }
    string user_kernel_name;
    if (!mapParams["name"].first.empty()) {
      user_kernel_name = mapParams["name"].first[0];
    }
#if PROJDEBUG
    printf("Find a kernel pragma : %s\n", pragma_str.c_str());
#endif
    string pragma_task =
        "\n#pragma ACCEL task " +
        (user_kernel_name.empty() ? "" : ("name=\"" + user_kernel_name + "\""));
    //  check die attributes
    string die_name = "NULL";
    if (!mapParams["die"].first.empty()) {
      pragma_task += " die=" + mapParams["die"].first[0];
      die_name = mapParams["die"].first[0];
    }

    count_kernel++;
    void *next_stmt = codegen->GetNextStmt(kernel_pragma);
    if (codegen->IsFunctionDeclaration(next_stmt) != 0) {
      //  1. Deal with one kernel function

      void *kernel_decl = next_stmt;
      //            void * scope = codegen->GetScope(kernel_decl);

      bool is_cpp_design = codegen->isWithInCppFile(kernel_decl);
      bool is_cpp_linkage = codegen->isCppLinkage(kernel_decl);
      string kernel_name = codegen->GetFuncName(kernel_decl);
      kernel_die += "//" + kernel_name + "=" + die_name + "\n";
      void *return_type = codegen->GetFuncReturnType(kernel_decl);
      void *new_return_type =
          codegen->RemoveConstVolatileType(return_type, kernel_decl);
      string return_string = codegen->UnparseToString(return_type);
      string kernel_top_head_h =
          "__merlinhead_kernel_top.h";  //  the file itself is not generated
                                        //  here, just to add #include

      //  1.1 Create api list file, and insert kernel info into the list file
      {
        string api_file_name;
        if (is_cpp_design) {
          api_file_name = "__merlin_api_list.cpp";
        } else {
          api_file_name = "__merlin_api_list.c";
        }
        void *api_file = codegen->CreateSourceFile(
            api_file_name);  //  ZP: FIXME 08302017: Created multiple times ??

        string kernel_api_name;
        if (!mapParams["API_name"].first.empty()) {
          kernel_api_name = mapParams["API_name"].first[0];
        } else {
          kernel_api_name = kernel_name;
        }
        kernel_api_name = kernel_name;
        string api_file_string =
            "//" + kernel_name + "=" + kernel_api_name + "\n";
        codegen->AddDirectives(api_file_string, api_file);
      }

      //  1.2 Greate wrapper files: c and header
      void *wrapper_file = nullptr;
      string wrapper_name;
      {
        if (user_kernel_name.empty()) {
          user_kernel_name = kernel_name;
        }
        user_kernel_name = kernel_name;
        wrapper_name = MERLIN_WRAPPER_PREFIX + user_kernel_name;
        string wrapper_c_file_name =
            wrapper_name + (is_cpp_design ? ".cpp" : ".c");
        //  ZP: 20160922
        {
          string cmd = "rm -rf " + wrapper_c_file_name;
          system(cmd.c_str());
        }

        wrapper_file = codegen->CreateSourceFile(wrapper_c_file_name);
        //  youxiang: 20161129
        copy_include_directive(codegen, wrapper_file,
                               codegen->GetGlobal(kernel_decl));

        //  ZP: 20160922
        {
          string cmd = "echo '" + wrapper_c_file_name + "' >> " +
                       string(KERNEL_FILE_LIST);
          system(cmd.c_str());
        }
      }

      //  /////////////////////////////////////////////////////////////////////////////////////////////////
      //  / 1.4 Create Main kernel wrapper function
      //  /////////////////////////////////////////////////////////////////////////////////////////////////
      //  /
      //
      //  1.4.1 get function decl and call param list
      //  printf("type = %s\n", codegen->UnparseToString(return_type).c_str());
      void *param_type;
      vector<vector<void *>> vec_param_list;
      const int num_copy = 11;
      vec_param_list.resize(num_copy);
      vector<void *> type_list;
      {
        int func_param_num = codegen->GetFuncParamNum(kernel_decl);
        string func_name = codegen->GetFuncName(kernel_decl);
        void *func_body = codegen->GetFuncBody(kernel_decl);
        set<string> set_name;
        set_name.insert(func_name);
        for (int j = 0; j < func_param_num; j++) {
          void *func_param = codegen->GetFuncParam(kernel_decl, j);
          string var_name = codegen->GetVariableName(func_param);
          param_type = codegen->GetTypebyVar(func_param);
          //  FIX bug1779
          int suffix = 0;
          while (set_name.count(var_name + (suffix > 0 ? ("_" + my_itoa(suffix))
                                                       : "")) > 0) {
            suffix++;
          }
          string new_var_name = var_name;
          if (suffix > 0) {
            new_var_name = var_name + "_" + my_itoa(suffix);
            string wrapper_pragma_str = "ACCEL wrapper variable=" + var_name;
            wrapper_pragma_str += " new_variable=" + new_var_name;
            void *wrapper_pragma =
                codegen->CreatePragma(wrapper_pragma_str, func_body);
            codegen->PrependChild(func_body, wrapper_pragma);
          }
          set_name.insert(new_var_name);
          for (int ic = 0; ic < num_copy; ++ic) {
            vec_param_list[ic].push_back(
                codegen->CreateVariable(param_type, new_var_name));
          }
          type_list.push_back(param_type);
        }
      }
      //
      //  1.4.2 create definition of variable

      //  /////////////////////////////////////////////////////////////////////////////////////
      //  / ZP : 20160925 Move global variable definition into wrapper files
      {
        CSageCodeGen *ast = codegen;
        void *kernel_global = ast->GetGlobal(next_stmt);
        void *wrapper_global = wrapper_file;
        vector<void *> v_decl;
        ast->GetNodesByType(kernel_global, "preorder", "SgVariableDeclaration",
                            &v_decl);
        for (auto decl : v_decl) {
          void *init = ast->GetVariableInitializedName(decl);
          if (ast->IsGlobalInitName(init) == 0) {
            continue;
          }
          if (ast->GetVariableDefinition(decl, init) == nullptr) {
            continue;
          }
          void *var_type = ast->GetTypebyVar(init);
          void *base_type = ast->GetBaseType(var_type, true);
          if (ast->GetStringByType(base_type).find("merlin_stream") !=
              string::npos) {
            continue;
          }
          //  youxiang 20161005
#if 0
                    //  string sFileExt = get_filename_ext(
                    //        ast->get_file(decl));
                    //  if (sFileExt != "cpp" && sFileExt!="c") continue;
#else
          if ((ast->IsFromInputFile(decl) == 0) ||
              ast->isWithInHeaderFile(decl)) {
            continue;
          }
#endif

          string s_name = ast->_up(init);
          void *initializer = ast->GetInitializer(init);
          //  1. copy to wrapper.cpp
          if (ast->IsExtern(decl) || (ast->IsConstType(var_type) == 0)) {
            //  void * new_decl = ast->CopyStmt(decl);
            //  ast->DiscardPreprocInfo(new_decl);
            //  youxiang 20161004 discard static keyword
            void *new_decl = ast->CreateVariableDecl(
                var_type, ast->GetVariableName(init),
                initializer != nullptr ? ast->CopyExp(initializer) : nullptr,
                wrapper_global, decl);
            ast->AppendChild(wrapper_global, new_decl);
            if (ast->IsExtern(decl))
              ast->SetExtern(new_decl);
          }

          //  youxiang 20160927
          if (ast->IsConstType(var_type) != 0) {
            continue;
          }

          //  youxiang 20161030
          if (ast->isWithInHeaderFile(decl)) {
            continue;
          }
          //  2. create the dummy func decl
          //  static void __merlin_dummy_varname();
          if (!ast->IsExtern(decl)) {
            void *v_type = ast->GetTypebyVar(init);
            if ((ast->IsArrayType(v_type) != 0) && (initializer != nullptr)) {
              auto *init_type = ast->GetTypeByExp(initializer);
              if (ast->IsArrayType(init_type) != 0) {
                // initializer may has array size info
                // e.g. int a[] = {1, 2};
                // {1,2} has type "int [2]".
                // initializer may not has array size info
                // e.g. char A[8] = "abcdefgh";
                // initializer is Cstring  pointer type
                v_type = init_type;
              }
            }
            void *extern_decl = ast->CreateVariableDecl(v_type, s_name, nullptr,
                                                        kernel_global, decl);
            ast->SetExtern(extern_decl);
            ast->InsertStmt(extern_decl, decl);

            vector<string> empty;
            void *new_decl =
                ast->CreateFuncDecl("void", "__merlin_dummy_" + s_name, empty,
                                    empty, kernel_global, false, decl);
            ast->SetStatic(new_decl);
            ast->ReplaceStmt(decl, new_decl, true);
          }
        }
      }
      //  /////////////////////////////////////////////////////////////////////////////////////
      //  /

      //  1.4.3 create original function declaration
      void *kernel_func = codegen->CreateFuncDecl(new_return_type, kernel_name,
                                                  vec_param_list[0],
                                                  wrapper_file, false, nullptr);
      codegen->AppendChild(wrapper_file, kernel_func);

      //  1.4.4 create wrapper function declaration
      void *wrapper_func =
          codegen->CreateFuncDecl(return_type, wrapper_name, vec_param_list[1],
                                  wrapper_file, true, nullptr);
      codegen->AppendChild(wrapper_file, wrapper_func);
      string write_buffer_name = MERLIN_WRITE_BUFFER_PREFIX + user_kernel_name;
      string read_buffer_name = MERLIN_READ_BUFFER_PREFIX + user_kernel_name;
      string execute_name = MERLIN_EXECUTE_PREFIX + user_kernel_name;
      string final_name = "__merlin_" + user_kernel_name;
      void *final_func = codegen->CreateFuncDecl(new_return_type, final_name,
                                                 vec_param_list[2],
                                                 wrapper_file, true, nullptr);
      codegen->AppendChild(wrapper_file, final_func);
      //  if memcoy use variable from scalar interface. need scalar interaface,
      //  not only array interface
      void *write_buffer_func = codegen->CreateFuncDecl(
          return_type, write_buffer_name, vec_param_list[3], wrapper_file, true,
          nullptr);
      codegen->AppendChild(wrapper_file, write_buffer_func);
      void *read_buffer_func = codegen->CreateFuncDecl(
          return_type, read_buffer_name, vec_param_list[4], wrapper_file, true,
          nullptr);
      codegen->AppendChild(wrapper_file, read_buffer_func);
      void *execute_func =
          codegen->CreateFuncDecl(return_type, execute_name, vec_param_list[5],
                                  wrapper_file, true, nullptr);
      codegen->AppendChild(wrapper_file, execute_func);
      //  Han 03/17/2017#
      codegen->InsertHeader(kernel_top_head_h, wrapper_func);
      if (is_cpp_design && !is_cpp_linkage) {
        string linkage_guard = "\nextern \"C\" {";
        codegen->AddDirectives(linkage_guard, kernel_func);
        codegen->AddDirectives(linkage_guard, wrapper_func);
        linkage_guard = "\n}\n";
        codegen->AddDirectives(linkage_guard, kernel_func, 0);
        codegen->AddDirectives(linkage_guard, wrapper_func, 0);
      }
      //  1.4.5 create kernel function call, and insert into wrapper
      vector<void *> call_param_list;
      call_param_list.clear();
      for (auto param : codegen->GetFuncParams(wrapper_func)) {
        call_param_list.push_back(codegen->CreateVariableRef(param));
      }
      void *kernel_call = codegen->CreateFuncCall(
          kernel_name, return_type, call_param_list, wrapper_file);
      void *kernel_call_stmt;
      if (return_string == "void") {
        kernel_call_stmt = codegen->CreateStmt(V_SgExprStatement, kernel_call);
      } else {
        kernel_call_stmt = codegen->CreateVariableDecl(
            new_return_type, "__merlinwrapper_return", kernel_call,
            wrapper_file);
      }

      void *func_body = codegen->GetFuncBody(wrapper_func);
      codegen->AppendChild(func_body, kernel_call_stmt);
      if (return_string != "void") {
        void *return_var = codegen->CreateVariableRef(kernel_call_stmt);
        void *return_stmt = codegen->CreateStmt(V_SgReturnStmt, return_var);
        codegen->InsertAfterStmt(return_stmt, kernel_call_stmt);
      }

      {  //  generate final kernel call, need check if have return value
        void *final_body = codegen->GetFuncBody(final_func);
        call_param_list.clear();
        for (auto param : codegen->GetFuncParams(final_func)) {
          call_param_list.push_back(codegen->CreateVariableRef(param));
        }
        void *final_call = codegen->CreateFuncCall(
            wrapper_name, return_type, call_param_list, wrapper_file);
        //        void *final_call_stmt =
        //            codegen->CreateStmt(V_SgExprStatement, final_call);
        void *final_call_stmt;
        if (return_string == "void") {
          final_call_stmt = codegen->CreateStmt(V_SgExprStatement, final_call);
        } else {
          final_call_stmt = codegen->CreateVariableDecl(
              new_return_type, "__merlin_return", final_call, final_body);
        }

        codegen->AppendChild(final_body, final_call_stmt);

        string init_call_string =
            "ACCEL string __merlin_init_" + kernel_name + "();";
        void *init_pragma = codegen->CreatePragma(
            init_call_string, static_cast<SgScopeStatement *>(wrapper_file));
        codegen->PrependChild(final_body, init_pragma);
        string check_string = "ACCEL string __merlin_check_opencl();";
        void *check_pragma = codegen->CreatePragma(
            check_string, static_cast<SgScopeStatement *>(wrapper_file));
        codegen->PrependChild(final_body, check_pragma);
        string release_call_string =
            "ACCEL string __merlin_release_" + kernel_name + "();";
        void *release_pragma = codegen->CreatePragma(
            release_call_string, static_cast<SgScopeStatement *>(wrapper_file));
        codegen->AppendChild(final_body, release_pragma);

        if (return_string != "void") {
          void *return_var = codegen->CreateVariableRef(final_call_stmt);
          void *return_stmt = codegen->CreateStmt(V_SgReturnStmt, return_var);
          //          codegen->InsertAfterStmt(return_stmt, final_call_stmt);
          codegen->AppendChild(final_body, return_stmt);
        }
      }

      //  1.4.6 create pragma before function call
      //  codegen->AddDirectives(pragma_task, kernel_call_stmt);
      void *new_pragma = codegen->CreatePragma(
          pragma_str, codegen->GetScope(kernel_call_stmt));
      codegen->InsertStmt(new_pragma, kernel_call_stmt);

      //  create head file
      string head_file_name_new = "kernel_top";
      string head_file_name = "__merlinhead_" + head_file_name_new +
                              (is_cpp_design ? ".cpp" : ".c");
      if (count_kernel == 1) {
        string cmd = "rm -rf " + head_file_name;
        system(cmd.c_str());
        head_file = codegen->CreateSourceFile(head_file_name);
        string opencl_init =
            "extern int opencl_init(const char * bitstream);\n";
        string __merlin_init =
            "extern int __merlin_init(const char * bitstream);\n";
        //  string __merlin_init_opencl     = "extern int
        //  __merlin_init_opencl(const char * bitstream);\n";
        string __merlin_release = "extern int __merlin_release();\n";
        //  string __merlin_release_opencl  = "extern int
        //  codegen->AddDirectives(opencl_init, head_file);
        codegen->AddDirectives(__merlin_init, head_file);
        //  codegen->AddDirectives(__merlin_init_opencl, head_file);
        codegen->AddDirectives(__merlin_release, head_file);
        //  codegen->AddDirectives(__merlin_release_opencl, head_file);
      }
      string __merlin_init_kernel =
          "extern int __merlin_init_" + user_kernel_name + "();\n";
      string __merlin_release_kernel =
          "extern int __merlin_release_" + user_kernel_name + "();\n";
      string __merlin_wait_kernel =
          "extern int __merlin_wait_kernel_" + user_kernel_name + "();\n";
      string __merlin_wait_write =
          "extern int __merlin_wait_write_" + user_kernel_name + "();\n";
      string __merlin_wait_read =
          "extern int __merlin_wait_read_" + user_kernel_name + "();\n";
      codegen->AddDirectives(__merlin_init_kernel, head_file);
      codegen->AddDirectives(__merlin_release_kernel, head_file);
      codegen->AddDirectives(__merlin_wait_kernel, head_file);
      codegen->AddDirectives(__merlin_wait_write, head_file);
      codegen->AddDirectives(__merlin_wait_read, head_file);

      void *wrapper_func_no_body =
          codegen->CreateFuncDecl(return_type, wrapper_name, vec_param_list[6],
                                  wrapper_file, false, nullptr);
      void *write_buffer_func_no_body = codegen->CreateFuncDecl(
          return_type, write_buffer_name, vec_param_list[7], wrapper_file,
          false, nullptr);
      void *read_buffer_func_no_body = codegen->CreateFuncDecl(
          return_type, read_buffer_name, vec_param_list[8], wrapper_file, false,
          nullptr);
      void *execute_func_no_body =
          codegen->CreateFuncDecl(return_type, execute_name, vec_param_list[9],
                                  wrapper_file, false, nullptr);
      void *final_func_no_body = codegen->CreateFuncDecl(
          new_return_type, final_name, vec_param_list[10], wrapper_file, false,
          nullptr);
      codegen->AppendChild(head_file, wrapper_func_no_body);
      codegen->AppendChild(head_file, write_buffer_func_no_body);
      codegen->AppendChild(head_file, read_buffer_func_no_body);
      codegen->AppendChild(head_file, execute_func_no_body);
      codegen->AppendChild(head_file, final_func_no_body);
      codegen->SetExtern(wrapper_func_no_body);
      codegen->SetExtern(write_buffer_func_no_body);
      codegen->SetExtern(read_buffer_func_no_body);
      codegen->SetExtern(execute_func_no_body);
      codegen->SetExtern(final_func_no_body);
      //  string linkage_guard;
      //  if (is_cpp_design && !is_cpp_linkage) {
      //   linkage_guard = "\n#ifdef __cplusplus\n"
      //                     "\n extern \"C\"\n"
      //                     "\n#else\n"
      //                     "\nextern \n"
      //                     "\n#endif\n";
      //  } else {
      //  linkage_guard = "\nextern\n";
      //  }
      //  codegen->AddDirectives(linkage_guard, wrapper_func_no_body);
      //  codegen->AddDirectives(linkage_guard, write_buffer_func_no_body);
      //  codegen->AddDirectives(linkage_guard, read_buffer_func_no_body);
      //  codegen->AddDirectives(linkage_guard, execute_func_no_body);
      //  comment original kernel pragma
      {
        //  youxiang 20161027 add a dummy function declaration to avoid
        //  the following statement removal stuck
        vector<string> empty;
        void *dummy_decl = codegen->CreateFuncDecl(
            "void", "__merlin_dummy_kernel_pragma", empty, empty,
            codegen->GetGlobal(kernel_decl), false, nullptr);
        codegen->SetStatic(dummy_decl);
        codegen->InsertStmt(dummy_decl, kernel_pragma);
      }
      codegen->RemoveStmt(kernel_pragma, true);
      string old_pragma = "#pragma " + pragma_str;
      codegen->AddComment("Original: " + old_pragma, kernel_decl);
    }
  }

  //  write die information to kernel_die.h
  FILE *f = fopen("kernel_die.h", "w");
  if (f == nullptr) {
    fprintf(stderr, "Error Openning kernel_die.h\n");
    throw std::exception();
  }
  fprintf(f, "%s\n", kernel_die.c_str());
  fclose(f);

  if (err_ret != 0) {
    throw std::exception();
  }

  return 0;
}

static string get_org_name_from_wrapper_pragma(string port_name, void *wrapper,
                                               CSageCodeGen *ast) {
  CMarsIrV2 mars_ir(ast);
  if (ast->GetFuncBody(wrapper) == nullptr) {
    return "";
  }

  vector<void *> vec_pragmas;
  ast->GetNodesByType(wrapper, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir.get_pragma_attribute(pragma, "variable");
    string s_port = mars_ir.get_pragma_attribute(pragma, "port");
    string s_intf = mars_ir.get_pragma_attribute(pragma, "wrapper");
    boost::algorithm::to_lower(s_intf);
    if (s_port == port_name && s_intf == "wrapper") {
      return s_var;
    }
  }

  return port_name;
}

//  /////////////////////////////  /
//  This is the interface for the middle-end modules to generate/specify
//  sub-kernels Parameters:
//    parent_kernel - the user specified kernel
//    sub_kernel - the kernels spawned in merlin optimizations
//  Functionality:
//    The proper transformation in wrapper function will be done to enable the
//    host synchronization with sub-kernel
//  Operation:
//    1. instantiate sub_kernel calls in wrapper file
//    2. add "kernel parent" pragma to the call
//
//  Limitations: (TODO: not fully checked yet!)
//  - sub-kernel and parent_kernel share the host OpenCL buffer using the same
//  variable name
//  - sub-kernel only has __global argument on its argument list (suppose other
//  data are transfered via channels)
//  - sub-kernel share the same synchronization point to host with the parent
//  kernel
//
int merlin_wrapper_add_sub_kernel(CMarsAST_IF *ast, void *parent_kernel,
                                  void *sub_kernel, bool is_before /*=true*/) {
  //  1. Get wrapper function and kernel calls;
  void *kernel_call = nullptr;
  void *wrapper_func = nullptr;
  {
    vector<void *> vec_calls;
    ast->GetFuncCallsFromDecl(parent_kernel, ast->GetProject(), &vec_calls);
    if (vec_calls.size() != 1) {
      return 0;
    }

    kernel_call = vec_calls[0];
    if (kernel_call == nullptr) {
      return 0;
    }

    wrapper_func = ast->TraceUpToFuncDecl(kernel_call);
  }

  //  2. Create and add kernel call
  void *new_call_stmt = nullptr;
  void *insert_scope = nullptr;
  {
    if (sub_kernel == nullptr) {
      return 0;
    }
    vector<void *> param_list;
    {
      for (auto one_param : ast->GetFuncParams(sub_kernel)) {
        string param_name = ast->_up(one_param);
        void *var =
            ast->find_variable_by_name("__m_" + param_name, kernel_call);
        if (var == nullptr) {
          //  For scalar variable, we use the original variable name
          //  scalar in struct, such as void top(st a) { //  st {int b;} will
          //  have a local variable '__m_a_b' for port 'a_b'
          string org_port_name =
              get_org_name_from_wrapper_pragma(param_name, wrapper_func, ast);
          var = ast->find_variable_by_name(org_port_name, kernel_call);
        }
        // TODO(youxiang): We assume name matching is ensured in middle-end
        // passes
        if (ast->IsInitName(var) == 0) {
          MERLIN_ERROR("cannot find the corresponding variable "
                       "in the wrapper function for port \'" +
                       param_name + "\'");
          return 0;
        }

        void *ref = ast->CreateVariableRef(var);
        param_list.push_back(ref);
      }
    }
    new_call_stmt = ast->CreateFuncCallStmt(sub_kernel, param_list);
    void *insert_pos = ast->TraceUpToStatement(kernel_call);

    if (is_before) {
      while (ast->IsPragmaDecl(ast->GetPreviousStmt(insert_pos)) != 0) {
        insert_pos = ast->GetPreviousStmt(insert_pos);
      }
      insert_scope = ast->GetScope(ast->GetParent(insert_pos));
      if (ast->IsBasicBlock(insert_scope) == 0) {
        return 0;
      }
      ast->InsertStmt(new_call_stmt, insert_pos);
    } else {
      insert_scope = ast->GetScope(ast->GetParent(insert_pos));
      if (ast->IsBasicBlock(insert_scope) == 0) {
        return 0;
      }
      ast->InsertAfterStmt(new_call_stmt, insert_pos);
    }
  }

  //  3. Add pragma
  {
    string str_pragma =
        "ACCEL sub_kernel parent=" + ast->GetFuncName(parent_kernel);
    void *pragma = ast->CreatePragma(str_pragma, insert_scope);
    ast->InsertStmt(pragma, new_call_stmt);
  }

  //  4. insert forward declaration
  {
    void *clone_func =
        ast->CloneFuncDecl(sub_kernel, ast->GetGlobal(wrapper_func), false);
    void *insert_pos = wrapper_func;

    while (ast->IsPragmaDecl(ast->GetPreviousStmt(insert_pos)) != 0) {
      insert_pos = ast->GetPreviousStmt(insert_pos);
    }
    ast->InsertStmt(clone_func, insert_pos);
  }

  return 1;
}
