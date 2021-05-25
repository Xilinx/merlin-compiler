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


#include <algorithm>
#include <unordered_set>
#include "struct_decompose.h"
#include "cmdline_parser.h"
#include "codegen.h"
#include "file_parser.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "xml_parser.h"
#define MERLIN_ASSIGN_PREFIX "merlin_assign_"
#define MERLIN_DECOMPOSE_SUFFIX "merlin_decompose"
#define ALTERA_ARGUMENTS_LIMITATION 128
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

using std::tuple;
//  decompose arguments, local variables and global variables with
//  struct type recursively into members with primative types
//  e.g. struct st { int a; char b;};  struct st *var;
//  ==>
//  int *var_a;
//  char *var_b;
//
//  1. decompose local variables and global variables
//  1.1 update all references except references on function call
//  1.2 store the mapping relation from orignal variables to
//     new decomposed variables
//  2. decompose arguments
//  2.1 create new functions with new decomposed member arguments
//  2.2 update arguments referencs with local functions
//  2.3 update function calls
//  3. remove all dead struct arguments
//  3.1 create new functions with no struct arguments
//
static string get_dim_str(vector<size_t> size, int cont_flag) {
  string ret;
  //  if (size.empty())
  //  return ret;
  size_t mult = 1;
  for (auto t : size) {
    ret += my_itoa(t) + ",";
    mult *= t;
  }

  int n_size = ret.size();
  string str_non_cont = ret.substr(0, n_size - 1);

  return cont_flag != 0 ? my_itoa(mult) : str_non_cont;
}

static string get_wrapper_variable_name(CSageCodeGen *codegen,
                                        const string &var_name,
                                        void *func_decl) {
  static map<void *, map<string, string>> pragma_cache;
  if (pragma_cache.count(func_decl) <= 0) {
    vector<void *> vec_pragma;
    codegen->GetNodesByType(func_decl, "preorder", "SgPragmaDeclaration",
                            &vec_pragma);
    for (auto pragma : vec_pragma) {
      CAnalPragma ana_pragma(codegen);
      bool errout;
      ana_pragma.PragmasFrontendProcessing(pragma, &errout, false);
      string old_var_name = ana_pragma.get_attribute(CMOST_variable);
      string new_var_name = ana_pragma.get_attribute(CMOST_new_variable);
      if (!old_var_name.empty() && !new_var_name.empty())
        pragma_cache[func_decl][old_var_name] = new_var_name;
    }
  }
  return pragma_cache[func_decl].count(var_name) > 0
             ? pragma_cache[func_decl][var_name]
             : var_name;
}

static std::string arrow_to_dot(string name) {
  std::string::size_type pos;
  while ((pos = name.find("->")) != std::string::npos) {
    name = name.substr(0, pos) + "." + arrow_to_dot(name.substr(pos + 2));
  }
  return name;
}

vector<size_t>
StructDecompose::get_array_dimension(string field_name, int *cont_flag,
                                     void *pos,
                                     const vector<void *> &vec_pragma) {
  vector<string> vs_expr = str_split(field_name, "->");
  void *func_decl = m_ast->TraceUpToFuncDecl(pos);
  string func_decl_str = m_ast->_up(func_decl);

  int first = 1;
  string org_loc = getUserCodeFileLocation(m_ast, pos, true);
  void *curr_struct_type = nullptr;
  void *sg_type = nullptr;
  vector<size_t> dims;
  string var_prefix;
  for (auto s_var : vs_expr) {
    if (first != 0) {
      var_prefix = s_var;
    } else {
      var_prefix += "->" + s_var;
    }
    void *sg_init = nullptr;
    if (first != 0) {
      sg_init = m_ast->find_variable_by_name(s_var, pos);
      first = 0;
    } else {
      if (curr_struct_type != nullptr) {
        sg_init = m_ast->GetClassMemberByName(s_var, curr_struct_type);
        pos = m_ast->GetVariableDecl(sg_init);
      }
    }
    if (sg_init == nullptr) {
      return dims;
    }
    vector<size_t> sub_dims =
        mMarsIrV2.get_array_dimension(sg_init, cont_flag, sg_init);
    bool has_more = false;
    for (auto dim : sub_dims) {
      if (static_cast<int64_t>(dim) <= 0) {
        has_more = true;
      }
    }
    if (has_more) {
      for (auto pragma : vec_pragma) {
        if (mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable) ==
            var_prefix) {
          string pragma_info = getUserCodeFileLocation(m_ast, pragma, true);
          string s_max_depth =
              mMarsIrV2.get_pragma_attribute(pragma, "max_depth");

          vector<string> vs_max_depth = str_split(s_max_depth, ',');
          size_t i = 0;
          if (vs_max_depth.size() > sub_dims.size()) {
            dump_critical_message(MDARR_ERROR_7(s_max_depth, var_prefix,
                                                std::to_string(sub_dims.size()),
                                                pragma_info));
            mErrorOut = true;
          }
          for (auto depth : vs_max_depth) {
            if (0 >= static_cast<int64_t>(sub_dims[i]) && i < sub_dims.size()) {
              sub_dims[i] = my_atoi(depth);
            }
            ++i;
          }

          string s_depth = mMarsIrV2.get_pragma_attribute(pragma, "depth");

          vector<string> vs_depth = str_split(s_depth, ',');
          if (vs_depth.size() > sub_dims.size()) {
            dump_critical_message(MDARR_ERROR_7(s_depth, var_prefix,
                                                std::to_string(sub_dims.size()),
                                                pragma_info));
            mErrorOut = true;
          }
          i = 0;
          for (auto depth : vs_depth) {
            if (0 >= static_cast<int64_t>(sub_dims[i]) && i < sub_dims.size()) {
              sub_dims[i] = my_atoi(depth);
            }
            ++i;
          }
        }
      }
    }
    has_more = false;
    for (auto dim : sub_dims) {
      if (static_cast<int64_t>(dim) <= 0) {
        has_more = true;
      }
    }
    if (has_more) {
      vector<void *> vec_curr_pragma;
      m_ast->GetNodesByType(m_ast->TraceUpToScope(pos), "preorder",
                            "SgPragmaDeclaration", &vec_curr_pragma);
      for (auto pragma : vec_curr_pragma) {
        if (mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable) == s_var) {
          string s_max_depth =
              mMarsIrV2.get_pragma_attribute(pragma, "max_depth");

          vector<string> vs_max_depth = str_split(s_max_depth, ',');
          size_t i = 0;
          for (auto depth : vs_max_depth) {
            if (0 >= static_cast<int64_t>(sub_dims[i]) && i < sub_dims.size()) {
              sub_dims[i] = my_atoi(depth);
            }
            ++i;
          }

          string s_depth = mMarsIrV2.get_pragma_attribute(pragma, "depth");

          vector<string> vs_depth = str_split(s_depth, ',');
          i = 0;
          for (auto depth : vs_depth) {
            if (0 >= static_cast<int64_t>(sub_dims[i]) && i < sub_dims.size()) {
              sub_dims[i] = my_atoi(depth);
            }
            ++i;
          }
        }
      }
    }
    string identifier = func_decl_str + ":" + var_prefix;
    if (has_check(identifier) == 0) {
      for (size_t i = 0; i < sub_dims.size(); i++) {
        int64_t s = sub_dims[i];
        if (s <= 0) {
          mVar2ZeroDims[func_decl][var_prefix] =
              make_tuple(sg_init, sub_dims, org_loc);
        }
      }
    }
    dims.insert(dims.end(), sub_dims.begin(), sub_dims.end());
    sg_type = m_ast->GetTypebyVar(sg_init);
    void *sg_base = m_ast->GetBaseType(sg_type, true);

    if ((m_ast->IsStructureType(sg_base) != 0) ||
        (m_ast->IsClassType(sg_base) != 0)) {
      curr_struct_type = sg_base;
    }
  }
  return dims;
}

vector<string>
StructDecompose::get_array_depth(string field_name, void *pos,
                                 const vector<void *> &vec_pragma) {
  vector<string> vs_expr = str_split(field_name, "->");
  string func_decl = m_ast->_up(m_ast->TraceUpToFuncDecl(pos));

  int first = 1;

  void *curr_struct_type = nullptr;
  void *sg_type = nullptr;
  vector<string> depth_res;
  string var_prefix;
  for (auto s_var : vs_expr) {
    if (first != 0) {
      var_prefix = s_var;
    } else {
      var_prefix += "->" + s_var;
    }
    void *sg_init = nullptr;
    if (first != 0) {
      sg_init = m_ast->find_variable_by_name(s_var, pos);
      first = 0;
    } else {
      if (curr_struct_type != nullptr) {
        sg_init = m_ast->GetClassMemberByName(s_var, curr_struct_type);
        pos = m_ast->GetVariableDecl(sg_init);
      }
    }
    if (sg_init == nullptr) {
      return depth_res;
    }
    sg_type = m_ast->GetTypebyVar(sg_init);
    if ((m_ast->IsPointerType(sg_type) != 0) ||
        (m_ast->IsArrayType(sg_type) != 0)) {
      vector<string> curr_depth_res;
      for (auto pragma : vec_pragma) {
        if (mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable) ==
            var_prefix) {
          string s_depth = mMarsIrV2.get_pragma_attribute(pragma, "depth");
          if (s_depth.empty())
            s_depth = mMarsIrV2.get_pragma_attribute(pragma, "max_depth");
          vector<string> vs_depth = str_split(s_depth, ',');
          for (auto depth : vs_depth) {
            if (depth.find("->") != string::npos) {
              vector<string> vs_vec = str_split(depth, "->");
              string field_name = str_merge('_', vs_vec);
              if ((m_ast->find_variable_by_name(field_name, pragma) !=
                   nullptr) ||
                  m_ast->check_valid_field_name(depth, pragma)) {
#if 0
                depth = field_name;
#endif
              } else {
                cout << "Find unsupported depth " << depth << endl;
                string pragma_info =
                    "'" + depth + "' " +
                    getUserCodeFileLocation(m_ast, pragma, true);
                dump_critical_message(DISAG_ERROR_4(pragma_info));
                mErrorOut = true;
              }
            }
            curr_depth_res.push_back(depth);
          }
          break;
        }
      }
      vector<void *> vec_curr_pragma;
      m_ast->GetNodesByType(m_ast->TraceUpToScope(pos), "preorder",
                            "SgPragmaDeclaration", &vec_curr_pragma);
      for (auto pragma : vec_curr_pragma) {
        if (mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable) == s_var) {
          string s_depth = mMarsIrV2.get_pragma_attribute(pragma, "depth");
          if (s_depth.empty())
            s_depth = mMarsIrV2.get_pragma_attribute(pragma, "max_depth");
          vector<string> vs_depth = str_split(s_depth, ',');
          int dim = 0;
          for (auto depth : vs_depth) {
            if (depth.find("->") != string::npos) {
              vector<string> vs_vec = str_split(depth, "->");
              string field_name = str_merge('_', vs_vec);
              if ((m_ast->find_variable_by_name(field_name, pragma) !=
                   nullptr) ||
                  m_ast->check_valid_field_name(depth, pragma)) {
#if 0
                depth = field_name;
#endif
              } else {
                cout << "Find unsupported depth " << depth << endl;
                string pragma_info =
                    "'" + depth + "' " +
                    getUserCodeFileLocation(m_ast, pragma, true);
                dump_critical_message(DISAG_ERROR_4(pragma_info));
                mErrorOut = true;
              }
            }
            if (!depth.empty() &&
                !std::all_of(depth.begin(), depth.end(),
                             [](auto c) { return isdigit(c); })) {
              depth = var_prefix + "->" + depth;
            }
            if (dim < curr_depth_res.size()) {
              if (curr_depth_res[dim].empty()) {
                curr_depth_res[dim] = depth;
              }
            } else {
              curr_depth_res.push_back(depth);
            }
            dim++;
          }
          break;
        }
      }
      int cont_flag;
      vector<size_t> sub_dims =
          mMarsIrV2.get_array_dimension(sg_init, &cont_flag, sg_init);
      int dim = 0;
      for (auto sub_dim : sub_dims) {
        if (dim < curr_depth_res.size()) {
          if (curr_depth_res[dim].empty())
            curr_depth_res[dim] = my_itoa(sub_dim);
        } else {
          curr_depth_res.push_back(my_itoa(sub_dim));
        }
        dim++;
      }
      depth_res.insert(depth_res.end(), curr_depth_res.begin(),
                       curr_depth_res.end());
    }

    void *sg_base = m_ast->GetBaseType(sg_type, true);

    if ((m_ast->IsStructureType(sg_base) != 0) ||
        (m_ast->IsClassType(sg_base) != 0)) {
      curr_struct_type = sg_base;
    }
  }
  return depth_res;
}

int StructDecompose::get_dim_offset(string field_name, void *pos) {
  vector<string> vs_expr = str_split(field_name, "->");
  if (vs_expr.size() <= 1)
    return 0;
  string func_decl = m_ast->_up(m_ast->TraceUpToFuncDecl(pos));
  void *curr_struct_type = nullptr;
  void *sg_type = nullptr;
  int offset = 0;
  for (size_t i = 0; i < vs_expr.size() - 1; ++i) {
    string s_var = vs_expr[i];
    void *sg_init = nullptr;
    if (i == 0) {
      sg_init = m_ast->find_variable_by_name(s_var, pos);
    } else {
      if (curr_struct_type != nullptr) {
        sg_init = m_ast->GetClassMemberByName(s_var, curr_struct_type);
        pos = m_ast->GetVariableDecl(sg_init);
      }
    }
    if (sg_init == nullptr) {
      return offset;
    }
    offset += m_ast->get_dim_num_from_var(sg_init);
    sg_type = m_ast->GetTypebyVar(sg_init);
    void *sg_base = m_ast->GetBaseType(sg_type, true);

    if ((m_ast->IsStructureType(sg_base) != 0) ||
        (m_ast->IsClassType(sg_base) != 0)) {
      curr_struct_type = sg_base;
    }
  }
  return offset;
}

int StructDecompose::get_dim_size(string field_name, void *pos) {
  vector<string> vs_expr = str_split(field_name, "->");
  string func_decl = m_ast->_up(m_ast->TraceUpToFuncDecl(pos));
  void *curr_struct_type = nullptr;
  void *sg_type = nullptr;
  for (size_t i = 0; i < vs_expr.size(); ++i) {
    string s_var = vs_expr[i];
    void *sg_init = nullptr;
    if (i == 0) {
      sg_init = m_ast->find_variable_by_name(s_var, pos);
    } else {
      if (curr_struct_type != nullptr) {
        sg_init = m_ast->GetClassMemberByName(s_var, curr_struct_type);
        pos = m_ast->GetVariableDecl(sg_init);
      }
    }
    if (sg_init == nullptr) {
      return 0;
    }
    if (i == vs_expr.size() - 1)
      return m_ast->get_dim_num_from_var(sg_init);
    sg_type = m_ast->GetTypebyVar(sg_init);
    void *sg_base = m_ast->GetBaseType(sg_type, true);

    if ((m_ast->IsStructureType(sg_base) != 0) ||
        (m_ast->IsClassType(sg_base) != 0)) {
      curr_struct_type = sg_base;
    }
  }
  return 0;
}

int struct_decompose_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  StructDecompose sd(codegen, pTopFunc, options);
  sd.run();
  sd.check();
  if (sd.fail() != 0) {
    throw std::exception();
  }
  return 1;
}

void StructDecompose::init() {
  mMarsIr.clear();
  mMarsIrV2.clear();
  mMarsIr.get_mars_ir(m_ast, mTopFunc, mOptions, true);
  mMarsIrV2.build_mars_ir(m_ast, mTopFunc);
  //  collect all the kernel functions
  vector<CMirNode *> vec_nodes;
  mMarsIr.get_topological_order_nodes(&vec_nodes);
  mErrorOut = false;
  mKernelFuncs.clear();
  mKernelFuncsSet.clear();
  mTopKernel.clear();
  for (auto node : vec_nodes) {
    if (node->isFunction()) {
      void *func_body = node->get_ref();
      void *func_decl =
          m_ast->GetFuncDeclByDefinition(m_ast->GetParent(func_body));
      mKernelFuncs.push_back(func_decl);
      mKernelFuncsSet.insert(func_decl);
      if (mMarsIr.is_kernel(m_ast, func_decl)) {
        mTopKernel.insert(func_decl);
      }
    }
  }
}

// convert general assignment expression between two struct variables into
// bitwise assignment between each fields recursively
// e.g.
// struct st {int f1, int f2[10];} a, b;
// a = b;
// =>
// ({a.f1 = b.f1;
// for (int i = 0; i < 2; ++i) {
//   a.f2[i] = b.f2[i];
// }
// });
bool StructDecompose::process_assign_exp(void *assign_exp, void *left_op,
                                         void *right_op, void *func) {
  void *left_type = m_ast->GetTypeByExp(left_op);
  void *right_type = m_ast->GetTypeByExp(right_op);
  if (m_ast->IsReferenceType(left_type) != 0) {
    left_type = m_ast->GetElementType(left_type);
  }
  if (m_ast->IsReferenceType(right_type) != 0) {
    right_type = m_ast->GetElementType(right_type);
  }
  if ((m_ast->IsPointerType(left_type) != 0) ||
      (m_ast->IsArrayType(left_type) != 0) ||
      (m_ast->IsCompatibleType(left_type, right_type, assign_exp, true) == 0) ||
      containsCompoundType(left_type) != SUPPORTED_COMPOUND_TYPE ||
      m_ast->has_side_effect(right_op) != 0) {
    return false;
  }
  //  FIXME: because we need to copy 'left_op', we cannot support if
  //  'left_op' has side effect
  if (m_ast->has_side_effect(left_op) != 0) {
    split_expression_with_side_effect(left_op);

    //  cout << "Cannot support struct assignment with side effect on the
    //  left "
    //        "operand "
    //     << m_ast->_p(assign_exp) << endl;
    //  continue;
  }
  auto inlined_stmts =
      InlineBuiltInAssign(assign_exp, left_type, func,
                          m_ast->TraceUpToBasicBlock(assign_exp), assign_exp);
  void *assign_exp_p = m_ast->GetParent(assign_exp);
  void *stmt_expr = nullptr;
  if (m_ast->TraceUpToExpression(assign_exp_p) != nullptr ||
      m_ast->IsExprStatement(assign_exp_p) == 0) {
    void *inlined_block = m_ast->CreateBasicBlock();
    for (auto stmt : inlined_stmts) {
      m_ast->AppendChild(inlined_block, stmt);
    }
    stmt_expr = m_ast->CreateExp(V_SgStatementExpression, inlined_block,
                                 nullptr, 0, assign_exp);
  }
  if (m_ast->TraceUpToExpression(assign_exp_p) != nullptr) {
    //  replace it with comma expression
    void *copy_left_op = m_ast->CopyExp(left_op);
    void *comma_exp = m_ast->CreateExp(V_SgCommaOpExp, stmt_expr, copy_left_op,
                                       0, assign_exp);
    m_ast->ReplaceExp(assign_exp, comma_exp);
  } else {
    if (m_ast->IsExprStatement(assign_exp_p)) {
      for (auto stmt : inlined_stmts)
        m_ast->InsertStmt(stmt, assign_exp_p);
      m_ast->RemoveStmt(assign_exp_p);
    } else {
      m_ast->ReplaceExp(assign_exp, stmt_expr);
    }
  }
  return true;
}

int StructDecompose::preprocess() {
  bool Changed = false;
  //  1. convert struct return type into argument
  vector<void *> vec_new_funcs;
  for (auto func : mKernelFuncs) {
    void *new_func = convertStructReturnTypeIntoParameter(func);
    vec_new_funcs.push_back(new_func);
    if (new_func == func) {
      continue;
    }
    void *ret_type = m_ast->GetFuncReturnType(func);
    if (m_ast->IsReferenceType(ret_type) != 0) {
      ret_type = m_ast->GetElementType(ret_type);
    }
    vector<void *> vec_call;
    m_ast->GetNodesByType(nullptr, "postorder", "SgFunctionCallExp", &vec_call);
    for (auto call : vec_call) {
      void *callee_func = m_ast->GetFuncDeclByCall(call);
      if (callee_func == nullptr) {
        continue;
      }
      if (!m_ast->isSameFunction(func, callee_func)) {
        continue;
      }
      void *scope = m_ast->TraceUpToBasicBlock(call);
      void *caller_func = m_ast->TraceUpToFuncDecl(call);
      string ret_var_name = "ret_var";
      m_ast->get_valid_local_name(caller_func, &ret_var_name);
      void *ret_var = m_ast->CreateVariableDecl(ret_type, ret_var_name, nullptr,
                                                scope, call);
      m_ast->PrependChild(scope, ret_var, true);
      vector<void *> vec_args;
      vec_args.push_back(m_ast->CreateExp(
          V_SgAddressOfOp, m_ast->CreateVariableRef(ret_var, call), nullptr, 0,
          call));
      for (int i = 0; i != m_ast->GetFuncCallParamNum(call); ++i) {
        vec_args.push_back(m_ast->CopyExp(m_ast->GetFuncCallParam(call, i)));
      }
      void *new_call = m_ast->CreateFuncCall(new_func, vec_args, call);
      void *orig_stmt = m_ast->TraceUpToStatement(call);
      void *new_stmt =
          m_ast->CreateStmt(V_SgExprStatement, new_call, orig_stmt);
      m_ast->InsertStmt(new_stmt, orig_stmt);
      m_ast->ReplaceExp(call, m_ast->CreateVariableRef(ret_var, call));
      if (mTopKernel.count(func) <= 0) {
        setFuncDeclUserCodeScopeId(m_ast, new_func, new_call);
      }
    }
    mKernelFuncsSet.erase(func);
    mKernelFuncsSet.insert(new_func);
    m_ast->RemoveStmt(func);
    Changed = true;
  }

  //  split initializer
  int num_func = vec_new_funcs.size();
  for (int i = 0; i != num_func; ++i) {
    void *func = vec_new_funcs[i];
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    vector<void *> var_decls;
    m_ast->GetNodesByType(func_body, "preorder", "SgVariableDeclaration",
                          &var_decls);
    for (auto var : var_decls) {
      if (!m_ast->IsBasicBlock(m_ast->GetParent(var))) {
        // cannot support split initializer for variable declaration not in a
        // basic block
        continue;
      }
      void *var_init = m_ast->GetVariableInitializedName(var);
      void *var_initializer = m_ast->GetInitializer(var_init);
      if ((var_initializer == nullptr) ||
          (m_ast->IsAggregateInitializer(var_initializer) != 0)) {
        continue;
      }
      void *init_op = m_ast->GetInitializerOperand(var_initializer);
      if (init_op == nullptr) {
        continue;
      }
      //  if there is no variable ref, we can handle in the future
      vector<void *> var_refs;
      m_ast->GetNodesByType(var_initializer, "preorder", "SgVarRefExp",
                            &var_refs);
      if (var_refs.empty()) {
        continue;
      }
      void *var_type = m_ast->GetTypebyVar(var_init);
      if (containsCompoundType(var_type) != SUPPORTED_COMPOUND_TYPE) {
        continue;
      }
      //  FIXME: we cannot split reference or array variables
      if ((m_ast->IsPointerType(var_type) != 0) ||
          (m_ast->IsArrayType(var_type) != 0) ||
          (m_ast->IsReferenceType(var_type) != 0)) {
        continue;
      }
      if (m_ast->IsConstType(var_type) != 0) {
        bool changed = false;
        void *new_type = m_ast->RemoveConstType(var_type, var_init, &changed);
        if ((new_type != nullptr) && (new_type != nullptr)) {
          m_ast->SetTypetoVar(var_init, new_type);
        }
      }
      void *var_ref = m_ast->CreateVariableRef(var);
      m_ast->SetInitializer(var_init, nullptr);
      void *new_stmt =
          m_ast->CreateStmt(V_SgAssignStatement, var_ref, init_op, var);
      m_ast->InsertAfterStmt(new_stmt, var);
    }
  }

  //  convert sizeof expression
  for (int i = 0; i != num_func; ++i) {
    void *func = vec_new_funcs[i];
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    m_ast->replace_sizeof(func_body);
  }

  //  2. process all the access on struct variables into
  //  access on the leaf node
  //  e.g. struct st {int a; int b;};
  //  st sa, sb;
  //  sa = sb;
  //  ===>
  //  assign_st(&sa, sb);

  //  we only need to handle built-in assignment, for other operation
  //  it should be called via class member functions
  for (int i = 0; i != num_func; ++i) {
    void *func = vec_new_funcs[i];
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    vector<void *> var_assign_exp;
    bool split_multi_assign;
    do {
      split_multi_assign = false;
      var_assign_exp.clear();
      m_ast->GetNodesByType(func_body, "preorder", "SgAssignOp",
                            &var_assign_exp);
      for (auto assign_exp : var_assign_exp) {
        void *left_op = nullptr;
        void *right_op = nullptr;
        m_ast->GetExpOperand(assign_exp, &left_op, &right_op);
        void *left_type = m_ast->GetTypeByExp(left_op);
        void *right_type = m_ast->GetTypeByExp(right_op);
        if ((m_ast->IsPointerType(left_type) != 0) ||
            (m_ast->IsArrayType(left_type) != 0) ||
            !SageInterface::checkTypesAreEqual(
                isSgType(static_cast<SgNode *>(left_type)),
                isSgType(static_cast<SgNode *>(right_type))) ||
            containsCompoundType(left_type) != SUPPORTED_COMPOUND_TYPE) {
          continue;
        }
        if (m_ast->SplitMultiAssign(assign_exp)) {
          split_multi_assign = true;
          break;
        }
      }
    } while (split_multi_assign);

    var_assign_exp.clear();
    m_ast->GetNodesByType(func_body, "postorder", "SgAssignOp",
                          &var_assign_exp);
    for (auto assign_exp : var_assign_exp) {
      void *left_op = nullptr;
      void *right_op = nullptr;
      if (m_ast->IsGeneralAssignOp(assign_exp, &left_op, &right_op)) {
        Changed |= process_assign_exp(assign_exp, left_op, right_op, func);
      }
    }
    vector<void *> implicit_assign_exp;
    m_ast->GetNodesByType(func_body, "postorder", "SgFunctionCallExp",
                          &implicit_assign_exp);
    for (auto assign_exp : implicit_assign_exp) {
      void *left_op = nullptr;
      void *right_op = nullptr;
      if (m_ast->IsGeneralAssignOp(assign_exp, &left_op, &right_op)) {
        Changed |= process_assign_exp(assign_exp, left_op, right_op, func);
      }
    }
  }

  //  remove redundant cast
  for (int i = 0; i != num_func; ++i) {
    void *func = vec_new_funcs[i];
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    vector<void *> var_cast_exp;
    m_ast->GetNodesByType(func_body, "postorder", "SgCastExp", &var_cast_exp);
    for (auto cast_exp : var_cast_exp) {
      void *orig_op = m_ast->GetExpUniOperand(cast_exp);
      void *orig_type = m_ast->GetTypeByExp(orig_op);
      void *cast_type = m_ast->GetTypeByExp(cast_exp);
      if ((m_ast->IsCompatibleType(orig_type, cast_type, cast_exp, true) ==
           0) ||
          containsCompoundType(cast_type) != SUPPORTED_COMPOUND_TYPE) {
        continue;
      }
      m_ast->ReplaceExp(cast_exp, m_ast->CopyExp(orig_op));
      Changed = true;
    }
  }

  // split pragma with dim=0 into multiple pragma according to dimension of
  // variables
  for (auto func : vec_new_funcs) {
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    vector<void *> vec_pragma;
    m_ast->GetNodesByType_int(func_body, "preorder", V_SgPragmaDeclaration,
                              &vec_pragma);
    for (auto pragma : vec_pragma) {
      string str_dim = mMarsIrV2.get_pragma_attribute(pragma, CMOST_dim, true);
      if (str_dim.empty() || str_dim != "0")
        continue;
      string str_var =
          mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable, true);
      vector<string> vec_var = str_split(str_var, ',');
      bool all_contains_valid_size = true;
      vector<int> vec_dim_size;
      for (auto var : vec_var) {
        int dim_size = get_dim_size(var, pragma);
        if (dim_size > 0)
          vec_dim_size.push_back(dim_size);
        else
          all_contains_valid_size = false;
      }
      if (!all_contains_valid_size)
        continue;
      for (size_t i = 0; i < vec_var.size(); ++i) {
        auto var = vec_var[i];
        auto dim_size = vec_dim_size[i];
        for (int dim = 1; dim <= dim_size; ++dim) {
          auto copy_pragma = m_ast->CopyStmt(pragma);
          m_ast->InsertStmt(copy_pragma, pragma);
          mMarsIrV2.set_pragma_attribute(&copy_pragma, CMOST_variable, var,
                                         true);
          mMarsIrV2.set_pragma_attribute(&copy_pragma, CMOST_dim, my_itoa(dim),
                                         true);
        }
      }
      m_ast->RemoveStmt(pragma);
      Changed = true;
    }
  }
  return static_cast<int>(Changed);
}

int StructDecompose::run() {
  int Changed = 0;

  collect_used_fields();

  bool LocalChanged = preprocess() != 0;
  if (LocalChanged) {
    init();
    Changed = 1;
  }

  //  collect all global variables with struct type
  map<void *, set<void *>> global_vars;
  set<void *> used_struct_vars;
  for (auto func : mKernelFuncs) {
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    vector<void *> var_refs;
    m_ast->GetNodesByType(func_body, "preorder", "SgVarRefExp", &var_refs);
    for (auto ref : var_refs) {
      void *var_init = m_ast->GetVariableInitializedName(ref);
      if (m_ast->IsGlobalInitName(var_init) == 0 ||
          m_ast->IsInputOutputGlobalVariable(var_init)) {
        continue;
      }
      if (global_vars.count(var_init) > 0) {
        global_vars[var_init].insert(func);
      } else {
        void *var_type = m_ast->GetTypebyVar(var_init);
        string type_info;
        enum type_kind res = containsCompoundType(var_type, &type_info);
        if (res != SUPPORTED_COMPOUND_TYPE) {
          if (res == UNSUPPORTED_COMPOUND_TYPE) {
            string port_info = "global port \'" +
                               m_ast->GetVariableName(var_init) + "\' " +
                               getUserCodeFileLocation(m_ast, var_init, true) +
                               " with unsupported type: \n" + type_info;
            dump_critical_message(DISAG_ERROR_2(port_info));
            mErrorOut = true;
          }
        } else {
          global_vars[var_init].insert(func);
        }
      }
    }
  }

  //  decompose global variables
  for (auto var_and_ref : global_vars) {
    void *var_init = var_and_ref.first;
    void *initializer = m_ast->GetInitializer(var_init);
    if (m_ast->UnparseToString(initializer).empty()) {
      m_ast->SetInitializer(var_init, nullptr);
      initializer = nullptr;
    }
    set<void *> funcs = var_and_ref.second;
    vector<void *> refs;
    for (auto func : funcs) {
      void *func_body = m_ast->GetFuncBody(func);
      if (func_body == nullptr) {
        continue;
      }
      vector<void *> sub_refs;
      m_ast->get_ref_in_scope(var_init, func_body, &sub_refs);
      refs.insert(refs.end(), sub_refs.begin(), sub_refs.end());
    }

    string diagnosis_info;
    if ((canSafeDecompose(var_init, refs, &diagnosis_info) != 0) &&
        (canDecomposeInitializer(initializer, &diagnosis_info) != 0)) {
      for (auto func : funcs) {
        void *func_body = m_ast->GetFuncBody(func);
        if (func_body == nullptr) {
          continue;
        }
        decomposeGlobalVariable(var_init, func_body);
      }
      used_struct_vars.insert(var_init);
      Changed = 1;
    } else {
      string port_info =
          "global port \'" + m_ast->GetVariableName(var_init) + "\' " +
          getUserCodeFileLocation(m_ast, var_init, true) +
          " with unsupported reference or initializer: \n" + diagnosis_info;
      dump_critical_message(DISAG_ERROR_1(port_info));
      mErrorOut = true;
    }
  }

  //  decompose each function
  vector<void *> new_funcs;
  for (size_t i = 0; i != mKernelFuncs.size(); ++i) {
    void *func = mKernelFuncs[i];
    bool is_dead = true;
    void *new_func = decomposeFunction(func, &is_dead);
    void *new_func_body = m_ast->GetFuncBody(new_func);
    //  decompose local variables
    vector<void *> var_decls;
    m_ast->GetNodesByType(new_func_body, "preorder", "SgVariableDeclaration",
                          &var_decls);
    for (auto var : var_decls) {
      void *var_init = m_ast->GetVariableInitializedName(var);
      void *initializer = m_ast->GetInitializer(var_init);
      if (m_ast->UnparseToString(initializer).empty()) {
        m_ast->SetInitializer(var_init, nullptr);
        initializer = nullptr;
      }
      void *var_type = m_ast->GetTypebyVar(var_init);
      string type_info;
      enum type_kind res = containsCompoundType(var_type, &type_info);
      if (res != SUPPORTED_COMPOUND_TYPE) {
        if (res == UNSUPPORTED_COMPOUND_TYPE) {
          string port_info = "local variable \'" +
                             m_ast->GetVariableName(var_init) + "\' " +
                             getUserCodeFileLocation(m_ast, var_init, true) +
                             " with unsupported type: \n" + type_info;
          dump_critical_message(DISAG_ERROR_2(port_info));
          mErrorOut = true;
        }
      } else {
        vector<void *> refs;
        void *scope = m_ast->TraceUpToBasicBlock(var);
        m_ast->get_ref_in_scope(var_init, scope, &refs);
        string diagnosis_info;
        if ((canSafeDecompose(var_init, refs, &diagnosis_info) != 0) &&
            (canDecomposeInitializer(initializer, &diagnosis_info) != 0)) {
          decomposeLocalVariable(var_init, scope);
          used_struct_vars.insert(var_init);
          Changed = 1;
        } else {
          string port_info =
              "local variable \'" + m_ast->GetVariableName(var_init) + "\' " +
              getUserCodeFileLocation(m_ast, var_init, true) +
              " with unsupported reference or initializer: \n" + diagnosis_info;
          dump_critical_message(DISAG_ERROR_1(port_info), 1);
        }
      }
    }
    //  decompose parameters
    int num_arg = m_ast->GetFuncParamNum(func);
    for (int j = 0; j != num_arg; ++j) {
      void *param = m_ast->GetFuncParam(func, j);
      void *param_type = m_ast->GetTypebyVar(param);
      string type_info;
      enum type_kind res = containsCompoundType(param_type, &type_info);
      if (res != SUPPORTED_COMPOUND_TYPE) {
        if (res == UNSUPPORTED_COMPOUND_TYPE) {
          string port_info = "interface port \'" +
                             m_ast->GetVariableName(param) + "\' " +
                             getUserCodeFileLocation(m_ast, param, true) +
                             " with unsupported type: \n" + type_info;
          dump_critical_message(DISAG_ERROR_2(port_info), 1);
        }
      } else {
        vector<void *> refs;
        m_ast->get_ref_in_scope(param, new_func_body, &refs);
        string diagnosis_info;
        if (canSafeDecompose(param, refs, &diagnosis_info) != 0) {
          decomposeParameter(param, new_func_body);
        } else {
          string port_info =
              "interface port \'" + m_ast->GetVariableName(param) + "\' " +
              getUserCodeFileLocation(m_ast, param, true) +
              " with unsupported reference or initializer: \n" + diagnosis_info;
          dump_critical_message(DISAG_ERROR_1(port_info), 1);
        }
      }
    }
    if (new_func != func) {
      mKernelFuncsSet.erase(func);
      mKernelFuncsSet.insert(new_func);
      if (mTopKernel.count(func) > 0) {
        mTopKernel.erase(func);
        mTopKernel.insert(new_func);
        mVar2ZeroDims[new_func].swap(mVar2ZeroDims[func]);
        mVar2ZeroDims.erase(func);
      }
      if (is_dead) {
        m_ast->RemoveStmt(func);
      }
      Changed = 1;
    }
    new_funcs.push_back(new_func);
    // update all the pragma according to token map
    update_pragma_with_token_map(new_func);
  }

  mKernelFuncs.clear();
  mKernelFuncs = new_funcs;
  // update variable references now
  std::unordered_set<void *> visited_vars;
  for (auto var_init_scope : m_var_vec) {
    void *var_init = var_init_scope.first;
    void *scope = var_init_scope.second;
    if (visited_vars.count(var_init) > 0)
      continue;
    visited_vars.insert(var_init);
    if (m_ast->IsLocalInitName(var_init) ||
        m_ast->IsArgumentInitName(var_init)) {
      replaceVariableRefs(var_init, scope);
    } else {
      for (auto func : mKernelFuncs) {
        void *func_body = m_ast->GetFuncBody(func);
        if (func_body != nullptr)
          replaceVariableRefs(var_init, func_body);
      }
    }
  }
  // check whether some variale references are not updated
  visited_vars.clear();
  for (auto var_init_scope : m_var_vec) {
    void *var_init = var_init_scope.first;
    void *scope = var_init_scope.second;
    if (visited_vars.count(var_init) > 0)
      continue;
    visited_vars.insert(var_init);
    if (m_ast->IsLocalInitName(var_init) ||
        m_ast->IsArgumentInitName(var_init)) {
      checkRemainingVariableRefs(var_init, scope);
    } else {
      for (auto func : mKernelFuncs) {
        void *func_body = m_ast->GetFuncBody(func);
        if (func_body != nullptr)
          checkRemainingVariableRefs(var_init, func_body);
      }
    }
  }

  //  remove unused struct arguments
  new_funcs.clear();
  for (size_t i = mKernelFuncs.size(); i > 0; --i) {
    void *func = mKernelFuncs[i - 1];
    //  remove unused struct variables (global/local)
    bool LocalChanged;
    do {
      LocalChanged = false;

      vector<void *> vec_refs;
      m_ast->GetNodesByType(nullptr, "preorder", "SgVarRefExp", &vec_refs);
      map<void *, vector<void *>> live_var_set;
      for (auto ref : vec_refs) {
        live_var_set[m_ast->GetVariableInitializedName(ref)].push_back(ref);
      }

      vector<void *> remained_vars(used_struct_vars.begin(),
                                   used_struct_vars.end());
      for (auto var : remained_vars) {
        if (live_var_set.count(var) > 0) {
          continue;
        }
        used_struct_vars.erase(var);
        void *var_decl = m_ast->GetVariableDecl(var);
        if ((var_decl == nullptr) || m_ast->isWithInHeaderFile(var_decl) ||
            (m_ast->is_floating_node(var_decl) != 0)) {
          continue;
        }
        LocalChanged = true;
        if (m_ast->IsGlobalInitName(var) != 0 &&
            m_ast->IsFromInputFile(var) != 0) {
          vector<void *> fp_list;
          void *dummy_global = m_ast->CreateFuncDecl(
              "void", "__merlin_dummy_" + m_ast->GetVariableName(var), fp_list,
              m_ast->GetGlobal(var_decl), false, nullptr);
          m_ast->SetStatic(dummy_global);
          m_ast->ReplaceStmt(var_decl, dummy_global);
        } else {
          m_ast->RemoveStmt(var_decl);
        }
      }
    } while (LocalChanged && !used_struct_vars.empty());
    void *new_func = removeUnusedParameters(func);
    if (new_func != func) {
      mKernelFuncsSet.erase(func);
      mKernelFuncsSet.insert(new_func);
      if (mTopKernel.count(func) > 0) {
        mTopKernel.erase(func);
        mTopKernel.insert(new_func);
        mVar2ZeroDims[new_func].swap(mVar2ZeroDims[func]);
        mVar2ZeroDims.erase(func);
      }
      m_ast->RemoveStmt(func);
      Changed = 1;
    }
    new_funcs.push_back(new_func);
  }
  mKernelFuncs.clear();
  mKernelFuncs = new_funcs;
  return Changed;
}

void StructDecompose::update_pragma_with_token_map(void *func) {
  void *func_body = m_ast->GetFuncBody(func);
  if (func_body == nullptr) {
    return;
  }
  std::unordered_map<string, string> token_map;
  token_map.insert(m_global_token_map.begin(), m_global_token_map.end());
  token_map.insert(m_local_token_map.begin(), m_local_token_map.end());
  m_ast->ReplacePragmaTokens(func_body, token_map, " =,");
  m_local_token_map.clear();
}

void StructDecompose::check() {
  bool has_failed = fail() != 0;

  //  collect all global variables with struct type
  set<void *> global_vars;
  set<string> all_var_names;
  for (auto func : mKernelFuncs) {
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    vector<void *> var_refs;
    m_ast->GetNodesByType(func_body, "preorder", "SgVarRefExp", &var_refs);
    for (auto ref : var_refs) {
      void *var_init = m_ast->GetVariableInitializedName(ref);
      if (m_ast->IsGlobalInitName(var_init) == 0 ||
          m_ast->IsFromInputFile(var_init) == 0) {
        continue;
      }
      if (global_vars.count(var_init) > 0) {
        continue;
      }
      global_vars.insert(var_init);
      string var_name = m_ast->GetVariableName(var_init);
      all_var_names.insert(var_name);
      void *var_type = m_ast->GetTypebyVar(var_init);
      string type_info;
      if (!has_failed &&
          (containsUnsupportedType(var_type, &type_info, true) != 0)) {
        string port_info = "global port \'" + var_name + "\' " +
                           getUserCodeFileLocation(m_ast, var_init, true) +
                           " with unsupported type: \n" + type_info;
        dump_critical_message(DISAG_ERROR_2(port_info));
        mErrorOut = true;
      }
    }
  }
  bool has_generate_opencl_host_library_generation_disaled_tag = false;
  //  check each function
  for (size_t i = 0; i != mKernelFuncs.size(); ++i) {
    void *func = mKernelFuncs[i];
    void *func_body = m_ast->GetFuncBody(func);
    if (!has_failed) {
      //  check local variables
      vector<void *> var_decls;
      m_ast->GetNodesByType(func_body, "preorder", "SgVariableDeclaration",
                            &var_decls);
      for (auto var : var_decls) {
        void *var_init = m_ast->GetVariableInitializedName(var);
        vector<void *> refs;
        m_ast->get_ref_in_scope(var_init, func_body, &refs);
        if (refs.empty()) {
          continue;
        }
        string var_name = m_ast->GetVariableName(var_init);
        void *var_type = m_ast->GetTypebyVar(var_init);
        string type_info;
        if (containsUnsupportedType(var_type, &type_info, true) != 0) {
          string port_info = "local variable \'" + var_name + "\' " +
                             getUserCodeFileLocation(m_ast, var, true) +
                             " with unsupported type: \n" + type_info;
          dump_critical_message(DISAG_ERROR_2(port_info));
          mErrorOut = true;
        }
      }
      //  collect unsupported type cast
      vector<void *> vec_cast_expr;
      m_ast->GetNodesByType(func_body, "preorder", "SgCastExp", &vec_cast_expr);
      for (auto expr : vec_cast_expr) {
        void *expr_type = m_ast->GetTypeByExp(expr);
        string type_info;
        if (containsCompoundType(expr_type) != SCALAR_TYPE) {
          void *stmt = m_ast->TraceUpToStatement(expr);
          string expr_info = "statement \'" + m_ast->_up(stmt) + "\' " +
                             getUserCodeFileLocation(m_ast, stmt, true) +
                             " which contains unsupported type cast: \'" +
                             m_ast->_up(expr) + "\'";
          dump_critical_message(DISAG_ERROR_2(expr_info));
          mErrorOut = true;
        }
      }

      //  collect unsupported sizeof
      vector<void *> vec_sizeof_expr;
      m_ast->GetNodesByType(func_body, "preorder", "SgSizeOfOp",
                            &vec_sizeof_expr);
      for (auto expr : vec_sizeof_expr) {
        if (m_ast->get_sizeof_value(expr) != nullptr)
          continue;
        void *stmt = m_ast->TraceUpToStatement(expr);
        string expr_info = "statement \'" + m_ast->_up(stmt) + "\' " +
                           getUserCodeFileLocation(m_ast, stmt, true) +
                           " which contains unsupported size calculation: \'" +
                           m_ast->_up(expr) + "\'";
        dump_critical_message(DISAG_ERROR_2(expr_info));
        mErrorOut = true;
      }
    }

    if (mTopKernel.count(func) > 0 && mOpenCL_flow) {
      set<string> curr_all_var_names = all_var_names;
      //  check parameters
      int num_arg = m_ast->GetFuncParamNum(func);
      for (int j = 0; j != num_arg; ++j) {
        void *param = m_ast->GetFuncParam(func, j);
        void *param_type = m_ast->GetTypebyVar(param);
        string param_name = m_ast->GetVariableName(param);
        curr_all_var_names.insert(param_name);
        string type_info;
        if (!has_failed &&
            (containsUnsupportedType(param_type, &type_info, true) != 0)) {
          string port_info = "interface port \'" + param_name + "\' " +
                             getUserCodeFileLocation(m_ast, param, true) +
                             " with unsupported type: \n" + type_info;
          dump_critical_message(DISAG_ERROR_2(port_info));
          mErrorOut = true;
        }
      }
      for (auto var_info : mVar2ZeroDims[func]) {
        string var_prefix = var_info.first;
        void *sg_init = std::get<0>(var_info.second);
        string type_info = m_ast->GetStringByType(m_ast->GetTypebyVar(sg_init));
        vector<size_t> dims = std::get<1>(var_info.second);
        string var_prefix_legalized = m_ast->legalizeName(var_prefix);
        bool used = false;
        for (auto var : curr_all_var_names) {
          if (var.find(var_prefix_legalized) == 0) {
            used = true;
            break;
          }
        }

        if (!used) {
          continue;
        }

        string pragma_info =
            "#pragma ACCEL interface variable=" + var_prefix + " max_depth=";
        size_t i = 0;
        for (auto dim : dims) {
          if (0 == dim) {
            pragma_info += "?";
          }
          if (i < dims.size() - 1) {
            pragma_info += ",";
          }
          ++i;
        }
        string var_loc =
            "'" + var_prefix_legalized + "' " + std::get<2>(var_info.second);
        string msg = "Cannot determine some of the dimension sizes of the "
                     "interface variable " +
                     var_loc + " of type \'" + type_info +
                     "\'. Please specify the missing dimension sizes in the "
                     "following pragma " +
                     "and add it to your kernel:\n" + pragma_info;
        dump_critical_message(MDARR_WARNING_1(var_loc, type_info, pragma_info));
        if (!has_generate_opencl_host_library_generation_disaled_tag) {
          string cmd_generate_opencl_host_library_generation_disabled_tag =
              std::string("touch ") + OPENCL_HOST_GENERATION_DISABLED_TAG;
          system(
              cmd_generate_opencl_host_library_generation_disabled_tag.c_str());
          has_generate_opencl_host_library_generation_disaled_tag = true;
        }
      }
    }
  }
}

void *StructDecompose::decomposeFunction(void *func_decl, bool *is_dead) {
  //  1. create new functions with decomposed variables
  void *ret_type = m_ast->GetFuncReturnType(func_decl);
  string sFuncName = m_ast->GetFuncName(func_decl);
  bool has_struct = false;
  int num_arg = m_ast->GetFuncParamNum(func_decl);
  map<void *, void *> orgArg2newArg;
  // wrapper name, port_name, data_type
  vector<tuple<string, string, void *>> member2port;
  void *func_body = m_ast->GetFuncBody(func_decl);
  int total_num_arg = 0;
  for (int i = 0; i < num_arg; ++i) {
    void *arg = m_ast->GetFuncParam(func_decl, i);
    void *arg_type = m_ast->GetTypebyVar(arg);
    string var_name = m_ast->GetVariableName(arg);
    vector<void *> vec_refs;
    m_ast->get_ref_in_scope(arg, func_body, &vec_refs);
    string type_info;
    enum type_kind res = containsCompoundType(arg_type, &type_info);
    if (res != SUPPORTED_COMPOUND_TYPE) {
      if (res == UNSUPPORTED_COMPOUND_TYPE) {
        string port_info = "interface port \'" + m_ast->GetVariableName(arg) +
                           "\' " + getUserCodeFileLocation(m_ast, arg, true) +
                           " with unsupported type: \n" + type_info;
        if (mTopKernel.count(func_decl) > 0) {
          dump_critical_message(DISAG_ERROR_2(port_info));
          mErrorOut = true;
        }
      } else {
        void *new_arg = m_ast->CreateVariable(arg_type, var_name, arg);
        orgArg2newArg[arg] = new_arg;
      }
    } else {
      string diagnosis_info;
      int ret = canSafeDecompose(arg, vec_refs, &diagnosis_info);
      if (ret != 0) {
        vector<tuple<string, void *, bool, bool>> member_types;
        decomposeType(arg_type, &member_types, arg);
        vector<void *> vec_member;
        vector<int> vec_size;
        int member_size = 0;
        for (auto t : member_types) {
          if (!std::get<3>(t)) {
            vec_member.push_back(nullptr);
            continue;
          }
          string mem_name = var_name + "->" + std::get<0>(t);
          string port_name = m_ast->legalizeName(mem_name);
          m_ast->get_valid_local_name(func_decl, &port_name);
          add_token_map(mem_name, port_name, false /*non global*/);
          member2port.push_back(
              make_tuple(mem_name, port_name, std::get<1>(t)));

          void *member = m_ast->CreateVariable(std::get<1>(t), port_name, arg);
          vec_member.push_back(member);
          member_size++;
        }

        mVar2Members[arg] = vec_member;
        has_struct = true;
        total_num_arg += member_size;
      } else {
        string port_info = "interface port \'" + m_ast->GetVariableName(arg) +
                           "\' " + getUserCodeFileLocation(m_ast, arg, true) +
                           " with unsupported reference or initializer: \n" +
                           diagnosis_info;
        if (mTopKernel.count(func_decl) > 0) {
          dump_critical_message(DISAG_ERROR_1(port_info));
          mErrorOut = true;
        }
#if PROJDEBUG
        string var_info = "'" + m_ast->GetVariableName(arg) + "'";
        cerr << "cannot decompose argument " + var_info << endl;
#endif
        void *new_arg = m_ast->CreateVariable(arg_type, var_name, arg);
        orgArg2newArg[arg] = new_arg;
      }
    }
    total_num_arg++;
    member2port.push_back(make_tuple(var_name, var_name, arg_type));
  }
  if (mTopKernel.count(func_decl) > 0) {
    //  create wrapper pragma in the wrapper function
    vector<void *> vec_calls;
    std::reverse(member2port.begin(), member2port.end());
    m_ast->GetFuncCallsFromDecl(func_decl, nullptr, &vec_calls);
    vector<void *> wrapper_calls;
    for (auto call : vec_calls) {
      if (mTopKernel.count(m_ast->TraceUpToFuncDecl(call)) <= 0)
        wrapper_calls.push_back(call);
    }
    for (auto call : wrapper_calls) {
      void *scope = m_ast->TraceUpToBasicBlock(call);
      for (auto m2p : member2port) {
        string var_name = std::get<0>(m2p);
        string new_var_name =
            get_wrapper_variable_name(m_ast, var_name, func_decl);
        string wrapper_str = "ACCEL wrapper";
        void *wrapper_pragma = m_ast->CreatePragma(wrapper_str, scope);

        m_ast->PrependChild(scope, wrapper_pragma);
        mMarsIrV2.set_pragma_attribute(&wrapper_pragma, CMOST_variable,
                                       new_var_name);
        mMarsIrV2.set_pragma_attribute(&wrapper_pragma, "port",
                                       std::get<1>(m2p));
        mMarsIrV2.set_pragma_attribute(
            &wrapper_pragma, "data_type",
            m_ast->GetStringByType(
                m_ast->GetBaseType(std::get<2>(m2p), false)));
      }
    }
    vector<void *> vec_pragma;
    m_ast->GetNodesByType(func_decl, "preorder", "SgPragmaDeclaration",
                          &vec_pragma);
    for (auto pragma : vec_pragma) {
      if (!mMarsIrV2.get_pragma_attribute(pragma, "wrapper").empty()) {
        m_ast->RemoveStmt(pragma);
      }
    }
  }
  if (!has_struct) {
    for (auto &&[old_arg, new_arg] : orgArg2newArg) {
      m_ast->DeleteNode(new_arg);
    }
    return func_decl;
  }

  //  1. update its call sites

  vector<void *> all_calls;
  m_ast->GetNodesByType(nullptr, "postorder", "SgFunctionCallExp", &all_calls);

  vector<void *> vec_calls;
  map<void *, vector<void *>> calls_info;
  map<void *, bool> wrapper_call_map;
  for (auto func_call : all_calls) {
    void *curr_decl = m_ast->GetFuncDeclByCall(func_call);
    if (!m_ast->isSameFunction(curr_decl, func_decl)) {
      continue;
    }
    for (int i = 0; i != num_arg; ++i) {
      void *param = m_ast->GetFuncParam(func_decl, i);
      if (mVar2Members.count(param) > 0) {
        void *caller = m_ast->TraceUpToFuncDecl(func_call);
        if (mTopKernel.count(func_decl) <= 0 || mTopKernel.count(caller) > 0) {
          vector<void *> vec_members;
          void *arg = m_ast->GetFuncCallParam(func_call, i);
          if (decomposeExpression(arg, &vec_members) == NO_REPLACED_EXP)
            calls_info[arg] = vec_members;
          else
            mVar2Members.erase(param);
        }
      }
    }
    vec_calls.push_back(func_call);
  }
  vector<void *> fp_list;
  bool has_decomposed_ports = false;
  for (int i = 0; i != num_arg; ++i) {
    void *param = m_ast->GetFuncParam(func_decl, i);
    if (mVar2Members.count(param) > 0) {
      has_decomposed_ports = true;
      for (auto new_param : mVar2Members[param]) {
        if (new_param == nullptr)
          continue;
        fp_list.push_back(new_param);
      }
    } else {
      if (orgArg2newArg.count(param) <= 0) {
        void *new_param = m_ast->CreateVariable(
            m_ast->GetTypebyVar(param), m_ast->GetVariableName(param), param);
        orgArg2newArg[param] = new_param;
      }
      fp_list.push_back(orgArg2newArg[param]);
    }
  }
  if (!has_decomposed_ports)
    return func_decl;
  *is_dead = true;
  string newFuncName = sFuncName;
  void *new_func_decl =
      m_ast->CreateFuncDecl(ret_type, newFuncName, fp_list,
                            m_ast->GetGlobal(func_decl), true, func_decl);
  m_ast->InsertStmt(new_func_decl, func_decl);

  if (m_ast->IsStatic(func_decl)) {
    m_ast->SetStatic(new_func_decl);
  }
  void *clone_func_body = m_ast->CopyStmt(func_body);
  for (auto arg_map : orgArg2newArg) {
    m_ast->replace_variable_references_in_scope(arg_map.first, arg_map.second,
                                                clone_func_body);
  }

  m_ast->SetFuncBody(new_func_decl, clone_func_body);
  void *new_func_body = m_ast->GetFuncBody(new_func_decl);

  replaceLocalVariableReference(new_func_body, func_body);
  if (mTopKernel.count(func_decl) > 0) {
    vector<void *> vec_pragma;
    m_ast->GetNodesByType(new_func_body, "preorder", "SgPragmaDeclaration",
                          &vec_pragma);
    std::unordered_map<std::string, vector<pair<CAnalPragma, void *>>>
        pragma_info_map;
    for (auto pragma : vec_pragma) {
      string var_name = mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable);
      bool is_interface =
          !mMarsIrV2.get_pragma_attribute(pragma, CMOST_interface).empty();
      if (is_interface) {
        CAnalPragma one_ana_pragma(m_ast);
        bool errout;
        one_ana_pragma.PragmasFrontendProcessing(pragma, &errout, false);
        pragma_info_map[var_name].push_back(
            std::make_pair(one_ana_pragma, pragma));
      }
    }

    for (int i = 0; i < num_arg; ++i) {
      void *arg = m_ast->GetFuncParam(func_decl, i);
      void *arg_type = m_ast->GetTypebyVar(arg);
      if (containsCompoundType(arg_type) == SUPPORTED_COMPOUND_TYPE) {
        //  update interface pragma
        string var_name = m_ast->GetVariableName(arg);
        set<void *> unused_pragma;
        if (mVar2Members.count(arg) <= 0) {
          continue;
        }
        bool found_interface_pragma = false;
        vector<CAnalPragma> vec_ana_pragma;
        if (pragma_info_map.count(var_name) > 0) {
          found_interface_pragma = true;
          for (auto pragma_info : pragma_info_map[var_name]) {
            unused_pragma.insert(pragma_info.second);
            vec_ana_pragma.push_back(pragma_info.first);
          }
        }
        const vector<void *> &vec_members = mVar2Members[arg];
        vector<tuple<string, void *, bool, bool>> member_types;
        decomposeType(arg_type, &member_types, arg);
        int j = 0;
        for (auto mem : vec_members) {
          if (mem == nullptr) {
            j++;
            continue;
          }
          string field_name = var_name + "->" + std::get<0>(member_types[j]);
          int cont_flag = 0;
          vector<size_t> vec_dims =
              get_array_dimension(field_name, &cont_flag, arg, vec_pragma);
          ++j;
          vector<CAnalPragma> vec_curr_ana_pragma;
          vector<CAnalPragma> vec_parent_pragma = vec_ana_pragma;
          //  field_name pragma override the parent pragma
          bool found_field_interface_pragma = found_interface_pragma;
          if (pragma_info_map.count(field_name) > 0) {
            for (auto curr_pragma_info : pragma_info_map[field_name]) {
              unused_pragma.insert(curr_pragma_info.second);
              vec_curr_ana_pragma.push_back(curr_pragma_info.first);
            }
            found_field_interface_pragma = true;
          }
          vector<string> vec_fields = str_split(field_name, "->");
          string prefix = vec_fields[0];
          int i = 0;
          while (i + 1 < vec_fields.size()) {
            if (pragma_info_map.count(prefix) > 0) {
              for (auto pragma_info : pragma_info_map[prefix]) {
                vec_parent_pragma.push_back(pragma_info.first);
                unused_pragma.insert(pragma_info.second);
              }
              found_field_interface_pragma = true;
            }
            i++;
            prefix += "->" + vec_fields[i];
          }
          if (!found_field_interface_pragma) {
            continue;
          }
          //  merge attribute from its parent
          CAnalPragma curr_ana_pragma(m_ast);
          for (auto one_ana_pragma : vec_parent_pragma)
            for (auto attr : one_ana_pragma.get_attribute()) {
              curr_ana_pragma.add_attribute(attr.first, attr.second);
            }
          for (auto one_ana_pragma : vec_curr_ana_pragma) {
            for (auto attr : one_ana_pragma.get_attribute()) {
              curr_ana_pragma.add_attribute(attr.first, attr.second);
            }
          }
          curr_ana_pragma.set_pragma_type(CMOST_interface);
          void *new_pragma =
              curr_ana_pragma.update_pragma(nullptr, true, new_func_body);
          m_ast->PrependChild(new_func_body, new_pragma);
          mMarsIrV2.set_pragma_attribute(&new_pragma, CMOST_variable,
                                         m_ast->GetVariableName(mem));
          if (!vec_dims.empty()) {
            string max_depth_str = get_dim_str(vec_dims, 0);
            //  ZP: 20160928
            if (!max_depth_str.empty()) {
              mMarsIrV2.set_pragma_attribute(&new_pragma, "max_depth",
                                             max_depth_str);
            }
          }
          vector<string> vec_depth =
              get_array_depth(field_name, arg, vec_pragma);
          if (!vec_depth.empty()) {
            string depth_str = str_merge(',', vec_depth);
            mMarsIrV2.set_pragma_attribute(&new_pragma, "depth", depth_str);
          }
        }
        for (auto pragma : unused_pragma) {
          m_ast->RemoveStmt(pragma);
        }
      } else {
        //  check primitive type
        int cont_flag = 0;
        get_array_dimension(m_ast->GetVariableName(arg), &cont_flag, arg,
                            vec_pragma);
      }
    }
  }

  //  create new function calls
  for (auto func_call : vec_calls) {
    vector<void *> vec_args;
    for (int i = 0; i != num_arg; ++i) {
      void *param = m_ast->GetFuncParam(func_decl, i);
      void *param_type = m_ast->GetTypebyVar(param);
      void *arg = m_ast->GetFuncCallParam(func_call, i);
      if (mVar2Members.count(param) > 0) {
        void *caller = m_ast->TraceUpToFuncDecl(func_call);
        if (mTopKernel.count(func_decl) > 0 && mTopKernel.count(caller) <= 0) {
          void *scope = m_ast->TraceUpToBasicBlock(func_call);
          //  create dummy decomposed varaibles in the wrapper function
          const vector<void *> &vec_members = mVar2Members[param];
          for (auto mem : vec_members) {
            if (mem == nullptr)
              continue;
            void *type = m_ast->GetTypebyVar(mem);
            if (m_ast->IsReferenceType(type) != 0) {
              type = m_ast->GetElementType(type);
            }
            if (m_ast->IsArrayType(type) != 0) {
              if (m_ast->IsNullExp(isSgArrayType(static_cast<SgNode *>(type))
                                       ->get_index()) != 0) {
                type = m_ast->CreatePointerType(m_ast->GetElementType(type));
              }
            }
            void *dummy_var = m_ast->CreateVariableDecl(
                type, m_ast->GetVariableName(mem), nullptr, scope);
            m_ast->PrependChild(scope, dummy_var);
            vec_args.push_back(m_ast->CreateVariableRef(dummy_var));
          }
          continue;
        } else {
          if (calls_info.count(arg) > 0) {
            vector<tuple<string, void *, bool, bool>> vec_types;
            decomposeType(param_type, &vec_types, param);
            int i = 0;
            for (auto member : calls_info[arg]) {
              if (member != nullptr) {
                void *curr_param_type = std::get<1>(vec_types[i]);
                if ((m_ast->IsPointerType(curr_param_type) != 0) &&
                    (m_ast->IsCompatibleType(curr_param_type,
                                             m_ast->GetTypeByExp(member), arg,
                                             true) == 0)) {
                  vec_args.push_back(
                      m_ast->CreateCastExp(member, curr_param_type, arg));
                } else {
                  vec_args.push_back(member);
                }
              }
              ++i;
            }
            continue;
          }
        }
      }
      vec_args.push_back(m_ast->CopyExp(arg));
    }
    void *new_call = m_ast->CreateFuncCall(new_func_decl, vec_args, func_call);
    m_ast->ReplaceExp(func_call, new_call);
    if (mTopKernel.count(func_decl) <= 0) {
      setFuncDeclUserCodeScopeId(m_ast, new_func_decl, new_call);
    }
  }

  //  update original declaration with no body
  vector<void *> vec_decls = m_ast->GetAllFuncDecl(func_decl);
  for (auto decl : vec_decls) {
    if (decl == func_decl) {
      continue;
    }
    void *func_body = m_ast->GetFuncBody(decl);
    if (func_body != nullptr) {
      continue;
    }
    //  youxiang: 20160923 bug639: do not touch header file
    if (m_ast->isWithInHeaderFile(decl)) {
      string msg = "Kernel subfunction can not be declared in the header file, "
                   "if it contains struct parameters\n";
      msg += "    Function name  : " + m_ast->_up(decl) + "\n";
      msg +=
          "    File location  : " + getUserCodeFileLocation(m_ast, decl, true);
      string decl_info = m_ast->_up(decl);
      string file_info = getUserCodeFileLocation(m_ast, decl, true);

      //  dump_critical_message("INFTF", "ERROR", msg, 301, 0);
      dump_critical_message(INFTF_ERROR_1(decl_info, file_info), 0);
#if PROJDEBUG
      cout << "ERROR: " << msg << endl;
#endif
      mErrorOut = true;
      continue;
    }
    void *new_decl =
        m_ast->CloneFuncDecl(new_func_decl, m_ast->GetGlobal(func_decl), false);

    if (*is_dead) {
      m_ast->InsertStmt(new_decl, decl);
      m_ast->RemoveStmt(decl);
      //  m_ast->ReplaceStmt(decl, new_decl);
    } else {
      m_ast->InsertAfterStmt(new_decl, decl);
    }
  }
  return new_func_decl;
}

void *StructDecompose::removeUnusedParameters(void *func_decl) {
  if (!mOpenCL_flow && mTopKernel.count(func_decl) > 0) {
    // do not remove unused interface for top kernel if it is not opencl flow.
    // Otherwise, it may affect the vendor tool simulation
    return func_decl;
  }
  remove_dead_code(func_decl);

  //  1. create new functions with decomposed variables
  int num_arg = m_ast->GetFuncParamNum(func_decl);
  void *func_body = m_ast->GetFuncBody(func_decl);
  vector<void *> refs;
  SetVector<void *> used_vars;
  m_ast->GetNodesByType(func_body, "preorder", "SgVarRefExp", &refs);
  vector<void *> vec_pragma;
  m_ast->GetNodesByType(func_body, "preorder", "SgPragmaDeclaration",
                        &vec_pragma);
  std::unordered_map<string, vector<void *>> variable_pragma_map;
  for (auto pragma : vec_pragma) {
    CAnalPragma ana_pragma(m_ast);
    bool errout;
    ana_pragma.PragmasFrontendProcessing(pragma, &errout, false);
    SetVector<void *> one_used_vars;
    if (ana_pragma.is_interface()) {
      one_used_vars = m_ast->GetVariablesUsedByPragmaInScope(pragma, true);
      auto var_name = mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable);
      if (!var_name.empty())
        variable_pragma_map[var_name].push_back(pragma);
    } else {
      one_used_vars = m_ast->GetVariablesUsedByPragmaInScope(pragma, false);
    }
    used_vars.insert(one_used_vars.begin(), one_used_vars.end());
  }
  //  2. update its call sites
  vector<void *> all_calls;
  m_ast->GetNodesByType(nullptr, "postorder", "SgFunctionCallExp", &all_calls);

  vector<void *> vec_calls;
  for (auto func_call : all_calls) {
    void *curr_decl = m_ast->GetFuncDeclByCall(func_call);
    //  void *caller_decl = m_ast->TraceUpToFuncDecl(func_call);
    //  if (!isKernelFunction(caller_decl))
    //  continue;
    if (!m_ast->isSameFunction(curr_decl, func_decl)) {
      continue;
    }
    vec_calls.push_back(func_call);
  }

  set<void *> unused_pragma;
  for (auto ref : refs) {
    used_vars.insert(m_ast->GetVariableInitializedName(ref));
  }
  map<void *, void *> orgArg2newArg;
  void *ret_type = m_ast->GetFuncReturnType(func_decl);
  string sFuncName = m_ast->GetFuncName(func_decl);
  vector<void *> fp_list;
  bool has_dead_arg = false;
  size_t total_num_arg = 0;
  for (int i = 0; i < num_arg; ++i) {
    bool has_side_effect = false;
    for (auto func_call : vec_calls) {
      has_side_effect |=
          m_ast->has_side_effect(m_ast->GetFuncCallParam(func_call, i));
      if (has_side_effect) {
        break;
      }
    }
    void *arg = m_ast->GetFuncParam(func_decl, i);
    void *arg_type = m_ast->GetTypebyVar(arg);
    string var_name = m_ast->GetVariableName(arg);
    if (used_vars.count(arg) <= 0 && !has_side_effect) {
      has_dead_arg = true;
      if (variable_pragma_map.count(var_name) > 0) {
        auto &vec_pragma = variable_pragma_map[var_name];
        unused_pragma.insert(vec_pragma.begin(), vec_pragma.end());
      }
    } else {
      used_vars.insert(arg);
      void *new_arg = m_ast->CreateVariable(arg_type, var_name, arg);
      orgArg2newArg[arg] = new_arg;
      fp_list.push_back(new_arg);
      total_num_arg++;
    }
  }
  if (mTopKernel.count(func_decl) > 0) {
    if (mIntel_flow && total_num_arg > ALTERA_ARGUMENTS_LIMITATION) {
      string limit_msg = my_itoa(ALTERA_ARGUMENTS_LIMITATION);
      string curr_args = "Current number of final arguments is " +
                         my_itoa(total_num_arg) + ".\n";
      dump_critical_message(DISAG_ERROR_3(limit_msg, curr_args));
      mErrorOut = true;
    }
  }

  if (!has_dead_arg) {
    for (auto &&[old_arg, new_arg] : orgArg2newArg) {
      m_ast->DeleteNode(new_arg);
    }
    return func_decl;
  }

  //  remove usused pragmas
  for (auto pragma : unused_pragma) {
    m_ast->RemoveStmt(pragma);
  }

  {
    //  check whether original declaration is in the header files
    vector<void *> vec_decls = m_ast->GetAllFuncDecl(func_decl);
    for (auto decl : vec_decls) {
      if (decl == func_decl) {
        continue;
      }
      if (!m_ast->isWithInHeaderFile(decl)) {
        continue;
      }
      //  has declaration is in the header files
      return func_decl;
    }
  }

  void *new_func_decl =
      m_ast->CreateFuncDecl(ret_type, sFuncName, fp_list,
                            m_ast->GetGlobal(func_decl), true, func_decl);
  m_ast->InsertStmt(new_func_decl, func_decl);
  if (m_ast->IsStatic(func_decl)) {
    m_ast->SetStatic(new_func_decl);
  }
  void *clone_func_body = m_ast->CopyStmt(func_body);
  for (auto arg_map : orgArg2newArg) {
    m_ast->replace_variable_references_in_scope(arg_map.first, arg_map.second,
                                                clone_func_body);
  }

  m_ast->SetFuncBody(new_func_decl, clone_func_body);
  void *new_func_body = m_ast->GetFuncBody(new_func_decl);
  replaceLocalVariableReference(new_func_body, func_body);

  //  2. update its call sites
  for (auto func_call : vec_calls) {
    vector<void *> vec_args;
    for (int i = 0; i != num_arg; ++i) {
      void *param = m_ast->GetFuncParam(func_decl, i);

      void *arg = m_ast->GetFuncCallParam(func_call, i);
      if (used_vars.count(param) <= 0) {
        continue;
      }
      vec_args.push_back(m_ast->CopyExp(arg));
    }
    void *new_func_call =
        m_ast->CreateFuncCall(new_func_decl, vec_args, func_call);
    m_ast->ReplaceExp(func_call, new_func_call);
    if (mTopKernel.count(func_decl) <= 0) {
      setFuncDeclUserCodeScopeId(m_ast, new_func_decl, new_func_call);
    }
  }

  //  update original declaration with no body
  vector<void *> vec_decls = m_ast->GetAllFuncDecl(func_decl);
  for (auto decl : vec_decls) {
    if (decl == func_decl) {
      continue;
    }
    void *func_body = m_ast->GetFuncBody(decl);
    if (func_body != nullptr) {
      continue;
    }
    void *new_decl =
        m_ast->CloneFuncDecl(new_func_decl, m_ast->GetGlobal(func_decl), false);
    m_ast->InsertStmt(new_decl, decl);
    m_ast->RemoveStmt(decl);
    //  m_ast->ReplaceStmt(decl, new_decl);
  }

  return new_func_decl;
}

int StructDecompose::decomposeParameter(void *var_init, void *scope) {
  if (mVar2Members.find(var_init) == mVar2Members.end()) {
    return 0;
  }

  // replaceVariableRefs(var_init, scope);
  replaceVariableRefsLater(var_init, scope);

  decomposePragma(var_init, scope);
  return 1;
}

int StructDecompose::decomposeLocalVariable(void *var_init, void *scope) {
  void *var_type = m_ast->GetTypebyVar(var_init);
  if (containsCompoundType(var_type) != SUPPORTED_COMPOUND_TYPE) {
    return 0;
  }
  void *var_decl = m_ast->GetVariableDecl(var_init);
  void *initializer = m_ast->GetInitializer(var_init);
  bool has_init = initializer != nullptr;
  vector<void *> vec_new_initializer;
  if (has_init) {
    decomposeInitializer(initializer, &vec_new_initializer);
  }
  string var_name = m_ast->GetVariableName(var_init);
  void *kernel_decl = m_ast->TraceUpToFuncDecl(var_decl);
  vector<tuple<string, void *, bool, bool>> vec_types;
  decomposeType(var_type, &vec_types, var_init);
  vector<void *> vec_member;

  for (size_t i = 0; i != vec_types.size(); ++i) {
    auto mem_info = vec_types[i];
    //  skip unused field
    if (!std::get<3>(mem_info)) {
      vec_member.push_back(nullptr);
      continue;
    }
    string mem_name = var_name + "->" + std::get<0>(mem_info);
    string new_mem_name = m_ast->legalizeName(mem_name);
    m_ast->get_valid_local_name(kernel_decl, &new_mem_name);
    add_token_map(mem_name, new_mem_name, false /*non global*/);
    void *initializer = has_init && i < vec_new_initializer.size()
                            ? vec_new_initializer[i]
                            : nullptr;
    void *mem_type = std::get<1>(mem_info);
    if (std::get<2>(mem_info)) {
      bool changed = false;
      void *new_mem_type = m_ast->RemoveConstType(mem_type, var_decl, &changed);
      if (changed && (new_mem_type != nullptr)) {
        mem_type = new_mem_type;
      }
    }
    void *mem_decl = m_ast->CreateVariableDecl(mem_type, new_mem_name,
                                               initializer, scope, var_decl);
    m_ast->InsertAfterStmt(mem_decl, var_decl);
    m_ast->SetInitializer(var_init, nullptr);
    if (m_ast->IsStatic(var_decl)) {
      m_ast->SetStatic(mem_decl);
    }
    vec_member.push_back(mem_decl);
  }
  mVar2Members[var_init] = vec_member;

  replaceVariableRefsLater(var_init, scope);
  // replaceVariableRefs(var_init, scope);

  decomposePragma(var_init, scope);
  return 1;
}

void StructDecompose::replaceVariableRefs(void *var_init, void *scope) {
  bool local_changed;
  do {
    local_changed = false;
    vector<void *> refs;
    m_ast->get_ref_in_scope(var_init, scope, &refs);
    if (refs.empty()) {
      break;
    }
    vector<void *> vec_unsupported_exp;
    for (size_t i = refs.size(); i > 0; i--) {
      void *ref = refs[i - 1];
      void *leaf_exp = nullptr;
      vector<void *> new_vec_exp;
      decomposed_info ret =
          decomposeVariableRef(ref, &leaf_exp, &new_vec_exp, false, true);
      if (REPLACED_EXP == ret) {
        local_changed = true;
        break;
      }
    }
  } while (local_changed);
}

void StructDecompose::checkRemainingVariableRefs(void *var_init, void *scope) {
  vector<void *> refs;
  m_ast->get_ref_in_scope(var_init, scope, &refs);
  if (refs.empty()) {
    return;
  }
  string diagnosis_info;
  for (auto ref : refs) {
    void *exp = nullptr;
    get_leaf_expression_from_var_ref(ref, &exp);
    if (exp == nullptr)
      exp = ref;
    diagnosis_info += "  \'" + m_ast->_p(exp) + "\' " +
                      getUserCodeFileLocation(m_ast, exp, true) + " in " +
                      m_ast->GetASTTypeString(m_ast->GetParent(exp)).substr(2) +
                      "\n";
  }
  string port_info = "Variable \'" + m_ast->GetVariableName(var_init) + "\' " +
                     getUserCodeFileLocation(m_ast, var_init, true);
  dump_critical_message(DISAG_ERROR_5(port_info, diagnosis_info));
  mErrorOut = true;
}

int StructDecompose::decomposeGlobalVariable(void *var_init, void *scope) {
  void *var_type = m_ast->GetTypebyVar(var_init);
  if (containsCompoundType(var_type) != SUPPORTED_COMPOUND_TYPE) {
    return 0;
  }
  void *var_decl = m_ast->GetVariableDecl(var_init);
  void *initializer = m_ast->GetInitializer(var_init);
  bool has_init = initializer != nullptr;
  vector<void *> vec_new_initializer;
  if (has_init) {
    decomposeInitializer(initializer, &vec_new_initializer);
  }
  string var_name = m_ast->GetVariableName(var_init);
  vector<tuple<string, void *, bool, bool>> vec_types;
  decomposeType(var_type, &vec_types, var_init);
  vector<void *> vec_member;
  void *global_scope = m_ast->GetGlobal(var_decl);
  for (size_t i = 0; i != vec_types.size(); ++i) {
    auto mem_info = vec_types[i];
    //  skip unused fields
    if (!std::get<3>(mem_info)) {
      vec_member.push_back(nullptr);
      continue;
    }
    string mem_name = var_name + "->" + std::get<0>(mem_info);
    string new_mem_name = m_ast->legalizeName(mem_name);
    m_ast->get_valid_global_name(global_scope, &new_mem_name);
    add_token_map(mem_name, new_mem_name, true /*global*/);

    void *initializer = has_init && i < vec_new_initializer.size()
                            ? vec_new_initializer[i]
                            : nullptr;
    void *mem_type = std::get<1>(mem_info);
    if (std::get<2>(mem_info)) {
      bool changed = false;
      void *new_type = m_ast->RemoveConstType(mem_type, var_decl, &changed);
      if (changed && (new_type != nullptr)) {
        mem_type = new_type;
      }
    }
    void *mem_decl = m_ast->CreateVariableDecl(
        mem_type, new_mem_name, initializer, global_scope, var_decl);
    m_ast->InsertAfterStmt(mem_decl, var_decl);
    if (m_ast->IsStatic(var_decl)) {
      m_ast->SetStatic(mem_decl);
    }
    vec_member.push_back(mem_decl);
  }
  mVar2Members[var_init] = vec_member;

  replaceVariableRefsLater(var_init, scope);
  // replaceVariableRefs(var_init, scope);

  decomposePragma(var_init, scope);
  return 1;
}

void StructDecompose::replaceVariableRefsLater(void *var_init, void *scope) {
  m_var_vec.push_back(std::make_pair(var_init, scope));
}

int StructDecompose::canSafeDecompose(void *var_init,
                                      const vector<void *> &refs,
                                      string *diagnosis_info) {
  static unordered_map<void *, bool> m_param_check;
  if (m_ast->IsArgumentInitName(var_init)) {
    if (m_param_check.count(var_init) > 0) {
      return m_param_check[var_init];
    }
  }
  void *type = m_ast->GetTypebyVar(var_init);
  vector<tuple<string, void *, bool, bool>> sub_types;
  int can_decompose = decomposeType(type, &sub_types, var_init, diagnosis_info);
  if (can_decompose == 0) {
    m_param_check[var_init] = false;
    return 0;
  }
  int ret = 1;
  for (auto ref : refs) {
    void *exp = ref;
    while (true) {
      void *parent = m_ast->GetParent(exp);
      if ((m_ast->IsPointerDerefExp(parent) != 0) ||
          (m_ast->IsPntrArrRefExp(parent) != 0) ||
          (m_ast->IsDotExp(parent) != 0) || (m_ast->IsArrowExp(parent) != 0) ||
          (m_ast->IsAddressOfOp(parent) != 0) ||
          (m_ast->IsAddOp(parent) != 0) || (m_ast->IsSubtractOp(parent) != 0)) {
        exp = parent;
        if ((m_ast->IsDotExp(parent) != 0) ||
            (m_ast->IsArrowExp(parent) != 0)) {
          //  FIXME: cannot support member function call
          //  member function call should be transferred into ordinary function
          //  call before decomposing
          void *rhs = m_ast->GetExpRightOperand(parent);
          if (m_ast->IsVarRefExp(rhs) == 0) {
            cout << "only support access on field variables but found "
                    "unsupported "
                 << "expression on the struct variables "
                 << (static_cast<SgNode *>(rhs))->class_name() << endl;
#if PROJDEBUG
            cout << m_ast->_up(parent) << endl;
#endif
            *diagnosis_info += "non-field access : " + m_ast->_up(exp) + " " +
                               getUserCodeFileLocation(m_ast, exp, true) + "\n";
            m_param_check[var_init] = false;
            return 0;
          }
        }
        void *sg_type = m_ast->GetTypeByExp(parent);
        if (containsCompoundType(sg_type) != SUPPORTED_COMPOUND_TYPE) {
          break;
        }
      } else {
        break;
      }
    }
    void *sg_type = m_ast->GetTypeByExp(exp);
    if (containsCompoundType(sg_type) != SUPPORTED_COMPOUND_TYPE) {
      continue;
    }
    int index = m_ast->GetFuncCallParamIndex(exp);
    if (index == -1) {
      void *parent = m_ast->GetParent(exp);
      if ((m_ast->IsPointerType(sg_type) != 0) ||
          (m_ast->IsArrayType(sg_type) != 0)) {
        if ((m_ast->IsAssignOp(parent) != 0) ||       //  p = q;
            (m_ast->IsPlusPlusOp(parent) != 0) ||     //  p++;
            (m_ast->IsMinusMinusOp(parent) != 0) ||   //  q--;
            (m_ast->IsPlusAssignOp(parent) != 0) ||   //  p += 2;
            (m_ast->IsMinusAssignOp(parent) != 0) ||  //  p -= 2;
            (m_ast->IsCompareOp(parent) != 0) ||      //  p == q;
            is_pointer_test(exp) ||
            (m_ast->IsExprList(parent) != 0) ||           //  p, q;
            (m_ast->IsAssignInitializer(parent) != 0)) {  //  st *p = q + 2;
          continue;
        }
        if (m_ast->IsCastExp(parent) != 0 &&
            m_ast->IsCompatibleType(sg_type, m_ast->GetTypeByExp(parent),
                                    parent, false) != 0)
          continue;
        cout << "found unsupported expression on the struct variables "
             << (static_cast<SgNode *>(parent))->class_name() << endl;
#if PROJDEBUG
        cout << m_ast->_up(parent) << endl;
#endif
        *diagnosis_info += "unsupported expression : " + m_ast->_up(parent) +
                           +" " + getUserCodeFileLocation(m_ast, parent, true) +
                           "\n";
      } else {
        if (m_ast->IsAssignInitializer(parent) != 0) {
          void *var_init = m_ast->GetParent(parent);
          if ((m_ast->IsInitName(var_init) != 0) &&
              (m_ast->IsReferenceType(m_ast->GetTypebyVar(var_init)) != 0)) {
            continue;
          }
        }
        if ((m_ast->IsCommaExp(parent) != 0) ||
            (m_ast->IsExprStatement(parent) != 0) ||
            (m_ast->IsExprList(parent) != 0)) {
          continue;
        }
        cout << "found unsupported expression on the struct variables "
             << (static_cast<SgNode *>(exp))->class_name() << endl;
#if PROJDEBUG
        cout << m_ast->_up(exp) << endl;
#endif
        cout << "Parent: " << (static_cast<SgNode *>(parent))->class_name()
             << endl;
#if PROJDEBUG
        cout << m_ast->_up(parent) << endl;
#endif
        *diagnosis_info += "unsupported expression : " + m_ast->_up(exp) +
                           +" " + getUserCodeFileLocation(m_ast, exp, true) +
                           "\n";
      }
      ret = 0;
    } else {
      void *func_call = m_ast->TraceUpToFuncCall(exp);
      assert(func_call);
      void *func_decl = m_ast->GetFuncDeclByCall(func_call);
      if (func_decl != nullptr) {
        void *func_body = m_ast->GetFuncBody(func_decl);
        void *param = m_ast->GetFuncParam(func_decl, index);
        vector<void *> refs;
        m_ast->get_ref_in_scope(param, func_body, &refs);
        if (canSafeDecompose(param, refs, diagnosis_info) == 0) {
          ret = 0;
        }
      } else {
        *diagnosis_info += "unsupported call expression : " + m_ast->_up(exp) +
                           +" " + getUserCodeFileLocation(m_ast, exp, true) +
                           "\n";
        ret = 0;
      }
    }
  }
  m_param_check[var_init] = ret;
  return ret;
}

//  tuple<string, void *, bool, bool>
//  field_name, filed_type, is_mutable, is_used
int StructDecompose::decomposeType(
    void *sg_orig_type, vector<tuple<string, void *, bool, bool>> *res,
    void *pos, string *diagnosis_info, void *sg_modifier_type) {
  void *sg_type = m_ast->GetOrigTypeByTypedef(sg_orig_type, true);
  if ((m_ast->IsScalarType(sg_type) != 0) ||
      (cannotDecomposeType(sg_type) && (sg_modifier_type == nullptr))) {
    // MER1587: for anonymous enum, use its original typedef type
    if (sg_modifier_type != nullptr) {
      sg_type = m_ast->CreateModifierType(sg_orig_type, sg_modifier_type);
    } else {
      sg_type = sg_orig_type;
    }
    res->push_back(make_tuple(string(""), sg_type, false, true));
    return 1;
  }
  if (sg_modifier_type == nullptr &&
      m_cache_type_decomposed.count(sg_type) > 0) {
    auto &info = m_cache_type_decomposed[sg_type];
    *res = info.res;
    *diagnosis_info = info.message;
    return info.valid;
  }

  int ret = 1;
  if (m_ast->IsPointerType(sg_type) != 0) {
    void *base_type = m_ast->GetElementType(sg_type);
    vector<tuple<string, void *, bool, bool>> sub_res;
    ret = decomposeType(base_type, &sub_res, pos, diagnosis_info,
                        sg_modifier_type);
    for (auto p : sub_res) {
      void *new_type = m_ast->CreatePointerType(std::get<1>(p));
      res->push_back(
          make_tuple(std::get<0>(p), new_type, std::get<2>(p), std::get<3>(p)));
    }
  } else if (m_ast->IsReferenceType(sg_type) != 0) {
    void *base_type = m_ast->GetElementType(sg_type);
    vector<tuple<string, void *, bool, bool>> sub_res;
    ret = decomposeType(base_type, &sub_res, pos, diagnosis_info,
                        sg_modifier_type);
    for (auto p : sub_res) {
      void *new_type = m_ast->CreateReferenceType(std::get<1>(p));
      res->push_back(
          make_tuple(std::get<0>(p), new_type, std::get<2>(p), std::get<3>(p)));
    }
  } else if (m_ast->IsModifierType(sg_type) != 0) {
    void *base_type = m_ast->GetElementType(sg_type);
    bool is_pointer = m_ast->IsPointerType(base_type) != 0;
    vector<tuple<string, void *, bool, bool>> sub_res;
    ret = decomposeType(base_type, &sub_res, pos, diagnosis_info,
                        is_pointer ? nullptr : sg_type);
    for (auto p : sub_res) {
      if (is_pointer) {
        res->push_back(make_tuple(
            std::get<0>(p), m_ast->CreateModifierType(std::get<1>(p), sg_type),
            std::get<2>(p), std::get<3>(p)));
      } else {
        res->push_back(p);
      }
    }
  } else if (m_ast->IsArrayType(sg_type) != 0) {
    void *base_type = m_ast->GetElementType(sg_type);
    vector<size_t> dims;
    m_ast->get_type_dimension_new(sg_type, &base_type, &dims, pos,
                                  /*stop pointer*/ true);
    vector<tuple<string, void *, bool, bool>> sub_res;
    ret = decomposeType(base_type, &sub_res, pos, diagnosis_info,
                        sg_modifier_type);
    for (auto p : sub_res) {
      void *new_type = m_ast->CreateArrayType(std::get<1>(p), dims);
      res->push_back(
          make_tuple(std::get<0>(p), new_type, std::get<2>(p), std::get<3>(p)));
    }
  } else if (((m_ast->IsStructureType(sg_type) != 0) ||
              (m_ast->IsClassType(sg_type) != 0))) {
    vector<void *> data_members;
    m_ast->GetClassDataMembers(sg_type, &data_members);
    int idx = 0;
    for (auto m : data_members) {
      idx++;
      //  not touch static members
      if (m_ast->IsStatic(m)) {
        continue;
      }
      bool used = contain_used_or_non_pointer_field(m);
      void *var_init = m_ast->GetVariableInitializedName(m);
      void *var_type = m_ast->GetTypebyVar(var_init);
      string var_name = m_ast->GetVariableName(var_init);
      if (m_ast->IsAnonymousName(var_name) != 0) {
        var_name = "_anon_" + my_itoa(idx);
      }
      vector<tuple<string, void *, bool, bool>> sub_res;
      ret &= decomposeType(var_type, &sub_res, var_init, diagnosis_info,
                           sg_modifier_type);
      for (auto p : sub_res) {
        string m_name = std::get<0>(p);
        if (m_name.empty()) {
          m_name = var_name;
        } else {
          m_name = var_name + "->" + m_name;
        }
        res->push_back(make_tuple(m_name, std::get<1>(p),
                                  static_cast<int>(m_ast->IsMutable(m)) |
                                      static_cast<int>(std::get<2>(p)),
                                  static_cast<int>(used) &
                                      static_cast<int>(std::get<3>(p))));
      }
    }
  } else {
    cout << "found unsupported type : " << m_ast->_up(sg_type) << endl;
    *diagnosis_info += "cannot support type: " + m_ast->_up(sg_type) + "\n";
    ret = 0;
  }
  if (sg_modifier_type == nullptr) {
    DecomposedTypeInfo info;
    info.res = *res;
    info.message = *diagnosis_info;
    info.valid = ret;
    m_cache_type_decomposed[sg_type] = info;
  }
  return ret;
}

void StructDecompose::getClassDataMembers(void *sg_type_, vector<void *> *res) {
  void *sg_type = m_ast->GetOrigTypeByTypedef(sg_type_, true);

  if ((m_ast->IsScalarType(sg_type) != 0) || cannotDecomposeType(sg_type)) {
    return;
  }

  if ((m_ast->IsPointerType(sg_type) != 0) ||
      (m_ast->IsReferenceType(sg_type) != 0) ||
      (m_ast->IsModifierType(sg_type) != 0) ||
      (m_ast->IsArrayType(sg_type) != 0)) {
    void *base_type = m_ast->GetElementType(sg_type);
    getClassDataMembers(base_type, res);
    return;
  }

  if ((m_ast->IsStructureType(sg_type) != 0) ||
      (m_ast->IsClassType(sg_type) != 0)) {
    m_ast->GetClassDataMembers(sg_type, res);
    return;
  }
}
decomposed_info StructDecompose::decomposeExpression(void *leaf_exp,
                                                     vector<void *> *vec_exp) {
  set<void *> visited;
  return decomposeExpression(leaf_exp, vec_exp, &visited);
}

decomposed_info StructDecompose::decomposeExpression(void *leaf_exp,
                                                     vector<void *> *vec_exp,
                                                     set<void *> *visited) {
  if (visited->count(leaf_exp) > 0) {
    return NO_REPLACED_EXP;
  }
  visited->insert(leaf_exp);
  if (m_ast->IsCompoundLiteralExp(leaf_exp) != 0) {
    string diagnosis_msg;
    if (void *agg_init =
            m_ast->GetAggrInitializerFromCompoundLiteralExp(leaf_exp)) {
      auto ret = decomposeInitializer(agg_init, vec_exp, false, &diagnosis_msg,
                                      visited);
      return ret;
    }
  }

  if (m_ast->IsInitializer(leaf_exp) != 0) {
    string diagnosis_msg;
    return decomposeInitializer(leaf_exp, vec_exp, false, &diagnosis_msg,
                                visited);
  }

  if (m_ast->IsCommaExp(leaf_exp) != 0) {
    vector<void *> vec_orig_exp;
    m_ast->GetExprListFromCommaOp(leaf_exp, &vec_orig_exp);
    assert(!vec_orig_exp.empty());
    // FIXME: why we only handle the last operand
    void *last_operand = vec_orig_exp.back();
    vector<void *> vec_sub_exp;
    auto ret = decomposeExpression(last_operand, &vec_sub_exp, visited);
    if (ret != 0) {
      void *new_comma = m_ast->CopyExp(vec_orig_exp[0]);
      for (int i = 1; i < static_cast<int>(vec_orig_exp.size()) - 1; ++i) {
        new_comma =
            m_ast->CreateExp(V_SgCommaOpExp, new_comma,
                             m_ast->CopyExp(vec_orig_exp[i]), 0, leaf_exp);
      }
      assert(!vec_sub_exp.empty());
      new_comma = m_ast->CreateExp(V_SgCommaOpExp, new_comma, vec_sub_exp[0], 0,
                                   leaf_exp);
      vec_exp->push_back(new_comma);
      for (size_t i = 1; i < vec_sub_exp.size(); ++i) {
        vec_exp->push_back(vec_sub_exp[i]);
      }
    }
    return ret;
  }

  if (m_ast->IsNullPointerCast(leaf_exp) != 0) {
    vector<tuple<string, void *, bool, bool>> vec_types;
    string diagnosis;
    int ret = decomposeType(m_ast->GetTypeByExp(leaf_exp), &vec_types, leaf_exp,
                            &diagnosis);
    for (auto name_ty : vec_types) {
      if (!std::get<3>(name_ty)) {
        vec_exp->push_back(nullptr);
        continue;
      }
      vec_exp->push_back(m_ast->CreateCastExp(m_ast->CreateConst(0),
                                              std::get<1>(name_ty), leaf_exp));
    }
    return ret != 0 ? NO_REPLACED_EXP : UNSUPPORTED_REPLACED_EXP;
  }

  void *var_ref = leaf_exp;
  while ((var_ref != nullptr) && (m_ast->IsVarRefExp(var_ref) == 0)) {
    if (m_ast->IsAddOp(var_ref) != 0) {
      void *lhs, *rhs;
      lhs = rhs = nullptr;
      m_ast->GetExpOperand(var_ref, &lhs, &rhs);
      void *lhs_type = m_ast->GetTypeByExp(lhs);
      if (m_ast->IsPointerType(lhs_type) != 0 ||
          m_ast->IsArrayType(lhs_type) != 0)
        var_ref = lhs;
      else
        var_ref = rhs;
    } else {
      var_ref = m_ast->GetExpLeftOperand(var_ref);
    }
  }

  if (m_ast->IsVarRefExp(var_ref) != 0) {
    void *leaf_exp_res = nullptr;
    auto ret = decomposeVariableRef(var_ref, &leaf_exp_res, vec_exp, true,
                                    false, visited);
    if (ret == NO_REPLACED_EXP && leaf_exp_res == leaf_exp)
      return ret;
    if (ret != NO_REPLACED_EXP)
      return ret;
  }

  return UNSUPPORTED_REPLACED_EXP;
}

int StructDecompose::get_leaf_expression_from_var_ref(void *var_ref,
                                                      void **leaf_exp) {
  void *exp = var_ref;
  void *type = m_ast->GetTypeByExp(var_ref);

  void *curr_type = type;
  void *curr_member = nullptr;
  int offset = 0;
  bool anonymous_data_member = false;
  while (true) {
    if (!anonymous_data_member && containsCompoundType(m_ast->GetTypeByExp(
                                      exp)) != SUPPORTED_COMPOUND_TYPE) {
      break;
    }
    anonymous_data_member = false;
    void *parent = m_ast->GetParent(exp);
    bool pointer_def_exp = m_ast->IsPointerDerefExp(parent) != 0;
    bool pntr_exp = (m_ast->IsPntrArrRefExp(parent) != 0) &&
                    m_ast->GetExpLeftOperand(parent) == exp;
    bool dot_exp = m_ast->IsDotExp(parent) != 0;
    bool arrow_exp = m_ast->IsArrowExp(parent) != 0;
    bool address_exp = m_ast->IsAddressOfOp(parent) != 0;
    bool add_op = m_ast->IsAddOp(parent) != 0;
    bool sub_op = m_ast->IsSubtractOp(parent) != 0;
    bool cast_op = m_ast->IsCastExp(parent) != 0;
    if (pointer_def_exp || pntr_exp || dot_exp || arrow_exp || address_exp ||
        add_op || sub_op || cast_op) {
      if (dot_exp || arrow_exp) {
        void *data_exp = m_ast->GetExpRightOperand(parent);
        if (m_ast->IsVarRefExp(data_exp)) {
          anonymous_data_member =
              m_ast->IsAnonymousName(m_ast->GetVariableName(data_exp));
        }
        if (arrow_exp) {
          curr_type = m_ast->GetElementType(curr_type);
        }
        vector<void *> data_members;
        getClassDataMembers(curr_type, &data_members);
        void *member_exp = m_ast->GetExpRightOperand(parent);
        void *member_ref = nullptr;
        m_ast->parse_array_ref_from_pntr(member_exp, &member_ref);
        assert(member_ref);
        curr_member = m_ast->GetVariableInitializedName(member_ref);
        if (m_ast->IsAnonymousName(m_ast->GetVariableName(curr_member))) {
          exp = parent;
          continue;
        }
        int hit = 0;
        for (auto member_decl : data_members) {
          if (m_ast->IsStatic(member_decl)) {
            continue;
          }
          void *member = m_ast->GetVariableInitializedName(member_decl);
          void *member_type = m_ast->GetTypebyVar(member);
          if (m_ast->GetVariableName(curr_member) ==
              m_ast->GetVariableName(member)) {
            curr_type = member_type;
            hit = 1;
            break;
          }
          vector<tuple<string, void *, bool, bool>> sub_res;
          decomposeType(member_type, &sub_res, member);
          offset += sub_res.size();
        }
        assert(hit);
      } else if (pointer_def_exp || pntr_exp) {
        curr_type = m_ast->GetElementType(curr_type);
      } else if (address_exp) {
        if (m_ast->IsReferenceType(curr_type) != 0) {
          curr_type = m_ast->GetElementType(curr_type);
        }
        curr_type = m_ast->CreatePointerType(curr_type);
      } else if (add_op || sub_op || cast_op) {
        if ((m_ast->IsPointerType(curr_type) == 0) &&
            (m_ast->IsArrayType(curr_type) == 0)) {
          break;
        }
        //  do nothing
      }
      exp = parent;
    } else {
      break;
    }
  }
  *leaf_exp = exp;
  return offset;
}

decomposed_info
StructDecompose::decomposeVariableRef(void *var_ref, void **leaf_exp,
                                      vector<void *> *vec_new_exp,
                                      bool generate_new_expr, bool replace) {
  set<void *> visited;
  return decomposeVariableRef(var_ref, leaf_exp, vec_new_exp, generate_new_expr,
                              replace, &visited);
}

//  0: fail to decompose
//  1: decompose and replace
//  2: decompose but not replaced
decomposed_info StructDecompose::decomposeVariableRef(
    void *var_ref, void **leaf_exp, vector<void *> *vec_new_exp,
    bool generate_new_expr, bool replace, set<void *> *visited) {
  void *exp = var_ref;
  assert(m_ast->IsVarRefExp(var_ref));
  void *var_init = m_ast->GetVariableInitializedName(var_ref);

  if (mVar2Members.count(var_init) <= 0) {
    return UNSUPPORTED_REPLACED_EXP;
  }
  const vector<void *> &vec_member = mVar2Members[var_init];

  int offset = get_leaf_expression_from_var_ref(var_ref, leaf_exp);
  if (*leaf_exp == nullptr) {
    return UNSUPPORTED_REPLACED_EXP;
  }
  if (replace && (m_ast->has_side_effect(*leaf_exp) != 0)) {
    split_expression_with_side_effect(*leaf_exp);
  }
  void *exp_type = m_ast->GetTypeByExp(*leaf_exp);

  vector<tuple<string, void *, bool, bool>> sub_vec_types;
  decomposeType(exp_type, &sub_vec_types, var_ref);
  int num_sub_member = sub_vec_types.size();
  if (!generate_new_expr) {
    if (num_sub_member == 1) {
      //  youxiang 20161028 delay to replace argument with compound type
      if (m_ast->GetFuncCallParamIndex(*leaf_exp) != -1) {
        if (containsCompoundType(exp_type) == SUPPORTED_COMPOUND_TYPE) {
          return NO_REPLACED_EXP;
        }
      }
    } else {
      if (m_ast->GetFuncCallParamIndex(*leaf_exp) != -1) {
        return NO_REPLACED_EXP;
      }
      void *parent = m_ast->GetParent(*leaf_exp);
      //  int *p = p;
      if (m_ast->IsAssignInitializer(parent) != 0) {
        //  delay to variable declaration
        return NO_REPLACED_EXP;
      }
    }
  }

  for (int i = 0; i != num_sub_member; ++i) {
    void *mem = vec_member[offset + i];
    if (mem == nullptr)
      vec_new_exp->push_back(nullptr);
    else
      vec_new_exp->push_back(m_ast->CreateVariableRef(mem, var_ref));
  }
  exp = var_ref;
  while (true) {
    void *curr_type =
        m_ast->GetOrigTypeByTypedef(m_ast->GetTypeByExp(exp), true);
    if (containsCompoundType(curr_type) != SUPPORTED_COMPOUND_TYPE) {
      break;
    }
    bool is_pointer = (m_ast->IsPointerType(curr_type) != 0) ||
                      (m_ast->IsArrayType(curr_type) != 0);
    void *parent = m_ast->GetParent(exp);
    bool pointer_def_exp = m_ast->IsPointerDerefExp(parent) != 0;
    bool pntr_exp = (m_ast->IsPntrArrRefExp(parent) != 0) &&
                    m_ast->GetExpLeftOperand(parent) == exp;
    bool dot_exp = m_ast->IsDotExp(parent) != 0;
    bool arrow_exp = m_ast->IsArrowExp(parent) != 0;
    bool address_exp = m_ast->IsAddressOfOp(parent) != 0;
    bool add_op = (m_ast->IsAddOp(parent) != 0) && is_pointer;
    bool sub_op = (m_ast->IsSubtractOp(parent) != 0) && is_pointer;
    bool cast_op = (m_ast->IsCastExp(parent) != 0) && is_pointer;
    if (pointer_def_exp || pntr_exp || dot_exp || arrow_exp || address_exp ||
        add_op || sub_op || cast_op) {
      if (pointer_def_exp || arrow_exp) {
        for (int i = 0; i != vec_new_exp->size(); ++i) {
          if ((*vec_new_exp)[i] == nullptr)
            continue;
          (*vec_new_exp)[i] = m_ast->CreateExp(
              V_SgPointerDerefExp, (*vec_new_exp)[i], nullptr, 0, parent);
        }
      }
      if (address_exp) {
        for (int i = 0; i != vec_new_exp->size(); ++i) {
          if ((*vec_new_exp)[i] == nullptr)
            continue;
          (*vec_new_exp)[i] = m_ast->CreateExp(
              V_SgAddressOfOp, (*vec_new_exp)[i], nullptr, 0, parent);
        }
      }
      if (pntr_exp) {
        void *index_exp = m_ast->GetExpRightOperand(parent);
        for (int i = 0; i != vec_new_exp->size(); ++i) {
          if ((*vec_new_exp)[i] == nullptr)
            continue;
          (*vec_new_exp)[i] =
              m_ast->CreateExp(V_SgPntrArrRefExp, (*vec_new_exp)[i],
                               m_ast->CopyExp(index_exp), 0, parent);
        }
      }
      if (dot_exp || arrow_exp) {
        void *member_exp = m_ast->GetExpRightOperand(parent);
        vector<void *> indices;
        void *array;
        m_ast->parse_pntr_ref_new(member_exp, &array, &indices);
        std::reverse(indices.begin(), indices.end());
        for (auto index : indices) {
          for (int i = 0; i != vec_new_exp->size(); ++i) {
            if ((*vec_new_exp)[i] == nullptr)
              continue;
            (*vec_new_exp)[i] =
                m_ast->CreateExp(V_SgPntrArrRefExp, (*vec_new_exp)[i],
                                 m_ast->CopyExp(index), 0, parent);
          }
        }
      }

      if (add_op || sub_op) {
        void *sg_parent_type = m_ast->GetTypeByExp(parent);
        if ((m_ast->IsPointerType(sg_parent_type) == 0) &&
            (m_ast->IsArrayType(sg_parent_type) == 0)) {
          break;
        }
        void *lhs;
        void *rhs;
        m_ast->GetExpOperand(parent, &lhs, &rhs);
        void *offset = rhs;
        void *sg_type = m_ast->GetTypeByExp(lhs);
        if (add_op && (m_ast->IsPointerType(sg_type) == 0) &&
            (m_ast->IsArrayType(sg_type) == 0)) {
          offset = lhs;
        }

        for (int i = 0; i != vec_new_exp->size(); ++i) {
          if ((*vec_new_exp)[i] == nullptr)
            continue;
          (*vec_new_exp)[i] = m_ast->CreateExp(
              add_op ? V_SgAddOp : V_SgSubtractOp, (*vec_new_exp)[i],
              m_ast->CopyExp(offset), 0, parent);
        }
      }
      if (cast_op) {
        void *cast_type = m_ast->GetTypeByExp(parent);
        vector<tuple<string, void *, bool, bool>> sub_types;
        if (decomposeType(cast_type, &sub_types, parent) == 0) {
          return UNSUPPORTED_REPLACED_EXP;
        }
        void *dummy_leaf_exp;
        size_t idx_type =
            get_leaf_expression_from_var_ref(exp, &dummy_leaf_exp);
        for (size_t i = 0; i != vec_new_exp->size(); ++i, ++idx_type) {
          if ((*vec_new_exp)[i] == nullptr)
            continue;
          (*vec_new_exp)[i] = m_ast->CreateCastExp(
              (*vec_new_exp)[i], std::get<1>(sub_types[idx_type]), parent);
        }
      }
      exp = parent;
    } else {
      break;
    }
  }
  *leaf_exp = exp;
  if (replace && vec_new_exp->size() == 1) {
    if ((*vec_new_exp)[0] != nullptr)
      m_ast->ReplaceExp(exp, (*vec_new_exp)[0]);
    return REPLACED_EXP;
  }
  void *sg_type = m_ast->GetTypeByExp(exp);
  void *parent = m_ast->GetParent(exp);
  if ((m_ast->IsPointerType(sg_type) != 0) ||
      (m_ast->IsArrayType(sg_type) != 0)) {
    //  pointer arithmetic
    //  p++, p--
    if (m_ast->IsPlusPlusOp(parent) != 0 ||
        m_ast->IsMinusMinusOp(parent) != 0) {
      *leaf_exp = parent;
      vector<void *> vec_new_op;
      for (auto new_exp : *vec_new_exp) {
        if (new_exp == nullptr) {
          if (!replace)
            vec_new_op.push_back(nullptr);
          continue;
        }
        void *copy_parent = m_ast->CopyExp(parent);
        m_ast->ReplaceExp(m_ast->GetExpUniOperand(copy_parent), new_exp);
        vec_new_op.push_back(copy_parent);
      }
      if (!replace) {
        vec_new_exp->swap(vec_new_op);
        return NO_REPLACED_EXP;
      }
      void *new_expr_list = m_ast->CreateExpList(vec_new_op);
      m_ast->ReplaceExp(parent, new_expr_list);
      return REPLACED_EXP;
    }
    //  p+= 2
    if (m_ast->IsPlusAssignOp(parent) != 0) {
      *leaf_exp = parent;
      void *rhs = m_ast->GetExpRightOperand(parent);
      vector<void *> vec_new_op;
      for (auto new_exp : *vec_new_exp) {
        if (new_exp == nullptr) {
          if (!replace)
            vec_new_op.push_back(nullptr);
          continue;
        }
        vec_new_op.push_back(m_ast->CreateExp(V_SgPlusAssignOp, new_exp,
                                              m_ast->CopyExp(rhs), 0, parent));
      }
      if (!replace) {
        vec_new_exp->swap(vec_new_op);
        return NO_REPLACED_EXP;
      }
      void *new_expr_list = m_ast->CreateExpList(vec_new_op);
      m_ast->ReplaceExp(parent, new_expr_list);
      return REPLACED_EXP;
    }

    //  p -= 2;
    if (m_ast->IsMinusAssignOp(parent) != 0) {
      *leaf_exp = parent;
      void *rhs = m_ast->GetExpRightOperand(parent);
      vector<void *> vec_new_op;
      for (auto new_exp : *vec_new_exp) {
        if (new_exp == nullptr) {
          if (!replace)
            vec_new_op.push_back(nullptr);
          continue;
        }
        vec_new_op.push_back(m_ast->CreateExp(V_SgMinusAssignOp, new_exp,
                                              m_ast->CopyExp(rhs), 0, parent));
      }
      if (!replace) {
        vec_new_exp->swap(vec_new_op);
        return NO_REPLACED_EXP;
      }
      void *new_expr_list = m_ast->CreateExpList(vec_new_op);
      m_ast->ReplaceExp(parent, new_expr_list);
      return REPLACED_EXP;
    }

    //  p = q

    if (m_ast->IsAssignOp(parent) != 0) {
      void *lhs;
      void *rhs;
      m_ast->GetExpOperand(parent, &lhs, &rhs);
      vector<void *> vec_new_exp_2;
      if (lhs == exp) {
        auto ret = decomposeExpression(rhs, &vec_new_exp_2, visited);
        if (ret == UNSUPPORTED_REPLACED_EXP) {
          return UNSUPPORTED_REPLACED_EXP;
        }
      } else {
        //  delay to decompose because the other operand has not been
        //  decomposed
        return NO_REPLACED_EXP;
      }
      *leaf_exp = parent;
      assert(vec_new_exp_2.size() == vec_new_exp->size());
      vector<void *> vec_new_op;
      for (size_t i = 0; i != vec_new_exp->size(); ++i) {
        if ((*vec_new_exp)[i] == nullptr) {
          if (!replace)
            vec_new_op.push_back(nullptr);
          continue;
        }
        void *rhs = vec_new_exp_2[i];
        void *lhs = (*vec_new_exp)[i];
        vec_new_op.push_back(
            m_ast->CreateExp(V_SgAssignOp, lhs, rhs, 0, parent));
      }
      if (!replace) {
        vec_new_exp->swap(vec_new_op);
        return NO_REPLACED_EXP;
      }
      void *new_expr_list = m_ast->CreateExpList(vec_new_op);
      m_ast->ReplaceExp(parent, new_expr_list);
      return REPLACED_EXP;
    }

    //  pointer comparsion
    if (m_ast->IsCompareOp(parent) != 0) {
      void *lhs;
      void *rhs;
      m_ast->GetExpOperand(parent, &lhs, &rhs);
      vector<void *> vec_new_exp_2;
      if (lhs == exp) {
        auto ret = decomposeExpression(rhs, &vec_new_exp_2, visited);
        if (ret == UNSUPPORTED_REPLACED_EXP) {
          return UNSUPPORTED_REPLACED_EXP;
        }
      } else {
        //  delay to decompose because the other operand has not been
        //  decomposed
        return NO_REPLACED_EXP;
      }
      *leaf_exp = parent;
      assert(vec_new_exp_2.size() == vec_new_exp->size());
      vector<void *> vec_new_op;
      int com_op = m_ast->GetCompareOp(parent);
      for (size_t i = 0; i != vec_new_exp->size(); ++i) {
        if ((*vec_new_exp)[i] == nullptr) {
          if (!replace)
            vec_new_op.push_back(nullptr);
          continue;
        }
        void *rhs = vec_new_exp_2[i];
        void *lhs = (*vec_new_exp)[i];
        vec_new_op.push_back(m_ast->CreateExp(com_op, lhs, rhs, 0, parent));
      }
      if (!replace) {
        vec_new_exp->swap(vec_new_op);
        return NO_REPLACED_EXP;
      }
      void *new_and_expr = m_ast->CreateAndOpFromExprList(vec_new_op);
      m_ast->ReplaceExp(parent, new_and_expr);
      return REPLACED_EXP;
    }

    //  pointer test
    if (is_pointer_test(exp)) {  //  if (p) or if (!p)
      if (!vec_new_exp->empty()) {
        if (!replace) {
          return NO_REPLACED_EXP;
        }
        m_ast->ReplaceExp(exp, (*vec_new_exp)[0]);
      } else {
        MERLIN_ERROR("find a empty struct");
      }
      return REPLACED_EXP;
    }
  }
  if (m_ast->IsCommaExp(parent) != 0) {
    //  Fix Bug2242
    if (vec_new_exp->empty()) {
      void *dummy_zero = m_ast->CreateConst(0);
      if (!replace) {
        vec_new_exp->push_back(dummy_zero);
        return NO_REPLACED_EXP;
      }
      m_ast->ReplaceExp(exp, dummy_zero);
      return REPLACED_EXP;
    }
    void *new_exp = nullptr;
    for (size_t i = 0; i != vec_new_exp->size(); ++i) {
      if ((*vec_new_exp)[i] == nullptr)
        continue;
      if (new_exp == nullptr)
        new_exp = (*vec_new_exp)[i];
      else
        new_exp = m_ast->CreateExp(V_SgCommaOpExp, new_exp, (*vec_new_exp)[i],
                                   0, parent);
    }
    if (!replace) {
      vec_new_exp->clear();
      vec_new_exp->push_back(new_exp);
      return NO_REPLACED_EXP;
    }
    if (new_exp != nullptr)
      m_ast->ReplaceExp(exp, new_exp);
    return REPLACED_EXP;
  }

  if (m_ast->IsExprStatement(parent) != 0) {
    if (!replace) {
      return NO_REPLACED_EXP;
    }
    vector<void *> vec_new_exp_without_nullptr;
    for (auto new_exp : *vec_new_exp) {
      if (new_exp != nullptr) {
        vec_new_exp_without_nullptr.push_back(new_exp);
      }
    }
    void *new_expr_list = m_ast->CreateExpList(vec_new_exp_without_nullptr);
    m_ast->ReplaceExp(exp, new_expr_list);
    return REPLACED_EXP;
  }

  if (m_ast->IsExprList(parent) != 0) {
    //  should happen in function argument or initializer
    return NO_REPLACED_EXP;
  }
  if (!replace) {
    return NO_REPLACED_EXP;
  }
#ifdef PROJDEBUG
  cout << m_ast->_up(exp) << " in " << m_ast->_p(parent) << endl;
#endif
  return UNSUPPORTED_REPLACED_EXP;
}

vector<void *> StructDecompose::InlineBuiltInAssign(void *assign_exp,
                                                    void *struct_type,
                                                    void *caller_decl,
                                                    void *scope,
                                                    void *bindNode) {
  void *left_exp, *right_exp;
  left_exp = right_exp = nullptr;
  m_ast->IsGeneralAssignOp(assign_exp, &left_exp, &right_exp);
  vector<void *> inlined_stmts;
  vector<void *> vec_data_members;
  getClassDataMembers(struct_type, &vec_data_members);
  int member_idx = 0;
  for (auto member : vec_data_members) {
    member_idx++;
    if (m_ast->IsStatic(member) || !contain_used_or_non_pointer_field(member)) {
      continue;
    }
    void *var_init = m_ast->GetVariableInitializedName(member);
    void *type = m_ast->GetTypebyVar(var_init);
    void *member_left_ref = m_ast->CreateVariableRef(var_init, assign_exp);
    void *member_right_ref = m_ast->CreateVariableRef(var_init, assign_exp);
    void *new_left_exp = m_ast->CreateExp(V_SgDotExp, m_ast->CopyExp(left_exp),
                                          member_left_ref, 0, assign_exp);
    void *new_right_exp = m_ast->CreateExp(
        V_SgDotExp, m_ast->CopyExp(right_exp), member_right_ref, 0, assign_exp);
    if ((m_ast->IsPointerType(type) != 0) || (m_ast->IsScalarType(type) != 0)) {
      void *left_exp_ty = m_ast->GetTypeByExp(new_left_exp);
      if (m_ast->IsPointerType(left_exp_ty) != 0)
        new_right_exp =
            m_ast->CreateCastExp(new_right_exp, left_exp_ty, assign_exp);
      void *assign_stmt = m_ast->CreateStmt(V_SgAssignStatement, new_left_exp,
                                            new_right_exp, bindNode);
      inlined_stmts.push_back(assign_stmt);
      continue;
    }
    vector<void *> vec_iters;
    vector<size_t> dims;
    void *base_type = nullptr;
    m_ast->get_type_dimension(type, &base_type, &dims, var_init);
    for (size_t i = 0; i < dims.size(); ++i) {
      string iter_name = "i_" + my_itoa(member_idx) + "_" + my_itoa(i);
      m_ast->get_valid_local_name(caller_decl, &iter_name);
      void *iter_decl =
          m_ast->CreateVariableDecl("int", iter_name, nullptr, scope);
      m_ast->PrependChild(scope, iter_decl);
      vec_iters.push_back(m_ast->GetVariableInitializedName(iter_decl));
    }
    if (!dims.empty()) {
      for (size_t i = dims.size(); i > 0; --i) {
        new_left_exp =
            m_ast->CreateExp(V_SgPntrArrRefExp, new_left_exp,
                             m_ast->CreateVariableRef(vec_iters[i - 1]));
        new_right_exp =
            m_ast->CreateExp(V_SgPntrArrRefExp, new_right_exp,
                             m_ast->CreateVariableRef(vec_iters[i - 1]));
      }
    }
    void *sub_assign_stmt = nullptr;
    void *sub_assign_exp = nullptr;
    if (containsCompoundType(base_type) == SUPPORTED_COMPOUND_TYPE) {
      sub_assign_exp = m_ast->CreateExp(V_SgAssignOp, new_left_exp,
                                        new_right_exp, 0, assign_exp);
      sub_assign_stmt =
          m_ast->CreateStmt(V_SgExprStatement, sub_assign_exp, bindNode);
    } else {
      sub_assign_stmt = m_ast->CreateStmt(V_SgAssignStatement, new_left_exp,
                                          new_right_exp, bindNode);
    }
    void *for_stmt = sub_assign_stmt;
    if (static_cast<unsigned int>(!dims.empty()) != 0U) {
      void *loop_body = m_ast->CreateBasicBlock();
      m_ast->PrependChild(loop_body, sub_assign_stmt);
      for (size_t i = 0; i < dims.size(); ++i) {
        void *loop = m_ast->CreateStmt(
            V_SgForStatement, vec_iters[i], m_ast->CreateConst(0),
            m_ast->CreateConst(dims[i]), loop_body, nullptr, nullptr, bindNode);
        if (i < dims.size() - 1) {
          loop_body = m_ast->CreateBasicBlock();
          m_ast->PrependChild(loop_body, loop);
        } else {
          for_stmt = loop;
        }
      }
    }
    inlined_stmts.push_back(for_stmt);
    if (sub_assign_exp) {
      auto sub_inlined_stmts = InlineBuiltInAssign(
          sub_assign_exp, base_type, caller_decl, scope, bindNode);
      if (m_ast->GetParent(sub_assign_stmt)) {
        for (auto stmt : sub_inlined_stmts)
          m_ast->InsertStmt(stmt, sub_assign_stmt);
        m_ast->RemoveStmt(sub_assign_stmt);
      } else {
        inlined_stmts.pop_back();
        inlined_stmts.insert(inlined_stmts.end(), sub_inlined_stmts.begin(),
                             sub_inlined_stmts.end());
      }
    }
  }
  return inlined_stmts;
}

decomposed_info
StructDecompose::decomposeInitializer(void *initializer,
                                      vector<void *> *new_initializer,
                                      bool check, string *diagnosis_info) {
  set<void *> visited;
  return decomposeInitializer(initializer, new_initializer, check,
                              diagnosis_info, &visited);
}

decomposed_info StructDecompose::decomposeInitializer(
    void *initializer, vector<void *> *new_initializer, bool check,
    string *diagnosis_info, set<void *> *visited) {
  if (initializer == nullptr) {
    return NO_REPLACED_EXP;
  }
  void *type = m_ast->GetTypeByExp(initializer);
  if (m_ast->IsConstructorInitializer(initializer) ||
      containsCompoundType(type) != SUPPORTED_COMPOUND_TYPE) {
    if (check) {
      return NO_REPLACED_EXP;
    }
    new_initializer->push_back(m_ast->CopyExp(initializer));
    return NO_REPLACED_EXP;
  }
  SgAggregateInitializer *aggr_initializer =
      isSgAggregateInitializer(static_cast<SgNode *>(initializer));
  if (aggr_initializer != nullptr) {
    vector<size_t> dims;
    void *struct_type = nullptr;
    m_ast->get_type_dimension(type, &struct_type, &dims, initializer);
    if (dims.empty()) {
      SgExprListExp *sg_init_exp = aggr_initializer->get_initializers();

      SgExpressionPtrList &exps = sg_init_exp->get_expressions();
      for (size_t i = 0; i != exps.size(); ++i) {
        vector<void *> sub_initializer;
        if (decomposeInitializer(exps[i], &sub_initializer, check,
                                 diagnosis_info,
                                 visited) == UNSUPPORTED_REPLACED_EXP) {
          return UNSUPPORTED_REPLACED_EXP;
        }
        if (check) {
          continue;
        }
        new_initializer->insert(new_initializer->end(), sub_initializer.begin(),
                                sub_initializer.end());
      }
      return NO_REPLACED_EXP;
    }
    SgExprListExp *sg_init_exp = aggr_initializer->get_initializers();
    SgExpressionPtrList &exps = sg_init_exp->get_expressions();
    vector<vector<void *>> aggr_res;
    vector<tuple<string, void *, bool, bool>> sub_types;
    if (decomposeType(struct_type, &sub_types, initializer) == 0) {
      return UNSUPPORTED_REPLACED_EXP;
    }
    size_t mem_size = sub_types.size();
    aggr_res.resize(mem_size);
    for (size_t i = 0; i != exps.size(); ++i) {
      vector<void *> sub_initializer;
      if (decomposeInitializer(exps[i], &sub_initializer, check, diagnosis_info,
                               visited) == UNSUPPORTED_REPLACED_EXP) {
        return UNSUPPORTED_REPLACED_EXP;
      }
      if (check) {
        continue;
      }
      for (size_t j = 0; j != mem_size; ++j) {
        if (j < sub_initializer.size())
          aggr_res[j].push_back(sub_initializer[j]);
      }
    }
    if (check) {
      return NO_REPLACED_EXP;
    }
    for (size_t i = 0; i != mem_size; ++i) {
      if (!aggr_res[i].empty())
        new_initializer->push_back(m_ast->CreateAggregateInitializer(
            aggr_res[i], std::get<1>(sub_types[i])));
    }
    return NO_REPLACED_EXP;
  }
  void *init_operand = initializer;
  if (m_ast->IsAssignInitializer(initializer) != 0) {
    SgAssignInitializer *sg_assign_initializer =
        isSgAssignInitializer(static_cast<SgNode *>(initializer));
    init_operand = sg_assign_initializer->get_operand();
  } else if (m_ast->IsInitializer(initializer) != 0) {
    if (check) {
      *diagnosis_info +=
          "unsupported initializer: " + m_ast->_up(initializer) + "\n";
    }
    return UNSUPPORTED_REPLACED_EXP;
  }
  vector<void *> vec_exp;
  auto ret = decomposeExpression(init_operand, &vec_exp, visited);
  if (check) {
    if (ret == UNSUPPORTED_REPLACED_EXP) {
      *diagnosis_info +=
          "unsupported initializer: " + m_ast->_up(initializer) + "\n";
    }
    return ret;
  }
  if (ret != UNSUPPORTED_REPLACED_EXP) {
    for (auto exp : vec_exp) {
      new_initializer->push_back(exp);
    }
  }
  if (ret == UNSUPPORTED_REPLACED_EXP) {
    cout << "invalid initializer"
         << (static_cast<SgNode *>(initializer))->class_name() << endl;
#ifdef PROJDEBUG
    cout << m_ast->_up(initializer) << endl;
#endif
  }
  return ret;
}

void *StructDecompose::convertStructReturnTypeIntoParameter(void *func) {
  void *ret_type = m_ast->GetFuncReturnType(func);
  string type_info;
  enum type_kind res = containsCompoundType(ret_type, &type_info);
  if (res != SUPPORTED_COMPOUND_TYPE) {
    if (res == UNSUPPORTED_COMPOUND_TYPE) {
      string port_info = "return port " +
                         getUserCodeFileLocation(m_ast, func, true) +
                         " with unsupported type: \n" + type_info;
      dump_critical_message(DISAG_ERROR_2(port_info));
      mErrorOut = true;
    }
    return func;
  }

  string ret_var_name = "ret_var";
  m_ast->get_valid_local_name(func, &ret_var_name);
  void *new_ret_type = m_ast->GetTypeByString("void");
  vector<void *> fp_list;
  void *ret_var = m_ast->CreateVariable(m_ast->CreatePointerType(ret_type),
                                        ret_var_name, func);
  fp_list.push_back(ret_var);
  map<void *, void *> orgArg2newArg;
  for (int i = 0; i != m_ast->GetFuncParamNum(func); ++i) {
    void *old_arg = m_ast->GetFuncParam(func, i);
    void *new_arg = m_ast->CreateVariable(
        m_ast->GetTypebyVar(old_arg), m_ast->GetVariableName(old_arg), old_arg);
    orgArg2newArg[old_arg] = new_arg;
    fp_list.push_back(new_arg);
  }
  string func_name = m_ast->GetFuncName(func);
  void *new_func = m_ast->CreateFuncDecl(new_ret_type, func_name, fp_list,
                                         m_ast->GetGlobal(func), true, func);
  m_ast->InsertStmt(new_func, func);
  if (m_ast->IsStatic(func)) {
    m_ast->SetStatic(new_func);
  }
  void *func_body = m_ast->GetFuncBody(func);
  void *clone_body = m_ast->CopyStmt(func_body);
  for (auto arg_map : orgArg2newArg) {
    m_ast->replace_variable_references_in_scope(arg_map.first, arg_map.second,
                                                clone_body);
  }
  m_ast->SetFuncBody(new_func, clone_body);
  void *new_func_body = m_ast->GetFuncBody(new_func);

  replaceLocalVariableReference(new_func_body, func_body);
  vector<void *> vec_stmt;
  m_ast->GetNodesByType(new_func_body, "preorder", "SgReturnStmt", &vec_stmt);
  for (auto ret_stmt : vec_stmt) {
    void *ret_exp = m_ast->GetReturnExp(ret_stmt);
    void *assign_stmt = m_ast->CreateStmt(
        V_SgAssignStatement,
        m_ast->CreateExp(V_SgPointerDerefExp,
                         m_ast->CreateVariableRef(ret_var, ret_stmt), nullptr,
                         0, ret_stmt),
        m_ast->CopyExp(ret_exp), ret_stmt);
    m_ast->InsertStmt(assign_stmt, ret_stmt);
    void *new_ret_stmt = m_ast->CreateStmt(V_SgReturnStmt, nullptr, ret_stmt);
    m_ast->InsertStmt(new_ret_stmt, ret_stmt);
    m_ast->RemoveStmt(ret_stmt);
  }
  //  update original declaration with no body
  vector<void *> vec_decls = m_ast->GetAllFuncDecl(func);
  for (auto decl : vec_decls) {
    if (decl == func) {
      continue;
    }
    void *func_body = m_ast->GetFuncBody(decl);
    if (func_body != nullptr) {
      continue;
    }
    //  youxiang: 20160923 bug639: do not touch header file
    if (m_ast->isWithInHeaderFile(decl)) {
      continue;
    }
    void *new_decl =
        m_ast->CloneFuncDecl(new_func, m_ast->GetGlobal(func), false);
    m_ast->InsertStmt(new_decl, decl);
    m_ast->RemoveStmt(decl);
    //  m_ast->ReplaceStmt(decl, new_decl);
  }
  return new_func;
}

void StructDecompose::replaceLocalVariableReference(void *new_body,
                                                    void *org_body) {}

bool StructDecompose::remove_dead_code(void *func) {
  void *func_body = m_ast->GetFuncBody(func);
  bool Changed = m_ast->dead_stmts_removal(func_body);
  return Changed;
}

void StructDecompose::decomposePragma(void *var_init, void *scope) {
  if (mVar2Members.count(var_init) <= 0) {
    return;
  }
  const vector<void *> &vec_members = mVar2Members[var_init];
  void *var_type = m_ast->GetTypebyVar(var_init);
  string var_name = m_ast->GetVariableName(var_init);
  vector<tuple<string, void *, bool, bool>> member_types;
  decomposeType(var_type, &member_types, var_init);
  vector<void *> vec_pragma;
  m_ast->GetNodesByType(scope, "preorder", "SgPragmaDeclaration", &vec_pragma);
  for (auto pragma : vec_pragma) {
    if (!mMarsIrV2.get_pragma_attribute(pragma, CMOST_interface).empty()) {
      continue;
    }
    string pragma_name =
        mMarsIrV2.get_pragma_attribute(pragma, CMOST_variable, true);
    if (pragma_name.empty()) {
      continue;
    }
    vector<string> vec_pragma_name = str_split(pragma_name, "->");
    void *pragma_var = m_ast->find_variable_by_name(vec_pragma_name[0], pragma);
    if (pragma_var != var_init && (vec_pragma_name[0] != var_name)) {
      continue;
    }
    int j = 0;
    for (auto mem : vec_members) {
      if (mem == nullptr) {
        ++j;
        continue;
      }
      string field_name = var_name + "->" + std::get<0>(member_types[j]);
      ++j;
      if (field_name == pragma_name ||
          (field_name.find(pragma_name) == 0 &&
           field_name[pragma_name.size()] == '-')) {
        void *new_pragma = m_ast->CopyStmt(pragma);
        m_ast->InsertStmt(new_pragma, pragma);
        mMarsIrV2.set_pragma_attribute(&new_pragma, CMOST_variable,
                                       m_ast->GetVariableName(mem));
        string dim_str =
            mMarsIrV2.get_pragma_attribute(pragma, CMOST_dim, true);
        if (!dim_str.empty()) {
          int dim_offset = get_dim_offset(field_name, var_init);
          if (dim_offset > 0)
            mMarsIrV2.set_pragma_attribute(
                &new_pragma, CMOST_dim, my_itoa(my_atoi(dim_str) + dim_offset),
                true);
        }
      }
    }
    m_ast->RemoveStmt(pragma);
  }
}

bool StructDecompose::is_pointer_test(void *exp) {
  void *parent = m_ast->GetParent(exp);
  void *cond;
  void *true_val;
  void *false_val;
  if (m_ast->IsUnderConditionStmt(parent) ||  //  if (p); while (p); etc
      (m_ast->IsNotOp(parent) != 0) ||        //  !p;
      (m_ast->IsAndOp(parent) != 0) || (m_ast->IsOrOp(parent) != 0) ||
      (m_ast->IsConditionalExp(parent, &cond, &true_val, &false_val) &&
       cond == exp) ||
      ((m_ast->IsCastExp(parent) != 0) &&
       (m_ast->IsBoolType(m_ast->GetTypeByExp(parent)) != 0))) {
    return true;
  }
  return false;
}

void StructDecompose::collect_used_fields() {
  vector<void *> vec_refs;
  m_ast->GetNodesByType_int(nullptr, "preorder", V_SgVarRefExp, &vec_refs);
  for (auto ref : vec_refs) {
    void *var_init = m_ast->GetVariableInitializedName(ref);
    if (m_ast->IsMemberVariable(var_init)) {
      mUsedFields[var_init] = true;
      mUsedFieldNames.insert(get_qualified_member_name(var_init));
    }
  }
}

string StructDecompose::get_qualified_member_name(void *var_init) {
  string base_name = m_ast->GetVariableName(var_init);
  if (!m_ast->IsMemberVariable(var_init)) {
    return base_name;
  }
  void *type_decl = m_ast->TraceUpToClassDecl(var_init);
  while (m_ast->IsAnonymousTypeDecl(type_decl) != 0) {
    type_decl = m_ast->GetParent(type_decl);
  }
  if (type_decl != nullptr) {
    return m_ast->GetTypeName(type_decl, true) + "::" + base_name;
  }
  return base_name;
}

bool StructDecompose::contain_used_or_non_pointer_field(void *var_decl) {
  void *var_init = m_ast->GetVariableInitializedName(var_decl);
  if (mUsedFields.count(var_init) > 0) {
    return mUsedFields[var_init];
  }
  string var_qualified_name = get_qualified_member_name(var_init);
  if (mUsedFieldNames.count(var_qualified_name) > 0) {
    mUsedFields[var_init] = true;
    return true;
  }
  void *var_type = m_ast->GetTypebyVar(var_init);
  if (m_ast->IsPointerType(var_type) != 0) {
    mUsedFields[var_init] = false;
    return false;
  }
  vector<size_t> dims;
  void *base_type = nullptr;
  m_ast->get_type_dimension(var_type, &base_type, &dims, var_init);
  if (containsCompoundType(base_type) == SCALAR_TYPE) {
    mUsedFields[var_init] = true;
    return true;
  }
  if (containsCompoundType(base_type) == SUPPORTED_COMPOUND_TYPE) {
    vector<void *> vec_data_members;
    m_ast->GetClassDataMembers(base_type, &vec_data_members);
    for (auto mem : vec_data_members) {
      if (contain_used_or_non_pointer_field(mem)) {
        mUsedFields[var_init] = true;
        return true;
      }
    }
  }
  mUsedFields[var_init] = false;
  return false;
}

void StructDecompose::split_expression_with_side_effect(void *exp) {
  vector<void *> vec_exprs;
  m_ast->GetNodesByType(exp, "postorder", "SgExpression", &vec_exprs, true);
  for (auto sub_exp : vec_exprs) {
    if (m_ast->has_side_effect(sub_exp) != 0) {
#if DISABLE_DEBUG
      m_ast->splitExpression(isSgExpression(static_cast<SgNode *>(sub_exp)));
#else
      void *ret = m_ast->splitExpression(
          isSgExpression(static_cast<SgNode *>(sub_exp)));
      cout << "split expression ret: " << m_ast->_p(ret) << endl;
#endif
    }
  }
}
void StructDecompose::add_token_map(string arrow_access_exp,
                                    string decomposed_name, bool global) {
  auto &token_map = global ? m_global_token_map : m_local_token_map;
  token_map[arrow_access_exp] = decomposed_name;
  token_map[arrow_to_dot(arrow_access_exp)] = decomposed_name;
  auto pos = arrow_access_exp.rfind("->");
  while (pos != string::npos) {
    arrow_access_exp = arrow_access_exp.substr(0, pos);
    if (token_map.count(arrow_access_exp) > 0) {
      token_map[arrow_access_exp] += "," + decomposed_name;
    } else {
      token_map[arrow_access_exp] = decomposed_name;
    }
    string dot_access_exp = arrow_to_dot(arrow_access_exp);
    if (dot_access_exp != arrow_access_exp) {
      if (token_map.count(dot_access_exp) > 0) {
        token_map[dot_access_exp] += "," + decomposed_name;
      } else {
        token_map[dot_access_exp] = decomposed_name;
      }
    }
    pos = arrow_access_exp.rfind("->");
  }
}
