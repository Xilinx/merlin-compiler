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


#include <time.h>
#include <sys/time.h>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

#include "cmdline_parser.h"
#include "file_parser.h"

#include "mars_opt.h"
#include "PolyModel.h"
#include "tldm_annotate.h"
#include "codegen.h"
#include "program_analysis.h"

#include "mars_ir.h"
#include "dse_utils.h"
using std::queue;
using std::unordered_map;
#define REG_SIZE_UPPER_BOUND 64

#define INTEL_BRAM_UNIT_CAPACITY 512
#define INTEL_BRAM_BLOCK_BITWIDTH 40
#define INTEL_BRAM_BLOCK_BITWIDTH_GEN 32
#define INTERFACE_WRITE_BRAM_COUNT 43
#define INTERFACE_READ_BRAM_COUNT 16
#define INTERFACE_MIN_LOOP_DEPTH 1
#define MAX_UNROLL_FACTOR_LIMIT 32
#define MIN_TRIP_COUNT_LIMIT 512

struct Buffer {
  void *buf_decl;
  string name;
  int capacity;
  int bitwidth;
  int num_dimensions;
  vector<size_t> dimension_sizes;
  vector<int> partition_factors;
  int parallel_factor;      // the para factor of the entire kernel
  int in_func_para_factor;  // the para factor in its function
  bool read_only;
  int lut_usage;
  int num_ports;
};

extern bool is_buffer_read_only(CSageCodeGen *m_ast, void *array_decl);
extern bool is_buffer_accessed(CSageCodeGen *m_ast, void *stmt_block,
                               void *val_decl);
extern int cal_mux_lut(int width);
extern void
display_weights(const unordered_map<string, unordered_map<string, int>>
                    &weighted_callgraph);

// initalizes the attributes of a given buffer
Buffer init_buffer_intel(CSageCodeGen *m_ast, void *array_decl) {
  void *array_init_name = m_ast->GetVariableInitializedName(array_decl);
  void *array_type = m_ast->GetTypebyVarRef(array_decl);
  void *array_base_type = m_ast->get_array_base_type(array_init_name);
  Buffer buffer;
  buffer.buf_decl = array_decl;
  buffer.name = m_ast->GetVariableName(array_decl);
  buffer.bitwidth = m_ast->get_bitwidth_from_type(array_type);
  buffer.num_dimensions = m_ast->get_type_dimension_new(
      array_type, &array_base_type, &buffer.dimension_sizes);
  buffer.capacity = buffer.bitwidth / 8;
  for (auto dim_size : buffer.dimension_sizes) {
    buffer.capacity *= dim_size;
  }
  buffer.parallel_factor = 1;
  // FIXME: A buffer will be replicated only if it is wrapped
  // in a function, so in_func_para_factor is always 1
  buffer.in_func_para_factor = 1;
  buffer.lut_usage = 0;
  // FIXME: currently all single-port BRAM blocks
  buffer.num_ports = 1;
  buffer.read_only = is_buffer_read_only(m_ast, array_decl);
  if (buffer.read_only) {
    cout << "Found read only buffer " << buffer.name << endl;
  }
  return buffer;
}
// collects the info of all defined and used local buffers
vector<Buffer> collect_buffers_intel(CSageCodeGen *m_ast, void *stmt_block) {
  vector<Buffer> buffers;
  if (stmt_block == NULL || m_ast->GetChildStmtNum(stmt_block) <= 0) {
    return buffers;
  }

  vector<void *> var_decls;
  m_ast->GetNodesByType(stmt_block, "preorder", "SgVariableDeclaration",
                        &var_decls);
  for (auto var_decl : var_decls) {
    void *var_type = m_ast->GetTypebyVarRef(var_decl);
    if ((m_ast->IsArrayType(var_type) != 0) &&
        is_buffer_accessed(m_ast, stmt_block, var_decl)) {
      buffers.push_back(init_buffer_intel(m_ast, var_decl));
    }
  }

  return buffers;
}
int simple_bram_usage_cnt(CSageCodeGen *m_ast, const Buffer &buffer,
                          unordered_map<string, int> *buf_parallel_factors) {
  int unit_size = buffer.capacity;
  int unit_bram_usage = buffer.bitwidth;

  void *loop = m_ast->TraceUpToForStatement(buffer.buf_decl);
  if (buffer.dimension_sizes.size() == 1 && (loop != nullptr) &&
      unit_bram_usage > INTEL_BRAM_BLOCK_BITWIDTH_GEN) {
    unit_bram_usage = buffer.bitwidth;
  } else if (unit_bram_usage > (INTEL_BRAM_BLOCK_BITWIDTH_GEN >> 1)) {
    unit_bram_usage = buffer.bitwidth;
    unit_bram_usage = unit_bram_usage >> 2;
  }
  if (buffer.dimension_sizes.size() == 1 && (loop == nullptr)) {
    unit_bram_usage = unit_bram_usage >> 1;
  }

  int unit_bram_capacity = unit_bram_usage * unit_size;
  int intel_bram_bit_cnt =
      (INTEL_BRAM_BLOCK_BITWIDTH * INTEL_BRAM_UNIT_CAPACITY);
  if (((*buf_parallel_factors)[buffer.name] > 0) || (loop != nullptr)) {
    intel_bram_bit_cnt =
        (INTEL_BRAM_BLOCK_BITWIDTH_GEN * INTEL_BRAM_UNIT_CAPACITY);
  }
  int64_t overall_bram_usage =
      (unit_bram_capacity + intel_bram_bit_cnt) / intel_bram_bit_cnt;

  if (loop != nullptr) {
    int64_t trip_cnt = 0;
    int64_t CurrentLoopTC = 0;
    m_ast->get_loop_trip_count(loop, &trip_cnt, &CurrentLoopTC);
    if (trip_cnt < (MIN_TRIP_COUNT_LIMIT >> 3)) {
      int64_t rep_factor = (trip_cnt + INTEL_BRAM_BLOCK_BITWIDTH_GEN) /
                           INTEL_BRAM_BLOCK_BITWIDTH_GEN;
      overall_bram_usage = overall_bram_usage * rep_factor;
    }
  }
  return overall_bram_usage;
}

// calculates the BRAM block usage of a given local buffer
int analyze_buffer_intel(CSageCodeGen *m_ast, Buffer *buffer,
                         unordered_map<string, int> *buf_parallel_factors) {
  int overall_bram_usage = 0;
  // Step 1: calculate the size of each single partition of the buffer, if
  // the size is too small, then it will be synthesized into FFs instead of BRAM
  // blocks
  int num_partitions = 1;  // For Intel it is just 1
  int unit_size = buffer->capacity;

  // Step 2: calculate the BRAM block usage of a single buffer definition, the
  // buffer could have been replicated through loop unrolling

  // the bitwidth of the buffer determines its unit bram usage
  int unit_bram_usage = (buffer->bitwidth + INTEL_BRAM_BLOCK_BITWIDTH - 1) /
                        INTEL_BRAM_BLOCK_BITWIDTH;
  int unit_bram_capacity = unit_bram_usage * INTEL_BRAM_UNIT_CAPACITY;
  int num_units = (unit_size + unit_bram_capacity - 1) / unit_bram_capacity;
  int aligned_partition_usage = unit_bram_usage * num_units;
  // for every odd number that is larger than 1, align it to its closest even
  // number
  if (aligned_partition_usage > 1 && aligned_partition_usage % 2 != 0) {
    aligned_partition_usage++;
  }

  // Step 3: calculate the overall BRAM block usage, in which for read-only
  // buffers with small sizes (less than half of a BRAM block), Xilinx will
  // arrange two replica into one block
  int effective_parallel_factor = buffer->parallel_factor;
  if (buffer->read_only && unit_size <= INTEL_BRAM_UNIT_CAPACITY / 2 &&
      buffer->parallel_factor >= 2) {
    effective_parallel_factor = (effective_parallel_factor + 1) / 2;
  }
  overall_bram_usage =
      simple_bram_usage_cnt(m_ast, *buffer, buf_parallel_factors);

  if (/*buf_parallel_factors->count(buffer->name) &&*/
      ((*buf_parallel_factors)[buffer->name] > effective_parallel_factor) &&
      ((*buf_parallel_factors)[buffer->name] < 32) &&
      (buffer->dimension_sizes.size() > 1)) {
    effective_parallel_factor = (*buf_parallel_factors)[buffer->name];
    overall_bram_usage = overall_bram_usage * effective_parallel_factor;
  }

  int addr_width = ceil(log2(unit_size * 8.0 / buffer->bitwidth));
  int ce_width = 1;
  int we_width = 1;
  int64_t individual_lut = 0;
  if (unit_size > REG_SIZE_UPPER_BOUND) {
    individual_lut =
        buffer->num_ports * (addr_width + ce_width + buffer->bitwidth) +
        we_width;
  } else {
    individual_lut = buffer->num_ports * (addr_width + buffer->bitwidth) +
                     ceil(unit_size * 8.0 / 64);
  }
  int mux_lut = buffer->bitwidth * cal_mux_lut(num_partitions);
  int64_t overall_lut =
      (individual_lut * num_partitions + mux_lut) * buffer->in_func_para_factor;

  buffer->lut_usage = overall_lut;

  if (unit_size <= REG_SIZE_UPPER_BOUND) {
    return 0;
  }

  // for log debug
  cout << "    Buffer " << buffer->name << " uses " << overall_bram_usage
       << " BRAM blocks, and " << overall_lut << " LUTs." << endl;
  return overall_bram_usage;
}
void get_interface_var_depth(string pragma_str, string *interface_var,
                             int64_t *depth) {
  vector<string> attribute_set = str_split(pragma_str, " ");
  for (auto attr : attribute_set) {
    if (attr.find("variable") != string::npos) {
      int pos = attr.find('=');
      *interface_var = attr.substr(pos + 1, attr.size());
    }

    if (attr.find("depth") != string::npos) {
      int pos = attr.find('=');
      string token = attr.substr(pos + 1, attr.size());
      *depth = my_atoi(token);
    }
  }
}

int64_t get_interface_var_trip_cnt(CSageCodeGen *m_ast, void *pTopFunc,
                                   void *interface_var) {
  int64_t cnt = 0;
  void *blk = m_ast->GetFuncBody(pTopFunc);
  vector<void *> vecLoops;
  m_ast->GetNodesByType(blk, "preorder", "SgForStatement", &vecLoops);
  for (auto loop : vecLoops) {
    int64_t trip_cnt = 0;
    int64_t CurrentLoopTC = 0;
    m_ast->get_loop_trip_count(loop, &trip_cnt, &CurrentLoopTC);
    void *loop_body = m_ast->GetLoopBody(loop);
    vector<void *> lowerLoops;
    m_ast->GetNodesByType(loop_body, "preorder", "SgForStatement", &lowerLoops);
    for (auto lowerloop : lowerLoops) {
      void *lower_loop_body = m_ast->GetLoopBody(lowerloop);
      bool var_accessed =
          is_buffer_accessed(m_ast, lower_loop_body, interface_var);
      if (var_accessed) {
        int64_t lower_trip_cnt = 0;
        CurrentLoopTC = 0;
        m_ast->get_loop_trip_count(lowerloop, &lower_trip_cnt, &CurrentLoopTC);
        if (trip_cnt == lower_trip_cnt) {
          continue;
        }
        cnt = trip_cnt * lower_trip_cnt;
        return cnt;
      }
    }
  }
  return 0;
}

int get_nested_loops_depth(CSageCodeGen *m_ast, void *pTopFunc, void *sg_loop) {
  int depth = 0;
  void *curr_stmt = sg_loop;
  while (m_ast->IsForStatement(curr_stmt) != 0) {
    depth++;
    curr_stmt = m_ast->GetParent(curr_stmt);
    while (m_ast->IsForStatement(curr_stmt) == 0) {
      curr_stmt = m_ast->GetParent(curr_stmt);
      if (curr_stmt == pTopFunc) {
        break;
      }
    }
  }
  return depth;
}

int get_loop_depth(CSageCodeGen *m_ast, void *pTopFunc, void *interface_var) {
  void *blk = m_ast->GetFuncBody(pTopFunc);
  vector<void *> vecLoops;
  set<void *> loopset;
  m_ast->GetNodesByType(pTopFunc, "preorder", "SgForStatement", &vecLoops,
                        false, true);
  int ref_cnt = 0;
  for (size_t i = 0; i < vecLoops.size(); i++) {
    void *loop = vecLoops[i];
    loopset.insert(loop);
    void *loop_body = m_ast->GetLoopBody(loop);
    int cnt = get_nested_loops_depth(m_ast, pTopFunc, loop);

    vector<void *> func_calls;
    m_ast->GetNodesByType(blk, "preorder", "SgFunctionCallExp", &func_calls);
    for (auto func_call : func_calls) {
      void *func_def = m_ast->GetFuncDefinitionByDecl(func_call);
      void *func_decl = m_ast->GetFuncDeclByCall(func_call);
      vector<void *> func_params = m_ast->GetFuncParams(func_def);

      string func_name = m_ast->GetFuncNameByCall(func_call);
      if (func_def == nullptr) {
        continue;
      }
      bool var_access_in_func =
          is_buffer_accessed(m_ast, func_def, interface_var);
      int index = m_ast->GetFuncParamIndex(interface_var);
      void *sg_var = m_ast->GetFuncParam(func_decl, index);
      if (sg_var == nullptr) {
        continue;
      }
      bool var_access_in_loop = is_buffer_accessed(m_ast, loop_body, sg_var);
      if (var_access_in_func && var_access_in_loop) {
        cnt += get_nested_loops_depth(m_ast, func_def, loop);
      }
    }

    if (cnt > ref_cnt) {
      ref_cnt = cnt;
    }
  }
  return ref_cnt;
}
// calculates the BRAM block usage of the kernel interface
int analyze_interface_intel(CSageCodeGen *m_ast, void *pTopFunc) {
  vector<void *> pragmas;
  m_ast->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration", &pragmas);
  int bram_usage = 0;
  for (auto pragma : pragmas) {
    string pragma_str = m_ast->GetPragmaString(pragma);
    // transform the pragma string to its lower-case equivalent
    transform(pragma_str.begin(), pragma_str.end(), pragma_str.begin(),
              ::tolower);
    int pos = pragma_str.find("interface variable");
    if (pos != string::npos) {
      string sig_name;
      int64_t depth;
      get_interface_var_depth(pragma_str, &sig_name, &depth);
      void *func_decl = m_ast->TraceUpToFuncDecl(pragma);
      vector<void *> func_params = m_ast->GetFuncParams(func_decl);
      for (auto param : func_params) {
        SgInitializedName *param_name =
            isSgInitializedName(static_cast<SgNode *>((param)));
        string param_str = param_name->get_name();
        transform(param_str.begin(), param_str.end(), param_str.begin(),
                  ::tolower);
        if (sig_name != param_str) {
          continue;
        }
        void *param_type = m_ast->GetTypebyVar(param);
        if (m_ast->IsScalarType(param_type) == 0) {
          void *func_body = m_ast->GetFuncBody(func_decl);
          int write_in_scope = m_ast->has_write_in_scope(
              isSgInitializedName(static_cast<SgNode *>(param)), func_body);
          int read_in_scope = m_ast->has_read_in_scope(
              isSgInitializedName(static_cast<SgNode *>(param)), func_body);
          int loop_depth =
              get_loop_depth(m_ast, pTopFunc, static_cast<SgNode *>(param));
          if (loop_depth > INTERFACE_MIN_LOOP_DEPTH) {
            loop_depth = loop_depth >> INTERFACE_MIN_LOOP_DEPTH;
          }
          int orig_depth = loop_depth;
          if (write_in_scope != 0) {
            if (loop_depth > INTERFACE_MIN_LOOP_DEPTH) {
              loop_depth -= INTERFACE_MIN_LOOP_DEPTH;
            }
            bram_usage += INTERFACE_WRITE_BRAM_COUNT * loop_depth;
          }
          loop_depth = orig_depth;
          if (read_in_scope != 0) {
            bram_usage += INTERFACE_READ_BRAM_COUNT * loop_depth;
          }
        }
      }
    }
  }
  cout << "    All interfaces use " << bram_usage << " BRAM blocks." << endl;
  return bram_usage;
}

// calculates the overall BRAM block usage by examining all buffers and the
// interface
int calculate_bram_usage_intel(
    CSageCodeGen *m_ast, void *pTopFunc,
    const unordered_map<string, vector<Buffer>> &func_to_buffers,
    unordered_map<string, int> *buf_parallel_factors) {
  int bram_usage = 0;
  std::ofstream lut_out("lut_memctrl.json");
  lut_out << "{\n";

  bool first_ele = true;
  for (auto func : func_to_buffers) {
    int lut_usage = 0;
    for (auto buffer : func.second) {
      bram_usage += analyze_buffer_intel(m_ast, &buffer, buf_parallel_factors);
      lut_usage += buffer.lut_usage;
    }
    if (first_ele) {
      lut_out << "   \"" << func.first << "\": " << lut_usage;
      first_ele = false;
    } else {
      lut_out << ",\n   \"" << func.first << "\": " << lut_usage;
    }
  }
  lut_out << "\n}";

  bram_usage += analyze_interface_intel(m_ast, pTopFunc);

  return bram_usage;
}
// for log debug
void display_buffers(
    const unordered_map<string, vector<Buffer>> &func_to_buffers) {
  cout << "Begin printing local buffers:" << endl;
  for (auto iter = func_to_buffers.cbegin(); iter != func_to_buffers.cend();
       iter++) {
    if (!iter->second.empty()) {
      cout << "[" << iter->first << "]" << endl;
      for (auto buffer : iter->second) {
        cout << "    " << buffer.name << " (C: " << buffer.capacity
             << ", B: " << buffer.bitwidth << ", P: " << buffer.parallel_factor
             << ", ";
        for (auto dim_size : buffer.dimension_sizes) {
          cout << "[" << dim_size << "]";
        }
        cout << ", ";
        for (auto factor : buffer.partition_factors) {
          cout << "{" << factor << "}";
        }
        cout << ")" << endl;
      }
    }
  }
  cout << "End printing local buffers." << endl;
}

// topological sorts the weighted call graph to get the parallel factor of each
// buffer
void update_parallel_factors_intel(
    unordered_map<string, vector<Buffer>> *func_to_buffers,
    unordered_map<string, unordered_map<string, int>> *weighted_callgraph) {
  unordered_map<string, unordered_map<string, int>> indegree_callgraph;
  // a weighted graph needs an attribute for each vertex to track the maximum
  // propagated weights from all paths
  unordered_map<string, int> propagated_weights;
  for (auto vertex : *weighted_callgraph) {
    indegree_callgraph[vertex.first] = unordered_map<string, int>();
    propagated_weights[vertex.first] = 1;
  }
  for (auto vertex : *weighted_callgraph) {
    for (auto edge : vertex.second) {
      indegree_callgraph[edge.first].insert({vertex.first, edge.second});
    }
  }
  queue<pair<string, int>> vertex_queue;
  for (auto vertex : indegree_callgraph) {
    if (vertex.second.empty()) {
      vertex_queue.push({vertex.first, propagated_weights[vertex.first]});
    }
  }
  while (!vertex_queue.empty()) {
    pair<string, int> cur_vertex = vertex_queue.front();
    vertex_queue.pop();
    if ((func_to_buffers->count(cur_vertex.first) != 0U) &&
        !(*func_to_buffers)[cur_vertex.first].empty()) {
      for (auto iter = (*func_to_buffers)[cur_vertex.first].begin();
           iter != (*func_to_buffers)[cur_vertex.first].end(); iter++) {
        iter->parallel_factor = cur_vertex.second;
      }
    }
    for (auto edge : (*weighted_callgraph)[cur_vertex.first]) {
      propagated_weights[edge.first] =
          max(propagated_weights[edge.first], edge.second * cur_vertex.second);
      indegree_callgraph[edge.first].erase(cur_vertex.first);
      if (indegree_callgraph[edge.first].empty()) {
        vertex_queue.push({edge.first, propagated_weights[edge.first]});
      }
    }
  }
}

void collect_unrollfactor_on_loops(
    CSageCodeGen *m_ast, void *pTopFunc,
    const unordered_map<string, vector<Buffer>> &func_to_buffers,
    unordered_map<string, int> *buf_parallel_factors) {
  void *blk = m_ast->GetFuncBody(pTopFunc);
  vector<void *> vecLoops;
  m_ast->GetNodesByType(blk, "preorder", "SgForStatement", &vecLoops);
  for (auto loop : vecLoops) {
    int64_t factor = 0;
    bool is_unroll = GetUnrollFactorFromPragma(m_ast, loop, &factor);
    int64_t trip_count = 0;
    int64_t CurrentLoopTC = 0;
    m_ast->get_loop_trip_count(loop, &trip_count, &CurrentLoopTC);
    if (factor != trip_count && factor < MAX_UNROLL_FACTOR_LIMIT) {
      continue;
    }
    if (is_unroll) {
      void *loop_body = m_ast->GetLoopBody(loop);
      for (auto iter = func_to_buffers.begin(); iter != func_to_buffers.end();
           iter++) {
        if (!iter->second.empty()) {
          for (auto buffer : iter->second) {
            int write_access = m_ast->has_write_in_scope(
                m_ast->GetVariableInitializedName(buffer.buf_decl), loop_body);
            int read_access = m_ast->has_read_in_scope(
                m_ast->GetVariableInitializedName(buffer.buf_decl), loop_body);
            if ((write_access != 0) || (read_access != 0)) {
              if (factor > 0 && (*buf_parallel_factors)[buffer.name] < factor) {
                void *parent_loop = m_ast->GetEnclosingNode("ForLoop", loop);
                int64_t p_factor = 0;
                bool is_unroll_p = false;
                if (parent_loop != nullptr) {
                  is_unroll_p =
                      GetUnrollFactorFromPragma(m_ast, parent_loop, &p_factor);
                }
                if (is_unroll_p && buffer.dimension_sizes.size() > 2) {
                  m_ast->get_loop_trip_count(parent_loop, &trip_count,
                                             &CurrentLoopTC);
                  if (trip_count > MIN_TRIP_COUNT_LIMIT) {
                    factor = trip_count / MIN_TRIP_COUNT_LIMIT;
                  }
                  (*buf_parallel_factors)[buffer.name] = factor;
                } else {
                  (*buf_parallel_factors)[buffer.name] = factor;
                }
              }
            }
          }
        }
      }
    }
  }
}

// evaluate the usage of BRAM blocks
int bram_eval_intel(CSageCodeGen *m_ast, void *pTopFunc) {
  int bram_usage = -1;
  unordered_map<string, int> buf_parallel_factors;  // FOR INTEL
  // the evaluation of BRAM usage contains two steps

  // Step #1: traverse the AST, collect all the local buffers
  unordered_map<string, vector<Buffer>> func_to_buffers;
  unordered_map<string, unordered_map<string, int>> weighted_callgraph;
  vector<void *> func_decls;
  m_ast->GetNodesByType(pTopFunc, "preorder", "SgFunctionDeclaration",
                        &func_decls);
  for (auto func_decl : func_decls) {
    // omit built-in functions that have no stmt body since it does not affect
    // BRAM usage
    void *func_body = m_ast->GetFuncBody(func_decl);
    if (func_body == NULL || m_ast->GetChildStmtNum(func_body) == 0) {
      continue;
    }
    string func_name = m_ast->GetFuncName(func_decl);
    if (func_to_buffers.count(func_name) == 0) {
      func_to_buffers[func_name] = collect_buffers_intel(m_ast, func_body);
      weighted_callgraph[func_name] =
          createWeightedCallgraphForFunction(m_ast, func_body);
    }
  }
  // for INTEL
  collect_unrollfactor_on_loops(m_ast, pTopFunc, func_to_buffers,
                                &buf_parallel_factors);
  update_parallel_factors_intel(&func_to_buffers, &weighted_callgraph);
  // for log debug
  display_buffers(func_to_buffers);

  // Step #2: calculate the usage of BRAM blocks
  bram_usage = calculate_bram_usage_intel(m_ast, pTopFunc, func_to_buffers,
                                          &buf_parallel_factors);

  return bram_usage;
}

// evaluate the usage of DSPs
int dsp_eval_intel(CSageCodeGen *m_ast, void *pTopFunc) {
  // FIXME: TBA
  return -1;
}

// evaluate the usage of LUTs
int lut_eval_intel(CSageCodeGen *m_ast, void *pTopFunc) {
  // FIXME: TBA
  return -1;
}

// evaluate the usage of FFs
int ff_eval_intel(CSageCodeGen *m_ast, void *pTopFunc) {
  // FIXME: TBA
  return -1;
}

int ds_resource_eval_top_intel(CSageCodeGen *m_ast, void *pTopFunc) {
  int bram_usage = bram_eval_intel(m_ast, pTopFunc);
  int dsp_usage = dsp_eval_intel(m_ast, pTopFunc);
  int lut_usage = lut_eval_intel(m_ast, pTopFunc);
  int ff_usage = ff_eval_intel(m_ast, pTopFunc);

  cout << "BRAM usage is " << bram_usage << endl;

  std::ofstream output_file("usage.txt");
  output_file << bram_usage << endl;
  output_file << dsp_usage << endl;
  output_file << lut_usage << endl;
  output_file << ff_usage << endl;

  return 1;
}
