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
  for (int i = 1; i < argc; i++) {
    str_args += string(argv[i]) + " ";
  }
  string cmd = " ";
  cmd += "python "
         "$MERLIN_COMPILER_HOME/mars-gen/scripts/project/create_project.py " +
         str_args;
  int ret = system(cmd.c_str());
  return ret;
}
