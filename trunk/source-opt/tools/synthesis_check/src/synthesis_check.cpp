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

#include "codegen.h"
#include "mars_opt.h"
// #include "prefrontend_check.h"
#include "input_checker.h"

using std::cout;
using std::endl;

int check_non_static_pointer_interface_top(CSageCodeGen *codegen,
                                           void *pTopFunc,
                                           const CInputOptions &options);
int DetectPointerCompareInterfaceTop(CSageCodeGen *codegen, void *pTopFunc,
                                     const CInputOptions &options);
int DetectPointerTypeCastInterfaceTop(CSageCodeGen *codegen, void *pTopFunc,
                                      const CInputOptions &options);
int DetectLibraryCallsTop(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options);

int SynthesisCheckTop(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options) {
  InputChecker instance(codegen, pTopFunc, options);
  if ("test_precheck" == options.get_option_key_value("-a", "test_precheck")) {
    instance.PreCheck();
    return 0;
  }
  if (instance.mPure_kernel) {
    instance.ExpandDefineDirectiveInPragma();
  }
  instance.build_mars_ir(instance.mPure_kernel, false, true);
  instance.build_mars_ir_v2(false);

  if (instance.mPure_kernel) {
    instance.CheckInterfacePragma();
    instance.CheckOldStyle();
  }
  instance.CheckInvalidVariableInPragma();
  instance.CheckUserDefinedTypeOutsideHeaderFile();
  instance.CheckFunctionPointerTop();
  instance.CheckKernelName();

  instance.CheckAddressOfPortArray();
  instance.CheckValidTop();
  instance.CheckKernelArgumentNumberTop();
  instance.CheckKernelArgumentTop();
  instance.CheckMemberFunction();
  instance.CheckAssertHeader();

  if (instance.mPure_kernel) {
    instance.CheckOverloadInKernelFunction();
    instance.PreCheckLocalVariableWithNonConstantDimension();
  } else {
    instance.PostCheckLocalVariableWithNonConstantDimension();
  }

  instance.CheckOpenclConflictName();

  instance.CheckRangeAnalysis();

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "check non-static pointer/array " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  check_non_static_pointer_interface_top(codegen, pTopFunc, options);

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Pointer compare " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  DetectPointerCompareInterfaceTop(codegen, pTopFunc, options);

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Pointer reinterpret cast " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  DetectPointerTypeCastInterfaceTop(codegen, pTopFunc, options);

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Library calls " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  DetectLibraryCallsTop(codegen, pTopFunc, options);
  instance.check_result();
  return 1;
}
