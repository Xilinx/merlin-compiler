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
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char **argv) {
  std::string str_args;
  for (int i = 1; i < argc; i++) {
    str_args += std::string(argv[i]) + " ";
  }
  std::string cmd =
      "cd $MERLIN_COMPILER_HOME/set_env; source "
      "$MERLIN_COMPILER_HOME/set_env/env.sh >& /dev/null; cd -;  ";
  cmd += "mars_perl "
         "${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/"
         "run_and_update.pl " +
         str_args;
  int ret = system(cmd.c_str());
  return ret;
}
