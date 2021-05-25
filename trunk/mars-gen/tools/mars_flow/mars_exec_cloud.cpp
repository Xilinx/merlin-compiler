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

void ShowUsageMessage() {
  cout << "Usage: mars_exec_cloud [-s init_script] [-i inputs] [-o outputs] "
          "[-t title] cmd ..."
       << endl;
  cout << "       cmd : the command to run in the cloud " << endl;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    ShowUsageMessage();
    exit(0);
  }

  int i = 1;
  string str_init_script;
  string str_input;
  string str_output;  // "mars_cloud_start.o,mars_cloud_done.o";
  string str_cmd;
  string str_args;
  string str_title = "mars_cloud_job.sh";
  string str_cpu = "1";
  string str_priority;
  string str_output_dir;
  int b_force_transfer_from_arg = 0;
  if (i + 1 < argc && string(argv[i]) == "-s") {
    str_init_script += string(argv[i + 1]);
    i += 2;
  }
  if (i < argc && string(argv[i]) == "-force_transfer") {
    b_force_transfer_from_arg = 1;
    i += 1;
  }
  if (i + 1 < argc && string(argv[i]) == "-i") {
    str_input += string(argv[i + 1]);
    i += 2;
  }
  if (i + 1 < argc && string(argv[i]) == "-o") {
    if (!str_output_dir.empty()) {
      // ZP: // the remaining output file are normal
      str_output += "," + string(argv[i + 1]);
    } else {  // if (str_output_dir == "" ) {    // ZP: the first output dir are
              // used to store standard condor_output
      str_output_dir = string(argv[i + 1]);
      str_output += "," + str_output_dir + ".condor_output";

      char *pForceTransfer = getenv("MARS_TEST_CONDOR_FORCE_TRANSFER");

      if ((pForceTransfer != nullptr) || (b_force_transfer_from_arg != 0)) {
        string sForceTransfer;  // (pForceTransfer);
        if (pForceTransfer != nullptr) {
          sForceTransfer = string(pForceTransfer);
        }
        if (sForceTransfer == "1" || sForceTransfer == "TRUE" ||
            sForceTransfer == "true" || (b_force_transfer_from_arg != 0)) {
          str_output += "," + string(argv[i + 1]);
        }
      }

      str_input +=
          "," + str_output_dir +
          ".condor_output";  // TODO(youxiang): to be fixed, was a quick
                             // workaround for aws condor execution
    }
    i += 2;
  }
  if (i + 1 < argc && string(argv[i]) == "-t") {
    str_title = string(argv[i + 1]);
    i += 2;
  }
  // modify by Han, for test cloud
  if (i + 1 < argc && string(argv[i]) == "-cpu") {
    str_cpu = string(argv[i + 1]);
    i += 2;
    //    printf("str_cpu=%s\n",str_cpu);
  }
  if (i + 1 < argc && string(argv[i]) == "-p") {
    str_priority = string(argv[i + 1]);
    i += 2;
    //    printf("str_prioritiy=%s\n",str_priority);
  }
  if (i < argc) {
    str_cmd = string(argv[i]);
    i++;
  }

  for (; i < argc; i++) {
    str_args += string(argv[i]) + " ";
  }

  if (str_cmd.empty()) {
    ShowUsageMessage();
    exit(0);
  }

  string job_file = str_title;

  string cmd = "#! /bin/bash\n";
  cmd += "export HOME=/home/nobody \n";
  cmd += "# " + str_cmd + " " + str_args + "\n";
  cmd += "date \n";
  cmd += "whoami \n";
  cmd += "hostname \n";
  cmd += "pwd \n";
  cmd += "\n";
  cmd += "date > " + str_output_dir + ".condor_output/mars_cloud_start.o\n";
  cmd += "export XILINX_LOCAL_USER_DATA=no\n";  // ZP: 20150828: Xilinx bug on
                                                // access conflicts
  if (!str_init_script.empty()) {
    cmd += "source " + str_init_script + "; \n";
  }
  cmd += str_cmd + " " + str_args + "\n";
  cmd += "cat " + str_output_dir + ".condor_output/mars_cloud_start.o > " +
         str_output_dir + ".condor_output/mars_cloud_done.o\n";
  cmd += "date >> " + str_output_dir + ".condor_output/mars_cloud_done.o\n";
  write_string_into_file(cmd, job_file);

  cmd = "mars_env_loader cp \\$MARS_BUILD_DIR/scripts/condor/mars_job.condor .";
  system(cmd.c_str());

  str_cmd = str_replace(str_cmd, "/", "\\/");
  str_args = str_replace(str_args, "/", "\\/");
  str_input = str_replace(str_input, "/", "\\/");
  str_output = str_replace(str_output, "/", "\\/");

  // The real command is put in the script
  str_cmd = str_title;
  str_args = "";

  assert(!str_cmd.empty());
  string str_cmd_org = str_cmd;
  if (!str_cmd.empty()) {
    str_cmd = "executable=" + str_cmd;
  }
  if (!str_args.empty()) {
    str_args = "arguments=" + str_args;
  }
  if (!str_input.empty()) {
    str_input = "transfer_input_files=" + str_input;
  }
  if (!str_output.empty()) {
    str_output = "transfer_output_files=" + str_output;
  }

  // modify by Han, for test cloud
  char filename[] = "choose_test_machine.pl";
  FILE *fptr = fopen(filename, "r");
  if (fptr != nullptr) {
    fclose(fptr);
    printf("File exists\n");
    system("perl choose_test_machine.pl");
  }
  cmd = "perl -p -i -e 's/request_cpus=1/request_cpus=" + str_cpu +
        "/' mars_job.condor";
  system(cmd.c_str());
  cmd = "perl -p -i -e 's/priority=0/priority=" + str_priority +
        "/' mars_job.condor";
  system(cmd.c_str());

  cmd = "perl -p -i -e 's/__mars_cloud_job_executable__/" + str_cmd +
        "/' mars_job.condor";
  system(cmd.c_str());
  cmd = "perl -p -i -e 's/__mars_cloud_job_arguments__/" + str_args +
        "/' mars_job.condor";
  system(cmd.c_str());
  cmd = "perl -p -i -e 's/__mars_cloud_job_inputs__/" + str_input +
        "/' mars_job.condor";
  system(cmd.c_str());
  cmd = "perl -p -i -e 's/__mars_cloud_job_outputs__/" + str_output +
        "/' mars_job.condor";
  system(cmd.c_str());

  cmd = "rm -rf " + str_output_dir + ".condor_output/; mkdir " +
        str_output_dir + ".condor_output/";
  system(cmd.c_str());
  cmd = "date > " + str_output_dir + ".condor_output/" + str_cmd_org +
        ".condor_submitted";
  system(cmd.c_str());
  cmd = "condor_submit mars_job.condor |& tee mars_exec_cloud.log";
  system(cmd.c_str());

  return 0;
}
