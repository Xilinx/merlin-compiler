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


#include "program_analysis.h"
#include "rose.h"

using MarsProgramAnalysis::CMarsAccess;

vector<vector<int>> enumerate_point_space(const vector<int> &dim_size) {
  vector<vector<int>> ret;
  ret.clear();
  if (dim_size.empty()) {
    // return empty set
  } else if (dim_size.size() == 1) {
    for (int i = 0; i < dim_size[0]; i++) {
      vector<int> point_1d;
      point_1d.push_back(i);
      ret.push_back(point_1d);
    }
  } else {
    int dim_minus_1 = static_cast<int>(dim_size.size() - 1);
    vector<int> dim_size_minus_1;
    for (int i = 0; i < dim_minus_1; i++) {
      dim_size_minus_1.push_back(dim_size[i]);
    }
    vector<vector<int>> point_set_minus_1 =
        enumerate_point_space(dim_size_minus_1);

    for (size_t i = 0; i < point_set_minus_1.size(); i++) {
      vector<int> curr_point = point_set_minus_1[i];
      for (int j = 0; j < dim_size[dim_minus_1]; j++) {
        vector<int> one_new_point = curr_point;
        one_new_point.push_back(j);
        ret.push_back(one_new_point);
      }
    }
  }

  return ret;
}
//
// Notes:
// 1. Only handle full dimension read accesses (ignore other access)
// 2. Use new variable xxx_r to replace original references
// 3. The access index must be continuous
// 4. Only handle the innermost loop ( assume single scope )
int index_switch_transform(CSageCodeGen *codegen, void *input_array,
                           void *input_loop, vector<int> switch_factor_in,
                           bool read_only) {
  // 1. Find the feasible accesses
  // 2. Check and assign the offsets
  // 3. Generate code

  // 0. basic information
  vector<int> switch_factor;
  for (int j = 0; j < static_cast<int>(switch_factor_in.size()); j++) {
    if (switch_factor_in[j] == 0) {
      switch_factor.push_back(1);
    } else {
      switch_factor.push_back(switch_factor_in[j]);
    }
  }

  int input_dim = 0;
  void *input_base_type;
  {
    void *sg_type = codegen->GetTypebyVar(input_array);
    vector<size_t> vec_sizes;
    input_dim = codegen->get_type_dimension(sg_type, &input_base_type,
                                            &vec_sizes, input_loop);
    if (codegen->IsModifierType(input_base_type) != 0) {
      input_base_type = codegen->GetElementType(input_base_type);
    }
  }

  // 1. Find the feasible accesses
  //    a) read access
  //    b) full dimension
  vector<int> global_is_read;
  vector<CMarsAccess> org_access;
  vector<CMarsAccess> org_global_access;
  vector<void *> org_pntr;
  vector<void *> org_global_pntr;
  vector<CMarsAccess> org_write_access;
  vector<void *> org_write_pntr;
  {
    vector<void *> vecNodes;
    codegen->GetNodesByType_int(input_loop, "preorder", V_SgVarRefExp,
                                &vecNodes);

    for (size_t i = 0; i < vecNodes.size(); i++) {
      void *array_ref = vecNodes[i];

      if (codegen->GetVariableInitializedName(array_ref) != input_array) {
        continue;
      }

      void *sg_pntr;
      int pointer_dim = 0;
      {
        void *sg_array;
        vector<void *> sg_indexes;
        codegen->parse_pntr_ref_by_array_ref(array_ref, &sg_array, &sg_pntr,
                                             &sg_indexes, &pointer_dim);
        assert(sg_array == input_array);
      }

      bool has_write = false;
      if (codegen->has_write_conservative(sg_pntr) != 0) {
        has_write = true;
      }

      if (pointer_dim == input_dim) {
        CMarsAccess ma(sg_pntr, codegen, nullptr);
        if ((codegen->has_read_conservative(sg_pntr) != 0) && !has_write) {
          org_access.push_back(ma);
          org_global_access.push_back(ma);
          global_is_read.push_back(1);
          org_pntr.push_back(sg_pntr);
          org_global_pntr.push_back(sg_pntr);
        } else if (!read_only && has_write) {
          org_write_access.push_back(ma);
          org_global_access.push_back(ma);
          global_is_read.push_back(0);
          org_write_pntr.push_back(sg_pntr);
          org_global_pntr.push_back(sg_pntr);
        }
      }
    }
  }

  if (org_global_access.size() < 2) {
    return 0;
  }
  CMarsAccess base_access = org_global_access[0];

  // 2. Check and assign the offsets
  vector<vector<int>> vec_offsets;        // [access][dim]-> offset
  vector<vector<int>> vec_write_offsets;  // [access][dim]-> offset
  {
    for (size_t i = 0; i < org_global_access.size(); i++) {
      vector<int> one_offset;
      one_offset.resize(input_dim);
      CMarsAccess one_access = org_global_access[i];

      for (int j = 0; j < input_dim; j++) {
        int n_switch_factor = switch_factor[j];
        if (n_switch_factor == 0) {
          n_switch_factor = 1;
        }

        if (n_switch_factor > 1) {
          MarsProgramAnalysis::CMarsExpression offset =
              one_access.GetIndex(j) - base_access.GetIndex(j);

          if (offset.IsConstant() == 0) {
            return 0;
          }

          one_offset[j] = offset.GetConstant();
        } else {
          one_offset[j] = 0;
        }
      }
      if (global_is_read[i] != 0) {
        vec_offsets.push_back(one_offset);
      } else {
        vec_write_offsets.push_back(one_offset);
      }
    }
  }

  // 3. Generate code
  bool local_changed = false;
  {
    void *curr_scope = codegen->GetLoopBody(input_loop);
    void *curr_pos = nullptr;
    if (read_only) {
      curr_pos = codegen->TraceUpToStatement(org_pntr[0]);
    } else {
      curr_pos = codegen->TraceUpToStatement(org_global_pntr[0]);
    }

    void *curr_bb = codegen->CreateBasicBlock();
    codegen->InsertStmt(curr_bb, curr_pos);
    string s_prefix = "_" + codegen->UnparseToString(input_array);
    string var_name;
    void *decl;
    void *initer;

    // 3.1 create the declaration of the index variables
    vector<void *> addr_decl;
    vector<void *> bank_decl;
    {
      for (int i = 0; i < input_dim; i++) {
        int n_switch_factor = switch_factor[i];
        if (n_switch_factor == 0) {
          n_switch_factor = 1;
        }

        void *base_idx = base_access.GetIndex(i).get_expr_from_coeff();

        if (n_switch_factor > 1) {
          var_name = s_prefix + "_addr" + my_itoa(i);
          initer = codegen->CreateExp(V_SgDivideOp, base_idx,
                                      codegen->CreateConst(n_switch_factor));
          decl = codegen->CreateVariableDecl(codegen->GetTypeByString("int"),
                                             var_name, initer, curr_bb);
          // InsertStmt(decl, curr_pos);
          codegen->AppendChild(curr_bb, decl);
        } else {
          decl = nullptr;
        }
        addr_decl.push_back(decl);

        if (n_switch_factor > 1) {
          var_name = s_prefix + "_bank" + my_itoa(i);
          initer = codegen->CreateExp(V_SgModOp, codegen->CopyExp(base_idx),
                                      codegen->CreateConst(n_switch_factor));
          decl = codegen->CreateVariableDecl(codegen->GetTypeByString("int"),
                                             var_name, initer, curr_bb);
          // InsertStmt(decl, curr_pos);
          codegen->AppendChild(curr_bb, decl);
        } else {
          decl = nullptr;
        }
        bank_decl.push_back(decl);
      }
    }
    // 3.2 create the declaration of the access variables
    vector<void *> ref_decl;
    {
      int num_ref = org_access.size();
      ref_decl.resize(num_ref);
      for (size_t i = 0; i < static_cast<int>(org_access.size()); i++) {
        int ii = static_cast<int>(num_ref - 1 - i);
        var_name = s_prefix + "_r" + my_itoa(ii);
        decl = codegen->CreateVariableDecl(input_base_type, var_name, nullptr,
                                           curr_scope);
        codegen->PrependChild(curr_scope, decl, true);
        ref_decl[ii] = decl;
      }
    }

    // 3.3 create the assignment of the access variables
    vector<void *> vec_if_cond;
    vector<void *> vec_if_body;
    vector<vector<int>> bank_cond_combination;  // [cond_idx][dim] = #bank
    {
      // 3.3.1 enumerate conditions
      bank_cond_combination = enumerate_point_space(switch_factor);

      // 3.3.2 Get If-statement conditions
      for (size_t i = 0; i < bank_cond_combination.size(); i++) {
        // Condition:
        // if (_bank0 == 0 && _bank1 == 1 && ...
        vector<int> one_case = bank_cond_combination[i];
        assert(one_case.size() == (size_t)input_dim);
        // cout << one_case[0] << " " << one_case[1] << " " << one_case[2] <<
        // endl;

        void *curr_cond = nullptr;
        for (int j = 0; j < input_dim; j++) {
          int n_switch_factor = switch_factor[j];
          if (n_switch_factor == 0) {
            n_switch_factor = 1;
          }

          if (n_switch_factor > 1) {
            void *one_cond = codegen->CreateExp(
                V_SgEqualityOp, codegen->CreateVariableRef(bank_decl[j]),
                codegen->CreateConst(one_case[j]));
            if (curr_cond == nullptr) {
              curr_cond = one_cond;
            } else {
              curr_cond = codegen->CreateExp(V_SgAndOp, curr_cond, one_cond);
            }
          }
        }
        vec_if_cond.push_back(curr_cond);
      }

      // 3.3.3 Get if-statement bodies
      for (size_t i = 0; i < bank_cond_combination.size(); i++) {
        // Body Assignement
        // _a_ref0 = a[factor*addr0 + bank0][...];
        // _a_ref1 = a[factor*addr0 + bank0+1][...];
        vector<int> one_case = bank_cond_combination[i];
        assert(one_case.size() == (size_t)input_dim);

        void *curr_if_body = codegen->CreateBasicBlock();

        // vec_offset [access][dim] -> offset
        for (size_t j = 0; j < vec_offsets.size(); j++) {
          vector<int> offset_of_one_ref = vec_offsets[j];
          assert(offset_of_one_ref.size() == (size_t)input_dim);

          vector<void *> new_ref_index;
          for (int k = 0; k < input_dim; k++) {
            int n_switch_factor = switch_factor[k];
            if (n_switch_factor == 0) {
              n_switch_factor = 1;
            }

            void *new_index;
            if (n_switch_factor > 1) {
              new_index = codegen->CreateExp(
                  V_SgAddOp,
                  codegen->CreateExp(V_SgMultiplyOp,
                                     codegen->CreateVariableRef(addr_decl[k]),
                                     codegen->CreateConst(n_switch_factor)),
                  codegen->CreateConst(offset_of_one_ref[k] + one_case[k]));
            } else {
              new_index = codegen->CopyExp(
                  org_access[j].GetIndex(k).get_expr_from_coeff());
            }
            new_ref_index.push_back(new_index);
          }
          void *one_assign = codegen->CreateExp(
              V_SgAssignOp, codegen->CreateVariableRef(ref_decl[j]),
              codegen->CreateArrayRef(codegen->CreateVariableRef(input_array),
                                      new_ref_index));
          void *stmt = codegen->CreateStmt(V_SgExprStatement, one_assign);
          codegen->AppendChild(curr_if_body, stmt);
          local_changed |= true;
        }

        vec_if_body.push_back(curr_if_body);
      }

      // 3.3.4 Build if-statement
      {
        assert(vec_if_cond.size() == vec_if_body.size());

        if (!vec_if_cond.empty()) {
          void *curr_else_stmt = vec_if_body[vec_if_cond.size() - 1];
          for (size_t i = 1; i < static_cast<int>(vec_if_cond.size()); i++) {
            int ii = static_cast<int>(vec_if_cond.size() - 1 - i);
            void *one_cond = vec_if_cond[ii];
            void *one_body = vec_if_body[ii];

            void *curr_if_stmt =
                codegen->CreateIfStmt(one_cond, one_body, curr_else_stmt);
            curr_else_stmt = curr_if_stmt;
          }
          codegen->AppendChild(curr_bb, curr_else_stmt);
        }
      }
    }

    // 3.4 Replace original references
    for (size_t i = 0; i < org_pntr.size(); i++) {
      void *org_ref = org_pntr[i];
      void *new_ref = codegen->CreateVariableRef(ref_decl[i]);
      codegen->ReplaceExp(org_ref, new_ref);
    }

    // 3.5 Deal with write references
    if (!read_only) {
      // create the declaration of the access variables
      vector<void *> write_ref_decl;
      {
        int num_ref = org_write_access.size();
        write_ref_decl.resize(num_ref);
        for (size_t i = 0; i < static_cast<int>(org_write_access.size()); i++) {
          int ii = static_cast<int>(num_ref - 1 - i);
          var_name = s_prefix + "_w" + my_itoa(ii);
          decl = codegen->CreateVariableDecl(input_base_type, var_name, nullptr,
                                             curr_scope);
          codegen->PrependChild(curr_scope, decl, true);
          write_ref_decl[ii] = decl;
        }
      }
      vector<void *> write_pos_stmts;
      vector<int> write_changed;
      for (size_t ii = 0; ii < org_write_pntr.size(); ii++) {
        bool changed = false;
        void *write_ref = org_write_pntr[ii];
        void *write_pos = codegen->TraceUpToStatement(org_write_pntr[ii]);
        write_pos_stmts.push_back(write_pos);
        void *sg_assign = codegen->GetParent(write_ref);

        if (sg_assign != nullptr) {
          void *right = codegen->GetExpRightOperand(sg_assign);

          if (codegen->is_movable_test(right, curr_pos, write_pos) != 0) {
            void *new_write_ref =
                codegen->CreateVariableRef(write_ref_decl[ii]);
            codegen->ReplaceExp(write_ref, new_write_ref);

            for (size_t i = 0; i < bank_cond_combination.size(); i++) {
              // Body Assignement
              // a[factor*addr0 + bank0][...] = ...;
              vector<int> one_case = bank_cond_combination[i];
              assert(one_case.size() == (size_t)input_dim);

              void *curr_if_body = vec_if_body[i];
              void *write_stmt = codegen->CopyStmt(write_pos);
              codegen->AppendChild(curr_if_body, write_stmt);
              local_changed |= true;
              changed |= true;

              // vec_offset [access][dim] -> offset
              //             for (size_t j = 0; j < vec_write_offsets.size();
              //             j++) {
              //  vector<int> offset_of_one_ref = vec_write_offsets[j];
              vector<int> offset_of_one_ref = vec_write_offsets[ii];
              assert(offset_of_one_ref.size() == (size_t)input_dim);

              vector<void *> new_ref_index;
              for (int k = 0; k < input_dim; k++) {
                int n_switch_factor = switch_factor[k];
                if (n_switch_factor == 0) {
                  n_switch_factor = 1;
                }

                void *new_index;
                if (n_switch_factor > 1) {
                  new_index = codegen->CreateExp(
                      V_SgAddOp,
                      codegen->CreateExp(
                          V_SgMultiplyOp,
                          codegen->CreateVariableRef(addr_decl[k]),
                          codegen->CreateConst(n_switch_factor)),
                      codegen->CreateConst(offset_of_one_ref[k] + one_case[k]));
                } else {
                  new_index = codegen->CopyExp(
                      org_write_access[ii].GetIndex(k).get_expr_from_coeff());
                }
                new_ref_index.push_back(new_index);
              }
              void *one_assign = codegen->CreateExp(
                  V_SgAssignOp,
                  codegen->CreateArrayRef(
                      codegen->CreateVariableRef(input_array), new_ref_index),
                  codegen->CreateVariableRef(write_ref_decl[ii]));
              void *stmt = codegen->CreateStmt(V_SgExprStatement, one_assign);
              codegen->AppendChild(curr_if_body, stmt);
              //             }
            }
          }
        }
        write_changed.push_back(
            static_cast<
                std::vector<int, class std::allocator<int>>::value_type>(
                changed));
      }

      for (size_t ii = 0; ii < write_pos_stmts.size(); ii++) {
        if (write_changed[ii] != 0) {
          codegen->RemoveStmt(write_pos_stmts[ii]);
        }
      }
    }
    if (!local_changed) {
      codegen->RemoveStmt(curr_bb);
    }
  }
  return 1;
}
