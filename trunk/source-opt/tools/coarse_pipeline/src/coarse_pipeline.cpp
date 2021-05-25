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
#include <string>

#include "coarse_pipeline.h"
#include "codegen.h"
#include "id_update.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "memory_partition.h"
#include "program_analysis.h"
#include "rose.h"
#include "ir_types.h"
using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using std::map;
using std::set;
using std::string;
using std::vector;

int coarse_pipeline_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  CoarsePipeline(codegen, pTopFunc, options);
  return 1;
}

void CoarsePipeline(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options) {
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  mars_ir.update_loop_node_info(codegen);
  mars_ir.check_hls_partition(codegen, pTopFunc);
  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(codegen, pTopFunc, false, false);
  std::cout << "==============================================" << std::endl;
  std::cout << "-----=# Coarse Pipeline Optimization Start#=----\n";
  std::cout << "==============================================" << std::endl;

  if (!options.get_option_key_value("-a", "naive_hls").empty()) {
    cout << "[MARS-PARALLEL-MSG] Naive HLS mode.\n";
    vector<CMirNode *> vec_nodes;
    mars_ir.get_topological_order_nodes(&vec_nodes);
    for (size_t j = 0; j < vec_nodes.size(); j++) {
      CMirNode *new_node = vec_nodes[j];
      if (!new_node->is_function && (new_node->has_pipeline() != 0) &&
          !new_node->is_fine_grain) {
        remove_pragmas(codegen, new_node);
      }
    }
    return;
  }

  bool Changed = true;
  int iterative_tag = 0;
  codegen->init_defuse_range_analysis();
  while (Changed) {
    Changed = false;

    //  Revised by Yuxin, 23-Feb-2016
    //  Change the order of applying CGPIP, using mars ir topological order
    vector<CMirNode *> vec_nodes;
    mars_ir.get_topological_order_nodes(&vec_nodes);
    for (size_t j = 0; j < vec_nodes.size(); j++) {
      CMirNode *new_node = vec_nodes[j];
      if (new_node->is_while) {
        continue;
      }
      if (!new_node->is_function && (new_node->has_pipeline() != 0) &&
          !new_node->is_fine_grain) {
        vector<CMirNode *> childs = new_node->get_all_child();
        CMirNode *fNode = new_node->get_func();
        string node_num_str =
            my_itoa(iterative_tag) + "_" + my_itoa(static_cast<int>(j));

        //  Print info
#ifdef DEBUG
        cout << "[TESTING] BSU node is: " << new_node->unparseToString()
             << endl;
        cout << endl << "[TESTING] Children list " << endl;
        cout << "[TESTING] BSU node is:" << new_node->is_fine_grain
             << " unroll:" << new_node->is_complete_unroll()
             << " has_loop: " << new_node->has_loop << " \n"
             << new_node->unparseToString() << endl;
#endif

        if (generate_pipeline(codegen, pTopFunc, &mars_ir, &mars_ir_v2,
                              new_node, fNode, node_num_str)) {
          cout << "[MARS-PIPELINE-MSG] Coarse-grained pipeline is applied."
               << endl
               << endl;
          Changed = true;
        } else {
          cout << "[MARS-PIPELINE-MSG] Coarse-grained pipeline isn't "
                  "applied."
               << endl;
          cout << "\n============================================\n\n";
        }

        remove_pragmas(codegen, new_node);
        std::cout << "============================================"
                  << std::endl;
      }

      //    file_count++;
      if (Changed) {
        mars_ir.clear();
        codegen->reset_func_decl_cache();
        codegen->reset_func_call_cache();
        mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
        mars_ir_v2.build_mars_ir(codegen, pTopFunc, false, false);
        mars_ir.update_loop_node_info(codegen);
        codegen->init_defuse_range_analysis();
        break;
      }
    }
    iterative_tag++;
  }
  std::cout << "============================================" << std::endl;
  std::cout << "-----=# Coarse Pipeline Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;
}

bool generate_pipeline(CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir,
                       CMarsIrV2 *mars_ir_v2, CMirNode *bNode, CMirNode *fNode,
                       string node_num_str) {
  node_table_map node_table;
  var_table_map var_table;
  vector<SgVariableDeclaration *> var_declare_vec;

  //  Preprocess
  void *for_loop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  bool inc_dir;
  bool inclusive_bound;
  int64_t nStep = 0;
  bool is_canonical =
      codegen->IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step, &body,
                                  &inc_dir, &inclusive_bound) != 0;
  is_canonical &= codegen->GetLoopStepValueFromExpr(step, &nStep);
  if (!is_canonical) {
#if PROJDEBUG
    cout << codegen->_up(for_loop) << endl;
#endif
    cout << "[MARS-CGPIP-WARN] Non-canonical loop is found\n";
    reportNonCanonical(for_loop, bNode, codegen);
    return false;
  }
  //  FIXME:
  if (!inc_dir) {
    cout << "[MARS-CGPIP-WARN] Incremantal direction is negative\n";
    return false;
  }

  bool ret = label_processing(codegen, pTopFunc, bNode);
  if (!ret) {
    cout << "Goto will jump into the current loop. Quit coarse pipeline for "
            "this loop.\n";
    return false;
  }
  codegen->init_defuse_range_analysis();
  //  Step 1: Build dependency graph
  if (!build_graph(codegen, bNode, &node_table, &var_declare_vec)) {
    return false;
  }
  vector<void *> var_decl_old;
  vector<void *> var_decl_new;
  set<void *> var_init_new_set;

  if (!var_declare_vec.empty()) {
    cout << "[preprocessing: ]\n";
    bool changed = false;
    if (!preprocessing(codegen, mars_ir, bNode, var_declare_vec, &var_decl_old,
                       &var_decl_new, &var_init_new_set, &changed)) {
      postprocessing_fail(codegen, mars_ir, bNode, var_decl_old, var_decl_new);
      cout << "Preprocessing failed. Quit coarse pipeline for this loop.\n";
      return false;
    }
    if (changed) {
      codegen->reset_func_call_cache();
    }
    if (!build_graph(codegen, bNode, &node_table, &var_declare_vec)) {
      postprocessing_fail(codegen, mars_ir, bNode, var_decl_old, var_decl_new);
      cout << "[build graph FAIL ]\n";
      return false;
    }
  }
#if 1
  bool changed1 = false;
  if (!frame_nodes(codegen, &node_table, bNode, &changed1)) {
    postprocessing_fail(codegen, mars_ir, bNode, var_decl_old, var_decl_new);
    cout << "[frame nodes FAIL ]\n";
    return false;
  }

  if (changed1) {
    codegen->init_defuse_range_analysis();
    if (!build_graph(codegen, bNode, &node_table, &var_declare_vec)) {
      postprocessing_fail(codegen, mars_ir, bNode, var_decl_old, var_decl_new);
      return false;
    }
  }
#endif
#ifdef DEBUG
  print_table(&node_table);
#endif

  //  Step1
  if (!dependency_analysis(codegen, pTopFunc, mars_ir, bNode, fNode,
                           var_init_new_set, &node_table, &var_table)) {
    postprocessing_fail(codegen, mars_ir, bNode, var_decl_old, var_decl_new);
    return false;
  }

  //  Step2
  int stage_sum;
  if (!stage_assignment(codegen, bNode, &node_table, &stage_sum, &var_table,
                        var_init_new_set)) {
    postprocessing_fail(codegen, mars_ir, bNode, var_decl_old, var_decl_new);
    return false;
  }
  postprocessing_success(codegen, mars_ir, bNode, var_decl_old, var_decl_new);

  //  step3

  bool cg_level_2 = false;
  if (!channel_analysis(codegen, bNode, fNode, &node_table, &var_table,
                        stage_sum, var_init_new_set, &cg_level_2)) {
    return false;
  }

  cout << "Total pipeline stage level: " << stage_sum << endl;
  //  Step4
  if (stage_sum > 1) {
    bool ret = code_generation(codegen, mars_ir, mars_ir_v2, bNode, fNode,
                               &node_table, stage_sum, node_num_str, &var_table,
                               var_init_new_set, cg_level_2);
    if (ret) {
      return true;
    }
  }
  return false;
}

bool build_graph(CSageCodeGen *codegen, CMirNode *bNode,
                 node_table_map *node_table,
                 vector<SgVariableDeclaration *> *var_declare_vec) {
#if PROJDEBUG
  cout << "[MARS-PIPELINE-PARSING] Dependency graph parsing: " << endl;
#endif
  var_declare_vec->clear();
  node_table->clear();

  SgBasicBlock *kernel_body = bNode->ref;
  SgForStatement *sg_loop = isSgForStatement(kernel_body->get_parent());
  Rose_STL_Container<SgNode *> breakList =
      NodeQuery::querySubTree(kernel_body, V_SgBreakStmt);
  if (!breakList.empty()) {
    Rose_STL_Container<SgNode *>::iterator iter = breakList.begin();
    for (iter = breakList.begin(); iter != breakList.end(); iter++) {
      if (codegen->IsScopeBreak(*iter, sg_loop)) {
        reportUnsupportStatement(*iter, bNode, codegen);
        cout << "Direct Break statement is not supported.\n";
        return false;
      }
    }
  }

  Rose_STL_Container<SgNode *> continueList =
      NodeQuery::querySubTree(kernel_body, V_SgContinueStmt);
  if (!continueList.empty()) {
    Rose_STL_Container<SgNode *>::iterator iter = continueList.begin();
    for (iter = continueList.begin(); iter != continueList.end(); iter++) {
      if (SageInterface::getEnclosingNode<SgForStatement>(*iter) == sg_loop) {
        reportUnsupportStatement(*iter, bNode, codegen);
        cout << "Direct Continue statement is not supported.\n";
        return false;
      }
    }
  }

  Rose_STL_Container<SgNode *> gotoList =
      NodeQuery::querySubTree(kernel_body, V_SgGotoStatement);
  if (!gotoList.empty()) {
    Rose_STL_Container<SgNode *>::iterator iter = gotoList.begin();
    for (iter = gotoList.begin(); iter != gotoList.end(); iter++) {
      reportUnsupportStatement(*iter, bNode, codegen);
      cout << "goto statement is not supported.\n";
      return false;
    }
  }

  SgStatementPtrList &stmt_lst = kernel_body->getStatementList();
  int stmt_tag = 0;
  int node_count = 0;

  if (stmt_lst.empty()) {
    return false;
  }

  for (size_t i = 0; i < stmt_lst.size(); i++) {
    if (isSgPragmaDeclaration(stmt_lst[i]) != nullptr) {
      continue;
    }
    if (isSgVariableDeclaration(stmt_lst[i]) != nullptr) {
#if DEBUG
      cout << "[MARS-IR-PARSING] Variable declarations are parsed as a "
              "pre-pipeline graph node "
           << endl;
#endif
      SgVariableDeclaration *init_decl_stmt =
          isSgVariableDeclaration(stmt_lst[i]);
      var_declare_vec->push_back(init_decl_stmt);
      if (stmt_tag != 0) {
        node_count++;
        stmt_tag = 0;
      }

    } else if (isSgIfStmt(stmt_lst[i]) != nullptr) {
#if DEBUG
      cout << "[MARS-IR-PARSING] If statement is parsed as a graph node "
           << endl;
#endif
      if (stmt_tag != 0) {
        node_count++;
        stmt_tag = 0;
      }
      CGraphNode graph_node;
      graph_node.push_back(stmt_lst[i]);
      (*node_table)[node_count] = graph_node;
      (*node_table)[node_count].type = 3;
      node_count++;
    } else if (isSgBasicBlock(stmt_lst[i]) != nullptr) {
#if DEBUG
      cout << "[MARS-IR-PARSING] Basic block is parsed as a graph node "
           << endl;
#endif
      if (stmt_tag != 0) {
        node_count++;
        stmt_tag = 0;
      }

      CGraphNode graph_node;
      graph_node.push_back(stmt_lst[i]);
      (*node_table)[node_count] = graph_node;
      (*node_table)[node_count].type = 2;
      node_count++;
    } else if (isSgForStatement(stmt_lst[i]) != nullptr) {
#ifdef DEBUG
      cout << "[MARS-IR-PARSING] For loop is parsed as a graph node " << endl;
#endif
      if (stmt_tag != 0) {
        node_count++;
        stmt_tag = 0;
      }

      CGraphNode graph_node;
      graph_node.push_back(stmt_lst[i]);
      (*node_table)[node_count] = graph_node;
      (*node_table)[node_count].type = 0;
      node_count++;
    } else if (isSgWhileStmt(stmt_lst[i]) != nullptr) {
#if DEBUG
      cout << "[MARS-IR-PARSING] While statement is parsed as a graph node "
           << endl;
#endif
      if (stmt_tag != 0) {
        node_count++;
        stmt_tag = 0;
      }

      CGraphNode graph_node;
      graph_node.push_back(stmt_lst[i]);
      (*node_table)[node_count] = graph_node;
      (*node_table)[node_count].type = 4;
      node_count++;
    } else if (isSgExprStatement(stmt_lst[i]) != nullptr) {
      SgExprStatement *sg_stmt = isSgExprStatement(stmt_lst[i]);

      if (isSgFunctionCallExp(sg_stmt->get_expression()) != nullptr) {
#ifdef DEBUG
        cout
            << "[MARS-COARSE-PIPTLINE] Function call is parsed as a graph node "
            << endl;
#endif

        if (stmt_tag != 0) {
          node_count++;
          stmt_tag = 0;
        }
        CGraphNode graph_node;
        graph_node.push_back(stmt_lst[i]);
        (*node_table)[node_count] = graph_node;
        (*node_table)[node_count].type = 1;
        node_count++;
      } else {
#ifdef DEBUG
        cout << "[MARS-IR-PARSING] An expression statement is parsed as a "
                "graph node "
             << endl;
#endif
        if (stmt_tag != 0) {
          (*node_table)[node_count].push_back(stmt_lst[i]);
        } else {
          stmt_tag = 1;
          CGraphNode graph_node;
          graph_node.push_back(stmt_lst[i]);
          (*node_table)[node_count] = graph_node;
          (*node_table)[node_count].type = 6;
        }
      }

    } else {
#ifdef DEBUG
      cout << "[MARS-IR-PARSING] A statement is parsed as a graph node "
           << endl;
#endif
      if (stmt_tag != 0) {
        (*node_table)[node_count].push_back(stmt_lst[i]);
      } else {
        stmt_tag = 1;
        CGraphNode graph_node;
        graph_node.push_back(stmt_lst[i]);
        (*node_table)[node_count] = graph_node;
        (*node_table)[node_count].type = 6;
      }
    }
  }

  if (node_table->size() == 1 && (*node_table)[0].size() > 1) {
    cout << "NOTE: EACH STATEMENT will be a node\n";
    node_table->clear();
    for (size_t i = 0; i < stmt_lst.size(); i++) {
      CGraphNode graph_node;
      graph_node.push_back(stmt_lst[i]);
      (*node_table)[i] = graph_node;
      (*node_table)[i].type = 6;
    }
    return true;
  }

  if (node_table->size() == 1 && var_declare_vec->empty()) {
    cout << "[MARS-CGPIP-WARN] Only one statement exists in the loop body. "
            "Quit coarse-grained pipeline."
         << endl;
    reportOneStatement(bNode, codegen);
    return false;
  }

  return true;
}

bool dependency_analysis(CSageCodeGen *codegen, void *pTopFunc,
                         CMarsIr *mars_ir, CMirNode *bNode, CMirNode *fNode,
                         set<void *> var_init_new_set,
                         node_table_map *node_table, var_table_map *var_table) {
  for (size_t i = 0; i < node_table->size(); i++) {
    vector<SgExpression *> write_ref;
    vector<SgExpression *> read_ref;
    for (auto it = (*node_table)[i].begin(); it != (*node_table)[i].end();
         it++) {
      SgStatement *stmt = *it;
      if (!parse_ref_normal(codegen, pTopFunc, mars_ir, stmt, &(*node_table)[i],
                            bNode, fNode, var_table, i)) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < node_table->size(); i++) {
    for (size_t j = 0; j < node_table->size(); j++) {
      //  FIXME: Only do dependency analysis between two neibourgh nodes July
      //  24 2015
      cout << "Dependency analysis between node " << i << " and " << j << endl;
      if (!GetNodeDependence(codegen, mars_ir, bNode, node_table,
                             &(*node_table)[i], &(*node_table)[j],
                             var_init_new_set, i, j)) {
        cout << "===> Check fail between node " << i << " and " << j << endl;
        return false;
      }
    }
  }
  return true;
}

bool parse_ref_normal(CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir,
                      SgNode *stmt, CGraphNode *graph_node, CMirNode *bNode,
                      CMirNode *fNode, var_table_map *var_table, int node_i) {
  SgFunctionDeclaration *kernel_decl =
      isSgFunctionDefinition(fNode->ref->get_scope())->get_declaration();

  Rose_STL_Container<SgNode *> varList =
      NodeQuery::querySubTree(stmt, V_SgVarRefExp);
  Rose_STL_Container<SgNode *>::iterator var;

  for (var = varList.begin(); var != varList.end(); var++) {
    SgExpression *ref = isSgExpression(*var);
    void *arr_var = codegen->GetVariableInitializedName(ref);
    if (arr_var == nullptr) {
      cout << "[MARS-PIPELINE-WARN] Didnt find variable\n";
      return false;
    }
    //  For global data
    if (codegen->IsGlobalInitName(arr_var) != 0) {
#if PROJDEBUG
      cout << "Global decl ref: " << codegen->_p(arr_var) << endl;
#endif
      continue;
    }
    string varName = codegen->_up(arr_var);

    //  Testing subloop canonical
    void *for_loop = codegen->GetEnclosingNode("ForLoop", ref);
    void *iv;
    void *lb;
    void *ub;
    void *step;
    void *body;
    bool inc_dir;
    bool inclusive_bound;
    int64_t nStep = 0;
    bool is_canonical =
        codegen->IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step, &body,
                                    &inc_dir, &inclusive_bound) != 0;
    is_canonical &= codegen->GetLoopStepValueFromExpr(step, &nStep);
    if (!is_canonical) {
#if PROJDEBUG
      cout << codegen->_up(for_loop) << endl;
#endif
      cout << "[MARS-CGPIP-WARN] Non-canonical loop is found\n";
      reportNonCanonical(for_loop, bNode, codegen);
      return false;
    }

    if (isLoopIterator(codegen, bNode->ref, arr_var, ref)) {
#if PROJDEBUG
      cout << "Loop iterator ref: " << codegen->_p(arr_var) << endl;
#endif
      continue;
    }

    void *scope = codegen->GetScope(arr_var);
    if (codegen->IsInScope(scope, stmt) != 0) {
#if PROJDEBUG
      cout << "Out of scope ref: " << codegen->_p(arr_var) << endl;
#endif
      continue;
    }

    map<void *, bool> res;
    bool port_tag =
        mars_ir->any_trace_is_bus(codegen, kernel_decl, arr_var, &res);
    if (graph_node->port.find(varName) == graph_node->port.end()) {
      graph_node->port[varName].var = arr_var;
      graph_node->port[varName].is_port = port_tag;
      graph_node->port[varName].w_type = 0;
    }

    if (var_table->find(varName) == var_table->end()) {
      CVarNode arr_node;
      arr_node.var = arr_var;
      arr_node.is_port = port_tag;
      arr_node.w_type = 0;
      (*var_table)[varName] = arr_node;
    }
    //  return 0 for read, 1 for write, 2 for read_write
    int wr_tag = codegen->array_ref_is_write_conservative(ref);
    int self_write_tag = codegen->is_write_conservative(ref, false);
    if (self_write_tag != 0) {
      (*var_table)[varName].self_w_type = 1;
    }
#if PROJDEBUG
    cout << "[print ] wr_type to array/pointer '" << ref->unparseToString()
         << "' :" << wr_tag << endl;
#endif
    if (wr_tag == 2) {
#if PROJDEBUG
      cout << " Read & write to array/pointer " << ref->unparseToString()
           << endl;
#endif
      graph_node->read_ref.push_back(ref);
      graph_node->write_ref.push_back(ref);
      (*var_table)[varName].w_type = 1;
      graph_node->port[varName].w_type = 1;
    } else if (wr_tag == 0) {
#if PROJDEBUG
      cout << " Read from array/pointer " << ref->unparseToString() << endl;
#endif
      graph_node->read_ref.push_back(ref);
    } else if (wr_tag == 1) {
#if PROJDEBUG
      cout << " Write to array/pointer " << ref->unparseToString() << endl;
#endif
      graph_node->write_ref.push_back(ref);
      (*var_table)[varName].w_type = 1;
      graph_node->port[varName].w_type = 1;
    }
  }

  return true;
}

bool frame_nodes(CSageCodeGen *codegen, node_table_map *node_table,
                 CMirNode *bNode, bool *changed) {
  for (size_t i = 0; i < node_table->size(); i++) {
    CGraphNode graph_node = (*node_table)[i];
    if (graph_node.size() > 1 && graph_node.type != 5) {
      vector<SgStatement *> stmt_lst;
      auto it = graph_node.begin();
      SgStatement *previous_stmt =
          static_cast<SgStatement *>(codegen->GetPreviousStmt(*it));
      size_t loc = codegen->GetChildStmtIdx(bNode->ref, *it);
      for (; it != graph_node.end(); it++) {
        SgStatement *stmt = *it;
        if (isSgVariableDeclaration(stmt) != nullptr) {
          cout << "var: " << codegen->_p(stmt) << endl;
          return false;
        }
        stmt_lst.push_back(static_cast<SgStatement *>(codegen->CopyStmt(stmt)));
      }
      void *new_node = codegen->CreateBasicBlock(&stmt_lst, true);
      if (loc > 0) {
        codegen->InsertAfterStmt(new_node, previous_stmt);
      } else {
        codegen->AppendChild(bNode->ref, new_node);
      }
      for (stmt_table_vec::iterator it = graph_node.begin();
           it != graph_node.end(); it++) {
        SgStatement *stmt = *it;
        codegen->RemoveStmt(stmt);
      }
      *changed = true;
    }
  }
  return true;
}

void print_table(node_table_map *node_table) {
  cout << "Parsing node for dependency analysis: " << endl;

  for (size_t i = 0; i < node_table->size(); i++) {
    CGraphNode graph_node = (*node_table)[i];
    cout << "Node " << i << ":" << endl;
    for (stmt_table_vec::iterator it = graph_node.begin();
         it != graph_node.end(); it++) {
      SgStatement *stmt = *it;
      cout << stmt->unparseToString() << endl;
    }
  }
}

bool GetNodeDependence(CSageCodeGen *codegen, CMarsIr *mars_ir, CMirNode *bNode,
                       node_table_map *node_table, CGraphNode *write_node,
                       CGraphNode *read_node, set<void *> var_init_new_set,
                       int num_w, int num_r) {
  //  1- read after write; 2-write after write; 3-write after read; 4-read
  //  after read

  void *sg_loop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
  void *sg_func = codegen->GetEnclosingNode("Function", sg_loop);
  codegen->set_function_for_liveness(sg_func);
  vector<void *> liveIn;
  vector<void *> liveOut;
  codegen->get_liveness_for_loop(sg_loop, &liveIn, &liveOut);

  write_node->dep_map[num_r] = NA;
  map<void *, string> arr_report;
  bool check_fail = false;
  map<void *, vector<vector<size_t>>> write_lb;
  map<void *, vector<vector<size_t>>> write_ub;
  for (auto w_it = write_node->write_ref.begin();
       w_it != write_node->write_ref.end(); ++w_it) {
    for (auto r_it = read_node->read_ref.begin();
         r_it != read_node->read_ref.end(); ++r_it) {
      SgInitializedName *w_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*w_it));
      SgInitializedName *r_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*r_it));
      if ((w_arr == r_arr) && (write_node->port_dep_map[num_r].find(w_arr) ==
                               write_node->port_dep_map[num_r].end())) {
        write_node->dep_map[num_r] = RaW;
        write_node->port_dep_map[num_r][w_arr] = RaW;
#if PROJDEBUG
        cout << "Read after write dependency: " << codegen->_up(w_arr) << endl;
#endif
        assert(write_node->size() == 1);
        assert(read_node->size() == 1);
        string msg;
        vector<size_t> lb_vec;
        vector<size_t> ub_vec;
        bool bound_check = true;
        void *arr_decl = codegen->GetVariableDecl(w_arr);
        if (!test_range_legality(codegen, mars_ir, sg_loop, node_table,
                                 (*write_node)[0], (*read_node)[0], w_arr,
                                 num_w, num_r, &msg, var_init_new_set, &lb_vec,
                                 &ub_vec, &bound_check)) {
          if (codegen->IsInScope(arr_decl, bNode->ref) == 0) {
            arr_report[w_arr] = msg;
            check_fail = true;
            cout << "Read after write legality check fail.\n";
          }
        }
        // Check range only for local buffers which will have channels
        if (mars_ir->every_trace_is_bus(codegen, sg_func, w_arr, sg_loop) ==
            0) {
          bool skip_check = true;
          for (auto live : liveOut) {
            // If the array is written and alive after the loop,
            // it should be assign back and check write range.
            if (live == w_arr)
              skip_check = false;
          }
          // This is a bug of liveness analysis
          if (codegen->IsArgumentInitName(w_arr) != 0) {
            skip_check = false;
          }
          if (!skip_check && !bound_check) {
            arr_report[w_arr] = msg;
            check_fail = true;
          }

          if (write_lb.count(w_arr) > 0) {
            if (!bound_check) {
              arr_report[w_arr] = msg;
              check_fail = true;
            }
            write_lb[w_arr].push_back(lb_vec);
            write_ub[w_arr].push_back(ub_vec);
          } else {
            if (bound_check) {
              vector<vector<size_t>> arr_lb_vec;
              vector<vector<size_t>> arr_ub_vec;
              arr_lb_vec.push_back(lb_vec);
              arr_ub_vec.push_back(ub_vec);
              write_lb[w_arr] = arr_lb_vec;
              write_ub[w_arr] = arr_ub_vec;
            }
          }
        }
      }
    }
  }
  if (num_r == num_w) {
    return true;
  }
  // Yuxin: Nov/15/2019
  // The final write should cover all the previous write in
  // the current iteration.
  // Otherwise each cyclic channel will hold part of the data
  if (!check_fail) {
    for (auto check_it : write_lb) {
      void *sg_array = check_it.first;
      vector<vector<size_t>> dim_lb = check_it.second;
      vector<vector<size_t>> dim_ub = write_ub[sg_array];
      for (int i = 0; i < static_cast<int>(dim_lb.size()); i++) {
        size_t ref_lb = 0;
        size_t ref_ub = 0;
        for (int j = static_cast<int>(dim_lb[i].size() - 1); j >= 0; j--) {
          if (j == static_cast<int>(dim_lb[i].size() - 1)) {
            ref_lb = dim_lb[i][j];
            ref_ub = dim_ub[i][j];
          }
          if ((dim_lb[i][j] < ref_lb) || (dim_ub[i][j] > ref_ub)) {
            arr_report[sg_array] = "";
            check_fail = true;
          }
        }
      }
    }
  }
  for (auto it : arr_report) {
    void *init = it.first;
    string msg = it.second;
    reportLegalityCheck(codegen, bNode, init, msg, false);
  }
  if (check_fail) {
    return false;
  }

  for (auto w_it = write_node->write_ref.begin();
       w_it != write_node->write_ref.end(); ++w_it) {
    for (auto ww_it = read_node->write_ref.begin();
         ww_it != read_node->write_ref.end(); ++ww_it) {
      SgInitializedName *w_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*w_it));

      SgInitializedName *ww_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*ww_it));

      if (w_arr == ww_arr) {
        if (write_node->dep_map[num_r] == 0U) {
          write_node->dep_map[num_r] = WaW;
        }
        if (write_node->port_dep_map[num_r][w_arr] == 0U) {
          write_node->port_dep_map[num_r][w_arr] = WaW;
        }
        cout << "Write after write dependency: " << codegen->_up(w_arr) << endl;
      }
    }
  }

  for (auto r_it = write_node->read_ref.begin();
       r_it != write_node->read_ref.end(); ++r_it) {
    for (auto w_it = read_node->write_ref.begin();
         w_it != read_node->write_ref.end(); ++w_it) {
      SgInitializedName *r_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*r_it));

      SgInitializedName *w_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*w_it));

      if (r_arr == w_arr) {
        if (write_node->dep_map[num_r] == 0U) {
          write_node->dep_map[num_r] = WaR;
        }
        if (write_node->port_dep_map[num_r][w_arr] == 0U) {
          write_node->port_dep_map[num_r][w_arr] = WaR;
        }
        cout << "Write after read dependency: " << w_arr->unparseToString()
             << endl;
      }
    }
  }

  for (auto r_it = write_node->read_ref.begin();
       r_it != write_node->read_ref.end(); ++r_it) {
    for (auto rr_it = read_node->read_ref.begin();
         rr_it != read_node->read_ref.end(); ++rr_it) {
      SgInitializedName *r_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*r_it));

      SgInitializedName *rr_arr = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(*rr_it));

      if (r_arr == rr_arr) {
        if (write_node->dep_map[num_r] == 0U) {
          write_node->dep_map[num_r] = RaR;
        }
        if (write_node->port_dep_map[num_r][r_arr] == 0U) {
          write_node->port_dep_map[num_r][r_arr] = RaR;
        }
        cout << "read after read dependency: " << r_arr->unparseToString()
             << endl;
      }
    }
  }
  return true;
}

bool stage_assignment(CSageCodeGen *codegen, CMirNode *lnode,
                      node_table_map *node_table, int *stage_sum,
                      var_table_map *var_table, set<void *> var_init_new_set) {
  *stage_sum = 0;
  for (int i = 0; i < static_cast<int>(node_table->size()); i++) {
    if (i == 0) {
      (*node_table)[i].stage = 0;
      cout << "Node " << i << " stage " << (*node_table)[i].stage << endl;
    } else {
      int tag = 0;
      int max_stage = -1;
      for (int j = i - 1; j >= 0; j--) {
        if (((*node_table)[j].dep_map[i] != 0U) &&
            (*node_table)[j].stage > max_stage) {
          max_stage = (*node_table)[j].stage;
          tag = 1;
        }
      }
      if (tag == 0) {
        (*node_table)[i].stage = 0;
        cout << "Node " << i << " stage " << (*node_table)[i].stage << endl;
        (*node_table)[i].stage = 0;
      } else {
        (*node_table)[i].stage = max_stage + 1;
        cout << "Node " << i << " stage " << (*node_table)[i].stage << endl;
        if ((*node_table)[i].stage > *stage_sum) {
          *stage_sum = (*node_table)[i].stage;
        }
      }
    }
  }

  (*stage_sum)++;
  void *sg_loop = codegen->GetEnclosingNode("ForLoop", lnode->ref);
  cout << "===Feedback testing\n";
  map<enum wr_type, set<void *>> arr_checked;
  for (size_t i = 0; i < node_table->size(); i++) {
    for (size_t j = 0; j < node_table->size(); j++) {
      cout << "test node " << i << " and " << j
           << " dep: " << (*node_table)[j].dep_map[i] << endl;
      vector<void *> arr_report;
      bool check_fail = false;
      if ((*node_table)[j].stage > (*node_table)[i].stage) {
        cout << "===> \n";
        for (map<void *, enum wr_type>::iterator it =
                 (*node_table)[j].port_dep_map[i].begin();
             it != (*node_table)[j].port_dep_map[i].end(); ++it) {
          void *sg_array = it->first;
          enum wr_type dep_type = it->second;
          string arr_name = codegen->_up(sg_array);
          cout << arr_name << ", " << dep_type << endl;
          if (arr_checked[dep_type].count(sg_array) > 0) {
            continue;
          }
          if (dep_type == WaR || dep_type == RaR) {
            continue;
          }
          if (var_table->find(arr_name) != var_table->end()) {
            if (dep_type == WaW && !(*var_table)[arr_name].is_port) {
              continue;
            }
          }
          if (!test_feedback_dependency(codegen, sg_array, sg_loop, *stage_sum,
                                        dep_type, var_init_new_set)) {
            arr_report.push_back(sg_array);
            cout << "Feedback dependency exists between node " << j << " and "
                 << i << " on var " << codegen->_up(sg_array)
                 << ". Quit coarse-grained pipeline." << endl;
            check_fail = true;
          }
          arr_checked[dep_type].insert(sg_array);
        }

        if (check_fail) {
          reportFeedback(codegen, lnode, arr_report, j, i);
          return false;
        }
      }
    }
  }

  //  Node: adjust
  for (size_t i = 0; i < node_table->size(); i++) {
    int dep_stage = *stage_sum;
    if ((*node_table)[i].stage == 0) {
      for (size_t j = 0; j < node_table->size(); j++) {
        if (((*node_table)[j].dep_map[i] != 0U) && i != j &&
            (*node_table)[j].stage < dep_stage) {
          dep_stage = (*node_table)[j].stage;
        }
      }
    }
    cout << "[print dep:] node" << i << ", stage:" << dep_stage << endl;
    if (dep_stage > 1 && dep_stage < *stage_sum) {
      (*node_table)[i].stage = dep_stage - 1;
    }
  }

  return true;
}

bool channel_analysis(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *fNode,
                      node_table_map *node_table, var_table_map *var_table,
                      int stage_sum, set<void *> var_init_new_set,
                      bool *cg_level_2) {
  //  FIXME:
  //  Access to a variable which is connected to AXI bus can only appear in
  //  one pipeline stage
  for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
    int appear = 0;
    for (size_t i = 0; i < node_table->size(); i++) {
      CGraphNode node_i = (*node_table)[i];
      if (node_i.port.find(v_it->first) != node_i.port.end()) {
        appear++;
        v_it->second.stage_appear[node_i.stage] = 1;
      }
    }
  }

  check_bus_access(node_table, cg_level_2);

  int t = 1;
  while (t != 0) {
    t = 0;
    //  Set first and last appear stage of each variable
    for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
      //  if(!v_it) continue;
      v_it->second.first_appear = stage_sum;
      v_it->second.last_appear = 0;
      if (v_it->second.stage_appear.empty()) {
#if PROJDEBUG
        cout << "The variable is declared but never used :" << v_it->first
             << endl;
#endif
        //        v_it->second.first_appear = 0;
        //    v_it->second.last_appear = 0;
        var_table->erase(v_it->first);
        t = 1;
        break;
      }
      for (map<int, int>::iterator s_it = v_it->second.stage_appear.begin();
           s_it != v_it->second.stage_appear.end(); s_it++) {
        if (s_it->first < v_it->second.first_appear) {
          v_it->second.first_appear = s_it->first;
        }

        if (s_it->first > v_it->second.last_appear) {
          v_it->second.last_appear = s_it->first;
        }
      }

#if PROJDEBUG
      cout << "First appear of '" << v_it->first << "' is in stage "
           << v_it->second.first_appear << ", last appear of '" << v_it->first
           << "' is in stage " << v_it->second.last_appear << endl;
#endif
      if (v_it->second.first_appear >= stage_sum ||
          v_it->second.first_appear < 0 ||
          v_it->second.first_appear > v_it->second.last_appear) {
        cout << "Stage appear calculation error" << endl;
        return false;
      }
    }
  }
  //  Note: set channel depth: stage number needs to be divided by depth
  for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
    v_it->second.buf_depth =
        v_it->second.last_appear - v_it->second.first_appear + 1;

    while ((stage_sum % v_it->second.buf_depth) != 0) {
      v_it->second.buf_depth++;
    }
  }

  //  Livenes analysis
  cout << "LIVENESS CHECK========\n";
  vector<void *> vecExpr;
  SgFunctionDeclaration *pMainFunc =
      isSgFunctionDefinition(fNode->ref->get_scope())->get_declaration();
  codegen->set_function_for_liveness(pMainFunc);

  void *sg_curr = bNode->ref->get_scope();
  void *sg_pre_stmt = codegen->GetPreviousStmt(sg_curr);
  vector<void *> liveIn;
  vector<void *> liveOut;
  vector<void *>::iterator it;

  codegen->get_liveness_for_loop(sg_curr, &liveIn, &liveOut);
  printf("  LIVE in : ");
  for (auto in : liveIn) {
    cout << codegen->_up(in) << ",";
  }
  printf("\n");
  printf("  LIVE out: ");
  for (auto out : liveOut) {
    cout << codegen->_up(out) << ",";
  }
  printf(" \n");

  //  For pre initial,
  //  i=0->in liveness, i=1->out liveness
  for (int i = 0; i < 2; i++) {
    for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
      if (var_init_new_set.count(v_it->second.var) > 0) {
        v_it->second.do_init[i] = false;
        continue;
      }

      bool find_live = false;
      if (i == 0) {
        for (it = liveIn.begin(); it != liveIn.end(); it++) {
          if (*it == v_it->second.var) {
            find_live = true;
            break;
          }
        }
      } else {
        for (it = liveOut.begin(); it != liveOut.end(); it++) {
          if (*it == v_it->second.var) {
            find_live = true;
            break;
          }
        }
      }
      if (codegen->IsArgumentInitName(v_it->second.var) != 0) {
        find_live = true;
      }

      v_it->second.do_init[i] = find_live;
    }
  }
  //  Get defuse of the variable before CGPIP
  cout << "DEFUSE CHECK========\n";

  for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
    CVarNode *arr_node = &v_it->second;
    if (codegen->IsScalarType(arr_node->var) != 0) {
      arr_node->do_init[LIVENESS_IN] = false;
      if (arr_node->w_type == 0)
        arr_node->do_init[LIVENESS_OUT] = false;
      continue;
    }

    vector<void *> vec_defs =
        codegen->GetVarDefbyPosition(arr_node->var, sg_pre_stmt);
    if (vec_defs.size() == 1) {
      void *sg_def = vec_defs[0];
      if (codegen->IsInitName(sg_def) != 0 &&
          codegen->IsArgumentInitName(sg_def) == 0) {
        void *initor = codegen->GetInitializer(sg_def);
        if (initor == nullptr) {
          arr_node->do_init[LIVENESS_IN] = false;
          cout << "DEF HERE: " << codegen->_p(sg_def)
               << "Initializer is unnecessary\n";
        }
      }
    }
  }
  return true;
}

bool code_generation(CSageCodeGen *codegen, CMarsIr *mars_ir,
                     CMarsIrV2 *mars_ir_v2, CMirNode *bNode, CMirNode *fNode,
                     node_table_map *node_table, int stage_sum,
                     string node_num_str, var_table_map *var_table,
                     set<void *> var_init_new_set, const bool &cg_level_2) {
  int i;
  int j;

  //  Note: AST nodes containers
  map<string, map<string, void *>> arg_map_kernel;
  map<string, SgInitializedName *> name_map_org;
  map<string, void *> name_map_kernel_L1;
  map<string, SgInitializedName *> name_move_var;
  map<string, SgVariableDeclaration *> name_move_decl;

  map<string, void *> func_map_kernel;
  map<string, void *> all_func_map;
  map<void *, void *> all_func_call_map;
  cout << "Code transformation started." << endl;
  SgBasicBlock *body = bNode->ref;
  SgForStatement *targetLoop = isSgForStatement(bNode->ref->get_scope());
  map<void *, vector<void *>> existing_table = mars_ir->get_partition_table();
  map<void *, vector<void *>> gen_table;
  SgInitializedName *curr_iter = isSgInitializedName(
      static_cast<SgNode *>(codegen->GetLoopIterator(targetLoop)));

  //  ////////////////////////////////////////////////  /
  //  ZP: 20151124
  //  Get the printing information before transformation
  //  string critical_msg;
  string sSourceFile = getUserCodeFileLocation(codegen, targetLoop);
  string str_target = codegen->get_internal_loop_label(targetLoop);
  void *iter = codegen->GetLoopIterator(targetLoop);
  if (iter) {
    str_target = codegen->UnparseToString(iter);
  }
  int num_nodes = node_table->size();
  string loop_info = "'" + str_target + "'";
  loop_info += " (" + sSourceFile + ")";
  string node_info = my_itoa(num_nodes);
  string stage_info = my_itoa(stage_sum);
  //  ////////////////////////////////////////////////  /

  string kernel_str = "mars_kernel_";
  kernel_str += node_num_str;
  SgName name1(kernel_str);

  SgStatementPtrList &stmt_lst = body->getStatementList();
  SgStatement *local_scope = nullptr;
  for (size_t i = 0; i < stmt_lst.size(); i++) {
    if (isSgPragmaDeclaration(stmt_lst[i]) != nullptr) {
      continue;
    }
    if (isSgVariableDeclaration(stmt_lst[i]) != nullptr) {
      local_scope = stmt_lst[i];
    } else {
      break;
    }
  }

  SgFunctionDeclaration *curr_decl =
      isSgFunctionDefinition(fNode->ref->get_scope())->get_declaration();
  SgScopeStatement *generalScope = fNode->ref->get_scope()->get_scope();

  //  Build CGPIP for statement
  void *targetLoop_scope = targetLoop;
  if (codegen->IsLabelStatement(codegen->GetParent(targetLoop))) {
    targetLoop_scope = codegen->GetParent(targetLoop);
  }

  //  //////////////////////////////////////////////  /
  //  Note: Build initialization function
  build_init_function(codegen, bNode, var_table, &all_func_map, kernel_str);

  //  Analyze incremental expression
  SgExpression *incr_exp = targetLoop->get_increment();
  int64_t incr = 0;
  int ret = codegen->GetLoopStepValueFromExpr(incr_exp, &incr);
  if (ret == 0) {
    return false;
  }

  //  Analyze test expression
  SgExpression *cond =
      isSgExprStatement(targetLoop->get_test())->get_expression();
  ROSE_ASSERT(cond);
  //  Initializer of a for loop is SgStatementPtrList
  SgExpression *old_test = isSgBinaryOp(cond)->get_rhs_operand();

  CMarsRangeExpr mr = CMarsVariable(targetLoop, codegen).get_range();
  CMarsExpression me_lb, me_ub;
  if (mr.get_simple_bound(&me_lb, &me_ub) == 0) {
    cout << "Cannot get a loop range1" << endl;
    return false;
  }
  void *old_lb = codegen->CopyExp(me_lb.get_expr_from_coeff());
  void *old_ub = codegen->CopyExp(me_ub.get_expr_from_coeff());
  void *new_test = codegen->CreateExp(V_SgAddOp, codegen->CopyExp(old_test),
                                      codegen->CreateConst(stage_sum - 1));
  codegen->ReplaceExp(old_test, new_test);

  //  Variable declarations
  void *v_count = codegen->CreateVariableDecl(
      "int", "mars_count_" + node_num_str, codegen->CreateConst(0), body);
  codegen->InsertStmt_v1(targetLoop_scope, v_count);
  cout << "vcount: " << codegen->_up(v_count) << endl;
  //  Note:Build kernel declaration
  SgFunctionParameterList *kernel_parameters =
      static_cast<SgFunctionParameterList *>(
          codegen->buildFunctionParameterList());

  SgInitializedName *arg1 = static_cast<SgInitializedName *>(
      codegen->CreateVariable("int", "mars_" + codegen->_up(curr_iter)));
  codegen->appendArg(kernel_parameters, arg1);
  name_map_kernel_L1["mars_" + codegen->_up(curr_iter)] = arg1;

  SgInitializedName *arg2_0 = static_cast<SgInitializedName *>(
      codegen->CreateVariable("int", "mars_init"));
  SgInitializedName *arg2 = static_cast<SgInitializedName *>(
      codegen->CreateVariable("int", "mars_cond"));
  codegen->appendArg(kernel_parameters, arg2_0);
  codegen->appendArg(kernel_parameters, arg2);
  name_map_kernel_L1["mars_init"] = arg2_0;
  name_map_kernel_L1["mars_cond"] = arg2;

  //  Note: Build kernel function declaration
  for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
    void *sg_type = codegen->GetTypebyVar(v_it->second.var);
    if (v_it->second.is_port) {
      SgInitializedName *arg_channel_0 = nullptr;
      if (v_it->second.self_w_type != 0) {
        arg_channel_0 = codegen->CreateVariable_v1(
            SgName(v_it->first), codegen->CreatePointerType(sg_type));
      } else {
        arg_channel_0 =
            codegen->CreateVariable_v1(SgName(v_it->first), sg_type);
      }

      codegen->appendArg(kernel_parameters, arg_channel_0);
      name_map_kernel_L1[v_it->first] = arg_channel_0;
      continue;
    }

    for (i = v_it->second.first_appear; i <= v_it->second.last_appear; i++) {
      string channel_str = my_itoa(i);
      channel_str = "mars_" + v_it->first + "_" + channel_str;

      SgInitializedName *arg_channel_1 = nullptr;
      if (v_it->second.self_w_type != 0) {
        arg_channel_1 = codegen->CreateVariable_v1(
            SgName(channel_str), codegen->CreatePointerType(sg_type));
      } else {
        arg_channel_1 =
            codegen->CreateVariable_v1(SgName(channel_str), sg_type);
      }
      cout << channel_str << endl;
      codegen->appendArg(kernel_parameters, arg_channel_1);
      name_map_kernel_L1[channel_str] = arg_channel_1;
    }
  }
  SgFunctionDeclaration *kernel_decl =
      static_cast<SgFunctionDeclaration *>(codegen->CreateFuncDecl(
          "void", name1, kernel_parameters, generalScope, true, body));

  all_func_map[name1.str()] = kernel_decl;
  kernel_decl->set_definingDeclaration(kernel_decl);
  arg_map_kernel["kernel"] = name_map_kernel_L1;
  func_map_kernel["kernel"] = kernel_decl;
  codegen->InsertStmt_v1(curr_decl, kernel_decl, true);

  if (cg_level_2) {
    for (i = 0; i < 2; i++) {
      map<string, void *> name_map_kernel_L2;
      string str_L2;
      if (i == 0) {
        str_L2 = "bus";
      } else {
        str_L2 = "comp";
      }
      string kernel_str_L2 = kernel_str + "_" + str_L2;
      SgName name1_L2(kernel_str_L2);
      SgFunctionParameterList *kernel_parameters_L2 =
          static_cast<SgFunctionParameterList *>(
              codegen->CopyStmt(kernel_parameters));
      SgFunctionDeclaration *kernel_decl_L2 =
          static_cast<SgFunctionDeclaration *>(
              codegen->CreateFuncDecl("void", name1_L2, kernel_parameters_L2,
                                      generalScope, true, body));

      all_func_map[name1_L2.str()] = kernel_decl_L2;
      kernel_decl_L2->set_definingDeclaration(kernel_decl_L2);
      codegen->InsertStmt_v1(kernel_decl, kernel_decl_L2, true);

      SgInitializedNamePtrList &args = kernel_parameters_L2->get_args();
      for (SgInitializedNamePtrList::iterator it_begin = args.begin();
           it_begin != args.end(); ++it_begin) {
        void *initName = *it_begin;
        cout << codegen->_up(initName) << endl;
        name_map_kernel_L2[codegen->_up(initName)] = initName;
      }
      arg_map_kernel[str_L2] = name_map_kernel_L2;
      func_map_kernel[str_L2] = kernel_decl_L2;
    }
  }
  //  ///////////////////////////////////////////////  /
  //  Note: Build pipelined kernels, top pipeline control

  //  Note: Build channel pre initilization and post assignment
  for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
    if (v_it->second.is_port) {
      continue;
    }

    void *sg_type = codegen->GetTypebyVar(v_it->second.var);
    if (v_it->second.is_local) {
#if PROJDEBUG
      cout << "Locally declared variable: " << v_it->first << endl;
#endif
      void *initor = codegen->GetInitializer(v_it->second.var);
      if (name_move_decl.find(v_it->first) != name_move_decl.end()) {
        initor = nullptr;
      }

      if (initor != nullptr) {
        Rose_STL_Container<SgNode *> varList = NodeQuery::querySubTree(
            static_cast<SgNode *>(initor), V_SgVarRefExp);
        Rose_STL_Container<SgNode *>::iterator var;

        for (var = varList.begin(); var != varList.end(); var++) {
          SgExpression *ref = isSgExpression(*var);
          SgInitializedName *arr_var;
          void *var_ref;
          if (codegen->get_var_from_pntr_ref(ref, &var_ref) == 0) {
            arr_var = isSgInitializedName(static_cast<SgNode *>(
                codegen->GetVariableInitializedName(ref)));
          } else {
            arr_var = isSgInitializedName(static_cast<SgNode *>(
                codegen->GetVariableInitializedName(var_ref)));
          }
          if (arr_var == curr_iter) {
            void *new_ref =
                codegen->CreateExp(V_SgSubtractOp, codegen->CopyExp(ref),
                                   codegen->CreateConst(stage_sum - 1));
            codegen->ReplaceExp(ref, new_ref);
          }
        }
      }

      for (i = 0; i < v_it->second.buf_depth; i++) {
        string channel_str = my_itoa(i);
        channel_str = kernel_str + "_" + v_it->first + "_" + channel_str;
        SgVariableDeclaration *channel_var =
            static_cast<SgVariableDeclaration *>(codegen->CreateVariableDecl(
                sg_type, channel_str,
                initor != nullptr
                    ? static_cast<SgInitializer *>(
                          static_cast<SgExpression *>(codegen->CopyExp(initor)))
                    : nullptr,
                fNode->ref));
        SgInitializedName *channel_init = static_cast<SgInitializedName *>(
            codegen->GetVariableInitializedName(channel_var));
        name_map_org[channel_str] = channel_init;
        //  YX: 20170701 bug1168 static variable will prevent CG parallel
        //  from happening
        //  setStatic(channel_var);
        if (name_move_decl.find(v_it->first) != name_move_decl.end()) {
          codegen->InsertAfterStmt(channel_var, name_move_decl[v_it->first]);
        } else {
          codegen->InsertAfterStmt(channel_var, local_scope);
        }

        if (existing_table.find(v_it->second.var) != existing_table.end()) {
          if (gen_table.find(v_it->second.var) == gen_table.end()) {
            vector<void *> new_vars;
            new_vars.push_back(channel_init);
            gen_table[v_it->second.var] = new_vars;
          } else {
            gen_table[v_it->second.var].push_back(channel_init);
          }
        }
      }
      continue;
    }

    if (v_it->second.buf_depth > 1) {
      // Yuxin: Nov/14/2019
      // to use the original variable's initializer is incorrect
      for (i = 0; i < v_it->second.buf_depth; i++) {
        string channel_str = my_itoa(i);
        channel_str = kernel_str + "_" + v_it->first + "_" + channel_str;
        // Yuxin: Nov/14/2019
        // to use the original variable's initializer is incorrect
        void *new_initor = nullptr;
        if (codegen->IsScalarType(v_it->second.var) != 0) {
          new_initor = codegen->CreateVariableRef(v_it->second.var);
        }
        SgVariableDeclaration *channel_var =
            static_cast<SgVariableDeclaration *>(codegen->CreateVariableDecl(
                sg_type, channel_str, new_initor, fNode->ref));
        SgInitializedName *channel_init = static_cast<SgInitializedName *>(
            codegen->GetVariableInitializedName(channel_var));
        name_map_org[channel_str] = channel_init;
        codegen->InsertStmt_v1(targetLoop_scope, channel_var);

        //  YX: 20170701 bug1168 static variable will prevent CG parallel
        //  from happening
        //  setStatic(channel_var);

        if (existing_table.find(v_it->second.var) != existing_table.end()) {
          if (gen_table.find(v_it->second.var) == gen_table.end()) {
            vector<void *> new_vars;
            new_vars.push_back(channel_init);
            gen_table[v_it->second.var] = new_vars;
          } else {
            gen_table[v_it->second.var].push_back(channel_init);
          }
        }

        if (v_it->second.do_init[LIVENESS_IN]) {
          vector<void *> param2_0_list;
          if (v_it->second.self_w_type != 0) {
            param2_0_list.push_back(codegen->CreateExp(
                V_SgAddressOfOp, codegen->CreateVariableRef(channel_var)));
            param2_0_list.push_back(codegen->CreateExp(
                V_SgAddressOfOp, codegen->CreateVariableRef(v_it->second.var)));
          } else {
            param2_0_list.push_back(codegen->CreateVariableRef(channel_var));
            param2_0_list.push_back(
                codegen->CreateVariableRef(v_it->second.var));
          }
          void *parameters2_0 = codegen->CreateExpList(param2_0_list);

          string func_name = kernel_str + "_assign_" + v_it->first;
          void *init_func = all_func_map[func_name];
          assert(init_func);
          void *initial_call =
              codegen->CreateFuncCallStmt(init_func, parameters2_0, targetLoop);
          codegen->InsertAfterStmt(initial_call, channel_var);
          all_func_call_map[init_func] = initial_call;
        }
        //  Only assign back to the original buffer from the
        //  first channel
        if (i == 0 && v_it->second.w_type == 1 &&
            v_it->second.do_init[LIVENESS_OUT]) {
          vector<void *> param2_1_list;
          if (v_it->second.self_w_type != 0) {
            param2_1_list.push_back(codegen->CreateExp(
                V_SgAddressOfOp, codegen->CreateVariableRef(v_it->second.var)));
            param2_1_list.push_back(codegen->CreateExp(
                V_SgAddressOfOp, codegen->CreateVariableRef(channel_var)));
          } else {
            param2_1_list.push_back(
                codegen->CreateVariableRef(v_it->second.var));
            param2_1_list.push_back(codegen->CreateVariableRef(channel_var));
          }
          void *parameters2_1 = codegen->CreateExpList(param2_1_list);
          void *assign_func =
              all_func_map[kernel_str + "_assign_" + v_it->first];
          assert(assign_func);
          void *output_call = codegen->CreateFuncCallStmt(
              assign_func, parameters2_1, targetLoop);
          codegen->InsertAfterStmt(output_call, targetLoop);
          all_func_call_map[assign_func] = output_call;
          //            setFuncDeclUserCodeScopeId(codegen,
          //  assign_func, output_call);
        }
      }
    }
  }

  pragma_propagate(codegen, mars_ir, existing_table, gen_table,
                   var_init_new_set);

  SgStatement *false_stmt = nullptr;
  SgIfStmt *if_pipe_stmt = nullptr;
  void *call_stmt_bind = nullptr;
  void *func_decl_bind = nullptr;

  for (i = stage_sum - 1; i >= 0; i--) {
    void *mars_count = codegen->CreateVariableRef(v_count);
    void *if_cond_stmt = codegen->CreateStmt(
        V_SgExprStatement, codegen->CreateExp(V_SgEqualityOp, mars_count,
                                              codegen->CreateConst(i)));
    void *parameters2 = codegen->buildExprListExp();
    void *arg_exp = codegen->CreateVariableRef(curr_iter);
    codegen->appendExp(parameters2, arg_exp);
    codegen->appendExp(parameters2, codegen->CopyExp(old_lb));
    codegen->appendExp(parameters2, codegen->CopyExp(old_ub));

    //  Note:Arguments of pipelined kernel function call

    for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
      if (v_it->second.is_port || v_it->second.buf_depth == 1) {
        void *c_arg = nullptr;
        if (v_it->second.self_w_type != 0) {
          c_arg = codegen->CreateExp(
              V_SgAddressOfOp, codegen->CreateVariableRef(v_it->second.var));
        } else {
          c_arg = codegen->CreateVariableRef(v_it->second.var);
        }
        codegen->appendExp(parameters2, c_arg);
        continue;
      }
      int range = v_it->second.last_appear - v_it->second.first_appear;
      int count = 0;
      for (j = -i; j < v_it->second.buf_depth - i; j++) {
        if (count > range) {
          break;
        }

        int c_num = (j + stage_sum) % v_it->second.buf_depth;
        string c_str = my_itoa(c_num);
        c_str = kernel_str + "_" + v_it->first + "_" + c_str;

        void *c_arg = nullptr;
        if (v_it->second.self_w_type != 0) {
          c_arg = codegen->CreateExp(
              V_SgAddressOfOp, codegen->CreateVariableRef(name_map_org[c_str]));
        } else {
          c_arg = codegen->CreateVariableRef(name_map_org[c_str]);
        }

        codegen->appendExp(parameters2, c_arg);
        count++;
      }
    }
    void *kernel_func = all_func_map[name1.str()];
    assert(kernel_func);
    SgStatement *call_body = static_cast<SgStatement *>(
        codegen->CreateFuncCallStmt(kernel_func, parameters2, body));
    call_stmt_bind = call_body;
    func_decl_bind = kernel_func;

    if (stage_sum > 1) {
      if (i == stage_sum - 1) {
        false_stmt = call_body;
      } else {
        SgIfStmt *if_stmt = static_cast<SgIfStmt *>(
            codegen->CreateIfStmt(if_cond_stmt, call_body, false_stmt));
        if_pipe_stmt = if_stmt;
        false_stmt = if_stmt;
      }
    }
  }
  codegen->AppendChild_v1(if_pipe_stmt, body);
  all_func_call_map[func_decl_bind] = call_stmt_bind;
  //    setFuncDeclUserCodeScopeId(codegen, func_decl_bind, call_stmt_bind);

  void *mars_count_0 = codegen->CreateVariableRef(v_count);
  void *count_incr = codegen->CreateStmt(
      V_SgExprStatement, codegen->CreateExp(V_SgPlusPlusOp, mars_count_0,
                                            nullptr, SgUnaryOp::postfix));
  codegen->AppendChild_v1(count_incr, body);
  void *call_body_1 =
      codegen->CreateStmt(V_SgAssignStatement, codegen->CopyExp(mars_count_0),
                          codegen->CreateConst(0));
  void *count_if_stmt = codegen->CreateIfStmt(
      codegen->CreateStmt(V_SgExprStatement,
                          codegen->CreateExp(V_SgEqualityOp,
                                             codegen->CopyExp(mars_count_0),
                                             codegen->CreateConst(stage_sum))),
      call_body_1, nullptr);
  codegen->AppendChild_v1(count_if_stmt, body);

  //  //////////////////////////////////////////////  /
  //  Note: Build node functions
  build_node_functions(codegen, bNode, var_table, &func_map_kernel,
                       &all_func_map, &all_func_call_map, &arg_map_kernel,
                       node_table, stage_sum, incr, kernel_str);

  if (func_map_kernel.size() == 3) {
    void *top_kernel_body = codegen->GetFuncBody(func_map_kernel["kernel"]);
    void *parameters_L2 = codegen->buildExprListExp();
    SgInitializedNamePtrList &args = kernel_parameters->get_args();
    for (SgInitializedNamePtrList::iterator it_begin = args.begin();
         it_begin != args.end(); ++it_begin) {
      SgInitializedName *initName = *it_begin;
      SgExpression *arg =
          static_cast<SgVarRefExp *>(codegen->CreateVariableRef(initName));
      codegen->appendExp(parameters_L2, arg);
    }

    cout << "here5\n";
    SgName name1_L2(kernel_str + "_bus");
    SgName name2_L2(kernel_str + "_comp");
    void *bus_func = all_func_map[name1_L2.str()];
    void *comp_func = all_func_map[name2_L2.str()];

    if (bus_func) {
      void *node_call_body1 =
          codegen->CreateFuncCallStmt(bus_func, parameters_L2, body);
      codegen->AppendChild_v1(node_call_body1, top_kernel_body);
      all_func_call_map[bus_func] = node_call_body1;
    }
    if (comp_func) {
      void *node_call_body2 = codegen->CreateFuncCallStmt(
          comp_func, codegen->CopyExp(parameters_L2), body);
      codegen->AppendChild_v1(node_call_body2, top_kernel_body);
      all_func_call_map[comp_func] = node_call_body2;
    }
    string pragma_depend_str = std::string(HLS_PRAGMA) + " INLINE OFF";
    void *new_depend_pragma =
        codegen->CreatePragma(pragma_depend_str, top_kernel_body);
    codegen->PrependChild_v1(new_depend_pragma, top_kernel_body);
  }

  for (auto func_bind : all_func_call_map) {
    setFuncDeclUserCodeScopeId(codegen, func_bind.first, func_bind.second);
  }

  dump_critical_message(CGPIP_INFO_1(loop_info, node_info, stage_info));
  map<string, string> msg_map;
  msg_map["coarse_grained_pipeline"] = "on";
  insertMessage(codegen, targetLoop, msg_map);
  setQoRMergeMode(codegen, targetLoop, "major");

  return true;
}

bool isLoopIterator(CSageCodeGen *codegen, void *loop_scope, void *arr_var,
                    void *ref) {
  void *sg_loop = codegen->GetEnclosingNode("ForLoop", ref);
  void *curr_loop = codegen->GetEnclosingNode("ForLoop", loop_scope);
  while (sg_loop != nullptr && codegen->IsInScope(sg_loop, curr_loop)) {
    void *iter = codegen->GetLoopIterator(sg_loop);
    if (iter == arr_var) {
      return true;
    }
    sg_loop = codegen->GetEnclosingNode("ForLoop", sg_loop);
  }
  return false;
}

bool preprocessing(CSageCodeGen *codegen, CMarsIr *mars_ir, CMirNode *bNode,
                   const vector<SgVariableDeclaration *> &var_declare_vec,
                   vector<void *> *var_decl_old, vector<void *> *var_decl_new,
                   set<void *> *var_init_new_set, bool *changed) {
  void *sg_loop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
  void *sg_func = codegen->GetEnclosingNode("Function", bNode->ref);
  void *sg_scope = codegen->GetEnclosingNode("BasicBlock", sg_loop);

  for (auto it : var_declare_vec) {
    SgVariableDeclaration *init_decl_stmt = isSgVariableDeclaration(it);
    void *old_init = codegen->GetVariableInitializedName(init_decl_stmt);
    void *sg_type = codegen->GetTypebyVar(old_init);
    void *initer = codegen->GetInitializer(old_init);
    //  Whether if the declaration can be moved outside the loop
    bool is_movable = true;
    //  Whehter if the initializer and the declaration can be splitted
    bool is_splittable = true;

    //  Aggregate initializer is unsplittable with the declaration
    {
      SgAggregateInitializer *aggr_init =
          isSgAggregateInitializer(static_cast<SgNode *>(initer));
      if (aggr_init != nullptr) {
        is_movable = false;
        is_splittable = false;
      }
    }

    //  Const modifier is unsplittable with the declaration
    if (codegen->IsModifierType(sg_type) != 0) {
      SgModifierType *sg_m_type =
          isSgModifierType(static_cast<SgNode *>(sg_type));
      SgTypeModifier new_modifier = sg_m_type->get_typeModifier();
      SgConstVolatileModifier &cv = new_modifier.get_constVolatileModifier();
      if (cv.isConst()) {
        is_movable = true;
        is_splittable = false;
        cout << "[MARS-COARSE-WARN] unsplittable type: " << codegen->_p(sg_type)
             << endl;
      }
    }
    //  To test if the assign initializer is movable
    if (initer != nullptr) {
      if (codegen->is_movable_test(initer, sg_loop, old_init) == 0) {
        cout << "[MARS-COARSE-WARN] Unremovable initializer: "
             << codegen->_p(old_init)
             << ". Try split the declaration and initializer. \n";
        is_movable = false;
      }
      //  Aggregate initializer cannot be splitted from the declaration
      if (!is_movable && !is_splittable) {
        cout << "[MARS-COARSE-WARN] The variable declaration cannot be"
                " splitted. Quit CGPIP. \n";
        return false;
      }
    }

    *changed = true;
    string var_name = codegen->GetVariableName(old_init);
    codegen->get_valid_local_name(sg_func, &var_name);
    void *new_decl = nullptr;
    if (codegen->IsConstructorInitializer(initer)) {
      if (codegen->GetFuncCallParamNum(initer) == 0)
        initer = nullptr;
    }
    if ((initer != nullptr) && !is_splittable) {
      new_decl = codegen->CreateVariableDecl(
          sg_type, var_name, codegen->CopyExp(initer), sg_scope, old_init);
    } else {
      new_decl = codegen->CreateVariableDecl(sg_type, var_name, nullptr,
                                             sg_scope, old_init);
    }

    codegen->InsertStmt(new_decl, sg_loop);
    void *new_init = codegen->GetVariableInitializedName(new_decl);
    cout << "Locally new var: " << codegen->_up(new_decl) << endl;
    codegen->replace_variable_references_in_scope(old_init, new_init,
                                                  bNode->ref);
    var_decl_new->push_back(new_decl);
    var_init_new_set->insert(new_init);

    if ((initer != nullptr) && is_splittable) {
      (static_cast<SgInitializedName *>(new_init)->set_initializer(nullptr));
      void *new_assign_stmt = codegen->CreateStmt(
          V_SgAssignStatement, codegen->CreateVariableRef(new_init),
          codegen->CopyExp(codegen->GetInitializerOperand(initer)));
      codegen->ReplaceStmt(init_decl_stmt, new_assign_stmt);
      cout << "Split assign and decl: " << codegen->_p(new_assign_stmt) << endl;
      var_decl_old->push_back(new_assign_stmt);
    } else {
      var_decl_old->push_back(init_decl_stmt);
    }
  }
  return true;
}

void postprocessing_fail(CSageCodeGen *codegen, CMarsIr *mars_ir,
                         CMirNode *bNode, const vector<void *> &var_decl_old,
                         const vector<void *> &var_decl_new) {
  for (size_t j = 0; j < var_decl_old.size(); j++) {
    void *old_decl = var_decl_old[j];
    if (codegen->IsVariableDecl(old_decl) == 0) {
      continue;
    }
    void *new_decl = var_decl_new[j];
    void *old_init = codegen->GetVariableInitializedName(old_decl);
    void *new_init = codegen->GetVariableInitializedName(new_decl);
    codegen->replace_variable_references_in_scope(new_init, old_init,
                                                  bNode->ref);

    codegen->RemoveStmt(new_decl);
  }
}

void postprocessing_success(CSageCodeGen *codegen, CMarsIr *mars_ir,
                            CMirNode *bNode, const vector<void *> &var_decl_old,
                            const vector<void *> &var_decl_new) {
  map<void *, vector<void *>> existing_table = mars_ir->get_partition_table();

  for (size_t j = 0; j < var_decl_old.size(); j++) {
    void *old_decl = var_decl_old[j];
    if (codegen->IsVariableDecl(old_decl) == 0) {
      continue;
    }
    void *new_decl = var_decl_new[j];
    void *old_init = codegen->GetVariableInitializedName(old_decl);
    void *new_init = codegen->GetVariableInitializedName(new_decl);

    //  Copy pragma table
    vector<void *> pragma_vec;
    vector<void *> new_pragma_vec;

    codegen->PropagatePragma(old_init, bNode->ref, codegen->_up(new_init), true,
                             &new_pragma_vec);
    mars_ir->set_partition_table(new_init, new_pragma_vec);
    codegen->RemoveStmt(old_decl);
  }
}

bool test_range_legality(CSageCodeGen *codegen, CMarsIr *mars_ir, void *sg_loop,
                         node_table_map *node_table, void *sg_scope_w,
                         void *sg_scope_r, void *sg_array, int num_w, int num_r,
                         string *msg, set<void *> var_init_new_set,
                         vector<size_t> *lb_vec, vector<size_t> *ub_vec,
                         bool *bound_check) {
#if PROJDEBUG
  cout << "Analyze range legality: " << codegen->_p(sg_array) << endl;
  cout << "write: \n" << codegen->_p(sg_scope_w) << endl;
  cout << "read: \n" << codegen->_p(sg_scope_r) << endl;
#endif

  //  Yuxin: Jul 19 2018
  //  If the buffer is declared inside the loop
  //  it is supposed to be fully initilialized
  //  skip the coverage test
  if (var_init_new_set.count(sg_array) > 0) {
    return true;
  }

  ArrayRangeAnalysis mar_w(sg_array, codegen, sg_scope_w, sg_scope_w, false,
                           false);
  ArrayRangeAnalysis mar_r(sg_array, codegen, sg_scope_r, sg_scope_r, false,
                           false);
  vector<CMarsRangeExpr> vec_mr_w = mar_w.GetRangeExprWrite();
  CMarsRangeExpr flatten_mr_w = mar_w.GetFlattenRangeExprWrite();
  vector<CMarsRangeExpr> vec_mr_r = mar_r.GetRangeExprRead();
  CMarsRangeExpr flatten_mr_r = mar_r.GetFlattenRangeExprRead();
  bool legal_tag = false;

  legal_tag |=
      check_range_flatten(codegen, mars_ir, sg_loop, node_table, flatten_mr_r,
                          flatten_mr_w, sg_array, num_w, num_r, msg);

  legal_tag |= check_range_multiple(codegen, mars_ir, sg_loop, node_table,
                                    vec_mr_r, vec_mr_w, sg_array, num_w, num_r,
                                    msg, lb_vec, ub_vec, bound_check);

  return legal_tag;
}

bool check_range_flatten(CSageCodeGen *codegen, CMarsIr *mars_ir, void *sg_loop,
                         node_table_map *node_table,
                         const CMarsRangeExpr &flatten_mr_r,
                         const CMarsRangeExpr &flatten_mr_w, void *sg_array,
                         int num_w, int num_r, string *msg) {
#if PROJDEBUG
  cout << "Check flattened range on : " << codegen->_p(sg_array) << endl;
#endif
  CMarsRangeExpr mr_w = flatten_mr_w;
  CMarsRangeExpr mr_r = flatten_mr_r;

#if PROJDEBUG
  cout << "flatten write range: " << mr_w.print() << endl;
  cout << "flatten write exact flag :" << mr_w.is_exact() << endl;
  cout << "flatten read range:" << mr_r.print() << endl;
#endif

  ArrayRangeAnalysis mr_loop(sg_array, codegen, sg_loop, sg_loop, false, false);
  if (mr_loop.has_write() != 0) {
    CMarsRangeExpr mr_w_loop = mr_loop.GetFlattenRangeExprWrite();
    CMarsRangeExpr merged_range;
    if ((mr_w_loop.is_exact() != 0) && (mr_r.is_exact() != 0)) {
      CMarsRangeExpr mr_intersect = mr_w_loop.Intersect(mr_r);
      if (mr_intersect.is_empty() != 0) {
        cout << "[report] Read and write are not intersect in CGPIP scope\n";
        return true;
      }
    }
  }
  bool merge_tag = false;
  CMarsRangeExpr mr_w_merged = mr_w;
  if (!union_access_range(codegen, node_table, &mr_w_merged, &merge_tag,
                          sg_array, num_w, num_r, msg)) {
    cout << "[report] union range analysis false \n";
    return false;
  }
  // Yuxin: Jan/08/2020
  // If cannot get a valid range, quit coarse_pipeline
  if ((mr_w_merged.has_lb() == 0) || (mr_w_merged.has_ub() == 0) ||
      (mr_r.has_lb() == 0) || (mr_r.has_ub() == 0)) {
    cout << "NOTE: cannot get range\n" << endl;
    return false;
  }

  CMarsRangeExpr merged_range;
  if (mr_w_merged.is_cover(mr_r, &merged_range) == 0) {
    cout << "[report] Read is not covered by write in CGPIP scope on var "
         << codegen->_p(sg_array) << endl;
    return false;
  }

  return true;
}

bool check_range_multiple(CSageCodeGen *codegen, CMarsIr *mars_ir,
                          void *sg_loop, node_table_map *node_table,
                          const vector<CMarsRangeExpr> &vec_mr_r,
                          const vector<CMarsRangeExpr> &vec_mr_w,
                          void *sg_array, int num_w, int num_r, string *msg,
                          vector<size_t> *lb_vec, vector<size_t> *ub_vec,
                          bool *bound_check) {
#if PROJDEBUG
  cout << "Check multiple range on : " << codegen->_p(sg_array) << endl;
#endif
  string w_msg;
  string r_msg;
  if (vec_mr_w.size() != vec_mr_r.size()) {
    cout << "[Legality check:] Incorrect dimension analysis\n";
    return false;
  }

  for (size_t k = 0; k < vec_mr_w.size(); k++) {
    bool Check = false;
    CMarsRangeExpr mr_w = vec_mr_w[k];
    CMarsRangeExpr mr_r = vec_mr_r[k];
    r_msg += mr_r.print();
    bool merge_tag = false;
    CMarsRangeExpr mr_w_merged = mr_w;
    // Yuxin: Jan/08/2020
    // If cannot get a valid range, quit coarse_pipeline
    if ((mr_w.has_lb() == 0) || (mr_w.has_ub() == 0) || (mr_r.has_lb() == 0) ||
        (mr_r.has_ub() == 0)) {
      cout << "NOTE: cannot get range\n" << endl;
      return false;
    }
    if (codegen->IsScalarType(sg_array) == 0) {
      *bound_check &=
          check_range_bound(codegen, mars_ir, &mr_w_merged, lb_vec, ub_vec);
    }
    if (!union_access_range_multiple(codegen, node_table, &mr_w_merged,
                                     &merge_tag, sg_array, num_w, num_r, msg,
                                     k)) {
      return false;
    }
#if PROJDEBUG
    cout << "multi write range: " << mr_w.print() << endl;
    cout << "multi union range: " << mr_w_merged.print() << endl;
    cout << "multi write exact flag :" << mr_w.is_exact() << endl;
    cout << "multi read range:" << mr_r.print() << endl;
#endif

    CMarsRangeExpr merged_range;
    Check |= mr_w_merged.is_cover(mr_r, &merged_range);
    w_msg += mr_w_merged.print();

    if (!Check) {
      cout << "[report] Multi read is not covered by write in "
              "CGPIP scope on var "
           << codegen->_p(sg_array) << endl;
      return false;
    }
  }

  return true;
}

bool union_access_range(CSageCodeGen *codegen, node_table_map *node_table,
                        CMarsRangeExpr *mr_merged, bool *merge_tag,
                        void *sg_array, int num_w, int num_r, string *msg) {
#if PROJDEBUG
  cout << "Access union range analysis: " << codegen->_p(sg_array) << endl;
#endif
  for (int i = 0; i < num_r; i++) {
    CGraphNode write_node = (*node_table)[i];
    if (write_node.type == 5) {
      continue;
    }
    if (write_node.size() > 1) {
      *msg = "multiple write is not supported";
      return false;
    }

    void *sg_scope_w = write_node[0];
    ArrayRangeAnalysis mar_w(sg_array, codegen, sg_scope_w, sg_scope_w, false,
                             false);
    CMarsRangeExpr flatten_mr_w;

    if (mar_w.has_write() != 0) {
      flatten_mr_w = mar_w.GetFlattenRangeExprWrite();
      *mr_merged = mr_merged->Union(flatten_mr_w);
      *merge_tag = true;
      //    cout <<"write range: " << flatten_mr_w.print()<<endl;
      //    cout <<"write exact flag :" <<flatten_mr_w.is_exact()<<endl;
#if PROJDEBUG
      cout << "merge write range: " << mr_merged->print() << endl;
      cout << "merge write exact flag :" << mr_merged->is_exact() << endl;
#endif
    }
  }
  return true;
}

bool union_access_range_multiple(CSageCodeGen *codegen,
                                 node_table_map *node_table,
                                 CMarsRangeExpr *mr_merged, bool *merge_tag,
                                 void *sg_array, int num_w, int num_r,
                                 string *msg, int dim) {
#if PROJDEBUG
  cout << "Access union range analysis: " << codegen->_p(sg_array) << endl;
#endif
  for (int i = 0; i < num_r; i++) {
    CGraphNode write_node = (*node_table)[i];
    if (write_node.type == 5) {
      continue;
    }
    if (write_node.size() > 1) {
      *msg = "multiple write is not supported";
      return false;
    }

    void *sg_scope_w = write_node[0];
    ArrayRangeAnalysis mar_w(sg_array, codegen, sg_scope_w, sg_scope_w, false,
                             false);
    vector<CMarsRangeExpr> vec_mr_w;

    if (mar_w.has_write() != 0) {
      vec_mr_w = mar_w.GetRangeExprWrite();
      *mr_merged = mr_merged->Union(vec_mr_w[dim]);
      *merge_tag = true;
#if PROJDEBUG
      cout << "merge write range: " << mr_merged->print() << endl;
      cout << "merge write exact flag :" << mr_merged->is_exact() << endl;
#endif
    }
  }
  return true;
}

void reportFeedback(CSageCodeGen *codegen, CMirNode *lnode,
                    vector<void *> arr_report, int j, int i) {
  void *scope = lnode->ref;
  void *sg_loop = codegen->GetParent(scope);
  assert(codegen->IsForStatement(sg_loop));
  string sFile = getUserCodeFileLocation(codegen, sg_loop);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string str_loop = codegen->UnparseToString(codegen->GetLoopIterator(sg_loop));

  bool first = true;
  string var_info = " ";
  for (auto it : arr_report) {
    void *var = it;
    string var_name = codegen->_up(var);
    if (!first) {
      var_info += ", ";
    }
    var_info += "variable '" + var_name + "' " +
                getUserCodeFileLocation(codegen, var, true);
    first = false;
  }
  string loop_info = "'" + str_loop + "' (" + sFile + ")";
  string msg_L2 = "Data dependency across loop iterations on " + var_info +
                  ". Try to move dependency into the same loop iteration.";
  dump_critical_message(CGPIP_WARNING_1(loop_info, var_info), 0, codegen,
                        sg_loop);

  map<string, string> msg_map;
  msg_map["coarse_grained_pipeline"] = "off";
  insertMessage(codegen, sg_loop, msg_map);
}

void reportUnsupportStatement(void *stmt, CMirNode *lnode,
                              CSageCodeGen *codegen) {
  void *scope = lnode->ref;
  void *sg_loop = codegen->GetParent(scope);
  assert(codegen->IsForStatement(sg_loop));
  string sFile = getUserCodeFileLocation(codegen, sg_loop);
  string sStmt = getUserCodeFileLocation(codegen, stmt);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string str_loop = codegen->UnparseToString(codegen->GetLoopIterator(sg_loop));
  std::ostringstream oss;
  string stmt_type;
  if (codegen->IsBreakStatement(stmt) != 0) {
    stmt_type = "break";
  } else if (codegen->IsContinueStatement(stmt) != 0) {
    stmt_type = "continue";
  } else if (codegen->IsLabelStatement(stmt) != 0) {
    stmt_type = "label";
  } else if (codegen->IsGotoStatement(stmt) != 0) {
    stmt_type = "goto";
  } else {
    assert(0);
  }
#if 0
    oss << "Coarse-grained pipelining NOT applied: loop \'"
        << str_loop << "\' (" << sFile << ":" << nLine << ")" << endl;
    oss << "  Reason: ";
    oss << stmt_type
      << " statements are not allowed in coarse-grained pipeline \n";
    oss << "(" << sFile << ":" << nStmtLine << ")\n";
#endif
  string hint_info = " ";
  if (codegen->IsLabelStatement(stmt) != 0) {
    hint_info += "Hint: try to bracket the label and the loop together.\n";
  }
  //  oss << "Hint: try to bracket the label and the loop together.\n";
  //  string loop_info = "'" + str_loop + "' (" +
  //                                    sFile + ":" + my_itoa(nLine) + ")";
  //  string stmt_info = "(" + sFile + ":" + my_itoa(nStmtLine) + ")";
  string loop_info = "'" + str_loop + "' (" + sFile + ")";
  string stmt_info = "(" + sStmt + ")";
  string msg_L2 =
      stmt_type + " statements are not allowed in coarse-grained pipeline.";
  dump_critical_message(
      CGPIP_WARNING_2(loop_info, stmt_type, stmt_info, hint_info), 0, codegen,
      sg_loop);

  map<string, string> msg_map;
  msg_map["coarse_grained_pipeline"] = "off";
  insertMessage(codegen, sg_loop, msg_map);
}

void reportNonCanonical(void *stmt, CMirNode *lnode, CSageCodeGen *codegen) {
  void *scope = lnode->ref;
  void *sg_loop = codegen->GetParent(scope);
  assert(codegen->IsForStatement(sg_loop));
  assert(codegen->IsForStatement(stmt));
  string sFile = getUserCodeFileLocation(codegen, sg_loop);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string str_loop = codegen->UnparseToString(codegen->GetLoopIterator(sg_loop));
#if 0
    std::ostringstream oss;
    oss << "Coarse-grained pipelining NOT applied: loop \'"
        << str_loop << "\' (" << sFile << ":" << nLine << ")" << endl;
    oss << "  Reason: ";
    oss << "the loop is non-canonical \n";
#endif
  //  string loop_info = "'" + str_loop + "' (" +
  //                                 sFile + ":" + my_itoa(nLine) + ")";
  string loop_info = "'" + str_loop + "' (" + sFile + ")";
  dump_critical_message(CGPIP_WARNING_5(loop_info), 0, codegen, sg_loop);

  map<string, string> msg_map;
  msg_map["coarse_grained_pipeline"] = "off";
  insertMessage(codegen, sg_loop, msg_map);
}

void reportLegalityCheck(CSageCodeGen *codegen, CMirNode *lnode, void *sg_array,
                         const string &reason, bool local) {
  void *scope = lnode->ref;
  void *sg_loop = codegen->GetParent(scope);
  assert(codegen->IsForStatement(sg_loop));
  string sFile = getUserCodeFileLocation(codegen, sg_loop);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string str_loop = codegen->UnparseToString(codegen->GetLoopIterator(sg_loop));
  string var_name = "'" + codegen->GetVariableName(sg_array) + "'";
  string var_info =
      var_name + " " + getUserCodeFileLocation(codegen, sg_array, true);
  string loop_info = "'" + str_loop + "' (" + sFile + ")";
  string msg_L2_1 = "Potential cross-iteration data dependence on variable " +
                    var_info + ". " + reason;
  string msg_L2_2 = "Potential uninitialized data elements in variable " +
                    var_info + ". " + reason;

  if (local) {
    dump_critical_message(CGPIP_WARNING_9(loop_info, var_info), 0, codegen,
                          sg_loop);
  } else {
    dump_critical_message(CGPIP_WARNING_6(loop_info, var_info, reason), 0,
                          codegen, sg_loop);
  }

  map<string, string> msg_map;
  msg_map["coarse_grained_pipeline"] = "off";
  insertMessage(codegen, sg_loop, msg_map);
}

void reportOneStatement(CMirNode *lnode, CSageCodeGen *codegen) {
  void *scope = lnode->ref;
  void *sg_loop = codegen->GetParent(scope);
  assert(codegen->IsForStatement(sg_loop));
  string sFile = getUserCodeFileLocation(codegen, sg_loop);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string str_loop = codegen->UnparseToString(codegen->GetLoopIterator(sg_loop));
  string loop_info = "'" + str_loop + "' (" + sFile + ")";
  dump_critical_message(CGPIP_WARNING_8(loop_info), 0, codegen, sg_loop);

  map<string, string> msg_map;
  msg_map["coarse_grained_pipeline"] = "off";
  insertMessage(codegen, sg_loop, msg_map);
}
//  false: has feedback dependency convertively
//  true: no feedback dependency
bool test_feedback_dependency(CSageCodeGen *codegen, void *sg_array,
                              void *sg_loop, int stage, enum wr_type dep_type,
                              set<void *> var_init_new_set) {
  //  Yuxin: Sep 17 2018
  //  If the buffer is declared inside the loop
  //  it is supposed to be fully initilialized
  //  skip the coverage test
  if (var_init_new_set.count(sg_array) > 0) {
    return true;
  }
  if (codegen->IsForStatement(sg_loop) == 0) {
    return false;
  }
  if (codegen->IsCanonicalForLoop(sg_loop) == 0) {
    return false;
  }
  void *loop_body = codegen->GetLoopBody(sg_loop);
  if (loop_body == nullptr) {
    return false;
  }
  void *sg_var_decl = codegen->GetVariableDecl(sg_array);
  if ((codegen->IsVariableDecl(sg_var_decl) != 0) &&
      (codegen->IsInScope(sg_var_decl, loop_body) != 0) &&
      !codegen->IsStatic(sg_var_decl)) {
    return true;
  }
  if (codegen->IsScalarType(sg_array) != 0) {
    return false;
  }
  ArrayRangeAnalysis mr_range(sg_array, codegen, loop_body, loop_body, false,
                              false);
  if (mr_range.has_write() == 0) {
    return true;
  }
  vector<CMarsRangeExpr> mr_write_range = mr_range.GetRangeExprWrite();

  //  forall delta_i in [1..#PipelineStage],
  //  x + [0..L) intersect
  //  (delta_i)*c+y+[0..L1) = empty

  //  check read after write dependence
  //  dep_type: 1-read after write checking
  //                      2-only write after write checking
  cout << "==> check read after write\n";
  if (mr_range.has_read() != 0) {
    vector<CMarsRangeExpr> mr_read_range = mr_range.GetRangeExprRead();
    if (mr_read_range.size() != mr_write_range.size()) {
      return false;
    }
    int dim_size = mr_read_range.size();
    bool no_read_after_write_dep = false;
    for (int i = 0; i < dim_size; ++i) {
      //  as long as there is one dimension, which has no overlap,
      //  we can assure there is no dependency
      CMarsRangeExpr read_range = mr_read_range[i];
      CMarsRangeExpr write_range = mr_write_range[i];
      CMarsExpression r_lb;
      CMarsExpression r_ub;
      CMarsExpression w_lb;
      CMarsExpression w_ub;
#if PROJDEBUG
      cout << "read range: " << read_range.print() << endl;
      cout << "write range: " << write_range.print() << ", "
           << write_range.is_exact() << endl;
#endif
      if ((read_range.has_lb() == 0) || (read_range.has_ub() == 0) ||
          (write_range.has_lb() == 0) || (write_range.has_ub() == 0)) {
        continue;
      }

      if ((read_range.get_simple_bound(&r_lb, &r_ub) == 0) ||
          (write_range.get_simple_bound(&w_lb, &w_ub) == 0)) {
        continue;
      }

      if (r_lb.IsLoopInvariant(sg_loop) && r_ub.IsLoopInvariant(sg_loop) &&
          w_lb.IsLoopInvariant(sg_loop) && w_ub.IsLoopInvariant(sg_loop)) {
        CMarsRangeExpr mr_intersect = read_range.Intersect(write_range);
        cout << "intersect: " << mr_intersect.print() << ", "
             << mr_intersect.is_exact() << endl;
        if (mr_intersect.is_empty() != 0) {
          no_read_after_write_dep = true;
          break;
        }
      } else {
        //  should have the same coefficent on loop_iterator
        CMarsExpression r_lb_coef = r_lb.get_coeff(sg_loop);
        CMarsExpression r_ub_coef = r_ub.get_coeff(sg_loop);
        CMarsExpression w_lb_coef = w_lb.get_coeff(sg_loop);
        CMarsExpression w_ub_coef = w_ub.get_coeff(sg_loop);
        if (((r_lb_coef == r_ub_coef) != 0) &&
            ((r_ub_coef == w_lb_coef) != 0) &&
            ((w_lb_coef == w_ub_coef) != 0)) {
          if ((r_lb_coef == 0) != 0) {
            continue;
          }
          CMarsExpression new_r_lb;
          CMarsExpression new_r_ub;
          new_r_lb = r_lb;
          new_r_ub = r_ub;
          new_r_lb.substitute(sg_loop, 0);
          new_r_ub.substitute(sg_loop, 0);
          if (!new_r_lb.IsLoopInvariant(sg_loop) ||
              !new_r_ub.IsLoopInvariant(sg_loop)) {
            continue;
          }
          CMarsRangeExpr left_range(new_r_lb, new_r_ub);
          bool has_overlap = false;
          for (int i = 1; i < stage; ++i) {
            CMarsExpression new_w_lb = w_lb;
            new_w_lb.substitute(sg_loop, i);
            CMarsExpression new_w_ub = w_ub;
            new_w_ub.substitute(sg_loop, i);
            CMarsRangeExpr right_range(new_w_lb, new_w_ub);
            CMarsRangeExpr intersect_range = left_range.Intersect(right_range);
#if PROJDEBUG
            cout << left_range.print() << endl;
            cout << right_range.print() << endl;
            cout << intersect_range.print() << endl;
#endif
            if (intersect_range.is_empty() == 0) {
              has_overlap = true;
              break;
            }
          }
          if (!has_overlap) {
            no_read_after_write_dep = true;
            break;
          }
        }
      }
    }
    if (!no_read_after_write_dep) {
      return false;
    }
  }

  //  check write after write dependence
  if (dep_type == WaW) {
    cout << "==> check write after write\n";
    int dim_size = mr_write_range.size();
    bool no_write_after_write_dep = false;
    for (int i = 0; i < dim_size; ++i) {
      CMarsRangeExpr write_range = mr_write_range[i];
      CMarsExpression w_lb;
      CMarsExpression w_ub;
      if (write_range.get_simple_bound(&w_lb, &w_ub) == 0) {
        continue;
      }
      if (w_lb.IsLoopInvariant(sg_loop) || w_ub.IsLoopInvariant(sg_loop)) {
        continue;
      }

      //  should have the same coefficent on loop_iterator
      CMarsExpression w_lb_coef = w_lb.get_coeff(sg_loop);
      CMarsExpression w_ub_coef = w_ub.get_coeff(sg_loop);
      if ((w_lb_coef == w_ub_coef) != 0) {
        if ((w_lb_coef == 0) != 0) {
          continue;
        }
        CMarsExpression new_w_lb;
        CMarsExpression new_w_ub;
        new_w_lb = w_lb;
        new_w_ub = w_ub;
        new_w_lb.substitute(sg_loop, 0);
        new_w_ub.substitute(sg_loop, 0);
        if (!new_w_lb.IsLoopInvariant(sg_loop) ||
            !new_w_ub.IsLoopInvariant(sg_loop)) {
          continue;
        }
        CMarsRangeExpr left_range(new_w_lb, new_w_ub);
        bool has_overlap = false;
        for (int i = 1; i < stage; ++i) {
          CMarsExpression new_w_lb = w_lb;
          new_w_lb.substitute(sg_loop, i);
          CMarsExpression new_w_ub = w_ub;
          new_w_ub.substitute(sg_loop, i);
          CMarsRangeExpr right_range(new_w_lb, new_w_ub);
          CMarsRangeExpr intersect_range = left_range.Intersect(right_range);
#if PROJDEBUG
          cout << left_range.print() << endl;
          cout << right_range.print() << endl;
          cout << intersect_range.print() << endl;
#endif
          if (intersect_range.is_empty() == 0) {
            has_overlap = true;
            break;
          }
        }
        if (!has_overlap) {
          no_write_after_write_dep = true;
          break;
        }
      }
    }
    if (!no_write_after_write_dep) {
      return false;
    }
  }
  return true;
}

void pragma_propagate(CSageCodeGen *codegen, CMarsIr *mars_ir,
                      map<void *, vector<void *>> existing_table,
                      map<void *, vector<void *>> gen_table,
                      set<void *> var_init_new_set) {
  cout << "Pragma propagate:\n";

  map<void *, vector<map<int, int>>> partition_factors;
  for (auto it : existing_table) {
    cout << codegen->_p(it.first) << endl;
    if (gen_table.find(it.first) != gen_table.end()) {
      for (auto sg_pragma : it.second) {
        cout << codegen->_p(sg_pragma) << endl;
        if (mars_ir->isHLSPartition(sg_pragma)) {
          map<int, int> hls_factors;
          int ret = static_cast<int>(factor_extract(codegen, mars_ir, sg_pragma,
                                                    &hls_factors, it.first));
          if (ret == 0) {
            continue;
          }

          for (auto ps : hls_factors) {
            for (auto arr_decl : gen_table[it.first]) {
              partition_pragma_gen_xilinx(codegen, arr_decl, ps.first,
                                          ps.second);
            }
          }
        }
      }
    }
  }
  for (auto it : existing_table) {
    if (gen_table.find(it.first) != gen_table.end() &&
        var_init_new_set.count(it.first) > 0) {
      vector<void *> pragma_list = it.second;
      cout << pragma_list.size() << endl;
      for (size_t i = 0; i < pragma_list.size(); i++) {
        cout << codegen->_p(pragma_list[i]) << endl;
        if (mars_ir->isHLSPartition(pragma_list[i])) {
          codegen->RemoveStmt(pragma_list[i]);
        }
      }
    }
  }
}

bool label_processing(CSageCodeGen *codegen, void *pTopFunc, CMirNode *bNode) {
  cout << "==> Label preprocessing......\n";
  Rose_STL_Container<SgNode *> gotoList = NodeQuery::querySubTree(
      static_cast<SgNode *>(pTopFunc), V_SgGotoStatement);
  map<SgLabelStatement *, void *> label_vec;
  for (auto goto_it : gotoList) {
    SgGotoStatement *sg_goto = isSgGotoStatement(goto_it);
    SgLabelStatement *label0 = sg_goto->get_label();
    if ((label0 != nullptr) && label_vec.find(label0) == label_vec.end()) {
      label_vec[label0] = sg_goto;
    }
  }

  void *sg_loop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
  void *sg_pre_stmt = codegen->GetParent(sg_loop);

  if (sg_pre_stmt == nullptr) {
    return true;
  }

  if (isSgLabelStatement(static_cast<SgNode *>(sg_pre_stmt)) != nullptr) {
    SgLabelStatement *label1 = static_cast<SgLabelStatement *>(sg_pre_stmt);
    cout << "label: " << codegen->_p(sg_pre_stmt) << endl;
    if (label_vec.find(label1) != label_vec.end()) {
      reportUnsupportStatement(label_vec[label1], bNode, codegen);
      return false;
    }
    string label_str = label1->unparseToString();
    codegen->AddComment_v1(label1, "Original label: " + label_str);
    codegen->RemoveLabel(label1);
  }

  vector<void *> v_label;
  codegen->GetNodesByType_int(sg_loop, "preorder", V_SgLabelStatement,
                              &v_label);
  for (size_t j = 0; j < v_label.size(); j++) {
    SgLabelStatement *label1 = static_cast<SgLabelStatement *>(v_label[j]);
    cout << "label: " << codegen->_p(label1) << endl;
    if (label_vec.find(label1) != label_vec.end()) {
      reportUnsupportStatement(label_vec[label1], bNode, codegen);
      return false;
    }
    string label_str = label1->unparseToString();
    codegen->AddComment_v1(label1, "Original label: " + label_str);
    codegen->RemoveLabel(label1);
  }
  return true;
}

void remove_pragmas(CSageCodeGen *codegen, CMirNode *new_node) {
  for (size_t i = 0; i < new_node->pragma_table.size(); i++) {
    auto decl = new_node->pragma_table[i];
    void *curr_loop = codegen->GetEnclosingNode("ForLoop", new_node->ref);
    void *sg_loop = codegen->GetEnclosingNode("ForLoop", decl);
    if (curr_loop == sg_loop) {
      CAnalPragma ana_pragma(codegen);
      bool errorOut = false;
      if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut)) {
        if (ana_pragma.is_pipeline()) {
          string pragmaString = codegen->GetPragmaString(decl);
          codegen->AddComment_v1(new_node->ref,
                                 "Original: #pragma " + pragmaString);
          codegen->RemoveStmt(decl);
        }
      }
    }
  }
}

void check_bus_access(node_table_map *node_table, bool *cg_level_2) {
  bool memcpy_tag = false;
  int memcpy_stage = -1;
  for (size_t i = 0; i < node_table->size(); i++) {
    (*node_table)[i].cg_bus = false;
    bool bus_tag = false;

    for (stmt_table_vec::iterator it = (*node_table)[i].begin();
         it != (*node_table)[i].end(); it++) {
      SgStatement *stmt = *it;
      Rose_STL_Container<SgNode *> callList =
          NodeQuery::querySubTree(stmt, V_SgFunctionCallExp);
      Rose_STL_Container<SgNode *>::iterator func_call = callList.begin();
      for (func_call = callList.begin(); func_call != callList.end();
           func_call++) {
        SgFunctionCallExp *sg_call = isSgFunctionCallExp(*func_call);
        SgFunctionSymbol *sg_symbol = sg_call->getAssociatedFunctionSymbol();
        if (sg_symbol->get_name().getString().find("memcpy") != string ::npos) {
          cout << "Memcpy in node " << i << endl;
          bus_tag = true;
          if (memcpy_tag) {
            cout << "Second level of CG is generated for bus conflict on "
                    "node "
                 << i << endl;
            *cg_level_2 = true;
            (*node_table)[i].cg_bus = true;
            break;
          }
          memcpy_tag = true;
          memcpy_stage = (*node_table)[i].stage;
          (*node_table)[i].cg_bus = true;
          break;
        }
      }
      if (memcpy_tag) {
        break;
      }
    }
    if (!bus_tag) {
      for (auto p_it = (*node_table)[i].port.begin();
           p_it != (*node_table)[i].port.end(); p_it++) {
        if (p_it->second.is_port) {
          cout << "Port direct access in node " << i << endl;
          bus_tag = true;
          break;
        }
      }
      if (bus_tag) {
        if (memcpy_tag) {
          if ((*node_table)[i].stage != memcpy_stage) {
            cout << "Second level of CG is generated for bus conflict on "
                    "node "
                 << i << endl;
            *cg_level_2 = true;
            (*node_table)[i].cg_bus = true;
          }
        } else {
          memcpy_tag = true;
          memcpy_stage = (*node_table)[i].stage;
          (*node_table)[i].cg_bus = true;
        }
      }
    }
  }
}

void build_init_function(CSageCodeGen *codegen, CMirNode *bNode,
                         var_table_map *var_table,
                         map<string, void *> *all_func_map,
                         const string &kernel_str) {
  void *curr_loop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
  int i;
  void *curr_decl = codegen->GetEnclosingNode("Function", bNode->ref);
  void *generalScope = codegen->GetScope(curr_decl);

  for (auto v_it = var_table->begin(); v_it != var_table->end(); v_it++) {
    CVarNode arr_node = v_it->second;
    if (arr_node.is_port)
      continue;
    if (!arr_node.do_init[LIVENESS_IN] &&
        (!arr_node.do_init[LIVENESS_OUT] || arr_node.w_type == 0))
      continue;
    if (arr_node.buf_depth <= 1)
      continue;
    void *sg_type = codegen->GetTypebyVar(arr_node.var);
    const string target_name = "target";
    const string orig_name = "orig";
    SgInitializedName *target_arg = nullptr;
    SgInitializedName *orig_arg = nullptr;
    if ((codegen->IsPointerType(sg_type) == 0) &&
        (codegen->IsArrayType(sg_type) == 0) && (arr_node.w_type != 0)) {
      target_arg = codegen->CreateVariable_v1(
          SgName(target_name), codegen->CreatePointerType(sg_type));
      orig_arg = codegen->CreateVariable_v1(
          SgName(orig_name), codegen->CreatePointerType(sg_type));
    } else {
      target_arg = codegen->CreateVariable_v1(SgName(target_name), sg_type);
      orig_arg = codegen->CreateVariable_v1(SgName(orig_name), sg_type);
    }
    SgFunctionParameterList *init_parameters =
        static_cast<SgFunctionParameterList *>(
            codegen->buildFunctionParameterList());
    codegen->appendArg(init_parameters, target_arg);
    codegen->appendArg(init_parameters, orig_arg);

    string func_name = kernel_str + "_assign_" + v_it->first;
    SgFunctionDeclaration *init_decl =
        static_cast<SgFunctionDeclaration *>(codegen->CreateFuncDecl(
            "void", func_name, init_parameters, generalScope, true, curr_loop));
    (*all_func_map)[func_name] = init_decl;
    codegen->InsertStmt_v1(curr_decl, init_decl, true);
    SgBasicBlock *init_body = init_decl->get_definition()->get_body();
    void *init_loop_body = codegen->CreateBasicBlock();

    SgExpression *target_expr =
        static_cast<SgVarRefExp *>(codegen->CreateVariableRef(target_arg));
    SgExpression *orig_expr =
        static_cast<SgVarRefExp *>(codegen->CreateVariableRef(orig_arg));
    if ((codegen->IsPointerType(sg_type) == 0) &&
        (codegen->IsArrayType(sg_type) == 0) && (arr_node.w_type != 0)) {
      target_expr = static_cast<SgExpression *>(codegen->CreateExp(
          V_SgPointerDerefExp,
          static_cast<SgVarRefExp *>(codegen->CreateVariableRef(target_arg))));
      orig_expr = static_cast<SgExpression *>(codegen->CreateExp(
          V_SgPointerDerefExp,
          static_cast<SgVarRefExp *>(codegen->CreateVariableRef(orig_arg))));
    }
    void *basic_type = nullptr;
    vector<size_t> vec_sizes;
    int dim = codegen->get_type_dimension(sg_type, &basic_type, &vec_sizes,
                                          (arr_node.var));
    if (dim == 0) {
      void *init =
          codegen->CreateStmt(V_SgAssignStatement, target_expr, orig_expr);
      codegen->AppendChild_v1(init, init_body);
    } else {
      for (i = 0; i < dim; i++) {
        string i_dim_str = "i" + my_itoa(i);

        void *v_iter =
            codegen->CreateVariableDecl("int", i_dim_str, nullptr, init_body);
        codegen->PrependChild_v1(v_iter, init_body);

        SgExpression *i_arg =
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(v_iter));
        target_expr = static_cast<SgExpression *>(
            codegen->CreateExp(V_SgPntrArrRefExp, target_expr, i_arg));
        orig_expr = static_cast<SgExpression *>(
            codegen->CreateExp(V_SgPntrArrRefExp, orig_expr, i_arg));

        void *init = codegen->CreateStmt(V_SgAssignStatement,
                                         codegen->CreateVariableRef(v_iter),
                                         codegen->CreateConst(0));
        int buffer_size = vec_sizes[dim - i - 1];
        void *cond = codegen->CreateStmt(
            V_SgExprStatement,
            codegen->CreateExp(V_SgLessThanOp,
                               codegen->CreateVariableRef(v_iter),
                               codegen->CreateConst(buffer_size)));
        void *incr = codegen->CreateExp(V_SgPlusPlusOp,
                                        codegen->CreateVariableRef(v_iter),
                                        nullptr, SgUnaryOp::postfix);
        void *initLoop = codegen->CreateForLoop(init, cond, incr,
                                                codegen->CreateBasicBlock());

        if (i == 0) {
          codegen->AppendChild_v1(initLoop, init_body);
        } else {
          codegen->AppendChild_v1(initLoop, init_loop_body);
        }

        init_loop_body = codegen->GetLoopBody(initLoop);
      }

      void *init =
          codegen->CreateStmt(V_SgAssignStatement, target_expr, orig_expr);
      codegen->AppendChild_v1(init, init_loop_body);
    }
  }
}

void build_node_functions(CSageCodeGen *codegen, CMirNode *bNode,
                          var_table_map *var_table,
                          map<string, void *> *func_map_kernel,
                          map<string, void *> *all_func_map,
                          map<void *, void *> *all_func_call_map,
                          map<string, map<string, void *>> *arg_map_kernel,
                          node_table_map *node_table, int stage_sum,
                          int64_t incr, const string &kernel_str) {
  void *loop_body = bNode->ref;
  void *curr_loop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
  void *curr_iter = codegen->GetLoopIterator(curr_loop);
  int num_nodes = node_table->size();
  void *curr_decl = codegen->GetEnclosingNode("Function", bNode->ref);
  void *generalScope = codegen->GetScope(curr_decl);

  for (auto f_it = func_map_kernel->begin(); f_it != func_map_kernel->end();
       f_it++) {
    int stage_count = 0;
    void *sg_kernel = f_it->second;
    void *curr_kernel_body = codegen->GetFuncBody(sg_kernel);
    map<string, void *> name_map_kernel;
    name_map_kernel = (*arg_map_kernel)[f_it->first];
    if (func_map_kernel->size() == 3 && f_it->first == "kernel") {
      continue;
    }

    string pragma_depend_str = std::string(HLS_PRAGMA) + " INLINE OFF";
    void *new_depend_pragma =
        codegen->CreatePragma(pragma_depend_str, curr_kernel_body);
    codegen->PrependChild_v1(new_depend_pragma, curr_kernel_body);

    while (stage_count < stage_sum) {
      for (size_t i = 0; i < num_nodes; i++) {
        CGraphNode node_i = (*node_table)[i];
        if (f_it->first == "bus") {
          if (!node_i.cg_bus) {
            continue;
          }
        } else if (f_it->first == "comp") {
          if (node_i.cg_bus) {
            continue;
          }
        }

        if (node_i.stage == stage_count) {
          map<string, SgInitializedName *> name_map_node;
          string stage_str = my_itoa(node_i.stage);
          string node_str = kernel_str + "_node_" +
                            my_itoa(static_cast<int>(i)) + "_stage_" +
                            stage_str;
          SgName name2(node_str);

          SgFunctionParameterList *node_parameters =
              static_cast<SgFunctionParameterList *>(
                  codegen->buildFunctionParameterList());
          /*
                    for (symbol_table_map::const_iterator pp =
                             bNode->iterator_table.begin();
                         pp != bNode->iterator_table.end(); ++pp) {
                      SgInitializedName *iter_arg =
             static_cast<SgInitializedName *>( codegen->CreateVariable("int",
             pp->second));

                      codegen->appendArg(node_parameters, iter_arg);
                      name_map_node[pp->second] = iter_arg;
                    }
                    */
          SgInitializedName *iter_arg = static_cast<SgInitializedName *>(
              codegen->CreateVariable("int", codegen->_up(curr_iter)));
          codegen->appendArg(node_parameters, iter_arg);
          name_map_node[codegen->_up(curr_iter)] = iter_arg;

          SgInitializedName *exec_arg = static_cast<SgInitializedName *>(
              codegen->CreateVariable("int", "exec"));
          codegen->appendArg(node_parameters, exec_arg);
          name_map_node["exec"] = exec_arg;

          for (auto p_it = node_i.port.begin(); p_it != node_i.port.end();
               p_it++) {
            void *sg_type = codegen->GetTypebyVar(p_it->second.var);
            SgInitializedName *new_arg = nullptr;
            new_arg = codegen->CreateVariable_v1(SgName(p_it->first), sg_type);
            name_map_node[p_it->first] = new_arg;
            codegen->appendArg(node_parameters, new_arg);
          }

          SgFunctionDeclaration *node_decl =
              static_cast<SgFunctionDeclaration *>(
                  codegen->CreateFuncDecl("void", name2, node_parameters,
                                          generalScope, true, loop_body));

          (*all_func_map)[name2.str()] = node_decl;
          codegen->InsertStmt_v1(sg_kernel, node_decl, true);
          SgBasicBlock *node_body = node_decl->get_definition()->get_body();

          string pragma_depend_str = std::string(HLS_PRAGMA) + " INLINE OFF";
          void *new_depend_pragma =
              codegen->CreatePragma(pragma_depend_str, node_body);
          codegen->PrependChild_v1(new_depend_pragma, node_body);

          SgStatement *node_if_cond_stmt =
              static_cast<SgStatement *>(codegen->CreateStmt(
                  V_SgExprStatement,
                  codegen->CreateExp(V_SgEqualityOp,
                                     codegen->CreateVariableRef(exec_arg),
                                     codegen->CreateConst(1))));
          void *if_body = codegen->CreateBasicBlock();
          void *node_if_stmt =
              codegen->CreateIfStmt(node_if_cond_stmt, if_body, nullptr);
          codegen->AppendChild_v1(node_if_stmt, node_body);

          for (auto it = node_i.begin(); it != node_i.end(); it++) {
            SgStatement *st = (*it);
            if (i == 0 && (isSgDeclarationStatement(st) != nullptr)) {
              continue;
            }
            void *new_st = codegen->CopyStmt(st, true);
            codegen->AppendChild_v1(new_st, if_body);

            void *sg_pre = codegen->GetPreviousStmt(st);
            if (sg_pre != nullptr) {
              SgStatement *sg_pre_ =
                  isSgStatement(static_cast<SgNode *>(sg_pre));
              if (isSgForStatement(st) == nullptr) {
                if (isSgLabelStatement(sg_pre_) != nullptr) {
                  void *new_st_pre = codegen->CopyStmt(sg_pre_);
                  codegen->InsertStmt_v1(new_st, new_st_pre);
                  codegen->RemoveStmt(sg_pre_);
                }
              }
            }
            codegen->RemoveStmt(st);
          }
#if 1
          map<string, int> iter_map;
          for (auto it = node_i.begin(); it != node_i.end(); it++) {
            SgStatement *node_stmt = *it;
            Rose_STL_Container<SgNode *> forLoops =
                NodeQuery::querySubTree(node_stmt, V_SgForStatement);
            if (forLoops.empty()) {
              continue;
            }

            Rose_STL_Container<SgNode *>::iterator itt = forLoops.begin();
            for (itt = forLoops.begin(); itt != forLoops.end(); itt++) {
              SgForStatement *stmt = isSgForStatement(*itt);
              SgStatement *init_stmt = stmt->get_init_stmt()[0];
              if (isSgVariableDeclaration(init_stmt) != nullptr) {
                continue;
              }
              SgInitializedName *iter = isSgInitializedName(
                  static_cast<SgNode *>(codegen->GetLoopIterator(stmt)));
              if (iter_map.find(iter->unparseToString()) == iter_map.end() &&
                  node_i.port.find(iter->unparseToString()) ==
                      node_i.port.end()) {
                void *iter_var = codegen->CreateVariableDecl(
                    "int", codegen->_up(iter), nullptr, bNode->ref);
                codegen->PrependChild_v1(iter_var, if_body);
                iter_map[iter->unparseToString()] = 1;
              }
            }
          }
#endif
          //  Youxiang: 20170413 replace variable reference
          {
            map<void *, void *> to_replace_ref;
            for (auto p_it = node_i.port.begin(); p_it != node_i.port.end();
                 p_it++) {
              void *old_var = (*var_table)[p_it->first].var;
              SgInitializedName *new_var =
                  static_cast<SgInitializedName *>(name_map_node[p_it->first]);
              bool t_pointer = false;
              if (((*var_table)[p_it->first].self_w_type != 0) &&
                  (codegen->has_write_in_scope_fast(old_var, if_body) != 0)) {
                void *sg_type = codegen->GetTypebyVar(new_var);
                new_var->set_type(static_cast<SgPointerType *>(
                    codegen->CreatePointerType(sg_type)));
                t_pointer = true;
              }

              vector<void *> vec_refs;
              codegen->get_ref_in_scope(old_var, if_body, &vec_refs);
              for (auto old_ref : vec_refs) {
                void *new_ref = codegen->CreateVariableRef(new_var);
                if (t_pointer) {
                  new_ref = codegen->CreateExp(V_SgPointerDerefExp, new_ref);
                }
                to_replace_ref[old_ref] = new_ref;
              }
            }
            for (auto it : to_replace_ref) {
              codegen->ReplaceExp(it.first, it.second);
            }
          }
          //  //////////////////////////////////////////////  /
          //  Note:Build node function calls in kernel body
          void *parameters3 = codegen->buildExprListExp();
          /*     for (auto pp = bNode->iterator_table.begin();
                    pp != bNode->iterator_table.end(); ++pp) {
                 if (pp->second != codegen->_up(curr_iter)) {
                   void *arg_exp = codegen->CreateVariableRef(
                       name_map_kernel["mars_" + pp->second]);
                   codegen->appendExp(parameters3, arg_exp);
                 } else */
          {
            string iter_str = codegen->_up(curr_iter);
            void *arg_exp = codegen->CreateExp(
                V_SgMultiplyOp, codegen->CreateConst(node_i.stage),
                codegen->CreateConst(incr));
            arg_exp = codegen->CreateExp(
                V_SgSubtractOp,
                codegen->CreateVariableRef(name_map_kernel["mars_" + iter_str]),
                arg_exp);

            if (incr == 1) {
              arg_exp =
                  codegen->CreateExp(V_SgSubtractOp,
                                     codegen->CreateVariableRef(
                                         name_map_kernel["mars_" + iter_str]),
                                     codegen->CreateConst(node_i.stage));
            }
            codegen->appendExp(parameters3, arg_exp);
            void *exec_less_op = codegen->CreateExp(
                V_SgLessOrEqualOp,
                codegen->CreateVariableRef(name_map_kernel["mars_" + iter_str]),
                codegen->CreateExp(
                    V_SgAddOp,
                    codegen->CreateVariableRef(name_map_kernel["mars_cond"]),
                    codegen->CreateExp(V_SgMultiplyOp,
                                       codegen->CreateConst(node_i.stage),
                                       codegen->CreateConst(incr))));

            if (incr == 1) {
              exec_less_op = codegen->CreateExp(
                  V_SgLessOrEqualOp,
                  codegen->CreateVariableRef(
                      name_map_kernel["mars_" + iter_str]),
                  codegen->CreateExp(
                      V_SgAddOp,
                      codegen->CreateVariableRef(name_map_kernel["mars_cond"]),
                      codegen->CreateConst(node_i.stage)));
            }

            void *exec_greater_op = codegen->CreateExp(
                V_SgGreaterOrEqualOp,
                codegen->CreateVariableRef(name_map_kernel["mars_" + iter_str]),
                codegen->CreateExp(
                    V_SgAddOp,
                    codegen->CreateVariableRef(name_map_kernel["mars_init"]),
                    codegen->CreateExp(V_SgMultiplyOp,
                                       codegen->CreateConst(node_i.stage),
                                       codegen->CreateConst(incr))));

            if (incr == 1) {
              exec_greater_op = codegen->CreateExp(
                  V_SgGreaterOrEqualOp,
                  codegen->CreateVariableRef(
                      name_map_kernel["mars_" + iter_str]),
                  codegen->CreateExp(
                      V_SgAddOp,
                      codegen->CreateVariableRef(name_map_kernel["mars_init"]),
                      codegen->CreateConst(node_i.stage)));
            }

            void *exec_op =
                codegen->CreateExp(V_SgBitAndOp, exec_greater_op, exec_less_op);
            void *exec_int = codegen->CreateCastExp(exec_op, "int");
            codegen->appendExp(parameters3, exec_int);
          }
          //  }

          void *node_func = (*all_func_map)[name2.str()];
          for (auto p_it = node_i.port.begin(); p_it != node_i.port.end();
               p_it++) {
            if (p_it->second.is_port) {
              SgExpression *c_arg = static_cast<SgVarRefExp *>(
                  codegen->CreateVariableRef(name_map_kernel[p_it->first]));
              codegen->appendExp(parameters3, c_arg);
              continue;
            }

            string channel_str = my_itoa(node_i.stage);
            channel_str = "mars_" + p_it->first + "_" + channel_str;
            SgExpression *c_arg = static_cast<SgVarRefExp *>(
                codegen->CreateVariableRef(name_map_kernel[channel_str]));

            if (((*var_table)[p_it->first].self_w_type != 0) &&
                (codegen->has_write_in_scope_fast(name_map_node[p_it->first],
                                                  node_func) == 0)) {
              c_arg = static_cast<SgExpression *>(
                  codegen->CreateExp(V_SgPointerDerefExp, c_arg));
            }
            codegen->appendExp(parameters3, c_arg);
          }
          assert(node_func);
          void *node_call_body =
              codegen->CreateFuncCallStmt(node_func, parameters3, loop_body);
          codegen->AppendChild_v1(node_call_body, curr_kernel_body);
          (*all_func_call_map)[node_func] = node_call_body;
        }
      }
      stage_count++;
    }
  }
}

bool check_range_bound(CSageCodeGen *codegen, CMarsIr *mars_ir,
                       CMarsRangeExpr *m_range, vector<size_t> *lb_size,
                       vector<size_t> *ub_size) {
  bool is_full_cover = true;
  if (m_range->is_exact() == 0)
    is_full_cover = false;
  CMarsExpression lb;
  CMarsExpression ub;
  int simple_bound = m_range->get_simple_bound(&lb, &ub);
  if (simple_bound == 0)
    is_full_cover = false;
  if (lb.IsConstant() == 0 || ub.IsConstant() == 0)
    is_full_cover = false;

  if (is_full_cover) {
    lb_size->push_back(lb.GetConstant());
    ub_size->push_back(ub.GetConstant());
  }
  return is_full_cover;
}

void partition_pragma_gen_xilinx(CSageCodeGen *codegen, void *arr_init, int dim,
                                 int factor) {
  void *sg_scope = nullptr;
  void *arr_decl = codegen->GetVariableDecl(arr_init);
  string arr_for_pragma = codegen->_up(arr_init);
  if (arr_decl != nullptr) {
    cout << "decl: " << codegen->_p(arr_decl) << endl;
  }

  vector<void *> insert_scope;
  vector<string> insert_name;
  set<void *> member_scope;
  if (codegen->IsGlobalInitName(arr_init) != 0) {
    vector<void *> vec_tmp;
    codegen->GetNodesByType_compatible(codegen->GetProject(), "SgVarRef",
                                       &vec_tmp);
    for (size_t i = 0; i < vec_tmp.size(); i++) {
      void *ref = vec_tmp[i];
      void *target_init = codegen->GetVariableInitializedName(ref);
      if (target_init == arr_init) {
        void *sg_decl = codegen->GetEnclosingNode("Function", ref);
        sg_scope = codegen->GetFuncBody(sg_decl);
        if (sg_scope != nullptr) {
          break;
        }
      }
    }
    insert_name.push_back(arr_for_pragma);
    insert_scope.push_back(sg_scope);
  } else if (codegen->IsMemberVariable(arr_init)) {
    vector<void *> vec_tmp;
    codegen->GetNodesByType_compatible(codegen->GetProject(), "SgVarRef",
                                       &vec_tmp);
    for (size_t i = 0; i < vec_tmp.size(); i++) {
      void *ref = vec_tmp[i];
      void *target_init = codegen->GetVariableInitializedName(ref);
      if (target_init == arr_init) {
        void *parent = codegen->GetParent(ref);
        if ((codegen->IsDotExp(parent) != 0) ||
            (codegen->IsArrowExp(parent) != 0)) {
          void *func_decl = codegen->GetEnclosingNode("Function", parent);
          if (member_scope.count(func_decl) > 0) {
            continue;
          }
          member_scope.insert(func_decl);
          void *sg_member_stmt = codegen->GetEnclosingNode("Statement", parent);
          insert_name.push_back(codegen->_up(parent));
          insert_scope.push_back(sg_member_stmt);
        }
      }
    }
  } else if (codegen->IsArgumentInitName(arr_init) != 0) {
    void *func_decl = codegen->GetEnclosingNode("Function", arr_init);
    if (func_decl == nullptr) {
      cout << "Couldnt generate new array_partition pragma for variable '"
           << arr_for_pragma << "': cannot find the related function\n";
      return;
    }
    sg_scope = codegen->GetFuncBody(func_decl);

    insert_name.push_back(arr_for_pragma);
    insert_scope.push_back(sg_scope);
  } else {
    insert_name.push_back(arr_for_pragma);
    insert_scope.push_back(arr_decl);
  }

  for (size_t i = 0; i < insert_name.size(); i++) {
    string arr_str = insert_name[i];
    codegen->removeSpaces(&arr_str);
    void *sg_scope = insert_scope[i];
    string pragma_str;
    string DIM_str;
    DIM_str += std::string(CMOST_dim_low) + "=" + my_itoa(dim + 1);
    if (factor == 1) {
      return;
    }
    if (factor == 0) {
      pragma_str = std::string(HLS_PRAGMA) + " " + CMOST_ARR_PARTITION_low +
                   " " + CMOST_variable_low + "=" + arr_str + " complete " +
                   DIM_str;
    } else {
      pragma_str = std::string(HLS_PRAGMA) + " " + CMOST_ARR_PARTITION_low +
                   " " + CMOST_variable_low + "=" + arr_str + " cyclic " +
                   CMOST_factor_low + "=" + my_itoa(factor) + " " + DIM_str;
    }

    cout << "[pragma gen] " << pragma_str << endl;
    if (sg_scope == nullptr) {
      cout << "Couldnt generate new array_partition pragma for variable '"
           << arr_str << "': cannot find the related scope\n";
      return;
    }
    void *new_pragma = codegen->CreatePragma(pragma_str, sg_scope);
    if (new_pragma == nullptr) {
      cout << "Couldnt generate new array_partition pragma for variable '"
           << arr_str << "'\n";
      return;
    }

    if (codegen->IsBasicBlock(sg_scope) != 0) {
      codegen->PrependChild(sg_scope, new_pragma);
    } else {
      codegen->InsertAfterStmt(new_pragma, sg_scope);
    }
  }
}
