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
// under the License. (edited) 


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using std::string;

bool is_file(string path) {
  struct stat path_stat;
  if (stat(path.c_str(), &path_stat) != 0) {
    return false;
  }
  if (!S_ISREG(path_stat.st_mode)) {
    return false;
  }
  return true;
}

bool is_dir(const string &path) {
  struct stat path_stat;
  if (stat(path.c_str(), &path_stat) != 0) {
    return false;
  }
  if (!S_ISDIR(path_stat.st_mode)) {
    return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  string msg_report_cmd =
      "python -O "
      "$MERLIN_COMPILER_HOME/mars-gen/scripts/msg_report/msg_report.py";
  string run_args = "--dst run/implement/export/lc --src src --metadata "
                    "run/implement/metadata --xml run/spec/directive.xml "
                    "--report run/implement/exec/hls/report_merlin";
  if (argc == 1 || argc == 2) {
    msg_report_cmd += string(" ") + run_args;
    if (argc == 2) {
      msg_report_cmd += string(" --step ") + argv[argc - 1];
    }
  } else {
    string args;
    for (int i = 1; i < argc; ++i) {
      args += string(argv[i]) + " ";
    }
    msg_report_cmd += string(" ") + args;
  }
  printf("run :%s\n", msg_report_cmd.c_str());
  return system(msg_report_cmd.c_str());
}
