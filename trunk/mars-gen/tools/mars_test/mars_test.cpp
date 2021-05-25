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
#include <string>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

using std::string;

int main(int argc, char **argv) {
  string str_args;
  int is_aws = 0;  // ZP: 20170917
  for (int i = 1; i < argc; i++) {
    if (string(argv[i]) != "-aws") {
      str_args += string(argv[i]) + " ";
    } else {
      is_aws = 1;
    }
  }
  string cmd =
      "mars_env_loader perl \\$MARS_TEST_HOME_GIT/script/run/mars_test_top.pl ";
  if (is_aws != 0) {
    cmd = "mars_env_loader perl "
          "\\$MARS_TEST_HOME_GIT/script/run/mars_test_aws.pl ";  // a wrapper
                                                                 // based
  }
  // on mar_test_top
  cmd += str_args;
  int ret = system(cmd.c_str());
  return ret;
}
