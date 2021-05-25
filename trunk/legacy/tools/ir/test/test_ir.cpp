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



#include "pltf_annotate.h"
#include "tldm_annotate.h"
#include "file_parser.h"

int test_core(CXMLNode *pTLDM_in, CXMLNode *pPltf_in, int b_tldm,
              int b_platform) {
  if (b_tldm != 0) {
    mark_tldm_annotation(pTLDM_in);
    print_tldm_annotation(pTLDM_in);
    mark_tldm_annotation_release_resource();
  }

  if (b_platform != 0) {
    mark_pltf_annotation(pPltf_in);
    print_pltf_annotation(pPltf_in);
    mark_pltf_annotation_release_resource();
  }

  return 1;
}

int main(int argc, char *argv[]) {
  CInputOptions options;
  options.set_flag("", 2);
  options.set_flag("-p", 1, 0);

  if (options.parse(argc, argv) == 0) {
    printf("Usage: test_ir tldm_xml [-p platform_xml] \n");
    exit(0);
  }

  string sTLDMin = options.get_option("", 1);
  string sPlatformin = options.get_option("-p");

  CXMLParser parser;
  CXMLNode *pTLDM_in =
      (!sTLDMin.empty()) ? parser.parse_from_file(sTLDMin) : nullptr;

  CXMLParser parser1;
  CXMLNode *pPltf_in =
      (!sPlatformin.empty()) ? parser1.parse_from_file(sPlatformin) : nullptr;

  test_core(pTLDM_in, pPltf_in, static_cast<int>(nullptr != pTLDM_in),
            static_cast<int>(nullptr != pPltf_in));

  return 0;
}
