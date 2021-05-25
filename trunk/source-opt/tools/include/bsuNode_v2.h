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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BSUNODE_V2_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BSUNODE_V2_H_

#include <list>
#include <map>
#include <utility>
#include <set>
#include <vector>
#include <string>

#include "analPragmas.h"
#include "codegen.h"
#include "ir_types.h"
#include "program_analysis.h"
#include "rose.h"

class VEC_2L_PLUS_ONE : public std::vector<int> {
 public:
  int is_zero();
};

typedef VEC_2L_PLUS_ONE CMarsDepVector;
typedef VEC_2L_PLUS_ONE CMarsScheduleVector;
typedef VEC_2L_PLUS_ONE CMarsOrderVector;

int get_sync_level_from_schedule(CMarsScheduleVector sch0,
                                 CMarsScheduleVector sch1);

/* youxiang:
 *CMarsNodeV2 represents a group of continuous statements in the
 *same loop hierarchy
 **** mLoops: represent its iterator domain
 **** mStmts: represent its statement segment
 **** mOrders: represent its sync point and pipeline stage (2L)
 ***********   2*n: sync point / location
 **********    2*n + 1: pipeline stage
 **** mPorts: represent its all external variables
 * */
enum dep_type {
  NO_DEP = 0,
  RAW = 1 << ((READ << 2) | WRITE),
  RAR = 1 << ((READ << 2) | READ),
  WAW = 1 << ((WRITE << 2) | WRITE),
  WAR = 1 << ((WRITE << 2) | WRITE),
  RWAW = 1 << ((READWRITE << 2) | WRITE),
  RWAR = 1 << ((READWRITE << 2) | READ),
  RWARW = 1 << ((READWRITE << 2) | READWRITE),
  WARW = 1 << ((WRITE << 2) | READWRITE),
  RARW = 1 << ((READ << 2) | READWRITE),
};
#define IS_WRITE_DEP(dep) ((dep) & (RAW | WAW | RWAW | RWARW | WARW | RARW))

class CMarsNode {
 private:
  std::vector<void *> mLoops;  //  surrounding loops: outer to inner

  std::map<void *, std::vector<pair<void *, bool>>>
      mLoop2Conds;  //  loop conditions map
  std::vector<pair<void *, bool>> mInnermostConds;
  std::vector<void *> mStmts;           //
  std::vector<int> mOrders;             //  2L+1 representation
  CMarsScheduleVector mSchedule;        //  2L+2 representation
  CMarsScheduleVector mSchedule_depth;  //  2L+2 representation
  std::map<int, int> mLoop2Schedule;    //  2L+1 representation
  t_func_call_path mCallPath;
  SetVector<void *> mPorts;
  std::map<void *, access_type> mPort2AccessType;
  std::map<void *, std::set<void *>> mPort2Pntr;
  std::map<void *, std::set<void *>> mPort2Refs;
  CSageCodeGen *m_ast;
  void *mUserKernel;

 private:
  //  void init();
  CMarsNode();
  void *get_top_instance(void *var_init);  //  trace to local/global variable
                                           //  initialized name, or top kernel
                                           //  function argument

 public:
  CMarsNode(CSageCodeGen *codegen, const vector<void *> &stmts,
            const vector<void *> &loops,
            const map<void *, vector<pair<void *, bool>>> &loop2conds,
            const vector<pair<void *, bool>> &conds, const vector<int> &orders,
            const t_func_call_path &call_path, void *kernel);
  ~CMarsNode() {}
  void print();
  string unparse();
  CMarsAST_IF *get_ast() { return m_ast; }
  std::vector<void *> get_common_loops(CMarsNode *other);
  bool has_common_loops(CMarsNode *other);
  std::set<void *> get_common_ports(CMarsNode *other);
  bool is_after(CMarsNode *other);
  bool is_subNode(void *kernel_top, const std::vector<int> &order_prefix);
  auto get_ports() const { return mPorts; }
  std::set<void *> get_port_references(void *port) { return mPort2Pntr[port]; }
  std::set<void *> get_port_refs(void *port) { return mPort2Refs[port]; }
  void remove_port(void *port);
  //  std::vector<MarsProgramAnalysis::CMarsRangeExpr>
  //  get_port_access_union(void *port);

  std::vector<MarsProgramAnalysis::CMarsRangeExpr>
  get_port_access_union(void *port, void *range_scope, bool read);

  void set_ports(const std::set<void *> &ports);

  std::vector<void *> get_stmts() const { return mStmts; }

  int get_loop_depth() const { return mLoops.size(); }

  int get_loop_level(void *loop) const;

  void *get_loop(int depth) const;

  bool has_loops() const { return mLoops.size() > 0; }
  std::vector<void *> get_loops() const { return mLoops; }
  std::vector<void *> get_inner_loops() const;
  std::vector<void *> get_inner_loops(void *ref);
  bool contains(void *loop) const;
  std::vector<int> get_orders() const { return mOrders; }
  int get_order(size_t depth) const;

  std::vector<pair<void *, bool>> get_loop_conditions(void *loop) {
    return mLoop2Conds[loop];
  }

  void set_schedule(int level, int sched) {
    mLoop2Schedule[level] = sched;
    return;
  }

  void set_schedule(int sched) {
    mSchedule.push_back(sched);
    return;
  }

  void set_schedule_depth(size_t level, int depth);

  std::map<int, int> get_loop2schedule() { return mLoop2Schedule; }
  int get_loop2schedule(int level);

  CMarsScheduleVector get_schedule() const { return mSchedule; }
  int get_schedule(size_t level) const;
  int get_schedule_depth(size_t level) const;

  void schedule_map2vec();

  access_type get_port_access_type(void *port) const;
  bool port_is_write(void *port) const {
    auto acc_type = get_port_access_type(port);
    return acc_type == WRITE || acc_type == READWRITE;
  }

  bool port_is_read(void *port) const {
    auto acc_type = get_port_access_type(port);
    return acc_type == READ || acc_type == READWRITE;
  }

  void *get_user_kernel() const { return mUserKernel; }

  //  data dependency
  void get_dependency(CMarsNode *other, std::map<void *, dep_type> *res);
  //  xxx  after write
  bool has_direct_write_dependency(CMarsNode *other);
  friend void get_dependency(const std::vector<CMarsNode *> &one,
                             const std::vector<CMarsNode *> &other,
                             std::map<void *, dep_type> *res);

  //  code generation
  void code_generation(void *insert_before = nullptr);

  void remove_all_statements(bool keep_assert_stmt = false);
  //  ////////////////////////////////////  /
  //  Auxilliary Functions
  //  ////////////////////////////////////  /

  //  Analyze conditions
  //  Assumptions:
  //  a) if statement, not else
  //  b) the condition is i==0 or i==N-1, i==...
  //  Input: pos can be an referece or a statement
  //  Output: whether the if-stmt is in the simple form
  bool condition_is_simple(void *pos);

  //  for-loop -> (position, equal) (0: before the loop, 1: after the loop, 2:
  //  in the middle iterations, 3: unknown pos)
  std::map<void *, pair<int, bool>> get_simple_condition(void *pos);

  //  return the ID size
  MarsProgramAnalysis::CMarsExpression
  get_iteration_domain_size_by_port_and_level(void *port, int sync_level);
  MarsProgramAnalysis::CMarsExpression
  get_iteration_domain_size_by_port_and_level(void *port, int sync_level,
                                              int parallel_level);

  void update_reference();
  void insert_statement(void *new_stmt, void *old_stmt, bool before = true);

  void *get_scope();

  t_func_call_path get_call_path_in_scope(void *scope);

  t_func_call_path get_call_path() { return mCallPath; }

  bool is_real_shared_port(CMarsNode *other, void *port);

  bool is_dead() const;
};

enum comm_type {
  COMMTYPE_SHARED_MEM_DDR,    //  0 default
  COMMTYPE_CHANNEL_FIFO,      //  1
  COMMTYPE_HOST_IF,           //  2
  COMMTYPE_KERNEL_LOCAL,      //  3
  COMMTYPE_CHANNEL_FIFO_INV,  //  4
  COMMTYPE_UNKNOWN
};

class CMarsEdge {
 protected:
  std::vector<CMarsNode *> mNodes;
  void *mPort;
  CMarsDepVector *mDepVector;
  CMarsAST_IF *m_ast;

  enum comm_type m_comm_type;

  std::map<string, string> m_properties;

  bool m_ssst;  //  mark to be single source single target

 protected:
  void update_dependence();

 public:
  void clear_dep_cache();

  ~CMarsEdge() { clear_dep_cache(); }

  CMarsEdge(CMarsAST_IF *ast, CMarsNode *node0, CMarsNode *node1, void *port);

  CMarsAST_IF *get_ast() { return m_ast; }
  CMarsNode *GetNode(int index);
  void *GetPort() { return mPort; }
  CMarsDepVector GetDependence();

  bool is_ssst() { return m_ssst; }

  void set_ssst(bool flag) { m_ssst = flag; }

  string print();

  enum comm_type GetCommType() { return m_comm_type; }
  void SetCommType(enum comm_type type) {
    m_comm_type = type;
    SetProperty("comm_type", my_itoa(static_cast<int>(m_comm_type)));
  }

  //  Properties summary
  //  channel_size: non-neg int
  //  variable_name: string_id
  //  sync_level: non-neg int
  //  comm_type: 0..5
  //  reordering_flag: 0 or 1
  //  access_type: raw, rar, waw, war
  void SetProperty(string key, string value) { m_properties[key] = value; }
  string GetProperty(string key) { return m_properties[key]; }
};

class CMarsLoop {
  //  main structure
  void *ref;  //  A pointer to the loop *
  std::set<void *> pragma_table;
  std::vector<CAnalPragma> vec_pragma;
  CMarsAST_IF *m_ast;
  int64_t m_trip_count;
  int64_t m_max_trip_count;

 public:
  CMarsLoop(void *loop, CMarsAST_IF *ast)
      : ref(loop), m_ast(ast), m_trip_count(-1), m_max_trip_count(-1) {}
  ~CMarsLoop() {}

  int has_opt_pragmas();
  int has_pipeline();
  int has_parallel();

  int is_complete_unroll();

  int is_partial_unroll(int *factor);

  int is_tiled_loop() {
    for (auto pragma : vec_pragma)
      if (pragma.is_loop_tiled_tag())
        return true;
    return false;
  }

  void analyze_pragma(void *decl);
  void set_trip_count(int64_t trip_count, int64_t max_trip_count);
  int64_t get_trip_count() const { return m_trip_count; }
  int64_t get_max_trip_count() const { return m_max_trip_count; }
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BSUNODE_V2_H_
