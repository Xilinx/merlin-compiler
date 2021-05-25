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
#include "license.h"
#define CHECKED_LICENSE ".merlin_flow_checked_license"

using std::string;

int main(int argc, char **argv) {
  // checking license
  //  Feature feature = FALCON_CUSTOM;
  //  bool need_check_license = true;

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
    //    if (string(argv[i]) == "xilinx" || string(argv[i]) == "sdaccel") {
    //      feature = FALCON_XILINX;
    //      feature_s = "XILINX";
    //    } else if (string(argv[i]) == "custom") {
    //      feature = FALCON_CUSTOM;
    //      feature_s = "CUSTOM";
    //    } else {
    //      feature = FALCON_CUSTOM;
    //      feature_s = "CUSTOM";
    //    }
    //    if (string(argv[i]) == "syncheck") {
    //      need_check_license = false;
    //    }
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
  //
  //  bool merlin_flow_checked_license = false;
  //  //  string flag_file = CHECKED_LICENSE;
  //  //  if (FILE *file = fopen(flag_file.c_str(), "r")) {
  //  //    merlin_flow_checked_license = true;
  //  //    fclose(file);
  //  //  }
  //  bool no_env_set = true;
  //  //  char *checkout_env = getenv("FALCON_LICENSE_CHECKEOUT");
  //  //  if (checkout_env) {
  //  //    if (strcmp(checkout_env, "CHECKOUT") == 0) {
  //  //        no_env_set = false;
  //  //    }
  //  //  }
  //  bool merlin_dse = false;
  //  char *dse_env = getenv("MERLIN_DSE");
  //  if (dse_env != nullptr) {
  //    if (strcmp(dse_env, "ON") == 0) {
  //      merlin_dse = true;
  //    }
  //  }
  //  if (need_check_license && no_env_set && !merlin_flow_checked_license &&
  //      !merlin_dse) {
  //    printf("INFO: [MERCC-1005] Checking license.\n");
  //    fflush(stdout);
  //    fc_license_init();
  //    int ret = fc_license_checkout(feature, 0);
  //    if (ret != 0) {
  //      return ret;
  //    }
  //    char fstring[100] = {'\0'};
  //    feature2string(feature, fstring);
  //    printf("INFO: [MERCC-1007] Feature %s checked out.\n", fstring);
  //    fflush(stdout);
  //
  //    setenv("FALCON_LICENSE_CHECKEOUT", "CHECKOUT", 1);
  //    //    string cmd = "touch " + flag_file;
  //    //    system(cmd.c_str());
  //  }
  int ret = 0;
  ret = system(cmd.c_str());
  //  if (need_check_license && no_env_set && !merlin_flow_checked_license &&
  //      !merlin_dse) {
  //    fc_license_checkin(feature);
  //    fc_license_cleanup();
  //    setenv("FALCON_LICENSE_CHECKEOUT", "CHECKIN", 1);
  //  }
  return ret;
}
