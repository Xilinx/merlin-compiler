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


//  ///////////////////////////////////////////////////////////  /
//  kernel_separate.cpp
//
//  A source file in Merlin compiler
//
//  Description:
//      This pass creates a new file that contains the necessary
//      sub-functions and declarations for the specified kernels.
//
//  Author(s):
//      Peng Zhang (2016-10-25)
//      Tianyi Lu  (2017-05-09)
//      Peng Zhang (2017-09-02, generate_l2_api, task-level API)
//  ///////////////////////////////////////////////////////////  /
#include <unordered_map>
#include <unordered_set>
#include "kernel_separate.h"
#include "id_update.h"
#include "report.h"
#include "stream_ir.h"
#include "mars_opt.h"
#include "pool_visit.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>

#define DEBUG_L2_API_GEN 1

#define KERNEL_FILE_PREFIX "__merlinkernel_"
//  also defined in kernel_wrapper.cpp
#define KERNEL_FILE_LIST "kernel.list"
#define HOST_FILE_LIST "host.list"
#define KERNEL_TASK_API_PREFIX "__merlin_"
#define TASK_FILE_PREFIX "__merlintask_"
#define TASK_HEAD_FILE_PRE "__merlintask_top_h"
#define TASK_HEAD_FILE "__merlintask_top.h"
#define MERLIN_TYPE_DEFINE "merlin_type_define.h"
#define KERNEL_HEADER_FILE "__merlinhead_kernel_top.h"

using MerlinStreamModel::CStreamChannel;
using MerlinStreamModel::CStreamIR;
using MerlinStreamModel::DEFAULT_CHANNEL_DEPTH;
using std::unordered_map;
using std::unordered_set;
void check_stream_model(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options);
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
static bool is_numeric(std::string number) {
  const std::set<std::string> integer_suffix{"u",  "U",  "L",  "l",   "LL",
                                             "ll", "ul", "UL", "ull", "ULL"};
  const std::set<std::string> float_suffix{"f", "F", "l", "L"};
  size_t sz;
  try {
    std::stoll(number, &sz, 0);
    if (sz == number.size() || integer_suffix.count(number.substr(sz)) > 0)
      return true;
  } catch (std::exception &a) {
  }

  try {
    std::stold(number, &sz);
    if (sz == number.size() || float_suffix.count(number.substr(sz)) > 0)
      return true;
  } catch (std::exception &a) {
  }
  return false;
}

static void add_namespace_prefix(string *type_string, const string &pattern,
                                 const string &replacement) {
  string::size_type pos = type_string->find(pattern);
  while (pos != string::npos) {
    type_string->replace(pos, pattern.size(), replacement);
    pos = type_string->find(pattern);
  }
  return;
}

// ZP: MER-1532
void simplify_constant_val(CSageCodeGen *ast, void *pTopFunc) {
  vector<void *> v_ref;
  ast->GetNodesByType_int(nullptr, "preorder", V_SgValueExp, &v_ref, true);
  for (auto x_ : v_ref) {
    auto x = static_cast<SgValueExp *>(x_);
    // the child of SgType has different parent link, so it will crash in
    // ReplaceStmt
    if (!x->get_parent())
      continue;

    // avoid unnecessary replacement
    string str = x->unparseToString();
    if (is_numeric(str))
      continue;

//     cout << "-- SCV Before: " << ASTDebug::print_brief(x) << " " <<
//     x->unparseToString() << endl;

//    ZP: investigation results:
//    - The value and valueString can be changed independently
//    - But these changes does not reflect in unparseToString() results
//    and final code generation if (isSgCharVal(x)) {
//      cout << "-- B Value: " << my_itoa(isSgCharVal(x)->get_value()) <<
//      endl; cout << "-- B ValueString: " <<
//      isSgCharVal(x)->get_valueString() << endl;
//      isSgCharVal(x)->set_value(isSgCharVal(x)->get_value()+1);
//      isSgCharVal(x)->set_valueString("1024");
//      cout << "-- A Value: " << my_itoa(isSgCharVal(x)->get_value()) <<
//      endl; cout << "-- A ValueString: " <<
//      isSgCharVal(x)->get_valueString() << endl;
//    }

// Note: For the short integers (e.g. char or short), ROSE may generate
// ConstValues
//       like 'char(127)' which is not valid in C files, we check if generated
//       value is still not 'digital', we use its Integer value instead.
#define REPLACE_VAL_WITH_NEW_TYPE(TYPE, n, NEW_TYPE)                           \
  if (isSg##TYPE(n)) {                                                         \
    SgExpression *new_value =                                                  \
        SageBuilder::build##TYPE(isSg##TYPE(n)->get_value());                  \
    SgExpression *new_value_n =                                                \
        SageBuilder::build##NEW_TYPE(isSg##TYPE(n)->get_value());              \
    if (!is_numeric(new_value->unparseToString()) &&                           \
        is_numeric(new_value_n->unparseToString()))                            \
      new_value = new_value_n;                                                 \
    auto parent = (n)->get_parent();                                           \
    if (auto sg_array_type = isSgArrayType(parent)) {                          \
      if (sg_array_type->get_index() == (n)) {                                 \
        sg_array_type->set_index(new_value);                                   \
      }                                                                        \
    } else if (auto sg_case_option_stmt = isSgCaseOptionStmt(parent)) {        \
      if (sg_case_option_stmt->get_key() == (n)) {                             \
        sg_case_option_stmt->set_key(new_value);                               \
      }                                                                        \
    } else if (ast->IsSupportedReplaceExpressionParent(parent)) {              \
      SageInterface::replaceExpression(static_cast<SgExpression *>(n),         \
                                       new_value, true);                       \
    } else {                                                                   \
      cout << "Warning: meet unsupported parent of SgValueExp: "               \
           << parent->class_name() << endl;                                    \
    }                                                                          \
  }
#define REPLACE_VAL(TYPE, n) REPLACE_VAL_WITH_NEW_TYPE(TYPE, n, TYPE)

    // Complete List of SgValueExp (ZP: 2019-11-25)
    REPLACE_VAL(BoolValExp, x);
    REPLACE_VAL(Char16Val, x);
    REPLACE_VAL(Char32Val, x);
    REPLACE_VAL_WITH_NEW_TYPE(CharVal, x, IntVal);
    // REPLACE_VAL(ComplexVal, x); // do not have get_value
    REPLACE_VAL(DoubleVal, x);
    // REPLACE_VAL(EnumVal, x); // buildEnumVal(SgEnumFieldSymbol * )
    // REPLACE_VAL(Float128Val, x); // do not have type Float128 or Float80
    // REPLACE_VAL(Float80Val, x);
    REPLACE_VAL(FloatVal, x);
    REPLACE_VAL(IntVal, x);
    REPLACE_VAL(LongDoubleVal, x);
    REPLACE_VAL(LongIntVal, x);
    REPLACE_VAL(LongLongIntVal, x);
    // REPLACE_VAL(NullptrValExp, x);   // no get_value()
    REPLACE_VAL_WITH_NEW_TYPE(ShortVal, x, IntVal);
    REPLACE_VAL(StringVal, x);
    // REPLACE_VAL(TemplateParameterVal, x); // no get_value()
    REPLACE_VAL_WITH_NEW_TYPE(UnsignedCharVal, x, UnsignedIntVal);
    REPLACE_VAL(UnsignedIntVal, x);
    REPLACE_VAL(UnsignedLongLongIntVal, x);
    REPLACE_VAL(UnsignedLongVal, x);
    REPLACE_VAL_WITH_NEW_TYPE(UnsignedShortVal, x, UnsignedIntVal);
    // REPLACE_VAL(UpcMythread, x);
    // REPLACE_VAL(UpcTheads, x);
    // REPLACE_VAL(VoidVal, x);  // no SgVoidVal
    REPLACE_VAL(WcharVal, x);

    //    cout << "-- SCV After: " << ASTDebug::print_brief(x) << " " <<
    //    x->unparseToString() << endl;
  }
}

enum INLINE_TYPE { INLINED_NOTHING = 0, INLINED_CLASS = 1, INLINED_FUNC = 2 };
//  20170519
set<string> header_list;
int m_error;
void *get_type_decl_location(CSageCodeGen *ast, void *type) {
  void *decl = ast->GetTypeDeclByType(type, 1);
  if (decl == nullptr) {
    decl = ast->GetTypeDeclByType(type, 0);
    while ((decl != nullptr) && (ast->is_floating_node(decl) != 0)) {
      decl = ast->GetParent(decl);
    }
  }
  if (ast->IsGlobal(decl) != 0) {
    return nullptr;
  }
  return decl;
}

INLINE_TYPE is_inlined_type(CSageCodeGen *ast, void *type,
                            void **non_floating_node) {
  void *decl = ast->GetTypeDeclByType(type, 1);
  if (decl != nullptr) {
    return INLINED_NOTHING;
  }
  bool found_non_floating_node = false;
  if (decl == nullptr) {
    decl = ast->GetTypeDeclByType(type, 0);
    while (decl != nullptr) {
      if (!ast->is_floating_node(decl) && !found_non_floating_node) {
        *non_floating_node = decl;
        found_non_floating_node = true;
      }
      if (ast->IsClassDefinition(decl) != 0) {
        return INLINED_CLASS;
      }
      if (ast->IsFunctionDefinition(decl) != 0) {
        return INLINED_FUNC;
      }
      decl = ast->GetParent(decl);
    }
  }
  return INLINED_NOTHING;
}

void reportUndefinedTypeDecl(CSageCodeGen *ast, void *one_type) {
  string type_name = ast->_up(one_type);
  void *decl = get_type_decl_location(ast, one_type);
  if (type_name.empty() && (decl != nullptr)) {
    type_name = ast->_up(decl);
  }
  string type_info =
      "\'" + type_name + "\' " +
      getUserCodeFileLocation(ast, decl != nullptr ? decl : one_type, true);
  string msg = "Cannot support undefined class type: \n " + type_info;
  dump_critical_message(SEPAR_ERROR_8(type_info), 0);
#ifdef PROJDEBUG
  cout << "ERROR: " << msg << endl;
#endif
}

string strip_anonymous_name(string str) {
  //  strip anonymous name
  size_t pos;
  do {
    pos = str.find("__anonymous_");
    if (pos != string::npos) {
      size_t pre_len = pos;
      pos += string("__anonymous_").size();
      while (pos != str.size() && str[pos] != ' ' && str[pos] != ':') {
        pos++;
      }
      str = str.substr(0, pre_len) + str.substr(pos);
    } else {
      break;
    }
  } while (true);
  return str;
}

void reportAnoymousTypeDecl(CSageCodeGen *ast, void *decl) {
  string str = strip_anonymous_name(ast->_up(decl));
  string type_info =
      "\'" + str + "\' " + getUserCodeFileLocation(ast, decl, true);
  string msg = "Cannot support anonymous class type: \n " + type_info;
  dump_critical_message(SEPAR_ERROR_10(type_info), 0);
#ifdef PROJDEBUG
  cout << "ERROR: " << msg << endl;
#endif
}

void get_immediate_typedef_decl(CSageCodeGen *ast, void **typedef_decl) {
  if (ast->IsTypedefDecl(*typedef_decl) != 0) {
    void *base_type =
        isSgTypedefDeclaration(static_cast<SgNode *>(*typedef_decl))
            ->get_base_type();
    while (ast->IsTypedefType(base_type) != 0) {
      void *prev_decl = ast->GetTypeDeclByType(base_type);
      if (prev_decl != nullptr) {
        *typedef_decl = prev_decl;
      } else {
        break;
      }
      if (ast->IsTypedefDecl(*typedef_decl) != 0) {
        base_type = isSgTypedefDeclaration(static_cast<SgNode *>(*typedef_decl))
                        ->get_base_type();
      } else {
        break;
      }
    }
  }
}

bool skip_class(string type_name) {
  return boost::regex_match(type_name, boost::regex("ap_int <.*> ")) ||
         boost::regex_match(type_name, boost::regex("ap_uint <.*> ")) ||
         boost::regex_match(type_name, boost::regex("ap_fixed <.*> ")) ||
         boost::regex_match(type_name, boost::regex("ap_ufixed <.*> ")) ||
         boost::regex_match(type_name, boost::regex("stream <.*> ")) ||
         boost::regex_match(type_name, boost::regex("Mat <.*> "));
}

class Memo {
  static unordered_set<string> extern_identifiers;
  static unordered_set<string> identifiers;
  map<void *, void *> s_done;

  string
  make_unique(const string &ident, int uid,
              const unordered_set<string> &exclude = unordered_set<string>()) {
    string s = ident + "_" + my_itoa(uid);
    if (identifiers.count(s) <= 0 && extern_identifiers.count(s) <= 0) {
      if (exclude.count(s) <= 0) {
        return s;
      }
    }
    return make_unique(ident, uid + 1, exclude);
  }

 public:
  static void collect_extern_identifiers(CSageCodeGen *cg, void *pTopFunc);
  CSageCodeGen *cg;
  explicit Memo(CSageCodeGen *cg) : cg(cg) {}

  bool has(void *node) { return s_done.count(node) > 0; }

  void add(void *node, void *new_node) { s_done[node] = new_node; }

  void add(string ident) { identifiers.insert(ident); }

  string
  make_unique(const string &ident,
              const unordered_set<string> &exclude = unordered_set<string>()) {
    const int uid = 1;
    if (identifiers.count(ident) <= 0 && extern_identifiers.count(ident) <= 0) {
      return ident;
    }
    return make_unique(ident, uid, exclude);
  }

  unordered_map<SgSymbol *, void *> map_for_variable() {
    unordered_map<SgSymbol *, void *> res;
    for (auto &pa : s_done) {
      if (cg->IsInitName(pa.first)) {
        res[static_cast<SgInitializedName *>(pa.first)
                ->search_for_symbol_from_symbol_table()] = pa.second;
      }
    }
    return res;
  }

  bool has_processed(void *node) {
    if (s_done.count(node) > 0) {
      return true;
    }

    if (cg->IsFunctionDeclaration(node) != 0) {
      for (auto pair : s_done) {
        if (node == pair.first) {
          return true;
        }
      }
      return false;
    }

    if (cg->IsVariableDecl(node) != 0) {
      string name = cg->GetVariableName(node, true);
      for (auto pair : s_done) {
        void *curr_node = pair.first;
        if ((cg->IsVariableDecl(curr_node) != 0) &&
            cg->GetVariableName(curr_node, true) == name) {
          return true;
        }
      }
      return false;
    }

    if (cg->IsTypeDecl(node) != 0) {
      string name = cg->GetTypeName(node, true);
      for (auto pair : s_done) {
        void *curr_node = pair.first;
        if ((cg->IsTypeDecl(curr_node) != 0) &&
            cg->GetTypeName(curr_node, true) == name) {
          return true;
        }
      }
    }
    return false;
  }
};

unordered_set<string> Memo::extern_identifiers;
unordered_set<string> Memo::identifiers;
void Memo::collect_extern_identifiers(CSageCodeGen *cg, void *pTopFunc) {
  vector<void *> nodes;
  cg->GetNodesByType(pTopFunc, "preorder", "SgVariableDeclaration", &nodes);
  for (auto &decl : nodes) {
    if (!cg->IsStatic(decl)) {
      void *name = cg->GetVariableInitializedName(decl);
      if (cg->IsGlobalInitName(name)) {
        Memo::extern_identifiers.insert(cg->GetVariableName(name));
      }
    }
  }
  nodes.clear();
  cg->GetNodesByType(pTopFunc, "preorder", "SgFunctionDeclaration", &nodes);
  for (auto &decl : nodes) {
    if (!cg->IsStatic(decl)) {
      Memo::extern_identifiers.insert(cg->GetFuncName(decl));
    }
  }
}

void *clone_function(CSageCodeGen *ast, void *kernel, void *insert_scope,
                     map<void *, void *> *s_clone_func, Memo *pmemo) {
  string new_func_name = ast->GetFuncName(kernel, true);
  if (new_func_name.find("::") == 0) {
    new_func_name = new_func_name.substr(2);
  }
  new_func_name = ast->legalizeName(new_func_name);
  new_func_name = pmemo->make_unique(new_func_name);
  pmemo->add(new_func_name);

  void *new_func =
      ast->CloneFuncDecl(kernel, insert_scope, new_func_name, true);
  (*s_clone_func)[kernel] = new_func;
  return new_func;
}

void remap_ref_for_cloned_variable(CSageCodeGen *cg, void *insert_scope,
                                   Memo *pmemo) {
  const auto &vmap = pmemo->map_for_variable();
  vector<void *> refs;
  cg->GetNodesByType_int(insert_scope, "preorder", V_SgVarRefExp, &refs);
  for (auto ref : refs) {
    if (cg->GetParent(ref) == nullptr) {
      continue;
    }
    auto it = vmap.find(static_cast<SgVarRefExp *>(ref)->get_symbol());
    if (it != vmap.end())
      cg->ReplaceExp(ref, cg->CreateVariableRef(it->second));
  }
}

void remap_call_for_cloned_function(CSageCodeGen *ast, void *scope,
                                    const map<void *, void *> &s_clone_func) {
  for (const auto &kv : s_clone_func) {
    // The vector 'funcs' collects the function declaration corresponding to
    // each function call within each function clone. The following algorithm
    // for 'funcs' is here because some function call may reference a static
    // function local to its translation unit *and* other functions with the
    // same signature happen to be local to other translation units. So as to
    // find the correct function declaration with such same signature
    // functions, we first get from the original calls the function
    // declarations and then map them to the clone ones.
    vector<void *> funcs;
    vector<void *> calls;
    ast->GetNodesByType(ast->GetFuncBody(kv.first), "preorder",
                        "SgFunctionCallExp", &calls);

    for (auto call : calls) {
      void *fn = ast->GetFuncDeclByCall(call);
      if (fn == nullptr || s_clone_func.count(fn) <= 0) {
        funcs.push_back(nullptr);
      } else {
        funcs.push_back(s_clone_func.find(fn)->second);
      }
    }

    ast->GetNodesByType(ast->GetFuncBody(kv.second), "preorder",
                        "SgFunctionCallExp", &calls);

    for (auto k = calls.size(); k > 0; --k) {
      size_t i = k - 1;
      if (funcs[i] == nullptr) {
        continue;
      } else {
        vector<void *> args;
        for (size_t j = 0; j < ast->GetFuncCallParamNum(calls[i]); ++j) {
          args.push_back(ast->CopyExp(ast->GetFuncCallParam(calls[i], j)));
        }
        ast->ReplaceExp(calls[i], ast->CreateFuncCall(funcs[i], args));
      }
    }
  }
}

set<void *> get_used_global_and_static(CSageCodeGen *ast, void *func,
                                       set<void *> *visit_func,
                                       bool is_kernel = false) {
  set<void *> ret;
  if (visit_func->count(func) > 0) {
    return ret;
  }
  visit_func->insert(func);

  vector<void *> func_calls;
  ast->GetNodesByType_int(func, "preorder", V_SgFunctionCallExp, &func_calls);
  for (auto call : func_calls) {
    void *decl = ast->GetFuncDeclByCall(call, 1);
    if (decl == nullptr) {
      continue;
    }
    auto sub_calls = get_used_global_and_static(ast, decl, visit_func);
    ret.insert(sub_calls.begin(), sub_calls.end());
  }
  vector<void *> ref_vars;
  ast->GetNodesByType_int(func, "preorder", V_SgVarRefExp, &ref_vars);
  for (auto var : ref_vars) {
    void *name = ast->GetVariableInitializedName(var);
    if ((ast->IsGlobalInitName(name) != 0) &&
        (ast->IsFromInputFile(name) != 0 || ast->IsTransformation(name) != 0)) {
      ret.insert(name);
    }
    if (!is_kernel && (ast->IsLocalInitName(name) != 0)) {
      void *decl = ast->GetVariableDecl(var);
      if (ast->IsStatic(decl)) {
        ret.insert(name);
      }
    }
  }
  return ret;
}

bool check_system_header_file(CSageCodeGen *ast, void *decl, bool skip) {
  if (nullptr == decl) {
    return false;
  }
  if (auto *tf = ast->GetTemplateFuncDecl(decl)) {
    decl = tf;
  }
  if (auto *tmf = ast->GetTemplateMemFuncDecl(decl)) {
    decl = tmf;
  }
  if (auto *tc = ast->GetTemplateClassDecl(decl)) {
    decl = tc;
  }
  if ((ast->IsFromInputFile(decl) == 0) && !ast->IsCompilerGenerated(decl) &&
      !ast->IsTransformation(decl)) {
    string header_name = ast->get_file(decl);

    if (header_name == "NULL_FILE") {
      return false;
    }

    // It is not always possible to include one of the following headers by
    // specifying the absolute path to them. For example, in cmathcalls.h,
    // there's an #error directive prompting not to include that header file
    // directly but rather another header file complex.h.
    if (header_name == "mathcalls.h") {
      header_name = "math.h";
    } else if (header_name == "cmathcalls.h") {
      header_name = "complex.h";
    } else if (header_name == "stdint-gcc.h" || header_name == "types.h") {
      header_name = "stdint.h";
    } else if (header_name == "libio.h") {
      header_name = "stdio.h";
    }

    if (!skip) {
      header_list.insert(header_name);
    }
    return true;
  }
  return false;
}

bool check_internal_system_header_file(CSageCodeGen *ast, void *decl,
                                       bool skip = false) {
  static set<string> ap_unused{"ap_int_ref.h", "ap_fixed_ref.h"};
  if (auto *tf = ast->GetTemplateFuncDecl(decl)) {
    decl = tf;
  }
  if (auto *tmf = ast->GetTemplateMemFuncDecl(decl)) {
    decl = tmf;
  }
  if (auto *tc = ast->GetTemplateClassDecl(decl)) {
    decl = tc;
  }
  string header_name = ast->get_file(decl);
  if (ast->IsFromMerlinSystemFile(decl) != 0) {
    if (!skip) {
      if (ap_unused.count(header_name) > 0) {
        header_name = "ap_int.h";
      }
      header_list.insert(header_name);
    }
    return true;
  }
  return false;
}

void reportSystemStruct(CSageCodeGen *ast, void *type, void *decl) {
  string node_info = "\'" + strip_anonymous_name(ast->_up(type, 0)) + "\'";
  if (nullptr != decl) {
    node_info +=
        " (" + ast->get_file(decl) + ":" + my_itoa(ast->get_line(decl)) + ")";
  }
  dump_critical_message(SEPAR_ERROR_7(node_info), 0);
  m_error++;
}

void reportInlinedType(CSageCodeGen *ast, void *type, void *decl) {
  string node_info = "\'" + strip_anonymous_name(ast->_up(type, 0)) + "\'";
  if (nullptr != decl) {
    node_info +=
        " (" + ast->get_file(decl) + ":" + my_itoa(ast->get_line(decl)) + ")";
  }
  dump_critical_message(SEPAR_ERROR_20(node_info), 0);
  m_error++;
}

void clean_func_body_before_copy(CSageCodeGen *ast, void *kernel_body) {
  //  20170629 Remove function declaration before copying to avoid failure
  //  in ast->CopyStmt
  vector<void *> v_func_decl;
  ast->GetNodesByType(kernel_body, "postorder", "SgFunctionDeclaration",
                      &v_func_decl);
  for (auto func_decl : v_func_decl) {
    if (ast->IsFunctionDeclaration(func_decl) != 0) {
      ast->ReplaceStmt(func_decl,
                       ast->CreateStmt(V_SgExprStatement,
                                       ast->CreateExp(V_SgNullExpression)));
    }
  }
}

void clean_func_body_after_copy(CSageCodeGen *ast, void *kernel_body) {
  {
    //  Fix Bug2143
    vector<void *> v_usingdir;
    ast->GetNodesByType(kernel_body, "postorder", "SgUsingDirectiveStatement",
                        &v_usingdir);
    for (auto usingdir : v_usingdir) {
      ast->ReplaceStmt(usingdir,
                       ast->CreateStmt(V_SgExprStatement,
                                       ast->CreateExp(V_SgNullExpression)));
    }
  }

  {
    vector<void *> v_usingdecl;
    ast->GetNodesByType(kernel_body, "postorder", "SgUsingDeclarationStatement",
                        &v_usingdecl);
    for (auto usingdecl : v_usingdecl) {
      ast->ReplaceStmt(usingdecl,
                       ast->CreateStmt(V_SgExprStatement,
                                       ast->CreateExp(V_SgNullExpression)));
    }
  }

  {
    vector<void *> v_ns_alias;
    ast->GetNodesByType(kernel_body, "postorder",
                        "SgNamespaceAliasDeclarationStatement", &v_ns_alias);
    for (auto ns_alias : v_ns_alias) {
      ast->ReplaceStmt(ns_alias,
                       ast->CreateStmt(V_SgExprStatement,
                                       ast->CreateExp(V_SgNullExpression)));
    }
  }

  //  remove preprocess directive, such as #include
  {
    vector<void *> v_located_node;
    ast->GetNodesByType(kernel_body, "postorder", "SgLocatedNode",
                        &v_located_node, true);

    for (auto located_node : v_located_node) {
      ast->DiscardIncludePreprocInfo(located_node);
    }
  }
}

void collect_shadowing_identifier(CSageCodeGen *cg, void *sg_name,
                                  unordered_set<string> *res) {
  vector<void *> refs;
  set<void *> scopes;
  cg->get_ref_in_scope(sg_name, cg->GetGlobal(sg_name), &refs);
  for (auto &ref : refs) {
    void *sc = cg->TraceUpToFuncDecl(ref);
    if (sc == nullptr) {
      sc = cg->GetGlobal(ref);
    }
    if (scopes.count(sc) <= 0) {
      vector<void *> names;
      cg->GetNodesByType_int(sc, "preorder", V_SgInitializedName, &names);
      for (auto &na : names) {
        res->insert(cg->GetVariableName(na));
      }
      scopes.insert(sc);
    }
  }
}

struct InsertNode {
  void *Node;
  bool IsType;

  explicit InsertNode(void *node, bool isType = false)
      : Node(node), IsType(isType) {}
};

vector<InsertNode> copy_decl_in_scope_between_files(
    void *scope, void *insert_scope, CSageCodeGen *ast, set<void *> *visited,
    set<void *> *visited_global, Memo *pmemo, map<void *, void *> *s_clone,
    bool cross_function, const set<void *> &all_kernels);

bool check_decl_scope(CSageCodeGen *ast, void *decl, void *insert_scope,
                      set<void *> *visited, set<void *> *visited_global,
                      Memo *pmemo, map<void *, void *> *s_clone,
                      bool cross_function, vector<InsertNode> *res,
                      void *curr_scope, const set<void *> &all_kernels) {
  void *scope = ast->GetScope(decl);
  if (scope == curr_scope) {
    return true;
  }
  void *class_decl = nullptr;
  if (ast->IsClassDefinition(scope) != 0) {
    //  skip inner declaration
    class_decl = ast->GetTypeDeclByDefinition(scope);
    auto sub_res = copy_decl_in_scope_between_files(
        class_decl, insert_scope, ast, visited, visited_global, pmemo, s_clone,
        cross_function, all_kernels);
    res->insert(res->end(), sub_res.begin(), sub_res.end());
    check_decl_scope(ast, class_decl, insert_scope, visited, visited_global,
                     pmemo, s_clone, cross_function, res, curr_scope,
                     all_kernels);
    return true;
  }
  if (ast->TraceUpToFuncDecl(scope) != nullptr) {
    // skip local type declaration defined in a function body
    return true;
  }
  if (visited->count(decl) > 0) {
    return false;
  }
  if (ast->GetParent(decl) == nullptr) {
    return false;
  }
  if ((ast->IsClassDecl(decl) != 0) || (ast->IsTypedefDecl(decl) != 0) ||
      (ast->IsEnumDecl(decl) != 0) ||
      (ast->IsTemplateMemberFunctionDecl(decl) != 0)) {
    if (ast->IsTemplateInstClassDecl(decl) != 0) {
      if (ast->GetTemplateClassDecl(decl) != nullptr) {
        void *tc = ast->GetTemplateClassDecl(decl);
        res->push_back(InsertNode(tc, true));
      }
      if (!ast->IsCompilerGenerated(decl)) {
        res->push_back(InsertNode(decl, true));
      }
    } else {
      res->push_back(InsertNode(decl, true));
    }
  }
  return false;
}

//  NOTE:
//  1. copy all the decls (sub-function, struct, typedef, extern global_var) to
//  the new pos
//  2. do not remove the original ones (so that the duplication can be avoided)
//
//  scope: input scope, all the decl in the scope will be copied
//  insert_scope: the position to insert the new decls

// copy function and global variables into an ordered vector according to use
// dependence by depth first search and postorder but unordered in the same
// level (topological order) and store original type declaration into an ordered
// vector
// input: 'scope' (function declaration, variable declaration, type declaration)
// output: vector<InsertNode> (all used functions, global variables and type
// declarations in 'scope' excluding all processed nodes)
vector<InsertNode> copy_decl_in_scope_between_files(
    void *scope, void *insert_scope, CSageCodeGen *ast, set<void *> *visited,
    set<void *> *visited_global, Memo *pmemo, map<void *, void *> *s_clone,
    bool cross_function, const set<void *> &all_kernels) {
  vector<InsertNode> ret;
  if ((scope == nullptr) || visited->count(scope) > 0) {
    return ret;
  }
  visited->insert(scope);
  if (ast->IsClassDecl(scope) != 0) {
    if (auto *tc_cls = ast->GetTemplateClassDecl(scope)) {
      if (tc_cls != scope) {
        if (visited->count(tc_cls) > 0) {
          return ret;
        }
        visited->insert(tc_cls);
      }
    }
  }

  if (void *parent_decl = ast->GetParent(scope)) {
    if (ast->IsTypedefDecl(parent_decl) != 0) {
      if (visited->count(parent_decl) > 0) {
        return ret;
      }
      visited->insert(parent_decl);
    }
  }

  SetVector<void *> fused_set;
  if ((ast->IsStructureType(scope) != 0) || (ast->IsClassType(scope) != 0) ||
      (ast->IsTypedefType(scope) != 0) || (ast->IsEnumType(scope) != 0) ||
      (ast->IsUnionType(scope) != 0)) {
    fused_set.insert(scope);
  }
  if (ast->IsClassDecl(scope) != 0) {
    fused_set.insert(scope);
  }
  if (cross_function) {
    //  /////////////////////////////////////  /
    //  1. handling all the sub functions, struct, class, typedef
    //  /////////////////////////////////////  /
    //  1.1 sub function call
    vector<void *> v_call;
    ast->GetNodesByType(scope, "preorder", "SgFunctionCallExp", &v_call);
    for (auto call : v_call) {
      void *decl = ast->GetFuncDeclByCall(call, 1);
      if (decl != nullptr && all_kernels.count(decl) > 0) {
        // support task with multiple kernels
        void *new_decl = ast->CloneFuncDecl(decl, insert_scope, false);
        ret.push_back(InsertNode(new_decl));
        continue;
      }
      if ((decl != nullptr) &&
          (!ast->IsMemberFunction(decl) || ast->IsStatic(decl) ||
           ast->IsStatic(ast->GetTemplateMemFuncDecl(decl)))) {
        fused_set.insert(decl);
        continue;
      }
      if (decl == nullptr) {
        //  20170621 Automatically add headers according to system
        //  function call. Currently, we add the headers by force.
        decl = ast->GetFuncDeclByCall(call, 0);
      }
      string header_name = ast->get_file(decl);
      if (check_internal_system_header_file(ast, decl) ||
          header_name == "merlin_stream.h") {
        continue;
      }
      if (check_system_header_file(ast, decl, false)) {
        continue;
      }
      if ((ast->IsCompilerGenerated(decl) ||
           header_name == "compilerGenerated") &&
          (ast->IsTemplateInstantiationFunctionDecl(decl) != 0)) {
        string func_name = ast->GetFuncName(decl, false);
        void *sg_template = ast->GetTemplateFuncDecl(decl);
        if (sg_template != nullptr) {
          string file_name = ast->get_file(sg_template);
          cout << "template function \'" << func_name << "\' in file "
               << file_name << endl;
          cout << ast->_up(sg_template, 100) << endl;
          cout << ast->get_template_string(sg_template) << endl;
        }
        string func_info =
            "\'" + func_name + "\' " + getUserCodeFileLocation(ast, call, true);
        string msg = "Cannot support template function: \n " + func_info;
        dump_critical_message(SEPAR_ERROR_4(func_info), 0);
#ifdef PROJDEBUG
        cout << "ERROR: " << msg << endl;
#endif
        m_error++;
      }
      if (ast->IsMemberFunction(decl)) {
        if (ast->IsAssignOperator(decl) || ast->IsConstructor(decl)) {
          // skip default assign operator or construtor
          continue;
        }
        void *class_decl = ast->GetTypeDeclByMemberFunction(decl);
        if (class_decl != nullptr) {
          string type_str = ast->GetTypeName(class_decl, false);
          if (skip_class(type_str) &&
              check_internal_system_header_file(ast, class_decl)) {
            continue;
          }
        }
        void *tmf = ast->GetTemplateMemFuncDecl(decl);
        if (tmf != nullptr) {
          decl = tmf;
        }
        if (!pmemo->has(decl)) {
          pmemo->add(decl, decl);
          check_decl_scope(ast, decl, insert_scope, visited, visited_global,
                           pmemo, s_clone, cross_function, &ret, scope,
                           all_kernels);
        }
        continue;
      }
      void *new_decl = ast->CloneFuncDecl(decl, insert_scope, false);
      pmemo->add(decl, new_decl);
      ret.push_back(InsertNode(new_decl));
    }
  }

  // 1.2.1 collect all the used variables
  SetVector<void *> s_name;
  {
    vector<void *> v_name;
    ast->GetNodesByType(scope, "preorder", "SgInitializedName", &v_name);
    for (auto name : v_name) {
      s_name.insert(name);
    }

    vector<void *> v_ref;
    ast->GetNodesByType(scope, "preorder", "SgVarRefExp", &v_ref);
    for (auto ref : v_ref) {
      void *var = ast->GetVariableInitializedName(ref);
      s_name.insert(var);
    }

    //  Fix bug2312
    v_ref.clear();
    ast->get_floating_var_refs(scope, &v_ref);
    for (auto ref : v_ref) {
      void *name = ast->GetVariableInitializedName(ref);
      s_name.insert(name);
    }
  }
  for (auto name : s_name) {
    void *type = ast->GetTypebyVar(name);
    void *basic_type = ast->GetBaseType(type, true);
    // TODO(youxiang): 20170701 detect function pointer and error out
    if (ast->IsFunctionType(basic_type) != 0) {
      string func_pointer_name = ast->GetVariableName(name);
      string func_pointer_info = "\'" + func_pointer_name + "\' " +
                                 getUserCodeFileLocation(ast, name, true);
      string msg = "Cannot support function pointer: \n " + func_pointer_info;
      dump_critical_message(SEPAR_ERROR_3(func_pointer_info), 0);
#ifdef PROJDEBUG
      cout << "ERROR: " << msg << endl;
#endif
      m_error++;
    }
  }

  //  1.2.2 class/struct/enum/union/typedef
  SetVector<void *> s_base_type_set;
  SetVector<void *> s_type;
  if (ast->IsFunctionDeclaration(scope) != 0) {
    void *return_type = ast->GetFuncReturnType(scope);
    void *basic_type = nullptr;
    vector<size_t> nSizes;
    ast->get_type_dimension(return_type, &basic_type, &nSizes, scope);
    s_base_type_set.insert(basic_type);
  }

  vector<void *> s_exp;
  ast->GetNodesByType_int_compatible(scope, V_SgExpression, &s_exp);
  for (auto exp : s_exp) {
    void *exp_type = nullptr;
    if (SgSizeOfOp *sg_sizeof_op = isSgSizeOfOp(static_cast<SgNode *>(exp))) {
      void *sg_type = sg_sizeof_op->get_operand_type();
      if (sg_type != nullptr) {
        exp_type = sg_type;
      }
    } else if ((ast->IsCastExp(exp) != 0) || (ast->IsEnumVal(exp) != 0)) {
      exp_type = ast->GetTypeByExp(exp);
    }

    if (exp_type == nullptr) {
      continue;
    }
    s_type.insert(exp_type);
    void *basic_type = nullptr;
    vector<size_t> nSizes;
    ast->get_type_dimension(exp_type, &basic_type, &nSizes, exp);
    s_base_type_set.insert(basic_type);
    if (ast->IsEnumVal(exp) != 0) {
      basic_type = ast->GetTypeByEnumVal(exp);
      if (basic_type != nullptr) {
        s_base_type_set.insert(basic_type);
        s_type.insert(basic_type);
      }
    }
  }
  for (auto name : s_name) {
    void *type = ast->GetTypebyVar(name);
    s_type.insert(type);
    void *basic_type = nullptr;
    vector<size_t> nSizes;
    ast->get_type_dimension(type, &basic_type, &nSizes, name);
    s_base_type_set.insert(basic_type);
  }
  for (auto basic_type : s_base_type_set) {
    void *curr_type = basic_type;
    while (curr_type != nullptr) {
      if ((ast->IsTypedefType(curr_type) != 0) ||
          (ast->IsEnumType(curr_type) != 0) ||
          isSgClassType(static_cast<SgNode *>(curr_type)) != nullptr) {
        fused_set.insert(curr_type);
      }
      curr_type = ast->GetBaseTypeOneLayer(curr_type);
    }
  }
  for (auto type : s_type) {
    void *curr_type = type;
    while (curr_type != nullptr) {
      if (ast->IsTypedefType(curr_type) != 0) {
        fused_set.insert(curr_type);
      }
      curr_type = ast->GetBaseTypeOneLayer(curr_type);
    }
  }

  // ZP: add missing types MER-1524 etc.
  {
    vector<void *> v_name;
    ast->GetNodesByType(scope, "preorder", "SgClassType", &v_name);
    for (auto name : v_name) {
      fused_set.insert(name);
    }
  }

  //  1.2.3 collect all variables references for global analysis
  vector<void *> v_ref;
  ast->GetNodesByType(scope, "preorder", "SgVarRefExp", &v_ref);
  for (auto ref : v_ref) {
    fused_set.insert(ref);
  }
  v_ref.clear();
  ast->get_floating_var_refs(scope, &v_ref);
  for (auto ref : v_ref) {
    fused_set.insert(ref);
  }

  //  1.2.4 inner type declaration
  {
    vector<void *> vec_decl;
    ast->GetNodesByType_int(scope, "preorder", V_SgTypedefDeclaration,
                            &vec_decl);
    fused_set.insert(vec_decl.begin(), vec_decl.end());
    ast->GetNodesByType_int_compatible(scope, V_SgClassDeclaration, &vec_decl);
    fused_set.insert(vec_decl.begin(), vec_decl.end());
    ast->GetNodesByType_int(scope, "preorder", V_SgEnumDeclaration, &vec_decl);
    fused_set.insert(vec_decl.begin(), vec_decl.end());
  }
  // 1.2.5 standalone type
  {
    vector<void *> vec_type;
    ast->GetNodesByType_int(scope, "preorder", V_SgTypedefType, &vec_type);
    fused_set.insert(vec_type.begin(), vec_type.end());
    ast->GetNodesByType_int(scope, "preorder", V_SgClassType, &vec_type);
    fused_set.insert(vec_type.begin(), vec_type.end());
    ast->GetNodesByType_int(scope, "preorder", V_SgEnumType, &vec_type);
    fused_set.insert(vec_type.begin(), vec_type.end());
  }
  //  1.3 handle floating expression in the scope
  {
    vector<void *> vec_exprs;
    ast->get_floating_exprs(scope, &vec_exprs);
    for (auto expr : vec_exprs) {
      auto sub_res = copy_decl_in_scope_between_files(
          expr, insert_scope, ast, visited, visited_global, pmemo, s_clone,
          cross_function, all_kernels);
      ret.insert(ret.end(), sub_res.begin(), sub_res.end());
    }
  }

  // 1.4 collect variable used in pragmas
  {
    auto used_variables_in_pragma =
        ast->GetVariablesUsedByPragmaInScope(scope, false);
    fused_set.insert(used_variables_in_pragma.begin(),
                     used_variables_in_pragma.end());
  }
  //  1.4
  //  i) store the decls (including sub functions, struct, class, typedef) in a
  //  nested order ii)After the recur is done: insert the nested decls into the
  //  new kernel function
  //  Peng: will unordered set have problem???
  //  'fused_set' includes sub function decls, global variable initialized name,
  //  type (not declaration) which are used in the current scope
  for (auto one_type : fused_set) {
    if (cross_function && (ast->IsFunctionDeclaration(one_type) != 0)) {
      if (one_type != nullptr) {
        if (pmemo->has_processed(one_type)) {
          continue;
        }

        ast->replace_floating_var_refs(one_type);
        ast->replace_sizeof(one_type);
        //  copy the function decl
        void *new_decl =
            clone_function(ast, one_type, insert_scope, s_clone, pmemo);

        void *decl_body = ast->GetFuncBody(one_type);

        clean_func_body_before_copy(ast, decl_body);

        void *new_body = ast->CopyStmt(decl_body);
        clean_func_body_after_copy(ast, new_body);
        for (int i = 0; i < ast->GetFuncParamNum(one_type); ++i) {
          ast->replace_variable_references_in_scope(
              ast->GetFuncParam(one_type, i), ast->GetFuncParam(new_decl, i),
              new_body);
        }
        ast->SetFuncBody(new_decl, new_body);

        //  change the function type as static
        ast->SetStatic(new_decl);

        //  recur the function
        auto one_type_insert = copy_decl_in_scope_between_files(
            one_type, insert_scope, ast, visited, visited_global, pmemo,
            s_clone, cross_function, all_kernels);
        ret.insert(ret.end(), begin(one_type_insert), end(one_type_insert));

        pmemo->add(one_type, new_decl);

        void *kernel_pragma = ast->GetPreviousStmt(one_type, false);
        void *new_pragma = nullptr;
        if (ast->IsPragmaDecl(kernel_pragma) != 0) {
          //  keep all pragmas makes preprocess fail
          //  #pragma ACCEL kernel die=SLR1
          //  like above in multi kernel
          //  invalidates build_mars_ir
          //  so only keep inline pragma for now
          CMarsIrV2 mars_ir(ast);
          if (mars_ir.get_pragma_attribute(kernel_pragma, "inline") ==
              "inline") {
            new_pragma = ast->CopyStmt(kernel_pragma);
            ast->DiscardPreprocInfo(new_pragma);
            ret.push_back(InsertNode(new_pragma));
          }
        }

        ret.push_back(InsertNode(new_decl));
      }
    }

    //  if (TYPE_FLAG == STRUCT_CLASS_UNION_ENUM_TYPE)
    void *class_decl = nullptr;
    string type_str;
    if (ast->IsClassDecl(one_type) != 0) {
      class_decl = one_type;
      one_type =
          isSgClassDeclaration(static_cast<SgNode *>(class_decl))->get_type();
      type_str = ast->GetTypeName(class_decl);
    } else if ((ast->IsStructureType(one_type) != 0) ||
               (ast->IsClassType(one_type) != 0) ||
               (ast->IsUnionType(one_type) != 0) ||
               (ast->IsEnumType(one_type) != 0)) {
      type_str = ast->GetTypeNameByType(one_type, false);
      class_decl = ast->GetTypeDeclByType(one_type, 1);
      //  20170526 Check resursive type and error out
      void *rec_type = nullptr;
      if (ast->IsRecursiveType(one_type, &rec_type) != 0) {
        void *check_decl = class_decl;
        if (check_decl == nullptr) {
          check_decl = ast->GetTypeDeclByType(one_type, 0);
        }
        if ((check_decl != nullptr) &&
            check_system_header_file(ast, check_decl, false)) {
          continue;
        }
        string type_name = ast->_up(one_type);
        string type_info = "\'" + type_name + "\' " +
                           getUserCodeFileLocation(ast, rec_type, true);
        string msg = "Cannot support recursive type: \n " + type_info;
        string::size_type idx = type_name.find("merlin_stream");
        if (idx == string::npos) {
          dump_critical_message(SEPAR_ERROR_2(type_info), 0);
#ifdef PROJDEBUG
          cout << "ERROR: " << msg << endl;
#endif
          throw std::exception();
        }
      }
      if (class_decl == nullptr) {
        void *non_floating_node = one_type;
        if (INLINE_TYPE it =
                is_inlined_type(ast, one_type, &non_floating_node)) {
          if (INLINED_CLASS != it) {
            reportInlinedType(ast, non_floating_node,
                              ast->GetTypeDeclByType(one_type, 0));
            continue;
          }
        }

        void *class_decl_without_def = ast->GetTypeDeclByType(one_type, 0);
        if (ast->IsTemplateClassDecl(class_decl_without_def) != 0) {
          class_decl = class_decl_without_def;

        } else if ((class_decl_without_def != nullptr) &&
                   check_decl_scope(ast, class_decl_without_def, insert_scope,
                                    visited, visited_global, pmemo, s_clone,
                                    cross_function, &ret, scope, all_kernels)) {
          continue;
        }
      }
      if (class_decl == nullptr) {
        reportUndefinedTypeDecl(ast, one_type);
        m_error++;
        continue;
      }
    } else if (ast->IsComplexType(one_type)) {
      header_list.insert("complex.h");
    }
    if (class_decl != nullptr) {
      if (pmemo->has_processed(class_decl) ||
          pmemo->has_processed(ast->GetParent(class_decl))) {
        continue;
      }
      if (auto *tc = isSgTemplateInstantiationDecl(
              static_cast<SgNode *>(ast->GetDirectTypeDeclByType(one_type)))) {
        for (SgTemplateArgument *targ : tc->get_templateArguments()) {
          SgType *type = targ->get_type();
          if (type != nullptr) {
            auto basic_type_insert = copy_decl_in_scope_between_files(
                type, insert_scope, ast, visited, visited_global, pmemo,
                s_clone, cross_function, all_kernels);
            ret.insert(ret.end(), begin(basic_type_insert),
                       end(basic_type_insert));
          }
          SgExpression *exp = targ->get_expression();
          if (exp != nullptr) {
            auto basic_type_insert = copy_decl_in_scope_between_files(
                exp, insert_scope, ast, visited, visited_global, pmemo, s_clone,
                cross_function, all_kernels);
            ret.insert(ret.end(), begin(basic_type_insert),
                       end(basic_type_insert));
          }
        }
      }
      if (check_internal_system_header_file(ast, class_decl)) {
        //  20170619 Check <ap_int.h>
        if (boost::regex_match(type_str, boost::regex("ap_int <.*> ")) ||
            boost::regex_match(type_str, boost::regex("ap_uint <.*> ")) ||
            boost::regex_match(type_str, boost::regex("ap_fixed <.*> ")) ||
            boost::regex_match(type_str, boost::regex("ap_ufixed <.*> "))) {
          header_list.insert("ap_int.h");
          continue;
        }

        if (boost::regex_match(type_str, boost::regex("Mat <.*> "))) {
          header_list.insert("xf_structs.h");
          continue;
        }
        if (boost::regex_match(type_str, boost::regex("stream <.*> "))) {
          header_list.insert("hls_stream.h");
          continue;
        }
        if (boost::regex_match(type_str, boost::regex("ap_shift_reg <.*> "))) {
          header_list.insert("ap_shift_reg.h");
          continue;
        }
      }
      if (boost::regex_match(type_str, boost::regex("complex <.*> "))) {
        header_list.insert("complex");
        continue;
      }
      //  recur the function
      if (ast->GetClassDefinition(class_decl) != nullptr) {
        auto class_insert = copy_decl_in_scope_between_files(
            class_decl, insert_scope, ast, visited, visited_global, pmemo,
            s_clone, cross_function, all_kernels);
        ret.insert(ret.end(), begin(class_insert), end(class_insert));
      }

      if (check_internal_system_header_file(ast, class_decl)) {
        continue;
      }

      if (check_system_header_file(ast, class_decl, false)) {
        reportSystemStruct(ast, one_type, class_decl);
        continue;
      }

      //  20170621 Check typedef struct{...}st;
      void *parent_decl = ast->GetParent(class_decl);
      // Judge if the base type of the typedef is not a pointer to struct by
      // expecting the base type and "one_type" to be the same.
      if (ast->IsTypedefDecl(parent_decl) != 0 &&
          SageInterface::checkTypesAreEqual(
              static_cast<SgTypedefDeclaration *>(parent_decl)->get_base_type(),
              static_cast<SgType *>(one_type))) {
        get_immediate_typedef_decl(ast, &parent_decl);
        pmemo->add(parent_decl, parent_decl);
        check_decl_scope(ast, parent_decl, insert_scope, visited,
                         visited_global, pmemo, s_clone, cross_function, &ret,
                         scope, all_kernels);
      } else {
        pmemo->add(class_decl, class_decl);
        if (check_decl_scope(ast, class_decl, insert_scope, visited,
                             visited_global, pmemo, s_clone, cross_function,
                             &ret, scope, all_kernels)) {
          continue;
        }
        // Merlin not supporting the typedef for pointer to an anonymous struct
        if (ast->IsAnonymousTypeDecl(class_decl) != 0) {
          reportAnoymousTypeDecl(ast, class_decl);
          m_error++;
          continue;
        }
      }
    }
    //  if (TYPE_FLAG == TYPEDEF_TYPE)
    void *typedef_decl = nullptr;
    if (ast->IsTypedefDecl(one_type) != 0) {
      typedef_decl = one_type;
      one_type =
          isSgTypedefDeclaration(static_cast<SgNode *>(one_type))->get_type();
    } else if (ast->IsTypedefType(one_type) != 0) {
      typedef_decl = ast->GetTypeDefDecl(one_type);
    }
    if (typedef_decl != nullptr) {
      if (pmemo->has_processed(typedef_decl)) {
        continue;
      }
      if (check_internal_system_header_file(ast, typedef_decl)) {
        continue;
      }

      if (check_system_header_file(ast, typedef_decl, false)) {
        continue;
      }

      void *base_type = ast->GetBaseTypeOneLayer(one_type);
      auto basetype_insert = copy_decl_in_scope_between_files(
          base_type, insert_scope, ast, visited, visited_global, pmemo, s_clone,
          cross_function, all_kernels);
      ret.insert(ret.end(), begin(basetype_insert), end(basetype_insert));
      pmemo->add(typedef_decl, nullptr);
      check_decl_scope(ast, typedef_decl, insert_scope, visited, visited_global,
                       pmemo, s_clone, cross_function, &ret, scope,
                       all_kernels);
    }

    //  /////////////////////////////////////  /
    //  2. handling all the global variables
    //  /////////////////////////////////////  /

    //  if (TYPE_FLAG == GLOBAL_VAR)
    void *name = nullptr;
    if (ast->IsVarRefExp(one_type) != 0) {
      name = ast->GetVariableInitializedName(one_type);
    } else if (ast->IsInitName(one_type)) {
      name = one_type;
    }
    if (ast->IsInitName(name)) {
      if (ast->IsGlobalInitName(name) == 0) {
        continue;
      }
      void *decl = ast->GetVariableDecl(name);
      if (check_system_header_file(ast, decl, false)) {
        continue;
      }
      name = ast->GetVariableDefinitionByName(name);
      decl = ast->GetVariableDecl(name);

      if (check_decl_scope(ast, decl, insert_scope, visited, visited_global,
                           pmemo, s_clone, cross_function, &ret, scope,
                           all_kernels)) {
        continue;
      }
      if (ast->IsVariableDecl(decl) == 0) {
        continue;
      }
      ast->replace_floating_var_refs(decl);
      ast->replace_sizeof(decl);
      if (((ast->IsFromInputFile(decl) != 0) || ast->IsTransformation(decl))) {
        if (pmemo->has_processed(name)) {
          if (visited_global->count(name) > 0) {
            // found recursive dependency in global initialization, such as
            // int a = b; int b = a;
            string var_name = ast->GetVariableName(name);
            cout << "find recursive global definition: " << var_name << endl;
            string global_info = "\'" + var_name + "\' " +
                                 getUserCodeFileLocation(ast, name, true);
            string msg = "Cannot support recursive global: \n " + global_info;
            dump_critical_message(SEPAR_ERROR_9(global_info), 0);
#ifdef PROJDEBUG
            cout << "ERROR: " << msg << endl;
#endif
            throw std::exception();
          }
          continue;
        }

        visited_global->insert(name);

        void *type = ast->GetTypebyVar(name);
        string s_var = ast->_up(name);
        void *var_init = ast->GetInitializer(name);
        // get the missing leftmost dimension from initializer
        if (ast->IsArrayType(type) && var_init != nullptr) {
          void *init_type = ast->GetTypeByExp(var_init);
          if (ast->IsArrayType(init_type) != 0) {
            type = init_type;
          }
        }
        if (ast->IsStatic(ast->GetVariableDecl(name))) {
          unordered_set<string> idents;
          collect_shadowing_identifier(ast, name, &idents);
          s_var = pmemo->make_unique(s_var, idents);
          pmemo->add(s_var);
          //  youxiang 20180320
          if ((ast->IsConstType(type) == 0) &&
              (ast->has_write_in_scope_fast(name, nullptr) == 0) &&
              var_init != nullptr) {
            type = ast->CreateConstType(type, name);
          }
        }
        if (ast->IsExtern(decl) && ast->IsConstType(type) &&
            var_init == nullptr) {
          // MER-1678 check non-defined constant global varibles
          string global_info = "\'" + ast->_up(decl) + "\' " +
                               getUserCodeFileLocation(ast, decl, true) + "\n";
          string msg = "Non initialized constant " + global_info;
          dump_critical_message(GLOBL_ERROR_5(global_info), 0);
          m_error++;
        }
        void *new_decl = ast->CreateVariableDecl(
            type, s_var,
            ast->IsExtern(decl) && !ast->IsConstType(type)
                ? nullptr
                : (var_init != nullptr ? ast->CopyExp(var_init) : nullptr),
            insert_scope, name);
        if (ast->IsExtern(decl) && !ast->IsConstType(type)) {
          ast->SetExtern(new_decl);
        }
        if (ast->IsStatic(decl) || ast->IsConstType(type)) {
          ast->SetStatic(new_decl);
        }

        pmemo->add(name, new_decl);
        auto var_insert = copy_decl_in_scope_between_files(
            decl, insert_scope, ast, visited, visited_global, pmemo, s_clone,
            cross_function, all_kernels);
        ret.insert(ret.end(), begin(var_insert), end(var_insert));
        visited_global->erase(name);
        void *pragma_decl =
            ast->GetPreviousStmt(ast->GetVariableDecl(one_type), false);
        if (ast->IsPragmaDecl(pragma_decl) != 0) {
          void *copy_pragma_decl = ast->CopyStmt(pragma_decl);
          ast->DiscardPreprocInfo(copy_pragma_decl);
          ret.push_back(InsertNode(copy_pragma_decl));
          ret.push_back(InsertNode(new_decl));
        } else {
          ret.push_back(InsertNode(new_decl));
        }
      }
    }
  }

  visited->erase(scope);
  if (ast->IsClassDecl(scope) != 0) {
    if (auto *tc_cls = ast->GetTemplateClassDecl(scope)) {
      visited->erase(tc_cls);
    }
  }

  if (void *parent_decl = ast->GetParent(scope)) {
    if (ast->IsTypedefDecl(parent_decl) != 0) {
      visited->erase(parent_decl);
    }
  }
  return ret;
}

//  only handle the 1-d array: because of MarsIR_v2::get_array_dimension()
void update_interface_pragma_from_host(void *kernel, CSageCodeGen *ast,
                                       CMarsIrV2 *mars_ir) {
  for (int idx = 0; idx < ast->GetFuncParamNum(kernel); idx++) {
    void *arg = ast->GetFuncParam(kernel, idx);
    string s_arg = ast->_up(arg);

    int cont_flag;
    vector<size_t> vn_depth = mars_ir->get_array_dimension(arg, &cont_flag);

    if (vn_depth.empty()) {
      continue;
    }

    string s_depth;
    for (auto n_depth : vn_depth) {
      if (0 == n_depth) {
        s_depth += ",";
      } else {
        s_depth += my_itoa(n_depth) + ",";
      }
    }
    //  remove the last ","
    s_depth = s_depth.substr(0, s_depth.length() - 1);
    if (s_depth.empty()) {
      continue;
    }
    void *pragma = mars_ir->get_pragma_by_variable(s_arg, kernel, "interface");
    if (pragma != nullptr) {
      if (mars_ir->get_pragma_attribute(pragma, "depth").empty()) {
        mars_ir->set_pragma_attribute(&pragma, "depth", s_depth);
      }
    } else {
      string s_pragma =
          "ACCEL interface variable=" + s_arg + " depth=" + s_depth;
      void *scope = ast->GetFuncBody(kernel);
      pragma = ast->CreatePragma(s_pragma, scope);
      ast->PrependChild(scope, pragma);
    }
  }
}

//  return=1 means use new user interface, task pragma follow by function decl
//  return=0 means use old user interface, task pragma follow by basic block
//  check task pragma legality
//  the basic block or function body can only have more than kernel call,
//  do ot support other statements
int check_task_pragma_legality(CSageCodeGen *codegen, void *pTopFunc,
                               const CInputOptions &options) {
  cout << "Enter check task pragma legality." << endl;
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  vector<void *> v_pragma;

  codegen->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
                          &v_pragma);

  int ret = 0;
  int valid_flag = 0;
  int return_value = 0;
  for (auto pragma : v_pragma) {
    string Cmd = mars_ir.get_pragma_attribute(pragma, "task");
    boost::algorithm::to_lower(Cmd);
    if (Cmd != "task") {
      continue;
    }
    void *next_stmt = codegen->GetNextStmt(pragma);
    //  task pragma must followed by basic block or function declaration
    void *scope = nullptr;
    if (codegen->IsFunctionDeclaration(next_stmt) != 0) {
      scope = codegen->GetFuncBody(next_stmt);
    } else {
      string msg = "Task pragma must followed by function declaration:\n" + Cmd;
      dump_critical_message(SEPAR_ERROR_12(msg), 0);
      throw std::exception();
    }
    //    check_global_and_static(codegen, next_stmt);
    //  task pragma scope can only contain kernel declaratio
    if (scope != nullptr) {
      int num = codegen->GetChildStmtNum(scope);
      int kernel_number = 0;
      if (num > 1) {
        vector<void *> child_stmts = codegen->GetChildStmts(scope);
        vector<void *> visited_kernel;
        for (auto one_stmt : child_stmts) {
          void *one_call = codegen->GetFuncCallInStmt(one_stmt);
          if (one_call != nullptr) {
            void *one_decl = codegen->GetFuncDeclByCall(one_call);
            if (std::find(kernels.begin(), kernels.end(), one_decl) !=
                kernels.end()) {
              cout << "Find valid kernel: "
                   << codegen->UnparseToString(one_stmt) << endl;
              if (std::find(visited_kernel.begin(), visited_kernel.end(),
                            one_decl) != visited_kernel.end()) {
                string msg = "Do not support same kernel called multiple times "
                             "in one task:\n" +
                             codegen->UnparseToString(one_decl);
                dump_critical_message(SEPAR_ERROR_15(msg), 0);
                throw std::exception();
              }
              kernel_number++;
              visited_kernel.push_back(one_decl);

              for (int j = 0; j < codegen->GetFuncCallParamNum(one_call); ++j) {
                void *param_kernel_call =
                    codegen->GetFuncCallParam(one_call, j);
                cout << "param = "
                     << codegen->UnparseToString(param_kernel_call) << endl;
                if (codegen->IsVarRefExp(param_kernel_call) == 0) {
                  string var_pos =
                      getUserCodeFileLocation(codegen, param_kernel_call, true);
                  string msg = "Only support variable reference expression in "
                               "task kernel call\n " +
                               codegen->UnparseToString(param_kernel_call) +
                               "" + var_pos;
                  dump_critical_message(SEPAR_ERROR_16(msg), 0);
                }
              }
            } else {
              ret = 1;
              string msg = "Task pragma scope can only filled by kernel call, "
                           "not support: \n" +
                           codegen->UnparseToString(one_stmt);
              dump_critical_message(SEPAR_ERROR_13(msg), 0);
            }
          } else {
            ret = 1;
            string msg = "Task pragma scope can only filled by kernel call, "
                         "not support: \n" +
                         codegen->UnparseToString(one_stmt);
            dump_critical_message(SEPAR_ERROR_13(msg), 0);
          }
        }
      } else {
        //  scope must have more than 1 child
        string msg =
            "Task pragma scope should filled by more than one kernel call\n" +
            codegen->UnparseToString(pragma);
        dump_critical_message(SEPAR_ERROR_14(msg), 0);
        throw std::exception();
      }
      if (kernel_number <= 1) {
        string msg =
            "Task pragma scope should filled by more than one kernel call\n";
        dump_critical_message(SEPAR_ERROR_14(msg), 0);
        throw std::exception();
      }
      valid_flag = 1;

      if (valid_flag == 1) {
        return_value = 1;
      }
    } else {
      //  scope must be valid
      string msg = "Task pragma scope can only filled by kernel call, not "
                   "support empty scope\n";
      dump_critical_message(SEPAR_ERROR_13(msg), 0);
      throw std::exception();
    }
  }
  if (ret == 1) {
    throw std::exception();
  }
  return return_value;
}

int remove_static_assert(CSageCodeGen *cg, void *pTopFunc,
                         const CInputOptions &options) {
  vector<void *> vec_static_assert;
  cg->GetNodesByType(pTopFunc, "preorder", "SgStaticAssertionDeclaration",
                     &vec_static_assert);
  for (auto x : vec_static_assert) {
    if (isSgStaticAssertionDeclaration(reinterpret_cast<SgNode *>(x))) {
      void *null_expr_stmt =
          cg->CreateStmt(V_SgExprStatement, cg->CreateExp(V_SgNullExpression));
      cg->ReplaceStmt(x, null_expr_stmt);
    }
  }

  return 0;
}

int transfer_task_to_kernel(CSageCodeGen *codegen, void *pTopFunc,
                            const CInputOptions &options,
                            vector<string> *vec_tasks) {
  int task_pass = check_task_pragma_legality(codegen, pTopFunc, options);
  if (task_pass == 0) {
    return 0;
  }
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  auto kernels = mars_ir.get_top_kernels();
  CSageCodeGen *ast = codegen;
  map<string, map<string, string>> task_kernel_file;
  map<string, map<string, string>> task_attribute;
  vector<void *> v_pragma;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
                          &v_pragma);
  for (auto pragma : v_pragma) {
    string cmd = mars_ir.get_pragma_attribute(pragma, "task");
    boost::algorithm::to_lower(cmd);
    if (cmd != "task") {
      continue;
    }
    cout << "Find one task pragma : " << ast->UnparseToString(pragma) << endl;
    string attribute;
    cmd = "";
    cmd = mars_ir.get_pragma_attribute(pragma, "parallel");
    if (cmd == "__merlin_parallel") {
      attribute = "__merlin_parallel";
    }
    //    cmd = mars_ir.get_pragma_attribute(pragma, "pipeline");
    //    if(cmd == "__merlin_pipeline") {
    //        attribute = "__merlin_pipeline";
    //    }
    cout << "task attribute = " << attribute << endl;
    map<string, string> attribute_empty;
    attribute_empty.insert(pair<string, string>(attribute, ""));

    void *next_stmt = codegen->GetNextStmt(pragma);
    if (codegen->IsFunctionDeclaration(next_stmt) != 0) {
      void *func_decl = next_stmt;
      void *func_body = codegen->GetFuncBody(next_stmt);
      string task_name = ast->GetFuncName(func_decl);
      vec_tasks->push_back(task_name);
      //  extract all kernel declaration
      vector<void *> kernel_call_v;
      map<string, string> kernel_file;
      for (auto stmt : ast->GetChildStmts(func_body)) {
        void *expr = ast->GetExprFromStmt(stmt);
        if (ast->IsFunctionCall(expr) == 0) {
          string msg;
          msg += "Do not support following function call in task scope:\n\t" +
                 ast->UnparseToString(stmt) +
                 getUserCodeFileLocation(ast, stmt, true) + "\n";
          msg += "\tCan only support function call format:\n"
                 "\t\tfunc();\n\tin task scope.\n ";
          dump_critical_message(SEPAR_ERROR_19(msg), 0);
          throw std::exception();
        }
        void *decl = ast->GetFuncDeclByCall(expr);
        cout << "Insert kernel declaration... " << ast->UnparseToString(decl)
             << endl;
        kernel_file.insert(
            pair<string, string>(ast->GetFuncName(decl), ast->get_file(decl)));
        kernel_call_v.push_back(expr);
      }

      //  extract kernel pragma to task pragma
      //  then interface transforma will generate opencl memory burst
      for (int i = 0; i < ast->GetFuncParamNum(func_decl); ++i) {
        void *param_task = ast->GetFuncParam(func_decl, i);
        string param_s = ast->UnparseToString(param_task);
        cout << "param_task = " << ast->UnparseToString(param_task) << endl;
        int depth = 1;
        int max_depth = 1;
        bool has_interface_pragma = false;
        string bank;
        string bank_final;
        vector<string> bank_v;
        vector<void *> bank_pragma_v;
        for (auto one_call : kernel_call_v) {
          for (int j = 0; j < ast->GetFuncCallParamNum(one_call); ++j) {
            void *param_kernel_call = ast->GetFuncCallParam(one_call, j);
            void *var_ref = nullptr;
            ast->parse_array_ref_from_pntr(param_kernel_call, &var_ref);
            if (var_ref != nullptr) {
              void *sg_name = ast->GetVariableInitializedName(var_ref);
              if (sg_name == param_task) {
                cout << "param_kernel_call = " << ast->UnparseToString(sg_name)
                     << endl;
                void *kernel = ast->GetFuncDeclByCall(one_call);
                void *param_kernel_decl = ast->GetFuncParam(kernel, j);
                string param_s = ast->UnparseToString(param_kernel_decl);
                for (auto pragma : v_pragma) {
                  void *pragma_kernel = ast->TraceUpToFuncDecl(pragma);
                  if (pragma_kernel != kernel) {
                    continue;
                  }
                  string cmd =
                      mars_ir.get_pragma_attribute(pragma, "interface");
                  boost::algorithm::to_lower(cmd);
                  if (cmd != "interface") {
                    continue;
                  }
                  string variable =
                      mars_ir.get_pragma_attribute(pragma, "variable");
                  if (variable == param_s) {
                    cout << "pragma = " << ast->UnparseToString(pragma) << endl;
                    cout << "variable = " << variable << endl;
                    string depth_s =
                        mars_ir.get_pragma_attribute(pragma, "depth");
                    if (depth < atoi(depth_s.c_str())) {
                      depth = atoi(depth_s.c_str());
                    }
                    string max_depth_s =
                        mars_ir.get_pragma_attribute(pragma, "max_depth");
                    if (max_depth < atoi(max_depth_s.c_str())) {
                      max_depth = atoi(max_depth_s.c_str());
                    }
                    bank = mars_ir.get_pragma_attribute(pragma, "bank");
                    if (!bank.empty()) {
                      bank_final = bank;
                      if (std::find(bank_v.begin(), bank_v.end(), bank) ==
                          bank_v.end()) {
                        bank_v.push_back(bank);
                        bank_pragma_v.push_back(pragma);
                      }
                    }
                    has_interface_pragma = true;
                  }
                }
              }
            }
          }
        }
        if (has_interface_pragma) {
          string new_pragma_s = "ACCEL interface variable=" + param_s;
          new_pragma_s += " depth=" + std::to_string(depth);
          if (max_depth > depth) {
            new_pragma_s += " max_depth=" + std::to_string(max_depth);
          }
          if (!bank_final.empty()) {
            new_pragma_s += " bank=" + bank_final;
          }
          void *new_pragma = ast->CreatePragma(new_pragma_s, func_body);
          ast->PrependChild(func_body, new_pragma);
          cout << "new_pragma = " << ast->UnparseToString(new_pragma) << endl;
        }
      }
      string kernel_pragma_s = "ACCEL kernel";
      void *kernel_pragma =
          ast->CreatePragma(kernel_pragma_s, ast->GetGlobal(func_decl));
      ast->ReplaceStmt(pragma, kernel_pragma);
      task_kernel_file.insert(
          pair<string, map<string, string>>(task_name, kernel_file));
      writeInterReport(TASK_KERNEL_JSON, task_kernel_file);
      task_attribute.insert(
          pair<string, map<string, string>>(task_name, attribute_empty));
      writeInterReport(TASK_ATTRIBUTE_JSON, task_attribute);
    }
  }
  return 0;
}

// check whether there are global/static variables shared between mutiple
// kernels
// TODO(youxiang) Peng said we may be able to support???
int check_shared_global_variables_between_kernels(
    CSageCodeGen *ast, const vector<void *> &kernels,
    const vector<string> &vec_tasks) {
  std::multimap<void *, void *> global_and_static_to_kernel;
  for (auto kernel : kernels) {
    set<void *> visit_func;
    auto used_global_static =
        get_used_global_and_static(ast, kernel, &visit_func, true);
    vector<void *> common;
    string kernel_name = ast->GetFuncName(kernel);
    if (std::find(vec_tasks.begin(), vec_tasks.end(), kernel_name) !=
        vec_tasks.end()) {
      continue;
    }
    string kernel_pos = getUserCodeFileLocation(ast, kernel, true);
    for (auto v : used_global_static) {
      //  YX: 20180731 support constant global/static variables
      void *type = ast->GetTypebyVar(v);
      void *basic_type = nullptr;
      vector<size_t> nSizes;
      ast->get_type_dimension(type, &basic_type, &nSizes, v);
      if (ast->IsConstType(basic_type) != 0) {
        continue;
      }
      global_and_static_to_kernel.insert(std::make_pair(v, kernel));
    }
  }

  bool contain_unsupported_shared_global = false;
  for (auto it = global_and_static_to_kernel.begin();
       it != global_and_static_to_kernel.end();) {
    size_t c = global_and_static_to_kernel.count(it->first);
    if (c <= 1) {
      ++it;
      continue;
    }
    void *var = it->first;
    //  check whether the variable is read-only
    int is_write = 0;
    {
      vector<void *> vec_refs;
      ast->get_ref_in_scope(var, nullptr, &vec_refs, true);
      for (auto ref : vec_refs) {
        if (ast->is_write_conservative(ref) != 0) {
          is_write = 1;
          break;
        }
      }
    }
    if (is_write == 0) {
      while (c-- > 0) {
        ++it;
      }
      continue;
    }
    string var_name = ast->GetVariableName(var);
    string var_pos = getUserCodeFileLocation(ast, var, true);
    void *kernel = it->second;
    string kernel_name = ast->GetFuncName(kernel);
    string kernel_pos = getUserCodeFileLocation(ast, kernel, true);
    string msg = "'" + kernel_name + "' " + kernel_pos + " ";
    ++it;
    --c;
    while (c-- > 0) {
      void *kernel = it->second;
      string kernel_name = ast->GetFuncName(kernel);
      string kernel_pos = getUserCodeFileLocation(ast, kernel, true);
      msg += ", '" + kernel_name + "' " + kernel_pos + " ";
      ++it;
    }
    void *var_type = ast->GetTypebyVar(var);
    if (ast->IsArrayType(var_type) != 0) {
      var_type = ast->GetBaseType(var_type, true);
    }
    //  Yuxin: Sep 21
    //  skip merlin streaming channel type
    if (ast->GetStringByType(var_type).find("merlin_stream") != string::npos) {
      continue;
    }
    dump_critical_message(SEPAR_ERROR_11(var_name, var_pos, msg));
    contain_unsupported_shared_global = true;
  }

  if (contain_unsupported_shared_global) {
    throw std::exception();
  }
  return contain_unsupported_shared_global;
}

// Move the kernel pragma from forward decl to defining decl, except that:
// 1. No kernel pragma on the forward decl
// 2. the definiting decl is not found
// 3. the definition decl has kernel pragma already
// Note: if moved, the original pragma will be deleted
int move_kernel_pragma(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options) {
  CMarsAST_IF *ast = codegen;
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  for (auto k : kernels) {
    void *kernel_pragma = ast->GetPreviousStmt(k, false);

    if (ast->GetFuncBody(k) == nullptr) {
      continue;  // kernel is from mars_ir parsing, it has body for sure
    } else if (kernel_pragma && ast->IsPragmaDecl(kernel_pragma) &&
               "kernel" == mars_ir.get_pragma_attribute(
                               kernel_pragma,
                               "kernel")) {  // it already has kernel pragma
      continue;
    }

    // 1. find the kernel pragma in the forward decl
    kernel_pragma = nullptr;
    {
      vector<void *> v_decls = ast->GetAllFuncDecl(k);
      for (auto one_decl : v_decls) {
        if (one_decl == k || ast->GetFuncBody(one_decl) != nullptr) {
          continue;
        }

        void *pre_stmt = ast->GetPreviousStmt(one_decl, false);
        if (pre_stmt && ast->IsPragmaDecl(pre_stmt) &&
            "kernel" == mars_ir.get_pragma_attribute(pre_stmt, "kernel")) {
          kernel_pragma = pre_stmt;
        }
      }
    }

    // 2. move the pragma
    if (kernel_pragma) {
      void *new_pragma = nullptr;
      new_pragma = ast->CopyStmt(kernel_pragma);
      ast->DiscardPreprocInfo(new_pragma);
      ast->InsertStmt(new_pragma, k);
      ast->RemoveStmt(kernel_pragma);
    }
  }

  return 1;
}

//  ///////////////////////////////////////////////  /
//  Note:
//  For each kernel, create a new cpp file which contains all the subfunctions
//  required, without include user header files but system header file and new
//  header file (merlin_typedef.h) with user defined type(s)
//
//
//
//  Process:
//  1. For each kernel top:
//  1.1  copy the kernel function to new file
//  1.2  copy all sub functions and mark them with static and replace their
//  calls
//  1.3  copy used global variables to new file
//  1.4  unparse type declarations (structure, typedef, etc. ) with namespace
//  info
//  1.5  write out file list
//  (kernel_file.list)
int kernel_separate_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  // ZP: remove Static Assert due to MER-1564
  remove_static_assert(codegen, pTopFunc, options);

  // ZP: Move kernel pragma from forward decl to defining decl (MER-1879)
  move_kernel_pragma(codegen, pTopFunc, options);

  codegen->splitInlinedAggregatedDefinition(pTopFunc);
  vector<string> vec_tasks;
  transfer_task_to_kernel(codegen, pTopFunc, options, &vec_tasks);
  // although typedef may be kept, we tried to replace all the typedef for two
  // reasons 1) for typedef from system header files, we may include incorrect
  // header files 2) some user defined typedef name may conflict with OpenCL
  // reserved key words, such as int16
  typedef_spread_top(codegen, pTopFunc, options, false, false, nullptr, false);

  check_stream_model(codegen, pTopFunc, options);

  // ZP: MER-1532
  simplify_constant_val(codegen, pTopFunc);
  Memo::collect_extern_identifiers(codegen, pTopFunc);

  printf("Kernel Seperate ... \n");
  string cmd = "rm -rf " + string(KERNEL_FILE_LIST);
  cout << cmd << endl;
  system(cmd.c_str());

  m_error = 0;
  set<void *> visited_global;
  set<void *> visited;
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  assert(!kernels.empty() && "no kernel found");
  CSageCodeGen *ast = codegen;
  {
    int static_kernels = 0;
    for (auto ker : kernels) {
      if (codegen->IsStatic(ker)) {
        static_kernels += 1;
        string msg = codegen->_up(ker) + " " +
                     getUserCodeFileLocation(codegen, ker, true);
        dump_critical_message(SYNCHK_ERROR_35(msg));
      }
    }
    if (static_kernels > 0) {
      throw std::exception();
    }
  }
  // we shall not infer depth from host
  // for (auto kernel : kernels) {
  //   update_interface_pragma_from_host(kernel, ast, &mars_ir);
  // }

  if (check_shared_global_variables_between_kernels(ast, kernels, vec_tasks)) {
    return 0;
  }

  set<string> set_kernel_files;
  set<string> set_host_files;

  int kernel_count = 0;

  //  0. 20170612 Create __merlin_kernel_list.h
  printf("    Enter kernel_file_list_gen...\n");
  string kernel_list_file = "__merlin_kernel_list.h";
  {
    string cmd = "rm -rf " + kernel_list_file;
    system(cmd.c_str());
  }
  vector<string> vec_strings;
  vector<string> vec_type_strings;
  //  avoid redeclaration of typedef struct
  set<string> check_insert_list;
  set<void *> all_kernels(kernels.begin(), kernels.end());
  for (auto kernel : kernels) {
    map<void *, void *> s_clone_func;
    cout << "Kernel -- " << ast->_p(kernel) << endl;
    //  1. Create new file
    string s_kernel = ast->GetFuncName(kernel);
    string file_name = KERNEL_FILE_PREFIX + ast->legalizeName(s_kernel);
    bool is_cpp_design = ast->isWithInCppFile(kernel);
    file_name = file_name + (is_cpp_design ? ".cpp" : ".c");
    cout << "filename : " << file_name << endl;
    {
      string cmd = "rm -rf " + file_name;
      system(cmd.c_str());
    }

#define NEW_SCHEME_ON_CREATE_FILE 0
#if NEW_SCHEME_ON_CREATE_FILE

    CSageCodeGen new_cg;
    vector<string> src_list;
    src_list.push_back(file_name);
    {
      string cmd;
      // cmd = "echo '#include <hls_math.h>' >> " + new_file_name;
      // system(cmd.c_str());
      cmd = "echo 'int __START__=0;' >> " + file_name;
      system(cmd.c_str());
    }
    new_cg.OpenSourceFile(src_list, "");
    new_cg.InitBuiltinTypes();

    void *new_global_scope = new_cg.GetGlobal(0);

#else
    void *new_global_scope = ast->CreateSourceFile(file_name);
#endif
    //  0. Store vec_strings for __merlin_kernel_list.h
    string kernel_name = codegen->GetFuncName(kernel);
    string add_string = "//" + kernel_name + "=" + file_name + "\n";
    vec_strings.push_back(add_string);

    // if there is some floating variable references left,
    // we will try to trace the remaining floating nodes
    // Is it necessary ???
    ast->replace_floating_var_refs(kernel);
    ast->replace_sizeof(kernel);

    //  2.1 Clone function declaration without function body
    void *new_kernel_func_decl =
        ast->CloneFuncDecl(kernel, new_global_scope, true);
    s_clone_func[kernel] = new_kernel_func_decl;
    codegen->UnSetStatic(new_kernel_func_decl);
    //  2.3 Append new function declaration into new file
    ast->AppendChild(new_global_scope, new_kernel_func_decl);

    //  2.2 Copy function body into new function declaration
    //  2.2.1 Create new function body
    void *kernel_body = ast->GetFuncBody(kernel);

    clean_func_body_before_copy(ast, kernel_body);

    void *new_body = ast->CopyStmt(kernel_body);

    clean_func_body_after_copy(ast, new_body);
    //  2.2.2 Update variable references which are function arguments
    //  i) map oringal argument list to the new argument list
    //  ii) for all the variable references:
    //      if var initialized name == argument, then update variable
    //      reference
    for (int i = 0; i < ast->GetFuncParamNum(kernel); ++i) {
      ast->replace_variable_references_in_scope(
          ast->GetFuncParam(kernel, i),
          ast->GetFuncParam(new_kernel_func_decl, i), new_body);
    }
    //  2.2.3 Insert new function body into new function declaration
    ast->SetFuncBody(new_kernel_func_decl, new_body);

    //  3. Copy or create kernel pragma
    //  Kernel pragma will be updated in the next pass -- kernel_pragma_gen
    void *kernel_pragma = ast->GetPreviousStmt(kernel, false);
    void *new_pragma = nullptr;
    if (ast->IsPragmaDecl(kernel_pragma) != 0) {
      new_pragma = ast->CopyStmt(kernel_pragma);
      ast->DiscardPreprocInfo(new_pragma);
    } else {
      new_pragma = ast->CreatePragma("ACCEL kernel", new_global_scope);
    }
    ast->InsertStmt(new_pragma, new_kernel_func_decl);

    Memo memo(ast);
    auto insertList = copy_decl_in_scope_between_files(
        kernel, new_global_scope, ast, &visited, &visited_global, &memo,
        &s_clone_func, true, all_kernels);

    {
      //  move template member function to the last
      vector<InsertNode> tmfList;
      vector<InsertNode> nonTmfList;
      for (const auto &node : insertList) {
        if (ast->IsTemplateMemberFunctionDecl(node.Node) != 0) {
          tmfList.push_back(node);
        } else {
          nonTmfList.push_back(node);
        }
      }
      insertList.swap(nonTmfList);
      insertList.insert(insertList.end(), tmfList.begin(), tmfList.end());
    }
    //  YX: 20170622 detect merlin_stream
    bool include_merlin_stream = false;

    //  4.1 PostProcessing: Insert the nested sub function calls into the new
    //  kernel function
    for (const auto &node : insertList) {
      //  20170521 ast->_up can't work for global variables
      //  cout << "Copying ... " << ast->_up(insert_node) << endl;
      if (node.IsType) {
        //  strip C++-esque modifiers.
        void *unp_node = node.Node;
        if (isSgDeclarationStatement(static_cast<SgNode *>(node.Node)) !=
            nullptr) {
          //  20170629 Remove dummy function declaration before copying
          vector<void *> v_func_decl;
          ast->GetNodesByType(node.Node, "preorder", "SgFunctionDeclaration",
                              &v_func_decl);
          for (auto func_decl : v_func_decl) {
            if (ast->IsFunctionDeclaration(func_decl) != 0) {
              ast->RemoveStmt(func_decl);
            }
          }
        }

        ast->replace_floating_var_refs(unp_node);
        ast->replace_sizeof(unp_node);
        // 20170620 Write strings into merlin_type_define.h
        string orig_type_string = ast->UnparseToString(unp_node, 0) + "\n";
        if (ast->IsTypedefDecl(unp_node) != 0 &&
            ast->IsAnonymousType(
                static_cast<SgTypedefDeclaration *>(unp_node)->get_base_type()))
          orig_type_string = strip_anonymous_name(orig_type_string);
        string type_string = orig_type_string;
        if (type_string.find("struct merlin_stream") != string::npos) {
          include_merlin_stream = true;
          continue;
        }
        if ((ast->IsFromInputFile(node.Node) == 0) &&
            !ast->IsCompilerGenerated(node.Node) &&
            !ast->IsTransformation(node.Node)) {
          continue;
        }
        // add newline for each pragma
        vector<void *> v_pragma;
        vector<string> v_pragma_string;

        if (ast->IsClassDecl(node.Node) != 0) {
          ast->GetNodesByType(ast->GetClassDefinition(node.Node), "preorder",
                              "SgPragmaDeclaration", &v_pragma);
        } else {
          ast->GetNodesByType(node.Node, "preorder", "SgPragmaDeclaration",
                              &v_pragma);
        }

        for (auto pragma : v_pragma) {
          v_pragma_string.push_back(ast->UnparseToString(pragma));
        }

        size_t offset = 0;
        for (size_t n = 0; n < v_pragma_string.size(); n++) {
          if (type_string.find(v_pragma_string[n], offset) <=
              type_string.size()) {
            type_string.insert(type_string.find(v_pragma_string[n], offset),
                               "\n");
            type_string.insert(type_string.find(v_pragma_string[n], offset) +
                                   v_pragma_string[n].length(),
                               "\n");
            offset = type_string.find(v_pragma_string[n], offset) +
                     v_pragma_string[n].length();
          }
        }
        //  add outside namespace
        SgScopeStatement *scope =
            isSgScopeStatement(static_cast<SgNode *>(ast->GetScope(unp_node)));
        while (auto *ns = isSgNamespaceDefinitionStatement(scope)) {
          string ns_name = ns->get_qualified_name().str();
          while (!ns_name.empty()) {
            auto pos = ns_name.rfind("::");
            string inner_ns_name = ns_name;
            if (pos != string::npos) {
              inner_ns_name = ns_name.substr(pos + 2);
              ns_name = ns_name.substr(0, pos);
            } else {
              ns_name = "";
            }
            type_string = string("namespace ") + inner_ns_name + "{\n" +
                          type_string + "\n}\n";
          }
          scope = isSgScopeStatement(scope->get_parent());
        }
        //  add std for complex
        add_namespace_prefix(&type_string, " complex<", " std::complex<");
        // add hls for stream
        add_namespace_prefix(&type_string, " stream<", " hls::stream<");
        if (check_insert_list.find(type_string) == check_insert_list.end() &&
            check_insert_list.find(orig_type_string) ==
                check_insert_list.end()) {
          vec_type_strings.push_back(type_string);
          check_insert_list.insert(type_string);
          check_insert_list.insert(orig_type_string);
        }
      } else {
        ast->InsertStmt(node.Node, new_pragma);
      }
    }

    //  Insert the header files into the new kernel file
    for (auto insert_header : header_list) {
      void *scope = ast->GetGlobal(new_kernel_func_decl);
      ast->InsertHeader(insert_header, scope, 1, 1);
    }
    //  Insert the header of merlin_type_define.h
    void *scope = ast->GetGlobal(new_kernel_func_decl);
    ast->InsertHeader("merlin_type_define.h", scope, 1, 0);
    if (include_merlin_stream) {
      ast->InsertHeader("merlin_stream.h", scope, 1, 0);
    }

    //  5. Delete the original kernel pragma
    if (ast->IsPragmaDecl(kernel_pragma) != 0) {
      //  comment original kernel pragma
      {
        //  youxiang 20161027 add a dummy function declaration to avoid
        //  the following statement removal stuck
        vector<string> empty;
        void *dummy_decl = codegen->CreateFuncDecl(
            "void", "__merlin_dummy_kernel_pragma", empty, empty,
            codegen->GetGlobal(kernel), false, nullptr);
        codegen->SetStatic(dummy_decl);
        codegen->InsertStmt(dummy_decl, kernel_pragma);
      }
      codegen->RemoveStmt(kernel_pragma, true);
    }

    //  7.1 Add the new file into file list
    //  i) kernel.list
    set_kernel_files.insert(file_name);
    //  ii) host.list
    string host_name = codegen->get_file(kernel);
    set_host_files.insert(host_name);

    //  Yuxin: 20171010:
    typedef_spread_top(codegen, new_global_scope, options, false, false,
                       new_global_scope, false);
    remap_call_for_cloned_function(codegen, new_global_scope, s_clone_func);
    remap_ref_for_cloned_variable(codegen, new_global_scope, &memo);
    kernel_count++;

    ast->clean_implict_conversion_operator(new_global_scope);
    ast->recover_hls_namespace_prefix(new_global_scope);

#if NEW_SCHEME_ON_CREATE_FILE
    new_cg.GenerateCode();
#endif
  }

  //  7.2 Add the new file into file list
  //  i) kernel.list
  for (auto kernel_name : set_kernel_files) {
    string cmd = "echo '" + kernel_name + "' >> " + string(KERNEL_FILE_LIST);
    cout << cmd << endl;
    system(cmd.c_str());
  }
  //  ii) host.list
  for (auto host_name : set_host_files) {
    string cmd = "echo '" + host_name + "' >> " + string(HOST_FILE_LIST);
    cout << cmd << endl;
    system(cmd.c_str());
  }

  FILE *fp = fopen(kernel_list_file.c_str(), "w");
  for (auto add_string : vec_strings) {
    fprintf(fp, "%s", add_string.c_str());
  }
  fclose(fp);

  //  20170620
  {
    printf(" Generating merlin_type_define.h...\n");
    string type_file = "merlin_type_define.h";
    {
      string cmd = "rm -rf " + type_file;
      system(cmd.c_str());
    }
    FILE *fp = fopen(type_file.c_str(), "w");
    fprintf(fp, "%s", "\n#ifndef __MERLIN_TYPE_DEFINE_H__\n");
    fprintf(fp, "%s", "\n#define __MERLIN_TYPE_DEFINE_H__\n");
    for (auto type_string : vec_type_strings) {
      fprintf(fp, "%s", type_string.c_str());
    }
    fprintf(fp, "%s", "\n#endif\n");
    fclose(fp);
  }

  if (m_error != 0) {
    throw std::exception();
  }

  // Leave a tag file marking success of kernel separation
  {
    string cmd = string("rm -f ") + FUNCTION_NAME_UNIQUFIED_TAG;
    system(cmd.c_str());
    if (FILE *fp = fopen(FUNCTION_NAME_UNIQUFIED_TAG, "w")) {
      fclose(fp);
    } else {
      throw std::exception();
    }
  }

  return 0;
}

void check_stream_model(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(codegen, pTopFunc);
  CStreamIR stream_ir(codegen, options, &mars_ir, &mars_ir_v2);
  stream_ir.ParseStreamModel();

  vector<CStreamChannel *> m_fifos = stream_ir.get_fifos();
  for (auto &fifo : m_fifos) {
    void *var_decl = fifo->get_ref();
    void *var_init = codegen->GetVariableInitializedName(var_decl);
    cout << "fifo: " << codegen->_p(var_decl) << endl;

    //  1. check if missing the depth pragma
    string channel_depth = fifo->get_attribute(PRAGMA_CHANNEL_DEPTH_ATTRIBUTE);
    if (channel_depth.empty()) {
      cout << "warnig: missing depth attribute, set as "
           << DEFAULT_CHANNEL_DEPTH << " by default\n";
      fifo->set_attribute(PRAGMA_CHANNEL_DEPTH_ATTRIBUTE,
                          my_itoa(DEFAULT_CHANNEL_DEPTH));
      fifo->update_pragma();
      dump_critical_message(SEPAR_WARNING_2(codegen->_up(var_init),
                                            my_itoa(DEFAULT_CHANNEL_DEPTH)));
    }

    //  2. check write/read to avoid deadlock
    int is_write = 0;
    {
      vector<void *> vec_refs;
      codegen->get_ref_in_scope(var_init, nullptr, &vec_refs, true);
      for (auto ref : vec_refs) {
        if (codegen->is_write_conservative(ref) != 0) {
          is_write = 1;
          break;
        }
      }
    }

    int is_read = 0;
    {
      vector<void *> vec_refs;
      codegen->get_ref_in_scope(var_init, nullptr, &vec_refs, true);
      for (auto ref : vec_refs) {
        if (codegen->has_read_conservative(ref, true /*recursive*/) != 0) {
          is_read = 1;
          break;
        }
      }
    }

    cout << "w/r: " << is_write << "," << is_read << endl;
    //  read only or write only
    if ((is_write == 0) && (is_read != 0)) {
      dump_critical_message(SEPAR_WARNING_3(codegen->_up(var_init)));
    }
    if ((is_write != 0) && (is_read == 0)) {
      dump_critical_message(SEPAR_WARNING_4(codegen->_up(var_init)));
    }
  }
}
