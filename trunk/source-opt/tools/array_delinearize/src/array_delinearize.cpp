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


#include <fstream>
#include <iostream>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "loop_flatten.h"
#include "mars_opt.h"

#include "tldm_annotate.h"

#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "program_analysis.h"

#include "array_delinearize.h"
#include "mars_message.h"

//  for a test temporarily
#define TEST_FAKE_ALIAS_RECORD 0

#if 1  //  control delinearize log
#define DEL_ALGO(x) cout << "[DEL ALGO][" << __func__ << "]" << (x) << endl;
#else
#define DEL_ALGO(x)
#endif

#if 1
#define DEL_DALGO(x) cout << "[DEL ALGO][" << __func__ << "]" << (x) << endl;
#else
#define DEL_DALGO(x)
#endif

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using std::cout;
using std::endl;
using std::map;
using std::set;
using std::string;
using std::vector;

template <typename T> void print(vector<T> vec, string leadingStr) {
  if (vec.empty()) {
    DEL_ALGO(leadingStr + "empty vector");
    return;
  }

  cout << leadingStr << vec[0];
  for (size_t i = 1; i < vec.size(); ++i) {
    cout << ", " << vec[i];
  }
  cout << endl;
}

#define div_floor(x, y) ((int64_t)floor((x)*1.0 / (y)))
#define rem_floor(x, y) ((int64_t)((x) - (y)*div_floor((x), (y))))
#define max_divide(x, y) ((int64_t)(div_floor((x), (y)) * (y)))

#if 0
#define my_assert_or_return(retval) assert(0);
#else
#define my_assert_or_return(retval) return retval;
#endif

int analysis_delinearize(CSageCodeGen *ast, void *array_init, int dim,
                         vector<size_t> *dim_split_steps,
                         map<void *, size_t> *mapAlias2BStep,
                         bool altera_flow) {
  CMarsExpression offset(ast, 0);
  bool is_simple = true;
  return analysis_delinearize(ast, array_init, dim, dim_split_steps,
                              mapAlias2BStep, altera_flow, &is_simple, &offset,
                              nullptr);
}

//  Note ZP: 20170102
//  1. support multi-dim array
//
//  Output: dim_split_steps: the coefficients to split the index into dimension
//         e.g. dim_split_steps (1, 8, 256) -> array dim [org_size/256][32][8]
int analysis_delinearize(CSageCodeGen *ast, void *array_init, int dim,
                         vector<size_t> *dim_split_steps,
                         map<void *, size_t> *mapAlias2BStep, bool altera_flow,
                         bool *is_simple, void *offset, void *scope) {
  DEFINE_START_END;
  STEMP(start);

  int dim_idx_org = dim;
  {
    DEL_ALGO("****  DELINEARIZE " +
             printNodeInfo(ast, static_cast<SgNode *>(array_init)) + " in " +
             ast->GetFuncName(ast->TraceUpToFuncDecl(array_init)) + "() **** ");
  }

  size_t i;
  size_t j;
  void *sg_scope = scope;
  if (sg_scope == nullptr) {
    sg_scope = ast->GetProject();
  }

  int type_dim;  //  = ast->get_pntr_type_dim(sg_pntr);
  int org_dim_size = 0;
  {
    if (ast->IsInitName(array_init) == 0) {
      DEL_ALGO("[quit delinearize] array_init is not a variable definition, "
               "should not happen.");
      return 0;
    }

    void *tt_type = ast->GetTypebyVar(array_init);
    void *tt_base_type;
    vector<size_t> tt_size;
    ast->get_type_dimension(tt_type, &tt_base_type, &tt_size, array_init);
    type_dim = tt_size.size();

    if (type_dim < 1) {
      DEL_ALGO("[quit delinearize] ignore scalar, should not be here");
      return 0;  //  not touch scalar
    }

    org_dim_size = tt_size[type_dim - 1 - dim_idx_org];

    //  ZP: 20170309, I comment it out for unification
    //  if (!altera_flow && tt_size[0] < 64)
    //  return 0; //  DO NOT convert for small arrays

    DEL_ALGO("type_dim: " + my_itoa(type_dim));
    for (auto ele : tt_size) {
      DEL_ALGO("type dim size on i-th dim: " + my_itoa(ele));
    }
  }

  //  FIXME: ZP: 20170330: limitation of initialization list,
  //     to be removed, but to achieve that, the init_list need to be
  //     reorganized
  {
    if (ast->GetInitializer(array_init) != nullptr) {
      DEL_ALGO("[quit delinearize] do not support initialization yet.");
      return 0;
    }
  }

  //  ///////////////////////////////////////////////////  /
  //  Ovarall Algorithm
  //  1. Get all the references with their sections (var range + steps)
  //  2. For each reference, merge the sections according to the split
  //  requirements
  //    a. divisible: the step can divide all the larger steps
  //    b. range cover: the step value is larger than the total low dim ranges
  //  3. Merge the sections across references
  //    a. All the steps in all the refs are considered as candidate
  //    b. check and select the step candidates that satisifies the split
  //       requirements above of all the accesses
  //  ///////////////////////////////////////////////////  /

  vector<void *> vec_access;
  //  1. Get all the references, and iterator range and steps
  {
    //  not feasible if non-single-assigned alias
    int allow_artifical_ref = 1;
    bool feasible =
        ast->GetPntrReferenceOfArray(array_init, sg_scope, &vec_access,
                                     allow_artifical_ref) != 0;
    if (!feasible) {
      DEL_ALGO("[quit delinearize] get array acces references failed.");
      return 0;
    }
  }

  //  ZP: 20180310: Fixing of bug 2247: if has memcpy reference, all split
  //  factors needs to be able to divide the original dim size
  void *memcpy_ref = nullptr;
  int is_leftmost_dim = static_cast<int>(0 == dim_idx_org);

  //  ZP: 20170314: fixing for the single constant offset
  //  it does not hurt to have more candidate
  vector<int64_t> additional_step_candidate;

  //  2. For each reference, merge the sections according to the split
  //  requirements
  typedef vector<vector<int64_t>>
      t_indepent_sections;  //  vector<(step, lb, ub)>
  vector<t_indepent_sections> sec_new_all_access;
  vector<void *> sec_new_sg_pntr;  //  just for recoding the pntr for printing
  {
    set<void *> set_alias;
    {
      for (auto sg_pntr : vec_access) {
        vector<void *> varRefs;
        ast->GetNodesByType(sg_pntr, "preorder", "SgVarRef", &varRefs, true);
        for (auto var : varRefs) {
          if ((ast->is_floating_node(var) == 0) &&
              ((static_cast<SgNode *>(ast->GetParent(var)))->class_name() ==
                   "SgPlusPlusOp" ||
               (static_cast<SgNode *>(ast->GetParent(var)))->class_name() ==
                   "SgMinusMinusOp")) {
            DEL_ALGO("[quit delinearize] " + ast->_p(sg_pntr) +
                     " has ++/-- in pointer arith");
            return 0;
          }
        }

        void *sg_alias =
            ast->get_alias_array_from_assignment_expression(sg_pntr);
        if (sg_alias != nullptr) {
          if ((set_alias.count(sg_alias) != 0U) &&
              (ast->IsArgumentInitName(sg_alias) == 0)) {
#if PROJDEBUG
            //  cout << "[Delinearization] analysis_delinear : multiple
            //  assignment "
            //         "alias: \n  "
            //      << ast->_p(sg_alias) << endl;
            DEL_ALGO("[quit delinearize] do not support multiple assignment "
                     "alias: \n" +
                     printNodeInfo(ast, static_cast<SgNode *>(sg_alias)));
#endif
            my_assert_or_return(0);
          }

          set_alias.insert(sg_alias);
        }

        //  ZP: 20170316 exclude system calls except memcpy
        {
          //  void *sg_array_ref;
          int arg_idx = ast->GetFuncCallParamIndex(sg_pntr);
          if (-1 != arg_idx) {
            void *func_call = ast->TraceUpToFuncCall(sg_pntr);
            assert(func_call);
            void *func_decl = ast->GetFuncDeclByCall(func_call);

            if (ast->IsMemCpy(func_call)) {
              memcpy_ref = func_call;
            } else if (ast->GetFuncNameByCall(func_call).find(
                           "memcpy_wide_bus_read") == 0 ||
                       ast->GetFuncNameByCall(func_call).find(
                           "memcpy_wide_bus_write") == 0) {
              memcpy_ref = func_call;
            } else if (ast->GetFuncNameByCall(func_call).find("__merlin_") ==
                       0) {
            } else if (ast->IsFunctionDeclaration(func_decl) != 0) {
            } else {
              void *func_decl_no_body = ast->GetFuncDeclByCall(func_call, 0);
              void *param = ast->GetFuncParam(func_decl_no_body, arg_idx);
              if (ast->IsScalarType(param) == 0) {
                DEL_ALGO("[quit delinearize] do not supprt system calls except "
                         "memcpy functions");
                return 0;
              }
            }
          }
        }
      }
    }

    //  printf(" ---- org_dim_size = %d, dim_idx=%d, is_leftmost=%d
    //  has_memcpy=%d\n", org_dim_size, dim_idx_org, is_leftmost_dim,
    //  has_memcpy_ref);

    map<void *, vector<CMarsRangeExpr>> m_r_range;
    map<void *, vector<CMarsRangeExpr>> m_w_range;
    for (size_t i0 = 0; i0 < vec_access.size(); i0++) {  //  for each access
      void *sg_pntr = vec_access[i0];
      DEL_DALGO("iterate on access: " +
                printNodeInfo(ast, static_cast<SgNode *>(sg_pntr)));

      if (ast->IsCastExp(sg_pntr) != 0) {
        ast->remove_cast(&sg_pntr);
      }

      int type_dim;
      int pntr_dim;
      void *tt_array;
      {
        vector<void *> tt_vec;

        //  get pntr_dim
        void *array_ref;
        ast->parse_array_ref_from_pntr(sg_pntr, &array_ref, ast->GetProject());
        void *tt_pntr;
        ast->parse_pntr_ref_by_array_ref(array_ref, &tt_array, &tt_pntr,
                                         &tt_vec, &pntr_dim, ast->GetProject());

        //  get type_dim
        void *tt_type = ast->GetTypebyVar(tt_array);
        void *tt_base_type;
        vector<size_t> tt_size;
        ast->get_type_dimension(tt_type, &tt_base_type, &tt_size, sg_pntr);
        type_dim = tt_size.size();

        DEL_DALGO("pntr_dim: " + my_itoa(pntr_dim));
        DEL_DALGO("type_dim: " + my_itoa(type_dim));
      }

      if (pntr_dim <= -1) {
        DEL_ALGO("[quit delinearize] does not handle address of a pointer, "
                 "skip the whole array");
        return 0;
      }

      //  if (type_dim != pntr_dim) {
      //    if (pntr_dim == 0) //  pure VarRef of the array
      //        continue; //  skip this reference
      //  }

      //  CMarsAccess ac(sg_pntr, ast, nullptr, ast->GetProject());
      void *sg_pos =
          (ast->is_floating_node(sg_pntr)) != 0 ? ast->GetProject() : sg_pntr;
      CMarsAccess ac(sg_pntr, ast, nullptr, sg_pos);

      int dim_in_access = dim;
      {
        void *sg_alias = tt_array;
        if (set_alias.count(sg_alias) != 0U) {
          int alias_dim = ast->get_dim_num_from_var(sg_alias);
          int array_dim = ast->get_dim_num_from_var(array_init);

          dim_in_access = dim - (array_dim - alias_dim);
        }
      }

      //  int idx_dim_size = ac.GetIndexes().size();
      if (ac.GetIndexes().size() > static_cast<size_t>(dim_in_access)) {
        //  E.g. 35*i + 4*j + k, and i=0..3, j=0..7, k=0..5

        CMarsExpression me = ac.GetIndexes()[dim_in_access];

        //  ZP: 20170314: fixing for the single constant offset
        //  it does not hurt to have more candidate
        if ((me.IsConstant() != 0) && me.get_const() != 0) {
          int64_t curr_step = std::abs(me.get_const());
          additional_step_candidate.push_back(curr_step);
        }

        //  ZP: 20170314
        if ((me == 0) != 0) {
          continue;  //  this access can be ignored, because it does not impact
        }
        //  other accesses (good or bad)

        //  For memory burst with variable starting address
        //  For the other circumstances, offset can be a constant 0
        me = me - *static_cast<CMarsExpression *>(offset);

        CMarsRangeExpr mr_tmp(me.get_range());

        vector<CMarsExpression> vec_indexes;
        vec_indexes.push_back(me);

        //  ZP: 20170317 not sure if is_simple is used
        *is_simple &= CMarsAccess::is_simple_type_v1(vec_indexes);

        if (me.has_coeff_range_simple() == 0) {
          int array_dim = ast->get_dim_num_from_var(array_init);

          int alias_idx = dim_idx_org -
                          (array_dim - ast->get_dim_num_from_var(
                                           ast->get_array_from_pntr(sg_pntr)));

          if (alias_idx < 0) {
            continue;  //  the dim to split is at the higher level, and the
          }
          //  access of the alias will not impact on that dim
          void *func_decl = ast->TraceUpToFuncDecl(sg_pntr);
          if (m_r_range.count(func_decl) <= 0) {
            //  ZP: 20170317 Support of range intrisic
            //  If the simple bound can not find, try range intrinsic
            ArrayRangeAnalysis mar(ast->get_array_from_pntr(sg_pntr), ast,
                                   func_decl, ast->GetProject(), false, false);
            m_r_range[func_decl] = mar.GetRangeExprRead();
            m_w_range[func_decl] = mar.GetRangeExprWrite();
          }
          assert(alias_idx < m_w_range[func_decl].size());
          assert(alias_idx < m_r_range[func_decl].size());
          CMarsRangeExpr mr_w;
          if (alias_idx < m_w_range[func_decl].size()) {
            mr_w = m_w_range[func_decl][alias_idx];
          }
          CMarsRangeExpr mr_r;
          if (alias_idx < m_r_range[func_decl].size()) {
            mr_r = m_r_range[func_decl][alias_idx];
          }

          if (((mr_w.is_empty() == 0) && (mr_w.is_const_bound() == 0)) ||
              ((mr_r.is_empty() == 0) && (mr_r.is_const_bound() == 0))) {
            sec_new_all_access
                .clear();  //  disable the delinearization of the whole array
            sec_new_sg_pntr.clear();

            cout << "Delinearization disabled, due to unknown access bounds of "
                    "Pntr \""
                 << printNodeInfo(ast, static_cast<SgNode *>(sg_pntr)) << "\" "
                 << endl;
            return 0;
          }

          int64_t lb_w = mr_w.is_empty() != 0 ? 0 : mr_w.get_const_lb();
          int64_t ub_w = mr_w.is_empty() != 0 ? -1 : mr_w.get_const_ub();
          int64_t lb_r = mr_r.is_empty() != 0 ? 0 : mr_r.get_const_lb();
          int64_t ub_r = mr_r.is_empty() != 0 ? -1 : mr_r.get_const_ub();

          int64_t n_ub = max(ub_w, ub_r);
          int64_t n_lb = max(lb_w, lb_r);

          vector<vector<int64_t>>
              sec_new_one_access_one_access;  //  [dim][step,lb,ub]
          vector<int64_t> new_term;
          new_term.push_back(1);
          new_term.push_back(n_lb);
          new_term.push_back(n_ub);
          sec_new_one_access_one_access.push_back(new_term);
          sec_new_all_access.push_back(sec_new_one_access_one_access);
          sec_new_sg_pntr.push_back(sg_pntr);
          continue;
        }

        //  Get the initial section from expression analysis
        //  sec_org_one_access: (1,0,0),(1,0..5),(4,0..7),(35,0..3)
        t_indepent_sections sec_org_one_access = me.get_coeff_range_simple();

        //  separate_pos: determine the split positions of the sections
        map<int, int> separate_pos;  //  only used for old method

        //  sec_org_one_access: (1,0..33),(35,0..3)
        vector<vector<int64_t>>
            sec_new_one_access_one_access;  //  [dim][step,lb,ub]
        sec_new_one_access_one_access.clear();
        int last_step = 1;
        CMarsExpression me_curr_dim(ast, 0);
        for (i = 0; i < sec_org_one_access.size(); i++) {
          vector<int64_t> one_term = sec_org_one_access[i];
          int64_t one_step = std::abs(one_term[0]);

          CMarsExpression me_var(CMarsVariable(
              ast, CMarsRangeExpr(one_term[1], one_term[2], ast)));

          //          CMarsVariable mv(ast, CMarsRangeExpr(one_term[1],
          //          one_term[2], ast)); me_var.get_range().get_const_lb();
          //          CMarsRangeExpr mr_var(me_var.get_range());

          //  /////////////////////////////////////////////////////////  /
          //  Split Condition: check if the two section can be merged
          //  1. next step >= partial total range (old)
          //  -> LB and uB of partial total range is in the same
          //     interval of next step (new)
          //  2. the dim size can divide all the large coeffs
          //  /////////////////////////////////////////////////////////  /
          int is_divisible = 1;
          {
            for (j = i + 1; j < sec_org_one_access.size(); j++) {
              int64_t one_step_1 = std::abs(sec_org_one_access[j][0]);
              if (one_step_1 % one_step != 0) {
                is_divisible = 0;
                break;
              }
            }
          }

          CMarsRangeExpr mr(me_curr_dim.get_range());
          bool is_const_bound = mr.is_const_bound() != 0;
          int64_t n_lb = mr.get_const_lb();
          int64_t n_ub = mr.get_const_ub();

          //  To support constant offset shifting between sections,
          //    lb and ub of me_curr_dim shoule be in the
          //    same interval of length one_step

          if (one_step > 1 && is_const_bound && (is_divisible != 0) &&
              div_floor(n_lb, one_step) == div_floor(n_ub, one_step)) {
            separate_pos[i] = 1;
            vector<int64_t> new_term;
            new_term.push_back(last_step);
            new_term.push_back(rem_floor(n_lb, one_step) / last_step);
            new_term.push_back(rem_floor(n_ub, one_step) / last_step);
            sec_new_one_access_one_access.push_back(new_term);

            me_curr_dim = CMarsExpression(ast, max_divide(n_lb, one_step));
            last_step = one_step;
          } else {
            separate_pos[i] = 0;
          }

          //  sum up the range of existing variables
          me_curr_dim = me_curr_dim + me_var * one_term[0];
        }

        //  handling the tail iteration
        {
          CMarsRangeExpr mr(me_curr_dim.get_range());
          bool is_const_bound = mr.is_const_bound() != 0;
          assert(is_const_bound);
          int64_t n_lb = mr.get_const_lb();
          int64_t n_ub = mr.get_const_ub();

          vector<int64_t> new_term;
          new_term.push_back(last_step);
          new_term.push_back(n_lb / last_step);
          new_term.push_back(n_ub / last_step);
          sec_new_one_access_one_access.push_back(new_term);
          sec_new_all_access.push_back(sec_new_one_access_one_access);
          sec_new_sg_pntr.push_back(sg_pntr);
        }
      }
    }
  }

  {
    int i = 0;
    DEL_ALGO("access sections");
    for (auto one_access : sec_new_all_access) {
      string access_info =
          printNodeInfo(ast, static_cast<SgNode *>(sec_new_sg_pntr[i++]));
      for (auto term : one_access) {
        DEL_ALGO(access_info + ": " + my_itoa(term[0]) + "," +
                 my_itoa(term[1]) + "," + my_itoa(term[2]));
      }
    }
  }

  vector<size_t> merged_step_all_access;

  //  //////////////////////////////////////////////////  /
  //  Algorithm (merge access):
  //  1. Get all the candidates from all possible step from the sections
  //  2. Test the candidate one-by-one and select the ones that can split all
  //  the accesses
  //  3. Exception: Xilinx small dim on lowest level (not split)
  //  //////////////////////////////////////////////////  /
  {
    set<int64_t> step_candidates;
    for (size_t i = 0; i < sec_new_all_access.size(); i++) {
      vector<vector<int64_t>> curr_sec = sec_new_all_access[i];

      for (size_t dim = 0; dim < curr_sec.size(); dim++) {
        int64_t curr_step = curr_sec[dim][0];
        step_candidates.insert(curr_step);
      }
    }

    //  ZP: 20170314: fixing for the single constant offset
    //  it does not hurt to have more candidate
    for (auto step : additional_step_candidate) {
      step_candidates.insert(step);
    }

    //  ZP: 20170630: filter the small steps that can not be divided by larger
    //  ones
    {
      vector<int64_t> v_cand;
      for (auto step : step_candidates) {
        v_cand.push_back(step);
      }

      std::sort(v_cand.begin(), v_cand.end());

      set<int64_t> new_step_candidates;
      vector<int64_t> debug_step_candidates;
      for (size_t i = 0; i < v_cand.size(); ++i) {
        bool isKeep = true;
        for (size_t j = i + 1; j < v_cand.size(); ++j) {
          if (v_cand[j] % v_cand[i] != 0) {
            isKeep = false;
            break;
          }
        }
        if (isKeep) {
          new_step_candidates.insert(v_cand[i]);
          debug_step_candidates.push_back(v_cand[i]);
        }
      }
      //  int curr_factor = 1;
      //  for (auto step : v_cand) {
      //   if (step % curr_factor == 0) { //  is it reasonable?
      //     new_step_candidates.insert(step);
      //     curr_factor = step;
      //   }
      //  }
      step_candidates = new_step_candidates;
      print(debug_step_candidates,
            "[DEL ALGO] steps after filter non-divisible ones: ");
    }

    for (auto step : step_candidates) {
      int is_step_compatible_with_sec = 1;

      for (size_t i = 0; i < sec_new_all_access.size(); i++) {
        vector<vector<int64_t>> curr_sec = sec_new_all_access[i];

        //  input: int step;
        //  input: curr_sec
        //  input: CMarsAST_IF * ast //  used for CMarsExpression
        //  output: bool is_step_compatible_with_sec
        {
          //  definition of compatible
          //  1. the 'step' can divide all the coefficient that larger
          //  2. the range of the remain is in an interval of 'step'
          //  3. ZP 20180310 Fixing of bug 2247: if has memcpy reference,
          //    all split factors needs to be able to divide the original dim
          //    size

          //  1. the 'step' can divide all the coefficient that larger
          CMarsExpression me_remain(ast, 0);
          for (size_t dim = 0; dim < curr_sec.size(); dim++) {
            vector<int64_t> one_term = curr_sec[dim];
            int64_t curr_step = one_term[0];
            if (curr_step >= step) {
              if (curr_step % step != 0) {
                is_step_compatible_with_sec = 0;
              }
            } else {
              CMarsExpression me_var(CMarsVariable(
                  ast, CMarsRangeExpr(one_term[1], one_term[2], ast)));
              me_remain = me_remain + me_var * curr_step;
            }
          }

          //  2. the range of the remain is in an interval of 'step'

          CMarsRangeExpr mr(me_remain.get_range());
          bool is_const_bound = mr.is_const_bound() != 0;
          int64_t n_lb = mr.get_const_lb();
          int64_t n_ub = mr.get_const_ub();
          if (!is_const_bound ||
              div_floor(n_lb, step) != div_floor(n_ub, step)) {
            is_step_compatible_with_sec = 0;
          }
        }
        //  output is is_step_compatible_with_sec
        //  ///////////////////////////////////  /

        if (is_step_compatible_with_sec == 0) {
          break;
        }
      }  //  all the accesses

      //  ZP 20180310 Fixing of bug 2247: if has memcpy reference,
      //    all split factors needs to be able to divide the original dim size
      if ((memcpy_ref != nullptr) && (is_leftmost_dim == 0)) {
        is_step_compatible_with_sec &=
            static_cast<int>((org_dim_size % step) == 0);
        if (is_step_compatible_with_sec == 0) {
          string var_info = ast->GetVariableName(array_init);
          auto user_info = getUserCodeInfo(ast, array_init);
          if (!user_info.name.empty())
            var_info = user_info.name;
          string warning_info = '\'' + var_info + "\' " +
                                getUserCodeFileLocation(ast, memcpy_ref, true);
          dump_critical_message(PROCS_WARNING_40(warning_info));
        }
      }

      if (is_step_compatible_with_sec != 0) {
        merged_step_all_access.push_back(step);
      }
    }
  }

  {
    DEL_ALGO("  ---- after access merge: ");
    print(merged_step_all_access, "[DEL ALGO] split step: ");
  }

  analysis_delinearize_postproc_alias_offset(
      ast, array_init, dim, sg_scope, &merged_step_all_access, mapAlias2BStep);

  {
    DEL_ALGO("  ---- after alias update: ");
    print(merged_step_all_access, "[DEL ALGO] split step: ");

    if (mapAlias2BStep->empty()) {
      DEL_ALGO("alias: empty");
    } else {
      for (auto it : *mapAlias2BStep) {
        DEL_ALGO("alias: " + ast->UnparseToString(it.first) + "->" +
                 my_itoa(it.second));
      }
    }
  }

  *dim_split_steps = merged_step_all_access;
  TIMEP("  analysis_delinearize", start, end);

  return 1;
}

int apply_delinear_index_transform(CSageCodeGen *ast, CMarsExpression *me,
                                   vector<size_t> modified_old_steps,
                                   vector<CMarsExpression> *vec_new_indexes,
                                   bool has_address_of) {
  for (size_t dim = 0; dim < modified_old_steps.size(); dim++) {
    int steps_num = modified_old_steps.size();
    int factor =
        modified_old_steps[steps_num - 1 - dim];  //  start from large step
    CMarsExpression remainder(ast, 0);

    vector<void *>
        matched_vars;  //  get the vars whose coeff is divisible by the step
    {
      vector<void *> vars;
      me->get_vars(&vars);
      for (auto var : vars) {
        //  1. Either coeff divisible
        CMarsExpression coeff = me->get_coeff(var);
        if (coeff.GetConstant() % factor == 0) {
          matched_vars.push_back(var);
        }
      }
    }

    CMarsRangeExpr mr_reminder = remainder.get_range();

    //  1. me_new_term: Build sigma(var*coeff) for matched_vars
    //  And remove the term from me
    CMarsExpression me_new_term(me->get_ast(), 0);
    for (auto var : matched_vars) {
      int64_t n_coeff = me->get_coeff(var).GetConstant() / factor;

      me_new_term = me_new_term + CMarsExpression(var, ast, me->get_pos()) *
                                      CMarsExpression(ast, n_coeff);

      me->set_coeff(var, CMarsExpression(ast, 0));
    }

    //  2. merge constant to the term
    //  if (factor == 1) {
    //  me_new_term = me_new_term + CMarsExpression(me->get_const(), ast);
    //  me->set_const(0);
    //  } else {
    //  me_new_term = me_new_term + CMarsExpression(me->get_const() /
    //  factor, ast); me->set_const(me->get_const() % factor);
    //  }

    //  3. shifting the index to make the remains in the range of dimension
    //  while (!me->IsNonNegative()) {
    //  *me = *me + factor;
    //  me_new_term = me_new_term - 1;
    //  }

    //  ////////////////////////////////////  /
    //  ZP: 20170311: more general solution for 2 and 3
    //  offset shifting between dimensions is handled
    {
      CMarsRangeExpr mr(me->get_range());
      bool is_const_bound = mr.is_const_bound() != 0;
      if (!is_const_bound)
        return (0);
      int64_t n_lb = mr.get_const_lb();  //  lb and ub are in the same
      //  interval of factor
      int64_t base = div_floor(n_lb, factor);

      *me = *me - (base * factor);  //  the range is in [0..factor-1]
      me_new_term = me_new_term + base;
#if 1  //  for validation only
      {
        CMarsRangeExpr mr(me->get_range());
        bool is_const_bound = mr.is_const_bound() != 0;
        if (!is_const_bound)
          return (0);
        int64_t n_lb = mr.get_const_lb();
        int64_t n_ub = mr.get_const_ub();
        if (!(0 <= n_lb && n_lb < factor && 0 <= n_ub && n_ub < factor))
          return (0);
      }
#endif
    }
    //  ZP: 20170312: handle top level pointer (and offset) in a unified
    //  way the last dim of the created one need to be deleted if
    //  originally has address_of
    if (has_address_of && dim == modified_old_steps.size() - 1) {
      //  assert(me_new_term == 0);
      //  skip the last dim
      //
      //  if (me_new_term == 0) has_address_of = 0;
      //  else
      { vec_new_indexes->push_back(me_new_term); }
    } else {
      vec_new_indexes->push_back(me_new_term);
    }
  }
  return 1;
}

void analysis_delinearize_postproc_alias_offset(
    CSageCodeGen *ast, void *array_init, int dim_idx, void *sg_scope,
    vector<size_t> *merged_step_all_access,  //  input/output
    map<void *, size_t> *mapAlias2BStep) {   //  output
  //  Overall Algorithm:
  //  1. Get all the <alias, exp> pairs
  //  2. For each pair, determine the boundary step if feasible

  //  1. Get all the <alias, exp> pairs
  vector<void *> vec_access;
  int allow_artifical_ref = 1;
  int valid = ast->GetPntrReferenceOfArray(array_init, sg_scope, &vec_access,
                                           allow_artifical_ref);
  assert(valid);
  int array_dim = ast->get_dim_num_from_var(array_init);
  map<void *, CMarsExpression> map_alias_to_exp;
  {
    for (auto sg_pntr : vec_access) {
      void *sg_alias = ast->get_alias_array_from_assignment_expression(sg_pntr);
      if (sg_alias != nullptr) {
        CMarsAccess ac(sg_pntr, ast, nullptr);
        int pntr_array_dim =
            ast->get_dim_num_from_var(ast->get_array_from_pntr(sg_pntr));
        //                int array_dim = ast->get_dim_num_from_var(array_init);
        int dim_idx_in_pntr = dim_idx - (array_dim - pntr_array_dim);
        if (dim_idx_in_pntr < 0) {
          continue;
        }

        if (map_alias_to_exp.end() != map_alias_to_exp.find(sg_alias)) {
          //  ZP: 20170318: fixing for multi-call with intrinsic
          {
            if ((map_alias_to_exp[sg_alias] == ac.GetIndex(dim_idx_in_pntr)) !=
                0) {
              continue;
            }
            //  if all the exps are from the same array, ignore the new pntr;
            //  void * exp0 = map_alias_to_exp[sg_alias].get_expr_from_coeff();
            //  void * exp1 =
            //  ac.GetIndex(dim_idx_in_pntr).get_expr_from_coeff();

            //  if (ast->IsVarRefExp(exp0) && ast->IsVarRefExp(exp1))
            //  {
            //    if (ast->GetVariableInitializedName(exp0) ==
            //    ast->GetVariableInitializedName(exp1))
            //        continue;
            //  }
          }
#if PROJDEBUG
          cout << "[Delinearization] analysis_postproc: multiple assignment "
                  "alias with different offset: \n  "
               << ast->_p(sg_alias) << endl;
#endif

          //  ZP: 20170324: there is actually a situation that assert fails: due
          //  to the ZP:20170318 fixing
          //    there is multi-calls pntr, but its index range are constrained
          //    by the range intrinsic In this case, we skip the delinearization
          //    for the dimension, instead of assert.
          merged_step_all_access
              ->clear();  //  disable the delinearization of the current dim
          mapAlias2BStep->clear();
          return;

          //  my_assert_or_return();
        }

        if (dim_idx_in_pntr >=
            0) {  //  ignore the alias if the dim_to_split is not in its scope
          map_alias_to_exp.insert(pair<void *, CMarsExpression>(
              sg_alias, ac.GetIndex(dim_idx_in_pntr)));
        }
      }
    }
  }

  //  2. For each pair, determine the boundary step if feasible
  //    a. The step can divide all the coeff of the offset expression
  //    b. The accesses of the alias is within [0, the step)
  //    Note: find the smallest step that satisfies a) and b), and put in
  //    mapAlias2BStep
  {
    for (auto it : map_alias_to_exp) {
      void *sg_alias = it.first;
      CMarsExpression me_offset = it.second;

      //  2.1 calculate the range of alias accesses
      int64_t alias_access_ub = 0;
      {
        int alias_dim = ast->get_dim_num_from_var(sg_alias);
        ArrayRangeAnalysis mar(sg_alias, ast, ast->TraceUpToFuncDecl(sg_alias),
                               ast->GetProject(), false, false);

        int alias_idx = dim_idx - (array_dim - alias_dim);
        if (alias_idx < 0) {
          continue;  //  the dim to split is at the higher level, and the access
        }
        //  of the alias will not impact on that dim
        CMarsRangeExpr mr_w = mar.GetRangeExprWrite()[alias_idx];
        CMarsRangeExpr mr_r = mar.GetRangeExprRead()[alias_idx];

        if (((mr_w.is_empty() == 0) && (mr_w.is_const_bound() == 0)) ||
            ((mr_r.is_empty() == 0) && (mr_r.is_const_bound() == 0))) {
          merged_step_all_access
              ->clear();  //  disable the delinearization of the current dim
          mapAlias2BStep->clear();
          return;
        }

        int64_t lb_w = mr_w.is_empty() != 0 ? 0 : mr_w.get_const_lb();
        int64_t ub_w = mr_w.is_empty() != 0 ? -1 : mr_w.get_const_ub();
        int64_t lb_r = mr_r.is_empty() != 0 ? 0 : mr_r.get_const_lb();
        int64_t ub_r = mr_r.is_empty() != 0 ? -1 : mr_r.get_const_ub();

        if (lb_w < 0 || lb_r < 0) {
          merged_step_all_access
              ->clear();  //  disable the delinearization of the current dim
          mapAlias2BStep->clear();
          return;
        }
        alias_access_ub = max(ub_w, ub_r);
      }

      int selected_step = -1;
      for (auto curr_step : *merged_step_all_access) {
        //  from small to large
        if (static_cast<int64_t>(curr_step) > alias_access_ub &&
            (divisible(me_offset, curr_step, false) != 0)) {
          selected_step = curr_step;
          break;
        }
      }
      if (selected_step == -1 && ((me_offset != 0) != 0)) {
        merged_step_all_access
            ->clear();  //  disable the delinearization of the current dim
        mapAlias2BStep->clear();
        return;
      }
      if (mapAlias2BStep->end() != mapAlias2BStep->find(sg_alias)) {
        //  multiple assignment to one alias
        merged_step_all_access
            ->clear();  //  disable the delinearization of the current dim
        mapAlias2BStep->clear();
        return;
      }
      mapAlias2BStep->insert(pair<void *, size_t>(sg_alias, selected_step));
    }
  }
}

int apply_delinearize(CSageCodeGen *ast, void *array_init, int dim,
                      const vector<size_t> &dim_split_steps,
                      map<void *, size_t> *mapAlias2BStep) {
  CMarsExpression offset(ast, 0);
  bool is_simple = true;
  map<void *, vector<CMarsExpression>> mapIndex2Delinear;
  vector<size_t> new_array_dims;
  return apply_delinearize(ast, array_init, dim, dim_split_steps,
                           mapAlias2BStep, false, &is_simple, &offset, nullptr,
                           &mapIndex2Delinear, &new_array_dims);
}

int check_only_delinearize(
    CSageCodeGen *ast, void *array_init, int dim,
    const vector<size_t> &dim_split_steps, map<void *, size_t> *mapAlias2BStep,
    map<void *, vector<CMarsExpression>> *mapIndex2Delinear,
    vector<size_t> *new_array_dims) {
  CMarsExpression offset(ast, 0);
  bool is_simple = true;
  return apply_delinearize(ast, array_init, dim, dim_split_steps,
                           mapAlias2BStep, true, &is_simple, &offset, nullptr,
                           mapIndex2Delinear, new_array_dims);
}

int apply_delinearize(CSageCodeGen *ast, void *array_init, int dim_idx,
                      const vector<size_t> &dim_split_steps,
                      map<void *, size_t> *mapAlias2BStep, bool check_only,
                      bool *is_simple, void *offset, void *scope,
                      map<void *, vector<CMarsExpression>> *mapIndex2Delinear,
                      vector<size_t> *new_array_dims) {
  //  ///////////////////////////////////////////////////////////////////////  /
  //  ZP: 20170312: handling offset on partial pntr
  //  Use small steps for alias pntr, and use large steps for alias assignment
  //  expression
  auto get_small_step_list = [](vector<size_t> vec, size_t val) {
    decltype(vec) out;
    for (auto t : vec) {
      if (t < val || (t == 1 && val == 1)) {
        out.push_back(t);
      }
    }
    return out;
  };
  auto get_large_step_list = [](vector<size_t> vec, size_t val) {
    decltype(vec) out;
    out.push_back(1);  //  ZP: 20150312: fixing for the last dim
    for (auto t : vec) {
      if (t >= val && !(t == 1 && val == 1)) {
        out.push_back(t);
      }
    }
    return out;
  };
  //  ///////////////////////////////////////////////////////////////////////  /

  size_t i;
  size_t j;
  *is_simple = true;
  void *sg_scope = scope;
  if (sg_scope == nullptr) {
    sg_scope = ast->GetProject();
  }
  int dim_num = dim_split_steps.size();

  vector<size_t> old_steps = dim_split_steps;
  if (old_steps.size() <= 1) {
    return 0;
  }

  //  ///////////////////////////////////////////////////////////////  /
  //  Ovarall Algorithm
  //  1. Get all the references, and iterator range and steps
  //  2. For each reference, check if all the referneces are transformable
  //  3. do the transform one by one
  //  ///////////////////////////////////////////////////////////////  /

  vector<void *> vec_access;
  //  1. Get all the references, and iterator range and steps
  int allow_artifical_ref = 1;
  int valid = ast->GetPntrReferenceOfArray(array_init, sg_scope, &vec_access,
                                           allow_artifical_ref);
  if (valid == 0) {
    return 0;
  }

  set<void *> set_alias;
  {
    for (auto sg_pntr : vec_access) {
      int is_memcpy;
      void *sg_alias =
          ast->get_alias_array_from_assignment_expression(sg_pntr, &is_memcpy);
      if (sg_alias != nullptr) {
        if (false) {  //  set_alias.count(sg_alias))
#if PROJDEBUG
          cout << "[Delinearization] analysis_delinear : multiple assignment "
                  "alias: \n  "
               << ast->_p(sg_alias) << endl;
#endif
          my_assert_or_return(0);
        }

        set_alias.insert(sg_alias);
      }
    }
  }

  int array_dim = ast->get_dim_num_from_var(array_init);

  //  check feasibility again
  int is_feasible = 1;

  int dim_idx_org = dim_idx;

  //  //  ZP: 20170312: move check_only function to analysis_delinearize()
  //  20170318: memory_burst_cg.cpp calls this function with check_only==1
  //  if (check_only) my_assert_or_return();
  if (check_only) {
    for (i = 0; i < vec_access.size(); i++) {
      void *sg_pntr = vec_access[i];
      assert(!ast->is_floating_node(sg_pntr));

      if (ast->IsCastExp(sg_pntr) != 0) {
        ast->remove_cast(&sg_pntr);
      }

      //  int type_dim = ast->get_pntr_type_dim(sg_pntr);
      //  int pntr_dim = get_pntr_pntr_dim(sg_pntr);
      //  {
      //  void *tt_array;
      //  vector<void *> tt_vec;
      //  ast->parse_pntr_ref_new(sg_pntr, &tt_array, &tt_vec);
      //  pntr_dim = tt_vec.size();

      //  void *tt_type = ast->GetTypebyVar(tt_array);
      //  void *tt_base_type;
      //  vector<size_t> tt_size;
      //  ast->get_type_dimension(tt_type, &tt_base_type, &tt_size, sg_pntr);
      //  type_dim = tt_size.size();
      //  }

      CMarsAccess ac(sg_pntr, ast, nullptr);

      int dim_idx_in_pntr = dim_idx;
      {
        int pntr_dim =
            ast->get_dim_num_from_var(ast->get_array_from_pntr(sg_pntr));
        dim_idx_in_pntr = dim_idx - (array_dim - pntr_dim);
      }

      if (ac.GetIndexes().size() < static_cast<size_t>(dim_idx_in_pntr)) {
        is_feasible = 0;
        break;
      }

      CMarsExpression me = ac.GetIndexes()[dim_idx_in_pntr];

      me = me - *(static_cast<CMarsExpression *>(offset));
      for (size_t dim = 0; dim < old_steps.size(); dim++) {
        int factor = old_steps[dim_num - 1 - dim];

        vector<void *> vars;
        me.get_vars(&vars);
        CMarsExpression remainder(ast, me.GetConstant());
        vector<void *> matched_vars;
        for (j = 0; j < vars.size(); j++) {
          //  1. Either coeff divisible
          CMarsExpression coeff = me.get_coeff(vars[j]);
          if (coeff.IsConstant() == 0) {
            is_feasible = 0;
            break;
          }
          if (coeff.GetConstant() % factor != 0) {
            //  2. or the itotal remainer is less than coeff
            CMarsExpression me_var(vars[j], ast, me.get_pos());
            remainder = remainder + me_var * coeff;
          } else {
            matched_vars.push_back(vars[j]);
          }
        }

        CMarsRangeExpr mr_remainder = remainder.get_range();

        if (mr_remainder.is_const_bound() != 0) {
          int64_t lb = mr_remainder.get_const_lb();
          int64_t ub = mr_remainder.get_const_ub();

          if (lb < 0 || ub > factor) {
            is_feasible = 0;
          }
        }

        if (is_feasible == 0) {
          break;
        }

        for (j = 0; j < matched_vars.size(); j++) {
          me.set_coeff(matched_vars[j], CMarsExpression(ast, 0));
        }
      }

      if (is_feasible == 0) {
        break;
      }
    }
  }

  //  1. get all the array init names to be changed
  map<void *, int> init_to_change;
  {
    for (i = 0; i < vec_access.size(); i++) {
      void *sg_pntr = vec_access[i];

      //  ZP: 20170401
      if (ast->is_floating_node(sg_pntr) != 0) {
        continue;
      }

      void *curr_array;
      vector<void *> curr_idx;
      ast->parse_pntr_ref_new(sg_pntr, &curr_array, &curr_idx);

      if (curr_array != nullptr) {
        init_to_change[curr_array] = 1;
      }
    }
  }
  //    int array_dim = ast->get_dim_num_from_var(array_init);

  //  ZP: 20170322
  //  Apply the type change for all the forward declarations, not only the
  //  definition
  int has_decl_in_header = 0;
  vector<pair<void *, void *>>
      map_alias_to_forward_decl;  //  collect the decls to be updated
  set<void *> set_alias_decl;
  //  apply_delinear_type_change:
  //  change the type of the array
  //  ZP: 20170320 //  record alias type first and apply the change finally
  map<void *, void *> map_update_alias_type;
  {
    //  1. get all the array init names to be changed
    //  output: map<void*, int> init_to_change
    //  moved to front

    //  2. apply the type change
    map<void *, int>::iterator it;
    for (it = init_to_change.begin(); it != init_to_change.end(); it++) {
      void *one_init = it->first;

      int dim_idx_in_alias = dim_idx_org;
      if (one_init != array_init) {
        dim_idx_in_alias -= (array_dim - ast->get_dim_num_from_var(one_init));
      }
      if (dim_idx_in_alias < 0) {
        continue;
      }

      //  ///////////////////////////////////////////////////////////////////////
      //  / ZP: 20170312: handling offset on partial pntr Use small steps for
      //  alias pntr, and use large steps for alias assignment expression
      vector<size_t> modified_old_steps = old_steps;
      //  1. filter out large values if it is an alias
      if (mapAlias2BStep->end() != mapAlias2BStep->find(one_init)) {
        modified_old_steps =
            get_small_step_list(old_steps, (*mapAlias2BStep)[one_init]);
      }
      //  ///////////////////////////////////////////////////////////////////////
      //  /
      vector<size_t> new_array_dims_tmp;
      void *sg_base_type;
      vector<size_t> one_init_dims;
      ast->get_type_dimension_new(ast->GetTypebyVar(one_init), &sg_base_type,
                                  &one_init_dims, one_init);
      size_t curr_total = one_init_dims[dim_idx_in_alias];

      for (int t = 0; t < dim_idx_in_alias; t++) {
        //  keep original dims
        new_array_dims_tmp.push_back(one_init_dims[t]);
      }

      int new_dim = modified_old_steps.size();
      for (i = 0; i < modified_old_steps.size(); i++) {
        //  split dim: if curr_total==0, a pointer at leftmost level is
        //  generated
        size_t dim_size = static_cast<size_t>(
            ceil(curr_total * 1.0 / modified_old_steps[new_dim - 1 - i]));
        new_array_dims_tmp.push_back(dim_size);
        curr_total =
            modified_old_steps[new_dim - 1 - i];  //  propagate to another dim
      }

      for (size_t t = dim_idx_in_alias + 1; t < one_init_dims.size(); t++) {
        //  keep original dims
        new_array_dims_tmp.push_back(one_init_dims[t]);
      }

      cout << "NEW array dims: ";
      for (auto dim : new_array_dims_tmp) {
        cout << dim << ",";
        new_array_dims->push_back(dim);
      }
      cout << endl;

#if PROJDEBUG
      string old_type_msg = ast->UnparseToString(ast->GetTypebyVar(one_init));
#endif

      //  ZP: 20170312
      //  Alias type generation Overall:
      //   1. originally array type without split -> array type with old dim
      //   size
      //   2. originally point type without split -> point type
      //   3. split dim -> the leftmost generate dim follows the original type
      //                   the remaining generated dims have array sizes of
      //                   "modified old_steps"
      //  ast->SetTypetoVar(one_init,
      //  ast->CreateCombinedPointerArrayType(sg_base_type,
      //  new_array_dims_tmp));
      //
      void *sg_new_type =
          ast->CreateCombinedPointerArrayType(sg_base_type, new_array_dims_tmp);
      map_update_alias_type.insert(pair<void *, void *>(one_init, sg_new_type));

      //  ZP: 20170322
      //  Apply the type change for all the forward declarations, not only the
      //  definition int has_decl_in_header = 0; map<void*,void*>
      //  map_alias_to_forward_decl; //  collect the decls to be updated
      {
        if (ast->IsArgumentInitName(one_init) != 0) {
          //  a. Find all the init names
          void *curr_decl = ast->TraceUpToFuncDecl(one_init);

          vector<void *> v_decl = ast->GetAllFuncDecl(curr_decl);
          for (auto decl : v_decl) {
            //  void * init = ast->GetFuncParam(decl, arg_idx);
            if (decl == curr_decl) {
              continue;
            }

            if (ast->isWithInHeaderFile(decl)) {
              //  FIXME: ZP: 20170322
              //  Need to add user messsage as well
//  #ifdef PROJDEBUG
#if 0
              string msg = "\n[DELINEAR] Warning: array delinearization is ";
              msg += "disabled due to declaration in the header file.\n";
              msg += "  Argument: " + ast->_up(init) + "\n";
              msg += "  Function: " + ast->_up(decl) + "\n";
              msg += "  File: " + ast->GetFileInfo_filename(decl) + "\n";
              cout << msg << endl;
#endif

              has_decl_in_header = 1;
              break;
            }
            map_alias_to_forward_decl.push_back(
                pair<void *, void *>(decl, one_init));
          }
          set_alias_decl.insert(curr_decl);
        }
      }

#if PROJDEBUG
      cout << "  ---- TYPE: " << ast->_p(one_init) << " " << old_type_msg
           << " -> ";
#endif
#if PROJDEBUG
      if (has_decl_in_header != 0) {
        cout << ast->UnparseToString(sg_new_type)
             << " Disabled due to decls in header" << endl;
      } else {
        cout << ast->UnparseToString(sg_new_type) << endl;
      }
#endif

      if (has_decl_in_header != 0) {
        return 0;  //  skip this dimension
      }
    }

    //  //  3. apply to function arg of memset and memcpy
    //  [Old code, deleted]
  }
  //  end_of_apply_delinear_type_change:

  //  perform transformation for each reference
  vector<void *> vec_sys_call_ref;
  vector<pair<void *, void *>> vec_exp_replace;
  {
    for (i = 0; i < vec_access.size(); i++) {
      void *sg_pntr = vec_access[i];
      //  void *sg_org_pntr = sg_pntr;
      //  if (ast->IsCastExp(sg_pntr)) ast->remove_cast(&sg_pntr);

      //  ZP: 20170401
      if (ast->is_floating_node(sg_pntr) != 0) {
        continue;
      }

      //  int type_dim; //  = ast->get_pntr_type_dim(sg_pntr);
      int pntr_dim;  //  = ast->get_pntr_type_dim(sg_pntr);
      void *tt_array = nullptr;
      {
        vector<void *> tt_vec;
        ast->parse_pntr_ref_new(sg_pntr, &tt_array, &tt_vec);
        pntr_dim = tt_vec.size();

        //  void *tt_type = ast->GetTypebyVar(tt_array);
        //  void *tt_base_type;
        //  vector<size_t> tt_size;
        //  ast->get_type_dimension(tt_type, &tt_base_type, &tt_size, sg_pntr);
        //  type_dim = tt_size.size();
      }
      int pntr_dim_org = pntr_dim;

      //  if (pntr_dim == 0) {
      //  int param_index = ast->GetFuncCallParamIndex(sg_org_pntr);
      //  if (-1 != param_index) {
      //    void *sg_call = ast->TraceUpByTypeCompatible(sg_pntr,
      //    V_SgFunctionCallExp); assert(sg_call);

      //    string str_call = ast->GetFuncNameByCall(sg_call);

      //    if (str_call == "memcpy" || str_call == "memset") {
      //      vec_sys_call_ref.push_back(sg_pntr);
      //    }
      //  }
      //  continue;
      //  }

      //  ZP: 20170312: handle top level pointer (and offset) in a unified way
      int has_address_of = 0;
      //  if (pntr_dim == 0) {
      //    pntr_dim = 1;
      //    has_address_of = 1;
      //  }
      //
      //
      int dim_idx_in_pntr;
      {
        int pntr_dim =
            ast->get_dim_num_from_var(ast->get_array_from_pntr(sg_pntr));
        dim_idx_in_pntr = dim_idx_org - (array_dim - pntr_dim);
      }
      if (dim_idx_in_pntr < 0) {
        continue;
      }

      if (pntr_dim != array_dim) {
        pntr_dim++;
        has_address_of = 1;
      }

      vector<CMarsExpression> vec_new_indexes;

      CMarsAccess ac(sg_pntr, ast, nullptr);
      vector<CMarsExpression> vec_me_idx = ac.GetIndexes();
      CMarsExpression me = vec_me_idx[dim_idx_in_pntr];
      me = me - *static_cast<CMarsExpression *>(offset);

      //  ////////  /
      //  ZP: 20170104 // support of multiple dimension array //  handling left
      //  dims
      int tmp_size = vec_me_idx.size();
      for (int t = tmp_size; t < pntr_dim; t++) {
        vec_me_idx.push_back(CMarsExpression(ast, 0));
      }

      for (int t = 0; t < min(dim_idx_in_pntr, pntr_dim); t++) {
        vec_new_indexes.push_back(vec_me_idx[t]);
      }

      void *pntr_init = ast->get_array_from_pntr(sg_pntr);
      if (TEST_FAKE_ALIAS_RECORD) {
        void *pntr_alias =
            ast->get_alias_array_from_assignment_expression(sg_pntr);
        if (pntr_alias == nullptr) {
          pntr_alias = pntr_init;
        }
        if (ast->IsPointerType(ast->GetTypebyVar(pntr_alias)) != 0) {
          if (mapAlias2BStep->find(pntr_alias) == mapAlias2BStep->end()) {
            mapAlias2BStep->insert(pair<void *, size_t>(pntr_alias, 32));
          }
        }
      }

      //  ///////////////////////////////////////////////////////////////////////
      //  / ZP: 20170312: handling offset on partial pntr Use small steps for
      //  alias pntr, and use large steps for alias assignment expression
      vector<size_t> modified_old_steps = old_steps;
      //  1. filter out large values if it is an alias
      if (mapAlias2BStep->end() != mapAlias2BStep->find(pntr_init)) {
        modified_old_steps =
            get_small_step_list(old_steps, (*mapAlias2BStep)[pntr_init]);
      }
      //  2. filter out small values if it is an assignment expression to
      //  another alias
      void *alias_array =
          ast->get_alias_array_from_assignment_expression(sg_pntr);
      if ((alias_array != nullptr) &&
          mapAlias2BStep->end() != mapAlias2BStep->find(alias_array)) {
        modified_old_steps = get_large_step_list(
            modified_old_steps, (*mapAlias2BStep)[alias_array]);
      }
      //  ///////////////////////////////////////////////////////////////////////
      //  /
      //      if (modified_old_steps.size() == 0)
      //      modified_old_steps.push_back(1);

      //  apply_delinear_index_transform:
      if (dim_idx_in_pntr < pntr_dim) {
        int ret = apply_delinear_index_transform(ast, &me, modified_old_steps,
                                                 &vec_new_indexes,
                                                 has_address_of != 0);
        if (ret == 0) {
          cout << " -- Give up array_delinearization while applying" << endl;
          return (0);
        }
      }

      if (dim_idx_in_pntr < pntr_dim_org && !modified_old_steps.empty())
        if ((me != 0) != 0)
          return (0);

      //  ////////  /
      //  ZP: 20170104 // support of multiple dimension array //  handling the
      //  right dims
      //  for (int t = dim_idx+1; t < vec_me_idx.size(); t++)
      for (int t = dim_idx_in_pntr + 1; t < pntr_dim;
           t++) {  // for partial-dim array, skip the missing dims on the right
        vec_new_indexes.push_back(vec_me_idx[t]);
      }

#if 1
      if (has_address_of != 0) {
        int size = vec_new_indexes.size();
        CMarsExpression me = vec_new_indexes[size - 1];
        if ((me == 0) != 0) {
          vec_new_indexes.erase(vec_new_indexes.begin() + size - 1);
          has_address_of = 0;
        }
      }
#endif

      if (check_only) {
        (*mapIndex2Delinear)[sg_pntr] = vec_new_indexes;
        *is_simple &= CMarsAccess::is_simple_type_v1(vec_new_indexes);
        continue;
      }
      //  apply the new expression
      {
        void *local_var;
        {
          vector<void *> vec;
          ast->parse_pntr_ref_new(sg_pntr, &local_var, &vec);
          assert(local_var);
        }
        vector<void *> vec_new_expr;
        for (auto index : vec_new_indexes) {
          vec_new_expr.push_back(index.get_expr_from_coeff());
        }
        void *new_access = ast->CreateArrayRef(
            ast->CreateVariableRef(local_var), vec_new_expr);

        if (has_address_of != 0) {
          //  FIXME: adding address of only works if the split_dim is the last
          //  one of the alias assert( dim_idx+1 >= pntr_dim); //  have not
          //  reached FIXME: adding cases
          new_access = ast->CreateExp(V_SgAddressOfOp, new_access);
        }

        //  ZP: 20170320: add type cast conversion automatically for
        //  alias/func_arg
        {
          void *pntr_alias =
              ast->get_alias_array_from_assignment_expression(sg_pntr);
          if (pntr_alias != nullptr) {
            void *sg_type_alias = ast->GetTypebyVar(pntr_alias);
            void *sg_type_exp = ast->GetTypeByExp(new_access);

            //  ZP: 20170320 //  record alias type first and apply the change
            //  finally
            if (map_update_alias_type.end() !=
                map_update_alias_type.find(pntr_alias)) {
              sg_type_alias = map_update_alias_type[pntr_alias];
            }

            if (0 == ast->IsCompatibleType(sg_type_alias, sg_type_exp, sg_pntr,
                                           true) &&
                (ast->IsArrayType(sg_type_alias) == 0)) {
              new_access =
                  ast->CreateExp(V_SgCastExp, new_access, sg_type_alias);
            }
          }
        }

#if 0
        //  for debug
        int t = 0;
        for (auto expr : vec_new_expr) {
            CMarsExpression me(expr, ast, sg_pntr,
                ast->TraceUpToFuncDecl(sg_pntr));
            cout << "RANGE: " << vec_new_indexes[t++].print(2) << " -> " <<
                ast->_p(expr) << " "<<  me.get_range().print_const_bound()
                << endl;
        }
        init_defuse();
        for (auto expr : vec_new_expr) {
            CMarsExpression me(expr, ast, sg_pntr,
                ast->TraceUpToFuncDecl(sg_pntr));
            cout << "RANGE: " << ast->_p(expr) << " "
              << me.get_range().print_const_bound() << endl;
        }
#endif

#if PROJDEBUG
        cout << "  ---- EXP: " << ast->_p(sg_pntr) << " -> ";
#endif

        vec_exp_replace.push_back(pair<void *, void *>(sg_pntr, new_access));

#if PROJDEBUG
        cout << ast->_p(new_access) << endl;
#endif
      }
    }
    if (check_only) {
      return is_feasible;
    }
  }

  for (auto it : vec_exp_replace) {
    ast->ReplaceExp(it.first, it.second);
  }

  for (auto init_alias_type : map_update_alias_type) {
    void *one_init = init_alias_type.first;
    if (ast->IsArgumentInitName(one_init) == 0) {
      ast->SetTypetoVar(one_init, init_alias_type.second);
    }
  }

  for (auto decl : set_alias_decl) {
    vector<void *> vec_calls;
    ast->GetFuncCallsFromDecl(decl, nullptr, &vec_calls);
    for (auto arg : ast->GetFuncParams(decl)) {
      //  ZP: 20170320 //  record alias type first and apply the change finally
      if (map_update_alias_type.count(arg) > 0) {
        ast->SetTypetoVar(arg, map_update_alias_type[arg]);
      }
    }
  }
  //  ZP: 20170322
  //  Change the type in the forward declarations
  //  NOTE: only set argument type does not work for forward decl
  for (auto one_pair : map_alias_to_forward_decl) {
    void *one_decl = one_pair.first;
    void *one_init = one_pair.second;
    void *new_decl = ast->TraceUpToFuncDecl(one_init);

    void *new_forward_decl =
        ast->CloneFuncDecl(new_decl, ast->GetGlobal(new_decl), false);
    ast->ReplaceStmt(one_decl, new_forward_decl);
  }

  return is_feasible;
}

int local_array_delinearization(CSageCodeGen *codegen, void *pTopFunc,
                                const CInputOptions &options) {
  DEFINE_START_END;
  STEMP(start);

  if (!options.get_option_key_value("-a", "naive_hls").empty()) {
    return 0;
  }
  bool xilinx_flow = false;
  if ("sdaccel" != options.get_option_key_value("-a", "impl_tool")) {
    xilinx_flow = true;
  }

  size_t j;
  map<void *, int> map_array;  //  the array to transform

  //  1. get the arrays to transform
  //
#if 0
  //  Array of interests:
  //  a. it is local declared (not func_arg or global var)
  //  b0. Xilinx: it has reference in the loops with pragma
  //  b1. Altera: it has reference in the kernel (wider than Xilinx)
  CMarsIr::iterator it;
  for (it = mars_ir.ir_list.begin(); it != mars_ir.ir_list.end(); ++it) {
    CMirNodeSet new_MirNodeSet = *it;
    for (int i = 0; i < new_MirNodeSet.size(); i++) {
      CMirNode *new_node = new_MirNodeSet[i];

      //  ZP: 20170310: do not differentiate Xilinx and Altera
      if (all_nodes_set.count(new_node) <= 0)
        continue;

      //  if (!altera_flow && !new_node->has_opt_pragmas())
      //    continue;
      //
      //  //ZP: 20170307: host function needs to be excluded from the mars_ir
      //  //( v2 has excluded host functions)
      //  if (altera_flow && all_nodes_set.count(new_node) <= 0)
      //    continue;
      void *scope = new_node->ref;

      int nLine = codegen->GetFileInfo_line(codegen->_pa(scope));
      string sFile = codegen->GetFileInfo_filename(codegen->_pa(scope), 1);
#if PROJDEBUG
      cout << "  ---- Loop [" << sFile << ":" << nLine << "] "
        << codegen->_p(codegen->_pa(scope)) << endl;
#endif
      vector<void *> vec_ref;
      codegen->GetNodesByType(scope, "preorder", "SgVarRefExp", &vec_ref);
      for (j = 0; j < vec_ref.size(); j++) {
        void *sg_ref = vec_ref[j];
        void *sg_array = codegen->GetVariableInitializedName(sg_ref);
        if (codegen->IsLocalInitName(sg_array) &&
            codegen->IsArrayType(codegen->GetTypebyVar(sg_array))) {
          void *curr_func = codegen->TraceUpByTypeCompatible(
              sg_array, V_SgFunctionDeclaration);
          map<void *, bool> res;
          if (curr_func &&
              !mars_ir.any_trace_is_bus(codegen, curr_func, sg_array,
                &res)) {
            map_array[sg_array] = 1;
          }
        }
      }
    }
  }
#endif

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();

  for (auto k : kernels) {
    SetVector<void *> v_sub;
    SetVector<void *> v_call;
    codegen->GetSubFuncDeclsRecursively(k, &v_sub, &v_call);
    v_sub.insert(k);

    for (auto sub : v_sub) {
      vector<void *> vec_ref;
      codegen->GetNodesByType(sub, "preorder", "SgVarRefExp", &vec_ref);
      for (j = 0; j < vec_ref.size(); j++) {
        void *sg_ref = vec_ref[j];
        void *sg_array = codegen->GetVariableInitializedName(sg_ref);
        if ((codegen->IsLocalInitName(sg_array) != 0) &&
            (codegen->IsArrayType(codegen->GetTypebyVar(sg_array)) != 0) &&
            (codegen->is_all_dim_array(codegen->GetTypebyVar(sg_array)) != 0) &&
            !mars_ir.get_coalescing_flag(sg_array)) {
#if 0  //  PROJDEBUG
          if (map_array.find(sg_array) == map_array.end())
            cout << " -- Find an array to delinearize -- "
              << codegen->_p(sg_array) << endl;
#endif
          map_array[sg_array] = 1;
        }
      }
    }
  }

  //  2. transform each
  map<void *, int>::iterator it1;
  for (it1 = map_array.begin(); it1 != map_array.end(); it1++) {
    void *one_array = it1->first;
    int dim_size = codegen->get_dim_num_from_var(one_array);
    string type_old =
        codegen->UnparseToString(codegen->GetTypebyVar(one_array));
    for (int dimIdx = dim_size - 1; dimIdx >= 0; dimIdx--) {
      vector<size_t> dim_split_steps;
      map<void *, size_t> mapAlias2BStep;

      analysis_delinearize(codegen, one_array, dimIdx, &dim_split_steps,
                           &mapAlias2BStep, xilinx_flow);
      if (apply_delinearize(codegen, one_array, dimIdx, dim_split_steps,
                            &mapAlias2BStep) == 0) {
        continue;
      }
      codegen
          ->init_defuse_range_analysis();  //  update the range information
                                           //  for the transformed loop bounds

#if PROJDEBUG
      string s_new_dims;
      {
        auto old_steps = dim_split_steps;
        auto new_dims = dim_split_steps;
        for (auto t : old_steps) {
          s_new_dims += "," + my_itoa(t);
        }
        if (static_cast<unsigned int>(!old_steps.empty()) != 0U) {
          s_new_dims[0] = ' ';
        }
        int pos = s_new_dims.size();
        for (auto t : new_dims) {
          s_new_dims += "," + my_itoa(t);
        }
        if (static_cast<unsigned int>(!new_dims.empty()) != 0U) {
          s_new_dims[pos] = ':';
        }
        s_new_dims += " ";
      }
      cout << "  ---- " << codegen->_p(one_array) << ": "
           << " dim=" << my_itoa(dimIdx) << " (" << s_new_dims << ") " << endl;
#endif
    }
    string type_new =
        codegen->UnparseToString(codegen->GetTypebyVar(one_array));

#if PROJDEBUG
    cout << "  ---- " << codegen->_p(one_array) << ": " << type_old << " -> "
         << type_new << endl;
#endif
    //  #if 1
    if (type_old != type_new && (g_debug_mode != 0)) {
      string msg = "Delinearizing the array '" +
                   codegen->UnparseToString(one_array) + "' " +
                   getUserCodeFileLocation(codegen, one_array, true) + "\n";
      //            msg += string("  ") + type_old + " -> " + type_new + "\n";

#if PROJDEBUG
      cout << msg << endl;
      dump_critical_message("DELIN", "INFO", msg, 101, 1);
#endif
    }
    //  #endif
  }
  TIMEP("local_array_delinearization", start, end);
  return 1;
}
