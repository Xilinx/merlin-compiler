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


#ifndef TRUNK_SOURCE_OPT_TOOLS_PERFORMANCE_ESTIMATION_SRC_PERFORMANCE_ESTIMATION_H_
#define TRUNK_SOURCE_OPT_TOOLS_PERFORMANCE_ESTIMATION_SRC_PERFORMANCE_ESTIMATION_H_
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "stdio.h"
#include "stdlib.h"

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "mars_ir.h"
#include "mars_opt.h"
#include "program_analysis.h"

#include "PolyModel.h"
#include "codegen.h"
#include "locations.h"
#include "report.h"
#include "rose.h"
#include "tldm_annotate.h"

// using std::map;
// using namespace rapidjson;
// using namespace SageInterface;
// using namespace SageBuilder;
// using namespace MarsProgramAnalysis;
using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;

//  map<string, map<string, string>> g_AnnRpt;
//  map<string, string> TopoIDExecMap;

#define ENABLE_SIM 0
#define PERF_EST_DBG 0
#define PRINTLOG 0
#define JSON_MSG_OUTPUT 0

#define ENABLE_MEM_CONFLICT_UPDATE 1

#define A2S(X) codegen->UnparseToString(X).c_str()

typedef enum blocktype {
  LOOP = 0,
  FNCALL = 1,
  FNDECL = 2,
  MEMCPY = 3,
  IF = 4,
  MEMREF = 5,
  BASICBLOCK = 6,
  FNCALL_STMT = 7,
  SUBKERNEL = 8,
  TOPKERNEL = 9,
  AUTOKERNEL = 10,
  TASK = 11
} BlockType;
typedef struct __ExtMemParam {
  double RBW_GBs;
  double WBW_GBs;
  double RLat_ns;
  double WLat_ns;
  double RandLat_ns;
  double FPGACLK_GHz;
  double MAX_BITWIDTH;
  double LATENCY;
} ExtMemParam;
class CPerfEstBlock;

//  CPerfEstBlock *MakeNewBlock(CSageCodeGen *codegen,
//                            vector<CPerfEstBlock *> &AllBlockList,
//                            vector<CPerfEstBlock *> &AllFuncDeclList,
//                            BlockType Type, void *AstPtr, CPerfEstBlock
//                            *Parent, void *ast_node = nullptr);
//  CPerfEstBlock *MakeNewBlock_AOCL(CSageCodeGen *codegen, string Tool,
//                                 vector<CPerfEstBlock *> &AllBlockList,
//                                 vector<CPerfEstBlock *> &AllFuncDeclList,
//                                 BlockType Type, void *AstPtr,
//                                 CPerfEstBlock *Parent,
//                                 map<string, string> TopoIDExecMap,
//                                 map<string, map<string, string>>
//                                 &KernelGroup, void *TopKernel, void *ast_node
//                                 = nullptr);
CPerfEstBlock *MakeNewBlocks(
    CSageCodeGen *codegen, string Tool, vector<CPerfEstBlock *> *AllBlockList,
    vector<CPerfEstBlock *> *AllFuncDeclList, BlockType Type, void *AstPtr,
    CPerfEstBlock *Parent, map<string, string> TopoIDExecMap,
    const map<string, map<string, string>> &KernelGroup, void *TopKernel,
    void *ast_node = nullptr);
void ReadAnnotatedReport(CSageCodeGen *codegen, string AnnRptPath,
                         const vector<CPerfEstBlock *> &AllBlockList,
                         const vector<CPerfEstBlock *> &AllFuncDeclList,
                         string Mode, string Tool, void *ast_node = nullptr);
ExtMemParam ReadExtMemParam(string ExtMemParamPath);
#if ENABLE_SIM
void AddCycleModel(CSageCodeGen *codegen,
                   vector<CPerfEstBlock *> AllFuncDeclList,
                   ExtMemParam *ExtMemParamData);
#endif
// void FindFnCallDuplicates(CSageCodeGen *codegen, CPerfEstBlock *IfBlockPtr,
//                           map<CPerfEstBlock *, int> &FnCallList);
void MultStaticEstAllChild(CSageCodeGen *codegen, CPerfEstBlock *BlockPtr,
                           int64_t MulFactor, bool ExceptHeadNode,
                           bool doNotShowHead = false);
void UpdateFnCallCnt(CSageCodeGen *codegen, CPerfEstBlock *FnDeclBlock,
                     int64_t MulFactor);
// void ComputeStaticCycle(CSageCodeGen *codegen, CPerfEstBlock *TopFuncBlock,
//                         ExtMemParam *ExtMemParamData,
//                         vector<CPerfEstBlock *> &AllBlockList);
// void ComputeStaticCycle_AOCL(CSageCodeGen *codegen,
//                              CPerfEstBlock *TopFuncBlock,
//                              ExtMemParam *ExtMemParamData,
//                              vector<CPerfEstBlock *> &AllBlockList,
//                              string Tool);
void ComputeStaticCycles(CSageCodeGen *codegen, ExtMemParam *ExtMemParamData,
                         const vector<CPerfEstBlock *> &AllBlockList);
void AddSimOutputMaker(CSageCodeGen *codegen, string PerfRptPath,
                       vector<CPerfEstBlock *> AllFuncDeclList,
                       CPerfEstBlock *TopFuncBlock, float FPGACLK_GHz,
                       string Mode, bool Print2Screen);
void MergeFiles(string SimOutputPath, string AnnRptPath, string PerfRptPath);

vector<void *> FindExtPortList(CSageCodeGen *codegen,
                               vector<void *> ParentExtPortList, void *FuncCall,
                               void *FuncDecl);
// void FindAllFuncArgInUnrolledLoop(CSageCodeGen *codegen, void *UnrolledLoop,
//                                   vector<void *> &UnrollArgList);
// int CheckDependencyFUNC(CSageCodeGen *codegen,
//                         const vector<CPerfEstBlock *> ParallelSchedule,
//                         const CPerfEstBlock *NewBlock);
// void MakeDRAMCycleExprFromByte(CSageCodeGen *codegen, void *scope,
//                                void *ExtPort, void *ByteExpr, float BW_GBs,
//                                float Lat_ns, float FPGACLK_GHz,
//                                void *&CycleExpr, void *&DataOnlyExpr);
// void MakeDRAMCycleExprFromTC(CSageCodeGen *codegen,
//                              void *scope, void *ExtPort,
//                              void *TCExpr, float BW_GBs, float Lat_ns,
//                              float FPGACLK_GHz, void *&CycleExpr,
//                              void *&DataOnlyExpr, void *&ByteExpr);
// void MakeTCByLoopFlatten(CSageCodeGen *codegen,
//                          CPerfEstBlock *PipeParentBlock,
//                          CPerfEstBlock *CurrentBlock,
//                          void *&TCExpr,
//                          bool ignore_unrolled);
// void MakeTCByLoopFlatten(CSageCodeGen *codegen,
//                          CPerfEstBlock *PipeParentBlock,
//                          CPerfEstBlock *CurrentBlock,
//                          int64_t &TCStatic,
//                          bool ignore_unrolled);
void *FindExtPortFromExp(CSageCodeGen *codegen, vector<void *> ExtPortList,
                         void *exp);
void FindRWMemcpy(CSageCodeGen *codegen, void *MemcpyStmt,
                  vector<void *> ExtPortList, void **ReadExtPort,
                  int read_arg_idx, void **WriteExtPort, int write_arg_idx);
CPerfEstBlock *GetFuncDeclBlock(CSageCodeGen *codegen, CPerfEstBlock *Block);
CPerfEstBlock *GetInnermostLoopBlock(CSageCodeGen *codegen,
                                     CPerfEstBlock *Block);
bool CheckIfConsecutiveAccess(CSageCodeGen *codegen, void *InnerLoop,
                              void *MemRefExpr);
// void MakeDRAMCycleExprNonConsecuFromTC(CSageCodeGen *codegen, void *scope,
//                                        void *ExtPort, void *TCExpr,
//                                        float RandLat_ns, float FPGACLK_GHz,
//                                        void *&CycleExpr, void *&DataOnlyExpr,
//                                        void *&ByteExpr);
// void AppendCycleByteExprToCode(CSageCodeGen *codegen, void *CycleExpr,
//                                void *DataOnlyExpr, void *ByteExpr,
//                                CPerfEstBlock *Block, bool max_cycle);
void CreateHelperFunctions(CSageCodeGen *codegen, void *pKernel);
string GetNiceLookingName(CSageCodeGen *codegen, CPerfEstBlock *BlockTemp);
void DBGPrintStructure(CSageCodeGen *codegen,
                       vector<CPerfEstBlock *> AllFuncDeclList);
map<void *, int> CheckBurstLoop(CSageCodeGen *codegen, void *pKernel,
                                string AnnRptPath);
void FastDse(CSageCodeGen *codegen,
             map<string, map<string, string>> KernelGroup, string ChangeJson,
             string HistoryJson, string OutputJson, string Tool);

class CPerfEstBlock {
 public:
  int64_t AutoKernelDepth;
  int64_t II = 0;
  int64_t IIOrg = 0;
  int64_t IL = 0;
  int64_t UnrollFactor = 1;
  int64_t InstanceNumber = 1;
  int64_t RptLat = 1;
  int64_t GlueLatency = 0;
  int64_t LogicLatency = 0;
  int64_t GlueLatencyIteration = 0;
  int64_t LogicLatencyIteration = 0;
  int64_t LatencyMax = 0;
  int64_t IterationLatencyMax = 0;
  int64_t m_Bitwidth = 0;
  double MemCflFactor = 0.0;
  int64_t CalculatedFlattenIL = 1;
  int64_t InnerLoopTripcountProduct = 1;
  int64_t FlattenTripCount = 1;
  int64_t CycleStatic = 0;
  int64_t CycleStaticTmp = 0;  //  this used for total cycles cross functions
  int64_t CycleUnitStatic = 0;
  int64_t CalculatedCycles = 0;
  int64_t TCStatic = -1;
  int64_t DRAMDataOnlyCycleStatic = 0;
  int64_t DRAMDataByteStatic = 0;
  int64_t BurstWidth = 0;
  int64_t VendorLatency = 1;
  int64_t AccCycles = 0;
  int64_t ComputeCycles = 0;
  int64_t BurstCycles = 0;
  int64_t BurstCyclesVarRef = 0;
  int64_t BurstDataSize = 0;
  int64_t BurstBitWidth = 65536;
  double BurstFactor = 1.0;
  int64_t BurstTypeSize = 0;
  int64_t BurstVarRefDataSize = 0;
  int64_t BurstVarRefBitWidth = 0;
  double BurstVarRefFactor = 0;
  int64_t MerlinMemcpyDataSize = 0;
  int64_t MerlinMemcpyBitwidth = 0;

  int Burst = 0;            //  use int to distiguish different burst type
  int MemoryReadWrite = 0;  //  0, no memory acccess
                            //  1, read
                            //  2, write
                            //  3, read and write
  bool IsInlineFunc = false;
  bool HaveNotUnrollLoop = false;
  bool isParentUnrolled = false;
  bool IsPoison = false;
  bool ShouldBorrow = false;
  bool ExecFlow = false;  //  0 serial exec; 1 parallel exec
  bool HaveMemoryBurstChild = false;
  bool Flattened = false;
  bool FlattenedSon = false;
  bool MemCflUpdateFlag;
  bool IsIFTaken;
  bool PureBlock = true;
  bool UnrollFailed = false;
  bool IsDead = false;
  bool IsAlteraMemoryCoalescing = false;
  bool Pipelined = false;
  bool IsFlatten = false;
  bool IsFlattenToPipeline = false;
  bool IsOutmostFlattenLoop = false;
  bool IsInnermostFlattenLoop = false;
  bool DataUpdated = false;

  string OutmostFlattenID = "";
  string InnermostFlattenID = "";
  string FlattenID = "";
  string TopoID;
  string KernelName;
  string DependencyVariableList;

  void *TotCycleDecl = nullptr;
  void *UnitCycleDecl = nullptr;
  void *CycleDecl = nullptr;
  void *TCDecl = nullptr;
  void *DRAMDataOnlyCycleDecl = nullptr;
  void *TotDRAMDataByteDecl = nullptr;
  void *AstPtr;
  void *AutoKernelParentScope;
  void *DRAMDataByteDecl = nullptr;
  void *Kernel = nullptr;

  BlockType Type;
  CPerfEstBlock *Parent;
  CPerfEstBlock *FuncDecl;
  CSageCodeGen *codegen;

  string Tool = "";

  vector<void *> UnrollCycleDecl;
  vector<void *> ExtPortList;
  vector<void *> BurstExtPortList;
  vector<vector<string>> StateVector;
  bool UpdatedByADB = false;
  map<void *, int64_t> DataSizeDict;
  map<void *, int64_t> BitwidthDict;

  map<CPerfEstBlock *, int> FnCallCnt;
  map<string, string> RequireToCheckInstanceAndLoopID;
  vector<vector<CPerfEstBlock *>> Schedule;
  vector<vector<CPerfEstBlock *>> ScheduleElse;
  vector<CPerfEstBlock *> ScheduleSpawn;
  map<void *, map<string, int64_t>> GlobalMemoryAttributes;

  CPerfEstBlock(CSageCodeGen *codegen, BlockType Type, void *AstPtr,
                CPerfEstBlock *Parent, string Tool = "");
  ~CPerfEstBlock();

  int64_t GetBitwidth();
  void PrintDataSizeDict(CSageCodeGen *codegen);
  void PrintBitwidthDict(CSageCodeGen *codegen, int depth);
  void GetExecFlowFromInput(map<string, string> TopoIDExecMap, string Tool);
  //  void AddChildBlock(vector<CPerfEstBlock *> &AllBlockList,
  //                     vector<CPerfEstBlock *> &AllFuncDeclList,
  //                     void *ast_node = nullptr);
  void AddChildBlock_AOCL(string Tool, vector<CPerfEstBlock *> *AllBlockList,
                          vector<CPerfEstBlock *> *AllFuncDeclList,
                          map<string, string> TopoIDExecMap,
                          const map<string, map<string, string>> &KernelGroup,
                          void *TopKernel, void *ast_node = nullptr);
  //  void AdjustScheduleUnrollLoop(CPerfEstBlock *UnrolledLoop);
  void
  ReadAnnotatedReportHelper(map<string, map<string, string>> AnnRpt,
                            const vector<CPerfEstBlock *> &AllBlockList,
                            const vector<CPerfEstBlock *> &AllFuncDeclList);
  void ReSchedule(vector<vector<CPerfEstBlock *>> *SchedulePtr);
  void ReScheduleByADB(CSageCodeGen *codegen,
                       vector<vector<CPerfEstBlock *>> *SchedulePtr);
  void ReScheduleByDataflow(CSageCodeGen *codegen,
                            vector<vector<CPerfEstBlock *>> *SchedulePtr,
                            void *AstPtr);
  void FindPossibleDependencyVariableInScope();

#if ENABLE_SIM
  void AddCycleModel_VarDecl(CPerfEstBlock *PipeParent);
  void AddDRAMModel(ExtMemParam *ExtMemParamData, CPerfEstBlock *PipeParent);
  void AddChildCycleModel();
  void AddSelfCycleModel();
#endif
  void ComputeStaticCyclesForLoop(CSageCodeGen *codegen,
                                  const vector<CPerfEstBlock *> &AllBlockList);
  void ComputeGlueCyclesForLoop(CSageCodeGen *codegen,
                                vector<vector<CPerfEstBlock *>> *SchedulePtr,
                                const vector<CPerfEstBlock *> &AllBlockList);
  void ComputeStaticCyclesForFunc(CSageCodeGen *codegen,
                                  vector<vector<CPerfEstBlock *>> *SchedulePtr,
                                  const vector<CPerfEstBlock *> &AllBlockList);
  void CheckIfUnrollSuccess(CSageCodeGen *codegen,
                            const vector<CPerfEstBlock *> &AllBlockList);
  void MarkParentAsRequireToCheck(CSageCodeGen *codegen, int64_t InstanceNumber,
                                  const vector<CPerfEstBlock *> &AllBlockList);
  //  void ComputeStaticCycleHelper(vector<CPerfEstBlock *> &AllFuncDeclList,
  //                                ExtMemParam *ExtMemParamData,
  //                                CPerfEstBlock *PipeParent, int depth,
  //                                vector<CPerfEstBlock *> &AllBlockList);
  //  void ComputeStaticCycleHelper_AOCL(CSageCodeGen *codegen,
  //                                     vector<CPerfEstBlock *>
  //                                     &AllFuncDeclList, ExtMemParam
  //                                     *ExtMemParamData, CPerfEstBlock
  //                                     *PipeParent, vector<CPerfEstBlock *>
  //                                     &AllBlockList);
  void ComputeStaticCycleHelper(
      CSageCodeGen *codegen, vector<CPerfEstBlock *> *AllFuncDeclList,
      ExtMemParam *ExtMemParamData, CPerfEstBlock *PipeParent,
      const vector<CPerfEstBlock *> &AllBlockList,
      vector<CPerfEstBlock *> TopKernelBlock = vector<CPerfEstBlock *>());

  void ComputeTotalStaticCycle(
      CSageCodeGen *codegen, vector<CPerfEstBlock *> *AllFuncDeclList,
      CPerfEstBlock *PipeParent, int64_t factor,
      vector<CPerfEstBlock *> TopKernelBlock = vector<CPerfEstBlock *>());
  void ComputeMemoryAccessDataSize(CSageCodeGen *codegen,
                                   ExtMemParam *ExtMemParamData);
  void ComputeMemoryAccessBitwidth(CSageCodeGen *codegen,
                                   vector<void *> ExtPortList,
                                   ExtMemParam *ExtMemParamData, int depth);
  void UpdateCycleByMemoryBurst(CSageCodeGen *codegen,
                                ExtMemParam *ExtMemParamData, int depth);
  void UpdateCycleByMemoryBurstTop(
      CSageCodeGen *codegen,
      vector<CPerfEstBlock *> TopKernelBlock = vector<CPerfEstBlock *>());
  void UpdateBurstCyclesBySelf(CSageCodeGen *codegen,
                               vector<CPerfEstBlock *> *AllFuncDeclList);
  void UpdateBurstCyclesFactor(CSageCodeGen *codegen,
                               const vector<CPerfEstBlock *> &AllFuncDeclList);
  void UpdateBurstCycles(CSageCodeGen *codegen,
                         const vector<CPerfEstBlock *> &AllFuncDeclList,
                         double ComboBurstFactor);
  void
  GetInnerLoopTripcountProduct(CSageCodeGen *codegen,
                               const vector<CPerfEstBlock *> &AllBlockList);
  bool IfEmptyLoop(CSageCodeGen *codegen);
  void SetParentAttributeHaveUnrollLoopOnly(
      CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList);
  int64_t GetMemcpyCycles(CSageCodeGen *codegen);
  int CheckIfRealMemcpy(CSageCodeGen *codegen, void *AstPtr, void **ExtPort,
                        void **ByteExprMemcpy, CPerfEstBlock *Parent);
  bool CheckFlattenToPipeline(CSageCodeGen *codegen,
                              const vector<CPerfEstBlock *> &AllBlockList,
                              map<string, map<string, string>> AnnRpt);
  bool CheckIfInnermostFlattenLoop(CSageCodeGen *codegen,
                                   const vector<CPerfEstBlock *> &AllBlockList,
                                   map<string, map<string, string>> AnnRpt);
  void SetInnermostFlattenID(CSageCodeGen *codegen,
                             const vector<CPerfEstBlock *> &AllBlockList,
                             map<string, map<string, string>> AnnRpt);
  void GetFlattenTripCount(CSageCodeGen *codegen,
                           const vector<CPerfEstBlock *> &AllBlockList);
  //  int64_t ComputeStaticCycleFromSchedule(vector<vector<CPerfEstBlock *>>
  //  *Schedule, void *AstPtr);
  vector<int64_t> ComputeStaticCycleFromSchedule(
      CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *Schedule,
      vector<CPerfEstBlock *> *ScheduleSpawn, void *AstPtr,
      vector<CPerfEstBlock *> *AllFuncDeclList, ExtMemParam *ExtMemParamData,
      CPerfEstBlock *PipeParent, const vector<CPerfEstBlock *> &AllBlockList);
  void ComputeTotalStaticCycleFromSchedule(
      CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *Schedule,
      void *AstPtr, vector<CPerfEstBlock *> *AllFuncDeclList,
      CPerfEstBlock *PipeParent, int64_t factor);

  void Add2SimOutputList(vector<CPerfEstBlock *> *OutputBlockList,
                         CPerfEstBlock *PipeParent);

  //  void CollectStmts(CSageCodeGen *codegen, int depth);

  void PrintSchedule();
  void test();
};
// bool GetUnrollFactorFromVendorPragma(CSageCodeGen *codegen, void *loop,
//                                     string Tool, int64_t &factor);
int64_t GetUnrollFactorFromVendorPragma(CSageCodeGen *codegen, void *loop,
                                        string Tool);
#endif  // TRUNK_SOURCE_OPT_TOOLS_PERFORMANCE_ESTIMATION_SRC_PERFORMANCE_ESTIMATION_H_
