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
#include <map>
#include <set>
#include <string>

#include "cmdline_parser.h"
#include "codegen.h"
#include "locations.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "postwrap_process.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "report.h"
#include "rose.h"

using rapidjson::Document;
using rapidjson::kArrayType;
using rapidjson::kObjectType;
using rapidjson::OStreamWrapper;
using rapidjson::PrettyWriter;
using rapidjson::Value;
using std::map;
using std::set;
using std::string;
using std::to_string;

#undef DEBUG
#define DEBUG 0

#if DEBUG
#define debug_print(fmt, ...)                                                  \
  do {                                                                         \
    fprintf(stdout, fmt, __VA_ARGS__);                                         \
  } while (0)
#else
#define debug_print(fmt, ...)
#endif

bool is_legal_topo_id(string topo_id) {
  size_t idx = topo_id.find('_');
  return idx != string::npos && idx <= 2;
}

Value gen_func_hierarchy(CSageCodeGen *codegen, void *func_node,
                         Document::AllocatorType *p_alloc);

enum BasicInfoNodeType {
  kKernel = 0,
  kFunction = 1,
  kLoop = 2,
  kInterface = 3,
  kArray = 4,
  kCallFunction = 5,
  kGlobalVariable = 6,
  kVarRef = 7,
  kVarDef = 8,
  kStmts = 9,
  kArgument = 10,
};

map<void *, string> g_topo_ids;

Value create_info_node(CSageCodeGen *codegen, void *node,
                       BasicInfoNodeType type,
                       Document::AllocatorType *p_alloc) {
  debug_print("create_info_node:%p = %s\n", node,
              codegen->UnparseToString(node).c_str());
  void *orig_node = codegen->GetOriginalNode(node);
  int line = codegen->get_line(orig_node);
  int col = codegen->get_col(orig_node);
  string src = codegen->get_file(orig_node);
  Value v(kObjectType);
  v.AddMember("src_line", Value().SetInt(line), *p_alloc);
  v.AddMember("src_col", Value().SetInt(col), *p_alloc);
  v.AddMember("src_filename", Value().SetString(src.c_str(), *p_alloc),
              *p_alloc);
  auto sg_node = static_cast<SgNode *>(node);
  auto it = g_topo_ids.find(node);
  string src_topo_id;
  if (it == g_topo_ids.end()) {
    string topo_id = getTopoLocation(codegen, sg_node);
    debug_print("unknow topo id with node\n%p:%s:%s\n", node,
                codegen->UnparseToString(node).c_str(), topo_id.c_str());
    src_topo_id = topo_id;
  } else {
    src_topo_id = it->second;
  }
  v.AddMember("src_topo_id", Value().SetString(src_topo_id.c_str(), *p_alloc),
              *p_alloc);
  switch (type) {
  case kKernel: {
    v.AddMember("type", Value().SetString("kernel", *p_alloc), *p_alloc);
    string name = codegen->GetFuncName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kFunction: {
    v.AddMember("type", Value().SetString("function", *p_alloc), *p_alloc);
    string name = codegen->GetFuncName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kLoop: {
    v.AddMember("type", Value().SetString("loop", *p_alloc), *p_alloc);
    if (codegen->IsForStatement(node)) {
      void *loop_iter = codegen->GetLoopIterator(node);
      auto name = codegen->UnparseToString(loop_iter);
      void *prev = codegen->GetParent(node);
      if (codegen->IsLabelStatement(prev)) {
        auto label_node = static_cast<SgLabelStatement *>(prev);
        name = label_node->get_label().getString();
      }
      v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    } else if (codegen->IsWhileStatement(node)) {
      string name = "while";
      v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    } else if (codegen->IsDoWhileStatement(node) != 0) {
      string name = "do_while";
      v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    }
    cout << "fill info for loop = " << codegen->UnparseToString(node) << endl;
    int64_t TC = -1;
    int64_t TC_ub = -1;
    codegen->get_loop_trip_count(sg_node, &TC, &TC_ub);
    cout << "TC = " << TC << ", TC_ub = " << TC_ub << endl;
    if (TC_ub >= 0) {
      v.AddMember("trip-count",
                  Value().SetString(to_string(TC_ub).c_str(), *p_alloc),
                  *p_alloc);
    } else {
      v.AddMember("trip-count", Value().SetString("?", *p_alloc), *p_alloc);
    }
    void *parent_loop =
        codegen->TraceUpToLoopScope(codegen->GetParent(sg_node));
    if (parent_loop) {
      string parent_id =
          getTopoLocation(codegen, static_cast<SgNode *>(parent_loop));
      v.AddMember("parent_loop", Value().SetString(parent_id.c_str(), *p_alloc),
                  *p_alloc);
    }
  } break;
  case kInterface:
  case kArgument:
  case kGlobalVariable: {
    if (type == kInterface) {
      v.AddMember("type", Value().SetString("interface", *p_alloc), *p_alloc);
    } else if (type == kGlobalVariable) {
      v.AddMember("type", Value().SetString("global_variable", *p_alloc),
                  *p_alloc);
    } else if (type == kArgument) {
      v.AddMember("type", Value().SetString("argument", *p_alloc), *p_alloc);
    }
    string name = codegen->GetVariableName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    auto sg_init_name = static_cast<SgInitializedName *>(node);
    auto var_type = sg_init_name->get_type();
    void *element_type = codegen->GetBaseType(var_type, true);
    int element_size = codegen->get_bitwidth_from_type(element_type);
    string s = my_itoa(element_size);
    v.AddMember("interface_bitwidth", Value().SetString(s.c_str(), *p_alloc),
                *p_alloc);
    if ((codegen->IsArrayType(var_type) != 0) ||
        (codegen->IsPointerType(var_type) != 0)) {
      v.AddMember("var_type", Value().SetString("array", *p_alloc), *p_alloc);
    } else if (codegen->IsCompoundType(var_type) != 0) {
      v.AddMember("var_type", Value().SetString("compound", *p_alloc),
                  *p_alloc);
    } else {
      v.AddMember("var_type", Value().SetString("scalar", *p_alloc), *p_alloc);
    }
  } break;
  case kArray: {
    v.AddMember("type", Value().SetString("array", *p_alloc), *p_alloc);
    string name = codegen->GetVariableName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kCallFunction: {
    v.AddMember("type", Value().SetString("callfunction", *p_alloc), *p_alloc);
    debug_print("func_call name :%s \n",
                codegen->UnparseToString(node, 20).c_str());
    string name = codegen->GetFuncNameByCall(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
    void *func_decl = codegen->GetFuncDeclByCall(node);
    string decl_id = getTopoLocation(codegen, static_cast<SgNode *>(func_decl));
    v.AddMember("function_id", Value().SetString(decl_id.c_str(), *p_alloc),
                *p_alloc);
    /*
      void *func_decl = codegen->GetFuncDeclByCall(node);
      if (func_decl != nullptr)
      {
      string name = codegen->GetFuncName(func_decl);
      v.AddMember("name", Value().SetString(name.c_str(), alloc), alloc);
      }
      else
      {
      string name = "miss function declaration";
      v.AddMember("name", Value().SetString(name.c_str(), alloc), alloc);
      }
    */
  } break;
  case kVarRef: {
    v.AddMember("type", Value().SetString("var_ref", *p_alloc), *p_alloc);
    void *var_init = codegen->GetVariableInitializedName(node);
    string name = codegen->GetVariableName(var_init);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kVarDef: {
    v.AddMember("type", Value().SetString("var_def", *p_alloc), *p_alloc);
    string name = codegen->GetVariableName(node);
    v.AddMember("name", Value().SetString(name.c_str(), *p_alloc), *p_alloc);
  } break;
  case kStmts: {
    v.AddMember("type", Value().SetString("stmt", *p_alloc), *p_alloc);
    void *parent_stmt = codegen->GetParent(node);
    if (codegen->IsLoopStatement(parent_stmt) && codegen->IsBasicBlock(node)) {
      auto sg_parent = static_cast<SgNode *>(parent_stmt);
      string parent_id = getTopoLocation(codegen, sg_parent);
      v.AddMember("map_to_id", Value().SetString(parent_id.c_str(), *p_alloc),
                  *p_alloc);
    }
  } break;
  }
  return v;
}

void gen_loop_func_hierarchy(CSageCodeGen *codegen, void *node, Value *root,
                             string parent_func_name,
                             Document::AllocatorType *p_alloc) {
  debug_print("gen_loop_hierarchy:%s\n", codegen->_p(node, 80).c_str());
  if (node == nullptr) {
    return;
  }
  bool isUserFunctionCall = false;
  bool is_valid_func = false;
  if (codegen->IsFunctionCall(node) != 0) {
    debug_print("call node: %s\n", codegen->_p(node, 80).c_str());
    void *decl = codegen->GetFuncDeclByCall(node);
    if ((decl != nullptr) && (codegen->GetFuncBody(decl) != nullptr) &&
        (isSgConstructorInitializer(static_cast<SgNode *>(node)) == nullptr)) {
      is_valid_func = true;
      debug_print("%s", "is valid call node \n");
    } else {
      debug_print("%s", "is not valid call node \n");
    }
    //    if(!isSgConstructorInitializer(static_cast<SgNode *>(node)) ){
    //      is_valid_func = true;
    //      debug_print("is valid call node \n", "1");
    //    } else {
    //      debug_print("is not valid call node \n", "1");
    //    }
    string func_name = codegen->GetFuncNameByCall(node);
    if (func_name.substr(0, 2) != "__") {
      isUserFunctionCall = true;
    }
  }
  if ((codegen->IsLoopStatement(node) != 0) ||
      (isUserFunctionCall && is_valid_func)) {
    debug_print("%s", "valid func and loop\n");
    Value sub(kArrayType);
    SgNode *cur = static_cast<SgNode *>(node);
    size_t total = cur->get_numberOfTraversalSuccessors();
    for (size_t i = 0; i < total; ++i) {
      void *child = cur->get_traversalSuccessorByIndex(i);
      gen_loop_func_hierarchy(codegen, child, &sub, parent_func_name, p_alloc);
    }
    BasicInfoNodeType info_node_type = kKernel;
    if (codegen->IsLoopStatement(node)) {
      //      void *prev = codegen.GetParent(node);
      //      debug_print("stmt before loop: %s\n",
      //                  codegen.UnparseToString(prev, 20).c_str());
      //      if (codegen.IsLabelStatement(prev)) {
      //        auto label = static_cast<SgLabelStatement *>(prev);
      //        debug_print("label name
      //        %s\n",label->get_label().getString().c_str());
      //      }
      info_node_type = kLoop;
    } else if (codegen->IsFunctionCall(node) != 0) {
      info_node_type = kCallFunction;
    }
    Value v = create_info_node(codegen, node, info_node_type, p_alloc);
    if (codegen->IsLoopStatement(node) != 0) {
      void *prev = codegen->GetParent(node);
      if (codegen->IsLabelStatement(prev) != 0) {
        auto label_node = static_cast<SgLabelStatement *>(prev);
        Value label;
        label.SetString(label_node->get_label().getString().c_str(), *p_alloc);
        v.AddMember("loop_label", label, *p_alloc);
      }
    }
    v.AddMember("src_func_name",
                Value().SetString(parent_func_name.c_str(), *p_alloc),
                *p_alloc);
    v.AddMember("childs", sub, *p_alloc);
    root->PushBack(v, *p_alloc);
  } else {
    SgNode *cur = static_cast<SgNode *>(node);
    size_t total = cur->get_numberOfTraversalSuccessors();
    for (size_t i = 0; i < total; ++i) {
      void *child = cur->get_traversalSuccessorByIndex(i);
      gen_loop_func_hierarchy(codegen, child, root, parent_func_name, p_alloc);
    }
  }
}

Value get_call_functions(CSageCodeGen *codegen, void *func,
                         Document::AllocatorType *p_alloc) {
  string func_name = codegen->GetFuncName(func);
  debug_print("get_call_functions:%p, %s\n", func, func_name.c_str());
  Value ret(kArrayType);
  vector<void *> funcs;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &funcs);
  for (auto it = funcs.begin(); it != funcs.end(); ++it) {
    void *func_decl = codegen->GetFuncDeclByCall(*it);
    if (func_decl == nullptr) {
      continue;
    }
    string name = codegen->GetFuncName(func_decl);
    debug_print("call func: %s \n", name.c_str());
    Value func = create_info_node(codegen, *it, kCallFunction, p_alloc);
    ret.PushBack(func, *p_alloc);
  }
  return ret;
}

Value get_interfaces(CSageCodeGen *codegen, void *kernel_node,
                     Document::AllocatorType *p_alloc) {
  debug_print("get_interfaces:%p\n", kernel_node);
  Value ret(kArrayType);
  auto params = codegen->GetFuncParams(kernel_node);
  for (auto p : params) {
    Value v = create_info_node(codegen, p, kInterface, p_alloc);
    ret.PushBack(v, *p_alloc);
  }
  return ret;
}

Value get_arguments(CSageCodeGen *codegen, void *func_node,
                    Document::AllocatorType *p_alloc) {
  debug_print("get_argument:%p\n", func_node);
  Value ret(kArrayType);
  auto params = codegen->GetFuncParams(func_node);
  for (auto p : params) {
    Value v = create_info_node(codegen, p, kArgument, p_alloc);
    ret.PushBack(v, *p_alloc);
  }
  return ret;
}

Value get_used_global_variables(CSageCodeGen *codegen, void *kernel_node,
                                Document::AllocatorType *p_alloc,
                                bool hls_flow) {
  debug_print("get_used_global_variables:%p\n", kernel_node);
  Value ret(kArrayType);

  set<void *> variables;
  map<void *, set<void *>> var2refs;
  bool errorOut = false;
  collect_used_global_variable(kernel_node, &variables, &var2refs, codegen,
                               &errorOut, hls_flow);
  for (auto var : variables) {
    auto var_decl = codegen->GetVariableDecl(var);
    if (codegen->isWithInHeaderFile(var_decl)) {
      string name = codegen->GetVariableName(var);
      dump_critical_message(MSG_RPT_WARNING_1(name), 0);
    }
    Value v = create_info_node(codegen, var, kGlobalVariable, p_alloc);
    ret.PushBack(v, *p_alloc);
  }

  return ret;
}

Value get_arrays(CSageCodeGen *codegen, void *func,
                 Document::AllocatorType *p_alloc) {
  debug_print("get_arrays:%p\n", func);
  Value ret(kArrayType);
  vector<void *> decls;
  // TODO(youxiang):
  codegen->GetNodesByType(func, "preorder", "SgVariableDeclaration", &decls);
  for (auto d : decls) {
    auto sg_var_decl = static_cast<SgVariableDeclaration *>(d);
    for (auto var : sg_var_decl->get_variables()) {
      auto type = codegen->GetTypebyVar(var);
      if ((codegen->IsArrayType(type) == 0) &&
          (codegen->IsPointerType(type) == 0)) {
        continue;
      }
      Value v = create_info_node(codegen, var, kArray, p_alloc);
      ret.PushBack(v, *p_alloc);
    }
  }
  return ret;
}

Value get_var_def(CSageCodeGen *codegen, void *func,
                  Document::AllocatorType *p_alloc) {
  Value ret(kArrayType);
  vector<void *> decls;
  codegen->GetNodesByType(func, "preorder", "SgVariableDeclaration", &decls);
  for (auto d : decls) {
    auto sg_var_decl = static_cast<SgVariableDeclaration *>(d);
    for (auto var : sg_var_decl->get_variables()) {
      auto type = codegen->GetTypebyVar(var);
      if ((codegen->IsArrayType(type) != 0) ||
          (codegen->IsPointerType(type) != 0)) {
        continue;
      }
      Value v = create_info_node(codegen, var, kVarDef, p_alloc);
      ret.PushBack(v, *p_alloc);
    }
  }
  return ret;
}

Value get_var_ref(CSageCodeGen *codegen, void *func,
                  Document::AllocatorType *p_alloc) {
  vector<void *> var_refs;
  codegen->GetNodesByType(func, "preorder", "SgVarRefExp", &var_refs);

  Value ret(kArrayType);
  for (auto v : var_refs) {
    void *sg_type = codegen->GetTypebyVarRef(v);
    if (codegen->IsScalarType(sg_type)) {
      continue;
    }
    Value var_ref_node = create_info_node(codegen, v, kVarRef, p_alloc);
    ret.PushBack(var_ref_node, *p_alloc);
  }
  return ret;
}

Value get_stmts(CSageCodeGen *codegen, void *func,
                Document::AllocatorType *p_alloc) {
  vector<void *> stmts;
  codegen->GetNodesByType_int_compatible(func, V_SgStatement, &stmts);

  Value ret(kArrayType);
  for (auto v : stmts) {
    Value stmt_node = create_info_node(codegen, v, kStmts, p_alloc);
    ret.PushBack(stmt_node, *p_alloc);
  }
  return ret;
}

SgMemberFunctionDeclaration *getMemberFunctionValidDecl(CSageCodeGen *codegen,
                                                        void *func_decl) {
  SgMemberFunctionDeclaration *sg_mem_func_decl =
      isSgMemberFunctionDeclaration(static_cast<SgNode *>(func_decl));
  if ((isSgTemplateInstantiationMemberFunctionDecl(
           static_cast<SgNode *>(func_decl)) != nullptr) ||
      (isSgTemplateMemberFunctionDeclaration(
           static_cast<SgNode *>(func_decl)) != nullptr)) {
    return nullptr;
  }
  if (sg_mem_func_decl == nullptr) {
    return nullptr;
  }
  vector<void *> func_defs;
  codegen->GetNodesByType(codegen->GetProject(), "preorder",
                          "SgFunctionDefinition", &func_defs);
  for (auto d : func_defs) {
    SgFunctionDefinition *def =
        isSgFunctionDefinition(static_cast<SgNode *>(d));
    if (def == nullptr) {
      continue;
    }
    SgMemberFunctionDeclaration *decl =
        isSgMemberFunctionDeclaration(def->get_declaration());
    if (decl == nullptr) {
      continue;
    }
    string decl_name = codegen->GetFuncName(decl);
    string sg_mem_func_name = codegen->GetFuncName(sg_mem_func_decl);
    if (decl_name == sg_mem_func_name) {
      return decl;
    }
  }
  return nullptr;
}

void get_sub_funcs_recurs(CSageCodeGen *codegen, void *sg_func_decl,
                          SetVector<void *> *func_decls) {
  debug_print("get_sub_funcs_recurs_for function: %s  \n",
              codegen->UnparseToString(sg_func_decl).c_str());
  if (codegen->IsFunctionDeclaration(sg_func_decl) == 0) {
    return;
  }
  if (codegen->GetFuncBody(sg_func_decl) == nullptr) {
    return;
  }

  vector<void *> all_calls;
  codegen->GetNodesByType_int(codegen->GetFuncBody(sg_func_decl), "preorder",
                              V_SgFunctionCallExp, &all_calls);
  for (auto call : all_calls) {
    void *sg_curr_decl = codegen->GetFuncDeclByCall(call);
    debug_print("get_sub_funcs_recurs_for function call: %s  \n",
                codegen->UnparseToString(call).c_str());
    debug_print("get_sub_funcs_recurs_for function call decl: %s  \n",
                codegen->UnparseToString(sg_curr_decl).c_str());
    if (sg_curr_decl == nullptr) {
      void *decl = isSgFunctionCallExp(static_cast<SgNode *>(call))
                       ->getAssociatedFunctionDeclaration();
      if ((codegen->GetFuncBody(decl) == nullptr) &&
          (isSgMemberFunctionDeclaration(static_cast<SgNode *>(decl)) !=
           nullptr) &&
          (isSgTemplateInstantiationMemberFunctionDecl(
               static_cast<SgNode *>(decl)) == nullptr) &&
          (isSgTemplateMemberFunctionDeclaration(static_cast<SgNode *>(decl)) ==
           nullptr)) {
        sg_curr_decl = getMemberFunctionValidDecl(codegen, decl);
      }
    }
    //  ZP: 20190729 use its orignal function declaration if it is function
    //  instantiation generated by compiler;
    //  if it is fully instantiation explicitly, we can continue to use it, such
    //  as template<> void f<int>() {...}
    if ((codegen->IsTemplateInstantiationFunctionDecl(sg_curr_decl) != 0) &&
        codegen->IsCompilerGenerated(sg_curr_decl)) {
      sg_curr_decl = codegen->GetTemplateFuncDecl(sg_curr_decl);
    }

    if ((sg_curr_decl == nullptr) || codegen->IsNULLFile(sg_curr_decl)) {
      continue;
    }
    if (func_decls->count(sg_curr_decl) == 0) {
      func_decls->insert(sg_curr_decl);
      get_sub_funcs_recurs(codegen, sg_curr_decl, func_decls);
    }
  }
}

Value gen_kernel_hierarchy(CSageCodeGen *codegen, void *kernel_node,
                           Document::AllocatorType *p_alloc, bool hls_flow) {
  debug_print("gen_kernel_hierarchy:%p\n", kernel_node);
  Value v = create_info_node(codegen, kernel_node, kKernel, p_alloc);

  Value global_variables =
      get_used_global_variables(codegen, kernel_node, p_alloc, hls_flow);
  v.AddMember("global_variables", global_variables, *p_alloc);

  Value interfaces = get_interfaces(codegen, kernel_node, p_alloc);
  v.AddMember("interfaces", interfaces, *p_alloc);

  Value arrays = get_arrays(codegen, kernel_node, p_alloc);
  v.AddMember("arrays", arrays, *p_alloc);

  Value var_refs = get_var_ref(codegen, kernel_node, p_alloc);
  v.AddMember("var_refs", var_refs, *p_alloc);

  Value var_defs = get_var_def(codegen, kernel_node, p_alloc);
  v.AddMember("var_defs", var_defs, *p_alloc);

  Value stmts = get_stmts(codegen, kernel_node, p_alloc);
  v.AddMember("stmts", stmts, *p_alloc);

  Value loop(kArrayType);
  string parent_func_name = codegen->GetFuncName(kernel_node);
  gen_loop_func_hierarchy(codegen, kernel_node, &loop, parent_func_name,
                          p_alloc);
  v.AddMember("childs", loop, *p_alloc);

  Value sub_funcs(kArrayType);
  SetVector<void *> functions;
  get_sub_funcs_recurs(codegen, kernel_node, &functions);
  for (auto f : functions) {
    if (f == kernel_node) {
      continue;
    }
    auto ret = gen_func_hierarchy(codegen, f, p_alloc);
    sub_funcs.PushBack(ret, *p_alloc);
  }
  v.AddMember("sub_functions", sub_funcs, *p_alloc);

  Value call_funcs = get_call_functions(codegen, kernel_node, p_alloc);
  v.AddMember("call_functions", call_funcs, *p_alloc);
  return v;
}

Value gen_func_hierarchy(CSageCodeGen *codegen, void *func_node,
                         Document::AllocatorType *p_alloc) {
  debug_print("gen_func_hierarchy:%p = %s\n", func_node,
              codegen->UnparseToString(func_node).c_str());
  Value v = create_info_node(codegen, func_node, kFunction, p_alloc);

  Value arrays = get_arrays(codegen, func_node, p_alloc);
  v.AddMember("arrays", arrays, *p_alloc);

  Value args = get_arguments(codegen, func_node, p_alloc);
  v.AddMember("args", args, *p_alloc);

  Value loop(kArrayType);
  string parent_func_name = codegen->GetFuncName(func_node);
  gen_loop_func_hierarchy(codegen, func_node, &loop, parent_func_name, p_alloc);
  v.AddMember("childs", loop, *p_alloc);

  Value call_funcs = get_call_functions(codegen, func_node, p_alloc);
  v.AddMember("call_functions", call_funcs, *p_alloc);

  Value var_refs = get_var_ref(codegen, func_node, p_alloc);
  v.AddMember("var_refs", var_refs, *p_alloc);

  Value var_defs = get_var_def(codegen, func_node, p_alloc);
  v.AddMember("var_defs", var_defs, *p_alloc);

  Value stmts = get_stmts(codegen, func_node, p_alloc);
  v.AddMember("stmts", stmts, *p_alloc);

  return v;
}

void gen_hierarchy_top(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options) {
  debug_print("gen_hierarchy_top:%p\n", pTopFunc);
  string tool_version = options.get_option_key_value("-a", "tool_version");
  bool hls_flow = false;
  if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
    hls_flow = true;
  }
  string mode = options.get_option_key_value("-a", "mode");
  if (mode != "simple") {
    string input = options.get_option_key_value("-a", "input");
    if (input.empty()) {
      input = "inter_report.json";
    }
    string output = options.get_option_key_value("-a", "output");
    if (output.empty()) {
      output = "src_inter_report.json";
    }
    string trans_id_flag = options.get_option_key_value("-a", "switch");
    map<string, map<string, string>> org_report;
    if (!trans_id_flag.empty()) {
      readInterReport(input, &org_report);
    }
    debug_print("read inter report:%p\n", pTopFunc);
    for (auto it = org_report.begin(); it != org_report.end(); ++it) {
      string key_name = "org_identifier";
      if (it->second.count(key_name) <= 0) {
        continue;
      }
      string org_id = it->second[key_name];
      debug_print("org_id:%s\n", org_id.c_str());
      if (org_id.empty() || !is_legal_topo_id(org_id)) {
        fprintf(stderr, "illegal org_identifier: %s, topo_id: %s \n",
                org_id.c_str(), it->first.c_str());
        continue;
      }
      debug_print("before getSgNode%s\n", "");
      SgNode *org_node = getSgNode(codegen, org_id);
      if (org_node == nullptr) {
        fprintf(stderr, "cant find source node, wrong org_identifier: %s\n",
                org_id.c_str());
        continue;
      }
      debug_print("before get_file_info%s\n", "");
      if ((static_cast<SgNode *>(org_node)->get_file_info() == nullptr)) {
        fprintf(stderr, "cant find file info, wrong org_identifier: %s\n",
                org_id.c_str());
        continue;
      }
      debug_print("after get_file_info%s\n", "");
      int line = codegen->get_line(org_node);
      int col = codegen->get_col(org_node);
      string src = codegen->get_file(org_node);
      it->second["src_line"] = std::to_string(line);
      it->second["src_col"] = std::to_string(col);
      it->second["src_filename"] = src;
    }
    debug_print("parse org id:%p\n", pTopFunc);
    if (!trans_id_flag.empty()) {
      writeInterReport(output, org_report);
    }
    const int all_visit_types[] = {
        V_SgForStatement,         V_SgDoWhileStmt,     V_SgWhileStmt,
        V_SgFunctionDeclaration,  V_SgFunctionCallExp, V_SgVariableDeclaration,
        V_SgDeclarationStatement, V_SgInitializedName};

    vector<void *> all_nodes;
    for (auto t : all_visit_types) {
      vector<void *> nodes;
      codegen->GetNodesByType_int(pTopFunc, "preorder", t, &nodes);
      debug_print("get %d nodes, type: %d\n", static_cast<int>(nodes.size()),
                  t);
      for (auto n : nodes) {
        if (n == nullptr) {
          continue;
        }
        auto sg_node = static_cast<SgNode *>(n);
        string src_topo_id = getTopoLocation(codegen, sg_node);
        if (!src_topo_id.empty()) {
          g_topo_ids[n] = src_topo_id;
          debug_print("%p:%s:%s\n", n, codegen->UnparseToString(n).c_str(),
                      src_topo_id.c_str());
        }
      }
    }
    vector<void *> defines;
    //  for workaround
    codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgFunctionDefinition,
                                &defines);
    for (auto d : defines) {
      if (d == nullptr) {
        continue;
      }
      SgFunctionDefinition *def =
          isSgFunctionDefinition(static_cast<SgNode *>(d));
      if (def == nullptr) {
        continue;
      }
      SgMemberFunctionDeclaration *decl =
          isSgMemberFunctionDeclaration(def->get_declaration());
      if (decl == nullptr) {
        continue;
      }
      auto sg_node = static_cast<SgNode *>(decl);
      string src_topo_id = getTopoLocation(codegen, sg_node);
      if (!src_topo_id.empty() && g_topo_ids.count(decl) == 0) {
        g_topo_ids[decl] = src_topo_id;
        debug_print("%p:%s:%s\n", sg_node,
                    codegen->UnparseToString(sg_node).c_str(),
                    src_topo_id.c_str());
      }
    }
  }

  debug_print("write inter report:%p\n", pTopFunc);
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  Document d(kArrayType);
  Document::AllocatorType &alloc = d.GetAllocator();
  debug_print("mars ir :%p\n", pTopFunc);
  auto kernels = mars_ir.get_top_kernels();
  if (kernels.empty()) {
    map<string, map<string, string>> KernelGroup;
    readInterReport("kernel_list.json", &KernelGroup);
    for (auto OneKernel : KernelGroup) {
      string TopKernelName = OneKernel.first;
      void *pKernel = codegen->GetFuncDeclByName(TopKernelName, 1);
      kernels.push_back(pKernel);
    }
  }
  for (auto kernel : kernels) {
    Value k = gen_kernel_hierarchy(codegen, kernel, &alloc, hls_flow);
    d.PushBack(k, alloc);
  }

  string hierarchy_url = options.get_option_key_value("-a", "hierarchy");
  if (hierarchy_url.empty()) {
    hierarchy_url = "hierarchy.json";
  }
  std::ofstream ofs(hierarchy_url);
  OStreamWrapper osw(ofs);
  PrettyWriter<OStreamWrapper> writer(osw);
  d.Accept(writer);
}
