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
//  module           :
//  description      :
//  input            :
//  output           :
//  limitation       :
//  author           :   Shan
//  ******************************************************************************************//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <set>
#include <string>
#include <utility>

#include "cmdline_parser.h"
#include "codegen.h"
#include "locations.h"
#include "report.h"
#include "rose.h"
#include "analPragmas.h"

#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"

using rapidjson::Document;
using rapidjson::kArrayType;
using rapidjson::kObjectType;
using rapidjson::OStreamWrapper;
using rapidjson::PrettyWriter;
using rapidjson::Value;
#define DEBUG 0

#if DEBUG
#define debug_print(fmt, ...)                                                  \
  do {                                                                         \
    fprintf(stdout, fmt, __VA_ARGS__);                                         \
  } while (0)
#else
#define debug_print(fmt, ...)
#endif

enum NodeType {
  kKernel = 0,
  kFunction = 1,
  kLoop = 2,
  kInterface = 3,
  kArray = 4,
  kCallFunction = 5,
  kVarRef = 6,
  kVarDef = 7,
  kStmt = 8,
};

pair<int, int> get_scope_range_line(CSageCodeGen *codege, void *node);

pair<int, int> get_scope_range_line(CSageCodeGen *codege, void *node) {
  if (codege->IsFunctionDeclaration(node) != 0) {
    node = codege->GetFuncBody(node);
  }
  if (codege->IsScopeStatement(node) == 0) {
    int line = codege->get_line(node);
    return std::make_pair(line, line);
  }
  int max_line = 0;
  int min_line = codege->get_line(node);
  if (codege->IsIfStatement(node) != 0) {
    SgIfStmt *if_stmt = isSgIfStmt(static_cast<SgNode *>(node));
    SgStatement *false_stmt = if_stmt->get_false_body();
    if (false_stmt != nullptr) {
      auto ret = get_scope_range_line(codege, false_stmt);
      max_line = max(max_line, ret.second);
    }
    SgStatement *true_stmt = if_stmt->get_true_body();
    if (true_stmt != nullptr) {
      auto ret = get_scope_range_line(codege, true_stmt);
      max_line = max(max_line, ret.second);
    }
  } else {
    int num = codege->GetChildStmtNum(node);
    if (num <= 0) {
      max_line = max(max_line, codege->get_line(node));
    } else {
      void *stmt = codege->GetChildStmt(node, num - 1);
      auto ret = get_scope_range_line(codege, stmt);
      max_line = max(max_line, ret.second);
    }
  }
  return std::make_pair(min_line, max_line + 1);
}

Value create_topo_info_node(CSageCodeGen *codegen, void *node, NodeType type,
                            Document::AllocatorType *p_alloc);
Value get_interface_topo(CSageCodeGen *codegen, void *kernel,
                         Document::AllocatorType *p_alloc);
Value get_kernel_topo(CSageCodeGen *codegen, void *kernel,
                      Document::AllocatorType *p_alloc);
Value get_function_topo(CSageCodeGen *codegen, void *func,
                        Document::AllocatorType *p_alloc);
void get_loop_func_topo(CSageCodeGen *codegen, void *node, Value *root,
                        bool parent_unroll, Document::AllocatorType *p_alloc);
Value get_array_topo(CSageCodeGen *codegen, void *func,
                     Document::AllocatorType *p_alloc);
Value get_var_ref_topo(CSageCodeGen *codegen, void *func,
                       Document::AllocatorType *p_alloc);
Value get_var_def_topo(CSageCodeGen *codegen, void *func,
                       Document::AllocatorType *p_alloc);
Value get_stmt_topo(CSageCodeGen *codegen, void *func,
                    Document::AllocatorType *p_alloc);

Value create_topo_info_node(CSageCodeGen *codegen, void *node, NodeType type,
                            Document::AllocatorType *p_alloc) {
  auto sg_node_p = static_cast<SgNode *>(node);
  string topo_id;
  // TODO(youxiang): fix this
  if (isSkipped(codegen, sg_node_p)) {
    topo_id = "is_skipped_node";
  } else {
    topo_id = getTopoLocation(codegen, sg_node_p);
  }
  if (sg_node_p->get_file_info() == nullptr) {
    fprintf(stderr, "empty file info, topo_id: %s \n", topo_id.c_str());
    return Value();
  }

  int line = codegen->get_line(node);
  int col = codegen->get_col(node);
  string src = codegen->get_file(node);
  Value v(kObjectType);
  v.AddMember("line", Value().SetInt(line), *p_alloc);
  v.AddMember("col", Value().SetInt(col), *p_alloc);
  v.AddMember("source", Value().SetString(src.c_str(), *p_alloc), *p_alloc);
  v.AddMember("topo_id", Value().SetString(topo_id.c_str(), *p_alloc),
              *p_alloc);
  string s = codegen->UnparseToString(node);
  v.AddMember("unparse_info", Value().SetString(s.c_str(), *p_alloc), *p_alloc);
  switch (type) {
  case kKernel: {
    debug_print("create kernel:%s\n", codegen->UnparseToString(node).c_str());
    debug_print("topo_id: %s\n", topo_id.c_str());
    v.AddMember("type", Value().SetString("kernel", *p_alloc), *p_alloc);
    string name = codegen->GetFuncName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    auto line_pair = get_scope_range_line(codegen, node);
    v.AddMember(
        "begin",
        Value().SetString(std::to_string(line_pair.first).c_str(), *p_alloc),
        *p_alloc);
    v.AddMember(
        "end",
        Value().SetString(std::to_string(line_pair.second).c_str(), *p_alloc),
        *p_alloc);
  } break;
  case kFunction: {
    debug_print("create function:%s\n", codegen->UnparseToString(node).c_str());
    debug_print("topo_id: %s\n", topo_id.c_str());
    v.AddMember("type", Value().SetString("function", *p_alloc), *p_alloc);
    string name = codegen->GetFuncName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    auto line_pair = get_scope_range_line(codegen, node);
    v.AddMember(
        "begin",
        Value().SetString(std::to_string(line_pair.first).c_str(), *p_alloc),
        *p_alloc);
    v.AddMember(
        "end",
        Value().SetString(std::to_string(line_pair.second).c_str(), *p_alloc),
        *p_alloc);
  } break;
  case kLoop: {
    debug_print("create loop:%s\n", codegen->UnparseToString(node).c_str());
    debug_print("topo_id: %s\n", topo_id.c_str());
    v.AddMember("type", Value().SetString("loop", *p_alloc), *p_alloc);
    if (codegen->IsForStatement(node) != 0) {
      void *loop_iter = codegen->GetLoopIterator(node);
      auto name = codegen->UnparseToString(loop_iter);
      v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    }
    if (codegen->IsCanonicalForLoop(node) != 0) {
      v.AddMember("canonical", Value().SetString("yes", *p_alloc), *p_alloc);
    } else {
      v.AddMember("canonical", Value().SetString("no", *p_alloc), *p_alloc);
    }
    auto line_pair = get_scope_range_line(codegen, node);
    v.AddMember(
        "begin",
        Value().SetString(std::to_string(line_pair.first).c_str(), *p_alloc),
        *p_alloc);
    v.AddMember(
        "end",
        Value().SetString(std::to_string(line_pair.second).c_str(), *p_alloc),
        *p_alloc);
    int64_t trip_count = 0;
    codegen->get_loop_trip_count_simple(node, &trip_count);
    v.AddMember("lc-trip-count", Value().SetInt(trip_count), *p_alloc);
  } break;
  case kInterface: {
    debug_print("createinterface:%s\n", codegen->UnparseToString(node).c_str());
    debug_print("topo_id: %s\n", topo_id.c_str());
    v.AddMember("type", Value().SetString("interface", *p_alloc), *p_alloc);
    string name = codegen->GetVariableName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    auto sg_init_name = static_cast<SgInitializedName *>(node);
    auto var_type = sg_init_name->get_type();
    if ((codegen->IsArrayType(var_type) != 0) ||
        (codegen->IsPointerType(var_type) != 0)) {
      v.AddMember("var_type", Value().SetString("array", *p_alloc), *p_alloc);
      if (codegen->IsModifierType(codegen->GetBaseType(var_type, false)) != 0) {
        v.AddMember("const", Value().SetString("no", *p_alloc), *p_alloc);
      } else {
        v.AddMember("const", Value().SetString("yes", *p_alloc), *p_alloc);
      }
    } else if (codegen->IsCompoundType(var_type) != 0) {
      v.AddMember("var_type", Value().SetString("compound", *p_alloc),
                  *p_alloc);
      v.AddMember("const", Value().SetString("no", *p_alloc), *p_alloc);
    } else {
      v.AddMember("var_type", Value().SetString("scalar", *p_alloc), *p_alloc);
      if (codegen->IsModifierType(var_type) != 0) {
        v.AddMember("const", Value().SetString("no", *p_alloc), *p_alloc);
      } else {
        v.AddMember("const", Value().SetString("yes", *p_alloc), *p_alloc);
      }
    }
  } break;
  case kArray: {
    v.AddMember("type", Value().SetString("array", *p_alloc), *p_alloc);
    string name = codegen->GetVariableName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kCallFunction: {
    debug_print("create call:%s\n", codegen->UnparseToString(node).c_str());
    debug_print("topo_id: %s\n", topo_id.c_str());
    v.AddMember("type", Value().SetString("callfunction", *p_alloc), *p_alloc);
    void *func_decl = codegen->GetFuncDeclByCall(node);
    string name = codegen->GetFuncName(func_decl);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    string decl_id = getTopoLocation(codegen, static_cast<SgNode *>(func_decl));
    v.AddMember("function_id", Value().SetString(decl_id.c_str(), *p_alloc),
                *p_alloc);
  } break;
  case kVarRef: {
    v.AddMember("type", Value().SetString("var_ref", *p_alloc), *p_alloc);
    void *var_init = codegen->GetVariableInitializedName(node);
    string name = codegen->GetVariableName(var_init);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kVarDef: {
    v.AddMember("type", Value().SetString("var_def", *p_alloc), *p_alloc);
    void *var_init = codegen->GetVariableInitializedName(node);
    string name = codegen->GetVariableName(var_init);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kStmt: {
    v.AddMember("type", Value().SetString("stmt", *p_alloc), *p_alloc);
  } break;
  default:
    break;
  }
  return v;
}

Value get_interface_topo(CSageCodeGen *codegen, void *kernel,
                         Document::AllocatorType *p_alloc) {
  Value ret(kArrayType);
  auto params = codegen->GetFuncParams(kernel);
  for (auto p : params) {
    Value node = create_topo_info_node(codegen, p, kInterface, p_alloc);
    ret.PushBack(node, *p_alloc);
  }
  return ret;
}

Value get_kernel_topo(CSageCodeGen *codegen, void *kernel,
                      Document::AllocatorType *p_alloc) {
  Value k = create_topo_info_node(codegen, kernel, kKernel, p_alloc);

  Value interfaces = get_interface_topo(codegen, kernel, p_alloc);
  k.AddMember("interfaces", interfaces, *p_alloc);

  Value arrays = get_array_topo(codegen, kernel, p_alloc);
  k.AddMember("arrays", arrays, *p_alloc);

  Value var_refs = get_var_ref_topo(codegen, kernel, p_alloc);
  k.AddMember("var_refs", var_refs, *p_alloc);

  Value var_defs = get_var_def_topo(codegen, kernel, p_alloc);
  k.AddMember("var_defs", var_defs, *p_alloc);

  Value stmts = get_stmt_topo(codegen, kernel, p_alloc);
  k.AddMember("stmts", stmts, *p_alloc);

  Value childs(kArrayType);
  get_loop_func_topo(codegen, kernel, &childs, false, p_alloc);
  k.AddMember("childs", childs, *p_alloc);

  Value funcs(kArrayType);
  SetVector<void *> functions;
  SetVector<void *> _;
  codegen->GetSubFuncDeclsRecursively(kernel, &functions, &_);
  for (auto f : functions) {
    if (f == kernel) {
      continue;
    }
    debug_print("get sub func decl : %p\n", f);
    Value func = get_function_topo(codegen, f, p_alloc);
    funcs.PushBack(func, *p_alloc);
  }
  k.AddMember("sub_functions", funcs, *p_alloc);
  return k;
}

Value get_function_topo(CSageCodeGen *codegen, void *func,
                        Document::AllocatorType *p_alloc) {
  Value f = create_topo_info_node(codegen, func, kFunction, p_alloc);

  Value arrays = get_array_topo(codegen, func, p_alloc);
  f.AddMember("arrays", arrays, *p_alloc);

  Value childs(kArrayType);
  get_loop_func_topo(codegen, func, &childs, false, p_alloc);
  f.AddMember("childs", childs, *p_alloc);

  Value var_refs = get_var_ref_topo(codegen, func, p_alloc);
  f.AddMember("var_refs", var_refs, *p_alloc);

  Value var_defs = get_var_def_topo(codegen, func, p_alloc);
  f.AddMember("var_defs", var_defs, *p_alloc);

  Value stmts = get_stmt_topo(codegen, func, p_alloc);
  f.AddMember("stmts", stmts, *p_alloc);

  return f;
}

void get_loop_func_topo(CSageCodeGen *codegen, void *node, Value *root,
                        bool parent_unroll, Document::AllocatorType *p_alloc) {
  debug_print("gen_loop_hierarchy: %s\n",
              codegen->UnparseToString(node).c_str());
  if (node == nullptr) {
    return;
  }
  bool isCommonFuncCall = false;
  if (codegen->IsFunctionCall(node) != 0) {
    //  ignore compiler genereate function
    if (!codegen->IsCompilerGenerated(node)) {
      isCommonFuncCall = true;
    }
  }
  bool is_unrolled = false;
  if (codegen->IsLoopStatement(node) != 0) {
    void *loop_body = codegen->GetLoopBody(node);
    int num_child = codegen->GetChildStmtNum(loop_body);
    CAnalPragma anal_pragma(codegen);
    for (int i = 0; i < num_child; ++i) {
      void *stmt = codegen->GetChildStmt(loop_body, i);
      if ((codegen->IsPragmaDecl(stmt) != 0) ||
          (codegen->IsLabelStatement(stmt) != 0)) {
        if (codegen->IsLabelStatement(stmt) != 0) {
          continue;
        }
        bool err = false;
        anal_pragma.PragmasFrontendProcessing(stmt, &err, false);
        if (anal_pragma.is_hls_unroll()) {
          is_unrolled = true;
          break;
        }
      } else {
        break;
      }
    }
  }
  if ((codegen->IsLoopStatement(node) != 0) || isCommonFuncCall) {
    Value sub(kArrayType);
    SgNode *cur = static_cast<SgNode *>(node);
    size_t total = cur->get_numberOfTraversalSuccessors();
    for (size_t i = 0; i < total; ++i) {
      void *child = cur->get_traversalSuccessorByIndex(i);
      get_loop_func_topo(codegen, child, &sub, is_unrolled || parent_unroll,
                         p_alloc);
    }
    NodeType info_node_type;
    if (codegen->IsLoopStatement(node) != 0) {
      info_node_type = kLoop;
    } else {
      assert(codegen->IsFunctionCall(node));
      info_node_type = kCallFunction;
    }
    Value v = create_topo_info_node(codegen, node, info_node_type, p_alloc);
    if (codegen->IsLoopStatement(node) != 0) {
      void *prev = codegen->GetParent(node);
      if (codegen->IsLabelStatement(prev) != 0) {
        auto label_node = static_cast<SgLabelStatement *>(prev);
        Value label;
        label.SetString(label_node->get_label().getString().c_str(), *p_alloc);
        v.AddMember("loop_label", label, *p_alloc);
        auto loop_body = codegen->GetLoopBody(node);
        int line = codegen->get_line(loop_body);
        int col = codegen->get_col(loop_body);
        v.AddMember("loop_body_line", Value().SetInt(line), *p_alloc);
        v.AddMember("loop_body_col", Value().SetInt(col), *p_alloc);
      }
    }
    v.AddMember("childs", sub, *p_alloc);
    if (is_unrolled || parent_unroll) {
      v.AddMember("hls_mark_unroll", Value().SetString("yes", *p_alloc),
                  *p_alloc);
    }
    root->PushBack(v, *p_alloc);
  } else {
    SgNode *cur = static_cast<SgNode *>(node);
    size_t total = cur->get_numberOfTraversalSuccessors();
    for (size_t i = 0; i < total; ++i) {
      void *child = cur->get_traversalSuccessorByIndex(i);
      get_loop_func_topo(codegen, child, root, parent_unroll, p_alloc);
    }
  }
}

Value get_var_def_topo(CSageCodeGen *codegen, void *func,
                       Document::AllocatorType *p_alloc) {
  vector<void *> decls;
  codegen->GetNodesByType(func, "preorder", "SgVariableDeclaration", &decls);

  Value ret(kArrayType);
  for (auto d : decls) {
    auto sg_var_decl = static_cast<SgVariableDeclaration *>(d);
    for (auto var : sg_var_decl->get_variables()) {
      auto type = codegen->GetTypebyVar(var);
      if ((codegen->IsArrayType(type) != 0) ||
          (codegen->IsPointerType(type) != 0)) {
        continue;
      }
      Value arr = create_topo_info_node(codegen, var, kVarDef, p_alloc);
      ret.PushBack(arr, *p_alloc);
    }
  }
  return ret;
}

Value get_var_ref_topo(CSageCodeGen *codegen, void *func,
                       Document::AllocatorType *alloc) {
  vector<void *> var_refs;
  codegen->GetNodesByType(func, "preorder", "SgVarRefExp", &var_refs);

  Value ret(kArrayType);
  for (auto v : var_refs) {
    void *sg_type = codegen->GetTypebyVarRef(v);
    if (codegen->IsScalarType(sg_type)) {
      continue;
    }
    Value var_ref_node = create_topo_info_node(codegen, v, kVarRef, alloc);
    ret.PushBack(var_ref_node, *alloc);
  }
  return ret;
}

Value get_stmt_topo(CSageCodeGen *codegen, void *func,
                    Document::AllocatorType *p_alloc) {
  vector<void *> stmts;
  codegen->GetNodesByType_int_compatible(func, V_SgStatement, &stmts);

  Value ret(kArrayType);
  for (auto s : stmts) {
    Value stmt_node = create_topo_info_node(codegen, s, kStmt, p_alloc);
    ret.PushBack(stmt_node, *p_alloc);
  }
  return ret;
}

Value get_array_topo(CSageCodeGen *codegen, void *func,
                     Document::AllocatorType *p_alloc) {
  vector<void *> decls;
  codegen->GetNodesByType(func, "preorder", "SgVariableDeclaration", &decls);

  Value ret(kArrayType);
  for (auto d : decls) {
    auto sg_var_decl = static_cast<SgVariableDeclaration *>(d);
    for (auto var : sg_var_decl->get_variables()) {
      auto type = codegen->GetTypebyVar(var);
      if ((codegen->IsArrayType(type) == 0) &&
          (codegen->IsPointerType(type) == 0)) {
        continue;
      }
      Value arr = create_topo_info_node(codegen, var, kArray, p_alloc);
      ret.PushBack(arr, *p_alloc);
    }
  }
  return ret;
}

int gen_token_id_top(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options) {
  printf("gen token id top\n");

  string kernel_list_path = options.get_option_key_value("-a", "kernel_list");
  if (kernel_list_path.empty()) {
    kernel_list_path = "kernel_list.json";
  }

  map<string, map<string, string>> kernel_info;
  string line;
  std::ifstream kernel_list_file(kernel_list_path);
  if (kernel_list_file.is_open()) {
    kernel_list_file.close();
    readInterReport(kernel_list_path, &kernel_info);
  } else {
    fprintf(stderr, "error: cant find file %s\n", kernel_list_path.c_str());
    assert(0);
    return 0;
  }
  vector<string> kernel_list;
  for (const auto &kernel_pair : kernel_info) {
    for (const auto &sub_kernel_pair : kernel_pair.second) {
      kernel_list.push_back(sub_kernel_pair.first);
    }
  }

  Document d(kArrayType);
  Document::AllocatorType &alloc = d.GetAllocator();
  for (auto name : kernel_list) {
    auto kernel = codegen->GetFuncDeclByName(name);
    if (kernel == nullptr || codegen->GetFuncBody(kernel) == nullptr ||
        isSkipped(codegen, static_cast<SgNode *>(kernel))) {
      continue;
    }
    Value k = get_kernel_topo(codegen, kernel, &alloc);
    d.PushBack(k, alloc);
  }
  string output = options.get_option_key_value("-a", "output");
  if (output.empty()) {
    output = "topo_info.json";
  }
  std::ofstream ofs(output);
  OStreamWrapper osw(ofs);
  PrettyWriter<OStreamWrapper> writer(osw);
  d.Accept(writer);
  return 0;
}
