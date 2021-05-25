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


#include "SystolicData.h"
#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "codegen.h"
#include "history.h"
#include "mars_ir_v2.h"
#include "program_analysis.h"
#include "array_delinearize.h"

#define div_floor(x, y) ((int64_t)floor((x)*1.0 / (y)))
#define max_divide(x, y) ((int64_t)div_floor((x), (y)) * (y))

#define DEBUG_EXTRACT 1

#define DEBUGINFO(str)                                                         \
  std::cout << __LINE__ << " line: " << __FUNCTION__                           \
            << " func: " << std::string(str) << std::endl;

#define DEBUG_CODEGEN_GOLDEN(str)                                              \
  std::cout << __LINE__ << " line: " << __FUNCTION__                           \
            << " func: " << std::string(str) << std::endl;

#define SIMPLE_DELINEARIZE 0
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
float SystolicData::GetCompThrpt() {
  // Computation throughput
  double *thrpt = new double[GetLayerNum()];

  // Cache hit
  if (cacheModelVal.find("layer_0_tripcount") != cacheModelVal.end()) {
    for (int i = 0; i < GetLayerNum(); i++) {
      thrpt[i] = cacheModelVal["layer_" + std::to_string(i) + "_tripcount"];
    }
  } else {  // Compute the product of tripcount for every layer
    for (int i = 0; i < GetLayerNum(); i++) {
      thrpt[i] = 1;
    }
    for (auto pair : mapLoop2TCs) {
      for (int i = 0; i < GetLayerNum(); i++) {
        thrpt[i] *= pair.second[i];
      }
    }
    for (int i = 0; i < GetLayerNum(); i++) {
      cacheModelVal["layer_" + std::to_string(i) + "_tripcount"] = thrpt[i];
    }
  }

  // Compute throughput considering systolic PEs and feeders
  for (auto pair : mapLoop2TCs) {
    int feeder = 1;
    if (mapFeeder2Value.find(pair.first) != mapFeeder2Value.end()) {
      feeder = mapFeeder2Value[pair.first];
    }
    int sys = 1;
    for (auto pe : vecPE) {
      if (pe == pair.first) {
        sys = mapPE2Value[pair.first];
        break;
      }
    }

    for (int i = 0; i < GetLayerNum(); i++) {
      int value = pair.second[i];
      thrpt[i] /=
          ceil(static_cast<float>(value) / static_cast<float>(feeder * sys)) *
          static_cast<float>(feeder);
    }
  }

  // Average throughput
  double avgThrpt = 0;
  for (int i = 0; i < GetLayerNum(); i++) {
    avgThrpt += thrpt[i];
  }
  avgThrpt /= GetLayerNum();

  delete[] thrpt;
  return static_cast<float>((FREQ * 2 * avgThrpt) / 1000);
}

// TODO(youxiang): Cache result
float SystolicData::GetThrpt() {
  // Computation throughput
  float thrpt = GetCompThrpt();

  //    std::cout << "CompThrpt=" << thrpt << "Gops" << std::endl;

  // BW throughput
  int64_t base = 2;
  for (auto pair : mapFeeder2Value) {
    base *= pair.second;
  }
  for (auto pe : vecPE) {
    base *= mapPE2Value[pe];
  }

  vector<float> vecDataSize;
  vecDataSize.resize(4);
  vecDataSize[IN] = GetRealMemoryUsage(IN);
  vecDataSize[WT] = GetRealMemoryUsage(WT);
  vecDataSize[OUT] = GetRealMemoryUsage(OUT);
  vecDataSize[3] = vecDataSize[0] + vecDataSize[1] + vecDataSize[2];

  //    std::cout << "vecDataSize[IN]=" << vecDataSize[IN] << std::endl;
  //    std::cout << "vecDataSize[WT]=" << vecDataSize[WT] << std::endl;
  //    std::cout << "vecDataSize[OUT]=" << vecDataSize[OUT] << std::endl;
  //    std::cout << "vecDataSize[Total]=" << vecDataSize[3] << std::endl;

  vector<float> requiredBW;
  requiredBW.resize(4);
  GetBW(&requiredBW);

  //    std::cout << "requiredBW[IN]=" << requiredBW[IN] << std::endl;
  //    std::cout << "requiredBW[WT]=" << requiredBW[WT] << std::endl;
  //    std::cout << "requiredBW[OUT]=" << requiredBW[OUT] << std::endl;
  //    std::cout << "requiredBW[Total]=" << requiredBW[3] << std::endl;

  float maxBWPerPort = GetMaxBWperPort();

  //    std::cout << "maxBWPerPort=" << maxBWPerPort << std::endl;

  for (int i = 0; i < 3; i++) {
    float realBW =
        (requiredBW[i] > maxBWPerPort) ? maxBWPerPort : requiredBW[i];
    thrpt = min(thrpt, (base * realBW) / vecDataSize[i]);

    //    std::cout << "thrpt@port " << i << " is " << (base * realBW) /
    // vecDataSize[i] << std::endl;
  }
  float realBW =
      (requiredBW[3] > MAX_BANDWIDTH) ? MAX_BANDWIDTH : requiredBW[3];
  thrpt = min(thrpt, (base * realBW) / vecDataSize[3]);

  return thrpt;
}

int SystolicData::GetBRAMBlk() {
  int wt = 1;
  for (auto feeder : mapFeeder2Value) {
    if (vecSetRelatedLoop[WT].find(feeder.first) ==
        vecSetRelatedLoop[WT].end()) {
      continue;
    }
    int blk = feeder.second;
    if (feeder.first == vecPE[SIMD]) {
      blk *= mapPE2Value[vecPE[SIMD]];
    }
    wt *= GetRoundUpPow2(blk);  // UpPower2(Tile * VEC_OR_1)
  }
  wt *= 2;                                         // Double buffing
  wt *= typeSize;                                  // Type size in byte
  wt = ceil(static_cast<float>(wt) / (1024 * 2));  // Block size (2KB)
  wt += 4;                        // Altera flow overhead (4 blocks)
  wt *= mapPE2Value[vecPE[ROW]];  // #FeederWt
#ifdef MSG_FINE
  cerr << MSG_FINE;
  cerr << "WT (" << wt << "), ";
#endif

  int in = 1;
  for (auto feeder : mapFeeder2Value) {
    if (vecSetRelatedLoop[IN].find(feeder.first) ==
        vecSetRelatedLoop[IN].end()) {
      continue;
    }
    int blk = feeder.second;
    if (feeder.first == vecPE[SIMD]) {
      blk *= mapPE2Value[vecPE[SIMD]];
    }
    in *= GetRoundUpPow2(blk);  // UpPower2(Tile * VEC_OR_1)
  }
  in *= 2;                                         // Double buffing
  in *= typeSize;                                  // Type size in byte
  in = ceil(static_cast<float>(in) / (1024 * 2));  // Block size (2KB)
  in += 4;                        // Altera flow overhead (4 blocks)
  in *= mapPE2Value[vecPE[COL]];  // #FeederIn

#ifdef MSG_FINE
  cerr << "IN (" << in << ")" << endl;
#endif

  int pe = 1;
  pe *= mapPE2Value[vecPE[ROW]];
  pe *= mapPE2Value[vecPE[COL]];
  pe *= ALTERA_SYSTOLIC_ARRAY_PE_BLK;  // Altera flow overhead, heuristic value

  return wt + in + pe;
}

float SystolicData::GetRealMemoryUsage(SYSTOLIC_IO ioType) {
  void *exp;
  int totalSize = 1;
  totalSize *= typeSize;  // Type size in byte
  exp = vecExp[ioType];
  while (m_ast->IsPntrArrRefExp(exp) != 0) {
    // Fetch the last dimension index
    void *idxExp = m_ast->GetExpRightOperand(exp);
    float size = 0;
    float subSize = 0;

    // Compute memory usage
    // FIXME: Now assuming multi-dimensional arrays.
    // Doesn't support [i * 10 + j].
    if (m_ast->IsBinaryOp(idxExp) != 0) {
      switch (GetBinaryOperation(idxExp)) {
      case V_SgAddOp: {
        void *l =
            m_ast->GetVariableInitializedName(m_ast->GetExpLeftOperand(idxExp));
        void *r = m_ast->GetVariableInitializedName(
            m_ast->GetExpRightOperand(idxExp));
        if (mapFeeder2Value.find(l) != mapFeeder2Value.end()) {
          subSize = this->mapFeeder2Value[l];
        } else {
          subSize = 1;
        }
        if (IsSystolicPE(l)) {
          subSize *= this->mapPE2Value[l];
        }
        size += subSize;

        if (mapFeeder2Value.find(r) != mapFeeder2Value.end()) {
          subSize = this->mapFeeder2Value[r];
        } else {
          subSize = 1;
        }
        if (IsSystolicPE(r)) {
          subSize *= this->mapPE2Value[r];
        }
        size += subSize;

        size -= 1;
        break;
      }
      default: {
        cerr << MSG_ERROR;
        cerr << "Unsupported binary operator in array index accessing";
        cerr << endl;
        assert(0);
      }
      }
    } else {
      void *r = m_ast->GetVariableInitializedName(idxExp);
      assert(r);
      if (mapFeeder2Value.find(r) != mapFeeder2Value.end()) {
        subSize = this->mapFeeder2Value[r];
      } else {
        subSize = 1;
      }
      if (IsSystolicPE(r)) {
        subSize *= this->mapPE2Value[r];
      }
      size += subSize;
    }
    totalSize *= static_cast<int>(size);
    exp = m_ast->GetExpLeftOperand(exp);
  }

  return static_cast<float>(totalSize);
}

// Altera given bandwidth per port
float SystolicData::GetMaxBWperPort() {
  return static_cast<float>(GetTypeSize() * GetSystolicPESize(SIMD) * FREQ) /
         1000.0f * 0.84f;
}

// TODO(youxiang): Fully one-dimensional array support
void SystolicData::GetBW(vector<float> *bw) {
  // Compute actual memory usage
  (*bw)[IN] = GetRealMemoryUsage(IN);
  (*bw)[WT] = GetRealMemoryUsage(WT);
  (*bw)[OUT] = GetRealMemoryUsage(OUT);
  (*bw)[3] = (*bw)[IN] + (*bw)[WT] + (*bw)[OUT];

  float cycle = 1;
  for (auto feeder : this->mapFeeder2Value) {
    cycle *= feeder.second;
  }

  for (int i = 0; i < 4; i++) {
    (*bw)[i] = ((*bw)[i] * FREQ / 1000) / cycle;
  }
}

string SystolicData::GetFeederIdxInfo(void *ref_expr,
                                      const vector<void *> &outer_loops,
                                      const vector<void *> &feeder_loops,
                                      const vector<void *> &pe_loops,
                                      vector<int> *buffer_dim) {
  CSageCodeGen *ast = m_ast;
  string access_pattern;
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  vector<void *> loops_to_substitute;
  loops_to_substitute.insert(loops_to_substitute.begin(), outer_loops.begin(),
                             outer_loops.end());
  loops_to_substitute.insert(loops_to_substitute.begin(), pe_loops.begin(),
                             pe_loops.end());

#if (SIMPLE_DELINEARIZE == 1)
  // step 1: delinearize
  vector<CMarsExpression> new_vec_idx;
  for (auto ref_idx : vec_idx) {
    CMarsExpression ma_idx = ref_idx;
    for (auto loop : loops_to_substitute) {
      ma_idx.substitute(loop, 0);
    }

    vector<CMarsExpression> vec_ma_idx;

    SimpleDelinearization(ma_idx, vec_ma_idx);

    for (auto new_ref_idx : vec_ma_idx) {
      new_vec_idx.push_back(new_ref_idx);
    }
  }
  // step 2: get upper bounds of each dimension
  int dim = 0;
  for (auto new_ref_idx : new_vec_idx) {
    int ub = new_ref_idx.get_range().get_ub().get_const();

    buffer_dim->push_back(ub + 1);

    string str_idx = GetCoeffString(new_ref_idx, feeder_loops);

    if (dim != 0) {
      access_pattern += ";";
    }
    access_pattern += str_idx;

    dim++;
  }
#else
  vector<size_t> new_ubs;
  vector<size_t> old_steps;
  vector<size_t> new_dims;

  int dim = 0;
  for (auto ref_idx : vec_idx) {
    // substitute the outer and inner loops

    CMarsExpression new_idx = ref_idx;
    for (auto loop : loops_to_substitute) {
      new_idx.substitute(loop, 0);
    }

    int num_new_ubs =
        AnalysisDelinearize(&new_idx, &old_steps, &new_dims, &new_ubs);

    // string str_idx = GetCoeffString(new_idx, feeder_loops);
    string str_idx = GetCoeffString(new_idx, feeder_loops, num_new_ubs);

    // FIXME
    vector<void *> vec_vars = new_idx.get_vars();
    if (vec_vars.size() == 1) {
      //    int ub = new_idx.get_range().get_ub().get_const();
      CMarsVariable mv = CMarsVariable(vec_vars[0], ast);
      int i_ub = mv.get_range().get_ub().get_const();
      buffer_dim->push_back(i_ub + 1);
    } else {
      if (num_new_ubs > 1) {
        size_t ub0 = new_ubs[0];
        size_t ub1 = new_ubs[1];
        size_t ub_max = ub0 > ub1 ? ub0 : ub1;

        for (auto var : vec_vars) {
          CMarsExpression coeff = new_idx.get_coeff(var);
          CMarsVariable mv = CMarsVariable(var, ast);

          int i_coeff = coeff.get_const();
          int i_ub = mv.get_range().get_ub().get_const();

          //    printf("coeff=%d\n", i_coeff);
          //    printf("ub=%d\n", i_ub);

          if (static_cast<size_t>(i_coeff) * i_ub == ub_max) {
            buffer_dim->push_back(i_ub + 1);
            buffer_dim->push_back(i_coeff);
          }
        }
      } else {
        buffer_dim->push_back(new_ubs[0] + 1);
      }
    }

    if (dim != 0) {
      access_pattern += ";";
    }
    access_pattern += str_idx;

    dim++;

    new_ubs.clear();
  }
#endif

  return access_pattern;
}

/*
void SystolicData::GetRightSideBounds(void *ref_expr,
        const vector<void *> &outer_loops,
        const vector<void *> &feeder_loops,
        const vector<void *> &pe_loops,
        FeederLoops &Feeder)
{
    CSageCodeGen * ast = m_ast;
    string access_pattern = "";
    CMarsAccess ma(ref_expr, ast, nullptr);
    auto vec_idx = ma.GetIndexes();

    vector<void*> loops_to_substitute;
    loops_to_substitute.insert(
            loops_to_substitute.begin(),
            outer_loops.begin(), outer_loops.end());
    loops_to_substitute.insert(
            loops_to_substitute.begin(),
            pe_loops.begin(), pe_loops.end());

    int dim = 0;
    for (auto ref_idx: vec_idx)
    {
        CMarsExpression new_idx = ref_idx;
        for (auto loop : loops_to_substitute)
        {
            new_idx.substitute( loop, 0);
        }

        if (dim++ == Feeder.pe_iter_dim)
        {
            Feeder.right_side_lb = new_idx.get_range().get_lb().get_const();
            Feeder.right_side_ub = new_idx.get_range().get_ub().get_const();
        }
    }
}
*/

void SystolicData::SetPEIterIdx(int pe_iter_idx, FeederLoops *Feeder) {
  //    Feeder_H.pe_iter_idx = 0;
  //    Feeder_V.pe_iter_idx = 1;
  Feeder->pe_iter_idx = pe_iter_idx;
}

void SystolicData::SetPEIterDim(void *ref_expr, const vector<void *> &pe_loops,
                                FeederLoops *Feeder) {
  CSageCodeGen *ast = m_ast;
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  int dim = 0;
  for (auto ref_idx : vec_idx) {
    CMarsExpression coeff = ref_idx.get_coeff(pe_loops[Feeder->pe_iter_idx]);

    if ((coeff != 0) != 0) {
      Feeder->pe_iter_dim = dim;
      break;
    }

    dim++;
  }
}

void SystolicData::CheckFeederLoopFeasibility(
    const vector<void *> &feeder_loops, void *ref_expr_o, void *ref_expr_h,
    void *ref_expr_v) {
  CSageCodeGen *ast = m_ast;

  CMarsAccess ma_o(ref_expr_o, ast, nullptr);
  auto vec_idx_o = ma_o.GetIndexes();

  CMarsAccess ma_h(ref_expr_h, ast, nullptr);
  auto vec_idx_h = ma_h.GetIndexes();

  CMarsAccess ma_v(ref_expr_v, ast, nullptr);
  auto vec_idx_v = ma_v.GetIndexes();

  /*
   * Check the feasibility of three loop iteratorsets (accumulation,
   * row reuse, and column reuse)
   * */
  // in which expression a feeder loop iterator resides
  int o = 1;  // 0001
  int h = 2;  // 0010
  int v = 4;  // 0100
  int flag = 0;
  // 3 sets: accumulation, horizontal PE reuse, vertical PE reuse
  int set[4] = {0};

  for (auto loop : feeder_loops) {
    flag = 0;
    std::cout << "flag=" << flag << std::endl;

    for (auto ref_idx : vec_idx_o) {
      CMarsExpression coeff = ref_idx.get_coeff(loop);

      if ((coeff != 0) != 0) {
        flag |= o;
        break;
      }
    }

    for (auto ref_idx : vec_idx_h) {
      CMarsExpression coeff = ref_idx.get_coeff(loop);

      if ((coeff != 0) != 0) {
        flag |= h;
        break;
      }
    }

    for (auto ref_idx : vec_idx_v) {
      CMarsExpression coeff = ref_idx.get_coeff(loop);

      if ((coeff != 0) != 0) {
        flag |= v;
        break;
      }
    }

    std::cout << "flag=" << flag << std::endl;

    int i = 0;
    if (flag == 6) {  // h and v, 0110
      i = 0;
      //    flag[o] = 1;
    }
    if (flag == 3) {  // o and h, 0011
      i = 1;
      //    flag[h] = 1;
    }
    if (flag == 5) {  // o and v, 0101
      i = 2;
      //    flag[v] = 1;
    }

    set[i] = 1;
    for (int j = i + 1; j < 4; j++) {
      // later sets should never appear before current set
      if (set[j] != 0) {
        std::cout << "Feeder loops are illegal." << std::endl;
        exit(1);
      }
    }
  }

  /*
   * Check the feasibility of loop order within a set.
   * Method: test if relative orders of loop iterators and dimensions in out
   * array match.
   * */
  int idx = 0;
  for (auto loop : feeder_loops) {
    int dim = 0;
    for (auto ref_idx : vec_idx_o) {
      CMarsExpression coeff = ref_idx.get_coeff(loop);

      if ((coeff != 0) != 0) {
        if (idx != dim) {
          std::cout << "Feeder loops are illegal." << std::endl;
          exit(1);
        }
        idx++;
      }
      dim++;
    }
  }
}

void SystolicData::GetBlockDim(void *ref_expr,
                               const vector<void *> &outer_loops,
                               const vector<void *> &feeder_loops,
                               const vector<void *> &pe_loops,
                               vector<int> *block_dim) {
  // xuechao: 11/07/2018
  // test Merlin's delinearization functionality
  /*
  void *one_array = ref_expr;
  void *one_array_var = NULL;

  m_ast->get_var_from_pntr_ref(one_array, one_array_var);
  auto *one_array_init_name =
      m_ast->GetVariableInitializedName(one_array_var);

  std::cout << __func__ << ":" << "one_array="
      << m_ast->_p(one_array) << std::endl;

  vector<size_t> dim_split_steps;
  map<void *, size_t> mapAlias2BStep;
  bool is_simple = true;
  CMarsExpression zero(m_ast, 0LL);

  int delinearized = analysis_delinearize(
     // *m_ast, one_array, 0, dim_split_steps, mapAlias2BStep, true,
      *m_ast, one_array_init_name, 0, dim_split_steps, mapAlias2BStep, true,
      is_simple, &zero, nullptr);
  if (!is_simple && delinearized)
  {
    delinearized =
       // apply_delinearize(*m_ast, one_array, 0, dim_split_steps,
        apply_delinearize(*m_ast, one_array_init_name, 0, dim_split_steps,
                          mapAlias2BStep, true, is_simple, &zero, nullptr);
    is_simple &= delinearized;
  }
  */

  CSageCodeGen *ast = m_ast;
  //    string access_pattern = "";
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  vector<void *> loops_to_substitute;
  loops_to_substitute.insert(loops_to_substitute.begin(), feeder_loops.begin(),
                             feeder_loops.end());
  loops_to_substitute.insert(loops_to_substitute.begin(), pe_loops.begin(),
                             pe_loops.end());

#if (SIMPLE_DELINEARIZE == 1)
  // xuechao: 11/01/2018
  // delinearlization for 1d array
  vector<CMarsExpression> new_vec_idx;
  for (auto ref_idx : vec_idx) {
    CMarsExpression ma_idx = ref_idx;
    //    std::cout << "ref_idx=" << ref_idx.print(0) << std::endl;
    for (auto loop : loops_to_substitute) {
      ma_idx.substitute(loop, 0);
    }
    //    std::cout << "new_idx=" << new_idx.print(0) << std::endl;

    //    vector<size_t> old_steps;
    //    vector<size_t> new_dims;
    //    vector<size_t> new_ubs;

    vector<CMarsExpression> vec_ma_idx;

    //    AnalysisDelinearize(new_idx, old_steps, new_dims, new_ubs);
    //    ApplyDelinearize(new_idx, old_steps, new_dims, vec_new_index);
    SimpleDelinearization(ma_idx, vec_ma_idx);

    for (auto new_ref_idx : vec_ma_idx) {
      std::cout << "new_ref_idx=" << new_ref_idx.print(0) << std::endl;
      string str_idx = GetCoeffString(new_ref_idx, outer_loops);
      /*
       * flag is used to filter the last new_ref_idx from delinearization
       * for the cases like 2048*i0+256*k0.
       * For these cases, as the coefficient of k0 is not 1,
       * a tail idx is generated with all its coeffs and upper bounds are 0.
       * */
      //    int flag = 0;
      //    for (auto c : str_idx)
      //    {
      //        if (c != '0' && c != ',')
      //        {
      //            flag = 1;
      //            break;
      //        }
      //    }
      //    if (flag)
      { new_vec_idx.push_back(new_ref_idx); }
    }
  }

  int dim = 0;
  for (auto new_ref_idx : new_vec_idx) {
#if DEBUG_EXTRACT
    cout << "--BLK" << my_itoa(dim) << ": " << new_ref_idx.print(0) << endl;
#endif

    string str_idx = GetCoeffString(new_ref_idx, outer_loops);

#if DEBUG_EXTRACT
    cout << str_idx << endl;
#endif

    int coeff = 1;
    string str_coeff = "";

    // find the first non-zero coefficinece, there should be only
    // one non-zero number in the str_idx for each index variable.
    while (str_idx != "") {
      size_t pos = str_idx.find(',');
      if (pos == string::npos) {  // the last coefficience
        str_coeff = str_idx;
      } else {
        str_coeff = str_idx.substr(0, pos);
      }
      coeff = atoi(str_coeff.c_str());

      if (coeff != 0 || pos == string::npos)
        break;

      str_idx = str_idx.substr(pos + 1);

#if DEBUG_EXTRACT
      cout << str_idx << endl;
#endif
    }

    if (coeff == 0)
      coeff = 1;

    int ub = new_ref_idx.get_range().get_ub().get_const();
    //    block_dim->push_back((ub+1) / coeff);
    block_dim->push_back((ub / coeff) + 1);

#if DEBUG_EXTRACT
    cout << "--idx" << my_itoa(dim) << "[" << my_itoa(ub) << "]: " << str_idx
         << endl;
#endif

    dim++;
  }

#else
  int dim = 0;
  for (auto ref_idx : vec_idx) {
#if DEBUG_EXTRACT
    cout << "--BLK" << my_itoa(dim) << ": " << ref_idx.print(0) << endl;
#endif
    // substitute the outer and inner loops

    CMarsExpression new_idx = ref_idx;
    for (auto loop : loops_to_substitute) {
      new_idx.substitute(loop, 0);
    }

#if DEBUG_EXTRACT
    cout << "--BLK" << my_itoa(dim) << ": " << new_idx.print(0) << endl;
#endif

    string str_idx = GetCoeffString(new_idx, outer_loops);

#if DEBUG_EXTRACT
    cout << str_idx << endl;
#endif

    int coeff = 1;
    string str_coeff;

    // find the first non-zero coefficinece, there should be only
    // one non-zero number in the str_idx for each index variable.
    while (!str_idx.empty()) {
      size_t pos = str_idx.find(',');
      if (pos == string::npos) {  // the last coefficience
        str_coeff = str_idx;
      } else {
        str_coeff = str_idx.substr(0, pos);
      }
      coeff = atoi(str_coeff.c_str());

      if (coeff != 0 || pos == string::npos) {
        break;
      }

      str_idx = str_idx.substr(pos + 1);

#if DEBUG_EXTRACT
      cout << str_idx << endl;
#endif
    }

    if (coeff == 0) {
      coeff = 1;
    }

    int ub = new_idx.get_range().get_ub().get_const();
    //    block_dim->push_back((ub+1) / coeff);
    block_dim->push_back((ub / coeff) + 1);

#if DEBUG_EXTRACT
    cout << "--idx" << my_itoa(dim) << "[" << my_itoa(ub) << "]: " << str_idx
         << endl;
#endif

    dim++;
  }
#endif
}

void SystolicData::GetBlockSize(void *ref_expr,
                                const vector<void *> &outer_loops,
                                const vector<void *> &feeder_loops,
                                const vector<void *> &pe_loops,
                                vector<int> *block_size) {
  CSageCodeGen *ast = m_ast;
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  // xuechao: 11/08/2018
  // test SimpleDelinearization functionality
#if (SIMPLE_DELINEARIZE == 1)
  vector<CMarsExpression> new_vec_idx;
  for (auto ref_idx : vec_idx) {
    CMarsExpression ma_idx = ref_idx;
    vector<CMarsExpression> vec_ma_idx;
    SimpleDelinearization(ma_idx, vec_ma_idx);

    for (auto new_ref_idx : vec_ma_idx) {
      new_vec_idx.push_back(new_ref_idx);
    }
  }
#endif

  vector<void *> block_loops;
  vector<void *> loops_to_substitute;

  block_loops.insert(block_loops.begin(), pe_loops.begin(), pe_loops.end());
  block_loops.insert(block_loops.begin(), feeder_loops.begin(),
                     feeder_loops.end());

  loops_to_substitute.insert(loops_to_substitute.begin(), outer_loops.begin(),
                             outer_loops.end());

  int dim = 0;
#if (SIMPLE_DELINEARIZE == 1)
  for (auto ref_idx : new_vec_idx)
#else
  for (auto ref_idx : vec_idx)
#endif
  {
    // substitute the feeder and inner loops

    CMarsExpression new_idx = ref_idx;
    for (auto loop : loops_to_substitute) {
      new_idx.substitute(loop, 0);
    }

    int ub = new_idx.get_range().get_ub().get_const();
    block_size->push_back(ub + 1);

    dim++;
  }
}

void SystolicData::GetBlockSize(void *ref_expr,
                                const vector<void *> &outer_loops,
                                const vector<void *> &feeder_loops,
                                const vector<void *> &pe_loops,
                                vector<int> *new_block_size,
                                FeederLoops *Feeder) {
  CSageCodeGen *ast = m_ast;
  //    string access_pattern = "";
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  vector<void *> block_loops;
  vector<void *> loops_to_substitute;

  block_loops.insert(block_loops.begin(), pe_loops.begin(), pe_loops.end());
  block_loops.insert(block_loops.begin(), feeder_loops.begin(),
                     feeder_loops.end());

  loops_to_substitute.insert(loops_to_substitute.begin(), outer_loops.begin(),
                             outer_loops.end());

  vector<size_t> old_steps;
  vector<size_t> new_dims;
  vector<size_t> new_ubs;

  int dim = 0;
  for (auto ref_idx : vec_idx) {
    CMarsExpression new_idx = ref_idx;
    for (auto loop : loops_to_substitute) {
      new_idx.substitute(loop, 0);
    }

    CMarsExpression coeff = new_idx.get_coeff(pe_loops[Feeder->pe_iter_idx]);

    if ((coeff != 0) != 0) {
      // first round delinearization
      vector<CMarsExpression> vec_new_indexes;

      AnalysisDelinearize(&new_idx, &old_steps, &new_dims, &new_ubs);
      //    std::cout << "num_new_ubs=" << num_new_ubs << std::endl;
      ApplyDelinearize(&new_idx, old_steps, new_dims, &vec_new_indexes);

      for (auto new_ref_idx : vec_new_indexes) {
        //    std::cout << "new_ref_idx=" << new_ref_idx.print(0) << std::endl;
        CMarsExpression coeff =
            new_ref_idx.get_coeff(pe_loops[Feeder->pe_iter_idx]);

        int ub = new_ref_idx.get_range().get_ub().get_const();
        new_block_size->push_back(ub + 1);

        if ((coeff != 0) != 0) {
          Feeder->pe_iter_dim = dim;
          Feeder->pe_iter_coeff = coeff.get_const();

          for (auto loop : pe_loops) {
            new_ref_idx.substitute(loop, 0);
          }
          Feeder->pe_dim_no_r2_lb =
              new_ref_idx.get_range().get_lb().get_const();
          Feeder->pe_dim_no_r2_ub =
              new_ref_idx.get_range().get_ub().get_const();
        }

        dim++;
      }
    } else {
      int ub = new_idx.get_range().get_ub().get_const();
      new_block_size->push_back(ub + 1);

      dim++;
    }
  }
}

void SystolicData::GetReuseNum(void *ref_expr,
                               const vector<void *> &outer_loops,
                               const vector<void *> &feeder_loops,
                               const vector<void *> &pe_loops,
                               vector<int> *reuse_num) {
  CSageCodeGen *ast = m_ast;
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  vector<void *> loops_to_substitute;

  loops_to_substitute.insert(loops_to_substitute.begin(), outer_loops.begin(),
                             outer_loops.end());
  loops_to_substitute.insert(loops_to_substitute.begin(), pe_loops.begin(),
                             pe_loops.end());

  for (auto ref_idx : vec_idx) {
    CMarsExpression new_idx = ref_idx;
    for (auto loop : loops_to_substitute) {
      new_idx.substitute(loop, 0);
    }

    for (auto loop : feeder_loops) {
      if ((new_idx.get_coeff(loop) != 0) != 0) {
        CMarsVariable mv = CMarsVariable(loop, ast);
        // int i_coeff = new_idx.get_coeff(loop).get_const();
        int i_ub = mv.get_range().get_ub().get_const();

        reuse_num->push_back(i_ub + 1);
      }
    }
  }
}

void SystolicData::GetPEIterCoeff(void *ref_expr,
                                  const vector<void *> &outer_loops,
                                  const vector<void *> &feeder_loops,
                                  const vector<void *> &pe_loops,
                                  FeederLoops *Feeder) {
  CSageCodeGen *ast = m_ast;
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  vector<void *> loops_to_substitute;

  loops_to_substitute.insert(loops_to_substitute.begin(), outer_loops.begin(),
                             outer_loops.end());
  loops_to_substitute.insert(loops_to_substitute.begin(), feeder_loops.begin(),
                             feeder_loops.end());

  int dim = 0;
  for (auto ref_idx : vec_idx) {
    CMarsExpression new_idx = ref_idx;
    for (auto loop : loops_to_substitute) {
      new_idx.substitute(loop, 0);
    }

    CMarsExpression coeff = new_idx.get_coeff(pe_loops[Feeder->pe_iter_idx]);

    if (dim++ == Feeder->pe_iter_dim) {
      Feeder->pe_iter_coeff = coeff.get_const();
      break;
    }
  }
}

void *SystolicData::GetExpPEArrayOperand(void *loop, void *ref_expr_l,
                                         void *ref_expr_r) {
  CSageCodeGen *ast = m_ast;

  CMarsAccess ma_l(ref_expr_l, ast, nullptr);
  auto vec_idx_l = ma_l.GetIndexes();

  for (auto ref_idx : vec_idx_l) {
    CMarsExpression coeff = ref_idx.get_coeff(loop);

    if ((coeff != 0) != 0) {
      return ref_expr_l;
    }
  }

  CMarsAccess ma_r(ref_expr_r, ast, nullptr);
  auto vec_idx_r = ma_r.GetIndexes();

  for (auto ref_idx : vec_idx_r) {
    CMarsExpression coeff = ref_idx.get_coeff(loop);

    if ((coeff != 0) != 0) {
      return ref_expr_r;
    }
  }

  return nullptr;
}

int SystolicData::GetCoalescedWordsNum(void *ref_expr,
                                       vector<int> *v_num_coalesced_words) {
  CSageCodeGen *ast = m_ast;
  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  int num_coalesced_words = 1;

  for (auto ref_idx : vec_idx) {
    int ub = ref_idx.get_range().get_ub().get_const();

    v_num_coalesced_words->push_back(ub + 1);
    num_coalesced_words *= (ub + 1);
  }

  // FIXME: return vector size of PE_COL dimension
  return num_coalesced_words / PE.loop_bounds[1];
}

void SystolicData::GetExprUB(const vector<int> &v_block_dim_h,
                             const vector<int> &v_block_size_h,
                             const vector<int> &v_block_dim_v,
                             const vector<int> &v_block_size_v,
                             const vector<int> &v_block_dim_o,
                             const vector<int> &v_block_size_o) {
#if (SIMPLE_DELINEARIZE == 1)
  int expr_ub_h = 1;
  for (size_t i = 0; i < v_block_dim_h.size(); i++) {
    expr_ub_h *= v_block_dim_h[i];
  }
  for (size_t i = 0; i < v_block_size_h.size(); i++) {
    expr_ub_h *= v_block_size_h[i];
  }
  m_expr_ub[IN] = expr_ub_h;

  int expr_ub_v = 1;
  for (size_t i = 0; i < v_block_dim_v.size(); i++) {
    expr_ub_v *= v_block_dim_v[i];
  }
  for (size_t i = 0; i < v_block_size_v.size(); i++) {
    expr_ub_v *= v_block_size_v[i];
  }
  m_expr_ub[WT] = expr_ub_v;

  int expr_ub_o = 1;
  for (size_t i = 0; i < v_block_dim_o.size(); i++) {
    expr_ub_o *= v_block_dim_o[i];
  }
  for (size_t i = 0; i < v_block_size_o.size(); i++) {
    expr_ub_o *= v_block_size_o[i];
  }
  m_expr_ub[OUT] = expr_ub_o;
#else
  int expr_ub_h = 1;
  for (size_t i = 0; i < v_block_dim_h.size(); i++) {
    expr_ub_h *= v_block_dim_h[i] * v_block_size_h[i];
  }
  m_expr_ub[IN] = expr_ub_h;

  int expr_ub_v = 1;
  for (size_t i = 0; i < v_block_dim_v.size(); i++) {
    expr_ub_v *= v_block_dim_v[i] * v_block_size_v[i];
  }
  m_expr_ub[WT] = expr_ub_v;

  int expr_ub_o = 1;
  for (size_t i = 0; i < v_block_dim_o.size(); i++) {
    expr_ub_o *= v_block_dim_o[i] * v_block_size_o[i];
  }
  m_expr_ub[OUT] = expr_ub_o;
#endif
}

void *SystolicData::GetInitializedNameFromPntrArrRefExp(void *_exp) {
  if (m_ast->IsExpression(_exp) == 0) {
    return nullptr;
  }

  void *arr = _exp;

  while (true) {
    if (m_ast->IsPntrArrRefExp(arr) != 0) {
      arr = m_ast->GetExpLeftOperand(arr);
    } else if (m_ast->IsPointerDerefExp(arr) != 0) {
      arr = m_ast->GetExpUniOperand(arr);
    } else {
      break;
    }
  }

  if (m_ast->IsVarRefExp(arr) == 0) {
    return nullptr;
  }
  return m_ast->GetVariableInitializedName(arr);
}

void *SystolicData::GetMultiDimTypeByString(string typeName) {
  if (typeName.find("*") != string::npos) {
    return m_ast->GetTypeByString(typeName);
  }

  if (typeName.find("::") == 0) {
    typeName = typeName.substr(2);
  }
  size_t j = 0;
  string sPrefix = get_sub_delimited(typeName, j, ' ');
  j += sPrefix.size() + 1;
  string sSuffix;

  size_t j0 = j;
  size_t j1 = j;

  while (j < typeName.size()) {
    j1 = j;
    sSuffix = get_sub_delimited(typeName, j, ' ');
    j += sSuffix.size() + 1;
  }

  if (sSuffix[0] == '[') {
    string sSubType = typeName.substr(0, j1 - 1);

    string sSize = sSuffix.substr(sSuffix.find_first_of('[') + 1,
                                  sSuffix.find_first_of(']'));
    sSuffix = sSuffix.substr(sSuffix.find_first_of(']') + 1);

    if (sSuffix[0] == '[') {
      sSubType = sSubType + " " + sSuffix;
      return SageBuilder::buildArrayType(
          isSgType(static_cast<SgNode *>(GetMultiDimTypeByString(sSubType))),
          SageBuilder::buildUnsignedLongVal(atoi(sSize.c_str())));
    }
    return SageBuilder::buildArrayType(
        isSgType(static_cast<SgNode *>(m_ast->GetTypeByString(sSubType))),
        SageBuilder::buildUnsignedLongVal(atoi(sSize.c_str())));
  }
  if (sPrefix == "const") {
    string sSubType = typeName.substr(j0);
    return SageBuilder::buildConstType(
        isSgType(static_cast<SgNode *>(m_ast->GetTypeByString(sSubType))));
  }
  return nullptr;
}

int SystolicData::GetBinaryOperation(void *_exp) {
  SgBinaryOp *bin = isSgBinaryOp(static_cast<SgNode *>(_exp));
  if (bin == nullptr) {
    return 0;
  }
  return bin->variantT();
}

int SystolicData::GetConstantFromExp(void *_exp) {
  if (isSgIntVal(static_cast<SgNode *>(_exp)) != nullptr) {
    return isSgIntVal(static_cast<SgNode *>(_exp))->get_value();
  }

  CMarsExpression val(_exp, m_ast);
  if (val.IsConstant() == 0) {
    return 0;
  }
  return val.GetConstant();
}

vector<int> SystolicData::GetListFromAggInitializer(void *_exp) {
  vector<int> ret;

  if (m_ast->IsPntrArrRefExp(_exp) == 0) {
    return ret;
  }

  void *initName =
      m_ast->GetVariableInitializedName(m_ast->GetExpLeftOperand(_exp));

  SgAggregateInitializer *initer = isSgAggregateInitializer(
      static_cast<SgNode *>(m_ast->GetInitializer(initName)));
  assert(initer);

  SgExpressionPtrList &exps = initer->get_initializers()->get_expressions();
  for (size_t i = 0; i < exps.size(); i++) {
    SgAssignInitializer *assigner = isSgAssignInitializer(exps[i]);
    int value = GetConstantFromExp(assigner->get_operand_i());
    ret.push_back(value);
  }

  return ret;
}

void SystolicData::GetLoopBounds(vector<int> *loop_bounds,
                                 const vector<void *> &loops) {
  for (auto curr_loop : loops) {
    // fetch the upper bound of loop
    void *loop_test_expr =
        m_ast->GetExprFromStmt(m_ast->GetLoopTest(curr_loop));
    void *rh_op = m_ast->GetExpRightOperand(loop_test_expr);
    int tc = GetConstantFromExp(rh_op);

    loop_bounds->push_back(tc);
  }
}

void *SystolicData::CreateSimpleForLoop(string iterName, void *bound,
                                        void *body, void *scope) {
  // NOTE: loop init and test are statemetns while loop increment is expression.
  void *iter =
      m_ast->CreateVariableDecl("int", iterName, m_ast->CreateConst(0), scope);

  void *test = m_ast->CreateStmt(
      V_SgExprStatement,
      m_ast->CreateExp(V_SgLessThanOp, m_ast->CreateVariableRef(iter),
                       m_ast->CopyExp(bound)));

  void *inc = m_ast->CreateExp(V_SgPlusPlusOp, m_ast->CreateVariableRef(iter));

  return m_ast->CreateForLoop(iter, test, inc, body);
}

// Map loop iterator to trip count bound expression
// Assume the loop is a perfect loop, which has
// 1) zero initial value, and
// 2) simple less then test

map<void *, vector<int>> SystolicData::GetLoopTripCounts(void *loop) {
  map<void *, vector<int>> mapLoop2TCs;

  // xuechao: 10/31/2018
  // handle BasicBlock and ForStatement
  /*
  do {
      void *iter = m_ast->GetLoopIterator(loop);

      // Fetch the upper bound from loop test
      void *tc = m_ast->GetExprFromStmt(m_ast->GetLoopTest(loop));
      assert(m_ast->IsBinaryOp(tc));
      tc = m_ast->GetExpRightOperand(tc);

      vector<int> vecTCs;
      int value = GetConstantFromExp(tc);
      if (value)
          vecTCs.push_back(value);
      else
          vecTCs = GetListFromAggInitializer(tc);
      mapLoop2TCs[iter] = vecTCs;

      loop = m_ast->GetLoopBody(loop);
  } while (loop && m_ast->IsForStatement(loop));
  */

  while ((m_ast->IsBasicBlock(loop) != 0) ||
         (m_ast->IsForStatement(loop) != 0)) {
    if (m_ast->IsBasicBlock(loop) != 0) {
      loop = m_ast->GetChildStmt(loop, 0);
    }

    if (m_ast->IsForStatement(loop) != 0) {
      void *iter = m_ast->GetLoopIterator(loop);

      // Fetch the upper bound from loop test
      void *tc = m_ast->GetExprFromStmt(m_ast->GetLoopTest(loop));
      assert(m_ast->IsBinaryOp(tc));
      tc = m_ast->GetExpRightOperand(tc);

      vector<int> vecTCs;
      int value = GetConstantFromExp(tc);
      if (value != 0) {
        vecTCs.push_back(value);
      } else {
        vecTCs = GetListFromAggInitializer(tc);
      }
      mapLoop2TCs[iter] = vecTCs;

      loop = m_ast->GetLoopBody(loop);
    }
  }

  return mapLoop2TCs;
}

void *SystolicData::GetFirstStatementFromNestedLoop(void *loop) {
  // xuechao: 10/31/2018
  // handle BasicBlock and ForStatement
  /*
  do {
      loop = m_ast->GetLoopBody(loop);
      std::cout << "loop=" << m_ast->_p(loop) << std::endl;
  } while (loop && m_ast->IsForStatement(loop));

  return m_ast->GetChildStmt(loop, 0);
  */
  void *body = m_ast->GetLoopBody(loop);
  while ((m_ast->IsForStatement(body) != 0) ||
         (m_ast->IsBasicBlock(body) != 0)) {
    if (m_ast->IsForStatement(body) != 0) {
      body = m_ast->GetLoopBody(body);
    } else if (m_ast->IsBasicBlock(body) != 0) {
      body = m_ast->GetChildStmt(body, 0);
    }
  }

  return body;
}

void SystolicData::GetLoopItersFromExp(void *exp, set<void *> *setIters) {
  if ((exp == nullptr) || (m_ast->IsExpression(exp) == 0)) {
    return;
  }
  if (m_ast->IsVarRefExp(exp) != 0) {
    setIters->insert(m_ast->GetVariableInitializedName(exp));
  } else if (m_ast->IsBinaryOp(exp) != 0) {
    void *lhs;
    void *rhs;
    m_ast->GetExpOperand(exp, &lhs, &rhs);
    GetLoopItersFromExp(lhs, setIters);
    GetLoopItersFromExp(rhs, setIters);
  }
}

void SystolicData::GetLoopMainItersFromExp(void *exp, set<void *> *setIters) {
  if ((exp == nullptr) || (m_ast->IsExpression(exp) == 0)) {
    return;
  }
  if (m_ast->IsVarRefExp(exp) != 0) {
    setIters->insert(m_ast->GetVariableInitializedName(exp));
  } else if (m_ast->IsBinaryOp(exp) != 0) {
    void *lhs;
    void *rhs;
    m_ast->GetExpOperand(exp, &lhs, &rhs);

    if (GetBinaryOperation(exp) == V_SgAddOp &&
        (m_ast->IsVarRefExp(lhs) != 0) && (m_ast->IsVarRefExp(rhs) != 0)) {
      // For the expression that adding two iterators (e.g. r+p),
      // we always take the first one (e.g. r). FIXME
      GetLoopMainItersFromExp(lhs, setIters);

      /*
      // For the expression that adding two iterators, we only care the one
      // with larger trip count

      // Get range of the first loop
      CMarsExpression lb, ub, len;
      int64_t lLen, rLen;
      void *lloop = m_ast->GetLoopFromIteratorRef(lhs);
      CMarsRangeExpr lmr = CMarsVariable(lloop, m_ast).get_range();
      int err = lmr.get_simple_bound(lb, ub);
      assert(err);
      len = ub - lb + 1;
      if (!len.IsConstant()) {
      cerr << MSG_ERROR << "Unbounded inner loop is not supported." << endl;
      assert(0);
      }
      lLen = len.GetConstant();

      // Get range of the second loop
      void *rloop = m_ast->GetLoopFromIteratorRef(rhs);
      CMarsRangeExpr rmr = CMarsVariable(rloop, m_ast).get_range();
      err = rmr.get_simple_bound(lb, ub);
      assert(err);
      len = ub - lb + 1;
      if (!len.IsConstant()) {
      cerr << MSG_ERROR << "Unbounded inner loop is not supported." << endl;
      assert(0);
      }
      rLen = len.GetConstant();

#ifdef MSG_FINE
cerr << MSG_FINE;
cerr << "Compare " << m_ast->UnparseToString(lhs) << "(" << lLen << ") and ";
cerr << m_ast->UnparseToString(rhs) << "(" << rLen << ")" << endl;
#endif

if (lLen >= rLen)
GetLoopMainItersFromExp(lhs, setIters);
else
GetLoopMainItersFromExp(rhs, setIters);
*/
    } else {
      GetLoopMainItersFromExp(lhs, setIters);
      GetLoopMainItersFromExp(rhs, setIters);
    }
  }
}

void SystolicData::Analyze() {
  void *top_loop = m_ast->GetNextStmt(m_pragma);

  while (top_loop != nullptr) {
    if (m_ast->IsForStatement(top_loop) == 0) {
      top_loop = m_ast->GetChildStmt(top_loop, 0);

      continue;
    }
    break;
  }

  if (top_loop == nullptr) {
    return;
  }

  std::cout << "top_loop=" << m_ast->_p(top_loop) << std::endl;
  void *body = m_ast->GetLoopBody(top_loop);
  std::cout << "loop_body=" << m_ast->_p(body) << std::endl;
  if (m_ast->IsForStatement(top_loop) == 0) {
    return;
  }

  // TODO(youxiang): Remove the function argument for low level model
  sysloop = top_loop;
  layerLoop = m_ast->TraceUpToForStatement(m_ast->GetParent(top_loop));
  if (model == HIGH_LEVEL) {
    AnalyzeHighLevelModel();
    // [xuechao - 01/01/2019] Code generation for golden kernels
    CodegenGolden();
  } else {
    AnalyzeLowLevelModel(top_loop);
    // AutoCodegen();
  }
}

void SystolicData::AnalyzeHighLevelModel() {
  void *sysStmt = nullptr;
  void *sysExp = nullptr;
  CSageCodeGen &codegen = *m_ast;

  // Map loop iterator to trip count (expression)
  mapLoop2TCs = GetLoopTripCounts(GetSysLoop());

  for (auto iter : mapLoop2TCs) {
    setLoopIters.insert(iter.first);
  }

  /*
   * xuechao: 04/17/2019
   * add tag for loop iterators for grouping reduction and
   * 0: non-reduction variable
   * 1: reduction variable
   * 2: pe_simd variable
   * default: reduction variable
   * */
  for (auto iter : setLoopIters) {
    mapIter2Tag.insert(pair<void *, bool>(iter, 1));
  }

#ifdef MSG_FINE
  cerr << MSG_FINE;
  cerr << "Loop to trip count:" << endl;
  for (auto iter : mapLoop2TCs) {
    cerr << "\t" << codegen._up(iter.first) << " -> ";
    for (auto tc : iter.second) {
      cerr << tc << " ";
      cerr << endl;
    }
  }
#endif

  // Check legalization and retrieve information
  sysStmt = GetFirstStatementFromNestedLoop(GetSysLoop());
  assert(sysStmt);
  //    std::cout << "sysStmt=" << codegen._p(sysStmt);
  sysExp = codegen.GetExprFromStmt(sysStmt);
  assert(sysExp);

  // Check and extract the reduction operation
  if (codegen.IsAssignOp(sysExp) != 0) {
    int reductionOp = GetBinaryOperation(sysExp);
    SetReductionOp(reductionOp);

    void *l = codegen.GetExpLeftOperand(sysExp);

    // Update expression to have a unified format: out op1 w op2 in
    sysExp = codegen.GetExpRightOperand(sysExp);
    assert(sysExp);

    void *r = codegen.GetExpLeftOperand(sysExp);
    if ((l == nullptr) || (r == nullptr) ||
        (codegen.GetVariableDecl(l) != codegen.GetVariableDecl(r))) {
      cerr << MSG_ERROR << "Cannot find reduction pattern" << endl;
      return;
    }
  } else if (codegen.IsCompoundAssignOp(sysExp) != 0) {
    int reductionOp = codegen.GetBinaryOperationFromCompoundAssignOp(sysExp);
    SetReductionOp(reductionOp);
  } else {
    cerr << MSG_ERROR << "Cannot find reduction pattern" << endl;
    return;
  }

  // Extract the map operation
  void *subSysExp = codegen.GetExpRightOperand(sysExp);
  assert(codegen.IsBinaryOp(subSysExp));
  int mapOp = GetBinaryOperation(subSysExp);
  SetMapOp(mapOp);

  // Extract I/O
  SetExp(SystolicData::OUT, codegen.GetExpLeftOperand(sysExp));
  void *rhs = codegen.GetExpRightOperand(sysExp);
  if ((codegen.GetExpLeftOperand(rhs) == nullptr) ||
      (codegen.IsPntrArrRefExp(codegen.GetExpLeftOperand(rhs)) == 0)) {
    cerr << MSG_ERROR;
    cerr << "The statement is not legal for mapping to systolic array." << endl;
    cerr << "The weight is either null or not a pointer array ";
    cerr << "reference expression." << endl;
    return;
  }
  SetExp(SystolicData::WT, codegen.GetExpLeftOperand(rhs));

  if ((codegen.GetExpRightOperand(rhs) == nullptr) ||
      (codegen.IsPntrArrRefExp(codegen.GetExpRightOperand(rhs)) == 0)) {
    cerr << MSG_ERROR;
    cerr << "The statement is not legal for mapping to systolic array." << endl;
    cerr << "The input is either null or not a pointer array ";
    cerr << "reference expression." << endl;
    return;
  }
  SetExp(SystolicData::IN, codegen.GetExpRightOperand(rhs));

  // Analyze parallelizable loops
  set<void *> tmpSetOut;
  set<void *> tmpSet;
  for (auto exp : GetIdxExps(SystolicData::OUT)) {
    GetLoopItersFromExp(exp, &tmpSetOut);
  }

  /*
   * 0: non-reduction variable
   * */
  for (auto iter : tmpSetOut) {
    mapIter2Tag[iter] = 0;
  }

  /*
   * 2: pe_simd variable
   * */
  for (auto exp : GetIdxExps(SystolicData::WT)) {
    std::cout << codegen._up(exp) << std::endl;
    CMarsExpression me(exp, m_ast);
    std::cout << me.print() << std::endl;
    std::cout << me.get_range().get_const_ub() << std::endl;

    for (auto iter : mapLoop2TCs) {
      if ((mapIter2Tag[iter.first] == 1) &&
          (codegen._up(iter.first) == codegen._up(exp))) {
        std::cout << codegen._up(iter.first) << std::endl;
        mapIter2Tag[iter.first] = 2;
      }
    }
  }

  std::cout << __func__ << ":" << __LINE__ << std::endl;
  for (auto iter : mapIter2Tag) {
    std::cout << "iter=" << iter.first << " ";
    std::cout << "iter=" << codegen._up(iter.first) << " "
              << "tag=" << iter.second << std::endl;
  }

  SetRelatedLoop(SystolicData::OUT, tmpSetOut);
  tmpSet.clear();

  set<void *> tmpSetW;
  for (auto exp : GetIdxExps(SystolicData::WT)) {
    GetLoopItersFromExp(exp, &tmpSetW);
  }
  SetRelatedLoop(SystolicData::WT, tmpSetW);
  tmpSet.clear();

  set<void *> tmpSetIn;
  for (auto exp : GetIdxExps(SystolicData::IN)) {
    GetLoopMainItersFromExp(exp, &tmpSetIn);
  }

  SetRelatedLoop(SystolicData::IN, tmpSetIn);
  tmpSet.clear();
}

void *SystolicData::CreateNestedLoopExpr(vector<void *> vec_loop, void *scope,
                                         map<void *, void *> &loop_iter_map) {
  void *parent_loop = nullptr;
  //    void *scope = m_ast->GetFuncBody(func);
  for (auto &loop : vec_loop) {
    void *iter_old = m_ast->GetLoopIterator(loop);
    CMarsVariable mv = CMarsVariable(loop, m_ast);

    //    DEBUG_CODEGEN_GOLDEN(mv.print_var());

    //    DEBUG_CODEGEN_GOLDEN(m_ast->_up(iter));

    // Fetch the upper bound from loop test
    void *tc = m_ast->GetExprFromStmt(m_ast->GetLoopTest(loop));
    assert(m_ast->IsBinaryOp(tc));
    tc = m_ast->GetExpRightOperand(tc);

    //    DEBUG_CODEGEN_GOLDEN(m_ast->_up(tc));

    //    void *gen_loop = CreateSimpleForLoop(mv.print_var(), tc,
    //            m_ast->CreateBasicBlock(), m_ast->GetScope(func));
    void *gen_loop = CreateSimpleForLoop(mv.print_var(), tc,
                                         m_ast->CreateBasicBlock(), scope);
    //    void *gen_loop = CreateSimpleForLoop(mv.print_var()+"XXX", tc,
    //            m_ast->CreateBasicBlock(), scope);

    void *iter_new = m_ast->GetLoopIterator(gen_loop);
    loop_iter_map.insert(std::make_pair(iter_old, iter_new));
    // FIXME: how to print gen_loop?
    //    DEBUG_CODEGEN_GOLDEN(printNodeInfo(m_ast, gen_loop));

    //    if (parent_loop)
    //    {
    //    //    m_ast->ReplaceChild(m_ast->GetLoopBody(parent_loop), gen_loop);
    //        m_ast->AppendChild(m_ast->GetLoopBody(parent_loop), gen_loop);
    //    }
    //    else
    //    {
    //        m_ast->AppendChild(m_ast->GetFuncBody(func), gen_loop);
    //    }
    m_ast->AppendChild(scope, gen_loop);

    parent_loop = gen_loop;
    scope = m_ast->GetLoopBody(parent_loop);
  }

  return parent_loop;
}

void *SystolicData::CreateLinearizedExpr(void *ref_expr) {
  void *ref_idx_expr = m_ast->CreateConst(0);

  CMarsAccess ma(ref_expr, m_ast, nullptr);
  std::vector<int> vec_idx_ubs;
  for (auto &ref_idx : ma.GetIndexes()) {
    CMarsExpression ma_idx = ref_idx;

    //    std::cout << ma_idx.get_range().get_const_ub() << std::endl;
    vec_idx_ubs.push_back(ma_idx.get_range().get_const_ub() + 1);
  }
  vec_idx_ubs.push_back(1);

  uint32_t i = 1;
  for (auto &ref_idx : ma.GetIndexes()) {
    //  DEBUG_CODEGEN_GOLDEN(ref_idx.print_s());

    // FIXME: how to get ast node type of ref_idx from CMarsExpression type?
    //    void *ast_ref_idx = ref_idx.get_pointer();
    void *ast_ref_idx = ref_idx.get_expr_from_coeff();

    // FIXME: how to transform ref_idx into a floating node?
    //    if (!m_ast->is_floating_node(ast_ref_idx))
    //    {
    //        ast_ref_idx = m_ast->CopyExp(ast_ref_idx);
    //    }
    for (size_t j = i; j < vec_idx_ubs.size(); j++) {
      ast_ref_idx = m_ast->CreateExp(V_SgMultiplyOp, ast_ref_idx,
                                     m_ast->CreateConst(vec_idx_ubs[j]));
    }

    ref_idx_expr = m_ast->CreateExp(V_SgAddOp, ref_idx_expr, ast_ref_idx);
    i++;
  }

  return ref_idx_expr;
}

void SystolicData::CodegenGolden() {
  const string KTypeName = "data_precision";

  m_ast->RegisterType(KTypeName);

  CSageCodeGen *ast = m_ast;
  void *newFile = ast->CreateSourceFile("golden.cpp");

  string sDirective = "#include \"__merlinhead_kernel_top.h\"";
  m_ast->AddHeader(sDirective, newFile);

  vector<void *> vecParams;

  vecParams.push_back(ast->CreateVariable((KTypeName + "*"), "out"));
  vecParams.push_back(ast->CreateVariable((KTypeName + "*"), "in"));
  vecParams.push_back(ast->CreateVariable((KTypeName + "*"), "weight"));

  void *func =
      ast->CreateFuncDecl("void", "conv", vecParams, newFile, true, nullptr);

  ast->AppendChild(newFile, func);

  // Step 1: Retrieve reduction expression
  void *sysStmt = nullptr;
  void *sysExp = nullptr;

  sysStmt = GetFirstStatementFromNestedLoop(GetSysLoop());
  assert(sysStmt);
  //    std::cout << "sysStmt=" << codegen._p(sysStmt);
  //    DEBUG_CODEGEN_GOLDEN(m_ast->_up(sysStmt));
  sysExp = m_ast->GetExprFromStmt(sysStmt);
  assert(sysExp);
  //    DEBUG_CODEGEN_GOLDEN(m_ast->_up(sysExp));

  //    // Check and extract the reduction operation
  //    if (m_ast->IsAssignOp(sysExp)) {
  //        int reductionOp = GetBinaryOperation(sysExp);
  //        SetReductionOp(reductionOp);
  //    }

  // Step 2: Generate out
  /*
   * Step 2.1: Generate loops
   * Algorithm: scan all the loops and collect
   * those belonging to out expression
   * */
  void *out_expr = m_ast->GetExpLeftOperand(sysExp);
  //    DEBUG_CODEGEN_GOLDEN(m_ast->_up(out_expr));
  CMarsAccess ma(out_expr, m_ast, nullptr);

  std::vector<void *> vec_out_loop;
  std::vector<void *> map_loop;  // for Step 3
  vector<CMarsExpression> new_vec_idx;

  void *loop = sysloop;
  //    while (m_ast->IsBasicBlock(loop) || m_ast->IsForStatement(loop))
  while (m_ast->IsForStatement(loop) != 0) {
    DEBUG_CODEGEN_GOLDEN(m_ast->_up(loop));

    int flag = 1;
    for (auto &ref_idx : ma.GetIndexes()) {
      CMarsExpression ma_idx = ref_idx;

      CMarsExpression coeff = ma_idx.get_coeff(loop);
      //    CMarsVariable mv = CMarsVariable(loop, m_ast);

      int i_coeff = coeff.get_const();

      if (0 != i_coeff) {
        vec_out_loop.push_back(loop);
        flag = 0;
      }
    }

    if (flag != 0) {
      map_loop.push_back(loop);
    }

    if (m_ast->IsForStatement(loop) != 0) {
      loop = m_ast->GetLoopBody(loop);
      if (m_ast->IsBasicBlock(loop) != 0) {
        loop = m_ast->GetChildStmt(loop, 0);
      }
    }
  }

  /*
   * "loop_iter_map" builds a map between loop iterators and
   * their copies in case they are gone due to some AST node replacemets.
   * For example, the "codegen.ReplaceStmt(...) to replace the loops with
   * a function call to "systolic_array_kernel" in input test case would
   * cause such scenario.
   * If this happens, all the expressions built on these loop iterators would
   * lose connection with the AST.
   * */
  map<void *, void *> reduce_loop_iter_map;

  void *parent_loop = CreateNestedLoopExpr(
      vec_out_loop, m_ast->GetFuncBody(func), reduce_loop_iter_map);

  /*
   * Step 2.2: Generate out index expression, say
   * "out_idx=i*N+j" given "C[i][j]"
   * Algorithm: linearization
   * */

  void *out_idx_expr = CreateLinearizedExpr(out_expr);

  for (auto &pair : reduce_loop_iter_map) {
    m_ast->replace_variable_references_in_scope(pair.first, pair.second,
                                                out_idx_expr);
  }
  void *out_idx = ast->CreateVariableDecl("int", "out_idx", out_idx_expr,
                                          m_ast->GetLoopBody(parent_loop));
  m_ast->AppendChild(ast->GetLoopBody(parent_loop), out_idx);

  /*
   * Step 2.3: Generate initialization code for out, e.g.,
   * C[out_idx] = 0
   * */
  void *assign_left_operand =
      m_ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[0]),
                       ast->CreateVariableRef(out_idx));
  void *assign_right_operand = ast->CreateConst(0);
  void *init_stmt = ast->CreateStmt(V_SgAssignStatement, assign_left_operand,
                                    assign_right_operand);
  ast->AppendChild(ast->GetLoopBody(parent_loop), init_stmt);

  // Step 3: Generate reduce loops
  /*
   * Step 3.1: Generate the loops
   * */
  map<void *, void *> map_loop_iter_map;
  void *parent_loop2 = CreateNestedLoopExpr(
      map_loop, m_ast->GetLoopBody(parent_loop), map_loop_iter_map);
  map<void *, void *> loop_iter_map = reduce_loop_iter_map;

  loop_iter_map.insert(map_loop_iter_map.begin(), map_loop_iter_map.end());

  /*
   * Step 3.2: Generate index expressions for reduction arrays
   * */
  void *op_map = m_ast->GetExpRightOperand(sysExp);
  void *in_expr = ast->GetExpLeftOperand(op_map);
  void *wt_expr = ast->GetExpRightOperand(op_map);

  void *in_idx_expr = CreateLinearizedExpr(in_expr);
  void *wt_idx_expr = CreateLinearizedExpr(wt_expr);

  for (auto &pair : loop_iter_map) {
    m_ast->replace_variable_references_in_scope(pair.first, pair.second,
                                                in_idx_expr);
    m_ast->replace_variable_references_in_scope(pair.first, pair.second,
                                                wt_idx_expr);
  }
  void *in_idx = ast->CreateVariableDecl("int", "in_idx", in_idx_expr,
                                         m_ast->GetLoopBody(parent_loop2));
  m_ast->AppendChild(ast->GetLoopBody(parent_loop2), in_idx);
  void *wt_idx = ast->CreateVariableDecl("int", "wt_idx", wt_idx_expr,
                                         m_ast->GetLoopBody(parent_loop2));
  m_ast->AppendChild(ast->GetLoopBody(parent_loop2), wt_idx);

  /*
   * Step 3.3: Generate the reduction expression
   * */
  void *map_left_operand =
      m_ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[1]),
                       ast->CreateVariableRef(in_idx));
  void *map_right_operand =
      m_ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[2]),
                       ast->CreateVariableRef(wt_idx));
  void *reduce_right_operand =
      m_ast->CreateExp(V_SgMultiplyOp, map_left_operand, map_right_operand);

  void *reduce_left_operand = m_ast->CreateExp(
      V_SgPntrArrRefExp, m_ast->CreateVariableRef(vecParams[0]),
      m_ast->CreateVariableRef(out_idx));

  void *comp_expr = m_ast->CreateExp(V_SgPlusAssignOp, reduce_left_operand,
                                     reduce_right_operand);
  void *comp_stmt = m_ast->CreateStmt(V_SgExprStatement, comp_expr);
  m_ast->AppendChild(m_ast->GetLoopBody(parent_loop2), comp_stmt);
}

string SystolicData::GetCoeffString(CMarsExpression new_idx,
                                    vector<void *> var_order) {
  string str;
  for (auto var : var_order) {
    CMarsExpression coeff = new_idx.get_coeff(var);
    CMarsExpression me_ub(var, m_ast);

    int i_ub = me_ub.get_range().get_ub().get_const();

    if (i_ub != 0) {
      str += my_itoa(coeff.get_const()) + ",";
    }
  }
  str += my_itoa(new_idx.get_const());

  // if (str != "" && str[str.length()-1] ==',') str = str.substr(0,
  // str.length()-1);

  return str;
}

string SystolicData::GetCoeffString(CMarsExpression new_idx,
                                    vector<void *> var_order,
                                    int num_new_dims) {
  string str;
  for (auto var : var_order) {
    CMarsExpression coeff = new_idx.get_coeff(var);
    if (num_new_dims > 1) {
      if (coeff.get_const() > 0) {
        str += "1,";
      } else {
        str += "0,";
      }
    } else {
      str += my_itoa(coeff.get_const()) + ",";
    }
  }
  str += my_itoa(new_idx.get_const());

  // if (str != "" && str[str.length()-1] ==',') str = str.substr(0,
  // str.length()-1);

  return str;
}

// FIXME: to support more operations
string SystolicData::GetOpString(void *op) {
  string type = m_ast->GetASTTypeString(op);
  if (type == "SgPlusAssignOp") {
    return "+";
  }
  if (type == "SgAddO") {
    return "+";
  }
  if (type == "SgMultiplyOp") {
    return "*";
  }
  return "";
}

string IntVectorToString(vector<int> v_int) {
  string ret;
  for (auto one : v_int) {
    ret += my_itoa(one) + ",";
  }
  ret = ret.substr(0, ret.length() - 1);
  return ret;
}

int SystolicData::AnalysisDelinearize(CMarsExpression *me,
                                      vector<size_t> *old_steps,
                                      vector<size_t> *new_dims,
                                      vector<size_t> *new_ubs) {
  typedef vector<vector<int64_t>> t_indepent_sections;

  cout << "==> expression: " << me->print() << endl;

  if (me->has_coeff_range_simple() == 0) {
    std::cout << "Not Simple" << std::endl;

    return 0;
  }

  t_indepent_sections sec_org = me->get_coeff_range_simple();
  t_indepent_sections sec_pre = sec_org;

  map<int, int> separate_pos;
  //    int curr_length = 0;
  CMarsExpression me_curr_dim(m_ast, 0);
  for (size_t i = 0; i < sec_org.size(); i++) {
    std::cout << "#sec_org=" << i << std::endl;

    vector<int64_t> one_term = sec_org[i];
    int64_t one_step = std::abs(one_term[0]);
    //    int one_length = one_term[2] - one_term[1];

    CMarsExpression me_var(
        CMarsVariable(m_ast, CMarsRangeExpr(one_term[1], one_term[2], m_ast)));

    std::cout << one_term[0] << " " << one_term[1] << " " << one_term[2]
              << std::endl;

    // check if the two sections can be merged
    // 1. next coeff >= current range
    // 2. the dim size can divide large coeffs
    int is_dividable = 1;
    {
      for (size_t j = i + 1; j < sec_org.size(); j++) {
        int64_t one_step_1 = std::abs(sec_org[j][0]);

        if ((one_step_1 % one_step) != 0) {
          is_dividable = 0;
          break;
        }
      }
    }

    CMarsRangeExpr mr(me_curr_dim.get_range());
    bool is_const_bound = mr.is_const_bound() != 0;
    int64_t n_lb = mr.get_const_lb();
    int64_t n_ub = mr.get_const_ub();

    //    if (one_step > curr_length && is_dividable && (
    //                /*altera_flow*/ 1 || (one_step >= 16 || one_step == 3 ||
    //                    one_step == 5)))
    if (one_step > 1 && is_const_bound && (is_dividable != 0) &&
        (div_floor(n_lb, one_step) == div_floor(n_ub, one_step))) {
      separate_pos[i] = 1;
    } else {
      separate_pos[i] = 0;
      me_curr_dim = CMarsExpression(m_ast, max_divide(n_lb, one_step));
    }

    me_curr_dim = me_curr_dim + me_var * one_term[0];
  }

  vector<vector<int64_t>> new_org;
  int curr_lb = 0;
  int curr_ub = 0;
  int curr_step = 1;
  vector<int64_t> term;
  for (size_t i = 0; i < sec_org.size(); i++) {
    vector<int64_t> one_term = sec_org[i];
    int one_step = one_term[0];
    int one_lb = one_term[1];
    int one_ub = one_term[2];
    if (separate_pos[i] == 1) {
      term.clear();
      term.push_back(curr_step);
      int rest_lb = curr_lb >= 0
                        ? (curr_lb % one_step)
                        : ((one_step - (-curr_lb % one_step)) % one_step);
      int rest_ub = curr_ub >= 0
                        ? (curr_ub % one_step)
                        : ((one_step - (-curr_ub % one_step)) % one_step);
      term.push_back(rest_lb);
      term.push_back(rest_ub);
      new_org.push_back(term);
      curr_step = one_step;
      curr_lb = one_lb + (curr_lb - rest_lb);
      curr_ub = one_ub + (curr_ub - rest_ub);
    } else {
      curr_lb += one_lb;
      curr_ub += one_ub;
    }
  }
  term.clear();
  term.push_back(curr_step);
  term.push_back(curr_lb);
  term.push_back(curr_ub);
  new_org.push_back(term);

  //    vec_old_dim.push_back(new_org);

  vector<int64_t> separate_factors;
  {
    // int is_same_separation = 1;
    //    for (size_t i = 0; i < vec_old_dim.size(); i++)
    {
      //    vector<vector<int64_t> > one_access = vec_old_dim[i];
      vector<vector<int64_t>> one_access = new_org;
      std::cout << "one_access.size()=" << one_access.size() << std::endl;
      for (size_t j = 0; j < one_access.size(); j++) {
        vector<int64_t> term = one_access[j];
        int64_t one_step = term[0];
        // int64_t one_lb = term[1];
        int64_t one_ub = term[2];

        new_ubs->push_back(one_ub);

        if (/*i == 0 || */ j >= separate_factors.size()) {
          separate_factors.push_back(one_step);
        } else {
          if (one_step != separate_factors[j]) {
            // is_same_separation = 0;
            break;
          }
        }
      }
      //    if (!is_same_separation)
      //        break;
    }
  }

  old_steps->clear();
  new_dims->clear();
  for (auto f : separate_factors) {
    std::cout << "separate_factor=" << f << std::endl;
    old_steps->push_back(f);
    new_dims->push_back(f);
  }

  return separate_factors.size();
}

void SystolicData::SimpleDelinearization(
    const CMarsExpression &me, vector<CMarsExpression> *vec_new_term) {
  //    typedef vector<vector<int64_t>> t_indepent_sections;

  std::cout << "==> function: " << __func__ << endl;

  cout << "==> expression: " << me.print() << endl;

  /*
   * a simple test on sorting on map by value
  map<int, int> m = {{1, 10}, {2, 5}, {4, 6}, {6, 6}};
  using mypair = pair<int, int>;

  vector<mypair> v(begin(m), end(m));

  sort(begin(v), end(v), [](const mypair& a, const mypair& b)
          { return a.second < b.second; });

  for(auto const &p : v)
          cout << "m[" << p.first << "] = " << p.second << endl;
  */

  //    map<CMarsVariable, int> m_mv2coeff;
  map<void *, int> m_mv2coeff;
  vector<void *> vars;
  me.get_vars(&vars);

  for (auto var : vars) {
    //    CMarsVariable mv = CMarsVariable(var, m_ast);
    //    CMarsExpression coeff = me.get_coeff(var);
    int i_coeff = me.get_coeff(var).GetConstant();
    //    m_mv2coeff.insert(std::pair<CMarsVariable, int>(mv, i_coeff));
    //    m_mv2coeff.insert(std::pair<void *, int>(mv, i_coeff));
    m_mv2coeff.insert(std::pair<void *, int>(var, i_coeff));
  }
  //    using mypair = pair<CMarsVariable, int>;
  using mypair = pair<void *, int>;
  vector<mypair> v_term(begin(m_mv2coeff), end(m_mv2coeff));

  sort(begin(v_term), end(v_term),
       [](const mypair &a, const mypair &b) { return a.second < b.second; });

  //    for(auto const &p : v)
  //            cout << "m[" << (p.first).print() << "] = " << p.second << endl;

#if 0
    CMarsExpression me_new_term(me.get_ast(), 0);
    for (auto var : vars) {
        CMarsVariable mv = CMarsVariable(var, m_ast);
    //    std::cout << "lb=" << mv.get_range().get_const_lb() << std::endl;
    //    std::cout << "ub=" << mv.get_range().get_const_ub() << std::endl;
    //    std::cout << "var=" << m_ast->_p(var) << std::endl;
        CMarsExpression coeff = me.get_coeff(var);
    //    std::cout << "coeff=" << coeff.print(0) << std::endl;

        int n_coeff = me.get_coeff(var).GetConstant();
        me_new_term = me_new_term +
            CMarsExpression(var, m_ast, me.get_pos()) *
            n_coeff;
    }
//    std::cout << me_new_term.print(0) << std::endl;
#endif

  int num_term = v_term.size();

  std::cout << "num_term=" << num_term << std::endl;

  for (int p = 0; p < num_term;) {
    CMarsExpression me_new_coeff_term(me.get_ast(), 0);
    CMarsExpression me_new_term(me.get_ast(), 0);

    void *range_var = v_term[p].first;
    int range_coeff = v_term[p].second;

    me_new_coeff_term =
        me_new_term +
        CMarsExpression(range_var, m_ast, me.get_pos()) * range_coeff;
    me_new_term =
        me_new_term + CMarsExpression(range_var, m_ast, me.get_pos()) *
                          (range_coeff / range_coeff);

    if (p == (num_term - 1)) {
      p++;
      vec_new_term->push_back(me_new_term);
      break;
    }

    for (int q = p + 1; q < num_term; q++) {
      void *step_var = v_term[q].first;
      int step_coeff = v_term[q].second;

      int curr_step = step_coeff;
      //    int curr_range = me_new_term.get_range().get_ub().get_const() + 1;
      int64_t curr_range =
          me_new_coeff_term.get_range().get_ub().get_const() + 1;

      std::cout << "(p,q)="
                << "(" << p << "," << q << ")" << std::endl;

      std::cout << "me_new_coeff_term=" << me_new_coeff_term.print(0)
                << std::endl;
      std::cout << "me_new_term=" << me_new_term.print(0) << std::endl;
      //    std::cout << "range_coeff=" << range_coeff << std::endl;
      std::cout << "curr_step=" << curr_step << std::endl;
      std::cout << "curr_range=" << curr_range << std::endl;

      if (curr_step < curr_range) {
        // merge two sections and update step pointer
        me_new_coeff_term =
            me_new_coeff_term +
            CMarsExpression(step_var, m_ast, me.get_pos()) * step_coeff;
        me_new_term =
            me_new_term + CMarsExpression(step_var, m_ast, me.get_pos()) *
                              (step_coeff / range_coeff);
        if (q >= (num_term - 1)) {
          vec_new_term->push_back(me_new_term);
          p = q + 1;
          break;  // restart for the new range pointer
        }
      } else {
        // apply delinearization and update range pointer
        vec_new_term->push_back(me_new_term);
        p = q;
        break;  // restart for the new range pointer
      }
    }
  }

  std::reverse(vec_new_term->begin(), vec_new_term->end());

  //    for (auto term : *vec_new_term)
  //    {
  //        std::cout << term.print(0) << std::endl;
  //    }
}

int SystolicData::ApplyDelinearize(CMarsExpression *me,
                                   const vector<size_t> &old_steps,
                                   const vector<size_t> &new_dims,
                                   vector<CMarsExpression> *vec_new_indexes) {
  CSageCodeGen *ast = m_ast;

  int dim_num = old_steps.size();

  for (size_t dim = 0; dim < old_steps.size(); dim++) {
    int factor = old_steps[dim_num - 1 - dim];
    //    CMarsExpression remainder(0, this);
    CMarsExpression remainder(ast, 0);

    vector<void *> vars;
    me->get_vars(&vars);
    vector<void *> matched_vars;
    for (size_t j = 0; j < vars.size(); j++) {
      // 1. Either coeff dividable
      CMarsExpression coeff = me->get_coeff(vars[j]);
      if (coeff.GetConstant() % factor == 0) {
        matched_vars.push_back(vars[j]);
      }
    }
    CMarsRangeExpr mr_reminder = remainder.get_range();

    CMarsExpression me_new_term(me->get_ast(), 0);
    for (size_t j = 0; j < matched_vars.size(); j++) {
      int64_t n_coeff = me->get_coeff(matched_vars[j]).GetConstant() / factor;

      //    me_new_term = me_new_term +
      //        CMarsExpression(matched_vars[j], this, me->get_pos()) *
      //        CMarsExpression(n_coeff, this);
      me_new_term =
          me_new_term +
          CMarsExpression(matched_vars[j], ast, me->get_pos()) * n_coeff;

      //    me->set_coeff(matched_vars[j], CMarsExpression(this, 0));
      me->set_coeff(matched_vars[j], CMarsExpression(ast, 0));
    }

    if (factor == 1) {
      //    me_new_term = me_new_term + CMarsExpression(me->get_const(), this);
      me_new_term = me_new_term + me->get_const();
      me->set_const(0);
    } else {
      //    me_new_term = me_new_term + CMarsExpression(me->get_const() /
      //    factor,
      // this);
      me_new_term = me_new_term + me->get_const() / factor;
      me->set_const(me->get_const() % factor);
    }
    while (me->IsNonNegative() == 0) {
      me = me + factor;
      me_new_term = me_new_term - 1;
    }

    vec_new_indexes->push_back(me_new_term);

    //    std::cout << __FUNCTION__ <<  ":" << me_new_term.print() << std::endl;
  }
  return 0;
}

void SystolicData::SystolicTest(void *expr_ref,
                                const vector<void *> &outer_loops,
                                const vector<void *> &pe_loops) {
  CSageCodeGen *ast = m_ast;

  //    std::cout << ast->_p(ast->_pa(expr_ref)) << std::endl;
  std::cout << ast->_p(expr_ref) << std::endl;

  // delinearization test

#if 0
    vector<void *> vec_ref;
    ast->GetNodesByType(expr_ref, "preorder", "SgVarRefExp", &vec_ref);

    std::cout << __FUNCTION__ << std::endl;
    std::cout << vec_ref.size() << std::endl;

    for (size_t i = 0; i < vec_ref.size(); i++) {
        std::cout << "i=" << i << std::endl;

        void *sg_ref = vec_ref[i];
        void *sg_array = ast->GetVariableInitializedName(sg_ref);
        std::cout << "  ---- " << ast->_p(sg_array) << std::endl;

        if (ast->IsPointerType(ast->GetTypebyVar(sg_array))) {
            int dim_size = ast->get_dim_num_from_var(sg_array);

            std::cout << dim_size << std::endl;
            for (int dimIdx = dim_size - 1; dimIdx >= 0; dimIdx--) {
                vector<size_t> old_steps;
                vector<size_t> new_dims;

                CMarsExpression offset(0, ast);
                bool is_simple = true;
                ast->analysis_delinearize(sg_array, dimIdx, old_steps,
                                          new_dims, 1);
            //  ast->analysis_delinearize(sg_array, dimIdx, old_steps, new_dims,
                                          1, is_simple, &offset, expr_ref);
                ast->apply_delinearize(sg_array, dimIdx, old_steps, new_dims);
            //  ast->apply_delinearize(sg_array, dimIdx, old_steps, new_dims,
                                       false, is_simple, &offset, expr_ref);

                string s_new_dims;
                {
                    for (auto t : old_steps) {
                        s_new_dims += "," + my_itoa(t);
                    }
                    if (old_steps.size()) s_new_dims[0] = ' ';
                    int pos = s_new_dims.size();
                    for (auto t : new_dims) {
                        s_new_dims += "," + my_itoa(t);
                    }
                    if (new_dims.size()) s_new_dims[pos] = ':';
                    s_new_dims += " ";
                }
                std::cout << "  ---- " << ast->_p(sg_array) << ": "
                    <<  " dim=" << my_itoa(dimIdx) << " (" << s_new_dims
                    << ") " << std::endl;
            }
        }
    }
#endif

#define BLACK_BOX 0

#if BLACK_BOX == 0

  void *sg_ref = expr_ref;
  //    void *sg_array = ast->GetVariableInitializedName(sg_ref);
  void *sg_array = GetInitializedNameFromPntrArrRefExp(sg_ref);
  std::cout << "  ---- " << ast->_p(sg_array) << std::endl;

  for (int dim = 0; dim < 3; dim++) {
    std::cout << "dim=" << dim << std::endl;
    int type_dim;  // = get_pntr_type_dim(sg_pntr);
    {
      if (ast->IsInitName(sg_array) == 0) {
        //    return 0;
        std::cout << "sg_array is not init name" << std::endl;
      }

      void *tt_type = ast->GetTypebyVar(sg_array);
      void *tt_base_type;
      vector<size_t> tt_size;
      ast->get_type_dimension(tt_type, &tt_base_type, &tt_size, sg_array);
      type_dim = tt_size.size();

      //    std::cout << "type_dim=" << type_dim << std::endl;

      // if (type_dim != 1)
      if (type_dim < 1) {
        //    return 0;
        std::cout << "type_dim=" << type_dim << std::endl;
      }
      //    if (!altera_flow && tt_size[0] < 64)
      //    {
      //        return 0; // DO NOT convert for small arrays
      //    }
    }

    vector<void *> vec_access;

    typedef vector<vector<int64_t>> t_indepent_sections;
    vector<t_indepent_sections> vec_old_dim;
    // 1. Get all the references, and iterator range and steps
    {
      ast->GetPntrReferenceOfArray(sg_array, sg_ref, &vec_access);

      for (size_t i0 = 0; i0 < vec_access.size(); i0++) {
        void *sg_pntr = vec_access[i0];

        cout << "sg_pntr[" << i0 << "]=" << ast->_p(sg_pntr) << endl;

        if (ast->IsCastExp(sg_pntr) != 0) {
          ast->remove_cast(&sg_pntr);
        }

        int type_dim;  // = get_pntr_type_dim(sg_pntr);
        int pntr_dim;  // = get_pntr_pntr_dim(sg_pntr);
        {
          void *tt_array;
          vector<void *> tt_vec;
          ast->parse_pntr_ref_new(sg_pntr, &tt_array, &tt_vec);
          pntr_dim = tt_vec.size();

          void *tt_type = ast->GetTypebyVar(tt_array);
          void *tt_base_type;
          vector<size_t> tt_size;
          ast->get_type_dimension(tt_type, &tt_base_type, &tt_size, sg_pntr);
          type_dim = tt_size.size();
        }

        // if (type_dim != 1)
        if (type_dim < 1) {
          //    return 0;
          cout << "type_dim < 1" << endl;
        }

        if (type_dim != pntr_dim) {
          if (pntr_dim == 0) {
            continue;
          }
          //    return 0;
          cout << "pntr_dim != 0" << endl;
        }

        CMarsAccess ac(sg_pntr, ast, nullptr);

        // bool is_simple = true;
        CMarsExpression offset(ast, 0);

        // if (ac.GetIndexes().size() == 1)
        if (ac.GetIndexes().size() > static_cast<size_t>(dim)) {
          CMarsExpression me = ac.GetIndexes()[dim];
          // For memory burst with variable starting address
          // For the other circumstances, offset can be a constant 0
          //    me = me - *(CMarsExpression*)offset;
          me = me - offset;

          //    for (auto loop: outer_loops)
          //    {
          //        me.substitute(loop, 0);
          //    }
          //    for (auto loop: pe_loops)
          //    {
          //        me.substitute(loop, 0);
          //    }

          vector<CMarsExpression> vec_indexes;
          vec_indexes.push_back(me);
          // is_simple &= CMarsAccess::is_simple_type_v1(vec_indexes);
          cout << "==> expression: " << me.print() << endl;
          if (me.has_coeff_range_simple() == 0) {
            //    return 0;
            std::cout << "Not Simple" << std::endl;
          }

          t_indepent_sections sec_org = me.get_coeff_range_simple();
          t_indepent_sections sec_pre = sec_org;

          map<int, int> separate_pos;
          int curr_length = 0;
          for (size_t i = 0; i < sec_org.size(); i++) {
            std::cout << "#sec_org=" << i << std::endl;

            vector<int64_t> one_term = sec_org[i];
            int64_t one_step = std::abs(one_term[0]);
            int64_t one_length = one_term[2] - one_term[1];

            std::cout << one_term[0] << " " << one_term[1] << " " << one_term[2]
                      << std::endl;

            // check if the two section can be merged
            // 1. next coeff >= current range
            // 2. the dim size can divide large coeffs
            int is_dividable = 1;
            {
              for (size_t j = i + 1; j < sec_org.size(); j++) {
                int64_t one_step_1 = std::abs(sec_org[j][0]);

                if (one_step_1 % one_step != 0) {
                  is_dividable = 0;
                  break;
                }
              }
            }

            if (one_step > curr_length && (is_dividable != 0)) {
              if (one_step > 1) {
                separate_pos[i] = 1;
              } else {
                separate_pos[i] = 0;
              }
              curr_length = one_length;
            } else {
              curr_length += one_length;
            }
          }

          vector<vector<int64_t>> new_org;
          int64_t curr_lb = 0;
          int64_t curr_ub = 0;
          int64_t curr_step = 1;
          vector<int64_t> term;
          for (size_t i = 0; i < sec_org.size(); i++) {
            vector<int64_t> one_term = sec_org[i];
            int64_t one_step = one_term[0];
            int64_t one_lb = one_term[1];
            int64_t one_ub = one_term[2];
            if (separate_pos[i] == 1) {
              term.clear();
              term.push_back(curr_step);
              int64_t rest_lb =
                  curr_lb >= 0
                      ? (curr_lb % one_step)
                      : ((one_step - (-curr_lb % one_step)) % one_step);
              int64_t rest_ub =
                  curr_ub >= 0
                      ? (curr_ub % one_step)
                      : ((one_step - (-curr_ub % one_step)) % one_step);
              term.push_back(rest_lb);
              term.push_back(rest_ub);
              new_org.push_back(term);
              curr_step = one_step;
              curr_lb = one_lb + (curr_lb - rest_lb);
              curr_ub = one_ub + (curr_ub - rest_ub);
            } else {
              curr_lb += one_lb;
              curr_ub += one_ub;
            }
          }
          term.clear();
          term.push_back(curr_step);
          term.push_back(curr_lb);
          term.push_back(curr_ub);
          new_org.push_back(term);

          vec_old_dim.push_back(new_org);
        }
      }
    }

    vector<int64_t> separate_factors;
    {
      int is_same_separation = 1;
      for (size_t i = 0; i < vec_old_dim.size(); i++) {
        vector<vector<int64_t>> one_access = vec_old_dim[i];
        std::cout << "one_access.size()=" << one_access.size() << std::endl;
        for (size_t j = 0; j < one_access.size(); j++) {
          vector<int64_t> term = one_access[j];
          int64_t one_step = term[0];
          int64_t one_lb = term[1];
          int64_t one_ub = term[2];

          std::cout << "one_step=" << one_step << " one_lb=" << one_lb
                    << " one_ub=" << one_ub << std::endl;

          if (i == 0 || j >= separate_factors.size()) {
            std::cout << "i=" << i << " j=" << j << std::endl;
            separate_factors.push_back(one_step);
          } else {
            if (one_step != separate_factors[j]) {
              is_same_separation = 0;
              break;
            }
          }
        }
        if (is_same_separation == 0) {
          break;
        }
      }
      if ((is_same_separation == 0) ||
          (separate_factors.size() == 1 && separate_factors[0] == 1)) {
        //    old_steps.clear();
        //    new_dims.clear();

        std::cout << "is_same_separation=" << is_same_separation << std::endl;
        std::cout << "separate_factors_size==" << separate_factors.size()
                  << std::endl;
        std::cout << "separate_factors[0]=" << separate_factors[0] << std::endl;
        //    return 0;
      }

      for (size_t i = 0; i < separate_factors.size(); i++) {
        std::cout << "separate_factors[" << i << "]=" << separate_factors[i]
                  << std::endl;
      }

      // complement missing dimensions
      size_t new_dim_size = separate_factors.size();
      for (size_t i = 0; i < vec_old_dim.size(); i++) {
        vector<vector<int64_t>> one_access = vec_old_dim[i];
        if (one_access.size() == new_dim_size) {
          continue;
        }
        assert(one_access.size() < new_dim_size);
        int64_t factor_product = 1;
        size_t j = 0;
        while (j + 1 < one_access.size()) {
          factor_product *= separate_factors[j++];
        }
        int64_t curr_lb = one_access[j][1];
        int64_t curr_ub = one_access[j][2];
        vec_old_dim[i].resize(new_dim_size);
        while (j < new_dim_size) {
          vector<int64_t> term;
          int64_t curr_factor = separate_factors[j];
          term.push_back(curr_factor);
          factor_product *= curr_factor;
          term.push_back(curr_lb % factor_product);
          term.push_back(curr_ub % factor_product);
          vec_old_dim[i][j] = term;
          ++j;
        }
      }

      int is_satisfy_range = 1;
      vector<int> union_lb;
      vector<int> union_ub;
      for (size_t i = 0; i < vec_old_dim.size(); i++) {
        vector<vector<int64_t>> one_access = vec_old_dim[i];
        for (size_t j = 0; j < one_access.size(); j++) {
          vector<int64_t> term = one_access[j];
          int one_lb = term[1];
          int one_ub = term[2];

          if (i == 0 || j >= union_lb.size()) {
            union_lb.push_back(one_lb);
            union_ub.push_back(one_ub);
          } else {
            union_lb[j] = min(union_lb[j], one_lb);
            union_ub[j] = max(union_ub[j], one_ub);
          }
        }
      }

      assert(union_lb.size() == union_ub.size());
      assert(separate_factors.size() == union_ub.size());
      for (size_t i = 1; i < separate_factors.size(); i++) {
        if (separate_factors[i] < union_ub[i - 1] - union_lb[i - 1] + 1) {
          is_satisfy_range = 0;
          break;
        }
      }

      assert(union_lb.size() == union_ub.size());
      assert(separate_factors.size() == union_ub.size());
      for (size_t i = 1; i < separate_factors.size(); i++) {
        if (separate_factors[i] < union_ub[i - 1] - union_lb[i - 1] + 1) {
          is_satisfy_range = 0;
          break;
        }
      }

      if (is_satisfy_range == 0) {
        //    old_steps.clear();
        //    new_dims.clear();
        //    return 0;
        std::cout << "is_satisfy_range = " << is_satisfy_range << std::endl;
      }
    }
  }

#else
  void *sg_ref = expr_ref;
  void *sg_array = GetInitializedNameFromPntrArrRefExp(sg_ref);
  std::cout << "  ---- " << ast->_p(sg_array) << std::endl;

  if (ast->IsPointerType(ast->GetTypebyVar(sg_array))) {
    int dim_size = ast->get_dim_num_from_var(sg_array);
    std::cout << dim_size << std::endl;

    for (int dimIdx = dim_size - 1; dimIdx >= 0; dimIdx--) {
      vector<size_t> old_steps;
      vector<size_t> new_dims;

      CMarsExpression offset(0, ast);
      bool is_simple = true;

      bool is_analysis_success;
      bool is_apply_success;

      is_analysis_success =
          ast->analysis_delinearize(sg_array, dimIdx, old_steps, new_dims, 1);
      //    ast->analysis_delinearize(sg_array, dimIdx, old_steps, new_dims, 1,
      // is_simple, &offset, expr_ref);
      if (is_analysis_success) {
        std::cout << "analysis successful!" << std::endl;
      } else {
        std::cout << "analysis fails!" << std::endl;
      }
      ast->apply_delinearize(sg_array, dimIdx, old_steps, new_dims);
      //    is_apply_success = ast->apply_delinearize(sg_array, dimIdx,
      // old_steps, new_dims, false, is_simple, &offset, expr_ref);     if
      // (is_apply_success)
      //    {
      //        std::cout << "apply successful!" << std::endl;
      //    }

      string s_new_dims;
      {
        for (auto t : old_steps) {
          s_new_dims += "," + my_itoa(t);
        }
        if (old_steps.size())
          s_new_dims[0] = ' ';
        int pos = s_new_dims.size();
        for (auto t : new_dims) {
          s_new_dims += "," + my_itoa(t);
        }
        if (new_dims.size())
          s_new_dims[pos] = ':';
        s_new_dims += " ";
      }
      std::cout << "  ---- " << ast->_p(sg_array) << ": "
                << " dim=" << my_itoa(dimIdx) << " (" << s_new_dims << ") "
                << std::endl;
    }
  }
#endif

  //    int paren = 0;
  //    for (auto ref_idx: vec_idx)
  //    {
  //        printf("paren=%d\n", paren);
  //        vector<void *> vec_vars = ref_idx.get_vars();
  //        int dim = 0;
  //        for (auto var : vec_vars)
  //        {
  //        //    CMarsExpression coeff = ref_idx.get_coeff(var);
  //            CMarsVariable mv = CMarsVariable(var, ast);
  //
  //        //    printf("dim=%d, const=%d\n", dim, coeff.get_const());
  //            printf("dim=%d, ub=%d\n", dim,
  // (mv.get_range().get_ub().get_const() + 1));
  //
  //            dim++;
  //        }
  //        paren++;
  //    }
}

// Calculate overlap size
// FIXME: only handle Feeder_H now, and "[i2][i1]" cases.
int SystolicData::GetOverlapSize(void *ref_expr,
                                 const vector<void *> &outer_loops,
                                 //  const vector<void *> &feeder_loops,
                                 const vector<void *> &pe_loops,
                                 const vector<int> &v_buffer_dim) {
  // FIXME: Now simplified, to be enhanced
  //  Check the overlap only happens in rr dimension
  // TODO(youxiang):

  CSageCodeGen *ast = m_ast;

  //  Get the overlap dimension of the array
  void *pe_row_loop = pe_loops[0];
  int overlap_dim = 0;
  CMarsExpression overlap_idx;
  {
    CMarsAccess ma(ref_expr, ast, nullptr);
    auto vec_idx = ma.GetIndexes();

#if (SIMPLE_DELINEARIZE == 1)
    vector<CMarsExpression> new_vec_idx;
    for (auto ref_idx : vec_idx) {
      CMarsExpression ma_idx = ref_idx;
      vector<CMarsExpression> vec_ma_idx;
      SimpleDelinearization(ma_idx, vec_ma_idx);

      for (auto new_ref_idx : vec_ma_idx) {
        new_vec_idx.push_back(new_ref_idx);
      }
    }
#endif

#if (SIMPLE_DELINEARIZE == 1)
    for (auto ref_idx : new_vec_idx)
#else
    for (auto ref_idx : vec_idx)
#endif
    {
      if (!(ref_idx.get_const_coeff(pe_row_loop) == 0)) {
        overlap_idx = ref_idx;
        break;
      }
      overlap_dim++;
    }
  }

  std::cout << "overlap_dim=" << overlap_dim << std::endl;

  // calculate overlap
  // FIXME: overlap_size = up(idx[rr=0]) - lb(idx[rr=1])
  int overlap_size_in_dim = 0;
  {
    CMarsExpression me0 = overlap_idx;
    CMarsExpression me1 = overlap_idx;
    for (auto loop : outer_loops) {
      me0.substitute(loop, 0);
      me1.substitute(loop, 0);
    }
    me0.substitute(pe_row_loop, 0);
    me1.substitute(pe_row_loop, 1);

    CMarsExpression ub0 = me0.get_range().get_ub();
    CMarsExpression lb1 = me1.get_range().get_lb();

    printf("ub0=%ld, lb1=%ld\n", ub0.get_const(), lb1.get_const());

    CMarsExpression size = ub0 - lb1 + 1;
    overlap_size_in_dim = size.get_const();
  }

  // calculate total overlap size for the block
  int i = 0;
  int size = 1;
  for (auto dim : v_buffer_dim) {
    if (i == overlap_dim) {
      size *= overlap_size_in_dim;
    } else {
      size *= dim;
    }
    i++;
  }

  return size;
}

void SystolicData::SetDrainMap(void *ref_expr, const vector<void *> &loops,
                               const vector<void *> &outer_loops,
                               const vector<void *> &feeder_loops,
                               const vector<void *> &pe_loops, Drain *drain_o) {
  CSageCodeGen *ast = m_ast;

  CMarsAccess ma(ref_expr, ast, nullptr);
  auto vec_idx = ma.GetIndexes();

  vector<void *> feeder_pe_loops;
  feeder_pe_loops.insert(feeder_pe_loops.begin(), feeder_loops.begin(),
                         feeder_loops.end());
  feeder_pe_loops.insert(feeder_pe_loops.begin(), pe_loops.begin(),
                         pe_loops.end());

  // step 0: outer loops
  // outer loop order is strictly accord with the 9-loop
  vector<int> i_outer_loop_ubs;
  for (auto loop : outer_loops) {
    for (auto ref_idx : vec_idx) {
      if ((ref_idx.get_coeff(loop) != 0) != 0) {
        CMarsVariable mv = CMarsVariable(loop, ast);
        int i_ub = mv.get_range().get_ub().get_const();

        i_outer_loop_ubs.push_back(i_ub + 1);
      }
    }
  }

  drain_o->outer_loop_ubs = "";
  for (size_t i = 0; i < i_outer_loop_ubs.size(); i++) {
    std::cout << i_outer_loop_ubs[i] << std::endl;

    drain_o->outer_loop_ubs += std::to_string(i_outer_loop_ubs[i]);

    if (i < i_outer_loop_ubs.size() - 1) {
      drain_o->outer_loop_ubs += ",";
    }
  }

  // step 1: number all loops
  std::map<void *, int> map_loop_order;

  int order = 0;
  for (auto loop : loops) {
    //    std::cout << ast->_p(loop) << std::endl;
    //    loop_order.insert(std::pair<void *, int>(loop, order));
    map_loop_order[loop] = order;

    order++;
  }

  // step 2: loop order and data arrangement for data out of drain
  vector<int> drain_out_order;

  drain_out_order.push_back(map_loop_order[pe_loops[0]]);

  for (auto ref_idx : vec_idx) {
    CMarsExpression new_idx = ref_idx;
    for (auto loop : outer_loops) {
      new_idx.substitute(loop, 0);
    }

    // deal with PE_ROW expression
    if ((ref_idx.get_coeff(pe_loops[0]) != 0) != 0) {
      new_idx.substitute(pe_loops[0], 0);
    }
    if ((ref_idx.get_coeff(pe_loops[1]) != 0) != 0) {
      new_idx.substitute(pe_loops[1], 0);
    }

    vector<CMarsExpression> vec_new_indexes;
    vector<size_t> new_ubs;
    vector<size_t> old_steps;
    vector<size_t> new_dims;

    AnalysisDelinearize(&new_idx, &old_steps, &new_dims, &new_ubs);
    ApplyDelinearize(&new_idx, old_steps, new_dims, &vec_new_indexes);

    for (auto new_ref_idx : vec_new_indexes) {
      //    std::cout << new_ref_idx.print() << std::endl;
      for (auto loop : feeder_pe_loops) {
        if ((new_ref_idx.get_coeff(loop) != 0) != 0) {
          // find the loop for this delinearized loop iterator
          drain_out_order.push_back(map_loop_order[loop]);
        }
      }
    }

    if ((ref_idx.get_coeff(pe_loops[1]) != 0) != 0) {
      drain_out_order.push_back(map_loop_order[pe_loops[1]]);
    }
  }

  drain_o->drain_out_order_ubs = "";
  for (size_t i = 0; i < drain_out_order.size(); i++) {
    int order = drain_out_order[i];
    CMarsVariable mv = CMarsVariable(loops[order], ast);
    int i_ub = mv.get_range().get_ub().get_const();

    //    std::cout << "i_ub=" << i_ub << std::endl;

    drain_o->drain_out_order_ubs += std::to_string(i_ub + 1);

    if (i < drain_out_order.size() - 1) {
      drain_o->drain_out_order_ubs += ",";
    }
  }

  // step 3: data arrangement in drain buffer
  vector<int> drain_buffer_order;

  for (auto ref_idx : vec_idx) {
    CMarsExpression new_idx = ref_idx;
    for (auto loop : outer_loops) {
      new_idx.substitute(loop, 0);
    }

    vector<CMarsExpression> vec_new_indexes;
    vector<size_t> new_ubs;
    vector<size_t> old_steps;
    vector<size_t> new_dims;

    AnalysisDelinearize(&new_idx, &old_steps, &new_dims, &new_ubs);
    ApplyDelinearize(&new_idx, old_steps, new_dims, &vec_new_indexes);

    for (auto new_ref_idx : vec_new_indexes) {
      //    std::cout << new_ref_idx.print() << std::endl;
      for (auto loop : feeder_pe_loops) {
        if ((new_ref_idx.get_coeff(loop) != 0) != 0) {
          // find the loop for this delinearized loop iterator
          drain_buffer_order.push_back(map_loop_order[loop]);
        }
      }
    }
  }

  drain_o->drain_buffer_order_ubs = "";
  for (size_t i = 0; i < drain_buffer_order.size(); i++) {
    int order = drain_buffer_order[i];
    CMarsVariable mv = CMarsVariable(loops[order], ast);
    int i_ub = mv.get_range().get_ub().get_const();

    drain_o->drain_buffer_order_ubs += std::to_string(i_ub + 1);

    if (i < drain_buffer_order.size() - 1) {
      drain_o->drain_buffer_order_ubs += ",";
    }
  }

  // step 4: construct a map between drain_out_order and drain_buffer_order
  drain_o->out_to_buffer_map = "";
  for (size_t i = 0; i < drain_out_order.size(); i++) {
    int out_order = drain_out_order[i];
    for (size_t j = 0; j < drain_buffer_order.size(); j++) {
      int buffer_order = drain_buffer_order[j];
      if (buffer_order == out_order) {
        drain_o->out_to_buffer_map += std::to_string(j);
        break;
      }
    }

    if (i < drain_out_order.size() - 1) {
      drain_o->out_to_buffer_map += ",";
    }
  }
}

void SystolicData::AnalyzeLowLevelModel(void *sysLoop) {
  CSageCodeGen *ast = m_ast;

  // 1. get the loop nests
  vector<void *> loops;
  vector<void *> outer_loops;
  vector<void *> feeder_loops;
  vector<void *> pe_loops;
  void *inner_stmt;
  {
    void *curr_loop = sysLoop;
    int loop_state = 0;
    while (true) {
      if (ast->IsBasicBlock(curr_loop) != 0) {
        void *child = ast->GetChildStmt(curr_loop, 0);
        while (child != nullptr) {
          if (ast->IsForStatement(child) != 0) {
            curr_loop = child;
            break;
          }
          void *preChild = child;
          child = ast->GetChildStmt(preChild, 0);
          if (preChild == child) {
            break;
          }
        }
      }

      if (ast->IsForStatement(curr_loop) == 0) {
        return;  // FIXME: error out
      }
      loops.push_back(curr_loop);
#if DEBUG_EXTRACT
      cout << "-- Loop[" << my_itoa(loop_state) << "]: " << ast->_up(curr_loop)
           << endl;
#endif

      if (loop_state == 0) {
        outer_loops.push_back(curr_loop);
      } else if (loop_state == 1) {
        feeder_loops.push_back(curr_loop);
      } else if (loop_state == 2) {
        pe_loops.push_back(curr_loop);
      }

      void *body = ast->GetLoopBody(curr_loop);
      if (ast->IsBasicBlock(body) != 0) {
        void *child = ast->GetChildStmt(body, 0);
        while (child != nullptr) {
          if (ast->IsForStatement(child) == 0) {
            void *preChild = child;
            child = ast->GetChildStmt(preChild, 0);
            if (preChild == child) {
              break;
            }
          } else {
            body = child;

            break;
          }
        }
      }

      if (ast->IsBasicBlock(body) != 0) {
        curr_loop = ast->GetChildStmt(body, 0);
        if ((ast->IsPragmaDecl(curr_loop) != 0) &&
            ast->GetChildStmtNum(body) == 2) {
#if DEBUG_EXTRACT
          cout << "-- Pragma: " << ast->_up(curr_loop) << endl;
#endif
          curr_loop = ast->GetChildStmt(body, 1);
          while (ast->IsForStatement(curr_loop) == 0) {
            curr_loop = ast->GetChildStmt(curr_loop, 0);
          }
          loop_state++;
        }

        if (ast->IsForStatement(curr_loop) == 0) {
          curr_loop = body;
        }
      } else {
        curr_loop = body;
      }

      if (ast->IsForStatement(curr_loop) != 0) {
        continue;
      }
      inner_stmt = curr_loop;
      break;
    }
  }

#if DEBUG_EXTRACT
  cout << "-- Innermost: " << ast->_up(inner_stmt) << endl;
#endif

  // 2. loop analysis
  // Outer
  for (auto loop : outer_loops) {
    int tc = GetConstantFromExp(
        ast->GetExpRightOperand(ast->GetExprFromStmt(ast->GetLoopTest(loop))));
    vector<int> tmp;
    tmp.push_back(tc);
    AddOuter(ast->GetLoopIterator(loop),
             tmp);  // FIXME comaniac: Outer range is now a vector
  }

  GetLoopBounds(&Outer.loop_bounds, outer_loops);
#if DEBUG_EXTRACT
  for (size_t i = 0; i < Outer.loop_bounds.size(); i++) {
    printf("%d\n", Outer.loop_bounds[i]);
  }
#endif
  // Feeder
  for (auto loop : feeder_loops) {
    int tc = GetConstantFromExp(
        ast->GetExpRightOperand(ast->GetExprFromStmt(ast->GetLoopTest(loop))));
    AddFeeder(ast->GetLoopIterator(loop), tc);
  }

  GetLoopBounds(&Feeder_H.loop_bounds, feeder_loops);

  GetLoopBounds(&Feeder_V.loop_bounds, feeder_loops);

#if DEBUG_EXTRACT
  for (size_t i = 0; i < Feeder_V.loop_bounds.size(); i++) {
    printf("%d\n", Feeder_V.loop_bounds[i]);
  }
#endif
  // PE
  for (auto loop : pe_loops) {
    int tc = GetConstantFromExp(
        ast->GetExpRightOperand(ast->GetExprFromStmt(ast->GetLoopTest(loop))));
    mapPE2Range[ast->GetLoopIterator(loop)] = ast->CreateConst(tc);
  }

  GetLoopBounds(&PE.loop_bounds, pe_loops);
#if DEBUG_EXTRACT
  for (size_t i = 0; i < PE.loop_bounds.size(); i++) {
    printf("%d\n", PE.loop_bounds[i]);
  }
#endif
  cout << "zhudl begin debug loopbounds" << endl;
  for (size_t i = 0; i < PE.loop_bounds.size(); i++) {
    printf("%d\n", PE.loop_bounds[i]);
  }

  // 3. expression analysis
  void *ref_o;
  void *ref_h;
  void *ref_v;
  void *ref_l;
  void *ref_r;
  void *op_map;
  void *op_reduce;
  {
    vector<void *> vec_exp_stmt;
    ast->GetNodesByType(inner_stmt, "preorder", "SgExprStatement",
                        &vec_exp_stmt);

    if (vec_exp_stmt.size() != 1) {
      for (auto stmt : vec_exp_stmt) {
        std::cout << "stmt=" << ast->_p(stmt) << std::endl;
      }

      cout << MSG_ERROR << "Invalid Innermost Statement - exit at " << FILE_POS
           << endl;
      cout << endl;
      cout << endl;
      assert(0);
    }

    void *calc_stmt = vec_exp_stmt[0];
    void *calc_expr = ast->GetExprFromStmt(calc_stmt);

    // TODO(youxiang): now only support c+=a*b, not support c = c+a*b
    op_reduce = calc_expr;
    op_map = ast->GetExpRightOperand(op_reduce);
    ref_o = ast->GetExpLeftOperand(op_reduce);
    // ref_h = ast->GetExpLeftOperand(op_map);
    // ref_v = ast->GetExpRightOperand(op_map);
    ref_l = ast->GetExpLeftOperand(op_map);
    ref_r = ast->GetExpRightOperand(op_map);

    ref_h = GetExpPEArrayOperand(pe_loops[0], ref_l, ref_r);
    ref_v = GetExpPEArrayOperand(pe_loops[1], ref_l, ref_r);

#if DEBUG_EXTRACT
    cout << "OUT: " << ast->_up(ref_o) << endl;
    cout << "INH: " << ast->_up(ref_h) << endl;
    cout << "INV: " << ast->_up(ref_v) << endl;
    cout << "L: " << ast->_up(ref_l) << endl;
    cout << "R: " << ast->_up(ref_r) << endl;
#endif
  }

  // get operator
  PE.reduce_op = GetOpString(op_reduce);
  if (PE.reduce_op.empty()) {
    cout << MSG_ERROR << "Reduction Op not feasible" << endl;
    exit(-1);
  }
  PE.map_op = GetOpString(op_map);
  if (PE.map_op.empty()) {
    cout << MSG_ERROR << "Map Op not feasible" << endl;
    exit(-1);
  }

  // Reimplement buffer_dim and iteration_domain
  // FIXME: to be extern to both H and V
  // FIXME: to be integrated
  vector<int> v_buffer_dim_h;
  vector<int> v_buffer_dim_v;
  vector<int> v_iteration_domain;
  string s_iteration_domain;
  {
    for (auto loop : feeder_loops) {
      int64_t ub =
          CMarsVariable(loop, ast).get_range().get_ub().get_const() + 1;
      v_iteration_domain.push_back(ub);
      s_iteration_domain += my_itoa(ub) + ",";
    }
    s_iteration_domain =
        s_iteration_domain.substr(0, s_iteration_domain.length() - 1);
  }
  Feeder_H.iteration_domain = s_iteration_domain;
  Feeder_V.iteration_domain = s_iteration_domain;

  //    CheckFeederLoopFeasibility(feeder_loops, ref_o, ref_h, ref_v);

  // Get index information for feeder_H
  Feeder_H.access_pattern = GetFeederIdxInfo(ref_h, outer_loops, feeder_loops,
                                             pe_loops, &v_buffer_dim_h);
  std::cout << "-- Feeder_H access_pattern: " << Feeder_H.access_pattern
            << endl;
  // Get index information for feeder_V
  Feeder_V.access_pattern = GetFeederIdxInfo(ref_v, outer_loops, feeder_loops,
                                             pe_loops, &v_buffer_dim_v);
  std::cout << "-- Feeder_V access_pattern: " << Feeder_V.access_pattern
            << endl;

#if DEBUG_EXTRACT
  cout << "-- iteration_domain: " << s_iteration_domain << endl;
  cout << "-- buffer_dim_h: " << IntVectorToString(v_buffer_dim_h) << endl;
  cout << "-- buffer_dim_v: " << IntVectorToString(v_buffer_dim_v) << endl;
#endif
  Feeder_H.buffer_dim = IntVectorToString(v_buffer_dim_h);
  Feeder_V.buffer_dim = IntVectorToString(v_buffer_dim_v);

  Feeder_H.overlap_size =
      GetOverlapSize(ref_h, outer_loops, pe_loops, v_buffer_dim_h);
  Feeder_V.overlap_size = 0;  // FIXME

#if DEBUG_EXTRACT
  cout << "-- overlap_size_h: " << my_itoa(Feeder_H.overlap_size) << endl;
#endif

  vector<int> v_block_dim_v;
  vector<int> v_block_dim_h;
  vector<int> v_block_dim_o;

  GetBlockDim(ref_h, outer_loops, feeder_loops, pe_loops, &v_block_dim_h);
  GetBlockDim(ref_v, outer_loops, feeder_loops, pe_loops, &v_block_dim_v);
  GetBlockDim(ref_o, outer_loops, feeder_loops, pe_loops, &v_block_dim_o);

#if DEBUG_EXTRACT
  //       cout << "-- iteration_domain: " << s_iteration_domain << endl;
  cout << "-- block_dim_h: " << IntVectorToString(v_block_dim_h) << endl;
  cout << "-- block_dim_v: " << IntVectorToString(v_block_dim_v) << endl;
#endif
  Loader_H.block_dim = IntVectorToString(v_block_dim_h);
  Loader_V.block_dim = IntVectorToString(v_block_dim_v);

  SetPEIterIdx(0, &Feeder_H);
  SetPEIterIdx(1, &Feeder_V);

  vector<int> v_block_size_h;
  vector<int> v_block_size_v;
  vector<int> v_block_size_o;
  vector<int> v_new_block_size_h;
  vector<int> v_new_block_size_v;

  GetBlockSize(ref_h, outer_loops, feeder_loops, pe_loops, &v_block_size_h);
  GetBlockSize(ref_v, outer_loops, feeder_loops, pe_loops, &v_block_size_v);
  GetBlockSize(ref_o, outer_loops, feeder_loops, pe_loops, &v_block_size_o);

  GetBlockSize(ref_h, outer_loops, feeder_loops, pe_loops, &v_new_block_size_h,
               &Feeder_H);
  GetBlockSize(ref_v, outer_loops, feeder_loops, pe_loops, &v_new_block_size_v,
               &Feeder_V);

  Loader_H.block_size = IntVectorToString(v_block_size_h);
  Loader_V.block_size = IntVectorToString(v_block_size_v);
  Loader_H.new_block_size = IntVectorToString(v_new_block_size_h);
  Loader_V.new_block_size = IntVectorToString(v_new_block_size_v);

  vector<int> v_reuse_num_o;

  GetReuseNum(ref_o, outer_loops, feeder_loops, pe_loops, &v_reuse_num_o);

  Drain_O.reuse_num = IntVectorToString(v_reuse_num_o);

  vector<int> v_num_coalesced_words;

  Drain_O.num_coalesced_words =
      GetCoalescedWordsNum(ref_o, &v_num_coalesced_words);

  GetExprUB(v_block_dim_h, v_block_size_h, v_block_dim_v, v_block_size_v,
            v_block_dim_o, v_block_size_o);

  //    SetPEIterDim(ref_h, pe_loops, Feeder_H);
  //    SetPEIterDim(ref_v, pe_loops, Feeder_V);

  //    GetPEIterCoeff(ref_h, outer_loops, feeder_loops, pe_loops, Feeder_H);
  //    GetPEIterCoeff(ref_v, outer_loops, feeder_loops, pe_loops, Feeder_V);

  //    GetRightSideBounds(ref_h, outer_loops, feeder_loops, pe_loops,
  // Feeder_H);     GetRightSideBounds(ref_v, outer_loops, feeder_loops,
  // pe_loops, Feeder_V);

  //    SystolicTest(ref_h, outer_loops, pe_loops);
  //    SystolicTest(ref_v);
  //    SystolicTest(op_map);

  SetDrainMap(ref_o, loops, outer_loops, feeder_loops, pe_loops, &Drain_O);
}

void SystolicData::DumpXML(string filename) {
  CXMLParser parser;
  // root
  CXMLNode *root = parser.parse_empty("systolic");

  // PE
  CXMLNode *pe = root->CreateAppendChild("PE");

  CXMLNode *pe_name = pe->CreateAppendChild("name");
  pe_name->SetValue("cnn_pearray");

  CXMLNode *pe_rows = pe->CreateAppendChild("PE_ROWS");
  pe_rows->SetValue(std::to_string(PE.loop_bounds[ROW]));
  CXMLNode *pe_cols = pe->CreateAppendChild("PE_COLS");
  pe_cols->SetValue(std::to_string(PE.loop_bounds[COL]));
  CXMLNode *pe_dsize = pe->CreateAppendChild("PE_DSIZE");
  pe_dsize->SetValue(std::to_string(PE.loop_bounds[SIMD]));
  CXMLNode *pe_reduce_op = pe->CreateAppendChild("reduce_op");
  pe_reduce_op->SetValue((PE.reduce_op));
  CXMLNode *pe_map_op = pe->CreateAppendChild("map_op");
  pe_map_op->SetValue((PE.map_op));

  // Loader_H
  CXMLNode *loader_h = root->CreateAppendChild("Loader_H");
  CXMLNode *loader_h_name = loader_h->CreateAppendChild("name");
  loader_h_name->SetValue("Loader_H");
  CXMLNode *loader_h_block_dim = loader_h->CreateAppendChild("block_dim");
  loader_h_block_dim->SetValue(Loader_H.block_dim);
  //    CXMLNode * loader_h_block_dim_iteration_domain =
  // loader_h->CreateAppendChild("block_dim_iteration_domain");
  //    loader_h_block_dim_iteration_domain->SetValue(Loader_H.block_dim_iteration_domain);
  //    CXMLNode * loader_h_block_dim_access_pattern =
  // loader_h->CreateAppendChild("block_dim_access_pattern");
  //    loader_h_block_dim_access_pattern->SetValue(Loader_H.block_dim_access_pattern);
  CXMLNode *loader_h_block_size = loader_h->CreateAppendChild("block_size");
  loader_h_block_size->SetValue(Loader_H.block_size);
  CXMLNode *loader_h_new_block_size =
      loader_h->CreateAppendChild("new_block_size");
  loader_h_new_block_size->SetValue(Loader_H.new_block_size);
  //    CXMLNode * loader_h_block_size_iteration_domain =
  // loader_h->CreateAppendChild("block_size_iteration_domain");
  //    loader_h_block_size_iteration_domain->SetValue(Loader_H.block_size_iteration_domain);
  //    CXMLNode * loader_h_block_size_access_pattern =
  // loader_h->CreateAppendChild("block_size_access_pattern");
  //    loader_h_block_size_access_pattern->SetValue(Loader_H.block_size_access_pattern);

  // Loader_V
  CXMLNode *loader_v = root->CreateAppendChild("Loader_V");
  CXMLNode *loader_v_name = loader_v->CreateAppendChild("name");
  loader_v_name->SetValue("Loader_V");
  CXMLNode *loader_v_block_dim = loader_v->CreateAppendChild("block_dim");
  loader_v_block_dim->SetValue(Loader_V.block_dim);
  //    CXMLNode * loader_v_block_dim_iteration_domain =
  // loader_v->CreateAppendChild("block_dim_iteration_domain");
  //    loader_v_block_dim_iteration_domain->SetValue(Loader_V.block_dim_iteration_domain);
  //    CXMLNode * loader_v_block_dim_access_pattern =
  // loader_v->CreateAppendChild("block_dim_access_pattern");
  //    loader_v_block_dim_access_pattern->SetValue(Loader_V.block_dim_access_pattern);
  CXMLNode *loader_v_block_size = loader_v->CreateAppendChild("block_size");
  loader_v_block_size->SetValue(Loader_V.block_size);
  CXMLNode *loader_v_new_block_size =
      loader_v->CreateAppendChild("new_block_size");
  loader_v_new_block_size->SetValue(Loader_V.new_block_size);
  //    CXMLNode * loader_v_block_size_iteration_domain =
  // loader_v->CreateAppendChild("block_size_iteration_domain");
  //    loader_v_block_size_iteration_domain->SetValue(Loader_V.block_size_iteration_domain);
  //    CXMLNode * loader_v_block_size_access_pattern =
  // loader_v->CreateAppendChild("block_size_access_pattern");
  //    loader_v_block_size_access_pattern->SetValue(Loader_V.block_size_access_pattern);

  // Feeder_H
  CXMLNode *feeder_h = root->CreateAppendChild("Feeder_H");

  CXMLNode *feeder_h_name = feeder_h->CreateAppendChild("name");
  feeder_h_name->SetValue("Feeder_H");
  CXMLNode *feeder_h_num = feeder_h->CreateAppendChild("num");
  feeder_h_num->SetValue(std::to_string(PE.loop_bounds[0]));
  CXMLNode *feeder_h_buffer_dim = feeder_h->CreateAppendChild("buffer_dim");
  feeder_h_buffer_dim->SetValue(Feeder_H.buffer_dim);
  CXMLNode *feeder_h_overlap_size = feeder_h->CreateAppendChild("overlap_size");
  feeder_h_overlap_size->SetValue(std::to_string(Feeder_H.overlap_size));
  CXMLNode *feeder_h_iteration_domain =
      feeder_h->CreateAppendChild("iteration_domain");
  feeder_h_iteration_domain->SetValue(Feeder_H.iteration_domain);
  CXMLNode *feeder_h_access_pattern =
      feeder_h->CreateAppendChild("access_pattern");
  feeder_h_access_pattern->SetValue(Feeder_H.access_pattern);
  CXMLNode *feeder_h_pe_iter_dim = feeder_h->CreateAppendChild("pe_iter_dim");
  feeder_h_pe_iter_dim->SetValue(std::to_string(Feeder_H.pe_iter_dim));
  CXMLNode *feeder_h_pe_iter_coeff =
      feeder_h->CreateAppendChild("pe_iter_coeff");
  feeder_h_pe_iter_coeff->SetValue(std::to_string(Feeder_H.pe_iter_coeff));
  CXMLNode *feeder_h_pe_dim_no_r2_lb =
      feeder_h->CreateAppendChild("pe_dim_no_r2_lb");
  feeder_h_pe_dim_no_r2_lb->SetValue(std::to_string(Feeder_H.pe_dim_no_r2_lb));
  CXMLNode *feeder_h_pe_dim_no_r2_ub =
      feeder_h->CreateAppendChild("pe_dim_no_r2_ub");
  feeder_h_pe_dim_no_r2_ub->SetValue(std::to_string(Feeder_H.pe_dim_no_r2_ub));

  CXMLNode *feeder_v = root->CreateAppendChild("Feeder_V");
  CXMLNode *feeder_v_name = feeder_v->CreateAppendChild("name");
  feeder_v_name->SetValue("Feeder_V");
  CXMLNode *feeder_v_num = feeder_v->CreateAppendChild("num");
  feeder_v_num->SetValue(std::to_string(PE.loop_bounds[1]));
  CXMLNode *feeder_v_buffer_dim = feeder_v->CreateAppendChild("buffer_dim");
  feeder_v_buffer_dim->SetValue(Feeder_V.buffer_dim);
  CXMLNode *feeder_v_overlap_size = feeder_v->CreateAppendChild("overlap_size");
  feeder_v_overlap_size->SetValue(std::to_string(Feeder_V.overlap_size));
  CXMLNode *feeder_v_iteration_domain =
      feeder_v->CreateAppendChild("iteration_domain");
  feeder_v_iteration_domain->SetValue(Feeder_V.iteration_domain);
  CXMLNode *feeder_v_access_pattern =
      feeder_v->CreateAppendChild("access_pattern");
  feeder_v_access_pattern->SetValue(Feeder_V.access_pattern);
  CXMLNode *feeder_v_pe_iter_dim = feeder_v->CreateAppendChild("pe_iter_dim");
  feeder_v_pe_iter_dim->SetValue(std::to_string(Feeder_V.pe_iter_dim));
  CXMLNode *feeder_v_pe_iter_coeff =
      feeder_v->CreateAppendChild("pe_iter_coeff");
  feeder_v_pe_iter_coeff->SetValue(std::to_string(Feeder_V.pe_iter_coeff));
  CXMLNode *feeder_v_pe_dim_no_r2_lb =
      feeder_v->CreateAppendChild("pe_dim_no_r2_lb");
  feeder_v_pe_dim_no_r2_lb->SetValue(std::to_string(Feeder_V.pe_dim_no_r2_lb));
  CXMLNode *feeder_v_pe_dim_no_r2_ub =
      feeder_v->CreateAppendChild("pe_dim_no_r2_ub");
  feeder_v_pe_dim_no_r2_ub->SetValue(std::to_string(Feeder_V.pe_dim_no_r2_ub));

  CXMLNode *drain_o = root->CreateAppendChild("Drain_O");
  CXMLNode *drain_o_name = drain_o->CreateAppendChild("name");
  drain_o_name->SetValue("Drain_O");
  CXMLNode *drain_o_reuse_num = drain_o->CreateAppendChild("reuse_num");
  drain_o_reuse_num->SetValue(Drain_O.reuse_num);
  CXMLNode *drain_o_num_coalesced_words =
      drain_o->CreateAppendChild("num_coalesced_words");
  drain_o_num_coalesced_words->SetValue(
      std::to_string(Drain_O.num_coalesced_words));
  CXMLNode *drain_o_out_order_ubs =
      drain_o->CreateAppendChild("drain_out_order_ubs");
  drain_o_out_order_ubs->SetValue(Drain_O.drain_out_order_ubs);
  CXMLNode *drain_o_buffer_order_ubs =
      drain_o->CreateAppendChild("drain_buffer_order_ubs");
  drain_o_buffer_order_ubs->SetValue(Drain_O.drain_buffer_order_ubs);
  CXMLNode *drain_o_outer_loop_ubs =
      drain_o->CreateAppendChild("outer_loop_ubs");
  drain_o_outer_loop_ubs->SetValue(Drain_O.outer_loop_ubs);
  CXMLNode *drain_o_out_to_buffer_map =
      drain_o->CreateAppendChild("out_to_buffer_map");
  drain_o_out_to_buffer_map->SetValue(Drain_O.out_to_buffer_map);

  // FIXME: just for CNN
  //    CXMLNode * conv_params = root->CreateAppendChild("Conv_Params");
  //    CXMLNode * conv_params_name = conv_params->CreateAppendChild("name");
  //    conv_params_name->SetValue("Conv_Params");
  //    CXMLNode * conv_params_o =
  //    conv_params->CreateAppendChild("IMG_OUT_NUM");
  //    conv_params_o->SetValue(std::to_string(m_conv_params[IMG_OUT_NUM]));
  //    CXMLNode * conv_params_p = conv_params->CreateAppendChild("KERNEL");
  //    conv_params_p->SetValue(std::to_string(m_conv_params[P]));
  //    CXMLNode * conv_params_i = conv_params->CreateAppendChild("IMG_IN_NUM");
  //    conv_params_i->SetValue(std::to_string(m_conv_params[IMG_IN_NUM]));
  //    CXMLNode * conv_params_rp =
  //    conv_params->CreateAppendChild("IMG_IN_ROW");
  //    conv_params_rp->SetValue(std::to_string(m_conv_params[IMG_IN_ROW]));
  //    CXMLNode * conv_params_cq =
  //    conv_params->CreateAppendChild("IMG_IN_COL");
  //    conv_params_cq->SetValue(std::to_string(m_conv_params[IMG_IN_COL]));
  //    CXMLNode * conv_params_r = conv_params->CreateAppendChild("IMG_ROW");
  //    conv_params_r->SetValue(std::to_string(m_conv_params[IMG_ROW]));
  //    CXMLNode * conv_params_c = conv_params->CreateAppendChild("IMG_COL");
  //    conv_params_c->SetValue(std::to_string(m_conv_params[IMG_COL]));
  CXMLNode *host_params = root->CreateAppendChild("Host_Params");
  CXMLNode *host_params_name = host_params->CreateAppendChild("name");
  host_params_name->SetValue("Host_Params");
  CXMLNode *expr_ub_o = host_params->CreateAppendChild("OUT_UB");
  expr_ub_o->SetValue(std::to_string(m_expr_ub[OUT]));
  CXMLNode *expr_ub_h = host_params->CreateAppendChild("IN_UB");
  expr_ub_h->SetValue(std::to_string(m_expr_ub[IN]));
  CXMLNode *expr_ub_v = host_params->CreateAppendChild("WT_UB");
  expr_ub_v->SetValue(std::to_string(m_expr_ub[WT]));
  CXMLNode *outer_loop_bounds =
      host_params->CreateAppendChild("outer_loop_bounds");
  outer_loop_bounds->SetValue(to_string(Outer.loop_bounds));
  CXMLNode *feeder_loop_bounds =
      host_params->CreateAppendChild("feeder_loop_bounds");
  feeder_loop_bounds->SetValue(to_string(Feeder_H.loop_bounds));

  cout << endl;
  // print loop information
  cout << "Outer Loops (Sequential):\n";
  for (auto iter : mapOuter2Range) {
    // cout << "for " << m_ast->UnparseToString(iter.first) << " <- 0 to ";
    // cout << m_ast->UnparseToString(iter.second);
    // cout << "\n    ";
    cout << "Loop "
         << m_ast->UnparseToString(iter.first) + ": 0.." +
                m_ast->UnparseToString(iter.second)
         << endl;
  }
  //    cout << endl;
  cout << "Reuse Loops (for Feeders):\n";
  for (auto iter : mapFeeder2Range) {
    // cout << "for " << m_ast->UnparseToString(iter.first) << " <- 0 to ";
    // cout << m_ast->UnparseToString(iter.second);
    // cout << "\n    ";
    cout << "Loop "
         << m_ast->UnparseToString(iter.first) + ": 0.." +
                m_ast->UnparseToString(iter.second)
         << endl;
  }
  //    cout << endl;
  cout << "Parallel Loops (for PEs): \n";
  for (auto iter : mapPE2Range) {
    // cout << "for " << m_ast->UnparseToString(iter.first) << " <- 0 to ";
    // cout << m_ast->UnparseToString(iter.second);
    // cout << "\n    ";
    cout << "Loop "
         << m_ast->UnparseToString(iter.first) + ": 0.." +
                m_ast->UnparseToString(iter.second)
         << endl;
  }
  //    cout << endl;

  // print the metrics

  //  float thrpt_ideal = 2.0;
  //  for (vector<int>::iterator iter = PE.loop_bounds.begin();
  //       iter != PE.loop_bounds.end(); iter++) {
  //    thrpt_ideal *= *iter;
  //  }
  //  thrpt_ideal *= 0.3;
  //  cout << "Ideal throughput = " << thrpt_ideal << " GFLOPS" << endl;

  parser.write_into_file(filename);

  cout << "\nOpenCL Design Generated!" << endl;
}

string SystolicData::toString() {
  string s;

  // Print source information
#ifdef MSG_FINE
  s += "- Source code analysis result:\n";
  s += "Out: " + m_ast->UnparseToString(vecExp[OUT]) + "\n";
  s += "\tNot related loops: ";
  for (auto iter : vecSetNotRelatedLoop[OUT])
    s += m_ast->UnparseToString(iter) + " ";
  s += "\n";

  s += "Weight: " + m_ast->UnparseToString(vecExp[WT]) + "\n";
  s += "\tNot related loops: ";
  for (auto iter : vecSetNotRelatedLoop[WT])
    s += m_ast->UnparseToString(iter) + " ";
  s += "\n";

  s += "In: " + m_ast->UnparseToString(vecExp[IN]) + "\n";
  s += "\tNot related loops: ";
  for (auto iter : vecSetNotRelatedLoop[IN])
    s += m_ast->UnparseToString(iter) + " ";
  s += "\n\n";
#endif

  // Print systolic array information
  s += "- Systolic array:\n";
  s += "\tPE row: " + m_ast->UnparseToString(vecPE[ROW]) + " -> " +
       m_ast->UnparseToString(mapPE2Range[vecPE[ROW]]) + "\n";
  s += "\tPE col: " + m_ast->UnparseToString(vecPE[COL]) + " -> " +
       m_ast->UnparseToString(mapPE2Range[vecPE[COL]]) + "\n";
  s += "\tPE SIMD: " + m_ast->UnparseToString(vecPE[SIMD]) + " -> " +
       m_ast->UnparseToString(mapPE2Range[vecPE[SIMD]]) + "\n";

  s += "- Feeder:\n";
  for (auto iter : mapFeeder2Range) {
    s += "\t" + m_ast->UnparseToString(iter.first) + " -> " +
         m_ast->UnparseToString(iter.second) + "\n";
  }

  s += "- Outer:\n";
  for (auto iter : mapOuter2Range) {
    s += "\t" + m_ast->UnparseToString(iter.first) + " -> " +
         m_ast->UnparseToString(iter.second) + "\n";
  }
  s += "\n";

  int optBlk = GetBRAMBlk();
  s += "BRAM usage: " + std::to_string(optBlk);
  s += " (" + std::to_string(static_cast<float>(optBlk) / BRAM_BOUND * 100.0) +
       "%)\n";

  vector<float> bwGiven(4);
  std::fill(bwGiven.begin(), bwGiven.end() - 1, GetMaxBWperPort());
  bwGiven[3] = MAX_BANDWIDTH;  // Altera given overall bandwidth
  vector<float> optBW(4);
  GetBW(&optBW);
  s += "Bandwidth: " + std::to_string(optBW[3]);
  s += " (" + std::to_string(optBW[3] / bwGiven[3] * 100.0) + "%)\n";
  s += "\tIN: " + std::to_string(optBW[0]);
  s += " (" + std::to_string(optBW[0] / bwGiven[0] * 100.0) + "%)\n";
  s += "\tOUT: " + std::to_string(optBW[1]);
  s += " (" + std::to_string(optBW[1] / bwGiven[1] * 100.0) + "%)\n";
  s += "\tWT: " + std::to_string(optBW[2]);
  s += " (" + std::to_string(optBW[2] / bwGiven[2] * 100.0) + "%)\n";

  s += "Throughput: " + std::to_string(GetThrpt()) + " GFlop/s\n";
  return s;
}
