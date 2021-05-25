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


#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define PATH_MAX 1000

using std::string;

int main(int argc, char **argv) {
  string str_prj;
  int i = 1;
  string str_args;
  for (; i < argc; i++) {
    if (strstr(argv[i], " ") != nullptr) {
      str_args += "\"" + string(argv[i]) + "\" ";
    } else {
      str_args += string(argv[i]) + " ";
    }
  }

  string cmd = "mars_perl "
               "$MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/"
               "merlin_backend_top.pl " +
               str_args;
  int ret = system(cmd.c_str());
  if (ret == 0) {
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
