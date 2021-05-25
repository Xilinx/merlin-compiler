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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_COMM_OPT_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_COMM_OPT_H_
#include <map>
#include <set>
#include <vector>
#include <string>

#include "mars_ir_v2.h"
#include "mars_message.h"

#define IVDEP_SPEC_PREFIX "__merlin_ivdep_"
namespace MarsCommOpt {

typedef struct mark_struct {
  bool check_non_canonical;
  bool check_break;
  bool check_ref;
  mark_struct() {
    check_non_canonical = false;
    check_break = false;
    check_ref = false;
  }
} mark_struct;

void assign_communication_type(CMarsIrV2 *mars_ir, bool check_parallel,
                               bool report);
int assign_node_scheduling(CMarsIrV2 *mars_ir,
                           bool detect_shared_memory = false);
int assign_channel_size(CMarsIrV2 *mars_ir);
int false_data_dep_top(CMarsIrV2 *mars_ir, int ivdep_flag);
int channel_gen_top(CMarsIrV2 *mars_ir);
int channel_reorder_top(CMarsIrV2 *mars_ir);
void gen_one_channel(CMarsAST_IF *codegen, void *ref_w, void *ref_r,
                     string channel_name, int channel_depth, int width,
                     void *channel_base_type, void *write_value = nullptr,
                     int parallel_bit = 0);
int generate_reorder_channel(CMarsAST_IF *codegen, CMarsEdge *edge);

/*comm_token*/
int shared_mem_gen_top(CMarsIrV2 *mars_ir);
//  int gen_token(CMarsAST_IF  &codegen, CMarsEdge *edge, int t_c, bool
//  back_dep, int common_level);
int gen_token(CMarsAST_IF *codegen, CMarsNode *node0, CMarsNode *node1, int t_c,
              bool back_dep, int common_level, string token_info);
int gen_token_write(CMarsAST_IF *codegen, CMarsNode *node, void *new_var,
                    int t_c, bool back_dep, int common_level,
                    string token_info);
//        void *array);
int gen_token_read(CMarsAST_IF *codegen, CMarsNode *node, void *new_var,
                   void *new_dummy, int t_c, bool back_dep, int common_level,
                   string token_info);
//    void *array);
int build_edge2chain(CMarsAST_IF *codegen, CMarsIrV2 *mars_ir,
                     map<CMarsEdge *, int> *edge2chain);
void build_seq_length(CMarsAST_IF *codegen, CMarsIrV2 *mars_ir,
                      map<void *, int> *maxlength);
bool has_sync_edge(CMarsIrV2 *mars_ir, CMarsNode *node0, CMarsNode *node1,
                   int sync_level);
int get_max_length(vector<CMarsNode *> t_nodes, int level, CMarsNode *node_t);

/*==========comm_refine==========*/
int comm_refine_top(CMarsIrV2 *mars_ir);
void refine_write_ref(CSageCodeGen *codegen, CMarsNode *node, void *port,
                      void *ref, vector<void *> loops, void *new_var,
                      bool write_out, bool read_in, int refine_type,
                      void *parent_scope);
void refine_read_ref(CSageCodeGen *codegen, CMarsNode *node, void *port,
                     void *ref, vector<void *> loops, void *new_var,
                     bool read_in, int refine_type, void *parent_scope);
bool test_parallel(CSageCodeGen *codegen, CMarsIrV2 *mars_ir, CMarsNode *node,
                   void *ref);
void *get_public_scope(CSageCodeGen *codegen, set<void *> refs0);

void refine_transformation(CSageCodeGen *codegen, CMarsNode *node, void *port,
                           const map<void *, vector<void *>> &refine_iter_map,
                           int port_count, int edge_loc, int refine_type);
int test_common_loop(CMarsIrV2 *mars_ir);
bool test_identical_index(CSageCodeGen *codegen,
                          const map<void *, vector<void *>> &refine_iter_map);
bool test_identical_index(CSageCodeGen *codegen, const set<void *> &refs);

void loop_classification(CSageCodeGen *codegen, CMarsNode *node, void *ref,
                         vector<void *> loops,
                         map<void *, int> *unrelated_parallel_loops,
                         map<void *, int> *related_parallel_loops,
                         map<void *, int> *related_unparallel_loops);

/*==========comm_sync==========*/
int comm_sync_top(CMarsIrV2 *mars_ir, void *pTopFunc);
int test_parallel_validity(CSageCodeGen *codegen, CMarsNode *node,
                           map<void *, int> loop2parallel,
                           map<void *, int> loop2spread, void *port);

int analyze_parallelism(CMarsIrV2 *mars_ir, CMarsEdge *edge,
                        map<void *, int> *loop2parallel,
                        const map<void *, int> &loop2spread,
                        int *parallel_level0, int *parallel_level1);

void insert_parallelism(CMarsIrV2 *mars_ir, CMarsEdge *edge,
                        const map<void *, int> &loop2parallel,
                        map<void *, int> *loop2spread,
                        map<void *, int> *node2level,
                        const int &parallel_level0, const int &parallel_level1);

int check_propagate_fin(CMarsIrV2 *mars_ir, CMarsEdge *edge,
                        const map<void *, int> &loop2spread);

int check_parallel_bits(CMarsIrV2 *mars_ir, CMarsEdge *edge, int *is_parallel);
int check_parallel_match(CMarsIrV2 *mars_ir, CMarsEdge *edge);
int check_reorder_flag(CMarsEdge *edge);
//  Message dumping
void reportNonCanonical(void *sg_loop, CSageCodeGen *codegen);
void reportCGParallel(void *sg_loop, void *port, CSageCodeGen *codegen);
void reportIllegalRef(void *ref, void *port, CSageCodeGen *codegen);

int comm_rename_top(CMarsIrV2 *mars_ir);

bool check_dead_node(CMarsNode *);

bool check_node(CMarsNode *node, map<void *, mark_struct> *loop2mark,
                CMarsIrV2 *mars_ir, bool pre_check = false);
// coarse_pipeline_altera
void assign_schedule_for_marsIR_nodes(CMarsIrV2 *mars_ir,
                                      bool detect_shared_memory = false);
}  //  namespace MarsCommOpt
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_COMM_OPT_H_
