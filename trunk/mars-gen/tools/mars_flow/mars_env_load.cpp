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


#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define PATH_MAX 1000

using std::string;

int main(int argc, char **argv) {
  string str_args;
  bool reset_ld_path = false;
  for (int i = 1; i < argc; i++) {
    if (string(argv[i]) == "-reset_ld_path") {
      reset_ld_path = true;
      continue;
    }
    str_args += " \"" + string(argv[i]) + "\" ";
  }
  string str_shell;
  str_shell = "sh";  // system() call in C always use Bash
  string cmd;
  if (reset_ld_path) {
    cmd = "cd $MERLIN_COMPILER_HOME/set_env;  source ./env.sh -reset_ld_path; "
          "cd - > /dev/null;  ";
  } else {
    cmd = "cd $MERLIN_COMPILER_HOME/set_env;  source ./env.sh; cd - > "
          "/dev/null;  ";
  }
  cmd += str_args;
  int ret = system(cmd.c_str());
  return std::min(ret, 255);
}
