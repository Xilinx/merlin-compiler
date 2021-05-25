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


#include "performance_estimation.h"
#define DEVICE_BITWIDTH_MAX 512
#define DEVICE_BITWIDTH_INIT 65536
#define COEFFICIENT_FOR_SMALL_BURST 1.05

extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
bool IsFuncNameMemcpy(string func_name);
bool IsLoopNode(CSageCodeGen *codegen, void *Node);
CPerfEstBlock *GetBlockFromID(CSageCodeGen *codegen, string ID,
                              vector<CPerfEstBlock *> AllBlockList);
CPerfEstBlock *GetBlockFromNode(CSageCodeGen *codegen, void *Node,
                                vector<CPerfEstBlock *> AllBlockList);
int CheckIfFromFuncCall(CSageCodeGen *codegen, void *Node);

//  ************************************************************
//  Compuate unit cycles and total cyclles for each block
//  ************************************************************
void ComputeStaticCycles(CSageCodeGen *codegen, ExtMemParam *ExtMemParamData,
                         const vector<CPerfEstBlock *> &AllBlockList) {
  vector<CPerfEstBlock *> TopKernelBlock;
  for (auto OneBlock : AllBlockList) {
    if (OneBlock->Type == TOPKERNEL) {
      TopKernelBlock.push_back(OneBlock);
      cout << "Find Top Block = " << OneBlock->TopoID << endl;
    }
  }

  for (auto OneBlock : AllBlockList) {
    if (OneBlock->Type == TASK) {
      CPerfEstBlock *TaskBlock = OneBlock;
      vector<CPerfEstBlock *> AllFuncDeclList;
      cout << "**********************************************************"
           << endl;
      cout << "3.1 Update cycles by compute all data" << endl;
      cout << "**********************************************************"
           << endl;
      TaskBlock->ComputeStaticCycleHelper(codegen, &AllFuncDeclList,
                                          ExtMemParamData, nullptr,
                                          AllBlockList, TopKernelBlock);
      PRESTP("EST", " Print schedule for initial framework.");
      DBGPrintStructure(codegen, AllFuncDeclList);

      if (TaskBlock->Tool != "sdaccel") {
        cout << "**********************************************************"
             << endl;
        cout << "3.2 Update cycles by memory burst" << endl;
        cout << "**********************************************************"
             << endl;
        TaskBlock->UpdateCycleByMemoryBurstTop(codegen, TopKernelBlock);
        PRESTP("EST", " Print schedule for after memory burst updating.");
        DBGPrintStructure(codegen, AllFuncDeclList);
      }
      AllFuncDeclList.clear();

      cout << "**********************************************************"
           << endl;
      cout << "3.3 Update total cycles." << endl;
      cout << "**********************************************************"
           << endl;
      TaskBlock->ComputeTotalStaticCycle(codegen, &AllFuncDeclList, NULL, 1,
                                         TopKernelBlock);
      PRESTP("EST", " Print schedule for after compute total cycle.");
      DBGPrintStructure(codegen, AllFuncDeclList);
    }
  }
}

//  ************************************************************
//  Update unit cycles by memory burst
//  if compute bound, still using original cycles
//  if memory bound, using memory burst cycles instead
//  ************************************************************
void CPerfEstBlock::UpdateCycleByMemoryBurstTop(
    CSageCodeGen *codegen, vector<CPerfEstBlock *> TopKernelBlock) {
  for (auto OneTopKernelBlock : TopKernelBlock) {
    vector<CPerfEstBlock *> AllFuncDeclList;
    cout << "**********************************************************"
         << endl;
    cout << "3.2.1 Update burst data size" << endl;
    cout << "**********************************************************"
         << endl;
    OneTopKernelBlock->UpdateBurstCyclesBySelf(codegen, &AllFuncDeclList);
    cout << "**********************************************************"
         << endl;
    cout << "3.2.2 Update burst cycles factor" << endl;
    cout << "**********************************************************"
         << endl;
    OneTopKernelBlock->UpdateBurstCyclesFactor(codegen, AllFuncDeclList);
    cout << "**********************************************************"
         << endl;
    cout << "3.2.3 Update burst cycles" << endl;
    cout << "**********************************************************"
         << endl;
    OneTopKernelBlock->UpdateBurstCycles(codegen, AllFuncDeclList, 1.0);
    PRESTP("EST",
           " Total cycle of top kernel:" << OneTopKernelBlock->CycleStatic);
    cout << "SECOND_CYCLES=" << OneTopKernelBlock->CycleUnitStatic << endl;
  }
}

//  ************************************************************
//  Trace up to get parent loop or function for one node
//  ************************************************************
void *GetParentLoopOrFunc(CSageCodeGen *codegen, void *Node) {
  void *Parent = Node;
  while (true) {
    Parent = codegen->GetParent(Parent);
    if (Parent != nullptr) {
      if (IsLoopNode(codegen, Parent)) {
        return Parent;
      }
      if (codegen->IsFunctionDeclaration(Parent) != 0) {
        return Parent;
      }
    }
  }
  return nullptr;
}

//  ************************************************************
//  Get parent of loop scope of a loop
//  FIXME, espically for auto kernel now, need review
//  ************************************************************
void *GetParentScope(CSageCodeGen *codegen, void *node) {
  void *scope = NULL;
  void *parent = node;
  while ((codegen->IsFunctionDeclaration(parent) == 0) &&
         (codegen->IsForStatement(parent) == 0)) {
    parent = codegen->GetParent(parent);
  }
  scope = parent;
  //  PRESTP("EST", " Auto kernel parent scope: " << codegen->_p(scope, 30));
  return scope;
}

//  ************************************************************
//  get block execution flow squence from input json file
//  ************************************************************
void CPerfEstBlock::GetExecFlowFromInput(map<string, string> TopoIDExecMap,
                                         string Tool) {
  //  FIXME check if and basic block parent, if exec, exec; if parallel,
  //  parallel generate schedule map; should get from vendor report
  if (this->Type == FNDECL || this->Type == SUBKERNEL) {
    this->ExecFlow = false;
    if (Tool == "sdaccel" && !TopoIDExecMap.empty()) {
      if (TopoIDExecMap.find(this->TopoID) == TopoIDExecMap.end()) {
        this->ExecFlow = true;
      } else {
        this->ExecFlow = false;
      }
    }
  } else if (this->Type == LOOP) {
    if (TopoIDExecMap.find(this->TopoID) == TopoIDExecMap.end()) {
      this->ExecFlow = true;
    } else {
      this->ExecFlow = false;
    }
  } else if (this->Type == TOPKERNEL) {
    this->ExecFlow = true;
  } else {
    this->ExecFlow = false;
  }
  //  set all execute in serial, then modify it after cycles calculated
  if (Tool == "sdaccel" && TopoIDExecMap.empty()) {
    this->ExecFlow = false;
  }
  if (static_cast<int>(this->ExecFlow) == 0) {
    PRESTP("EST", " Block execute in serial.");
  } else {
    PRESTP("EST", " Block execute in parallel.");
  }
}

//  ************************************************************
//  Add and create child block for parent block
//  FIXME better to wrap for more sub functions
//  ************************************************************
void CPerfEstBlock::AddChildBlock_AOCL(
    string Tool, vector<CPerfEstBlock *> *AllBlockList,
    vector<CPerfEstBlock *> *AllFuncDeclList, map<string, string> TopoIDExecMap,
    const map<string, map<string, string>> &KernelGroup, void *TopKernel,
    void *ast_node) {
  int useful_child = 0;
  SgStatementPtrList child_list;
  SgStatementPtrList child_list_else;
  SgStatementPtrList child_list_tmp;
  SgStatementPtrList top_child_list;
  if (Type == TASK) {
    for (auto OneKernel : KernelGroup) {
      string TopKernelName = OneKernel.first;
      void *pKernel = codegen->GetFuncDeclByName(TopKernelName, 1);
      if (pKernel == nullptr) {
        printf("[Error] Kernel name not found.\n");
        return;
      }
      MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList, TOPKERNEL,
                    pKernel, NULL, TopoIDExecMap, KernelGroup, pKernel);
    }
  } else if (Type == TOPKERNEL) {
    // all sub kernels execute on top level in parallel
    top_child_list.clear();
    for (auto OneKernel : KernelGroup) {
      string TopKernelName = OneKernel.first;
      string InputKernelName = codegen->GetFuncName(TopKernel);
      if (InputKernelName == TopKernelName) {
        map<string, string> subkernel_file = OneKernel.second;
        for (auto sub : subkernel_file) {
          string kernel_name = sub.first;
          string type = sub.second;
          if (type == "subkernel") {
            void *kernel_decl = codegen->GetFuncDeclByName(kernel_name);
            top_child_list.push_back(
                isSgStatement(static_cast<SgNode *>(kernel_decl)));
          }
        }
        this->KernelName = TopKernelName;
      }
    }
    child_list = top_child_list;
  } else {
    codegen->GetChildStmtList(AstPtr, &child_list);
    //  Get child list for all kind of types
    if (Type == FNCALL) {
      //  if function call, switch to function declaration
      void *func_decl = codegen->GetFuncDeclByCall(AstPtr, 1);
      string func_name = codegen->GetFuncNameByCall(AstPtr, false);
      int already_processed = 0;
      if (func_decl != NULL) {
        CPerfEstBlock *ChildBlock = nullptr;
        //  Check if this func decl exists in AllFuncDeclList
        for (size_t k = 0; k < (*AllFuncDeclList).size(); k++) {
          if ((*AllFuncDeclList)[k]->AstPtr == func_decl) {
            this->FuncDecl = (*AllFuncDeclList)[k];
            already_processed = 1;
            break;
          }
        }
        if (already_processed == 0) {
          PRESTP("EST", " Create a function declaration block: "
                            << codegen->_p(func_decl, 30));
          //  If this func decl does not exist in AllFuncDeclList, create a new
          //  block
          ChildBlock = MakeNewBlocks(
              codegen, Tool, AllBlockList, AllFuncDeclList, FNDECL, func_decl,
              this, TopoIDExecMap, KernelGroup, TopKernel, ast_node);
          //  Mark the function declaration for the function call
          this->FuncDecl = ChildBlock;
        }
      }
    } else if (Type == FNDECL || Type == SUBKERNEL || Type == AUTOKERNEL) {
      //  get child statements for function declaration
      PRESTP("EST",
             " Find a function declaration: " << codegen->_p(AstPtr, 30));
      void *func_body = codegen->GetFuncBody(AstPtr);
      cout << "Function body = " << codegen->UnparseToString(func_body) << endl;
      if (func_body != nullptr) {
        codegen->GetChildStmtList(func_body, &child_list);
      } else {
        child_list.clear();
      }
    } else if (Type == MEMCPY) {
      PRESTP("EST",
             " Find a memcpy function, do not care about the inner logic: "
                 << codegen->_p(AstPtr, 30));
      return;
    } else if (Type == LOOP) {
      //  get child statements for loop
      PRESTP("EST", " Find a loop: " << codegen->_p(AstPtr, 30));
      void *loop_body = codegen->GetLoopBody(AstPtr);
      codegen->GetChildStmtList(loop_body, &child_list);
    } else if (Type == IF) {
      //  If statement will generate two children list
      //  get if children statement to normal children list
      PRESTP("EST", " Find a if: " << codegen->_p(AstPtr, 30));
      void *if_body = codegen->GetIfStmtTrueBody(AstPtr);
      child_list.clear();
      codegen->GetChildStmtList(if_body, &child_list);
      PRESTP("EST", "     If body : " << codegen->_p(if_body, 300));
      //  get else children statement to else children list
      //  FIXME if else body is a if statement, then child_list = esle_body
      //  now GetChildStmtList will get if body for child list
      void *else_body = codegen->GetIfStmtFalseBody(AstPtr);
      PRESTP("EST", "     Else body : " << codegen->_p(else_body, 300));
      if (static_cast<bool>(codegen->IsIfStatement(else_body))) {
        //        cout << "Else body is if." << endl;
        child_list_else.clear();
        child_list_else.push_back(
            isSgStatement(static_cast<SgNode *>(else_body)));
        //        cout << "Else body = " << codegen->UnparseToString(else_body)
        //        << endl;
      } else {
        //        cout << "Else body is normal." << endl;
        codegen->GetChildStmtList(else_body, &child_list_else);
      }
    } else if (Type == BASICBLOCK) {
      //  get children statements for basic block
      PRESTP("EST", " Find a basic block: " << codegen->_p(AstPtr, 30));
      codegen->GetChildStmtList(AstPtr, &child_list);
    } else {
    }
  }

  vector<CPerfEstBlock *> *SchedulePtrSpawn;
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  vector<CPerfEstBlock *> FuncCallSchedule;
  vector<CPerfEstBlock *> tempSchedule;
  this->GetExecFlowFromInput(TopoIDExecMap, Tool);

  //  scan each children list, and generate new block
  for (int i = 0; i < 2; i++) {
    if (i == 0) {  //  if notmal, parse child list and get schedule
      SchedulePtr = &Schedule;
      SchedulePtrSpawn = &ScheduleSpawn;
      child_list_tmp = child_list;
    } else if (i == 1) {  //  if else body, parse child for else
      SchedulePtr = &ScheduleElse;
      SchedulePtrSpawn = &ScheduleSpawn;
      child_list_tmp = child_list_else;
    }
    vector<CPerfEstBlock *> ChildBlockSet;
    for (size_t j = 0; j < child_list_tmp.size(); j++) {
      useful_child = 0;
      bool is_from_spawn_pragma = false;
      void *child = child_list_tmp[j];
      //      cout << "child = " << codegen->UnparseToString(child) << endl;
      if (codegen->IsLabelStatement(child) != 0) {
        child = codegen->GetStmtFromLabel(child);
      }

      //  Make new blocks and add to schedule based on the type of child
      CPerfEstBlock *ChildBlock = nullptr;
      if (static_cast<bool>(codegen->IsPragmaDecl(child))) {
        string sFilter;
        string sCmd;
        map<string, pair<vector<string>, vector<string>>> mapParams;
        string string_pragma = codegen->GetPragmaString(child);
        tldm_pragma_parse_whole(string_pragma, &sFilter, &sCmd, &mapParams);
        if (("ACCEL" != sFilter) || ("spawn-position" != sCmd)) {
          continue;
        }
        cout << "Find spawn-position pragma" << codegen->UnparseToString(child)
             << endl;
        string kernel_name;
        if (!mapParams["kernel"].first.empty()) {
          kernel_name = mapParams["kernel"].first[0];
          cout << "sub kernel name = " << kernel_name << endl;
        }
        void *parent_scope = GetParentScope(codegen, child);
        void *decl = codegen->GetFuncDeclByName(kernel_name);
        int param_number = codegen->GetFuncParamNum(decl);
        //=======================================================
        // sub kernel can be treated as normal kernel, so as memory burst
        // auto kernel is not normal kernel, need new algorithm, but do not need
        // to calculate memory burst
        //=======================================================
        bool is_auto_kernel = false;
        if (param_number == 0) {
          is_auto_kernel = true;
        }
        is_from_spawn_pragma = true;
        if (is_auto_kernel) {
          void *scope = codegen->GetFuncBody(decl);
          vector<void *> func_calls;
          codegen->GetNodesByType(scope, "preorder", "SgFunctionCallExp",
                                  &func_calls);
          for (auto call : func_calls) {
            string call_name = codegen->GetFuncNameByCall(call);
            if (call_name.substr(0, 19) != "read_channel_hidden" &&
                call_name.substr(0, 20) != "write_channel_hidden") {
              printf("call = %s\n", call_name.c_str());
              void *true_auto_decl = codegen->GetFuncDeclByName(call_name);
              PRESTP("EST", " Create a auto kernel block: "
                                << codegen->_p(true_auto_decl, 30));
              ChildBlock = MakeNewBlocks(
                  codegen, Tool, AllBlockList, AllFuncDeclList, AUTOKERNEL,
                  true_auto_decl, NULL, TopoIDExecMap, KernelGroup, TopKernel);
              ChildBlock->AutoKernelParentScope = parent_scope;
              int64_t depth = 1;
              if (!mapParams["factor"].first.empty()) {
                depth = atoi(mapParams["factor"].first[0].c_str());
              }
              ChildBlock->AutoKernelParentScope = parent_scope;
              ChildBlock->AutoKernelDepth = depth;
              useful_child = 1;
            }
          }
          //        } else {
          //            ChildBlock =
          //                MakeNewBlocks(codegen, Tool, AllBlockList,
          //                AllFuncDeclList,
          //                        SUBKERNEL, decl, NULL,
          //                        TopoIDExecMap, KernelGroup, TopKernel);
          //            PRESTP("EST", " Create a sub kernel block: "
          //                    << codegen->_p(decl, 30));
          //            useful_child = 1;
        }
      } else if (static_cast<bool>(codegen->IsFunctionDeclaration(child))) {
        //  Create a block for for sub-kernel
        PRESTP("EST", " Create a kernel block: " << codegen->_p(child, 30));
        ChildBlock = MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList,
                                   SUBKERNEL, child, NULL, TopoIDExecMap,
                                   KernelGroup, TopKernel);
        useful_child = 1;
      } else if (IsLoopNode(codegen, child)) {
        //  Create a block for for loop, while loop and do while
        PRESTP("EST", " Create a loop block: " << codegen->_p(child, 30));
        ChildBlock = MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList,
                                   LOOP, child, this, TopoIDExecMap,
                                   KernelGroup, TopKernel, ast_node);
        ChildBlockSet.push_back(ChildBlock);
        useful_child = 1;
      } else if (static_cast<bool>(codegen->IsIfStatement(child))) {
        //  create a block for if statement
        PRESTP("EST", " Create a if block: " << codegen->_p(child, 30));
        ChildBlock = MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList,
                                   IF, child, this, TopoIDExecMap, KernelGroup,
                                   TopKernel, ast_node);
        ChildBlock->ExecFlow = this->ExecFlow;
        ChildBlockSet.push_back(ChildBlock);
        useful_child = 1;
      } else if (static_cast<bool>(codegen->IsBasicBlock(child))) {
        //  create a basic block statement
        PRESTP("EST", " Create a basic block: " << codegen->_p(child, 30));
        ChildBlock = MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList,
                                   BASICBLOCK, child, this, TopoIDExecMap,
                                   KernelGroup, TopKernel, ast_node);
        ChildBlock->ExecFlow = this->ExecFlow;
        ChildBlockSet.push_back(ChildBlock);
        useful_child = 1;
      } else {
        //  if child have one or multiple function call
        //  Now if have multi-function calls in one statement, do not handle it
        //  well FIXME if one stme have multi-function call, suggest transform
        //  to one statement one call
        RoseAst ast(static_cast<SgNode *>(child));
        for (RoseAst::iterator it = ast.begin(); it != ast.end(); it++) {
          //  Finds function calls
          if (static_cast<bool>(codegen->IsFunctionCall(*it))) {
            void *func_call = *it;
            void *func_decl = codegen->GetFuncDeclByCall(func_call, 1);
            string func_name = codegen->GetFuncNameByCall(func_call, false);
            CPerfEstBlock *ChildBlockCall;
            if (IsFuncNameMemcpy(func_name)) {
              PRESTP("EST", " Create memcpy block: " << codegen->_p(child, 30));
              void *ExtPort = nullptr;
              void *ByteExprMemcpy = nullptr;
              int ReadWriteFlag = this->CheckIfRealMemcpy(
                  codegen, func_call, &ExtPort, &ByteExprMemcpy, this);
              //  Check if can find external export list
              if ((ExtPort != nullptr) && ReadWriteFlag > 0) {
                ChildBlockCall =
                    MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList,
                                  MEMCPY, func_call, this, TopoIDExecMap,
                                  KernelGroup, TopKernel, ast_node);
              } else {
                ChildBlockCall =
                    MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList,
                                  FNCALL, func_call, this, TopoIDExecMap,
                                  KernelGroup, TopKernel, ast_node);
              }
              FuncCallSchedule.push_back(ChildBlockCall);
              useful_child = 1;
            } else if (func_decl != nullptr &&
                       !codegen->isWithInHeaderFile(func_decl)) {
              PRESTP("EST", " Create a function call block: "
                                << codegen->_p(child, 30));
              ChildBlockCall =
                  MakeNewBlocks(codegen, Tool, AllBlockList, AllFuncDeclList,
                                FNCALL, func_call, this, TopoIDExecMap,
                                KernelGroup, TopKernel, ast_node);
              FuncCallSchedule.push_back(ChildBlockCall);
              useful_child = 1;
            }
          } else {  //  Not a function call
            //  Try to find a reference to external memory
            SgVarRefExp *Ref = isSgVarRefExp(static_cast<SgNode *>(*it));
            if ((Ref != nullptr) && (CheckIfFromFuncCall(codegen, Ref) == 0)) {
              void *InitName = codegen->GetVariableInitializedName(Ref);
              for (size_t ee = 0; ee < this->ExtPortList.size(); ee++) {
                if (InitName == ExtPortList[ee]) {
                  //  Excludes external memory reference in function argument
                  cout << "Find a external reference = "
                       << codegen->UnparseToString(Ref) << endl;
                  //  FIXME. currrently bound to parent loop directory
                  void *ParentNode = GetParentLoopOrFunc(codegen, Ref);
                  //  cout << "Parent Node = " <<
                  //  codegen->UnparseToString(ParentNode) << endl;
                  CPerfEstBlock *ParentBlock =
                      GetBlockFromNode(codegen, ParentNode, *AllBlockList);
                  //  cout << "Parent Block = " <<
                  //  codegen->UnparseToString(ParentBlock->AstPtr) << endl;
                  void *Type = codegen->GetTypebyVar(InitName);
                  int TypeSize = (codegen->get_type_unit_size(Type, Type) * 8);
                  if (ParentBlock->Type == LOOP) {
                    //  FIXME this may have issue if iteration is not outmost
                    //  index ex: for i < 10
                    //      for j < 20
                    //        for k < 20
                    //          a[j][k][i]
                    //  this only give a accese index bound, not the total size
                    ArrayRangeAnalysis range(InitName, codegen, ParentNode,
                                             ParentNode, false, false);
                    CMarsRangeExpr w_expr = range.GetFlattenRangeExprWrite();
                    CMarsExpression w_lb = w_expr.get_lb();
                    CMarsExpression w_ub = w_expr.get_ub();
                    CMarsExpression w_range = w_ub - w_lb + 1;
                    CMarsRangeExpr r_expr = range.GetFlattenRangeExprRead();
                    CMarsExpression r_lb = r_expr.get_lb();
                    CMarsExpression r_ub = r_expr.get_ub();
                    CMarsExpression r_range = r_ub - r_lb + 1;
                    int64_t size_w;
                    if (w_ub.GetConstant() == 0 && w_lb.GetConstant() == 0) {
                      size_w = 0;
                    } else {
                      size_w = w_range.GetConstant();
                    }
                    int64_t size_r;
                    if (r_ub.GetConstant() == 0 && r_lb.GetConstant() == 0) {
                      size_r = 0;
                    } else {
                      size_r = r_range.GetConstant();
                    }
                    int64_t data_size = size_w + size_r;
                    map<string, int64_t> ExtRefAttribute;
                    data_size *= TypeSize;
                    PRESTP("EST",
                           "Read data size = "
                               << size_r << ", r_ub = " << r_ub.GetConstant()
                               << ", r_lb = " << r_lb.GetConstant()
                               << ", for : " << codegen->UnparseToString(Ref));
                    PRESTP("EST",
                           "Write data size = "
                               << size_w << ", w_ub = " << w_ub.GetConstant()
                               << ", w_lb = " << w_lb.GetConstant()
                               << ", for : " << codegen->UnparseToString(Ref));
                    ExtRefAttribute.insert(
                        pair<string, int64_t>("DATASIZE", data_size));
                    ExtRefAttribute.insert(
                        pair<string, int64_t>("BITWIDTH", TypeSize));
                    (ParentBlock->GlobalMemoryAttributes)
                        .insert(pair<void *, map<string, int64_t>>(
                            Ref, ExtRefAttribute));
                    bool IsConsective =
                        CheckIfConsecutiveAccess(codegen, ParentNode, Ref);
                    ExtRefAttribute.insert(pair<string, int64_t>(
                        "ACCESS", static_cast<int64_t>(IsConsective)));
                    int64_t ReadWrite = 0;
                    if (w_range.GetConstant() > 0 &&
                        r_range.GetConstant() > 0) {
                      ReadWrite = 0;
                    } else if (r_range.GetConstant() > 0) {
                      ReadWrite = 1;
                    } else if (w_range.GetConstant() > 0) {
                      ReadWrite = 2;
                    } else {
                      ReadWrite = 3;
                    }
                    ExtRefAttribute.insert(
                        pair<string, int64_t>("READWRITE", ReadWrite));
                    PRESTP("EST", "One ref data size = "
                                      << data_size << " for : "
                                      << codegen->UnparseToString(Ref));
                    PRESTP("EST", "One ref type size = "
                                      << TypeSize << " for : "
                                      << codegen->UnparseToString(Ref));
                  } else if (ParentBlock->Type == FNDECL) {
                  }
                }
              }
            }
          }
        }
      }

      //  schedule for each node
      //  get flag from json file, inf json file all nodes in function execute
      //  in serial in default, all node in loop execute in parallel if loop
      //  marked serial execution, all nodes execute in parallel
      if (useful_child == 1) {
        if (is_from_spawn_pragma) {
          // add to specific vector for spawn pragma schedule
          (*SchedulePtrSpawn).push_back(ChildBlock);
          cout << "add spawn kernel to schedule "
               << codegen->UnparseToString(ChildBlock->AstPtr) << endl;
        } else {
          if (static_cast<int>(this->ExecFlow) == 0) {
            //          PRESTP("EST", " serial " << j << ", add a block to
            //          schedule: " << codegen->_p(ChildBlock->AstPtr, 30));
            // if execute in serial, push back a new vector
            if (!FuncCallSchedule.empty()) {  // if child block is null, it must
              // come from loop/if/basic block
              for (size_t i = 0; i < FuncCallSchedule.size(); i++) {
                tempSchedule.push_back(FuncCallSchedule[i]);
                (*SchedulePtr).push_back(tempSchedule);
                tempSchedule.clear();
              }
            } else {
              tempSchedule.push_back(ChildBlock);
              (*SchedulePtr).push_back(tempSchedule);
              tempSchedule.clear();
            }
          } else if (static_cast<int>(this->ExecFlow) == 1) {
            //          PRESTP("EST", " parallel " << j << ", add a block to
            //          schedule: " << codegen->_p(ChildBlock->AstPtr, 30));
            // if execute in parallel
            // generate first node for vector SchedulePtr
            if (!FuncCallSchedule.empty()) {  // only have function call
              for (size_t i = 0; i < FuncCallSchedule.size(); i++) {
                tempSchedule.push_back(FuncCallSchedule[i]);
                (*SchedulePtr).clear();
                (*SchedulePtr).push_back(tempSchedule);
              }
            } else {
              tempSchedule.push_back(ChildBlock);
              (*SchedulePtr).clear();
              (*SchedulePtr).push_back(tempSchedule);
            }
          }
        }
      }
      FuncCallSchedule.clear();
    }  //  for(j){...}
    //  } //  if(Type){...}-elif(){...}-else{...}
    //    PRESTP("EST", " - schedule sequntial size: " <<
    //    (*SchedulePtr).size()); for (size_t j = 0; j < (*SchedulePtr).size();
    //    j++) {
    //      PRESTP("EST", " - schedule parallel size: " <<
    //      (*SchedulePtr)[j].size());
    //    }
  }
}

//  ************************************************************
//  Generate new blocks ands external port list
//  ************************************************************
CPerfEstBlock *MakeNewBlocks(
    CSageCodeGen *codegen, string Tool, vector<CPerfEstBlock *> *AllBlockList,
    vector<CPerfEstBlock *> *AllFuncDeclList, BlockType Type, void *AstPtr,
    CPerfEstBlock *Parent, map<string, string> TopoIDExecMap,
    const map<string, map<string, string>> &KernelGroup, void *TopKernel,
    void *ast_node) {
  CPerfEstBlock *NewBlock;
  if (Type == TOPKERNEL) {
    NewBlock = new CPerfEstBlock(codegen, Type, AstPtr, NULL, Tool);
    PRESTP("EST", " Create top kernel: " << NewBlock->TopoID);
  } else if (Type == FNDECL || Type == SUBKERNEL) {
    NewBlock = new CPerfEstBlock(codegen, Type, AstPtr, NULL, Tool);
    (*AllFuncDeclList).push_back(NewBlock);
    if (Type == FNDECL) {
      PRESTP("EST", " Create func decl: " << NewBlock->TopoID);
    } else {
      PRESTP("EST", " Create sub kernel: " << NewBlock->TopoID);
    }
  } else if (Type == TASK) {
    NewBlock = new CPerfEstBlock(codegen, Type, nullptr, NULL, Tool);
    PRESTP("EST", " Create TOP TASK.");
  } else {
    NewBlock = new CPerfEstBlock(codegen, Type, AstPtr, Parent, Tool);
  }
  NewBlock->Kernel = TopKernel;
  (*AllBlockList).push_back(NewBlock);

  //  Find the external port list if it is function declaration
  if (Type == FNDECL || Type == SUBKERNEL) {
    if (Parent == NULL) {  //  If TOP FUNC, get exteral port
      vector<void *> empty_list;
      NewBlock->ExtPortList =
          FindExtPortList(codegen, empty_list, NULL, AstPtr);
    } else {  //  if not TOP, get exteral port list
      NewBlock->ExtPortList = FindExtPortList(
          codegen, GetFuncDeclBlock(codegen, Parent)->ExtPortList,
          Parent->AstPtr, AstPtr);
    }

    if (!NewBlock->ExtPortList.empty()) {
      PRESTP("EST", " ExtPortList: ");
      for (auto ele : NewBlock->ExtPortList) {
        PRESTP("EST", "    " << codegen->_p(ele));
      }
    }
  } else if (Type == TOPKERNEL || Type == AUTOKERNEL) {
  } else {
    if (Parent != nullptr) {
      CPerfEstBlock *FromFuncDecl = GetFuncDeclBlock(codegen, Parent);
      NewBlock->ExtPortList = FromFuncDecl->ExtPortList;
    }
  }

  //  Find & add child blocks
  NewBlock->AddChildBlock_AOCL(Tool, AllBlockList, AllFuncDeclList,
                               TopoIDExecMap, KernelGroup, TopKernel, ast_node);
  return NewBlock;
}

//  ************************************************************
//  Check if reald memcopy function call
//  if readl memcopy call, return is read or write port
//  output memcopy call and expression which is length of the call
//  ************************************************************
int CPerfEstBlock::CheckIfRealMemcpy(CSageCodeGen *codegen, void *AstPtr,
                                     void **ExtPort, void **ByteExprMemcpy,
                                     CPerfEstBlock *Parent) {
  int ReadWrite = 0;
  PRESTP("EST", " Check if real memcpy call.");
  vector<void *> ParentExtPortList =
      GetFuncDeclBlock(codegen, Parent)->ExtPortList;

  void *ReadExtPort = nullptr;
  void *WriteExtPort = nullptr;
  string func_name = codegen->GetFuncNameByCall(AstPtr, false);

  //  1. get expression of memcpy burst length
  //  2. get external port
  //  3. check if read or write memcpy
  if (func_name.find("merlin_memcpy_") != std::string::npos) {
    int ParamNumber = codegen->GetFuncCallParamNum(AstPtr);
    if (ParamNumber == 6) {
      FindRWMemcpy(codegen, AstPtr, ParentExtPortList, &ReadExtPort, 2,
                   &WriteExtPort, 0);
    } else if (ParamNumber == 7) {
      FindRWMemcpy(codegen, AstPtr, ParentExtPortList, &ReadExtPort, 3,
                   &WriteExtPort, 0);
    } else if (ParamNumber == 8) {
      FindRWMemcpy(codegen, AstPtr, ParentExtPortList, &ReadExtPort, 4,
                   &WriteExtPort, 0);
    } else {
      FindRWMemcpy(codegen, AstPtr, ParentExtPortList, &ReadExtPort, 2,
                   &WriteExtPort, 0);
    }
    if (ReadExtPort != nullptr) {
      ReadWrite = 1;
      *ExtPort = ReadExtPort;
    } else {
      ReadWrite = 2;
      *ExtPort = WriteExtPort;
    }
    *ByteExprMemcpy = codegen->GetFuncCallParam(
        AstPtr, codegen->GetFuncCallParamNum(AstPtr) - 1);
  } else if (func_name.find("memcpy_wide_bus_single") != std::string::npos) {
    int BusByteWidth;
    if (func_name.find("read") != std::string::npos) {  //  Read
      ReadExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                       codegen->GetFuncCallParam(AstPtr, 0));
      SgInitializedName *sgExtPort =
          isSgInitializedName(static_cast<SgNode *>(ReadExtPort));
      BusByteWidth = codegen->get_bitwidth_from_type(sgExtPort->get_type()) / 8;
      ReadWrite = 1;
    } else {  //  write
      WriteExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                        codegen->GetFuncCallParam(AstPtr, 0));
      SgInitializedName *sgExtPort =
          isSgInitializedName(static_cast<SgNode *>(WriteExtPort));
      BusByteWidth = codegen->get_bitwidth_from_type(sgExtPort->get_type()) / 8;
      ReadWrite = 2;
    }
    *ByteExprMemcpy = codegen->CreateConst(BusByteWidth);
  } else if (func_name.find("memcpy_wide_bus") != std::string::npos) {
    if (func_name.find("2d") != std::string::npos) {
      if (func_name.find("read") != std::string::npos) {
        ReadExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                         codegen->GetFuncCallParam(AstPtr, 3));
        *ExtPort = ReadExtPort;
        ReadWrite = 1;
      } else {
        WriteExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                          codegen->GetFuncCallParam(AstPtr, 0));
        *ExtPort = WriteExtPort;
        ReadWrite = 2;
      }
    } else if (func_name.find("3d") != std::string::npos) {
      if (func_name.find("read") != std::string::npos) {
        ReadExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                         codegen->GetFuncCallParam(AstPtr, 4));
        *ExtPort = ReadExtPort;
        ReadWrite = 1;
      } else {
        WriteExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                          codegen->GetFuncCallParam(AstPtr, 0));
        *ExtPort = WriteExtPort;
        ReadWrite = 2;
      }
    } else if (func_name.find("4d") != std::string::npos) {
      if (func_name.find("read") != std::string::npos) {
        ReadExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                         codegen->GetFuncCallParam(AstPtr, 5));
        *ExtPort = ReadExtPort;
        ReadWrite = 1;
      } else {
        WriteExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                          codegen->GetFuncCallParam(AstPtr, 0));
        *ExtPort = WriteExtPort;
        ReadWrite = 2;
      }
    } else {
      if (func_name.find("read") != std::string::npos) {
        ReadExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                         codegen->GetFuncCallParam(AstPtr, 1));
        *ExtPort = ReadExtPort;
        ReadWrite = 1;
      } else {
        WriteExtPort = FindExtPortFromExp(codegen, ParentExtPortList,
                                          codegen->GetFuncCallParam(AstPtr, 0));
        *ExtPort = WriteExtPort;
        ReadWrite = 2;
      }
    }
    *ByteExprMemcpy = codegen->GetFuncCallParam(
        AstPtr, codegen->GetFuncCallParamNum(AstPtr) - 1);
  } else if (codegen->GetFuncCallParamNum(AstPtr) == 3) {  //  memcpy
    FindRWMemcpy(codegen, AstPtr, ParentExtPortList, &ReadExtPort, 1,
                 &WriteExtPort, 0);
    if (ReadExtPort != nullptr) {
      ReadWrite = 1;
      *ExtPort = ReadExtPort;
    } else {
      ReadWrite = 2;
      *ExtPort = WriteExtPort;
    }
    *ByteExprMemcpy = codegen->GetFuncCallParam(
        AstPtr, codegen->GetFuncCallParamNum(AstPtr) - 1);
  } else {
    *ByteExprMemcpy = codegen->CreateConst(0);
    ERRORP("EST", "unknown memcpy");
  }
  if (*ExtPort != nullptr) {
    PRESTP("EST", " Is real memcpy call.");
  } else {
    PRESTP("EST", " Is fake memcpy call.");
  }
  return ReadWrite;
}

//  ************************************************************
//  get memcpy call cycles
//  return cycles of the memcpy call
//  ************************************************************
int64_t CPerfEstBlock::GetMemcpyCycles(CSageCodeGen *codegen) {
  void *AstPtr = this->AstPtr;
  void *ExtPort = nullptr;
  void *ByteExprMemcpy = nullptr;
  CPerfEstBlock *Parent = this->Parent;
  int ReadWriteFlag =
      CheckIfRealMemcpy(codegen, AstPtr, &ExtPort, &ByteExprMemcpy, Parent);
  this->MemoryReadWrite = ReadWriteFlag;

  int64_t Cycle = 0;
  int64_t Byte = 0;
  string func_name = codegen->GetFuncNameByCall(AstPtr, false);

  //  Get total bytes of memcpy
  PRESTP("EST", " start calculate range of "
                    << printNodeInfo(codegen, ByteExprMemcpy));
  //  Get bitwidth of memcpy
  SgInitializedName *sgExtPort =
      isSgInitializedName(static_cast<SgNode *>(ExtPort));
  int64_t BusByteWidth =
      codegen->get_bitwidth_from_type(sgExtPort->get_type()) / 8;
  PRESTP("EST", " - BusByteWidth for type ("
                    << codegen->_p(sgExtPort->get_type())
                    << "): " << BusByteWidth);

  CMarsExpression CME_ByteMemcpy(ByteExprMemcpy, codegen);
  if (CME_ByteMemcpy.IsConstant() != 0) {
    Byte = CME_ByteMemcpy.GetConstant();
  } else {
    CMarsRangeExpr RangeByte = CME_ByteMemcpy.get_range();
    if (RangeByte.is_const_ub() != 0) {
      Byte = RangeByte.get_const_ub();
      // TODO(youxiang): How to give warning if does not look feasible?
    }
  }
  if (func_name == "memcpy") {
    //  if memcpy, the last data means byte number, not element number
    Byte /= BusByteWidth;
  }
  PRESTP("EST", " - number of elements: " << Byte);

  if (func_name.find("memcpy_wide_bus") != std::string::npos) {
    // find the start end end point of var type position
    // extract string of type, then get bitwidh of the type
    size_t start_pos = 0;
    if (func_name.find("write") != std::string::npos) {
      start_pos = func_name.find("write");
      start_pos += 6;
    } else if (func_name.find("read") != std::string::npos) {
      start_pos = func_name.find("read");
      start_pos += 5;
    }
    string sub_str = func_name.substr(start_pos);
    size_t end_pos = sub_str.find_first_of("_");
    string type_str = sub_str.substr(0, end_pos);
    cout << "type of string = " << type_str << endl;
    void *created_type = codegen->GetTypeByString(type_str);
    int bitwidth = codegen->get_bitwidth_from_type(created_type);
    cout << "bitwidth of type = " << bitwidth << endl;
    Byte *= bitwidth / 8;
  } else if (func_name == "memcpy") {
    Byte *= BusByteWidth;
  }
  PRESTP("EST", " - number of Bytes: " << Byte);

  this->MerlinMemcpyDataSize = Byte * 8;
  this->MerlinMemcpyBitwidth = BusByteWidth * 8;

  //  Sometimes Merlin memcpy II != 1, need to calculate it
  //  calcylate cycles
  Cycle = Byte / BusByteWidth;
  PRESTP("EST", " - Cycles without II: " << Cycle);
  Cycle *= this->II;
  PRESTP("EST", " - Cycles with II: " << Cycle << ", II = " << this->II);
  if (Cycle <= 1) {
    Cycle = 1;
  }
  PRESTP("EST", " - Final Cycles : " << Cycle);
  return Cycle;
}

//  ************************************************************
//  check if empty loop
//  return 1 if empty loop
//  ************************************************************
bool CPerfEstBlock::IfEmptyLoop(CSageCodeGen *codegen) {
  void *AstPtr = this->AstPtr;
  SgStatementPtrList child_list;
  void *loop_body = codegen->GetLoopBody(AstPtr);
  codegen->GetChildStmtList(loop_body, &child_list);
  if (child_list.empty()) {
    cout << "Is empty loop" << endl;
    return true;
  }
  return false;
}

//  ************************************************************
//  get CPerfEstBlock from ID string
//  ************************************************************
CPerfEstBlock *GetBlockFromID(CSageCodeGen *codegen, string ID,
                              vector<CPerfEstBlock *> AllBlockList) {
  for (auto OneBlock : AllBlockList) {
    if (OneBlock->TopoID == ID) {
      return OneBlock;
    }
  }
  return nullptr;
}

//  ************************************************************
//  get CPerfEstBlock from ast node
//  ************************************************************
CPerfEstBlock *GetBlockFromNode(CSageCodeGen *codegen, void *Node,
                                vector<CPerfEstBlock *> AllBlockList) {
  string TopoID = getTopoLocation(codegen, static_cast<SgNode *>(Node));
  return GetBlockFromID(codegen, TopoID, AllBlockList);
}

//  ************************************************************
//  get the inner loop trip count product for a flatten loop
//  ************************************************************
void CPerfEstBlock::GetInnerLoopTripcountProduct(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList) {
  cout << "Set inner loop tripcount product." << endl;
  int64_t TCProduct = 1;
  string TopoID = this->TopoID;
  while (true) {
    bool Found = false;
    for (auto OneBlock : AllBlockList) {
      string FlattenID = OneBlock->FlattenID;
      //          cout << "TopoID = " << TopoID << ", FlattenID = " << FlattenID
      //          << endl;
      if (TopoID == FlattenID) {
        TopoID = OneBlock->TopoID;
        TCProduct *= OneBlock->TCStatic;
        //              cout << "TopoID = " << TopoID << ", TC = " <<
        //              OneBlock->TCStatic << endl;
        Found = true;
        break;
      }
    }
    if (!Found) {
      cout << "Get product = " << TCProduct << endl;
      this->InnerLoopTripcountProduct = TCProduct;
      return;
    }
  }
}

//  ************************************************************
//  transfer serial to parallel
//  ************************************************************
void CPerfEstBlock::ReSchedule(vector<vector<CPerfEstBlock *>> *SchedulePtr) {
  vector<vector<CPerfEstBlock *>> NewSchedule;
  vector<CPerfEstBlock *> ParallelSchedule;
  PRESTP("EST", " - schedule sequntial size: " << (*SchedulePtr).size());
  for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
    PRESTP("EST", " - schedule parallel size: " << (*SchedulePtr)[j].size());
    for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
      CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
      ParallelSchedule.push_back(ChildBlock);
    }
  }
  NewSchedule.push_back(ParallelSchedule);
  this->Schedule = NewSchedule;
}

// ************************************************************
// reschedule by adb file
// FIXME: this may have issues, because current assume all parallel execution
// based on multi-port, if child across blocks, this may have issue
// ************************************************************
void CPerfEstBlock::ReScheduleByADB(
    CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *SchedulePtr) {
  //  Get all Blocks from schedule
  map<string, CPerfEstBlock *> NameBlockMap;
  vector<CPerfEstBlock *> BlockList;
  vector<vector<CPerfEstBlock *>> NewSchedule;
  vector<CPerfEstBlock *> ParallelSchedule;
  PRESTP("EST", " - schedule sequntial size: " << (*SchedulePtr).size());
  for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
    PRESTP("EST", " - schedule parallel size: " << (*SchedulePtr)[j].size());
    for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
      CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
      ParallelSchedule.push_back(ChildBlock);
      BlockList.push_back(ChildBlock);
    }
  }
  cout << "block size = " << BlockList.size() << endl;
  //  Get Name Block map of every sub blocks
  for (auto OneBlock : BlockList) {
    string Name;
    void *Node = OneBlock->AstPtr;
    if (OneBlock->Type == FNCALL) {
      Name = codegen->GetFuncNameByCall(Node);
    } else if (OneBlock->Type == MEMCPY) {
      Name = codegen->GetFuncNameByCall(Node);
    } else {
      Name = codegen->UnparseToString(Node);
    }
    NameBlockMap.insert(pair<string, CPerfEstBlock *>(Name, OneBlock));
  }

  vector<CPerfEstBlock *> ProcessedBlock;
  vector<string> ProcessedName;
  //  analyze serial states
  for (auto L1 : this->StateVector) {
    vector<CPerfEstBlock *> TmpSchedule;
    //  analyze parallel state, same func name only analyze once
    for (auto L2 : L1) {
      string func_name = L2;
      //      cout << "func name = " << func_name << endl;
      if (NameBlockMap.find(func_name) != NameBlockMap.end() &&
          find(ProcessedName.begin(), ProcessedName.end(), func_name) ==
              ProcessedName.end()) {
        cout << "Find block for this func name" << endl;
        TmpSchedule.push_back(NameBlockMap[func_name]);
        ProcessedBlock.push_back(NameBlockMap[func_name]);
        ProcessedName.push_back(func_name);
      }
    }
    NewSchedule.push_back(TmpSchedule);
  }

  //  Find all block list which is not in StateVector, and list for serial
  //  if function call multiple times, then just need to detect once
  //  same function name block execute in serial
  for (auto OneBlock : BlockList) {
    if (find(ProcessedBlock.begin(), ProcessedBlock.end(), OneBlock) ==
        ProcessedBlock.end()) {
      cout << "Find one block " << codegen->UnparseToString(OneBlock->AstPtr)
           << " which not in StateVector" << endl;
      vector<CPerfEstBlock *> TmpSchedule;
      TmpSchedule.push_back(OneBlock);
      NewSchedule.push_back(TmpSchedule);
    }
  }

  PRESTP("EST", " reschedule sequntial size: " << NewSchedule.size());
  for (size_t j = 0; j < NewSchedule.size(); j++) {
    PRESTP("EST", " reschedule parallel size: " << NewSchedule[j].size());
    for (size_t k = 0; k < NewSchedule[j].size(); k++) {
    }
  }
  this->Schedule = NewSchedule;
}

//  ************************************************************
//  reschedule by data flow, all execute in parallel
//  ************************************************************
void CPerfEstBlock::ReScheduleByDataflow(
    CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *SchedulePtr,
    void *AstPtr) {
  bool has_dataflow = false;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(AstPtr, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  vector<void *> vecTopKernels;
  for (size_t i = vecTldmPragmas.size() - 1; i < vecTldmPragmas.size(); i--) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams, true);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);
    if ("HLS" != sFilter || "DATAFLOW" != sCmd) {
      continue;
    }
    has_dataflow = true;
    cout << "find dataflow pragma" << endl;
  }
  //  Get all Blocks from schedule
  if (has_dataflow) {
    vector<CPerfEstBlock *> BlockList;
    vector<vector<CPerfEstBlock *>> NewSchedule;
    PRESTP("EST", " - schedule sequntial size: " << (*SchedulePtr).size());
    for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
      PRESTP("EST", " - schedule parallel size: " << (*SchedulePtr)[j].size());
      for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
        CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
        BlockList.push_back(ChildBlock);
      }
    }
    NewSchedule.push_back(BlockList);
    this->Schedule = NewSchedule;
  }
}

//  ************************************************************
//  calculate cycles for loop
//  ************************************************************
void CPerfEstBlock::ComputeStaticCyclesForLoop(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList) {
  //  For intel if memory burst loop. set II=1. IL=1
  {
    //  If pipeline loop or flatten to pipeline loop or innermost loop,
    //  we directly use vendor loop iteration latency
    if (this->VendorLatency > this->IL) {
      if ((this->Pipelined || this->IsFlattenToPipeline ||
           (!(this->HaveNotUnrollLoop)))) {
        this->IL = this->VendorLatency;
        cout << "Update IL by vendor latency." << endl;
      }
    }
  }
  cout << "Final IL = " << this->IL
       << ", vendor latency = " << this->VendorLatency << endl;

  //  calculate effective TC
  int64_t TmpTCStatic = -1;
  if (this->TCStatic == 0) {
    TmpTCStatic = 0;
  } else if (this->TCStatic == -1) {
    TmpTCStatic = -1;
  } else {
    if (this->TCStatic % this->UnrollFactor == 0) {
      TmpTCStatic = this->TCStatic / this->UnrollFactor;
    } else {
      TmpTCStatic = this->TCStatic / this->UnrollFactor + 1;
    }
  }

  if (TmpTCStatic == -1 || this->IL == -1) {
    this->CycleUnitStatic = -1;
    this->LogicLatencyIteration = 0;
    this->GlueLatencyIteration = 0;
    this->LogicLatency = 0;
    this->GlueLatency = 0;
    return;
  } else if (TmpTCStatic == 0) {
    this->CycleUnitStatic = 0;
    this->LogicLatencyIteration = 0;
    this->GlueLatencyIteration = 0;
    this->LogicLatency = 0;
    this->GlueLatency = 0;
    return;
  } else if (this->IsFlatten) {
    this->GetFlattenTripCount(codegen, AllBlockList);
    CPerfEstBlock *InnermostBlock =
        GetBlockFromID(codegen, this->InnermostFlattenID, AllBlockList);
    //  if flatten loop, and loop latencyh is ?
    //  vendor latency marked as innermost loop IL
    this->CalculatedFlattenIL = InnermostBlock->IL;
    if (this->IsFlattenToPipeline) {
      //  loop flatten to pipeline, each loop data is the same
      this->IL = InnermostBlock->IL;
      this->GlueLatencyIteration = InnermostBlock->GlueLatencyIteration;
      this->CycleUnitStatic = (this->FlattenTripCount - 1) * this->IIOrg +
                              this->IL + this->GlueLatencyIteration;
      PRESTP("EST",
             " Loop flatten to pipelined"
                 << ", TC=" << this->FlattenTripCount
                 << ", UnrollFactor=" << this->UnrollFactor
                 << ", II=" << this->IIOrg << ", IL=" << this->IL
                 << ", LogicLatencyIteration=" << this->LogicLatencyIteration
                 << ", GlueLatencyIteration=" << this->GlueLatencyIteration
                 << ", CycleUnitStatic=" << this->CycleUnitStatic);
    } else {
      //  loop flatten not to pipeline, calculate by itself, each loop data
      //  is different
      this->CycleUnitStatic =
          TmpTCStatic * (this->IL + this->GlueLatencyIteration);
      PRESTP("EST",
             " Loop flatten not to pipelined, TC="
                 << TCStatic << ", UnrollFactor=" << this->UnrollFactor
                 << ", II=" << this->II << ", IL=" << this->IL
                 << ", LogicLatencyIteration=" << this->LogicLatencyIteration
                 << ", GlueLatencyIteration=" << this->GlueLatencyIteration
                 << ", CycleUnitStatic=" << this->CycleUnitStatic);
    }
  } else {
    if (static_cast<int>(this->Pipelined) == 1) {
      //  if pipeline loop, directly calculate unit cycles
      this->CycleUnitStatic =
          (TmpTCStatic - 1) * this->II + this->IL + this->GlueLatencyIteration;
      PRESTP("EST",
             " Loop pipelined, TC="
                 << TCStatic << ", UnrollFactor=" << this->UnrollFactor
                 << ", II=" << this->II << ", IL=" << this->IL
                 << ", LogicLatencyIteration=" << this->LogicLatencyIteration
                 << ", GlueLatencyIteration=" << this->GlueLatencyIteration
                 << ", CycleUnitStatic=" << this->CycleUnitStatic);
    } else {
      //  if not pipeline loop, gei all children total cycles as IL
      this->CycleUnitStatic =
          TmpTCStatic * (this->IL + this->GlueLatencyIteration);
      PRESTP("EST",
             " Loop not pipelined, TC="
                 << TCStatic << ", UnrollFactor=" << this->UnrollFactor
                 << ", II=" << this->II << ", IL=" << this->IL
                 << ", LogicLatencyIteration=" << this->LogicLatencyIteration
                 << ", GlueLatencyIteration=" << this->GlueLatencyIteration
                 << ", CycleUnitStatic=" << this->CycleUnitStatic);
    }
  }

  //  if effective fully unroll loop, and IL <= 1,  static cycle = 0,
  if (TmpTCStatic == 1 && this->UnrollFactor > 0 && this->IL <= 1) {
    cout << "Find fully unroll loop with IL > 1." << endl;
    this->CycleUnitStatic = 0;
    this->LogicLatencyIteration = 0;
    this->GlueLatencyIteration = 0;
    this->LogicLatency = 0;
    this->GlueLatency = 0;
    return;
  }
  return;
}

//  ************************************************************
//  calculate glue cycles for loop
//  ************************************************************
void CPerfEstBlock::ComputeGlueCyclesForLoop(
    CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *SchedulePtr,
    const vector<CPerfEstBlock *> &AllBlockList) {
  if (this->IL == -1 || this->TCStatic == -1) {
    this->LogicLatencyIteration = 0;
    this->GlueLatencyIteration = 0;
    this->LogicLatency = 0;
    this->GlueLatency = 0;
    return;
  }
  //  In sdaccel, calculate glue latency and logic latency
  if (this->Tool == "sdaccel" && !(this->DataUpdated)) {
    PRESTP("EST", " Compute Glue cycles for loop.");
    //  if trip count = 1 loop may not shown in vendor report
    if (this->TCStatic == 1) {
      //      this->LogicLatency = this->LogicLatencyIteration;
      this->LogicLatency = this->IL;
      this->GlueLatency = this->GlueLatencyIteration;
      return;
    }
    //  if vendor data is ?, directly use calculated date before
    if (this->IterationLatencyMax == -1 || this->LatencyMax == -1) {
      //      this->IterationLatencyMax = this->LogicLatencyIteration;
      if (this->IsFlatten) {
        this->IterationLatencyMax = this->CalculatedFlattenIL;
        this->LatencyMax = this->CycleUnitStatic;
      }
    }
    PRESTP("EST",
           " IterationLatencyMax = " << this->IterationLatencyMax
                                     << ", LatencyMax = " << this->LatencyMax
                                     << ", AccCycles = " << this->AccCycles);
    //  In default, execute in serial, the calculated data is accumulate cycle
    if (this->IterationLatencyMax > 0 && this->LatencyMax >= 0) {
      this->CycleUnitStatic = this->LatencyMax;
      this->LogicLatencyIteration = this->IterationLatencyMax;
      //  if vendor latency > sub block accumate latency, serial exec
      //  else, parallel exec
      //      if (!(this->DataUpdated)) {
      if (this->LogicLatencyIteration >= this->AccCycles) {
        this->ExecFlow = false;
        PRESTP("EST", " SDx execute in serial");
      } else {
        this->ExecFlow = true;
        PRESTP("EST", " SDx execute in parallel");
        PRESTP("EST", "Reschedule " << codegen->UnparseToString(this->AstPtr));
        this->ReSchedule(SchedulePtr);
      }
      //      }
      //  if flatten loop, cycles must have accurate cycles
      if (this->IsFlatten) {
        //  if flatten pipeline and not outmost loop, all = 0
        //  if flatten pipeline and outmost loop, all = vendor data
        //  If flatten no pileline, calculate each loop unit cycles
        if (this->IsFlattenToPipeline) {
          if (this->IsInnermostFlattenLoop) {
            PRESTP("EST", " Is innermost flatten loop and pipelined.");
            this->CycleUnitStatic = this->LatencyMax;
            this->LogicLatencyIteration = this->IterationLatencyMax;
            this->GlueLatencyIteration = 0;
            this->LogicLatency = this->LatencyMax;
            this->GlueLatency = 0;
          } else {
            PRESTP("EST", " Is not innermost flatten loop and pipelined.");
            CPerfEstBlock *InnermostBlock =
                GetBlockFromID(codegen, this->InnermostFlattenID, AllBlockList);
            this->CycleUnitStatic = this->LatencyMax;
            this->LogicLatencyIteration = InnermostBlock->LogicLatencyIteration;
            this->GlueLatencyIteration = InnermostBlock->GlueLatencyIteration;
            this->LogicLatency = InnermostBlock->LogicLatency;
            this->GlueLatency = InnermostBlock->GlueLatency;
          }
        } else {
          this->GetInnerLoopTripcountProduct(codegen, AllBlockList);
          if (this->IsInnermostFlattenLoop) {
            PRESTP("EST", " Is flatten loop and not pipelined innermost.");
            this->CycleUnitStatic = this->IterationLatencyMax * this->TCStatic;
            this->LogicLatencyIteration = this->AccCycles;
            this->GlueLatencyIteration =
                this->IterationLatencyMax - this->AccCycles;
            this->LogicLatency = this->CycleUnitStatic;
            this->GlueLatency = 0;
          } else {
            PRESTP("EST", " Is flatten loop and not pipelined non-innermost.");
            //  FIXME: If outmost loop vendor latency larger than unit cycle
            //  Should use vendor latency
            //  Current not use, but did not have serial impact
            this->CycleUnitStatic =
                this->IterationLatencyMax *
                //                                    this->InnerLoopTripcountProduct
                //                                    *
                this->TCStatic;
            this->LogicLatencyIteration =
                this->IterationLatencyMax * this->InnerLoopTripcountProduct;
            //  this->CycleUnitStatic = this->IterationLatencyMax *
            //  this->TCStatic; this->LogicLatencyIteration =
            //  this->IterationLatencyMax;
            this->GlueLatencyIteration = 0;
            this->LogicLatency = this->CycleUnitStatic;
            this->GlueLatency = 0;
          }
        }
      } else if (this->IterationLatencyMax != 0 && this->LatencyMax != 0) {
        //  first get calculated logic latency
        this->LogicLatencyIteration = this->LogicLatency;
        //  If parallel execution loop
        //      iteration glue latency = 0
        //      iteration logic latency = iteration latency
        //  If pipeline loop, then must have no call or fully unrolled loop
        //      iteration glue latency = 0
        //      iteration logic latency = iteration latency
        //  If no pipeline loop
        //  1. If without non-unroll loop inside,
        //      iteration glue latency = 0
        //      iteration logic latency = iteration latency
        //  2. If with non-unroll loop inside
        //      iteration logic latency = acc latency
        //      iteration glue latency = iteration latency - acc latency
        if (static_cast<int>(this->Pipelined) == 1 ||
            static_cast<int>(this->ExecFlow) == 1) {
          PRESTP("EST", " With pipeline children.");
          this->LogicLatencyIteration = this->IterationLatencyMax;
          this->GlueLatencyIteration = 0;
        } else {
          if (!this->HaveNotUnrollLoop) {
            this->LogicLatencyIteration = this->IterationLatencyMax;
            this->GlueLatencyIteration = 0;
            PRESTP("EST", " Loop body is clean, no not unroll loop.");
          } else {
            this->LogicLatencyIteration = this->AccCycles;
            this->GlueLatencyIteration =
                this->IterationLatencyMax - this->AccCycles;
            PRESTP("EST", " Loop body is not clean, have not unroll loop.");
          }
        }
        this->LogicLatency = this->CycleUnitStatic;
        this->GlueLatency = 0;
      } else {
        this->LogicLatencyIteration = 0;
        this->GlueLatencyIteration = 0;
        this->LogicLatency = 0;
        this->GlueLatency = 0;
      }
    }
  }
}

//  ************************************************************
//  check if unroll sucessful for multi-instance
//  current only check function call instance
// TODO(youxiang): for loop instance, still need real case to coding and test
//  ************************************************************
void CPerfEstBlock::CheckIfUnrollSuccess(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllBlockList) {
  cout << "CheckIfUnrollSuccess " << codegen->UnparseToString(this->AstPtr)
       << endl;
  if ((codegen->IsFunctionDeclaration(this->AstPtr) != 0) ||
      IsLoopNode(codegen, this->AstPtr)) {
    map<string, string> MapID = this->RequireToCheckInstanceAndLoopID;
    for (auto OnePair : MapID) {
      string MultiInstanceID = OnePair.first;
      string UnrollLoopID = OnePair.second;
      CPerfEstBlock *MultiInstanceBlock =
          GetBlockFromID(codegen, MultiInstanceID, AllBlockList);
      CPerfEstBlock *UnrollLoopBlock =
          GetBlockFromID(codegen, UnrollLoopID, AllBlockList);
      int64_t CalculatedCycles = this->AccCycles;
      int64_t InstanceLatency = MultiInstanceBlock->CycleUnitStatic;
      int64_t InstanceNumber = MultiInstanceBlock->InstanceNumber;
      int64_t Delta2 = (InstanceNumber - 1) * InstanceLatency;
      int64_t VendorCycles = 0;
      if (codegen->IsFunctionDeclaration(this->AstPtr) != 0) {
        VendorCycles = this->CycleUnitStatic;
      } else if (IsLoopNode(codegen, this->AstPtr)) {
        VendorCycles = this->VendorLatency;
      }
      int64_t Delta = std::abs(CalculatedCycles - VendorCycles);
      double Ratio = (Delta * 1.0) / (Delta2 * 1.0);
      double RatioRef = 1.0 / (InstanceNumber * 1.0);
      if (Ratio > RatioRef) {
        UnrollLoopBlock->UnrollFailed = true;
        //  if unroll failed, then glue latency of loop is not reliable
        this->GlueLatency = 0;
        this->GlueLatencyIteration = 0;
      }
      cout << "CalculatedCycles = " << CalculatedCycles
           << ", VendorCycles = " << VendorCycles
           << ", InstanceLatency = " << InstanceLatency
           << ", InstanceNumber = " << InstanceNumber << ", Delta = " << Delta
           << ", Delta2 = " << Delta2 << ", Ratio = " << Ratio
           << ", RatioRef = " << RatioRef
           << ", Final success of fail = " << UnrollLoopBlock->UnrollFailed
           << "\n          Unroll Loop = "
           << codegen->UnparseToString(UnrollLoopBlock->AstPtr) << endl;
    }
  }
}

//  ************************************************************
//  calculate cycles for func
//  ************************************************************
void CPerfEstBlock::ComputeStaticCyclesForFunc(
    CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *SchedulePtr,
    const vector<CPerfEstBlock *> &AllBlockList) {
  this->AccCycles = this->CycleUnitStatic;
  if (this->Tool == "sdaccel") {
    // if have vendor latency, use vendor latency
    // if all child cycles is normal, then total cycles is normal
    // if one of the child cycles is -1, then parent cycles is -1
    if (this->LatencyMax >= 0) {
      this->CycleUnitStatic = this->LatencyMax;
      //    } else if (this->AccCycles > 0) {
      //      this->CycleUnitStatic = this->AccCycles;
    } else {
      this->CycleUnitStatic = -1;
    }
  }
  if (this->CycleUnitStatic == -1) {
    this->LogicLatency = 0;
    this->GlueLatency = 0;
    return;
  }
  cout << "Logic latency = " << this->LogicLatency
       << ", Unit cycles = " << this->CycleUnitStatic
       << ", AccCycles = " << this->AccCycles << endl;
  if (this->Tool == "sdaccel" && !(this->DataUpdated) &&
      !(this->UpdatedByADB)) {
    //  if vendor latency > sub block accumate latency, serial exec
    //  else, parallel exec
    if (this->CycleUnitStatic >= this->AccCycles) {
      this->ExecFlow = false;
      PRESTP("EST", " SDx execute in serial");
    } else {
      this->ExecFlow = true;
      //  FIXME need switch schedule size to parallel
      PRESTP("EST", " SDx execute in parallel");
      PRESTP("EST", "Reschedule " << codegen->UnparseToString(this->AstPtr));
      this->ReSchedule(SchedulePtr);
    }
  }

  if (!(this->DataUpdated)) {
    if (static_cast<int>(this->ExecFlow) == 1) {
      this->GlueLatency = 0;
    } else {
      this->GlueLatency = this->CycleUnitStatic - this->LogicLatency;
    }
  } else {
    if (static_cast<int>(this->ExecFlow) == 0) {
      this->CycleUnitStatic += this->GlueLatency;
    }
  }

  if (this->IsDead) {
    this->CycleUnitStatic = 0;
    this->LogicLatency = 0;
    this->GlueLatency = 0;
  }
}

void CPerfEstBlock::MarkParentAsRequireToCheck(
    CSageCodeGen *codegen, int64_t InstanceNumber,
    const vector<CPerfEstBlock *> &AllBlockList) {
  cout << "MarkParentAsRequireToCheck" << endl;
  string MultiInstanceID = this->TopoID;
  string UnrollLoopID;
  void *MultiInstanceNode = this->AstPtr;
  if (!((codegen->IsFunctionCall(MultiInstanceNode) != 0) ||
        IsLoopNode(codegen, MultiInstanceNode))) {
    //  if not function call or loop, return directly
    return;
  }
  cout << "MultiInstanceNode Parent = "
       << codegen->UnparseToString(MultiInstanceNode) << endl;

  void *Parent = codegen->GetParent(MultiInstanceNode);
  while (true) {
    //  If did not trace to unroll loop, then just ignore the multi-instance
    //  do not need to mark
    if ((Parent == nullptr) || (codegen->IsFunctionDeclaration(Parent) != 0)) {
      return;
    }
    cout << "Mark Parent = " << codegen->UnparseToString(Parent) << endl;

    if (IsLoopNode(codegen, Parent)) {
      string TopoID = getTopoLocation(codegen, static_cast<SgNode *>(Parent));
      CPerfEstBlock *ParentBlock =
          GetBlockFromID(codegen, TopoID, AllBlockList);
      if (ParentBlock->UnrollFactor > 1) {
        UnrollLoopID = TopoID;
        break;
      }
    }
    Parent = codegen->GetParent(Parent);
  }
  cout << "Mark Unroll Loop  = " << codegen->UnparseToString(Parent) << endl;
  Parent = codegen->GetParent(Parent);
  while (true) {
    if (Parent == nullptr) {
      return;
    }
    cout << "Mark Parent = " << codegen->UnparseToString(Parent) << endl;

    string TopoID = getTopoLocation(codegen, static_cast<SgNode *>(Parent));
    CPerfEstBlock *ParentBlock = GetBlockFromID(codegen, TopoID, AllBlockList);
    if (IsLoopNode(codegen, Parent) ||
        (codegen->IsFunctionDeclaration(Parent) != 0)) {
      ParentBlock->RequireToCheckInstanceAndLoopID.insert(
          pair<string, string>(MultiInstanceID, UnrollLoopID));
      cout << "Mark on this one." << endl;
      break;
    }
    Parent = codegen->GetParent(Parent);
  }
}

//  ************************************************************
//  compute initial cycle static
//  FIXME require to split to small sub functions
//  ************************************************************
void CPerfEstBlock::ComputeStaticCycleHelper(
    CSageCodeGen *codegen, vector<CPerfEstBlock *> *AllFuncDeclList,
    ExtMemParam *ExtMemParamData, CPerfEstBlock *PipeParent,
    const vector<CPerfEstBlock *> &AllBlockList,
    vector<CPerfEstBlock *> TopKernelBlock) {
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  vector<vector<CPerfEstBlock *>> *SchedulePtrElse;
  vector<CPerfEstBlock *> *SchedulePtrSpawn;
  SchedulePtr = &Schedule;
  SchedulePtrElse = &ScheduleElse;
  SchedulePtrSpawn = &ScheduleSpawn;
  bool have_valid_child =
      (!(*SchedulePtrSpawn).empty()) || (!(*SchedulePtr).empty());
  bool have_valid_child_else =
      (!(*SchedulePtrSpawn).empty()) || (!(*SchedulePtrElse).empty());
  cout << "ComputeStaticCycleHelper spawn size = " << ScheduleSpawn.size()
       << endl;
  // FIXME if schedule size equal 0, initial unit cycle to 1
  if (!have_valid_child) {
    this->CycleUnitStatic = 1;
  }

  if (Type == TASK) {
    PRESTP("EST", " Start compute cycles for TASK. ");
    for (auto OneTopKernelBlock : TopKernelBlock) {
      OneTopKernelBlock->ComputeStaticCycleHelper(
          codegen, AllFuncDeclList, ExtMemParamData, nullptr, AllBlockList);
      PRESTP("EST", " Unit cycle of top kernel:"
                        << OneTopKernelBlock->CycleUnitStatic);
      PRESTP("EST", " FIRST_CYCLES=" << OneTopKernelBlock->CycleUnitStatic);
    }
    return;
  }
  if (Type == TOPKERNEL) {
    PRESTP("EST",
           " Start compute cycles for top kernel: " << codegen->_p(AstPtr));
    vector<int64_t> DataSet = ComputeStaticCycleFromSchedule(
        codegen, SchedulePtr, SchedulePtrSpawn, AstPtr, AllFuncDeclList,
        ExtMemParamData, PipeParent, AllBlockList);
    this->CycleUnitStatic = DataSet[0];
    this->LogicLatency = DataSet[1];
    this->GlueLatency = DataSet[2];
    //  If no valid subblocks, set cycle to 1
    if (this->CycleUnitStatic == 0) {
      this->CycleUnitStatic = 1;
    }
    PRESTP("EST", " Finish compute cycles for top kernel: "
                      << codegen->_p(AstPtr)
                      << ", Cycles: " << this->CycleUnitStatic);
  } else if (Type == SUBKERNEL || Type == FNDECL) {
    PRESTP("EST", " Start compute cycles for function declaration: "
                      << codegen->_p(AstPtr));
    //  If get some information from adb.schedule.xml file, update schedule
    if (!(this->StateVector).empty()) {
      PRESTP("EST",
             "RescheduleByADB " << codegen->UnparseToString(this->AstPtr));
      cout << "Vector size = " << (this->StateVector).size() << endl;
      this->ReScheduleByADB(codegen, SchedulePtr);
      SchedulePtr = &Schedule;
      this->UpdatedByADB = true;
    }
    //  update schedule by dataflow, if no data flow, no impact
    this->ReScheduleByDataflow(codegen, SchedulePtr, this->AstPtr);
    SchedulePtr = &Schedule;
    if (have_valid_child) {
      vector<int64_t> DataSet = ComputeStaticCycleFromSchedule(
          codegen, SchedulePtr, SchedulePtrSpawn, AstPtr, AllFuncDeclList,
          ExtMemParamData, PipeParent, AllBlockList);
      this->CycleUnitStatic = DataSet[0];
      this->LogicLatency = DataSet[1];
      this->GlueLatency = DataSet[2];
    } else {
      this->CycleUnitStatic = 0;
      this->LogicLatency = 0;
      this->GlueLatency = 0;
    }

    this->ComputeStaticCyclesForFunc(codegen, SchedulePtr, AllBlockList);
    if (!(this->DataUpdated)) {
      if (!(this->RequireToCheckInstanceAndLoopID).empty()) {
        this->CheckIfUnrollSuccess(codegen, AllBlockList);
      }
    }
    AllFuncDeclList->push_back(this);
    //  If no valid subblocks, set cycle to 1
    if (this->CycleUnitStatic == 0) {
      this->CycleUnitStatic = 1;
    }
    PRESTP("EST", " Finish compute cycles for function declaration: "
                      << codegen->_p(AstPtr)
                      << ", Cycles: " << this->CycleUnitStatic
                      << ", LogicLatency: " << this->LogicLatency
                      << ", GlueLatency: " << this->GlueLatency);
  } else if (Type == AUTOKERNEL) {
    PRESTP("EST",
           " Start compute cycles for auto kernel: " << codegen->_p(AstPtr));
    if (have_valid_child) {
      vector<int64_t> DataSet = ComputeStaticCycleFromSchedule(
          codegen, SchedulePtr, SchedulePtrSpawn, AstPtr, AllFuncDeclList,
          ExtMemParamData, PipeParent, AllBlockList);
      this->CycleUnitStatic = DataSet[0];
      this->LogicLatency = DataSet[1];
      this->GlueLatency = DataSet[2];
    }
    AllFuncDeclList->push_back(this);
    PRESTP("EST", " Finish compute cycles for auto kernel: "
                      << codegen->_p(AstPtr)
                      << ", Cycles: " << this->CycleUnitStatic);
  } else if (Type == MEMCPY) {
    int64_t Cycles = this->GetMemcpyCycles(codegen);
    this->CycleUnitStatic = Cycles;
    this->GlueLatency = 0;
    this->LogicLatency = Cycles;
    PRESTP("EST", " Finish compute cycles for memcpy: "
                      << codegen->_p(AstPtr)
                      << ", Cycles: " << this->CycleUnitStatic
                      << ", LogicLatency: " << this->LogicLatency
                      << ", GlueLatency: " << this->GlueLatency);
  } else if (Type == LOOP) {
    PRESTP("EST", " Start compute cycles for loop: " << codegen->_p(AstPtr));

    // if have child, calculate children acc-cycles as loop iteration cycle
    if (have_valid_child) {
      vector<int64_t> DataSet = ComputeStaticCycleFromSchedule(
          codegen, SchedulePtr, SchedulePtrSpawn, AstPtr, AllFuncDeclList,
          ExtMemParamData, PipeParent, AllBlockList);
      this->IL = DataSet[0];
      //  first time execution will calculate glue latency and logic latency
      if (!(this->DataUpdated)) {
        this->LogicLatencyIteration = DataSet[1];
        this->GlueLatencyIteration = DataSet[2];
      }
      this->AccCycles = this->IL;
    }

    this->ComputeStaticCyclesForLoop(codegen, AllBlockList);
    PRESTP("EST",
           " Prefinish compute cycles for loop: "
               << codegen->_p(AstPtr) << ", Cycles: " << this->CycleUnitStatic
               << ", LogicLatency: " << this->LogicLatency
               << ", GlueLatency: " << this->GlueLatency
               << ", LogicLatencyIteration: " << this->LogicLatencyIteration
               << ", GlueLatencyIteration: " << this->GlueLatencyIteration);
    this->ComputeGlueCyclesForLoop(codegen, SchedulePtr, AllBlockList);
    if (!(this->DataUpdated)) {
      if (!(this->RequireToCheckInstanceAndLoopID).empty()) {
        this->CheckIfUnrollSuccess(codegen, AllBlockList);
      }
    }

    if (this->IfEmptyLoop(codegen)) {
      this->CycleUnitStatic = 0;
      this->GlueLatencyIteration = 0;
      this->LogicLatencyIteration = 0;
      this->LogicLatency = 0;
      this->GlueLatency = 0;
    }
    PRESTP("EST", " Finish compute cycles for loop: "
                      << codegen->_p(AstPtr)
                      << ", Cycles: " << this->CycleUnitStatic
                      << ", LogicLatency: " << LogicLatency
                      << ", GlueLatency: " << GlueLatency
                      << ", LogicLatencyIteration: " << LogicLatencyIteration
                      << ", GlueLatencyIteration: " << GlueLatencyIteration);
  } else if (Type == IF) {
    //  If statement will calculate if branch and else branch separately
    int64_t Cycles = 0;
    int64_t CyclesElse = 0;
    int64_t LogicCycles = 0;
    int64_t LogicCyclesElse = 0;
    int64_t GlueCycles = 0;
    int64_t GlueCyclesElse = 0;
    if (have_valid_child) {
      PRESTP("EST", " Start compute cycles for if: " << codegen->_p(AstPtr));
      vector<int64_t> DataSet = ComputeStaticCycleFromSchedule(
          codegen, SchedulePtr, SchedulePtrSpawn, AstPtr, AllFuncDeclList,
          ExtMemParamData, PipeParent, AllBlockList);
      Cycles = DataSet[0];
      LogicCycles = DataSet[1];
      GlueCycles = DataSet[2];
      PRESTP("EST", " Finish compute cycles for branch if: "
                        << codegen->_p(AstPtr) << ", Cycles=" << Cycles
                        << ", LogicCycles=" << LogicCycles
                        << ", GlueCycles=" << GlueCycles);
    }
    if (have_valid_child_else) {
      PRESTP("EST", " Start compute cycles for else: " << codegen->_p(AstPtr));
      vector<int64_t> DataSet = ComputeStaticCycleFromSchedule(
          codegen, SchedulePtrElse, SchedulePtrSpawn, AstPtr, AllFuncDeclList,
          ExtMemParamData, PipeParent, AllBlockList);
      CyclesElse = DataSet[0];
      LogicCyclesElse = DataSet[1];
      GlueCyclesElse = DataSet[2];
      PRESTP("EST", " Finish compute cycles for branch else: "
                        << codegen->_p(AstPtr) << ", Cycles=" << CyclesElse
                        << ", LogicCycles=" << LogicCyclesElse
                        << ", GlueCycles=" << GlueCyclesElse);
    }
    //  if (Cycles > CyclesElse) {
    //  this->CycleUnitStatic = Cycles;
    //  } else {
    //  this->CycleUnitStatic = CyclesElse;
    //  }
    this->CycleUnitStatic = max(Cycles, CyclesElse);
    this->LogicLatency = max(LogicCycles, LogicCyclesElse);
    this->GlueLatency = max(GlueCycles, GlueCyclesElse);
    PRESTP("EST", " Finish compute cycles for statement if: "
                      << codegen->_p(AstPtr)
                      << ", Cycles=" << this->CycleUnitStatic
                      << ", LogicCycles=" << LogicCycles
                      << ", GlueCycles=" << GlueCycles);
  } else if (Type == FNCALL) {
    PRESTP("EST",
           " Start Compute cycles for function call: " << codegen->_p(AstPtr));
    //  Checks if the latency of the function declariton of the func call has
    //  been computed or not
    bool already_processed = false;
    for (size_t k = 0; k < (*AllFuncDeclList).size(); k++) {
      if ((*AllFuncDeclList)[k] == this->FuncDecl) {
        already_processed = true;
        break;
      }
    }
    if (!already_processed) {
      //  if not computed yet, recursively compute the cycle value down its
      //  child
      this->FuncDecl->ComputeStaticCycleHelper(
          codegen, AllFuncDeclList, ExtMemParamData, nullptr, AllBlockList);
      //      AllFuncDeclList.push_back(FuncDecl);
    }
    //  Copy from from the function declartion of the function call
    this->CycleUnitStatic = this->FuncDecl->CycleUnitStatic;
    //  If func decl is inline function,
    //  then move gule latency and logic latency to function call
    //  If func decl is not inline function,
    //  then glue latency = 0, logic latency = unit cycles
    if (this->FuncDecl->IsInlineFunc) {
      //      this->GlueLatency = FuncDecl->GlueLatency;
      //      this->LogicLatency = FuncDecl->LogicLatency;
      this->GlueLatency = 0;
      this->LogicLatency = this->CycleUnitStatic;
    } else {
      this->GlueLatency = 0;
      this->LogicLatency = this->CycleUnitStatic;
    }

    //  is dead function declaration, then set to 0
    if (this->FuncDecl->IsDead) {
      PRESTP("EST", "Set all to 0 for dead function.");
      this->CycleUnitStatic = 0;
      this->LogicLatency = 0;
      this->GlueLatency = 0;
    }

    PRESTP("EST", " Finish compute cycles for function call: "
                      << codegen->_p(AstPtr)
                      << ", Cycles: " << this->CycleUnitStatic
                      << ", LogicLatency: " << this->LogicLatency
                      << ", GlueLatency: " << this->GlueLatency);
  } else if (Type == BASICBLOCK) {
    PRESTP("EST",
           " Start compute cycles for basic block: " << codegen->_p(AstPtr));
    if (have_valid_child) {
      vector<int64_t> DataSet = ComputeStaticCycleFromSchedule(
          codegen, SchedulePtr, SchedulePtrSpawn, AstPtr, AllFuncDeclList,
          ExtMemParamData, PipeParent, AllBlockList);
      this->CycleUnitStatic = DataSet[0];
      this->LogicLatency = DataSet[1];
      this->GlueLatency = DataSet[2];
    }
    PRESTP("EST", " Finish compute cycles for basic block: "
                      << codegen->_p(AstPtr)
                      << ", Cycles: " << this->CycleUnitStatic);
  }

  if (HaveMemoryBurstChild) {
    if (Parent != nullptr) {
      //  parent node also need to set this flag to 1
      Parent->HaveMemoryBurstChild = true;
    }
  }

  //  if have more than one instance, mark paranet as required to check
  if (!(this->DataUpdated)) {
    if (this->InstanceNumber > 1) {
      cout << "Instance Number = " << this->InstanceNumber << endl;
      MarkParentAsRequireToCheck(codegen, this->InstanceNumber, AllBlockList);
    }
  }
}

//  ************************************************************
//  compute cycle statict for sub blocks
//  ************************************************************
vector<int64_t> CPerfEstBlock::ComputeStaticCycleFromSchedule(
    CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *Schedule,
    vector<CPerfEstBlock *> *ScheduleSpawn, void *AstPtr,
    vector<CPerfEstBlock *> *AllFuncDeclList, ExtMemParam *ExtMemParamData,
    CPerfEstBlock *PipeParent, const vector<CPerfEstBlock *> &AllBlockList) {
  vector<int64_t> DataSet;
  //  If Loop, read II and write IL to each loop
  int64_t TotalCycles = 0;
  int64_t GlueLatency = 0;
  int64_t LogicLatency = 0;
  string id = getTopoLocation(codegen, static_cast<SgNode *>(AstPtr));
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  SchedulePtr = Schedule;
  PRESTP("EST", id << " - schedule sequntial size: " << (*SchedulePtr).size());
  for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
    PRESTP("EST",
           id << " - schedule parallel size: " << (*SchedulePtr)[j].size());
    int64_t CyclesTemp = 0;
    int64_t GlueTemp = 0;
    int64_t LogicTemp = 0;
    for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
      CPerfEstBlock *PipeParent_temp =
          (Type == LOOP && II > 0) ? this : PipeParent;
      CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
      PRESTP("EST", " j=" << j << " ,k=" << k
                          << " ,Block=" << codegen->_p(ChildBlock->AstPtr));
      //  recursively compute the cycle value down its child
      ChildBlock->ComputeStaticCycleHelper(codegen, AllFuncDeclList,
                                           ExtMemParamData, PipeParent_temp,
                                           AllBlockList);
      // if one of che child cycles is -1, then the parent cycles is -1
      if (ChildBlock->CycleUnitStatic == -1 || CyclesTemp == -1) {
        CyclesTemp = -1;
        GlueTemp = 0;
        LogicTemp = 0;
        continue;
      }
      //  for parallel module, get the max one
      if (ChildBlock->CycleUnitStatic > CyclesTemp) {
        CyclesTemp = ChildBlock->CycleUnitStatic;
      }
      //  for parallel module
      //  if glue latency > max sub loop or function latency, glue latency =
      //  cycle ybut
      if (ChildBlock->GlueLatency > GlueTemp) {
        GlueTemp = ChildBlock->GlueLatency;
      }
      if (ChildBlock->LogicLatency > LogicTemp) {
        LogicTemp = ChildBlock->LogicLatency;
      }
    }
    //  for serial module, get the sum
    if (CyclesTemp == -1 || TotalCycles == -1) {
      TotalCycles = -1;
      GlueLatency = 0;
      LogicLatency = 0;
    } else {
      TotalCycles += CyclesTemp;
      GlueLatency += GlueTemp;
      LogicLatency += LogicTemp;
    }
    PRESTP("EST", " ComputeTemp: " << CyclesTemp << ", LogicTemp: " << LogicTemp
                                   << ", GlueTemp: " << GlueTemp
                                   << ", TotalCycles: " << TotalCycles
                                   << ", LogicLatency: " << LogicLatency
                                   << ", GlueLatency: " << GlueLatency);
  }

  // all spawn kernel execute in parallel with other modules
  vector<CPerfEstBlock *> *SchedulePtrSpawn;
  SchedulePtrSpawn = ScheduleSpawn;
  PRESTP("EST", id << " - schedule spawn size: " << (*SchedulePtrSpawn).size());
  for (size_t j = 0; j < (*SchedulePtrSpawn).size(); j++) {
    CPerfEstBlock *PipeParent_temp = PipeParent;
    CPerfEstBlock *ChildBlock = (*SchedulePtrSpawn)[j];
    PRESTP("EST", " j=" << j << " ,Block=" << codegen->_p(ChildBlock->AstPtr));
    // recursively compute the cycle value down its child
    ChildBlock->ComputeStaticCycleHelper(codegen, AllFuncDeclList,
                                         ExtMemParamData, PipeParent_temp,
                                         AllBlockList);
    if (ChildBlock->CycleUnitStatic > TotalCycles) {
      TotalCycles = ChildBlock->CycleUnitStatic;
    }
    if (ChildBlock->GlueLatency > GlueLatency) {
      GlueLatency = ChildBlock->GlueLatency;
    }
    if (ChildBlock->LogicLatency > LogicLatency) {
      LogicLatency = ChildBlock->LogicLatency;
    }
    PRESTP("EST", "TotalCycles: " << TotalCycles
                                  << ", LogicLatency: " << LogicLatency
                                  << ", GlueLatency: " << GlueLatency);
  }
  DataSet.push_back(TotalCycles);
  DataSet.push_back(LogicLatency);
  DataSet.push_back(GlueLatency);
  return DataSet;
}

//  ************************************************************
//  check if the func name is memcpy function name
//  ************************************************************
bool IsFuncNameMemcpy(string func_name) {
  return func_name == "memcpy" ||
         func_name.substr(0, 16) == "memcpy_wide_bus_" ||
         func_name.substr(0, 14) == "merlin_memcpy_";
}

//  ************************************************************
//  check if node is from function call
//  return 0 : not from function call
//  return 1 : is from memcpy
//  return 2 : is from normal function call
//  ************************************************************
int CheckIfFromFuncCall(CSageCodeGen *codegen, void *Node) {
  //  cout << "Checking if ref from memcpy : " << codegen->UnparseToString(Node)
  //  << endl;
  void *Parent = codegen->GetParent(Node);
  while (true) {
    //  PRESTP("EST", "Parent : " << codegen->_p(Parent));
    if (Parent != nullptr) {
      if (codegen->IsFunctionCall(Parent) != 0) {
        if (codegen->GetFuncDeclByCall(Parent) != nullptr) {
          string func_name = codegen->GetFuncNameByCall(Parent);
          //          cout << "Come from : " << func_name << endl;
          if (IsFuncNameMemcpy(func_name)) {
            return 1;
          }
          return 2;
        }
        return 0;
      }
      if (IsLoopNode(codegen, Parent)) {
        return 0;
      }
      if (codegen->IsFunctionDeclaration(Parent) != 0) {
        return 0;
      }
      Parent = codegen->GetParent(Parent);

    } else {
      return 0;
    }
  }
  return 0;
}

//  ************************************************************
//  Update memory burst data size.
//  ************************************************************
void CPerfEstBlock::UpdateBurstCyclesBySelf(
    CSageCodeGen *codegen, vector<CPerfEstBlock *> *AllFuncDeclList) {
  PRESTP("EST", " Start compute for block: " << codegen->_p(this->AstPtr));
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  SchedulePtr = &Schedule;

  int64_t MaxDataSize = 0;
  int64_t SubBlocksDataSize = 0;
  //  int64_t SubBlocksBitwidth = DEVICE_BITWIDTH_INIT;
  int64_t SubBlocksBitwidth = 0;
  if ((*SchedulePtr).empty()) {
    SubBlocksDataSize = 0;
    SubBlocksBitwidth = 0;
  } else {
    PRESTP("EST", " - schedule sequntial size: " << (*SchedulePtr).size());
    bool HasUpdated = false;
    for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
      PRESTP("EST", " - schedule parallel size: " << (*SchedulePtr)[j].size());
      int64_t ParallelBitWidth = 0;
      int64_t ParallelDataSize = 0;
      for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
        CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
        ChildBlock->UpdateBurstCyclesBySelf(codegen, AllFuncDeclList);
        SubBlocksDataSize += ChildBlock->BurstDataSize;
        ParallelDataSize += ChildBlock->BurstDataSize;
        ParallelBitWidth += ChildBlock->BurstBitWidth;
        cout << "Sub block bitwidth = " << ChildBlock->BurstBitWidth << endl;
        if (ChildBlock->BurstBitWidth > 0) {
          HasUpdated = true;
        }
      }
      if (ParallelBitWidth > DEVICE_BITWIDTH_MAX) {
        ParallelBitWidth = DEVICE_BITWIDTH_MAX;
      }
      if (ParallelBitWidth > 0) {
        //  get min bitwidth if calculated as parallel unit
        //  if (ParallelBitWidth < SubBlocksBitwidth) {
        //  SubBlocksBitwidth = ParallelBitWidth;
        //  }
        //  take max data size bitwidth as serial total bitwidth
        if (MaxDataSize < ParallelDataSize) {
          MaxDataSize = ParallelDataSize;
          SubBlocksBitwidth = ParallelBitWidth;
        }
      }
      cout << "parallel bitwidth = " << ParallelBitWidth << endl;
    }
    if (!HasUpdated) {
      SubBlocksBitwidth = 0;
    }
  }

  if (this->Type == LOOP) {
    SubBlocksDataSize *= this->TCStatic;
    SubBlocksBitwidth *= this->UnrollFactor;
    //    MaxDataSize *= this->TCStatic;
  }
  PRESTP("EST", " SubBlocksBitwidth = "
                    << SubBlocksBitwidth
                    << ", SubBlocksDataSize = " << SubBlocksDataSize
                    << ", for block: " << codegen->_p(AstPtr));

  int64_t CyclesSerialTotal = 0;
  int64_t VarRefTotalData = 0;
  //  int64_t VarRefBitWidth = DEVICE_BITWIDTH_INIT;
  int64_t VarRefBitWidth = 0;
  if (this->Type == MEMCPY) {
    PRESTP("EST", " Start compute for memcpy: " << codegen->_p(AstPtr));
    VarRefTotalData = this->MerlinMemcpyDataSize;
    VarRefBitWidth = this->MerlinMemcpyBitwidth;
    //  FIXME this is an temp fix, if memory burst cycles < 1000,
    //  will give a penalty  coefficient
    if (this->CycleUnitStatic <= 1024) {
      cout << "Update burst cycles by coefficien." << endl;
      this->CycleUnitStatic *= COEFFICIENT_FOR_SMALL_BURST;
    }
    cout << "cycles = " << this->CycleUnitStatic << endl;
    PRESTP("EST", " Finish compute for memcpy: " << codegen->_p(AstPtr));
  } else if (this->GlobalMemoryAttributes.empty() && Type != FNCALL) {
    VarRefBitWidth = 0;
    VarRefTotalData = 0;
  } else {
    bool HasUpdated = false;
    if (this->Type == TOPKERNEL) {
      PRESTP("EST", " Start compute for top kernel: " << codegen->_p(AstPtr));
      PRESTP("EST", " Finish compute for top kernel: " << codegen->_p(AstPtr));
    } else if (this->Type == SUBKERNEL || this->Type == FNDECL) {
      PRESTP("EST", " Start compute for func decl: " << codegen->_p(AstPtr));
      PRESTP("EST", " Finish compute for funcn decl: " << codegen->_p(AstPtr));
    } else if (this->Type == FNCALL) {
      //  Checks if the latency of the function declariton of the func call has
      //  been computed or not
      bool already_processed = false;
      for (size_t k = 0; k < (*AllFuncDeclList).size(); k++) {
        if ((*AllFuncDeclList)[k] == FuncDecl) {
          already_processed = true;
          break;
        }
      }
      if (!already_processed) {
        //  if not computed yet, recursively compute the cycle value down its
        //  child
        FuncDecl->UpdateBurstCyclesBySelf(codegen, AllFuncDeclList);
        this->BurstDataSize = FuncDecl->BurstDataSize;
        this->BurstBitWidth = FuncDecl->BurstBitWidth;
        AllFuncDeclList->push_back(FuncDecl);
      }
      PRESTP("EST", " Finish compute for func call: " << codegen->_p(AstPtr));
    } else if (this->Type == LOOP) {
      PRESTP("EST", " Start compute for loop: " << codegen->_p(AstPtr));
      map<void *, map<string, int64_t>>::iterator iter1;
      map<string, int64_t>::iterator iter2;
      for (iter1 = (this->GlobalMemoryAttributes).begin();
           iter1 != (this->GlobalMemoryAttributes).end(); iter1++) {
        void *OneRef = iter1->first;
        cout << "one ref = " << codegen->UnparseToString(OneRef);
        int64_t OneDataSize = 0;
        int64_t OneBitWidth = 0;
        int64_t OrgBitWidth = 0;
        map<string, int64_t> KeyValue = iter1->second;
        for (iter2 = KeyValue.begin(); iter2 != KeyValue.end(); iter2++) {
          string Key = iter2->first;
          int64_t Value = iter2->second;
          if (Key == "BITWIDTH") {
            OrgBitWidth = Value;
            OneBitWidth = Value;
            if (OneBitWidth > 0) {
              HasUpdated = true;
            }
            //  get parallel width
            if (this->UnrollFactor > 0) {
              OneBitWidth *= this->UnrollFactor;
            }
            if (OneBitWidth > DEVICE_BITWIDTH_MAX) {
              OneBitWidth = DEVICE_BITWIDTH_MAX;
            }
            if (OneBitWidth > 0) {
              //  if (OneBitWidth < VarRefBitWidth) {
              //  VarRefBitWidth = OneBitWidth;
              //  }
              VarRefBitWidth += OneBitWidth;
            }
            cout << " bitwidth = " << OneBitWidth;
          }
          if (Key == "DATASIZE") {
            OneDataSize = Value;
            cout << " OneDataSize 1 = " << OneDataSize;
            //  If did not calculate size before, we directly set size as trip
            //  count
            if (OneDataSize == 0) {
              OneDataSize = this->TCStatic * OrgBitWidth;
            }
            //  FIXME if OneDataSize larger than tripcount, so non-continuous
            //  access just direct use trip count
            if (OneDataSize > this->TCStatic * OrgBitWidth) {
              OneDataSize = this->TCStatic * OrgBitWidth;
            }
            //
            //  FIXME the data in GlobalMemoryAttributes is the total burst size
            //  not accurate.
            //  FIXME Currently directly use datasize in direct parent loop
            //  the index of interface may not related to direct parent loop
            //
            //  OneDataSize = this->TCStatic * OrgBitWidth;
            VarRefTotalData += OneDataSize;
            //  cout << " data size = " << OneDataSize;
          }
        }
        if (OneBitWidth == 0)
          OneBitWidth = 1;
        int64_t OneCycle = OneDataSize / OneBitWidth;
        cout << " Cycles = " << OneCycle << endl;
        CyclesSerialTotal += OneCycle;
      }

      PRESTP("EST", " Finish compute for loop: " << codegen->_p(AstPtr));
    } else {
      PRESTP("EST", " Start compute for block: " << codegen->_p(AstPtr));
      PRESTP("EST", " Finish compute for block: " << codegen->_p(AstPtr));
    }
    if (!HasUpdated) {
      VarRefBitWidth = 0;
    }
  }
  if (VarRefBitWidth > DEVICE_BITWIDTH_MAX) {
    VarRefBitWidth = DEVICE_BITWIDTH_MAX;
  }
  this->BurstVarRefDataSize = VarRefTotalData;
  this->BurstVarRefBitWidth = VarRefBitWidth;
  PRESTP("EST",
         " VarRefTotalData = " << VarRefTotalData
                               << ", VarRefBitWidth = " << VarRefBitWidth
                               << ", for block: " << codegen->_p(AstPtr));

  if (this->Type != FNCALL) {
    this->BurstDataSize = VarRefTotalData + SubBlocksDataSize;
    if (VarRefBitWidth == 0 && SubBlocksBitwidth == 0) {
      this->BurstBitWidth = 0;
    } else {
      //  if (VarRefBitWidth <= SubBlocksBitwidth) { //  serial execution
      //  this->BurstBitWidth = VarRefBitWidth;
      //  } else {
      //  this->BurstBitWidth = SubBlocksBitwidth;
      //  }
      //  take max data size bitwidth as serial total bitwidth
      //  if(MaxDataSize < VarRefTotalData) {
      //    MaxDataSize = VarRefTotalData;
      //    this->BurstBitWidth = VarRefBitWidth;
      //  } else {
      //    this->BurstBitWidth = SubBlocksBitwidth;
      //  }
      //  Currently. we think var ref covered by blocks and execute in parallel
      this->BurstBitWidth = VarRefBitWidth + SubBlocksBitwidth;
    }
  }

  PRESTP("EST",
         " Get Burst Data Size = " << this->BurstDataSize
                                   << ", for block: " << codegen->_p(AstPtr));
  PRESTP("EST",
         " Get Burst Bit Width = " << this->BurstBitWidth
                                   << ", for block: " << codegen->_p(AstPtr));
}

//  ************************************************************
//  Update memory burst cycles factor.
//  ************************************************************
void CPerfEstBlock::UpdateBurstCyclesFactor(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllFuncDeclList) {
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  SchedulePtr = &Schedule;

  PRESTP("EST", " Start update factor for type : " << this->Type << ", block: "
                                                   << codegen->_p(AstPtr));
  PRESTP("EST", " - schedule sequntial size: " << (*SchedulePtr).size());
  for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
    for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
      int64_t ChildDataSize = 0;
      CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
      PRESTP("EST", " - schedule parallel size: " << (*SchedulePtr)[j].size());
      if (static_cast<int>(this->ExecFlow) == 0) {
        this->BurstFactor = 1.0;
        // ChildBlock->BurstFactor = 1.0;
      } else {
        PRESTP("EST",
               " 111 Child Data Size = "
                   << ChildBlock->BurstDataSize
                   << ", Parent Data Size = " << this->BurstDataSize
                   << ", for block = " << codegen->_up(ChildBlock->AstPtr));
        if (ChildBlock->Type == FNCALL) {
          CPerfEstBlock *FuncDecl = ChildBlock->FuncDecl;
          ChildDataSize = FuncDecl->BurstDataSize;
        } else {
          ChildDataSize = ChildBlock->BurstDataSize;
        }
        if (this->Type == LOOP) {
          ChildDataSize = ChildBlock->BurstDataSize * this->TCStatic;
          cout << "parent TCStatic = " << this->TCStatic << endl;
        }
        PRESTP("EST", " 222 Child Data Size = "
                          << ChildDataSize << ", Parent Data Size = "
                          << this->BurstDataSize << ", for block = "
                          << codegen->_up(ChildBlock->AstPtr));
        if (ChildDataSize == 0) {
          ChildBlock->BurstFactor = 1.0;
        } else {
          ChildBlock->BurstFactor =
              (this->BurstDataSize * 1.0) / (ChildDataSize * 1.0);
        }
        // update factor by bitwidth
        // float BitWidthFactor = this->BurstWidth / ChildBlock->BurstWidth;

        PRESTP("EST", " Factor = " << ChildBlock->BurstFactor
                                   << ", for block = "
                                   << codegen->_up(ChildBlock->AstPtr));
      }
      if (ChildBlock->Type == FNCALL) {
        CPerfEstBlock *FuncDecl = ChildBlock->FuncDecl;
        FuncDecl->UpdateBurstCyclesFactor(codegen, AllFuncDeclList);
      } else {
        ChildBlock->UpdateBurstCyclesFactor(codegen, AllFuncDeclList);
      }
    }
    if (this->BurstVarRefDataSize > 0) {
      this->BurstVarRefFactor =
          (this->BurstDataSize * 1.0) / (this->BurstVarRefDataSize * 1.0);
    }
  }
  PRESTP("EST", " BurstFactor = " << this->BurstFactor
                                  << ", for block: " << codegen->_p(AstPtr));
}

//  ************************************************************
//  Update memory burst cycles.
//  ************************************************************
void CPerfEstBlock::UpdateBurstCycles(
    CSageCodeGen *codegen, const vector<CPerfEstBlock *> &AllFuncDeclList,
    double ComboBurstFactor) {
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  SchedulePtr = &Schedule;
  int64_t CycleUnitStaticOrg = this->CycleUnitStatic;
  //  Calculate sub blocks cycles
  PRESTP("EST", " - schedule sequntial size: " << (*SchedulePtr).size());
  int64_t ChildUnitCyclesSum = 0;
  int64_t ChildBurstCyclesSum = 0;
  for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
    PRESTP("EST", " - schedule parallel size: " << (*SchedulePtr)[j].size());
    int64_t ChildUnitCyclesMax = 0;
    int64_t ChildBurstCyclesMax = 0;
    for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
      CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
      int64_t ChildUnitCyclesTmp = 0;
      int64_t ChildBurstCyclesTmp = 0;
      double ComboBurstFactorTmp = 1.0;
      PRESTP("EST", " Start calculate as subblock for block: "
                        << codegen->_p(ChildBlock->AstPtr));
      PRESTP("EST", " ChildBurstFactor = " << ChildBlock->BurstFactor);
      //  calculate latest unit cycles
      if (ChildBlock->Type == FNCALL) {
        CPerfEstBlock *FuncDecl = ChildBlock->FuncDecl;
        ComboBurstFactorTmp = ComboBurstFactor * FuncDecl->BurstFactor;
        FuncDecl->UpdateBurstCycles(codegen, AllFuncDeclList,
                                    ComboBurstFactorTmp);
        ChildUnitCyclesTmp = FuncDecl->CycleUnitStatic;
      } else {
        ComboBurstFactorTmp = ComboBurstFactor * ChildBlock->BurstFactor;
        ChildBlock->UpdateBurstCycles(codegen, AllFuncDeclList,
                                      ComboBurstFactorTmp);
        ChildUnitCyclesTmp = ChildBlock->CycleUnitStatic;
      }
      //  cout << "child block = " << codegen->_p(ChildBlock->AstPtr) << endl;
      if (ChildBlock->BurstDataSize > 0) {
        double Tmp = static_cast<double>(ChildBlock->BurstDataSize) /
                     ChildBlock->BurstBitWidth * ComboBurstFactorTmp;
        ChildBurstCyclesTmp = static_cast<int64_t>(Tmp);
        // ChildBurstCyclesTmp = ChildBlock->BurstDataSize /
        //                      this->BurstBitWidth * ComboBurstFactorTmp;
        if (ChildBurstCyclesTmp > ChildBurstCyclesMax) {
          ChildBurstCyclesMax = ChildBurstCyclesTmp;
        }
      }
      //  get parallel block max unit cycles
      if (ChildUnitCyclesTmp > ChildUnitCyclesMax) {
        ChildUnitCyclesMax = ChildUnitCyclesTmp;
      }
      PRESTP("EST", " Finish calculate as subblock for block: "
                        << codegen->_p(ChildBlock->AstPtr));
      cout << "ChildBurstCyclesMax = " << ChildBurstCyclesMax
           << ", ChildUnitCyclesMax = " << ChildUnitCyclesMax << endl;
    }
    ChildBurstCyclesSum += ChildBurstCyclesMax;
    ChildUnitCyclesSum += ChildUnitCyclesMax;
    cout << "Finish calculate for one parallel scope:"
         << " Child burst cycles max = " << ChildBurstCyclesMax
         << ", Child unit cycles max = " << ChildUnitCyclesMax << endl;
  }
  cout << "Prefinish block Cycles calculation: ChildBurstCyclesSum="
       << ChildBurstCyclesSum << ", ChildUnitCyclesSum=" << ChildUnitCyclesSum
       << ", for block: " << codegen->UnparseToString(this->AstPtr) << endl;
  if (this->Type == LOOP) {
    ChildBurstCyclesSum *= this->TCStatic;
    if (this->TCStatic == 0 && this->UnrollFactor > 0) {
      if (this->Pipelined) {
        ChildUnitCyclesSum += this->TCStatic - 1;
      } else {
        ChildUnitCyclesSum *= this->TCStatic / this->UnrollFactor;
      }
    }
  }
  cout << "Finish block Cycles calculation: ChildBurstCyclesSum="
       << ChildBurstCyclesSum << ", ChildUnitCyclesSum=" << ChildUnitCyclesSum
       << ", for block: " << codegen->UnparseToString(this->AstPtr) << endl;
  // TODO(youxiang): directory burstdatasize/bitwidth = total burst cycles
  // TODO(youxiang): then compare with ChildUnitCyclesSum, burst cycles sum
  // should
  //  calculated from last formulat Calculate variable reference cycles
  int64_t BurstVarRefCycles = 0;
  if (this->BurstDataSize > 0) {
    this->BurstCycles = this->BurstDataSize / this->BurstBitWidth;
  } else {
    this->BurstCycles = 0;
  }
  //  if (this->BurstVarRefBitWidth > 0) {
  //    BurstVarRefCycles = (this->BurstVarRefDataSize * 1.0) /
  //                        (this->BurstVarRefBitWidth * 1.0) *
  //                        ComboBurstFactor;
  //    cout << "Burst data size = " << this->BurstVarRefDataSize
  //        << ", BUrst var ref bit width = " << this->BurstVarRefBitWidth
  //        << endl;
  //  }
  cout << "Child unit cycles sum = " << ChildUnitCyclesSum
       << ", Child burst cycles sum = " << ChildBurstCyclesSum
       << ", Burst var ref cycles = " << BurstVarRefCycles << endl;
  //  FIXME TMP FIX, when Xilinx, top kernel only have on kernel,
  //  no need to calculate, set burst = 0
  //  for intel, this may have several kernels, still need to calculate
  if (this->Tool == "sdaccel" && this->Type == TOPKERNEL) {
    ChildBurstCyclesSum = 0;
    BurstVarRefCycles = 0;
  }
  //  no matter serial or parallel,
  //  assume var ref burst covered by compute
  //  if (BurstVarRefCycles > 0) {
  //    if (ChildBurstCyclesSum > BurstVarRefCycles) {
  //      this->BurstCycles = ChildBurstCyclesSum;
  //    } else {
  //      this->BurstCycles = BurstVarRefCycles;
  //    }
  //    if (ChildUnitCyclesSum > BurstVarRefCycles) {
  //      this->CycleUnitStatic = ChildUnitCyclesSum;
  //    } else {
  //      this->CycleUnitStatic = BurstVarRefCycles;
  //    }
  //  } else {
  //    this->BurstCycles = ChildBurstCyclesSum;
  //    this->CycleUnitStatic = ChildUnitCyclesSum;
  //  }
  //  this->BurstCycles = ChildBurstCyclesSum;
  //  this->CycleUnitStatic = ChildUnitCyclesSum;
  //  if memory burst block. update cycles to burst cycles
  if (this->CycleUnitStatic < this->BurstCycles) {
    this->CycleUnitStatic = this->BurstCycles;
  }
  //  if empcy sub blocks and varref, will directly assign to CycleUnitStaticOrg
  if (this->CycleUnitStatic < CycleUnitStaticOrg) {
    this->CycleUnitStatic = CycleUnitStaticOrg;
  }
  this->ComputeCycles = CycleUnitStaticOrg;
  PRESTP("EST", " CycleUnitStatic = "
                    << this->CycleUnitStatic
                    << ", CycleUnitStaticOrg = " << CycleUnitStaticOrg
                    << ", BurstCycles = " << this->BurstCycles
                    << ", SubBlocksCycles = " << ChildBurstCyclesSum
                    << ", VarRefCycles = " << BurstVarRefCycles
                    << ", ComboBurstFactor = " << ComboBurstFactor
                    << ", for block: " << codegen->_p(this->AstPtr));
}

//  ************************************************************
//  get channel depth for one channel call
//  ************************************************************
int64_t GetChannelDepth(CSageCodeGen *codegen, void *func_decl,
                        void *channel_call) {
  int64_t size = 1;
  void *parent = channel_call;
  while (parent != func_decl) {
    parent = codegen->GetParent(parent);
    if (codegen->IsForStatement(parent) != 0) {
      int64_t TC = 0;
      int64_t TC_ub = 0;
      codegen->get_loop_trip_count(parent, &TC, &TC_ub);
      //            printf("TC=%lld, TC_ub=%lld\n", TC, TC_ub);
      size *= TC;
    }
  }
  return size;
}

//  ************************************************************
//  get all channels for auto run kernel
//  return a map for channel call and it's depth
//  ************************************************************
map<void *, int64_t> GetChannelMapForAutoRunKernel(CSageCodeGen *codegen,
                                                   void *func_decl) {
  map<void *, int64_t> channel_size;
  vector<void *> func_calls;
  codegen->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                          &func_calls);
  for (auto call : func_calls) {
    string call_name = codegen->GetFuncNameByCall(call);
    if (call_name.substr(0, 19) == "read_channel_hidden" ||
        call_name.substr(0, 20) == "write_channel_hidden") {
      PRESTP("EST", " Find a channel access: " << codegen->_p(call, 30));
      //  get channel depth
      int64_t channel_depth = GetChannelDepth(codegen, func_decl, call);
      //  only need the first argument
      void *exp = codegen->GetFuncCallParam(call, 0);
      vector<void *> vecRef;
      codegen->GetNodesByType(exp, "preorder", "SgVarRefExp", &vecRef);
      for (size_t j = 0; j < vecRef.size(); j++) {
        SgVarRefExp *ref = isSgVarRefExp(static_cast<SgNode *>(vecRef[j]));
        //  only need the first reference expreassion
        if ((ref != nullptr) && j == 0) {
          void *call_arg = codegen->GetVariableInitializedName(ref);
          PRESTP("EST",
                 " Channel argument is : " << codegen->_p(call_arg, 30)
                                           << ", size = " << channel_depth);
          channel_size.insert(pair<void *, int64_t>(call_arg, channel_depth));
        }
      }
    }
  }
  return channel_size;
}

//  ************************************************************
//  get all channels for auto run kernel wrapper
//  return a map for channel call and it's depth
//  ************************************************************
map<void *, int64_t> GetChannelMapForAutoRunKernelWrapper(CSageCodeGen *codegen,
                                                          void *func_decl,
                                                          void *scope) {
  PRESTP("EST", " Read channel depth for wrapper.");
  map<void *, int64_t> channel_size;
  if (codegen->IsFunctionDeclaration(scope) != 0) {
    scope = codegen->GetFuncBody(scope);
  }
  if (codegen->IsForStatement(scope) != 0) {
    scope = codegen->GetLoopBody(scope);
  }
  vector<void *> func_calls;
  codegen->GetNodesByType(scope, "preorder", "SgFunctionCallExp", &func_calls);
  codegen->GetNodesByType(scope, "preorder", "SgFunctionCallExp", &func_calls);
  for (auto one_call : func_calls) {
    PRESTP("EST", " one call = " << codegen->_p(one_call, 130));
    string one_call_name = codegen->GetFuncNameByCall(one_call);
    if (one_call_name.substr(0, 9) == "msm_port_") {
      void *port_decl = codegen->GetFuncDeclByCall(one_call);
      vector<void *> channel_calls;
      codegen->GetNodesByType(port_decl, "preorder", "SgFunctionCallExp",
                              &channel_calls);
      for (auto call : channel_calls) {
        PRESTP("EST", " call = " << codegen->_p(call, 130));
        string call_name = codegen->GetFuncNameByCall(call);
        if (call_name.substr(0, 19) == "read_channel_hidden" ||
            call_name.substr(0, 20) == "write_channel_hidden") {
          PRESTP("EST", " Find a channel access: " << codegen->_p(call, 30));
          //  get channel depth
          int64_t channel_depth = GetChannelDepth(codegen, port_decl, call);
          //  only need the first argument
          void *exp = codegen->GetFuncCallParam(call, 0);
          vector<void *> vecRef;
          codegen->GetNodesByType(exp, "preorder", "SgVarRefExp", &vecRef);
          for (size_t j = 0; j < vecRef.size(); j++) {
            SgVarRefExp *ref = isSgVarRefExp(static_cast<SgNode *>(vecRef[j]));
            //  only need the first reference expreassion
            if ((ref != nullptr) && j == 0) {
              void *call_arg = codegen->GetVariableInitializedName(ref);
              PRESTP("EST",
                     " Channel argument is : " << codegen->_p(call_arg, 30)
                                               << ", size = " << channel_depth);
              channel_size.insert(
                  pair<void *, int64_t>(call_arg, channel_depth));
            }
          }
        }
      }
    }
  }
  return channel_size;
}

// ************************************************************
// calculate auto kernel factor for compute unit
// Factor = wrapper channel depth / auto kernel depth / auto kernel compute
// units number
// ************************************************************
int64_t CaculatorFactorForAutoKernel(CSageCodeGen *codegen,
                                     map<void *, int64_t> channel_size,
                                     map<void *, int64_t> channel_size_wrapper,
                                     int64_t AutoKernelDepth) {
  int64_t Factor = 1;
  for (auto one_info : channel_size) {
    void *one_ref = one_info.first;
    int64_t inner_depth = one_info.second;
    if (channel_size_wrapper.count(one_ref) > 0) {
      int64_t outer_depth = channel_size_wrapper[one_ref];
      PRESTP("EST",
             " calculate depth = " << outer_depth << " : " << inner_depth);
      int64_t TempFactor = outer_depth / inner_depth;
      if (TempFactor > Factor) {
        Factor = TempFactor;
      }
    }
  }
  PRESTP("EST", " kernel depth = " << AutoKernelDepth);
  if (Factor % AutoKernelDepth == 0) {
    Factor = Factor / AutoKernelDepth;
  } else {
    Factor = Factor / AutoKernelDepth + 1;
  }
  PRESTP("EST", " get Factor = " << Factor);
  return Factor;
}

//  ************************************************************
//  Compute total cycles for merlin report
//  ************************************************************
void CPerfEstBlock::ComputeTotalStaticCycle(
    CSageCodeGen *codegen, vector<CPerfEstBlock *> *AllFuncDeclList,
    CPerfEstBlock *PipeParent, int64_t Factor,
    vector<CPerfEstBlock *> TopKernelBlock) {
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  vector<vector<CPerfEstBlock *>> *SchedulePtrElse;
  SchedulePtr = &Schedule;
  SchedulePtrElse = &ScheduleElse;
  //  printf("- schedule sequntial size: %zu\n", (*SchedulePtr).size());
  if (Type == TASK) {
    PRESTP("EST", " Start compute cycles for TASK. ");
    for (auto OneTopKernelBlock : TopKernelBlock) {
      OneTopKernelBlock->ComputeTotalStaticCycle(codegen, AllFuncDeclList,
                                                 nullptr, 1);
      PRESTP("EST", " Unit cycle of top kernel:"
                        << OneTopKernelBlock->CycleUnitStatic);
      PRESTP("EST", " THIRD_CYCLES=" << OneTopKernelBlock->CycleUnitStatic);
    }
    return;
  }
  if (Type == FNDECL || Type == SUBKERNEL || Type == TOPKERNEL) {
    PRESTP("EST", " Start compute cycles for func "
                      << codegen->UnparseToString(AstPtr));
    PRESTP("EST", " Input Factor = " << Factor);
    int64_t OrgFactor = Factor;
    string func_name = codegen->GetFuncName(AstPtr);
    //  Need a better way to do this later
    //  current use this way to check if merlin generated function
    if (!(func_name.substr(0, 5) == "mars_" ||
          func_name.substr(0, 7) == "merlin_")) {
      Factor = 1;
    }
    ComputeTotalStaticCycleFromSchedule(codegen, SchedulePtr, AstPtr,
                                        AllFuncDeclList, PipeParent, Factor);
    this->CycleStatic = Factor * CycleUnitStatic;
    this->CycleStaticTmp = OrgFactor * CycleUnitStatic;
    AllFuncDeclList->push_back(this);
    PRESTP("EST",
           " Factor = " << Factor << ", Cycles " << this->CycleUnitStatic);
    PRESTP("EST", " Compute cycles for function declaration: "
                      << codegen->_p(AstPtr) << ", TotalCycles "
                      << this->CycleStatic << ". TmpCycles "
                      << this->CycleStaticTmp);
  } else if (Type == AUTOKERNEL) {
    PRESTP("EST", " Start compute cycles for autokernel "
                      << codegen->UnparseToString(AstPtr));
    PRESTP("EST", " Input Factor = " << Factor);
    //  get the map for channel and size
    map<void *, int64_t> channel_size =
        GetChannelMapForAutoRunKernel(codegen, AstPtr);
    map<void *, int64_t> channel_size_wrapper =
        GetChannelMapForAutoRunKernelWrapper(codegen, AstPtr,
                                             AutoKernelParentScope);
    int64_t new_Factor = CaculatorFactorForAutoKernel(
        codegen, channel_size, channel_size_wrapper, AutoKernelDepth);
    Factor = new_Factor;  //  need change the Factor
    ComputeTotalStaticCycleFromSchedule(codegen, SchedulePtr, AstPtr,
                                        AllFuncDeclList, PipeParent, Factor);
    this->CycleStatic = Factor * CycleUnitStatic;
    AllFuncDeclList->push_back(this);
    PRESTP("EST", " Compute cycles for auto kernel: " << codegen->_p(AstPtr)
                                                      << ", TotalCycles "
                                                      << this->CycleStatic);
  } else if (Type == LOOP) {
    PRESTP("EST", " Start compute cycles for loop "
                      << codegen->UnparseToString(AstPtr));
    //  compute total cycles for loop
    PRESTP("EST", " Input Factor = " << Factor);
    //  normally, total cycles = Factor * unit cycles
    this->CycleStatic = Factor * CycleUnitStatic;
    this->CycleStaticTmp = this->CycleStatic;
    CPerfEstBlock *PipeParentLoop = nullptr;
    if (static_cast<int>(this->Pipelined) == 1) {
      PipeParentLoop = this;
    }
    //  calculate effective TC
    int64_t TmpTCStatic = 0;
    if (TCStatic == 0) {
      TmpTCStatic = 1;
    } else {
      if (TCStatic % this->UnrollFactor == 0) {
        TmpTCStatic = TCStatic / this->UnrollFactor;
      } else {
        TmpTCStatic = TCStatic / this->UnrollFactor + 1;
      }
    }
    //  if pipelined, directly pass factor
    //  if not pipelined, need * trip count
    //  int64_t NewFactor = 1;
    int64_t NewFactor = Factor;
    if (this->IsFlattenToPipeline) {
      //  NewFactor = 1;
      NewFactor = Factor;
    } else {
      if (this->Pipelined) {
        NewFactor = Factor;
      } else {
        NewFactor = TmpTCStatic * Factor;
      }
    }
    PRESTP("EST", " UnrollFactor = " << this->UnrollFactor
                                     << ", TCStatic = " << TCStatic
                                     << ", TmpTCStatic = " << TmpTCStatic
                                     << ", NewFactor = " << NewFactor);

    ComputeTotalStaticCycleFromSchedule(codegen, SchedulePtr, AstPtr,
                                        AllFuncDeclList, PipeParentLoop,
                                        NewFactor);
    PRESTP("EST", " Compute cycles for loop: "
                      << codegen->_p(AstPtr)
                      << ", IsPipeline=" << this->Pipelined << ", TotalCycles "
                      << this->CycleStatic);
  } else if (Type == IF) {
    PRESTP("EST",
           " Start compute cycles for if " << codegen->UnparseToString(AstPtr));
    PRESTP("EST", " Input Factor = " << Factor);
    //  compute total cycles for if else
    ComputeTotalStaticCycleFromSchedule(codegen, SchedulePtr, AstPtr,
                                        AllFuncDeclList, PipeParent, Factor);
    ComputeTotalStaticCycleFromSchedule(codegen, SchedulePtrElse, AstPtr,
                                        AllFuncDeclList, PipeParent, Factor);
    //  if have pipeline parent loop, do not propragate Factor
    //  else propragate Factor to children
    if (PipeParent == nullptr) {
      this->CycleStatic = Factor * CycleUnitStatic;
    } else {
      this->CycleStatic = PipeParent->CycleStatic;
    }
    this->CycleStaticTmp = this->CycleStatic;
    PRESTP("EST", " Compute cycles for if: " << codegen->_p(AstPtr)
                                             << ", TotalCycles "
                                             << this->CycleStatic);
  } else if (Type == FNCALL) {
    PRESTP("EST", " Start compute cycles for call "
                      << codegen->UnparseToString(AstPtr));
    PRESTP("EST", " Input Factor = " << Factor);
    //  Checks if the latency of the function declariton of the func call has
    //  been computed or not
    bool already_processed = false;
    for (size_t k = 0; k < (*AllFuncDeclList).size(); k++) {
      if ((*AllFuncDeclList)[k] == FuncDecl) {
        already_processed = true;
        break;
      }
    }
    if (!already_processed) {
      //  if not computed yet, recursively compute the cycle value down its
      //  child
      FuncDecl->ComputeTotalStaticCycle(codegen, AllFuncDeclList, nullptr,
                                        Factor);
      AllFuncDeclList->push_back(FuncDecl);
    }
    //  if (PipeParent == nullptr) {
    //  //  this->CycleStatic = Factor * FuncDecl->CycleStatic;
    //  this->CycleStatic = Factor * FuncDecl->CycleStaticTmp;
    //  } else {
    //  //  this->CycleStatic = PipeParent->CycleStatic;
    //  this->CycleStatic = PipeParent->CycleStaticTmp;
    //  }
    this->CycleStatic = FuncDecl->CycleStaticTmp;
    PRESTP("EST", " Compute cycles for function call: " << codegen->_p(AstPtr)
                                                        << ", TotalCycles "
                                                        << this->CycleStatic);
  } else if (Type == BASICBLOCK) {
    PRESTP("EST", " Start compute cycles for basic block "
                      << codegen->UnparseToString(AstPtr));
    PRESTP("EST", " Input Factor = " << Factor);
    ComputeTotalStaticCycleFromSchedule(codegen, SchedulePtr, AstPtr,
                                        AllFuncDeclList, PipeParent, Factor);
    if (PipeParent == nullptr) {
      this->CycleStatic = Factor * CycleUnitStatic;
    } else {
      this->CycleStatic = PipeParent->CycleStatic;
    }
    this->CycleStaticTmp = this->CycleStatic;
    PRESTP("EST", " Compute cycles for basic block: " << codegen->_p(AstPtr)
                                                      << ", TotalCycles "
                                                      << this->CycleStatic);
  }
}

//  ************************************************************
//  Compute total cycles for sub blocks for merlin report
//  ************************************************************
void CPerfEstBlock::ComputeTotalStaticCycleFromSchedule(
    CSageCodeGen *codegen, vector<vector<CPerfEstBlock *>> *Schedule,
    void *AstPtr, vector<CPerfEstBlock *> *AllFuncDeclList,
    CPerfEstBlock *PipeParent, int64_t Factor) {
  string id = getTopoLocation(codegen, static_cast<SgNode *>(AstPtr));
  vector<vector<CPerfEstBlock *>> *SchedulePtr;
  SchedulePtr = Schedule;
  PRESTP("EST", id << " - schedule sequntial size: " << (*SchedulePtr).size());
  for (size_t j = 0; j < (*SchedulePtr).size(); j++) {
    PRESTP("EST",
           id << " - schedule parallel size: " << (*SchedulePtr)[j].size());
    for (size_t k = 0; k < (*SchedulePtr)[j].size(); k++) {
      CPerfEstBlock *ChildBlock = (*SchedulePtr)[j][k];
      if (PipeParent != nullptr) {
        PRESTP("EST", " Have Pipeline parent.");
      }
      ChildBlock->ComputeTotalStaticCycle(codegen, AllFuncDeclList, PipeParent,
                                          Factor);
    }
  }

  // all spawn kernel execute in parallel with other modules
  vector<CPerfEstBlock *> *SchedulePtrSpawn;
  SchedulePtrSpawn = &ScheduleSpawn;
  PRESTP("EST", id << " - schedule spawn size: " << (*SchedulePtrSpawn).size());
  for (size_t j = 0; j < (*SchedulePtrSpawn).size(); j++) {
    CPerfEstBlock *ChildBlock = (*SchedulePtrSpawn)[j];
    PRESTP("EST", " j=" << j << " ,Block=" << codegen->_p(ChildBlock->AstPtr));
    ChildBlock->ComputeTotalStaticCycle(codegen, AllFuncDeclList, PipeParent,
                                        Factor);
  }
}
