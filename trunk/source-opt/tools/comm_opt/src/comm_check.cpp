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


#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

#include "comm_opt.h"
#include "mars_ir_v2.h"
#include "program_analysis.h"

using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using MarsProgramAnalysis::is_exact_condition;

namespace MarsCommOpt {

bool check_node(CMarsNode *node, map<void *, mark_struct> *loop2mark,
                CMarsIrV2 *mars_ir, bool pre_check) {
  CMarsAST_IF *ast = node->get_ast();
  //  1. if there is no port in the current node,
  //   no need to check
  set<void *> ports = mars_ir->get_shared_ports(node);
  if (ports.empty()) {
    return true;
  }
  bool valid = true;
  void *loc = node->get_stmts()[0];
  //  2. check whether all the outer loops are canonical loop
  if (!pre_check) {
    for (auto &loop : node->get_loops()) {
      if ((*loop2mark)[loop].check_non_canonical) {
        continue;
      }
      (*loop2mark)[loop].check_non_canonical = true;
      string file_name = ast->GetFileInfo_filename(loop, 1);
      int line_num = ast->GetFileInfo_line(loop);
      string loop_label = ast->get_internal_loop_label(loop);
      if (ast->IsCanonicalForLoop(loop) == 0) {
        /*
             string msg = "Cannot support non-canonical loop \'" + loop_label
             + "\' (" + file_name +
             ":" + my_itoa(line_num) + ")\n";
             dump_critical_message("COMCK", "WARNING", msg, 202, 1);
             */
        //  string file_info = "\'" + loop_label + "\' (" + file_name +
        //    ":" + my_itoa(line_num) + ")\n";
        string file_info = "\'" + loop_label + "\' (" + file_name + ")\n";
        dump_critical_message(COMCK_WARNING_1(file_info));

        valid = false;
      } else {
        CMarsRangeExpr mr = CMarsVariable(loop, ast, loc).get_range();
        CMarsExpression me_lb;
        CMarsExpression me_ub;
        int ret = mr.get_simple_bound(&me_lb, &me_ub);
        if (ret == 0) {
          /*
               string msg = "Cannot support the loop \'" + loop_label
               + "\' (" + file_name +
               ":" + my_itoa(line_num) + ") without the simple bound\n";

               dump_critical_message("COMCK", "WARNING", msg, 206);
               */
          string file_info = "\'" + loop_label + "\' (" + file_name + ":" +
                             my_itoa(line_num) + ")";
          dump_critical_message(COMCK_WARNING_2(file_info));

          valid = false;
        }
      }
    }
  }
  if (pre_check) {
    //  3.2 check whether there are do-while/while loops in which there is
    //  communication
    //        printf("    ==>  check do-while statements...\n\n");

    for (auto &stmt : node->get_stmts()) {
      vector<void *> vec_while;
      ast->GetNodesByType(stmt, "preorder", "SgWhileStmt", &vec_while);
      vector<void *> vec_do_while;
      ast->GetNodesByType(stmt, "preorder", "SgDoWhileStmt", &vec_do_while);
      vec_while.insert(vec_while.end(), vec_do_while.begin(),
                       vec_do_while.end());
      for (auto &while_loop : vec_while) {
        vector<void *> vec_refs;
        ast->GetNodesByType(while_loop, "preorder", "SgVarRefExp", &vec_refs);
        bool has_communication = false;
        for (auto &ref : vec_refs) {
          void *var_init = ast->GetVariableInitializedName(ref);
          if (ports.count(var_init) > 0) {
            has_communication = true;
            break;
          }
        }
        if (!has_communication) {
          continue;
        }

        string file_name = ast->GetFileInfo_filename(stmt, 1);
        int line_num = ast->GetFileInfo_line(stmt);

        string loop_info = "'" + ast->UnparseToString(stmt) + "' (" +
                           file_name + ":" + my_itoa(line_num) + ")";
        dump_critical_message(COMCK_WARNING_7(loop_info));
        //        dump_critical_message("COMCK", "WARNING", msg, 204);
        //    return false;
        valid = false;
      }
    }
  }
  //  4 check whether there is continue/break statement
  vector<void *> loops = node->get_loops();
  if (!loops.empty()) {
    void *inner_loop = loops.back();
    if (!(*loop2mark)[inner_loop].check_break) {
      (*loop2mark)[inner_loop].check_break = true;
      for (auto &stmt : node->get_stmts()) {
        vector<void *> vec_break;
        ast->GetNodesByType(stmt, "preorder", "SgBreakStmt", &vec_break);
        vector<void *> vec_continue;
        ast->GetNodesByType(stmt, "preorder", "SgContinueStmt", &vec_continue);
        vec_break.insert(vec_break.end(), vec_continue.begin(),
                         vec_continue.end());
        for (auto &break_stmt : vec_break) {
          if (!ast->IsScopeBreak(break_stmt, inner_loop)) {
            continue;
          }
          vector<void *> vec_refs;
          ast->GetNodesByType(inner_loop, "preorder", "SgVarRefExp", &vec_refs);
          bool has_communication = false;
          for (auto &ref : vec_refs) {
            void *var_init = ast->GetVariableInitializedName(ref);
            if (ports.count(var_init) > 0) {
              has_communication = true;
              break;
            }
          }
          if (!has_communication) {
            continue;
          }

          string file_name = ast->GetFileInfo_filename(break_stmt, 1);
          //  int line_num = ast->GetFileInfo_line(break_stmt);
          /*      string msg = "Cannot support loop with the continue/break
             statement (" + file_name +
                          ":" + my_itoa(line_num) + ")\n";

                          dump_critical_message("COMCK", "WARNING", msg, 201);
           */

          //  string file_info =     "(" + file_name +
          //    ":" + my_itoa(line_num) + ")\n";
          string file_info = "(" + file_name + ")\n";

          if (pre_check) {
            dump_critical_message(PROCS_WARNING_27(file_info));
          } else {
            dump_critical_message(COMCK_WARNING_3(file_info));
          }

          valid = false;
        }
      }
    }
  }

  //  5 check whether ports reference occurs on for-loop initialized statement
  //  for-loop condtion statement, increment expr
  for (auto &loop : loops) {
    if ((*loop2mark)[loop].check_ref) {
      continue;
    }
    (*loop2mark)[loop].check_ref = true;
    string loop_label = ast->get_internal_loop_label(loop);

    void *loop_init = ast->GetLoopInit(loop);
    void *loop_test = ast->GetLoopTest(loop);
    void *loop_incr = ast->GetLoopIncrementExpr(loop);
    vector<void *> var_refs;
    ast->GetNodesByType(loop_init, "preorder", "SgVarRefExp", &var_refs);
    vector<void *> tmp_refs;
    ast->GetNodesByType(loop_test, "preorder", "SgVarRefExp", &tmp_refs);
    var_refs.insert(var_refs.end(), tmp_refs.begin(), tmp_refs.end());
    ast->GetNodesByType(loop_incr, "preorder", "SgVarRefExp", &tmp_refs);
    var_refs.insert(var_refs.end(), tmp_refs.begin(), tmp_refs.end());
    auto loop_conds = node->get_loop_conditions(loop);
    for (auto &cond : loop_conds) {
      void *cond_stmt = ast->GetIfStmtCondition(cond.first);
      vector<void *> tmp_refs;
      ast->GetNodesByType(cond_stmt, "preorder", "SgVarRefExp", &tmp_refs);
      var_refs.insert(var_refs.end(), tmp_refs.begin(), tmp_refs.end());
    }
    set<void *> check_ports;
    for (auto &ref : var_refs) {
      void *port = ast->GetVariableInitializedName(ref);
      if (ports.count(port) > 0) {
        if (check_ports.count(port) > 0) {
          continue;
        }
        check_ports.insert(port);
        string file_name = ast->GetFileInfo_filename(ref, 1);
        /*int line_num = ast->GetFileInfo_line(ref);

             string msg = "The Communication on the shared port \'"
             + ast->GetVariableName(port) + "\' (" + file_name +
             ":" + my_itoa(line_num) + ") whose references are";
             msg += " on either the loop \'" + loop_label + "\' (" +
             file_name + ":" + my_itoa(loop_line_num) + ") or if condition";
             msg += " cannot be supported.\n";

             dump_critical_message("COMCK", "WARNING", msg, 205);
             */
        //  string port_info = "\'" + ast->GetVariableName(port) + "\' (" +
        //  file_name +
        //    ":" + my_itoa(line_num) + ")";
        //  string file_info =  "\'" + loop_label + "\' (" +
        //    file_name + ":" + my_itoa(loop_line_num) + ")";
        string port_info =
            "\'" + ast->GetVariableName(port) + "\' (" + file_name + ")";
        string file_info = "\'" + loop_label + "\' (" + file_name + ")";

        dump_critical_message(COMCK_WARNING_4(port_info, file_info));

        valid = false;
      }
    }
  }
  if (!loops.empty()) {
    void *scope = loops[0];
    for (auto &loop : loops) {
      auto loop_conds = node->get_loop_conditions(loop);
      for (auto &cond_pair : loop_conds) {
        void *if_stmt = cond_pair.first;
        bool true_branch = cond_pair.second;
        void *cond = ast->GetIfStmtCondition(if_stmt);
        if (is_exact_condition(ast, cond, scope, true_branch) == 0) {
          //        string file_name = ast->GetFileInfo_filename(if_stmt, true);
          //        int line_num = ast->GetFileInfo_line(if_stmt);
          /*
               string msg = "Cannot support a loop within an if-statement (" +
             file_name +
               ":" + my_itoa(line_num) +
               "), where the condition is not related to the outter-loop
             iterators.\n";

               dump_critical_message("COMCK", "WARNING", msg, 207);
               */
          //  string file_info =     "(" + file_name +
          //    ":" + my_itoa(line_num) + ")";

          if (pre_check) {
            string loop_label = ast->get_internal_loop_label(loop);
            string loop_info = "'" + loop_label + "' " +
                               getUserCodeFileLocation(ast, loop, true);
            dump_critical_message(COMCK_WARNING_5(loop_info));
          } else {
            string loop_info;
            dump_critical_message(COMCK_WARNING_5(loop_info));
          }

          valid = false;
        }
      }
    }
  }

  //  youxiang 20161106
  //  6 check whether the shared ports is valid
  //  we cannot support pointer alias as shared ports
  if (!pre_check) {
    for (auto port : ports) {
      void *port_type = ast->GetOrigTypeByTypedef(ast->GetTypebyVar(port));
      if ((ast->IsLocalInitName(port) != 0) &&
          (ast->IsPointerType(port_type) != 0)) {
        string msg = "Cannot support pointer alias as shared port ";
        string port_info = "\'" + ast->GetVariableName(port) + "\' (" +
                           ast->get_file(port) + ")";
        dump_critical_message(COMCK_WARNING_6(port_info));
        valid = false;
      }
    }
  }
  return valid;
}
}  //  namespace MarsCommOpt

using MarsCommOpt::mark_struct;
int comm_check_top(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options) {
  size_t i;
  printf("Hello Communication Check ... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, true /*check valid*/, true);
  map<void *, mark_struct> loop2mark;
  bool valid = mars_ir.is_valid();
  for (i = 0; i != mars_ir.size(); ++i) {
    CMarsNode *node = mars_ir.get_node(i);
    valid &= check_node(node, &loop2mark, &mars_ir, false);
  }
  if (!valid) {
    return 0;
  }
  return 1;
}
