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



#include "cmdline_parser.h"
#include "file_parser.h"

int CInputOptions::parse(int argc, char *argv[]) {
  int i;
  for (i = 0; i < argc; i++) {
    string flag = string(argv[i]);
    if (m_flags.find(flag) != m_flags.end()) {
      assert(m_options.find(flag) !=
             m_options.end());  // should have been set by set_flag

      int max = m_flags[flag].second;
      if (max == -1) {  // binary argument
        m_options[flag].push_back("1");
        continue;
      }

      if (i >= argc - 1) {
        return 0;  // missing argument at the end
      }
      m_options[flag].push_back(string(argv[i + 1]));
      i++;
      continue;
    }  // non-flag argument

    m_options[string("")].push_back(string(argv[i]));
  }

  // check arguments
  map<string, vector<string>>::iterator it;
  for (it = m_options.begin(); it != m_options.end(); it++) {
    string flag = it->first;
    int num = it->second.size();
    int min = m_flags[flag].first;
    int max = m_flags[flag].second;

    if (max == -1) {
      continue;  // binary flag, without value;
    }
    if (num < min || num > max) {
      return 0;
    }
  }

  return 1;
}

string CInputOptions::get_option(string flag, int idx) const {
  if (get_option_num(flag) == 0) {
    return string("");
  }

  vector<string> options = m_options.find(flag)->second;
  if (idx < 0 || idx >= static_cast<int>(options.size())) {
    return ("");
  }

  return options[idx];
}

int CInputOptions::get_option_num(string flag) const {
  if (m_options.find(flag) == m_options.end()) {
    return 0;
  }
  return m_options.find(flag)->second.size();
}

// ZP: 20151029: This function is not tested
string CInputOptions::get_option_key_value(string flag, string key) const {
  for (int i = 0; i < get_option_num(flag); i++) {
    string term = get_option(flag, i);
    vector<string> sub_term = str_split(term, '=');
    if (sub_term.size() == 1) {
      RemoveSpace(&term);
      if (key == term) {
        return term;
      }
    } else if (sub_term.size() == 2) {
      string curr_key = sub_term[0];
      RemoveSpace(&curr_key);
      if (key == curr_key) {
        return sub_term[1];
      }
    } else if (sub_term.size() > 2) {
      string curr_key = sub_term[0];
      RemoveSpace(&curr_key);
      if (key == curr_key) {
        string res;
        for (size_t j = 1; j < sub_term.size(); ++j) {
          if (j > 1) {
            res += "=";
          }
          res += sub_term[j];
        }
        return res;
      }
    } else {
      // assert(0);
    }
  }
  return "";
}
