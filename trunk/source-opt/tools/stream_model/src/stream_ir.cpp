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


//  ///////////////////////////////////////////////////////////  /
//  stream_ir.cpp
//
//  A source file in Merlin compiler
//
//  Description:
//      This file provides the infrastructure for the stream model,
//      used for the streaming optimization passes. The major functionaity
//      inlcudes:
//        1. Convert from sequential code into stream model program using kernel
//        spawn
//        2. Provide the parser to establish the in-memory data structure from
//        the stream model program
//        3. Provide the functionality of the operations on the stream model
//        4. Generate the concurrent code (e.g. OpenCL) from the stream model
//        program
//
//  Author(s):
//      Peng Zhang        (2017-5-10)
//      Youxiang Chen     (2017-5-17)
//  ///////////////////////////////////////////////////////////  /

#include <iostream>
#include <string>

#include "codegen.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "rose.h"

#include "PolyModel.h"
#include "analPragmas.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

#include "stream_ir.h"
using std::exception;
//  using namespace SageInterface;
//  using namespace SageBuilder;
using MarsProgramAnalysis::CheckAccessSeparabilityInScope;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using MarsProgramAnalysis::CopyAccessInScope;
// using namespace MerlinStreamModel;
using MerlinStreamModel::CStreamBase;
using MerlinStreamModel::CStreamChannel;
using MerlinStreamModel::CStreamIR;
using MerlinStreamModel::CStreamNode;
using MerlinStreamModel::CStreamPort;

#define _DEBUG_pntr_list 0
#define _DEBUG_ref_map_detail 0

extern vector<void *> get_top_kernels(CSageCodeGen *codegen, void *pTopFunc);

//  using namespace mars_ir;
int test_stream_ir_port_gen(CMarsAST_IF *ast, void *pTopFunc,
                            const CInputOptions &);

int test_stream_ir_top(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options) {
  cout << "\n[INFO] Here we enter test_stream_ir_top() \n" << endl;

  if ("test_port_gen" == options.get_option_key_value("-a", "test_port_gen")) {
    test_stream_ir_port_gen(codegen, pTopFunc, options);
    return 0;
  }

  if ("test_spawn" == options.get_option_key_value("-a", "test_spawn")) {
    CMarsIr mars_ir;
    mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
    CMarsIrV2 mars_ir_v2;
    mars_ir_v2.build_mars_ir(codegen, pTopFunc);
    CStreamIR *sir = new CStreamIR(codegen, options, &mars_ir, &mars_ir_v2);
    sir->SpawnKernelTransform();
    delete sir;
  }

  if ("test_codegen" == options.get_option_key_value("-a", "test_codegen")) {
    CMarsIr mars_ir;
    mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
    CMarsIrV2 mars_ir_v2;
    mars_ir_v2.build_mars_ir(codegen, pTopFunc);
    CStreamIR *sir = new CStreamIR(codegen, options, &mars_ir, &mars_ir_v2);
    sir->ParseStreamModel();
    auto top_kernels = get_top_kernels(codegen, pTopFunc);
    map<void *, vector<void *>> kernel_autokernel;
    sir->ConcurrentCodeGeneration(top_kernels, &kernel_autokernel);
    delete sir;
  }
  return 1;
}
int test_stream_ir_port_gen(CMarsAST_IF *ast, void *pTopFunc,
                            const CInputOptions &options) {
  cout << "\n[INFO] Here we enter test_stream_ir_port_gen() \n" << endl;

  CMarsIr mir;
  mir.get_mars_ir(ast, ast->GetProject(), options, true);
  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(ast, pTopFunc);
  CStreamIR sir(ast, options, &mir, &mars_ir_v2);

  vector<CMirNode *> vec_nodes;
  mir.get_topological_order_nodes(&vec_nodes);

  for (auto node : vec_nodes) {
    if (node->is_kernel) {
      void *func = ast->TraceUpToFuncDecl(node->ref);
      assert(node->ref == ast->GetFuncBody(func));

      CStreamNode *sp_node = sir.CreateNode(func, &mir, &mars_ir_v2);

      sp_node->print_ref();
      CStreamBase sp_node_base = *sp_node;
      sp_node_base.print();
      sp_node_base.print_ref();
      map<void *, void *> map_spref2snref;
      vector<void *> vec_params;

      vector<CMerlinMessage> vec_msg;
      ast->reset_func_call_cache();
      ast->reset_func_decl_cache();
      ast->init_defuse_range_analysis();
      if (sir.CheckStreamPortSeparability(func, &vec_msg) == 0) {
        cout << "StreamPort is not generated: in function "
             << ast->GetFuncName(func) << endl;
        for (auto msg : vec_msg) {
          cout << "-- " << msg.get_message() << endl;
        }
        cout << endl;

        continue;
      }

      for (auto arg : ast->GetFuncParams(func)) {
        //  build stream port object

        ast->reset_func_call_cache();
        ast->reset_func_decl_cache();
        ast->init_defuse_range_analysis();
        CMerlinMessage msg;

        vector<CStreamPort *> vec_port = sir.CreateStreamPortBySeparation(
            sp_node, arg, &map_spref2snref, &msg);
        if (vec_port.empty()) {
          cout << "StreamPort is not generated: " << ast->_p(arg)
               << " in function " << ast->GetFuncName(func) << endl;
          cout << "-- " << msg.get_message() << endl;
          cout << endl;
        } else {
          cout << "StreamPort is generated: " << ast->_p(arg) << " in function "
               << ast->GetFuncName(func) << endl;
        }
        for (auto port : vec_port) {
          port->print_ref();
        }

#if 0
        int is_write = 0;
        void * sg_port =
            MarsProgramAnalysis::CopyAccessInScope(ast,
              arg, func, func, &is_write, &map_spref2snref, &vec_params);

        if (!ast->IsStatement(sg_port)) continue;

        void * insert_pos = ast->GetFuncBody(func);

        //  add the ref statement into ast
        ast->PrependChild(insert_pos, sg_port);

        //  wrap into funcion call
        void * sp_call; void * sp_decl;
        string str_sp_name = "__msm_port_"+ast->_up(arg)+"_"i
          +ast->GetFuncName(func)+"_";
        ast->function_outline(sg_port, &sp_call, &sp_decl, str_sp_name,
                              true, false, false, false);

        //  build stream port object
        CStreamPort * port = sir.CreatePort(sp_call, arg, sir.CreateNode(func));

        //  add the pragma into ast
        port->set_attribute("node",  ast->GetFuncName(func));
        port->set_attribute("array", ast->_up(arg));
        port->set_attribute("dim_num",
            my_itoa(ast->get_dim_num_from_var(arg)));

        // TODO(Han): to be replaced as attributes
        string str_pragma = "ACCEL " + PRAGMA_TYPE_PORT +" ";
        str_pragma += port->print_attribute();
        ast->PrependChild(insert_pos,
            ast->CreatePragma(str_pragma, ast->GetFuncBody(func)));

        cout << "One port: " << port->print() << endl;
        cout << "  " << port->print_content() << endl;
#endif
      }
    }
  }

  return 0;
}

//  simply get the previous stmt for the pragma, can be overloaded
void *CStreamBase::get_pragma_by_ref() {
  if (nullptr == m_ref) {
    return nullptr;
  }

  void *pre_stmt = m_ast->GetPreviousStmt(m_ref);

  if (m_ast->IsPragmaDecl(pre_stmt) != 0) {
    return pre_stmt;
  }
  { return nullptr; }
}

string CStreamBase::print_attribute() {
  string ret;
  for (auto it : m_attr) {
    ret += it.first + "=" + it.second + " ";
  }
  return ret;
}

CStreamNode *CStreamIR::CreateNode(void *ref, CMarsIr *mars_ir,
                                   CMarsIrV2 *mars_ir_v2) {
  CStreamNode *ret = new CStreamNode(m_ast, ref, mars_ir, mars_ir_v2);
  m_nodes.push_back(ret);
  return ret;
}
CStreamPort *CStreamIR::CreatePort(void *ref, void *array, CStreamNode *node,
                                   CMarsIr *mars_ir, CMarsIrV2 *mars_ir_v2) {
  CStreamPort *ret =
      new CStreamPort(m_ast, ref, array, node, mars_ir, mars_ir_v2);
  m_ports.push_back(ret);
  return ret;
}
CStreamChannel *CStreamIR::CreateChannels(void *ref) {
  CStreamChannel *ret = new CStreamChannel(m_ast, ref);
  m_fifos.push_back(ret);
  return ret;
}
#if 0
CStreamBuffer *CStreamIR::CreateBuffer(void *ref) {
  CStreamBuffer *ret = new CStreamBuffer(m_ast, ref);
  m_buffers.push_back(ret);
  return ret;
}
#endif

//  This function transform a sequential program into a stream model
//  Input:
//    pragma ACCEL spawn_kernel
//  Process:
//    Spawn kernels on the pragma, this is used as an example for CGPar and
//    CGPip where SpawnNodeFromAST will be also called to do the model transform
//  Output:
//    1. sub_kernel with stream_node pragma
//    2. separated s2a or a2s transfer with stream_port pragma
//    3. merlin_stream
//    4. stream_buffer pragma
void CStreamIR::SpawnKernelTransform() {
  CMarsAST_IF *ast = get_ast();

  CMarsIr mir;
  mir.get_mars_ir(ast, ast->GetProject(), mOptions, true);
  vector<CMirNode *>
      vec_nodes;  //  all the stmts followed by #pragma ACCEL spawn
  mir.get_topological_order_nodes(&vec_nodes);

  for (auto node : vec_nodes) {
    void *loop_body = node->ref;

    //  cout << "One node: " << ast->_p(loop_body) << endl;

    if (node->has_spawn() != 0) {
      string err_msg;
      SpawnNodeFromAST(loop_body, &err_msg);
    }
  }
}

// TODO(youxiang): Peng
CStreamNode *CStreamIR::FindNodeByAST(void *stmt) {
  CStreamNode *ret = nullptr;
  for (auto node : m_nodes) {
    if (node->get_ref() == stmt) {
      return node;
    }
  }
  return ret;
}

// TODO(youxiang): Peng
CStreamPort *CStreamIR::FindPortByAST(void *ref) {
  CStreamPort *ret = nullptr;
  for (auto port : m_ports) {
    if (port->get_ref() == ref) {
      return port;
    }
  }
  return ret;
}

#if 0
// TODO(Han): Peng
CStreamPort *CStreamIR::FindMatchPort(void *ref) { return nullptr; }

// TODO(Yuxin): add by Yuxin
CStreamChannel *CStreamIR::FindChannelByAST(void *ref) { return nullptr; }

// TODO(Yuxin): add by Yuxin
CStreamBuffer *CStreamIR::FindBufferByAST(void *ref) { return nullptr; }
#endif
//  replace the access of the scalar variable with a local copy, and reuse the
//  copy for all the local references
void scalar_refine_for_function_snode(CMarsAST_IF *ast, void *var, void *func,
                                      map<void *, void *> *map_spref2snref) {
#if 1
  void *func_body = ast->GetFuncBody(func);
  assert(func_body);

  //  used as a mark for adding declarations
  void *assign_insert_pos =
      ast->CreateStmt(V_SgExprStatement, ast->CreateConst(1));
  ast->PrependChild(func_body, assign_insert_pos);
  void *decl_insert_pos =
      ast->CreateStmt(V_SgExprStatement, ast->CreateConst(0));
  ast->PrependChild(func_body, decl_insert_pos);

  map<void *, void *> map_ref2expr;
  void *new_ref = nullptr;
  for (auto arg : ast->GetFuncParams(func)) {
    if (arg != var) {
      continue;
    }

    //  1. only work for scalar
    if (0 != ast->get_dim_num_from_var(arg)) {
      continue;
    }

    //  2. check write/read
    int is_write = ast->has_write_in_scope(arg, func_body);
    int is_read = ast->has_read_in_scope(arg, func_body);

    //  3. generate new var declaration
    CNameConflictSolver conflict_solver(ast, func_body);
    void *new_var = ast->CreateVariableDecl(
        ast->GetTypebyVar(arg),
        conflict_solver.get_and_assign(
            "_" + ast->_up(arg)),  //  add "_" prefix for the generated iterator
        nullptr, func_body);
    ast->InsertStmt(new_var, decl_insert_pos);

    //  4. get replacement list
    map<void *, void *> map_var2expr;
    map_var2expr[arg] = ast->CreateVariableRef(new_var);
    ast->get_ref_map_from_iter_map(func_body, map_var2expr, &map_ref2expr);

    new_ref = ast->CreateVariableRef(arg);
    //  5. insert assigment of new var
    if (is_read != 0) {
      ast->InsertStmt(
          ast->CreateStmt(V_SgExprStatement,
                          ast->CreateExp(V_SgAssignOp,
                                         ast->CreateVariableRef(new_var),
                                         new_ref)),
          assign_insert_pos);
    } else if (is_write != 0) {
      ast->AppendChild(
          func_body,
          ast->CreateStmt(V_SgExprStatement,
                          ast->CreateExp(V_SgAssignOp, new_ref,
                                         ast->CreateVariableRef(new_var))));
    } else {
      new_ref = nullptr;
      continue;
    }
  }

  //  6. cleaning up
  if (ast->IsStatement(decl_insert_pos) != 0) {
    ast->RemoveStmt(decl_insert_pos);
  }
  if (ast->IsStatement(assign_insert_pos) != 0) {
    ast->RemoveStmt(assign_insert_pos);
  }

  //  7. apply the replacement list
  //  int no_copy = true;
  //  void * new_body =
  ast->copy_and_replace_var_ref(func_body, map_ref2expr, nullptr, 1);
  //  ast->ReplaceChild(func_body, new_body);

  //  8. update the reference map
  for (auto &it : *map_spref2snref) {
    if (it.second == var) {
#if _DEBUG_ref_map_detail
      cout << " -- sn_scalar_refine old_ref " << it.second << " "
           << ast->_p(it.second) << endl;
      cout << " -- sn_scalar_refine new_ref " << new_ref << " "
           << ast->_p(new_ref) << endl;
#endif
      if (new_ref != nullptr) {
        it.second = new_ref;
      } else {
        map_spref2snref->erase(map_spref2snref->find(it.first));
        break;
      }
    }
  }
#endif
}

//  replace the access of the scalar variable with a local copy, and reuse the
//  copy for all the local references
void scalar_refine_for_function(CMarsAST_IF *ast, void *func,
                                map<void *, void *> *map_spref2snref) {
#if 1
  void *func_body = ast->GetFuncBody(func);
  assert(func_body);

  //  used as a mark for adding declarations
  void *assign_insert_pos =
      ast->CreateStmt(V_SgExprStatement, ast->CreateConst(1));
  ast->PrependChild(func_body, assign_insert_pos);
  void *decl_insert_pos =
      ast->CreateStmt(V_SgExprStatement, ast->CreateConst(0));
  ast->PrependChild(func_body, decl_insert_pos);

  map<void *, void *> map_ref2expr;
  for (auto arg : ast->GetFuncParams(func)) {
    //  1. only work for scalar
    if (0 != ast->get_dim_num_from_var(arg)) {
      continue;
    }

    //  ZP: 20170612
    void *old_ref = nullptr;
    {
      vector<void *> old_arg_ref;
      ast->get_ref_in_scope(arg, func, &old_arg_ref);

      if (old_arg_ref.size() == 1) {
        old_ref = old_arg_ref[0];
      }
    }

    //  2. check write/read
    int is_write = ast->has_write_in_scope(arg, func_body);
    int is_read = ast->has_read_in_scope(arg, func_body);

    //  3. generate new var declaration
    CNameConflictSolver conflict_solver(ast, func_body);
    void *new_var = ast->CreateVariableDecl(
        ast->GetTypebyVar(arg),
        conflict_solver.get_and_assign(
            "_" + ast->_up(arg)),  //  add "_" prefix for the generated iterator
        nullptr, func_body);
    ast->InsertStmt(new_var, decl_insert_pos);

    //  4. get replacement list
    map<void *, void *> map_var2expr;
    void *new_var_ref = ast->CreateVariableRef(new_var);
    map_var2expr[arg] = new_var_ref;
    ast->get_ref_map_from_iter_map(func_body, map_var2expr, &map_ref2expr);

    //  5. insert assigment of new var
    void *var_ref = ast->CreateVariableRef(arg);
    if (is_read != 0) {
      ast->InsertStmt(
          ast->CreateStmt(V_SgExprStatement,
                          ast->CreateExp(V_SgAssignOp, new_var_ref, var_ref)),
          assign_insert_pos);
    } else if (is_write != 0) {
      ast->AppendChild(
          func_body,
          ast->CreateStmt(V_SgExprStatement,
                          ast->CreateExp(V_SgAssignOp, var_ref, new_var_ref)));
    }

    //  ZP: 20170612
    //  8. update the reference map
    {
      if (old_ref != nullptr) {
        auto it = map_spref2snref->find(old_ref);
        if (it != map_spref2snref->end()) {
          pair<void *, void *> new_pair(var_ref, it->second);
#if _DEBUG_ref_map_detail
          cout << " -- sp_scalar_refine old_ref " << old_ref << " "
               << ast->_p(old_ref) << endl;
          void *new_ref = var_ref;
          cout << " -- sp_scalar_refine new_ref " << new_ref << " "
               << ast->_p(new_ref) << endl;
#endif
          map_spref2snref->erase(it);
          map_spref2snref->insert(new_pair);
        }
      }
    }
  }

  //  6. cleaning up
  if (ast->IsStatement(decl_insert_pos) != 0) {
    ast->RemoveStmt(decl_insert_pos);
  }
  if (ast->IsStatement(assign_insert_pos) != 0) {
    ast->RemoveStmt(assign_insert_pos);
  }

  //  7. apply the replacement list
  //  int no_copy = true;
  //  void * new_body =
  ast->copy_and_replace_var_ref(func_body, map_ref2expr, nullptr, 1);
  //  ast->ReplaceChild(func_body, new_body);

  // TODO(youxiang): note: the scalar parameters are not updated in
  // map_spref2snref

#endif
}
map<void *, void *> update_ref2ref_map(map<void *, void *> map_spref2snref,
                                       vector<void *> pntr_list_org,
                                       vector<void *> pntr_list_new) {
  assert(pntr_list_org.size() == pntr_list_new.size());

  map<void *, void *> map_org2new;
  for (size_t i = 0; i < pntr_list_org.size(); i++) {
    map_org2new.insert(
        pair<void *, void *>(pntr_list_org[i], pntr_list_new[i]));
  }

  map<void *, void *> map_changes;
  for (auto one_pair : map_spref2snref) {
    if (map_org2new.find(one_pair.first) != map_org2new.end()) {
      map_changes.insert(
          pair<void *, void *>(one_pair.first, map_org2new[one_pair.first]));
    }
  }

  for (auto one_change : map_changes) {
    auto i = map_spref2snref.find(one_change.first);
    assert(i != map_spref2snref.end());
    auto value = i->second;
    map_spref2snref.erase(i);
    map_spref2snref[one_change.second] = value;
  }
  return map_spref2snref;
}

vector<void *> get_pntr_list(CMarsAST_IF *ast, void *scope) {
  vector<void *> v_ref;
  ast->GetNodesByType(scope, "preorder", "SgVarRefExp", &v_ref);

  vector<void *> v_pntr;
  for (auto ref : v_ref) {
    //    void * sg_pntr = ast->get_pntr_from_array(ref, ref);

    void *curr_ref = ref;

    void *sg_array;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim;
    ast->parse_pntr_ref_by_array_ref(curr_ref, &sg_array, &sg_pntr, &sg_indexes,
                                     &pointer_dim);

    assert(sg_pntr);
    assert(sg_array);

    v_pntr.push_back(sg_pntr);
  }

  return v_pntr;
}

#if _DEBUG_pntr_list
void print_pntr_list(vector<void *> list, CMarsAST_IF *ast) {
  for (auto pntr : list) {
    cout << "  " << pntr << ": " << ast->_p(pntr) << endl;
  }
}
#endif

int CStreamIR::CheckStreamPortSeparability(CStreamNode *node, void *sg_arg,
                                           CMerlinMessage *msg) {
  //  0. Praparation for the AST information
  CMarsAST_IF *ast = m_ast;

  void *func = nullptr;
  //  int node_type = 0; //  standard: node ref is a function call

  if (ast->IsExprStatement(node->get_ref()) !=
      0) {  //  standard type: a node ref is a call_ref
    //  node_type = 0;
    void *call = ast->GetExprFromStmt(node->get_ref());
    func = ast->GetFuncDeclByCall(call);
  } else if (ast->IsFunctionDeclaration(node->get_ref()) !=
             0) {  //  alternative: a node ref is a func decl
    //  node_type = 1;
    func = node->get_ref();
  }
  void *func_body = ast->GetFuncBody(func);
  return CheckAccessSeparabilityInScope(ast, sg_arg, func_body, func_body, msg);
}

//  ZP: 20170612: assume no global variables
int CStreamIR::CheckStreamPortSeparability(void *sg_scope,
                                           vector<CMerlinMessage> *vec_msg,
                                           set<void *> *checked_names) {
  CMarsAST_IF *ast = m_ast;
  set<string> copy_in_vars;
  set<string> copy_out_vars;
  vector<void *> vec_pragma;
  ast->GetNodesByType_int(sg_scope, "preorder", V_SgPragmaDeclaration,
                          &vec_pragma);
  for (auto pragma : vec_pragma) {
    CAnalPragma pragma_info(ast);
    bool errout;
    pragma_info.PragmasFrontendProcessing(pragma, &errout, false);
    string copy_in_str = pragma_info.get_attribute(CMOST_copy_in);
    vector<string> vec_copy_in = str_split(copy_in_str, ',');
    copy_in_vars.insert(vec_copy_in.begin(), vec_copy_in.end());
    string copy_out_str = pragma_info.get_attribute(CMOST_copy_out);
    vector<string> vec_copy_out = str_split(copy_out_str, ',');
    copy_out_vars.insert(vec_copy_out.begin(), vec_copy_out.end());
  }
  void *ref_scope = sg_scope;
  if (ast->IsFunctionDeclaration(ref_scope) != 0) {
    ref_scope = ast->GetFuncBody(ref_scope);
  }
  //  1. get all the port variables
  //  ZP: 20170612: assume no global variables
  set<void *> init_names;
  if (checked_names == nullptr) {
    vector<void *> v_ref;
    ast->GetNodesByType(ref_scope, "preorder", "SgVarRefExp", &v_ref);
    for (auto ref : v_ref) {
      void *decl = ast->GetVariableDecl(ref);

      if ((decl != nullptr) &&
          (ast->is_located_in_scope(decl, ref_scope) != 0)) {
        continue;
      }

      void *init = ast->GetVariableInitializedName(ref);
      //  20180609 Youxiang do not check copy in/out variables
      string var_name = ast->GetVariableName(init);
      if (copy_in_vars.count(var_name) > 0 ||
          copy_out_vars.count(var_name) > 0) {
        continue;
      }
      void *type = ast->GetTypebyVar(init);
      //  skip constant global variable
      if ((ast->IsConstType(type) != 0) && (ast->IsGlobalInitName(init) != 0)) {
        continue;
      }
      //  skip loop iterator
      if (ast->GetLoopFromIteratorByPos(init, ref, 1, 1) != nullptr) {
        continue;
      }
      //  skip merlin interal streaming channel
      if (ast->GetStringByType(ast->GetBaseType(type, true))
              .find("merlin_stream") != string::npos) {
        continue;
      }

      init_names.insert(init);
    }
  } else {
    init_names = *checked_names;
  }

  int valid = 1;
  for (auto init : init_names) {
    if (init != nullptr) {
      CMerlinMessage msg;
      if (CheckAccessSeparabilityInScope(ast, init, ref_scope, ref_scope,
                                         &msg) == 0) {
        valid = 0;
        vec_msg->push_back(msg);
      }
    }
  }

  return valid;
}

// TODO(youxiang): Peng
vector<CStreamPort *> CStreamIR::CreateStreamPortBySeparation(
    CStreamNode *node, void *sg_arg, map<void *, void *> *map_spref2snref,
    CMerlinMessage *msg, bool is_skip_node_create, void *sg_kernel) {
  DEFINE_START_END;
  CMarsAST_IF *ast = m_ast;
  vector<CStreamPort *> ret_port_vec;

  void *func = nullptr;
  void *insert_pos_r = nullptr;
  void *insert_pos_w = nullptr;
  int node_type = 0;  //  standard
  void *sn_decl = nullptr;

  void *node_ref = (is_skip_node_create) ? sg_kernel : node->get_ref();

  STEMP(start);
  if (ast->IsExprStatement(node_ref) !=
      0) {  //  standard type: a node ref is a call_ref
    node_type = 0;
    void *call = ast->GetExprFromStmt(node_ref);
    func = ast->GetFuncDeclByCall(call);

    insert_pos_r = node_ref;
    insert_pos_w = node_ref;
    while (ast->IsPragmaDecl(ast->GetPreviousStmt(insert_pos_r)) != 0) {
      insert_pos_r = ast->GetPreviousStmt(insert_pos_r);
    }
  } else if (ast->IsFunctionDeclaration(node_ref) !=
             0) {  //  alternative: a node ref is a func decl
    node_type = 1;
    insert_pos_r = insert_pos_w = ast->GetFuncBody(node_ref);
    func = node_ref;
  }
  sn_decl = func;
  TIMEP("CreateStreamPortBySeparation___init", start, end);

  assert(insert_pos_w && insert_pos_r);

  if (!is_skip_node_create) {
    STEMP(start);
    //  0. Check separability
    if (CheckStreamPortSeparability(node, sg_arg, msg) == 0) {
      dump_critical_message(
          STREAM_ERROR_2(ast->_up(sg_arg), msg->get_message()));
      throw std::exception();
    }
    TIMEP("CreateStreamPortBySeparation___CheckStreamPortSeparability", start,
          end);
  }

  vector<void *> vec_params;

  void *arg = sg_arg;
  void *func_body = ast->GetFuncBody(func);
  //  for (auto arg : ast->GetFuncParams(func))
  {
    int is_write = 0;

    STEMP(start);
    //  1. copy the access and create the basicblock (no basicblock for scalar
    //  port)
    void *sg_port = MarsProgramAnalysis::CopyAccessInScope(
        ast, arg, func_body, func_body, &is_write, map_spref2snref,
        &vec_params);
    //  assert(ast->IsStatement(sg_port));
    if (ast->IsStatement(sg_port) == 0) {
      dump_critical_message(STREAM_ERROR_1(ast->_p(arg)));
      throw std::exception();
    }
    TIMEP("CreateStreamPortBySeparation___CopyAccessInScope", start, end);

    STEMP(start);
    //  2. add the basicblock into ast
    if (0 == node_type) {  //  node ref is a call
      if (is_write != 0) {
        ast->InsertAfterStmt(sg_port, insert_pos_w);
      } else {
        ast->InsertStmt(sg_port, insert_pos_r);
      }
    } else if (1 == node_type) {  //  node ref is a func decl
      if (is_write != 0) {
        ast->AppendChild(insert_pos_w, sg_port);
      } else {
        ast->PrependChild(insert_pos_r, sg_port);
      }
    }
    setParallel(ast, sg_port, true /*parallel*/, true /*recursive*/);
    TIMEP("CreateStreamPortBySeparation___InsertStmt", start, end);

#if _DEBUG_ref_map_detail
    {
      cout << "-- ref_map:" << endl;
      for (auto iter : *map_spref2snref) {
        cout << "  " << iter.first << " " << ast->_p(iter.first) << "@"
             << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.first)) << " -> "
             << iter.second << " " << ast->_p(iter.second) << "@"
             << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.second)) << endl;
      }
    }
#endif

#if 1
    STEMP(start);
    //  3. outline the basicblock into stream port funcion
    vector<void *> pntr_list_org = get_pntr_list(ast, sg_port);
#if _DEBUG_pntr_list
    {
      cout << "-- pre-outline  pntr_list:" << endl;
      print_pntr_list(pntr_list_org, ast);
    }
#endif
    vector<void *> pntr_list_new;
    vector<pair<void *, void *>> call_decl_list;
    int num_access = 0;
    for (auto sg_stmt : ast->GetChildStmts(sg_port)) {
      void *sp_call;
      void *sp_decl;
      string str_sp_name =
          "msm_port_" + ast->_up(arg) + "_" + ast->GetFuncName(func) + "_";
      if (num_access > 0) {
        str_sp_name += my_itoa(num_access++) + "_";
      }
      ast->function_outline(sg_stmt, &sp_call, &sp_decl, str_sp_name, true,
                            false, false, false, true);
      auto sub_pntr_list_new = get_pntr_list(ast, sp_decl);
      pntr_list_new.insert(pntr_list_new.end(), sub_pntr_list_new.begin(),
                           sub_pntr_list_new.end());
      call_decl_list.emplace_back(sp_call, sp_decl);
    }
#if _DEBUG_pntr_list
    {
      cout << "-- post_outline pntr_list:" << endl;
      print_pntr_list(pntr_list_new, ast);
    }
#endif
    *map_spref2snref =
        update_ref2ref_map(*map_spref2snref, pntr_list_org, pntr_list_new);
    TIMEP("CreateStreamPortBySeparation___function_outline", start, end);

#if _DEBUG_ref_map_detail
    {
      cout << "-- ref_map:" << endl;
      for (auto iter : *map_spref2snref) {
        cout << "  " << iter.first << " " << ast->_p(iter.first) << "@"
             << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.first)) << " -> "
             << iter.second << " " << ast->_p(iter.second) << "@"
             << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.second)) << endl;
      }
    }
#endif
    for (auto call_decl : call_decl_list) {
      void *sp_call = call_decl.first;
      void *sp_decl = call_decl.second;
      void *sp_call_stmt = ast->TraceUpToStatement(sp_call);
      CStreamPort *ret_port =
          CreatePort(sp_call_stmt, arg, node, m_mars_ir, m_mars_ir_v2);

      STEMP(start);
      //  4. scalar refinement in the port function body
      scalar_refine_for_function(
          ast, sp_decl,
          map_spref2snref);  //  ZP: 20170609: map of scalar is not used yet
      TIMEP("CreateStreamPortBySeparation___scalar_refine_for_function", start,
            end);

      //  if (node_type == 0)
      {
        //  5. scalar refinement in the node function body
        STEMP(start);
        scalar_refine_for_function_snode(ast, arg, sn_decl, map_spref2snref);
        TIMEP("CreateStreamPortBySeparation___scalar_refine_for_function_snode",
              start, end);
      }
#else
    CStreamPort *ret_port = CreatePort(sg_port, arg, node);
#endif
      STEMP(start);
      //  5. add the pragma for the stream port
      ret_port->set_attribute(PRAGMA_NODE_ATTRIBUTE, ast->GetFuncName(func));
      ret_port->set_attribute(PRAGMA_VARIABLE_ATTRIBUTE, ast->_up(arg));
      ret_port->set_attribute(PRAGMA_DIM_NUM_ATTRIBUTE,
                              my_itoa(ast->get_dim_num_from_var(arg)));
      ret_port->set_attribute(PRAGMA_ACCESS_TYPE_ATTRIBUTE,
                              is_write != 0 ? "write" : "read");
      //  if (prefetch) {
      //   ret_port->set_attribute(PRAGMA_SPAWN_ATTRIBUTE, "1");
      //   ret_port->set_attribute(PRAGMA_PREFETCH_ATTRIBUTE, "1");
      //  }
      ret_port->update_pragma();
      TIMEP("CreateStreamPortBySeparation___add_pragma", start, end);

#if 1
      {
        cout << "-- ref_map:" << endl;
        for (auto iter : *map_spref2snref) {
#if _DEBUG_ref_map_detail
          {
            cout << "  " << iter.first << " " << ast->_p(iter.first) << "@"
                 << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.first))
                 << " -> " << iter.second << " " << ast->_p(iter.second) << "@"
                 << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.second))
                 << endl;
          }
#else
          cout << "  " << ast->_p(iter.first) << "@"
               << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.first)) << " -> "
               << ast->_p(iter.second) << "@"
               << ast->GetFuncName(ast->TraceUpToFuncDecl(iter.second)) << endl;
        }
#endif
        }
#endif

        cout << "** Stream port generated: " << ret_port->print_content()
             << endl;
        cout << endl;
        ret_port_vec.push_back(ret_port);
      }
    }

    return ret_port_vec;
  }

  int CStreamPort::get_sufficient_channel_size(void *sp_ref) {
    CMarsAST_IF *ast = m_ast;
    CMarsIrV2 mars_ir(m_ast);
    //  if (m_map_ref2fifo.end() == m_map_ref2fifo.find(sp_ref))
    //  {
    //  return -1;
    //  }
    //  else
    {
      void *array = ast->get_array_from_pntr(sp_ref);
      if (array == nullptr) {
        return -1;
      }

      if (ast->IsScalarType(ast->GetTypebyVar(array)) != 0) {
        return DEFAULT_CHANNEL_DEPTH;
      }

      //  find all the surronding for loops in the SP function

      void *sp_decl;
      void *sp_call;
      get_func_call_decl(&sp_call, &sp_decl);

      assert(ast->IsFunctionDeclaration(sp_decl));
      assert(ast->GetFuncBody(sp_decl));

      vector<void *> loops;
      ast->get_surrounding_control_stmt(sp_ref, ast->GetFuncBody(sp_decl),
                                        &loops);

      int trip_count = 1;
      for (auto loop : loops) {
        if (ast->IsLoopStatement(loop) != 0) {
          //  YX: skip completely unroll loop
          CMarsLoop *loop_info = mars_ir.get_loop_info(loop);
          if (loop_info->is_complete_unroll() != 0) {
            continue;
          }
#if 0
        void * iter_ref = ast->CreateVariableRef(ast->GetLoopIterator(loop));
        CMarsExpression me(iter_ref, ast, sp_ref);
        CMarsRangeExpr mr = me.get_range();
#else
        CMarsRangeExpr mr = CMarsVariable(loop, ast, sp_ref).get_range();
#endif
          if ((mr.is_const_lb() == 0) || (mr.is_const_ub() == 0)) {
            return -1;
          }
          int lb = mr.get_const_lb();
          int ub = mr.get_const_ub();

          trip_count *= (ub - lb + 1);
        }
      }

      return trip_count;
    }

    return 0;
  }

#if 0
// TODO(Peng): Peng
void CStreamIR::InheritPort(CStreamPort *s_p, CStreamNode *s_n_parent,
                            CStreamNode *s_n_child) {}
// TODO(Peng): Peng
void CStreamIR::ConnectPort(CStreamPort *s_p0, CStreamPort *s_p1) {}
// TODO(Peng): Peng
void CStreamIR::EmbedPort(CStreamPort *s_p0, void *embed_pos) {}
#endif
  string CStreamNode::print_ref() {
    if (m_ast->IsExprStatement(m_ref) != 0) {
      void *call_expr = m_ast->GetExprFromStmt(m_ref);
      assert(m_ast->IsFunctionCall(call_expr));
      return m_ast->GetFuncNameByCall(call_expr);
    }
    if (m_ast->IsFunctionCall(m_ref) != 0) {
      return m_ast->GetFuncNameByCall(m_ref);
    }
    if (m_ast->IsFunctionDeclaration(m_ref) != 0) {
      return m_ast->GetFuncName(m_ref);
    }
    { return CStreamBase::print_ref(); }
  }
#if 0
void *CStreamNode::get_scope() {
  if (m_ast->IsExprStatement(m_ref)) {
    void *call_expr = m_ast->GetExprFromStmt(m_ref);
    assert(m_ast->IsFunctionCall(call_expr));
    void *func_decl = m_ast->GetFuncDeclByCall(call_expr, 1);
    assert(func_decl);
    return m_ast->GetFuncBody(func_decl);
  }
  if (m_ast->IsFunctionCall(m_ref)) {
    void *func_decl = m_ast->GetFuncDeclByCall(m_ref, 1);
    assert(func_decl);
    return m_ast->GetFuncBody(func_decl);
  }
  if (m_ast->IsFunctionDeclaration(m_ref)) {
    return m_ast->GetFuncBody(m_ref);
  } else {
    return m_ref;
  }
}
#endif

  void *CStreamNode::get_pos() {
    void *curr_pos = m_ref;
    if (m_ast->IsFunctionCall(m_ref) != 0) {
      curr_pos = m_ast->TraceUpToStatement(m_ref);
    }
    if (m_ast->IsFunctionDeclaration(m_ref) != 0) {
      vector<void *> vec_calls;
      m_ast->GetFuncCallsFromDecl(m_ref, nullptr, &vec_calls);
      if (vec_calls.size() == 1) {
        curr_pos = m_ast->TraceUpToStatement(vec_calls[0]);
      }
    }
    if (m_ast->IsStatement(curr_pos) == 0) {
      curr_pos = m_ast->TraceUpToStatement(curr_pos);
    }
    return curr_pos;
  }

#if 0
void *CStreamPort::get_scope() {
  if (m_ast->IsExprStatement(m_ref)) {
    void *call_expr = m_ast->GetExprFromStmt(m_ref);
    assert(m_ast->IsFunctionCall(call_expr));
    void *func_decl = m_ast->GetFuncDeclByCall(call_expr, 1);
    assert(func_decl);
    return m_ast->GetFuncBody(func_decl);
  }
  if (m_ast->IsFunctionCall(m_ref)) {
    void *func_decl = m_ast->GetFuncDeclByCall(m_ref, 1);
    assert(func_decl);
    return m_ast->GetFuncBody(func_decl);
  }
  if (m_ast->IsFunctionDeclaration(m_ref)) {
    return m_ast->GetFuncBody(m_ref);
  } else {
    return m_ref;
  }
}
#endif

  void *CStreamPort::get_pos() {
    void *curr_pos = m_ref;
    if (m_ast->IsFunctionCall(m_ref) != 0) {
      curr_pos = m_ast->TraceUpToStatement(m_ref);
    }
    if (m_ast->IsFunctionDeclaration(m_ref) != 0) {
      vector<void *> vec_calls;
      m_ast->GetFuncCallsFromDecl(m_ref, nullptr, &vec_calls);
      if (vec_calls.size() == 1) {
        curr_pos = m_ast->TraceUpToStatement(vec_calls[0]);
      }
    }
    if (m_ast->IsStatement(curr_pos) == 0) {
      curr_pos = m_ast->TraceUpToStatement(curr_pos);
    }
    return curr_pos;
  }
  void CStreamIR::clear() {
    //  delete all the elements in side
    for (auto node : m_nodes) {
      delete node;
    }
    m_nodes.clear();
    for (auto port : m_ports) {
      delete port;
    }
    m_ports.clear();
    for (auto buffer : m_buffers) {
      delete buffer;
    }
    m_buffers.clear();
    for (auto fifo : m_fifos) {
      delete fifo;
    }
    m_fifos.clear();
  }

  bool CStreamIR::is_fifo(void *var) {
    for (auto fifo : m_fifos) {
      void *fifo_decl = fifo->get_ref();
      void *fifo_init = m_ast->GetVariableInitializedName(fifo_decl);
      cout << "fifo: " << m_ast->_p(fifo_init) << endl;
      if (fifo_init == var) {
        return true;
      }
    }
    return false;
  }
