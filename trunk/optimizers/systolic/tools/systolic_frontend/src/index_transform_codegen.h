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


#ifndef TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_INDEX_TRANSFORM_CODEGEN_H_
#define TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_INDEX_TRANSFORM_CODEGEN_H_

#include <list>
#include <map>
#include <string>
#include <vector>
#include "codegen.h"

class CMarsIrV2;
const char CDefaultFileName[] = "block_wise_reformat.cpp";

class SystolicIndexTransformCodegen {
 public:
  SystolicIndexTransformCodegen(void *pragma, CSageCodeGen *ast, CMarsIrV2 *ir);

  // generate code info file fileName
  void autoCodegen(std::string fileName = CDefaultFileName);

 private:
  enum enumLoopState { OUTER, FEEDER, PE };

  // analyze for loop and statement info
  void extractLoopAndStmtInfo(void *sysLoop);
  void *getFirstForLoop(void *stmt);
  void *getFirstForLoop(void *stmt, const enum enumLoopState &enLoopState);
  void *getFirstForLoopFromSubStmt(void *stmt, enum enumLoopState enLoopState);
  void extractDepthInfo();
  void getSeparateOperands();
  void analysisOperandIndex(const std::string &sOperand);
  void genInputH(void *sg_scope);
  void getInputLoopSequence(std::vector<std::string> *loop_iter_var_seq);
  void getOutputLoopSequence(std::vector<std::string> *loop_iter_var_seq);
  void *genForLoops(const std::vector<std::string> &loop_iter_var_seq,
                    void *func, std::map<std::string, void *> *iter_var_map);

  void *genIdxExpr(const std::map<std::string, void *> &iter_var_map);
  void *getSubIndexExpr(const std::vector<std::string> &vec_subElem, int *bound,
                        const std::map<std::string, void *> &iter_var_map);
  void *genBwIdxExpr(const std::map<std::string, void *> &iter_var_map,
                     bool bAdjustLoopSequence = true);
  void genInput_weight(void *sg_scope);
  void *genCondExpr(const std::map<std::string, void *> &iter_var_map);
  void genOutput(void *sg_scope);
  void genReorganize(void *sg_scope);
  void exchangeLastTwoLoopIter();

  void *m_pragma;
  CSageCodeGen *m_ast;
  CMarsIrV2 *m_ir;
  std::vector<void *> m_outer_loops;
  std::vector<void *> m_feeder_loops;
  std::vector<void *> m_pe_loops;
  std::vector<void *> m_all_loops;
  std::map<std::string, int> m_var_coefs;
  std::map<std::string, int> m_loop_bounds;
  void *m_inner_stmt;
  std::vector<void *> m_inner_declarations;
  std::vector<std::vector<std::string>> m_vecLevel;
  std::map<std::string, std::vector<int>> m_depth_map_vec;
  std::map<std::string, void *> m_map_operands;
  std::list<std::vector<std::vector<std::string>>> m_operands_names;
  std::vector<std::string> m_loop_iter_var_names;
  std::string m_operand_name;
};

#endif  // TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_INDEX_TRANSFORM_CODEGEN_H_
