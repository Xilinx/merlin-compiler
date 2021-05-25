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



#ifndef TRUNK_LEGACY_TOOLS_IR_PLTF_ANNOTATE_H_
#define TRUNK_LEGACY_TOOLS_IR_PLTF_ANNOTATE_H_

#include <map>
#include <vector>
#include <string>
#include "cmdline_parser.h"
#include "xml_parser.h"

typedef struct CXMLAnn_imp_option__ {
  string name;
  string task;
  string generator;
  string report;
  string module_impl_dir;
  string evaluator;
  string src_dir;
  map<string, string> metrics;
  int regenerate;  // 0 for output, 1 for input, 2 for iterator
  string component_name;
} CXMLAnn_imp_option;

class CXMLAnn_PL_Component : public CXMLNodeAnnotationBase {
 public:
  // enum
  // {
  //   IOTYPE_OUTPUT = 0,
  //   IOTYPE_INPUT,
  //   IOTYPE_ITERATOR
  // };

 public:
  virtual string GetClassString() { return "CXMLAnn_PL_Component"; }

  void SetParam(string key, string val) { m_mapParams[key] = val; }
  string GetParam(string key) {
    if (m_mapParams.end() == m_mapParams.find(key))
      return "";
    else
      return m_mapParams[key];
  }

  void AppendImp(CXMLAnn_imp_option imp) { m_vecImps.push_back(imp); }
  int GetImpNum() { return m_vecImps.size(); }
  CXMLAnn_imp_option &GetImp(int i) { return m_vecImps[i]; }

  string print();

  void AppendBaseAddr(string port_name, string addr) {
    m_base_addr[port_name] = addr;
  }
  string GetBaseAddr(string port_name) { return m_base_addr[port_name]; }

 protected:
  // Scalar parameters
  map<string, string> m_mapParams;

  map<string, string> m_base_addr;

  vector<CXMLAnn_imp_option> m_vecImps;
};

int mark_pltf_annotation(CXMLNode *pPltfRoot);
int print_pltf_annotation(CXMLNode *pPltfRoot);
int mark_pltf_annotation_release_resource();
CXMLAnn_PL_Component *get_platform_component_ann_by_name(string sName);
CXMLAnn_imp_option &get_platform_impl_ann_by_name(string sName);

#endif  // TRUNK_LEGACY_TOOLS_IR_PLTF_ANNOTATE_H_
