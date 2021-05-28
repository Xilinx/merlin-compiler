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
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "stdio.h"
#include "stdlib.h"

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "mars_ir.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "loop_parallel.h"

#include "PolyModel.h"
#include "codegen.h"
#include "locations.h"
#include "performance_estimation.h"
#include "report.h"
#include "rose.h"
#include "tldm_annotate.h"

#define TEST_SUBKERNEL 1
int64_t GetUnrollFactorFromVendorPragma(CSageCodeGen *codegen, void *loop,
                                        string Tool) {
  int64_t factor = 1;
  int64_t trip_count = 0;
  int64_t CurrentLoopTC = 0;
  codegen->get_loop_trip_count(loop, &trip_count, &CurrentLoopTC);
  if (Tool == "sdaccel") {
    void *body = codegen->GetLoopBody(loop);
    vector<void *> stmts = codegen->GetChildStmts(body);
    if (!stmts.empty()) {
      void *first_stmt = stmts[0];
      if (codegen->IsPragmaDecl(first_stmt) != 0) {
        string pragma_s = codegen->GetPragmaString(first_stmt);
        vector<string> v_s = str_split(pragma_s, ' ');
        if (!v_s.empty()) {
          if (v_s[0] != "HLS" || (v_s[1] != "unroll" && v_s[1] != "UNROLL")) {
            return 1;
          }
          if (v_s.size() == 2) {
            factor = trip_count;
          } else if (v_s.size() == 3) {
            factor = atoi(v_s[2].c_str());
          } else {
            factor = trip_count;
          }
          return factor;
        }
      }
    } else {
      factor = trip_count;
      return factor;
    }
  }
  return 1;
}

map<string, map<string, string>> g_AnnRpt;

//  ************************************************************
//  check if the node is loop node
//  ************************************************************
bool IsLoopNode(CSageCodeGen *codegen, void *Node) {
  return static_cast<bool>(codegen->IsForStatement(Node)) ||
         static_cast<bool>(codegen->IsWhileStatement(Node)) ||
         static_cast<bool>(codegen->IsDoWhileStatement(Node));
}

CPerfEstBlock::CPerfEstBlock(CSageCodeGen *codegen, BlockType Type,
                             void *AstPtr, CPerfEstBlock *Parent, string Tool) {
  this->codegen = codegen;
  this->Type = Type;
  this->AstPtr = AstPtr;
  this->Parent = Parent;

  this->FuncDecl = nullptr;
  this->II = 0;
  this->IL = 0;
  this->UnrollFactor = 0;
  this->Flattened = false;
  this->RptLat = 0;
  this->Pipelined = false;

  this->TotCycleDecl = nullptr;
  this->UnitCycleDecl = nullptr;
  this->CycleDecl = nullptr;
  this->TCDecl = nullptr;
  this->DRAMDataOnlyCycleDecl = nullptr;
  this->TotDRAMDataByteDecl = nullptr;
  this->DRAMDataByteDecl = nullptr;

  this->CycleStatic = 0;
  this->CycleUnitStatic = 0;
  this->TCStatic = -1;
  this->DRAMDataOnlyCycleStatic = 0;
  this->DRAMDataByteStatic = 0;
  this->Tool = Tool;

  TopoID = getTopoLocation(codegen, static_cast<SgNode *>(AstPtr));

  isParentUnrolled = false;

  //  printf("New block made! (Type:%d) : %s\n", Type, A2S(AstPtr));
}

CPerfEstBlock::~CPerfEstBlock() {}

//  ************************************************************
//  print schedule information
//  ************************************************************
void CPerfEstBlock::PrintSchedule() {
  for (int i = 0; i < 2; i++) {
    vector<vector<CPerfEstBlock *>> *SchedulePtr;

    string prefix;
    if (i == 1) {
      //  this case is only used for else statement
      if (Type == IF) {
        SchedulePtr = &ScheduleElse;
        prefix = "EL";
      } else {
        prefix = "  ";
        continue;
      }
    } else {
      if (Type == IF) {
        prefix = "IF";
      } else {
        prefix = "  ";
      }
      SchedulePtr = &Schedule;
    }
    if (Type == FNDECL || Type == SUBKERNEL || Type == TOPKERNEL) {
      printf("Function: TopoID:%s Unit Cycles:%ld "
             "LogicLatency:%ld GlueLatency:%ld\n",
             TopoID.c_str(), CycleUnitStatic, LogicLatency, GlueLatency);
    }
    for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
      for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
        printf("%s(%d,%d) ", prefix.c_str(), static_cast<int>(j),
               static_cast<int>(k));
        CPerfEstBlock *BlockTemp = (*SchedulePtr)[j][k];

        if (BlockTemp->Type == LOOP) {
          if (static_cast<bool>(codegen->IsForStatement(BlockTemp->AstPtr))) {
            printf("%s TopoID:%s II:%ld IL:%ld UF:%ld Flat:%d "
                   "TCStatic:%ld Unit Cycles:%ld Total Cycles:%ld\n",
                   GetNiceLookingName(codegen, BlockTemp).c_str(),
                   BlockTemp->TopoID.c_str(), BlockTemp->II, BlockTemp->IL,
                   BlockTemp->UnrollFactor,
                   static_cast<int>(BlockTemp->Flattened), BlockTemp->TCStatic,
                   BlockTemp->CycleUnitStatic, BlockTemp->CycleStatic);
          } else {  //  while stmt
            printf("%s TopoID:%s II:%ld IL:%ld \n",
                   GetNiceLookingName(codegen, BlockTemp).c_str(),
                   BlockTemp->TopoID.c_str(), BlockTemp->II, BlockTemp->IL);
          }
        } else {
          printf("%s TopoID:%s Unit Cycles=%ld Total Cycles:%ld\n",
                 GetNiceLookingName(codegen, BlockTemp).c_str(),
                 BlockTemp->TopoID.c_str(), BlockTemp->CycleUnitStatic,
                 BlockTemp->CycleStatic);
        }
        cout << "            LogicLatency: " << BlockTemp->LogicLatency
             << ", GlueLatency: " << BlockTemp->GlueLatency
             << ", LogicLatencyIteration: " << BlockTemp->LogicLatencyIteration
             << ", GlueLatencyIteration: " << BlockTemp->GlueLatencyIteration
             << endl;

        if (BlockTemp->Type == LOOP && BlockTemp->II != 0) {
          //  Do not print the sub-structure of loops that has been pipelined
        } else {
          BlockTemp->PrintSchedule();
        }
      }
    }
  }
}

/*
int get_coalescing_width(CSageCodeGen *ast, void *&block, long &val) {
  if (block == nullptr) {
    return 0;
  }

  string coalescing_key = "COALESCING_WIDTH";

  bool found = false;
  while ((block != nullptr) && !found) {
    found = (ast->get_block_pragma_val(block, coalescing_key, val) != 0);
    block = ast->GetParent(block);
  }

  while (block != nullptr) {
    if (ast->IsLoopStatement(block) != 0) {
      break;
    }
    block = ast->GetParent(block);
  }

  if (ast->IsLoopStatement(block) == 0) {
    block = nullptr;
  }

  return static_cast<int>(found);
}
*/

void CPerfEstBlock::Add2SimOutputList(vector<CPerfEstBlock *> *OutputBlockList,
                                      CPerfEstBlock *PipeParent) {
  if (Type == LOOP || Type == FNCALL || Type == FNDECL || Type == MEMCPY ||
      Type == TOPKERNEL || Type == SUBKERNEL || Type == AUTOKERNEL) {
    OutputBlockList->push_back(this);
  }

  CPerfEstBlock *PipeParent_temp =
      (PipeParent == nullptr && Type == LOOP && II > 0) ? this : PipeParent;
  if (PipeParent_temp != nullptr) {
    Pipelined = true;
  }

  for (int i = 0; i < 2; i++) {
    vector<vector<CPerfEstBlock *>> *SchedulePtr;
    vector<CPerfEstBlock *> *SchedulePtrSpawn;
    if (i == 1) {
      //  this case is only used for else statement
      if (Type == IF) {
        SchedulePtr = &ScheduleElse;
      } else {
        continue;
      }
    } else {
      SchedulePtr = &Schedule;
    }
    SchedulePtrSpawn = &ScheduleSpawn;

    for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
      for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
        CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
        ChildBlock->Add2SimOutputList(OutputBlockList, PipeParent_temp);
      }
    }

    for (size_t j = 0; j < (*SchedulePtrSpawn).size(); j++) {
      CPerfEstBlock *ChildBlock = (*SchedulePtrSpawn)[j];
      ChildBlock->Add2SimOutputList(OutputBlockList, PipeParent_temp);
    }
  }
}

//  ************************************************************
//  transfer string to int64_t data type
//  ************************************************************
int64_t ParseString2LongLong(string Input) {
  if (Input == "yes") {
    return 1;
  } else if (Input == "no" || Input == "-") {
    return 0;
  } else {
    return atoll(Input.c_str());
  }
  //  note: will return 0 if some kind of string
}
//  ************************************************************
//  transfer string to int64_t data type, if not digital, return -1
//  ************************************************************
int64_t ParseString2LongLong_1(string Input) {
  if (Input == "yes") {
    return 1;
  } else if (Input == "no" || Input == "-") {
    return 0;
  } else if (Input == "?") {
    return -1;
  } else if (atoll(Input.c_str()) == 0) {
    return -1;
  } else {
    return atoll(Input.c_str());
  }
  //  note: will return 0 if some kind of string
}

void NullifyPipelinedParent(CPerfEstBlock *Parent,
                            const vector<CPerfEstBlock *> &AllBlockList) {
  if (Parent != nullptr) {
    Parent->II = 0;
    Parent->Pipelined = false;
    if (Parent->Parent != nullptr) {
      NullifyPipelinedParent(Parent->Parent, AllBlockList);
    } else if (Parent->Type == FNDECL) {
      for (auto b : AllBlockList) {
        if (b->Type == FNCALL && b->FuncDecl == Parent) {
          NullifyPipelinedParent(b->Parent, AllBlockList);
        }
      }
    }
  }
}

//  ************************************************************
//  set innermost flatten loop ID to all flattend loops
//  ************************************************************
void CPerfEstBlock::SetInnermostFlattenID(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList,
    map<string, map<string, string>> AnnRpt) {
  PRESTP("EST", "--------Set innermost flatten id...");
  CPerfEstBlock *Block = this;
  while (true) {
    string TopoID = Block->TopoID;
    bool Found = false;
    for (auto OneBlock : AllBlockList) {
      string BlockID = OneBlock->TopoID;
      string FlattenID = AnnRpt[BlockID]["flatten-id"];
      if (TopoID == FlattenID) {
        Block = OneBlock;
        Found = true;
        break;
      }
    }
    if (!Found) {
      this->InnermostFlattenID = TopoID;
      PRESTP("EST", "--------Set id = " + TopoID);
      return;
    }
  }
}

//  ************************************************************
//  check if current loop is innermost flatten loop
//  ************************************************************
bool CPerfEstBlock::CheckIfInnermostFlattenLoop(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList,
    map<string, map<string, string>> AnnRpt) {
  //  PRESTP("EST", "--------Check if innermost flatten loop...");
  for (auto OneBlock : AllBlockList) {
    string BlockID = OneBlock->TopoID;
    string FlattenID = AnnRpt[BlockID]["flatten-id"];
    if (this->TopoID == FlattenID) {
      return false;
    }
  }
  //  PRESTP("EST", "--------Is innermost flatten loop...");
  return true;
}

//  ************************************************************
//  check if the flatten loop flattend to pipelinep
//  ************************************************************
bool CPerfEstBlock::CheckFlattenToPipeline(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList,
    map<string, map<string, string>> AnnRpt) {
  PRESTP("EST", "--------Check if flatten to pipeline...");
  CPerfEstBlock *Block = this;
  string TopoID;
  while (true) {
    bool Found = false;
    TopoID = Block->TopoID;
    // FIXME do we need to set all not innermost loop pipeline = no
    //    if (Block->CheckIfInnermostFlattenLoop(codegen, AllBlockList, AnnRpt))
    //    {
    if (AnnRpt[TopoID]["pipelined"] == "yes") {
      return true;
    } else {
      return false;
    }
    //    }
    for (auto OneBlock : AllBlockList) {
      string BlockID = OneBlock->TopoID;
      string FlattenID = AnnRpt[BlockID]["flatten-id"];
      if (FlattenID == TopoID) {
        if (OneBlock->CheckIfInnermostFlattenLoop(codegen, AllBlockList,
                                                  AnnRpt)) {
          return AnnRpt[BlockID]["pipelined"] == "yes";
        }
        Block = OneBlock;

        Found = true;
      }
    }
    if (!Found) {
      break;
    }
  }
  return false;
}

//  ************************************************************
//  get flatten trip count product
//  ************************************************************
void CPerfEstBlock::GetFlattenTripCount(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList) {
  PRESTP("EST", "----Get flatten trip count.");
  string ID = this->InnermostFlattenID;
  int64_t TripCountProduct = 1;
  while (true) {
    for (auto OneBlock : AllBlockList) {
      if (!ID.empty() && OneBlock->TopoID == ID) {
        TripCountProduct *= OneBlock->TCStatic;
        ID = OneBlock->FlattenID;
      }
    }
    if (ID.empty()) {
      break;
    }
  }
  this->FlattenTripCount = TripCountProduct;
  PRESTP("EST", "----Number =" + my_itoa(TripCountProduct));
}

//  ************************************************************
//  Check if have not unroll loop
//  ************************************************************
void TraceUpToSetHaveNotUnrollLoopOnly(
    CSageCodeGen *codegen, string KernelName, void *Node,
    const vector<CPerfEstBlock *> &AllBlockList) {
  void *Parent = Node;
  if (codegen->IsFunctionDeclaration(Parent) == 0) {
    Parent = codegen->GetParent(Node);
    if (codegen->IsLabelStatement(Parent) != 0) {
      Parent = codegen->GetParent(Parent);
    }
  }
  //  cout << "Parent1 = " << codegen->UnparseToString(Parent)<< endl;
  if (codegen->IsFunctionDeclaration(Parent) != 0) {
    if (codegen->GetFuncName(Parent) == KernelName) {
      return;
    }
    vector<void *> CallList;
    codegen->GetFuncCallsFromDecl(Parent, codegen->GetProject(), &CallList);
    for (auto OneCall : CallList) {
      Parent = OneCall;
      //      cout << "Call number = " << CallList.size() << endl;
      //      cout << "OneCall = " << codegen->UnparseToString(Parent) << endl;
      while (true) {
        Parent = codegen->GetParent(Parent);
        //        cout << "Node = " << codegen->UnparseToString(Node)<< endl;
        //        cout << "CallParent = " << codegen->UnparseToString(Parent)<<
        //        endl;
        if (Parent != nullptr) {
          if (IsLoopNode(codegen, Parent)) {
            for (auto OneBlock : AllBlockList) {
              if (OneBlock->AstPtr == Parent) {
                OneBlock->HaveNotUnrollLoop = true;
                PRESTP("EST", "----Setting parent " +
                                  codegen->UnparseToString(Parent));
                break;
              }
            }
            break;
          }
          if (codegen->IsFunctionDeclaration(Parent) != 0) {
            TraceUpToSetHaveNotUnrollLoopOnly(codegen, KernelName, Parent,
                                              AllBlockList);
            break;
          }
        }
      }
      //      cout << "Finish OneCall = " << codegen->UnparseToString(Parent) <<
      //      endl;
    }
    return;
  }
  if (IsLoopNode(codegen, Parent)) {
    for (auto OneBlock : AllBlockList) {
      if (OneBlock->AstPtr == Parent) {
        OneBlock->HaveNotUnrollLoop = true;
        PRESTP("EST", "----Setting parent " + codegen->UnparseToString(Parent));
        return;
      }
    }
    return;
  }
  TraceUpToSetHaveNotUnrollLoopOnly(codegen, KernelName, Parent, AllBlockList);
}

//  ************************************************************
//  if loop only have unroll loop only, then set it to parent
//  ************************************************************
void CPerfEstBlock::SetParentAttributeHaveUnrollLoopOnly(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList) {
  PRESTP("EST", " Setting parent loop as not unroll loop.");
  TraceUpToSetHaveNotUnrollLoopOnly(codegen, this->KernelName, this->AstPtr,
                                    AllBlockList);
}

//  ************************************************************
//  Find possible dependecy array or scalar
//  ************************************************************
void CPerfEstBlock::FindPossibleDependencyVariableInScope() {
  CSageCodeGen *codegen = this->codegen;
  void *loop = this->AstPtr;
  vector<void *> vec_ref;
  vector<string> vec_string;
  vector<string> vec_array;
  codegen->GetNodesByType_compatible(loop, "SgVarRef", &vec_ref);
  for (size_t i = 0; i < vec_ref.size(); i++) {
    void *ref = vec_ref[i];
    void *var_init = codegen->GetVariableInitializedName(ref);
    string var_name = codegen->GetVariableName(var_init);
    if (std::find(vec_string.begin(), vec_string.end(), var_name) ==
        vec_string.end()) {
      vec_string.push_back(var_name);
      // test_false_dependency will cause long time run if code have lot of if
      // else if runtime reduced, we can reuse this code
      //            void *dep_loop = test_false_dependency(codegen, loop,
      //            var_init); if (dep_loop) {
      //                vec_string.push_back(var_name);
      //            }
    }
  }
  this->DependencyVariableList = "";
  for (auto one_s : vec_string) {
    this->DependencyVariableList += " " + one_s;
  }
  PRESTP("EST", " Dependency list = " << this->DependencyVariableList);
}

//  ************************************************************
//  Read json file, and get related information
//  ************************************************************
void CPerfEstBlock::ReadAnnotatedReportHelper(
    map<string, map<string, string>> AnnRpt,
    const vector<CPerfEstBlock *> &AllBlockList,
    const vector<CPerfEstBlock *> &AllFuncDeclList) {
  PRESTP("EST", "*******************************");
  PRESTP("EST", "Read report for " << Type << ", ID = " << this->TopoID << ", "
                                   << codegen->UnparseToString(AstPtr));
  // Check if ID exist
  if (AnnRpt.find(TopoID) == AnnRpt.end()) {
    PRESTP("EST", "Critical Warning: TopoID does not exist in input json:");
  }

  PRESTP("EST", "----Checking if updated...");
  if (!AnnRpt[TopoID]["updated"].empty()) {
    this->DataUpdated = true;
  }
  PRESTP("EST", "----Final updated = " << this->DataUpdated);

  PRESTP("EST", "----Checking glue latency...");
  if (!AnnRpt[TopoID]["glue-latency"].empty()) {
    this->GlueLatency = ParseString2LongLong(AnnRpt[TopoID]["glue-latency"]);
  }
  PRESTP("EST", "----Final glue latency = " << this->GlueLatency);

  PRESTP("EST", "----Checking glue latency iteration...");
  if (!AnnRpt[TopoID]["glue-latency-iteration"].empty()) {
    this->GlueLatencyIteration =
        ParseString2LongLong(AnnRpt[TopoID]["glue-latency-iteration"]);
  }
  PRESTP("EST",
         "----Final glue latency iteration = " << this->GlueLatencyIteration);

  PRESTP("EST", "----Checking execution sequence...");
  if (!AnnRpt[TopoID]["serially_execution"].empty()) {
    this->ExecFlow =
        (ParseString2LongLong(AnnRpt[TopoID]["serially_execution"]) != 0);
  }
  PRESTP("EST", "----execution in " << this->ExecFlow);

  PRESTP("EST", "----Checking logic latency...");
  if (!AnnRpt[TopoID]["logic-latency"].empty()) {
    this->LogicLatency = ParseString2LongLong(AnnRpt[TopoID]["logic-latency"]);
  }
  PRESTP("EST", "----Final logic latency = " << this->LogicLatency);

  PRESTP("EST", "----Checking logic latency iteration...");
  if (!AnnRpt[TopoID]["logic-latency-iteration"].empty()) {
    this->LogicLatencyIteration =
        ParseString2LongLong(AnnRpt[TopoID]["logic-latency-iteration"]);
  }
  PRESTP("EST",
         "----Final logic latency iteration = " << this->LogicLatencyIteration);

  PRESTP("EST", "----Checking execution sequence...");
  if (!AnnRpt[TopoID]["serially_execution"].empty()) {
    this->ExecFlow =
        (ParseString2LongLong(AnnRpt[TopoID]["serially_execution"]) != 0);
  }
  PRESTP("EST", "----execution in " << this->ExecFlow);

  PRESTP("EST", "----Checking instance_number...");
  if (!AnnRpt[TopoID]["instance_number"].empty()) {
    this->InstanceNumber =
        ParseString2LongLong(AnnRpt[TopoID]["instance_number"]);
  }
  PRESTP("EST", "----instance number " << this->InstanceNumber);

  if (Type == LOOP) {
    //  TC processing
    PRESTP("EST", "----Checking TC...");
    if (!AnnRpt[TopoID]["trip-count"].empty()) {
      // if trip count is not digital, return -1
      this->TCStatic = ParseString2LongLong_1(AnnRpt[TopoID]["trip-count"]);
    } else {
      // if vendor report do not have loop tripcount data,
      // then the loop must be ignored, set tripcount to 0
      int64_t TC = 1;
      int64_t TC_ub = 1;
      codegen->get_loop_trip_count(AstPtr, &TC, &TC_ub);
      cout << "TC = " << TC << ", TC_ub = " << TC_ub << endl;
      if (TC_ub > 0) {
        this->TCStatic = TC_ub;
      }
      if (this->Tool == "sdaccel") {
        // if unrolled loop, should have tripcount,
        // if not unrolled and without tripcount, then it should be optimized
        if (AnnRpt[TopoID]["unrolled"] != "yes") {
          this->TCStatic = 0;
        }
      }
    }
    PRESTP("EST", "----Final TC = " << this->TCStatic);

    //  Processing flatten
    PRESTP("EST", "----Checking flatten...");
    if (AnnRpt[TopoID]["flatten"] == "yes") {
      this->IsFlatten = true;
      this->FlattenID = AnnRpt[TopoID]["flatten-id"];
      if (this->CheckFlattenToPipeline(codegen, AllBlockList, AnnRpt)) {
        this->IsFlattenToPipeline = true;
        PRESTP("EST", "--------Is flatten to pipeline...");
      } else {
        this->IsFlattenToPipeline = false;
        PRESTP("EST", "--------Not flatten to pipeline...");
      }
      if (this->CheckIfInnermostFlattenLoop(codegen, AllBlockList, AnnRpt)) {
        PRESTP("EST", "--------Is innermost flatten loop...");
        this->IsInnermostFlattenLoop = true;
      }
      this->SetInnermostFlattenID(codegen, AllBlockList, AnnRpt);
    }
    PRESTP("EST", "----Final flatten = " << this->IsFlatten);

    //  UnrollFactor processing, For xilinx, just keep the same now
    PRESTP("EST", "----Checking unroll factor...");
    if (AnnRpt[TopoID]["unrolled"].empty()) {
    } else if (AnnRpt[TopoID]["unrolled"] == "yes") {
      if (!AnnRpt[TopoID]["unroll-factor"].empty()) {
        this->UnrollFactor =
            ParseString2LongLong(AnnRpt[TopoID]["unroll-factor"]);
      } else {
        this->UnrollFactor = this->TCStatic;
      }
      if (AnnRpt[TopoID]["unroll-failed"] == "yes") {
        PRESTP("EST", "----Unroll failed");
        this->UnrollFactor = 1;
      }
    }
    if (this->UnrollFactor <= 1) {
      this->UnrollFactor = 1;
    }
    //  Check if parent have child loops which is not unroll
    if (this->UnrollFactor != this->TCStatic) {
      this->SetParentAttributeHaveUnrollLoopOnly(codegen, AllBlockList);
    }
    PRESTP("EST", "----Final unroll factor = " << this->UnrollFactor);

    //  II processing
    PRESTP("EST", "----Checking II...");
    if (!AnnRpt[TopoID]["II"].empty()) {
      this->II = ParseString2LongLong(AnnRpt[TopoID]["II"]);
      this->IIOrg = ParseString2LongLong(AnnRpt[TopoID]["II"]);
      if (AnnRpt[TopoID]["pipelined"] == "yes") {
        if (!this->IsInnermostFlattenLoop && this->IsFlatten) {
          this->Pipelined = false;
        } else {
          this->Pipelined = true;
        }
      } else {
      }
    }
    if (this->II <= 0) {
      this->II = 0;
    } else {
      NullifyPipelinedParent(Parent, AllBlockList);
    }
    PRESTP("EST", "----Final II = " << this->II);

    //  IL processing
    PRESTP("EST", "----Checking IL...");
    if (!AnnRpt[TopoID]["iteration-latency"].empty()) {
      this->IL = ParseString2LongLong_1(AnnRpt[TopoID]["iteration-latency"]);
    }
    //    if (this->IL <= 1) {
    //      this->IL = 1;
    //    }
    PRESTP("EST", "----Final IL = " << this->IL);

    PRESTP("EST", "----Checking total latency...");
    if (!AnnRpt[TopoID]["latency-max"].empty()) {
      this->LatencyMax = ParseString2LongLong_1(AnnRpt[TopoID]["latency-max"]);
    }
    PRESTP("EST", "----Final total latency = " << this->LatencyMax);

    PRESTP("EST", "----Checking iteration latency...");
    if (!AnnRpt[TopoID]["iteration-latency"].empty()) {
      this->IterationLatencyMax =
          ParseString2LongLong_1(AnnRpt[TopoID]["iteration-latency"]);
    }
    PRESTP("EST",
           "----Final iteration latency = " << this->IterationLatencyMax);

    PRESTP("EST", "----Checking burst...");
    if (!AnnRpt[TopoID]["burst"].empty()) {
      string bitwidth = AnnRpt[TopoID]["burstwidth"];
      vector<string> bitwidthlist;
      str_split(bitwidth, ',', &bitwidthlist);
      int bitwidth_int = 0;
      for (size_t i = 0; i < bitwidthlist.size(); i++) {
        bitwidth_int += ParseString2LongLong(bitwidthlist[i]);
      }
      this->Burst = 1;
      this->BurstWidth = bitwidth_int;
    }
    PRESTP("EST", "----Final burst = " << this->Burst << ". burst width = "
                                       << this->BurstWidth);

    PRESTP("EST", "----Checking vendor latnecy...");
    if (this->Tool == "sdaccel") {
      if (!AnnRpt[TopoID]["iteration-latency"].empty()) {
        this->VendorLatency =
            ParseString2LongLong_1(AnnRpt[TopoID]["iteration-latency"]);
      }
    }
    PRESTP("EST", "----Final vendor latency = " << this->VendorLatency);

    PRESTP("EST", "----Find possible dependency vaiable");
    this->FindPossibleDependencyVariableInScope();

  } else if (Type == FNDECL || Type == SUBKERNEL) {
    PRESTP("EST", "----Checking latnecy max...");
    if (!AnnRpt[TopoID]["latency-max"].empty()) {
      this->LatencyMax = ParseString2LongLong_1(AnnRpt[TopoID]["latency-max"]);
      this->VendorLatency = this->LatencyMax;
    }
    PRESTP("EST", "----Final latency max = " << this->VendorLatency);

    PRESTP("EST", "----Checking inline...");
    if (AnnRpt[TopoID]["inline"] != "yes") {
      this->IsInlineFunc = true;
    }
    PRESTP("EST", "----Is inline function = " << this->IsInlineFunc);

    PRESTP("EST", "----Checking dead...");
    if (AnnRpt[TopoID]["dead"] == "yes") {
      this->IsDead = true;
    }
    PRESTP("EST", "----Final dead = " << this->IsDead);

    PRESTP("EST", "----Checking execution sequence from json file...");
    map<string, string> KeyValue = AnnRpt[TopoID];
    for (auto const &x : KeyValue) {
      string Key = x.first;
      string Value = x.second;
      if (Key.substr(0, 5) == "state") {
        vector<string> ValueSet = str_split(Value, ' ');
        this->StateVector.push_back(ValueSet);
        cout << "Key:Value=" << x.first  //  string (key)
             << ':' << x.second          //  string's value
             << endl;
      }
    }
    PRESTP("EST", "----Vector size" << (this->StateVector).size());
  } else if (Type == MEMCPY) {
    //  Merlin Memcpy II may not be 1, need to calculate it
    //  II processing
    PRESTP("EST", "----Checking II...");
    if (!AnnRpt[TopoID]["II"].empty()) {
      this->II = ParseString2LongLong(AnnRpt[TopoID]["II"]);
    }
    if (this->II <= 0) {
      this->II = 1;
    }
    PRESTP("EST", "----Final II = " << this->II);
  }

  vector<CPerfEstBlock *> ChildList;
  for (int i = 0; i < 2; i++) {
    vector<vector<CPerfEstBlock *>> *SchedulePtr;
    vector<CPerfEstBlock *> *SchedulePtrSpawn;
    if (i == 1) {
      //  this case is only used for else statement
      if (Type == IF) {
        SchedulePtr = &ScheduleElse;
      } else {
        continue;
      }
    } else {
      SchedulePtr = &Schedule;
    }
    SchedulePtrSpawn = &ScheduleSpawn;

    //        printf("schedule sizej=%zu\n", (*SchedulePtr).size());
    for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
      //        printf("schedule sizek=%zu\n", (*SchedulePtr)[j].size());
      for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
        CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
        ChildBlock->ReadAnnotatedReportHelper(AnnRpt, AllBlockList,
                                              AllFuncDeclList);
      }
    }
    for (size_t j = 0; j < (*SchedulePtrSpawn).size(); j++) {
      CPerfEstBlock *ChildBlock = (*SchedulePtrSpawn)[j];
      ChildBlock->ReadAnnotatedReportHelper(AnnRpt, AllBlockList,
                                            AllFuncDeclList);
    }
  }
}

//  ************************************************************
//  Read json file
//  ************************************************************
void ReadAnnotatedReport(CSageCodeGen *codegen, string AnnRptPath,
                         const vector<CPerfEstBlock *> &AllBlockList,
                         const vector<CPerfEstBlock *> &AllFuncDeclList) {
  std::ifstream AnnRptFile(AnnRptPath.c_str());
  if (!AnnRptFile.good()) {
    //  already checked in script
    //    printf("[WARNING] Annotated report %s missing. The result will not be
    //    "
    //           "accurate.\n",
    //           AnnRptPath.c_str());
  } else {
    readInterReport(AnnRptPath, &g_AnnRpt);
  }

  for (size_t i = 0; i < AllFuncDeclList.size(); i++) {
    AllFuncDeclList[i]->ReadAnnotatedReportHelper(g_AnnRpt, AllBlockList,
                                                  AllFuncDeclList);
  }
}

//  ************************************************************
//  Read all specific key values from json file
//  ************************************************************
map<string, string> ReadAnnotatedReportKeyValue(CSageCodeGen *codegen,
                                                string AnnRptPath, string Key) {
  std::ifstream AnnRptFile(AnnRptPath.c_str());
  map<string, string> TopoID_Value;
  map<string, map<string, string>> ContentAnnRpt;
  if (!AnnRptFile.good()) {
    //  already checked in script
    //    printf("[WARNING] Annotated report %s missing. The result will not be
    //    "
    //           "accurate.\n",
    //           AnnRptPath.c_str());
  } else {
    printf("[INFO] Reading annotated report : %s\n", AnnRptPath.c_str());
    readInterReport(AnnRptPath, &ContentAnnRpt);
  }
  map<string, string> ID_Exec;
  map<string, map<string, string>>::iterator it;
  for (it = ContentAnnRpt.begin(); it != ContentAnnRpt.end(); it++) {
    string TopoID = it->first;
    map<string, string> Attribute = it->second;
    if (Attribute.find(Key) != Attribute.end()) {
      string Value = Attribute[Key];
      TopoID_Value.insert(pair<string, string>(TopoID, Value));
      //      cout << "ID = " << TopoID << ", Value = " << Value << endl;
    }
  }
  return TopoID_Value;
}

//  ************************************************************
//  Read all external memory parameter
//  ************************************************************
ExtMemParam ReadExtMemParam(string ExtMemParamPath) {
  ExtMemParam ExtMemParamData;

  //  dafault value for xilinx:adm-pcie-ku3:2ddr:3.3
  ExtMemParamData.RBW_GBs = 10.3;
  ExtMemParamData.WBW_GBs = 9.6;
  ExtMemParamData.RLat_ns = 434;
  ExtMemParamData.WLat_ns = 325;
  ExtMemParamData.RandLat_ns = 62;
  ExtMemParamData.FPGACLK_GHz =
      0.25;  // TODO(youxiang): Should get clock freq as input from a file
  ExtMemParamData.MAX_BITWIDTH = 512;
  ExtMemParamData.LATENCY = 240;

  map<string, map<string, string>> ExtMemParamDataTemp;

  std::ifstream ExtMemParamFile(ExtMemParamPath.c_str());

  if (!ExtMemParamFile.good()) {
    printf("[WARNING] External memory parameter file %s is missing. Applying "
           "default value for xilinx:adm-pcie-ku3:2ddr:3.3\n",
           ExtMemParamPath.c_str());
  } else {
    printf("[INFO] Reading external memory parameter file: %s\n",
           ExtMemParamPath.c_str());
    readInterReport(ExtMemParamPath, &ExtMemParamDataTemp);

    //  Assumes there is only one entry for the first map
    map<string, map<string, string>>::iterator it = ExtMemParamDataTemp.begin();

    if (it->second["RBW_GBs"].empty()) {
      printf("[WARNING] RBW_GBs not found.\n");
    } else {
      ExtMemParamData.RBW_GBs = atof(it->second["RBW_GBs"].c_str());
    }
    if (it->second["WBW_GBs"].empty()) {
      printf("[WARNING] WBW_GBs not found.\n");
    } else {
      ExtMemParamData.WBW_GBs = atof(it->second["WBW_GBs"].c_str());
    }
    if (it->second["RLat_ns"].empty()) {
      printf("[WARNING] RLat_ns not found.\n");
    } else {
      ExtMemParamData.RLat_ns = atof(it->second["RLat_ns"].c_str());
    }
    if (it->second["WLat_ns"].empty()) {
      printf("[WARNING] WLat_ns not found.\n");
    } else {
      ExtMemParamData.WLat_ns = atof(it->second["WLat_ns"].c_str());
    }
    if (it->second["RandLat_ns"].empty()) {
      printf("[WARNING] RandLat_ns not found.\n");
    } else {
      ExtMemParamData.RandLat_ns = atof(it->second["RandLat_ns"].c_str());
    }
    if (it->second["MAX_BITWIDTH"].empty()) {
      printf("[WARNING] MAX_BITWIDTH not found.\n");
    } else {
      ExtMemParamData.MAX_BITWIDTH = atof(it->second["MAX_BITWIDTH"].c_str());
    }
    if (it->second["LATENCY"].empty()) {
      printf("[WARNING] LATENCY not found.\n");
    } else {
      ExtMemParamData.LATENCY = atof(it->second["LATENCY"].c_str());
    }
  }

#ifdef PERF_EST_DBG
  printf("RBW_GBs=%f\n", ExtMemParamData.RBW_GBs);
  printf("WBW_GBs=%f\n", ExtMemParamData.WBW_GBs);
  printf("RLat_ns=%f\n", ExtMemParamData.RLat_ns);
  printf("WLat_ns=%f\n", ExtMemParamData.WLat_ns);
  printf("RandLat_ns=%f\n", ExtMemParamData.RandLat_ns);
  printf("FPGACLK_GHz=%f\n", ExtMemParamData.FPGACLK_GHz);
  printf("MAX_BITWIDTH=%f\n", ExtMemParamData.MAX_BITWIDTH);
  printf("LATENCY=%f\n", ExtMemParamData.LATENCY);
#endif

  return ExtMemParamData;
}

//  ************************************************************
//  Write and merge json filee
//  ************************************************************
void AddSimOutputMaker(CSageCodeGen *codegen, string PerfRptPath,
                       vector<CPerfEstBlock *> AllFuncDeclList,
                       vector<CPerfEstBlock *> AllBlockList, double FPGACLK_GHz,
                       string Tool) {
  vector<CPerfEstBlock *> OutputBlockList;
  for (size_t i = 0; i < AllFuncDeclList.size(); i++) {
    AllFuncDeclList[i]->Add2SimOutputList(&OutputBlockList, nullptr);
  }
  for (auto OneBlock : AllBlockList) {
    if (OneBlock->Type == TOPKERNEL) {
      OneBlock->Add2SimOutputList(&OutputBlockList, NULL);
    }
  }

  FILE *HLSCOPE_fout;
  printf("[INFO] Writing performance report : %s\n", PerfRptPath.c_str());
  HLSCOPE_fout = fopen(PerfRptPath.c_str(), "w");
  for (size_t i = 0; i < OutputBlockList.size(); i++) {
    cout << "ID = " << OutputBlockList[i]->TopoID << endl;
    cout << "Report unit number = " << i << endl;
    if (OutputBlockList[i]->TopoID.empty()) {
      if (i == OutputBlockList.size() - 1) {
        fprintf(HLSCOPE_fout, "%s", "\n\t\"\":{\"\":\"\"}");
        fprintf(HLSCOPE_fout, "%s", "\n}\n");
      }
      continue;
    }
    string prefix = (i == 0) ? "{\n" : "";
    string suffix = (i == OutputBlockList.size() - 1) ? "\n}\n" : ",\n";
    string trip_count = OutputBlockList[i]->TCStatic > 0
                            ? std::to_string(OutputBlockList[i]->TCStatic)
                            : "?";
    string tc_info =
        ((  //  OutputBlockList[i]->TCStatic == 0 ||
             OutputBlockList[i]->Type != LOOP)
             ? ","
             : string(",\n        \"trip-count\":\"") + trip_count +
                   string("\",\n        \"unroll-factor\":\"") +
                   std::to_string(OutputBlockList[i]->UnrollFactor) + "\",\n");
    string poison =
        "        \"poison\":\"" +
        std::to_string(static_cast<int>(OutputBlockList[i]->IsPoison)) + "\"";

    string content;
    if (OutputBlockList[i]->Type == LOOP &&
        ((OutputBlockList[i]->Pipelined && OutputBlockList[i]->II < 1) ||
         OutputBlockList[i]->FlattenedSon)) {
      content = prefix + "    \"" +
                (OutputBlockList[i]->TopoID +
                 "\": {\n        \"CYCLE_UNIT\":\"ignore\",\n        "
                 "\"CYCLE_TOT\":\"ignore\",\n        \"DBW\":\"ignore\"" +
                 tc_info + poison + "\n    }" + suffix);
      fprintf(HLSCOPE_fout, "%s", content.c_str());
      printf("%s", content.c_str());
    } else {
      string ID = OutputBlockList[i]->TopoID;
      if (OutputBlockList[i]->Type == TOPKERNEL) {
        ID = "TOP_" + OutputBlockList[i]->KernelName;
        content =
            prefix + "    \"" +
            (ID +
             "\": {\n        \"CYCLE_UNIT\":\"%ld\",\n        "
             "\"CYCLE_TOT\":\"%ld\",\n        \"DBW\":\"%.2f\"" +
             tc_info + "\n" + poison + ",\n" + "        \"func_name\": \"" +
             OutputBlockList[i]->KernelName + "\",\n     " +
             "        \"topo_id\": \"" + ID + "\",\n     " +
             "        \"type\":\"top_kernel\"\n     }" + suffix);
      } else {
        string additional;
        //      cout << "Tool = " << Tool << ", ExecFlow = " <<
        //      OutputBlockList[i]->ExecFlow << endl;
        if (!(OutputBlockList[i]->ExecFlow)) {
          additional +=
              "        \"serially_execution\":\"" +
              std::to_string(static_cast<int>(OutputBlockList[i]->ExecFlow)) +
              "\",\n";
        }
        if (Tool == "sdaccel") {
          additional += "        \"updated\":\"1\",\n";
          additional += "        \"glue-latency\":\"" +
                        std::to_string(OutputBlockList[i]->GlueLatency) +
                        "\",\n";
          additional +=
              "        \"glue-latency-iteration\":\"" +
              std::to_string(OutputBlockList[i]->GlueLatencyIteration) +
              "\",\n";
          additional += "        \"logic-latency\":\"" +
                        std::to_string(OutputBlockList[i]->LogicLatency) +
                        "\",\n";
          additional +=
              "        \"logic-latency-iteration\":\"" +
              std::to_string(OutputBlockList[i]->LogicLatencyIteration) +
              "\",\n";
          if (OutputBlockList[i]->IsFlattenToPipeline &&
              OutputBlockList[i]->IsFlatten) {
            additional += "        \"flatten-to-pipeline\":\"yes\",\n";
            if (OutputBlockList[i]->IsInnermostFlattenLoop) {
              additional += "        \"innermost-flatten-loop\":\"yes\",\n";
              additional += "        \"innermost-flatten-id\":\"" +
                            OutputBlockList[i]->InnermostFlattenID + "\",\n";
            }
          }
          if (OutputBlockList[i]->UnrollFailed) {
            additional += "        \"unroll-failed\":\"yes\",\n";
          }
        }
        additional += "        \"possible_dep_var\":\"" +
                      OutputBlockList[i]->DependencyVariableList + "\",\n";
        additional += "        \"CYCLE_BURST\":\"" +
                      std::to_string(OutputBlockList[i]->BurstCycles) + "\",\n";
        additional += "        \"CYCLE_COMPUTE\":\"" +
                      std::to_string(OutputBlockList[i]->ComputeCycles) +
                      "\",\n";
        content = prefix + "    \"" +
                  (ID + "\": {\n" + additional +
                   +"    \"CYCLE_UNIT\":\"%ld\",\n        "
                    "\"CYCLE_TOT\":\"%ld\",\n        \"DBW\":\"%.2f\"" +
                   tc_info + "\n" + poison + "\n    }" + suffix);
      }

      PRESTP("EST", "DBW - DRAMDataByteStatic: "
                        << OutputBlockList[i]->DRAMDataByteStatic);
      PRESTP("EST", "DBW - CycleStatic: " << OutputBlockList[i]->CycleStatic);
      PRESTP("EST", "DBW - FPGACLK_GHz: " << FPGACLK_GHz);

      //  float dbw_temp =
      //    (OutputBlockList[i]->CycleStatic == 0)
      //        ? 0.0f
      //        : 1000.0f * OutputBlockList[i]->DRAMDataByteStatic /
      //              OutputBlockList[i]->CycleStatic * FPGACLK_GHz;
      float dbw_temp = 0.0;
      fprintf(HLSCOPE_fout, content.c_str(),
              OutputBlockList[i]->CycleUnitStatic,
              OutputBlockList[i]->CycleStatic, dbw_temp);
      printf(content.c_str(), OutputBlockList[i]->CycleStatic, dbw_temp);
    }
  }
  fclose(HLSCOPE_fout);
  cout << "Finish write file " << PerfRptPath << endl;
}

//  ************************************************************
//  Merges two map of map to a single map
//  ************************************************************
void MergeFiles(string SimOutputPath, string AnnRptPath, string PerfRptPath) {
  map<string, map<string, string>> SimOutput;
  std::ifstream SimOutputFile(SimOutputPath.c_str());
  if (!SimOutputFile.good()) {
    printf("[WARNING] Simulation output %s missing.\n", SimOutputPath.c_str());
  } else {
    printf("[INFO] Reading simulation output : %s\n", SimOutputPath.c_str());
    readInterReport(SimOutputPath, &SimOutput);
  }

  map<string, map<string, string>> AnnRpt;
  std::ifstream AnnRptFile(AnnRptPath.c_str());
  if (!AnnRptFile.good()) {
    //  already checked in script
    //    printf("[WARNING] Annotated report %s missing.\n",
    //    AnnRptPath.c_str());
  } else {
    printf("[INFO] Reading annotated report : %s\n", AnnRptPath.c_str());
    readInterReport(AnnRptPath, &AnnRpt);
  }

  //  for (auto &node : SimOutput) {
  //    if (node.second["poison"] == "1") {
  //      node.second["CYCLE_UNIT"] = "-1";
  //      node.second["CYCLE_TOT"] = "-1";
  //      node.second["DBW"] = "n/a";
  //      for (auto &parent : SimOutput) {
  //        string key_child = node.first;
  //        string key_parent = parent.first;
  //        if (key_child.size() == 0 || key_parent.size() == 0) {
  //          ERRORP("EST",
  //                 "invalid key used: " << key_child << " or " << key_parent);
  //        } else {
  //          key_child[0] = 'X';
  //          key_parent[0] = 'X';
  //        }
  //      }
  //    }
  //  }

  //  Do merge
  for (map<string, map<string, string>>::iterator i = SimOutput.begin();
       i != SimOutput.end(); ++i) {
    //  note:insert may fail if there is an item with the same name. Do not
    //  overwrite, except trip-count
    for (map<string, string>::iterator j = i->second.begin();
         j != i->second.end(); ++j) {
      if (!AnnRpt[i->first].insert(make_pair(j->first, j->second)).second) {
        if (j->first ==
            string("trip-count")) {  //  WARNING : Overwriting trip-count..
          AnnRpt[i->first][j->first] = j->second;
        }
      }
    }
  }

  printf("[INFO] Generating output file : %s\n", PerfRptPath.c_str());
  writeInterReport(PerfRptPath, AnnRpt);
}

//  ************************************************************
//  Find all external port list for a function
//  ************************************************************
vector<void *> FindExtPortList(CSageCodeGen *codegen,
                               vector<void *> ParentExtPortList, void *FuncCall,
                               void *FuncDecl) {
  vector<void *> NewExtPortList;

  if (FuncCall == nullptr) {  //  If TOP Func
    //  All pointer types are array types in the top function is considered
    //  external memory
    for (int i = 0; i < codegen->GetFuncParamNum(FuncDecl); i++) {
      void *arg = codegen->GetFuncParam(FuncDecl, i);
      SgInitializedName *sg_arg =
          isSgInitializedName(static_cast<SgNode *>(arg));
      SgType *type = sg_arg->get_type();
      while (isSgModifierType(type) != nullptr) {
        type = (static_cast<SgModifierType *>(type)->get_base_type());
      }
      if ((isSgPointerType(type) != nullptr) ||
          (isSgArrayType(type) != nullptr)) {
        NewExtPortList.push_back(sg_arg);
      }
    }
  } else {  //  If not TOP Func
    //  WARNING: We extract the external memory list from the FIRST function
    //  call encountered. If certain function calls are used for BOTH local
    //  memory and external memory as its argument, this will not work
    //  correctly!!

    //  This routine checks if an function call argument has an external port
    for (int i = 0; i < codegen->GetFuncParamNum(FuncDecl); i++) {
      void *exp = codegen->GetFuncCallParam(FuncCall, i);
      vector<void *> vecRef;
      codegen->GetNodesByType(exp, "preorder", "SgVarRefExp", &vecRef);
      bool found = false;
      for (size_t j = 0; j < vecRef.size(); j++) {
        SgVarRefExp *ref = isSgVarRefExp(static_cast<SgNode *>(vecRef[j]));
        if (ref != nullptr) {
          void *call_arg = codegen->GetVariableInitializedName(ref);
          for (size_t k = 0; k < ParentExtPortList.size(); k++) {
            if (ParentExtPortList[k] == call_arg) {
              void *arg = codegen->GetFuncParam(FuncDecl, i);
              SgInitializedName *sg_arg =
                  isSgInitializedName(static_cast<SgNode *>(arg));
              cout << "Insert one ext port : "
                   << codegen->UnparseToString(sg_arg) << endl;
              if (sg_arg != nullptr) {
                NewExtPortList.push_back(sg_arg);
              }
              break;
              found = true;
            }
          }
        }
        if (found) {
          break;
        }
      }
    }
  }

  return NewExtPortList;
}

//  Searches if any of the expressions in ExtPortList is in exp. If so, return
//  the element in ExtPortList. Only returns the first found and ignores the
//  rest.
void *FindExtPortFromExp(CSageCodeGen *codegen, vector<void *> ExtPortList,
                         void *exp) {
  codegen->remove_cast(&exp);

  for (size_t i = 0; i < ExtPortList.size(); i++) {
    RoseAst ast(static_cast<SgNode *>(exp));
    for (RoseAst::iterator it = ast.begin(); it != ast.end(); it++) {
      void *sg_name = codegen->GetVariableInitializedName(*it);
      if (sg_name == ExtPortList[i]) {
        //  printf("Found %s in %s\n", A2S(ExtPortList[i]), A2S(exp));
        return ExtPortList[i];
      }
    }
  }
  return nullptr;
}

//  ************************************************************
//  Finds if MemcpyStmt is reading from or writing to anything in the
//  ExtPortList We assume there can be only 1 read or write port in either op0
//  and op1. However, having 1 read port in op0 and 1 write port in op1, or vice
//  versa, is OK
//  ************************************************************
void FindRWMemcpy(CSageCodeGen *codegen, void *MemcpyStmt,
                  vector<void *> ExtPortList, void **ReadExtPort,
                  int read_arg_idx, void **WriteExtPort, int write_arg_idx) {
  void *exp0 = codegen->GetFuncCallParam(MemcpyStmt, write_arg_idx);
  void *exp1 = codegen->GetFuncCallParam(MemcpyStmt, read_arg_idx);

  *WriteExtPort = FindExtPortFromExp(codegen, ExtPortList, exp0);
  *ReadExtPort = FindExtPortFromExp(codegen, ExtPortList, exp1);
}

//  ************************************************************
//  Searches the blocks upward until function decration is found
//  ************************************************************
CPerfEstBlock *GetFuncDeclBlock(CSageCodeGen *codegen, CPerfEstBlock *Block) {
  CPerfEstBlock *ReturnBlock;

  if (Block->Type == FNDECL || Block->Type == SUBKERNEL ||
      Block->Type == AUTOKERNEL) {
    ReturnBlock = Block;  //  itself
  } else if (Block->Parent != nullptr) {
    ReturnBlock = GetFuncDeclBlock(codegen, Block->Parent);
  } else {
    ReturnBlock = nullptr;  //  not found
  }

  return ReturnBlock;
}

//  ************************************************************
//  Searches the blocks upward until loop is found
//  ************************************************************
//  CPerfEstBlock *GetInnermostLoopBlock(CSageCodeGen *codegen,
//                                     CPerfEstBlock *Block) {
//  CPerfEstBlock *ReturnBlock;
//
//  if (Block->Type == LOOP) {
//    ReturnBlock = Block; //  itself
//  } else if (Block->Parent != nullptr) {
//    ReturnBlock = GetInnermostLoopBlock(codegen, Block->Parent);
//  } else {
//    ReturnBlock = nullptr; //  not found
//  }
//
//  return ReturnBlock;
//  }

bool CheckIfConsecutiveAccess(CSageCodeGen *codegen, void *InnerLoop,
                              void *MemRefExpr) {
  return true;
  //  We assume that the access if consecutive if the innermost outer loop has a
  //  coefficient of 1 for the loop iterator
  CMarsAccess CMA_MemRefExpr(MemRefExpr, codegen, nullptr);
  vector<CMarsExpression> IndexList = CMA_MemRefExpr.GetIndexes();
  //  WARNING:Assumes the last index of array[][][last] should be checked for
  //  consecutive access, and it exists at IndexList[IndexList.size()-1]
  CMarsExpression Index = IndexList[IndexList.size() - 1];
  vector<void *> VariableList = Index.get_vars();

  for (size_t i = 0; i < VariableList.size(); i++) {
    if (InnerLoop ==
        VariableList[i]) {  //  check if matches with innermost loop index
      CMarsExpression coef = Index.get_coeff(VariableList[i]);
      //  coef of 1 for the innermost loop should mean consecutive access
      if (coef.IsConstant() != 0) {
        if (coef.GetConstant() == 1) {
          return true;
        }
      }
    }
  }

  return false;
}

string GetNiceLookingName(CSageCodeGen *codegen, CPerfEstBlock *BlockTemp) {
  if (BlockTemp->Type == LOOP) {
    if (static_cast<bool>(codegen->IsForStatement(BlockTemp->AstPtr))) {
      return string("for(") + A2S(codegen->GetLoopInit(BlockTemp->AstPtr)) +
             "," + A2S(codegen->GetLoopTest(BlockTemp->AstPtr)) + "," +
             A2S(codegen->GetLoopIncrementExpr(BlockTemp->AstPtr)) + ")";
    }  //  while stmt
    return string("while(") +
           A2S(codegen->GetWhileCondition(BlockTemp->AstPtr)) + ")";
  }
  if (BlockTemp->Type == IF) {
    return string("if(") + A2S(codegen->GetIfStmtCondition(BlockTemp->AstPtr)) +
           ")";
  }
  if (BlockTemp->Type == MEMREF) {
    return string(A2S(codegen->TraceUpToStatement(BlockTemp->AstPtr)));
  }
  return A2S(BlockTemp->AstPtr);
}

void DBGPrintStructure(CSageCodeGen *codegen,
                       vector<CPerfEstBlock *> AllFuncDeclList) {
  for (size_t i = 0; i < AllFuncDeclList.size(); i++) {
    printf("\nFuncDecl(%d):%s TopoID:%s\n", static_cast<int>(i),
           A2S(AllFuncDeclList[i]->AstPtr), AllFuncDeclList[i]->TopoID.c_str());

    for (size_t j = 0; j < AllFuncDeclList[i]->ExtPortList.size(); j++) {
      printf("-ExtPort%d:%s\n", static_cast<int>(j),
             A2S(AllFuncDeclList[i]->ExtPortList[j]));
    }

    printf("-Schedule:\n");
    AllFuncDeclList[i]->PrintSchedule();
  }
}

string FindFlattenSonRecursive(CSageCodeGen *codegen, void *node,
                               map<string, map<string, string>> current_map) {
  string output_id = "";
  void *scope = node;
  if (codegen->IsLoopStatement(node)) {
    if (codegen->GetLoopBody(node)) {
      scope = codegen->GetLoopBody(node);
    }
  } else if (codegen->IsFunctionDeclaration(node)) {
    if (codegen->GetFuncBody(node)) {
      scope = codegen->GetFuncBody(node);
    }
  }
  // cout << "scope = " << codegen->UnparseToString(scope) << endl;
  vector<void *> for_stmts, vec_calls;
  codegen->GetNodesByType_int(scope, "preorder", V_SgForStatement, &for_stmts);
  for (auto stmt : for_stmts) {
    string stmt_id = getTopoLocation(codegen, static_cast<SgNode *>(stmt));
    cout << "for stmt = " << codegen->UnparseToString(static_cast<void *>(stmt))
         << endl;
    if (current_map.count(stmt_id) != 0) {
      if (current_map[stmt_id].count("unrolled") == 0 &&
          current_map[stmt_id].count("dead_code") == 0 &&
          current_map[stmt_id].count("flatten_son") > 0) {
        // cout << "Find flatten son ID = " << stmt_id << endl;
        output_id = stmt_id;
        return output_id;
      }
    }
  }
  codegen->GetNodesByType_int(scope, "preorder", V_SgFunctionCallExp,
                              &vec_calls);
  for (auto call : vec_calls) {
    string call_id = getTopoLocation(codegen, static_cast<SgNode *>(call));
    cout << "call_id = " << call_id << endl;
    if (current_map.count(call_id) != 0) {
      void *decl = codegen->GetFuncDeclByCall(call);
      string decl_id = getTopoLocation(codegen, static_cast<SgNode *>(decl));
      // cout << "decl stmt = " << codegen->UnparseToString((void *)decl) <<
      // endl; cout << "decl_id = " << decl_id << endl;
      if (decl) {
        if (current_map[decl_id].count("inline") != 0 &&
            current_map[decl_id]["inline"] == "yes") {
          cout << "find inlined function" << endl;
          output_id = FindFlattenSonRecursive(codegen, decl, current_map);
          if (output_id != "")
            return output_id;
        }
      }
    }
  }
  return output_id;
}

// find flatten son if flatten = yes
string FindFlattenSon(CSageCodeGen *codegen, string ID,
                      map<string, map<string, string>> current_map) {
  cout << "#################################################" << endl;
  cout << "Finding flatten son for ID " << ID << "." << endl;
  cout << "#################################################" << endl;
  string output_id = "";
  SgNode *org_node = getSgNode(codegen, ID);
  // cout << "loop = " << codegen->UnparseToString((void *)org_node) << endl;
  output_id = FindFlattenSonRecursive(codegen, static_cast<void *>(org_node),
                                      current_map);
  return output_id;
}

// do preprpocess for input json file
bool PreprocessForInput(CSageCodeGen *codegen, string InputJson) {
  cout << "#################################################" << endl;
  cout << "Do preprocess for input json " << InputJson << "." << endl;
  cout << "#################################################" << endl;
  map<string, string> flatten_id_map;
  map<string, map<string, string>> current_map;
  readInterReport(InputJson, &current_map);
  string temp_file = "preprocess.json";
  map<string, map<string, string>> input_map;
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;
  // get flatten id map
  for (iter1 = current_map.begin(); iter1 != current_map.end(); iter1++) {
    string input_id = iter1->first;
    // cout << "ID = " << input_id << endl;
    map<string, string> attribute_value = iter1->second;
    for (iter2 = attribute_value.begin(); iter2 != attribute_value.end();
         iter2++) {
      string attribute = iter2->first;
      string value = iter2->second;
      if (attribute == "flatten" && value == "yes") {
        cout << "Find flatten parent loop ID = " << input_id << endl;
        string output_id = FindFlattenSon(codegen, input_id, current_map);
        cout << "Found flatten son loop ID = " << output_id << endl;
        flatten_id_map.insert(pair<string, string>(output_id, input_id));
      }
    }
  }
  // insert attributes nodes
  for (iter1 = current_map.begin(); iter1 != current_map.end(); iter1++) {
    string input_id = iter1->first;
    // cout << "ID = " << input_id << endl;
    map<string, string> attribute_value = iter1->second;
    map<string, string> attribute_temp;
    bool write_flatten = true;
    for (iter2 = attribute_value.begin(); iter2 != attribute_value.end();
         iter2++) {
      string attribute = iter2->first;
      string value = iter2->second;
      attribute_temp.insert(pair<string, string>(attribute, value));
      if (attribute == "flatten" && value == "yes") {
        // if already have flatten, do not need to rewrite
        write_flatten = false;
      }
    }
    if (flatten_id_map.count(input_id) > 0) {
      cout << "fill data to flatten son ID = " << input_id << endl;
      if (write_flatten) {
        attribute_temp.insert(pair<string, string>("flatten", "yes"));
      }
      attribute_temp.insert(
          pair<string, string>("flatten-id", flatten_id_map[input_id]));
    }
    input_map.insert(
        pair<string, map<string, string>>(input_id, attribute_temp));
  }
  cout << "Write tmp.json file." << endl;
  writeInterReport(temp_file, input_map);
  cout << "\n" << endl;
  string cmd = "cp -r " + temp_file + " " + InputJson;
  system(cmd.c_str());
  return true;
}

// Merge data structure json file and org.json
bool PostProcessForOutput(CSageCodeGen *codegen, string OutputJson,
                          vector<string> DeleteAttriubte) {
  cout << "#################################################" << endl;
  cout << "Delete some attribtues from json " << OutputJson << "." << endl;
  cout << "#################################################" << endl;
  map<string, map<string, string>> current_map;
  readInterReport(OutputJson, &current_map);
  string cmd = "rm -rf " + OutputJson;
  system(cmd.c_str());
  map<string, map<string, string>> output_map;
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;
  for (iter1 = current_map.begin(); iter1 != current_map.end(); iter1++) {
    string ID = iter1->first;
    //    cout << "ID = " << ID << endl;
    map<string, string> attribute_value = iter1->second;
    map<string, string> attribute_temp;
    for (iter2 = attribute_value.begin(); iter2 != attribute_value.end();
         iter2++) {
      string attribute = iter2->first;
      string value = iter2->second;
      if (find(DeleteAttriubte.begin(), DeleteAttriubte.end(), attribute) ==
          DeleteAttriubte.end()) {
        attribute_temp.insert(pair<string, string>(attribute, value));
      }
    }
    output_map.insert(pair<string, map<string, string>>(ID, attribute_temp));
  }
  cout << "Write output.json file." << endl;
  writeInterReport(OutputJson, output_map);
  cout << "\n" << endl;
  return true;
}

//  get execution flow from json file
//  for xilinx, first time all assume serial execution,
//     after first hls, will analyze the block parallel or serial
map<string, string> GetExecFlow(CSageCodeGen *codegen, string Tool,
                                string AnnRptPath) {
  map<string, string> TopoIDExecMap;
  TopoIDExecMap =
      ReadAnnotatedReportKeyValue(codegen, AnnRptPath, "serially_execution");
  return TopoIDExecMap;
}

int performance_estimation_top(CSageCodeGen *codegen, void *pTopFunc,
                               const CInputOptions &options) {
  string Mode = options.get_option("-a", 0);
  //  FIXME: generate KernelGroup according to complete program info
  map<string, map<string, string>> KernelGroup;
  readInterReport("kernel_list.json", &KernelGroup);

  if (Mode == "SIM") {
#if ENABLE_SIM
    printf("[INFO] Begin performance estimation based on simulation\n");
#endif
  } else if (Mode == "STATIC") {
    printf("[INFO] Beginning static performance estimation\n");
  } else if (Mode == "MERGE") {
    string SimOutputPath = options.get_option("-a", 1);
    string AnnRptPath = options.get_option("-a", 2);
    string PerfRptPath = options.get_option("-a", 3);
    printf("[INFO] Beginning merging simulation output %s and annotated report "
           "%s into performance report %s\n",
           SimOutputPath.c_str(), AnnRptPath.c_str(), PerfRptPath.c_str());

    MergeFiles(SimOutputPath, AnnRptPath, PerfRptPath);
    printf("[INFO] Merge complete\n");
    return 1;
  } else if (Mode == "FASTDSE") {
    string ChangeJson = options.get_option("-a", 1);
    string HistoryJson = options.get_option("-a", 2);
    string OutputJson = options.get_option("-a", 3);
    string Tool = options.get_option("-a", 4);
    vector<string> DeleteAttriubte = {"CYCLE_BURST", "CYCLE_COMPUTE"};
    PostProcessForOutput(codegen, HistoryJson, DeleteAttriubte);
    FastDse(codegen, KernelGroup, ChangeJson, HistoryJson, OutputJson, Tool);
    return 1;
  } else {
    printf("[Error] Please specify mode: SIM / STATIC.\n");
    return 0;
  }
  string KernelListFile = options.get_option("-a", 1);
  string AnnRptPath = options.get_option("-a", 2);
  string ExtMemParamPath = options.get_option("-a", 3);
  string PerfRptPath = options.get_option("-a", 4);
  string Tool = options.get_option("-a", 5);

  if (Tool == "sdaccel") {
    PreprocessForInput(codegen, AnnRptPath);
  }

  vector<CPerfEstBlock *> AllBlockList;
  vector<CPerfEstBlock *> AllFuncDeclList;
  printf("\n\n\n");
  printf("[INFO] ========================================================\n");
  printf("[INFO] 1. Create blocks..\n");
  printf("[INFO] ========================================================\n");
  //  Get information of serial or parallel execution for blocks
  map<string, string> TopoIDExecMap = GetExecFlow(codegen, Tool, AnnRptPath);
  MakeNewBlocks(codegen, Tool, &AllBlockList, &AllFuncDeclList, TASK, nullptr,
                NULL, TopoIDExecMap, KernelGroup, nullptr);

  //  Read annotated report here
  printf("\n\n\n");
  printf("[INFO] ========================================================\n");
  printf("[INFO] 2. Reading annotated report : %s\n", AnnRptPath.c_str());
  printf("[INFO] ========================================================\n");
  ReadAnnotatedReport(codegen, AnnRptPath, AllBlockList, AllFuncDeclList);

  printf("[INFO] ========================================================\n");
  printf("[INFO] 3. Compute cycles.\n");
  printf("[INFO] ========================================================\n");
  //  Read external memory parameter here
  ExtMemParam ExtMemParamData = ReadExtMemParam(ExtMemParamPath);
  ComputeStaticCycles(codegen, &ExtMemParamData, AllBlockList);

  printf("[INFO] ========================================================\n");
  printf("[INFO] 4. Merge Json file.\n");
  printf("[INFO] ========================================================\n");
  AddSimOutputMaker(codegen, PerfRptPath, AllFuncDeclList, AllBlockList,
                    ExtMemParamData.FPGACLK_GHz, Tool);
  vector<string> DeleteAttriubte = {"latency-min", "latency-max",
                                    "interval-max", "interval-min"};
  PostProcessForOutput(codegen, PerfRptPath, DeleteAttriubte);
  PostProcessForOutput(codegen, AnnRptPath, DeleteAttriubte);

  //  deallocate all blocks on exit
  for (size_t i = 0; i < (AllBlockList).size(); i++) {
    delete AllBlockList[i];
  }

  printf("[INFO] Performance estimation complete\n");
  return 1;
}
