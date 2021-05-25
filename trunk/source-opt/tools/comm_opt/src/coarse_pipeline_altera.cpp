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

#include "bsuNode_v2.h"
#include "codegen.h"
#include "comm_opt.h"
#include "mars_ir_v2.h"
#include "program_analysis.h"
#include "rose.h"
#undef DEBUG
#if 1
#define DEBUG(x)
#else
#define DEBUG(x) x
#endif
#define USED_CODE_IN_COVERAGE_TEST 0

namespace MarsCommOpt {

class CMarsIrTree {
  void *mScope;
  bool mLeaf_node;
  CMarsNode *mNode;
  int mScopeLevel;
  int mMaxSync;
  int mMaxStage;
  vector<CMarsIrTree *> mVec_sub_tree;
  //  2 * L synchronization point
  map<CMarsIrTree *, int> mSync_sched;
  //  2 * L + 1 pipeline stage
  map<CMarsIrTree *, int> mPipe_sched;

  void build_tree(CMarsIrV2 *mars_ir);

 public:
  CMarsIrTree(void *scope, bool leaf, CMarsNode *node, CMarsIrV2 *mars_ir)
      : mScope(scope), mLeaf_node(leaf), mNode(node), mScopeLevel(-1),
        mMaxSync(0), mMaxStage(0) {
    if (!leaf) {
      build_tree(mars_ir);
    }
  }
  bool isLeafNode() { return mLeaf_node; }
  void *GetScope() { return mScope; }
  CMarsNode *GetSingleNode() { return mNode; }
  vector<CMarsIrTree *> GetSubTree() { return mVec_sub_tree; }
  CMarsIrTree *GetSubTree(size_t idx) {
    assert(idx < mVec_sub_tree.size());
    return mVec_sub_tree[idx];
  }
  vector<CMarsNode *> GetSubNodesfromTree(CMarsIrV2 *mars_ir) {
    vector<CMarsNode *> res;
    if (isLeafNode()) {
      res.push_back(mNode);
    } else if (mars_ir->get_ast()->IsForStatement(mScope) != 0) {
      res = mars_ir->get_nodes_with_common_loop(mScope);
    } else {
      res = mars_ir->get_all_nodes();
    }
    return res;
  }

  int GetScopeLevel() { return mScopeLevel; }

  size_t GetNumSubTree() { return mVec_sub_tree.size(); }
  void release_sub_tree() {
    for (auto &sub_tree : mVec_sub_tree) {
      delete sub_tree;
    }
    mVec_sub_tree.clear();
  }
  ~CMarsIrTree() { release_sub_tree(); }
  void set_sync_value(CMarsIrTree *sub_tree, int value) {
    assert(mSync_sched.find(sub_tree) == mSync_sched.end());
    mSync_sched[sub_tree] = value;
    mMaxSync = std::max(mMaxSync, value);
  }

  int get_sync_value(CMarsIrTree *sub_tree) {
    assert(mSync_sched.find(sub_tree) != mSync_sched.end());
    return mSync_sched[sub_tree];
  }

  void set_pipeline_value(CMarsIrTree *sub_tree, int value) {
    if (mPipe_sched.find(sub_tree) == mPipe_sched.end()) {
      mPipe_sched[sub_tree] = value;
    } else {
      assert(mPipe_sched[sub_tree] <= value && "break dependency");
      mPipe_sched[sub_tree] = value;
    }
    mMaxStage = std::max(mMaxStage, value);
  }

  int get_pipeline_value(CMarsIrTree *sub_tree) {
    assert(mPipe_sched.find(sub_tree) != mPipe_sched.end());
    return mPipe_sched[sub_tree];
  }

  int get_max_sync_point() const { return mMaxSync; }

  int get_max_pipeline_stage() const { return mMaxStage; }

#if USED_CODE_IN_COVERAGE_TEST
  void print(CMarsIrV2 *mars_ir, int curr_level) {
    if (isLeafNode()) {
      for (int i = 0; i < curr_level; ++i)
        cout << ' ';
      cout << "node" << mars_ir->get_node_idx(mNode) << endl;
    } else {
      int i = 0;
      for (auto &sub_tree : mVec_sub_tree) {
        for (int i = 0; i < curr_level; ++i)
          cout << ' ';
        cout << "sub" << curr_level << i++ << endl;
        sub_tree->print(mars_ir, curr_level + 1);
      }
    }
  }
#endif
};

enum CMARS_EDGE_TYPE { SYNC_TAG = 0, PIPE_TAG = 1, REORDER_TAG = 2 };

void CMarsIrTree::build_tree(CMarsIrV2 *mars_ir) {
  CSageCodeGen *codegen = mars_ir->get_ast();
  vector<CMarsNode *> sub_nodes;
  mScopeLevel = 0;
  if (codegen->IsForStatement(mScope) != 0) {
    sub_nodes = mars_ir->get_nodes_with_common_loop(mScope);
    mScopeLevel = sub_nodes[0]->get_loop_level(mScope) + 1;
  } else {
    sub_nodes = mars_ir->get_all_nodes();
  }
  set<void *> visited_loop;
  for (auto &node : sub_nodes) {
    auto loops = node->get_loops();
    CMarsIrTree *sub_tree = nullptr;
    if (static_cast<size_t>(mScopeLevel) < loops.size()) {
      auto sub_loop = loops[mScopeLevel];
      if (visited_loop.count(sub_loop) > 0) {
        continue;
      }
      visited_loop.insert(sub_loop);
      sub_tree = new CMarsIrTree(sub_loop, false, nullptr, mars_ir);
    } else {
      sub_tree = new CMarsIrTree(mScope, true, node, mars_ir);
    }
    mVec_sub_tree.push_back(sub_tree);
  }
}

void build_sync_graph(map<CMarsIrTree *, map<CMarsIrTree *, bool>> *sync_graph,
                      CMarsIrTree *root, CMarsIrV2 *mars_ir,
                      bool detect_shared_memory) {
  map<CMarsIrTree *, map<CMarsIrTree *, int>> inter_sync_graph;
#if PROJDEBUG
  CSageCodeGen *codegen = mars_ir->get_ast();
#endif
  bool has_sync_tag = false;
  bool has_shared_memory_tag = false;
  for (auto &sub_tree0 : root->GetSubTree()) {
    vector<CMarsNode *> nodes0 = sub_tree0->GetSubNodesfromTree(mars_ir);
    for (auto &sub_tree1 : root->GetSubTree()) {
      if (sub_tree0 == sub_tree1) {
        continue;
      }
      bool seq_tag = false;
      bool pipe_tag = false;
      bool reorder_tag = false;
      vector<CMarsNode *> nodes1 = sub_tree1->GetSubNodesfromTree(mars_ir);
      if (!nodes1[0]->is_after(nodes0[0])) {
        continue;
      }

      for (auto &t_node0 : nodes0) {
        for (auto &t_node1 : nodes1) {
          map<void *, dep_type> res;
          t_node0->get_dependency(t_node1, &res);
#if PROJDEBUG
          int idx0 = mars_ir->get_node_idx(t_node0);
          int idx1 = mars_ir->get_node_idx(t_node1);
#endif
          set<void *> shared_ports = t_node0->get_common_ports(t_node1);
          if (static_cast<unsigned int>(!res.empty()) != 0U) {
            for (auto &dep : res) {
              void *port = dep.first;
              if (shared_ports.find(port) == shared_ports.end()) {
                continue;
              }
              if (mars_ir->skip_synchronization(port)) {
                continue;
              }
              CMarsEdge *m_edge =
                  mars_ir->get_edge_by_node(t_node0, t_node1, port);
              if (m_edge->GetCommType() == COMMTYPE_HOST_IF ||
                  m_edge->GetCommType() == COMMTYPE_SHARED_MEM_DDR) {
#if PROJDEBUG
                cout << "Sequence reason: shared mem between node " << idx0
                     << " and " << idx1 << endl;
                cout << "Port: " << codegen->_p(port) << endl;
#endif
                seq_tag = true;
                //  youxiang 2016-11-11 disable all synchronization on both
                //  HOST_IF and SHARED_MEM
                //  if (m_edge->GetCommType() == COMMTYPE_SHARED_MEM_DDR) {
                //  cout << "found shared mem" << endl;
                has_shared_memory_tag = true;
                //  }
              } else if (m_edge->GetCommType() == COMMTYPE_CHANNEL_FIFO) {
                if (MarsCommOpt::check_reorder_flag(m_edge) != 0) {
                  reorder_tag = true;
                }
#if PROJDEBUG
                cout << "dependence between node " << idx0 << " and " << idx1
                     << endl;
                cout << "Channel: " << codegen->_p(port) << endl;
#endif
                pipe_tag = true;
              }
            }
          }
        }
      }

      if (seq_tag) {
        has_sync_tag = true;
        inter_sync_graph[sub_tree1][sub_tree0] = SYNC_TAG;
      } else if (reorder_tag) {
        inter_sync_graph[sub_tree1][sub_tree0] = REORDER_TAG;
      } else if (pipe_tag) {
        inter_sync_graph[sub_tree1][sub_tree0] = PIPE_TAG;
      }
    }
  }
  for (auto &edge_group : inter_sync_graph) {
    for (auto &edge : edge_group.second) {
      //  if there is sequential edge, we will change reorder edge
      //  into sequential edge. Otherwise, it will get stuck
      if ((edge.second == REORDER_TAG && has_sync_tag) ||
          edge.second == SYNC_TAG) {
        (*sync_graph)[edge_group.first][edge.first] = true;
        if (edge.second == REORDER_TAG) {
          has_shared_memory_tag = true;
        }
      } else {
        (*sync_graph)[edge_group.first][edge.first] = false;
      }
    }
  }
  if (has_shared_memory_tag && detect_shared_memory) {
    cout << "found shared memory in current scheduling result\n";
    system("touch merlin_shared_memory.log");
  }
}

void calculate_sync_schedule(
    map<CMarsIrTree *, map<CMarsIrTree *, bool>> *sync_graph, CMarsIrTree *root,
    CMarsIrV2 *mars_ir) {
  //  sync_graph
  //  pair<tree0, tree1> : tree0 depends on tree1
  set<CMarsIrTree *> visited;
  vector<CMarsIrTree *> remain_trees = root->GetSubTree();
  while (!remain_trees.empty()) {
    vector<CMarsIrTree *> candidates = remain_trees;
    remain_trees.clear();
    vector<CMarsIrTree *> output_trees;
    for (auto &sub_tree : candidates) {
      int curr_sync_point = 0;
      bool all_edge_resolved = true;
      for (auto &edge : (*sync_graph)[sub_tree]) {
        CMarsIrTree *prev_tree = edge.first;
        bool sync_edge = edge.second;
        if (visited.count(prev_tree) <= 0) {
          all_edge_resolved = false;
          break;
        }
        int prev_sync_point = root->get_sync_value(prev_tree);
        //  sync edge: increase by 1 (>)
        //  pipeline edge: >=
        if (sync_edge) {
          prev_sync_point++;
        }
        curr_sync_point = std::max(prev_sync_point, curr_sync_point);
      }
      if (all_edge_resolved) {
        root->set_sync_value(sub_tree, curr_sync_point);
        visited.insert(sub_tree);
      } else {
        remain_trees.push_back(sub_tree);
      }
    }
#if USED_CODE_IN_COVERAGE_TEST
    if (remain_trees.size() >= candidates.size()) {
      cout << "remaining trees" << endl;
      int i = 0;
      for (auto &sub_tree : remain_trees) {
        cout << i++ << "th substree:" << endl;
        sub_tree->print(mars_ir, 0);
      }
      cout << "..............." << endl;
    }
#endif
    assert(remain_trees.size() < candidates.size() &&
           "feedback dependency exits");
  }
}

void build_pipeline_graph(map<CMarsIrTree *, set<CMarsIrTree *>> *pipe_graph,
                          CMarsIrTree *root, CMarsIrV2 *mars_ir) {
#if PROJDEBUG
  CSageCodeGen *codegen = mars_ir->get_ast();
#endif
  for (auto &sub_tree0 : root->GetSubTree()) {
    vector<CMarsNode *> nodes0 = sub_tree0->GetSubNodesfromTree(mars_ir);
    for (auto &sub_tree1 : root->GetSubTree()) {
      if (sub_tree0 == sub_tree1) {
        continue;
      }
      int sync_value0 = root->get_sync_value(sub_tree0);
      int sync_value1 = root->get_sync_value(sub_tree1);
      if (sync_value0 != sync_value1) {
        continue;
      }
      bool seq_tag = false;
      bool pipe_tag = false;
      vector<CMarsNode *> nodes1 = sub_tree1->GetSubNodesfromTree(mars_ir);
      if (!nodes1[0]->is_after(nodes0[0])) {
        continue;
      }

      for (auto &t_node0 : nodes0) {
        for (auto &t_node1 : nodes1) {
          map<void *, dep_type> res;
          t_node0->get_dependency(t_node1, &res);
          set<void *> shared_ports = t_node0->get_common_ports(t_node1);
#if PROJDEBUG
          int idx0 = mars_ir->get_node_idx(t_node0);
          int idx1 = mars_ir->get_node_idx(t_node1);
#endif
          if (static_cast<unsigned int>(!res.empty()) != 0U) {
            for (auto &dep : res) {
              void *port = dep.first;
              if (shared_ports.find(port) == shared_ports.end()) {
                continue;
              }
              if (mars_ir->skip_synchronization(port)) {
                continue;
              }
              CMarsEdge *m_edge =
                  mars_ir->get_edge_by_node(t_node0, t_node1, port);
              if (m_edge->GetCommType() == COMMTYPE_HOST_IF ||
                  m_edge->GetCommType() == COMMTYPE_SHARED_MEM_DDR) {
#if PROJDEBUG
                cout << "Sequence reason: shared mem between node " << idx0
                     << " and " << idx1 << endl;
                cout << "Port: " << codegen->_p(port) << endl;
#endif
                seq_tag = true;
              } else if (m_edge->GetCommType() == COMMTYPE_CHANNEL_FIFO) {
                pipe_tag = true;
#if PROJDEBUG
                cout << "Dependency between node " << idx0 << " and " << idx1
                     << endl;
                cout << "Channel: " << codegen->_p(port) << endl;
#endif
              }
            }
          }
        }
      }
      if (!seq_tag && pipe_tag) {
        //       cout << "pipe here\n";
        (*pipe_graph)[sub_tree1].insert(sub_tree0);
      }
    }
  }
}

void calculate_pipeline_schedule(
    map<CMarsIrTree *, set<CMarsIrTree *>> *pipe_graph, CMarsIrTree *root,
    CMarsIrV2 *mars_ir) {
  //  pipe_graph
  //  pair<tree0, tree1> : tree0 depends on tree1
  //  reverse_pipe_graph
  map<CMarsIrTree *, set<CMarsIrTree *>> reverse_pipe_graph;
  for (auto &dep : (*pipe_graph)) {
    for (auto &tree : dep.second) {
      reverse_pipe_graph[tree].insert(dep.first);
    }
  }

  //  divide all the subtree into pipelined group according to sync point
  map<int, vector<CMarsIrTree *>> pipeline_group;
  for (auto &sub_tree : root->GetSubTree()) {
    pipeline_group[root->get_sync_value(sub_tree)].push_back(sub_tree);
  }

  for (auto &one_group : pipeline_group) {
    vector<CMarsIrTree *> remain_trees = one_group.second;
    set<CMarsIrTree *> visited;
    int max_stage = 0;
    //  propagate from top to bottem to calculate the minimum stage
    while (!remain_trees.empty()) {
      vector<CMarsIrTree *> candidates = remain_trees;
      remain_trees.clear();
      for (auto &sub_tree : candidates) {
        bool all_edge_resolved = true;
        int curr_pipe_stage = 0;
        for (auto &prev_tree : (*pipe_graph)[sub_tree]) {
          if (visited.count(prev_tree) <= 0) {
            all_edge_resolved = false;
            break;
          }
          int prev_pipe_stage = root->get_pipeline_value(prev_tree);
          curr_pipe_stage = std::max(curr_pipe_stage, prev_pipe_stage + 1);
        }
        if (all_edge_resolved) {
          visited.insert(sub_tree);
          root->set_pipeline_value(sub_tree, curr_pipe_stage);
          max_stage = std::max(max_stage, curr_pipe_stage);
        } else {
          remain_trees.push_back(sub_tree);
        }
      }
#if USED_CODE_IN_COVERAGE_TEST
      if (remain_trees.size() >= candidates.size()) {
        cout << "remaining trees" << endl;
        int i = 0;
        for (auto &sub_tree : remain_trees) {
          cout << i++ << "th substree:" << endl;
          sub_tree->print(mars_ir, 0);
        }
        cout << "..............." << endl;
      }
#endif
      assert(remain_trees.size() < candidates.size() &&
             "feedback dependency exits");
    }
    //  propgate from bottom to up to get the maximum stage to
    //  reduce the stage distance
    remain_trees = one_group.second;
    visited.clear();
    while (!remain_trees.empty()) {
      vector<CMarsIrTree *> candidates = remain_trees;
      remain_trees.clear();
      for (auto &sub_tree : candidates) {
        bool all_edge_resolved = true;
        int curr_pipe_stage = max_stage;
        for (auto &post_tree : reverse_pipe_graph[sub_tree]) {
          if (visited.count(post_tree) <= 0) {
            all_edge_resolved = false;
            break;
          }
          int post_pipe_stage = root->get_pipeline_value(post_tree);
          curr_pipe_stage = std::min(curr_pipe_stage, post_pipe_stage - 1);
        }
        if (all_edge_resolved) {
          visited.insert(sub_tree);
          //  check whether the substree is last one
          if (curr_pipe_stage < max_stage) {
            root->set_pipeline_value(sub_tree, curr_pipe_stage);
          }
        } else {
          remain_trees.push_back(sub_tree);
        }
      }
#if USED_CODE_IN_COVERAGE_TEST
      if (remain_trees.size() >= candidates.size()) {
        cout << "remaining trees" << endl;
        int i = 0;
        for (auto &sub_tree : remain_trees) {
          cout << i++ << "th substree:" << endl;
          sub_tree->print(mars_ir, 0);
        }
        cout << "..............." << endl;
      }
#endif
      assert(remain_trees.size() < candidates.size() &&
             "feedback dependency exits");
    }
  }
}

void update_local_schedule(CMarsIrTree *root, CMarsIrV2 *mars_ir,
                           bool detect_shared_memory) {
  if (root->isLeafNode()) {
    return;
  }
  //  build syncronization graph
  //  sync edge: true
  //  pipeline edge: false
  map<CMarsIrTree *, map<CMarsIrTree *, bool>> sync_graph;
  build_sync_graph(&sync_graph, root, mars_ir, detect_shared_memory);
  //  determine 2 * l value by using toposort
  calculate_sync_schedule(&sync_graph, root, mars_ir);
  //  build dependency graph for each sync point
  map<CMarsIrTree *, set<CMarsIrTree *>> pipe_graph;
  build_pipeline_graph(&pipe_graph, root, mars_ir);
  //  determine 2 * l + 1 value for nodes with same sync point
  calculate_pipeline_schedule(&pipe_graph, root, mars_ir);
}

void propagate_schedule(CMarsIrTree *root, CMarsIrTree *sub_tree,
                        CMarsIrV2 *mars_ir) {
  vector<CMarsNode *> sub_nodes = sub_tree->GetSubNodesfromTree(mars_ir);
  CMarsNode *node0 = sub_nodes[0];
  int loop_level = 0;
  void *root_scope = root->GetScope();
  if (mars_ir->get_ast()->IsForStatement(root_scope) != 0) {
    loop_level = node0->get_loop_level(root_scope) + 1;
  }
  int sync_point = root->get_sync_value(sub_tree);
  int pipe_stage = root->get_pipeline_value(sub_tree);
  int max_sync_point = root->get_max_sync_point();
  int max_pipe_stage = root->get_max_pipeline_stage();
  for (auto &node : sub_nodes) {
    node->set_schedule(2 * loop_level, sync_point);
    node->set_schedule_depth(2 * loop_level, max_sync_point);
    node->set_schedule(2 * loop_level + 1, pipe_stage);
    node->set_schedule_depth(2 * loop_level + 1, max_pipe_stage);
  }
}

void print_schedule(vector<CMarsNode *> t_nodes) {
  int node_num = t_nodes.size();
  for (int j = 0; j < node_num; ++j) {
    CMarsNode *node = t_nodes[j];
    vector<void *> m_loops = node->get_loops();
    map<int, int> sched = node->get_loop2schedule();
    cout << "[schedule](sync point, pipeline stage) node" << j << " = (";
    if (sched.find(0) != sched.end()) {
      cout << sched[0] << "," << sched[1];
    } else {
      cout << "*,*";
    }
    cout << ",";

    for (size_t i = 1; i <= m_loops.size(); ++i) {
      if (sched.find(i * 2 + 1) != sched.end()) {
        cout << sched[i * 2] << "," << sched[i * 2 + 1];
      } else {
        cout << "*,*";
      }
      cout << ",";
    }
    cout << ") " << endl;
  }
}
void assign_schedule_for_marsIR_nodes(CMarsIrV2 *mars_ir,
                                      bool detect_shared_memory) {
  std::cout << "==============================================" << std::endl;
  std::cout << "-----=# Coarse Pipeline Optimization Start#=----\n";
  if (detect_shared_memory) {
    std::cout << "-----=#detect shared mmeory#=----\n";
  }
  std::cout << "==============================================" << std::endl;
  //  find the topology level by level
  //  each iteration
  //    root->children
  size_t node_num = mars_ir->size();
  if (node_num <= 0) {
    return;
  }
  CMarsNode *node0 = mars_ir->get_node(0);
  void *kernel_top = node0->get_user_kernel();
  //  1 build top mars ir tree
  CMarsIrTree *mars_ir_tree =
      new CMarsIrTree(kernel_top, false, nullptr, mars_ir);
#if USED_CODE_IN_COVERAGE_TEST
  DEBUG(mars_ir_tree->print(mars_ir, 0);)
#endif
  //  2 push top tree into queue
  std::queue<CMarsIrTree *> ir_tree_q;
  ir_tree_q.push(mars_ir_tree);
  while (!ir_tree_q.empty()) {
    CMarsIrTree *root = ir_tree_q.front();
    ir_tree_q.pop();
    //  determine local scheduling vector elements (l level)
    update_local_schedule(root, mars_ir, detect_shared_memory);
    for (auto &sub_tree : root->GetSubTree()) {
      //  propagete parents' scheduing vector into children's
      propagate_schedule(root, sub_tree, mars_ir);
      //  push the subtree nto the queue
      ir_tree_q.push(sub_tree);
    }
  }

  vector<CMarsNode *> nodes = mars_ir_tree->GetSubNodesfromTree(mars_ir);
  print_schedule(mars_ir->get_all_nodes());
  for (size_t j = 0; j < nodes.size(); ++j) {
    CMarsNode *node = nodes[j];
    node->schedule_map2vec();
  }

  delete mars_ir_tree;
}
}  //  namespace MarsCommOpt
