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

#include "tldm_parser.h"
#include "file_parser.h"

int g_debug_print;

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    printf("Usaage: $0 tldm_in xml_out [-s]\n");
    printf("          -s : print debug info \n");
    exit(-1);
  }
  string sFileIn = string(argv[1]);
  string sFileOut = string(argv[2]);
  g_debug_print = static_cast<int>((argc == 4) && ("-s" == string(argv[3])));

  CTLDMParser xml_parser;
  g_xml_tree_for_tldm_parse = xml_parser.get_xml_tree();

  yy_main(sFileIn);

  string sXML =
      xml_parser.dump_element_to_string(g_xml_tree_for_tldm_parse->getRoot());
  write_string_into_file(sXML, sFileOut);

  if (g_debug_print != 0) {
    std::cout << sXML;
  }

  return 0;
}

void show_node(CXMLNode *pNode) {
#define MY_DEBUG 1
#ifdef MY_DEBUG
  if (g_debug_print != 0) {
    CTLDMParser xml_parser;
    string sxml = xml_parser.dump_element_to_string(pNode);
    fprintf(stdout, "\n======%s\n=======\n", sxml.c_str());
  }
#endif
}
