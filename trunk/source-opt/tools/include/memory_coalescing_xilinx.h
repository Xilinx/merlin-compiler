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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_COALESCING_XILINX_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_COALESCING_XILINX_H_
#include <map>
#include <set>
#include <vector>
#include <string>
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

class MemoryCoalescingXilinx {
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  CMarsIr mMars_ir;
  CMarsIrV2 mMars_ir_v2;
  int mBitWidth;
  bool mBitWidthSpecified;
  int mMaxDimension;
  bool mNaiveHLS;
  bool mEnableCDesign;
  bool mEnableSubOptimal;
  std::set<int> mBitWidthSet;
  size_t m_length_threshold;
  bool mCpp_design;
  bool mWideBusFlag;
  std::set<string> mCoalescingHeader;

 public:
  MemoryCoalescingXilinx(CSageCodeGen *Codegen, void *pTopFunc,
                         const CInputOptions &Options)
      : m_ast(Codegen), mTopFunc(pTopFunc), mOptions(Options), mBitWidth(512),
        mBitWidthSpecified(false), mMaxDimension(5), mNaiveHLS(false),
        mEnableCDesign(false), mEnableSubOptimal(false),
        m_length_threshold(WIDE_BUS_LENGTH_THRESHOLD), mCpp_design(false),
        mWideBusFlag(false) {
    init(Codegen);
  }
  bool run();

  ~MemoryCoalescingXilinx();

  void get_opt_bitwidth(int *opt_bitwidth, bool *specified_bitwdith, void *arg,
                        int bitwidth, const vector<void *> &vec_pragma,
                        bool report);

 private:
  bool update_interface(CMirNode *fNode);

  void init(CSageCodeGen *Codegen);

  void collectInterfaceInfo();

  void InitInterfaceInfo(void *func_def);

  void backwardInterfaceInfo(void *func_def);

  void forwardInterfaceInfo(void *func_def);

  void AdjustInterfaceInfo();
  bool updateKernelFunction();

  bool updateFunctionInterface();

  bool updateFunctionInterface(void *func_def);

  void *getLargeBitWidthType(void *orig_type, int opt_bitwidth, int bitwidth,
                             void *pos);

  void *getLargeBitWidthTypeWithoutModifier(void *orig_type, int opt_bitwidth);

  bool isMemcpy(void *func_call);

  bool updateMemcpyCallWithLargeBitWidth(const MemcpyInfo &info,
                                         void *orig_memcpy_call);
  bool isSupportedType(void *base_type);

  std::string StripSignnessInType(void *base_type, bool replace_space);

  void *AddCastToStripSignness(void *address_exp, void *base_type,
                               const MemcpyInfo &info);

  void AddMemCpyHeader(void *scope);

  void AddDataTypeHeader(void *scope);

  void AddPartitionPragma(const std::string &buf_name, void *stmt,
                          const MemcpyInfo &info);

  bool checkInvalidBitWidth(int opt_bitwidth, int orig_bitwidth);

  bool updateSingleAssignWithLargeBitWidth(const SingleAssignInfo &info,
                                           void *base_type, void *orig_access,
                                           void *kernel_decl);

  bool isUnsupportedSingleAssign(void *single_assign);

  void *createBuffer(void *base_type, void *sg_scope, void *func_decl);

  void detect_access_in_paralleled_loop(void *ref, ArgumentInfo *info);

  void auto_cache_for_coalescing();

  void parse_memcpy_info(void *func_call,
                         const std::map<void *, int> &paralists,
                         std::vector<ArgumentInfo> *arg_info_);

 private:
  std::set<void *> mGlobals_with_memcpy_header;
  std::set<void *> mGlobals_with_data_type_header;
  std::vector<void *> mFunc_defs;
  std::map<void *, std::set<std::string>> mMacroCall;
  //  key is the first declaration with no definition
  std::map<void *, std::vector<ArgumentInfo>> mFunc2Info;
  std::set<void *> mDead_func_calls;

  const std::vector<std::string> m_supported_types{
      "int",       "unsigned int",     "signed int",
      "char",      "signed char",      "unsigned char",
      "short",     "signed short",     "unsigned short",
      "long",      "signed long",      "unsigned long",
      "long long", "signed long long", "unsigned long long",
      "float",     "double",           "ap_int",
      "ap_uint"};
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_COALESCING_XILINX_H_
