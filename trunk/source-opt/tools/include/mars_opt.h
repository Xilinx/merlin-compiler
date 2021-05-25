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


#pragma once
#include <cassert>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "rose.h"
using std::map;
using std::set;
using std::string;
using std::vector;

#define DEBUG 0
#define XMD_GEN 0
#define RUNTIME_GEN 1
#define PCIE_GEN 2
#define CPU_GEN 3
#define MERLIN_TYPE_DEFINE "merlin_type_define.h"
#define MERLIN_WRAPPER_PREFIX "__merlinwrapper_"
#define MERLIN_WRITE_BUFFER_PREFIX "__merlin_write_buffer_"
#define MERLIN_READ_BUFFER_PREFIX "__merlin_read_buffer_"
#define MERLIN_EXECUTE_PREFIX "__merlin_execute_"
#define KERNEL_FILE_JSON "kernel_file.json"
#define KERNEL_LIST_JSON "kernel_list.json"
#define TASK_KERNEL_JSON "task_kernel.json"
#define TASK_ATTRIBUTE_JSON "task_attribute.json"
#define FUNCTION_NAME_UNIQUFIED_TAG "function_name_uniquified.tag"
#define MAX_SIZE_THRESHOLD (1LL << 32)
#define MAX_CONST_BYTE (1024 * 1024)
#define XILINX_ARG_LIMITATION 256
#define INTEL_ARG_LIMITATION 256
#define WARNING_ARG_LIMITATION 16
#define XILINX_ARG_SCALAR_LIMITATOIN 16
#define INTEL_DEFAULT_REGISTER_THRESHOLD                                       \
  512  // 64bytes for array in single work-item kernel
#define INTEL_MAX_REGISTER_THRESHOLD                                           \
  8192  // 1Kbytes for array in multiple work-item kernel
#define MERLIN_SMALL_TRIP_COUNT_THRESHOLD 16
#define MAX_PASS_LENGTH 100
#define WIDE_BUS_LENGTH_THRESHOLD 128
#define PARTITION_FACTOR_UPPER_THRESHOLD 256
#define SINGLE_BUFFER_SIZE_THRESHOLD (1 * 1024 * 1024)
#define OPENCL_HOST_GENERATION_DISABLED_TAG                                    \
  ".opencl_host_generation_disable_tag"
//  pass_name: MemBurst CoarseOpt FineOpt WideBus Outline Reduction
//  level: INFO, WARNING, ERROR
//  message: multi-line display message
//  number: INFO 1xx, Warning 2xx, Error 3xx
class CSageCodeGen;
void dump_critical_message(string pass_name, string level, string message,
                           int number = -1, int user_display = 0,
                           CSageCodeGen *ast = nullptr, void *node = nullptr);

//  To parallelize builds, let's dump a few things in the header:
void GetTLDMInfo_withPointer(void *sg_node, void *pArg);
int io_cfg_gen();
void dep_graph_gen();
int get_map_additionals(CSageCodeGen *codegen, const string &sPort,
                        void *sg_scope, map<string, string> *mapAdditional);
void *get_surrounding_graph_from_task(void *task_bb, CSageCodeGen *codegen,
                                      const string &sTaskName,
                                      const string &graph_name,
                                      const string &additional);

int profiling_top(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &options);
int cpu_ref_add_timer(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options);
int preprocessing(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &options);
int local_array_delinearization(CSageCodeGen *codegen, void *pTopFunc,
                                const CInputOptions &options);
int mars_gen_preproc(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options);
int mars_gen_check_task_interface_format(CSageCodeGen *codegen, void *pTopFunc,
                                         CInputOptions options);
int tldm_extraction_mars_gen(CSageCodeGen *codegen, void *pTopFunc,
                             CInputOptions options);

extern map<void *, vector<int>> mapTask2OrderVec;
extern map<void *, vector<int>> mapGraph2OrderVec;

//  added by Hui
struct GraphOrderLT {
  bool operator()(void *graph1, void *graph2) const {
    size_t i;
    assert(mapGraph2OrderVec.count(graph1) > 0 &&
           mapGraph2OrderVec.count(graph2) > 0);
    vector<int> order_vec1 = mapGraph2OrderVec[graph1];
    vector<int> order_vec2 = mapGraph2OrderVec[graph2];

    //  debug
    if (DEBUG) {
      cerr << "graph 1:";
      for (i = 0; i < order_vec1.size(); i++)
        cerr << order_vec1[i] << "\t";
      cerr << endl;
      cerr << "graph 2:";
      for (i = 0; i < order_vec2.size(); i++)
        cerr << order_vec2[i] << "\t";
      cerr << endl;
    }

    for (i = 0; i < min(order_vec1.size(), order_vec2.size()); i++) {
      if (order_vec1[i] < order_vec2[i])
        return true;
      if (order_vec1[i] > order_vec2[i])
        return false;
    }
    //  cout<<"Error: Graph order vector overlap..."<<endl;
    //  assert(0);
    return false;
  }
};

struct TaskOrderLT {
  bool operator()(void *task1, void *task2) const {
    size_t i;
    assert(mapTask2OrderVec.count(task1) > 0 &&
           mapTask2OrderVec.count(task2) > 0);
    vector<int> order_vec1 = mapTask2OrderVec[task1];
    vector<int> order_vec2 = mapTask2OrderVec[task2];
    for (i = 0; i < min(order_vec1.size(), order_vec2.size()); i++) {
      if (order_vec1[i] < order_vec2[i])
        return true;
      if (order_vec1[i] > order_vec2[i])
        return false;
    }
    //  cout<<"Error: Task order vector overlap..."<<endl;
    //  assert(0);
    return false;
  }
};

extern set<string> dump_array;
extern set<string> graph_names;
extern map<string, string> mapArray2DimStr;
extern map<string, string> mapArray2TypeName;
extern map<void *, set<void *, TaskOrderLT>> execute_seq;
extern set<void *, GraphOrderLT> sorted_execute_seq;
extern vector<string> graph_execute_seq;

extern int g_debug_mode;

#define STR(MSG) #MSG
#define XTR(MSG) STR(MSG)
#define WARNING XTR(WARNING)
static const char *opencl_keyword[] = {
    "__global",     "global",     "__local",      "local",
    "__constant",   "constant",   "__private",    "private",
    "__kernel",     "kernel",     "__read_only",  "read_only",
    "__write_only", "write_only", "__read_write", "read_write"};
static const char *hdl_keyword[] = {"begin",
                                    "end",
                                    "assert",
                                    "bit",
                                    "edge",
                                    "posedge",
                                    "negedge",
                                    "module",
                                    "endmodule",
                                    "macromodule",
                                    "wire",
                                    "reg",
                                    "always",
                                    "assign",
                                    "deassign",
                                    "signed",
                                    "integer",
                                    "in",
                                    "out",
                                    "input",
                                    "output",
                                    "inout",
                                    "pos",
                                    "val",
                                    "value",
                                    "label",
                                    "image",
                                    "automatic",
                                    "buf",
                                    "bufifo",
                                    "bufifi",
                                    "casex",
                                    "casez",
                                    "cell",
                                    "config",
                                    "disable",
                                    "defparam",
                                    "localparam",
                                    "parameter",
                                    "specparm",
                                    "design",
                                    "function",
                                    "endfunction",
                                    "primitive",
                                    "endprimitive",
                                    "specify",
                                    "endspecify",
                                    "table",
                                    "endtable",
                                    "task",
                                    "endtask",
                                    "event",
                                    "forever",
                                    "fork",
                                    "generate",
                                    "genvar",
                                    "highz0",
                                    "highz1",
                                    "ifnone",
                                    "initial",
                                    "instance",
                                    "join",
                                    "fork",
                                    "liblist",
                                    "macromodule",
                                    "nand",
                                    "nor",
                                    "showcancelled",
                                    "noshowcancelled",
                                    "not",
                                    "notif0",
                                    "notif1",
                                    "nmos",
                                    "pmos",
                                    "rcmos",
                                    "cmos",
                                    "rpmos",
                                    "rnmos",
                                    "pull0",
                                    "pull1",
                                    "pullup",
                                    "pulldown",
                                    "real",
                                    "time",
                                    "realtime",
                                    "pulsestyle_ondetect",
                                    "pulsestyle_onevent",
                                    "repeat",
                                    "scalared",
                                    "small",
                                    "large",
                                    "medium",
                                    "strength",
                                    "strong0",
                                    "strong1",
                                    "supply0",
                                    "supply1",
                                    "rtran",
                                    "tran",
                                    "rtranif0",
                                    "rtranif1",
                                    "tranif0",
                                    "tranif1",
                                    "tri",
                                    "tri0",
                                    "tri1",
                                    "triand",
                                    "trior",
                                    "trireg",
                                    "use",
                                    "vectored",
                                    "wait",
                                    "wand",
                                    "weak0",
                                    "weak1",
                                    "wor",
                                    "xnor",
                                    "xnor",
                                    "xor",
                                    "releases"};
static const char *other_keyword[] = {"loop", "function", "kernel",
                                      // math function
                                      "abs",
                                      // math function in matlab
                                      "rem"};
static const char *error_keyword[] = {"init", "check_opencl", "release"};
static const char *internal_name[] = {"opencl_init",
                                      "init",
                                      "init_opencl",
                                      "release",
                                      "release_opencl",
                                      "check_opencl",
                                      "opencl_create_kernel",
                                      "opencl_create_buffer",
                                      "opencl_create_ext_buffer",
                                      "opencl_write_buffer",
                                      "opencl_read_buffer",
                                      "opencl_write_buffer_nb",
                                      "opencl_read_buffer_nb",
                                      "opencl_set_kernel_arg",
                                      "opencl_enqueue_kernel",
                                      "opencl_enqueue_task",
                                      "opencl_flush"};
