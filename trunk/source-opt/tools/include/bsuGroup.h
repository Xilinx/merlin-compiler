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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BSUGROUP_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BSUGROUP_H_

#include <map>
#include <vector>
#include <string>

#include "analPragmas.h"
#include "bsuNode.h"
#include "codegen.h"
#include "ir_types.h"
#include "rose.h"

typedef std::vector<CMirNode *> mirnode_table_vec;

class CMirNodeSet : public mirnode_table_vec {
 public:
  CMirNodeSet() {}
  CMirNodeSet(const CMirNodeSet &node_set)
      : mirnode_table_vec(node_set), top_node(node_set.top_node),
        SgNode2CMirNode(node_set.SgNode2CMirNode),
        m_while_nodes(node_set.m_while_nodes) {}
  bool read_from_AST(CSageCodeGen *codegen, SgFunctionDeclaration *kernel,
                     bool pragma_in_loop, bool report = false);
  //  public interface
  string get_mir_node(int index);
  string get_function_name(SgFunctionDeclaration *kernel_decl);
  CMirNode *get_top_node() { return top_node; }
  vector<CMirNode *> get_while_nodes() { return m_while_nodes; }
  CMirNode *get_parent_node(CMirNode *bNode);
  CMirNode *get_parent_node(CSageCodeGen *codegen, CMirNode *bNode,
                            CMirNode *fNode);
  CMirNode *get_node(void *target_ref);
  void build_parent_relation();
  void printindex();
  void set_kernel() { top_node->set_kernel(); }

  void releaseMemory() {
    for (iterator it = begin(); it != end(); ++it)
      delete *it;
    clear();
  }

 protected:
  CMirNode *top_node;
  map<SgBasicBlock *, CMirNode *> SgNode2CMirNode;
  vector<CMirNode *> m_while_nodes;

  void nodeParsing(CSageCodeGen *codegen, SgFunctionDeclaration *kernel_decl,
                   SgBasicBlock *body, CMirNode *flnode, CMirNode *lnode,
                   bool pragma_in_loop);
  bool isOuttermostLoop(CSageCodeGen *codegen, void *sg_loop, void *sg_scope);
  CMirNode *init_loop_node(CSageCodeGen *codegen,
                           SgFunctionDeclaration *kernel_decl, void *sg_node);
  bool check_valid_reduction(CSageCodeGen *codegen, void *func_decl);
};

typedef std::vector<CMirNodeSet> mirnodeset_table_vec;

class CMirNodeFuncSet : public mirnodeset_table_vec {
 public:
  CMirNodeFuncSet(CSageCodeGen *codegen, SgSourceFile *file,
                  const SetVector<void *> &all_func_decls, bool pragma_in_loop,
                  bool report = false);
  int get_func_num() { return size(); }
  void releaseMemory() {
    for (iterator it = begin(); it != end(); ++it)
      it->releaseMemory();
    clear();
  }
  CMirNode *get_node(void *body_ref) {
    for (iterator it = begin(); it != end(); ++it) {
      CMirNode *node = it->get_node(body_ref);
      if (node)
        return node;
    }
    return nullptr;
  }
  CMirNodeFuncSet() {}
  CMirNodeFuncSet(const CMirNodeFuncSet &node_set)
      : mirnodeset_table_vec(node_set) {}
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BSUGROUP_H_
