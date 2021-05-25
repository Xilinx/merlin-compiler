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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_COALESCING_INTEL_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_COALESCING_INTEL_H_
#include <map>
#include <set>
#include <string>
#include <vector>

#include "rose.h"

#include "PolyModel.h"
#include "bitwidth_opt.h"
#include "cmdline_parser.h"
#include "codegen.h"
#include "file_parser.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

class MemoryCoalescingIntel {
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  CMarsIr mMars_ir;
  CMarsIrV2 mMars_ir_v2;
  int mBitWidth;
  bool m_valid;
  bool mNaiveHLS;
  bool mBitWidthSpecified;
  const unsigned int m_unroll_threshold = 64;
  const int m_max_bitwidth = 512;

 public:
  MemoryCoalescingIntel(CSageCodeGen *Codegen, void *pTopFunc,
                        const CInputOptions &Options)
      : m_ast(Codegen), mTopFunc(pTopFunc), mOptions(Options), mBitWidth(512),
        m_valid(true), mNaiveHLS(false), mBitWidthSpecified(false),
        m_count_memcpy(0) {
    init(Codegen);
  }
  bool run();

  ~MemoryCoalescingIntel();

 private:
  bool update_interface(CMirNode *fNode);

  void init(CSageCodeGen *Codegen);

  void collectInterfaceInfo();

  void collectInterfaceInfo(void *func_def);

  bool updateFunctionInterface();

  bool updateFunctionInterface(void *func_def);

  bool isMemcpy(void *func_call);

  bool updateMemcpyCallWithLargeBitWidth(const MemcpyInfo &info,
                                         void *orig_memcpy_call);
  bool isSupportedType(void *base_type);

  bool checkInvalidBitWidth(int opt_bitwidth, int orig_bitwidth);

  int memcpy_replace(const MemcpyInfo &info, void *func_call);

  int handle_single_assignment(void *sg_array1,
                               const std::vector<void *> &sg_idx1,
                               const std::vector<size_t> &dim1, void *sg_array2,
                               const std::vector<void *> &sg_idx2,
                               const std::vector<size_t> &dim2, void *sg_length,
                               int type_size, void *func_call);

  int generate_nested_loops(void *sg_array1, const std::vector<void *> &sg_idx1,
                            const std::vector<size_t> &dim1, void *sg_array2,
                            const std::vector<void *> &sg_idx2,
                            const std::vector<size_t> &dim2, void *sg_length,
                            int type_size, void *func_call,
                            const MemcpyInfo &info);

 private:
  std::set<void *> mGlobals_with_memcpy_header;
  std::set<void *> mGlobals_with_ap_int_header;
  std::vector<void *> mFunc_defs;
  std::map<void *, std::set<std::string>> mMacroCall;
  //  key is the first declaration with no definition
  std::map<void *, std::vector<ArgumentInfo>> mFunc2Info;
  std::set<void *> mDead_func_calls;

  const std::vector<std::string> m_supported_types{
      "int",           "unsigned int",   "signed int",  "short",
      "signed short",  "unsigned short", "char",        "signed char",
      "unsigned char", "long",           "signed long", "unsigned long",
      "float",         "double"};
  int m_count_memcpy;
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_COALESCING_INTEL_H_
