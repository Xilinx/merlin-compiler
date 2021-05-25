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
//  stream_ir.h
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

#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>

#include "codegen.h"
#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "program_analysis.h"

namespace MerlinStreamModel {

const int DEFAULT_CHANNEL_DEPTH = 32;
//  handling the common functionality among node
//  1. AST link (m_ast, m_ref)
//  2. attribute operation
//  3. pragma processing
class CStreamBase {
 public:
  CStreamBase(CMarsAST_IF *ast, const CInputOptions &options, CMarsIr *mars_ir,
              CMarsIrV2 *mars_ir_v2) {
    m_ast = ast;
    //  liangz: create CMarsIrV2 if it is nullptr
    if (!mars_ir) {
      CMarsIr *my_mars_ir = new CMarsIr();
      my_mars_ir->get_mars_ir(ast, ast->GetProject(), options, true);
      m_mars_ir = my_mars_ir;
      isCMarsIRCreated = true;
    } else {
      m_mars_ir = mars_ir;
    }
    if (!mars_ir_v2) {
      CMarsIrV2 *my_mars_ir_v2 = new CMarsIrV2();
      my_mars_ir_v2->build_mars_ir(ast, ast->GetProject());
      m_mars_ir_v2 = my_mars_ir_v2;
      isCMarsIR2Created = true;
    } else {
      m_mars_ir_v2 = mars_ir_v2;
    }
    m_ref = nullptr;
    m_pragma_type = "unknown";
  }
  CStreamBase(CMarsAST_IF *ast, void *ref, string type = "unknown",
              CMarsIr *mars_ir = nullptr, CMarsIrV2 *mars_ir_v2 = nullptr) {
    m_ast = ast;
    m_ref = ref;
    m_pragma_type = type;
    m_mars_ir_v2 = mars_ir_v2;
    m_mars_ir = mars_ir;
    isCMarsIR2Created = false;
    isCMarsIRCreated = false;
  }
  virtual ~CStreamBase() {
    if (isCMarsIRCreated) {
      delete m_mars_ir;
    }
    if (isCMarsIR2Created) {
      delete m_mars_ir_v2;
    }
  }
  CMarsAST_IF *get_ast() { return m_ast; }

  //  Attributes are coded as pragmas in stream model program
  void set_attribute(string key, string value) { m_attr[key] = value; }
  string get_attribute(string key) { return m_attr[key]; }
  void append_attribute(string key, string value) {
    m_attr[key] += "," + value;
  }

  //  if sg_pragma is nullptr, the statement before m_ref will be tried for the
  //  sg_pragma if sg_pragma is still nullptr, a new pragma will be generated
  void parse_pragma(void *sg_pragma = nullptr);   //  pragma -> attribute
  void update_pragma(void *sg_pragma = nullptr);  //  attribute -> pragma

  void *get_ref() { return m_ref; }
  virtual string print_ref() {
    return m_ast->_up(m_ref, 30);
  }  //  display length to be adjustable
  virtual string print_attribute();
  virtual string print() { return print_ref() + " : " + print_attribute(); }

  //  simply get the previous stmt, can be overloaded
  virtual void *get_pragma_by_ref();

  string get_pragma_type() { return m_pragma_type; }

 protected:
  CMarsAST_IF *m_ast;
  CMarsIr *m_mars_ir;
  CMarsIrV2 *m_mars_ir_v2;
  void *m_ref;
  map<string, string> m_attr;
  string m_pragma_type;
  bool isCMarsIRCreated = false;
  bool isCMarsIR2Created = false;

 public:
  map<void *, vector<void *>> map_kernel_spawn_pragma;
  map<void *, vector<void *>> map_kernel_spawn_kernel;
  vector<string> vec_auto_kernel;
  vector<void *> vec_merlin_stream;
  map<void *, void *> map_merlin_stream_depth;
};

//  ////////////////////////////////////////////////////  /
//  CStreamNode Class
//  ////////////////////////////////////////////////////  /
class CStreamPort;
class CStreamNode : public CStreamBase {
 public:
  CStreamNode(CMarsAST_IF *ast, void *ref, CMarsIr *mars_ir = nullptr,
              CMarsIrV2 *mars_ir_v2 = nullptr)
      : CStreamBase(ast, ref, PRAGMA_TYPE_NODE, mars_ir, mars_ir_v2) {}
  virtual ~CStreamNode() {}

 public:
  vector<CStreamPort *> get_ports() { return m_ports; }
  virtual string print_ref();
  void *get_scope();
  void *get_pos();

  vector<int> get_md_attribute();

  void get_func_call_decl(void **func_call, void **func_decl);

  vector<void *> get_para_loops();

 protected:
  vector<CStreamPort *> m_ports;

 protected:  //  for hierarchical structure
  CStreamNode *m_parent;
  vector<CStreamNode *> m_children;
};

//  ////////////////////////////////////////////////////  /
//  CStreamPort Class
//  ////////////////////////////////////////////////////  /
class CStreamChannel;
class CStreamPort : public CStreamBase {
 public:
  CStreamPort(CMarsAST_IF *ast, void *ref, void *array, CStreamNode *node,
              CMarsIr *mars_ir = nullptr, CMarsIrV2 *mars_ir_v2 = nullptr)
      : CStreamBase(ast, ref, PRAGMA_TYPE_PORT, mars_ir, mars_ir_v2) {
    m_node = node;
    m_array = array;
  }
  virtual ~CStreamPort() {}

 public:
  CStreamChannel *get_fifo_by_ref(void *ref) { return m_map_ref2fifo[ref]; }
  vector<void *> get_refs() {
    vector<void *> ret;
    for (auto it : m_map_ref2fifo)
      ret.push_back(it.first);
    return ret;
  }
  vector<CStreamChannel *> get_fifos() {
    vector<CStreamChannel *> ret;
    for (auto it : m_map_ref2fifo)
      ret.push_back(it.second);
    return ret;
  }

  virtual string print_ref() {
    return m_ast->_p(m_array) + "@" + m_node->print_ref();
  }
  virtual string print_content() { return m_ast->_up(m_ref); }
  void *get_content() { return m_ref; }
  void *get_scope();
  void *get_pos();
  void add_fifo(void *ref, CStreamChannel *fifo) { m_map_ref2fifo[ref] = fifo; }

  int is_write() {
    return get_attribute(PRAGMA_ACCESS_TYPE_ATTRIBUTE) == "write";
  }
  int is_read() {
    return get_attribute(PRAGMA_ACCESS_TYPE_ATTRIBUTE) == "read";
  }
  CStreamNode *get_node() { return m_node; }

  void get_func_call_decl(void **func_call, void **func_decl);

  int get_sufficient_channel_size(void *sp_ref);

 protected:
  map<void *, CStreamChannel *> m_map_ref2fifo;
  CStreamNode *m_node;

  void *m_array;
};

//  ////////////////////////////////////////////////////  /
//  CStreamChannel Class
//  ////////////////////////////////////////////////////  /
class CStreamChannel : public CStreamBase {
 public:
  CStreamChannel(CMarsAST_IF *ast, void *ref)
      : CStreamBase(ast, ref, PRAGMA_TYPE_CHANNEL) {}

  virtual ~CStreamChannel() {}
  int get_depth() {
    string channel_depth = get_attribute(PRAGMA_CHANNEL_DEPTH_ATTRIBUTE);
    if (channel_depth.empty())
      return DEFAULT_CHANNEL_DEPTH;
    return my_atoi(channel_depth);
  }

 protected:
};

//  ////////////////////////////////////////////////////  /
//  CStreamBuffer Class
//  ////////////////////////////////////////////////////  /
class CStreamBuffer : public CStreamBase {
 public:
  CStreamBuffer(CMarsAST_IF *ast, void *ref)
      : CStreamBase(ast, ref, PRAGMA_TYPE_BUFFER) {
    wr_port = rd_port = nullptr;
  }

  virtual ~CStreamBuffer() {}
  void set_write_port(CStreamPort *port) { wr_port = port; }

  void set_read_port(CStreamPort *port) { rd_port = port; }

  CStreamPort *get_write_port() { return wr_port; }

  CStreamPort *get_read_port() { return rd_port; }

 protected:
  CStreamPort *wr_port;
  CStreamPort *rd_port;
};

//  ////////////////////////////////////////////////////  /
//  CStreamIR class - the main class for operations and containers
//  ////////////////////////////////////////////////////  /
class CStreamIR : public CStreamBase {
  CInputOptions mOptions;

 public:
  CStreamIR(CMarsAST_IF *ast, const CInputOptions &option, CMarsIr *mars_ir,
            CMarsIrV2 *mars_ir_v2)
      : CStreamBase(ast, option, mars_ir, mars_ir_v2), mOptions(option),
        m_num_stream(0) {}

  ~CStreamIR() { clear(); }

  void clear();

 public:
  bool CheckIfHaveOldStream(void *decl);
  void CheckStreamLecality(void *decl);
  void ChannelReplacement();
  void GetAllStreamDecl();
  void InsertChannelDecl();
  void InsertChannelReadWrite();
  void CleanMerlinStream();
  void CodeTransformForSpawnKernels();
  void CheckSpawnLegality(CMarsIr *mars_ir, void *pragma);
  void GetChannelAttributesFromInit(void *init, void **depth_ref,
                                    string *opencl_channel_type);
  void InsertChannelExtension(void *decl, set<void *> *processed_global);
  void InsertChannelFuncDefinition(void *data_type, string wr_channel_func,
                                   string rd_channel_func);

  void
  SpawnKernelTransform();  // build stream model C-code from "spawn" pragmas
  void UpdateTopKernelInteraface(void *top_kernel);

  //  Parse the Stream Model from AST
  void ParseStreamModel();  //  parse stream model from ast

  //  Create the Stream Model Node, and transform AST
  CStreamNode *SpawnNodeFromAST(void *node_stmt, string *err_msg,
                                bool is_prefetch_only = false,
                                set<void *> *prefetch_vars = nullptr);

  //  Create the Stream Model Node for memory access
  CStreamNode *SpawnMemoryAccessFromAST(void *sg_init, void *sg_scope,
                                        string *err_msg);

  //  Access the Stream Model
  CStreamNode *
  FindNodeByAST(void *stmt);  //  trace upward to find the containing node
  CStreamPort *
  FindPortByAST(void *ref);  //  trace upward to find the containing port
  CStreamPort *FindMatchPort(void *ref);  //  match by SSST condition
  CStreamChannel *
  FindChannelByAST(void *ref);  //  trace upward to find the channel
  CStreamBuffer *
  FindBufferByAST(void *ref);  //  trace upward to find the buffer

  vector<CStreamChannel *> get_fifos() { return m_fifos; }
  bool is_fifo(void *var);

  //  Generate the Concurrent Code
  void ConcurrentCodeGeneration(
      const vector<void *> &top_kernels,
      map<void *, vector<void *>>
          *kernel_autokernel);  //  generate concurrent code with real kernels

  void CodeGeneration_channel(CStreamChannel *fifo,
                              set<void *> *processed_globals);

  void CodeGeneration_node(CStreamNode *node, const vector<void *> &top_kernels,
                           set<void *> *processed_globals,
                           map<void *, vector<void *>> *kernel_autokernel);

  void CodeGeneration_port(CStreamPort *port);

  vector<CStreamPort *> CreateStreamPortBySeparation(
      CStreamNode *node, void *sg_arg, map<void *, void *> *map_spref2snref,
      CMerlinMessage *msg = nullptr, bool is_skip_node_create = false,
      void *sg_kernel = nullptr);

  int contains_channel() { return m_fifos.size() > 0; }

  int CheckStreamPortSeparability(CStreamNode *node, void *sg_arg,
                                  CMerlinMessage *msg = nullptr);
  int CheckStreamPortSeparability(void *sg_scope,
                                  vector<CMerlinMessage> *vec_msg,
                                  set<void *> *checked_names = nullptr);
  bool is_node_kernel(void *func);
  bool is_port_kernel(void *func);
  CStreamPort *get_matched_port(void *func);
  CStreamNode *get_matched_node(void *func);

 protected:
  void InheritPort(CStreamPort *s_p, CStreamNode *s_n_parent,
                   CStreamNode *s_n_child);
  void ConnectPort(CStreamPort *s_p0, CStreamPort *s_p1);
  void EmbedPort(CStreamPort *s_p0, void *embed_pos);

 public:
  CStreamNode *CreateNode(void *ref, CMarsIr *, CMarsIrV2 *);
  CStreamPort *CreatePort(void *ref, void *array, CStreamNode *node, CMarsIr *,
                          CMarsIrV2 *);
  CStreamChannel *CreateChannels(void *ref);
  CStreamBuffer *CreateBuffer(void *ref);
  //  bool prefetch = false;
  //  bool spawn = false;

 protected:  //  containers
  vector<CStreamNode *> m_nodes;
  vector<CStreamPort *> m_ports;
  vector<CStreamChannel *> m_fifos;
  vector<CStreamBuffer *> m_buffers;
  int m_num_stream;

  void *m_content;
};
}  //  namespace MerlinStreamModel
