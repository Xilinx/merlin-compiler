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


#include <map>
#include "cmdline_parser.h"
#include "auto_parallel_insertion.h"
#include "gtest/gtest.h"

namespace {

int test_argc;
char **test_argv;

using ::testing::TestWithParam;
using ::testing::Values;

class AutoParallelInsertionTest : public testing::Test {
 public:
  AutoParallelInsertionTest() {
    ROSE_INITIALIZE;

    options.set_flag("-t", 1, 0);
    // options.set_flag("-e", 1);
    // options.set_flag("-p", 1);
    options.set_flag("-o", 1, 0);
    options.set_flag("-x", 1, 0);
    options.set_flag("-a", 20, 0);
    options.set_flag("-c", 2, 0);
    options.set_flag("-fgrain", 1, 0);
    options.set_flag("", 100000, 2);

    if (!options.parse(test_argc, test_argv)) {
      exit(-1);
    }

    int ret;
    // No matter C or Cl files, we will change the file name to c file
    for (int i = 1; i < options.get_option_num(""); i++) {
      string s_file = options.get_option("", i);
      string ext = s_file.substr(s_file.find(".") + 1);
      if (ext == "cl") {
        string s_file_org = s_file;
        s_file = s_file.substr(0, s_file.find(".")) + ".c";
        string cmd = "cp " + s_file_org + " " + s_file;
        ret = system(cmd.c_str());
        if (ret != 0) {
          fprintf(stderr, "Error: command %s failed\n", cmd.c_str());
          exit(ret);
        }
      }
      vec_src_list.push_back(s_file);
    }
  }
  CInputOptions options;
  //  map<string, void*> CollectLoops(CSageCodeGen &m_ast, DsGenerator& dg);

  vector<string> vec_src_list;
};

TEST_F(AutoParallelInsertionTest, FirstTest) {
  CSageCodeGen codegen;
  cout << "begin\n";
  AutoParallel auto_parallelizer(&codegen,
                                 codegen.OpenSourceFile(vec_src_list, ""));
  cout << "end\n";
  int ret = auto_parallelizer.run();
  EXPECT_EQ(0, ret);
}
}  // namespace

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  test_argc = argc;
  test_argv = argv;
  return RUN_ALL_TESTS();
}
