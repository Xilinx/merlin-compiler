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
#include "program_analysis.h"
#include "rose.h"

#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"

#include "stream_ir.h"
#include "stream_prefetch_xilinx.h"
// using namespace std;
//  using namespace SageInterface;
//  using namespace SageBuilder;
// using namespace MarsProgramAnalysis;
// using namespace MerlinStreamModel;
using MerlinStreamModel::CStreamBase;
using MerlinStreamModel::CStreamChannel;
using MerlinStreamModel::CStreamIR;
using MerlinStreamModel::CStreamNode;
using MerlinStreamModel::CStreamPort;

int isFuncPassByPointer(CMarsAST_IF *ast, void *access, void *funcCall) {
  if (ast->IsFunctionCall(funcCall) == 0) {
    return -1;
  }
  for (int i = 0; i < ast->GetFuncCallParamNum(funcCall); ++i) {
    if (ast->IsInScope(access, ast->GetFuncCallParam(funcCall, i)) != 0) {
      void *funcDecl = ast->GetFuncDeclByCall(funcCall);
      if (funcDecl == nullptr) {
        return -1;
      }

      void *type = ast->GetTypebyVar(ast->GetFuncParam(funcDecl, i));
      if ((ast->IsArrayType(type) != 0) || (ast->IsReferenceType(type) != 0) ||
          (ast->IsPointerType(type) != 0)) {
        return i;
      }
      return -1;
    }
  }
  return -1;
}

int getAccessTimes(CMarsAST_IF *ast, void *arg, void *scope) {
  int times = 0;
  for (auto access : ast->GetAllRefInScope(arg, scope)) {
    void *call = ast->TraceUpToFuncCall(access);
    if (ast->IsMerlinInternalIntrinsic(ast->GetFuncNameByCall(call))) {
      continue;
    }

    int funcIndex;
    if ((funcIndex = isFuncPassByPointer(ast, access, call)) >= 0) {
      void *funcDecl = ast->GetFuncDeclByCall(call);
      void *funcArg = ast->GetFuncParam(funcDecl, funcIndex);
      times += getAccessTimes(ast, funcArg, funcDecl);
    } else {
      ++times;
    }
  }
  return times;
}

int stream_prefetch_intel_top(CSageCodeGen *codegen, void *pTopFunc,
                              const CInputOptions &options) {
  CMarsAST_IF *ast = codegen;
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(codegen, pTopFunc);

  string intel_stream_prefetch =
      options.get_option_key_value("-a", "stream_prefetch");
  bool auto_intel_stream_prefetch = false;
  if ("auto" == intel_stream_prefetch) {
    auto_intel_stream_prefetch = true;
  }

  CStreamIR stream_ir(codegen, options, &mars_ir, &mars_ir_v2);
  bool firstTime = true;
  vector<string> vec_ports_with_multiple_accesses;
  for (auto kernel : mars_ir_v2.get_top_kernels()) {
    set<void *> prefetch_vars;
    set<void *> prefetch_pragmas;
    {
      vector<void *> vec_pragma;
      ast->GetNodesByType(ast->GetFuncBody(kernel), "preorder",
                          "SgPragmaDeclaration", &vec_pragma);

      for (auto arg : ast->GetFuncParams(kernel)) {
        string arg_name = ast->GetVariableName(arg);
        string port_info =
            "'" + arg_name + "' " + getUserCodeFileLocation(ast, arg, true);
        void *type = ast->GetTypebyVar(arg);
        if ((ast->IsPointerType(type) == 0) && (ast->IsArrayType(type) == 0)) {
          continue;
        }
        bool disable_stream_prefetch = false;
        bool enable_stream_prefetch = false;
        int access_times = getAccessTimes(ast, arg, kernel);
        for (auto pragma : vec_pragma) {
          CAnalPragma ana_pragma(ast);
          bool errorout;
          ana_pragma.PragmasFrontendProcessing(pragma, &errorout, false);
          if (!ana_pragma.is_interface()) {
            continue;
          }
          if (ana_pragma.get_attribute(CMOST_variable) == arg_name) {
            string stream_prefetch_option =
                ana_pragma.get_attribute(CMOST_stream_prefetch);
            boost::algorithm::to_upper(stream_prefetch_option);
            if (CMOST_OFF == stream_prefetch_option) {
              disable_stream_prefetch = true;
              continue;
            }
            if (CMOST_ON == stream_prefetch_option) {
              enable_stream_prefetch = true;
            }
            if (!disable_stream_prefetch &&
                (enable_stream_prefetch || auto_intel_stream_prefetch)) {
              prefetch_pragmas.insert(pragma);
            }
          }
        }
        assert(!disable_stream_prefetch || !enable_stream_prefetch);
        if (!disable_stream_prefetch &&
            (enable_stream_prefetch || auto_intel_stream_prefetch)) {
          prefetch_vars.insert(arg);
          if (access_times > 1) {
            vec_ports_with_multiple_accesses.push_back(port_info);
          }
        }
      }
    }

    if (prefetch_vars.empty()) {
      continue;
    }

    if (firstTime) {
      stream_ir.ParseStreamModel();
      firstTime = false;
    }

    string err_msg;
    if (stream_ir.SpawnNodeFromAST(kernel, &err_msg, true, &prefetch_vars) !=
        nullptr) {
      for (auto pragma : prefetch_pragmas) {
        mars_ir_v2.set_pragma_attribute(&pragma, "memory_burst", "off");
      }
      for (auto port_info : vec_ports_with_multiple_accesses) {
        dump_critical_message(STREAM_WARNING_2(port_info));
      }
    }
  }
  return 1;
}

int stream_prefetch_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  string tool_type = options.get_option_key_value("-a", "impl_tool");
  if (tool_type == "aocl") {
    return stream_prefetch_intel_top(codegen, pTopFunc, options);
  }

  if (tool_type == "sdaccel") {
    StreamPrefetchXilinx stream_prefetch(codegen, pTopFunc, options, false);
    return stream_prefetch.run();
  }
  return 0;
}
