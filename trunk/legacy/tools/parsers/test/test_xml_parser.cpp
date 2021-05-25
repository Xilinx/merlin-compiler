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


#include <string>
using std::string;

#include "../src/xml_parser.h"

int test_main(char *szFile1, char *szFile2) {
  // system("echo here is test_main");

  CXMLParser platform;
  platform.parse_from_file(string(szFile1));
  platform.write_into_file(string(szFile2));
  return 1;
}

int main(int argc, char *argv[]) {
  printf("Test Starting...\n");
  test_main(argv[1], argv[2]);
  printf("Test End.\n");
  return 0;
}
