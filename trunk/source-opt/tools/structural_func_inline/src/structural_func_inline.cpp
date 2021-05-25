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
#include <unordered_map>
#include <unordered_set>

#include "file_parser.h"
#include "math.h"

#include "codegen.h"
#include "ir_types.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "rose.h"

using std::unordered_map;
using std::unordered_set;

class StructuralInline {
 public:
  enum FunctionInlineProperty { FLATTEN = 1, INNER, LOOPSINGLECALL };
  StructuralInline(CMarsAST_IF *codegen_in, CMarsIrV2 *mars_ir_in,
                   const CInputOptions &options_in, string mode)
      : codegen(codegen_in), mars_ir(mars_ir_in), options(options_in),
        process_mode(mode) {}

  int run(void *kernel);

 protected:
  bool HasPragma(void *scope, const string &key);
  bool UpperFnHasLoop(void *fn_call);
  bool ScopeContainLoop(void *scope);
  // TODO(Min) : Promote to codegen
  bool IsAllFnCallInline(void *fn_decl);
  int GetLoopIITarget(void *loop);
  int InsertInlinePragma(void *fn_call);
  unordered_set<void *> GetUpperLoopScopes(void *fn_call);
  void TagFnCallProp(void *fn_call, void *caller_decl);
  void BuildUpCallGraph(void *fn_decl);
  void SetPragma();
  bool contains_parallel_logic(void *scope, void *fn_call);

 protected:
  CMarsAST_IF *codegen;
  CMarsIrV2 *mars_ir;
  CInputOptions options;
  string process_mode;
  unordered_map<void *, unordered_map<void *, unordered_set<void *>>>
      call_graph;
  unordered_map<void *, int> callee_stats;
  unordered_map<void *, FunctionInlineProperty> fn_tags;
  unordered_map<void *, unordered_set<void *>> upper_loop_visited;
  unordered_set<void *> inlined_func_HLS;
  unordered_map<void *, void *> cp_fn_map;
  unordered_map<void *, void *> call_to_scope;
  unordered_map<void *, unordered_set<void *>> scope_to_calls;
};

bool StructuralInline::HasPragma(void *scope, const string &key) {
  if (scope == nullptr) {
    return false;
  }
  vector<void *> pragmas;
  int is_compatible = 1;
  codegen->GetNodesByType(scope, "preorder", "SgPragmaDeclaration", &pragmas,
                          is_compatible != 0);
  for (auto one_pragma : pragmas) {
    void *loop_scope = codegen->TraceUpToLoopScope(one_pragma);
    void *func_scope = codegen->TraceUpToFuncDecl(one_pragma);
    if (key == "dataflow" &&
        (loop_scope != nullptr ? loop_scope != scope : func_scope != scope))
      continue;
    if ((key == "unroll" || key == "parallel") && (loop_scope != scope)) {
      continue;
    }
    bool allow_hls = true;
    string pragma_name =
        mars_ir->get_pragma_attribute(one_pragma, key, allow_hls);
    boost::algorithm::to_lower(pragma_name);
    if (pragma_name == key) {
      return true;
    }
  }
  return false;
}

bool StructuralInline::UpperFnHasLoop(void *fn_call) {
  void *par_decl = codegen->TraceUpToFuncDecl(fn_call);
  if (ScopeContainLoop(par_decl))
    return true;
  vector<void *> par_calls;
  codegen->GetFuncCallsFromDecl(par_decl, nullptr, &par_calls);
  for (auto &call : par_calls) {
    if (UpperFnHasLoop(call))
      return true;
  }
  return false;
}

bool StructuralInline::ScopeContainLoop(void *scope) {
  if (scope == nullptr)
    return false;
  vector<void *> for_loops, while_loops, do_while_loops;
  codegen->GetNodesByType(scope, "preorder", "SgForStatement", &for_loops);
  codegen->GetNodesByType(scope, "preorder", "SgWhileStmt", &while_loops);
  codegen->GetNodesByType(scope, "preorder", "SgDoWhileStmt", &do_while_loops);

  for (auto &loop : for_loops) {
    if (!HasPragma(loop, "unroll") && loop != scope) {
      return true;
    }
  }
  for (auto &loop : while_loops) {
    if (!HasPragma(loop, "unroll") && loop != scope) {
      return true;
    }
  }
  for (auto &loop : do_while_loops) {
    if (!HasPragma(loop, "unroll") && loop != scope) {
      return true;
    }
  }
  vector<void *> vec_calls;
  codegen->GetNodesByType(scope, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto &call : vec_calls) {
    void *decl = codegen->GetFuncDeclByCall(call);
    if (decl == scope)
      continue;
    if (codegen->IsMemCpy(call)) {
      return true;
    }
    if (ScopeContainLoop(decl))
      return true;
  }
  return false;
}

bool StructuralInline::IsAllFnCallInline(void *func_decl) {
  vector<void *> vec_calls;
  codegen->GetFuncCallsFromDecl(func_decl, codegen->GetGlobal(func_decl),
                                &vec_calls);
  int cnt = 0;
  void *caller = nullptr;
  bool is_flatten = false;
  bool is_in_same_caller = false;
  bool is_inlined = false;
  for (auto &one_call : vec_calls) {
    if (fn_tags.count(one_call)) {
      if (fn_tags[one_call] == LOOPSINGLECALL || fn_tags[one_call] == INNER) {
        cnt++;
        is_inlined = true;
      } else if (fn_tags[one_call] == FLATTEN) {
        is_flatten = true;
      }
    }
    void *curr_caller = codegen->TraceUpToFuncDecl(one_call);
    if (caller == nullptr) {
      caller = curr_caller;
      is_in_same_caller = true;
    } else {
      is_in_same_caller &= caller == curr_caller;
    }
  }
  if ((is_inlined || vec_calls.size() == 1) && is_flatten &&
      is_in_same_caller) {
    return true;
  }
  return cnt == vec_calls.size();
}

unordered_set<void *> StructuralInline::GetUpperLoopScopes(void *fn_call) {
  unordered_set<void *> ret;
  if (fn_call == nullptr)
    return ret;
  if (upper_loop_visited.count(fn_call))
    return upper_loop_visited[fn_call];
  void *fn_decl = codegen->GetFuncDeclByCall(fn_call);
  void **rec_func = nullptr;
  if (fn_decl == nullptr || codegen->IsRecursiveFunction(fn_decl, rec_func))
    return ret;

  void *loop = codegen->TraceUpToLoopScope(fn_call);
  if (loop != nullptr) {
    ret.insert(loop);
    upper_loop_visited[fn_call] = ret;
    return ret;
  }
  void *par_decl = codegen->TraceUpToFuncDecl(fn_call);
  vector<void *> par_calls;
  codegen->GetFuncCallsFromDecl(par_decl, nullptr, &par_calls);
  for (auto &call : par_calls) {
    unordered_set<void *> upper_call_loops = GetUpperLoopScopes(call);
    for (auto &upper_call_loop : upper_call_loops) {
      ret.insert(upper_call_loop);
    }
  }
  upper_loop_visited[fn_call] = ret;
  return ret;
}

int StructuralInline::GetLoopIITarget(void *loop) {
  if (loop == nullptr) {
    return 0;
  }

  vector<void *> pragmas;
  int is_compatible = 1;
  codegen->GetNodesByType(loop, "preorder", "SgPragmaDeclaration", &pragmas,
                          is_compatible != 0);
  for (auto one_pragma : pragmas) {
    bool allow_hls = true;
    string II_num = mars_ir->get_pragma_attribute(one_pragma, "II", allow_hls);
    if (!II_num.empty() &&
        II_num.find_first_not_of("0123456789") == std::string::npos) {
      return stoi(II_num);
    } else {
      cerr << "invalid II " << II_num << '\n';
    }
  }

  return 1;
}

// no loop under body or all loop has been labelled as parallel complete
void StructuralInline::TagFnCallProp(void *fn_call, void *caller_decl) {
  if (fn_call == nullptr)
    return;
  void *fn_decl = codegen->GetFuncDeclByCall(fn_call);
  if (fn_decl == nullptr)
    return;

  if (HasPragma(fn_decl, "inline") || HasPragma(fn_decl, "dataflow")) {
    return;
  }
  auto parent_loop = codegen->TraceUpToLoopScope(fn_call);
  auto par_decl = codegen->TraceUpToFuncDecl(fn_call);
  if (parent_loop != nullptr) {
    if (HasPragma(parent_loop, "dataflow"))
      return;
  } else {
    if (HasPragma(par_decl, "dataflow"))
      return;
  }
  if (ScopeContainLoop(fn_decl)) {
    if (parent_loop != nullptr) {
      if (GetLoopIITarget(parent_loop) != 1 ||
          HasPragma(parent_loop, "parallel") ||
          HasPragma(parent_loop, "unroll") ||
          contains_parallel_logic(parent_loop, fn_call)) {
        return;
      }
    } else {
      if (contains_parallel_logic(par_decl, fn_call)) {
        return;
      }
    }
    fn_tags[fn_call] = LOOPSINGLECALL;
    return;
  } else {
    fn_tags[fn_call] = FLATTEN;
    auto parent_loops = GetUpperLoopScopes(fn_call);
    for (auto parent_loop : parent_loops) {
      if (parent_loop == nullptr ||
          ScopeContainLoop(codegen->GetLoopBody(parent_loop)) ||
          GetLoopIITarget(parent_loop) != 1) {
        return;
      }
    }
    // No parent loop, meaning on the top level.
    // but have other loops of the same level as the call.
    if (parent_loops.empty() && UpperFnHasLoop(fn_call)) {
      return;
    }
    fn_tags[fn_call] = INNER;
  }
}

void StructuralInline::BuildUpCallGraph(void *fn_decl) {
  if (fn_decl == nullptr)
    return;
  cout << "Building up call graph for: " << codegen->_p(fn_decl) << "\n";
  vector<void *> func_calls;
  codegen->GetNodesByType(fn_decl, "preorder", "SgFunctionCallExp",
                          &func_calls);
  for (auto one_call : func_calls) {
    void *one_decl = codegen->GetFuncDeclByCall(one_call);
    // negelect header calls and member fucntion calls
    if (codegen->IsMemberFunction(one_decl))
      continue;
    void *one_body = codegen->GetFuncBody(one_decl);
    if (codegen->isWithInHeaderFile(one_body))
      continue;

    call_graph[fn_decl][one_decl].insert(one_call);
    callee_stats[one_decl]++;
    TagFnCallProp(one_call, fn_decl);

    if (!call_graph.count(one_decl)) {
      BuildUpCallGraph(one_decl);
    }
  }
}

int StructuralInline::InsertInlinePragma(void *call_or_func) {
  cout << "Inlining: " << codegen->_p(call_or_func) << endl;

  void *func_decl = call_or_func;
  void *call = nullptr;
  if (codegen->IsFunctionCall(call_or_func)) {
    func_decl = codegen->GetFuncDeclByCall(call_or_func);
    call = call_or_func;
  }
  //  create a function decl copy
  if (func_decl == nullptr) {
    return 0;
  }
  void *func_body = codegen->GetFuncBody(func_decl);
  if (func_body == nullptr) {
    return 0;
  }

  if (inlined_func_HLS.count(func_decl)) {
    return 0;
  }

  if (cp_fn_map.count(func_decl) == 0U) {
    if (!IsAllFnCallInline(func_decl)) {
      //  get a new name;
      string fn_name = codegen->GetFuncName(func_decl);
      string new_fn_name = fn_name + "_cp_";
      codegen->get_valid_global_name(codegen->GetGlobal(func_decl),
                                     &new_fn_name);

      void *new_fn_decl = codegen->CloneFuncDecl(
          func_decl, codegen->GetGlobal(func_decl), new_fn_name);
      void *new_fn_body = codegen->GetFuncBody(new_fn_decl);
      if (new_fn_body == nullptr) {
        return 0;
      }

      void *new_pragma = codegen->CreatePragma("HLS inline", new_fn_body);
      codegen->PrependChild(new_fn_body, new_pragma);

      inlined_func_HLS.insert(new_fn_decl);

      //  insert it to somewhere
      codegen->InsertAfterStmt(new_fn_decl, func_decl);

      cp_fn_map[func_decl] = new_fn_decl;
    } else {  //  used once only, inline decl directly
      void *new_pragma = codegen->CreatePragma("HLS inline", func_body);
      codegen->PrependChild(func_body, new_pragma);
      inlined_func_HLS.insert(func_decl);
      return 1;
    }
  }
  if (call == nullptr)
    return 1;
  void *cp_fn_decl = cp_fn_map[func_decl];

  //  replace call to copied fn decl
  vector<void *> fn_params = codegen->GetFuncCallParamList(call);
  vector<void *> cp_params;
  for (auto fn_param : fn_params) {
    void *cp_param = codegen->CopyExp(fn_param);
    cp_params.push_back(cp_param);
  }
  void *new_call = codegen->CreateFuncCall(cp_fn_decl, cp_params);
  codegen->ReplaceExp(call, new_call);

  return 1;
}

void StructuralInline::SetPragma() {
  for (auto &node : call_graph) {
    cout << "========" << codegen->_p(node.first) << "========\n";
    for (auto &call_subfn : node.second) {
      for (auto &one_call : call_subfn.second) {
        void *one_decl = call_subfn.first;
        cout << "  " << codegen->_p(one_decl) << " -> " << codegen->_p(one_call)
             << "\n";
      }
    }
  }
  for (auto &node : call_graph) {
    for (auto &call_subfn : node.second) {
      for (auto &one_call : call_subfn.second) {
        if (!fn_tags.count(one_call))
          continue;
        // void *ret_ty =
        //     codegen->GetFuncReturnType(codegen->GetFuncDeclByCall(one_call));
        // 1. Dont want to inline non-void function. Corresponding
        //    ACCEL pragmas will be redesigned.
        // 2. HLS yeild bad performance with inlining a function under
        // conditional scope.
        // if (!codegen->IsVoidType(ret_ty))
        //   continue;
        //  void *if_guard = codegen->TraceUpByTypeCompatible(
        //  one_call, V_SgIfStmt);
        //  if (codegen->IsControlStatement(if_guard))
        //    continue;
        void *one_decl = call_subfn.first;
        if (fn_tags[one_call] == FLATTEN) {
          if (callee_stats.count(one_decl) && callee_stats[one_decl] == 1) {
            InsertInlinePragma(one_call);
          }
        } else if (fn_tags[one_call] == INNER ||
                   fn_tags[one_call] == LOOPSINGLECALL) {
          InsertInlinePragma(one_call);
        }
      }
    }
  }
}

bool StructuralInline::contains_parallel_logic(void *scope, void *fn_call) {
  // one function call only be paralleled with paralleled loop, single
  // operations and other function calls one function call cannot be paralleled
  // with pipelined loop
  vector<void *> vec_calls;
  codegen->GetNodesByType(scope, "preorder", "SgFunctionCallExp", &vec_calls);
  vector<void *> vec_real_calls;
  for (auto call : vec_calls) {
    if (codegen->IsMemCpy(call) || codegen->IsAssertFailCall(call) ||
        codegen->IsSupportedMerlinSystemFunctionCall(call))
      continue;
    string func_name = codegen->GetFuncNameByCall(call);
    if (func_name.find("memcpy_wide_bus") == 0)
      continue;
    vec_real_calls.push_back(call);
  }
  // TODO(youxiang) check wheter the multiple calls have dependences
  return vec_real_calls.size() >= 2;
}

int StructuralInline::run(void *kernel) {
  cout << "[StructuralInline] processing " << codegen->_up(kernel) << " ..."
       << endl;
  call_graph.clear();
  callee_stats.clear();
  fn_tags.clear();
  upper_loop_visited.clear();
  BuildUpCallGraph(kernel);
  SetPragma();
  return 1;
}

int StructuralFuncInlineTop(CSageCodeGen *codegen, void *pTopFunc,
                            const CInputOptions &options) {
  printf("Merlin Pass [Structural Function Inlining] started ... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  StructuralInline str_inline(codegen, &mars_ir, options, "");

  auto kernels = mars_ir.get_top_kernels();

  for (auto kernel : kernels) {
    str_inline.run(kernel);
  }
  return 0;
}

//  #endif //  HAS_AUTO_FUNC_INLINE
