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


#include "analPragmas.h"
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include "codegen.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "rose.h"

#define USED_CODE_IN_COVERAGE_TEST 0

bool CAnalPragma::PragmasFrontendProcessing(void *decl, bool *errorOut,
                                            bool check, bool vendor_pragma) {
  string pragmaString = m_ast->GetPragmaString(decl);
  vector<string> constructs;
  bool user_pragma =
      m_ast->IsFromInputFile(decl) != 0 || m_ast->IsTransformation(decl) != 0;
  bool report = user_pragma && check;
  string diagnosis_info;
  if (!parse_pragma(decl, &diagnosis_info, report, vendor_pragma)) {
    if (report) {
      std::ostringstream oss;
      oss << pragmaString << " " << getUserCodeFileLocation(m_ast, decl, true);
      string pragma_info = oss.str();
      if (!m_pragma_type.empty()) {
#if 1
        if (!diagnosis_info.empty()) {
          dump_critical_message(
              PROCS_ERROR_63(m_pragma_type, pragma_info, diagnosis_info));
        } else {
          dump_critical_message(PROCS_ERROR_62(m_pragma_type, pragma_info));
        }
        *errorOut = true;
#endif
      } else {
        dump_critical_message(PROCS_WARNING_16(pragma_info));
      }
    }
    return false;
  }

  return true;
}

#if USED_CODE_IN_COVERAGE_TEST
static bool isInvalidTaskName(const string &name) {
  //  We should only support the task name which is an identifier:
  //  1. contains only letter, digit or '_'
  //  2. not start with digit
  if (name.empty())
    return true;
  for (auto c : name) {
    if (std::isalnum(c) || c == '_')
      continue;
    return true;
  }
  if (std::isdigit(*name.begin()))
    return true;
  return false;
}

string CAnalPragma::get_task_name() { return get_attribute(CMOST_name); }
#endif

void CAnalPragma::splitString(string str, string delim,
                              vector<string> *results) {
  //  remove consecutive spaces
  std::string::iterator new_end =
      std::unique(str.begin(), str.end(), [](char lhs, char rhs) {
        return (lhs == rhs) && (isspace(lhs) != 0);
      });
  str.erase(new_end, str.end());
  boost::replace_all(str, " =", "=");
  boost::replace_all(str, "= ", "=");

  size_t cutAt;
  while ((cutAt = str.find_first_of(delim)) != std::string::npos) {
    if (cutAt > 0) {
      results->push_back(str.substr(0, cutAt));
    }
    str = str.substr(cutAt + 1);
  }

  if (str.length() > 0) {
    results->push_back(str);
  }
}

void CAnalPragma::opt_clear() {
  opt_tag.pipeline = false;
  opt_tag.parallel = false;
  opt_tag.burst = false;
  opt_tag.false_dep = false;
  opt_tag.task = false;
  opt_tag.kernel = false;
  opt_tag.reduction = false;
  opt_tag.line_buffer = false;
  opt_tag.interface = false;
  opt_tag.attribute = false;
  opt_tag.task_wait = false;
  opt_tag.skip_sync = false;
  opt_tag.loop_tiling = false;
  opt_tag.spawn = false;
  opt_tag.spawn_kernel = false;
  opt_tag.coalescing = false;
  opt_tag.loop_flatten = false;
  opt_tag.loop_tiled_tag = false;
  opt_tag.cache = false;
  opt_tag.hls_unroll = false;
  opt_tag.partition = false;
  m_hls_fgopt = false;
  m_vendor = "";
}

bool CAnalPragma::parse_pragma(void *decl, string *diagnosis_info, bool report,
                               bool vendor_pragma) {
  string str_pragma = m_ast->GetPragmaString(decl);
  map<string, pair<vector<string>, vector<string>>> mapParams;
  vector<string> mapKeys;
  string sFilter;
  string sCmd;
  tldm_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams,
                          vendor_pragma);
  if (is_cmost_pragma(sFilter))
    m_vendor = ACCEL_PRAGMA;

  if (is_hls_pragma(sFilter)) {
    m_vendor = HLS_PRAGMA;
    string sFilter1;
    all_pragma_parse_whole(str_pragma, &sFilter1, &sCmd, &mapParams, &mapKeys);
    boost::algorithm::to_upper(sCmd);
    if (HLS_UNROLL == sCmd || HLS_PIPELINE == sCmd) {
      m_hls_fgopt = true;
    }
    if (HLS_UNROLL == sCmd) {
      opt_tag.hls_unroll = true;
    }
    m_cmd = sCmd;
    if (!vendor_pragma)
      return true;
  }
  if (is_cmost_pragma(sFilter) == 0 &&
      (!vendor_pragma || m_vendor != HLS_PRAGMA)) {
    if (report) {
      std::ostringstream oss;
      oss << str_pragma << " " << getUserCodeFileLocation(m_ast, decl, true);
      string pragma_info = oss.str();
      dump_critical_message(PROCS_WARNING_16(pragma_info));
    }
    return true;
  }
  // Yuxin: Dec/18/2019
  // Deal with HLS pragmas with separate command string
  // e.g. "array_partition" can be "array partition"
  if (is_hls_pragma(sFilter) && mapKeys.size() > 0) {
    string attr = mapKeys[0];
    boost::algorithm::to_upper(attr);
    if (sCmd == HLS_ARRAY) {
      if (attr == HLS_PARTITION) {
        sCmd = HLS_ARR_PARTITION;
        mapParams.erase(attr);
      }
    } else if (sCmd == HLS_LOOP) {
      if (attr == HLS_TC) {
        sCmd = HLS_TRIPCOUNT;
        mapParams.erase(attr);
      }
    }
  }
  bool valid = true;
  m_ref = decl;
  for (auto key_val : mapParams) {
    string attr = key_val.first;
    vector<string> vec_val = key_val.second.first;
    if (!vec_val.empty()) {
      valid &= add_attribute(attr, vec_val[0]);
#if USED_CODE_IN_COVERAGE_TEST
      //  No more use according to the current pragma parsing method
      for (size_t i = 1; i < vec_val.size(); ++i) {
        valid &= append_attribute(attr, vec_val[i]);
      }
#endif
    } else {
      valid &= add_attribute(attr, "");
    }
  }
  boost::algorithm::to_upper(sCmd);
  m_cmd = sCmd;
  if (m_vendor == HLS_PRAGMA) {
    if (HLS_interface == sCmd) {
      opt_tag.interface = true;
    }
    m_pragma_type = sCmd;
    return true;
  }
  if (CMOST_PARALLEL == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.parallel = true;
  } else if (CMOST_PIPELINE == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.pipeline = true;
  } else if (CMOST_LOOP_TILING == sCmd || CMOST_LOOP_TILE == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.loop_tiling = true;
  } else if (CMOST_KERNEL == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.kernel = true;
  } else if (CMOST_interface == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.interface = true;
  } else if (CMOST_attribute == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.attribute = true;
  } else if (CMOST_SKIP_SYNC == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.skip_sync = true;
  } else if (PRAGMA_TYPE_NODE == sCmd) {
    m_pragma_type = sCmd;
  } else if (PRAGMA_TYPE_PORT == sCmd) {
    m_pragma_type = sCmd;
  } else if (PRAGMA_TYPE_CHANNEL == sCmd) {
    m_pragma_type = sCmd;
  } else if (PRAGMA_TYPE_BUFFER == sCmd) {
    m_pragma_type = sCmd;
  } else if (CMOST_REDUCTION == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.reduction = true;
  } else if (CMOST_LINE_BUFFER == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.line_buffer = true;
  } else if (CMOST_TASK_WAIT == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.task_wait = true;
  } else if (CMOST_TASK == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.task = true;
  } else if (CMOST_BURST == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.burst = true;
  } else if (CMOST_SPAWN == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.spawn = true;
  } else if (CMOST_SPAWN_KERNEL == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.spawn_kernel = true;
  } else if (CMOST_false_dep == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.false_dep = true;
  } else if (CMOST_coalescing == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.coalescing = true;
  } else if (CMOST_loop_flatten == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.loop_flatten = true;
  } else if (CMOST_INLINE == sCmd) {
    m_pragma_type = sCmd;
  } else if (CMOST_LOOP_TILED_TAG == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.loop_tiled_tag = true;
  } else if (CMOST_cache == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.cache = true;
  } else if (CMOST_ARR_PARTITION == sCmd) {
    m_pragma_type = sCmd;
    opt_tag.partition = true;
  } else if (CMOST_wrapper == sCmd || CMOST_register == sCmd) {
  } else if (CMOST_bundle == sCmd) {
    // Do nothing
  } else if (regex_match_auto(sCmd)) {
    m_pragma_type = CMOST_auto;
    (*diagnosis_info) +=
        std::string("    Unsupport \"") + CMOST_auto + "\" as the pragma type";
    return false;
  } else {
    return false;
  }

  if (m_pragma_type == CMOST_PARALLEL) {
    if (get_attribute(CMOST_parallel_factor).empty()) {
      add_attribute(CMOST_complete, "");
    }
  } else if (m_pragma_type == CMOST_PIPELINE) {
    //   if (get_attribute(CMOST_ii) == "")
    //     add_attribute(CMOST_ii, "1");
  }
  valid &= check_attribute_legality(decl, diagnosis_info, report);
  // cout << "valid: " << valid << ", " << m_ast->_p(decl) <<endl;
  return valid;
}

bool CAnalPragma::add_attribute(string attr, string val) {
  if (!regex_match_auto(attr)) {
    boost::algorithm::to_upper(attr);
  }
  pragma_table[attr] = val;
  // TODO(youxiang): check valid
  return true;
}

#if USED_CODE_IN_COVERAGE_TEST
bool CAnalPragma::append_attribute(string attr, string val) {
  if (!regex_match_auto(attr))
    boost::algorithm::to_upper(attr);
  pragma_table[attr] += "," + val;
  // TODO(youxiang): check valid
  return true;
}
#endif

string CAnalPragma::print_attribute() {
  string ret;
  for (auto pair : pragma_table) {
    ret += pair.first;
    if (!pair.second.empty()) {
      ret += "=" + pair.second;
    }
    ret += " ";
  }
  return ret;
}

void *CAnalPragma::update_pragma(void *decl, bool keep_old, void *scope) {
  string new_pragma_str = m_vendor == HLS_PRAGMA ? HLS_PRAGMA : ACCEL_PRAGMA;
  new_pragma_str += " " + m_pragma_type + " " + print_attribute();
  auto sg_pragma_decl = isSgPragmaDeclaration(static_cast<SgNode *>(decl));
  if ((sg_pragma_decl != nullptr) && !keep_old) {
    m_ast->SetPragmaString(decl, new_pragma_str);
    return decl;
  } else {
    void *new_decl = m_ast->CreatePragma(
        new_pragma_str,
        scope != nullptr ? scope
                         : (decl != nullptr ? m_ast->GetScope(decl) : nullptr));

    return new_decl;
  }
}

string CAnalPragma::get_attribute(string attr) {
  boost::algorithm::to_upper(attr);
  if (pragma_table.count(attr) > 0) {
    string ret = pragma_table[attr];
    if (ret.empty()) {
      return attr;
    }
    return ret;
  }
  return "";
}

void CAnalPragma::remove_attribute(string attr) {
  boost::algorithm::to_upper(attr);
  pragma_table.erase(attr);
}

bool CAnalPragma::is_hls_pragma(string filter) {
  boost::algorithm::to_upper(filter);
  return filter == AP_PRAGMA || filter == HLS_PRAGMA;
}

bool CAnalPragma::is_hls_pragma() { return m_vendor == HLS_PRAGMA; }

bool CAnalPragma::check_attribute_legality(void *decl, string *diagnosis_info,
                                           bool report) {
  bool all_valid = true;
  static set<string> valid_attr_set;
  static set<string> no_check_attr_set;
  static int init_set = 0;
  if (0 == init_set) {
    valid_attr_set.insert(CMOST_continue);
    valid_attr_set.insert(CMOST_variable);
    valid_attr_set.insert(CMOST_depth);
    valid_attr_set.insert(CMOST_max_depth);
    valid_attr_set.insert(CMOST_migrate_mode);
    valid_attr_set.insert(CMOST_bank);
    valid_attr_set.insert(CMOST_factor);
    valid_attr_set.insert(CMOST_burst_max_size);
    valid_attr_set.insert(CMOST_burst_off);
    valid_attr_set.insert(CMOST_prior);
    valid_attr_set.insert(CMOST_bus_bitwidth);
    valid_attr_set.insert(CMOST_REDUCTION);
    valid_attr_set.insert(CMOST_LINE_BUFFER);
    valid_attr_set.insert(CMOST_ARR_PARTITION);
    valid_attr_set.insert(CMOST_complete);
    valid_attr_set.insert(CMOST_FLATTEN);
    valid_attr_set.insert(CMOST_OFF);
    valid_attr_set.insert(CMOST_ii);
    valid_attr_set.insert(CMOST_exclusive);
    valid_attr_set.insert(CMOST_name);
    valid_attr_set.insert(CMOST_copy_in);
    valid_attr_set.insert(CMOST_copy_out);
    valid_attr_set.insert(CMOST_copy);
    valid_attr_set.insert(CMOST_memory_burst);
    valid_attr_set.insert(CMOST_write_only);
    valid_attr_set.insert(CMOST_force);
    valid_attr_set.insert(CMOST_die);
    valid_attr_set.insert(CMOST_parallel_multi_level_factor);
    valid_attr_set.insert(CMOST_parallel);
    valid_attr_set.insert(CMOST_type);
    valid_attr_set.insert(CMOST_stream_prefetch);
    valid_attr_set.insert(CMOST_bundle);
    valid_attr_set.insert(CMOST_stream);
    valid_attr_set.insert(CMOST_io);
    valid_attr_set.insert(CMOST_data_type);
    valid_attr_set.insert(CMOST_scope_type);
    valid_attr_set.insert(PRAGMA_NODE_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_PORT_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_SPAWN_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_SPAWN_KERNEL_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_PREFETCH_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_READ_NODE_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_WRITE_NODE_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_VARIABLE_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_PARALLEL_FACTOR_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_DIM_NUM_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_ACCESS_TYPE_ATTRIBUTE);
    valid_attr_set.insert(PRAGMA_CHANNEL_DEPTH_ATTRIBUTE);
    valid_attr_set.insert(CMOST_SKIP);
    valid_attr_set.insert(CMOST_scheme);
    valid_attr_set.insert(CMOST_reduction_scheme);
    valid_attr_set.insert(CMOST_wrapper);
    valid_attr_set.insert(CMOST_register);

    no_check_attr_set.insert(PRAGMA_NODE_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_PORT_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_SPAWN_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_SPAWN_KERNEL_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_PREFETCH_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_READ_NODE_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_WRITE_NODE_ATTRIBUTE);
    //  no_check_attr_set.insert(PRAGMA_VARIABLE_ATTRIBUTE);
    //  no_check_attr_set.insert(PRAGMA_PARALLEL_FACTOR_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_DIM_NUM_ATTRIBUTE);
    no_check_attr_set.insert(PRAGMA_ACCESS_TYPE_ATTRIBUTE);
    no_check_attr_set.insert(CMOST_continue);
    no_check_attr_set.insert(CMOST_prior);
    no_check_attr_set.insert(CMOST_name);
    no_check_attr_set.insert(CMOST_die);
    no_check_attr_set.insert(CMOST_type);
    no_check_attr_set.insert(CMOST_io);
    no_check_attr_set.insert(CMOST_copy);
    no_check_attr_set.insert(CMOST_data_type);
    no_check_attr_set.insert(CMOST_scope_type);
    no_check_attr_set.insert(CMOST_SKIP);
    no_check_attr_set.insert(CMOST_reduction_scheme);
    no_check_attr_set.insert(CMOST_migrate_mode);
    no_check_attr_set.insert(CMOST_bundle);
    no_check_attr_set.insert(HLS_RESOURCE);
    init_set = 1;
  }

  for (auto attr_info : pragma_table) {
    string attr = attr_info.first;
    if (report) {
      if (!regex_match_auto(attr) && valid_attr_set.count(attr) <= 0) {
        (*diagnosis_info) += "unsupported attribute \'" + attr + "\'";
        all_valid = false;
      }
    }
    if (regex_match_auto(attr) || no_check_attr_set.count(attr) > 0) {
      continue;
    }
    string value = attr_info.second;
    if (CMOST_memory_burst == attr || CMOST_force == attr ||
        CMOST_stream_prefetch == attr || CMOST_stream == attr) {
      string tmp = value;
      boost::algorithm::to_upper(tmp);
      if (tmp != CMOST_ON && tmp != CMOST_OFF && tmp != CMOST_auto) {
        return false;
      }
      continue;
    }
    if (attr == CMOST_scheme) {
      if (!opt_tag.reduction) {
        (*diagnosis_info) +=
            "  Invalid value for attribute \'" + attr + "\' = \'" + value +
            "\'.\n  Hint: did you mean the attribute \'reduction_scheme\'?\n";
        all_valid = false;
      }
      continue;
    }

    if (value.empty()) {
      continue;
    }
    vector<string> vec_str = str_split(value, ',');
    for (auto one_var : vec_str) {
      //  check whether it is a number
      bool is_num = true;
      for (auto ch : one_var) {
        if (isdigit(ch) == 0) {
          is_num = false;
          break;
        }
      }
      if (is_num) {
        continue;
      }

      bool valid = true;
      for (auto ch : one_var) {
        if ((isalnum(ch) == 0) && ch != '_') {
          valid = false;
          break;
        }
      }
      if (!valid) {
        continue;
      }
      string one_val = one_var;
      boost::to_upper(one_val);
      if (attr == CMOST_bus_bitwidth && one_val == CMOST_OFF) {
        continue;
      }
      //  check whether we can trace to a variable
      void *find_var = m_ast->find_variable_by_name(one_var, decl);
      if (attr == CMOST_REDUCTION || attr == CMOST_LINE_BUFFER) {
        find_var = m_ast->find_variable_by_name(one_var, decl, true);
      }
      if ((find_var != nullptr) ||
          m_ast->check_valid_field_name(one_var, decl)) {
        continue;
      }
      (*diagnosis_info) += "  Invalid value for attribute \'" + attr +
                           "\' = \'" + value + "\'.\n";
      if (one_val == CMOST_auto) {
        (*diagnosis_info) += "  Hint: use auto{...} to specify design space, "
                             "or remove this pragma and change to "
                             "fully-automated DSE mode";
      } else {
        (*diagnosis_info) +=
            "  Hint: only support variable, constant and macro in "
            "pragma clause";
      }
      all_valid = false;
      break;
    }
  }
  //  check missing attribute
  if (opt_tag.loop_tiling) {
    string factor_str = get_attribute(CMOST_factor);
    boost::algorithm::to_upper(factor_str);
    if (factor_str.empty()) {
      (*diagnosis_info) +=
          std::string(" Missing attribute \'") + CMOST_factor + "\'\n";
      all_valid = false;
    } else {
      int tiling_factor = my_atoi(factor_str);
      string multi_level_factor =
          get_attribute(CMOST_parallel_multi_level_factor);
      boost::algorithm::to_upper(multi_level_factor);
      if (!multi_level_factor.empty()) {
        int parallel_factor = my_atoi(multi_level_factor);
        if ((tiling_factor != 0) && (parallel_factor != 0) &&
            (tiling_factor % parallel_factor) != 0) {
          (*diagnosis_info) += std::string(" Invalid attribute value for \'") +
                               CMOST_parallel_multi_level_factor + "\' (" +
                               multi_level_factor + ")\n";
          (*diagnosis_info) += std::string(" Because attribute value for \'") +
                               CMOST_factor + "\' (" + factor_str +
                               ") cannot be divided by it";
          all_valid = false;
        }
      }
    }
  }

  //  Check the use of reduction
  if (opt_tag.parallel || opt_tag.pipeline) {
    string scheme_str = get_attribute(CMOST_reduction_scheme);
    if (!scheme_str.empty()) {
      if (get_attribute(CMOST_REDUCTION).empty()) {
        (*diagnosis_info) += std::string(" Please use \'") + CMOST_REDUCTION +
                             "\' to mark the target reduction variable\n";
        all_valid = false;
      }
      if (scheme_str != CMOST_block && scheme_str != CMOST_cyclic) {
        (*diagnosis_info) += std::string(" Invalid attribute value for \'") +
                             CMOST_reduction_scheme + "\' (" + scheme_str +
                             ")\n";
        (*diagnosis_info) += std::string("   Hint: valid value for \'") +
                             CMOST_reduction_scheme + "\' is block/cyclic/auto";
        all_valid = false;
      }
    }
  }

  return all_valid;
}

void CAnalPragma::set_pragma_type(string pragma_type) {
  boost::algorithm::to_upper(pragma_type);
  m_pragma_type = pragma_type;
  if (CMOST_PARALLEL == pragma_type) {
    opt_tag.parallel = true;
  } else if (CMOST_PIPELINE == pragma_type) {
    opt_tag.pipeline = true;
  } else if (CMOST_LOOP_TILING == pragma_type ||
             CMOST_LOOP_TILE == pragma_type) {
    opt_tag.loop_tiling = true;
  } else if (CMOST_KERNEL == pragma_type) {
    opt_tag.kernel = true;
  } else if (CMOST_interface == pragma_type) {
    opt_tag.interface = true;
  } else if (CMOST_attribute == pragma_type) {
    opt_tag.attribute = true;
  } else if (CMOST_SKIP_SYNC == pragma_type) {
    opt_tag.skip_sync = true;
  } else if (PRAGMA_TYPE_NODE == pragma_type) {
  } else if (PRAGMA_TYPE_PORT == pragma_type) {
  } else if (PRAGMA_TYPE_CHANNEL == pragma_type) {
  } else if (PRAGMA_TYPE_BUFFER == pragma_type) {
  } else if (CMOST_REDUCTION == pragma_type) {
    opt_tag.reduction = true;
  } else if (CMOST_LINE_BUFFER == pragma_type) {
    opt_tag.line_buffer = true;
  } else if (CMOST_TASK_WAIT == pragma_type) {
    opt_tag.task_wait = true;
  } else if (CMOST_TASK == pragma_type) {
    opt_tag.task = true;
  } else if (CMOST_BURST == pragma_type) {
    opt_tag.burst = true;
  } else if (CMOST_SPAWN == pragma_type) {
    opt_tag.spawn = true;
  } else if (CMOST_SPAWN_KERNEL == pragma_type) {
    opt_tag.spawn_kernel = true;
  } else if (CMOST_false_dep == pragma_type) {
    opt_tag.false_dep = true;
  } else if (CMOST_coalescing == pragma_type) {
    opt_tag.coalescing = true;
  } else if (CMOST_loop_flatten == pragma_type) {
    opt_tag.loop_flatten = true;
  } else if (CMOST_INLINE == pragma_type) {
  } else if (CMOST_LOOP_TILED_TAG == pragma_type) {
    opt_tag.loop_tiled_tag = true;
  } else if (CMOST_cache == pragma_type) {
    opt_tag.cache = true;
  } else if (CMOST_ARR_PARTITION == pragma_type) {
    opt_tag.partition = true;
  } else {
    assert(0 && "unknown pragma type");
  }
}

map<string, string> CAnalPragma::get_attribute() { return pragma_table; }

bool CAnalPragma::contain_auto_value() const {
  for (auto &&[attr, value] : pragma_table) {
    if (regex_match_auto(value)) {
      return true;
    }
  }
  return false;
}

bool CAnalPragma::regex_match_auto(string str) const {
  return (
      boost::regex_match(str, boost::regex(std::string(CMOST_auto) + "\\{.+\\}",
                                           boost::regex::icase)));
}

string CAnalPragma::get_hls_interface_mode() const {
  if (opt_tag.interface == false)
    return "";
  if (m_vendor != HLS_PRAGMA)
    return "";
  for (auto key_val : pragma_table) {
    auto key = key_val.first;
    auto val = key_val.second;
    boost::algorithm::to_upper(key);
    if (!val.empty() || key == HLS_register)
      continue;
    return key;
  }
  return "";
}
