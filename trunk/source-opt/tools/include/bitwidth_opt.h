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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BITWIDTH_OPT_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BITWIDTH_OPT_H_
#include <vector>
#include <map>
#include <string>

#include "rose.h"

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "codegen.h"
#include "file_parser.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

struct MemcpyPort {
  void *array;
  void *pntr;
  std::vector<void *> index;
  int pointer;
  MemcpyPort() {
    array = nullptr;
    pntr = nullptr;
    pointer = 0;
  }
  std::string print() {
    std::string res = (pointer ? "pointer" : "array");
    res += " with " + my_itoa(index.size()) + " dimension";
    return res;
  }
  MemcpyPort(const MemcpyPort &port)
      : array(port.array), pntr(port.pntr), index(port.index),
        pointer(port.pointer) {}
};

struct MemcpyInfo {
  std::vector<MemcpyPort> port;
  void *length_exp;
  void *base_type;
  //  read from external memory to local buffer
  bool read;
  int bitwidth;
  int opt_bitwidth;
  int dimension;
  int opt_dimension;
  bool unaligned_buffer;
  int aligned_dimension;
  std::vector<size_t> dimension_size;
  MemcpyInfo() {
    port.resize(2);
    length_exp = nullptr;
    read = false;
    dimension = -1;
    opt_dimension = -1;
    unaligned_buffer = false;
    aligned_dimension = 0;
    bitwidth = 0;
    opt_bitwidth = 0;
    base_type = nullptr;
  }
  MemcpyInfo(const MemcpyInfo &info)
      : port(info.port), length_exp(info.length_exp), base_type(info.base_type),
        read(info.read), bitwidth(info.bitwidth),
        opt_bitwidth(info.opt_bitwidth), dimension(info.dimension),
        opt_dimension(info.opt_dimension),
        unaligned_buffer(info.unaligned_buffer),
        aligned_dimension(info.aligned_dimension),
        dimension_size(info.dimension_size) {}
};

struct SingleAssignInfo {
  bool read;
  bool write;
  std::vector<void *> index;
  void *arg;
  int opt_bitwidth;
  int bitwidth;
  SingleAssignInfo() {
    read = false;
    write = false;
    arg = nullptr;
    opt_bitwidth = 0;
    bitwidth = 0;
  }
  SingleAssignInfo(const SingleAssignInfo &info)
      : read(info.read), write(info.write), index(info.index), arg(info.arg),
        opt_bitwidth(info.opt_bitwidth), bitwidth(info.bitwidth) {}
};

struct CallInfo {
  //  a postion of a argument of  a callEE function
  int pos;
  //  a variable ref of a argument of a callER function
  void *ref;
};

struct ArgumentInfo {
  bool any_burst_access;
  bool all_support_access;
  bool suboptimal_coalescing;
  bool need_changed;
  void *base_type;
  int bitwidth;
  int opt_bitwidth;
  bool specified_bitwidth;
  enum access_type ac_type;
  bool any_unaligned_access;
  std::map<void *, std::vector<CallInfo>> calls2ref;
  std::map<void *, MemcpyInfo> memcpy;
  std::map<void *, SingleAssignInfo> single_assign;
  ArgumentInfo() {
    any_burst_access = false;
    all_support_access = true;
    suboptimal_coalescing = false;
    need_changed = false;
    base_type = nullptr;
    bitwidth = 0;
    opt_bitwidth = 0;
    specified_bitwidth = false;
    ac_type = NO_ACCESS;
    any_unaligned_access = false;
  }
  ArgumentInfo(const ArgumentInfo &info)
      : any_burst_access(info.any_burst_access),
        all_support_access(info.all_support_access),
        suboptimal_coalescing(info.suboptimal_coalescing),
        need_changed(info.need_changed), base_type(info.base_type),
        bitwidth(info.bitwidth), opt_bitwidth(info.opt_bitwidth),
        specified_bitwidth(info.specified_bitwidth), ac_type(info.ac_type),
        any_unaligned_access(info.any_unaligned_access),
        calls2ref(info.calls2ref), memcpy(info.memcpy),
        single_assign(info.single_assign) {}
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_BITWIDTH_OPT_H_
