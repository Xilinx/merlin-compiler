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


#include "memory_reduce.h"
#include <iostream>
#include <string>
#include <tuple>
#include "cmdline_parser.h"
#include "codegen.h"
#include "comm_opt.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "program_analysis.h"
using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using std::map;
using std::set;
using std::tuple;
using std::vector;

int memory_reduce_top(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &option) {
  MemoryReduce instance(codegen, pTopFunc, option);
  instance.run();
  return 0;
}

MemoryReduce::MemoryReduce(CSageCodeGen *codegen, void *pTopFunc,
                           const CInputOptions &option)
    : mCodegen(codegen) {
  mMars_ir.build_mars_ir(codegen, pTopFunc, false, true);
}

bool MemoryReduce::run() {
  bool Changed = false;
  size_t num_node = mMars_ir.size();
  size_t i = 0;
  bool LocalChanged = false;
  while (i < num_node) {
    CMarsNode *node = mMars_ir.get_node(i);
    LocalChanged |= process_one_node(node, ONE_ITERATION);
    ++i;
  }

  Changed |= LocalChanged;
  if (LocalChanged) {
    mCodegen->init_defuse();
  }

  i = 0;
  LocalChanged = false;
  while (i < num_node) {
    CMarsNode *node = mMars_ir.get_node(i);
    LocalChanged |= process_one_node(node, NO_INTRA_DEP);
    ++i;
  }

  Changed |= LocalChanged;
  if (LocalChanged) {
    mCodegen->init_defuse();
  }

  i = 0;
  LocalChanged = false;
  while (i < num_node) {
    CMarsNode *node = mMars_ir.get_node(i);
    LocalChanged |= optimize_local_memory_layout(node);
    ++i;
  }
  Changed |= LocalChanged;
  return Changed;
}

bool MemoryReduce::process_one_node(CMarsNode *node, enum reduce_type rt) {
  set<void *> all_refs;
  vector<void *> loops = node->get_loops();
  auto ports = node->get_ports();
  if (loops.empty()) {
    return false;
  }
  void *inner_loop = loops.back();
  void *loop_body = mCodegen->GetLoopBody(inner_loop);
  if (loop_body == nullptr) {
    return false;
  }
  void *scope = loops[0];
  void *func_decl = mCodegen->TraceUpToFuncDecl(inner_loop);
  vector<void *> refs;
  mCodegen->GetNodesByType(loop_body, "preorder", "SgVarRefExp", &refs);
  //  1 collect all the local arrays
  set<void *> local_vars;
  set<void *> local_varswithivdep;
  map<void *, set<void *>> local_var2pntr;
  map<void *, void *> old_var2new_var;
  for (auto &var_ref : refs) {
    void *sg_pntr = nullptr;
    void *var_init = nullptr;
    int pointer_dim = 0;
    vector<void *> sg_indexes;
    mCodegen->parse_pntr_ref_by_array_ref(var_ref, &var_init, &sg_pntr,
                                          &sg_indexes, &pointer_dim, var_ref);
    if (mCodegen->GetLoopFromIteratorByPos(var_init, var_ref) != nullptr) {
      continue;
    }
    if (var_init == nullptr) {
      continue;
    }
    if (ports.count(var_init) > 0) {
      continue;
    }
    void *var_type = mCodegen->GetTypebyVar(var_init);
    if (mCodegen->IsArrayType(var_type) == 0) {
      continue;
    }
    //  do not touch local variables with ivdep spec
    void *parent = mCodegen->GetParent(sg_pntr);
    if (parent != nullptr) {
      void *parent_parent = mCodegen->GetParent(parent);
      if ((parent_parent != nullptr) &&
          (mCodegen->IsFunctionCall(parent_parent) != 0)) {
        string func_name = mCodegen->GetFuncNameByCall(parent_parent);
        if (func_name.find(IVDEP_SPEC_PREFIX) == 0) {
          local_varswithivdep.insert(var_init);
          continue;
        }
      }
    }
    local_vars.insert(var_init);
    local_var2pntr[var_init].insert(sg_pntr);
  }
  //  2 check each array is candidate to do memory reduction
  //  2.1 the indecies of all the references should be simple type
  //  2.2 if the index of one dimension of one ref is related to one outer loop,
  //    then all other references should have the same index in the same
  //    dimension
  //
  bool Changed = false;
  for (auto var : local_vars) {
    if (local_varswithivdep.count(var) > 0) {
      continue;
    }
    old_var2new_var[var] = var;
    set<void *> refs = local_var2pntr[var];
    if (refs.size() <= 1) {
      continue;
    }
    if (mCodegen->GetInitializer(var) != nullptr) {
      continue;
    }
    vector<vector<tuple<void *, int64_t, int64_t>>> index_info_vec;
    bool valid = true;
    for (auto &ref : refs) {
      CMarsAccess ac(ref, mCodegen, loop_body);
      if (!ac.is_simple_type_v1()) {
        valid = false;
        break;
      }
      auto dim_to_var = ac.simple_type_v1_get_dim_to_var();
      index_info_vec.push_back(dim_to_var);
    }

    if (!valid) {
      continue;
    }
    auto dim_to_var0 = index_info_vec[0];
    set<void *> outer_loop_set(loops.begin(), loops.end());
    map<int, int> dim2outer_loop;
    for (size_t i = 0; i != dim_to_var0.size(); ++i) {
      auto index_info = dim_to_var0[i];
      void *loop = std::get<0>(index_info);
      if (outer_loop_set.count(loop) <= 0) {
        continue;
      }
      for (size_t j = 0; j < loops.size(); ++j) {
        if (loops[j] == loop) {
          dim2outer_loop[i] = j;
          break;
        }
      }
    }
    //  if there is no index related to outer loop iterator
    //  no reduction benefit
    if (dim2outer_loop.empty()) {
      continue;
    }

    for (size_t i = 1; i < index_info_vec.size(); ++i) {
      auto dim_to_var1 = index_info_vec[i];
      assert(dim_to_var0.size() == dim_to_var1.size());
      for (size_t j = 0; j != dim_to_var0.size(); ++j) {
        auto index_info0 = dim_to_var0[j];
        auto index_info1 = dim_to_var1[j];
        if (index_info0 == index_info1) {
          //  same index
          continue;
        }
        //  different index
        void *loop0 = std::get<0>(index_info0);
        void *loop1 = std::get<0>(index_info1);
        if (outer_loop_set.count(loop0) <= 0 &&
            outer_loop_set.count(loop1) <= 0) {
          //  false different index because it is related to
          //  paralleled inner loops
          continue;
        }
        valid = false;
        break;
      }
      if (!valid) {
        break;
      }
    }
    if (!valid) {
      continue;
    }
    void *var_type = mCodegen->GetTypebyVar(var);
    vector<size_t> dims;
    void *base_type = nullptr;
    mCodegen->get_type_dimension(var_type, &base_type, &dims, var);

    set<int> reduced_dims;
    if (rt == ONE_ITERATION) {
      check_const_indices(&reduced_dims, dim_to_var0);
      check_one_iteration(&reduced_dims, dim2outer_loop);
    } else if (rt == NO_INTRA_DEP) {
      valid = check_data_dependence(&reduced_dims, dim2outer_loop, loops, refs,
                                    var, scope, dims);
    } else {
      valid = false;
    }
    if (!valid || reduced_dims.empty()) {
      continue;
    }
    //  reduce all dimensions in set 'reduced_dims'
    //  Create new variable
    void *new_var_type = base_type;
    vector<size_t> new_dims;
    for (size_t dim = dims.size(); dim > 0; --dim) {
      if (reduced_dims.count(dims.size() - dim) > 0) {
        continue;
      }
      new_dims.push_back(dims[dim - 1]);
    }
    if (!new_dims.empty()) {
      new_var_type = mCodegen->CreateArrayType(base_type, new_dims);
    }
    string new_var_name = mCodegen->GetVariableName(var) + "_rd";
    mCodegen->get_valid_local_name(func_decl, &new_var_name);
    void *orig_var_decl = mCodegen->GetVariableDecl(var);
    void *scope = mCodegen->TraceUpToBasicBlock(orig_var_decl);
    void *new_var_decl = mCodegen->CreateVariableDecl(
        new_var_type, new_var_name, nullptr, scope);
    mCodegen->InsertStmt(new_var_decl, orig_var_decl);
    old_var2new_var[var] = mCodegen->GetVariableInitializedName(new_var_decl);
    //  Replace all the original reference
    vector<void *> vec_refs(refs.begin(), refs.end());
    for (auto &ref : vec_refs) {
      CMarsAccess ac(ref, mCodegen, loop_body);
      void *new_ref = mCodegen->CreateVariableRef(new_var_decl);
      for (size_t dim = 0; dim < dims.size(); ++dim) {
        if (reduced_dims.count(dim) > 0) {
          continue;
        }
        new_ref = mCodegen->CreateExp(V_SgPntrArrRefExp, new_ref,
                                      ac.GetIndex(dim).get_expr_from_coeff());
      }
      mCodegen->ReplaceExp(ref, new_ref);
    }
    mCodegen->RemoveStmt(orig_var_decl);
    Changed = true;
  }

  return Changed;
}

void MemoryReduce::check_const_indices(
    set<int> *reduced_dims,
    const vector<tuple<void *, int64_t, int64_t>> &dim2vars) {
  int dim = 0;
  for (auto index_info : dim2vars) {
    if (std::get<0>(index_info) == nullptr) {
      reduced_dims->insert(dim);
    }
    dim++;
  }
}

void MemoryReduce::check_one_iteration(set<int> *reduced_dims,
                                       const map<int, int> &dim2outer_loop) {
  map<int, int> outer_loop2dim;
  for (auto elem : dim2outer_loop) {
    outer_loop2dim[elem.second] = elem.first;
  }
  int outmost_level = 0;
  for (auto elem : outer_loop2dim) {
    if (elem.first > outmost_level) {
      break;
    }
    reduced_dims->insert(elem.second);
    outmost_level++;
  }
}

bool MemoryReduce::check_data_dependence(set<int> *reduced_dims,
                                         const map<int, int> &dim2outer_loop,
                                         const vector<void *> &outer_loops,
                                         const set<void *> &refs,
                                         void *sg_array, void *sg_scope,
                                         const vector<size_t> &dims) {
  //  1. check whether there is readwrite access on a single ref
  vector<void *> r_refs;
  vector<void *> w_refs;
  for (auto &ref : refs) {
    bool read = mCodegen->has_read_conservative(ref) != 0;
    bool write = mCodegen->has_write_conservative(ref) != 0;
    if (read) {
      r_refs.push_back(ref);
    }
    if (write) {
      w_refs.push_back(ref);
    }
  }

  //  do not touch readonly or writeonly variables
  if (r_refs.empty() || w_refs.empty()) {
    return false;
  }

  int first = 1;
  //  2. check whether all the read ref happen after at least one write ref
  //  2.1 for each read ref, collect all the write ref dominating it(spatial)
  for (auto r_ref : r_refs) {
    void *r_stmt = mCodegen->TraceUpToStatement(r_ref);
    vector<void *> vec_w_refs;
    for (auto w_ref : w_refs) {
      void *w_stmt = mCodegen->TraceUpToStatement(w_ref);
      if (!is_located_before(w_stmt, r_stmt)) {
        continue;
      }
      vec_w_refs.push_back(w_ref);
    }
    if (vec_w_refs.empty()) {
      return false;
    }
    //  2.2 check whether the write range is exact and cover
    //  the read range
    vector<CMarsRangeExpr> vec_mr_w;
    vector<CMarsRangeExpr> vec_mr_r;
    if (vec_w_refs.size() == w_refs.size()) {
      ArrayRangeAnalysis scope_range(sg_array, mCodegen, sg_scope, sg_scope,
                                     false, false);
      vec_mr_w = scope_range.GetRangeExprWrite();
      vec_mr_r = scope_range.GetRangeExprWrite();
    } else {
      //  FIXME get a common scope to handle if-else issue
      for (auto w_ref : vec_w_refs) {
        ArrayRangeAnalysis acc(sg_array, mCodegen, w_ref, sg_scope, false,
                               false);
        vector<CMarsRangeExpr> one_range = acc.GetRangeExprWrite();
        vec_mr_w = RangeUnioninVector(vec_mr_w, one_range);
      }

      ArrayRangeAnalysis acc(sg_array, mCodegen, r_ref, sg_scope, false, false);
      vec_mr_r = acc.GetRangeExprRead();
    }
    assert(vec_mr_w.size() == vec_mr_r.size());
    for (size_t i = 0; i < vec_mr_w.size(); ++i) {
      if (dim2outer_loop.count(i) <= 0) {
        continue;
      }
      CMarsRangeExpr mr_w = vec_mr_w[i];
      CMarsRangeExpr mr_r = vec_mr_r[i];
      CMarsRangeExpr res;
      if ((mr_w.is_exact() == 0) ||
          ((mr_w.is_cover(mr_r, &res) == 0) &&
           !mr_w.is_full_access(dims[dims.size() - 1 - i]))) {
        return false;
      }
    }

    map<int, int> outer_loop2dim;
    for (auto &rel : dim2outer_loop) {
      outer_loop2dim[rel.second] = rel.first;
    }
    //  3. check the reduced dimensions

    bool hit_outermost_related_loop = false;
    set<int> curr_reduced_dims;
    for (size_t i = 0; i < outer_loops.size(); ++i) {
      if (outer_loop2dim.count(i) > 0) {
        hit_outermost_related_loop = true;
        curr_reduced_dims.insert(outer_loop2dim[i]);
        continue;
      }
      void *loop = outer_loops[i];

      CMarsRangeExpr w_mr =
          CMarsVariable(loop, mCodegen, vec_w_refs[0]).get_range();
      for (size_t j = 1; j < vec_w_refs.size(); ++j) {
        w_mr = w_mr.Union(
            CMarsVariable(loop, mCodegen, vec_w_refs[j]).get_range());
      }

      CMarsRangeExpr r_mr = CMarsVariable(loop, mCodegen, r_ref).get_range();

      //  if the index of current loop is fixed, skip it
      if (w_mr.is_single_access() && r_mr.is_single_access() &&
          ((w_mr.get_lb(0) == r_mr.get_lb(0)) != 0)) {
        continue;
      }

      if (!hit_outermost_related_loop) {
        //  check all the unrelated outer loops in which all the write ref
        //  covers all the read ref
        CMarsRangeExpr res;
        if (w_mr.is_cover(r_mr, &res) != 0) {
          continue;
        }
        return false;
      }
      //  2.2 check whether all the write ref happens before all the read refs
      //  (temporal)
      //  check the low bound of w_mr is no more than the low bound of r_mr
      if ((w_mr.has_lb() == 0) || (r_mr.has_lb() == 0)) {
        return false;
      }
      vector<CMarsExpression> w_mr_lb = w_mr.get_lb_set();
      vector<CMarsExpression> r_mr_lb = r_mr.get_lb_set();
      if (w_mr_lb.size() != 1 || r_mr_lb.size() != 1) {
        return false;
      }
      CMarsExpression diff = r_mr_lb[0] - w_mr_lb[0];
      if (diff.IsNonNegative() == 0) {
        return false;
      }
    }
    if (first != 0) {
      first = 0;
      *reduced_dims = curr_reduced_dims;
    } else {
      set<int> tmp = *reduced_dims;
      reduced_dims->clear();
      std::set_intersection(
          tmp.begin(), tmp.end(), curr_reduced_dims.begin(),
          curr_reduced_dims.end(),
          std::inserter(*reduced_dims, reduced_dims->begin()));
    }
  }
  return true;
}

bool MemoryReduce::is_located_before(void *one_node, void *other_node) {
  if (one_node == other_node || (one_node == nullptr) ||
      (other_node == nullptr)) {
    return false;
  }

  if ((mCodegen->IsScopeStatement(one_node) != 0) &&
      (mCodegen->IsInScope(other_node, one_node) != 0)) {
    return false;
  }

  if ((mCodegen->IsScopeStatement(other_node) != 0) &&
      (mCodegen->IsInScope(one_node, other_node) != 0)) {
    return false;
  }

  void *one_parent = mCodegen->GetParent(one_node);
  void *other_parent = mCodegen->GetParent(other_node);
  if (one_parent == other_parent &&
      (mCodegen->IsScopeStatement(one_parent) != 0)) {
    size_t one_idx = mCodegen->GetChildStmtIdx(one_parent, one_node);
    size_t other_idx = mCodegen->GetChildStmtIdx(one_parent, other_node);
    return one_idx < other_idx;
  }
  if (is_located_before(one_node, other_parent)) {
    return true;
  }
  if (is_located_before(one_parent, other_node)) {
    return true;
  }
  return false;
}

bool MemoryReduce::optimize_local_memory_layout(CMarsNode *node) {
  set<void *> all_refs;
  vector<void *> loops = node->get_loops();
  auto ports = node->get_ports();
  if (loops.empty()) {
    return false;
  }
  void *inner_loop = loops.back();
  void *loop_body = mCodegen->GetLoopBody(inner_loop);
  if (loop_body == nullptr) {
    return false;
  }

  void *func_decl = mCodegen->TraceUpToFuncDecl(inner_loop);
  vector<void *> refs;
  mCodegen->GetNodesByType(loop_body, "preorder", "SgVarRefExp", &refs);
  //  1 collect all the local arrays
  set<void *> local_vars;
  set<void *> local_varswithivdep;
  map<void *, set<void *>> local_var2pntr;
  map<void *, void *> old_var2new_var;
  for (auto &var_ref : refs) {
    void *sg_pntr = nullptr;
    void *var_init = nullptr;
    int pointer_dim = 0;
    vector<void *> sg_indexes;
    mCodegen->parse_pntr_ref_by_array_ref(var_ref, &var_init, &sg_pntr,
                                          &sg_indexes, &pointer_dim, var_ref);
    if (mCodegen->GetLoopFromIteratorByPos(var_init, var_ref) != nullptr) {
      continue;
    }
    if (var_init == nullptr) {
      continue;
    }
    if (ports.count(var_init) > 0) {
      continue;
    }
    void *var_type = mCodegen->GetTypebyVar(var_init);
    if (mCodegen->IsArrayType(var_type) == 0) {
      continue;
    }
    //  do not touch local variables with ivdep spec
    void *parent = mCodegen->GetParent(sg_pntr);
    if (parent != nullptr) {
      void *parent_parent = mCodegen->GetParent(parent);
      if ((parent_parent != nullptr) &&
          (mCodegen->IsFunctionCall(parent_parent) != 0)) {
        string func_name = mCodegen->GetFuncNameByCall(parent_parent);
        if (func_name.find(IVDEP_SPEC_PREFIX) == 0) {
          local_varswithivdep.insert(var_init);
          continue;
        }
      }
    }
    local_vars.insert(var_init);
    local_var2pntr[var_init].insert(sg_pntr);
  }
  //  2 check each array is candidate to do memory layout reordoring
  //  2.1 the indecies of all the references should be simple type
  //  2.2 if the index of one dimension of one ref is related to one outer loop,
  //    then all other references should have the same index in the same
  //    dimension
  //
  bool Changed = false;
  for (auto var : local_vars) {
    if (local_varswithivdep.count(var) > 0) {
      continue;
    }
    old_var2new_var[var] = var;
    set<void *> refs = local_var2pntr[var];
    if (refs.size() <= 1) {
      continue;
    }
    if (mCodegen->GetInitializer(var) != nullptr) {
      continue;
    }
    vector<vector<tuple<void *, int64_t, int64_t>>> index_info_vec;
    bool valid = true;
    for (auto &ref : refs) {
      CMarsAccess ac(ref, mCodegen, loop_body);
      if (!ac.is_simple_type_v1()) {
        valid = false;
        break;
      }
      auto dim_to_var = ac.simple_type_v1_get_dim_to_var();
      index_info_vec.push_back(dim_to_var);
    }

    if (!valid) {
      continue;
    }
    auto dim_to_var0 = index_info_vec[0];
    set<void *> outer_loop_set(loops.begin(), loops.end());
    map<int, int> dim2outer_loop;
    for (size_t i = 0; i != dim_to_var0.size(); ++i) {
      auto index_info = dim_to_var0[i];
      void *loop = std::get<0>(index_info);
      if (outer_loop_set.count(loop) <= 0) {
        continue;
      }
      for (size_t j = 0; j < loops.size(); ++j) {
        if (loops[j] == loop) {
          dim2outer_loop[i] = j;
          break;
        }
      }
    }

    for (size_t i = 1; i < index_info_vec.size(); ++i) {
      auto dim_to_var1 = index_info_vec[i];
      assert(dim_to_var0.size() == dim_to_var1.size());
      for (size_t j = 0; j != dim_to_var0.size(); ++j) {
        auto index_info0 = dim_to_var0[j];
        auto index_info1 = dim_to_var1[j];
        if (index_info0 == index_info1) {
          //  same index
          continue;
        }
        //  different index
        void *loop0 = std::get<0>(index_info0);
        void *loop1 = std::get<0>(index_info1);
        if (outer_loop_set.count(loop0) <= 0 &&
            outer_loop_set.count(loop1) <= 0) {
          //  false different index because it is related to
          //  paralleled inner loops
          continue;
        }
        valid = false;
        break;
      }
      if (!valid) {
        break;
      }
    }
    if (!valid) {
      continue;
    }
    void *var_type = mCodegen->GetTypebyVar(var);
    vector<size_t> dims;
    void *base_type = nullptr;
    mCodegen->get_type_dimension(var_type, &base_type, &dims, var);

    set<int> parallel_dims;
    int curr_dim = 0;
    for (auto index_info : dim_to_var0) {
      void *loop = std::get<0>(index_info);
      if ((loop != nullptr) && outer_loop_set.count(loop) <= 0) {
        parallel_dims.insert(curr_dim);
      }
      curr_dim++;
    }
    //  if non dimension is related to paralleled loops or
    //  all dimensions related to paralleled loops are located in the least
    //  significant dimensions, do nothing;
    //  otherwise, reorder all dimensions related to paralleled loops to least
    //  significant dimensions to be friendly for memory partitioning
    if (parallel_dims.empty() ||
        (*parallel_dims.begin()) + parallel_dims.size() == dims.size()) {
      continue;
    }

    vector<size_t> new_dims;
    for (size_t dim = dims.size(); dim > 0; --dim) {
      if (parallel_dims.count(dims.size() - dim) <= 0) {
        new_dims.push_back(dims[dim - 1]);
      }
    }
    for (size_t dim = dims.size(); dim > 0; --dim) {
      if (parallel_dims.count(dims.size() - dim) > 0) {
        new_dims.push_back(dims[dim - 1]);
      }
    }

    void *new_var_type = base_type;
    if (!new_dims.empty())
      new_var_type = mCodegen->CreateArrayType(base_type, new_dims);
    string new_var_name = mCodegen->GetVariableName(var) + "_ro";
    mCodegen->get_valid_local_name(func_decl, &new_var_name);
    void *orig_var_decl = mCodegen->GetVariableDecl(var);
    void *scope = mCodegen->TraceUpToBasicBlock(orig_var_decl);
    void *new_var_decl = mCodegen->CreateVariableDecl(
        new_var_type, new_var_name, nullptr, scope);
    mCodegen->InsertStmt(new_var_decl, orig_var_decl);
    old_var2new_var[var] = mCodegen->GetVariableInitializedName(new_var_decl);
    //  Replace all the original reference
    vector<void *> vec_refs(refs.begin(), refs.end());
    for (auto &ref : vec_refs) {
      CMarsAccess ac(ref, mCodegen, loop_body);
      void *new_ref = mCodegen->CreateVariableRef(new_var_decl);
      for (size_t dim = 0; dim < dims.size(); ++dim) {
        if (parallel_dims.count(dim) <= 0) {
          new_ref = mCodegen->CreateExp(V_SgPntrArrRefExp, new_ref,
                                        ac.GetIndex(dim).get_expr_from_coeff());
        }
      }
      for (size_t dim = 0; dim < dims.size(); ++dim) {
        if (parallel_dims.count(dim) > 0) {
          new_ref = mCodegen->CreateExp(V_SgPntrArrRefExp, new_ref,
                                        ac.GetIndex(dim).get_expr_from_coeff());
        }
      }
      mCodegen->ReplaceExp(ref, new_ref);
    }
    mCodegen->RemoveStmt(orig_var_decl);
    Changed = true;
  }
  return Changed;
}
