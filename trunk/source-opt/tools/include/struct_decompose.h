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


#pragma once
#include <map>
#include <tuple>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "cmdline_parser.h"
#include "codegen.h"
#include "file_parser.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "xml_parser.h"

//  SgClassType->SgClassDeclaration->SgClassDefinition->SgDeclarationStatementList
//  struct expression
//  1) Pntr or array access
//  2) SgDotExp
//   SgArrowExp or field access
enum type_kind {
  SCALAR_TYPE = 0,
  SUPPORTED_COMPOUND_TYPE = 1,
  UNSUPPORTED_COMPOUND_TYPE = 2,
};
enum decomposed_info {
  UNSUPPORTED_REPLACED_EXP = 0,
  REPLACED_EXP = 1,
  NO_REPLACED_EXP = 2,
};

struct DecomposedTypeInfo {
  std::vector<std::tuple<string, void *, bool, bool>> res;
  std::string message;
  bool valid;
  DecomposedTypeInfo() { valid = false; }
};

class StructDecompose {
  CMarsIr mMarsIr;
  CMarsIrV2 mMarsIrV2;
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  std::vector<void *> mKernelFuncs;
  std::set<void *> mKernelFuncsSet;
  std::set<void *> mTopKernel;
  //  from original struct variables to
  //  decomposed fields
  std::map<void *, std::vector<void *>> mVar2Members;
  bool mErrorOut;
  bool m_aligned_struct_decomp;
  bool mOpenCL_flow;
  std::set<std::string> mCheckedIdentifier;
  bool mCheckDimensionSize;
  std::map<void *, std::map<std::string, std::tuple<void *, std::vector<size_t>,
                                                    std::string>>>
      mVar2ZeroDims;
  std::map<std::string, int> mBuiltInFunc;
  std::unordered_map<void *, bool> mUsedFields;
  std::unordered_set<std::string> mUsedFieldNames;
  std::unordered_map<std::string, std::string> m_global_token_map;
  std::unordered_map<std::string, std::string> m_local_token_map;

  std::vector<pair<void *, void *>> m_var_vec;
  std::map<void *, pair<enum type_kind, std::string>> m_type_info;
  std::unordered_map<void *, DecomposedTypeInfo> m_cache_type_decomposed;

 public:
  StructDecompose(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &option)
      : m_ast(codegen), mTopFunc(pTopFunc), mOptions(option),
      mCheckDimensionSize(true) {
        string tool_version = option.get_option_key_value("-a", "tool_version");
        if (tool_version == "vivado_hls" || tool_version == "vitis_hls") {
          mOpenCL_flow = false;
        } else {
          mOpenCL_flow = true;
        }
        m_aligned_struct_decomp =
          option.get_option_key_value("-a", "aligned_struct_decomp") != "off";
        if (option.get_option_key_value("-a", "ignore_unknown_dim") != "")
          mCheckDimensionSize = false;
        init();
      }

  void init();

  int run();

  void check();

  int preprocess();

  int fail() { return mErrorOut; }

 private:
  enum type_kind containsCompoundType(void *sg_type) {
    std::string type_info;
    return containsCompoundType(sg_type, &type_info);
  }

  enum type_kind containsCompoundType(void *sg_type, std::string *type_info) {
    void *base_type = m_ast->GetBaseType(sg_type, true);
    if (m_type_info.count(base_type) <= 0) {
      enum type_kind res;
      if (containsUnsupportedType(base_type, type_info, true)) {
        cout << "find unsupported type: " << *type_info << endl;
        res = UNSUPPORTED_COMPOUND_TYPE;
      } else {
        if (m_ast->IsStructureType(base_type) ||
            m_ast->IsClassType(base_type)) {
          if (donotDecomposeCompoundType(base_type))
            res = SCALAR_TYPE;
          else
            res = SUPPORTED_COMPOUND_TYPE;
        } else {
          if (m_ast->IsScalarType(base_type) ||
              cannotDecomposeType(base_type)) {
            res = SCALAR_TYPE;
          } else {
            //  FIXME: we only struct/class type now
            res = UNSUPPORTED_COMPOUND_TYPE;
          }
        }
      }
      m_type_info[base_type] = std::make_pair(res, *type_info);
      return res;
    }
    auto &cache_res = m_type_info[base_type];
    *type_info = cache_res.second;
    return cache_res.first;
  }

  int containsUnsupportedType(void *sg_type, std::string *type_info, bool top) {
    if (top) {
      if (m_ast->IsVoidType(m_ast->GetOrigTypeByTypedef(sg_type, true))) {
        return 0;
      }
    }
    void *base_type = m_ast->GetBaseType(sg_type, true);
    if (m_ast->IsEnumType(base_type) || m_ast->IsUnionType(base_type) ||
        m_ast->IsIntegerType(base_type) || m_ast->IsFloatType(base_type) ||
        m_ast->IsComplexType(base_type))
      return 0;
    if (m_ast->IsStructureType(base_type) || m_ast->IsClassType(base_type)) {
      if (donotDecomposeCompoundType(base_type))
        return 0;
    }
    std::string reason;
    void *unsupported_type = nullptr;
    int ret = 0;
    if (m_ast->IsRecursiveType(base_type, &unsupported_type) != 0) {
      reason = "recursive type";
      ret = 1;
    } else if (m_ast->IsStructureType(base_type) != 0 ||
               m_ast->IsClassType(base_type) != 0) {
      void *type_decl = m_ast->GetTypeDeclByType(base_type, 1);
      if (type_decl == nullptr) {
        unsupported_type = base_type;
        reason = "non-defined class type";
        ret = 1;
      } else {
        vector<void *> vec_members;
        m_ast->GetClassMembers(base_type, &vec_members);
        for (auto member : vec_members) {
          void *var_init = nullptr;
          void *var_type = nullptr;
          if (m_ast->IsVariableDecl(member) != 0) {
            if (static_cast<SgVariableDeclaration *>(member)->get_bitfield() !=
                nullptr) {
              unsupported_type = member;
              reason = "bitfield decl";
              ret = 1;
              break;
            }
            var_init = m_ast->GetVariableInitializedName(member);
            var_type = m_ast->GetTypebyVar(var_init);
          }

          if (var_type != nullptr) {
            if (containsUnsupportedType(var_type, &reason, false)) {
              ret = 1;
              break;
            }
          }
          if (m_ast->IsStatic(member)) {
            unsupported_type = member;
            reason = "static class member";
            ret = 1;
            break;
          }
        }
      }
    } else {
      ret = 1;
      unsupported_type = base_type;
      if (m_ast->IsFunctionType(base_type) != 0) {
        reason = "function pointer type";
      } else if (m_ast->IsUnionType(base_type) != 0) {
        reason = "union type";
      } else if (m_ast->IsVoidType(base_type) != 0) {
        reason = "void type";
      } else {
        reason = "unknown type";
      }
    }
    if (ret) {
      *type_info += reason;
      if (unsupported_type != nullptr)
        *type_info += ": " + m_ast->_up(unsupported_type) + "\n";
    }
    //  FIXME: we only struct/class type now
    return ret;
  }

  bool cannotDecomposeType(void *sg_type) {
    void *base_type = m_ast->GetBaseType(sg_type, true);
    if (m_ast->IsStructureType(base_type) || m_ast->IsClassType(base_type)) {
      if (donotDecomposeCompoundType(base_type))
        return true;
      return false;
    }
    if (m_ast->IsScalarType(base_type) || m_ast->IsComplexType(base_type))
      return true;
    if (m_ast->IsRecursiveType(base_type))
      return true;
    if (m_ast->IsFunctionType(base_type))
      return true;

    //  unknown type
    return true;
  }

  bool donotDecomposeCompoundType(void *base_type) {
    std::string class_type = m_ast->GetTypeNameByType(base_type, true);
    if (class_type.find("ap_int") == 0 || class_type.find("ap_uint") == 0 ||
        class_type.find("ap_fixed") == 0 ||
        class_type.find("ap_ufixed") == 0 || class_type.find("hls::") == 0 ||
        class_type.find("complex") == 0 ||
        class_type.find("std::complex") == 0)
      return true;
    if (!class_type.empty() &&
        m_ast->IsStructureWithAlignedScalarData(base_type) &&
        !m_aligned_struct_decomp)
      return true;

    if (class_type.find("merlin_stream") != std::string::npos)
      return true;
    return false;
  }

  int decomposeParameter(void *var_init, void *scope);

  int decomposeLocalVariable(void *var_init, void *scope);

  int decomposeGlobalVariable(void *var_init, void *scope);

  void decomposePragma(void *var_init, void *scope);

  void replaceVariableRefs(void *var_init, void *scope);

  void checkRemainingVariableRefs(void *var_init, void *scope);

  void replaceVariableRefsLater(void *var_init, void *scope);
  //  input: struct type
  //  output: a group of pair of name prefix and leaf node final type
  int decomposeType(
      void *sg_type,
      std::vector<std::tuple<std::string, void *, bool, bool>> *res, void *pos,
      std::string *diagnosis_info, void *sg_modifier_type = nullptr);

  int decomposeType(
      void *sg_type,
      std::vector<std::tuple<std::string, void *, bool, bool>> *res,
      void *pos) {
    std::string diagnosis_info;
    return decomposeType(sg_type, res, pos, &diagnosis_info);
  }

  int canSafeDecompose(void *var_init, const std::vector<void *> &refs,
                       std::string *diagnosis_info);

  int canSafeDecompose(void *var_init, const std::vector<void *> &refs) {
    std::string diagnosis_info;
    return canSafeDecompose(var_init, refs, &diagnosis_info);
  }

  void *decomposeFunction(void *func_decl, bool *is_dead);

  void *removeUnusedParameters(void *func_decl);

  decomposed_info decomposeExpression(void *leaf_exp,
                                      std::vector<void *> *vec_exp);

  decomposed_info decomposeVariableRef(void *sg_var_ref, void **leaf_exp,
                                       std::vector<void *> *vec_new_exp,
                                       bool generate_new_expr, bool replace);
  decomposed_info decomposeExpression(void *leaf_exp,
                                      std::vector<void *> *vec_exp,
                                      std::set<void *> *visited);

  decomposed_info decomposeVariableRef(void *sg_var_ref, void **leaf_exp,
                                       std::vector<void *> *vec_new_exp,
                                       bool generate_new_expr, bool replace,
                                       std::set<void *> *visited);

  int isKernelFunction(void *func_decl);

  int get_leaf_expression_from_var_ref(void *var_ref, void **leaf_exp);
#if 0
  void *createBuiltInAssignFunc(const std::string &func_name, void *struct_type,
                                void *caller_decl, void *bindNode);
#else
  std::vector<void *> InlineBuiltInAssign(void *assign_exp, void *struct_type,
                                          void *caller_decl, void *scope,
                                          void *bindNode);
#endif
  void *convertStructReturnTypeIntoParameter(void *func);

  void replaceLocalVariableReference(void *new_body, void *orig_body);

  decomposed_info decomposeInitializer(void *orig_init,
                                       std::vector<void *> *vec_new_init) {
    std::string diagnosis_info;
    return decomposeInitializer(orig_init, vec_new_init, false,
                                &diagnosis_info);
  }

  int canDecomposeInitializer(void *orig_init, std::string *diagnosis_info) {
    std::vector<void *> vec_new_init;
    return decomposeInitializer(orig_init, &vec_new_init, true, diagnosis_info);
  }

  decomposed_info decomposeInitializer(void *initializer,
                                       std::vector<void *> *new_initalizer,
                                       bool check, std::string *diagnosis_info);

  decomposed_info decomposeInitializer(void *initializer,
                                       std::vector<void *> *new_initalizer,
                                       bool check, std::string *diagnosis_info,
                                       std::set<void *> *visited);

  std::vector<size_t>
  get_array_dimension(std::string field_name, int *cont_flag, void *pos,
                      const std::vector<void *> &vec_pragma);

  std::vector<std::string>
  get_array_depth(std::string field_name, void *pos,
                  const std::vector<void *> &vec_pragma);

  int has_check(const std::string &identifier) {
    if (mCheckedIdentifier.count(identifier) > 0)
      return 1;
    mCheckedIdentifier.insert(identifier);
    return 0;
  }

  bool remove_dead_code(void *func_decl);

  void getClassDataMembers(void *sg_type, std::vector<void *> *res);

  bool is_pointer_test(void *exp);

  bool check_valid_field_name(std::string field_name, void *pos);

  void collect_used_fields();

  bool contain_used_or_non_pointer_field(void *var_decl);

  std::string get_qualified_member_name(void *var_init);

  void split_expression_with_side_effect(void *exp);

  bool process_assign_exp(void *assign_exp, void *left_op, void *right_op,
                          void *func);

  void update_pragma_with_token_map(void *func);
  void add_token_map(string arrow_mem_exp, string decomposed_name, bool global);
  int get_dim_offset(string field_name, void *pos);
  int get_dim_size(string field_name, void *pos);
};
