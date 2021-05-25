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

#include "memory_partition.h"

using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

#define ConstHeuriThresh 1024
#define ConstHeuriWidth 128
#define CONST_ITER nullptr

/* Partition factor calculation based on GCD test. Consider the references
 * generated after loop unrolling */
//  factor: '0'- complete unroll; '1'- pipeline; "x"- x>1, unroll factor
//  opt: '-1'- pipeline; '0'- complete unroll; "x"- x>0, unroll factor
//       '-2'- not in pipeline/parallel region
bool MemoryPartition::partition_factor_anal(CMirNode *bNode,
                                            AccessInfo *access_info, int t_dim,
                                            int *factor) {
  int opt = loop_partition_builder->GetLoopOpt(bNode);
  auto *arr_init = access_info->array;
  string prior = bNode->get_attribute(CMOST_prior);
  void *sg_type = m_ast->GetTypebyVar(arr_init);
  void *base_type = m_ast->GetBaseType(sg_type, true);
  auto arr_size = cache_var_size[arr_init];
  int dim_size = cache_var_size[arr_init][t_dim];

  // FIXME: Yuxin, Nov/11/2019
  // Get the actual dimension size of the pointer
  // Use the biggest size if the pointer points to different arrays
  if (dim_size == 0 && m_ast->IsPointerType(sg_type) != 0) {
    int max_size = dim_size;
    void *func_decl = m_ast->GetEnclosingNode("Function", arr_init);
    map<void *, bool> res;
    mMars_ir->trace_to_bus(m_ast, func_decl, arr_init, &res, bNode->ref);
    for (auto r : res) {
      void *sg_type = m_ast->GetTypebyVar(r.first);
      void *base_type;
      vector<size_t> arg_size;  //  The size is stored reversely
      m_ast->get_type_dimension(sg_type, &base_type, &arg_size);
      if (arg_size.size() == arr_size.size()) {
        int size = arg_size[arg_size.size() - t_dim - 1];
        if (size > 0 && size > max_size) {
          max_size = size;
        }
      }
    }
    dim_size = max_size;
  }
  cache_var_size[arr_init][t_dim] = dim_size;

  int64_t default_threshold;
  int bitwidth = m_ast->get_bitwidth_from_type(base_type);
  if (m_ast->IsConstType(sg_type) != 0) {
    bool is_orig_const = true;
    if (m_ast->IsArgumentInitName(arr_init)) {
      auto res = cache_trace_res[arr_init];
      for (auto R : res) {
        auto orig_var = R.first;
        void *orig_sg_type = m_ast->GetTypebyVar(orig_var);
        if (m_ast->IsConstType(orig_sg_type) == 0) {
          is_orig_const = false;
          break;
        }
      }
    }
    if (is_orig_const && bitwidth <= ConstHeuriWidth)
      default_threshold = ConstHeuriThresh;
    else
      default_threshold = m_heuristic_threshold;
  } else {
    default_threshold = m_heuristic_threshold;
  }

  //  Step 2: Unroll the accesses, and remove the repeat accesses
  vector<map<void *, int>> index_expr_copy;
  map<int, int> repeat_tag;
  map<int, void *> index_pos;
  bool UNROLLED = unroll_ref_generate_full(bNode, access_info, t_dim,
                                           &index_expr_copy, &index_pos);

  // Step 3: after unrolling need to remove repeat index
  remove_repeated_ref(index_expr_copy, &repeat_tag, index_pos);
  access_info->index_copy_info[t_dim] = index_expr_copy;

  // Step 4: check out-of-bound access for constant index
  if (access_info->access_is_exact) {
    bool check_ret = check_invalid_access_bound(
        bNode->ref, &index_expr_copy, &repeat_tag, arr_init, dim_size, t_dim,
        access_info->mod_vals[t_dim]);
    if (check_ret) {
      loop_partition_builder->loop_info_map[bNode]->has_out_of_bound_access =
          true;
      return false;
    }
  }

  // Step 5: check existing partition is enough for
  // conflict-free access
  int ret =
      check_enough_partition(access_info, t_dim, &repeat_tag, factor, UNROLLED);
  if (ret)
    return true;

  if (opt == -2 && !access_info->constant_index) {
    cout << "Didnt FOUND non pipeline scope has constant index access\n";
    return false;
  }

  // Step 6: heuristic partition analysis,
  // if not find a partition factor
  int conflict_ref =
      static_cast<int>(index_expr_copy.size() - repeat_tag.size());
  void *curr_loop = m_ast->TraceUpToLoopScope(bNode->ref);
  if (curr_loop) {
    void *curr_iter = m_ast->GetLoopIterator(curr_loop);
    for (size_t i = 0; i < index_expr_copy.size(); i++) {
      if (repeat_tag.find(i) == repeat_tag.end()) {
        for (auto itt : index_expr_copy[i]) {
          if (itt.first == CONST_ITER)
            continue;
          if (itt.first == curr_iter)
            continue;

          // Loop variant, Heuristic analysis
          if (m_ast->IsExpression(itt.first) &&
              !m_ast->IsLoopInvariant(curr_loop, itt.first, index_pos[i])) {
            access_info->heuristic_on = true;
          }
        }
      }
    }
  }

  if (dim_size > 0 && conflict_ref > 1 && conflict_ref >= dim_size / 2) {
    *factor = 0;
    access_info->dual_option = false;
    if (dim_size > XilinxThresh) {
      *factor = 1024;
      reportThresholdFactor(bNode, arr_init, t_dim);
    }
    return true;
  }

  // HEURISTIC: Sep/3/2020
  //    When array index is directly related to the unroll loop
  //    but not unrolled due to the complex access pattern,
  //    we cannot count the real referece number.
  if (opt >= 0 && !access_info->heuristic_on) {
    if (conflict_ref <= 1) {
      *factor = 1;
      return true;
    }
    if (access_info->dual_option) {
      if (conflict_ref <= 2) {
        *factor = 2;
        return true;
      }
    }
  } else if (opt < 0) {
    if (((conflict_ref - opt - 1) / (-opt)) <= 1) {
      *factor = 1;
      return true;
    }
    if (access_info->dual_option) {
      if ((conflict_ref - opt - 1) / (-opt) <= 2) {
        *factor = 2;
        return true;
      }
    }
  }

#ifdef _DEBUG_PARTITION_
  int total_ref = static_cast<int>(index_expr_copy.size());
  cout << "INFO ==> "
       << "dimension size: " << dim_size << endl;
  cout << "         "
       << " total, conflict reference: " << total_ref << ", " << conflict_ref
       << endl;
  print_index(&index_expr_copy, &repeat_tag);
#endif

  // Step 7: conflict free analyaiss
  int bank_proc = conflict_ref;
  bank_renew(prior, &bank_proc);
  cout << "bank : " << bank_proc << " for dimension size " << dim_size << endl;
  if (bank_proc > XilinxThresh) {
    *factor = XilinxThresh;
    access_info->dual_option = false;
    reportThresholdFactor(bNode, arr_init, t_dim);
    return true;
  }

  //  Set a limit for the partitioning factor
  while (bank_proc > 1 && bank_proc <= XilinxThresh) {
#ifdef _DEBUG_PARTITION_
    cout << "[PARTITION-ANALYSIS] Trying bank=" << bank_proc << " for array '"
         << m_ast->_up(arr_init) << "' dimension " << t_dim + 1 << endl;
#endif

    bool conflict_tag =
        conflict_detect_full(bank_proc, index_expr_copy, repeat_tag, index_pos,
                             access_info->mod_vals[t_dim]);

    if (conflict_tag) {
      std::cout << "[PARTITION-ANALYSIS] Bank = " << bank_proc
                << " success for non-conflict partitioning" << endl;
      *factor = bank_proc;
      return true;
    }
    bank_proc++;
    bank_renew(prior, &bank_proc);
    if (dim_size > 1 && bank_proc > dim_size / 2) {
      break;
    }
  }
  cout << "Didnt find a partition factor\n" << endl;

  if (access_info->heuristic_on) {
    if (dim_size > 0 && dim_size <= default_threshold) {
      cout << "Heuristically complete partition the dimension\n";
      *factor = 0;
      access_info->dual_option = false;
      return true;
    }
  }

  //  Yuxin: 07/19/2019
  //  Add strong heuristic: when analysis failed, complete partition the
  //  dimension if there are more than 2 accesses
  //
  //  Yuxin: Nov/06/2019
  //  If cannot find the factor and decide to heuristically partition it,
  //  we completely partition it.  (according to xfopencv/hog case)
  if (access_info->has_self_assign == true)
    conflict_ref = conflict_ref * 2;
  if (conflict_ref >= 2 && access_info->local_pt_factor.empty() &&
      dim_size > 0 && dim_size <= default_threshold) {
    cout << "Heuristically complete partition the dimension\n";
    *factor = 0;
    return true;
  }
  return false;
}

/*Add references introduced by loop unrolling*/
//  opt: '-1'- pipeline; '0'- complete unroll; "x"- x>0, unroll factor
bool MemoryPartition::unroll_ref_generate_full(
    CMirNode *bNode, AccessInfo *access_info, int t_dim,
    vector<map<void *, int>> *index_expr_copy, map<int, void *> *index_pos) {
  auto index_expr_full = access_info->index_expr_array[t_dim];
  cout << "[print ==>] Unroll the references:"
       << "\n original refs: " << index_expr_full.size() << endl;

  int opt = loop_partition_builder->GetLoopOpt(bNode);
  bool UNROLLED = false;
  map<void *, vector<map<void *, int>>> index_expr_full_copy;
  if (opt != -2) {
    void *curr_loop = m_ast->TraceUpToLoopScope(bNode->ref);
    map<void *, set<void *>> loop_trace;
    for (auto it : index_expr_full) {
      set<void *> loops;
      m_ast->get_loop_nest_from_ref(curr_loop, it.first, &loops);
      loop_trace[it.first] = loops;
    }

    for (auto it : index_expr_full) {
      vector<map<void *, int>> index_expr_copy_local;
      UNROLLED |= unroll_ref_generate(access_info, opt, curr_loop, it.first,
                                      &it.second, &index_expr_copy_local);
      index_expr_full_copy[it.first] = index_expr_copy_local;
    }

    for (auto it : index_expr_full_copy) {
      vector<map<void *, int>> index_expr_copy_local = it.second;
      for (auto loop : loop_trace[it.first]) {
        int opt_1 = 0;
        if (loop != curr_loop) {
          vector<map<void *, int>> index_expr_copy_tmp;
          UNROLLED |=
              unroll_ref_generate(access_info, opt_1, loop, it.first,
                                  &index_expr_copy_local, &index_expr_copy_tmp);
          index_expr_copy_local = index_expr_copy_tmp;
        }
      }
      index_expr_full_copy[it.first] = index_expr_copy_local;
    }
  } else {
    index_expr_full_copy = index_expr_full;
  }

  int i = 0;
  for (auto it : index_expr_full_copy) {
    auto index_copies = it.second;
    auto pos_org = it.first;
    void *sg_pntr;
    int pointer_dim = 0;
    void *sg_array;
    vector<void *> sg_indexes;
    m_ast->parse_pntr_ref_by_array_ref(pos_org, &sg_array, &sg_pntr,
                                       &sg_indexes, &pointer_dim);
    for (auto copies : index_copies) {
      map<void *, int> new_index;
      for (auto pp : copies) {
        new_index[pp.first] = pp.second;
      }
      if (new_index.size() == 1 && new_index.count(CONST_ITER) > 0)
        access_info->constant_index &= true;
      else
        access_info->constant_index &= false;

      index_expr_copy->push_back(new_index);
      //  Yuxin: Jul 12
      //  Save all the original locations for the unrolled references
      void *sg_pos = sg_pntr != nullptr ? sg_pntr : pos_org;
      (*index_pos)[i] = sg_pos;
      i++;
    }
  }

#ifdef _DEBUG_PARTITION_
  print_index(index_expr_copy);
  cout << "[print ==>] Unroll the references, copied refs: "
       << index_expr_copy->size() << endl;
#endif
  return UNROLLED;
}

/*Add references introduced by loop unrolling*/
//  opt: '-1'- pipeline; '0'- complete unroll; "x"- x>0, unroll factor
bool MemoryPartition::unroll_ref_generate(
    AccessInfo *access_info, int opt, void *curr_loop, void *curr_ref,
    vector<map<void *, int>> *index_expr_full,
    vector<map<void *, int>> *index_expr_copy) {
  void *sg_pos = m_ast->GetEnclosingNode("Statement", curr_ref);
  void *curr_iter = m_ast->GetLoopIterator(curr_loop);
  void *curr_scope = m_ast->GetLoopBody(curr_loop);
  bool UNROLLED = false;
  bool local_visited = false;
  auto &visited_loops = addr_visited_loops[access_info][curr_ref];

  //  pipeline: No unroll required by default
  int64_t upper_bound = 1;
  int64_t lower_bound = 0;
  int64_t tripcount = 0;

  if (opt == 0) {
    //  complete unroll
    CMarsVariable m_loop(curr_loop, m_ast, sg_pos);
    CMarsRangeExpr mr = m_loop.get_range();
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    int ret = mr.get_simple_bound(&me_lb, &me_ub);
    if (ret == 0) {
      CMarsVariable m_loop(curr_loop, m_ast, curr_loop);
      mr = m_loop.get_range();
      ret = mr.get_simple_bound(&me_lb, &me_ub);
      access_info->access_is_exact = false;
    }
    if (ret == 0 || !mr.is_exact())
      access_info->access_is_exact = false;

    if (ret) {
      CMarsExpression me_range = me_ub - me_lb + 1;
      if ((me_lb.IsConstant() != 0) && (me_ub.IsConstant() != 0)) {
        lower_bound = me_lb.GetConstant();
        upper_bound = me_ub.GetConstant() + 1;
      }
      // Yuxin: 13/Jan/2020
      int64_t range = me_range.GetConstant();
      if (me_range.IsConstant() == 0 || range <= 0) {
        access_info->access_is_exact = false;
      }
#if 0
      cout << "For loop: " << m_ast->_p(curr_loop) << endl;
      cout << "curr pos: " << m_ast->_p(sg_pos) << endl;
      cout << "exact: " << access_info->access_is_exact << endl;
      cout << "lb: " << me_lb.print() << endl;
      cout << "ub: " << me_ub.print() << endl;
      cout << "range: " << me_range.print() << endl;
#endif
    } else {
      *index_expr_copy = *index_expr_full;
      return UNROLLED;
    }
  } else if (opt > 0) {
    //  partial unroll: use unroll factor as upperbound
    upper_bound = opt;
    lower_bound = 0;
  }
  tripcount = upper_bound - lower_bound;

#ifdef _DEBUG_PARTITION_
  cout << "Unroll loop: " << m_ast->_p(curr_loop) << endl;
  cout << "unroll_bound: " << lower_bound << " to " << upper_bound << endl;
  print_index(index_expr_full);
#endif

  for (auto idx : *index_expr_full) {
    //  A[i+1];
    //  linear related to loop iterator i
    if (idx.find(curr_iter) != idx.end()) {
      if (!local_visited) {
        if (visited_loops.count(curr_loop) > 0) {
          cout << "[print ==>] SKIP replicated unrolled reference\n";
          cout << m_ast->_up(curr_loop) << endl;
          *index_expr_copy = *index_expr_full;
          return UNROLLED;
        }
      }
      for (size_t j = lower_bound; j < upper_bound; j++) {
        map<void *, int> idx_new;
        int const_value = 0;
        if (idx.find(CONST_ITER) != idx.end()) {
          const_value = idx[CONST_ITER];
        }
        for (auto pair_coeff : idx) {
          if (pair_coeff.first == CONST_ITER)
            continue;
          if (pair_coeff.first == curr_iter) {
            const_value += static_cast<int>(idx[curr_iter] * j);
            if (tripcount > 1) {
              UNROLLED = true;
              visited_loops.insert(curr_loop);
              local_visited = true;
            }
          }
          // If fully parallelize the iterator, it will become constant
          if (opt != 0 || (opt == 0 && pair_coeff.first != curr_iter)) {
            idx_new[pair_coeff.first] = pair_coeff.second;
          }
        }
        idx_new[CONST_ITER] = const_value;
        index_expr_copy->push_back(idx_new);
      }
    } else {
      //  A[idx[i]+2];
      //  non-linear related to loop iterator i
      index_expr_copy->push_back(idx);
      for (auto pair_coeff : idx) {
        void *pair_expr = pair_coeff.first;
        if (pair_expr == nullptr) {
          continue;
        }
        vector<void *> vec_tmp;
        m_ast->GetNodesByType_compatible(pair_expr, "SgVarRef", &vec_tmp);
        for (size_t i = 0; i < vec_tmp.size(); i++) {
          void *ref = vec_tmp[i];
          void *init = m_ast->GetVariableInitializedName(ref);
          if (init == curr_iter) {
            if (opt == 0) {
              access_info->heuristic_on = true;
              break;
            }
          }
          //  int var1 = idx[i+1];
          //  int var = var1+2;
          //  A[idx1[var]+2];
          //  non-linear related to loop iterator i
          void *sg_exp =
              m_ast->GetSingleAssignExpr(init, sg_pos, 0, curr_scope);
          if (sg_exp != nullptr) {
            vector<void *> vec_tmp1;
            m_ast->GetNodesByType_compatible(sg_exp, "SgVarRef", &vec_tmp1);
            for (size_t i = 0; i < vec_tmp1.size(); i++) {
              void *ref = vec_tmp1[i];
              void *init1 = m_ast->GetVariableInitializedName(ref);
              if (init1 == curr_iter) {
                if (opt == 0) {
                  access_info->heuristic_on = true;
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
#ifdef _DEBUG_PARTITION_
  cout << "[output ==>] Unroll the references, copied refs: "
       << index_expr_copy->size() << endl;
  print_index(index_expr_copy);
#endif
  return UNROLLED;
}

#ifdef _DEBUG_PARTITION_
void MemoryPartition::print_index(vector<map<void *, int>> *index_expr) {
  for (size_t i = 0; i < index_expr->size(); i++) {
    for (auto iter : (*index_expr)[i]) {
      if (iter.first == nullptr) {
        cout << iter.second << "+";
      } else {
        cout << m_ast->_up(iter.first) << "*" << iter.second << "+";
      }
    }
    cout << endl;
  }
}

void MemoryPartition::print_index(vector<map<void *, int>> *index_expr,
                                  map<int, int> *repeat_tag) {
  cout << "PRINT final conflict references: \n";
  for (size_t i = 0; i < index_expr->size(); i++) {
    if (repeat_tag->find(i) == repeat_tag->end()) {
      for (auto iter : (*index_expr)[i]) {
        if (iter.first == nullptr) {
          cout << iter.second << "+";
        } else {
          cout << m_ast->_up(iter.first) << "*" << iter.second << "+";
        }
      }
      cout << endl;
    }
  }
}
#endif

/*Remove the identical references from a CMirNode for array 'arr_name'*/
void MemoryPartition::remove_repeated_ref(
    const vector<map<void *, int>> &index_expr, map<int, int> *repeat_tag,
    map<int, void *> index_pos) {
  for (size_t i = 0; i < index_expr.size(); i++) {
    for (size_t j = i + 1; j < index_expr.size(); j++) {
      bool diff = false;
      map<void *, int> index_i = index_expr[i];
      map<void *, int> index_j = index_expr[j];
      if (repeat_tag->find(j) == repeat_tag->end()) {
        if (index_i.size() != index_j.size()) {
          continue;
        }
        for (auto iter : index_i) {
          void *exp_1 = iter.first;
          if (index_j.find(iter.first) == index_j.end()) {
            bool found = false;
            for (auto iter2 : index_j) {
              void *exp_2 = iter2.first;
              if (m_ast->is_identical_expr(exp_1, exp_2, index_pos[i],
                                           index_pos[j])) {
                found = true;
                if (iter.second != iter2.second) {
                  diff = true;
                  break;
                }
              }
            }
            if (!found) {
              diff = true;
            }
          } else {
            if (iter.second != index_j[iter.first]) {
              diff = true;
            }
          }
          if (diff) {
            break;
          }
        }
        // Yuxin: for the same address
        if (!diff) {
          // index_j is the repeat of index_i
          (*repeat_tag)[j] = i;
#if 0
          // Yuxin: Sep/02/2020
          // Remove the constraint
          if (index_pos[i] != index_pos[j]) {
            if ((m_ast->has_write_conservative(index_pos[i]) != 0) ||
                (m_ast->has_write_conservative(index_pos[j]) != 0)) {
              *dual_option = false;
            }
          }
#endif
        }
      }
    }
  }

#if PROJDEBUG
  cout << "[print ==>] Unroll the references, repeated refs: "
       << repeat_tag->size() << endl;
#endif
}

// Yuxin: Jan/03/2020, check access valid bound
// Currently only for constant index, including constant after unroll
bool MemoryPartition::check_invalid_access_bound(
    void *sg_scope, vector<map<void *, int>> *index_expr,
    map<int, int> *repeat_tag, void *arr_init, int dim_size, int t_dim,
    int mod_size) {
  if (dim_size <= 0)
    return false;

  for (size_t i = 0; i < index_expr->size(); i++) {
    if (repeat_tag->find(i) == repeat_tag->end()) {
      if ((*index_expr)[i].size() > 1)
        continue;
      for (auto iter : (*index_expr)[i]) {
        if (iter.first != CONST_ITER)
          break;
        int value = (*index_expr)[i][CONST_ITER];
        if (mod_size != 1) {
          value %= mod_size;
        }
        if (value < 0 || value >= dim_size) {
          string type_info = m_ast->_up(m_ast->GetTypebyVar(arr_init));
          string array_info = "'" + m_ast->_up(arr_init) + "' ";
          array_info += getUserCodeFileLocation(m_ast, arr_init, true);
          array_info += " with type '" + type_info + "'";
          string msg = "dimension '" + my_itoa(t_dim + 1) + "' access_range=[" +
                       my_itoa(value) + "..." + my_itoa(value) + "]";
          cout << "constant index out of bound: " << msg << "," << dim_size
               << endl;
          void *sg_loop = m_ast->TraceUpToLoopScope(sg_scope);
          void *func_decl = m_ast->TraceUpToFuncDecl(sg_scope);
          string location_info;
          if (sg_loop) {
            string str_label = m_ast->get_internal_loop_label(sg_loop);
            location_info = "loop \'" + str_label + "\' " +
                            getUserCodeFileLocation(m_ast, sg_loop, true);
          } else {
            location_info =
                "function " + getUserCodeFileLocation(m_ast, func_decl, true);
          }
          dump_critical_message(FGPIP_ERROR_3(array_info, location_info, msg));
          return true;
        }
      }
    }
  }
  return false;
}

/*Conflict access detection. Once conflict is detected, array partition will be
 * applied.*/
bool MemoryPartition::conflict_detect_full(int bank_proc,
                                           vector<map<void *, int>> index_expr,
                                           map<int, int> repeat_tag,
                                           map<int, void *> index_pos,
                                           int mod_size) {
  for (size_t i = 0; i < index_expr.size(); i++) {
    for (size_t j = i + 1; j < index_expr.size(); j++) {
      map<void *, int> coeffs_tmp;
      map<void *, int> coeffs_tmp1;
      map<void *, void *> pos_tmp;
      vector<int> coeffs;
      int cn = 1;
      if (repeat_tag.find(j) == repeat_tag.end()) {
        for (auto iter_a : index_expr[i]) {
          if (iter_a.first != nullptr) {
            if (index_expr[j].find(iter_a.first) == index_expr[j].end()) {
              coeffs_tmp[iter_a.first] = iter_a.second;
            } else {
              coeffs_tmp[iter_a.first] =
                  (iter_a.second - index_expr[j][iter_a.first]);
            }
            pos_tmp[iter_a.first] = index_pos[i];
          } else {
            cn = iter_a.second - index_expr[j][iter_a.first];
          }
        }
        for (auto iter_b : index_expr[j]) {
          if (iter_b.first != nullptr) {
            if (index_expr[i].find(iter_b.first) == index_expr[i].end()) {
              coeffs_tmp[iter_b.first] = -iter_b.second;
              pos_tmp[iter_b.first] = index_pos[j];
            }
          }
        }
        set<void *> coeffs_find;
        for (auto &iter1 : coeffs_tmp) {
          if (coeffs_find.count(iter1.first) > 0) {
            continue;
          }
          for (auto &iter2 : coeffs_tmp) {
            if (iter1 == iter2) {
              coeffs_tmp1[iter1.first] = iter1.second;
              continue;
            }
            void *exp_1 = iter1.first;
            void *exp_2 = iter2.first;
            if (m_ast->is_identical_expr(exp_1, exp_2, pos_tmp[exp_1],
                                         pos_tmp[exp_2])) {
              int new_value = iter1.second + iter2.second;
              coeffs_tmp1[exp_1] = new_value;
              coeffs_find.insert(exp_2);
            }
          }
        }

        for (auto iter1 : coeffs_tmp1) {
          coeffs.push_back(iter1.second);
        }
        if (coeffs.empty()) {
          coeffs.push_back(0);
        }

        if (mod_size == 1) {
          if (intdiv(cn, gcd2(ngcd(coeffs, coeffs.size()), bank_proc)) == 0) {
            return false;
          }
        } else {
          if (intdiv(cn, gcd3(ngcd(coeffs, coeffs.size()), bank_proc,
                              mod_size)) == 0) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

void MemoryPartition::bank_renew(string prior, int *bank_proc) {
  if (*bank_proc == 1) {
    return;
  }
  //  Partition factor can only be power-of-two
  if (prior == "logic") {
    while (!isPo2(*bank_proc)) {
      (*bank_proc)++;
    }
  }
}

int MemoryPartition::trace_up_to_loop_iterator(void *ref, void **sg_iter) {
  void *arr_var = m_ast->GetVariableInitializedName(ref);
  list<t_func_call_path> vec_paths;
  int ret = m_ast->get_call_paths(ref, m_ast->GetProject(), &vec_paths);
  t_func_call_path call_path;
  if ((ret != 0) && !vec_paths.empty()) {
    call_path = *vec_paths.begin();
  }
#if 0
  t_func_call_path::iterator fp;
  for (fp = call_path.begin(); fp != call_path.end(); fp++) {
    cout  << "callee: " << m_ast->_p(fp->first) <<endl;
    cout  << "call: " << m_ast->_p(fp->second) <<endl;
  }
#endif
  vector<void *> sg_loops;
  m_ast->get_loop_nest_in_scope(ref, m_ast->GetProject(), call_path, &sg_loops);
  for (auto curr_loop : sg_loops) {
    void *curr_iter = m_ast->GetLoopIterator(curr_loop);
    if (m_ast->is_same_instance(arr_var, curr_iter, call_path) != 0) {
      *sg_iter = curr_iter;
      return 1;
    }
  }
  return 0;
}

void MemoryPartition::index_trace_up(void *scope, void **ref) {
  list<t_func_call_path> vec_paths;
  int ret = m_ast->get_call_paths(*ref, m_ast->GetProject(), &vec_paths);
  if (ret == 0) {
    return;
  }
  t_func_call_path call_path;

  //  For multiple call paths
  t_func_call_path::iterator fp;
  bool find_path = false;
  for (auto path : vec_paths) {
    for (fp = path.begin(); fp != path.end(); fp++) {
      if (m_ast->IsInScope(fp->second, scope) != 0) {
        call_path = path;
        find_path = true;
        break;
      }
    }
    if (find_path) {
      break;
    }
  }

#if 0
  for (fp = call_path.begin(); fp != call_path.end(); fp++) {
    cout  << "callee: " << m_ast->_p(fp->first) <<endl;
    cout  << "call: " << m_ast->_p(fp->second) <<endl;
  }
#endif

  for (fp = call_path.begin(); fp != call_path.end(); fp++) {
    if (m_ast->IsVarRefExp(*ref) == 0) {
      return;
    }
    void *arr_var = m_ast->GetVariableInitializedName(*ref);
    if (m_ast->IsArgumentInitName(arr_var) == 0) {
      return;
    }
    void *call = fp->second;
    if (m_ast->IsFunctionCall(call) == 0) {
      continue;
    }

    void *func_decl = m_ast->TraceUpToFuncDecl(arr_var);
    if (func_decl == nullptr) {
      return;
    }
    int idx = m_ast->GetFuncParamIndex(arr_var);
    assert(-1 != idx);

    void *callee = fp->first;
    if (m_ast->IsMatchedFuncAndCall(func_decl, call, callee)) {
      void *actual_exp = m_ast->GetFuncCallParam(call, idx);
      *ref = actual_exp;
      break;
    }
  }
}

void MemoryPartition::reportThresholdFactor(CMirNode *node, void *arr,
                                            int dim) {
  void *sg_loop = m_ast->TraceUpToLoopScope(node->ref);
  if (sg_loop == nullptr)
    return;
  string str_label = m_ast->get_internal_loop_label(sg_loop);
  void *iter = m_ast->GetLoopIterator(sg_loop);
  if (iter) {
    str_label = m_ast->UnparseToString(iter);
  }
  string array_info = "'" + m_ast->UnparseToString(arr) + "'";
  string threshold_info = my_itoa(XilinxThresh);
  string dim_info = my_itoa(dim + 1);
  string loop_info =
      "\'" + str_label + "\' " + getUserCodeFileLocation(m_ast, sg_loop, true);
  if (node->has_parallel() != 0) {
    dump_critical_message(
        FGPIP_WARNING_15(loop_info, array_info, dim_info, threshold_info));
  }
  if (node->has_pipeline() != 0) {
    dump_critical_message(
        FGPIP_WARNING_22(loop_info, array_info, dim_info, threshold_info));
  }
}

int MemoryPartition::check_enough_partition(AccessInfo *access_info,
                                            int target_dim,
                                            map<int, int> *repeat_tag,
                                            int *factor, bool UNROLLED) {
  // Check previous partition and access pattern
  // combine multiple dimensions to check the unique of the access
  // such as: a[0][0], a[0][1], a[1][0]
  map<int, int> combined_repeat_tag;
  bool start = false;
  // int current_copies = access_info->index_copy_info[target_dim].size();
  for (auto pair : access_info->repeat_access_info) {
    auto dim = pair.first;
    auto exist_repeat_tag = pair.second;
    // Assume that if there is partitioning on the dimension,
    // the accesses are conflict free
    if (access_info->local_pt_factor.count(dim) == 0)
      continue;
    for (auto r : exist_repeat_tag) {
      auto repeat_ref = r.first;
      auto repeat_to_ref = r.second;
      if (!start) {
        combined_repeat_tag[repeat_ref] = repeat_to_ref;
      } else {
        if (combined_repeat_tag.count(repeat_ref) > 0) {
          if (combined_repeat_tag[repeat_ref] != repeat_to_ref)
            combined_repeat_tag.erase(repeat_ref);
        }
      }
    }
    if (!start) {
      start = true;
    } else {
      vector<int> to_erase;
      for (auto r : combined_repeat_tag) {
        auto repeat_ref = r.first;
        if (exist_repeat_tag.count(repeat_ref) == 0) {
          to_erase.push_back(repeat_ref);
        }
      }
      for (auto t : to_erase)
        combined_repeat_tag.erase(t);
    }
  }

  if (!access_info->local_pt_factor.empty()) {
    bool same_repeat = true;
    for (auto r : combined_repeat_tag) {
      if (repeat_tag->count(r.first) > 0) {
        if ((*repeat_tag)[r.first] != r.second) {
          same_repeat = false;
          break;
        }
      } else {
        same_repeat = false;
        break;
      }
    }
    if (same_repeat && !UNROLLED) {
      *factor = 1;
      cout << "Hooray! Indices are different!! \n";
      return true;
    }
  }
  access_info->repeat_access_info[target_dim] = *repeat_tag;
  return false;
}
