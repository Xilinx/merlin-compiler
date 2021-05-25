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


#include <assert.h>
#include <string>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

using std::cout;
using std::endl;
using std::string;

#include "file_parser.h"

#define MAX_COMMAND_STRING_LENGTH 1024
string GetStdoutFromCommand(string cmd) {
  string data;
  FILE *stream;
  char buffer[MAX_COMMAND_STRING_LENGTH];
  cmd.append(" 2>&1");  // Do we want STDERR?

  stream = popen(cmd.c_str(), "r");
  if (stream != nullptr) {
    while (feof(stream) == 0) {
      if (fgets(buffer, MAX_COMMAND_STRING_LENGTH, stream) != nullptr) {
        data.append(buffer);
      }
    }
    pclose(stream);
  }
  return data;
}

void ShowUsageMessage() { system("mars_flow"); }

int main(int argc, char **argv) {
  int ret = 0;

  if (argc <= 1) {
    ShowUsageMessage();
    exit(0);
  }

  int i = 1;
  int test_flag = 0;
  int bit_flag = 0;
  int sim_flag = 0;
  string str_init_script = "~/fcs_setting64.sh";
  string str_prj;
  string str_args;
  string str_case = "marsC";
  string str_flow;
  string str_title;
  string str_cpu;
  string str_priority;
  if (i < argc) {
    str_prj = string(argv[i]);
    i++;
  }
  for (; i < argc; i++) {
    str_args += string(argv[i]) + " ";
    if (string(argv[i]) == "-c" && i + 1 < argc) {
      str_case = string(argv[i + 1]);
    }
    if (string(argv[i]) == "-no_bit_test" && i + 1 < argc) {
      sim_flag = 1;
    }
    if (str_flow.empty()) {
      str_flow = string(argv[i]);
      if (str_flow[0] == '-') {
        str_flow = str_flow.substr(1);
      }

      // modify by Han, for test cloud
      if (str_flow == "test") {
        test_flag = 1;
      }
      if (str_flow == "bit") {
        bit_flag = 1;
      }

      str_flow = "_" + string(argv[i]);
    }
  }
  if ((sim_flag == 0) && (test_flag == 1)) {
    str_cpu = "2";
    str_priority = "1";
    // run script to choose test machine
    printf(
        "cp $MERLIN_COMPILER_HOME/mars-gen/scripts/choose_test_machine.pl .\n");
    system(
        "cp $MERLIN_COMPILER_HOME/mars-gen/scripts/choose_test_machine.pl .");
  } else if (bit_flag == 1) {
    str_cpu = "2";
    str_priority = "1";
  } else {
    str_cpu = "1";
    str_priority = "0";
  }
  // sim_flag = 0;
  // test_flag = 0;
  // bit_flag = 0;
  //    if (str_flow == "test") {
  //    str_cpu = "2";
  //    str_priority = "1";
  //    } else {
  //    str_cpu = "1";
  //    str_priority = "0";
  //    }
  str_title = str_case + str_flow;

  if (str_prj.empty()) {
    ShowUsageMessage();
    exit(0);
  }

  vector<string> str_prj_split = str_split(str_prj, '/');
  string top_dir = str_prj_split[0];

  if (top_dir.empty()) {
    cout << "[mars_flow_cloud] Unsupported absolute directory" << endl;
    exit(0);
  } else if (top_dir == "..") {
    cout << "[mars_flow_cloud] only support projects in the sub-directory"
         << endl;
    exit(0);
  }

  ///////////////////////////////////////////
  // ZP: 20170910
  string additional_copy;
  additional_copy +=
      "cp " + top_dir + "/.merlin_flow_end.o " + top_dir + ".condor_output/;";
  additional_copy += "cp " + top_dir + "/implement/merlin_summary.rpt " +
                     top_dir + ".condor_output/;";
  additional_copy += "cp " + top_dir +
                     "/implement/opencl_gen/opencl/mhs_cfg.xml " + top_dir +
                     ".condor_output/;";
  additional_copy += "cp " + top_dir +
                     "/implement/export/__merlin_kernel_list.h " + top_dir +
                     ".condor_output/;";

  // LZ : 20180411
  additional_copy +=
      "cp " + top_dir + "/merlin_sim.log " + top_dir + ".condor_output/;";
  additional_copy +=
      "cp " + top_dir + "/merlin_hls.log " + top_dir + ".condor_output/;";
  additional_copy +=
      "cp " + top_dir + "/merlin_impl.log " + top_dir + ".condor_output/;";
  additional_copy +=
      "cp " + top_dir + "/merlin_opt.log " + top_dir + ".condor_output/;";
  additional_copy +=
      "cp " + top_dir + "/merlin_test.log " + top_dir + ".condor_output/;";

  ///////////////////////////////////////////

  // modify by Han, for test cloud
  string cmd = "mars_exec_cloud -s " + str_init_script + " -i " + top_dir +
               " -o " + top_dir + " -t " + str_title + " -cpu " + str_cpu +
               " -p " + str_priority + " \"merlin_flow " + str_prj + " " +
               str_args + ";" + additional_copy + "\"";
  cout << cmd << endl;
  ret = system(cmd.c_str());

  return ret;
}
