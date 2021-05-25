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
using std::unordered_set;
#define REG_SIZE_UPPER_BOUND 64
#define BRAM_UNIT_CAPACITY 2048
#define BRAM_BLOCK_BITWIDTH 36

struct LocalBuffer {
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

// retrieves the partition factors of all dimensions of a given array
void init_partition_factors(CSageCodeGen *m_ast, void *array_decl,
                            LocalBuffer *buffer) {
  // initializes partition factors of all dimensions to 1
  for (int i = 0; i < buffer->num_dimensions; i++) {
    buffer->partition_factors.push_back(1);
  }

  // parses the array_partition pragmas for the input array
  void *stmt = array_decl;
  while ((stmt = m_ast->GetNextStmt(stmt)) != NULL) {
    if (m_ast->IsPragmaDecl(stmt) != 0) {
      string pragma_str = m_ast->GetPragmaString(stmt);
      // used for case-sensitive matches
      string pragma_str_original = pragma_str;
      // transform the pragma string to its lower-case equivalent
      transform(pragma_str.begin(), pragma_str.end(), pragma_str.begin(),
                ::tolower);
      if (pragma_str.find(" array_partition ") != string::npos &&
          pragma_str_original.find(" variable=" + buffer->name) !=
              string::npos) {
        // determines the dimension index
        size_t dim_idx_start_pos =
            pragma_str.rfind(" dim=") + string(" dim=").length();
        int dim_idx = atoi(pragma_str.substr(dim_idx_start_pos).c_str()) - 1;
        // determines the partition factor
        if (pragma_str.find(" complete ") != string::npos) {
          buffer->partition_factors[dim_idx] = buffer->dimension_sizes[dim_idx];
        } else {
          size_t factor_start_pos =
              pragma_str.rfind(" factor=") + string(" factor=").length();
          int factor = atoi(pragma_str.substr(factor_start_pos).c_str());
          buffer->partition_factors[dim_idx] = factor;
        }
      }
    }
  }
}

// checks if a given local buffer is read-only
bool is_buffer_read_only(CSageCodeGen *m_ast, void *array_decl) {
  return m_ast->has_write_in_scope(
             m_ast->GetVariableInitializedName(array_decl),
             m_ast->GetScope(array_decl)) == 0;
}

// initalizes the attributes of a given buffer
LocalBuffer init_buffer(CSageCodeGen *m_ast, void *array_decl) {
  void *array_init_name = m_ast->GetVariableInitializedName(array_decl);
  void *array_type = m_ast->GetTypebyVarRef(array_decl);
  void *array_base_type = m_ast->get_array_base_type(array_init_name);
  LocalBuffer buffer;
  buffer.name = m_ast->GetVariableName(array_decl);
  buffer.bitwidth = m_ast->get_bitwidth_from_type(array_type);
  buffer.num_dimensions = m_ast->get_type_dimension_new(
      array_type, &array_base_type, &buffer.dimension_sizes);
  buffer.capacity = buffer.bitwidth / 8;
  for (auto dim_size : buffer.dimension_sizes) {
    buffer.capacity *= dim_size;
  }
  init_partition_factors(m_ast, array_decl, &buffer);
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

// checks if a given buffer is actually used
bool is_buffer_accessed(CSageCodeGen *m_ast, void *stmt_block, void *val_decl) {
  vector<void *> accesses;
  m_ast->get_ref_in_scope(m_ast->GetVariableInitializedName(val_decl),
                          m_ast->GetScope(val_decl), &accesses);
  return !accesses.empty();
}

// collects the info of all defined and used local buffers
vector<LocalBuffer> collect_buffers(CSageCodeGen *m_ast, void *stmt_block) {
  vector<LocalBuffer> buffers;
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
      buffers.push_back(init_buffer(m_ast, var_decl));
    }
  }

  return buffers;
}

int cal_mux_lut(int width) {
  int lut_usage = 0;
  while (width > 1) {
    lut_usage += ceil(width / 4.0);
    width = ceil(width / 4.0);
  }
  return lut_usage;
}

// calculates the BRAM block usage of a given local buffer
int analyze_buffer(LocalBuffer *buffer) {
  // Step 1: calculate the size of each single partition of the buffer, if
  // the size is too small, then it will be synthesized into FFs instead of BRAM
  // blocks
  int num_partitions = 1;
  for (auto factor : buffer->partition_factors) {
    num_partitions *= factor;
  }
  int unit_size = buffer->capacity / num_partitions;

  // Step 2: calculate the BRAM block usage of a single buffer definition, the
  // buffer could have been replicated through loop unrolling

  // the bitwidth of the buffer determines its unit bram usage
  int unit_bram_usage =
      (buffer->bitwidth + BRAM_BLOCK_BITWIDTH - 1) / BRAM_BLOCK_BITWIDTH;
  int unit_bram_capacity = unit_bram_usage * BRAM_UNIT_CAPACITY;
  int num_units = (unit_size + unit_bram_capacity - 1) / unit_bram_capacity;
  int aligned_partition_usage = unit_bram_usage * num_units;
  // for every odd number that is larger than 1, align it to its closest even
  // number
  if (aligned_partition_usage > 1 && aligned_partition_usage % 2 != 0) {
    aligned_partition_usage++;
  }
  int buffer_bram_usage = aligned_partition_usage * num_partitions;

  // Step 3: calculate the overall BRAM block usage, in which for read-only
  // buffers with small sizes (less than half of a BRAM block), Xilinx will
  // arrange two replica into one block
  int effective_parallel_factor = buffer->parallel_factor;
  if (buffer->read_only && unit_size <= BRAM_UNIT_CAPACITY / 2 &&
      buffer->parallel_factor >= 2) {
    effective_parallel_factor = (effective_parallel_factor + 1) / 2;
  }
  int overall_bram_usage = buffer_bram_usage * effective_parallel_factor;

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

// calculates the BRAM block usage of the kernel interface
size_t analyze_interface(CSageCodeGen *m_ast, void *pTopFunc) {
  vector<void *> pragmas;
  m_ast->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration", &pragmas);
  size_t bram_usage = 0;
  unordered_set<int> int_bundles;
  unordered_set<int> float_bundles;
  int max_width = -1;
  for (auto pragma : pragmas) {
    string pragma_str = m_ast->GetPragmaString(pragma);
    // transform the pragma string to its lower-case equivalent
    transform(pragma_str.begin(), pragma_str.end(), pragma_str.begin(),
              ::tolower);
    // locates the first "HLS INTERFACE" pragma, and then traces back to the
    // top-level function
    if (pragma_str.find("hls interface m_axi") != string::npos) {
      void *func_decl = m_ast->TraceUpToFuncDecl(pragma);
      vector<void *> func_params = m_ast->GetFuncParams(func_decl);
      // traverses all top-level function parameters, and analyzes all
      // non-scalar ones; parameters with the same bitwidth will share the same
      // AXI interface, and vice versa
      for (auto param : func_params) {
        void *param_type = m_ast->GetTypebyVar(param);
        if (m_ast->IsScalarType(param_type) == 0) {
          int param_width = m_ast->get_bitwidth_from_type(param_type);
          void *base_type = m_ast->GetBaseType(param_type, true);
          if (m_ast->IsFloatType(base_type) != 0) {
            float_bundles.insert(param_width);
          } else {
            int_bundles.insert(param_width);
          }
          max_width = max(max_width, param_width);
        }
      }
      size_t num_bundles = int_bundles.size() + float_bundles.size();
      bram_usage =
          ((max_width + BRAM_BLOCK_BITWIDTH - 1) / BRAM_BLOCK_BITWIDTH) * 2 *
          num_bundles;
      break;
    }
  }
  cout << "    All interfaces use " << bram_usage << " BRAM blocks." << endl;
  return bram_usage;
}

// calculates the overall BRAM block usage by examining all buffers and the
// interface
size_t calculate_bram_usage(
    CSageCodeGen *m_ast, void *pTopFunc,
    const unordered_map<string, vector<LocalBuffer>> &func_to_buffers) {
  size_t bram_usage = 0;
  std::ofstream lut_out("lut_memctrl.json");
  lut_out << "{\n";

  bool first_ele = true;
  for (auto func : func_to_buffers) {
    int lut_usage = 0;
    for (auto buffer : func.second) {
      bram_usage += analyze_buffer(&buffer);
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

  bram_usage += analyze_interface(m_ast, pTopFunc);

  return bram_usage;
}

// for log debug
void display_buffers(
    const unordered_map<string, vector<LocalBuffer>> &func_to_buffers) {
  cout << "Begin printing local buffers:" << endl;
  for (auto iter = func_to_buffers.begin(); iter != func_to_buffers.end();
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

// for log debug
void display_weights(const unordered_map<string, unordered_map<string, int>>
                         &weighted_callgraph) {
  cout << "Begin printing weights:" << endl;
  for (auto iter = weighted_callgraph.begin(); iter != weighted_callgraph.end();
       iter++) {
    if (!iter->second.empty()) {
      cout << "[" << iter->first << "]" << endl;
      for (auto weight : iter->second) {
        cout << "    (" << weight.first << ", " << weight.second << ")" << endl;
      }
    }
  }
  cout << "End printing weights." << endl;
}

// topological sorts the weighted call graph to get the parallel factor of each
// buffer
void update_parallel_factors(
    unordered_map<string, vector<LocalBuffer>> *func_to_buffers,
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

// evaluate the usage of BRAM blocks
size_t bram_eval(CSageCodeGen *m_ast, void *pTopFunc) {
  // the evaluation of BRAM usage contains two steps

  // Step #1: traverse the AST, collect all the local buffers
  unordered_map<string, vector<LocalBuffer>> func_to_buffers;
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
      func_to_buffers[func_name] = collect_buffers(m_ast, func_body);
      weighted_callgraph[func_name] =
          createWeightedCallgraphForFunction(m_ast, func_body);
    }
  }
  update_parallel_factors(&func_to_buffers, &weighted_callgraph);
  // for log debug
  display_buffers(func_to_buffers);

  // Step #2: calculate the usage of BRAM blocks
  size_t bram_usage = calculate_bram_usage(m_ast, pTopFunc, func_to_buffers);

  return bram_usage;
}

// evaluate the usage of DSPs
size_t dsp_eval(CSageCodeGen *m_ast, void *pTopFunc) {
  // FIXME: TBA
  return -1;
}

// evaluate the usage of LUTs
size_t lut_eval(CSageCodeGen *m_ast, void *pTopFunc) {
  // FIXME: TBA
  return -1;
}

// evaluate the usage of FFs
size_t ff_eval(CSageCodeGen *m_ast, void *pTopFunc) {
  // FIXME: TBA
  return -1;
}

int ds_resource_eval_top_xilinx(CSageCodeGen *m_ast, void *pTopFunc) {
  size_t bram_usage = bram_eval(m_ast, pTopFunc);
  size_t dsp_usage = dsp_eval(m_ast, pTopFunc);
  size_t lut_usage = lut_eval(m_ast, pTopFunc);
  size_t ff_usage = ff_eval(m_ast, pTopFunc);

  cout << "BRAM usage is " << bram_usage << endl;

  std::ofstream output_file("usage.txt");
  output_file << bram_usage << endl;
  output_file << dsp_usage << endl;
  output_file << lut_usage << endl;
  output_file << ff_usage << endl;

  return 1;
}
