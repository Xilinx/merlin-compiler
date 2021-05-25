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


//  Note for Enhancement, 2016-06-30, Yuxin
//  Current token generation strategy, for the same sync level:
//  1) forward, the scheduling distance is 1 between two nodes, add token
//  2) backward, the scheduling distance is 1 between two nodes, add token

#include "cmdline_parser.h"
#include "comm_opt.h"
#include "file_parser.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "xml_parser.h"

using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

namespace MarsCommOpt {

#define _DEBUG_CHGEN_OUTPUT_ 1
#define USED_CODE_IN_COVERAGE_TEST 0

int shared_mem_gen_top(CMarsIrV2 *mars_ir) {
  cout << "Shared Memory Generation" << endl;
  int node_num = mars_ir->size();
  map<int, map<CMarsNode *, CMarsNode *>> scope2seq;
  int t_c = 0;

  for (int j = 0; j < node_num; ++j) {
    //        cout << "node " <<j <<endl;
    //            CMarsNode *node = mars_ir->get_node(j);

    //            for(size_t ii=0 ; ii < m_loops.size() ; ii++) {
    //                if(scope2seq.find(m_loops[ii]) != scope2seq.end())
    //                continue; vector<CMarsNode*> t_nodes =
    //  mars_ir->get_nodes_with_common_loop(m_loops[ii]);

    //                for (int i = 0; i <  node_num; i++) {
    for (int jj = 0; jj < node_num; jj++) {
      CMarsNode *node0 = mars_ir->get_node(j);
      CMarsNode *node1 = mars_ir->get_node(jj);
      //  void *kernel_top = node0->get_user_kernel();
      //                    CMarsNode *node0 = t_nodes[i];
      //                        CMarsNode *node1 = t_nodes[jj];
      int idx0 = mars_ir->get_node_idx(node0);
      int idx1 = mars_ir->get_node_idx(node1);

      CMarsScheduleVector schedule0 = node0->get_schedule();
      CMarsScheduleVector schedule1 = node1->get_schedule();
      int sync_level = get_sync_level_from_schedule(schedule0, schedule1);
      auto loops = node0->get_common_loops(node1);
      if (sync_level % 2 != 0) {
        continue;
      }
      if (static_cast<size_t>(sync_level) / 2 > loops.size()) {
        continue;
      }
      if (sync_level == 0 && !node1->is_after(node0)) {
        continue;
      }
      //  Note: conditions for quit
      if (schedule0.size() <= schedule1.size() &&
          static_cast<size_t>(sync_level) == schedule0.size()) {
        continue;
      }
      if (schedule0.size() > schedule1.size() &&
          static_cast<size_t>(sync_level) == schedule1.size()) {
        continue;
      }
      cout << "sync edge between node " << idx0 << " and " << idx1 << endl;
      cout << "sync_level: " << sync_level << endl;
      int max_length = 0;
      max_length = node0->get_schedule_depth(sync_level);
      cout << "max length: " << max_length << endl;
      bool forward = (schedule1[sync_level] - schedule0[sync_level]) == 1;
      bool backward =
          (schedule0[sync_level] - schedule1[sync_level]) == max_length;

#if 0
                if ((schedule1[sync_level]-schedule0[sync_level]) > 1 &&
                    node1->is_after(node0))
                  continue;
                if ((schedule0[sync_level]-schedule1[sync_level]) <
                    max_length && !node1->is_after(node0))
                  continue;
#else
      if (!forward && !backward) {
        continue;
      }
      //  Add by Yuxin for the nodes with forward edge, but dont obey the "is
      //  after" order, vice versa.
      if (forward && !node1->is_after(node0)) {
        continue;
      }
      if (backward && node1->is_after(node0)) {
        continue;
      }
#endif
      //                bool sync_tag = has_sync_edge(mars_ir, node0, node1,
      //  sync_level);                 if(!sync_tag) { continue;
      //                }
      //                    int loop_level = sync_level/2-1;
      //                    if(loop_level != ii) continue;
      //                    vector<void*> loops = node0->get_loops();
      /*
           void* sync_scope;
           if(loop_level>=0)
           sync_scope = loops[loop_level];
           else {  //  FIXME:
           sync_scope = kernel_top;
           }
           */
      if (scope2seq.find(sync_level) != scope2seq.end()) {
        map<CMarsNode *, CMarsNode *> sub_node = scope2seq[sync_level];
        if (sub_node.find(node0) != sub_node.end()) {
          if (sub_node[node0] == node1) {
            continue;
          }
        }
      }

      vector<void *> common_loops;
      common_loops = node0->get_common_loops(node1);
      string token_info = my_itoa(idx0) + " and " + my_itoa(idx1) +
                          " for sync level " + my_itoa(sync_level);
      cout << "[token info] " << token_info << endl;
      gen_token(mars_ir->get_ast(), node0, node1, t_c, backward,
                common_loops.size(), token_info);
      t_c++;
      scope2seq[sync_level][node0] = node1;
      std::cout << "============================================" << std::endl;
      //            }
      //        }
    }
  }
  return static_cast<int>(t_c > 0);
}

int gen_token(CMarsAST_IF *codegen, CMarsNode *node0, CMarsNode *node1, int t_c,
              bool back_dep, int common_level, string token_info) {
  string t_str = my_itoa(t_c);

  SgVariableDeclaration *new_var;
  SgVariableDeclaration *new_dummy;
  //    void * array = edge->GetPort();
  //    string port_str=codegen->_p(array);

  for (int j = 0; j < 2; j++) {
    CMarsNode *node;
    if (j == 0) {
      node = node0;
    } else {
      node = node1;
    }

    void *kernel_top = node->get_user_kernel();
    SgBasicBlock *sg_func_body =
        static_cast<SgBasicBlock *>(codegen->GetFuncBody(kernel_top));

    if (j == 0) {
      string token_name = "token_" + t_str;
      codegen->get_valid_local_name(kernel_top, &token_name);
      new_var = static_cast<SgVariableDeclaration *>(
          codegen->CreateVariableDecl(codegen->GetTypeByString("int"),
                                      token_name, nullptr, sg_func_body));
      codegen->PrependChild_v1(new_var, sg_func_body);
      codegen->AddComment("Token comes from node " + token_info, new_var);

      string dummy_name = "dummy_token_" + t_str;
      codegen->get_valid_local_name(kernel_top, &dummy_name);
      new_dummy = static_cast<SgVariableDeclaration *>(
          codegen->CreateVariableDecl(codegen->GetTypeByString("int"),
                                      dummy_name, nullptr, sg_func_body));
      codegen->PrependChild_v1(new_dummy, sg_func_body);
    }
    ROSE_ASSERT(new_var);
    ROSE_ASSERT(new_dummy);
    if (j == 0) {
      gen_token_write(codegen, node, new_var, t_c, back_dep, common_level,
                      token_info);
    } else {
      gen_token_read(codegen, node, new_var, new_dummy, t_c, back_dep,
                     common_level, token_info);
    }
  }
  return 1;
}

int gen_token_write(CMarsAST_IF *codegen, CMarsNode *node, void *new_var,
                    int t_c, bool back_dep, int common_level,
                    string token_info) {
  cout << "gen token write\n";
  auto loops = node->get_loops();
  auto stmts = node->get_stmts();
  SgStatement *loc = static_cast<SgStatement *>(stmts[stmts.size() - 1]);
  void *kernel_top = node->get_user_kernel();
  string port_str = token_info;

  if (back_dep) {
    port_str += "(backward)";
  } else {
    port_str += "(forward)";
  }

  cout << "[Write token]\n";
#if PROJDEBUG
  if (!loops.empty()) {
    cout << codegen->_p(loops[0]) << endl;
  }
#endif
  SgExpression *ub_cond = nullptr;
  SgExpression *ub_comm_cond = nullptr;
  SgExpression *ub_inner_cond = nullptr;

  for (int i = static_cast<int>(loops.size()) - 1; i >= 0; i--) {
    void *loop = loops[i];
    void *ivar = nullptr;
    int is_canonical = codegen->IsCanonicalForLoop(loop, &ivar);
    if (is_canonical == 0) {
      cout << "[Error] Loop is non-canonical. Quit processing." << endl;
      reportNonCanonical(loop, codegen);
      exit(1);
    }
    SgExpression *cond =
        isSgExprStatement((static_cast<SgForStatement *>(loop)->get_test()))
            ->get_expression();
    ROSE_ASSERT(cond);
    //                CMarsRangeExpr mr = CMarsVariable(loop,
    //  &codegen).get_range();
    CMarsRangeExpr mr = CMarsVariable(loop, codegen, loc).get_range();
    //                cout <<"[scope write:] " << codegen->_p(loc->get_scope())
    //  <<endl;
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    int ret = mr.get_simple_bound(&me_lb, &me_ub);
    //        cout <<"lb: " << me_lb.print_s()<<endl
    //        cout <<"ub: " << me_ub.print_s()<<endl;
    //        cout << "[range ]" << me_lb.print_s() << "," << me_ub.print_s() <<
    //  endl;
    assert(ret);

    void *iter = codegen->GetLoopIterator(loop);
    if (static_cast<size_t>(i) == loops.size() - 1) {
      if (loops.size() > static_cast<size_t>(common_level)) {
        ub_inner_cond = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_ub.get_expr_from_coeff())));
      } else if (back_dep) {
        ub_comm_cond = SageBuilder::buildNotEqualOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_ub.get_expr_from_coeff())));
      }
    } else {
      SgExpression *ub_exp = nullptr;
      if (i >= common_level) {
        ub_exp = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_ub.get_expr_from_coeff())));
        if (ub_inner_cond == nullptr) {
          ub_inner_cond = ub_exp;
        } else {
          ub_inner_cond = SageBuilder::buildAndOp(ub_inner_cond, ub_exp);
        }

      } else if (back_dep) {
        //                    cout <<"back dep\n" <<endl;
        ub_exp = SageBuilder::buildNotEqualOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_ub.get_expr_from_coeff())));
        if (ub_comm_cond == nullptr) {
          ub_comm_cond = ub_exp;
        } else {
          ub_comm_cond = SageBuilder::buildOrOp(ub_comm_cond, ub_exp);
        }
      }
    }
  }
  if ((ub_inner_cond != nullptr) && (ub_comm_cond != nullptr)) {
    ub_cond = SageBuilder::buildAndOp(ub_inner_cond, ub_comm_cond);
  } else if (ub_comm_cond != nullptr) {
    ub_cond = ub_comm_cond;
  } else if (ub_inner_cond != nullptr) {
    ub_cond = ub_inner_cond;
  }

  if (ub_cond != nullptr) {
    SgInitializedName *new_port = static_cast<SgInitializedName *>(
        codegen->GetVariableInitializedName(new_var));
    SgExpression *new_ref =
        static_cast<SgExpression *>(codegen->CreateVariableRef(new_port));
    SgStatement *init = SageBuilder::buildAssignStatement(
        static_cast<SgExpression *>(codegen->CopyExp(new_ref)),
        SageBuilder::buildIntVal(1));
    SgBasicBlock *if_body = SageBuilder::buildBasicBlock(init);
    SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(ub_cond);
    SgIfStmt *node_if_stmt =
        SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
    codegen->InsertAfterStmt_v1(loc, node_if_stmt);
    //  FIXEME: insert location before first stmt
    codegen->AddComment("Token gen: node " + port_str, node_if_stmt);
    std::ostringstream oss;
    oss << "ACCEL mem_fence";
    void *sg_pragma = codegen->CreatePragma(oss.str(), kernel_top);
    codegen->InsertStmt(sg_pragma, init);

  } else {
    SgInitializedName *new_port = static_cast<SgInitializedName *>(
        codegen->GetVariableInitializedName(new_var));
    SgExpression *new_ref =
        static_cast<SgExpression *>(codegen->CreateVariableRef(new_port));
    SgStatement *init = SageBuilder::buildAssignStatement(
        static_cast<SgExpression *>(codegen->CopyExp(new_ref)),
        SageBuilder::buildIntVal(1));
    //    SgStatement *loc = static_cast<SgStatement *>(stmts[stmts.size()-1]);
    codegen->InsertAfterStmt_v1(loc, init);
    //  FIXEME: insert location after last stmt
    codegen->AddComment("Token gen: node " + port_str, init);
    std::ostringstream oss;
    oss << "ACCEL mem_fence";
    void *sg_pragma = codegen->CreatePragma(oss.str(), kernel_top);
    codegen->InsertStmt(sg_pragma, init);
  }
  return 1;
}

int gen_token_read(CMarsAST_IF *codegen, CMarsNode *node, void *new_var,
                   void *new_dummy, int t_c, bool back_dep, int common_level,
                   string token_info) {
  auto loops = node->get_loops();
  auto stmts = node->get_stmts();
  SgStatement *loc = static_cast<SgStatement *>(stmts[0]);
  void *kernel_top = node->get_user_kernel();
  string port_str = token_info;
  if (back_dep) {
    port_str += "(backward)";
  } else {
    port_str += "(forward)";
  }

  cout << "[Read token]\n";
#if PROJDEBUG
  if (!loops.empty()) {
    cout << codegen->_p(loops[0]) << endl;
  }
#endif
  SgExpression *lb_cond = nullptr;
  SgExpression *lb_comm_cond = nullptr;
  SgExpression *lb_inner_cond = nullptr;
  //    cout << "read0\n";
  if (back_dep && (common_level == 0)) {
    //        cout << "read1\n";
    return 1;
    //            lb_cond=buildIntVal(0);
  }

  for (int i = static_cast<int>(loops.size()) - 1; i >= 0; i--) {
    void *loop = loops[i];
    void *ivar = nullptr;
    int is_canonical = codegen->IsCanonicalForLoop(loop, &ivar);
    if (is_canonical == 0) {
      cout << "[Error] Loop is non-canonical. Quit processing." << endl;
      reportNonCanonical(loop, codegen);
      exit(1);
    }

    SgExpression *cond =
        isSgExprStatement((static_cast<SgForStatement *>(loop)->get_test()))
            ->get_expression();
    ROSE_ASSERT(cond);
    //        CMarsRangeExpr mr = CMarsVariable(loop, codegen).get_range();
    CMarsRangeExpr mr = CMarsVariable(loop, codegen, loc).get_range();
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    //    cout << codegen->_p(loop) <<endl;
    int ret = mr.get_simple_bound(&me_lb, &me_ub);
    //            cout <<"lb: " << me_lb.print_s()<<endl;
    //                cout <<"ub: " << me_ub.print_s()<<endl;
    //    cout << "[range ]" << lb << "," << ub << endl;

    assert(ret);
    //                int lb = me_lb.get_const();
    //                int ub = me_ub.get_const();
    void *iter = codegen->GetLoopIterator(loop);

    if (static_cast<size_t>(i) == loops.size() - 1) {
      if (loops.size() > static_cast<size_t>(common_level)) {
        lb_inner_cond = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
      } else if (back_dep) {
        lb_comm_cond = SageBuilder::buildNotEqualOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
      }
    } else {
      SgExpression *lb_exp = nullptr;

      if (i >= common_level) {
        lb_exp = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
        if (lb_inner_cond == nullptr) {
          lb_inner_cond = lb_exp;
        } else {
          lb_inner_cond = SageBuilder::buildAndOp(lb_inner_cond, lb_exp);
        }

      } else if (back_dep) {
        //    cout <<"back dep\n" <<endl;
        lb_exp = SageBuilder::buildNotEqualOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
        if (lb_comm_cond == nullptr) {
          lb_comm_cond = lb_exp;
        } else {
          lb_comm_cond = SageBuilder::buildOrOp(lb_comm_cond, lb_exp);
        }
      }
    }
  }

  if ((lb_inner_cond != nullptr) && (lb_comm_cond != nullptr)) {
    lb_cond = SageBuilder::buildAndOp(lb_inner_cond, lb_comm_cond);
  } else if (lb_comm_cond != nullptr) {
    lb_cond = lb_comm_cond;
  } else if (lb_inner_cond != nullptr) {
    lb_cond = lb_inner_cond;
  }

  if ((lb_cond != nullptr) &&
      loops.size() > static_cast<size_t>(common_level)) {
    SgInitializedName *new_port = static_cast<SgInitializedName *>(
        codegen->GetVariableInitializedName(new_var));
    SgInitializedName *new_dum = static_cast<SgInitializedName *>(
        codegen->GetVariableInitializedName(new_dummy));
    SgExpression *new_ref =
        static_cast<SgExpression *>(codegen->CreateVariableRef(new_port));
    SgExpression *dum_ref =
        static_cast<SgExpression *>(codegen->CreateVariableRef(new_dum));
    //        SgStatement *init = SageBuilder::buildExprStatement(new_ref);
    SgStatement *init = SageBuilder::buildAssignStatement(dum_ref, new_ref);
    SgBasicBlock *if_body = SageBuilder::buildBasicBlock(init);
    SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(lb_cond);
    SgIfStmt *node_if_stmt =
        SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
    codegen->InsertStmt_v1(loc, node_if_stmt);
    //  FIXEME: insert location before first stmt
    codegen->AddComment("Token gen: node " + port_str, node_if_stmt);
    std::ostringstream oss;
    oss << "ACCEL mem_fence";
    void *sg_pragma = codegen->CreatePragma(oss.str(), kernel_top);
    codegen->InsertAfterStmt(sg_pragma, init);

  } else {
    SgInitializedName *new_port = static_cast<SgInitializedName *>(
        codegen->GetVariableInitializedName(new_var));
    SgInitializedName *new_dum = static_cast<SgInitializedName *>(
        codegen->GetVariableInitializedName(new_dummy));
    SgExpression *new_ref =
        static_cast<SgExpression *>(codegen->CreateVariableRef(new_port));
    SgExpression *dum_ref =
        static_cast<SgExpression *>(codegen->CreateVariableRef(new_dum));
    //        SgStatement *init = SageBuilder::buildExprStatement(new_ref);
    SgStatement *init = SageBuilder::buildAssignStatement(dum_ref, new_ref);
    //  SgStatement *loc = static_cast<SgStatement *>(stmts[0]);
#if PROJDEBUG
    cout << "Insert loc:" << codegen->_p(loc) << endl;
#endif
    SgStatement *insert_stmt = nullptr;
    if (back_dep) {
      SgBasicBlock *if_body = SageBuilder::buildBasicBlock(init);
      SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(lb_cond);
      insert_stmt =
          SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
    } else {
      insert_stmt = init;
    }

    codegen->InsertStmt_v1(loc, insert_stmt);
    //  FIXEME: insert location after last stmt
    codegen->AddComment("Token gen: node " + port_str, insert_stmt);
    std::ostringstream oss;
    oss << "ACCEL mem_fence";
    void *sg_pragma = codegen->CreatePragma(oss.str(), kernel_top);
    codegen->InsertAfterStmt(sg_pragma, init);
  }
  return 1;
}

#if USED_CODE_IN_COVERAGE_TEST
int build_edge2chain(CMarsAST_IF *codegen, CMarsIrV2 *mars_ir,
                     map<CMarsEdge *, int> *edge2chain) {
  map<void *, int> maxlength;
  build_seq_length(codegen, mars_ir, &maxlength);

  //  Print
  for (auto &len : maxlength) {
#if PROJDEBUG
    cout << "Seq length: " << len.second << endl;
#endif
  }

  for (auto &edge : mars_ir->get_all_edges()) {
    if (edge->GetCommType() != COMMTYPE_HOST_IF &&
        edge->GetCommType() != COMMTYPE_SHARED_MEM_DDR)
      continue;
    void *array = edge->GetPort();
    CMarsNode *node0 = edge->GetNode(0);
    CMarsNode *node1 = edge->GetNode(1);
    bool write0 = node0->port_is_write(array);
    bool write1 = node1->port_is_write(array);
    bool read0 = node0->port_is_read(array);
    bool read1 = node1->port_is_read(array);

    if (read0 && read1 && !write0 && !write1) {
#if PROJDEBUG
      cout << "[continue] read only mem: " << codegen->_p(array) << endl;
#endif
      (*edge2chain)[edge] = 1;
      //        read2read_vec[node0].insert(node1);
      continue;
    }

    if (!node1->is_after(node0)) {
      CMarsScheduleVector schedule0 = node0->get_schedule();
      CMarsScheduleVector schedule1 = node1->get_schedule();
      int sync_level = get_sync_level_from_schedule(schedule0, schedule1);
      int loop_level = sync_level / 2 - 1;
      vector<void *> m_loops = node0->get_loops();

      void *sync_scope;
      if (loop_level >= 0) {
        sync_scope = m_loops[loop_level];
      } else {
        if (m_loops.size() > 0) {
          sync_scope = codegen->GetScope(m_loops[0]);
        } else {
          vector<void *> m_stmts = node0->get_stmts();
          sync_scope = codegen->GetScope(m_stmts[0]);
        }
      }
      if (maxlength.find(sync_scope) != maxlength.end()) {
        int dist = 0;
        dist = schedule0[sync_level] - schedule1[sync_level];
        cout << "dist for current edge:" << dist << endl;
        if (dist < maxlength[sync_scope]) {
          (*edge2chain)[edge] = 1;
          continue;
        }
        if (dist == 1 && dist == maxlength[sync_scope]) {
          (*edge2chain)[edge] = 1;
          continue;
        }
      }
    }

    for (auto &edge : mars_ir->get_all_edges()) {
      if (edge->GetCommType() != COMMTYPE_HOST_IF &&
          edge->GetCommType() != COMMTYPE_SHARED_MEM_DDR)
        continue;
      CMarsNode *node0 = edge->GetNode(0);
      CMarsNode *node1 = edge->GetNode(1);
      if (edge2chain->find(edge) != edge2chain->end())
        continue;

      for (auto &c_edge : mars_ir->get_all_edges()) {
        //            if(array != c_edge->GetPort()) continue;

        if (edge->GetCommType() != COMMTYPE_HOST_IF &&
            edge->GetCommType() != COMMTYPE_SHARED_MEM_DDR)
          continue;
        if (c_edge == edge)
          continue;

        CMarsNode *c_node0 = c_edge->GetNode(0);
        CMarsNode *c_node1 = c_edge->GetNode(1);
        if (!c_node1->is_after(c_node0))
          continue;

        if (edge2chain->find(c_edge) != edge2chain->end())
          continue;
        if (node0 == c_node0 && c_node1 != node1 && c_node1->is_after(node1) &&
            node1->is_after(node0) && c_node1->is_after(c_node0)) {
          (*edge2chain)[c_edge] = 1;
        }
      }
    }
  }
  return 1;
}

void build_seq_length(CMarsAST_IF *codegen, CMarsIrV2 *mars_ir,
                      map<void *, int> *maxlength) {
  for (auto &edge : mars_ir->get_all_edges()) {
    if (edge->GetCommType() != COMMTYPE_HOST_IF &&
        edge->GetCommType() != COMMTYPE_SHARED_MEM_DDR)
      continue;
    CMarsNode *node0 = edge->GetNode(0);
    CMarsNode *node1 = edge->GetNode(1);
    if (!node1->is_after(node0))
      continue;

    CMarsScheduleVector schedule0 = node0->get_schedule();
    CMarsScheduleVector schedule1 = node1->get_schedule();
    int sync_level = get_sync_level_from_schedule(schedule0, schedule1);
    cout << "sync_level: " << sync_level << endl;
    if (sync_level % 2 != 0)
      continue;

    int loop_level = sync_level / 2 - 1;
    vector<void *> m_loops = node0->get_loops();

    void *sync_scope;
    if (loop_level >= 0) {
      sync_scope = m_loops[loop_level];
    } else {  //  FIXME:
      if (m_loops.size() > 0) {
        sync_scope = codegen->GetScope(m_loops[0]);
      } else {
        vector<void *> m_stmts = node0->get_stmts();
        sync_scope = codegen->GetScope(m_stmts[0]);
      }
    }
    //  FIXME:
    if (maxlength->find(sync_scope) != maxlength->end())
      continue;

    int dist = 0;
    dist = schedule1[sync_level] - schedule0[sync_level];
    //        cout << "dist0:" <<dist << endl;

    for (auto &c_edge : mars_ir->get_all_edges()) {
      if (c_edge->GetCommType() != COMMTYPE_HOST_IF &&
          c_edge->GetCommType() != COMMTYPE_SHARED_MEM_DDR)
        continue;
      CMarsNode *c_node0 = c_edge->GetNode(0);
      CMarsNode *c_node1 = c_edge->GetNode(1);
      if (!c_node1->is_after(c_node0))
        continue;

      CMarsScheduleVector c_schedule0 = c_node0->get_schedule();
      CMarsScheduleVector c_schedule1 = c_node1->get_schedule();
      int c_sync_level = get_sync_level_from_schedule(c_schedule0, c_schedule1);
      if (c_sync_level % 2 != 0)
        continue;

      int c_loop_level = c_sync_level / 2 - 1;
      vector<void *> c_m_loops = c_node0->get_loops();

      void *c_sync_scope;
      if (c_loop_level >= 0) {
        c_sync_scope = c_m_loops[c_loop_level];
      } else {  //  FIXME: top scope
        if (c_m_loops.size() > 0) {
          c_sync_scope = codegen->GetScope(c_m_loops[0]);
        } else {
          vector<void *> m_stmts = c_node0->get_stmts();
          c_sync_scope = codegen->GetScope(m_stmts[0]);
        }
      }
      if (c_sync_scope != sync_scope)
        continue;
      if (c_schedule1[c_sync_level] - c_schedule0[c_sync_level] > dist)
        dist = c_schedule1[c_sync_level] - c_schedule0[c_sync_level];
      cout << "dist1;" << dist << endl;
    }
    if (dist > 0)
      (*maxlength)[sync_scope] = dist;
    cout << "dist:" << dist << endl;
    //    cout << codegen->_p(sync_scope)<<endl;
  }
}

bool has_sync_edge(CMarsIrV2 *mars_ir, CMarsNode *node0, CMarsNode *node1,
                   int sync_level) {
  for (auto &edge : mars_ir->get_all_edges()) {
    if (edge->GetCommType() == COMMTYPE_HOST_IF ||
        edge->GetCommType() == COMMTYPE_SHARED_MEM_DDR) {
      CMarsNode *cnode0 = edge->GetNode(0);
      CMarsNode *cnode1 = edge->GetNode(1);
      CMarsScheduleVector schedule0 = cnode0->get_schedule();
      CMarsScheduleVector schedule1 = cnode1->get_schedule();
      int level = get_sync_level_from_schedule(schedule0, schedule1);
      if (sync_level != level)
        continue;

      if (cnode0 == node0 && cnode1 == node1)
        return true;
      if (cnode1 == node0 && cnode0 == node1)
        return true;
    }
  }
  return false;
}

int get_max_length(vector<CMarsNode *> t_nodes, int level, CMarsNode *node_t) {
  int max_l = 0;
  for (size_t i = 0; i < t_nodes.size(); i++) {
    CMarsNode *node0 = t_nodes[i];
    CMarsScheduleVector schedule0 = node0->get_schedule();
    CMarsScheduleVector schedule1 = node_t->get_schedule();
    int sync_level = get_sync_level_from_schedule(schedule0, schedule1);
    if (node0 != node_t) {
      if (sync_level != level)
        continue;
    }
    if (schedule0[level] > max_l)
      max_l = schedule0[level];
  }
  cout << "max : " << max_l << endl;
  return max_l;
}
#endif

void reportNonCanonical(void *sg_loop, CSageCodeGen *codegen) {
  string file_name = codegen->get_file(sg_loop);
  int line_num = codegen->get_line(sg_loop);
  string str_loop = codegen->UnparseToString(sg_loop).substr(0, 20);
  std::ostringstream oss;
  string file_info =
      "\'" + str_loop + "\' (" + file_name + ":" + my_itoa(line_num) + ")\n";
  dump_critical_message(CMOPT_ERROR_1(file_info));
}
}  //  namespace MarsCommOpt

using MarsCommOpt::assign_communication_type;
using MarsCommOpt::assign_node_scheduling;
using MarsCommOpt::shared_mem_gen_top;

int comm_shared_memory_token_top(CSageCodeGen *codegen, void *pTopFunc,
                                 const CInputOptions &options) {
  printf("Hello Communication Optimization (token generation for shared "
         "memory)... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
  assign_communication_type(&mars_ir, true, true);
  assign_node_scheduling(&mars_ir, true /*detect shared memory*/);
  return shared_mem_gen_top(&mars_ir);
}
