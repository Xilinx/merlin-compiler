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

#include "cmdline_parser.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#define DEBUG_DER(x)
#define DER_ERROR(x) cerr << "[DER ERROR][" << __func__ << "]" << (x) << endl;
#define DER_ALGO(x) cout << "[DER ALGO][" << __func__ << "]" << (x) << endl;

#if 0
#define DER_INFO(x)                                                            \
  cout << "[DER INFO][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define DER_INFO(x)
#endif

//  FIXME: only for fine-grained loops?
set<void *> getPipelinedLoops(CSageCodeGen *ast, CMarsIr *mars_ir,
                              void *kernel) {
  set<void *> pipelinedLoops;
  vector<CMirNode *> vec_nodes;
  mars_ir->get_topological_order_nodes(&vec_nodes);

  for (auto one_node : vec_nodes) {
    DER_INFO("try node: " << printNodeInfo(ast, ast->GetParent(one_node->ref)));
    if (!one_node->is_function && (one_node->has_pipeline() != 0)) {
      void *sg_loop = ast->GetParent(one_node->ref);
      if (ast->IsForStatement(sg_loop) == 0) {
        continue;
      }

      //  not needed
      //  void * sg_iter = ast->GetLoopIterator(sg_loop);
      //  if (!sg_iter)
      //   continue;

      DER_ALGO("find a pipelined loop: " + printNodeInfo(ast, sg_loop));
      pipelinedLoops.insert(sg_loop);
    }
  }
  return pipelinedLoops;
}

map<void *, vector<void *>> getVariableDefRefPairs(CSageCodeGen *ast,
                                                   void *candidate) {
  vector<void *> varRefs;
  ast->GetNodesByType_int(candidate, "preorder", V_SgVarRefExp, &varRefs);

  map<void *, vector<void *>> variableDefRefPairs;
  for (auto ref : varRefs) {
    void *initName = ast->GetVariableInitializedName(ref);
    if (variableDefRefPairs.find(initName) != variableDefRefPairs.end()) {
      variableDefRefPairs[initName].push_back(ref);
    } else {
      variableDefRefPairs[initName] = {ref};
    }
  }

  return variableDefRefPairs;
}

bool isSupportedType(void *sg_type) {
  DEBUG_DER(if (!sg_type) {
    DER_ERROR("nullptr found.");
    return false;
  })

  SgNode *sg_type_ = static_cast<SgNode *>(sg_type);
  return (isSgTypeFloat(sg_type_) != nullptr) ||
         (isSgTypeDouble(sg_type_) != nullptr) ||
         (isSgTypeLongDouble(sg_type_) != nullptr);
}

bool hasAlias(CSageCodeGen *ast, void *candidate, void *aRef) {
  void *sgInitName = ast->GetVariableInitializedName(aRef);
  // TODO(youxiang): to support global static variables
  if (ast->IsGlobalInitName(sgInitName) != 0) {
    DER_ERROR("we do not support static variables");
    return true;
  }

  // TODO(youxiang): to support case with reference type
  vector<void *> varSet;
  ast->GetNodesByType(nullptr, "preorder", "SgInitializedName", &varSet, true);
  for (auto var : varSet) {
    if (ast->GetFileInfo_line(var) == 0) {
      continue;
    }

    if (isSgReferenceType((static_cast<SgNode *>(ast->GetTypebyVar(var)))) !=
        nullptr) {
      DER_ERROR("we do not support any reference type");
      return true;
    }
  }

  bool confidence;
  set<void *> aliasSet = ast->GetAssociatedAlias(sgInitName, &confidence);

  bool existAlias = false;
  for (auto alias : aliasSet) {
    if (alias != sgInitName) {
      existAlias = true;
    }
  }

  return !confidence || existAlias;

#if 0
  void *sgInitName = ast->GetVariableInitializedName(aRef);
  void *sgDecl = isSgInitializedName(static_cast<SgNode *>(
        sgInitName)->get_declaration());
  if (ast->IsGlobal(sgInitName) && !ast->IsStatic(sgDecl)) {
    DER_ERROR("we do not support static global variables");
    return true;
  }

  SgNode *sgType = static_cast<SgNode *>(ast->GetTypebyVar(sgInitName));
  if (isSgReferenceType(sgType)) {
    return true;
  }

  void *sgGlobal = ast->GetGlobal(aRef);
  vector<void *> allRefs = ast->GetAllRefInScope(sgInitName, sgGlobal);
  for (auto &ref : allRefs) {
    SgNode *stmt = static_cast<SgNode *>(ast->TraceUpToStatement(ref));
    if (isSgVariableDeclaration(stmt) &&
        isSgReferenceType((static_cast<SgDeclarationStatement *>(
              stmt)->get_type()))) {
      return true;
    }

    SgNode *child = static_cast<SgNode *>(ref);
    SgNode *parent = static_cast<SgNode *>(ast->GetParent(child));
    while (isSgExpression(child) &&
        (static_cast<SgExpression *>(child)->isLValue())) {
      if (isSgAddressOfOp(parent)) {
        return true;
      } else if (isSgFunctionCallExp(parent)) {
        if ((static_cast<SgFunctionCallExp *>(parent))
                ->isChildUsedAsLValue(isSgExpression(child))) {
          return true;
        } else {
          return false;
        }
      }
      child = parent;
      parent = static_cast<SgNode *>(ast->GetParent(parent));
    }
  }

  return false;
#endif
}

bool isSuitableMulAssignOp(CSageCodeGen *ast, void *candidate, void *parent,
                           void *ref) {
  DEBUG_DER(if (!candidate || !parent || !ref) {
    DER_ERROR("nullptr found.");
    return false;
  });

  SgNode *parent_ = static_cast<SgNode *>(parent);
  void *inv = nullptr;
  if (isSgMultAssignOp(parent_) != nullptr) {
    SgNode *leftOp = isSgMultAssignOp(parent_)->get_lhs_operand();
    SgNode *rightOp = isSgMultAssignOp(parent_)->get_rhs_operand();
    if (leftOp != ref) {
      return false;
    }

    inv = rightOp;
  } else if (isSgAssignOp(parent_) != nullptr) {
    SgNode *leftOp = isSgAssignOp(parent_)->get_lhs_operand();
    SgNode *rightOp = isSgAssignOp(parent_)->get_rhs_operand();

    if (leftOp == ref && (isSgMultiplyOp(rightOp) != nullptr) &&
        ast->GetVariableInitializedName(
            isSgMultiplyOp(rightOp)->get_lhs_operand()) ==
            ast->GetVariableInitializedName(ref)) {
      inv = isSgMultiplyOp(rightOp)->get_rhs_operand();
    } else if (leftOp == ref && (isSgMultiplyOp(rightOp) != nullptr) &&
               ast->GetVariableInitializedName(
                   isSgMultiplyOp(rightOp)->get_rhs_operand()) ==
                   ast->GetVariableInitializedName(ref)) {
      inv = isSgMultiplyOp(rightOp)->get_lhs_operand();
    } else {
      return false;
    }
  } else {
    return false;
  }

  return ast->IsLoopInvariant(candidate, inv, inv);
}

bool accMulAssignNum(CSageCodeGen *ast, void *candidate, void *ref,
                     int *mulAssignNum, void **assignExp) {
  if (hasAlias(ast, candidate, ref)) {
    return false;
  }

  //  for each statement, if there is write on the variable it must be multiple
  //  assign statement, otherwise ignore the variable. For multiple assign
  //  statement, we remember the number of them in varialbe mulAssignNum. This
  //  will be checked later. We require the num to be 1.
  if (ast->has_write_conservative(ref) != 0) {
    if (isSuitableMulAssignOp(ast, candidate, ast->GetParent(ref), ref)) {
      *assignExp = ast->GetParent(ref);
      ++(*mulAssignNum);
      return true;
    }
    return false;
  }

  return true;
}

void getSuitableVariables(CSageCodeGen *ast, void *candidate,
                          map<void *, void *> *optVarNames) {
  map<void *, vector<void *>> variableDefRefPairs =
      getVariableDefRefPairs(ast, candidate);
  for (auto &refPair : variableDefRefPairs) {
    if (!isSupportedType(ast->GetTypebyVar(refPair.first))) {
      continue;
    }

    bool succ = true;
    int mulAssignNum = 0;
    void *assignExp = nullptr;
    for (auto ref : refPair.second) {
      if (!accMulAssignNum(ast, candidate, ref, &mulAssignNum, &assignExp)) {
        succ = false;
        break;
      }
    }

    if (succ && mulAssignNum == 1) {
      (*optVarNames)[refPair.first] = assignExp;
      DER_ALGO("find variable: " + printNodeInfo(ast, refPair.first));
      DER_ALGO("in stmt: " + printNodeInfo(ast, (*optVarNames)[refPair.first]));
    }
  }
}

void *getInvExp(CSageCodeGen *ast, void *mulAssignStmt) {
  DEBUG_DER(if (!mulAssignStmt) {
    DER_ERROR("nullptr found.");
    return nullptr;
  })

  SgNode *mulAssignStmt_ = static_cast<SgNode *>(mulAssignStmt);
  if (isSgAssignOp(mulAssignStmt_) != nullptr) {
    SgNode *rhs = isSgAssignOp(mulAssignStmt_)->get_rhs_operand();
    if (isSgMultiplyOp(rhs) != nullptr) {
      return isSgMultiplyOp(rhs)->get_rhs_operand();
    }
    DER_ERROR("assign exp is not of form x = x * ....");
    return nullptr;
  }
  if (isSgMultAssignOp(mulAssignStmt_) != nullptr) {
    return isSgMultAssignOp(mulAssignStmt_)->get_rhs_operand();
  }

  DER_ERROR("not a multiply assign expr.");
  return nullptr;
}

bool transform(CSageCodeGen *ast, const set<void *> &loopCandidates) {
  for (auto candidate : loopCandidates) {
    map<void *, void *> optVarNames;
    getSuitableVariables(ast, candidate, &optVarNames);

    for (auto optVarPair : optVarNames) {
      auto varInitName = optVarPair.first;
      auto mulAssignStmt = optVarPair.second;

      static set<void *> mathHeaderStatus;
      void *global = ast->GetGlobal(mulAssignStmt);
      if (mathHeaderStatus.find(global) == mathHeaderStatus.end()) {
        ast->AddHeader("\n#include <math.h>", global);
        mathHeaderStatus.insert(global);
      }

      //  define two varialbes: def1 for index i, def2 for x_0
      //  insert the varialbes before the candidate loop
      //  FIXME: x must be defined out of the loop
      //  example: insert "int i = 0; float x_0 = x;"
      static int varIndex = 0;
      void *sg_type = ast->GetTypebyVar(varInitName);
      void *varDef1 = ast->CreateVariableDecl(
          "int", "__merlin_mul_assign_reduce_" + std::to_string(++varIndex),
          ast->CreateConst(0), ast->GetScope(candidate));
      void *varDef2 = ast->CreateVariableDecl(
          sg_type, "__merlin_mul_assign_reduce_" + std::to_string(++varIndex),
          ast->CreateVariableRef(varInitName), ast->GetScope(candidate));
      ast->InsertStmt(varDef1, candidate);
      ast->InsertStmt(varDef2, candidate);

      //  FIXME: insert this after the target statement (x *= inv)
      //  example: insert "i++;"
      void *incStmt = ast->CreateStmt(
          V_SgExprStatement,
          ast->CreateExp(V_SgPlusPlusOp, ast->CreateVariableRef(varDef1),
                         nullptr, 0));
      ast->AppendChild(ast->GetLoopBody(candidate), incStmt);

      //  replace "x *= inv" with "x = x_0 * pow(inv, i + 1)"
      void *invExp = getInvExp(ast, mulAssignStmt);
      vector<void *> para_list = {
          ast->CopyExp(invExp),
          ast->CreateCastExp(ast->CreateExp(V_SgAddOp,
                                            ast->CreateVariableRef(varDef1),
                                            ast->CreateConst(1)),
                             "double")};
      void *powerExp = ast->CreateFuncCall("pow", sg_type, para_list,
                                           ast->GetScope(mulAssignStmt));
      void *powerAssignExp = ast->CreateExp(
          V_SgAssignOp, ast->CreateVariableRef(varInitName),
          ast->CreateExp(V_SgMultiplyOp, ast->CreateVariableRef(varDef2),
                         powerExp));
      ast->ReplaceExp(mulAssignStmt, powerAssignExp);
    }
  }

  return true;
}

static int run(CSageCodeGen *ast, CMarsIr *mars_ir, void *kernel) {
  auto loopCandidates = getPipelinedLoops(ast, mars_ir, kernel);

  return static_cast<int>(transform(ast, loopCandidates));
}

int mul_assign_reduce_top(CSageCodeGen *ast, void *pTopFunc,
                          const CInputOptions &options) {
  printf("Merlin Pass [Dependency Resolve: Multiply Assign Reduce] started ... "
         "\n");

  CMarsIr mars_ir;
  try {
    mars_ir.get_mars_ir(ast, pTopFunc, options, true);
  } catch (std::exception &e) {
    cout << "[MARS_IR] Exception: Recursive function is detected" << endl;
    return 0;
  }

  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(ast, pTopFunc);
  auto kernels = mars_ir_v2.get_top_kernels();
  if (kernels.empty()) {
    DER_ERROR("no kernel function detected!");
    return 0;
  }

  int status = 1;
  for (auto kernel : kernels) {
    status = static_cast<int>((static_cast<int>(status != 0) != 0) &&
                              (run(ast, &mars_ir, kernel)) != 0);
  }

  return status;
}

int dependency_resolve_top(CSageCodeGen *ast, void *pTopFunc,
                           const CInputOptions &options) {
  // TODO(youxiang): replace "x += inv" with "x = x_0 + inv * i"
  return mul_assign_reduce_top(ast, pTopFunc, options);
}
