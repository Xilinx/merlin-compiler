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


#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>

#include "rose.h"
#include "mars_opt.h"
#include "mars_ir_v2.h"
#include "mars_ir.h"

using MarsProgramAnalysis::CMarsRangeExpr;

// Yuxin: Dec/5/2019, remove hard code
#define LIVENESS_IN 0
#define LIVENESS_OUT 1

typedef std::vector<SgStatement *> stmt_table_vec;

struct CVarNode {
  void *var;
  int appear;
  int first_appear;
  int last_appear;
  bool is_port;
  bool is_local;
  bool do_init[2];  //  2 tags:do_init[0]-pre initialization;do_init[1]-post
                    //  processing
  int is_orig;
  int w_type;
  int self_w_type;
  int buf_depth;
  std::map<int, int> stage_appear;
  CVarNode() {
    var = nullptr;
    appear = 0;
    first_appear = 0;
    last_appear = 0;
    is_port = false;
    is_local = false;
    do_init[0] = do_init[1] = false;
    is_orig = false;
    w_type = 0;
    self_w_type = 0;
    buf_depth = 0;
  }
};

typedef std::map<string, CVarNode> var_table_map;
enum wr_type { NA = 0, RaW = 1, WaW = 2, WaR = 3, RaR = 4 };
class CGraphNode : public stmt_table_vec {
 public:
  void *ref;
  int stage;
  int type;  //  0-for statement; 1-function call; 2-basicblock; 3-if; 4-while;
             //  5-varible declare; 6-others
  std::map<int, enum wr_type>
      dep_map;  //  1- read after write; 2-write after write;
                //  3-write after read; 4-read after read
  std::map<int, std::map<void *, enum wr_type>> port_dep_map;
  std::vector<SgExpression *> write_ref;
  std::vector<SgExpression *> read_ref;
  var_table_map port;
  bool cg_bus;
  CGraphNode() {
    ref = nullptr;
    stage = 0;
    type = 0;
    cg_bus = false;
  }
};

typedef std::map<int, CGraphNode> node_table_map;  //  Table of iterative
                                                   //  variables

void CoarsePipeline(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options);
bool generate_pipeline(CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir,
                       CMarsIrV2 *mars_ir_v2, CMirNode *bNode, CMirNode *fNode,
                       std::string node_num_str);
bool build_graph(CSageCodeGen *codegen, CMirNode *bNode,
                 node_table_map *node_table,
                 std::vector<SgVariableDeclaration *> *var_declare_vec);

bool dependency_analysis(CSageCodeGen *codegen, void *pTopFunc,
                         CMarsIr *mars_ir, CMirNode *bNode, CMirNode *fNode,
                         std::set<void *> var_init_new_set,
                         node_table_map *node_table, var_table_map *var_table);

void print_table(node_table_map *node_table);

bool GetNodeDependence(CSageCodeGen *codegen, CMarsIr *mars_ir, CMirNode *bNode,
                       node_table_map *node_table, CGraphNode *write_node,
                       CGraphNode *read_node, std::set<void *> var_init_new_set,
                       int num_w, int num_r);

bool stage_assignment(CSageCodeGen *codegen, CMirNode *node,
                      node_table_map *node_table, int *stage_sum,
                      var_table_map *var_table,
                      std::set<void *> var_init_new_set);

bool channel_analysis(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *fNode,
                      node_table_map *node_table, var_table_map *var_table,
                      int stage_sum, std::set<void *> var_init_new_set,
                      bool *cg_level_2);

bool code_generation(CSageCodeGen *codegen, CMarsIr *mars_ir,
                     CMarsIrV2 *mars_ir_v2, CMirNode *bNode, CMirNode *fNode,
                     node_table_map *node_table, int stage_sum,
                     std::string node_num_str, var_table_map *var_table,
                     std::set<void *> var_init_new_set, const bool &cg_level_2);

bool parse_ref_normal(CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir,
                      SgNode *stmt, CGraphNode *graph_node, CMirNode *bNode,
                      CMirNode *fNode, var_table_map *var_table, int node_i);

bool isLoopIterator(CSageCodeGen *codegen, void *loop_scope, void *arr_var,
                    void *ref);

//  ///////  / Range analysis

bool test_range_legality(CSageCodeGen *codegen, CMarsIr *mars_ir, void *sg_loop,
                         node_table_map *node_table, void *sg_scope_w,
                         void *sg_scope_r, void *sg_array, int num_w, int num_r,
                         std::string *msg, std::set<void *> var_init_new_set,
                         vector<size_t> *lb_vec, vector<size_t> *ub_vec,
                         bool *bound_check);

bool check_range_flatten(CSageCodeGen *codegen, CMarsIr *mars_ir, void *sg_loop,
                         node_table_map *node_table,
                         const CMarsRangeExpr &flatten_mr_r,
                         const CMarsRangeExpr &flatten_mr_w, void *sg_array,
                         int num_w, int num_r, std::string *msg);

bool check_range_multiple(CSageCodeGen *codegen, CMarsIr *mars_ir,
                          void *sg_loop, node_table_map *node_table,
                          const std::vector<CMarsRangeExpr> &vec_mr_r,
                          const std::vector<CMarsRangeExpr> &vec_mr_w,
                          void *sg_array, int num_w, int num_r,
                          std::string *msg, vector<size_t> *lb_vec,
                          vector<size_t> *ub_vec, bool *bound_check);

bool union_access_range(CSageCodeGen *codegen, node_table_map *node_table,
                        CMarsRangeExpr *mr_merged, bool *merge_tag,
                        void *sg_array, int num_w, int num_r, std::string *msg);

bool union_access_range_multiple(CSageCodeGen *codegen,
                                 node_table_map *node_table,
                                 CMarsRangeExpr *mr_merged, bool *merge_tag,
                                 void *sg_array, int num_w, int num_r,
                                 std::string *msg, int dim);

//  ////////////  /

bool preprocessing(CSageCodeGen *codegen, CMarsIr *mars_ir, CMirNode *bNode,
                   const std::vector<SgVariableDeclaration *> &var_declare_vec,
                   std::vector<void *> *var_decl_old,
                   std::vector<void *> *var_decl_new,
                   std::set<void *> *var_init_new_set, bool *changed);

bool frame_nodes(CSageCodeGen *codegen, node_table_map *node_table,
                 CMirNode *bNode, bool *changed);

bool test_feedback_dependency(CSageCodeGen *codegen, void *sg_array,
                              void *sg_loop, int stage, enum wr_type dep_type,
                              std::set<void *> var_init_new_set);

void reportFeedback(CSageCodeGen *codegen, CMirNode *lnode,
                    std::vector<void *> arr_report, int i, int j);
void reportUnsupportStatement(void *stmt, CMirNode *lnode,
                              CSageCodeGen *codegen);

void reportBusConflict(CMirNode *lnode, CSageCodeGen *codegen);
void reportNonCanonical(void *stmt, CMirNode *lnode, CSageCodeGen *codegen);
void reportLegalityCheck(CSageCodeGen *codegen, CMirNode *lnode, void *sg_array,
                         const std::string &reason, bool local);
void reportOneStatement(CMirNode *lnode, CSageCodeGen *codegen);

void pragma_propagate(CSageCodeGen *codegen, CMarsIr *mars_ir,
                      std::map<void *, std::vector<void *>> existing_table,
                      std::map<void *, std::vector<void *>> gen_table,
                      std::set<void *> var_init_new_set);

bool label_processing(CSageCodeGen *codegen, void *pTopFunc, CMirNode *bNode);

void postprocessing_fail(CSageCodeGen *codegen, CMarsIr *mars_ir,
                         CMirNode *bNode,
                         const std::vector<void *> &var_decl_old,
                         const std::vector<void *> &var_decl_new);

void postprocessing_success(CSageCodeGen *codegen, CMarsIr *mars_ir,
                            CMirNode *bNode,
                            const std::vector<void *> &var_decl_old,
                            const std::vector<void *> &var_decl_new);

void remove_pragmas(CSageCodeGen *codegen, CMirNode *new_node);

void check_bus_access(node_table_map *node_table, bool *cg_level_2);

void build_init_function(CSageCodeGen *codegen, CMirNode *bNode,
                         var_table_map *var_table,
                         map<string, void *> *all_func_map,
                         const string &kernel_str);

void build_node_functions(CSageCodeGen *codegen, CMirNode *bNode,
                          var_table_map *var_table,
                          map<string, void *> *func_map_kernel,
                          map<string, void *> *all_func_map,
                          map<void *, void *> *all_func_call_map,
                          map<string, map<string, void *>> *arg_map_kernel,
                          node_table_map *node_table, int stage_sum,
                          int64_t incr, const string &kernel_str);
bool check_range_bound(CSageCodeGen *codegen, CMarsIr *mars_ir,
                       CMarsRangeExpr *m_range, vector<size_t> *lb_size,
                       vector<size_t> *ub_size);
void partition_pragma_gen_xilinx(CSageCodeGen *codegen, void *arr_init, int dim,
                                 int factor);
