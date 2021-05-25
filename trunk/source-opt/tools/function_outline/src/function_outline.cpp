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
//  module           :   function_outline
//  description      :   split each kernel top level for-loop to unit function,
//  and wrapper
//                      each function to a kernel file
//  input            :   1. code with all for loop flattened
//  output           :   1. multi-kernel-c-files respond to for-loop
//  limitation       :   Do not support no-variable-declartion out of top for
//  loop author           :   Han
//  ******************************************************************************************//
#include <list>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include "math.h"
#include "stdio.h"

#include "codegen.h"
#include "file_parser.h"
#include "function_outline.h"
#include "ir_types.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "rose.h"

// using namespace std;
using std::ofstream;
using std::string;
//  using namespace SageInterface;
//  using namespace SageBuilder;
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
extern vector<void *> get_top_kernels(CSageCodeGen *codegen, void *pTopFunc);

// FIXME: this file is not used, so just remove & before processed_kernels
// if need to use, require to check
bool move_variable_decls(CSageCodeGen *codegen, void *sg_kernel,
                         const set<void *> shared_ports,
                         set<void *> *processed_kernels) {
  if ((sg_kernel == nullptr) || processed_kernels->count(sg_kernel) > 0) {
    return false;
  }
  processed_kernels->insert(sg_kernel);
  vector<void *> vec_decl;
  codegen->GetNodesByType(sg_kernel, "preorder", "SgVariableDeclaration",
                          &vec_decl);
  void *func_body = codegen->GetFuncBody(sg_kernel);
  int i = 0;
  int num_stmts = codegen->GetChildStmtNum(func_body);
  set<void *> skip_decls;
  while (i < num_stmts) {
    void *stmt = codegen->GetChildStmt(func_body, i);
    if (codegen->IsVariableDecl(stmt) != 0) {
      skip_decls.insert(stmt);
      ++i;
    } else {
      break;
    }
  }
  if (i == num_stmts) {
    return false;
  }
  bool Changed = false;
  void *pos = codegen->GetChildStmt(func_body, i);
  //  printf("vec_init_size = %d\n",vec_init.size());
  for (auto &var_decl : vec_decl) {
    if (skip_decls.count(var_decl) > 0) {
      continue;
    }
    void *var_init = codegen->GetVariableInitializedName(var_decl);
    if (shared_ports.count(var_init) <= 0) {
      continue;
    }
    void *initializer = codegen->GetInitializer(var_init);
    if ((codegen->IsConstType(codegen->GetTypebyVar(var_init)) == 0) &&
        (initializer != nullptr)) {
      continue;
    }
    string var_name = codegen->GetVariableName(var_init);
    codegen->get_valid_local_name(sg_kernel, &var_name);
    void *new_var_decl = codegen->CreateVariableDecl(
        codegen->GetTypebyVar(var_init), var_name,
        initializer != nullptr ? codegen->CopyExp(initializer) : nullptr,
        func_body);
    if (codegen->is_register(var_decl) != 0) {
      codegen->set_register(new_var_decl);
    }
    codegen->InsertStmt(new_var_decl, pos);
    vector<void *> vec_ref;
    codegen->GetNodesByType(sg_kernel, "preorder", "SgVarRefExp", &vec_ref);
    for (auto &var_ref : vec_ref) {
      if (codegen->GetVariableInitializedName(var_ref) == var_init) {
        codegen->ReplaceExp(var_ref, codegen->CreateVariableRef(new_var_decl));
      }
    }
    codegen->RemoveStmt(var_decl);
    Changed = true;
  }

  vector<void *> vecCalls;
  codegen->GetNodesByType(sg_kernel, "preorder", "SgFunctionCallExp",
                          &vecCalls);
  for (size_t i = 0; i < vecCalls.size(); i++) {
    void *sub_kernel = codegen->GetFuncDeclByCall(vecCalls[i]);
    Changed |= move_variable_decls(codegen, sub_kernel, shared_ports,
                                   processed_kernels);
  }
  return Changed;
}

int move_variable_decl_top(CSageCodeGen *codegen, void *pTopFunc,
                           const set<void *> &shared_ports) {
  printf("    Enter move variable declarations...\n");
  auto kernels = get_top_kernels(codegen, pTopFunc);
  set<void *> processed_kernels;
  bool Changed = false;
  for (auto sg_kernel : kernels) {
    Changed |= move_variable_decls(codegen, sg_kernel, shared_ports,
                                   &processed_kernels);
  }
  return static_cast<int>(Changed);
}

int function_outline_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  //  1 . get each kernel top function
  //  2 . Get all for-loops in top function
  //  3 . Create a basic block, copy for-loop to this basic block
  //  4 . search all variable reference, get variable declaration, if
  //  declaration is not in for-loop,
  //      find original declaration and copy to basic block
  //  5 . create a new function declaration, move basic block to function
  //  6 . create a .c file, write basic block to file
  size_t i;
  string file_name_org;
  vector<string> vec_file_name;
  vector<string> vec_func_name;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
  map<void *, map<void *, bool>> top_loop_2_ports;
  map<void *, vector<void *>> top_loops;
  mars_ir.get_top_loop_info(&top_loop_2_ports, &top_loops);
  {
    //  flatten multiple dimension shared ports
    bool flatten = false;
    set<void *> shared_ports;
    for (auto loop_2_port : top_loop_2_ports) {
      auto ports_map = loop_2_port.second;
      for (auto port_pair : ports_map) {
        void *port = port_pair.first;
        if (port_pair.second) {
          continue;
        }
        shared_ports.insert(port);
      }
    }

    for (auto &port : shared_ports) {
      vector<size_t> old_dims;
      if (codegen->analysis_array_flatten(port, old_dims) != 0) {
        flatten |= codegen->apply_array_flatten(port, old_dims);
      }
    }

    bool lift_decls =
        move_variable_decl_top(codegen, pTopFunc, shared_ports) != 0;
    //  remove assert statement
    bool remove_assert = false;
#if 0
    //  delay to opencl generation
    for (auto &loop : top_loops) {
      vector<void*> vec_call;
      codegen->GetNodesByType(loop, "preorder", "SgFunctionCallExp",
                              &vec_call);
      for (auto &call : vec_call) {
        if (codegen->IsAssertFailCall(call)) {
          void *call_stmt = codegen->TraceUpToStatement(call);
          if (call_stmt) {
            remove_assert = true;
            codegen->RemoveStmt(call_stmt);
          }
        }
      }
    }
#endif
    if (flatten || remove_assert || lift_decls) {
      mars_ir.clear();
      codegen->init_defuse_range_analysis();
      mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
      top_loop_2_ports.clear();
      top_loops.clear();
      mars_ir.get_top_loop_info(&top_loop_2_ports, &top_loops);
    }
  }
  //  remove all merlin pragmas
  //  mars_ir.remove_all_pragmas();
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);

    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if (CMOST_KERNEL != sCmd) {
      continue;
    }
#if DEBUG
    printf("\n\ntask pragma = %s\n", vecTldmPragmas[i].second.c_str());
#endif
    void *sg_kernel_call = codegen->find_kernel_call(vecTldmPragmas[i].first);
    assert(sg_kernel_call);
    void *sg_task = sg_kernel_call;
    void *sg_task_stmt = codegen->TraceUpToStatement(sg_kernel_call);
    void *sg_kernel = codegen->GetFuncDeclByCall(sg_task);
    file_name_org = codegen->get_file(sg_kernel);

    //  fuction outline kernel
    string task_wait;
    string kernel_name = codegen->GetFuncName(sg_kernel);
#if DEBUG
    printf("kernel_name = %s\n", kernel_name.c_str());
#endif
    void *func_body = codegen->GetFuncBody(sg_kernel);
    int j = 0;
    map<string, map<string, int>> sm_size;
    vector<string> string_sm;
    for (auto &loop : top_loops[sg_kernel]) {
#if DEBUG
      cout << "sub_loop: " << codegen->UnparseToString(loop) << endl;
#endif
      map<void *, bool> ports = top_loop_2_ports[loop];

      //  get share memory list
      vector<string> share_memory_name;
      for (map<void *, bool>::iterator jt = ports.begin(); jt != ports.end();
           ++jt) {
#if DEBUG
        cout << "port : " << codegen->UnparseToString(jt->first) << "("
             << (jt->second ? "external port" : "shared port") << ")" << endl;
#endif
        if (!jt->second) {
          string share_memory = codegen->UnparseToString(jt->first);
          share_memory_name.push_back(share_memory);
        }
      }
      bool is_cpp_design = codegen->isWithInCppFile(loop);
      string file_name = "merlinkernel_" + kernel_name + "_" + my_itoa(j) +
                         (is_cpp_design ? ".cpp" : ".c");
#if DEBUG
      printf("file_name = %s\n", file_name.c_str());
#endif
      string func_name = "merlinkernel_" + kernel_name + "_" + my_itoa(j);
      void *new_file = codegen->CreateSourceFile(file_name);
      {
        //  YX: 20170626 add into kernel list
        ofstream ofs("kernel.list", std::ofstream::app);
        ofs << file_name << endl;
        ofs.close();
      }
      //  create basic block

      void *loop_node = codegen->CopyStmt(loop);
      //  youxiang 20160927 bug546
      codegen->DiscardPreprocInfo(loop_node);

      void *basic_block = codegen->CreateBasicBlock();
      codegen->PrependChild(basic_block, loop_node);

      //  generate variable declaration
      //  create a new init_name, create a new variable ref
      //  judge old var_ref's init_name, if match replace with the new one
      //  if global, prepend in new_file
      //  if not global, not in loop body, prepend in function body
      //  if in loop body, do nothing
      vector<void *> vec_ref_exp;
      map<string, int> used_var;
      map<void *, void *> old_init_name2new_var;
      codegen->GetNodesByType(loop_node, "preorder", "SgVarRefExp",
                              &vec_ref_exp);
      for (size_t k = 0; k < vec_ref_exp.size(); k++) {
        void *var_decl = codegen->GetVariableDecl(vec_ref_exp[k]);
        void *init_name = codegen->GetVariableInitializedName(var_decl);
        if (codegen->IsInitName(init_name) != 0) {
          string var_name = codegen->GetVariableName(init_name);
          if (find(share_memory_name.begin(), share_memory_name.end(),
                   var_name) != share_memory_name.end()) {
            continue;
          }
          if ((codegen->IsInScope(var_decl, loop_node) == 0) &&
              ((codegen->IsFromInputFile(var_decl) != 0) ||
               codegen->IsTransformation(var_decl))) {
            if (old_init_name2new_var.find(init_name) ==
                old_init_name2new_var.end()) {
              void *old_type = codegen->GetTypebyVar(init_name);
              void *initializer = codegen->GetInitializer(init_name);
              string old_type_str = codegen->UnparseToString(
                  codegen->GetBaseType(old_type, false));
              void *new_var_decl = codegen->CreateVariableDecl(
                  old_type, var_name,
                  initializer != nullptr ? codegen->CopyExp(initializer)
                                         : nullptr,
                  new_file);
              if (codegen->IsGlobalInitName(init_name) != 0) {
                string type_define =
                    "\ntypedef int __merlin_channel_typedef;\n";
                codegen->AddDirectives(type_define, new_var_decl);
                codegen->PrependChild(new_file, new_var_decl);
              } else {
                if (codegen->is_register(var_decl) != 0) {
                  codegen->set_register(new_var_decl);
                }
                codegen->PrependChild(basic_block, new_var_decl);
              }
              old_init_name2new_var[init_name] = new_var_decl;
              used_var[var_name] = 1;
#if DEBUG
              //  printf("init_name: %s\n",
              //  codegen->UnparseToString(init_name).c_str());
              //  printf("decl_name: %s\n",
              //  codegen->UnparseToString(new_var_decl).c_str());
              //  printf("var_name_use = %s\n",var_name.c_str());
#endif
            }
            codegen->ReplaceExp(
                vec_ref_exp[k],
                codegen->CreateVariableRef(old_init_name2new_var[init_name]));
          }
        }
      }

      //  generate new file, and write function to file
      //  remove(file_name.c_str());
      task_wait = task_wait + " " + func_name;
      //  printf("task_wait = %s\n", task_wait.c_str());
      vec_file_name.push_back(file_name);
      vec_func_name.push_back(func_name);
      string pragma_link;
      string pragma_pass = "\n";
      vector<string> vec_name;
      vector<void *> vec_type;
      map<void *, int> old_var2index;
      for (map<void *, bool>::iterator jt = ports.begin(); jt != ports.end();
           ++jt) {
        void *port = jt->first;
#if DEBUG
        cout << "port : " << codegen->UnparseToString(port) << "("
             << (jt->second ? "external port" : "shared port") << ")" << endl;
#endif
        string port_name = codegen->UnparseToString(port);
        void *var_type = codegen->GetTypebyVar(port);
        if ((codegen->IsArrayType(var_type) != 0) ||
            (codegen->IsPointerType(var_type) != 0)) {
          void *basic_type;
          vector<size_t> basic_size;
          codegen->get_type_dimension(var_type, &basic_type, &basic_size, port);
          size_t total_size = 1;
          for (auto size : basic_size) {
            total_size *= size;
          }
          basic_type = codegen->GetBaseType(basic_type, false);
          void *point_type = codegen->CreatePointerType(basic_type);
          var_type = point_type;
          string new_var_name = "__sm_" + port_name;
          sm_size[func_name][new_var_name] = total_size;

          //  check
          for (size_t m = 0; m < vecTldmPragmas.size(); m++) {
            string sFilter;
            string sCmd;
            map<string, pair<vector<string>, vector<string>>> mapParams;
            tldm_pragma_parse_whole(vecTldmPragmas[m].second, &sFilter, &sCmd,
                                    &mapParams);
            if (("ACCEL" != sFilter) || ("interface" != sCmd)) {
              continue;
            }

            string interface_type;
            string variable_name;
            string link_depth;
            if (!mapParams["type"].first.empty()) {
              interface_type = mapParams["type"].first[0];
            } else {
              interface_type = "globalmemory";
            }
            if (!mapParams["variable"].first.empty()) {
              variable_name = mapParams["variable"].first[0];
            }
            if (!mapParams["depth"].first.empty()) {
              link_depth = mapParams["depth"].first[0];
            }
            //  printf("type = %s\n", interface_type.c_str());
            //  printf("variable = %s\n", variable_name.c_str());
            //  printf("depth = %s\n", link_depth.c_str());
            if (variable_name == port_name &&
                interface_type == "globalmemory") {
              pragma_link = "#pragma " + vecTldmPragmas[m].second;
#if DEBUG
              printf(" Find a link pragma = %s!\n", pragma_link.c_str());
#endif
              pragma_pass += pragma_link + "\n";
            }
          }
        } else {
          var_type = codegen->GetBaseType(var_type, false);
        }
        old_var2index[port] = vec_name.size();
        vec_name.push_back(port_name);
        vec_type.push_back(var_type);
      }
      codegen->AddDirectives(pragma_pass, loop_node);
      void *new_func_decl =
          codegen->CreateFuncDecl(codegen->GetTypeByString("void"), func_name,
                                  vec_name, vec_type, new_file, true, loop);
      codegen->SetFuncBody(new_func_decl, basic_block);
      codegen->AppendChild(new_file, new_func_decl);
      //  setFuncDeclUserCodeScopeId(codegen, new_func_decl, loop);
      //  replace port variable references
      vec_ref_exp.clear();
      codegen->GetNodesByType(loop_node, "preorder", "SgVarRefExp",
                              &vec_ref_exp);
      for (auto &var_ref : vec_ref_exp) {
        void *old_var = codegen->GetVariableInitializedName(var_ref);
        if (old_var == nullptr) {
          continue;
        }
        if (old_var2index.count(old_var) <= 0) {
          continue;
        }
        int index = old_var2index[old_var];
        void *new_var = codegen->GetFuncParam(new_func_decl, index);
        codegen->ReplaceExp(var_ref, codegen->CreateVariableRef(new_var));
      }

      //  youxiang 20161101 we leave enum and union to HLS tool
      string wrapper_head_file = "__merlinheadwrapper_" + kernel_name + ".h";
      if (test_file_for_read(wrapper_head_file) != 0) {
        string include_type_def = "\n#include \"" + wrapper_head_file + "\"\n";
        codegen->AddDirectives(include_type_def, new_func_decl);
      }
      string include_def = "\n#include \"channel_def.h\"\n";
      codegen->AddDirectives(include_def, new_func_decl);

#if 1  //  ZP 20160919
      include_def = "\n#include \"stdio.h\"\n";
      codegen->AddDirectives(include_def, new_func_decl);
      include_def = "\n#include <assert.h>\n";
      codegen->AddDirectives(include_def, new_func_decl);
      //            include_def = "\n#include \"math.h\"\n";
      //            codegen->AddDirectives(include_def, new_func_decl);
      include_def = "\n#include \"__merlin_math.h\"\n";
      codegen->AddDirectives(include_def, new_func_decl);
#endif

      //  add access intrinsic declaration
      codegen->AddDirectives("\n#include \"cmost.h\"\n", new_func_decl);

      FILE *fp = fopen("altera_const.cl", "r");
      if (fp != NULL) {
        fclose(fp);
        string include_const = "\n//#include \"altera_const.cl\"\n";
        codegen->AddDirectives(include_const, new_func_decl);
      }
      FILE *fp1 = fopen("merlin_typedef.h", "r");
      if (fp1 != NULL) {
        fclose(fp1);
        string include_const = "\n#include \"merlin_typedef.h\"\n";
        codegen->AddDirectives(include_const, new_func_decl);
      }

      string pragma_outline =
          "\n#pragma ACCEL kernel name = \"" + func_name + "\"";
      codegen->AddDirectives(pragma_outline, new_func_decl);
      j++;

      //  generate ivdep pragma
      vector<void *> vec_call;
      codegen->GetNodesByType(loop_node, "preorder", "SgFunctionCallExp",
                              &vec_call);
      for (auto &call : vec_call) {
        string func_call_name = codegen->GetFuncNameByCall(call, false);
        if (func_call_name.find("__merlin_ivdep_") != std::string::npos) {
          //  printf("func_call_name = %s\n",func_call_name.c_str());

          void *param_var = codegen->GetFuncCallParam(call, 0);
          string pragma = "\n#pragma ivdep array (" +
                          codegen->UnparseToString(param_var) + ")";
#if DEBUG
          printf("var_name: %s\n", pragma.c_str());
#endif
          codegen->AddDirectives(pragma, loop_node);
          void *call_stmt = codegen->TraceUpToStatement(call);
          codegen->RemoveStmt(call_stmt);
        }
      }
    }

    //  remove all ivdep specs in the original function body
    vector<void *> vec_ivdep_call;
    codegen->GetNodesByType(func_body, "preorder", "SgFunctionCallExp",
                            &vec_ivdep_call);
    for (auto &call : vec_ivdep_call) {
      string func_call_name = codegen->GetFuncNameByCall(call, false);
      if (func_call_name.find("__merlin_ivdep_") != std::string::npos) {
        //  printf("func_call_name = %s\n",func_call_name.c_str());
        void *call_stmt = codegen->TraceUpToStatement(call);
        codegen->RemoveStmt(call_stmt);
      }
    }
    //  modify host program
    string match_port;
    string single_port;
    void *old_decl = codegen->TraceUpToFuncDecl(sg_task);

    vector<void *> call_param_node;
    int call_param_number = codegen->GetFuncCallParamNum(sg_task);
    for (int m = 0; m < call_param_number; m++) {
      void *param_node = codegen->GetFuncCallParam(sg_task, m);
      call_param_node.push_back(param_node);
    }
    vector<string> decl_param_name;
    int decl_param_number = codegen->GetFuncParamNum(sg_kernel);
    for (int m = 0; m < decl_param_number; m++) {
      void *param_node = codegen->GetFuncParam(sg_kernel, m);
      decl_param_name.push_back(codegen->GetVariableName(param_node));
    }

    map<string, void *> param_string;
    for (int m = 0; m < decl_param_number; m++) {
      void *node = call_param_node[m];
      string name = decl_param_name[m];
      param_string[name] = node;
    }

    vector<string> sm_name;
    for (size_t m = 0; m < vec_func_name.size(); m++) {
      void *scope = codegen->GetScope(sg_task);
      string func_call_name = vec_func_name[vec_func_name.size() - m - 1];
      void *func_decl = codegen->GetFuncDeclByName(func_call_name);
      void *func_return_type = codegen->GetFuncReturnType(func_decl);
      int param_number = codegen->GetFuncParamNum(func_decl);
      vector<void *> param_list;
      vector<void *> param_list_decl;
      //  vector<string> new_type, new_name;
      vector<string> new_name;
      vector<void *> new_type;
      for (int n = 0; n < param_number; n++) {
        void *call_node;
        void *param_node = codegen->GetFuncParam(func_decl, n);
        string param_name = codegen->GetVariableName(param_node);
        if (param_string.find(param_name) ==
            param_string.end()) {  //  share mamory
          param_name = "__sm_" + param_name;
          void *sm_type = codegen->GetTypebyVar(param_node);
          void *base_type = codegen->GetBaseType(sm_type, false);
          vector<size_t> vec_dim;
          vec_dim.push_back(sm_size[func_call_name][param_name]);
          void *array_type = codegen->CreateArrayType(base_type, vec_dim);
          if (find(sm_name.begin(), sm_name.end(), param_name) ==
              sm_name.end()) {
            sm_name.push_back(param_name);
            void *sm_decl = codegen->CreateVariableDecl(array_type, param_name,
                                                        NULL, scope);
            codegen->InsertStmt(sm_decl, sg_task_stmt);
          }
          vector<void *> all_init_name;
          codegen->GetNodesByType(scope, "preorder", "SgInitializedName",
                                  &all_init_name);
          for (size_t i = 0; i < all_init_name.size(); i++) {
            if (codegen->UnparseToString(all_init_name[i]) == param_name) {
              call_node = codegen->CreateVariableRef((all_init_name[i]));
            }
          }
        } else {  //  external memory
          call_node = param_string[param_name];
          for (int p = 0; p < call_param_number; p++) {
            void *param_node_org = codegen->GetFuncCallParam(sg_task, p);
            string param_name_org = codegen->GetVariableName(param_node_org);
            if (param_name_org == param_name) {
              match_port = match_port + func_call_name + " " + my_itoa(n) +
                           " " + my_itoa(p) + "\n";
            }
          }
        }
        param_list.push_back(call_node);
        new_name.push_back(param_name);
        //  new_type.push_back(codegen->GetStringByType(codegen->GetTypebyVar(param_node)));
        new_type.push_back(codegen->GetTypebyVar(param_node));
      }
      void *func_call = codegen->CreateFuncCall(
          func_call_name, func_return_type, param_list, scope);
      void *func_call_stmt = codegen->CreateStmt(V_SgExprStatement, func_call);
      codegen->InsertAfterStmt(func_call_stmt, sg_task_stmt);
      for (map<string, int>::iterator jt = sm_size[func_call_name].begin();
           jt != sm_size[func_call_name].end(); ++jt) {
        string depth = "\"" + my_itoa(jt->second) + "\"";
        string pragma_sm =
            "\n#pragma ACCEL interface type=\"sharememory\" variable=\"" +
            jt->first + "\" depth = " + depth;
        codegen->AddDirectives(pragma_sm, func_call_stmt);
      }
      string pragma_task =
          "\n#pragma ACCEL task name = \"" + func_call_name + "\" async\n";
      codegen->AddDirectives(pragma_task, func_call_stmt);
      if (m == 0) {
        codegen->AddDirectives("\n#pragma ACCEL taskwait " + task_wait + "\n",
                               func_call_stmt, 0);
      }
      void *func_decl_new = codegen->CreateFuncDecl(
          func_return_type, func_call_name, new_name, new_type,
          codegen->GetGlobal(scope), false, nullptr);

      //  add to the up of parent function
      //  codegen->PrependChild(codegen->GetGlobal(scope), func_decl_new);
      codegen->InsertStmt(func_decl_new, old_decl);
    }
#if DEBUG
    printf("match_port = %s\n", match_port.c_str());
#endif
    codegen->RemoveStmt(sg_task_stmt);
    codegen->RemoveStmt(vecTldmPragmas[i].first);
    FILE *fp_port = fopen("port.cfg", "a");
    if (fp_port != NULL) {
      fprintf(fp_port, "%s", match_port.c_str());
      fclose(fp_port);
    }
#if 0
        void * scope = codegen->GetScope(sg_task);
        FILE *fp2 = fopen("merlin_typedef.cpp", "r");
        if (fp2 != NULL) {
            fclose(fp2);
            string include_typedef = "\n#include \"merlin_typedef.h\"\n";
            codegen->AddDirectives(include_typedef, codegen->GetGlobal(scope));
        }
#endif
  }

#if 1
  string flag_file_name =
      "__merlinoldkernel_" +
      file_name_org;  //  create a new file to remember the old kernel file name
  codegen->CreateSourceFile(flag_file_name);
#endif

  //  youxiang 20161222 work around ROSE limitation
  codegen->remove_double_brace();
  return 1;
}
