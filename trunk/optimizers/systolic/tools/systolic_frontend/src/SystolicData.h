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


#ifndef TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_SYSTOLICDATA_H_
#define TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_SYSTOLICDATA_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include "codegen.h"
#include "program_analysis.h"
#include "mars_ir_v2.h"
#define CLASS_NAME "SystolicData"
// #define MSG_FINE "[" << CLASS_NAME << "][" << __FUNCTION__ << "] FINE: "
#define MSG_INFO "[" << CLASS_NAME << "][" << __FUNCTION__ << "] INFO: "
#define MSG_ERROR "[" << CLASS_NAME << "][" << __FUNCTION__ << "] ERROR: "

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define FILE_POS __FILENAME__ << ":" << __LINE__

#define ENABLE_PRE_POST_PROC 0
#define ENABLE_MULTI_LAYER_PROC 0

const int FREQ = 250;  // MHz
const int SYS_RES_BOUND = 160 * 8;
// const int SYS_RES_BOUND = 160 * 8 * 2; // Double resource for fixed point
const int MAX_BANDWIDTH = 19;  // GB/s

// # of available BRAM blocks (2KB per block)
// Reserve 30% for other usages.
// const float BRAM_BOUND = 2705 * 4; // Times 4 for fixed point
const float BRAM_BOUND = 2705;     // Times 4 for fixed point
const float BRAM_MAX_RATIO = 1.0;  // 0.8;
const float BRAM_AVAIL = BRAM_BOUND * BRAM_MAX_RATIO;
const int ALTERA_SYSTOLIC_ARRAY_PE_BLK = 9;

class SystolicData {
 private:
  // Data structures
  /////////////////////////////////////////
  // General
  enum SYSTOLIC_MODEL { HIGH_LEVEL, LOW_LEVEL };

  CMarsIrV2 *m_ir;
  CSageCodeGen *m_ast;
  void *m_pragma;
  enum SYSTOLIC_MODEL model;
  int typeSize;
  int layerNum;
  std::ofstream logFile[2];
  int loggingPhase = 0;

  /////////////////////////////////////////
  // High-level model
  // Source code information
  void *sysloop;
  void *layerLoop;
  int reductionOp;
  int mapOp;
  std::vector<void *> vecPreProcess;
  std::vector<void *> vecPostProcess;
  std::vector<void *> vecExp;
  std::vector<std::set<void *>> vecSetNotRelatedLoop;
  std::vector<std::set<void *>> vecSetRelatedLoop;
  std::map<void *, std::vector<int>> mapLoop2TCs;
  std::set<void *> setLoopIters;
  std::map<void *, int> mapIter2Tag;

  // Caching
  std::map<string, double> cacheModelVal;

  // Systolic array
  bool valid;
  std::vector<void *> vecPE;
  std::map<void *, void *> mapPE2Range;
  std::map<void *, void *> mapFeeder2Range;
  std::map<void *, void *> mapOuter2Range;

  std::map<void *, int> mapPE2Value;
  std::map<void *, int> mapFeeder2Value;
  std::map<void *, std::vector<int>> mapOuter2Value;

  /////////////////////////////////////////
  // High-level model
  void AnalyzeHighLevelModel();
  void CodegenGolden();
  void *CreateNestedLoopExpr(std::vector<void *>, void *scope,
                             std::map<void *, void *> &);
  void *CreateLinearizedExpr(void *);

  /////////////////////////////////////////
  // Low-level model
  void AnalyzeLowLevelModel(void *sysLoop);

  struct OuterLoops {
    std::vector<int> loop_bounds;
  } Outer;

  struct FeederLoops {
    std::vector<int> loop_bounds;
    string buffer_dim;
    int overlap_size;
    string iteration_domain;
    string access_pattern;
    int pe_iter_idx;
    int pe_iter_dim;
    int pe_iter_coeff;
    int pe_dim_no_r2_lb;
    int pe_dim_no_r2_ub;
  } Feeder_H, Feeder_V;

  struct PELoops {
    std::vector<int> loop_bounds;
    string reduce_op;
    string map_op;
  } PE;

  struct Loader {
    std::vector<int> loop_bounds;
    string block_dim;
    //  int overlap_size;
    //    string block_dim_iteration_domain;
    //    string block_dim_access_pattern;
    string block_size;
    string new_block_size;
    //    string block_size_iteration_domain;
    //    string block_size_access_pattern;
  } Loader_H, Loader_V;

  struct Drain {
    string reuse_num;
    int num_coalesced_words;
    string drain_out_order_ubs;
    string drain_buffer_order_ubs;
    string outer_loop_ubs;
    string out_to_buffer_map;
  } Drain_O;

  // FIXME:
  // int m_conv_params[10];
  int m_expr_ub[3];

 public:
  enum SYSTOLIC_IO { OUT, IN, WT };

  enum SYSTOLIC_PE_ARR { ROW, COL, SIMD };

  SystolicData(void *pragma, CSageCodeGen *ast, CMarsIrV2 *ir) {
    m_ir = ir;
    m_ast = ast;
    m_pragma = pragma;

    if ("auto" == m_ir->get_pragma_attribute(m_pragma, "auto")) {
      model = HIGH_LEVEL;
      reductionOp = 0;
      mapOp = 0;
      typeSize = 4;  // FIXME
      valid = true;
      layerNum = 1;

      vecExp.resize(3);
      vecSetNotRelatedLoop.resize(3);
      vecSetRelatedLoop.resize(3);
      vecPE.resize(3);

      mapPE2Range.clear();
      mapFeeder2Range.clear();
      mapOuter2Range.clear();

      mapPE2Value.clear();
      mapFeeder2Value.clear();
      mapOuter2Value.clear();
    } else {
      model = LOW_LEVEL;
      // Initialize data for low-level model
    }
  }

  // Copy constructor
  SystolicData(const SystolicData &that) {
    m_ir = that.m_ir;
    m_ast = that.m_ast;
    m_pragma = that.m_pragma;
    sysloop = that.sysloop;
    model = that.model;
    reductionOp = that.reductionOp;
    mapOp = that.mapOp;
    valid = that.valid;
    typeSize = that.typeSize;
    layerNum = that.layerNum;
    layerLoop = that.layerLoop;

    for (auto pe : that.vecPE) {
      vecPE.push_back(pe);
    }

    for (auto pair : that.mapPE2Range) {
      mapPE2Range[pair.first] = m_ast->CopyExp(pair.second);
    }

    for (auto pair : that.mapFeeder2Range) {
      mapFeeder2Range[pair.first] = m_ast->CopyExp(pair.second);
    }

    for (auto pair : that.mapOuter2Range) {
      mapOuter2Range[pair.first] = m_ast->CopyExp(pair.second);
    }

    for (auto pair : that.mapPE2Value) {
      mapPE2Value[pair.first] = pair.second;
    }

    for (auto pair : that.mapFeeder2Value) {
      mapFeeder2Value[pair.first] = pair.second;
    }

    for (auto pair : that.mapOuter2Value) {
      mapOuter2Value[pair.first] = pair.second;
    }

    for (auto exp : that.vecExp) {
      vecExp.push_back(exp);
    }

    for (auto s : that.vecSetNotRelatedLoop) {
      std::set<void *> newSet;
      for (auto e : s) {
        newSet.insert(e);
      }
      vecSetNotRelatedLoop.push_back(newSet);
    }

    for (auto s : that.vecSetRelatedLoop) {
      std::set<void *> newSet;
      for (auto e : s) {
        newSet.insert(e);
      }
      vecSetRelatedLoop.push_back(newSet);
    }

    for (auto pair : that.mapLoop2TCs) {
      mapLoop2TCs[pair.first] = pair.second;
    }

    for (auto e : that.setLoopIters) {
      setLoopIters.insert(e);
    }
  }

  ~SystolicData() {}

  // Functions
  /////////////////////////////////////////
  // General

  void SystolicTest(void *expr_ref, const std::vector<void *> &outer_loops,
                    const std::vector<void *> &pe_loops);

  void SimpleDelinearization(
      const MarsProgramAnalysis::CMarsExpression &me,
      std::vector<MarsProgramAnalysis::CMarsExpression> *vec_new_term);

  int AnalysisDelinearize(MarsProgramAnalysis::CMarsExpression *me,
                          std::vector<size_t> *old_steps,
                          std::vector<size_t> *new_dims,
                          std::vector<size_t> *new_ubs);

  int ApplyDelinearize(
      MarsProgramAnalysis::CMarsExpression *me,
      const std::vector<size_t> &old_steps, const std::vector<size_t> &new_dims,
      std::vector<MarsProgramAnalysis::CMarsExpression> *vec_new_indexes);

  void *GetInitializedNameFromPntrArrRefExp(void *_exp);

  void *GetMultiDimTypeByString(string typeName);

  int GetBinaryOperation(void *_exp);

  int GetConstantFromExp(void *_exp);

  std::vector<int> GetListFromAggInitializer(void *_exp);

  void GetLoopBounds(std::vector<int> *loop_bounds,
                     const std::vector<void *> &loops);

  string GetCoeffString(MarsProgramAnalysis::CMarsExpression new_idx,
                        std::vector<void *> var_order);
  string GetCoeffString(MarsProgramAnalysis::CMarsExpression new_idx,
                        std::vector<void *> var_order, int num_new_dims);

  int GetOverlapSize(void *ref_expr, const std::vector<void *> &outer_loops,
                     //  const std::vector<void *> &feeder_loops,
                     const std::vector<void *> &pe_loops,
                     const std::vector<int> &buffer_dim);

  string GetFeederIdxInfo(void *ref_expr,
                          const std::vector<void *> &outer_loops,
                          const std::vector<void *> &feeder_loops,
                          const std::vector<void *> &pe_loops,
                          std::vector<int> *buffer_dim);

  void SetDrainMap(void *ref_expr, const std::vector<void *> &loops,
                   const std::vector<void *> &outer_loops,
                   const std::vector<void *> &feeder_loops,
                   const std::vector<void *> &pe_loops, Drain *drain_o);

  /*
  void GetRightSideBounds(void *ref_expr,
          const std::vector<void *> &outer_loops,
          const std::vector<void *> &feeder_loops,
          const std::vector<void *> &pe_loops,
          FeederLoops &Feeder);
          */

  void SetPEIterIdx(int pe_iter_idx, FeederLoops *Feeder);
  void SetPEIterDim(void *ref_expr, const std::vector<void *> &pe_loops,
                    FeederLoops *Feeder);

  void *GetExpPEArrayOperand(void *loop, void *ref_expr_l, void *ref_expr_r);

  void CheckFeederLoopFeasibility(const std::vector<void *> &feeder_loops,
                                  void *ref_expr_o, void *ref_expr_h,
                                  void *ref_expr_v);

  void GetBlockDim(void *ref_expr, const std::vector<void *> &outer_loops,
                   const std::vector<void *> &feeder_loops,
                   const std::vector<void *> &pe_loops,
                   std::vector<int> *block_dim);

  void GetBlockSize(void *ref_expr, const std::vector<void *> &outer_loops,
                    const std::vector<void *> &feeder_loops,
                    const std::vector<void *> &pe_loops,
                    std::vector<int> *block_size);

  void GetBlockSize(void *ref_expr, const std::vector<void *> &outer_loops,
                    const std::vector<void *> &feeder_loops,
                    const std::vector<void *> &pe_loops,
                    std::vector<int> *new_block_size, FeederLoops *Feeder);

  void GetReuseNum(void *ref_expr, const std::vector<void *> &outer_loops,
                   const std::vector<void *> &feeder_loops,
                   const std::vector<void *> &pe_loops,
                   std::vector<int> *reuse_num);

  void GetPEIterCoeff(void *ref_expr, const std::vector<void *> &outer_loops,
                      const std::vector<void *> &feeder_loops,
                      const std::vector<void *> &pe_loops, FeederLoops *Feeder);

  int GetCoalescedWordsNum(void *ref_expr,
                           std::vector<int> *v_num_coalesced_words);

  // void GetExprUB(void *ref_h,
  //        void *ref_v,
  //        void *ref_o,
  //        const std::vector<void *> &loops);
  void GetExprUB(const std::vector<int> &v_block_dim_h,
                 const std::vector<int> &v_block_size_h,
                 const std::vector<int> &v_block_dim_v,
                 const std::vector<int> &v_block_size_v,
                 const std::vector<int> &v_block_dim_o,
                 const std::vector<int> &v_block_size_o);

  void *CreateSimpleForLoop(string iterName, void *bound, void *body,
                            void *scope);

  std::map<void *, std::vector<int>> GetLoopTripCounts(void *loop);

  void *GetFirstStatementFromNestedLoop(void *loop);

  void GetLoopItersFromExp(void *exp, std::set<void *> *setIters);

  void GetLoopMainItersFromExp(void *exp, std::set<void *> *setIters);

  float GetCompThrpt();

  float GetThrpt();

  int GetBRAMBlk();

  float GetRealMemoryUsage(SYSTOLIC_IO ioType);

  void GetBW(std::vector<float> *vecBW);

  float GetMaxBWperPort();

  int GetRoundUpPow2(int v) {
    return static_cast<int>(pow(2.0, ceil(log10(v) / log10(2))));
  }

  void AddPreProcessStmt(void *stmt) { vecPreProcess.push_back(stmt); }

  std::vector<void *> &GetPreProcessStmts() { return vecPreProcess; }

  void AddPostProcessStmt(void *stmt) { vecPostProcess.push_back(stmt); }

  std::vector<void *> &GetPostProcessStmts() { return vecPostProcess; }

  void *GetSysLoop() { return sysloop; }

  void *GetLayerLoop() { return layerLoop; }

  bool IsHighLevel() {
    if (model == HIGH_LEVEL) {
      return true;
    }
    return false;
  }

  void SetInvalid() { valid = false; }

  bool IsValid() { return valid; }

  int GetTypeSize() { return typeSize; }

  int GetLayerNum() { return layerNum; }

  std::map<void *, std::vector<int>> GetLoopTCs() { return mapLoop2TCs; }

  std::set<void *> GetLoopIters() { return setLoopIters; }

  std::map<void *, int> GetTagMap() { return mapIter2Tag; }

  void SetReductionOp(int op) { reductionOp = op; }

  int GetReductionOp() { return reductionOp; }
  string GetOpString(void *op);

  void SetMapOp(int op) { mapOp = op; }

  int GetMapOp() { return mapOp; }

  void SetExp(enum SYSTOLIC_IO io, void *exp) { vecExp[io] = exp; }

  void *GetExp(enum SYSTOLIC_IO io) { return vecExp[io]; }

  std::vector<void *> GetIdxExps(enum SYSTOLIC_IO io) {
    std::vector<void *> indexs;
    m_ast->get_index_from_pntr_ref(vecExp[io], &indexs);
    return indexs;
  }

  void SetRelatedLoop(enum SYSTOLIC_IO io, std::set<void *> s) {
    for (auto iter : setLoopIters) {
      if (s.find(iter) == s.end()) {
        vecSetNotRelatedLoop[io].insert(iter);
      } else {
        vecSetRelatedLoop[io].insert(iter);
      }
    }
  }

  std::set<void *> GetNotRelatedLoop(enum SYSTOLIC_IO io) {
    return vecSetNotRelatedLoop[io];
  }

  std::set<void *> GetRelatedLoop(enum SYSTOLIC_IO io) {
    return vecSetRelatedLoop[io];
  }

  void SetSystolicPEArr(enum SYSTOLIC_PE_ARR idx, void *loopIter, int value) {
    assert(value != 0);
    vecPE[idx] = loopIter;
    mapPE2Value[loopIter] = value;
  }

  std::vector<void *> GetvecPE() { return vecPE; }

  void *GetSystolicPE(enum SYSTOLIC_PE_ARR idx) { return vecPE[idx]; }

  std::map<void *, int> GetSystolicPE() { return mapPE2Value; }

  int GetSystolicPESize(enum SYSTOLIC_PE_ARR idx) {
    return mapPE2Value[vecPE[idx]];
  }

  bool IsSystolicPE(void *iter) {
    for (int i = 0; i < 3; i++) {
      if (vecPE[i] == iter) {
        return true;
      }
    }
    return false;
  }

  int GetDSPNum() {
    int ret = 1;
    for (int i = 0; i < 3; i++) {
      ret *= mapPE2Value[vecPE[i]];
    }
    return ret;
  }

  int AddFeeder(void *loopIter, int value) {
    if (mapFeeder2Value.find(loopIter) != mapFeeder2Value.end()) {
      return 1;
    }

    mapFeeder2Value[loopIter] = value;

    return 0;
  }

  int AddTag(void *loopIter, int value) {
    if (mapIter2Tag.find(loopIter) != mapIter2Tag.end()) {
      return 1;
    }

    mapIter2Tag[loopIter] = value;

    return 0;
  }

  void CleanFeeder() {
    if (mapFeeder2Value.empty()) {
      return;
    }
    mapFeeder2Value.clear();
    mapOuter2Value.clear();
  }

  bool IsFeeder(void *loopIter) {
    if (mapFeeder2Value.find(loopIter) != mapFeeder2Value.end()) {
      return true;
    }
    return false;
  }

  std::map<void *, int> GetFeeders() { return mapFeeder2Value; }

  int AddOuter(void *loopIter, std::vector<int> values) {
    if (mapOuter2Value.find(loopIter) != mapOuter2Value.end()) {
      return 1;
    }

    mapOuter2Value[loopIter] = values;

    return 0;
  }

  bool IsOuter(void *loopIter) {
    if (mapOuter2Value.find(loopIter) != mapOuter2Value.end()) {
      return true;
    }
    return false;
  }

  std::map<void *, std::vector<int>> GetOuters() {
    mapOuter2Value.clear();
    for (auto pair : mapLoop2TCs) {
      std::vector<int> tcs = pair.second;
      std::vector<int> outerSize;

      for (auto tc : tcs) {
        int block = 1;
        if (IsSystolicPE(pair.first)) {
          block *= mapPE2Value[pair.first];
        }
        if (mapFeeder2Value.find(pair.first) != mapFeeder2Value.end()) {
          block *= mapFeeder2Value[pair.first];
        }

        int res = tc / block;
        outerSize.push_back(ceil(static_cast<float>(res)));
      }

      // xuechao: 11/15/2018
      /* FIXME: workaround to remove outer loop
       * iterators with upper bounds equal to 1
       */
      //    if (outerSize[0] > 1)
      { mapOuter2Value[pair.first] = outerSize; }
      // mapOuter2Value[pair.first] = outerSize;
    }
    return mapOuter2Value;
  }

  void Analyze();

  void DumpXML(string filename);

  string toString();

  void OpenLogFile(string fileName) {
    logFile[loggingPhase].open(fileName, std::ios::out);
  }

  void CloseLogFile() {
    logFile[loggingPhase].close();
    loggingPhase++;
  }

  void WritelnToLogFile(string str) { logFile[loggingPhase] << str << endl; }
};

#endif  // TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_SYSTOLICDATA_H_
