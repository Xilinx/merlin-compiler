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


#include "input_checker.h"
#include "mars_message.h"
#undef DEBUG
#define DISABLE_DEBUG 1
#if DISABLE_DEBUG
#define DEBUG(stmts)
#else
#define DEBUG(stmts)                                                           \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#endif

void InputChecker::PostCheck() {
  CheckFunctionPointerTop();
  // PostCheckLocalVariableWithNonConstantDimension();
  CheckValidTop();
  check_result();
}

void InputChecker::InputChecker::PreCheck() {
  CheckOldStyle();
  ExpandDefineDirectiveInPragma();
  build_mars_ir(true, false, true);
  build_mars_ir_v2(false);
  CheckOverloadInKernelFunction();
  PreCheckLocalVariableWithNonConstantDimension();
  CheckInterfacePragma();
  check_result();
}

void InputChecker::check_result() {
  if (!mValid) {
    throw std::exception();
  }
}

void InputChecker::build_mars_ir(bool check_pragma, bool pragma_in_loop,
                                 bool build_node) {
  //  build Mars IR
  mMars_ir.clear();
  mMars_ir.get_mars_ir(codegen, pTopFunc, options, build_node, check_pragma,
                       pragma_in_loop);

  mValid &= !mMars_ir.isErrorOut();
}

void InputChecker::build_mars_ir_v2(bool build_node) {
  //  build Mars IR v2
  mMars_ir_v2.clear();
  mMars_ir_v2.build_mars_ir(codegen, pTopFunc, false, build_node);
}

void InputChecker::init() {
  // if ("naive_hls" == options.get_option_key_value("-a", "naive_hls")) {
  //   mNaive_tag = true;
  //   cout << "[MARS-PARALLEL-MSG] Naive HLS mode.\n";
  // }
  if ("sdaccel" == options.get_option_key_value("-a", "impl_tool")) {
    mXilinx_flow = true;
    mHls_flow = false;
    string tool_version = options.get_option_key_value("-a", "tool_version");
    if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
      mHls_flow = true;
    }
    cout << "[MARS-PARALLEL-MSG] Enable xilinx flow mode.\n";
  }
  if ("on" == options.get_option_key_value("-a", "pure_kernel")) {
    mPure_kernel = true;
    cout << "[MARS-PARALLEL-MSG] Enable pure kernel mode.\n";
  }

  if ("test_precheck" == options.get_option_key_value("-a", "test_precheck")) {
    return;
  }
}

void InputChecker::CheckMixedCCPPDesign() {
  if (codegen->IsMixedCAndCPlusPlusDesign() != 0) {
    dump_critical_message(SYNCHK_ERROR_3);
    mValid = false;
  }
}

void InputChecker::CheckRangeAnalysis() {
  auto top_kernels = GetTopKernels();
  set<void *> all_kernels;
  for (auto kernel : top_kernels) {
    all_kernels.insert(kernel);
    SetVector<void *> sub_kernels;
    SetVector<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(kernel, &sub_kernels, &sub_calls);
    all_kernels.insert(sub_kernels.begin(), sub_kernels.end());
  }
  int ret = constant_propagation_top(codegen, pTopFunc, options, false);
  if (ret != 0) {
    codegen->reset_range_analysis();
  }

  auto range_map = MarsProgramAnalysis::get_range_analysis(codegen);
  map<string, void *> loc_map;
  //  Sort by file location
  for (auto var_range : range_map) {
    void *var = var_range.first;
    string var_info = "\'" + codegen->_up(var) + "\'";
    var_info += " " + getUserCodeFileLocation(codegen, var, true);
    loc_map[var_info] = var;
  }
  for (auto var_loc : loc_map) {
    string var_info = var_loc.first;
    void *var = var_loc.second;
    CMarsRangeExpr range = range_map[var];
    if (isSgVarRefExp(static_cast<SgNode *>(var)) != nullptr) {
      continue;
    }
    void *func_decl = codegen->TraceUpToFuncDecl(var);
    if ((func_decl == nullptr) || all_kernels.count(func_decl) <= 0) {
      continue;
    }
    if ((range.has_lb() != 0) && (range.has_ub() != 0)) {
      continue;
    }
    if ((range.has_lb() == 0) && (range.has_ub() == 0)) {
      continue;
    }
    bool no_lb = range.has_lb() == 0;
    dump_critical_message(SYNCHK_WARNING_3(no_lb, var_info));
  }
}

bool InputChecker::CheckOpenclConflictName() {
  printf("    Enter check opencl conflict name...\n\n");
  int ret = 0;
  vector<string> OpenCLNameConflict = codegen->GetOpenCLName();
  //  Check all function name and argument name
  vector<void *> all_decls;
  codegen->GetNodesByType_compatible(nullptr, "SgFunctionDeclaration",
                                     &all_decls);
  for (auto decl : all_decls) {
    if (codegen->IsFromInputFile(decl) != 0) {
      string func_name = codegen->GetFuncName(decl);
      if (find(OpenCLNameConflict.begin(), OpenCLNameConflict.end(),
               func_name) != OpenCLNameConflict.end()) {
        string msg = "\"" + func_name + "\" in ";
        msg += codegen->UnparseToString(decl);
        msg += getUserCodeFileLocation(codegen, decl, true);
        dump_critical_message(SYNCHK_ERROR_4(msg));
        ret = 1;
      }
      void *return_type = codegen->GetFuncReturnType(decl);
      string return_string =
          codegen->GetStringByType(codegen->GetBaseType(return_type, false));
      if (find(OpenCLNameConflict.begin(), OpenCLNameConflict.end(),
               return_string) != OpenCLNameConflict.end()) {
        string msg = "\"" + return_string + "\" in ";
        msg += codegen->UnparseToString(decl);
        msg += getUserCodeFileLocation(codegen, decl, true);
        dump_critical_message(SYNCHK_ERROR_4(msg));
        ret = 1;
      }
      for (auto arg : codegen->GetFuncParams(decl)) {
        string arg_name = codegen->GetVariableName(arg);
        if (find(OpenCLNameConflict.begin(), OpenCLNameConflict.end(),
                 arg_name) != OpenCLNameConflict.end()) {
          string msg = "\"" + arg_name + "\" in ";
          msg += codegen->UnparseToString(decl);
          msg += getUserCodeFileLocation(codegen, decl, true);
          dump_critical_message(SYNCHK_ERROR_4(msg));
          ret = 1;
        }
        string arg_type_name = codegen->GetStringByType(
            codegen->GetBaseType(codegen->GetTypebyVar(arg), false));
        if (find(OpenCLNameConflict.begin(), OpenCLNameConflict.end(),
                 arg_type_name) != OpenCLNameConflict.end()) {
          string msg = "\"" + arg_type_name + "\" in ";
          msg += codegen->UnparseToString(decl);
          msg += getUserCodeFileLocation(codegen, decl, true);
          dump_critical_message(SYNCHK_ERROR_4(msg));
          ret = 1;
        }
      }
    }
  }
  //  Check variable name
  all_decls.clear();
  codegen->GetNodesByType_int(nullptr, "preorder", V_SgVariableDeclaration,
                              &all_decls);
  for (auto decl : all_decls) {
    if (codegen->IsFromInputFile(decl) != 0 ||
        codegen->get_file(decl) == MERLIN_TYPE_DEFINE) {
      string var_name = codegen->GetVariableName(decl);
      if (find(OpenCLNameConflict.begin(), OpenCLNameConflict.end(),
               var_name) != OpenCLNameConflict.end()) {
        string msg = "\"" + var_name + "\" in ";
        msg += codegen->UnparseToString(decl);
        msg += getUserCodeFileLocation(codegen, decl, true);
        dump_critical_message(SYNCHK_ERROR_4(msg));
        ret = 1;
      }
      string var_type_name = codegen->GetStringByType(codegen->GetBaseType(
          codegen->GetTypebyVar(codegen->GetVariableInitializedName(decl)),
          false));
      if (find(OpenCLNameConflict.begin(), OpenCLNameConflict.end(),
               var_type_name) != OpenCLNameConflict.end()) {
        string msg = "\"" + var_type_name + "\" in ";
        msg += codegen->UnparseToString(decl);
        msg += getUserCodeFileLocation(codegen, decl, true);
        dump_critical_message(SYNCHK_ERROR_4(msg));
        ret = 1;
      }
    }
  }
  all_decls.clear();
  vector<void *> vec_exp;
  codegen->GetNodesByType_compatible(nullptr, "SgExpression", &vec_exp);
  if (ret == 1) {
    throw std::exception();
  }
  return false;
}

void InputChecker::CheckOverloadInKernelFunction() {
  cout << "check overload function" << endl;
  vector<CMirNode *> kernels;
  set<CMirNode *> all_nodes;
  vector<CMirNode *> all_nodes_vec;
  mMars_ir.get_all_kernel_nodes(&kernels, &all_nodes, &all_nodes_vec, true);
  for (size_t i = 0; i != all_nodes_vec.size(); ++i) {
    CMirNode *node = all_nodes_vec[i];
    if (!node->is_function || node->is_kernel) {
      continue;
    }
    void *func_decl = codegen->TraceUpToFuncDecl(node->ref);
    for (size_t j = 0; j != kernels.size(); ++j) {
      CMirNode *kernel_node = kernels[j];
      void *kernel_decl = codegen->TraceUpToFuncDecl(kernel_node->ref);
      if (codegen->GetFuncName(func_decl, true) ==
          codegen->GetFuncName(kernel_decl, true)) {
        string func_info = "'" + codegen->GetFuncName(func_decl, true) + "' " +
                           getUserCodeFileLocation(codegen, func_decl, true);
        dump_critical_message(SYNCHK_ERROR_6(func_info));
        mValid = false;
        break;
      }
    }
  }
}

void InputChecker::PreCheckLocalVariableWithNonConstantDimension() {
  printf("    Enter check local variable with non-constant dimension ...\n");

  vector<void *> vec_funcs;
  codegen->GetNodesByType_int_compatible(nullptr, V_SgFunctionDeclaration,
                                         &vec_funcs);
  for (auto func : vec_funcs) {
    if (!mMars_ir_v2.is_used_by_kernel(func) ||
        (codegen->IsTemplateFunctionDecl(func) != 0) ||
        (codegen->IsTemplateMemberFunctionDecl(func) != 0)) {
      continue;
    }
    CheckLocalVariableWithNonConstantDimension(func);
  }
}

void InputChecker::PostCheckLocalVariableWithNonConstantDimension() {
  printf("    Enter check local variable with non-constant dimension ...\n");
  vector<void *> vecTopKernels = GetTopKernels();

  set<void *> visited_funcs;
  for (auto &func : vecTopKernels) {
    vector<void *> vec_funs;
    codegen->GetNodesByType(func, "preorder", "SgFunctionDeclaration",
                            &vec_funs, false, true);
    for (auto one_func : vec_funs) {
      if ((codegen->IsTemplateFunctionDecl(one_func) != 0) ||
          (codegen->IsTemplateMemberFunctionDecl(one_func) != 0)) {
        continue;
      }
      if (visited_funcs.count(one_func) > 0) {
        continue;
      }
      visited_funcs.insert(one_func);
      CheckLocalVariableWithNonConstantDimension(one_func);
    }
  }
}

void InputChecker::CheckLocalVariableWithNonConstantDimension(void *func) {
  vector<void *> local_decl;
  codegen->GetNodesByType_int(func, "preorder", V_SgVariableDeclaration,
                              &local_decl);
  for (auto var_decl : local_decl) {
    void *var_init = codegen->GetVariableInitializedName(var_decl);
    void *sg_type = codegen->GetTypebyVar(var_init);
    void *var_type = sg_type;
    void *init_type = nullptr;
    void *initializer = codegen->GetInitializer(var_init);
    if (initializer != nullptr) {
      sg_type = codegen->GetTypeByExp(initializer);
      init_type = codegen->GetTypeByExp(initializer);
    }
    if ((codegen->IsArrayType(sg_type) == 0) &&
        (codegen->IsPointerType(sg_type) == 0)) {
      continue;
    }
    DEBUG(cout << "var_type = " << codegen->UnparseToString(var_type) << endl;
          cout << "init_type = " << codegen->UnparseToString(init_type)
               << endl);
    // if user did not write the size of array, we rewrite the size
    if (codegen->IsArrayType(var_type) && codegen->IsArrayType(init_type)) {
      codegen->SetTypetoVar(var_init, init_type);
    }
    void *base_type;
    vector<size_t> nSizes;
    codegen->get_type_dimension(sg_type, &base_type, &nSizes, var_init);
    int num_dim = nSizes.size();
    if (num_dim <= 0) {
      continue;
    }
    for (auto dim : nSizes) {
      if (codegen->IsUnknownSize(dim)) {
        string var_info = "'" + codegen->GetVariableName(var_decl) + "' " +
                          getUserCodeFileLocation(codegen, var_init, true);
        dump_critical_message(SYNCHK_ERROR_7(var_info));
        mValid = false;
        break;
      }
      if (dim == 0) {
        if (codegen->IsCharType(base_type) && codegen->IsArrayType(var_type)) {
          string var_info = "'" + codegen->GetVariableName(var_decl) + "' " +
                            getUserCodeFileLocation(codegen, var_init, true);
          dump_critical_message(SYNCHK_ERROR_8(var_info));
          mValid = false;
          break;
        }
      }
      if (codegen->IsEmptySize(dim)) {
        string var_info = "'" + codegen->GetVariableName(var_decl) + "' " +
                          getUserCodeFileLocation(codegen, var_init, true);
        dump_critical_message(SYNCHK_ERROR_9(var_info));
        mValid = false;
        break;
      }
    }
  }

  for (auto arg : codegen->GetFuncParams(func)) {
    void *sg_type = codegen->GetTypebyVar(arg);
    if ((codegen->IsArrayType(sg_type) == 0) &&
        (codegen->IsPointerType(sg_type) == 0)) {
      continue;
    }
    void *base_type;
    vector<size_t> nSizes;
    codegen->get_type_dimension(sg_type, &base_type, &nSizes, arg);
    int num_dim = nSizes.size();
    if (num_dim <= 0) {
      continue;
    }
    for (auto dim : nSizes) {
      if (codegen->IsUnknownSize(dim)) {
        string var_info = "'" + codegen->GetVariableName(arg) + "' " +
                          getUserCodeFileLocation(codegen, arg, true);
        dump_critical_message(SYNCHK_ERROR_7(var_info));
        mValid = false;
        break;
      }
    }
  }
}
void InputChecker::CheckAssertHeader() {
  SgProject *project = static_cast<SgProject *>(pTopFunc);
  for (size_t i = 0; i < project->get_fileList().size(); i++) {
    SgSourceFile *file = isSgSourceFile(project->get_fileList()[i]);
    if ((codegen->IsFromInputFile(file) == 0) ||
        codegen->isWithInHeaderFile(file)) {
      continue;
    }
    //  1. check missing assert.h header file
    vector<void *> vecCall;
    codegen->GetNodesByType_int(file, "preorder", V_SgFunctionCallExp,
                                &vecCall);
    for (size_t i = 0; i != vecCall.size(); ++i) {
      void *call = vecCall[i];
      if (codegen->GetFuncNameByCall(call) == "assert") {
        void *func_decl = codegen->GetFuncDeclByCall(call);
        if (func_decl == nullptr) {
          string file_info = "\'" + codegen->get_file(call) + "\'";
          dump_critical_message(SYNCHK_ERROR_10(file_info));

          mValid = false;
          break;
        }
      }
    }
  }
}

void InputChecker::CheckMemberFunction() {
  set<void *> visited_funcs;
  for (auto kernel : GetTopKernels()) {
    SetVector<void *> decls;
    SetVector<void *> calls;
    codegen->GetSubFuncDeclsRecursively(kernel, &decls, &calls, false);
    vector<void *> vec_decls(decls.begin(), decls.end());
    sort(vec_decls.begin(), vec_decls.end(), [&](auto a, auto b) {
      return codegen->GetFuncName(a) < codegen->GetFuncName(b);
    });

    for (auto func_decl : vec_decls) {
      if (visited_funcs.count(func_decl) > 0) {
        continue;
      }
      visited_funcs.insert(func_decl);
      if (!codegen->UnSupportedMemberFunction(func_decl)) {
        continue;
      }
      string func_info = "'" + codegen->GetFuncName(func_decl, true) + "' " +
                         getUserCodeFileLocation(codegen, func_decl, true);
      dump_critical_message(SYNCHK_WARNING_4(func_info));
    }
  }
}

void InputChecker::CheckKernelArgument(void *pKernelTop) {
  int i;
  void *sg_func_decl = pKernelTop;
  for (i = 0; i < codegen->GetFuncParamNum(sg_func_decl); i++) {
    void *arg = codegen->GetFuncParam(sg_func_decl, i);
    void *sg_type = codegen->GetTypebyVar(arg);
    void *base_type;
    vector<size_t> nSizes;
    codegen->get_type_dimension(sg_type, &base_type, &nSizes, arg);
    if (!mPure_kernel && !mHls_flow) {
      if ((codegen->IsReferenceType(sg_type) != 0) ||
          (codegen->IsReferenceType(base_type) != 0)) {
        string detail = "argument: '" + codegen->UnparseToString(arg) + "' " +
                        getUserCodeFileLocation(codegen, arg, true);
        dump_critical_message(SYNCHK_ERROR_12(detail));
        mValid = false;
      }
    }
    if (codegen->IsXilinxStreamType(base_type)) {
      if ((!mPure_kernel && !mHls_flow) ||
          (!codegen->IsReferenceType(sg_type) &&
           !codegen->IsArrayType(sg_type) &&
           !codegen->IsPointerType(sg_type))) {
        string detail = "argument: '" + codegen->UnparseToString(arg) + "' " +
                        getUserCodeFileLocation(codegen, arg, true);
        dump_critical_message(SYNCHK_ERROR_38(detail));
        mValid = false;
      }
    }
    void *unsupported_type = nullptr;
    string reason;
    if (codegen->ContainsUnSupportedType(sg_type, &unsupported_type, &reason) != 0) {
      string detail =
          "Top function argument \'" + codegen->UnparseToString(arg) + "' " +
          getUserCodeFileLocation(codegen, arg, true) + " with type \'" +
          codegen->UnparseToString(sg_type) + " which contains " + reason +
          " (" + codegen->UnparseToString(unsupported_type) + ")";
      dump_critical_message(SYNCHK_ERROR_13(detail));
      mValid = false;
    }
  }
  //  check return type
  void *sg_type = codegen->GetFuncReturnType(sg_func_decl);
  if (codegen->IsVoidType(sg_type) != 0) {
    return;
  }
  void *base_type;
  vector<size_t> nSizes;
  codegen->get_type_dimension(sg_type, &base_type, &nSizes,
                              codegen->GetFuncBody(sg_func_decl));
  if ((codegen->IsReferenceType(sg_type) != 0) ||
      (codegen->IsReferenceType(base_type) != 0)) {
    string reason;
    string msg = "Cannot support reference type the kernel interface: ";
    string detail =
        "return type of '" + codegen->GetFuncName(sg_func_decl) + "' " +
        getUserCodeFileLocation(codegen, sg_func_decl, true) + " \n";
    dump_critical_message(SYNCHK_ERROR_12(detail));
    mValid = false;
  }

  void *unsupported_type = nullptr;
  string reason;
  string string_type;
  if (codegen->ContainsUnSupportedType(sg_type, &unsupported_type, &reason,
                                       true, &string_type) != 0) {
    string msg = "Cannot support the kernel interface port:";
    if (string_type == "") {
      string_type = codegen->UnparseToString(unsupported_type);
    }
    string detail = "Return type \'" + codegen->UnparseToString(sg_type) +

                    "\' of \'" + codegen->GetFuncName(sg_func_decl) + "\' " +
                    getUserCodeFileLocation(codegen, sg_func_decl, true) +
                    " which contains " + reason + " (" + string_type + ")";
    dump_critical_message(SYNCHK_ERROR_13(detail));
    mValid = false;
  }
}

int InputChecker::CheckKernelArgumentTop() {
  printf("    Enter check unsupported argument...\n");
  vector<void *> vecTopKernels = GetTopKernels();
  set<void *> visited_funcs;
  for (auto &func : vecTopKernels) {
    CheckKernelArgument(func);
  }
  return 0;
}

int InputChecker::CheckKernelArgumentNumber(vector<void *> vec_kernels,
                                            string tool) {
  int ret = 0;
  int arg_limitation = 16;
  arg_limitation = XILINX_ARG_LIMITATION;
  for (auto kernel : vec_kernels) {
    int count_scalar = 0;
    if (codegen->IsFunctionDeclaration(kernel) != 0) {
      int number = codegen->GetFuncParamNum(kernel);
      if (number > arg_limitation) {
        string msg = codegen->UnparseToString(kernel);
        msg += getUserCodeFileLocation(codegen, kernel, true);
        dump_critical_message(SYNCHK_ERROR_14(msg, std::to_string(number),
                                              std::to_string(arg_limitation)));
        ret = 1;
      }
      auto port_list = codegen->GetFuncParams(kernel);
      for (auto sg_port : port_list) {
        if (sg_port == nullptr) {
          continue;
        }
        void *sg_type = codegen->GetTypebyVar(sg_port);
        if (codegen->IsScalarType(sg_type) != 0) {
          count_scalar++;
        }
      }
      if (XILINX_ARG_SCALAR_LIMITATOIN < count_scalar &&
          (tool == "Xilinx" || tool == "sdaccel")) {
        string msg = codegen->UnparseToString(kernel);
        msg += getUserCodeFileLocation(codegen, kernel, true);
        dump_critical_message(
            SYNCHK_ERROR_15(msg, std::to_string(count_scalar),
                            std::to_string(XILINX_ARG_SCALAR_LIMITATOIN)));
        ret = 1;
      }
    }
  }
  if (ret == 1) {
    throw std::exception();
  }
  return 0;
}

int InputChecker::CheckKernelArgumentNumberTop() {
  printf("    Enter check kernel argument number...\n");
  string tool = "Xilinx";
  vector<void *> vec_kernels = GetTopKernels();
  CheckKernelArgumentNumber(vec_kernels, tool);
  return 0;
}

void InputChecker::CheckValidTop(void *func, set<void *> *p_visited, int top) {
  static const string xilinx_supported_builtin_func[] = {
      "__assert_fail",
      "__builtin_inff",
      "exit",
      "memset",
#include "builtin.list"
  };

  if (p_visited->count(func) > 0) {
    return;
  }

  static set<string> xilinx_supported_builtin_func_set;
  static int init_xilinx_set = 0;
  if (init_xilinx_set == 0) {
    for (auto builtin_func : xilinx_supported_builtin_func) {
      xilinx_supported_builtin_func_set.insert(builtin_func);
    }
    init_xilinx_set = 1;
  }

  p_visited->insert(func);

  if (codegen->IsSystemFunction(func) != 0) {
    string func_info = "\'" + codegen->GetFuncName(func) + "\' ";
    func_info += getUserCodeFileLocation(codegen, func, true);
    dump_critical_message(SYNCHK_ERROR_16(func_info));
    mValid = false;
    return;
  }

  vector<void *> vec_gotos;
  codegen->GetNodesByType_int(codegen->GetFuncBody(func), "preorder",
                              V_SgGotoStatement, &vec_gotos);
  printf("    Enter goto statement1...\n");
  DEBUG(cout << codegen->_p(codegen->GetFuncBody(func)) << endl);
  if (!vec_gotos.empty()) {
    printf("    goto...\n");
    for (auto &goto_stmt : vec_gotos) {
      string loc = getUserCodeFileLocation(codegen, goto_stmt, true);
      dump_critical_message(SYNCHK_WARNING_6(loc));
    }
  }

  vector<void *> vec_calls;
  codegen->GetNodesByType_int(func, "preorder", V_SgFunctionCallExp,
                              &vec_calls);
  for (auto &call : vec_calls) {
    if (codegen->IsFunctionPointerCall(call) != 0) {
      continue;
    }
    if (mXilinx_flow &&
        (codegen->IsSupportedMerlinSystemFunctionCall(call) != 0)) {
      continue;
    }
    void *func_decl = codegen->GetFuncDeclByCall(call);

    string func_name = codegen->GetFuncNameByCall(call);
    if (func_decl == nullptr) {
      // TODO(youxiang): Add by Yuxin, temporary solution for merlin_stream
      if (func_name.find("merlin_stream_reset") != string::npos ||
          func_name.find("merlin_stream_init") != string::npos ||
          func_name.find("merlin_stream_read") != string::npos ||
          func_name.find("merlin_stream_write") != string::npos) {
        continue;
      }
      void *decl = codegen->GetFuncDeclByCall(call, 0);
      if (mXilinx_flow &&
          (xilinx_supported_builtin_func_set.count(func_name) > 0 ||
           codegen->IsXilinxIntrinsic(decl))) {
        continue;
      }
      if (codegen->IsSystemFunction(decl) == 0) {
        if (codegen->IsTemplateInstantiationMemberFunctionDecl(decl) != 0) {
          void *tf = codegen->GetTemplateMemFuncDecl(decl);
          if ((tf != nullptr) &&
              (isSgTemplateMemberFunctionDeclaration(static_cast<SgNode *>(tf))
                   ->get_definition() != nullptr)) {
            continue;
          }
        }
        string func_info = "\'" + codegen->DemangleName(func_name) + "\' ";
        if (codegen->IsCompilerGenerated(call)) {
          auto *first_non_compiler_exp = call;
          while (codegen->IsCompilerGenerated(first_non_compiler_exp)) {
            first_non_compiler_exp = codegen->GetParent(first_non_compiler_exp);
          }
          func_info +=
              getUserCodeFileLocation(codegen, first_non_compiler_exp, true);
        } else {
          func_info += getUserCodeFileLocation(codegen, call, true);
        }
        string decl_info;
        if (codegen->IsMemberFunction(decl)) {
          if (codegen->IsAssignOperator(decl) || codegen->IsConstructor(decl)) {
            // skip assign operator and constructor
            continue;
          }
          // TODO(youxiang): should we promote to codegen?
          auto *m_fn_decl = static_cast<SgMemberFunctionDeclaration *>(decl);
          SgClassDeclaration *cls_decl = static_cast<SgClassDeclaration *>(
              m_fn_decl->get_associatedClassDeclaration());
          string cls_name = cls_decl->get_name().getString();
          decl_info = "\'" + cls_name + "\' " +
                      getUserCodeFileLocation(codegen, cls_decl, true);
          dump_critical_message(SYNCHK_ERROR_39(func_info, decl_info));
        } else {
          dump_critical_message(SYNCHK_ERROR_34(func_info));
        }
        mValid = false;
      }
      continue;
    }
    if (codegen->GetFuncParamNum(func_decl) !=
        codegen->GetFuncCallParamNum(call)) {
      string func_info = "\'" + func_name + "\' ";
      func_info += getUserCodeFileLocation(codegen, call, true);
      dump_critical_message(SYNCHK_ERROR_19(func_info));
      mValid = false;
      continue;
    }
    if (codegen->IsSystemFunction(func_decl) != 0) {
      string call_info = "\'" + codegen->_up(call) + "\' ";
      call_info += getUserCodeFileLocation(codegen, call, true);
      dump_critical_message(SYNCHK_ERROR_20(call_info));
      mValid = false;
      continue;
    }

    CheckValidTop(func_decl, p_visited, 0);
  }

  vector<void *> vec_func_refs;
  codegen->GetNodesByType_int(func, "preorder", V_SgFunctionRefExp,
                              &vec_func_refs);
  for (auto func_ref : vec_func_refs) {
    if (codegen->IsFunctionCall(codegen->GetParent(func_ref)) != 0) {
      continue;
    }
    string func_info =
        "\'" + codegen->DemangleName(codegen->GetFuncNameByRef(func_ref)) +
        "\' ";
    func_info += getUserCodeFileLocation(codegen, func_ref, true);
    dump_critical_message(SYNCHK_ERROR_21(func_info));
    mValid = false;
  }

  vector<void *> vec_asm_stmt;
  codegen->GetNodesByType_int(func, "preorder", V_SgAsmStmt, &vec_asm_stmt);
  for (auto stmt : vec_asm_stmt) {
    string asm_stmt_info = "'" + codegen->_up(stmt) + "' " +
                           getUserCodeFileLocation(codegen, stmt, true);
    dump_critical_message(SYNCHK_ERROR_22(asm_stmt_info));
    mValid = false;
  }

  vector<void *> vec_var_op;
  vector<void *> vec_var_arg_op;
  codegen->GetNodesByType_int(func, "preorder", V_SgVarArgOp, &vec_var_arg_op);
  vec_var_op.insert(vec_var_op.end(), vec_var_arg_op.begin(),
                    vec_var_arg_op.end());
  vector<void *> vec_var_arg_copy_op;
  codegen->GetNodesByType_int(func, "preorder", V_SgVarArgCopyOp,
                              &vec_var_arg_copy_op);
  vec_var_op.insert(vec_var_op.end(), vec_var_arg_copy_op.begin(),
                    vec_var_arg_copy_op.end());
  vector<void *> vec_var_arg_end_op;
  codegen->GetNodesByType_int(func, "preorder", V_SgVarArgEndOp,
                              &vec_var_arg_end_op);
  vec_var_op.insert(vec_var_op.end(), vec_var_arg_end_op.begin(),
                    vec_var_arg_end_op.end());
  vector<void *> vec_var_arg_start_one_op;
  codegen->GetNodesByType_int(func, "preorder", V_SgVarArgStartOneOperandOp,
                              &vec_var_arg_start_one_op);
  vec_var_op.insert(vec_var_op.end(), vec_var_arg_start_one_op.begin(),
                    vec_var_arg_start_one_op.end());
  vector<void *> vec_var_arg_start_op;
  codegen->GetNodesByType_int(func, "preorder", V_SgVarArgStartOp,
                              &vec_var_arg_start_op);
  vec_var_op.insert(vec_var_op.end(), vec_var_arg_start_op.begin(),
                    vec_var_arg_start_op.end());
  for (auto var_op : vec_var_op) {
    string var_op_info = "'" + codegen->_up(var_op) + "' " +
                         getUserCodeFileLocation(codegen, var_op, true);
    dump_critical_message(SYNCHK_ERROR_23(var_op_info));
    mValid = false;
  }
  //  Bug1777 temporary solution to error out
  // TODO(youxiang): support inner class later
  vector<void *> vec_class_decl;
  codegen->GetNodesByType_int_compatible(func, V_SgClassDeclaration,
                                         &vec_class_decl);
  for (auto class_decl : vec_class_decl) {
    string class_decl_info =
        "'" + codegen->_up(class_decl) + "' " +
        getUserCodeFileLocation(codegen,
                                codegen->TraceUpToStatement(class_decl), true);
    dump_critical_message(SYNCHK_ERROR_24(class_decl_info));
    mValid = false;
  }
}

int InputChecker::CheckValidTop() {
  printf("    Enter check unsupported statement...\n");
  codegen->reset_func_decl_cache();
  codegen->reset_func_call_cache();
  vector<void *> vecTopKernels = GetTopKernels();

  set<void *> visited_funcs;
  for (auto &func : vecTopKernels) {
    CheckValidTop(func, &visited_funcs, 1);
  }

  return 0;
}

int InputChecker::CheckAddressOfOnePortImpl(void *array, void *pos,
                                            const set<void *> ports_alias,
                                            bool top, set<void *> *p_visited) {
  DEBUG(cout << "Check address of -- " << codegen->_p(array) << " in "
             << codegen->_p(pos) << endl);

  if (p_visited->count(array) > 0) {
    return 1;
  }
  p_visited->insert(array);

  void *sg_name = array;
  vector<void *> v_refs = codegen->GetAllRefInScope(sg_name, pos);

  int ret = 1;
  int array_dim = 0;
  {
    void *base_type;
    vector<size_t> sizes;
    codegen->get_type_dimension(codegen->GetTypebyVar(sg_name), &base_type,
                                &sizes, sg_name);
    array_dim = sizes.size();
  }
  if (array_dim <= 0) {
    return ret;
  }

  for (auto ref : v_refs) {
    void *sg_array;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim = 0;

    codegen->parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr, &sg_indexes,
                                         &pointer_dim);
#if 0
    if (pointer_dim < 0) {
      void *func_scope = codegen->TraceUpToFuncDecl(pos);
      string str_array_info =
          "    Array: " + codegen->_up(codegen->GetTypebyVar(sg_array)) + " " +
          codegen->_up(sg_array) + " " +
          getUserCodeFileLocation(codegen, sg_array, true) + "\n";
      str_array_info += "    Reference: " + codegen->_up(sg_pntr) + " " +
                        getUserCodeFileLocation(codegen, sg_pntr, true) + "\n";
      str_array_info += "    In function: " + codegen->_up(func_scope) + " " +
                        getUserCodeFileLocation(codegen, func_scope, true) +
                        "\n";
      dump_critical_message(SYNCHK_ERROR_25(str_array_info));
      ret = 0;
      continue;
    }
#endif

    if ((top && (codegen->is_write_conservative(ref, false) != 0)) ||
        codegen->is_interface_alias_write(ref, ports_alias, top)) {
      void *func_scope = codegen->TraceUpToFuncDecl(pos);
      string str_array_info =
          "    Array: " + codegen->_up(codegen->GetTypebyVar(sg_array), 20) +
          " " + codegen->_up(sg_array) + " " +
          getUserCodeFileLocation(codegen, sg_array, true) + "\n";
      str_array_info +=
          "    Reference: " + codegen->_up(codegen->TraceUpToStatement(ref)) +
          " " + getUserCodeFileLocation(codegen, ref, true) + "\n";
      str_array_info += "    In function: " + codegen->_up(func_scope) + +" " +
                        getUserCodeFileLocation(codegen, func_scope, true) +
                        "\n";
      dump_critical_message(SYNCHK_ERROR_26(str_array_info));
      ret = 0;
      continue;
    }

    if (pointer_dim != array_dim) {
      void *new_array = nullptr;
      void *sg_alias_pos = nullptr;

      //  case 1: int ** a_lias = pntr(a);
      {
        void *var_name;
        void *value_exp;
        if (codegen->parse_assign_to_pntr(codegen->GetParent(sg_pntr),
                                          &var_name, &value_exp)) {
          if (value_exp == sg_pntr && (codegen->IsInitName(var_name) != 0)) {
            new_array = var_name;
            //  use TraceUpToFuncDecl will have some false positive
            //  use TraceUpToStatement will have some false negative
            //  sg_alias_pos = codegen->TraceUpToStatement(sg_pntr);
            sg_alias_pos = codegen->TraceUpToFuncDecl(sg_pntr);
          }
        }
      }

      //  case 2: func_call(pntr(a));
      {
        int arg_idx = codegen->GetFuncCallParamIndex(sg_pntr);
        if (-1 != arg_idx) {
          void *func_call = codegen->TraceUpToFuncCall(sg_pntr);
          assert(func_call);
          void *func_decl = codegen->GetFuncDeclByCall(func_call);

          if (func_decl != nullptr) {
            new_array = codegen->GetFuncParam(func_decl, arg_idx);
            sg_alias_pos = codegen->GetFuncBody(func_decl);
          }
        }
      }

      vector<void *> dummy_offset;
      if (new_array != nullptr) {
        ret &= CheckAddressOfOnePortImpl(new_array, sg_alias_pos, ports_alias,
                                         false, p_visited);
      } else {
        //  FIXME
        //  assert(0);
      }
    }
  }
  return ret;
}

int InputChecker::CheckAddressOfOnePort(void *array, void *pos,
                                        const set<void *> ports_alias,
                                        bool top) {
  set<void *> visited;
  return CheckAddressOfOnePortImpl(array, pos, ports_alias, top, &visited);
}

set<void *> InputChecker::GetPortAlias(void *var, void *pos,
                                       set<void *> *p_visited) {
  set<void *> ret;
  ret.insert(var);
  if (p_visited->count(var) > 0) {
    return ret;
  }
  p_visited->insert(var);

  vector<void *> v_refs = codegen->GetAllRefInScope(var, pos);

  int array_dim = 0;
  {
    void *base_type;
    vector<size_t> sizes;
    codegen->get_type_dimension(codegen->GetTypebyVar(var), &base_type, &sizes,
                                var);
    array_dim = sizes.size();
  }
  if (array_dim <= 0) {
    return ret;
  }

  for (auto ref : v_refs) {
    void *sg_array;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim = 0;
    codegen->parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr, &sg_indexes,
                                         &pointer_dim);
    if (pointer_dim != array_dim) {
      void *new_array = nullptr;
      void *sg_alias_pos = nullptr;

      //  case 1: int ** a_lias = pntr(a);
      {
        void *var_name;
        void *value_exp;
        if (codegen->parse_assign(codegen->GetParent(sg_pntr), &var_name,
                                  &value_exp) != 0) {
          if (value_exp == sg_pntr && (codegen->IsInitName(var_name) != 0)) {
            new_array = var_name;
            sg_alias_pos = codegen->TraceUpToStatement(sg_pntr);
          }
        }
      }

      //  case 2: func_call(pntr(a));
      {
        int arg_idx = codegen->GetFuncCallParamIndex(sg_pntr);
        if (-1 != arg_idx) {
          void *func_call = codegen->TraceUpToFuncCall(sg_pntr);
          assert(func_call);
          void *func_decl = codegen->GetFuncDeclByCall(func_call);

          if (func_decl != nullptr) {
            new_array = codegen->GetFuncParam(func_decl, arg_idx);
            sg_alias_pos = codegen->GetFuncBody(func_decl);
          }
        }
      }

      vector<void *> dummy_offset;
      if (new_array != nullptr) {
        auto next_ret = GetPortAlias(new_array, sg_alias_pos, p_visited);
        ret.insert(next_ret.begin(), next_ret.end());
      } else {
        //  FIXME
        //  assert(0);
      }
    }
  }
  return ret;
}

int InputChecker::CheckAddressOfPortArray() {
  printf("    Check address of port array...\n");

  auto kernels = GetTopKernels();

  int any_err = 0;

  for (auto kernel : kernels) {
    int param_num = codegen->GetFuncParamNum(kernel);
    vector<set<void *>> ports_alias(param_num);
    for (int i = 0; i < param_num; ++i) {
      set<void *> visited;
      auto port = codegen->GetFuncParam(kernel, i);
      set<void *> one_port_alias = GetPortAlias(port, kernel, &visited);
      ports_alias[i] = one_port_alias;
    }
    DEBUG(cout << "check port alias begin: " << endl;
          for (int i = 0; i < param_num; ++i) {
            cout << "port :" << i << " size: " << ports_alias[i].size() << endl;
            for (auto it : ports_alias[i]) {
              cout << codegen->_p(it) << endl;
            }
            cout << endl;
          });
    for (int i = 0; i < param_num; ++i) {
      auto port = codegen->GetFuncParam(kernel, i);
      string port_info = "\'" + codegen->GetVariableName(port) + "\' " +
                         getUserCodeFileLocation(codegen, port, true);
      for (int j = 0; j < param_num; ++j) {
        if (j == i) {
          continue;
        }
        auto curr_port = codegen->GetFuncParam(kernel, j);
        if (ports_alias[j].count(port) > 0) {
          any_err = 1;
          string curr_port_info =
              "\'" + codegen->GetVariableName(curr_port) + "\' " +
              getUserCodeFileLocation(codegen, curr_port, true);
          dump_critical_message(SYNCHK_ERROR_27(port_info, curr_port_info));
        }
      }
    }
    for (int i = 0; i < codegen->GetFuncParamNum(kernel); i++) {
      void *sg_name = codegen->GetFuncParam(kernel, i);
      any_err |= static_cast<int>(
          CheckAddressOfOnePort(sg_name, codegen->GetFuncBody(kernel),
                                ports_alias[i], true) == 0);
    }
  }
  if (any_err != 0) {
    mValid &= 0;
    return 0;
  }
  mValid &= 1;
  return 1;
}

vector<void *> InputChecker::GetTopKernels() {
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  vector<void *> vecTopKernels;
  for (size_t i = 0; i < vecTldmPragmas.size(); i++) {
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
    if (CMOST_TASK == sCmd) {
      void *sg_kernel_call = codegen->find_kernel_call(vecTldmPragmas[i].first);
      if (sg_kernel_call != nullptr) {
        void *kernel = codegen->GetFuncDeclByCall(sg_kernel_call);
        if (kernel != nullptr) {
          vecTopKernels.push_back(kernel);
        }
      }
      continue;
    }
    if (CMOST_KERNEL == sCmd) {
      void *sg_kernel_call = codegen->find_kernel_call(vecTldmPragmas[i].first);
      if (sg_kernel_call != nullptr) {
        void *kernel = codegen->GetFuncDeclByCall(sg_kernel_call);
        if (kernel != nullptr) {
          vecTopKernels.push_back(kernel);
        }
      }

      void *sg_kernel = codegen->GetNextStmt(vecTldmPragmas[i].first);
      if ((codegen->IsFunctionDeclaration(sg_kernel) != 0) &&
          (codegen->GetFuncBody(sg_kernel) != nullptr)) {
        if (std::find(vecTopKernels.begin(), vecTopKernels.end(), sg_kernel) ==
            vecTopKernels.end()) {
          vecTopKernels.push_back(sg_kernel);
        }
      }
      continue;
    }
  }
  return vecTopKernels;
}

bool InputChecker::CheckKernelName() {
  printf("    Enter check kernel name legality...\n\n");
  vector<void *> kernels = GetTopKernels();

  set<void *> visited;
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, false, false);
  bool valid = true;
  set<string> keywords;
  for (auto key : opencl_keyword) {
    keywords.insert(key);
  }
  for (auto key : hdl_keyword) {
    keywords.insert(key);
  }
  for (auto key : other_keyword) {
    keywords.insert(key);
  }
  for (auto key : internal_name) {
    keywords.insert(key);
  }
  //  only check kernel name, do not check kernel sub function name
  for (auto kernel : kernels) {
    string kernel_name = codegen->GetFuncName(kernel);
    if (std::find(keywords.begin(), keywords.end(), kernel_name) !=
            keywords.end() &&
        mPure_kernel) {
      dump_critical_message(SYNCHK_ERROR_28(kernel_name));
      throw std::exception();
    }
    if (kernel_name.find("__merlin") == 0) {
      dump_critical_message(SYNCHK_ERROR_29(kernel_name));
      throw std::exception();
    }
  }
  return valid;
}

void InputChecker::CheckFunctionPointer(void *func, set<void *> *p_visited) {
  if (p_visited->count(func) > 0) {
    return;
  }
  p_visited->insert(func);

  vector<void *> vec_calls;
  codegen->GetNodesByType_int(func, "preorder", V_SgFunctionCallExp,
                              &vec_calls);
  for (auto &call : vec_calls) {
    if (codegen->IsFunctionPointerCall(call) != 0) {
      string msg = "Cannot support function pointer call ";
      string func_info = getUserCodeFileLocation(codegen, call, true);
      dump_critical_message(SYNCHK_ERROR_30(func_info));
      mValid = false;
      continue;
    }
    void *sub_func = codegen->GetFuncDeclByCall(call);
    if (sub_func == nullptr) {
      continue;
    }
    CheckFunctionPointer(sub_func, p_visited);
  }
}

void InputChecker::CheckFunctionPointerTop() {
  printf("    Enter check function pointer statement...\n");
  vector<void *> vecTopKernels = GetTopKernels();

  set<void *> visited_funcs;
  for (auto &func : vecTopKernels) {
    CheckFunctionPointer(func, &visited_funcs);
  }
}

void InputChecker::CheckUserDefinedTypeOutsideHeaderFile() {
  printf("    Enter check user defined type definition...\n\n");
  vector<void *> vec_decls;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgClassDeclaration,
                              &vec_decls);
  vector<int> in_header_files(vec_decls.size());
  vector<string> type_name(vec_decls.size());
  for (size_t i = 0; i != vec_decls.size(); ++i) {
    in_header_files[i] = static_cast<
        __gnu_cxx::__alloc_traits<class std::allocator<int>>::value_type>(
        codegen->isWithInHeaderFile(vec_decls[i]));
    type_name[i] = codegen->GetTypeName(vec_decls[i], true);
  }
  set<int> checked;
  for (size_t i = 0; i != vec_decls.size(); ++i) {
    if (checked.count(i) > 0) {
      continue;
    }
    void *class_decl = vec_decls[i];
    void *class_def = codegen->GetClassDefinition(class_decl);
    //  1. no need to check forward declaration
    if (class_def == nullptr) {
      continue;
    }
    vector<int> duplicated;
    for (size_t j = i + 1; j != vec_decls.size(); ++j) {
      void *other_class_decl = vec_decls[j];
      void *other_class_def = codegen->GetClassDefinition(other_class_decl);
      //  2. no need to check forward declaration
      if (other_class_def == nullptr) {
        continue;
      }
      //  3. it is ok if both are in the header files
      if ((in_header_files[i] != 0) && (in_header_files[j] != 0)) {
        continue;
      }

      if (type_name[i] == type_name[j]) {
        duplicated.push_back(j);
      }
    }

    if (!duplicated.empty()) {
      checked.insert(duplicated.begin(), duplicated.end());
      if (codegen->IsFromInputFile(class_decl) == 0) {
        continue;
      }
      string msg =
          "Cannot support duplicate type definitions outside header files:\n";
      string type_info = codegen->_up(class_decl);
      dump_critical_message(SYNCHK_ERROR_31(type_info));
      mValid = false;
    }
  }
}

void InputChecker::CheckOldStyle() {
  printf("    Enter check old style function definition...\n\n");
  vector<void *> vecDecls;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgFunctionDeclaration,
                              &vecDecls);
  for (auto decl : vecDecls) {
    SgFunctionDeclaration *func_decl =
        static_cast<SgFunctionDeclaration *>(decl);
    string func_name = codegen->GetFuncName(func_decl, true);
    int param_num = codegen->GetFuncParamNum(func_decl);
    if (func_decl->get_oldStyleDefinition() &&
        param_num != 0) {  //  maybe no argument for main()
      DEBUG(cout << "report old style: " << func_name << endl);
      string func_info =
          "'" + codegen->DemangleName(codegen->GetFuncName(func_decl, true)) +
          "' " + getUserCodeFileLocation(codegen, func_decl, true);

      dump_critical_message(SYNCHK_ERROR_32(func_info));

      mValid = false;
    }
  }
}

int InputChecker::CheckInterfacePragma() {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  auto kernels = mars_ir.get_top_kernels();
  int any_err = 0;
  for (auto kernel : kernels) {
    for (int i = 0; i < codegen->GetFuncParamNum(kernel); i++) {
      void *sg_name = codegen->GetFuncParam(kernel, i);
      //  printf("sg_name = %s\n", codegen->UnparseToString(sg_name).c_str());
      vector<string> depths =
          codegen->GetUniqueInterfaceAttribute(sg_name, "depth", &mars_ir);
      vector<string> max_depths =
          codegen->GetUniqueInterfaceAttribute(sg_name, "max_depth", &mars_ir);
      if (CheckDepths(depths, kernel, sg_name) == 0) {
        any_err++;
      }
      if (CheckMaxDepths(max_depths) == 0) {
        any_err++;
      }
      vector<void *> vec_pragma =
          codegen->GetUniqueInterfacePragma(sg_name, &mars_ir);
      string port_info = "'" + codegen->GetVariableName(sg_name) + "' " +
                         getUserCodeFileLocation(codegen, sg_name, true);
      if (vec_pragma.size() > 1) {
        string pragma_info;
        for (auto pragma : vec_pragma) {
          pragma_info += codegen->UnparseToString(pragma) + " " +
                         getUserCodeFileLocation(codegen, pragma, true) + "\n";
        }
        dump_critical_message(INFTF_ERROR_6(port_info, pragma_info));
        any_err++;
      }

      int flag_interface = IsInterfaceRelated(depths, kernel);

      if (flag_interface != 0) {
        if (max_depths.empty()) {
          string str_example = "?";
          for (size_t j = 0; j < depths.size() - 1; j++) {
            str_example += ",?";
          }
          string sg_name_s = codegen->UnparseToString(sg_name);
          void *pragma = vec_pragma[0];
          string str_depth = mars_ir.get_pragma_attribute(pragma, "depth");
          string msg =
              "Incorrect pragma : " + codegen->UnparseToString(pragma) + " " +
              getUserCodeFileLocation(codegen, pragma, true) + "\n";
          msg +=
              "    depth defined as variable, so the max_depth must be set\n";
          msg += "Example : " + codegen->UnparseToString(pragma) +
                 " max_depth=" + str_example + "\n";
          dump_critical_message(INFTF_ERROR_4(msg));
          any_err++;
        }
      }
    }
  }
  mValid &= static_cast<int>(any_err == 0);
  return static_cast<int>(any_err == 0);
}

//  Detect and replace define macors to its defined value.
//  Rose uses token to handle what happened inside of the directive.
//  it may require the wave switch that results in crash.
//  In this function we tokenize the whole string directly, which might be
//  unstable.
void InputChecker::ExpandDefineDirectiveInPragma() {
  ScanDefineDirectives();
  EmbedRecursiveDefines();
  codegen->ReplacePragmaTokens(pTopFunc, def_directive_map_);
  // Yuxin: Aug/19/2019
  // For pragma syntax error, error out
  mValid &= EvaluatePragmaAttributes();
}

void InputChecker::ScanDefineDirectives() {
  vector<void *> located_nodes;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgLocatedNode,
                              &located_nodes, true);
  //  handle define macros
  for (auto &located_node : located_nodes) {
    SgLocatedNode *node = static_cast<SgLocatedNode *>(located_node);
    AttachedPreprocessingInfoType *comments =
        node->getAttachedPreprocessingInfo();
    if (comments != NULL) {
#if PROJDEBUG
      printf("-----------------------------------------------\n");
      printf("Found an IR node with preprocessing Info attached:\n");
      printf("(memaddr: %p Sage type: %s) in file \n%s (line %d col %d)\n",
             node, node->class_name().c_str(),
             (node->get_file_info()->get_filenameString()).c_str(),
             node->get_file_info()->get_line(),
             node->get_file_info()->get_col());
      int counter = 0;
#endif
      AttachedPreprocessingInfoType::iterator i;
      for (i = comments->begin(); i != comments->end(); i++) {
        if ((*i)->getTypeOfDirective() !=
            PreprocessingInfo::CpreprocessorDefineDeclaration) {
          continue;
        }
#if PROJDEBUG
        printf("-------------PreprocessingInfo #%d ----------- : \n",
               counter++);
        printf("classification = %s:\n String format = %s\n",
               PreprocessingInfo::directiveTypeName((*i)->getTypeOfDirective())
                   .c_str(),
               (*i)->getString().c_str());
#endif
        string stmt = (*i)->getString();
        SetDefineDirectives(stmt);
      }
    }
  }
  for (auto &[macro, def] : codegen->get_input_macro()) {
    def_directive_map_[macro] = def;
  }
  ifstream infile("__merlin_macro_list");
  string line;
  while (getline(infile, line)) {
    istringstream iss(line);
    string token;
    iss >> token;  // #define
    iss >> token;  // macro_name
    string macro = token;
    string macro_value = "";
    while (iss >> token) {
      macro_value += token;
      macro_value += " ";
    }
    if (!macro_value.empty()) {
      macro_value.pop_back();
    }
    def_directive_map_[macro] = macro_value;
  }
}

//  Parse and store macro "#define def_name def_content" to def_directive_map_.
void InputChecker::SetDefineDirectives(string stmt) {
  std::istringstream iss(stmt);
  string dummy;
  string def_name;
  string def_content;
  iss >> dummy;
  iss >> def_name;
  getline(iss, def_content);
  const auto strBegin = def_content.find_first_not_of(' ');
  if (strBegin != std::string::npos) {
    const auto strEnd = def_content.find_last_not_of(' ');
    const auto strRange(strEnd - strBegin + 1);
    def_content = def_content.substr(strBegin, strRange);
  }
  StripComments(def_content);
  DEBUG(cout << dummy << "-" << def_name << "-" << def_content << endl);
  def_directive_map_[def_name] = def_content;
}

//  Update define directive maps such that recurisve macros in map values are
//  replaced. The embedded macro expressions are calculated as numerical values.
//  This function only handles the #define macro embedding.
void InputChecker::EmbedRecursiveDefines() {
  for (auto &item : def_directive_map_) {
    string def_name = item.first;
    string def_content = item.second;
    vector<string> tokens;
    codegen->GetTokens(&tokens, def_content);
    bool recursive_replaced = false;
    for (auto token : tokens) {
      if (def_name == token) {
        recursive_replaced = true;
        break;
      }
    }
    if (recursive_replaced)
      continue;
    unordered_set<string> replace_def_names;
    while (HasDefNameToken(tokens, replace_def_names)) {
      for (auto &def_name : replace_def_names) {
        ReplaceTokens(tokens, def_name);
        codegen->MergeTokens(tokens, &def_content);
        codegen->GetTokens(&tokens, def_content);
      }
    }
    item.second = def_content;
  }
  //  This can be ignored, but the goal is to have as many numbers in the map
  //  as possible.
  for (auto &item : def_directive_map_) {
    string &def_content = item.second;
    //  StripComments(def_content);
    if (HasInvalidCalcChar(def_content)) {
      continue;
    }
    int64_t val = Calculate(def_content);
    def_content = std::to_string(val);
  }
}

int InputChecker::EvaluatePragmaAttributes() {
  build_mars_ir_v2(false);
  vector<void *> pragmas;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgPragmaDeclaration,
                              &pragmas);
  bool success = true;
  for (auto &pragma : pragmas) {
    CAnalPragma ana_pragma(codegen);
    bool errorOut = false;
    if (ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, true, true)) {
      bool need_rewrite = false;
      for (auto &[attr, str_attr_exps] : ana_pragma.get_attribute()) {
        if (str_attr_exps.empty()) {
          continue;
        }
        if (!codegen->is_numeric_option(attr, true)) {
          continue;
        }
        string str_attr_new;
        vector<string> attr_exps = str_split(str_attr_exps, ",");
        for (auto &attr_exp : attr_exps) {
          if (!HasInvalidCalcChar(attr_exp)) {
            int64_t exp_val = Calculate(attr_exp);
            str_attr_new += std::to_string(exp_val);
          } else {
            str_attr_new += attr_exp;
          }
          str_attr_new += ",";
        }
        //  if (attr_exps.size() <= 1)
        str_attr_new.pop_back();
        if (str_attr_new != str_attr_exps) {
          ana_pragma.add_attribute(attr, str_attr_new);
          need_rewrite = true;
        }
      }
      if (need_rewrite) {
        ana_pragma.update_pragma(pragma, false, nullptr);
      }
    } else {
      success = false;
    }
  }
  return static_cast<int>(success);
}

void InputChecker::ReplaceTokens(vector<string> &tokens, string def_name) {
  for (auto &token : tokens) {
    if (token == def_name) {
      token = def_directive_map_[def_name];
    }
  }
}

bool InputChecker::HasDefNameToken(vector<string> &tokens,
                                   unordered_set<string> &replace_def_names) {
  replace_def_names.clear();
  for (auto &token : tokens) {
    if (def_directive_map_.count(token) != 0U &&
        def_directive_map_[token] != token) {
      replace_def_names.insert(token);
    }
  }
  return !replace_def_names.empty();
}

bool InputChecker::HasInvalidCalcChar(string &s) {
  if (s.empty() || std::all_of(s.begin(), s.end(),
                               [](auto c) { return c == ' ' || c == '\t'; })) {
    return true;
  }
  unordered_set<char> valid_set = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
                                   '9', '+', '-', '*', '/', '(', ')', ' '};
  for (auto &ch : s) {
    if (valid_set.count(ch) == 0U) {
      return true;
    }
  }
  if (s.find("//  ") != string::npos) {
    return true;
  }
  if (s.find("/*") != string::npos) {
    return true;
  }

  return false;
}

int64_t InputChecker::Calculate(string &s) {
  int i = 0;
  return ParseExpr(s, i);
}

int64_t InputChecker::ParseExpr(string &s, int &i) {
  vector<int64_t> nums;
  char op = '+';
  for (; i < s.length() && op != ')'; i++) {
    if (s[i] == ' ') {
      continue;
    }
    int64_t n = s[i] == '(' ? ParseExpr(s, ++i) : ParseNum(s, i);
    switch (op) {
    case '+':
      nums.push_back(n);
      break;
    case '-':
      nums.push_back(-n);
      break;
    case '*':
      nums.back() *= n;
      break;
    case '/':
      if (n == 0) {
        assert(0 && "zero divisor");
        return -1;
      } else {
        nums.back() /= n;
      }
      break;
    }
    op = s[i];
  }
  int64_t res = 0;
  for (int64_t n : nums) {
    res += n;
  }
  return res;
}

int64_t InputChecker::ParseNum(string &s, int &i) {
  int64_t n = 0;
  while (i < s.length() && (isdigit(s[i]) != 0)) {
    n = s[i++] - '0' + 10 * n;
  }
  return n;
}

//  This is the handling for #define A 64 //  your commnets
//  which does not apply in normal ANSI-C style
//  A better solution may be required
//  Or we simply skip define with comments
void InputChecker::StripComments(string &s) {
  auto pos = s.find("//  ");
  if (pos != string::npos) {
    s = s.substr(0, pos);
  }

  pos = s.find("/*");
  if (pos != string::npos) {
    s = s.substr(0, pos);
  }
}

int InputChecker::IsInterfaceRelated(vector<string> depths, void *kernel) {
  for (size_t j = 0; j < depths.size(); j++) {
    //  printf("exp = %s\n", depths[j].c_str());
    void *init_name = codegen->find_interface_by_name(depths[j], kernel);
    if (init_name != nullptr) {
      return 1;
    }
  }
  return 0;
}

//  Han Jun8th 2017, depth can only be constant or interaface variable
int InputChecker::CheckMaxDepths(vector<string> depths) {
  for (size_t j = 0; j < depths.size(); j++) {
    //  printf("exp = %s\n", depths[j].c_str());
    if (codegen->IsNumberString(depths[j]) != 0) {
    } else {
      string msg = "Invalid interface max_depth value : " + depths[j];
      msg += "\n    max_depth can only be constant integer number, do not "
             "support expression.\n";
      dump_critical_message(INFTF_ERROR_5(msg));
      return 0;
    }
  }
  return 1;
}

//  Han Jun8th 2017, depth can only be constant or interaface variable
int InputChecker::CheckDepths(vector<string> depths, void *kernel,
                              void *var_init) {
  for (auto depth : depths) {
    if (codegen->IsNumberString(depth) != 0) {
    } else {
      void *init_name = codegen->find_interface_by_name(depth, kernel);
      void *var_name =
          codegen->find_variable_by_name(depth, codegen->GetGlobal(kernel));
      int flag = 0;
      if (var_name != nullptr) {
        void *var_type = codegen->GetTypebyVar(var_name);
        if (codegen->IsConstType(var_type) != 0) {
          flag = 1;
        }
      }
      if ((init_name != nullptr) || flag == 1 ||
          codegen->check_valid_field_name(depth, kernel) ||
          (codegen->IsStructElement(depth, kernel) != 0)) {
      } else {
        string depth_info = "'" + depth + "' for interface \'" +
                            codegen->GetVariableName(var_init) + "\' " +
                            getUserCodeFileLocation(codegen, var_init, true);
        string msg = "Invalid interface depth value : " + depth_info;
        msg += "\n    Hint: 1) depth can only be constant integer number "
               "or scalar from function parameter, do not support expression.\n"
               "2) If you have const decl in header file, please "
               "specify them in the cpp file.";
        cout << msg;
        dump_critical_message(INFTF_ERROR_3(depth_info));
        return 0;
      }
    }
  }
  return 1;
}

void InputChecker::CheckInvalidVariableInPragma() {
  vector<void *> pragmas;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgPragmaDeclaration,
                              &pragmas);
  for (auto &pragma : pragmas) {
    string token_str = "";
    bool local_err = false;
    auto used_vars = codegen->GetVariablesUsedByPragma(pragma, &local_err,
                                                       &token_str, false);
    if (local_err) {
      string array_info = token_str;
      string pragma_info = codegen->UnparseToString(pragma, 80) + " " +
                           getUserCodeFileLocation(codegen, pragma, true);
      dump_critical_message(PROCS_ERROR_81(array_info, pragma_info));
      mValid = false;
    }
  }
}
