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


#include "lower_separate.h"
#include <iostream>
#include <string>

//  #include "rose.h"
#include "kernel_pragma_gen.h"
#include "input_checker.h"

extern int check_repeat_task(CSageCodeGen *codegen, void *pTopFunc);

extern int simple_dce_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options);

int lower_separate_top(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options) {
  DEFINE_START_END;
  cout << "1 ..." << endl;
  string tool_type = options.get_option_key_value("-a", "impl_tool");
  STEMP(start);
  InputChecker instance(codegen, pTopFunc, options);
  instance.PreCheck();
  TIMEP("pre_check", start, end);
  //  cout << "2 ..." << endl;

  //  //  Done in kernel_pragma_gen
  //  STEMP(start);
  //  check_repeat_task(codegen, pTopFunc);
  //  TIMEP("check_repeat_task", start, end);

  cout << "3 ..." << endl;
  STEMP(start);
  if (!codegen->IsPureCDesign()) {
    lower_cpp_top(codegen, pTopFunc, options);
  }
  TIMEP("lower_cpp_top", start, end);

#if 0  // ZP: NOTE: keep it here for debugging
  codegen->GenerateCode();
  system("for f in $(ls rose_*); do mv $f lc_$f; done");
#endif

  simple_dce_top(codegen, pTopFunc, options);
  cout << "4 ..." << endl;
  STEMP(start);
  instance.PostCheck();
  TIMEP("post_check", start, end);

  cout << "5 ..." << endl;
  STEMP(start);
  kernel_pragma_gen(codegen, pTopFunc, options);
  TIMEP("kernel_pragma_gen", start, end);

  cout << "6 ..." << endl;
  STEMP(start);
  kernel_separate_top(codegen, pTopFunc, options);

  cout << "7 ..." << endl;
  TIMEP("kernel_separate_top", start, end);
  simple_dce_top(codegen, pTopFunc, options);

  cout << "8 ..." << endl;

  return 0;
}
