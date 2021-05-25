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


#include <iostream>
#include <string>

#include "id_update.h"
#include "mars_message.h"
#include "channel_partition.h"
#include "memory_partition.h"
#include "program_analysis.h"
#include "rose.h"

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using MerlinStreamModel::CStreamBase;
using MerlinStreamModel::CStreamChannel;
using MerlinStreamModel::CStreamIR;
using MerlinStreamModel::CStreamNode;
using MerlinStreamModel::CStreamPort;

using std::list;
using std::map;
using std::set;
using std::vector;

int channel_partition_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  if ("aocl" == options.get_option_key_value("-a", "impl_tool")) {
    return channel_partition_intel_top(codegen, pTopFunc, options);
  }
  return 0;
}

int channel_partition_intel_top(CSageCodeGen *codegen, void *pTopFunc,
                                const CInputOptions &options) {
  std::cout << "==============================================" << std::endl;
  std::cout << "-----=# Channel Partition Optimization Start#=----\n";
  std::cout << "==============================================" << std::endl;

  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(codegen, pTopFunc);
  CStreamIR stream_ir(codegen, options, &mars_ir, &mars_ir_v2);
  stream_ir.ParseStreamModel();

  if (!options.get_option_key_value("-a", "naive_hls").empty()) {
    cout << "[MARS-PARALLEL-MSG] Naive HLS mode.\n";
    return 0;
  }

  partition_analysis_intel(codegen, &mars_ir, &stream_ir);
  partition_transform_intel(codegen, &mars_ir, &stream_ir);
  return 1;
}

void partition_analysis_intel(CSageCodeGen *codegen, CMarsIr *mars_ir,
                              CStreamIR *stream_ir) {
  map<void *, map<int, int>> p_channels;
  map<void *, map<int, int>> p_buffers;

  // 1. Channel partition analysis
  vector<CStreamChannel *> m_fifos = stream_ir->get_fifos();
  for (auto fifo : m_fifos) {
    void *var_decl = fifo->get_ref();
    cout << "fifo: " << codegen->_p(var_decl) << endl;
    map<int, int> m_factors;
    int ret = static_cast<int>(
        partition_analysis_var(codegen, var_decl, mars_ir, &m_factors));
    if ((ret != 0) && (!m_factors.empty() != 0u)) {
      p_channels[var_decl] = m_factors;
    }
  }

  // 2. Insert internal partition pragma
  insert_internal_pragma(codegen, p_channels, p_buffers);
}

bool partition_analysis_var(CSageCodeGen *codegen, void *var_decl,
                            CMarsIr *mars_ir, map<int, int> *m_factors) {
  vector<void *> vec_tmp;
  codegen->GetNodesByType_compatible(codegen->GetProject(), "SgVarRef",
                                     &vec_tmp);

  void *target_init = codegen->GetVariableInitializedName(var_decl);
  void *base_type;
  vector<size_t> arr_size;  // The size is stored reversely
  codegen->get_type_dimension(codegen->GetTypebyVar(target_init), &base_type,
                              &arr_size);

  for (size_t i = 0; i < vec_tmp.size(); i++) {
    void *ref = vec_tmp[i];
    void *var_init = codegen->GetVariableInitializedName(ref);

    if (var_init == target_init) {
      vector<void *> loop_tmp;
      codegen->get_loop_nest_in_scope(ref, codegen->GetProject(), &loop_tmp);
      int tmp_dim = 0;
      for (size_t j = 0; j < loop_tmp.size(); j++) {
        void *loop_body = codegen->GetLoopBody(loop_tmp[j]);
        CMirNode *node = mars_ir->get_node(loop_body);

        if (!node->is_function && node->is_complete_unroll()) {
          int ret = static_cast<int>(
              analyze_channel(codegen, ref, loop_tmp[j], m_factors, tmp_dim));
          if (ret == 0) {
            return false;
          }
          tmp_dim++;
        }
      }
    }
  }

  // If the user has partitioned the channel
  if (arr_size.size() > 0 && m_factors->size() > arr_size.size()) {
    return false;
  }

  return true;
}

bool analyze_channel(CSageCodeGen *codegen, void *sg_ref, void *sg_loop,
                     map<int, int> *m_factors, int tmp_dim) {
  int64_t factor = 1;
  int dim = m_factors->size();
  int ret =
      static_cast<int>(codegen->get_loop_trip_count_simple(sg_loop, &factor));

  if (ret == 0) {
    cout << "Cannot get the range of the loop for complete unroll\n";
    return false;
  }
  if (factor <= 1) {
    return true;
  }

  if (tmp_dim >= dim) {
    (*m_factors)[tmp_dim] = factor;
  } else {
    if ((*m_factors)[tmp_dim] != factor) {
      cout << "Channel partition conflict\n";
      return false;
    }
  }
  return true;
}

void insert_internal_pragma(CSageCodeGen *codegen,
                            map<void *, map<int, int>> p_channels,
                            map<void *, map<int, int>> p_buffers) {
  // 1. Insert internal pragma for channels
  for (auto it : p_channels) {
    void *var_decl = it.first;
    void *var_init = codegen->GetVariableInitializedName(var_decl);
    for (auto itt : it.second) {
      string pragma_str = std::string(ACCEL_PRAGMA) + " " +
                          CMOST_ARR_PARTITION_low + " type=channel " +
                          CMOST_variable_low + "=" + codegen->_up(var_init) +
                          " " + CMOST_dim_low + "=" + my_itoa(itt.first + 1) +
                          " " + CMOST_factor_low + "=" + my_itoa(itt.second);
      void *new_pragma =
          codegen->CreatePragma(pragma_str, codegen->GetProject());
      codegen->InsertAfterStmt(new_pragma, var_decl);
      cout << "Insert internal partition pragma for " << codegen->_up(var_decl)
           << endl;
    }
  }

  // TODO(youxiang):
  // 2. Insert internal pragma for on-chip buffers
}

void partition_transform_intel(CSageCodeGen *codegen, CMarsIr *mars_ir,
                               CStreamIR *stream_ir) {
  map<void *, map<int, int>> p_channels;
  vector<void *> ch_pragma;
  vector<void *> user_channels;

  // 1. Parse the pragmas
  vector<void *> vec_pragma;
  codegen->GetNodesByType_compatible(codegen->GetProject(),
                                     "SgPragmaDeclaration", &vec_pragma);

  for (size_t i = 0; i < vec_pragma.size(); i++) {
    void *pragma_decl = vec_pragma[i];
    parse_internal_pragma(codegen, pragma_decl, mars_ir, stream_ir, &p_channels,
                          &ch_pragma);
  }

  transform_channel(codegen, mars_ir, stream_ir, p_channels);

  delete_internal_pragma(codegen, ch_pragma);
}

bool parse_internal_pragma(CSageCodeGen *codegen, void *sg_pragma,
                           CMarsIr *mars_ir, CStreamIR *stream_ir,
                           map<void *, map<int, int>> *p_channels,
                           vector<void *> *ch_pragma) {
  SgPragma *pragma =
      (static_cast<SgPragmaDeclaration *>(sg_pragma)->get_pragma());
  string pragmaString = pragma->get_pragma();
  vector<string> constructs;

  // 1. Channel partition analysis
  vector<CStreamChannel *> m_fifos = stream_ir->get_fifos();

  // Notes: parsing ACCEL array_partition pragma
  mars_ir->splitString(pragmaString, " ", &constructs);
  bool hls_id = false;
  bool opt_id = false;
  bool channel_id = false;

  for (auto itt : constructs) {
    string subPragma_0 = itt;
    if (subPragma_0 == ACCEL_PRAGMA) {
      hls_id = true;
    } else if (hls_id && subPragma_0 == CMOST_ARR_PARTITION_low) {
      opt_id = true;
    } else if (hls_id && opt_id &&
               subPragma_0.find(CMOST_type_low) != string::npos) {
      vector<string> constructs_0;
      subPragma_0 = itt;
      mars_ir->splitString(subPragma_0, "=", &constructs_0);
      bool found = false;
      for (auto it : constructs_0) {
        string subPragma = it;
        if (subPragma == CMOST_type_low && !found) {
          found = true;
        } else if (subPragma == "channel" && found) {
          channel_id = true;
          ch_pragma->push_back(sg_pragma);
        }
      }
    } else if (hls_id && opt_id && channel_id &&
               subPragma_0.find(CMOST_variable_low) != string::npos) {
      vector<string> constructs_0;
      subPragma_0 = itt;
      mars_ir->splitString(subPragma_0, "=", &constructs_0);
      bool found = false;
      for (auto it : constructs_0) {
        string subPragma = it;
        if (subPragma == CMOST_variable_low && !found) {
          found = true;
        } else if (found) {
          for (auto fifo : m_fifos) {
            void *var_decl = fifo->get_ref();
            void *var_init = codegen->GetVariableInitializedName(var_decl);
            if (codegen->_up(var_init) == subPragma) {
              map<int, int> hls_factors;
              int ret = factor_extract(codegen, mars_ir, sg_pragma,
                                       &hls_factors, var_init);
              assert(ret != 0);
              if (p_channels->find(var_decl) == p_channels->end()) {
                (*p_channels)[var_decl] = hls_factors;
              } else {
                for (auto pp : hls_factors) {
                  if ((*p_channels)[var_decl].find(pp.first) ==
                      (*p_channels)[var_decl].end()) {
                    (*p_channels)[var_decl][pp.first] = pp.second;
                  } else {
                    if ((*p_channels)[var_decl][pp.first] != pp.second) {
                      return false;
                    }
                  }
                }
              }
              return true;
            }
          }
        }
      }
    }
  }
  return true;
}

bool transform_channel(CSageCodeGen *codegen, CMarsIr *mars_ir,
                       CStreamIR *stream_ir,
                       const map<void *, map<int, int>> &p_channels) {
  map<CStreamBase *, set<int>> new_attribute;
  vector<void *> tmp_channels;
  for (auto it : p_channels) {
    size_t max_path = 0;
    map<void *, vector<void *>> parallel_path;
    void *orig_decl = it.first;
    void *orig_init = codegen->GetVariableInitializedName(orig_decl);
    void *sg_base_type = codegen->GetTypebyVar(orig_init);
    string old_var_name = codegen->_up(orig_init);
    string new_var_name = old_var_name + "_p";
    map<int, int> hls_factors = it.second;
    map<void *, void *> new_ref2pntr;

    void *base_type;
    vector<size_t> arr_size;  // The size is stored reversely
    codegen->get_type_dimension(sg_base_type, &base_type, &arr_size);
    // Note: if the user has partitioned the channel
    bool user_channel = false;
    if (hls_factors.size() == arr_size.size()) {
      cout << "Not necessarily for partitioning\n";
      user_channel = true;
    }

    // 1. Build new declaration

    vector<size_t> dims;
    for (auto itt : hls_factors) {
      dims.push_back(itt.second);
    }
    void *new_var_init = nullptr;
    if (!user_channel) {
      void *new_array_type = codegen->CreateArrayType(sg_base_type, dims);
      void *new_var_decl = codegen->CreateVariableDecl(
          new_array_type, new_var_name, nullptr, codegen->GetProject());
      codegen->InsertStmt(new_var_decl, orig_decl);
      tmp_channels.push_back(orig_decl);
      new_var_init = codegen->GetVariableInitializedName(new_var_decl);
    }
    // 2. Replace the references
    vector<void *> vec_tmp;
    codegen->GetNodesByType_compatible(codegen->GetProject(), "SgVarRef",
                                       &vec_tmp);

    for (size_t i = 0; i < vec_tmp.size(); i++) {
      size_t local_path = 0;
      void *ref = vec_tmp[i];

      void *var_init = codegen->GetVariableInitializedName(ref);
      vector<void *> loop_paths;

      if (var_init == orig_init) {
        vector<void *> loop_tmp;
        codegen->get_loop_nest_in_scope(ref, codegen->GetProject(), &loop_tmp);

        list<t_func_call_path> vec_paths;
        int ret =
            codegen->get_call_paths(ref, codegen->GetProject(), &vec_paths);
        t_func_call_path func_path;
        // by default, use the first path, use the
        // other function for specific aths
        if ((ret != 0) && !vec_paths.empty()) {
          func_path = *vec_paths.begin();
        }

        vector<void *> vec_new_expr;
        vector<void *> new_args_from_iter;

        for (size_t j = 0; j < loop_tmp.size(); j++) {
          void *sg_loop = loop_tmp[j];
          void *loop_body = codegen->GetLoopBody(sg_loop);
          void *curr_func = codegen->GetEnclosingNode("Function", ref);
          CMirNode *node = mars_ir->get_node(loop_body);

          if (!node->is_function && node->is_complete_unroll()) {
            local_path++;
            loop_paths.push_back(sg_loop);
            void *sg_iter = codegen->GetLoopIterator(sg_loop);

            if (codegen->IsInScope(sg_loop, curr_func) != 0) {
              vec_new_expr.push_back(codegen->CreateVariableRef(sg_iter));
            } else {  // Build cross function iterator and index
              vector<void *> funcs;
              vector<void *> calls;
              void *func_decl = curr_func;
              while (true) {
                void *sg_call = nullptr;
                t_func_call_path::iterator fp;
                for (fp = func_path.begin(); fp != func_path.end(); fp++) {
                  if (fp->first == func_decl) {
                    sg_call = fp->second;
                    funcs.push_back(func_decl);
                    calls.push_back(sg_call);
                    break;
                  }
                }
                if (sg_call == nullptr) {
                  return false;
                }
                if (codegen->IsInScope(sg_call, sg_loop) == 0) {
                  func_decl = codegen->GetEnclosingNode("Function", sg_call);
                } else {
                  break;
                }
              }

              if (!parse_iterator_for_call(codegen, stream_ir, sg_loop,
                                           &vec_new_expr, &new_attribute, funcs,
                                           calls)) {
                return false;
              }
            }
          }
        }

        if (user_channel) {
          check_user_channel(codegen, ref, vec_new_expr);
          continue;
        }
        void *sg_ref = codegen->CreateVariableRef(new_var_init);
        void *new_ref = codegen->CreateArrayRef(sg_ref, vec_new_expr);
        codegen->ReplaceExp(ref, new_ref);
        parallel_path[sg_ref] = loop_paths;
        new_ref2pntr[sg_ref] = new_ref;
        if ((static_cast<SgInitializedName *>(new_var_init))
                ->search_for_symbol_from_symbol_table() == nullptr) {
          cout << "NO SYMBOL!\n";
          return false;
        }
      }
      if (local_path > max_path) {
        max_path = local_path;
      }
    }
    if (user_channel) {
      continue;
    }
    cout << "Partition channel: " << old_var_name << " => "
         << codegen->_up(new_var_init) << endl;

    // 3. Synchronize the parallelized loops for merlin stream reset/init
    // function calls
    vector<void *> max_path_loops;
    for (auto p_it : parallel_path) {
      if (p_it.second.size() == max_path) {
        max_path_loops = p_it.second;
        break;
      }
    }

    vector<void *> stmts_to_remove;
    for (auto p_it : parallel_path) {
      if (codegen->GetEnclosingNode("FunctionCallExp", p_it.first) != nullptr) {
        void *sg_call =
            codegen->GetEnclosingNode("FunctionCallExp", p_it.first);
        void *sg_call_stmt = codegen->GetEnclosingNode("Statement", sg_call);
        string str_name = codegen->GetFuncNameByCall(sg_call);
        vector<void *> vec_new_body;

        void *prev_stmt = sg_call_stmt;
        vec_new_body.push_back(prev_stmt);
        while (codegen->IsLabelStatement(codegen->GetParent(prev_stmt)) != 0) {
          prev_stmt = codegen->GetParent(prev_stmt);
          vec_new_body.push_back(prev_stmt);
        }
        if (str_name.find("merlin_stream_reset") != string::npos ||
            str_name.find("merlin_stream_init") != string::npos) {
          if (p_it.second.size() < max_path) {
            vector<void *> iter_replace;
            bool first = true;
            void *sg_body = codegen->CreateBasicBlock();
            for (size_t i = p_it.second.size(); i < max_path; i++) {
              void *extra_loop = max_path_loops[i];
              void *old_incr = codegen->GetLoopIncrementExpr(extra_loop);
              void *old_iter = codegen->GetLoopIterator(extra_loop);

              void *sg_type = codegen->GetTypebyVar(old_iter);
              string new_var_name = codegen->_up(old_iter);
              codegen->get_valid_local_name(
                  codegen->GetEnclosingNode("Function", p_it.first),
                  &new_var_name);
              void *new_iter_decl = codegen->CreateVariableDecl(
                  sg_type, new_var_name, nullptr, codegen->GetScope(prev_stmt));

              if (first) {
                codegen->InsertStmt(new_iter_decl, prev_stmt);
              } else {
                codegen->AppendChild(sg_body, new_iter_decl);
              }
              void *new_iter =
                  codegen->GetVariableInitializedName(new_iter_decl);
              void *func = codegen->GetEnclosingNode("Function", extra_loop);
              CMarsRangeExpr mr =
                  CMarsVariable(extra_loop, codegen, func).get_range();
              CMarsExpression me_lb;
              CMarsExpression me_ub;
              mr.get_simple_bound(&me_lb, &me_ub);
              void *start_exp = codegen->CopyExp(me_lb.get_expr_from_coeff());
              void *end_exp = codegen->CopyExp(me_ub.get_expr_from_coeff());

              int64_t new_step = 1;
              if (isSgPlusPlusOp(static_cast<SgNode *>(old_incr)) != nullptr) {
                new_step = 1;
              } else if (isSgMinusMinusOp(static_cast<SgNode *>(old_incr)) !=
                         nullptr) {
                new_step = -1;
              } else {
                return false;
              }

              void *sg_body_local = codegen->CreateBasicBlock();
              void *new_for_loop =
                  codegen->CreateStmt(V_SgForStatement, new_iter, start_exp,
                                      end_exp, sg_body_local, &new_step);
              if (first) {
                codegen->InsertStmt(new_for_loop, prev_stmt);
              } else {
                codegen->AppendChild(sg_body, new_for_loop);
              }
              iter_replace.push_back(new_iter);
              void *new_pragma =
                  codegen->CreatePragma("ACCEL parallel", new_for_loop);
              codegen->PrependChild(sg_body_local, new_pragma);
              sg_body = sg_body_local;
              first = false;
            }

            for (auto stmt : vec_new_body) {
              stmts_to_remove.push_back(stmt);
              if (codegen->IsInScope(p_it.first, stmt) != 0) {
                vector<void *> sg_new_indexes;
                void *sg_array = nullptr;
                void *sg_pntr;
                vector<void *> sg_indexes;
                int pointer_dim;
                codegen->parse_pntr_ref_by_array_ref(
                    p_it.first, &sg_array, &sg_pntr, &sg_indexes, &pointer_dim);

                for (size_t jj = 0;
                     jj < sg_indexes.size() - iter_replace.size(); jj++) {
                  sg_new_indexes.push_back(codegen->CopyExp(sg_indexes[jj]));
                }

                for (size_t jj = 0; jj < iter_replace.size(); jj++) {
                  sg_new_indexes.push_back(
                      codegen->CreateVariableRef(iter_replace[jj]));
                }

                void *new_pntr = codegen->CreateArrayRef(
                    codegen->CreateVariableRef(new_var_init), sg_new_indexes);
                codegen->ReplaceExp(new_ref2pntr[p_it.first], new_pntr);
              }
              void *new_stmt = codegen->CopyStmt(stmt);
              codegen->AppendChild(sg_body, new_stmt);
            }
          }
        } else {
          if (p_it.second.size() < max_path) {
            cout << "[error] Parallel not match on " << old_var_name << endl;
            string msg = "Parallel not match on variable '";
            msg += old_var_name + "'\n";
            dump_critical_message("PARTITION", "ERROR", msg, 301);
            exit(-1);
          }
        }
      }
    }
    for (size_t i = 0; i < stmts_to_remove.size(); i++) {
      codegen->RemoveStmt(stmts_to_remove[i]);
    }
  }
  for (size_t i = 0; i < tmp_channels.size(); i++) {
    codegen->RemoveStmt(tmp_channels[i]);
  }

  return true;
}

bool parse_iterator_for_call(CSageCodeGen *codegen, CStreamIR *stream_ir,
                             void *sg_loop, vector<void *> *vec_new_expr,
                             map<CStreamBase *, set<int>> *new_attribute,
                             const vector<void *> &funcs,
                             const vector<void *> &calls) {
  int iter_idx = -1;
  void *sg_iter = codegen->GetLoopIterator(sg_loop);
  void *sg_type = codegen->GetTypebyVar(sg_iter);
  string s_var = codegen->_up(sg_iter);
  void *sg_param = nullptr;
  void *sg_top_call = calls[calls.size() - 1];

  for (int i = static_cast<int>(calls.size()) - 1; i >= 0; i--) {
    void *sg_call = calls[i];
    void *curr_func = funcs[i];
    int param_num = codegen->GetFuncCallParamNum(sg_call);
    int idx = -1;
    cout << codegen->_p(sg_call) << endl;
    cout << codegen->_p(curr_func) << endl;

    // 1. Check if iterator is in the arugment of the function call
    for (int ii = 0; ii < codegen->GetFuncCallParamNum(sg_call); ii++) {
      void *sg_arg = codegen->GetFuncCallParam(sg_call, ii);
      if (codegen->IsVarRefExp(sg_arg) == 0) {
        continue;
      }
      void *arg_init = codegen->GetVariableInitializedName(sg_arg);
      if (arg_init == sg_iter) {
        sg_param = codegen->GetFuncParam(curr_func, ii);
        idx = ii;
      }
    }

    // 2. If iterator is not in the arugment of the function call,
    //  add the iterator in the function parameter list
    if (idx < 0) {
      codegen->get_valid_local_name(curr_func, &s_var);
      void *new_var = codegen->CreateVariable(sg_type, s_var);
      codegen->insert_param_to_decl(curr_func, new_var, true);
      void *new_ref = codegen->CreateVariableRef(sg_iter);
      codegen->insert_argument_to_call(sg_call, new_ref);
      if (i == static_cast<int>(calls.size()) - 1) {
        iter_idx = param_num;
      }
      sg_param = new_var;
    } else {
      if (i == static_cast<int>(calls.size()) - 1) {
        iter_idx = idx;
      }
    }

    sg_iter = sg_param;
  }

  // 2.3 Add attribute for stream node
  if (iter_idx >= 0) {
    vec_new_expr->push_back(codegen->CreateVariableRef(sg_param));

    void *sg_call_stmt = codegen->GetEnclosingNode("Statement", sg_top_call);
    cout << codegen->_p(sg_call_stmt) << endl;
    cout << iter_idx << endl;
    CStreamNode *node = stream_ir->FindNodeByAST(sg_call_stmt);
    CStreamPort *port = stream_ir->FindPortByAST(sg_call_stmt);
    if (node != nullptr) {
      if (new_attribute->find(node) != new_attribute->end()) {
        if ((*new_attribute)[node].find(iter_idx) !=
            (*new_attribute)[node].end()) {
          return true;
        }
      }

      string val_arg = my_itoa(iter_idx);
      if (!node->get_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE).empty()) {
        node->append_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE, val_arg);
      } else {
        node->set_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE, val_arg);
      }
      (*new_attribute)[node].insert(iter_idx);
      node->update_pragma();
    } else if (port != nullptr) {
      if (new_attribute->find(port) != new_attribute->end()) {
        if ((*new_attribute)[port].find(iter_idx) !=
            (*new_attribute)[port].end()) {
          return true;
        }
      }

      string val_arg = my_itoa(iter_idx);
      if (!port->get_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE).empty()) {
        port->append_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE, val_arg);
      } else {
        port->set_attribute(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE, val_arg);
      }
      (*new_attribute)[port].insert(iter_idx);
      port->update_pragma();
    }
    return true;
  }
  cout << "Didnt find the corresponding iterator for call: "
       << codegen->_p(sg_top_call) << endl;
  return false;
}

bool check_user_channel(CSageCodeGen *codegen, void *sg_ref,
                        vector<void *> vec_new_expr) {
  cout << "check user channel: " << codegen->_p(sg_ref) << endl;
  if (vec_new_expr.empty()) {
    return false;
  }
  void *sg_array = nullptr;
  void *sg_pntr;
  vector<void *> sg_indexes;
  int pointer_dim;
  codegen->parse_pntr_ref_by_array_ref(sg_ref, &sg_array, &sg_pntr, &sg_indexes,
                                       &pointer_dim);

  for (size_t i = 0; i < sg_indexes.size(); i++) {
    void *sg_idx = sg_indexes[i];
    void *new_ref = vec_new_expr[i];
    void *target_init = codegen->GetVariableInitializedName(new_ref);
    CMarsExpression index1(sg_idx, codegen, sg_pntr);
    map<void *, CMarsExpression> *m_coeff_1 = index1.get_coeff();
    if (m_coeff_1->size() > 1) {
      return false;
    }
    map<void *, CMarsExpression>::iterator it = m_coeff_1->begin();
    // Caution!!!
    if (codegen->IsForStatement(it->first) == 0) {
      if (codegen->IsVarRefExp(it->first) == 0) {
        return false;
      }
      void *idx_init = codegen->GetVariableInitializedName(it->first);
      if (idx_init != target_init) {
        break;
      }
    }
  }
  void *new_ref = codegen->CreateVariableRef(sg_array);
  void *new_pntr = codegen->CreateArrayRef(new_ref, vec_new_expr);
  if (codegen->IsAddressOfOp(sg_pntr) != 0) {
    new_pntr = codegen->CreateExp(V_SgAddressOfOp, new_pntr);
  }
  cout << "REPLACE : " << codegen->_p(sg_array) << "=>" << codegen->_p(sg_pntr)
       << "=>" << codegen->_p(new_pntr) << endl;
  codegen->ReplaceExp(sg_pntr, new_pntr);
  return true;
}

void delete_internal_pragma(CSageCodeGen *codegen,
                            const vector<void *> &ch_pragma) {
  for (auto sg_pragma : ch_pragma) {
    string pragmaString = codegen->GetPragmaString(sg_pragma);
    codegen->AddComment("Original: #pragma " + pragmaString,
                        codegen->GetScope(sg_pragma));
    codegen->RemoveStmt(sg_pragma);
  }
}
