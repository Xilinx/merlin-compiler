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


#include "bsuGroup.h"
#include "mars_message.h"
#include "mars_opt.h"
#undef DEBUG
#define DISABLE_DEBUG 1
#if DISABLE_DEBUG
#define DEBUG(stmts)
#else
#define DEBUG(stmts)                                                           \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#endif
#define USED_CODE_IN_COVERAGE_TEST 0

CMirNodeFuncSet::CMirNodeFuncSet(CSageCodeGen *codegen, SgSourceFile *file,
                                 const SetVector<void *> &all_func_decls,
                                 bool pragma_in_loop, bool report) {
  vector<void *> vec_func;
  codegen->GetNodesByType_int(file, "preorder", V_SgFunctionDeclaration,
                              &vec_func, true);

  for (auto decl : vec_func) {
    CMirNodeSet nodeset;
    SgFunctionDeclaration *kernel_decl =
        static_cast<SgFunctionDeclaration *>(decl);
    if (all_func_decls.count(kernel_decl) <= 0) {
      continue;
    }
    DEBUG(cout << "FUNC in: " << codegen->_p(decl) << endl);
    //  SEAN: 20151116 skip template function
    //  FIXME: we need to figure out how to support template functions in
    //  the future
    //  currently, we cannot handle template parameter and other things
    if ((codegen->IsTemplateFunctionDecl(kernel_decl) != 0) ||
        (codegen->IsTemplateMemberFunctionDecl(kernel_decl) != 0)) {
      continue;
    }
    string sSourceFile = codegen->GetFileInfo_filename(kernel_decl);

    if (sSourceFile.find("g++_HEADERS") != string::npos) {
      continue;
    }
    if (sSourceFile.find("/usr") != string::npos) {
      continue;
    }
    if (sSourceFile.find("apint_include") != string::npos) {
      continue;
    }
    if (sSourceFile.find("compilerGenerated") != string::npos) {
      continue;
    }

    ROSE_ASSERT(kernel_decl);

    nodeset.read_from_AST(codegen, kernel_decl, pragma_in_loop, report);
    push_back(nodeset);
  }
}

bool CMirNodeSet::read_from_AST(CSageCodeGen *codegen,
                                SgFunctionDeclaration *kernel,
                                bool pragma_in_loop, bool report) {
  SgBasicBlock *kernel_body = kernel->get_definition()->get_body();
  ROSE_ASSERT(kernel_body);

  CMirNode *flnode = new CMirNode(kernel_body, true);
  SgNode2CMirNode[kernel_body] = flnode;
  flnode->funcname_str = get_function_name(kernel);
  flnode->funcname_without_args = kernel->get_name().str();
  flnode->is_function = true;
  flnode->ref = kernel_body;
  push_back(flnode);
  DEBUG(cout << "\n\n[MARS-IR-PARSING] In function: "
             << kernel->get_type()->get_return_type()->unparseToString() << " "
             << kernel->get_name().str() << "(";
        const SgTypePtrList &args = kernel->get_type()->get_arguments();

        for (SgTypePtrList::const_iterator i_args = args.begin();
             i_args != args.end();) {
          cout << (*i_args)->unparseToString();

          if (++i_args != args.end())
            cout << ",";
        } cout
        << ")" << endl);

  //  parsing task pragma to collect all the task functions
  Rose_STL_Container<SgNode *> nodeList =
      NodeQuery::querySubTree(kernel_body, V_SgPragmaDeclaration);

  Rose_STL_Container<SgNode *>::iterator nodeListIterator = nodeList.begin();
  map<void *, vector<SgNode *>> loop2pragma;
  for (nodeListIterator = nodeList.begin(); nodeListIterator != nodeList.end();
       ++nodeListIterator) {
    SgPragmaDeclaration *decl = isSgPragmaDeclaration(*nodeListIterator);
    ROSE_ASSERT(decl);
    CAnalPragma ana_pragma(codegen);
    bool errorOut;
    if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false)) {
      //  skip compiler generated statement
      void *next_stmt = codegen->GetNextStmt(decl, false);
      if (next_stmt == nullptr) {
        continue;
      }
      if (ana_pragma.is_reduction()) {
        void *expr = codegen->GetExprFromStmt(next_stmt);
        if ((expr != nullptr) && (codegen->IsFunctionCall(expr) != 0)) {
          void *reduction = codegen->GetFuncDeclByCall(expr);
          if (reduction != nullptr) {
            flnode->add_one_reduction(reduction);
          }
        }
      }
      //  Yuxin: May 9th 2019 previously a reduction pragma which is
      //  insert before a loop will not be store in loop2pragma
      if (!pragma_in_loop) {
        void *specified_loop = codegen->GetSpecifiedLoopByPragma(decl);
        if (specified_loop != nullptr) {
          loop2pragma[specified_loop].push_back(decl);
        }
      }
    }
  }

  Rose_STL_Container<SgNode *> forLoops =
      NodeQuery::querySubTree(kernel_body, V_SgForStatement);
#if 0
    if (forLoops.size() == 0) {
        std::cout << "[MARS-IR-WARN] No for loop is found in the function \n"
        flnode->has_loop = false;
        top_node = flnode;
        return 0;
    }
#endif
  Rose_STL_Container<SgNode *>::iterator it;
  for (it = forLoops.begin(); it != forLoops.end(); it++) {
    SgForStatement *forLoop = isSgForStatement(*it);
    CMirNode *lnode = init_loop_node(codegen, kernel, forLoop);
    if (lnode == nullptr) {
      continue;
    }
    lnode->set_func(flnode);
    SgBasicBlock *loop_body = isSgBasicBlock(forLoop->get_loop_body());
    nodeParsing(codegen, kernel, loop_body, flnode, lnode, pragma_in_loop);

    //  Pragma parsing
    if (!pragma_in_loop) {
      vector<SgNode *> &pragmaList = loop2pragma[forLoop];

      for (size_t j = 0; j != pragmaList.size(); ++j) {
        SgPragmaDeclaration *decl = isSgPragmaDeclaration(pragmaList[j]);
        ROSE_ASSERT(decl);
        //  FIXME: 15/12/2015, so far Rose backend will not rewrite the header
        //  file, so we ignore the pragma within header files
        if (codegen->isWithInHeaderFile(decl)) {
          continue;
        }
        //  do not store or handle task/reduction  pragma in the loop node
        CAnalPragma ana_pragma(codegen);
        bool errorOut;
        if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false) &&
            !ana_pragma.is_loop_related()) {  //??????
          continue;
        }

        lnode->add_pragma(ana_pragma);
        lnode->pragma_table.push_back(decl);
      }
    }

    push_back(lnode);
#if 0
        //  FIXME: in order to be backward-compatible,
        //  we collect all the lnodes recursively
        vector<CMirNode*> child_lnode = lnode->get_all_child(true);
        insert(end(), child_lnode.begin(), child_lnode.end());
        flnode->add_one_child(lnode);
        lnode->set_parent(flnode);
#endif
  }

  vector<void *> vec_while_loops;
  codegen->GetNodesByType_int(kernel, "preorder", V_SgWhileStmt,
                              &vec_while_loops);
  vector<void *> vec_doWhile_loops;
  codegen->GetNodesByType_int(kernel, "preorder", V_SgDoWhileStmt,
                              &vec_doWhile_loops);
  for (size_t i = 0; i < vec_doWhile_loops.size(); ++i) {
    vec_while_loops.push_back(vec_doWhile_loops[i]);
  }

  for (auto sg_while : vec_while_loops) {
    CMirNode *lnode = init_loop_node(codegen, kernel, sg_while);
    void *loop_body = codegen->GetLoopBody(sg_while);
    if (lnode == nullptr) {
      continue;
    }
    lnode->set_func(flnode);
    nodeParsing(codegen, kernel, static_cast<SgBasicBlock *>(loop_body), flnode,
                lnode, pragma_in_loop);

    //  Pragma parsing
    if (!pragma_in_loop) {
      vector<SgNode *> &pragmaList = loop2pragma[sg_while];
      for (size_t j = 0; j != pragmaList.size(); ++j) {
        SgPragmaDeclaration *decl = isSgPragmaDeclaration(pragmaList[j]);
        //  FIXME: 15/12/2015, so far Rose backend will not rewrite the header
        //  file, so we ignore the pragma within header files
        if (codegen->isWithInHeaderFile(decl)) {
          continue;
        }
        //  do not store or handle task/reduction  pragma in the loop node
        CAnalPragma ana_pragma(codegen);
        bool errorOut;
        if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false) &&
            !ana_pragma.is_loop_related()) {
          continue;
        }

        lnode->add_pragma(ana_pragma);
        lnode->pragma_table.push_back(decl);
      }
    }

    lnode->is_while = true;
    push_back(lnode);
  }

  //  post processing to build loop hierachy
  for (iterator it = begin(); it != end(); ++it) {
    CMirNode *node = *it;
    if (node->is_function) {
      continue;
    }
    CMirNode *parent_node = get_parent_node(codegen, node, flnode);
    if (parent_node != nullptr) {
      node->set_parent(parent_node);
      parent_node->add_one_child(node);
      node->add_one_predecessor(parent_node);
    } else {
      node->set_parent(flnode);
      flnode->add_one_child(node);
      node->add_one_predecessor(flnode);
    }
  }
  top_node = flnode;
  return false;
}

CMirNode *CMirNodeSet::init_loop_node(CSageCodeGen *codegen,
                                      SgFunctionDeclaration *kernel,
                                      void *sg_node) {
  void *loop_body = codegen->GetLoopBody(sg_node);
  // Yuxin: Nov/20/2019
  // allow empty loop body
  if (codegen->IsBasicBlock(loop_body) == 0) {
    DEBUG(cout << "The statements in the for loop should be bracketed for "
                  "analysis."
               << std::endl);
    return nullptr;
  }

  CMirNode *lnode = new CMirNode(static_cast<SgBasicBlock *>(loop_body), false);
  SgNode2CMirNode[static_cast<SgBasicBlock *>(loop_body)] = lnode;
  if ((codegen->IsWhileStatement(sg_node) != 0) ||
      (codegen->IsDoWhileStatement(sg_node) != 0)) {
    m_while_nodes.push_back(lnode);
  }

  //  Initialization
  //  /////////////////////////////////////////  /
  //  ZP: CONFIRM: do not suppose to do the reset of defuse/liveness again
  //  20151124
  //    lnode->node_liveness_analysis();
  //  /////////////////////////////////////////  /

  lnode->funcname_str = get_function_name(kernel);
  lnode->clear_pragma();
  return lnode;
}

string CMirNodeSet::get_function_name(SgFunctionDeclaration *kernel_decl) {
  string func_name;
  func_name += kernel_decl->get_type()->get_return_type()->unparseToString() +
               " " + kernel_decl->get_name().str() + "(";
  const SgTypePtrList &args = kernel_decl->get_type()->get_arguments();

  for (SgTypePtrList::const_iterator i_args = args.begin();
       i_args != args.end();) {
    func_name += (*i_args)->unparseToString();

    if (++i_args != args.end()) {
      func_name += ",";
    }
  }
  func_name += ")";

  return func_name;
}

bool CMirNodeSet::isOuttermostLoop(CSageCodeGen *codegen, void *sg_loop,
                                   void *sg_scope) {
  set<void *> loops;
  codegen->get_loop_nest_from_ref(sg_scope, sg_loop, &loops, true);
  if (loops.empty()) {
    DEBUG(std::cout << "[MARS-IR-PARSING] Outtermost loop found!" << std::endl);
    return true;
  }

  return false;
}

CMirNode *CMirNodeSet::get_parent_node(CSageCodeGen *codegen, CMirNode *bNode,
                                       CMirNode *fNode) {
  void *loop = codegen->TraceUpToLoopScope(bNode->ref);
  void *scope = codegen->GetEnclosingNode("Function", fNode->ref);
  if (!isOuttermostLoop(codegen, loop, scope)) {
    void *parent_loop = codegen->TraceUpToLoopScope(loop, true);
    void *loop_body = codegen->GetLoopBody(parent_loop);
    if (codegen->IsBasicBlock(loop_body) != 0) {
      for (iterator itt = begin(); itt != end(); ++itt) {
        if ((*itt)->ref == static_cast<SgBasicBlock *>(loop_body)) {
          return *itt;
        }
      }
    } else {
      //  FIXME: can have other solution in the future, such as pre processing
      DEBUG(
          cout
          << "[MARS-IR-WARN] The statements in a for loop should be bracketed "
             "for analysis."
          << std::endl);
      return nullptr;
    }

  } else {
    return nullptr;
  }
  return nullptr;
}

CMirNode *CMirNodeSet::get_node(void *target_ref) {
  SgBasicBlock *block = isSgBasicBlock(static_cast<SgNode *>(target_ref));
  if (block == nullptr) {
    return nullptr;
  }
  if (SgNode2CMirNode.count(block) > 0) {
    return SgNode2CMirNode[block];
  }
  return nullptr;
}

void CMirNodeSet::nodeParsing(CSageCodeGen *codegen,
                              SgFunctionDeclaration *kernel_decl,
                              SgBasicBlock *body, CMirNode *flnode,
                              CMirNode *lnode, bool pragma_in_loop) {
  SgStatementPtrList &stmt_lst = body->getStatementList();
  for (auto stmt : stmt_lst) {
    if (isSgLabelStatement(stmt) != nullptr) {
      stmt = isSgLabelStatement(stmt)->get_statement();
    }
    //  FIXME: The access directly in an if statement is not counted for
    //  partition analysis (Apr. 3, 2015, Yuxin)
    if (isSgIfStmt(stmt) != nullptr) {
      DEBUG(cout << "[MARS-IR-PARSING] Find if statement." << endl << endl);
      SgIfStmt *ifstmt = isSgIfStmt(stmt);
      if (ifstmt->get_true_body() != nullptr) {
        SgBasicBlock *true_body = isSgBasicBlock(ifstmt->get_true_body());
        if (true_body != nullptr) {
          nodeParsing(codegen, kernel_decl, true_body, flnode, lnode,
                      pragma_in_loop);
        }
      }

      if (ifstmt->get_false_body() != nullptr) {
        SgBasicBlock *false_body = isSgBasicBlock(ifstmt->get_false_body());
        if (false_body != nullptr) {
          nodeParsing(codegen, kernel_decl, false_body, flnode, lnode,
                      pragma_in_loop);
        }
      }
    } else if (isSgBasicBlock(stmt) != nullptr) {
      //  Occurs when there are redundant brackets existing
      nodeParsing(codegen, kernel_decl, isSgBasicBlock(stmt), flnode, lnode,
                  pragma_in_loop);
    } else if (codegen->IsLoopStatement(stmt) == 0) {
      if (!pragma_in_loop) {
        continue;
      }
      //  Pragma parsing
      Rose_STL_Container<SgNode *> nodeList =
          NodeQuery::querySubTree(stmt, V_SgPragmaDeclaration);
      for (auto it : nodeList) {
        //  SgPragmaDeclaration *decl =
        //  isSgPragmaDeclaration(*nodeListIterator);
        SgPragmaDeclaration *decl = isSgPragmaDeclaration(it);
        ROSE_ASSERT(decl);
        //  FIXME: 15/12/2015, so far Rose backend will not rewrite the header
        //  file, so we ignore the pragma within header files
        if (codegen->isWithInHeaderFile(decl)) {
          continue;
        }
        //  do not store or handle task/reduction  pragma in the loop node
        CAnalPragma ana_pragma(codegen);
        bool errorOut;
        if (!ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false,
                                                  true) ||
            !ana_pragma.is_loop_related()) {
          continue;
        }
        lnode->add_pragma(ana_pragma);
        lnode->pragma_table.push_back(decl);
      }
    }
  }
}
