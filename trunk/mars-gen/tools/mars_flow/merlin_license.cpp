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


/*************************************************************************
    > File Name: merlin_license.cpp
    > Author:
    > Mail:
    > Created Time: Sat 29 Feb 2020 01:13:55 AM EST
 ************************************************************************/

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "license.h"

using std::string;

//int license_check_out(string impl_tool, string evaluate) {
//  int ret = 0;
//  fc_license_init();
//
//  //  if (evaluate == "off") {
//  if ((impl_tool == "sdaccel" || impl_tool == "hls") &&
//      (evaluate == "xilinx" || evaluate == "off")) {  //  XILINX
//    int feature = FALCON_XILINX;
//    ret = fc_license_checkout(feature, 1);
//    printf("\n");
//  } else if (impl_tool == "cheetah") {  //  Baidu
//  } else if (evaluate == "on") {
//    int feature = FALCON_EVAL;
//    ret = fc_license_checkout(feature, 1);
//    printf("\n");
//  } else {
//    printf("\n ERROR: Invalid implementation tool: \"%s\" \n",
//           impl_tool.c_str());
//    exit(-1);
//  }
//  //  } else {
//  //    int feature = FALCON_EVAL;
//  //    ret = fc_license_checkout(feature, 1);
//  //    printf("\n");
//  //  }
//  return ret;
//}
//
//int license_check_in(string impl_tool, string evaluate) {
//  //  if (evaluate == "off") {
//  if ((impl_tool == "sdaccel" || impl_tool == "hls") &&
//      (evaluate == "xilinx" || evaluate == "off")) {  //  XILINX
//    int feature = FALCON_XILINX;
//    fc_license_checkin(feature);
//  } else if (impl_tool == "cheetah") {  //  Baidu
//  } else if (evaluate == "on") {
//    int feature = FALCON_EVAL;
//    fc_license_checkin(feature);
//  } else {
//    printf("\n ERROR: Invalid implementation tool: \"%s\" \n",
//           impl_tool.c_str());
//    exit(-1);
//  }
//  //  } else {
//  //    int feature = FALCON_EVAL;
//  //    fc_license_checkin(feature);
//  //  }
//
//  //    feature = FALCON_CUSTOM;
//  //    fc_license_checkin(feature);
//
//  //  shanh, bug 2310, a valgrind  Mismatched free() / delete / delete []
//  //  happened here fc_license_cleanup();V
//
//  return 0;
//}

int main(int argc, char **argv) {
  if (argc < 3) {
    return 0;
  }
  string tool = argv[1];
  string evaluate = argv[2];
  string commandline = argv[3];
  //    printf("commandline = %s\n", commandline.c_str());
  //license_check_out(tool, evaluate);
  system("touch .license_check_pass");
  commandline = "merlin_core " + commandline;
  system(commandline.c_str());
  //license_check_in(tool, evaluate);
  return 1;
}
