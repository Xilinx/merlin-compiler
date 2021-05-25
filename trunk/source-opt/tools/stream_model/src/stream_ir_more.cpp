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
//  stream_ir_more.cpp
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
//      Peng Zhang    (2017-5-10) peng@falcon-computing.com
//      Youxiang Chen (2017-5-17) youxiangchen@falcon-computing.com
//  ///////////////////////////////////////////////////////////  /

#include <iostream>
#include <string>

#include "codegen.h"
#include "mars_ir.h"
#include "rose.h"

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "kernel_wrapper.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

#include "stream_ir.h"
using std::exception;
using std::list;
using std::ofstream;
using std::reverse;
using std::to_string;
//  using namespace SageInterface;
//  using namespace SageBuilder;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using MarsProgramAnalysis::CopyAccessInScope;
using MerlinStreamModel::CStreamBase;
using MerlinStreamModel::CStreamChannel;
using MerlinStreamModel::CStreamIR;
using MerlinStreamModel::CStreamNode;
using MerlinStreamModel::CStreamPort;

// TODO(youxiang): Youxiang (1h)
//  if sg_pragma is nullptr, the statement before m_ref will be tried for the
//  sg_pragma if sg_pragma is still nullptr, a new pragma will be generated
void CStreamBase::parse_pragma(
    void *sg_pragma /* = nullptr*/) {  //  pragma -> attribute
  if (sg_pragma == nullptr) {
    return;
  }
  string str_pragma = m_ast->GetPragmaString(sg_pragma);
  map<string, pair<vector<string>, vector<string>>> mapParams;
  string sFilter;
  string sCmd;
  tldm_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams);

  if (is_cmost_pragma(sFilter) == 0) {
    return;
  }
  boost::algorithm::to_upper(sCmd);
  if (sCmd == PRAGMA_TYPE_NODE) {
    m_pragma_type = PRAGMA_TYPE_NODE;
  } else if (PRAGMA_TYPE_PORT == sCmd) {
    m_pragma_type = PRAGMA_TYPE_PORT;
  } else if (PRAGMA_TYPE_CHANNEL == sCmd) {
    m_pragma_type = PRAGMA_TYPE_CHANNEL;
  } else if (PRAGMA_TYPE_BUFFER == sCmd) {
    m_pragma_type = PRAGMA_TYPE_BUFFER;
  }

  for (auto key_val : mapParams) {
    string attr = key_val.first;
    boost::algorithm::to_upper(attr);
    vector<string> vec_val = key_val.second.first;
    if (!vec_val.empty()) {
      set_attribute(attr, vec_val[0]);
    }
    for (size_t i = 1; i < vec_val.size(); ++i) {
      append_attribute(attr, vec_val[i]);
    }
  }

  //  1. check if the pragma type matches with m_pragma_type
  //  2. parse the subclause as "key=value" into m_attr
}
void CStreamBase::update_pragma(
    void *sg_pragma /* = nullptr*/) {  //  attribute -> pragma
  string str_pragma =
      std::string(ACCEL_PRAGMA) + " " + m_pragma_type + " " + print_attribute();

  void *scope = m_ast->TraceUpToScope(m_ref);
  if (nullptr == sg_pragma) {
    sg_pragma = get_pragma_by_ref();
  }

  if (nullptr == sg_pragma) {
    //  create a new pragma
    sg_pragma = m_ast->CreatePragma(str_pragma, scope);
    m_ast->InsertStmt(sg_pragma, m_ref);
  } else {
    string old_str_pragma = m_ast->GetPragmaString(sg_pragma);
    if (str_pragma == old_str_pragma) {
      return;
    }
    //  set pragma string as "#pragma str_pragma"
    void *new_pragma = m_ast->CreatePragma(str_pragma, scope);
    m_ast->InsertStmt(new_pragma, sg_pragma);
    m_ast->AddComment("Original pragma: " + old_str_pragma, new_pragma);
    m_ast->RemoveStmt(sg_pragma);
  }
}
#if 0
vector<int> CStreamNode::get_md_attribute() {
  string md_str = get_attribute(PRAGMA_PARALLEL_FACTOR_ATTRIBUTE);
  vector<string> vec_str;
  str_split(md_str, ",", vec_str);
  vector<int> res;
  for (auto str : vec_str)
    res.push_back(my_atoi(str));
  return res;
}
#endif
//  CStreamNode * CStreamIR::SpawnMemoryAccessFromAST(void * sg_init, void *
//  sg_scope, string *err_msg)
//  {
//
//   //  CMarsAST_IF * ast = m_ast;
//   CStreamNode * node = nullptr;
//   //  CStreamNode * node = FindNodeByAST(node_stmt);
//   //  if (node) return node;
//
//   //  node = CreateNode(node_call_stmt);
//   //  node->update_pragma();
//   //
//
//
//     map<void*, void*> map_spref2snref;
//     vector<void *> vec_params;
//
//     //  1. copy the access and create the basicblock (no basicblock for
//     scalar
//     //  port)
//     {
//       void * arg = sg_init;
//     //  int is_write = 0;
//       //  void *sg_port = MarsProgramAnalysis::CopyAccessInScope(
//       //    ast, arg, sg_scope, sg_scope, is_write, &map_spref2snref,
//       &vec_params);
//       //  ast->PrependChild(ast->GetFuncBody(sg_scope), sg_port);
//
//     CMerlinMessage msg;
//     map<void*, void*> map_spref2snref;
//     STEMP(start);
//     bool is_skip_node_create = true;
//     //  CStreamPort * port =
//       CreateStreamPortBySeparation(nullptr, arg, &map_spref2snref, &msg,
//       is_skip_node_create, sg_scope);
//
//
//     }
//
//   return node;
//  }

// TODO(youxiang): Youxiang: step 0,1,3,4,5   1.5d
// TODO(youxiang): Peng: step 2
//  Create the Stream Model Node, and transform AST
//  Process:
//  0. Skip if it is already generated, FindNodeByAST()
//  1. Create Node
//  2. Create Ports
//  3. Create Channels
//  4. Update reference in Node and Ports
//  5. Link the ports (inherit, Connect or Embed)
CStreamNode *CStreamIR::SpawnNodeFromAST(void *node_stmt, string *err_msg,
                                         bool is_prefetch_only,
                                         set<void *> *prefetch_vars) {
  CMarsAST_IF *ast = get_ast();
  string str_node = ast->_up(node_stmt, 30);
  cout << "SpawnNodeFromAST -- " << str_node << endl;

  static int index = 0;

  vector<void *> for_stmts;
  ast->GetNodesByType_int(nullptr, "preorder", V_SgForStatement, &for_stmts);
  for (auto for_stmt : for_stmts) {
    void *old_iter = ast->GetLoopIterator(for_stmt);
    if (old_iter == nullptr) {
      continue;
    }
    if (ast->is_located_in_scope(old_iter, for_stmt) != 0) {
      SgName varname("_" + ast->_up(old_iter) + "_l" + std::to_string(++index));
      (static_cast<SgInitializedName *>(old_iter)->set_name(varname));
    }
    ast->replace_variable_references_in_scope(old_iter, old_iter, for_stmt);
  }

  DEFINE_START_END;

  //  return nullptr; //  to be removed
  CStreamNode *node = nullptr;
  if (!is_prefetch_only) {
    //  Detailed Process:
    //  0. Skip if it is already generated, FindNodeByAST()
    node = FindNodeByAST(node_stmt);
    if (node != nullptr) {
      return node;
    }
  }

  //  ZP: 20170612
  //  0.1 check port separability
  STEMP(start);
  {
    vector<CMerlinMessage> vec_msg;
    if (CheckStreamPortSeparability(node_stmt, &vec_msg, prefetch_vars) == 0) {
      cout << "StreamPort is not generated: in scope "
           << ast->_up(node_stmt, 30) << endl;
      *err_msg = "\'" + ast->_up(node_stmt, 30) + "\' " +
                 getUserCodeFileLocation(ast, node_stmt, true) + "\n";
      for (auto msg : vec_msg) {
        cout << "-- " << msg.get_message() << endl;
        *err_msg += "    " + msg.get_message() + "\n";
      }
      cout << endl;
      dump_critical_message(STREAM_WARNING_1(*err_msg));
      return nullptr;
    }
  }
  TIMEP("SpawnNodeFromAST___check_port_separability", start, end);

  //  1. Create Node (2h)
  //  1.1 Function outline for the node (special consideration on the
  //  arguments?) 1.2 add "stream_node" pragma on call and decl
  STEMP(start);
  void *node_decl = (is_prefetch_only) ? node_stmt : nullptr;
  if (!is_prefetch_only) {
    void *node_call;
    string str_sn_name = "msm_node_";
    ast->function_outline(node_stmt, &node_call, &node_decl, str_sn_name, true,
                          true, false, false, true);
    ast->init_defuse_range_analysis();  //  ZP: 20170529
    void *node_call_stmt = ast->GetParent(node_call);
    assert(ast->IsStatement(node_call_stmt));
    node = CreateNode(node_call_stmt, m_mars_ir, m_mars_ir_v2);
    node->update_pragma();
  }
  m_ast->RegisterType("merlin_stream");
  void *node_pos = (is_prefetch_only) ? nullptr : node->get_pos();
  TIMEP("SpawnNodeFromAST___create_node", start, end);

  //  the map between the refereces of StreamPort and StreamNode
  map<CStreamPort *, map<void *, void *>> port2map_spref2snref;
  vector<pair<CStreamPort *, void *>> vec_ports;
  for (auto sg_arg : ast->GetFuncParams(node_decl)) {
    //  2. Create Ports (to be considered, 24h)
    //  2.1 establish the ref-to-fifo map
    //    2.1.1 parse for each reference, the surrounding loops and the scalar
    //    variables

    if (is_prefetch_only &&
        prefetch_vars->find(sg_arg) == prefetch_vars->end()) {
      continue;
    }

    if (is_prefetch_only && (m_ast->IsScalarType(sg_arg) != 0)) {
      cerr << "do not support scalar type in prefetch" << endl;
      continue;
    }

    CMerlinMessage msg;
    map<void *, void *> map_spref2snref;
    STEMP(start);
    vector<CStreamPort *> vec_port = CreateStreamPortBySeparation(
        node, sg_arg, &map_spref2snref, &msg, is_prefetch_only, node_stmt);
    TIMEP("SpawnNodeFromAST___CreateStreamPortBySeparation", start, end);
    if (vec_port.empty()) {
      cout << msg.get_message() << endl;
    }

    if (vec_port.empty() || map_spref2snref.empty()) {
      continue;
    }
    for (auto spref2snref : map_spref2snref) {
      cout << "ref in port " << m_ast->_p(spref2snref.first) << "->"
           << "ref in node " << m_ast->_p(spref2snref.second) << endl;
    }
    STEMP(start);
    m_ast->init_defuse_range_analysis();
    TIMEP("SpawnNodeFromAST___init_defuse_range_analysis", start, end);
    int name_suffix = 0;
    for (auto port : vec_port) {
      void *port_call = nullptr;
      void *port_decl = nullptr;
      port->get_func_call_decl(&port_call, &port_decl);
      void *port_pos = port->get_pos();
      bool is_write_port = port->is_write() != 0;
      void *sg_arg_type = m_ast->GetTypebyVar(sg_arg);

      //
      //  /////////////////////////////////////  /
      if (is_prefetch_only) {
        int ret = merlin_wrapper_add_sub_kernel(ast, node_decl, port_decl,
                                                !is_write_port);
        if (ret == 0) {
          MERLIN_ERROR("fail to apply sub kernel into wrapper function");
        }
        cout << "[MERLIN WRAPPER] ret=" << ret << endl;
      }
      //  /////////////////////////////////////  /

      STEMP(start);
      void *sg_arg_basic_type = nullptr;
      vector<size_t> vec_dims;
      m_ast->get_type_dimension(sg_arg_type, &sg_arg_basic_type, &vec_dims,
                                sg_arg);
      if (sg_arg_basic_type != nullptr) {
        sg_arg_basic_type = m_ast->GetBaseType(sg_arg_basic_type, false);
      }
      int data_size = m_ast->get_bitwidth_from_type(sg_arg_type, false) >> 3;
      //  FIXME: it should be total depth instead of original buffer size
      int buffer_size = 1;
      for (auto dim : vec_dims) {
        buffer_size *= dim;
      }
      TIMEP("SpawnNodeFromAST___create_ports_rest", start, end);

      //  3. Create Channelss (merlin_stream variables) (1h) //  TODO
      //  3.1 Generate the declaration at file beginning (assume each kernel is
      //  in one file)
      //  4. Update reference in Node and Ports (1h) //  TODO
      //  4.1 replace reference with read/write accesses
      STEMP(start);
      for (auto spref2snref : map_spref2snref) {
        cout << "ref in port " << m_ast->_p(spref2snref.first) << "->"
             << "ref in node " << m_ast->_p(spref2snref.second) << endl;
      }
      bool local_changed = false;
      do {
        local_changed = false;
        for (auto spref2snref : map_spref2snref) {
          void *ref_in_port = spref2snref.first;
          void *ref_in_node = spref2snref.second;
          void *ref_in_port_decl = m_ast->TraceUpToFuncDecl(ref_in_port);
          if (ref_in_port_decl != port_decl) {
            continue;
          }

          //  create temp variables in the scope of current references
          void *port_bb = m_ast->TraceUpToBasicBlock(ref_in_port);
          string sp_tmp_name = m_ast->GetVariableName(sg_arg) + "_sp_tmp_" +
                               my_itoa(name_suffix);
          m_ast->get_valid_local_name(ref_in_port_decl, &sp_tmp_name);
          void *tmp_in_sp = m_ast->CreateVariableDecl(
              sg_arg_basic_type, sp_tmp_name, nullptr, port_bb);
          m_ast->PrependChild(port_bb, tmp_in_sp);

          void *node_bb = m_ast->TraceUpToBasicBlock(ref_in_node);
          string sn_tmp_name = m_ast->GetVariableName(sg_arg) + "_sn_tmp_" +
                               my_itoa(name_suffix++);
          m_ast->get_valid_local_name(node_decl, &sn_tmp_name);
          void *tmp_in_sn = m_ast->CreateVariableDecl(
              sg_arg_basic_type, sn_tmp_name, nullptr, node_bb);
          m_ast->PrependChild(node_bb, tmp_in_sn);

          void *var_decl =
              m_ast->CreateVariableDecl("merlin_stream",
                                        "ch_" + m_ast->GetVariableName(sg_arg) +
                                            "_" + my_itoa(m_num_stream++),
                                        nullptr, m_ast->GetGlobal(node_decl));
          m_ast->PrependChild(m_ast->GetGlobal(node_decl), var_decl);

          //  create channel variable
          CStreamChannel *fifo = CreateChannels(var_decl);
          fifo->set_attribute(PRAGMA_NODE_ATTRIBUTE,
                              m_ast->GetFuncName(node_decl));
          fifo->set_attribute(PRAGMA_PORT_ATTRIBUTE,
                              m_ast->GetFuncName(port_decl));
          if (!is_prefetch_only) {
            int depth = port->get_sufficient_channel_size(ref_in_port);

            //  ZP: 20170630, a quick fix for 1370, to be move to feasiblity
            //  check before port_gen
            {
              if (depth == -1) {
                string ref_info =
                    ast->_up(ref_in_node) +
                    getUserCodeFileLocation(ast, ref_in_node, true);
                string node_info =
                    "'" + ast->_up(node_stmt) + "' " +
                    getUserCodeFileLocation(ast, node_stmt, true);
                string msg = "Undetermined channel size for access '" +
                             ref_info + "' in scope " + node_info;
                cout << msg << endl;
                dump_critical_message(CGPAR_ERROR_1(ref_info, node_info), 0);
                exit(0);
              }
            }

            fifo->set_attribute(PRAGMA_CHANNEL_DEPTH_ATTRIBUTE, my_itoa(depth));
          } else {
            fifo->set_attribute(PRAGMA_CHANNEL_DEPTH_ATTRIBUTE,
                                PRAGMA_CHANNEL_DEPTH_ATTRIBUTE_DEFAULT);
          }
          fifo->update_pragma();

          //  replace original reference with channel access
          void *new_ref_in_port = ref_in_port;
          bool write_in_port = m_ast->is_write_ref(ref_in_port) != 0;
          void *parent_exp_in_port =
              write_in_port ? m_ast->GetParent(ref_in_port) : ref_in_port;
          void *value_exp_in_port =
              write_in_port ? m_ast->GetExpRightOperand(parent_exp_in_port)
                            : ref_in_port;
          void *parent_exp_in_node =
              write_in_port ? m_ast->GetParent(ref_in_node) : ref_in_node;
          void *value_exp_in_node =
              write_in_port ? m_ast->GetExpRightOperand(parent_exp_in_node)
                            : ref_in_node;
          //  update reference in port
          if (!write_in_port) {
            new_ref_in_port = m_ast->CopyExp(value_exp_in_port);
            void *assign_stmt_in_port = m_ast->CreateStmt(
                V_SgAssignStatement, m_ast->CreateVariableRef(tmp_in_sp),
                new_ref_in_port);
            void *orig_read_stmt_in_port =
                m_ast->TraceUpToStatement(ref_in_port);
            m_ast->InsertStmt(assign_stmt_in_port, orig_read_stmt_in_port);
          }
          port->add_fifo(new_ref_in_port, fifo);
          vector<void *> vec_args;
          vec_args.push_back(m_ast->CreateExp(
              V_SgAddressOfOp, m_ast->CreateVariableRef(var_decl)));
          vec_args.push_back(m_ast->CreateExp(
              V_SgAddressOfOp, m_ast->CreateVariableRef(tmp_in_sp)));
          void *stream_call_in_port = m_ast->CreateFuncCall(
              write_in_port ? "merlin_stream_read" : "merlin_stream_write",
              m_ast->GetTypeByString("void"), vec_args,
              m_ast->TraceUpToScope(ref_in_port));
          void *channel_access_in_port =
              m_ast->CreateStmt(V_SgExprStatement, stream_call_in_port);
          void *orig_assign_stmt_in_port =
              m_ast->TraceUpToStatement(ref_in_port);
          m_ast->InsertStmt(channel_access_in_port, orig_assign_stmt_in_port);
          if (is_write_port) {
            m_ast->ReplaceExp(value_exp_in_port,
                              m_ast->CreateVariableRef(tmp_in_sp));
          } else {
            void *exp_parent = m_ast->GetParent(value_exp_in_port);
            if (m_ast->IsExprStatement(exp_parent) != 0) {
              m_ast->RemoveStmt(exp_parent);
            }
          }
          //  update reference in node
          vec_args.clear();
          vec_args.push_back(m_ast->CreateExp(
              V_SgAddressOfOp, m_ast->CreateVariableRef(var_decl)));
          vec_args.push_back(m_ast->CreateExp(
              V_SgAddressOfOp, m_ast->CreateVariableRef(tmp_in_sn)));
          void *stream_call_in_node = m_ast->CreateFuncCall(
              write_in_port ? "merlin_stream_write" : "merlin_stream_read",
              m_ast->GetTypeByString("void"), vec_args,
              m_ast->TraceUpToScope(ref_in_node));
          if (!write_in_port) {
            void *read_channel_in_node =
                m_ast->CreateStmt(V_SgExprStatement, stream_call_in_node);
            void *orig_assign_stmt_in_node =
                m_ast->TraceUpToStatement(ref_in_node);
            m_ast->InsertStmt(read_channel_in_node, orig_assign_stmt_in_node);
            m_ast->ReplaceExp(value_exp_in_node,
                              m_ast->CreateVariableRef(tmp_in_sn));
          } else {
            void *write_channel_in_node =
                m_ast->CreateStmt(V_SgExprStatement, stream_call_in_node);
            void *orig_assign_stmt_in_node =
                m_ast->TraceUpToStatement(ref_in_node);
            m_ast->ReplaceExp(ref_in_node, m_ast->CreateVariableRef(tmp_in_sn));
            m_ast->InsertAfterStmt(write_channel_in_node,
                                   orig_assign_stmt_in_node);
          }

          //  init and reset merlin_stream
          {
            vec_args.clear();
            vec_args.push_back(m_ast->CreateExp(
                V_SgAddressOfOp, m_ast->CreateVariableRef(var_decl)));
            vec_args.push_back(m_ast->CreateConst(buffer_size));
            vec_args.push_back(m_ast->CreateConst(data_size));
            void *stream_init_call = m_ast->CreateFuncCall(
                "merlin_stream_init", m_ast->GetTypeByString("void"), vec_args,
                m_ast->TraceUpToScope(port_pos));
            void *stream_init_call_stmt =
                m_ast->CreateStmt(V_SgExprStatement, stream_init_call);

            if (!is_prefetch_only) {
              void *insert_pos = is_write_port ? node_pos : port_pos;
              while (m_ast->IsPragmaDecl(m_ast->GetPreviousStmt(insert_pos)) !=
                     0) {
                insert_pos = m_ast->GetPreviousStmt(insert_pos);
              }
              m_ast->InsertStmt(stream_init_call_stmt, insert_pos);
            } else {
              m_ast->PrependChild(ast->GetFuncBody(node_stmt),
                                  stream_init_call_stmt);
            }
          }
          {
            vec_args.clear();
            vec_args.push_back(m_ast->CreateExp(
                V_SgAddressOfOp, m_ast->CreateVariableRef(var_decl)));
            void *stream_reset_call = m_ast->CreateFuncCall(
                "merlin_stream_reset", m_ast->GetTypeByString("void"), vec_args,
                m_ast->TraceUpToScope(port_pos));
            void *stream_reset_call_stmt =
                m_ast->CreateStmt(V_SgExprStatement, stream_reset_call);
            if (!is_prefetch_only) {
              void *insert_pos = is_write_port ? port_pos : node_pos;
              m_ast->InsertAfterStmt(stream_reset_call_stmt, insert_pos);
            } else {
              m_ast->AppendChild(ast->GetFuncBody(node_stmt),
                                 stream_reset_call_stmt);
            }
          }
          local_changed = true;
          map_spref2snref.erase(ref_in_port);
          break;
        }
      } while (local_changed);
    }
    TIMEP("SpawnNodeFromAST___create_channels", start, end);

    //  5. Link the ports (inherit, Connect or Embed) (16h) //  TODO
    //  5.1 inherit
    //  5.2 connect
    //  5.3 embed
  }

  //  6 include merlin_stream header files
  STEMP(start);
  if (contains_channel() != 0) {
    m_ast->AddHeader("#include \"merlin_stream.h\" \n", node_decl);
  }
  TIMEP("SpawnNodeFromAST___add_merlin_stream_header", start, end);
  if (!is_prefetch_only) {
    return node;
  }
  return static_cast<CStreamNode *>(node_stmt);
}

// TODO(youxiang): Youxiang, 2d
//  Parse the stream model information from transformed AST in a stream model
//  Process:
//  1. Get all stream_node pragmas -> StreamNodes
//  2. Get all stream_port pragmas -> StreamPorts
//  2.1 parse ref-to-fifo map
//  3. Get all stream_buffer pragmas -> StreamBuffers
void CStreamIR::ParseStreamModel() {
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType(nullptr, "preorder", "SgPragmaDeclaration",
                        &vec_pragmas);

  for (auto sg_pragma : vec_pragmas) {
    CStreamBase *base =
        new CStreamBase(m_ast, mOptions, m_mars_ir, m_mars_ir_v2);
    base->parse_pragma(sg_pragma);
    string pragma_type = base->get_pragma_type();
    void *next_stmt = m_ast->GetNextStmt(sg_pragma);
    if (PRAGMA_TYPE_NODE == pragma_type) {
      if (m_ast->IsExprStatement(next_stmt) != 0) {
        void *call_expr = m_ast->GetExprFromStmt(next_stmt);
        if (m_ast->IsFunctionCall(call_expr) != 0) {
          CStreamNode *node = FindNodeByAST(next_stmt);
          if (node == nullptr) {
            node = CreateNode(next_stmt, m_mars_ir, m_mars_ir_v2);
          }
          node->parse_pragma(sg_pragma);
        }
      }
    } else if (PRAGMA_TYPE_PORT == pragma_type) {
      string node_name = base->get_attribute(PRAGMA_NODE_ATTRIBUTE);
      string array_name = base->get_attribute(PRAGMA_VARIABLE_ATTRIBUTE);
      //  prefetch = (("" != base->get_attribute(PRAGMA_PREFETCH_ATTRIBUTE)) &&
      //             ("0" != base->get_attribute(PRAGMA_PREFETCH_ATTRIBUTE)));
      //  spawn = ("" != base->get_attribute(PRAGMA_SPAWN_ATTRIBUTE) &&
      //          "0" != base->get_attribute(PRAGMA_SPAWN_ATTRIBUTE));
      //  cout << "PREFETCH: " << prefetch << endl;
      //  cout << "SPAWN: " << spawn << endl;
      CStreamNode *node = nullptr;
      if (node == nullptr) {
        vector<void *> node_instance;
        m_ast->GetFuncCallsByName(node_name, nullptr, &node_instance);
        if (node_instance.size() == 1) {
          void *call_expr = node_instance[0];
          void *call_stmt = m_ast->GetParent(call_expr);
          assert(m_ast->IsStatement(call_stmt));
          node = FindNodeByAST(call_stmt);
          if (node == nullptr) {
            node = CreateNode(call_stmt, m_mars_ir, m_mars_ir_v2);
          }
        }
      }
      if (node == nullptr) {
        void *func_decl = m_ast->GetFuncDeclByName(node_name, 1);
        node = FindNodeByAST(func_decl);
        if (node == nullptr) {
          node = CreateNode(func_decl, m_mars_ir, m_mars_ir_v2);
        }
      }
      if (node != nullptr) {
        void *array = m_ast->find_variable_by_name(array_name, sg_pragma);
        CStreamPort *port =
            CreatePort(next_stmt, array, node, m_mars_ir, m_mars_ir_v2);
        port->parse_pragma(sg_pragma);
      }
    } else if (PRAGMA_TYPE_CHANNEL == pragma_type) {
      assert(m_ast->IsVariableDecl(next_stmt));
      CStreamChannel *fifo = CreateChannels(next_stmt);
      fifo->parse_pragma(sg_pragma);
    }
    /*else if (PRAGMA_TYPE_BUFFER == pragma_type) {
      CStreamBuffer *buffer = CreateBuffer(next_stmt, m_mars_ir, m_mars_ir_v2);
      buffer->parse_pragma(sg_pragma);
      string array_name = buffer->get_attribute(PRAGMA_VARIABLE_ATTRIBUTE);
      void *array = m_ast->find_variable_by_name(array_name, sg_pragma);
      string read_node_name = buffer->get_attribute(PRAGMA_READ_NODE_ATTRIBUTE);
      vector<void *> node_instance;
      m_ast->GetFuncCallsByName(read_node_name, nullptr, &node_instance);
      if (node_instance.size() == 1) {
        void *call_expr = node_instance[0];
        void *call_stmt = m_ast->GetParent(call_expr);
        assert(m_ast->IsStatement(call_stmt));
        CStreamNode *node = FindNodeByAST(call_stmt);
        if (!node)
          node = CreateNode(call_stmt);
        CStreamPort *read_port = CreatePort(next_stmt, array, node, m_mars_ir,
    m_mars_ir_v2); buffer->set_read_port(read_port);
      }
      string write_node_name =
          buffer->get_attribute(PRAGMA_WRITE_NODE_ATTRIBUTE);
      m_ast->GetFuncCallsByName(write_node_name, nullptr, &node_instance);
      if (node_instance.size() == 1) {
        void *call_expr = node_instance[0];
        void *call_stmt = m_ast->GetParent(call_expr);
        assert(m_ast->IsStatement(call_stmt));
        CStreamNode *node = FindNodeByAST(call_stmt);
        if (!node)
          node = CreateNode(call_stmt);
        CStreamPort *write_port = CreatePort(next_stmt, array, node, m_mars_ir,
    m_mars_ir_v2); buffer->set_read_port(write_port);
      }
    } */
    delete base;
  }
}

// TODO(youxiang): Youxiang, 1W
//  Generate the Concurrent Code
void CStreamIR::ConcurrentCodeGeneration(
    const vector<void *> &top_kernels,
    map<void *, vector<void *>> *kernel_autokernel) {
  set<void *> processed_globals;
  //  Create Intel channel variables
  for (auto fifo : m_fifos) {
    CodeGeneration_channel(fifo, &processed_globals);
  }

  for (auto port : m_ports) {
    CodeGeneration_port(port);
  }

  processed_globals.clear();
  //  Create a kernel for each node
  for (auto node : m_nodes) {
    CodeGeneration_node(node, top_kernels, &processed_globals,
                        kernel_autokernel);
  }

  // TODO(youxiang): handle stream buffer
}

void CStreamIR::CodeGeneration_channel(CStreamChannel *fifo,
                                       set<void *> *processed_global) {
  m_ast->init_defuse_range_analysis();
  void *var_decl = fifo->get_ref();
  void *global = m_ast->GetGlobal(var_decl);
  if (processed_global->count(global) <= 0) {
    void *channel_pragma = m_ast->CreatePragma(
        "OPENCL EXTENSION cl_altera_channels : enable", global);
    m_ast->PrependChild(global, channel_pragma);
    processed_global->insert(global);
  }
  void *var_init = m_ast->GetVariableInitializedName(var_decl);
  void *pragma = fifo->get_pragma_by_ref();
  if (pragma != nullptr) {
    m_ast->replace_with_dummy_decl(pragma);
  }
  vector<void *> vec_refs;
  m_ast->get_ref_in_scope(var_init, nullptr, &vec_refs);
  //  if (vec_refs.size() <= 0) {
  //  m_ast->replace_with_dummy_decl(var_decl);
  //  return;
  //  }

  int channel_depth = fifo->get_depth();
  void *global_scope = m_ast->GetGlobal(var_decl);
  string channel_name = m_ast->GetVariableName(var_init);
  //  add channel false definition
  string channel_typedef = "__merlin_channel_typedef";
  m_ast->RegisterType(channel_typedef);
  vector<size_t> channel_dims;
  void *base_type = nullptr;
  m_ast->get_type_dimension(m_ast->GetTypebyVar(var_init), &base_type,
                            &channel_dims, var_init);
  void *channel_type = m_ast->GetTypeByString(channel_typedef);
  if (!channel_dims.empty()) {
    std::reverse(channel_dims.begin(), channel_dims.end());
    channel_type = m_ast->CreateArrayType(channel_type, channel_dims);
  }

  void *channel_name_decl = m_ast->CreateVariableDecl(
      channel_type, channel_name, nullptr, global_scope);
  m_ast->InsertStmt(channel_name_decl, var_decl);
  string channel_macro_guard = "__defined_" + channel_name;
  m_ast->AddDirectives("\n#endif\n", channel_name_decl, 0);
  void *channel_base_type = nullptr;
  string wr_channel_func =
      "write_channel_altera_" + channel_name + "__merlinalterafuncend";
  string rd_channel_func =
      "read_channel_altera_" + channel_name + "__merlinalterafuncend";
  std::reverse(vec_refs.begin(), vec_refs.end());
  for (auto ref : vec_refs) {
    void *func_call = m_ast->TraceUpToFuncCall(ref);

    string func_name = m_ast->GetFuncNameByCall(func_call);
    if (func_name == "merlin_stream_init" ||
        func_name == "merlin_stream_reset") {
      void *call_stmt = m_ast->GetParent(func_call);
      m_ast->RemoveStmt(call_stmt);
      continue;
    }
    list<t_func_call_path> vec_paths;
    m_ast->get_call_paths(ref, nullptr, &vec_paths);
    bool is_in_port = false;
    CStreamPort *port = nullptr;
    for (auto path : vec_paths) {
      for (auto pair : path) {
        void *curr_call = pair.second;
        void *curr_call_stmt = m_ast->TraceUpToStatement(curr_call);
        if (curr_call_stmt != nullptr) {
          port = FindPortByAST(curr_call_stmt);
          if (port != nullptr) {
            is_in_port = true;
            break;
          }
          CStreamNode *node = FindNodeByAST(curr_call_stmt);
          if (node != nullptr) {
            is_in_port = false;
            break;
          }
        }
      }
    }
    vector<void *> para_loops;
    vector<void *> new_para_loop_iterators;
    void *inner_body = nullptr;
    void *outer_body = nullptr;
    void *curr_scope = m_ast->TraceUpToScope(ref);
    if (is_in_port) {
      CStreamNode *node = port->get_node();
      para_loops = node->get_para_loops();
      if (!para_loops.empty()) {
        void *new_body = m_ast->CreateBasicBlock();
        inner_body = new_body;
        for (size_t i = para_loops.size(); i > 0; --i) {
          void *loop = para_loops[i - 1];
          void *bb = m_ast->TraceUpToBasicBlock(ref);
          void *func_decl = m_ast->TraceUpToFuncDecl(ref);
          void *old_iter = m_ast->GetLoopIterator(loop);
          string new_iter_name = m_ast->GetVariableName(old_iter);
          m_ast->get_valid_local_name(func_decl, &new_iter_name);
          void *new_iterator = m_ast->CreateVariableDecl(
              m_ast->GetTypeByString("int"), new_iter_name, nullptr, bb);
          m_ast->PrependChild(bb, new_iterator);
          CMarsVariable mv(loop, m_ast);
          CMarsRangeExpr mr = mv.get_range();
          CMarsExpression me_lb = mr.get_lb(0);
          CMarsExpression me_ub = mr.get_ub(0);
          void *start_exp = me_lb.get_expr_from_coeff();
          void *end_exp = me_ub.get_expr_from_coeff();
          start_exp = m_ast->CreateCastExp(start_exp, "int");
          end_exp = m_ast->CreateCastExp(end_exp, "int");
          int64_t new_step = 1;
          void *new_for_loop = m_ast->CreateStmt(
              V_SgForStatement, m_ast->GetVariableInitializedName(new_iterator),
              start_exp, end_exp, new_body, &new_step);
          new_body = m_ast->CreateBasicBlock();
          outer_body = new_body;
          void *para_pragma = m_ast->CreatePragma("unroll", new_body);
          m_ast->PrependChild(new_body, new_for_loop);
          m_ast->PrependChild(new_body, para_pragma);
          new_para_loop_iterators.push_back(new_iterator);
        }
      }
      vector<void *> vec_var_ref;
      m_ast->GetNodesByType(curr_scope, "preorder", "SgVarRefExp",
                            &vec_var_ref);
      for (auto var_ref : vec_var_ref) {
        void *loop = m_ast->GetLoopFromIteratorByPos(
            m_ast->GetVariableInitializedName(var_ref), var_ref, 1);
        for (size_t j = 0; j < para_loops.size(); ++j) {
          if (loop == para_loops[j]) {
            m_ast->ReplaceExp(
                var_ref, m_ast->CreateVariableRef(new_para_loop_iterators[j]));
            break;
          }
        }
      }
    }
    void *channel_exp = nullptr;
    vector<void *> param_list;
    if (func_name == "merlin_stream_read" ||
        func_name == "merlin_stream_write") {
      channel_exp = m_ast->CreateVariableRef(channel_name_decl);
      CMarsAccess ac_r(ref, m_ast, nullptr);
      for (size_t i = 0; i < channel_dims.size(); ++i) {
        channel_exp = m_ast->CreateExp(V_SgPntrArrRefExp, channel_exp,
                                       ac_r.GetIndex(i).get_expr_from_coeff());
      }
    }

    if (func_name == "merlin_stream_read") {
      //  gen channel read
      param_list.push_back(channel_exp);
      void *val_arg = m_ast->GetFuncCallParam(func_call, 1);
      m_ast->remove_cast(&val_arg);
      assert(m_ast->IsAddressOfOp(val_arg));
      val_arg = m_ast->GetExpUniOperand(val_arg);
      channel_base_type = m_ast->GetTypeByExp(val_arg);
      void *func_rd_call = m_ast->CreateFuncCall(
          rd_channel_func, channel_base_type, param_list, curr_scope);
      void *assign_exp =
          m_ast->CreateExp(V_SgAssignOp, m_ast->CopyExp(val_arg), func_rd_call);

      m_ast->ReplaceExp(func_call, assign_exp);
      //  youxiang 20161215 work around aoc type deduction system
      m_ast->AddCastToExp(func_rd_call, channel_base_type);
    }

    if (func_name == "merlin_stream_write") {
      //  gen channel write
      param_list.push_back(channel_exp);
      void *val_arg = m_ast->GetFuncCallParam(func_call, 1);
      m_ast->remove_cast(&val_arg);
      assert(m_ast->IsAddressOfOp(val_arg));
      val_arg = m_ast->GetExpUniOperand(val_arg);
      channel_base_type = m_ast->GetTypeByExp(val_arg);
      param_list.push_back(m_ast->CopyExp(val_arg));
      void *func_wr_call = m_ast->CreateFuncCall(
          wr_channel_func, channel_base_type, param_list, curr_scope);
      m_ast->ReplaceExp(func_call, func_wr_call);
    }

    if ((inner_body != nullptr) && (outer_body != nullptr)) {
      int num_new_iters = new_para_loop_iterators.size();
      vector<void *> dead_stmts;
      for (size_t i = num_new_iters; i < m_ast->GetChildStmtNum(curr_scope);
           ++i) {
        void *curr_child = m_ast->GetChildStmt(curr_scope, i);
        m_ast->AppendChild(inner_body, m_ast->CopyStmt(curr_child));
        dead_stmts.push_back(curr_child);
      }
      m_ast->InsertStmt(outer_body, dead_stmts[0]);
      for (auto dead_stmt : dead_stmts) {
        m_ast->RemoveStmt(dead_stmt);
      }
    }
  }

  //  add include
  string include_def = "\n#include \"channel_def.h\"\n";
  m_ast->AddDirectives(include_def, channel_name_decl);
  //  add channel definition
  string def_type = m_ast->UnparseToString(channel_base_type);
  string definition_channel = "channel " + def_type + " " + channel_name;
  for (auto dim : channel_dims) {
    definition_channel += "[" + my_itoa(dim) + "]";
  }

  definition_channel +=
      " __attribute__((depth(" + my_itoa(channel_depth) + ")));\n";
  void *channel_pragma = m_ast->CreatePragma(
      definition_channel, m_ast->GetGlobal(channel_name_decl));
  m_ast->InsertStmt(channel_pragma, channel_name_decl);
  m_ast->AddDirectives("\n#ifndef " + channel_macro_guard + "\n" + "#define " +
                           channel_macro_guard + "\n",
                       channel_pragma, 1);
  //  add typedef
  //  string type_def = "typedef " + def_type + " __merlin_channel_typedef;\n";
  //  string type_def = "typedef int __merlin_channel_typedef;\n";
  auto type = SageBuilder::buildIntType();
  void *typedef_pragma = SageBuilder::buildTypedefDeclaration(
      "__merlin_channel_typedef", type,
      static_cast<SgScopeStatement *>(m_ast->GetGlobal(channel_name_decl)));
  m_ast->InsertStmt(typedef_pragma, channel_name_decl);

  {
    //  remove merlin_stream
    m_ast->RemoveStmt(var_decl);
  }
  //  generate channel function definition
  FILE *fp;
  fp = fopen("channel_def.h", "a+");
  string def1 = "#ifndef __MERLIN_ALTERA_CHANNEL_DEF_H_INCLUDED_" +
                wr_channel_func + "\n";
  fprintf(fp, "%s", def1.c_str());
  string def2 = "#define __MERLIN_ALTERA_CHANNEL_DEF_H_INCLUDED_" +
                wr_channel_func + "\n";
  fprintf(fp, "%s", def2.c_str());

  string rd_func_string = def_type + " " + rd_channel_func + "(int a);\n";
  string wr_func_string =
      def_type + " " + wr_channel_func + "(int a, " + def_type + " b);\n";
  fprintf(fp, "%s", rd_func_string.c_str());
  fprintf(fp, "%s", wr_func_string.c_str());

  string def3 = "#endif\n";
  fprintf(fp, "%s", def3.c_str());

  fclose(fp);
}

void CStreamNode::get_func_call_decl(void **func_call, void **func_decl) {
  if (m_ast->IsExprStatement(m_ref) != 0) {
    *func_call = m_ast->GetExprFromStmt(m_ref);
    *func_decl = m_ast->GetFuncDeclByCall(*func_call);
  } else if (m_ast->IsFunctionCall(m_ref) != 0) {
    *func_call = m_ref;
    *func_decl = m_ast->GetFuncDeclByCall(*func_call);
  } else if (m_ast->IsFunctionDeclaration(m_ref) != 0) {
    *func_decl = m_ref;
    vector<void *> vec_calls;
    m_ast->GetFuncCallsFromDecl(*func_decl, nullptr, &vec_calls);
    assert(!vec_calls.empty());
    *func_call = vec_calls[0];
  }
}

void CStreamPort::get_func_call_decl(void **func_call, void **func_decl) {
  if (m_ast->IsExprStatement(m_ref) != 0) {
    *func_call = m_ast->GetExprFromStmt(m_ref);
    *func_decl = m_ast->GetFuncDeclByCall(*func_call);
  } else if (m_ast->IsFunctionCall(m_ref) != 0) {
    *func_call = m_ref;
    *func_decl = m_ast->GetFuncDeclByCall(*func_call);
  } else if (m_ast->IsFunctionDeclaration(m_ref) != 0) {
    *func_decl = m_ref;
    vector<void *> vec_calls;
    m_ast->GetFuncCallsFromDecl(*func_decl, nullptr, &vec_calls);
    assert(!vec_calls.empty());
    *func_call = vec_calls[0];
  }
}

void CStreamIR::CodeGeneration_node(
    CStreamNode *node, const vector<void *> &top_kernels,
    set<void *> *processed_globals,
    map<void *, vector<void *>> *kernel_autokernel) {
  void *pragma = node->get_pragma_by_ref();
  if (pragma != nullptr) {
    m_ast->RemoveStmt(pragma);
  }

  void *func_call;
  void *func_decl;
  func_call = func_decl = nullptr;

  node->get_func_call_decl(&func_call, &func_decl);

  assert(func_call && func_decl);

  void *top_kernel = nullptr;
  for (auto kernel : top_kernels) {
    if ((kernel != nullptr) &&
        (m_ast->is_located_in_scope(func_call, kernel) != 0)) {
      assert(!top_kernel && "cannot support multiple kernels");
      top_kernel = kernel;
      //  FIXME
      if (kernel_autokernel->find(top_kernel) == (*kernel_autokernel).end()) {
        vector<void *> empty;
        empty.push_back(func_decl);
        kernel_autokernel->insert(
            pair<void *, vector<void *>>(top_kernel, empty));
      }
    }
    if (kernel == func_decl) {
      return;
    }
  }
  //  1. _kernel and header attributes on kernel functions
  void *return_type = m_ast->GetFuncReturnType(func_decl);
  string return_type_string = m_ast->UnparseToString(return_type);

  string opencl_return_type = "__kernel " + return_type_string;
  m_ast->RegisterType(opencl_return_type);
  void *new_return_type = m_ast->GetTypeByString(opencl_return_type);
  m_ast->SetFuncReturnType(func_decl, new_return_type);

  //  create while loop
  void *func_body = m_ast->GetFuncBody(func_decl);
  void *copy_func_body = m_ast->CopyStmt(func_body);
  void *while_loop =
      m_ast->CreateWhileLoop(m_ast->CreateConst(1), copy_func_body);
  void *new_body = m_ast->CreateBasicBlock();
  m_ast->PrependChild(new_body, while_loop);
  m_ast->SetFuncBody(func_decl, new_body);

  //  Apply parallelization to OpenCL attribute
  string para_dims = node->get_attribute(PRAGMA_PARALLEL_FACTOR_ATTRIBUTE);
  if (!para_dims.empty()) {
    string para_iters = node->get_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE);
    vector<string> vec_str;
    str_split(para_iters, ",", &vec_str);
    int i = 0;
    for (auto str : vec_str) {
      int index = my_atoi(str);
      void *arg = m_ast->GetFuncParam(func_decl, index);
      vector<void *> vec_args;
      vec_args.push_back(m_ast->CreateConst(i));
      void *global_scope = m_ast->GetGlobal(func_decl);
      if (processed_globals->count(global_scope) <= 0) {
        m_ast->AddHeader("#define get_compute_id(x) x\n", global_scope);
        processed_globals->insert(global_scope);
      }
      void *get_compute_id = m_ast->CreateFuncCall(
          "get_compute_id", m_ast->GetTypeByString("int"), vec_args, new_body);
      string id_name = "id_" + my_itoa(i);
      m_ast->get_valid_local_name(func_decl, &id_name);
      void *compute_id =
          m_ast->CreateVariableDecl("int", id_name, get_compute_id, new_body);
      m_ast->PrependChild(new_body, compute_id);
      m_ast->replace_variable_references_in_scope(
          arg, m_ast->GetVariableInitializedName(compute_id), new_body);
      ++i;
    }
  }

  vector<void *> para_loops = node->get_para_loops();

  if (m_ast->remove_dead_parameters(&func_decl)) {
    vector<void *> vec_calls;
    m_ast->GetFuncCallsFromDecl(func_decl, nullptr, &vec_calls);
    assert(!vec_calls.empty());
    func_call = vec_calls[0];
  }

  //  add dummy function declaration to make sure attribute is set before
  vector<void *> vec_param;
  void *dummy_func_decl = m_ast->CreateFuncDecl(
      m_ast->GetTypeByString("void"),
      "__merlin_dummy_" + m_ast->GetFuncName(func_decl), vec_param,
      m_ast->GetGlobal(func_decl), false, nullptr);
  m_ast->InsertStmt(dummy_func_decl, func_decl);

  string auto_func_name = m_ast->GetFuncName(func_decl);
  string top_kernel_name = m_ast->GetFuncName(top_kernel);
  if (m_ast->GetFuncParamNum(func_decl) == 0) {
    assert(top_kernel && "cannot find top kernel");
    ofstream ofs("auto.list", std::ofstream::app);
    ofs << top_kernel_name << "=" << auto_func_name << endl;
    ofs.close();
    //  add 'autorun' attribute
    m_ast->set_kernel_attribute(func_decl, "autorun", "");
  }
  if (!para_dims.empty()) {
    m_ast->set_kernel_attribute(func_decl, "num_compute_units", para_dims);
    m_ast->set_kernel_attribute(func_decl, "max_global_work_dim", "0");
  }
  //  2. replace function call with mem_fence(CLK_CHANNEL_MEM_FENCH)
  void *curr_scope = m_ast->TraceUpToScope(func_call);
  void *mem_fence_pragma = m_ast->CreatePragma(
      "ACCEL mem_fence autokernel=" + auto_func_name +
          " parent=" + top_kernel_name + " depth=" + para_dims,
      curr_scope);

  if (!para_loops.empty()) {
    void *top_loop = para_loops[0];
    m_ast->InsertStmt(mem_fence_pragma, top_loop);
    m_ast->RemoveStmt(top_loop);
  } else {
    void *func_call_stmt = m_ast->GetParent(func_call);
    m_ast->ReplaceStmt(func_call_stmt, mem_fence_pragma);
  }
}

vector<void *> CStreamNode::get_para_loops() {
  vector<void *> para_loops;
  string para_iter = get_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE);
  if (!para_iter.empty()) {
    vector<string> vec_indices;
    str_split(para_iter, ",", &vec_indices);
    void *node_call = nullptr;
    void *node_decl = nullptr;
    get_func_call_decl(&node_call, &node_decl);
    for (size_t i = vec_indices.size(); i > 0; --i) {
      string index_str = vec_indices[i - 1];
      int index = my_atoi(index_str);
      void *arg = m_ast->GetFuncCallParam(node_call, index);
      if (arg != nullptr) {
        void *loop = m_ast->GetLoopFromIteratorRef(arg);
        if (loop != nullptr) {
          para_loops.push_back(loop);
        }
      }
    }
  }
  return para_loops;
}

void CStreamIR::CodeGeneration_port(CStreamPort *port) {
  CStreamNode *node = port->get_node();

  auto kernels = m_mars_ir_v2->get_top_kernels();
  void *node_func_call = nullptr;
  void *node_func_decl = nullptr;
  node->get_func_call_decl(&node_func_call, &node_func_decl);
  for (auto kernel : kernels) {
    if (node_func_decl == kernel) {
      void *func_call = nullptr;
      void *func_decl = nullptr;
      port->get_func_call_decl(&func_call, &func_decl);
      void *func_call_stmt = m_ast->TraceUpToStatement(func_call);
      m_ast->RemoveStmt(func_call_stmt);
      return;
    }
  }

  void *pragma = port->get_pragma_by_ref();
  if (pragma != nullptr) {
    m_ast->RemoveStmt(pragma);
  }
  bool is_write = port->is_write() != 0;
  vector<void *> para_loops = node->get_para_loops();
  set<void *> para_loops_set(para_loops.begin(), para_loops.end());

  if (!para_loops.empty()) {
    void *para_loop = para_loops[0];
    void *prev_pragma = m_ast->GetPragmaAbove(para_loop);
    if (prev_pragma != nullptr) {
      m_ast->RemoveStmt(prev_pragma);
    }
    void *func_call = nullptr;
    void *func_decl = nullptr;
    port->get_func_call_decl(&func_call, &func_decl);
    void *func_call_stmt = m_ast->TraceUpToStatement(func_call);
    for (int i = 0; i < m_ast->GetFuncCallParamNum(func_call); ++i) {
      void *arg = m_ast->GetFuncCallParam(func_call, i);
      if (m_ast->IsVarRefExp(arg) != 0) {
        void *loop = m_ast->GetLoopFromIteratorRef(arg);
        if ((loop != nullptr) && para_loops_set.count(loop) > 0) {
          m_ast->ReplaceExp(arg, m_ast->CreateConst(0));
        }
      }
    }
    //  if (!spawn) {
    void *copy_func_call_stmt = m_ast->CopyStmt(func_call_stmt);
    if (is_write) {
      m_ast->InsertAfterStmt(copy_func_call_stmt, para_loop);
    } else {
      m_ast->InsertStmt(copy_func_call_stmt, para_loop);
    }
    //  } else {
    //   void *return_type = m_ast->GetFuncReturnType(func_decl);
    //   string return_type_string = m_ast->UnparseToString(return_type);

    //   string opencl_return_type = "__kernel " + return_type_string;
    //   m_ast->RegisterType(opencl_return_type);
    //   void *new_return_type = m_ast->GetTypeByString(opencl_return_type);
    //   m_ast->SetFuncReturnType(func_decl, new_return_type);
    //  }
    m_ast->RemoveStmt(func_call_stmt);
  }
}

CStreamPort *CStreamIR::get_matched_port(void *func) {
  for (auto port : m_ports) {
    void *call_expr = m_ast->GetExprFromStmt(port->get_ref());
    void *callee = m_ast->GetFuncDeclByCall(call_expr);
    if (m_ast->isSameFunction(callee, func)) {
      return port;
    }
  }
  return nullptr;
}

#if 0
CStreamNode *CStreamIR::get_matched_node(void *func) {
  for (auto port : m_nodes) {
    void *call_expr = m_ast->GetExprFromStmt(port->get_ref());
    void *callee = m_ast->GetFuncDeclByCall(call_expr);
    if (m_ast->isSameFunction(callee, func))
      return port;
  }
  return nullptr;
}
#endif

bool CStreamIR::is_port_kernel(void *func) {
  return get_matched_port(func) != nullptr;
}

#if 0
bool CStreamIR::is_node_kernel(void *func) {
  if (get_matched_node(func))
    return true;
  return false;
}
#endif

/***************************************************************************/
// check merlin_stream usage
// merlin_steam must be used in specific funcion call
/***************************************************************************/
void CStreamIR::CheckStreamLecality(void *decl) {
  bool pass_total = true;
  void *init = m_ast->GetVariableInitializedName(decl);
  vector<void *> vec_refs;
  m_ast->get_ref_in_scope(init, nullptr, &vec_refs);
  for (auto one_ref : vec_refs) {
    bool pass = true;
    void *one_call = m_ast->TraceUpToFuncCall(one_ref);
    if (one_call == nullptr) {
      pass = false;
    } else {
      string call_name = m_ast->GetFuncNameByCall(one_call);
      if (call_name != "merlin_stream_init" &&
          call_name != "merlin_stream_init_debug" &&
          call_name != "merlin_stream_reset" &&
          call_name != "merlin_stream_write" &&
          call_name != "merlin_stream_read") {
        pass = false;
      }
    }
    if (!pass) {
      pass_total = false;
      string err_msg = "\'" + m_ast->_up(one_ref, 30) + "\' " +
                       getUserCodeFileLocation(m_ast, one_ref, true) + "\n";
      dump_critical_message(STREAM_ERROR_5(err_msg));
    }
  }
  if (!pass_total) {
    throw std::exception();
  }
}

/***************************************************************************/
// find original stream channel usage and skip, use original method
// use STREAM_PORT and STREAM_CHANNELpragma to detect
/***************************************************************************/
bool CStreamIR::CheckIfHaveOldStream(void *decl) {
  // detect STREAM_PORT
  string decl_name = m_ast->GetVariableName(decl);
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType(nullptr, "preorder", "SgPragmaDeclaration",
                        &vec_pragmas);
  for (auto sg_pragma : vec_pragmas) {
    string str_pragma = m_ast->GetPragmaString(sg_pragma);
    map<string, pair<vector<string>, vector<string>>> mapParams;
    string sFilter;
    string sCmd;
    tldm_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams);

    if (is_cmost_pragma(sFilter) == 0) {
      continue;
    }
    boost::algorithm::to_upper(sCmd);
    if (PRAGMA_TYPE_PORT != sCmd) {
      continue;
    }

    string port;
    for (auto key_val : mapParams) {
      string attr = key_val.first;
      boost::algorithm::to_upper(attr);
      vector<string> vec_val = key_val.second.first;
      if (attr == "VARIABLE") {
        port = vec_val[0];
        continue;
      }
    }

    if (decl_name.find("ch_" + port) != string::npos) {
      cout << "decl name = " << decl_name << endl;
      cout << "Port pragma = " << str_pragma << endl;
      cout << "YES" << endl;
      return true;
    }
  }

  // detect STREAM_CHANNEL
  void *prev_stmt = m_ast->GetPreviousStmt(decl);
  cout << "previous stmt = " << m_ast->UnparseToString(prev_stmt) << endl;
  if (m_ast->IsPragmaDecl(prev_stmt) != 0) {
    string str_pragma = m_ast->GetPragmaString(prev_stmt);
    map<string, pair<vector<string>, vector<string>>> mapParams;
    string sFilter;
    string sCmd;
    tldm_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams);
    cout << "filter = " << sFilter << ", cmd = " << sCmd
         << ", type = " << PRAGMA_TYPE_CHANNEL << endl;
    if (is_cmost_pragma(sFilter) == 0) {
      return false;
    }
    boost::algorithm::to_upper(sCmd);
    if (PRAGMA_TYPE_CHANNEL == sCmd) {
      return true;
    }
  }
  return false;
}

/***************************************************************************/
// get all stream declaration
/***************************************************************************/
void CStreamIR::GetAllStreamDecl() {
  printf("GetAllStreamDecl\n");
  vector<void *> decls;
  m_ast->GetNodesByType(nullptr, "preorder", "SgVariableDeclaration", &decls);
  for (auto decl : decls) {
    if (m_ast->IsFromInputFile(decl) != 0) {
      void *name = m_ast->GetVariableInitializedName(decl);
      void *type = m_ast->GetTypebyVar(name);
      void *base_type = m_ast->GetBaseType(type, true);
      void *type_decl = m_ast->GetTypeDeclByType(base_type);
      if (m_ast->IsStructureType(base_type) != 0) {
        string type_str = m_ast->GetStringByType(type);
        // if(type_str == "merlin_stream") {
        if (type_str.find("merlin_stream") != string::npos &&
            type_str.find("__merlin_stream") == string::npos) {
          if (CheckIfHaveOldStream(decl)) {
            continue;
          }
          CheckStreamLecality(decl);
          vec_merlin_stream.push_back(decl);
          printf("type_str = %s\n", type_str.c_str());
          printf("decl = %s, type = %s, base_type = %s, type_decl = %s\n",
                 m_ast->UnparseToString(decl).c_str(),
                 m_ast->UnparseToString(type).c_str(),
                 m_ast->UnparseToString(type_decl).c_str(),
                 m_ast->UnparseToString(base_type).c_str());
        }
      }
    }
  }
}

/***************************************************************************/
// get channel attributes
// 1. depth
// 2. channel data type
/***************************************************************************/
void CStreamIR::GetChannelAttributesFromInit(void *init, void **depth_ref,
                                             string *opencl_channel_type) {
  vector<void *> vec_refs;
  m_ast->get_ref_in_scope(init, nullptr, &vec_refs);
  for (auto one_ref : vec_refs) {
    void *one_call = m_ast->TraceUpToFuncCall(one_ref);
    if (one_call != nullptr) {
      string call_name = m_ast->GetFuncNameByCall(one_call);
      if (call_name == "merlin_stream_init" ||
          call_name == "merlin_stream_init_debug") {
        int depth_index = 1;
        int data_type_index = 2;

        // 1. get depth information from merlin_stream_init
        void *parameter_1 = m_ast->GetFuncCallParam(one_call, depth_index);
        *depth_ref = m_ast->CopyExp(parameter_1);

        // 2. get channel type information from merlin_stream_init
        //    should support struct type as string
        void *parameter_2 = m_ast->GetFuncCallParam(one_call, data_type_index);
        *opencl_channel_type = m_ast->UnparseToString(parameter_2);
        int length = opencl_channel_type->length();
        // if have \" inside, get the inner string, it not use the full string
        if (opencl_channel_type->substr(length - 1, length - 1) == "\"" ||
            opencl_channel_type->substr(length - 1, length - 1) == "\"") {
          *opencl_channel_type = opencl_channel_type->substr(1, length - 2);
        }
      }
    }
    if (*depth_ref == nullptr) {
      *depth_ref = m_ast->CreateConst(1);
    }
  }
  cout << "pragma channel depth_ref = " << m_ast->UnparseToString(*depth_ref)
       << endl;
  cout << "pragma channel type = " << *opencl_channel_type << endl;
}

/***************************************************************************/
// insert channel declaration
// 1. insert type def for channel for syntax check
// 2. insert type def declaration
// 3. get channel attributs
// 4. generate OpenCL channel stringg  pragma, will replace it with real OpenCL
// with script
/***************************************************************************/
void CStreamIR::InsertChannelDecl() {
  cout << "\nInsertChannelDecl\n" << endl;
  for (auto one_stream : vec_merlin_stream) {
    void *decl = one_stream;
    void *init = m_ast->GetVariableInitializedName(decl);

    // 1. Insert type def for channel for syntax check(including array)
    string channel_typedef = "__merlin_channel_typedef";
    m_ast->RegisterType(channel_typedef);
    string channel_name = m_ast->GetVariableName(init);
    void *channel_init_type = m_ast->GetTypebyVar(init);
    void *global_scope = m_ast->GetGlobal(decl);
    vector<size_t> channel_dims;
    void *base_type = nullptr;
    m_ast->get_type_dimension(channel_init_type, &base_type, &channel_dims,
                              init);
    void *channel_type = m_ast->GetTypeByString(channel_typedef);
    string channel_array_size_str;
    if (!channel_dims.empty()) {
      std::reverse(channel_dims.begin(), channel_dims.end());
      channel_type = m_ast->CreateArrayType(channel_type, channel_dims);
      for (auto size : channel_dims) {
        channel_array_size_str += "[" + to_string(size) + "]";
      }
    }
    void *channel_name_decl = m_ast->CreateVariableDecl(
        channel_type, channel_name, nullptr, global_scope);
    // m_ast->InsertStmt(channel_name_decl, decl);
    m_ast->PrependChild(global_scope, channel_name_decl);
    string include_def = "\n#include \"channel_def.h\"\n";
    m_ast->AddDirectives(include_def, channel_name_decl);

    // 2. insert type def declaration
    auto type = SageBuilder::buildIntType();
    void *typedef_pragma = SageBuilder::buildTypedefDeclaration(
        "__merlin_channel_typedef", type,
        static_cast<SgScopeStatement *>(m_ast->GetGlobal(channel_name_decl)));
    m_ast->InsertStmt(typedef_pragma, channel_name_decl);

    // 3. get channel attributs
    void *depth_ref = nullptr;
    string opencl_channel_type;
    GetChannelAttributesFromInit(init, &depth_ref, &opencl_channel_type);

    // 4. generate OpenCL channel stringg  pragma, will replace it with real
    // OpenCL with script
    string pragma_channel = "channel " + opencl_channel_type + " " +
                            channel_name + "" + channel_array_size_str +
                            " __attribute__((depth(" +
                            m_ast->UnparseToString(depth_ref) + ")));";
    cout << "pragma channel = " << pragma_channel << endl;
    // m_ast->InsertStmt(m_ast->CreatePragma(pragma_channel, global_scope),
    // decl);
    m_ast->PrependChild(global_scope,
                        m_ast->CreatePragma(pragma_channel, global_scope));
    map_merlin_stream_depth.insert(pair<void *, void *>(one_stream, depth_ref));
  }
}

/***************************************************************************/
// insert channel definition in C level
/***************************************************************************/
void CStreamIR::InsertChannelFuncDefinition(void *data_type,
                                            string wr_channel_func,
                                            string rd_channel_func) {
  FILE *fp;
  fp = fopen("channel_def.h", "a+");
  string def1 = "#ifndef __MERLIN_ALTERA_CHANNEL_DEF_H_INCLUDED_" +
                wr_channel_func + "\n";
  fprintf(fp, "%s", def1.c_str());
  string def2 = "#define __MERLIN_ALTERA_CHANNEL_DEF_H_INCLUDED_" +
                wr_channel_func + "\n";
  fprintf(fp, "%s", def2.c_str());
  string rd_func_string =
      m_ast->UnparseToString(data_type) + " " + rd_channel_func + "(int a);\n";
  string wr_func_string_def = m_ast->UnparseToString(data_type) + " " +
                              wr_channel_func + "(int a, " +
                              m_ast->UnparseToString(data_type) + " b);\n";
  fprintf(fp, "%s", rd_func_string.c_str());
  fprintf(fp, "%s", wr_func_string_def.c_str());
  string def3 = "#endif\n";
  fprintf(fp, "%s", def3.c_str());
  fclose(fp);
}

/***************************************************************************/
// insert channel extension
/***************************************************************************/
void CStreamIR::InsertChannelExtension(void *decl,
                                       set<void *> *processed_global) {
  void *global = m_ast->GetGlobal(decl);
  if (processed_global->count(global) <= 0) {
    void *channel_pragma = m_ast->CreatePragma(
        "OPENCL EXTENSION cl_intel_channels : enable", global);
    m_ast->PrependChild(global, channel_pragma);
    processed_global->insert(global);
  }
}

/***************************************************************************/
// insert channel read and write API
// 1. insert channel extension
// 2. replace merlin_stream read and write with opencl read and write
// 3. generate channel function definition
/***************************************************************************/
void CStreamIR::InsertChannelReadWrite() {
  cout << "\nInsertChannelReadWrite\n" << endl;
  set<void *> processed_global;
  for (auto one_stream : vec_merlin_stream) {
    void *decl = one_stream;
    // 1. insert extension
    InsertChannelExtension(decl, &processed_global);

    // 2. replace merlin_stream read and write with opencl read and write
    void *init = m_ast->GetVariableInitializedName(decl);
    void *data_type = nullptr;
    string wr_channel_func;
    string rd_channel_func;
    vector<void *> vec_refs;
    m_ast->get_ref_in_scope(init, nullptr, &vec_refs);
    std::reverse(vec_refs.begin(), vec_refs.end());
    for (auto one_ref : vec_refs) {
      void *one_call = m_ast->TraceUpToFuncCall(one_ref);
      if (one_call != nullptr) {
        string call_name = m_ast->GetFuncNameByCall(one_call);
        vector<void *> param_list_write;
        vector<void *> param_list_read;
        void *write_ref = nullptr;
        void *read_ref = nullptr;
        void *channel_node = nullptr;
        if (call_name == "merlin_stream_write" ||
            call_name == "merlin_stream_read") {
          int channel_index = 0;
          int data_index = 1;

          // 2.1 get channel expression
          void *parameter_channel =
              m_ast->GetFuncCallParam(one_call, channel_index);
          m_ast->RemoveCast(parameter_channel);
          void *basic_ref_channel = m_ast->GetExpUniOperand(parameter_channel);
          param_list_write.push_back(m_ast->CopyExp(basic_ref_channel));
          param_list_read.push_back(m_ast->CopyExp(basic_ref_channel));
          channel_node = basic_ref_channel;

          // 2.2 get data expression
          void *parameter_data = m_ast->GetFuncCallParam(one_call, data_index);
          m_ast->RemoveCast(parameter_data);
          void *basic_ref_data = m_ast->GetExpUniOperand(parameter_data);
          data_type = m_ast->GetTypeByExp(basic_ref_data);
          data_type = m_ast->GetBaseType(data_type, false);
          write_ref = m_ast->CopyExp(basic_ref_data);
          write_ref = m_ast->GetExpUniOperand(write_ref);
          param_list_write.push_back(write_ref);
          read_ref = m_ast->CopyExp(basic_ref_data);
          // FIXME why need do this again
          read_ref = m_ast->GetExpUniOperand(read_ref);
        }
        if (call_name == "merlin_stream_write") {
          // 2.3 create channel call by C exp
          string channel_name = m_ast->UnparseToString(init);
          wr_channel_func =
              "write_channel_altera_" + channel_name + "__merlinalterafuncend";
          void *curr_scope = m_ast->TraceUpToScope(channel_node);
          void *func_wr_call = m_ast->CreateFuncCall(
              wr_channel_func, m_ast->GetTypeByString("void"), param_list_write,
              curr_scope);
          void *original_stmt = m_ast->TraceUpToStatement(one_call);
          void *new_stmt = m_ast->CreateStmt(V_SgExprStatement, func_wr_call);
          m_ast->ReplaceStmt(original_stmt, new_stmt);
          cout << "write channel for " << channel_name << endl;
        } else if (call_name == "merlin_stream_read") {
          // 2.4 create channel call by C exp
          string channel_name = m_ast->UnparseToString(init);
          rd_channel_func =
              "read_channel_altera_" + channel_name + "__merlinalterafuncend";
          void *curr_scope = m_ast->TraceUpToScope(channel_node);
          void *func_rd_call = m_ast->CreateFuncCall(
              rd_channel_func, m_ast->GetTypeByString("void"), param_list_read,
              curr_scope);
          void *equal_exp =
              m_ast->CreateExp(V_SgAssignOp, read_ref, func_rd_call);
          void *original_stmt = m_ast->TraceUpToStatement(one_call);
          void *new_stmt = m_ast->CreateStmt(V_SgExprStatement, equal_exp);
          m_ast->ReplaceStmt(original_stmt, new_stmt);
          cout << "read channel for " << channel_name << endl;
        }
      }
    }
    m_ast->RemoveStmt(one_stream);

    // 3. generate channel function definition
    InsertChannelFuncDefinition(data_type, wr_channel_func, rd_channel_func);
  }
}

/***************************************************************************/
// clean merlin stream related things, including
//      merlin_stream_init
//      melrin_stream_reset
//      loops outside init and reset
/***************************************************************************/
void CStreamIR::CleanMerlinStream() {
  cout << "\nCleanMerlinStream\n" << endl;
  set<void *> delete_loops;  // set of loop which will be deleted
  vector<void *> calls;
  m_ast->GetNodesByType(nullptr, "preorder", "SgFunctionCallExp", &calls);
  for (auto one_call : calls) {
    string call_name = m_ast->GetFuncNameByCall(one_call);
    if (call_name == "merlin_stream_init" ||
        call_name == "merlin_stream_reset") {
      // if the loop only contain merlin_stream inside, will delete the loop
      void *parent_loop = m_ast->TraceUpToLoopScope(one_call);
      if (parent_loop != nullptr) {
        vector<void *> loop_sub_calls;
        m_ast->GetNodesByType(parent_loop, "preorder", "SgFunctionCallExp",
                              &loop_sub_calls);
        int number_call = loop_sub_calls.size();
        int stmt_call = m_ast->GetChildStmtNum(m_ast->GetLoopBody(parent_loop));
        bool if_all_stream = true;
        for (auto one_sub_call : loop_sub_calls) {
          string one_sub_func_name = m_ast->GetFuncNameByCall(one_sub_call);
          if (one_sub_func_name.find("merlin_stream") == string::npos) {
            if_all_stream = false;
          }
        }
        if (if_all_stream && number_call == stmt_call) {
          delete_loops.insert(parent_loop);
        }
      }

      // delete merlin_stream call
      void *call_stmt = m_ast->TraceUpToStatement(one_call);
      m_ast->RemoveStmt(call_stmt);
    }
  }

  // Delete loops which only have merlin_stream call inside
  for (auto one_loop : delete_loops) {
    void *prev_stmt = m_ast->GetPreviousStmt(one_loop);
    while (prev_stmt != nullptr) {
      if (m_ast->IsPragmaDecl(prev_stmt) != 0) {
        string pragma_str = m_ast->UnparseToString(prev_stmt);
        if (pragma_str.find("ACCEL") == string::npos) {
          m_ast->RemoveStmt(prev_stmt);
        }
      } else {
        break;
      }
      prev_stmt = m_ast->GetPreviousStmt(prev_stmt);
    }
    m_ast->RemoveStmt(one_loop);
  }
}

/**************************************************************************/
// 1. get all stream declaration
// 2. insert channel declaration
// 3. insert channel write and write
// 4. delete merlin_stream init/reset/write/read
/***************************************************************************/
void CStreamIR::ChannelReplacement() {
  GetAllStreamDecl();
  InsertChannelDecl();
  InsertChannelReadWrite();
  CleanMerlinStream();
}

/***************************************************************************/
// check spawn pragma legality
// 1. spawn must be before function call
// 2. spawn kernel do not have return
// 3. spawn kernel interface must from original kernel interface
/***************************************************************************/
void CStreamIR::CheckSpawnLegality(CMarsIr *mars_ir, void *pragma) {
  cout << "Check spawn pragma legality for : " << m_ast->UnparseToString(pragma)
       << endl;
  bool failed = false;
  // 1. spawn must be before function call
  void *call = nullptr;
  void *next_stmt = m_ast->GetNextStmt(pragma);
  vector<void *> vec_func_call;
  m_ast->GetNodesByType(next_stmt, "preorder", "SgFunctionCallExp",
                        &vec_func_call);
  if (vec_func_call.empty() || vec_func_call.size() > 1) {
    string err_msg = "\'" + m_ast->_up(pragma, 30) + "\' " +
                     getUserCodeFileLocation(m_ast, pragma, true) + "\n";
    dump_critical_message(STREAM_ERROR_3(err_msg));
    failed = true;
  } else {
    call = vec_func_call[0];
    void *trace_stmt = m_ast->TraceUpToStatement(call);
    if (trace_stmt != next_stmt) {
      string err_msg = "\'" + m_ast->_up(pragma, 30) + "\' " +
                       getUserCodeFileLocation(m_ast, pragma, true) + "\n";
      dump_critical_message(STREAM_ERROR_3(err_msg));
      failed = true;
    }
  }
  if (failed) {
    throw std::exception();
  }

  if (call != nullptr) {
    // 2. spawn kernel do not have return
    void *decl = m_ast->GetFuncDeclByCall(call);
    vector<void *> vec_stmt = m_ast->GetChildStmts(m_ast->GetFuncBody(decl));
    for (auto one_stmt : vec_stmt) {
      if (m_ast->IsReturnStatement(one_stmt) != 0) {
        string err_msg = "\'" + m_ast->_up(decl, 30) + "\' " +
                         getUserCodeFileLocation(m_ast, decl, true) + "\n";
        dump_critical_message(STREAM_ERROR_4(err_msg));
        failed = true;
      }
    }
    if (failed) {
      throw std::exception();
    }

    // 3. spawn kernel interface must from original kernel interface
    CAnalPragma curr_pragma(m_ast);
    bool errorOut = false;
    bool is_with_parallel_factor = false;
    string spawn_factor_str;
    if ((m_ast->IsPragmaDecl(pragma) != 0) &&
        curr_pragma.PragmasFrontendProcessing(pragma, &errorOut, false) &&
        curr_pragma.is_spawn_kernel()) {
      spawn_factor_str = curr_pragma.get_attribute(CMOST_factor);
      if (!spawn_factor_str.empty()) {
        is_with_parallel_factor = true;
      }
    }
    // parallel factor must used in side loop, and loop tripcount should
    // equal to parallel factor number
    if (is_with_parallel_factor) {
      void *parent = m_ast->GetParent(pragma);
      int number = m_ast->GetChildStmtNum(parent);
      parent = m_ast->GetParent(parent);
      cout << "DEBUG parent = " << m_ast->UnparseToString(parent) << endl;
      if ((m_ast->IsLoopStatement(parent) != 0) && number == 2) {
        int64_t trip_count = 0;
        m_ast->get_loop_trip_count(parent, &trip_count);
        cout << "trip count = " << trip_count << endl;
        int factor = atoi(spawn_factor_str.c_str());
        if (factor != trip_count) {
          string err_msg = "\'" + m_ast->_up(pragma, 30) + "\' " +
                           getUserCodeFileLocation(m_ast, decl, true) + "\n";
          dump_critical_message(STREAM_ERROR_8(err_msg));
          failed = true;
        }
      } else {
        string err_msg = "\'" + m_ast->_up(pragma, 30) + "\' " +
                         getUserCodeFileLocation(m_ast, decl, true) + "\n";
        dump_critical_message(STREAM_ERROR_7(err_msg));
        failed = true;
      }
    }
    vector<void *> param_list = m_ast->GetFuncParams(decl);
    for (auto one_param : param_list) {
      if (is_with_parallel_factor) {
        continue;
      }
      cout << "check param : " << m_ast->UnparseToString(one_param) << endl;
      if (mars_ir->every_trace_is_bus(m_ast, decl, one_param)) {
        cout << "Is from kernel interface" << endl;
      } else {
        cout << "Not from kernel interface" << endl;
        string err_msg = "\'" + m_ast->_up(one_param, 30) + "\' " +
                         getUserCodeFileLocation(m_ast, decl, true) + "\n";
        dump_critical_message(STREAM_ERROR_6(err_msg));
        failed = true;
      }
    }
    if (failed) {
      throw std::exception();
    }
  }
  cout << " passed" << endl;
}

/***************************************************************************/
// delete useless interface to save cache resource
/***************************************************************************/
void CStreamIR::UpdateTopKernelInteraface(void *top_kernel) {
  vector<void *> params = m_ast->GetFuncParams(top_kernel);
  vector<void *> new_params_list;
  for (auto param : params) {
    // void * init = m_ast->GetVariableInitializedName(decl);
    bool need_keep = false;
    vector<void *> vec_refs;
    m_ast->get_ref_in_scope(param, nullptr, &vec_refs);
    cout << "param = " << m_ast->UnparseToString(param)
         << ", size = " << vec_refs.size() << endl;
    std::reverse(vec_refs.begin(), vec_refs.end());
    for (auto one_ref : vec_refs) {
      // if not from function call, then keep it
      // if from function call and not from merlin API, keep it
      void *call = m_ast->TraceUpToFuncCall(one_ref);
      if (call != nullptr) {
        string call_name = m_ast->GetFuncNameByCall(call);
        if (m_ast->IsMerlinInternalIntrinsic(call_name)) {
          m_ast->RemoveStmt(m_ast->TraceUpToStatement(call));
        } else {
          need_keep = true;
          break;
        }
      } else {
        need_keep = true;
        break;
      }
      cout << "access = " << m_ast->UnparseToString(call) << endl;
    }
    if (need_keep) {
      // new_params_list.push_back(m_ast->CopyStmt(param));
      void *new_param = m_ast->CreateVariable(
          m_ast->GetTypebyVar(param), m_ast->GetVariableName(param), param);
      new_params_list.push_back(new_param);
    }
  }
  cout << "new param size = " << new_params_list.size() << endl;
  string top_kernel_name = m_ast->GetFuncName(top_kernel);
  void *return_type = m_ast->GetFuncReturnType(top_kernel);
  void *global_scope = m_ast->GetGlobal(top_kernel);
  void *new_decl =
      m_ast->CreateFuncDecl(return_type, top_kernel_name, new_params_list,
                            global_scope, true, nullptr);
  void *original_func_body = m_ast->GetFuncBody(top_kernel);
  void *copy_body = m_ast->CopyStmt(original_func_body);
  m_ast->ReplaceStmt(top_kernel, new_decl);
  m_ast->SetFuncBody(new_decl, copy_body);
}

/***************************************************************************/
// 1. Get all spawn kernels and spawn kernels factor
// 2. handle parallel PE of spawn
// 3. Delete spawn kernel call
// 4. replace spawn pragma with spawn-position pragma
// 5. add spawn kernel call in wrapper
// 6. add spawn definition in wrapper
/***************************************************************************/
void CStreamIR::CodeTransformForSpawnKernels() {
  CMarsAST_IF *ast = get_ast();
  CMarsIr mir;
  mir.get_mars_ir(ast, ast->GetProject(), mOptions, true);
  set<void *> all_spawn_pragmas;
  auto kernels = m_mars_ir_v2->get_top_kernels();
  for (auto top_kernel : kernels) {
    vector<void *> vec_spawn_pragma;
    vector<void *> vec_spawn_kernel;

    // Get wrapper function of top kernel
    void *top_kernel_wrapper = nullptr;
    string wrapper_func_name =
        MERLIN_WRAPPER_PREFIX + ast->GetFuncName(top_kernel);
    top_kernel_wrapper = ast->GetFuncDeclByName(wrapper_func_name);
    if (top_kernel_wrapper == nullptr) {
      continue;
    }
    vector<void *> top_calls;
    ast->GetFuncCallsByName(ast->GetFuncName(top_kernel), top_kernel_wrapper,
                            &top_calls);
    assert(top_calls.size() == 1);
    void *top_call_stmt = ast->TraceUpToStatement(top_calls[0]);

    // Get all pragmas from ref
    vector<void *> vec_pragmas;
    ast->GetNodesByType(top_kernel, "preorder", "SgPragmaDeclaration",
                        &vec_pragmas);
    for (size_t p = vec_pragmas.size(); p > 0; p--) {
      void *one_pragma = vec_pragmas[p - 1];
      bool errorOut = false;
      CAnalPragma curr_pragma(ast);
      // check if spawn pragma
      if ((ast->IsPragmaDecl(one_pragma) != 0) &&
          curr_pragma.PragmasFrontendProcessing(one_pragma, &errorOut, false) &&
          curr_pragma.is_spawn_kernel()) {
        if (all_spawn_pragmas.find(one_pragma) == all_spawn_pragmas.end()) {
          all_spawn_pragmas.insert(one_pragma);

          // 1. collect all spawn pragmas and kernels
          vec_spawn_pragma.push_back(one_pragma);
          CheckSpawnLegality(&mir, one_pragma);
          string spawn_factor_str = curr_pragma.get_attribute(CMOST_factor);
          cout << "Find spawn pragma."
               << ", pragma = " << ast->UnparseToString(one_pragma)
               << ", factor = " << spawn_factor_str.c_str() << endl;
          void *next_stmt = ast->GetNextStmt(one_pragma);
          vector<void *> vec_func_call;
          ast->GetNodesByType(next_stmt, "preorder", "SgFunctionCallExp",
                              &vec_func_call);
          assert(vec_func_call.size() == 1);

          void *spawn_call = vec_func_call[0];
          void *spawn_kernel = ast->GetFuncDeclByCall(spawn_call);
          string spawn_name = m_ast->GetFuncName(spawn_kernel);
          vec_spawn_kernel.push_back(spawn_kernel);

          // FIXME
          // 2. handle parallel PE of spawn
          // if factor is empty, pipelinel;
          // if factor is not empty, parallel, create a while loop outside
          // original loop
          bool is_auto_kernel = false;
          void *replace_stmt = one_pragma;
          if (!spawn_factor_str.empty()) {
            // create while loop for function
            void *global_scope = m_ast->GetGlobal(spawn_kernel);
            string original_spawn_name = "__merlin_org_" + spawn_name;
            void *clone_spawn_kernel = m_ast->CloneFuncDecl(
                spawn_kernel, m_ast->GetGlobal(spawn_kernel),
                original_spawn_name);
            m_ast->InsertStmt(clone_spawn_kernel, spawn_kernel);

            // delete original function arguments
            void *return_type = m_ast->GetFuncReturnType(spawn_kernel);
            vector<void *> empty_list;
            void *new_decl =
                m_ast->CreateFuncDecl(return_type, spawn_name, empty_list,
                                      global_scope, true, nullptr);
            m_ast->ReplaceStmt(spawn_kernel, new_decl);
            spawn_kernel = new_decl;

            // set while loop body
            void *func_body = m_ast->CreateBasicBlock();
            m_ast->SetFuncBody(spawn_kernel, func_body);
            void *while_body = m_ast->CreateBasicBlock();
            void *const_1 = m_ast->CreateConst(1);
            void *while_loop = m_ast->CreateWhileLoop(const_1, while_body);
            m_ast->AppendChild(func_body, while_loop);

            // handle compute id
            bool id_exist = false;
            vector<void *> calls;
            m_ast->GetNodesByType(global_scope, "preorder", "SgFunctionCallExp",
                                  &calls);
            for (auto one_call : calls) {
              string call_name = m_ast->GetFuncNameByCall(one_call);
              if (call_name == "get_compute_id") {
                id_exist = true;
              }
            }
            if (!id_exist) {
              m_ast->AddHeader("#define get_compute_id(x) x\n", global_scope);
            }
            vector<void *> vec_args;
            vec_args.push_back(m_ast->CreateConst(0));
            void *get_compute_id = m_ast->CreateFuncCall(
                "get_compute_id", m_ast->GetTypeByString("int"), vec_args,
                global_scope);
            string id_name = "id";
            void *compute_id = m_ast->CreateVariableDecl(
                "int", id_name, get_compute_id, func_body);
            m_ast->InsertStmt(compute_id, while_loop);
            vector<void *> vec_args_original;
            vec_args_original.push_back(m_ast->CreateVariableRef(compute_id));
            void *original_call_stmt = m_ast->CreateFuncCallStmt(
                clone_spawn_kernel, vec_args_original);
            m_ast->AppendChild(while_body, original_call_stmt);

            // add attributes
            m_ast->set_kernel_attribute(spawn_kernel, "autorun", "");
            m_ast->set_kernel_attribute(spawn_kernel, "num_compute_units",
                                        spawn_factor_str);
            m_ast->set_kernel_attribute(spawn_kernel, "max_global_work_dim",
                                        "0");

            // get replace spawn-position stmt
            replace_stmt = m_ast->TraceUpToLoopScope(one_pragma);
            cout << "Replace_stmt = " << m_ast->UnparseToString(replace_stmt)
                 << endl;

            is_auto_kernel = true;
            vec_auto_kernel.push_back(spawn_name);
          } else {
            int number = m_ast->GetFuncParamNum(spawn_kernel);
            if (number == 0) {
              is_auto_kernel = true;

              // create while loop for function
              void *global_scope = m_ast->GetGlobal(spawn_kernel);
              string original_spawn_name = "__merlin_org_" + spawn_name;
              void *clone_spawn_kernel = m_ast->CloneFuncDecl(
                  spawn_kernel, m_ast->GetGlobal(spawn_kernel),
                  original_spawn_name);
              m_ast->InsertStmt(clone_spawn_kernel, spawn_kernel);

              // delete original function arguments
              void *return_type = m_ast->GetFuncReturnType(spawn_kernel);
              vector<void *> empty_list;
              void *new_decl =
                  m_ast->CreateFuncDecl(return_type, spawn_name, empty_list,
                                        global_scope, true, nullptr);
              m_ast->ReplaceStmt(spawn_kernel, new_decl);
              spawn_kernel = new_decl;

              // set while loop body
              void *func_body = m_ast->CreateBasicBlock();
              m_ast->SetFuncBody(spawn_kernel, func_body);
              void *while_body = m_ast->CreateBasicBlock();
              void *const_1 = m_ast->CreateConst(1);
              void *while_loop = m_ast->CreateWhileLoop(const_1, while_body);
              m_ast->AppendChild(func_body, while_loop);
              vector<void *> vec_args_original;
              void *original_call_stmt = m_ast->CreateFuncCallStmt(
                  clone_spawn_kernel, vec_args_original);
              m_ast->AppendChild(while_body, original_call_stmt);

              vec_auto_kernel.push_back(spawn_name);
              m_ast->set_kernel_attribute(spawn_kernel, "autorun", "");
              m_ast->set_kernel_attribute(spawn_kernel, "max_global_work_dim",
                                          "0");
            }
          }

          // 4. Delete kernel call
          ast->RemoveStmt(next_stmt);

          // 3. Replace spawn pragma with spawn-position pragma
          string spawn_position_string =
              "ACCEL spawn-position kernel=" + ast->GetFuncName(spawn_kernel);
          if (!spawn_factor_str.empty()) {
            spawn_position_string += " factor=" + spawn_factor_str;
          }
          void *new_pragma =
              ast->CreatePragma(spawn_position_string, top_kernel);
          ast->ReplaceStmt(replace_stmt, new_pragma);

          // 5. Add spawn kernel call in wrapper if not auto kernel
          if (!is_auto_kernel) {
            vector<void *> vec_args;
            for (int j = 0; j < ast->GetFuncParamNum(spawn_kernel); ++j) {
              void *parameter = ast->GetFuncParam(spawn_kernel, j);
              string s_port = ast->GetVariableName(parameter);
              cout << "spawn s_port = " << s_port << endl;
              if (ast->IsScalarType(parameter) != 0) {
                cout << "find scalar port" << endl;
                for (int i = 0; i < ast->GetFuncParamNum(top_kernel_wrapper);
                     ++i) {
                  void *wrapper_parameter =
                      ast->GetFuncParam(top_kernel_wrapper, i);
                  if ((ast->IsScalarType(wrapper_parameter) != 0) &&
                      ast->GetVariableName(wrapper_parameter) == s_port) {
                    cout << "match" << endl;
                    void *ref = ast->CreateVariableRef(wrapper_parameter);
                    vec_args.push_back(ref);
                  }
                }
              } else {
                vector<void *> decls;
                ast->GetNodesByType(top_kernel_wrapper, "preorder",
                                    "SgVariableDeclaration", &decls);
                for (auto one_decl : decls) {
                  string var_name = ast->GetVariableName(one_decl);
                  if (var_name == "__m_" + s_port) {
                    void *ref = ast->CreateVariableRef(one_decl);
                    vec_args.push_back(ref);
                  }
                }
              }
            }
            void *wrapper_spawn_call =
                ast->CreateFuncCall(spawn_kernel, vec_args);
            void *wrapper_spawn_stmt =
                ast->CreateStmt(V_SgExprStatement, wrapper_spawn_call);
            ast->InsertAfterStmt(wrapper_spawn_stmt, top_call_stmt);

            // 6. Add spawn kernel definition in wrapper
            void *clone_spawn_func = ast->CloneFuncDecl(
                spawn_kernel, ast->GetGlobal(top_kernel_wrapper), false);
            ast->InsertStmt(clone_spawn_func, top_kernel_wrapper);
          }
        }
      }
    }
    map_kernel_spawn_pragma.insert(
        pair<void *, vector<void *>>(top_kernel, vec_spawn_pragma));
    map_kernel_spawn_kernel.insert(
        pair<void *, vector<void *>>(top_kernel, vec_spawn_kernel));
    // TODO(youxiang): if want to update kernel interface, need to check if
    // wrapper can generate successful UpdateTopKernelInteraface(top_kernel);
  }
}
