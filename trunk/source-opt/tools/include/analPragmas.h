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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ANALPRAGMAS_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ANALPRAGMAS_H_
#include <list>
#include <map>
#include <vector>
#include <string>

#include "codegen.h"
#include "ir_types.h"
#include "rose.h"

typedef std::list<SgNode *> task_list_type;

struct pragma_tag {
  bool pipeline;
  bool parallel;
  bool loop_tiling;
  bool burst;
  bool task;
  bool kernel;
  bool reduction;
  bool line_buffer;
  bool interface;
  bool attribute;
  bool task_wait;
  bool skip_sync;
  bool spawn;           // ZP: 2017-05-10
  bool spawn_kernel;    // HH: 2019-6-1
  bool false_dep;       // YX: 2017-09-13
  bool coalescing;      // YX: 2017-12-05
  bool loop_flatten;    // YXin: 2018-04-17
  bool loop_tiled_tag;  // YX: 2018-03-07
  bool cache;           // YX: 2018-12-30
  bool hls_unroll;
  bool partition;
};

class CAnalPragma {
 private:
  //  YX: 20180228 do not use it because we need a pointer to AST_IF
  CAnalPragma();
  std::map<std::string, std::string> pragma_table;
  CSageCodeGen *m_ast;
  void *m_ref;
  SgNode *location;
  pragma_tag opt_tag;
  std::string m_pragma_type;
  std::string m_cmd;
  std::string m_vendor;
  bool m_hls_fgopt;

 public:
  explicit CAnalPragma(CSageCodeGen *codegen)
      : m_ast(codegen), m_ref(nullptr), location(nullptr) {
    opt_clear();
  }
  bool PragmasFrontendProcessing(void *decl, bool *errorOut, bool check = false,
                                 bool vendor_pragma = false);
  void opt_clear();
  std::string get_hls_interface_mode() const;
  std::string get_task_name();
  bool is_hls_fg_opt() { return m_hls_fgopt; }
  //  Interface
  bool add_attribute(std::string attr, std::string val);
  bool append_attribute(std::string attr, std::string val);
  void *update_pragma(void *decl, bool keep_old, void *scope);
  std::string print_attribute();
  std::string get_attribute(std::string attr);
  std::string get_pragma_type() { return m_cmd; }
  std::string get_vendor_type() { return m_vendor; }
  void remove_attribute(std::string attr);
  bool is_hls_pragma(std::string filter);
  bool is_hls_pragma();
  void set_pragma_type(std::string pragma_type);
  std::map<std::string, std::string> get_attribute();

  bool is_loop_related() const {
    return opt_tag.pipeline || opt_tag.parallel || opt_tag.loop_tiling ||
           opt_tag.spawn || opt_tag.false_dep || opt_tag.loop_flatten ||
           opt_tag.loop_tiled_tag || opt_tag.hls_unroll ||
           //   opt_tag.reduction || opt_tag.line_buffer;
           opt_tag.reduction || opt_tag.line_buffer || opt_tag.partition;
  }

  bool is_interface() { return opt_tag.interface; }
  bool is_loop_tiled_tag() { return opt_tag.loop_tiled_tag; }
  bool is_cache() { return opt_tag.cache; }
  bool is_pipeline() { return opt_tag.pipeline; }
  bool is_parallel() { return opt_tag.parallel; }
  bool is_loop_tiling() { return opt_tag.loop_tiling; }
  bool is_loop_flatten() { return opt_tag.loop_flatten; }
  bool is_false_dep() { return opt_tag.false_dep; }
  bool is_reduction() { return opt_tag.reduction; }
  bool is_line_buffer() { return opt_tag.line_buffer; }
  bool is_spawn() { return opt_tag.spawn; }
  bool is_spawn_kernel() { return opt_tag.spawn_kernel; }
  bool is_task() { return opt_tag.task; }
  bool is_kernel() { return opt_tag.kernel; }
  bool is_burst() { return opt_tag.burst; }
  bool is_attribute() { return opt_tag.attribute; }
  bool is_task_wait() { return opt_tag.task_wait; }
  bool is_skip_sync() { return opt_tag.skip_sync; }
  bool is_coalescing() { return opt_tag.coalescing; }
  bool is_hls_unroll() { return opt_tag.hls_unroll; }
  bool is_partition() { return opt_tag.partition; }

  void splitString(std::string str, std::string delim,
                   vector<std::string> *results);
  std::string get_string() const { return m_ast->GetPragmaString(m_ref); }
  void *get_ref() const { return m_ref; }

  bool contain_auto_value() const;
  bool regex_match_auto(std::string str) const;

 protected:
  bool parse_pragma(void *decl, std::string *diagnosis_info,
                    bool report = false, bool vendor_pragma = false);
  bool isPipelinePragma(SgPragmaDeclaration *decl);
  bool isLoopTilingPragma(SgPragmaDeclaration *decl);
  bool isPipe_ParallelPragma(SgPragmaDeclaration *decl);
  bool isBurstPragma(SgPragmaDeclaration *decl);
  bool isTaskPragma(SgPragmaDeclaration *decl);
  bool isKernelPragma(SgPragmaDeclaration *decl);
  bool isInterfacePragma(SgPragmaDeclaration *decl);
  bool isSkipSynchronizationPragma(SgPragmaDeclaration *decl);
  bool check_attribute_legality(void *decl, std::string *diagnosis_info,
                                bool report);
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ANALPRAGMAS_H_
