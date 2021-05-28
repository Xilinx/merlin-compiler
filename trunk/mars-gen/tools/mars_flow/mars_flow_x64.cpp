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
#include <vector>
#include "dirent.h"
#include "file_parser.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

using std::string;

int main(int argc, char **argv) {

  string str_prj;
  int i = 1;
  string str_args;
  if (i < argc) {
    str_prj = string(argv[i]);
    i++;
  }
  int version_flag = 0;
  if (str_prj == "-v") {
    version_flag = 1;
  }
  string feature_s;
  for (; i < argc; i++) {
    str_args += string(argv[i]) + " ";
  }

  string timer_rpt_prefix;
  if (!str_prj.empty()) {
    vector<string> str_prj_split = str_split(str_prj, '/');
    int size = str_prj_split.size();
    int n = str_prj.size();
    int m = str_prj_split[size - 1].size();
    if (str_prj_split[size - 1].empty()) {
      timer_rpt_prefix = str_prj;
    } else if (str_prj_split[size - 1].find(".prj") != string::npos) {
      timer_rpt_prefix = str_prj.substr(0, n - m);
    } else {
      timer_rpt_prefix = str_prj + "/";
    }
  }
  DIR *dir = opendir(timer_rpt_prefix.c_str());

  string cmd = " ";
  cmd += "cd $MERLIN_COMPILER_HOME/set_env; source "
         "$MERLIN_COMPILER_HOME/set_env/env.sh; >& /dev/null; cd - >& "
         "/dev/null;  ";
  if (version_flag == 0 && (dir != nullptr)) {
    if (!timer_rpt_prefix.empty()) {
      cmd += "date > " + timer_rpt_prefix + ".merlin_flow_start.o; ";
    }
  }
  cmd += "mars_python "
         "${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/"
         "merlin_flow_start_new.py " +
         str_prj + " " + str_args + ";";
  if (version_flag == 0 && (dir != nullptr)) {
    if (!timer_rpt_prefix.empty()) {
      cmd += "cat " + timer_rpt_prefix + ".merlin_flow_start.o > " +
             timer_rpt_prefix + ".merlin_flow_end.o ;";
    }
    if (!timer_rpt_prefix.empty()) {
      cmd += "date >> " + timer_rpt_prefix + ".merlin_flow_end.o;";
    }
  }
  int ret = 0;
  ret = system(cmd.c_str());
  return ret;
}
