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


#include "interface_transform.h"

int interface_transform_top(CSageCodeGen *codegen, void *pTopFunc,
                            const CInputOptions &options) {
  bool is_systolic_array_flow =
      options.get_option_key_value("-a", "systolic_array") == "on";
  bool is_array_linearize =
      options.get_option_key_value("-a", "disable_array_linearize") != "off";

  bool is_pure_kernel_flow =
      options.get_option_key_value("-a", "pure_kernel") == "on";
  //  Turn on this definition for debugging if necessary, it works
  // #define INTF_TRANS_DUMP_INTERMEDIATE

#ifdef INTF_TRANS_DUMP_INTERMEDIATE
  codegen->GenerateCode();
  system("for f in $(ls rose_*); do mv $f in_$f; done");
#endif

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Struct transformation " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  struct_decompose_top(codegen, pTopFunc, options);
#ifdef INTF_TRANS_DUMP_INTERMEDIATE
  codegen->GenerateCode();
  system("for f in $(ls rose_*); do mv $f sd_$f; done");
#endif

  //  Youxiang bug1547 20170823
  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Remove simple pointer alias " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  remove_simple_pointer_alias_top(codegen, pTopFunc, options);

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Access range intrinsic gen " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  access_range_gen_top(codegen, pTopFunc, options);
#ifdef INTF_TRANS_DUMP_INTERMEDIATE
  codegen->GenerateCode();
  system("for f in $(ls rose_*); do mv $f al_b_$f; done");
#endif

  if (!is_systolic_array_flow && is_array_linearize) {
    cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "Array transformation " << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
    array_linearize_top(codegen, pTopFunc, options);
  }
#ifdef INTF_TRANS_DUMP_INTERMEDIATE
  codegen->GenerateCode();
  system("for f in $(ls rose_*); do mv $f al_$f; done");
#endif

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "convert bool to char for xilinx flow " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  bool2char_top(codegen, pTopFunc, options);

  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "rename interface variable name " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  rename_interface_name_top(codegen, pTopFunc, options);

  if (!is_pure_kernel_flow) {
    cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "Data transfer gen " << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
    data_transfer_gen_top(codegen, pTopFunc, options);
  }
  //  This is an after-transform check, not a synthesis check item.
  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Multi-dimensional pointer " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  DetectMultiDimPointerInterfaceTop(codegen, pTopFunc, options);
  return 1;
}
