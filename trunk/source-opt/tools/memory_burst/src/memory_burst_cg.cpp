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


#include "memory_burst.h"
#include <set>
#include <iostream>
#include <string>
#include <tuple>
//  #include "rose.h"
#include "math.h"
#include "array_delinearize.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "stream_ir.h"
#include "analPragmas.h"
#define CHECK_BURST_LENGTH_IS_LOOP_INVARIANT

using std::map;
using std::pair;
using std::set;
using std::tuple;
using std::vector;
//  using namespace SageInterface;
//  using namespace SageBuilder;
using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

extern void remove_parallel_pragma(CSageCodeGen *codegen, CMirNode *node);

enum check_type { NO_CHECK = 0, ANY_SINGLE_ACCESS = 1, ALL_VALID_ACCESS = 2 };

//  Get all the cg-pipeline loops with the topology order in program execution
void MemoryBurst::cg_get_loops_in_topology_order(vector<void *> *cg_scopes) {
  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);

  for (size_t i = 0; i != vec_nodes.size(); ++i) {
    CMirNode *node = vec_nodes[i];
    //  if (node->is_function || node->is_fine_grain ||
    //  Fix Bug1692
    //  if (node->is_function ||
    //    !node->has_opt_pragmas())
    if (node->is_function) {
      continue;
    }
    cg_scopes->push_back(node->ref);
  }
}

//  Get all the fined grained loops with the topology order in program execution
void MemoryBurst::fg_get_loops_in_topology_order(vector<void *> *fg_scopes) {
  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);

  for (size_t i = 0; i != vec_nodes.size(); ++i) {
    CMirNode *node = vec_nodes[i];
    if (node->is_function || !node->is_fine_grain) {
      continue;
    }
    fg_scopes->push_back(node->ref);
  }
}

//    a) Get the absolute UB and LB, if any of them is infinite, ignore
//    b) if upper bound of the length is larger than buffer size threshold,
//    ignore
//    c) if the symbolic start/length has multiple bounds, use the absolute
//    value instead
//    d) if the symbolic start/length is not movable to the insert position, use
//    absolute value instead
void MemoryBurst::cg_check_range_availability(
    void *sg_scope, void *sg_array, int element_size,
    const CMarsRangeExpr &mr_merged, CMarsExpression *me_start,
    CMarsExpression *me_length, int *is_applied, int64_t *buff_size,
    int *read_write, bool *is_non_negative_length, bool *is_exact,
    bool report) {
  *is_applied = BURST_APPLIED;
  CMarsExpression me_lb;
  CMarsExpression me_ub;
  *is_exact = mr_merged.is_exact() != 0;
  string var_name = m_ast->GetVariableName(sg_array);
  auto array_user_info = getUserCodeInfo(m_ast, sg_array);
  if (!array_user_info.name.empty())
    var_name = array_user_info.name;
  map<string, string> msg_map;
  msg_map["burst"] = "off";
  string var_info =
      "\'" + var_name + "\' (" + array_user_info.file_location + ")";
  string scope_info = getUserCodeFileLocation(m_ast, sg_scope, true);
  void *top_array = get_top_arg(sg_array);
  //  a) if the symbolic start/length has multiple bounds, use the absolute
  //  value instead b) if the symbolic start/length is not movable to the insert
  //  position, use absolute value instead
  {
    vector<CMarsExpression> lbs = mr_merged.get_lb_set();
    vector<CMarsExpression> ubs = mr_merged.get_ub_set();

    if (ubs.empty()) {
#if PROJDEBUG
      cout << "Cannot infer burst for variable \'" << var_name
           << "\' in scope \'" << m_ast->_p(sg_scope, 80)
           << "\' because of infinite bound" << endl;
#endif
      string range_info = mr_merged.print_e();
      if (report) {
        msg_map["burst_off_reasons"] = "infinite bound";
        insertMessage(m_ast, top_array, msg_map);
        void *sg_loop = m_ast->GetEnclosingNode("ForLoop", sg_scope);
        string qorMergeMode = getQoRMergeMode(m_ast, sg_loop);
        if (qorMergeMode != "tail") {
          dump_critical_message(
              BURST_WARNING_1(var_info, range_info, scope_info), 0, m_ast,
              top_array);
        }
      }
      *is_applied = INFINITE_RANGE;
      return;
    }

    if (lbs.size() > 1) {
      if (mr_merged.is_const_lb() != 0)
        me_lb = CMarsExpression(m_ast, mr_merged.get_const_lb());
      else
        me_lb = MarsProgramAnalysis::neg_INF;
      *is_exact = false;
    } else if (lbs.size() == 1) {
      me_lb = lbs[0];
    } else {
      //  youxiang 20170212 if there is no lower bound,
      //  set the lower bound is 0
      me_lb = CMarsExpression(m_ast, 0L);
      *is_exact = false;
    }

    if (ubs.size() > 1) {
      if (mr_merged.is_const_ub() != 0)
        me_ub = CMarsExpression(m_ast, mr_merged.get_const_ub());
      else
        me_ub = MarsProgramAnalysis::pos_INF;
      *is_exact = false;
    } else if (ubs.size() == 1) {
      me_ub = ubs[0];
    } else {
      me_ub = MarsProgramAnalysis::pos_INF;
    }

    bool out_loop = m_ast->IsForStatement(sg_scope) != 0;
    if (out_loop) {
      if (!me_lb.IsLoopInvariant(sg_scope) && mr_merged.is_const_lb() != 0) {
        me_lb = CMarsExpression(m_ast, mr_merged.get_const_lb());
        *is_exact = false;
      }
      if (!me_ub.IsLoopInvariant(sg_scope) && mr_merged.is_const_ub() != 0) {
        me_ub = CMarsExpression(m_ast, mr_merged.get_const_ub());
        *is_exact = false;
      }
    } else {
      void *first_stmt = m_ast->GetLocation(m_ast->GetChildStmt(sg_scope, 0));
      if ((me_lb.is_movable_to(first_stmt) == 0 ||
           me_lb.any_var_located_in_scope(sg_scope)) &&
          mr_merged.is_const_lb() != 0) {
        me_lb = CMarsExpression(m_ast, mr_merged.get_const_lb());
        *is_exact = false;
      }
      if ((me_ub.any_var_located_in_scope(sg_scope) ||
           me_ub.is_movable_to(first_stmt) == 0) &&
          mr_merged.is_const_ub() != 0) {
        me_ub = CMarsExpression(m_ast, mr_merged.get_const_ub());
        *is_exact = false;
      }
    }
  }
  //  c) Get the absolute UB and LB, if any of them is infinite, ignore
  //  d) if upper bound of the length is larger than buffer size threshold,
  //  ignore
  //  e) if the length is not always non negative, ignore
  {
    CMarsExpression length = me_ub - me_lb + 1;
    length.set_scope(sg_scope);
    CMarsRangeExpr mr_length = length.get_range();
    if (void *pos = length.get_pos()) {
      mr_length.refine_range_in_scope(nullptr, pos);
    }
    *is_non_negative_length = mr_length.IsNonNegative();
    if (mr_length.is_const_ub() == 0) {
#if PROJDEBUG
      cout << "Cannot infer burst for variable \'" << var_name
           << "\' in scope \'" << m_ast->_p(sg_scope, 80)
           << "\' because of infinite length (";
      cout << length.print_s() << ") of on-chip buffer" << endl;
#endif
      string len_info = length.print_s();
      if (report) {
        msg_map["burst_off_reasons"] = "infinite length";
        insertMessage(m_ast, top_array, msg_map);
        dump_critical_message(BURST_WARNING_4(var_info, len_info), 0, m_ast,
                              top_array);
      }
      *is_applied = UNDETERMINED_LEN;
      return;
    } else {
      int64_t n_ub = mr_length.get_const_ub();
      *me_start = me_lb;
      *me_length = length;
      *buff_size = n_ub;
    }
  }
  if (!*is_exact && ((*read_write & WRITE_FLAG) != 0)) {
    *read_write |= READ_FLAG;
  }
}

int MemoryBurst::cg_transform_refs_in_scope(void *sg_array, void *offset,
                                            void *insert_pos,
                                            void *sg_array_replace, void *scope,
                                            int check_only,
                                            const vector<void *> &vec_offset) {
  if (m_ast->IsLocalInitName(sg_array)) {
    void *var_decl = m_ast->GetVariableDecl(sg_array);
    if (m_ast->IsVariableDecl(var_decl) && m_ast->IsInScope(var_decl, scope)) {
      return 0;
    }
  }
  //  1. Get all the reference of the array init name
  vector<void *> vec_refs;

  m_ast->get_ref_in_scope(sg_array, scope, &vec_refs);

  if (vec_refs.empty()) {
    return 1;
  }

  //  2. Check if all the references are
  //    a) full dim access, or
  //    b) directly used as a function call argument
  map<void *, void *> old_ref2new_ref;
  std::reverse(vec_refs.begin(), vec_refs.end());
  for (auto ref : vec_refs) {
    if (m_ast->is_write_conservative(ref, false) != 0) {
      if (check_only == ALL_VALID_ACCESS) {
        return 0;
      }
    }
    void *pntr = nullptr;
    void *array = nullptr;
    vector<void *> indexes;
    int pointer_dim;
    void *base_type = nullptr;
    if (m_ast->parse_standard_pntr_ref_from_array_ref(
            ref, &array, &pntr, &base_type, &indexes, &pointer_dim, ref) == 0) {
      if (check_only == ALL_VALID_ACCESS) {
        return 0;
      }
    }

    int arg_idx = m_ast->GetFuncCallParamIndex(pntr);
    if (pointer_dim == 0 && -1 == arg_idx) {
      if (check_only == ALL_VALID_ACCESS) {
#ifdef PROJDEBUG
        cout << "unsupported references: pointer alias " << m_ast->_p(pntr, 0)
             << endl;
#endif
        return 0;
      }
      if (check_only == ANY_SINGLE_ACCESS) {
        return 1;
      }
    } else if (pointer_dim == 0 && -1 != arg_idx) {
      void *func_call = m_ast->TraceUpToFuncCall(pntr);
      assert(m_ast->IsFunctionCall(func_call));
      string sub_func_name = m_ast->GetFuncNameByCall(func_call);
      if (sub_func_name.find("memcpy") == 0) {
        if (check_only == ALL_VALID_ACCESS) {
#ifdef PROJDEBUG
          cout << "unsupported references: memcpy " << m_ast->_p(func_call, 0)
               << endl;
#endif
          return 0;
        }
        if (check_only == ANY_SINGLE_ACCESS) {
          continue;
        }
      }
      if (m_ast->IsMerlinInternalIntrinsic(sub_func_name)) {
        continue;
      }
    } else if (pointer_dim == 1) {
      if ((m_ast->IsPntrArrRefExp(pntr) == 0) &&
          (m_ast->IsPointerDerefExp(pntr) == 0)) {
        if (check_only == ALL_VALID_ACCESS) {
#ifdef PROJDEBUG
          cout << "unsupported references: non-pntr or pointer deref "
               << m_ast->_p(pntr, 0) << endl;
#endif
          return 0;
        }
        if (check_only == ANY_SINGLE_ACCESS) {
          return 1;
        }
      } else {
        bool is_manual_burst =
            m_ast->detect_user_for_loop_burst(ref, pntr, &mMars_ir);
        if (check_only == ANY_SINGLE_ACCESS && !is_manual_burst) {
          return 1;
        }
#if 0
        if (check_only == ALL_VALID_ACCESS && is_manual_burst) {
#ifdef PROJDEBUG
          cout << "detect manual burst: " << m_ast->_p(pntr, 0) << endl;
#endif
          return 0;
        }
#endif
      }
    } else if (pointer_dim >= 2) {
      return 0;  //  only handle 1-D array for interface
    }
    if (check_only != 0) {
      if ((offset != nullptr) &&
          (m_ast->is_movable_test(offset, pntr, insert_pos) == 0)) {
        if (check_only == ALL_VALID_ACCESS) {
#ifdef PROJDEBUG
          cout << "unsupported references: offset (" << m_ast->_p(offset, 0)
               << ") cannot pass movable test (" << m_ast->_p(pntr, 0) << ")"
               << endl;
#endif
          return 0;
        }
        if (check_only == ANY_SINGLE_ACCESS) {
          return 1;
        }
      }
    }

    if (pointer_dim == 0 && -1 != arg_idx) {
      void *func_call = m_ast->TraceUpToFuncCall(pntr);
      string str_name = m_ast->GetFuncNameByCall(func_call);
      /*        if (str_name.find("merlin_stream_reset") != string::npos ||
                      str_name.find("merlin_stream_init") != string::npos ||
                      str_name.find("merlin_stream_write") != string::npos ||
              str_name.find("merlin_stream_read") != string::npos)
                  continue;
      */
      void *func_decl = m_ast->GetFuncDeclByCall(func_call);
      if (func_decl != nullptr) {
        void *func_body = m_ast->GetFuncBody(func_decl);
        assert(func_body);

        void *new_array = m_ast->GetFuncParam(func_decl, arg_idx);
        vector<void *> new_vec_offset;
        int ret =
            cg_transform_refs_in_scope(new_array, nullptr, insert_pos, nullptr,
                                       func_body, check_only, new_vec_offset);

        if ((ret != 0) && check_only == ANY_SINGLE_ACCESS) {
          return 1;
        }
      }
    }

    if (check_only != 0) {
      continue;
    }
    if (pointer_dim <= 1 && (offset != nullptr)) {
      CMarsAccess access(ref, m_ast, scope, pntr);
      CMarsExpression me_idx = access.GetIndex(0);
      CMarsExpression me_offset(offset, m_ast, me_idx.get_pos(), scope);
      CMarsExpression me_new = me_idx - me_offset;
      if (((me_offset != 0) != 0) && ((me_idx >= me_offset) == 0)) {
        if (pointer_dim == 0 && -1 != arg_idx) {
#ifdef PROJDEBUG
          cout << "unsupported references: new offset (" << me_new.print_s()
               << ") may be negative (" << m_ast->_p(pntr, 0) << ")" << endl;
#endif
          void *func_call = m_ast->TraceUpToFuncCall(pntr);
          void *func_decl = m_ast->GetFuncDeclByCall(func_call);
          if (func_decl != nullptr) {
            void *new_array = m_ast->GetFuncParam(func_decl, arg_idx);
            add_offset_later(func_decl, new_array, func_call,
                             me_new.get_expr_from_coeff(true));
            me_new = CMarsExpression(m_ast, 0L);
          }
        }
      }
      if (sg_array == nullptr) {
        continue;
      }
      void *new_ref = nullptr;
      if (sg_array_replace != nullptr) {
        new_ref = m_ast->CreateVariableRef(sg_array_replace);
      } else {
        new_ref = m_ast->CreateVariableRef(sg_array);
      }

      if (!vec_offset.empty()) {
        for (auto one_offset : vec_offset) {
          new_ref = m_ast->CreateExp(V_SgPntrArrRefExp, new_ref,
                                     m_ast->CopyExp(one_offset));
        }
      }
      if ((sg_array_replace != nullptr) || ((me_offset != 0) != 0)) {
        void *orig_type = m_ast->GetTypeByExp(pntr);
        if (pointer_dim == 0 && ((me_new == 0) != 0)) {
          //  m_ast->ReplaceExp(pntr, new_ref);
          if ((m_ast->IsCompatibleType(orig_type, m_ast->GetTypeByExp(new_ref),
                                       pntr, true) == 0) &&
              (m_ast->IsPointerType(orig_type) != 0)) {
            new_ref = m_ast->CreateExp(V_SgCastExp, new_ref, orig_type);
          }
          old_ref2new_ref[pntr] = new_ref;
        } else {
          void *new_index = me_new.get_expr_from_coeff(true);
          void *new_pntr =
              m_ast->CreateExp(V_SgPntrArrRefExp, new_ref, new_index);
          if (pointer_dim == 0) {
            new_pntr = m_ast->CreateExp(V_SgAddressOfOp, new_pntr);
          }
          //  m_ast->ReplaceExp(pntr, new_pntr);
          if ((m_ast->IsCompatibleType(orig_type, m_ast->GetTypeByExp(new_pntr),
                                       pntr, true) == 0) &&
              (m_ast->IsPointerType(orig_type) != 0)) {
            new_pntr = m_ast->CreateExp(V_SgCastExp, new_pntr, orig_type);
          }
          old_ref2new_ref[pntr] = new_pntr;
        }
      }
      //  void * index = m_ast->GetExpRightOperand(pntr);
      //  CMarsExpression me_idx(index, m_ast);
      //  void * new_index = m_ast->CreateExp(V_SgSubtractOp,
      //        m_ast->CopyExp(index),
      //        m_ast->CopyExp(offset));
      //  m_ast->ReplaceExp(index, new_index);
    } else if (offset == nullptr) {
      continue;
    } else {
#if PROJDEBUG
      assert(0);
#endif
    }
  }

  for (auto pair : old_ref2new_ref) {
    m_ast->ReplaceExp(pair.first, pair.second);
  }

  if (check_only == ALL_VALID_ACCESS) {
    return 1;
  }
  if (check_only == ANY_SINGLE_ACCESS) {
    return 0;
  }
  return 1;
}

int MemoryBurst::cg_check_single_reference_availability(void *sg_scope,
                                                        void *sg_array) {
  int check_only = ANY_SINGLE_ACCESS;
  bool in_body = m_ast->IsBasicBlock(sg_scope) != 0;
  void *insert_pos = sg_scope;
  if (in_body) {
    insert_pos = m_ast->GetLocation(m_ast->GetChildStmt(sg_scope, 0));
  }
  void *start_index = m_ast->CreateConst(0);
  vector<void *> vec_offset;
  int ret =
      cg_transform_refs_in_scope(sg_array, start_index, insert_pos, nullptr,
                                 sg_scope, check_only, vec_offset);
  m_ast->DeleteNode(start_index);
  return ret;
}

int MemoryBurst::cg_check_reference_availability(
    void *sg_scope, void *sg_array, const CMarsExpression &me_start) {
  int check_only = ALL_VALID_ACCESS;
  bool in_body = m_ast->IsBasicBlock(sg_scope) != 0;
  void *insert_pos = sg_scope;
  if (in_body) {
    insert_pos = m_ast->GetLocation(m_ast->GetChildStmt(sg_scope, 0));
  }
  void *start_index = me_start.get_expr_from_coeff();
  vector<void *> vec_offset;
  int ret =
      cg_transform_refs_in_scope(sg_array, start_index, insert_pos, nullptr,
                                 sg_scope, check_only, vec_offset);
  m_ast->DeleteNode(start_index);
  return ret;
}

int MemoryBurst::cg_check_delinearize_availability(void *sg_scope,
                                                   void *sg_array,
                                                   CMarsExpression me_start) {
  vector<size_t> dim_split_steps;
  map<void *, size_t> mapAlias2BStep;
  bool is_simple = true;
  int ret = analysis_delinearize(m_ast, sg_array, 0, &dim_split_steps,
                                 &mapAlias2BStep, &is_simple, &me_start,
                                 sg_scope);
  if (is_simple) {
    return 1;
  }
  if (ret != 0) {
    map<void *, vector<CMarsExpression>> mapIndex2Delinear;
    vector<size_t> new_array_dims;
    ret = apply_delinearize(m_ast, sg_array, 0, dim_split_steps,
                            &mapAlias2BStep, true, &is_simple, &me_start,
                            sg_scope, &mapIndex2Delinear, &new_array_dims);
  }
  return static_cast<int>(is_simple && (ret) != 0);
}

bool MemoryBurst::cg_transform_burst(
    void *sg_scope, void *sg_array, const CMarsExpression &me_merged_start,
    const CMarsExpression &me_merged_length, bool is_non_negative_merged_len,
    bool is_merge_exact, const CMarsExpression &me_write_start,
    const CMarsExpression &me_write_length, bool is_non_negative_write_len,
    bool is_write_exact, int read_write, void **buf_decl, void **memcpy_r,
    void **memcpy_w, int64_t *merged_buffer_size,
    bool check_constant_burst_length, bool report, int element_size,
    bool *aggressive_write_only) {
  if (mMars_ir_v2.get_write_only_flag(sg_array, sg_scope)) {
    if (read_write != WRITE_FLAG) {
      *aggressive_write_only = true;
    }
    read_write = WRITE_FLAG;
  }

  void *func_decl = m_ast->TraceUpToFuncDecl(sg_scope);
  bool in_body = m_ast->IsBasicBlock(sg_scope) != 0;
  void *scope_body = sg_scope;
  void *insert_pos = sg_scope;

  if (!in_body) {
    scope_body = m_ast->TraceUpToBasicBlock(m_ast->GetParent(sg_scope));
  } else {
    insert_pos = m_ast->GetLocation(m_ast->GetChildStmt(sg_scope, 0));
  }

  bool is_non_negative_merged_start = true;
  *merged_buffer_size = 0;
  void *merged_burst_len = nullptr;
  void *merged_burst_start_expr = nullptr;
  refine_burst_start_and_length(
      me_merged_length, me_merged_start, insert_pos, sg_scope, sg_array,
      !is_merge_exact, merged_buffer_size, &merged_burst_len,
      &merged_burst_start_expr, &is_non_negative_merged_len,
      &is_non_negative_merged_start);
  if (check_constant_burst_length) {
    CMarsExpression me_merged_burst_len(merged_burst_len, m_ast, insert_pos);
    if (me_merged_burst_len.IsConstant() == 0) {
      return false;
    }
  }
  if (!is_non_negative_merged_start) {
    reportPossibleNegativeAccessStart(me_merged_start, sg_array, sg_scope);
  }

  bool is_non_negative_write_start = true;
  int64_t write_buffer_size = 0;
  void *write_burst_len = nullptr;
  void *write_burst_start_expr = nullptr;
  if ((read_write & WRITE_FLAG) != 0) {
    if ((read_write & READ_FLAG) != 0) {
      refine_burst_start_and_length(
          me_write_length, me_write_start, insert_pos, sg_scope, sg_array,
          !is_write_exact, &write_buffer_size, &write_burst_len,
          &write_burst_start_expr, &is_non_negative_write_len,
          &is_non_negative_write_start);
    } else {
      is_non_negative_write_len = is_non_negative_merged_len;
      write_buffer_size = *merged_buffer_size;
      write_burst_len = merged_burst_len;
      write_burst_start_expr = merged_burst_start_expr;
    }
    if (check_constant_burst_length) {
      CMarsExpression me_write_burst_len(write_burst_len, m_ast, insert_pos);
      if (me_write_burst_len.IsConstant() == 0) {
        return false;
      }
    }
  }
  if (*merged_buffer_size <= 0) {
    return false;
  }
  //  4.1 allocate local array
  void *basic_type = m_ast->GetBaseType(m_ast->GetTypebyVar(sg_array), false);
  void *buf_type = m_ast->CreateArrayType(
      basic_type, vector<size_t>(1, *merged_buffer_size));
  string buf_name = m_ast->GetVariableName(sg_array) + "_buf";
  m_ast->get_valid_name(scope_body, &buf_name);
  *buf_decl = m_ast->CreateVariableDecl(buf_type, buf_name, nullptr, scope_body,
                                        sg_array);
  m_ast->PrependChild(scope_body, *buf_decl, true);

  void *top_arg = get_top_arg(sg_array);
  if (mMars_ir_v2.get_coalescing_flag(top_arg)) {
    string coalescing_force_pragma_str =
        std::string(ACCEL_PRAGMA) + " " + CMOST_coalescing + " " +
        CMOST_variable + "=" + buf_name + " " + CMOST_force + "=" + CMOST_ON;
    void *coalescing_force_pragma =
        m_ast->CreatePragma(coalescing_force_pragma_str, scope_body);
    m_ast->InsertStmt(coalescing_force_pragma, *buf_decl);
  }

  m_ast->PropagatePragma(sg_array, sg_scope, buf_name);
  //  4.2 replace original accesses
  {
    int check_only = NO_CHECK;
    vector<void *> vec_offset;
    cg_transform_refs_in_scope(sg_array, merged_burst_start_expr, insert_pos,
                               *buf_decl, sg_scope, check_only, vec_offset);
  }

  //  4.3 create memcpy
  void *read_burst;
  void *write_burst;
  read_burst = write_burst = nullptr;
  void *read_bb;
  void *write_bb;
  read_bb = write_bb = nullptr;
  void *sizeof_expr_read, *sizeof_expr_write;
  sizeof_expr_read = sizeof_expr_write = nullptr;
  for (int i = 0; i != 2; ++i) {
    bool read_codegen = i == 0 && ((read_write & READ_FLAG) != 0);
    bool write_codegen = i == 1 && ((read_write & WRITE_FLAG) != 0);
    if (!read_codegen && !write_codegen) {
      continue;
    }
    if (read_codegen) {
      read_bb = m_ast->CreateBasicBlock();
      m_ast->InsertStmt(read_bb, update_position(insert_pos));
    } else {
      write_bb = m_ast->CreateBasicBlock();
      if (!in_body) {
        m_ast->InsertAfterStmt(write_bb, sg_scope);
      } else {
        //  FIXME: need to check whether there is write access
        //  within the return statement
        int num = m_ast->GetChildStmtNum(scope_body);
        assert(num > 0);
        void *last_stmt = m_ast->GetChildStmt(scope_body, num - 1);
        if (m_ast->IsReturnStatement(last_stmt) != 0) {
          m_ast->InsertStmt(write_bb, update_position(last_stmt));
        } else {
          m_ast->InsertAfterStmt(write_bb, last_stmt);
        }
      }
    }
  }
  void *assert_stmt_for_read = nullptr;
  void *assert_stmt_for_write = nullptr;
  for (int i = 0; i != 2; ++i) {
    bool read_codegen = i == 0 && ((read_write & READ_FLAG) != 0);
    bool write_codegen = i == 1 && ((read_write & WRITE_FLAG) != 0);
    if (!read_codegen && !write_codegen) {
      continue;
    }
    void *burst_start_expr =
        read_codegen ? merged_burst_start_expr : write_burst_start_expr;
    void *burst_len = read_codegen ? merged_burst_len : write_burst_len;
    CMarsExpression me_actual_len(burst_len, m_ast, insert_pos);
    void *assert_stmt = nullptr;
    if (me_actual_len.IsConstant() == 0) {
      void *burst_len_var = createTempVariableWithBoundAssertion(
          func_decl, read_codegen ? read_bb : write_bb, burst_len, "merlin_len",
          read_codegen ? *merged_buffer_size : write_buffer_size,
          read_codegen ? is_non_negative_merged_len : is_non_negative_write_len,
          &assert_stmt);
      burst_len = m_ast->CreateVariableRef(burst_len_var);
    }
    if (read_codegen) {
      assert_stmt_for_read = assert_stmt;
    } else {
      assert_stmt_for_write = assert_stmt;
    }
    void *offset_start_expr = nullptr;
    if (write_codegen && ((me_merged_start != me_write_start) != 0)) {
      offset_start_expr = m_ast->CreateExp(
          V_SgSubtractOp, m_ast->CopyExp(write_burst_start_expr),
          m_ast->CopyExp(merged_burst_start_expr));
    }
    void *target_expr =
        m_ast->CreateExp(V_SgPntrArrRefExp, m_ast->CreateVariableRef(*buf_decl),
                         offset_start_expr != nullptr ? offset_start_expr
                                                      : m_ast->CreateConst(0));
    void *target_exp_addr = m_ast->CreateExp(V_SgAddressOfOp, target_expr);
    void *source_expr =
        m_ast->CreateExp(V_SgPntrArrRefExp, m_ast->CreateVariableRef(sg_array),
                         burst_start_expr);
    void *source_exp_addr = m_ast->CreateExp(V_SgAddressOfOp, source_expr);
    void *sizeof_expr = m_ast->CreateExp(
        V_SgMultiplyOp, m_ast->CreateExp(V_SgSizeOfOp, basic_type),
        i == 0 ? burst_len : m_ast->CopyExp(burst_len));
    if (read_codegen) {
      vector<void *> sg_read_init_list;
      sg_read_init_list.push_back(m_ast->CreateCastExp(
          target_exp_addr, m_ast->GetTypeByString("void *")));
      sg_read_init_list.push_back(m_ast->CreateCastExp(
          source_exp_addr, m_ast->GetTypeByString("const void *")));
      sg_read_init_list.push_back(sizeof_expr);
      void *sg_read_func_call = m_ast->CreateFuncCall(
          "memcpy", m_ast->GetTypeByString("void"), sg_read_init_list,
          scope_body, get_top_arg(sg_array));
      read_burst = sg_read_func_call;
      *memcpy_r = m_ast->CreateStmt(V_SgExprStatement, sg_read_func_call);
      sizeof_expr_read = sizeof_expr;
    }
    if (write_codegen) {
      vector<void *> sg_write_init_list;
      sg_write_init_list.push_back(m_ast->CreateCastExp(
          source_exp_addr, m_ast->GetTypeByString("void *")));
      sg_write_init_list.push_back(m_ast->CreateCastExp(
          target_exp_addr, m_ast->GetTypeByString("const void *")));
      sg_write_init_list.push_back(sizeof_expr);
      void *sg_write_func_call = m_ast->CreateFuncCall(
          "memcpy", m_ast->GetTypeByString("void"), sg_write_init_list,
          scope_body, get_top_arg(sg_array));
      write_burst = sg_write_func_call;
      *memcpy_w = m_ast->CreateStmt(V_SgExprStatement, sg_write_func_call);
      sizeof_expr_write = sizeof_expr;
    }
  }

  if (*memcpy_r != nullptr) {
    if (!is_non_negative_merged_len) {
      void *cond = m_ast->CreateExp(
          V_SgGreaterOrEqualOp,
          m_ast->CreateSignnesCast(m_ast->CopyExp(merged_burst_len)),
          m_ast->CreateConst(0));
      void *true_body = m_ast->CreateBasicBlock();
      if (assert_stmt_for_read != nullptr) {
        m_ast->AppendChild(true_body, assert_stmt_for_read);
      }
      m_ast->AppendChild(true_body, *memcpy_r);
      *memcpy_r = m_ast->CreateIfStmt(cond, true_body, nullptr);
    }
    if (m_ast->GetChildStmtNum(read_bb) > 0) {
      m_ast->AppendChild(read_bb, *memcpy_r);
    } else {
      m_ast->ReplaceStmt(read_bb, *memcpy_r);
    }
    if (read_burst != nullptr) {
      if (report) {
        map<string, string> msg_map;
        msg_map["display_name"] =
            "auto memory burst for '" +
            m_ast->GetVariableName(get_top_arg(sg_array)) + "'";
        msg_map["burst"] = "on";
        msg_map["is_read"] = "1";
        msg_map["burst_length"] =
            CMarsExpression(sizeof_expr_read, m_ast, insert_pos).print_s();
        msg_map["burst_length_max"] =
            my_itoa(*merged_buffer_size * element_size);
        insertMessage(m_ast, read_burst, msg_map);
      }
      setNoHierarchy(m_ast, read_burst, 1);
    }
  }
  if (*memcpy_w != nullptr) {
    if (!is_non_negative_write_len) {
      void *cond = m_ast->CreateExp(
          V_SgGreaterOrEqualOp,
          m_ast->CreateSignnesCast(m_ast->CopyExp(write_burst_len)),
          m_ast->CreateConst(0));
      void *true_body = m_ast->CreateBasicBlock();
      if (assert_stmt_for_write != nullptr) {
        m_ast->AppendChild(true_body, assert_stmt_for_write);
      }
      m_ast->AppendChild(true_body, *memcpy_w);
      *memcpy_w = m_ast->CreateIfStmt(cond, true_body, nullptr);
    }
    if (m_ast->GetChildStmtNum(write_bb) > 0) {
      m_ast->AppendChild(write_bb, *memcpy_w);
    } else {
      m_ast->ReplaceStmt(write_bb, *memcpy_w);
    }
    if (write_burst != nullptr) {
      if (report) {
        map<string, string> msg_map;
        msg_map["display_name"] =
            "auto memory burst for '" +
            m_ast->GetVariableName(get_top_arg(sg_array)) + "'";
        msg_map["burst"] = "on";
        msg_map["is_write"] = "1";
        msg_map["burst_length"] =
            CMarsExpression(sizeof_expr_write, m_ast, insert_pos).print_s();
        msg_map["burst_length_max"] = my_itoa(write_buffer_size * element_size);
        insertMessage(m_ast, write_burst, msg_map);
      }
      setNoHierarchy(m_ast, write_burst, 1);
    }
  }
  return true;
}

bool MemoryBurst::cg_get_merged_access_range(void *sg_scope, void *sg_array,
                                             CMarsRangeExpr *mr_merged,
                                             CMarsRangeExpr *mr_write,
                                             int *read_write,
                                             int64_t *access_size) {
  ArrayRangeAnalysis mar(sg_array, m_ast, sg_scope, sg_scope,
                         access_size != nullptr, true);
  bool is_dataflow_process =
      m_ast->IsDataflowProcess(m_ast->TraceUpToFuncDecl(sg_scope));
  if (mar.has_read() != 0) {
    assert(mar.GetRangeExprRead().size() == 1);
    vector<CMarsRangeExpr> vec_mr = mar.GetRangeExprRead();
    assert(vec_mr.size() == 1);  //  FIXME: assume 1-D only
    *mr_merged = vec_mr[0];
  }
  if (mar.has_write() != 0) {
    assert(mar.GetRangeExprWrite().size() == 1);
    vector<CMarsRangeExpr> vec_mr = mar.GetRangeExprWrite();
    assert(vec_mr.size() == 1);  //  FIXME: assume 1-D only
    CMarsRangeExpr mr = vec_mr[0];
    *mr_write = mr;
    if (mar.has_read() != 0) {
      *mr_merged = mr_merged->Union(mr);
      *read_write |= READ_WRITE_FLAG;
    } else {
      *mr_merged = mr;
      if (mr_merged->is_exact() != 0) {
        *read_write |= WRITE_FLAG;
      } else {
        if (is_dataflow_process) {
          // do not infer read-write burst for write-only interface in the
          // dataflow scope
          return false;
        }
        *read_write |= READ_WRITE_FLAG;
      }
    }
  }
  if (access_size != nullptr) {
    *access_size = mar.get_access_size();
  }
#if PROJDEBUG
  cout << "access range for variable \'" + m_ast->GetVariableName(sg_array) +
              "\' "
       << "within scope \'" + m_ast->_up(sg_scope) + "\':  " << endl;
  cout << mr_merged->print_e() << endl;
#endif
  return true;
}

void MemoryBurst::cg_reportMessage(void *array, void *scope,
                                   int64_t buffer_size, int element_bytes,
                                   string kind, bool aggressive_write_only) {
  void *sg_loop = m_ast->GetEnclosingNode("ForLoop", scope);
  string qorMergeMode = getQoRMergeMode(m_ast, sg_loop);
  if (qorMergeMode == "tail")
    return;
  string name = m_ast->GetVariableName(array);
  auto array_user_info = getUserCodeInfo(m_ast, array);
  if (!array_user_info.name.empty())
    name = array_user_info.name;
  string sFile = m_ast->get_file(array);
  void *top_array = get_top_arg(array);
  cout << "Memory burst inferred: variable \'" << name << "\' for " << kind
       << " ";

  if (m_ast->IsLoopStatement(scope) != 0) {
    cout << "(" << sFile << ")" << endl;
  } else {
    cout << "(" << sFile << ")" << endl;
  }
  cout << "  burst buffer size = " << (buffer_size * element_bytes)
       << " bytes\n";

  string var_info = "\'" + name + "\' (" + array_user_info.file_location + ")";

  map<string, string> msg_map;
  msg_map["burst"] = "on";
  insertMessage(m_ast, top_array, msg_map);
  int64_t size_in_bytes = buffer_size * element_bytes;
  string buffer_size_info = my_itoa(size_in_bytes);
  string scope_info = getUserCodeFileLocation(m_ast, scope, true);
  if (check_whole_resource_limit(buffer_size, element_bytes)) {
    reportOutofResourceLimit(array, scope, buffer_size * element_bytes);
  } else {
    dump_critical_message(BURST_INFO_1(var_info, scope_info, buffer_size_info));
  }
  if (aggressive_write_only) {
    dump_critical_message(BURST_WARNING_21(var_info, scope_info));
  }
  if (static_cast<size_t>(size_in_bytes) <= m_length_threshold) {
    bool has_tiled_loop = mMars_ir_v2.is_tiled_loop(scope);
    vector<size_t> dim_sizes = mMars_ir_v2.get_array_depth(top_array);
    size_t total_size = 1;
    for (auto dim : dim_sizes) {
      total_size *= dim;
    }
    bool is_full_access =
        total_size > 0 && (total_size * element_bytes == size_in_bytes);
    if (!is_full_access) {
      dump_critical_message(BURST_WARNING_15(var_info, buffer_size_info,
                                             scope_info, has_tiled_loop,
                                             is_full_access),
                            0, m_ast, get_top_arg(array));
    }
  }
  if (m_current_total_usage <
      std::numeric_limits<size_t>::max() - size_in_bytes) {
    m_current_total_usage += size_in_bytes;
  } else {
    m_current_total_usage = std::numeric_limits<size_t>::max();
  }
  mMars_ir_v2.clear_port_is_bus_cache();
  mMars_ir.clear_port_is_bus_cache();
}

bool MemoryBurst::process_coarse_grained_top() {
  vector<void *> cg_scopes;
  cg_get_loops_in_topology_order(&cg_scopes);
  bool Changed = false;
  //  1. lifting burst for coarse grained paralleled loop
  //  infer coarse grained paralleled burst
  //  conditions:
  //  1. current scope is a coarse grained paralleled loop,
  //  2. in the loop body, the access range is not continuous or has some
  //  overlap or not equal size
  cout << "lifting burst for coarse grained paralleld loop: " << endl;
  for (size_t i = 0; i < cg_scopes.size(); i++) {
    void *curr_scope = cg_scopes[i];
    CMirNode *node = mMars_ir.get_node(curr_scope);
    if ((node == nullptr) || node->is_fine_grain ||
        (node->has_parallel() == 0)) {
      continue;
    }
    curr_scope = m_ast->GetParent(curr_scope);
    //  1. Get all the initialized names of the interface arrays of the current
    //  scope
    vector<void *> vec_refs;
    m_ast->GetNodesByType(curr_scope, "preorder", "SgVarRefExp", &vec_refs);

    set<void *> set_arrays;
    vector<void *> vec_arrays;
    void *func = m_ast->TraceUpToFuncDecl(curr_scope);
    for (auto ref : vec_refs) {
      void *var_init = m_ast->GetVariableInitializedName(ref);
      if (set_arrays.count(var_init) > 0) {
        continue;
      }
      if (m_ast->is_located_in_scope_simple(
              curr_scope, m_ast->TraceUpToScope(var_init)) == 0) {
        continue;
      }
      set_arrays.insert(var_init);
      if (is_burst_candidate(var_init, func, ref, false)) {
        vec_arrays.push_back(var_init);
      }
    }

    bool valid = true;
    string scope_info = getUserCodeFileLocation(m_ast, curr_scope, true);
    for (size_t j = 0; j < vec_arrays.size(); j++) {
      void *curr_array = vec_arrays[j];
      string msg;
      int is_para_applied =
          is_cg_parallel_available(curr_array, curr_scope, &msg);
      if (is_para_applied == 0) {
#if PROJDEBUG
        cout << "cannot infer cg parallel burst for varaible \'"
             << m_ast->GetVariableName(curr_array) << "\' in scope \'"
             << m_ast->_p(curr_scope, 80) << endl;
#endif
        string name = m_ast->GetVariableName(curr_array);
        auto array_user_info = getUserCodeInfo(m_ast, curr_array);
        if (!array_user_info.name.empty())
          name = array_user_info.name;
        string var_info =
            "\'" + name + "\' (" + array_user_info.file_location + ")";

        dump_critical_message(CGPAR_WARNING_13(var_info, scope_info, msg), 0,
                              m_ast, get_top_arg(curr_array));
        valid = false;
        break;
      }
    }
    if (!valid) {
      map<string, string> message;
      message["coarse_grained_parallel"] = "off";
      message["coarse_grained_parallel_off_reason"] =
          "External memory access cannot be parallelized";
      insertMessage(m_ast, curr_scope, message);
      remove_parallel_pragma(m_ast, node);
      continue;
    }

    if (valid) {
      for (auto curr_array : vec_arrays) {
        Changed |=
            cg_memory_burst_transform_for_parallel(curr_scope, curr_array);
      }
    }
  }

  cout << "lifting memory burst for general issues: " << endl;
  set<pair<void *, void *>> visited_pairs;
  set<pair<void *, void *>> cached_above_pairs;
  set<tuple<void *, void *, bool>> burst_candidate_pairs;
  vector<tuple<void *, void *, bool>> vec_burst_candidate_pairs;
  set<pair<void *, void *>> non_leaf_pairs;
  map<void *, map<void *, pair<vector<void *>, vector<void *>>>>
      scope_to_array_to_upper_analysis;
  for (size_t i = cg_scopes.size(); i > 0; i--) {
    void *curr_scope = cg_scopes[i - 1];

    //  1. Get all the initialized names of the interface arrays of the current
    //  scope
    vector<void *> vec_refs;
    m_ast->GetNodesByType(curr_scope, "preorder", "SgVarRefExp", &vec_refs);

    set<void *> set_arrays;
    vector<void *> vec_arrays;
    void *func = m_ast->TraceUpToFuncDecl(curr_scope);
    for (auto ref : vec_refs) {
      void *var_init = m_ast->GetVariableInitializedName(ref);
      if (set_arrays.count(var_init) > 0) {
        continue;
      }
      if (m_ast->is_located_in_scope_simple(
              curr_scope, m_ast->TraceUpToScope(var_init)) == 0) {
        continue;
      }
      set_arrays.insert(var_init);
      if (is_burst_candidate(var_init, func, ref, false)) {
        vec_arrays.push_back(var_init);
      }
    }
    for (auto curr_array : vec_arrays) {
      if (non_leaf_pairs.count(std::make_pair(curr_scope, curr_array)) > 0) {
        continue;
      }
      //  2. Get the surrounding for-loops of the pipelined loop and the array
      //  init_name
      //    It is an across-function analysis, the init_name is updated when
      //    crossing functions upward
      //    Stop tracing upward at
      //    a) kernel top function arrived
      //    b) actual argument is not simple pointer (e.g. alias or expressions)
      //    c) multiple function calls (currently as is, to be enhanced)
      vector<void *> vec_upper_scope;  //  from inner to outer
      vector<void *> vec_array_in_upper_scope;
      cg_get_options_for_lifting(curr_scope, curr_array, &vec_upper_scope,
                                 &vec_array_in_upper_scope);
      scope_to_array_to_upper_analysis[curr_scope][curr_array] =
          std::make_pair(vec_upper_scope, vec_array_in_upper_scope);
      size_t k;
      for (k = 1; k < vec_upper_scope.size(); ++k) {
        non_leaf_pairs.insert(
            std::make_pair(vec_upper_scope[k], vec_array_in_upper_scope[k]));
      }
      //  YX: bug2102 limit burst in tiled loop
      size_t upper_size = vec_upper_scope.size();
      for (k = 0; k < vec_upper_scope.size(); k++) {
        void *lift_scope = vec_upper_scope[k];
        if (mMars_ir_v2.is_tiled_loop(lift_scope)) {
          upper_size = k + 1;
        }
        if (m_cached_scope.count(lift_scope) > 0) {
          void *lift_array = vec_array_in_upper_scope[k];
          if (m_cached_scope[lift_scope].count(lift_array) > 0) {
            while (++k < vec_upper_scope.size()) {
              cached_above_pairs.insert(std::make_pair(
                  vec_upper_scope[k], vec_array_in_upper_scope[k]));
            }
          }
        }
      }
      for (k = upper_size; k < vec_upper_scope.size(); k++) {
        cached_above_pairs.insert(
            std::make_pair(vec_upper_scope[k], vec_array_in_upper_scope[k]));
      }
    }
  }
  for (auto curr_scope : cg_scopes) {
    vector<void *> vec_refs;
    m_ast->GetNodesByType(curr_scope, "preorder", "SgVarRefExp", &vec_refs);

    set<void *> set_arrays;
    vector<void *> vec_arrays;
    void *func = m_ast->TraceUpToFuncDecl(curr_scope);
    for (auto ref : vec_refs) {
      void *var_init = m_ast->GetVariableInitializedName(ref);
      if (set_arrays.count(var_init) > 0) {
        continue;
      }
      if (m_ast->is_located_in_scope_simple(
              curr_scope, m_ast->TraceUpToScope(var_init)) == 0) {
        continue;
      }
      set_arrays.insert(var_init);
      if (is_burst_candidate(var_init, func, ref, false)) {
        vec_arrays.push_back(var_init);
      }
    }
    for (auto curr_array : vec_arrays) {
      if (non_leaf_pairs.count(std::make_pair(curr_scope, curr_array)) > 0) {
        continue;
      }
      void *element_type =
          m_ast->GetBaseType(m_ast->GetTypebyVar(curr_array), true);
      int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
      vector<void *> &vec_upper_scope =
          scope_to_array_to_upper_analysis[curr_scope][curr_array].first;
      vector<void *> &vec_array_in_upper_scope =
          scope_to_array_to_upper_analysis[curr_scope][curr_array].second;
      bool start_from_fine_grain =
          mMars_ir.is_fine_grained_scope(m_ast, curr_scope);
      void *scope_to_lift = nullptr;
      void *array_to_lift = nullptr;
      void *cached_scope = nullptr;
      void *cached_array = nullptr;
      int64_t pre_buff_size = 0;
      {
        size_t pre_ref_size = 0;
        size_t k;
        size_t burst_level = 0;
        //  YX: bug2102 limit burst in tiled loop or cache scope
        size_t upper_size = vec_upper_scope.size();
        for (k = 0; k < vec_upper_scope.size(); k++) {
          void *lift_scope = vec_upper_scope[k];
          void *lift_array = vec_array_in_upper_scope[k];
          if (nullptr == cached_scope) {
            if (m_cached_scope.count(lift_scope) > 0) {
              if (m_cached_scope[lift_scope].count(lift_array) > 0) {
                cached_scope = lift_scope;
                cached_array = lift_array;
              }
            }
          }
          if (cached_above_pairs.count(std::make_pair(lift_scope, lift_array)) >
              0) {
            upper_size = k;
            break;
          }
        }
        if (cached_scope != nullptr) {
          int64_t new_buff_size = 0;
          bool report = !start_from_fine_grain;
          int64_t access_size = 0;
          int is_cf_applied = is_cg_burst_available(
              cached_array, cached_scope, element_size, &new_buff_size,
              &access_size, false, report);
          if (is_cf_applied == BURST_APPLIED) {
            if (!check_single_resource_limit(cached_array, new_buff_size,
                                             element_size)) {
              scope_to_lift = cached_scope;
              array_to_lift = cached_array;
            }
          }

        } else {
          void *pre_scope = nullptr;
          void *pre_array = nullptr;
          void *last_scope = nullptr;
          for (k = 0; k < upper_size; k++) {
            void *lift_array = vec_array_in_upper_scope[k];
            void *lift_scope = vec_upper_scope[k];
            if (visited_pairs.count(std::make_pair(lift_array, lift_scope)) >
                0) {
              continue;
            }
            vector<void *> vec_refs;
            m_ast->get_ref_in_scope(lift_array, lift_scope, &vec_refs, true);
            //  if current scope is not loop statement or no more references or
            //  no cross function or no coarse grained pipeline chance
            //  just skip it
            if ((pre_scope != nullptr) &&
                (m_ast->IsLoopStatement(lift_scope) == 0) &&
                (m_ast->IsForStatement(m_ast->GetParent(lift_scope)) == 0) &&
                m_ast->TraceUpToFuncDecl(lift_scope) ==
                    m_ast->TraceUpToFuncDecl(pre_scope) &&
                pre_ref_size == vec_refs.size()) {
              continue;
            }
            visited_pairs.insert(std::make_pair(lift_array, lift_scope));
            int64_t new_buff_size = 0;
            bool report = ((pre_scope == nullptr) || (pre_array == nullptr)) &&
                          !start_from_fine_grain;
            int64_t access_size = 0;
            int is_cf_applied = is_cg_burst_available(
                lift_array, lift_scope, element_size, &new_buff_size,
                &access_size, false, false);
            if (!is_cg_trace_up(is_cf_applied) && report) {
              is_cg_burst_available(lift_array, lift_scope, element_size,
                                    &new_buff_size, nullptr, false, report);
              break;
            }
            if (is_cf_applied != BURST_APPLIED) {
              continue;
            }
            MemoryAccessInfo access_info(1, pre_buff_size);

            if ((last_scope == nullptr) || vec_refs.size() != pre_ref_size) {
              access_info = MemoryAccessInfo(access_size, 1);
              last_scope = lift_scope;
              pre_ref_size = vec_refs.size();
            }
            if (check_lifting_resource_limit(lift_array, lift_scope,
                                             pre_buff_size, new_buff_size,
                                             element_size)) {
#ifdef PROJDEBUG
              cout << "cannot infer memory burst for variable \'"
                   << m_ast->GetVariableName(lift_array)
                   << "\' because of resource limitation in scope \'"
                   << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
              break;
            }
            if (check_lifting_efficiency(access_info,
                                         MemoryAccessInfo(1, new_buff_size),
                                         last_scope, lift_scope)) {
#ifdef PROJDEBUG
              cout << "cannot infer memory burst for variable \'"
                   << m_ast->GetVariableName(lift_array)
                   << "\' because of inefficiency in scope \'"
                   << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
              continue;
            }
            pre_scope = lift_scope;
            pre_array = lift_array;
            pre_buff_size = new_buff_size;
            last_scope = lift_scope;
            burst_level = k;
          }

          scope_to_lift = pre_scope;
          array_to_lift = pre_array;
        }
        if ((scope_to_lift != nullptr) && (array_to_lift != nullptr)) {
          //  youxiang: 20170331 ignore the upper scope
          for (k = burst_level; k < vec_upper_scope.size(); ++k) {
            visited_pairs.insert(std::make_pair(vec_array_in_upper_scope[k],
                                                vec_upper_scope[k]));
          }
        }
      }
      if ((scope_to_lift == nullptr) || (array_to_lift == nullptr)) {
        continue;
      }
      auto one_pair = std::make_tuple(scope_to_lift, array_to_lift,
                                      !(cached_scope == nullptr));
      if (burst_candidate_pairs.count(one_pair) > 0) {
        continue;
      }
      vec_burst_candidate_pairs.push_back(one_pair);
    }
  }
  //  sort the candidate according to scope orders, if other scope is located in
  //  the one scope, one candidate will be placed before the other candidate
  std::stable_sort(vec_burst_candidate_pairs.begin(),
                   vec_burst_candidate_pairs.end(), [=](auto one, auto other) {
                     return std::get<0>(other) != std::get<0>(one) &&
                            m_ast->is_located_in_scope(std::get<0>(other),
                                                       std::get<0>(one)) == 1;
                   });
  //  finally infer burst from outer scope to inner scope to avoid unused burst
  //  in the inner scope
  for (auto one_candidate : vec_burst_candidate_pairs) {
    void *scope_to_lift = std::get<0>(one_candidate);
    void *array_to_lift = std::get<1>(one_candidate);
    bool is_cached = std::get<2>(one_candidate);
    vector<void *> vec_refs;
    m_ast->get_ref_in_scope(array_to_lift, scope_to_lift, &vec_refs, true);
    void *pos = nullptr;
    if (!vec_refs.empty()) {
      pos = vec_refs[0];
    }
    if (!is_burst_candidate(array_to_lift,
                            m_ast->TraceUpToFuncDecl(scope_to_lift), pos,
                            false)) {
      continue;
    }
    void *element_type =
        m_ast->GetBaseType(m_ast->GetTypebyVar(array_to_lift), true);
    int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
    int64_t access_size = 0;
    int64_t buff_size = 0;
    int is_cf_applied =
        is_cg_burst_available(array_to_lift, scope_to_lift, element_size,
                              &buff_size, &access_size, false, false);
    if (is_cf_applied != BURST_APPLIED) {
      continue;
    }
    if (check_lifting_efficiency(MemoryAccessInfo(access_size, 1),
                                 MemoryAccessInfo(1, buff_size), scope_to_lift,
                                 scope_to_lift)) {
      continue;
    }
    if (!is_cached) {
      //  3. Get the surrounding for-loops of the pipelined loop and the array
      //  init_name
      bool infer_data_reuse = cg_memory_burst_transform_for_data_reuse(
          scope_to_lift, array_to_lift);
      if (infer_data_reuse) {
        Changed = true;
        continue;
      }

      if (mMars_ir.is_fine_grained_scope(m_ast, scope_to_lift)) {
        void *sg_loop = m_ast->GetEnclosingNode("ForLoop", scope_to_lift);
        string qorMergeMode = getQoRMergeMode(m_ast, sg_loop);
        if (qorMergeMode != "tail") {
          string var_name = m_ast->GetVariableName(array_to_lift);
          auto array_user_info = getUserCodeInfo(m_ast, array_to_lift);
          if (!array_user_info.name.empty())
            var_name = array_user_info.name;
          string var_info =
              "\'" + var_name + "\' (" + array_user_info.file_location + ")";
          string scope_info =
              getUserCodeFileLocation(m_ast, scope_to_lift, true);
          dump_critical_message(
              BURST_WARNING_22(var_info, scope_info, var_name), 1);
        }
#ifdef PROJDEBUG
        cout << "disable infer memory burst for variable \'"
             << m_ast->GetVariableName(array_to_lift) << "\' because scope \'"
             << m_ast->_p(scope_to_lift, 80) << "\' is fine grained" << endl;
#endif
        continue;
      }
    }
    Changed |=
        cg_memory_burst_transform(scope_to_lift, array_to_lift, false, true);
  }
  return Changed;
}

void MemoryBurst::cg_get_options_for_lifting(
    void *curr_scope, void *curr_array, vector<void *> *vec_upper_scope,
    vector<void *> *vec_array_in_upper_scope, bool mars_ir_v2) {
  /*if (!mars_ir_v2) {
    if (m_ast->IsForStatement(curr_scope)) {
      //  start from outmost fine grained loop
      CMirNode *curr_node = mMars_ir.get_node(curr_scope);
      if (curr_node->is_fine_grain)
        start_from_fine_grained = true;
      CMirNode *parent = curr_node->get_parent();
      while (parent && !parent->is_function && parent->is_fine_grain) {
        curr_node = parent;
        parent = parent->get_parent();
      }
      curr_scope = m_ast->GetParent(curr_node->ref);
    }
  }*/
  if (m_ast->is_located_in_scope_simple(
          curr_scope, m_ast->TraceUpByTypeCompatible(
                          curr_array, V_SgScopeStatement)) == 0) {
    return;
  }
  cg_get_cached_options(curr_scope);
  vec_upper_scope->push_back(curr_scope);
  vec_array_in_upper_scope->push_back(curr_array);
  void *upper_scope = m_ast->TraceUpByTypeCompatible(
      m_ast->GetParent(curr_scope), V_SgScopeStatement);
  while (upper_scope != nullptr) {
    if (m_ast->IsFunctionDefinition(upper_scope) != 0) {
      void *func_decl = m_ast->GetFuncDeclByDefinition(upper_scope);
      if (mars_ir_v2 ? mMars_ir_v2.is_kernel(func_decl)
                     : mMars_ir.is_kernel(m_ast, func_decl)) {
        break;
      }
      vector<void *> func_calls;
      m_ast->GetFuncCallsFromDecl(func_decl, nullptr, &func_calls);
      if (func_calls.size() != 1) {
        break;
      }
      void *func_call = func_calls[0];
      int param_index = -1;
      for (int i = 0; i < m_ast->GetFuncParamNum(func_decl); ++i) {
        if (m_ast->GetFuncParam(func_decl, i) == curr_array) {
          param_index = i;
          break;
        }
      }
      if (param_index < 0) {
        break;
      }
      void *arg = m_ast->GetFuncCallParam(func_call, param_index);
      void *array_ref = nullptr;
      m_ast->parse_array_ref_from_pntr(arg, &array_ref);
      assert(array_ref);
      curr_array = m_ast->GetVariableInitializedName(array_ref);
      while (m_ast->IsArgumentInitName(curr_array) == 0) {
        //  handle pointer alias
        auto vec_source = m_ast->get_alias_source(curr_array, array_ref);
        if (vec_source.size() != 1) {
          return;
        }
        void *source = vec_source[0];
        void *var_ref = nullptr;
        m_ast->parse_array_ref_from_pntr(source, &var_ref);
        if (var_ref == nullptr) {
          return;
        }
        curr_array = m_ast->GetVariableInitializedName(var_ref);
      }
      upper_scope =
          m_ast->TraceUpByTypeCompatible(func_call, V_SgScopeStatement);
    }
    if (m_ast->is_located_in_scope_simple(
            upper_scope, m_ast->TraceUpByTypeCompatible(
                             curr_array, V_SgScopeStatement)) == 0) {
      break;
    }
    cg_get_cached_options(upper_scope);
    vec_upper_scope->push_back(upper_scope);
    vec_array_in_upper_scope->push_back(curr_array);
    upper_scope = m_ast->TraceUpByTypeCompatible(m_ast->GetParent(upper_scope),
                                                 V_SgScopeStatement);
  }
}

int MemoryBurst::is_cg_burst_available(void *lift_array, void *lift_scope,
                                       int element_size, int64_t *buff_size,
                                       int64_t *access_size, bool fifo_check,
                                       bool report, bool auto_coalecing_check) {
  // Check whether there is single access
  string var_name = m_ast->GetVariableName(lift_array);
  auto array_user_info = getUserCodeInfo(m_ast, lift_array);
  if (!array_user_info.name.empty())
    var_name = array_user_info.name;
  void *top_array = get_top_arg(lift_array);
  map<string, string> msg_map;
  msg_map["burst"] = "off";
  int contains_unsupported_control_flow =
      check_control_flow_availability(lift_scope);
  if (contains_unsupported_control_flow == 0) {
#if PROJDEBUG
    cout << "cannot infer burst variable \'" << var_name << "\' in scope \'"
         << m_ast->_p(lift_scope, 80)
         << "\' because of unsupported control flow" << endl;
#endif
    return UNSUPPORTED_CONTROL_FLOW;
  }

  int need_burst =
      cg_check_single_reference_availability(lift_scope, lift_array);
  if (need_burst == 0) {
#if PROJDEBUG
    cout << "do not need to infer burst variable \'" << var_name
         << "\' in scope \'" << m_ast->_p(lift_scope, 80)
         << "\' because of no single references" << endl;
#endif
    return NO_NEED_BURST;
  }

  //  1. Get the merged range (1-D array/pointer for
  CMarsRangeExpr mr_merged;
  CMarsRangeExpr mr_write;
  int read_write = NO_ACCESS;
  string var_info =
      "\'" + var_name + "\' (" + array_user_info.file_location + ")";
  if (!cg_get_merged_access_range(lift_scope, lift_array, &mr_merged, &mr_write,
                                  &read_write, access_size)) {
    return NON_EXACT_WRITE_ONLY_IN_DATAFLOW;
  }

  //  2. Check whether to apply memory burst, starting address and length
  //    a) Get the absolute UB and LB, if any of them is infinite, ignore
  //    b) if upper bound of the length is larger than buffer size threshold,
  //    ignore
  //    c) if the symbolic start/length has multiple bounds, use the absolute
  //    value instead
  //    d) if the symbolic start/length is not movable to the insert position,
  //    use absolute value instead
  CMarsExpression me_start;
  CMarsExpression me_length;
  bool is_non_negative_len = true;
  int is_applied = NO_NEED_BURST;
  bool is_exact = false;
  cg_check_range_availability(lift_scope, lift_array, element_size, mr_merged,
                              &me_start, &me_length, &is_applied, buff_size,
                              &read_write, &is_non_negative_len, &is_exact,
                              report);
  if (is_applied != BURST_APPLIED) {
    return is_applied;
  }

  //  3.1 Check alias
  int ref_check =
      cg_check_reference_availability(lift_scope, lift_array, me_start);
  if (ref_check == 0) {
#if PROJDEBUG
    cout << "Cannot infer burst variable \'" << var_name << "\' in scope \'"
         << m_ast->_p(lift_scope, 80) << "\' because of unsupport references"
         << endl;
#endif
    if (report) {
      msg_map["burst_off_reasons"] = "unsupported references";
      insertMessage(m_ast, get_top_arg(lift_array), msg_map);
      dump_critical_message(BURST_WARNING_7(var_info), 0, m_ast, top_array);
    }
    return UNSUPPORTED_REFERENCE;
  }
  if (fifo_check) {
    if (mr_merged.is_exact() == 0) {
#if PROJDEBUG
      cout << "cannot infer memory burst for variable \'" << var_name
           << "\' because of inexact range in scope \'"
           << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
      if (report) {
        msg_map["burst_off_reasons"] = "inexact range";
        insertMessage(m_ast, get_top_arg(lift_array), msg_map);
        dump_critical_message(BURST_WARNING_6(var_info), 0, m_ast, top_array);
      }
      return NON_EXACT_ACCESS;
    }
    if (!is_non_negative_len) {
#if PROJDEBUG
      cout << "cannot infer memory burst for variable \'" << var_name
           << "\' because of possible negative burst length \'"
           << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
      if (report) {
        msg_map["burst_off_reasons"] = "possible negative burst length";
        insertMessage(m_ast, get_top_arg(lift_array), msg_map);
        dump_critical_message(BURST_WARNING_13(var_info), 0, m_ast, top_array);
      }
      return NEGATIVE_BURST_LENGTH;
    }
    bool is_simple_form = cg_check_delinearize_availability(
                              lift_scope, lift_array, me_start) != 0;
    if (!is_simple_form) {
#if PROJDEBUG
      cout << "cannot infer memory burst for variable \'" << var_name
           << "\' because of improper streaming access order \'"
           << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
      if (report) {
        msg_map["burst_off_reasons"] = "improper streaming access order";
        insertMessage(m_ast, get_top_arg(lift_array), msg_map);
        dump_critical_message(BURST_WARNING_11(var_info), 0, m_ast, top_array);
      }
      return NON_SIMPLE_FORM;
    }
    if (read_write == READ_WRITE_FLAG) {
      //  to be enabled 1) if the indexes associated with outer loops (at
      //  lifting position) are in the simple form; and all the outer loop
      //  iterators are associate with at least one dimension of the array
      //  2) all the refs happens in the scope
      void *func_decl = m_ast->TraceUpToFuncDecl(lift_scope);
      void *func_body = m_ast->GetFuncBody(func_decl);
      vector<void *> all_refs;
      m_ast->get_ref_in_scope(lift_array, func_decl, &all_refs, true);
      vector<void *> part_refs;
      m_ast->get_ref_in_scope(lift_array, lift_scope, &part_refs, true);
      bool supported = false;
      if (all_refs == part_refs) {
        //  check original access is streaming order
        CMarsExpression zero(m_ast, 0L);
        bool is_simple_form = cg_check_delinearize_availability(
                                  lift_scope, lift_array, zero) != 0;
        if (is_simple_form) {
          vector<void *> vec_atoms = me_start.get_vars();
          vector<void *> nested_loops;
          void *pos = lift_scope;
          if (m_ast->IsForStatement(lift_scope) != 0) {
            pos = m_ast->GetParent(lift_scope);
          }
          m_ast->get_loop_nest_in_scope(pos, func_decl, &nested_loops);
          if (set<void *>(vec_atoms.begin(), vec_atoms.end()) ==
              set<void *>(nested_loops.begin(), nested_loops.end())) {
            supported = true;
          }
        }
      }
      if (!supported) {
#ifdef RPOJDEBUG
        cout << "cannot infer memory burst for variable \'" << var_name
             << "\' because of read-write access in scope \'"
             << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
        if (report) {
          msg_map["burst_off_reasons"] = "read-write access";
          insertMessage(m_ast, get_top_arg(lift_array), msg_map);
          dump_critical_message(BURST_WARNING_10(var_info), 0, m_ast,
                                top_array);
        }
        return READ_WRITE_SYNC;
      }
      //  dump skip_sync pragma to let the following analysis not generate
      //  synchronization for the external memory port
      string skip_sync_pragma =
          "ACCEL skip_sync variable=" + m_ast->GetVariableName(lift_array);
      void *pragma_decl = m_ast->CreatePragma(skip_sync_pragma, func_body);
      m_ast->PrependChild(func_body, pragma_decl);
    }
  }
  if (auto_coalecing_check) {
    int opt_bitwidth = get_arg_opt_bitwidth(lift_array);
    int num_elements = opt_bitwidth / 8 / element_size;
    // 1. exact access
    if (mr_merged.is_exact() == 0) {
#if PROJDEBUG
      cout << "cannot infer caching for variable \'" << var_name
           << "\' because of inexact range in scope \'"
           << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
      return NON_EXACT_ACCESS;
    }
    // 2. constant aligned length
    if ((me_length.IsConstant() == 0) ||
        ((num_elements != 0) &&
         (me_length.GetConstant() % num_elements) != 0)) {
#if PROJDEBUG
      cout << "cannot infer caching for variable \'" << var_name
           << "\' because of variable or unaligned caching length \'"
           << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
      return UNALIGNED_BURST_LENGTH;
    }
    // 3. offset aligned with wide bus bitwidth
    if ((num_elements != 0) &&
        (divisible(me_start, num_elements, false) == 0)) {
#if PROJDEBUG
      cout << "cannot infer caching for variable \'" << var_name
           << "\' because of unaligned caching offset \'"
           << m_ast->_p(lift_scope, 80) << "\'" << endl;
#endif
      return UNALIGNED_BURST_OFFSET;
    }
  }
  if (m_ast->func_has_dataflow(m_ast->TraceUpToFuncDecl(lift_scope))) {
#if PROJDEBUG
    cout << "cannot infer caching for variable \'" << var_name
         << "\' because of dataflow directive \'" << m_ast->_p(lift_scope, 80)
         << "\'" << endl;
#endif
    return BREAK_DATAFLOW;
  }
  return is_applied;
}

bool MemoryBurst::cg_memory_burst_transform_for_data_reuse(
    void *scope_to_lift, void *array_to_lift) {
  void *top_arg = get_top_arg(array_to_lift);
  if (mMars_ir_v2.get_coalescing_flag(top_arg)) {
#if PROJDEBUG
    cout << "cannot infer data reuse driven burst for variable \'"
         << m_ast->GetVariableName(array_to_lift)
         << "\' because of coalescing force flag" << endl;
#endif
    return false;
  }
  void *curr_sg_scope =
      m_ast->TraceUpToForStatement(m_ast->GetParent(scope_to_lift));
  if (curr_sg_scope == nullptr) {
    return false;
  }
  //  1. Get the merged range (1-D array/pointer for
  CMarsRangeExpr mr_merged;
  CMarsRangeExpr mr_write;
  int read_write = NO_ACCESS;
  cg_get_merged_access_range(scope_to_lift, array_to_lift, &mr_merged,
                             &mr_write, &read_write, nullptr);
  CMarsExpression me_start;
  CMarsExpression me_length;
  bool is_non_negative_len = true;
  bool is_exact = false;
  int is_applied = 0;
  int64_t buff_size = 0;
  void *element_type =
      m_ast->GetBaseType(m_ast->GetTypebyVar(array_to_lift), true);
  int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
  cg_check_range_availability(scope_to_lift, array_to_lift, element_size,
                              mr_merged, &me_start, &me_length, &is_applied,
                              &buff_size, &read_write, &is_non_negative_len,
                              &is_exact, false);
  //  check whether there is overlap; otherwise there may be data dependence
  CMarsExpression coeff_for_iter = me_start.get_coeff(curr_sg_scope);
  if ((coeff_for_iter < me_length) != 0) {
    return false;
  }

  vector<void *> vec_loops;
  vector<void *> vec_lbs;
  vector<size_t> vec_lens;
  bool data_reuse = false;
  size_t total_size = buff_size;
  vector<void *> accesses;
  m_ast->get_ref_in_scope(array_to_lift, scope_to_lift, &accesses);
  if (check_single_resource_limit(array_to_lift, buff_size, element_size)) {
    return false;
  }
  void *insert_pos = scope_to_lift;
  CMarsExpression me_start_copy = me_start;
  CMarsExpression me_length_copy = me_length;
  do {
    //  lift non-continuous burst for data reuse
    //  conditions:
    vector<void *> new_accesses;
    m_ast->get_ref_in_scope(array_to_lift, curr_sg_scope, &new_accesses);
    //  1. all the references within the outmost scope is within the innermost
    //  scope
    if (new_accesses.size() != accesses.size() &&
        (m_ast->has_write_in_scope_fast(array_to_lift, curr_sg_scope) != 0)) {
      break;
    }
    CMarsExpression m_coeff = me_start.get_coeff(curr_sg_scope);
    if ((m_coeff == 0) != 0) {
      //  2. at lease there is a loop whose iterator is not related to start
      //  index
      if ((me_start_copy.is_movable_to(curr_sg_scope) == 0) ||
          me_start_copy.any_var_located_in_scope(curr_sg_scope) ||
          me_length_copy.any_var_located_in_scope(curr_sg_scope) ||
          (me_length_copy.is_movable_to(curr_sg_scope) == 0)) {
        break;
      }
      data_reuse = true;
    } else {
      //  3. start index is linear to iterators
      if (m_coeff.IsConstant() == 0) {
        break;
      }
      //  4. for any loop whose iterator is related to start index,
      //    it should be canonical loop with constant upper bound
      void *iter;
      void *lb;
      void *ub;
      void *step;
      void *body;
      iter = lb = ub = step = body = nullptr;
      bool has_incr_space;
      bool included_upper_bound;
      has_incr_space = included_upper_bound = false;
      if (m_ast->IsCanonicalForLoop(curr_sg_scope, &iter, &lb, &ub, &step,
                                    &body, &has_incr_space,
                                    &included_upper_bound) == 0) {
        break;
      }
      CMarsExpression len =
          CMarsExpression(ub, m_ast) - CMarsExpression(lb, m_ast);
      if (len.IsConstant() == 0) {
        break;
      }
      int64_t len_val = len.GetConstant();
      if (included_upper_bound) {
        len_val++;
      }
      if (len_val <= 0) {
        break;
      }
      if (check_single_resource_limit(array_to_lift, total_size * len_val,
                                      element_size)) {
        break;
      }
      //  5. m_start, m_length can be moved to here when removing current
      //  iterator part
      me_start_copy.substitute(curr_sg_scope, CMarsExpression(m_ast, 0));
      me_length_copy.substitute(curr_sg_scope, CMarsExpression(m_ast, 0));
      if ((me_start_copy.is_movable_to(curr_sg_scope) == 0) ||
          me_start_copy.any_var_located_in_scope(curr_sg_scope) ||
          me_length_copy.any_var_located_in_scope(curr_sg_scope) ||
          (me_length_copy.is_movable_to(curr_sg_scope) == 0)) {
        break;
      }
      total_size *= len_val;
      vec_loops.push_back(curr_sg_scope);
      vec_lens.push_back(len_val);
      vec_lbs.push_back(lb);
    }
    insert_pos = curr_sg_scope;
    curr_sg_scope =
        m_ast->TraceUpToForStatement(m_ast->GetParent(curr_sg_scope));
  } while (curr_sg_scope != nullptr);

  if ((!data_reuse || vec_loops.empty())  //  no data reuse chance
      && !mMars_ir.is_fine_grained_scope(
             m_ast,
             scope_to_lift)  //  original scope is not in fine grained scope
  ) {
    return false;
  }

  if (mMars_ir.is_fine_grained_scope(m_ast, insert_pos)) {
    return false;
  }
  void *buf_decl = nullptr;
  void *memcpy_r = nullptr;
  void *memcpy_w = nullptr;
  if (!cg_transform_burst_for_data_reuse(
          scope_to_lift, array_to_lift, me_start, me_length,
          is_non_negative_len, is_exact, read_write, &buf_decl, &memcpy_r,
          &memcpy_w, &buff_size, vec_loops, vec_lens, vec_lbs, insert_pos,
          element_size)) {
    return false;
  }
  add_header(scope_to_lift);
  total_size = buff_size;
  for (auto len : vec_lens) {
    total_size *= len;
  }
  cg_reportMessage(array_to_lift, scope_to_lift, total_size, element_size,
                   "DATA_REUSE");
  m_ast->init_defuse_range_analysis();
  return true;
}

bool MemoryBurst::cg_memory_burst_transform(void *scope_to_lift,
                                            void *array_to_lift,
                                            bool check_constant_burst_length,
                                            bool report) {
  //  1. Get the merged range (1-D array/pointer for
  CMarsRangeExpr mr_merged;
  CMarsRangeExpr mr_write;
  int read_write = NO_ACCESS;
  cg_get_merged_access_range(scope_to_lift, array_to_lift, &mr_merged,
                             &mr_write, &read_write, nullptr);
  CMarsExpression me_merged_start;
  CMarsExpression me_merged_length;
  bool is_non_negative_merged_len = true;
  int is_applied = 0;
  int64_t merged_buff_size = 0;
  bool is_merge_exact = false;
  void *element_type =
      m_ast->GetBaseType(m_ast->GetTypebyVar(array_to_lift), true);
  int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
  if (read_write != NO_ACCESS) {
    cg_check_range_availability(
        scope_to_lift, array_to_lift, element_size, mr_merged, &me_merged_start,
        &me_merged_length, &is_applied, &merged_buff_size, &read_write,
        &is_non_negative_merged_len, &is_merge_exact, false);
  }

  if (is_applied != BURST_APPLIED) {
    return false;
  }

  if (merged_buff_size <= 0) {
    return false;
  }

  CMarsExpression me_write_start;
  CMarsExpression me_write_length;
  bool is_non_negative_write_len = true;
  int64_t write_buff_size = 0;
  bool is_write_exact = false;
  if (read_write & WRITE_FLAG) {
    cg_check_range_availability(
        scope_to_lift, array_to_lift, element_size, mr_write, &me_write_start,
        &me_write_length, &is_applied, &write_buff_size, &read_write,
        &is_non_negative_write_len, &is_write_exact, false);
  }

  //  check_memory_resource(array_to_lift, merged_buff_size, element_size);

  void *buf_decl = nullptr;
  void *memcpy_r = nullptr;
  void *memcpy_w = nullptr;
  bool aggressive_write_only = false;
  if (!cg_transform_burst(scope_to_lift, array_to_lift, me_merged_start,
                          me_merged_length, is_non_negative_merged_len,
                          is_merge_exact, me_write_start, me_write_length,
                          is_non_negative_write_len, is_write_exact, read_write,
                          &buf_decl, &memcpy_r, &memcpy_w, &merged_buff_size,
                          check_constant_burst_length, report, element_size,
                          &aggressive_write_only)) {
    return false;
  }
  add_header(scope_to_lift);
  if (report) {
    cg_reportMessage(array_to_lift, scope_to_lift, merged_buff_size,
                     element_size, "GENERAL", aggressive_write_only);
  }
  mMars_ir.clear_port_is_bus_cache();
  mMars_ir_v2.clear_port_is_bus_cache();
  m_ast->init_defuse_range_analysis();
  return true;
}

int MemoryBurst::is_cg_parallel_available(void *array, void *scope,
                                          string *message) {
  void *top_arg = get_top_arg(array);
  if (mMars_ir_v2.get_coalescing_flag(top_arg)) {
#if PROJDEBUG
    cout << "cannot infer cg parallel driven burst for variable \'"
         << m_ast->GetVariableName(array)
         << "\' because of coalescing force flag" << endl;
#endif
    *message = "coalescing force flag.";
    return 0;
  }

  void *iter = nullptr;
  void *lb = nullptr;
  void *ub = nullptr;
  void *step = nullptr;
  void *body = nullptr;
  if (m_ast->IsCanonicalForLoop(scope, &iter, &lb, &ub, &step, &body) == 0) {
#if PROJDEBUG
    cout << "cannot infer cg parallel driven burst for non-canonical loop \'"
         << m_ast->get_internal_loop_label(scope) << "\'" << endl;
#endif
    *message = "non-canonical loop.";
    return 0;
  }
  CMarsExpression m_step(step, m_ast);
  CMarsRangeExpr loop_bound = CMarsVariable(scope, m_ast).get_range();

  CMarsExpression m_lb;
  CMarsExpression m_ub;
  int ret = loop_bound.get_simple_bound(&m_lb, &m_ub);
  if ((ret == 0) || (m_lb.IsConstant() == 0) || (m_ub.IsConstant() == 0) ||
      (m_step.IsConstant() == 0)) {
#if PROJDEBUG
    cout << "cannot infer cg parallel driven burst for loop \'"
         << m_ast->get_internal_loop_label(scope) << "\' "
         << " because loop lower bound or upper bound or step isnot constant"
         << endl;
#endif
    *message = "non-constant lower bound, upper bound or step.";
    return 0;
  }

  int64_t lb_val = m_lb.GetConstant();
  int64_t ub_val = m_ub.GetConstant();
  int64_t para_size = ub_val - lb_val + 1;
  if (para_size <= 0) {
#if PROJDEBUG
    cout << "cannot infer cg parallel driven burst for loop \'"
         << m_ast->get_internal_loop_label(scope) << "\' "
         << " because its tripcount is less than 1" << endl;
#endif
    *message = "loop tripcount less than 1.";
    return 0;
  }
  CMarsRangeExpr mr_merged;
  CMarsRangeExpr mr_write;
  int read_write = NO_ACCESS;
  cg_get_merged_access_range(body, array, &mr_merged, &mr_write, &read_write,
                             nullptr);

  //  2. Check whether to apply memory burst, starting address and length
  //    a) Get the absolute UB and LB, if any of them is infinite, ignore
  //    b) if upper bound of the length is larger than buffer size threshold,
  //    ignore
  //    c) if the symbolic start/length has multiple bounds, use the absolute
  //    value instead
  //    d) if the symbolic start/length is not movable to the insert position,
  //    use absolute value instead
  void *element_type = m_ast->GetBaseType(m_ast->GetTypebyVar(array), true);
  int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
  CMarsExpression me_start;
  CMarsExpression me_length;
  bool is_non_negative_len = true;
  int is_applied = NO_NEED_BURST;
  int64_t buff_size;
  bool is_exact = false;
  cg_check_range_availability(body, array, element_size, mr_merged, &me_start,
                              &me_length, &is_applied, &buff_size, &read_write,
                              &is_non_negative_len, &is_exact, false);
  if (NO_ACCESS == read_write) {
    //  dead port does not need to burst
    return 1;
  }
  if (is_applied != BURST_APPLIED) {
    *message = "undetermined access range.";
    return 0;
  }

  //  3.1 Check alias
  string var_name = m_ast->GetVariableName(array);
  if (cg_check_reference_availability(body, array, me_start) == 0) {
#if PROJDEBUG
    cout << "Cannot infer burst variable \'" << var_name << "\' in scopÃ« \'"
         << m_ast->_p(scope, 80) << "\' because of unsupport references"
         << endl;
#endif
    *message = "unsupported references.";
    return 0;
  }

  CMarsExpression coeff_for_iter = me_start.get_coeff(scope);
  if ((coeff_for_iter == me_length) != 0) {        //  continuous
  } else if ((coeff_for_iter > me_length) != 0) {  //  non-continuous
  } else {                                         //  overlap
    if ((read_write & WRITE_FLAG) != 0) {
#if PROJDEBUG
      cout << "Cannot infer cg parallel driven burst for variable \'"
           << var_name << "\' in scope \'" << m_ast->_p(scope, 80)
           << "\' because of loop "
           << "carried data dependence" << endl;
#endif
      *message = "possible loop carried data dependence.";
      return 0;
    }
  }

  if (check_single_resource_limit(array, buff_size * para_size, element_size)) {
    *message = "required buffer size (" +
               my_itoa(buff_size * para_size * element_size) + " bytes) " +
               "more than single buffer size threshold (" +
               my_itoa(get_single_threshold(array, element_size)) +
               " bytes).\n";
    *message += "  Hint: 1. decrease parallel factor\n";
    *message += "        2. specify the burst length threshold by adding "
                "interface attribute \n";
    *message += "        \'max_burst_size=?\'\n";
    *message += "        3. increase the single resource threshold by using \n";
    *message += "        \'--attribute burst_single_size_threshold=?\'\n";
    *message += "        4. make range analysis accurate by changing \n";
    *message +=
        "        pointer parameter into array parameter in sub function \n";
    return 0;
  }

  return 1;
}

bool MemoryBurst::cg_memory_burst_transform_for_parallel(void *scope,
                                                         void *array) {
  void *iter = nullptr;
  void *lb = nullptr;
  void *ub = nullptr;
  void *step = nullptr;
  void *body = nullptr;
  if (m_ast->IsCanonicalForLoop(scope, &iter, &lb, &ub, &step, &body) == 0) {
    return false;
  }

  CMarsExpression m_step(step, m_ast);
  if (m_step.IsConstant() == 0) {
    return false;
  }
  CMarsRangeExpr loop_bound = CMarsVariable(scope, m_ast).get_range();

  CMarsExpression m_lb;
  CMarsExpression m_ub;
  int ret = loop_bound.get_simple_bound(&m_lb, &m_ub);
#if PROJDEBUG
  assert(ret);
#else
  if (!ret)
    return 0;
#endif
  int64_t lb_val = m_lb.GetConstant();
  int64_t ub_val = m_ub.GetConstant();
  int64_t para_size = ub_val - lb_val + 1;
  if (para_size <= 0) {
    return false;
  }

  CMarsRangeExpr mr_merged;
  CMarsRangeExpr mr_write;
  int read_write = NO_ACCESS;
  cg_get_merged_access_range(body, array, &mr_merged, &mr_write, &read_write,
                             nullptr);

  CMarsExpression me_start;
  CMarsExpression me_length;
  bool is_non_negative_len = true;
  bool is_exact = false;
  int is_applied = 0;
  int64_t buff_size = 0;
  void *element_type = m_ast->GetBaseType(m_ast->GetTypebyVar(array), true);
  size_t element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
  cg_check_range_availability(body, array, element_size, mr_merged, &me_start,
                              &me_length, &is_applied, &buff_size, &read_write,
                              &is_non_negative_len, &is_exact, false);
  if (NO_ACCESS == read_write) {
    return false;
  }
  CMarsExpression coeff_for_iter = me_start.get_coeff(scope);
  bool continuous = false;
  if ((coeff_for_iter == me_length) != 0) {
    //  continuous
    //  if the burst length is less than wide bus length lower threshold, please
    //  generate continous to avoid wide bus failure
    //  youxiang: 20170321 bug1099
    //  if the lower bound isnot 512bit aligned,
    //  separate burst for each duplication
#if 0
    //  Youxiang: 20170409 bug1166
    //  if the total partition factor is more than threshold,
    //  separate burst for each duplication
    continuous =
        (buff_size < m_wide_bus_length_lower_threshold) ||
        (((element_size * buff_size * 8) % 512) == 0 &&
         min(64 / element_size, static_cast<size_t>(buff_size)) * para_size <=
             m_partition_factor_upper_threshold);
#else
    continuous = (buff_size < m_wide_bus_length_lower_threshold) ||
                 (((element_size * buff_size * 8) % 512) == 0);
#endif
  } else if ((coeff_for_iter > me_length) != 0) {  //  non-continuous
  } else {                                         //  overlap
    if ((read_write & WRITE_FLAG) != 0) {
      return false;
    }
  }

  void *buf_decl = nullptr;
  void *memcpy_r = nullptr;
  void *memcpy_w = nullptr;
  if (!cg_transform_burst_for_parallel(
          scope, array, me_start, me_length, is_non_negative_len, is_exact,
          read_write, &buf_decl, &memcpy_r, &memcpy_w, &buff_size, para_size,
          lb_val, continuous, element_size)) {
    return false;
  }
  add_header(scope);
  cg_reportMessage(array, scope, buff_size * para_size, element_size, "CGPAR");
  m_ast->init_defuse_range_analysis();
  return true;
}

bool MemoryBurst::cg_transform_burst_for_parallel(
    void *sg_loop, void *sg_array, const CMarsExpression &me_start,
    const CMarsExpression &me_length, bool is_non_negative_len, bool is_exact,
    int read_write, void **buf_decl, void **memcpy_r, void **memcpy_w,
    int64_t *buffer_size, int para_size, int64_t lb, bool continuous,
    int element_size) {
  void *func_decl = m_ast->TraceUpToFuncDecl(sg_loop);
  void *scope_body = sg_loop;
  void *insert_pos = sg_loop;
  void *top_arg = get_top_arg(sg_array);
  vector<void *> refs;
  m_ast->get_ref_in_scope(sg_array, sg_loop, &refs);
  CMarsExpression me_start_copy = me_start;
  CMarsExpression me_length_copy = me_length;
  me_start_copy.substitute(sg_loop, CMarsExpression(m_ast, 0));
  me_length_copy.substitute(sg_loop, CMarsExpression(m_ast, 0));
  do {
    //  Check if lifting is possible:
    //  1. iterator does not exist in start and
    //  length
    void *parent_loop =
        m_ast->TraceUpToForStatement(m_ast->GetParent(insert_pos));
    if (parent_loop == nullptr) {
      break;
    }
    int is_lifting = 0;
    {
      if (me_start.get_coeff(parent_loop).IsConstant() == 0) {
        is_lifting = 0;
      } else {
        int coeff1 = me_start.get_coeff(parent_loop).get_const();
        int coeff2 = me_length.get_coeff(parent_loop).get_const();
        is_lifting = static_cast<int>(coeff1 == 0 && coeff2 == 0);
      }
    }
    if (is_lifting == 0) {
      break;
    }
    vector<void *> curr_refs;
    m_ast->get_ref_in_scope(sg_array, parent_loop, &curr_refs);
    //  2. if has different references and contain write access, stop lifting
    if (refs != curr_refs &&
        (m_ast->has_write_in_scope_fast(sg_array, parent_loop) != 0)) {
      break;
    }
    //  3. movable test
    if ((me_start_copy.is_movable_to(parent_loop) == 0) ||
        me_start_copy.any_var_located_in_scope(parent_loop) ||
        me_length_copy.any_var_located_in_scope(parent_loop) ||
        (me_length_copy.is_movable_to(parent_loop) == 0)) {
      break;
    }
    insert_pos = parent_loop;
  } while (true);

  scope_body = m_ast->TraceUpToBasicBlock(m_ast->GetParent(insert_pos));
  CMarsExpression me_burst_start = me_start;
  bool duplicated_read_only_data = (read_write & WRITE_FLAG) == 0;
  if (continuous) {
    me_burst_start.substitute(sg_loop, CMarsExpression(m_ast, lb));
    duplicated_read_only_data = false;
  } else {
    duplicated_read_only_data &= me_burst_start.IsLoopInvariant(sg_loop);
  }

  CMarsExpression me_burst_len = me_length;
  if (continuous) {
    me_burst_len = me_burst_len * para_size;
  } else {
    duplicated_read_only_data &= me_burst_len.IsLoopInvariant(sg_loop);
  }

  bool is_non_negative_start = true;
  *buffer_size = 0;
  void *burst_len = nullptr;
  void *burst_start_expr = nullptr;
  void *start_expr = nullptr;
  refine_burst_start_and_length(me_burst_len, me_burst_start, insert_pos,
                                scope_body, sg_array, !is_exact, buffer_size,
                                &burst_len, &burst_start_expr,
                                &is_non_negative_len, &is_non_negative_start);
  refine_burst_start_and_length(me_burst_len, me_start, insert_pos, scope_body,
                                sg_array, false, buffer_size, &burst_len,
                                &start_expr, &is_non_negative_len,
                                &is_non_negative_start);
  if (!is_non_negative_start) {
    reportPossibleNegativeAccessStart(me_start, sg_array, sg_loop);
  }
  if (continuous) {
    *buffer_size /= para_size;
  }
  if (*buffer_size <= 0) {
    return false;
  }
  void *loop_body = m_ast->GetLoopBody(sg_loop);
  void *burst_pos = m_ast->GetLocation(m_ast->GetChildStmt(loop_body, 0));
  //  4.1 allocate local array
  void *basic_type = m_ast->GetBaseType(m_ast->GetTypebyVar(sg_array), false);
  vector<size_t> dims;
  if (!duplicated_read_only_data) {
    dims.push_back(para_size);
  }
  dims.push_back(*buffer_size);
  void *buf_type = m_ast->CreateArrayType(basic_type, dims);
  string buf_name = m_ast->GetVariableName(sg_array) + "_buf";
  m_ast->get_valid_name(scope_body, &buf_name);
  *buf_decl = m_ast->CreateVariableDecl(buf_type, buf_name, nullptr, scope_body,
                                        sg_array);
  m_ast->PrependChild(scope_body, *buf_decl, true);

  m_ast->PropagatePragma(sg_array, sg_loop, buf_name);

  //  4.2 replace original accesses
  {
    int check_only = NO_CHECK;
    vector<void *> vec_para_offset;
    if (!duplicated_read_only_data) {
      void *para_offset =
          m_ast->CreateVariableRef(m_ast->GetLoopIterator(sg_loop));
      para_offset =
          m_ast->CreateExp(V_SgSubtractOp, para_offset, m_ast->CreateConst(lb));
      vec_para_offset.push_back(para_offset);
    }
    cg_transform_refs_in_scope(sg_array, start_expr, burst_pos, *buf_decl,
                               sg_loop, check_only, vec_para_offset);
    for (auto para_offset : vec_para_offset) {
      m_ast->DeleteNode(para_offset);
    }
    m_ast->DeleteNode(start_expr);
  }

  //  4.3 create memcpy
  for (int i = 0; i != 2; ++i) {
    bool read_codegen = i == 0 && ((read_write & READ_FLAG) != 0);
    bool write_codegen = i == 1 && ((read_write & WRITE_FLAG) != 0);
    if (!read_codegen && !write_codegen) {
      continue;
    }
    void *sg_loop_body = nullptr;
    void *sg_loop_copy = nullptr;
    void *target_exp_addr = nullptr;
    void *source_exp_addr = nullptr;
    void *iter = m_ast->GetLoopIterator(sg_loop);
    void *all_body = m_ast->CreateBasicBlock();
    if (read_codegen) {
      m_ast->InsertStmt(all_body, update_position(insert_pos));
    } else if (write_codegen) {
      m_ast->InsertAfterStmt(all_body, insert_pos);
    }
    void *actual_burst_len = i == 0 ? burst_len : m_ast->CopyExp(burst_len);
    if (!continuous && !duplicated_read_only_data) {
      void *sg_init = m_ast->GetLoopInit(sg_loop);
      void *iter_decl = m_ast->GetVariableDecl(iter);
      if (m_ast->IsInScope(iter_decl, sg_init) == 0) {
        void *iter_type = m_ast->GetTypebyVar(iter);
        string iter_name = m_ast->GetVariableName(iter);
        void *iter_copy =
            m_ast->CreateVariableDecl(iter_type, iter_name, nullptr, all_body);
        m_ast->AppendChild(all_body, iter_copy);
      }
      void *sg_test = m_ast->GetLoopTest(sg_loop);
      void *sg_incr = m_ast->GetLoopIncrementExpr(sg_loop);
      sg_loop_body = m_ast->CreateBasicBlock();
      // MR1620 bind the loop into interface port
      sg_loop_copy = m_ast->CreateForLoop(
          m_ast->CopyStmt(sg_init), m_ast->CopyStmt(sg_test),
          m_ast->CopyExp(sg_incr), sg_loop_body, top_arg);
      m_ast->AppendChild(all_body, sg_loop_copy);
      void *para_offset = m_ast->CreateVariableRef(iter);
      para_offset =
          m_ast->CreateExp(V_SgSubtractOp, para_offset, m_ast->CreateConst(lb));
      target_exp_addr = m_ast->CreateExp(
          V_SgPntrArrRefExp, m_ast->CreateVariableRef(*buf_decl), para_offset);
      void *source_expr = m_ast->CreateExp(
          V_SgPntrArrRefExp, m_ast->CreateVariableRef(sg_array),
          i == 0 ? burst_start_expr : m_ast->CopyExp(burst_start_expr));
      source_exp_addr = m_ast->CreateExp(V_SgAddressOfOp, source_expr);
    } else {
      if (duplicated_read_only_data)
        target_exp_addr = m_ast->CreateVariableRef(*buf_decl);
      else
        target_exp_addr = m_ast->CreateExp(V_SgPntrArrRefExp,
                                           m_ast->CreateVariableRef(*buf_decl),
                                           m_ast->CreateConst(0));
      void *source_expr = m_ast->CreateExp(
          V_SgPntrArrRefExp, m_ast->CreateVariableRef(sg_array),
          i == 0 ? burst_start_expr : m_ast->CopyExp(burst_start_expr));
      source_exp_addr = m_ast->CreateExp(V_SgAddressOfOp, source_expr);
    }

    void *assert_stmt = nullptr;
    if (sg_loop_body != nullptr) {
      CMarsExpression me_actual_len(actual_burst_len, m_ast, insert_pos);
      if (me_actual_len.IsConstant() == 0) {
        void *burst_len_var = createTempVariableWithBoundAssertion(
            func_decl, sg_loop_body, actual_burst_len, "merlin_len",
            *buffer_size, is_non_negative_len, &assert_stmt);
        actual_burst_len = m_ast->CreateVariableRef(burst_len_var);
      }
    }

    void *sizeof_expr = m_ast->CreateExp(
        V_SgMultiplyOp, m_ast->CreateExp(V_SgSizeOfOp, basic_type),
        actual_burst_len);

    void *burst_call = nullptr;
    if (read_codegen) {
      vector<void *> sg_read_init_list;
      sg_read_init_list.push_back(m_ast->CreateCastExp(
          target_exp_addr, m_ast->GetTypeByString("void *")));
      sg_read_init_list.push_back(m_ast->CreateCastExp(
          source_exp_addr, m_ast->GetTypeByString("const void *")));
      sg_read_init_list.push_back(sizeof_expr);
      void *sg_read_func_call =
          m_ast->CreateFuncCall("memcpy", m_ast->GetTypeByString("void"),
                                sg_read_init_list, scope_body, top_arg);
      burst_call = sg_read_func_call;
      *memcpy_r = m_ast->CreateStmt(V_SgExprStatement, sg_read_func_call);
      if (!is_non_negative_len) {
        void *cond = m_ast->CreateExp(
            V_SgGreaterOrEqualOp,
            m_ast->CreateSignnesCast(m_ast->CopyExp(burst_len)),
            m_ast->CreateConst(0));
        void *true_body = m_ast->CreateBasicBlock();
        if (assert_stmt != nullptr) {
          m_ast->AppendChild(true_body, assert_stmt);
        }
        m_ast->AppendChild(true_body, *memcpy_r);
        *memcpy_r = m_ast->CreateIfStmt(cond, true_body, nullptr);
      }
      if (sg_loop_body != nullptr) {
        m_ast->AppendChild(sg_loop_body, *memcpy_r);
      } else {
        m_ast->AppendChild(all_body, *memcpy_r);
      }
    }

    if (write_codegen) {
      vector<void *> sg_write_init_list;
      sg_write_init_list.push_back(m_ast->CreateCastExp(
          source_exp_addr, m_ast->GetTypeByString("void *")));
      sg_write_init_list.push_back(m_ast->CreateCastExp(
          target_exp_addr, m_ast->GetTypeByString("const void *")));
      sg_write_init_list.push_back(sizeof_expr);
      void *sg_write_func_call =
          m_ast->CreateFuncCall("memcpy", m_ast->GetTypeByString("void"),
                                sg_write_init_list, scope_body, top_arg);
      burst_call = sg_write_func_call;
      *memcpy_w = m_ast->CreateStmt(V_SgExprStatement, sg_write_func_call);
      if (!is_non_negative_len) {
        void *cond = m_ast->CreateExp(
            V_SgGreaterOrEqualOp,
            m_ast->CreateSignnesCast(m_ast->CopyExp(burst_len)),
            m_ast->CreateConst(0));
        void *true_body = m_ast->CreateBasicBlock();
        if (assert_stmt != nullptr) {
          m_ast->AppendChild(true_body, assert_stmt);
        }
        m_ast->AppendChild(true_body, *memcpy_w);
        *memcpy_w = m_ast->CreateIfStmt(cond, true_body, nullptr);
      }
      if (sg_loop_body != nullptr) {
        m_ast->AppendChild(sg_loop_body, *memcpy_w);
      } else {
        m_ast->AppendChild(all_body, *memcpy_w);
      }
    }
    map<string, string> msg_map;
    msg_map["display_name"] = "auto memory burst for '" +
                              m_ast->GetVariableName(get_top_arg(sg_array)) +
                              "'";
    msg_map["burst"] = "on";
    msg_map["burst_length"] =
        CMarsExpression(sizeof_expr, m_ast, insert_pos).print_s();
    if (continuous) {
      msg_map["burst_length_max"] =
          my_itoa(para_size * *buffer_size * element_size);
    } else {
      msg_map["burst_length_max"] = my_itoa(*buffer_size * element_size);
    }
    if (read_codegen) {
      msg_map["is_read"] = "1";
    }
    if (write_codegen) {
      msg_map["is_write"] = "1";
    }
    insertMessage(m_ast, burst_call, msg_map);
    setNoHierarchy(m_ast, burst_call, 1);
  }
  return true;
}

void MemoryBurst::refine_burst_start_and_length(
    const CMarsExpression &me_length, const CMarsExpression &me_start,
    void *insert_pos, void *sg_scope, void *sg_array, bool refine_len,
    int64_t *buffer_size, void **burst_len, void **burst_start_expr,
    bool *is_non_negative_len, bool *is_non_negative_start) {
  int64_t max_buffer_size = std::numeric_limits<int64_t>::max();
  auto mr_length = me_length.get_range();
  if (*burst_len == nullptr) {
    *burst_len = me_length.get_expr_from_coeff();
  }
  if (*buffer_size <= 0) {
    if (mr_length.is_const_ub() != 0)
      *buffer_size = mr_length.get_const_ub();
  }
  if (refine_len) {
    if (auto *pos = me_length.get_pos()) {
      mr_length.refine_range_in_scope(nullptr, pos);
    }
    CMarsExpression me_burst_start = me_start;

    CMarsExpression me_burst_len = me_length;

    *burst_len = nullptr;
    vector<CMarsRangeExpr> access_range;
    bool access_range_intrinsic_valid = false;
    GetAccessRangeFromIntrinsic(sg_array, sg_scope, insert_pos, m_ast,
                                &access_range, &access_range_intrinsic_valid);

    if (access_range_intrinsic_valid) {
      CMarsRangeExpr me_range = access_range[0];
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      int ret = me_range.get_simple_bound(&me_lb, &me_ub);
      if (ret != 0) {
        CMarsExpression me_burst_bound = me_ub - me_burst_start + 1;
        if ((me_burst_bound >= me_burst_len) != 0) {
        } else {
          if ((me_burst_len >= me_burst_bound) != 0) {
            me_burst_len = me_burst_bound;
          } else {
            void *orig_burst_len = me_burst_len.get_expr_from_coeff(true);
            void *orig_burst_bound = me_burst_bound.get_expr_from_coeff(true);
            CMarsRangeExpr mr_burst_bound = me_burst_bound.get_range();
            if (void *pos = me_burst_bound.get_pos()) {
              mr_burst_bound.refine_range_in_scope(nullptr, pos);
            }
            *is_non_negative_len &= mr_burst_bound.IsNonNegative();
            void *test_expr = m_ast->CreateExp(
                V_SgLessOrEqualOp, orig_burst_len, orig_burst_bound);
            *burst_len = m_ast->CreateConditionExp(
                test_expr, m_ast->CopyExp(orig_burst_len),
                m_ast->CopyExp(orig_burst_bound));
          }
        }
        CMarsExpression me_total_access_range = me_ub - me_lb + 1;
        if (me_total_access_range.IsConstant() != 0) {
          max_buffer_size = me_total_access_range.GetConstant();
        }
      }
    }

    if (*burst_len == nullptr) {
      *burst_len = me_burst_len.get_expr_from_coeff(true);
    }
    CMarsRangeExpr me_burst_len_range = me_burst_len.get_range();
    if (me_burst_len_range.is_const_ub() != 0) {
      *buffer_size = min(*buffer_size, me_burst_len_range.get_const_ub());
    }
  }
  *burst_start_expr = create_offset_expr(
      me_start, burst_len, is_non_negative_len, is_non_negative_start,
      buffer_size, refine_len, true, insert_pos);
  *buffer_size = min(*buffer_size, max_buffer_size);
}

bool MemoryBurst::cg_transform_burst_for_data_reuse(
    void *sg_scope, void *sg_array, const CMarsExpression &me_start,
    const CMarsExpression &me_length, bool is_non_negative_len, bool exact,
    int read_write, void **buf_decl, void **memcpy_r, void **memcpy_w,
    int64_t *buffer_size, const vector<void *> &vec_loops,
    const vector<size_t> &vec_lens, const vector<void *> &vec_lbs,
    void *insert_pos, int element_size) {
  void *top_arg = get_top_arg(sg_array);
  void *func_decl = m_ast->TraceUpToFuncDecl(sg_scope);
  void *scope_body = m_ast->TraceUpToBasicBlock(m_ast->GetParent(insert_pos));
  bool in_body = m_ast->IsBasicBlock(sg_scope) != 0;
  void *burst_pos = sg_scope;
  if (in_body) {
    burst_pos = m_ast->GetLocation(m_ast->GetChildStmt(sg_scope, 0));
  }
  bool is_non_negative_start = true;
  *buffer_size = 0;
  void *burst_len = nullptr;
  void *burst_start_expr = nullptr;
  refine_burst_start_and_length(me_length, me_start, burst_pos, sg_scope,
                                sg_array, !exact, buffer_size, &burst_len,
                                &burst_start_expr, &is_non_negative_len,
                                &is_non_negative_start);
  if (!is_non_negative_start) {
    reportPossibleNegativeAccessStart(me_start, sg_array, sg_scope);
  }
  if (*buffer_size <= 0) {
    return false;
  }
  //  4.1 allocate local array
  void *basic_type = m_ast->GetBaseType(m_ast->GetTypebyVar(sg_array), false);
  vector<size_t> dims = vec_lens;
  dims.push_back(*buffer_size);
  void *buf_type = m_ast->CreateArrayType(basic_type, dims);
  string buf_name = m_ast->GetVariableName(sg_array) + "_buf";
  void *prev_stmt = insert_pos;
  void *insert_scope = m_ast->TraceUpToBasicBlock(prev_stmt);
  m_ast->get_valid_name(insert_scope, &buf_name);
  *buf_decl = m_ast->CreateVariableDecl(buf_type, buf_name, nullptr,
                                        insert_scope, sg_array);
  m_ast->PrependChild(insert_scope, *buf_decl, true);

  m_ast->PropagatePragma(sg_array, sg_scope, buf_name);

  //  4.2 replace original accesses
  {
    int check_only = NO_CHECK;
    vector<void *> vec_offset;
    for (size_t i = 0; i < vec_loops.size(); ++i) {
      void *sg_loop = vec_loops[i];
      void *sg_lb = vec_lbs[i];
      void *reuse_offset =
          m_ast->CreateVariableRef(m_ast->GetLoopIterator(sg_loop));
      CMarsExpression m_lb(sg_lb, m_ast, insert_pos);
      if ((m_lb != 0) != 0) {
        reuse_offset = m_ast->CreateExp(V_SgSubtractOp, reuse_offset,
                                        m_ast->CopyExp(sg_lb));
      }
      vec_offset.push_back(reuse_offset);
    }
    cg_transform_refs_in_scope(sg_array, burst_start_expr, burst_pos, *buf_decl,
                               sg_scope, check_only, vec_offset);
  }

  //  4.3 create memcpy
  for (int i = 0; i != 2; ++i) {
    bool read_codegen = i == 0 && ((read_write & READ_FLAG) != 0);
    bool write_codegen = i == 1 && ((read_write & WRITE_FLAG) != 0);
    if (!read_codegen && !write_codegen) {
      continue;
    }
    void *sg_inner_loop_body = nullptr;
    void *sg_outer_loop_copy = nullptr;
    void *target_exp_addr = m_ast->CreateVariableRef(*buf_decl);
    void *all_body = m_ast->CreateBasicBlock();
    void *burst_call = nullptr;
    if (read_codegen)
      m_ast->InsertStmt(all_body, update_position(insert_pos));
    else if (write_codegen)
      m_ast->InsertAfterStmt(all_body, insert_pos);
    void *sg_loop_body = m_ast->CreateBasicBlock();
    void *actual_burst_len = i == 0 ? burst_len : m_ast->CopyExp(burst_len);
    for (size_t j = 0; j < vec_loops.size(); ++j) {
      void *sg_loop = vec_loops[j];
      void *sg_lb = vec_lbs[j];
      void *sg_init = m_ast->GetLoopInit(sg_loop);
      void *iter = m_ast->GetLoopIterator(sg_loop);
      void *iter_decl = m_ast->GetVariableDecl(iter);
      if (m_ast->IsInScope(iter_decl, sg_init) == 0) {
        void *iter_type = m_ast->GetTypebyVar(iter);
        string iter_name = m_ast->GetVariableName(iter);
        void *iter_copy =
            m_ast->CreateVariableDecl(iter_type, iter_name, nullptr, all_body);
        m_ast->AppendChild(all_body, iter_copy);
      }
      void *sg_test = m_ast->GetLoopTest(sg_loop);
      void *sg_incr = m_ast->GetLoopIncrementExpr(sg_loop);
      if (j == 0) {
        sg_inner_loop_body = sg_loop_body;
      }
      // MR1620 bind the loop into interface port
      void *sg_loop_copy = m_ast->CreateForLoop(
          m_ast->CopyStmt(sg_init), m_ast->CopyStmt(sg_test),
          m_ast->CopyExp(sg_incr), sg_loop_body, top_arg);
      if (j == vec_loops.size() - 1) {
        sg_outer_loop_copy = sg_loop_copy;
      } else {
        sg_loop_body = m_ast->CreateBasicBlock();
        m_ast->AppendChild(sg_loop_body, sg_loop_copy);
      }
      void *offset = m_ast->CreateVariableRef(iter);
      CMarsExpression m_lb(sg_lb, m_ast, insert_pos);
      if ((m_lb != 0) != 0) {
        offset =
            m_ast->CreateExp(V_SgSubtractOp, offset, m_ast->CopyExp(sg_lb));
      }
      target_exp_addr =
          m_ast->CreateExp(V_SgPntrArrRefExp, target_exp_addr, offset);
    }

    m_ast->AppendChild(all_body, sg_outer_loop_copy);

    CMarsExpression me_actual_len(actual_burst_len, m_ast, burst_pos);
    void *assert_stmt = nullptr;
    if (me_actual_len.IsConstant() == 0) {
      void *burst_len_var = createTempVariableWithBoundAssertion(
          func_decl, sg_inner_loop_body, actual_burst_len, "merlin_len",
          *buffer_size, is_non_negative_len, &assert_stmt);
      actual_burst_len = m_ast->CreateVariableRef(burst_len_var);
    }

    void *source_expr = m_ast->CreateExp(
        V_SgPntrArrRefExp, m_ast->CreateVariableRef(sg_array),
        i == 0 ? burst_start_expr : m_ast->CopyExp(burst_start_expr));
    void *source_exp_addr = m_ast->CreateExp(V_SgAddressOfOp, source_expr);
    void *sizeof_expr = m_ast->CreateExp(
        V_SgMultiplyOp, m_ast->CreateExp(V_SgSizeOfOp, basic_type),
        actual_burst_len);
    if (read_codegen) {
      vector<void *> sg_read_init_list;
      sg_read_init_list.push_back(m_ast->CreateCastExp(
          target_exp_addr, m_ast->GetTypeByString("void *")));
      sg_read_init_list.push_back(m_ast->CreateCastExp(
          source_exp_addr, m_ast->GetTypeByString("const void *")));
      sg_read_init_list.push_back(sizeof_expr);
      void *sg_read_func_call =
          m_ast->CreateFuncCall("memcpy", m_ast->GetTypeByString("void"),
                                sg_read_init_list, scope_body, top_arg);
      burst_call = sg_read_func_call;
      *memcpy_r = m_ast->CreateStmt(V_SgExprStatement, sg_read_func_call);
      if (!is_non_negative_len) {
        void *cond = m_ast->CreateExp(
            V_SgGreaterOrEqualOp,
            m_ast->CreateSignnesCast(m_ast->CopyExp(burst_len)),
            m_ast->CreateConst(0));
        void *true_body = m_ast->CreateBasicBlock();
        if (assert_stmt != nullptr) {
          m_ast->AppendChild(true_body, assert_stmt);
        }
        m_ast->AppendChild(true_body, *memcpy_r);
        *memcpy_r = m_ast->CreateIfStmt(cond, true_body, nullptr);
      }
      m_ast->AppendChild(sg_inner_loop_body, *memcpy_r);
    }
    if (write_codegen) {
      vector<void *> sg_write_init_list;
      sg_write_init_list.push_back(m_ast->CreateCastExp(
          source_exp_addr, m_ast->GetTypeByString("void *")));
      sg_write_init_list.push_back(m_ast->CreateCastExp(
          target_exp_addr, m_ast->GetTypeByString("const void *")));
      sg_write_init_list.push_back(sizeof_expr);
      void *sg_write_func_call =
          m_ast->CreateFuncCall("memcpy", m_ast->GetTypeByString("void"),
                                sg_write_init_list, scope_body, top_arg);
      burst_call = sg_write_func_call;
      *memcpy_w = m_ast->CreateStmt(V_SgExprStatement, sg_write_func_call);
      if (!is_non_negative_len) {
        void *cond = m_ast->CreateExp(
            V_SgGreaterOrEqualOp,
            m_ast->CreateSignnesCast(m_ast->CopyExp(burst_len)),
            m_ast->CreateConst(0));
        void *true_body = m_ast->CreateBasicBlock();
        if (assert_stmt)
          m_ast->AppendChild(true_body, assert_stmt);
        m_ast->AppendChild(true_body, *memcpy_w);
        *memcpy_w = m_ast->CreateIfStmt(cond, true_body, nullptr);
      }
      m_ast->AppendChild(sg_inner_loop_body, *memcpy_w);
    }
    if (burst_call) {
      map<string, string> msg_map;
      msg_map["display_name"] =
          "auto memory burst for '" + m_ast->GetVariableName(top_arg) + "'";
      msg_map["burst"] = "on";
      msg_map["burst_length"] =
          CMarsExpression(sizeof_expr, m_ast, insert_pos).print_s();
      msg_map["burst_length_max"] = my_itoa(*buffer_size * element_size);
      if (read_codegen)
        msg_map["is_read"] = "1";
      if (write_codegen)
        msg_map["is_write"] = "1";
      insertMessage(m_ast, burst_call, msg_map);
      setNoHierarchy(m_ast, burst_call, 1);
    }
  }
  return true;
}

bool MemoryBurst::is_cg_trace_up(int burst_diagnosis_res) {
  return burst_diagnosis_res == BURST_APPLIED ||
         burst_diagnosis_res == INFINITE_RANGE ||
         burst_diagnosis_res == UNDETERMINED_LEN ||
         burst_diagnosis_res == NEGATIVE_BURST_LENGTH ||
         burst_diagnosis_res == NEGATIVE_OFFSET ||
         burst_diagnosis_res == UNSUPPORTED_REFERENCE;
}

bool MemoryBurst::is_burst_candidate(void *var_init, void *func, void *pos,
                                     bool force_on) {
  void *type = m_ast->GetTypebyVar(var_init);
  if ((m_ast->IsPointerType(type) == 0) && (m_ast->IsArrayType(type) == 0)) {
    return false;
  }
  if (m_ast->get_dim_num_from_var(var_init) != 1) {
    return false;
  }
  void *base_type = m_ast->GetBaseType(type, true);
  if ((m_ast->IsVoidType(base_type) != 0) ||
      m_ast->IsXilinxStreamType(base_type) != 0 ||
      m_ast->get_bitwidth_from_type(base_type, false) == 0) {
    return false;
  }
  //  YX: 20170913 disable burst for volatile interface
  if (m_ast->IsVolatileType(type) != 0) {
    return false;
  }
  if (!mMars_ir.every_trace_is_bus(m_ast, func, var_init, pos)) {
    return false;
  }
  if (force_on) {
    return true;
  }
  set<void *> top_args;
  mMars_ir.trace_to_bus_ports(m_ast, func, var_init, &top_args, pos);
  bool all_burst = true;
  for (auto arg : top_args) {
    if (mMars_ir_v2.get_burst_flag(arg)) {
      continue;
    }
    all_burst = false;
    break;
  }
  if (all_burst) {
    mArray2TopArg[var_init] = *top_args.begin();
  }
  return all_burst;
}

void *MemoryBurst::createTempVariableWithBoundAssertion(
    void *func_decl, void *sg_scope, void *initializer, string var_name,
    int64_t up_bound, bool is_non_negative_len, void **assert_stmt) {
  void *global = m_ast->GetGlobal(func_decl);
  if (m_assert_header_file.count(global) <= 0) {
    m_ast->AddHeader("\n#include<assert.h>\n", global);
    m_assert_header_file.insert(global);
  }
  m_ast->get_valid_local_name(func_decl, &var_name);
  void *tmp_var = m_ast->CreateVariableDecl(
      m_ast->GetTypeByString("unsigned long"), var_name, initializer, sg_scope);
  m_ast->AppendChild(sg_scope, tmp_var);
  void *assert_cond =
      m_ast->CreateExp(V_SgLessOrEqualOp, m_ast->CreateVariableRef(tmp_var),
                       m_ast->CreateConst(up_bound));
  vector<void *> vec_args;
  vec_args.push_back(assert_cond);
  void *assert_call = m_ast->CreateFuncCall(
      "assert", m_ast->GetTypeByString("void"), vec_args, sg_scope);
  void *local_assert_stmt = m_ast->CreateStmt(V_SgExprStatement, assert_call);
  if (is_non_negative_len) {
    m_ast->AppendChild(sg_scope, local_assert_stmt);
  } else {
    *assert_stmt = local_assert_stmt;
  }
  return tmp_var;
}

void MemoryBurst::add_offset_later(void *decl, void *parameter, void *call,
                                   void *offset_exp) {
  mParameter2CallAndOffset[decl][parameter][call] = offset_exp;
}

void MemoryBurst::add_offset() {
  while (!mParameter2CallAndOffset.empty()) {
    void *func_decl = mParameter2CallAndOffset.begin()->first;
    auto param_info = mParameter2CallAndOffset.begin()->second;
    vector<void *> vec_calls;
    m_ast->GetFuncCallsFromDecl(func_decl, nullptr, &vec_calls);
    for (auto param_call : param_info) {
      void *param = param_call.first;
      auto call_info = param_call.second;
      if (mParameter2Offset.count(param) <= 0) {
        string offset_name = m_ast->GetVariableName(param) + "_offset";
        m_ast->get_valid_local_name(func_decl, &offset_name);
        void *offset = m_ast->CreateVariable("long", offset_name, param);
        mParameter2Offset[param] = m_ast->GetFuncParamNum(func_decl);
        m_ast->insert_param_to_decl(func_decl, offset, false);
        for (auto call : vec_calls) {
          void *offset_exp = call_info.count(call) > 0 ? call_info[call]
                                                       : m_ast->CreateConst(0);
          m_ast->insert_argument_to_call(call, offset_exp);
        }
        update_array_index(param, m_ast->GetFuncBody(func_decl), offset);
      } else {
        int offset_pos = mParameter2Offset[param];
        for (auto call_offset : call_info) {
          void *call = call_offset.first;
          void *offset = call_offset.second;
          void *orig_offset = m_ast->GetFuncCallParam(call, offset_pos);
          void *new_offset =
              m_ast->CreateExp(V_SgAddOp, m_ast->CopyExp(orig_offset), offset);
          m_ast->ReplaceExp(orig_offset, new_offset);
        }
      }
    }
    mParameter2CallAndOffset.erase(func_decl);
  }
}

void MemoryBurst::update_array_index(void *param, void *scope, void *offset) {
  vector<void *> vec_refs;
  m_ast->get_ref_in_scope(param, scope, &vec_refs, true, true);
  std::reverse(vec_refs.begin(), vec_refs.end());
  for (auto ref : vec_refs) {
    void *pntr = nullptr;
    void *array;
    vector<void *> indexes;
    int pointer_dim;
    void *base_type = nullptr;
    m_ast->parse_standard_pntr_ref_from_array_ref(
        ref, &array, &pntr, &base_type, &indexes, &pointer_dim, ref);

    int arg_index = m_ast->GetFuncCallParamIndex(pntr);
    void *offset_ref = m_ast->CreateVariableRef(offset);
    if (pointer_dim == 0 && arg_index != -1) {
      void *call = m_ast->TraceUpToFuncCall(pntr);
      void *decl = m_ast->GetFuncDeclByCall(call);
      if (decl != nullptr) {
        void *param = m_ast->GetFuncParam(decl, arg_index);
        add_offset_later(decl, param, call, offset_ref);
        continue;
      }
    }
    assert(indexes.size() <= 1);
    void *new_index = offset_ref;
    if (indexes.size() == 1) {
      new_index =
          m_ast->CreateExp(V_SgAddOp, m_ast->CopyExp(indexes[0]), offset_ref);
    }
    void *new_pntr = m_ast->CreateExp(
        V_SgPntrArrRefExp, m_ast->CreateVariableRef(array), new_index);
    int curr_dim = 1;
    while (curr_dim > pointer_dim) {
      new_pntr = m_ast->CreateExp(V_SgAddressOfOp, new_pntr);
      curr_dim--;
    }
    m_ast->ReplaceExp(pntr, new_pntr);
  }
}

void MemoryBurst::cg_get_cached_options(void *scope) {
  if (m_cached_scope.count(scope) > 0) {
    return;
  }
  set<void *> cached_arrays;
  bool global_setting = false;
  for (auto child : m_ast->GetChildStmts(scope)) {
    if (m_ast->IsPragmaDecl(child) == 0) {
      continue;
    }
    CAnalPragma ana_pragma(m_ast);
    bool errorOut;
    if (ana_pragma.PragmasFrontendProcessing(child, &errorOut, false)) {
      if (ana_pragma.is_cache()) {
        string var_list = ana_pragma.get_attribute(CMOST_variable);
        if (var_list.empty()) {
          global_setting = true;
          break;
        }
        vector<string> vec_vars;
        m_ast->splitString(var_list, ",", &vec_vars);
        for (auto var_str : vec_vars) {
          void *var_init = m_ast->find_variable_by_name(var_str, child);
          if (var_init != nullptr) {
            cached_arrays.insert(var_init);
          }
        }
      }
    }
  }
  if (global_setting) {
    vector<void *> vec_refs;
    m_ast->GetNodesByType_int(scope, "preorder", V_SgVarRefExp, &vec_refs);
    for (auto ref : vec_refs) {
      void *type = m_ast->GetTypeByExp(ref);
      if ((m_ast->IsPointerType(type) != 0) ||
          (m_ast->IsArrayType(type) != 0)) {
        cached_arrays.insert(m_ast->GetVariableInitializedName(ref));
      }
    }
  }
  m_cached_scope[scope] = cached_arrays;
}

bool MemoryBurst::process_fine_grained_top() {
  vector<void *> fg_scopes;
  fg_get_loops_in_topology_order(&fg_scopes);
  bool Changed = false;
  cout << "infer small caching in fine grained scope for coalescing: " << endl;
  set<pair<void *, void *>> burst_candidate;
  vector<pair<void *, void *>> vec_burst_candidate;
  for (auto curr_scope : fg_scopes) {
    if (m_ast->is_located_in_scope(curr_scope, m_top_kernel) == 0) {
      continue;
    }
    // 1. Get all the initialized names of the interface arrays of the current
    // scope
    vector<void *> vec_refs;
    m_ast->GetNodesByType(curr_scope, "preorder", "SgVarRefExp", &vec_refs);

    set<void *> set_arrays;
    vector<void *> vec_arrays;
    void *func = m_ast->TraceUpToFuncDecl(curr_scope);
    for (auto ref : vec_refs) {
      void *var_init = m_ast->GetVariableInitializedName(ref);
      if (set_arrays.count(var_init) > 0) {
        continue;
      }
      if (m_ast->is_located_in_scope_simple(
              curr_scope, m_ast->TraceUpToScope(var_init)) == 0) {
        continue;
      }
      set_arrays.insert(var_init);
      if (is_burst_candidate(var_init, func, ref, true)) {
        vec_arrays.push_back(var_init);
      }
    }
    for (auto curr_array : vec_arrays) {
      if (!check_auto_caching_for_coalescing(curr_array, curr_scope)) {
        continue;
      }
      void *element_type =
          m_ast->GetBaseType(m_ast->GetTypebyVar(curr_array), true);
      int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
      int64_t new_buff_size = 0;
      bool report = false;
      int64_t access_size = 0;
      int is_cf_applied = is_cg_burst_available(
          curr_array, curr_scope, element_size, &new_buff_size, &access_size,
          false, report, true);
      if (is_cf_applied != BURST_APPLIED ||
          check_single_resource_limit(curr_array, new_buff_size,
                                      element_size)) {
        continue;
      }
      void *final_array;
      void *final_scope;
      final_array = curr_array;
      final_scope = curr_scope;
      if (m_lift_fine_grained_cache) {
        //  MER1029 try to lift the cache as long as the cache size is not
        //  changed
        vector<void *> vec_upper_scope;  //  from inner to outer
        vector<void *> vec_array_in_upper_scope;
        cg_get_options_for_lifting(curr_scope, curr_array, &vec_upper_scope,
                                   &vec_array_in_upper_scope);
        for (size_t i = 1; i < vec_upper_scope.size(); ++i) {
          int64_t lift_buff_size = 0;
          bool report = false;
          int64_t lift_access_size = 0;
          if (is_cg_burst_available(vec_array_in_upper_scope[i],
                                    vec_upper_scope[i], element_size,
                                    &lift_buff_size, &lift_access_size, false,
                                    report, true) != BURST_APPLIED) {
            break;
          }
          if (lift_buff_size > new_buff_size) {
            break;
          }

          final_array = vec_array_in_upper_scope[i];
          final_scope = vec_upper_scope[i];
        }
      }
      auto one_pair = std::make_pair(final_scope, final_array);
      if (burst_candidate.count(one_pair) > 0) {
        continue;
      }
      vec_burst_candidate.push_back(one_pair);
    }
  }
  stable_sort(vec_burst_candidate.begin(), vec_burst_candidate.end(),
              [=](auto one, auto other) {
                return other.first != one.first &&
                       m_ast->is_located_in_scope(other.first, one.first) == 1;
              });
  for (auto one_candidate : vec_burst_candidate) {
    void *final_scope = one_candidate.first;
    void *final_array = one_candidate.second;
    if (!is_burst_candidate(final_array, m_ast->TraceUpToFuncDecl(final_scope),
                            nullptr, true)) {
      continue;
    }
    if (!check_auto_caching_for_coalescing(final_array, final_scope)) {
      continue;
    }
    void *element_type =
        m_ast->GetBaseType(m_ast->GetTypebyVar(final_array), true);
    int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
    int64_t buff_size = 0;
    int64_t access_size = 0;
    int is_cf_applied =
        is_cg_burst_available(final_array, final_scope, element_size,
                              &buff_size, &access_size, false, false);
    if (is_cf_applied != BURST_APPLIED) {
      continue;
    }

    Changed |= cg_memory_burst_transform(final_scope, final_array, true, false);
  }
  return Changed;
}

bool MemoryBurst::check_auto_caching_for_coalescing(void *curr_array,
                                                    void *curr_scope) {
  set<void *> visited;
  int access_num = get_access_number(curr_array, curr_scope, &visited);
  return access_num > 1;
}

int MemoryBurst::get_access_number(void *array, void *sg_scope,
                                   set<void *> *visited) {
  if (visited->count(array) > 0) {
    return 0;
  }
  visited->insert(array);

  size_t i;
  //  1. get all the references
  vector<void *> vec_refs;
  m_ast->get_ref_in_scope(array, sg_scope, &vec_refs, true);
  int dim;
  //  int data_unit_size = 0;
  {
    void *sg_base_type;
    vector<size_t> sg_sizes;
    m_ast->get_type_dimension(m_ast->GetTypebyVar(array), &sg_base_type,
                              &sg_sizes, array);
    dim = sg_sizes.size();
  }

  //  2. handling the accesses
  //  2.1 element reference
  //  2.2 argument (with offset)
  //  2.3 full assignment (with offset)
  //  2.4 partial assignment (with offset) [ZP: supported, 20150821]
  //  2.5 memcpy (with offset)             [ZP: supported, 20150821]
  //  2.6 complex than assignment with offset => full range
  int count = 0;
  for (i = 0; i < vec_refs.size(); i++) {
    if (count >= 2) {
      return count;
    }
    void *curr_ref = vec_refs[i];

    void *sg_array = nullptr;
    void *sg_pntr = nullptr;
    vector<void *> sg_indexes;
    int pointer_dim;
    m_ast->parse_pntr_ref_by_array_ref(curr_ref, &sg_array, &sg_pntr,
                                       &sg_indexes, &pointer_dim);

    if ((sg_array == nullptr) || (sg_pntr == nullptr)) {
      continue;
    }

    void *loop = m_ast->TraceUpToLoopScope(curr_ref);
    bool paralled_loop = false;
    if ((loop != nullptr) &&
        (m_ast->is_located_in_scope_simple(loop, sg_scope) != 0)) {
      CMarsLoop *loop_info = mMars_ir_v2.get_loop_info(loop);
      if ((loop_info != nullptr) && (loop_info->has_parallel() != 0)) {
        paralled_loop = true;
      }
    }
    if (pointer_dim == dim) {
      //  1. full pntr
      count++;
      if (paralled_loop) {
        count++;
      }
      continue;
    }
    //  2. pointer assignement or function argument
    void *new_array = nullptr;
    void *new_scope = nullptr;

    //  2.0 point assignment lhs, e.g. int * array = ...;
    {
      //  FIXME: typically the array is current the alias of another array
      //       this reference is ignored in range analysis

      void *var_name;
      void *value_exp;
      if (m_ast->parse_assign(m_ast->GetParent(sg_pntr), &var_name,
                              &value_exp) != 0) {
        if (var_name == array) {
          continue;
        }
      }
    }

    //  2.1 point assignment rhs, e.g. int * alias = array;
    {
      void *var_name;
      void *value_exp;
      if (m_ast->parse_assign(m_ast->GetParent(sg_pntr), &var_name,
                              &value_exp) != 0) {
        if (value_exp == sg_pntr && (m_ast->IsInitName(var_name) != 0)) {
          new_array = var_name;
          new_scope = sg_scope;
        }
      }
    }

    //  2.2 function argument, e.g. sub_func(a); //  sub_func(int *b)
    {
      //  void *sg_array_ref;
      int arg_idx = m_ast->GetFuncCallParamIndex(sg_pntr);
      if (-1 != arg_idx) {
        void *func_call = m_ast->TraceUpToFuncCall(sg_pntr);
        assert(func_call);
        void *func_decl = m_ast->GetFuncDeclByCall(func_call);

        //  if ("memcpy" == GetFuncNameByCall(func_call))
        if (m_ast->IsMemCpy(func_call)) {
          continue;
        }
        if (m_ast->GetFuncNameByCall(func_call).find("memcpy_wide_bus_read") ==
                0 ||
            m_ast->GetFuncNameByCall(func_call).find("memcpy_wide_bus_write") ==
                0) {
          continue;
        }
        if (m_ast->GetFuncNameByCall(func_call).find("__merlin_") == 0) {
          //  skip internal intrinsics
          continue;
        }
        if (m_ast->IsFunctionDeclaration(func_decl) != 0) {
          new_array = m_ast->GetFuncParam(func_decl, arg_idx);
          new_scope = m_ast->GetFuncBody(func_decl);
        } else {
          //  undefined system call
        }
      }
    }
    if ((new_array != nullptr) && (new_scope != nullptr)) {
      int ret = get_access_number(new_array, new_scope, visited);
      if (paralled_loop) {
        ret *= 2;
      }
      count += ret;
      continue;
    }
  }

  return count;
}

int MemoryBurst::check_control_flow_availability(void *scope) {
  vector<void *> vec_ret_stmts;
  m_ast->GetNodesByType_int(scope, "postorder", V_SgReturnStmt, &vec_ret_stmts);
  if (!vec_ret_stmts.empty())
    return 0;
  vector<void *> vec_goto_stmts;
  m_ast->GetNodesByType_int(scope, "postorder", V_SgGotoStatement,
                            &vec_goto_stmts);
  for (auto goto_stmt : vec_goto_stmts) {
    void *label = m_ast->GetGotoLabel(goto_stmt);
    if (!label || !m_ast->IsLabelStatement(label) ||
        !m_ast->IsInScope(label, scope))
      return 0;
  }
  vector<void *> vec_break_stmts;
  m_ast->GetNodesByType_int(scope, "postorder", V_SgBreakStmt,
                            &vec_break_stmts);
  for (auto break_stmt : vec_break_stmts) {
    if (m_ast->IsScopeBreak(break_stmt, scope) &&
        !m_ast->IsLoopStatement(scope))
      return 0;
  }
  vector<void *> vec_continue_stmts;
  m_ast->GetNodesByType_int(scope, "postorder", V_SgContinueStmt,
                            &vec_continue_stmts);
  for (auto continue_stmt : vec_continue_stmts) {
    if (m_ast->IsScopeContinue(continue_stmt, scope) &&
        !m_ast->IsLoopStatement(scope))
      return 0;
  }
  return 1;
}
