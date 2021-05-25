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



#include "cmdline_parser.h"
#include "xml_parser.h"
#include "file_parser.h"

// 1. cfg.xml to perl definitions

// void set_cfg(CInputOptions & options)
// {
//     int i;
//
//     string sXMLFile = options.get_option("", 1);
//
//     vector<string> vecFields;
//     for (i = 2; i < options.get_option_num(""); i++)
//     {
//         vecFields.push_back(options.get_option("", i));
//     }
//
//     string sOut = "";
//
//     // configurations
//     CXMLParser xml_parser;
//     CXMLNode * pRoot = xml_parser.parse_from_file(sXMLFile);
//
//     CXMLNode * pNode = pRoot;
//     i = 0;
//     while (1)
//     {
//         int j = 0;
//         string sName       = get_sub_delimited(vecFields[i], j, ':');
// j+=sName.size() + 1;         string sParam      =
// get_sub_delimited(vecFields[i], j,
// ':'); j+=sParam.size() + 1;         string sParamValue =
// get_sub_delimited(vecFields[i], j, ':'); j+=sParamValue.size() + 1;;
//
//         sParamValue = RemoveQuote(sParamValue);
//
//         vector<CXMLNode *> ConfigNodes = pNode->TraverseByName(sName);
//         if (ConfigNodes.size() == 0) break;
//
//         for (j = 0; j < ConfigNodes.size(); j++)
//         {
//             pNode = ConfigNodes[j]; // only modify the first match
//             if (sParam == "" || pNode->GetParam(sParam) == sParamValue)
//             break;
//         }
//         if (j == ConfigNodes.size()) break;
//
//         if (i == vecFields.size()-2)
//         {
//             if (options.get_option("-i", 0) == string("set"))
// pNode->SetValue(vecFields[i+1]);             else printf("%s\n",
// pNode->GetValue().c_str());             break;
//         }
//         i++;
//     }
//     if (options.get_option("-i", 0) == string("set"))
// xml_parser.write_into_file(sXMLFile);
// }

int main(int argc, char *argv[]) {
  CInputOptions options;
  options.set_flag("", 10, 3);
  options.set_flag("-i", 1, 0);

  if (options.parse(argc, argv) == 0) {
    printf("Usage: set_cfg [-i get|set] cfg_xml hier1 hier2 ... value\n");
    printf("    Set/Get the configuration xxx/hier1/xxx/hier2/.../hierx into "
           "value.\n");
    printf("    each hier can be \"name\" or \"name:param:param_value\"\n");
    exit(1);
  }

  // default values;
  if (0 == options.get_option_num("-i")) {
    options.add_option("-i", "set");
  }

  string sOut = set_xml_cfg(options);
  if (!sOut.empty()) {
    printf("%s\n", sOut.c_str());
  }
  return 0;
}
