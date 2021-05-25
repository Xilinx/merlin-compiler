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


#include <map>
#include <set>
#include <string>
#include <vector>
#include "index_transform_codegen.h"
#include "mars_ir_v2.h"

using std::map;
using std::set;
using std::string;
using std::vector;

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsArrayRangeInScope;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using std::map;
using std::set;
using std::string;
using std::vector;

const char CSLeft[] = "left";
const char CSRight[] = "right";
const char CSOut[] = "out";
const char KTypeName[] = "data_precision";

#define DEBUGINFO(str)                                                         \
  std::cout << __LINE__ << " line: " << __FUNCTION__                           \
            << " func: " << std::string(str) << std::endl;

SystolicIndexTransformCodegen::SystolicIndexTransformCodegen(void *pragma,
                                                             CSageCodeGen *ast,
                                                             CMarsIrV2 *ir)
    : m_pragma(pragma), m_ast(ast), m_ir(ir) {
  m_inner_stmt = nullptr;
}

void SystolicIndexTransformCodegen::autoCodegen(string fileName) {
  void *top_loop = getFirstForLoop(m_ast->GetNextStmt(m_pragma));

  if ((top_loop == nullptr) ||
      ("auto" == m_ir->get_pragma_attribute(m_pragma, "auto"))) {
    return;
  }

  m_ast->RegisterType(KTypeName);
  void *newFile = m_ast->CreateSourceFile(fileName);
  string sDirective = "#include \"block_wise_reformat.h\"";
  m_ast->AddHeader(sDirective, newFile);

  // start analysis and do the transformation
  extractLoopAndStmtInfo(top_loop);
  extractDepthInfo();

  getSeparateOperands();
  analysisOperandIndex(CSLeft);
  genInputH(newFile);

  analysisOperandIndex(CSRight);
  genInput_weight(newFile);

  analysisOperandIndex(CSOut);
  genOutput(newFile);

  // reorganize has the same operand of output
  analysisOperandIndex(CSOut);
  genReorganize(newFile);
}

void SystolicIndexTransformCodegen::extractLoopAndStmtInfo(void *sysLoop) {
  CSageCodeGen *ast = m_ast;

  void *curr_loop = sysLoop;
  enumLoopState loop_state = OUTER;
  while (true) {
    if ((curr_loop = getFirstForLoop(curr_loop, loop_state)) == nullptr) {
      return;
    }

    // collect loop info
    if (loop_state == OUTER) {
      m_outer_loops.push_back(curr_loop);
    } else if (loop_state == FEEDER) {
      m_feeder_loops.push_back(curr_loop);
    } else if (loop_state == PE) {
      m_pe_loops.push_back(curr_loop);
    }

    m_all_loops.push_back(curr_loop);

    // step to next loop
    void *body = ast->GetLoopBody(curr_loop);
    void *temp = getFirstForLoop(body, loop_state);
    if (temp != nullptr) {
      body = temp;
    }
    curr_loop = body;

    // if no more loop, then collect inner stmt
    if (ast->IsForStatement(curr_loop) != 0) {
      continue;
    }
    m_inner_stmt = curr_loop;
    break;
  }
}

void *SystolicIndexTransformCodegen::getFirstForLoop(void *stmt) {
  // just for the parameter's form
  enumLoopState state = OUTER;

  return getFirstForLoop(stmt, state);
}

void *SystolicIndexTransformCodegen::getFirstForLoop(
    void *stmt, const enum enumLoopState &enLoopState) {
  if (m_ast->IsForStatement(stmt) != 0) {
    return stmt;
  }

  return getFirstForLoopFromSubStmt(stmt, enLoopState);
}

void *SystolicIndexTransformCodegen::getFirstForLoopFromSubStmt(
    void *stmt, enum enumLoopState enLoopState) {
  void *child = m_ast->GetChildStmt(stmt, 0);

  while (child != stmt) {
    if (m_ast->IsForStatement(child) != 0) {
      return child;
    }
    if ((m_ast->IsPragmaDecl(child) != 0) &&
        m_ast->GetChildStmtNum(stmt) == 2) {
      child = m_ast->GetChildStmt(stmt, 1);
      enLoopState = static_cast<enum enumLoopState>(enLoopState + 1);
      stmt = m_ast->GetChildStmt(stmt, 0);
    } else {
      stmt = child;
      child = m_ast->GetChildStmt(stmt, 0);
    }
  }

  return nullptr;
}

void SystolicIndexTransformCodegen::extractDepthInfo() {
  vector<void *> all_pragmas;
  m_ast->GetNodesByType(nullptr, "preorder", "SgPragmaDeclaration",
                        &all_pragmas);

  for (auto pragma : all_pragmas) {
    if ("interface" != m_ir->get_pragma_attribute(pragma, "interface")) {
      continue;
    }

    CAnalPragma ana_pragma(m_ast);
    bool errout;
    if (!ana_pragma.PragmasFrontendProcessing(pragma, &errout, false)) {
      continue;
    }

    string str_depth = ana_pragma.get_attribute(CMOST_depth);
    string str_name = ana_pragma.get_attribute(CMOST_variable);

    // depth pragma may have multi-dimension separated by comma
    vector<int> depth_vec;
    vector<string> sDepthVec = str_split(str_depth, ',');
    for (auto depth : sDepthVec) {
      depth_vec.push_back(atoi(depth.c_str()));
    }

    // any interface variable now have separated depth in a vector
    m_depth_map_vec[str_name] = depth_vec;
  }
}

void SystolicIndexTransformCodegen::getSeparateOperands() {
  // expression analysis
  vector<void *> vec_exp_stmt;
  m_ast->GetNodesByType(m_inner_stmt, "preorder", "SgExprStatement",
                        &vec_exp_stmt);
  assert(vec_exp_stmt.size() == 1);
  void *calc_stmt = vec_exp_stmt[0];
  void *calc_expr = m_ast->GetExprFromStmt(calc_stmt);
  void *op_reduce = calc_expr;
  void *op_map = m_ast->GetExpRightOperand(op_reduce);
  void *ref_l = m_ast->GetExpLeftOperand(op_map);
  void *ref_r = m_ast->GetExpRightOperand(op_map);
  void *ref_o = m_ast->GetExpLeftOperand(op_reduce);

  m_map_operands[CSLeft] = ref_l;
  m_map_operands[CSRight] = ref_r;
  m_map_operands[CSOut] = ref_o;
}

// analysis operand indexes, e.g. a[i][k], and i = i0 * 2 + i2 *1 + i1 *1,
// then we get i0, i2..., and there coeff, loop bound as loop iter
void SystolicIndexTransformCodegen::analysisOperandIndex(
    const string &sOperand) {
  void *operand = m_map_operands[sOperand];

  // step 1. map variable to their sub elements
  map<string, vector<string>> map_var2SubElem;  // we can know i has i0, i1, i2
                                                // part from this for example
  map<string, string>
      map_subElem2Var;  // we can know i0 has i as it's root for example

  m_ast->GetNodesByType(m_inner_stmt, "preorder", "SgVariableDeclaration",
                        &m_inner_declarations);
  for (auto decl : m_inner_declarations) {
    void *varName = m_ast->GetVariableInitializedName(decl);
    void *initializer = m_ast->GetInitializer(varName);

    vector<void *> vec_vars;
    m_ast->GetNodesByType(initializer, "preorder", "SgVarRefExp", &vec_vars);
    vector<string> vec_names;
    for (auto var : vec_vars) {
      vec_names.push_back(m_ast->_up(var));
      map_subElem2Var[m_ast->_up(var)] = m_ast->_up(varName);
    }
    map_var2SubElem[m_ast->_up(varName)] = vec_names;
  }

  // step 2. map iter loop bound and coef
  m_loop_bounds.clear();
  m_var_coefs.clear();
  m_loop_iter_var_names.clear();
  string key = m_ast->_up(m_ast->get_array_from_pntr(operand));
  m_operand_name = key;

  CMarsAccess ma(operand, m_ast, nullptr);
  for (int i = 0; i < ma.GetIndexes().size(); ++i) {
    CMarsExpression vec = ma.GetIndexes()[i];
    vector<void *> vec_vars = vec.get_vars();
    // vector<string> vec_indexes;
    for (auto var : vec_vars) {
      CMarsExpression coeff = vec.get_coeff(var);
      CMarsVariable mv = CMarsVariable(var, m_ast);
      int i_coeff = coeff.get_const();
      int i_ub = mv.get_range().get_ub().get_const();
      m_var_coefs[mv.print_var()] = i_coeff;
      m_loop_bounds[mv.print_var()] = i_ub;
      m_loop_iter_var_names.push_back(mv.print_var());
    }
  }

  // step 3. sort sub elements by coef, so we can know i = [i0][i2][i1] for
  // example
  for (auto &var : map_var2SubElem) {
    auto &subElem = var.second;

    for (size_t i = 0; i < subElem.size(); ++i) {
      int nMax = m_var_coefs[subElem[i]];
      int nIdx = i;
      for (size_t j = i; j < subElem.size(); ++j) {
        if (m_var_coefs[subElem[j]] > nMax) {
          nMax = m_var_coefs[subElem[j]];
          nIdx = j;
        }
      }
      string temp = subElem[i];
      subElem[i] = subElem[nIdx];
      subElem[nIdx] = temp;
    }
  }

  // step 4. add index to list m_operands_names
  m_operands_names.clear();
  // loop every index, e.g. [r + p] for one loop
  for (int i = 0; i < ma.GetIndexes().size(); ++i) {
    CMarsExpression vec = ma.GetIndexes()[i];
    vector<void *> vec_vars = vec.get_vars();  // get sub indexes
    set<string> set_vars;                      // get r, p for example
    for (auto var : vec_vars) {                // loop every sub index elems
      CMarsVariable mv = CMarsVariable(var, m_ast);
      set_vars.insert(map_subElem2Var[mv.print_var()]);
    }

    vector<vector<string>> vec_index;
    for (auto var : set_vars) {
      vec_index.push_back(map_var2SubElem[var]);
    }
    m_operands_names.push_back(vec_index);
  }
}

void SystolicIndexTransformCodegen::genInputH(void *sg_scope) {
  CSageCodeGen *ast = m_ast;

  // 1. create input function decalare
  vector<void *> vecParams;
  vecParams.push_back(ast->CreateVariable(string(KTypeName) + "*", "input"));
  vecParams.push_back(
      ast->CreateVariable(string(KTypeName) + "*", "input_block_wise"));

  void *func = ast->CreateFuncDecl("void", "block_wise_reformat_input",
                                   vecParams, sg_scope, true, nullptr);
  ast->AppendChild(sg_scope, func);

  // 2. generate for loops
  vector<string> loop_iter_var_seq;
  getInputLoopSequence(&loop_iter_var_seq);

  map<string, void *> iter_var_map;
  void *parentLoop = genForLoops(loop_iter_var_seq, func, &iter_var_map);

  exchangeLastTwoLoopIter();

  // 3. generate in_idx
  void *in_expr = genIdxExpr(iter_var_map);

  void *in_idx =
      ast->CreateVariableDecl("int", "in_idx", in_expr, ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), in_idx);

  // 4. generate if statement
  void *ifStmtExpr = genCondExpr(iter_var_map);

  // 5. generate in_bw_expr
  void *in_bw_expr = genBwIdxExpr(iter_var_map);
  void *in_bw_idx = ast->CreateVariableDecl("int", "in_bw_idx", in_bw_expr,
                                            ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), in_bw_idx);

  // 6. generate assign code
  void *assignOp1 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[0]),
                     ast->CreateVariableRef(in_idx));
  void *assignOp2 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[1]),
                     ast->CreateVariableRef(in_bw_idx));
  void *assignOp = ast->CreateStmt(V_SgAssignStatement, assignOp2, assignOp1);

  // 7. assemble all together
  if (ifStmtExpr != nullptr) {
    void *falseBody = ast->CreateStmt(
        V_SgAssignStatement, m_ast->CopyExp(assignOp2), ast->CreateConst(0));
    void *ifStmt = ast->CreateIfStmt(ifStmtExpr, assignOp, falseBody);
    ast->AppendChild(ast->GetLoopBody(parentLoop), ifStmt);
  } else {
    ast->AppendChild(ast->GetLoopBody(parentLoop), assignOp);
  }
}

// input loop sequence should be the same as the original code,
// except it don't contain some loops, e.g. a[i][k] has no 'j' loops
void SystolicIndexTransformCodegen::getInputLoopSequence(
    vector<string> *loop_iter_var_seq) {
  CSageCodeGen *ast = m_ast;

  // step 1, get all loops iter names to separate loop vector, which implicit
  // original loop order
  vector<string> outer_loop_iter_var_names;
  vector<string> feeder_loop_iter_var_names;
  vector<string> pe_loop_iter_var_names;
  for (auto loop : m_outer_loops) {
    outer_loop_iter_var_names.push_back(ast->_up(ast->GetLoopIterator(loop)));
  }
  for (auto loop : m_feeder_loops) {
    feeder_loop_iter_var_names.push_back(ast->_up(ast->GetLoopIterator(loop)));
  }
  for (auto loop : m_pe_loops) {
    pe_loop_iter_var_names.push_back(ast->_up(ast->GetLoopIterator(loop)));
  }

  // stpe 2, m_loop_iter_var_names include loops needed by generated code, but
  // may out of order, so we need rebuild proper loops according to these two
  // info
  for (auto iter : m_loop_iter_var_names) {
    if (outer_loop_iter_var_names.end() !=
        find(outer_loop_iter_var_names.begin(), outer_loop_iter_var_names.end(),
             iter)) {
      loop_iter_var_seq->push_back(iter);
    }
  }
  for (auto iter : m_loop_iter_var_names) {
    if (feeder_loop_iter_var_names.end() !=
        find(feeder_loop_iter_var_names.begin(),
             feeder_loop_iter_var_names.end(), iter)) {
      loop_iter_var_seq->push_back(iter);
    }
  }
  for (auto iter : m_loop_iter_var_names) {
    if (pe_loop_iter_var_names.end() != find(pe_loop_iter_var_names.begin(),
                                             pe_loop_iter_var_names.end(),
                                             iter)) {
      loop_iter_var_seq->push_back(iter);
    }
  }
}

void *SystolicIndexTransformCodegen::genForLoops(
    const vector<string> &loop_iter_var_seq, void *func,
    map<string, void *> *iter_var_map) {
  // generate for loops use loop iter name and there bounds
  void *parentLoop = nullptr;
  for (auto loop_var_name : loop_iter_var_seq) {
    void *iter =
        m_ast->CreateVariableDecl("int", loop_var_name, m_ast->CreateConst(0),
                                  m_ast->GetScope(m_ast->GetFuncBody(func)));
    (*iter_var_map)[loop_var_name] = iter;
    void *test = m_ast->CreateStmt(
        V_SgExprStatement,
        m_ast->CreateExp(V_SgLessThanOp, m_ast->CreateVariableRef(iter),
                         m_ast->CreateConst(m_loop_bounds[loop_var_name] + 1)));
    void *inc =
        m_ast->CreateExp(V_SgPlusPlusOp, m_ast->CreateVariableRef(iter));
    void *loop =
        m_ast->CreateForLoop(iter, test, inc, m_ast->CreateBasicBlock());
    if (parentLoop != nullptr) {
      m_ast->AppendChild(m_ast->GetLoopBody(parentLoop), loop);
    } else {
      m_ast->AppendChild(m_ast->GetFuncBody(func), loop);
    }

    parentLoop = loop;
  }

  return parentLoop;
}

void *SystolicIndexTransformCodegen::getSubIndexExpr(
    const vector<string> &vec_subElem, int *bound,
    const map<string, void *> &iter_var_map) {
  void *index_expr = nullptr;
  *bound = 1;
  for (auto var : vec_subElem) {
    if (index_expr != nullptr) {
      if (m_loop_bounds[var] > 0) {
        index_expr =
            m_ast->CreateExp(V_SgMultiplyOp, index_expr,
                             m_ast->CreateConst(m_loop_bounds[var] + 1));
      }

      assert(iter_var_map.find(var) != iter_var_map.end());
      index_expr =
          m_ast->CreateExp(V_SgAddOp, index_expr,
                           m_ast->CreateVariableRef(iter_var_map.at(var)));
      // index_expr = m_ast->CreateExp(V_SgAddOp, index_expr,
      // m_ast->CreateConst(0));
    } else {
      assert(iter_var_map.find(var) != iter_var_map.end());
      index_expr = m_ast->CreateVariableRef(iter_var_map.at(var));
      // index_expr = m_ast->CreateConst(0);
    }

    *bound *= m_loop_bounds[var] + 1;
  }

  return index_expr;
}

void *SystolicIndexTransformCodegen::genIdxExpr(
    const map<string, void *> &iter_var_map) {
  void *in_expr = nullptr;
  // list<vector<vector<string>>> m_operands_names;
  // step 1. get depth pragma as index bound
  int index_count = 0;
  vector<int> depth;
  if (m_depth_map_vec.find(m_operand_name) != m_depth_map_vec.end()) {
    depth = m_depth_map_vec[m_operand_name];
  }

  for (auto index : m_operands_names) {  // index is vector<vector<string>>
    void *index_expr = nullptr;
    int index_bound = 0;
    int count = 0;
    // get index expr and it's bound
    for (auto sub_index : index) {  // index is vector<string>
      count++;
      int sub_index_bound = 1;
      void *sub_index_expr =
          getSubIndexExpr(sub_index, &sub_index_bound, iter_var_map);
      if (index_expr != nullptr) {
        index_expr = m_ast->CreateExp(V_SgAddOp, index_expr, sub_index_expr);
      } else {
        index_expr = sub_index_expr;
      }
      index_bound += sub_index_bound;
    }

    // if we have depth pragma, prefer to use this as bound
    if (depth.size() > index_count) {  // dimension
      index_bound = depth[index_count++];
    } else {
      // in the same index, index bound should subtract their (count - 1)
      index_bound -= count - 1;
    }

    if (in_expr != nullptr) {
      if (index_bound > 1) {
        in_expr = m_ast->CreateExp(V_SgMultiplyOp, in_expr,
                                   m_ast->CreateConst(index_bound));
      }

      in_expr = m_ast->CreateExp(V_SgAddOp, in_expr, index_expr);
    } else {
      in_expr = index_expr;
    }
  }

  return in_expr;
}

void *SystolicIndexTransformCodegen::genBwIdxExpr(
    const map<string, void *> &iter_var_map, bool bAdjustLoopSequence) {
  // put the outer loop iter first
  // vector<void *> m_outer_loops;

  // step 1. modify sequence follow the outer
  if (bAdjustLoopSequence) {
    set<string> set_outer_names;
    for (auto loop : m_outer_loops) {
      set_outer_names.insert(m_ast->_up(m_ast->GetLoopIterator(loop)));
    }

    // get the outer iter and delete it in the original place
    vector<string> outer_iter;
    for (auto &index : m_operands_names) {  // index is vector<vector<string>>
      for (auto &sub_index : index) {       // sub_index is vector<string>
        auto it = sub_index.begin();
        while (it != sub_index.end()) {
          if (set_outer_names.find(*it) != set_outer_names.end()) {
            outer_iter.push_back(*it);
            it = sub_index.erase(it);
          } else {
            it++;
          }
        }
      }
    }

    // add the outer to the begin of the data
    for (auto it = outer_iter.rbegin(); it != outer_iter.rend(); ++it) {
      vector<string> vec_sub_index;
      vector<vector<string>> vec_index;

      vec_sub_index.push_back(*it);
      vec_index.push_back(vec_sub_index);

      m_operands_names.push_front(vec_index);
    }
  }

  // step 2. generate block wise index
  void *in_bw_expr = nullptr;
  // list<vector<vector<string>>> m_operands_names;
  for (auto index : m_operands_names) {
    void *index_expr = nullptr;
    int index_bound = 0;
    int count = 0;
    for (auto sub_index : index) {
      count++;
      int sub_index_bound = 1;
      void *sub_index_expr =
          getSubIndexExpr(sub_index, &sub_index_bound, iter_var_map);
      if (index_expr != nullptr) {
        index_expr = m_ast->CreateExp(V_SgAddOp, index_expr, sub_index_expr);
      } else {
        index_expr = sub_index_expr;
      }
      index_bound += sub_index_bound;
    }

    index_bound -= count - 1;

    if (in_bw_expr != nullptr) {
      if (index_bound > 1) {
        in_bw_expr = m_ast->CreateExp(V_SgMultiplyOp, in_bw_expr,
                                      m_ast->CreateConst(index_bound));
      }

      in_bw_expr = m_ast->CreateExp(V_SgAddOp, in_bw_expr, index_expr);
    } else {
      in_bw_expr = index_expr;
    }
  }

  return in_bw_expr;
}

void SystolicIndexTransformCodegen::genInput_weight(void *sg_scope) {
  CSageCodeGen *ast = m_ast;

  // 1. create input function decalare
  vector<void *> vecParams;
  vecParams.push_back(ast->CreateVariable(string(KTypeName) + "*", "weight"));
  vecParams.push_back(
      ast->CreateVariable(string(KTypeName) + "*", "weight_block_wise"));

  void *func = ast->CreateFuncDecl("void", "block_wise_reformat_weight",
                                   vecParams, sg_scope, true, nullptr);
  ast->AppendChild(sg_scope, func);

  // 2. generate for loops
  vector<string> loop_iter_var_seq;
  getInputLoopSequence(&loop_iter_var_seq);

  map<string, void *> iter_var_map;
  void *parentLoop = genForLoops(loop_iter_var_seq, func, &iter_var_map);

  exchangeLastTwoLoopIter();

  // 3. generate in_idx
  void *in_expr = genIdxExpr(iter_var_map);

  void *w_idx =
      ast->CreateVariableDecl("int", "w_idx", in_expr, ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), w_idx);

  // 4. generate if statement
  void *ifStmtExpr = genCondExpr(iter_var_map);

  // 5. generate in_bw_expr
  void *in_bw_expr = genBwIdxExpr(iter_var_map);
  void *in_bw_idx = ast->CreateVariableDecl("int", "w_bw_idx", in_bw_expr,
                                            ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), in_bw_idx);

  // 6. generate assign code
  void *assignOp1 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[0]),
                     ast->CreateVariableRef(w_idx));
  void *assignOp2 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[1]),
                     ast->CreateVariableRef(in_bw_idx));
  void *assignOp = ast->CreateStmt(V_SgAssignStatement, assignOp2, assignOp1);

  // 7. assemble all together
  if (ifStmtExpr != nullptr) {
    void *falseBody = ast->CreateStmt(
        V_SgAssignStatement, m_ast->CopyExp(assignOp2), ast->CreateConst(0));
    void *ifStmt = ast->CreateIfStmt(ifStmtExpr, assignOp, falseBody);
    ast->AppendChild(ast->GetLoopBody(parentLoop), ifStmt);
  } else {
    ast->AppendChild(ast->GetLoopBody(parentLoop), assignOp);
  }
}

void *SystolicIndexTransformCodegen::genCondExpr(
    const map<string, void *> &iter_var_map) {
  // don't need generate condition statement if don't have depth pragma
  if (m_depth_map_vec.find(m_operand_name) == m_depth_map_vec.end()) {
    return nullptr;
  }

  vector<int> vec_depth = m_depth_map_vec[m_operand_name];

  // create index_variable and there bound expression for ifstmt
  void *condExpr = nullptr;

  // list<vector<vector<string>>> m_operands_names;
  assert(vec_depth.size() == m_operands_names.size());
  int i = 0;
  for (auto index : m_operands_names) {  // index is vector<vector<string>>
    void *index_expr = nullptr;
    for (auto sub_index : index) {  // sub_index is vector<string>
      int sub_index_bound = 1;
      void *sub_index_expr =
          getSubIndexExpr(sub_index, &sub_index_bound, iter_var_map);
      if (index_expr != nullptr) {
        index_expr = m_ast->CreateExp(V_SgAddOp, index_expr, sub_index_expr);
      } else {
        index_expr = sub_index_expr;
      }
    }

    int depth = vec_depth[i++];
    assert(depth > 0);

    index_expr =
        m_ast->CreateExp(V_SgLessThanOp, index_expr, m_ast->CreateConst(depth));
    if (condExpr != nullptr) {
      condExpr = m_ast->CreateExp(V_SgAndOp, condExpr, index_expr);
    } else {
      condExpr = index_expr;
    }
  }

  return condExpr;
}

void SystolicIndexTransformCodegen::genOutput(void *sg_scope) {
  CSageCodeGen *ast = m_ast;

  // 1. create output function decalare
  vector<void *> vecParams;
  vecParams.push_back(
      ast->CreateVariable(string(KTypeName) + "*", "output_block_wise"));
  vecParams.push_back(ast->CreateVariable(string(KTypeName) + "*", "output"));

  void *func = ast->CreateFuncDecl("void", "block_wise_reformat_output",
                                   vecParams, sg_scope, true, nullptr);
  ast->AppendChild(sg_scope, func);

  // 2. generate for loops
  vector<string> loop_iter_var_seq;
  getOutputLoopSequence(&loop_iter_var_seq);

  map<string, void *> iter_var_map;
  void *parentLoop = genForLoops(loop_iter_var_seq, func, &iter_var_map);

  // 3. generate out_idx
  void *out_expr = genIdxExpr(iter_var_map);

  void *out_idx = ast->CreateVariableDecl("int", "out_idx", out_expr,
                                          ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), out_idx);

  // 4. generate if statement
  void *ifStmtExpr = genCondExpr(iter_var_map);

  // 5. generate out_bw_expr
  void *out_bw_expr = genBwIdxExpr(iter_var_map);
  void *out_bw_idx = ast->CreateVariableDecl("int", "out_bw_idx", out_bw_expr,
                                             ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), out_bw_idx);

  // 6. generate assign code
  void *assignOp1 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[0]),
                     ast->CreateVariableRef(out_bw_idx));
  void *assignOp2 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[1]),
                     ast->CreateVariableRef(out_idx));
  void *assignOp = ast->CreateStmt(V_SgAssignStatement, assignOp2, assignOp1);

  // 7. assemble all together
  if (ifStmtExpr != nullptr) {
    void *ifStmt = ast->CreateIfStmt(ifStmtExpr, assignOp, nullptr);
    ast->AppendChild(ast->GetLoopBody(parentLoop), ifStmt);
  } else {
    ast->AppendChild(ast->GetLoopBody(parentLoop), assignOp);
  }
}

void SystolicIndexTransformCodegen::genReorganize(void *sg_scope) {
  CSageCodeGen *ast = m_ast;

  // 1. create reorganize function decalare
  vector<void *> vecParams;
  vecParams.push_back(
      ast->CreateVariable(string(KTypeName) + "*", "output_fm_drain"));
  vecParams.push_back(ast->CreateVariable(string(KTypeName) + "*",
                                          "output_fm_opencl_block_wise"));

  void *func = ast->CreateFuncDecl("void", "data_reorganization", vecParams,
                                   sg_scope, true, nullptr);
  ast->AppendChild(sg_scope, func);

  // 2. generate for loops
  vector<string> loop_iter_var_seq;
  getOutputLoopSequence(&loop_iter_var_seq);

  map<string, void *> iter_var_map;
  void *parentLoop = genForLoops(loop_iter_var_seq, func, &iter_var_map);

  // 3. generate out_idx
  genIdxExpr(iter_var_map);
  void *out_expr = genBwIdxExpr(iter_var_map);

  void *out_idx = ast->CreateVariableDecl("int", "out_idx", out_expr,
                                          ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), out_idx);

  exchangeLastTwoLoopIter();

  // 4. generate out_bw_idx, reorganize the col order, that is exchange the last
  // two indexes of PE loop
  void *out_bw_expr = genBwIdxExpr(iter_var_map, false);

  void *out_bw_idx = ast->CreateVariableDecl("int", "out_bw_idx", out_bw_expr,
                                             ast->GetFuncBody(func));
  ast->AppendChild(ast->GetLoopBody(parentLoop), out_bw_idx);

  // 5. generate assign code
  void *assignOp1 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[0]),
                     ast->CreateVariableRef(out_bw_idx));
  void *assignOp2 =
      ast->CreateExp(V_SgPntrArrRefExp, ast->CreateVariableRef(vecParams[1]),
                     ast->CreateVariableRef(out_idx));
  void *assignOp = ast->CreateStmt(V_SgAssignStatement, assignOp2, assignOp1);
  ast->AppendChild(ast->GetLoopBody(parentLoop), assignOp);
}

// because systolic frontend give the oppsite order of the two last two iters,
// so we need exchange the last two loop iter if it is not outer loop
void SystolicIndexTransformCodegen::exchangeLastTwoLoopIter() {
  // list<vector<vector<string>>> m_operands_names;
  // step 1. find last two names
  string last;
  string secondFromLast;
  for (auto vec : m_operands_names.back()) {
    for (auto var : vec) {
      secondFromLast = last;
      last = var;
    }
  }

  if (secondFromLast.empty()) {
    return;
  }

  // step 2. if one of the loop is outer, no need to exchange
  for (auto loop : m_outer_loops) {
    string name = m_ast->_up(m_ast->GetLoopIterator(loop));
    if (name == last || name == secondFromLast) {
      return;
    }
  }

  // step 2. exchange the last two loop iter
  for (auto &vec : m_operands_names.back()) {
    for (auto &var : vec) {
      if (var == last) {
        var = secondFromLast;
      } else if (var == secondFromLast) {
        var = last;
      }
    }
  }
}

// generate loop sequence for output, loops with bigger coeff first.
void SystolicIndexTransformCodegen::getOutputLoopSequence(
    vector<string> *loop_iter_var_seq) {
  // list<vector<vector<string>>> m_operands_names;
  for (auto index : m_operands_names) {
    for (auto sub_index : index) {
      for (auto var : sub_index) {
        loop_iter_var_seq->push_back(var);
      }
    }
  }
}
