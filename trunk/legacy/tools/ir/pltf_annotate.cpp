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
#include <map>
#include <vector>
#include <string>

#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "pltf_annotate.h"
#include "PolyModel.h"

// #define DEBUG_PRINT

// #define PRINT_DEBUG 0  // define in compilation command line

XML_Object_Collector g_pltf_collector;

map<string, CXMLAnn_PL_Component *> g_map_name2comp;
map<string, CXMLAnn_imp_option> g_map_name2impl;

string CXMLAnn_PL_Component::print() {
  size_t i;
  string str_param = "Component ";
  map<string, string>::iterator it;
  for (it = m_mapParams.begin(); it != m_mapParams.end(); it++) {
    str_param += "[" + it->first + "]=" + it->second + " ";
  }
  str_param += "\n";

  string str_imp;
  for (i = 0; i < m_vecImps.size(); i++) {
    CXMLAnn_imp_option imp_option = m_vecImps[i];
    str_imp = "Implementation : " + imp_option.name + "\n";
    str_imp = str_imp + "  task_name=" + imp_option.task + "\n";
    str_imp = str_imp + "  source_dir=" + imp_option.src_dir + "\n";
    str_imp = str_imp + "  evaluator=" + imp_option.evaluator + "\n";
    str_imp = str_imp + "  generator=" + imp_option.generator + "\n";
    str_imp = str_imp + "  configuration=" + imp_option.generator + "\n";
    str_imp =
        str_imp + "  module_impl_dir=" + imp_option.module_impl_dir + "\n";
    str_imp = str_imp + "  regenerate=" + my_itoa(imp_option.regenerate) + "\n";

    map<string, string>::iterator it;
    for (it = imp_option.metrics.begin(); it != imp_option.metrics.end();
         it++) {
      str_imp += "    " + it->first + "=" + it->second + "\n";
    }
  }

  return str_param + str_imp;
}

void AnalyzeComponent(CXMLNode *pPltfRoot) {
  size_t i;
  size_t j;

  vector<CXMLNode *> vecNodes = pPltfRoot->TraverseByName("component", 2);
  for (j = 0; j < vecNodes.size(); j++) {
    // if (vecNodes[j]->GetDepth() < 2) continue;
    string sCompName = vecNodes[j]->GetParam("name");
    CXMLNode *pCompNode = vecNodes[j];

#ifdef DEBUG_PRINT
    // std::cout << "Component: " << sCompName << endl;
#endif

    if (pCompNode->GetAnnotation("CXMLAnn_PL_Component") != nullptr) {
      continue;
    }
    CXMLAnn_PL_Component *pAnnNode = new CXMLAnn_PL_Component;
    g_pltf_collector.Append(pAnnNode);
    pCompNode->SetAnnotation(pAnnNode);
    g_map_name2comp[pCompNode->GetParam("name")] = pAnnNode;

    CXMLNode *pSubNode;
    pSubNode = pCompNode->GetChildByName("type");
    if (pSubNode != nullptr) {
      pAnnNode->SetParam("type", pSubNode->GetValue());
    }
    pAnnNode->SetParam("name", pCompNode->GetParam("name"));

    {
      CXMLNode *pBaseAddr = pCompNode->GetChildByName("base_addr");
      if (pBaseAddr != nullptr) {
        for (int i = 0; i < pBaseAddr->GetChildNum(); i++) {
          CXMLNode *pSub = pBaseAddr->GetChildByIndex(i);
          pAnnNode->AppendBaseAddr(pSub->GetParam("name"), pSub->GetValue());
        }
      }
    }

    vector<CXMLNode *> vecImps = pCompNode->TraverseByName("implementation");
    for (i = 0; i < vecImps.size(); i++) {
      CXMLNode *pImpNode = vecImps[i];
      CXMLAnn_imp_option imp_option;
      imp_option.name = pImpNode->GetParam("name");
      imp_option.task = pImpNode->GetParam("task");
      CXMLNode *pAttr = pImpNode->GetChildByName("generator");
      if (pAttr != nullptr) {
        imp_option.generator = pAttr->GetValue();
      }
      pAttr = pImpNode->GetChildByName("report");
      if (pAttr != nullptr) {
        imp_option.report = pAttr->GetValue();
      }
      pAttr = pImpNode->GetChildByName("module_impl");
      if (pAttr != nullptr) {
        imp_option.module_impl_dir = pAttr->GetValue();
      } else {
        imp_option.module_impl_dir = "pcores";
      }
      pAttr = pImpNode->GetChildByName("source_dir");
      if (pAttr != nullptr) {
        imp_option.module_impl_dir =
            pAttr->GetValue() + "/" + imp_option.module_impl_dir;
      }
      if (pAttr != nullptr) {
        imp_option.src_dir = pAttr->GetValue();
      }
      pAttr = pImpNode->GetChildByName("evaluator");
      if (pAttr != nullptr) {
        imp_option.evaluator = pAttr->GetValue();
      }

      pAttr = pImpNode->GetChildByName("regenerate");
      if (pAttr != nullptr) {
        imp_option.regenerate = my_atoi(pAttr->GetValue());
      }

      pAttr = pImpNode->GetChildByName("metrics");
      if (pAttr != nullptr) {
        for (int ii = 0; ii < pAttr->GetChildNum(); ii++) {
          CXMLNode *pMetricAttr = pAttr->GetChildByIndex(ii);
          imp_option.metrics[pMetricAttr->GetName()] = pMetricAttr->GetValue();
        }
      }
      pAnnNode->AppendImp(imp_option);
      imp_option.component_name = pCompNode->GetParam("name");
      g_map_name2impl[imp_option.name] = imp_option;
    }

#ifdef DEBUG_PRINT
    std::cout << pAnnNode->print() << endl;
#endif
  }
}

int mark_pltf_annotation_release_resource() {
  g_pltf_collector.CleanAll();
  return 1;
}

int print_pltf_annotation(CXMLNode *pPltfRoot) {
  size_t j;

  vector<CXMLNode *> vecNodes = pPltfRoot->TraverseByName("component", 2);
  for (j = 0; j < vecNodes.size(); j++) {
    // if (vecNodes[j]->GetDepth() < 2) continue;
    string sCompName = vecNodes[j]->GetParam("name");
    CXMLNode *pCompNode = vecNodes[j];

    std::cout << "Component: " << sCompName << endl;

    CXMLAnn_PL_Component *pAnnTemp = dynamic_cast<CXMLAnn_PL_Component *>(
        pCompNode->GetAnnotation("CXMLAnn_PL_Component"));
    if (pAnnTemp != nullptr) {
      std::cout << pAnnTemp->print() << endl;
    }
  }
  return 1;
}

int mark_pltf_annotation(CXMLNode *pPltfRoot) {
  AnalyzeComponent(pPltfRoot);

  return 1;
}

CXMLAnn_PL_Component *get_platform_component_ann_by_name(string sName) {
  return g_map_name2comp[sName];
}

CXMLAnn_imp_option &get_platform_impl_ann_by_name(string sName) {
  return g_map_name2impl[sName];
}

//
// void wrapper_top_gen(CXMLNode *pTLDMRoot, string sTaskName)
//{
//    int i;
//    string sOut;
//
//    // Preparing info
//    vector<string> vecDepNames;
//    vector<int>    vecDepWidth;
//    int            nIterWidth;
//    vector<string> vecPortArrayNames;
//    vector<string> vecPortArrayPositions;
//    vector<int> vecPortAddrWidth;
//    vector<int> vecPortDataWidth;
//    string sTaskBody;
//    vector<string> vecPortBody;
//    map<string, vector<string> > mapBankToPorts;
//
//    {
//        CXMLNode * pTaskDef = SearchNodeNameUnique(pTLDMRoot, "task_def",
// sTaskName);         CXMLAnn_WrapperGen * pTaskAnn =
// dynamic_cast<CXMLAnn_WrapperGen*>(pTaskDef->GetAnnotation("CXMLAnn_WrapperGen"));
//        nIterWidth = pTaskAnn->GetDomain().get_iter_total_bitwidth();
//
//        for (i = 0; i < pTaskAnn->GetDeps().size(); i++)
//        {
//            string sDepName = pTaskAnn->GetDeps()[i].first;
//
//            CXMLNode * pDepDef = SearchNodeNameUnique(pTLDMRoot, "task_def",
// sDepName);             CXMLAnn_WrapperGen * pDepAnn =
// dynamic_cast<CXMLAnn_WrapperGen*>(pDepDef->GetAnnotation("CXMLAnn_WrapperGen"));
//            int nDepLength = pDepAnn->GetDomain().get_iter_total_bitwidth();
//
//            vecDepNames.push_back(sDepName);
//            vecDepWidth.push_back(nDepLength);
//        }
//
//
//        if (1)
//        {
//            for (i = 0; i < pTaskAnn->GetPortInfoNum(); i++)
//            {
//                CXMLAnn_WrapperGen_port_info port_info =
// pTaskAnn->GetPortInfo(i);                 if (port_info.io_type !=
// CXMLAnn_WrapperGen::IOTYPE_ITERATOR)
//                {
//                    vecPortArrayNames.push_back(port_info.array_name);
//                    vecPortArrayPositions.push_back(port_info.array_position);
//                    vecPortAddrWidth.push_back(32); // FIXME: do not have
// bit-width opt
// yet                     vecPortDataWidth.push_back(32); // FIXME: do not have
// bit-width opt yet vecPortBody.push_back(port_info.port_id);
//                }
//            }
//            sTaskBody = pTaskAnn->GetParam("body_id");
//        }
//
//        // mapBankToPorts
//        string sTaskPosition = pTaskAnn->GetParam("position");
//        for (i = 0; i < vecPortArrayNames.size(); i++)
//        {
//            if (vecPortArrayPositions[i] == sTaskPosition)
//            {
//                string bank_name = vecPortArrayNames[i] + "_" +
// vecPortArrayPositions[i];
// mapBankToPorts[bank_name].push_back(vecPortBody[i]);
//            }
//            //printf("task_pos = %s, port_array_pos = %s\n",
// sTaskPosition.c_str(), vecPortArrayPositions[i].c_str());
//        }
//    }
//
//    sOut += "1. task\n";
//    sOut += "//; my @task_names = param_array_define(\"task_name\", ";
//    sOut += "\"" + sTaskName + "\");\n";
//    sOut += "//; my @task_iter_widths =
//    param_array_define(\"task_iter_width\",
// ";     sOut += "\"" + my_itoa(nIterWidth) + "\");\n";     sOut += "//; my
// @dep_task_names = param_array_define(\"dep_task_name\", ";     sOut += "\"";
// for (i = 0; i < vecDepNames.size(); i++)
//    {
//        sOut += vecDepNames[i];
//        if (i != vecDepNames.size() - 1) sOut += " ";
//    }
//    sOut += "\");\n";
//    sOut += "//; my @dep_iter_widths = param_array_define(\"dep_iter_width\",
//    "; sOut += "\""; for (i = 0; i < vecDepWidth.size(); i++)
//    {
//        sOut += my_itoa(vecDepWidth[i]);
//        if (i != vecDepWidth.size() - 1) sOut += " ";
//    }
//    sOut += "\");\n";
//
//
//    sOut += "1.1. task ports\n";
//
//    sOut += "//; my @task_port_names = param_array_define(\"port_names\", ";
//    sOut += "\"";
//    for (i = 0; i < vecPortBody.size(); i++)
//    {
//        sOut += vecPortBody[i]; //"p" + my_itoa(i) + "_" +
// my_itoa(vecPortArrayNames[i]);         if (i != vecPortBody.size() - 1) sOut
// +=
// "
// ";
//    }
//    sOut += "\");\n";
//
//    sOut += "//; my @task_port_data_widths =
//    param_array_define(\"port_dwidth\",
//";     sOut += "\"";     for (i = 0; i < vecPortDataWidth.size(); i++)
//    {
//        sOut += my_itoa(vecPortDataWidth[i]);
//        if (i != vecPortDataWidth.size() - 1) sOut += " ";
//    }
//    sOut += "\");\n";
//
//    sOut += "//; my @task_port_addr_widths =
//    param_array_define(\"port_awidth\",
//";     sOut += "\"";     for (i = 0; i < vecPortAddrWidth.size(); i++)
//    {
//        sOut += my_itoa(vecPortAddrWidth[i]);
//        if (i != vecPortAddrWidth.size() - 1) sOut += " ";
//    }
//    sOut += "\");\n";
//
//    sOut += "//; my @task_port_body_names =
// param_array_define(\"port_body_names\", ";     sOut += "\"";     for (i = 0;
// i < vecPortBody.size(); i++)
//    {
//        sOut += vecPortBody[i];
//        if (i != vecPortBody.size() - 1) sOut += " ";
//    }
//    sOut += "\");\n";
//
//    sOut += "//; my @task_body_name = param_array_define(\"task_body_name\",
//    "; sOut += "\"" + sTaskBody + "\");\n";
//
//    sOut += "//; my @task_bank_names = param_array_define(\"bank_name\", ";
//    sOut += "\"";
//    map<string, vector<string> >::iterator it;
//    for (it = mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += it->first;
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += " ";
//    }
//    sOut += "\");\n";
//
//    sOut += "//; my @task_bank_port_names =
// param_array_define(\"bank_port_name\", ";     sOut += "\"";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            sOut += vecPorts[i];
//            if (i != vecPorts.size()-1) sOut += " ";
//        }
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ",";
//    }
//    sOut += "\");\n";
//
//    sOut += "2. local memory bank \n";
//    //bank_names
//    sOut += "//; my @bank_names = param_array_define(\"local_bank_name\", ";
//    for (it = mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"" + it->first + "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//    sOut += "//; my @bank_port_names =
// param_array_define(\"local_bank_port_name\", ";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            sOut += vecPorts[i];
//            if (i != vecPorts.size()-1) sOut += " ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//    sOut += "//; my @bank_port_awidths =
// param_array_define(\"local_bank_port_awidth\", ";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            int j;
//            for (j = 0; j < vecPortBody.size(); j++) if (vecPortBody[j] ==
// vecPorts[i]) break;             if (j < vecPortBody.size()) sOut +=
// my_itoa(vecPortAddrWidth[j]);             if (i != vecPorts.size()-1) sOut +=
// "
// ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//    sOut += "//; my @bank_port_dwidths =
// param_array_define(\"local_bank_port_dwidth\", ";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            int j;
//            for (j = 0; j < vecPortBody.size(); j++) if (vecPortBody[j] ==
// vecPorts[i]) break;             if (j < vecPortBody.size()) sOut +=
// my_itoa(vecPortDataWidth[j]);             if (i != vecPorts.size()-1) sOut +=
// "
// ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//
//
//
//    //TODO
//
//
//    sOut += "//; my @bank_port_local_conditions =
// param_array_define(\"local_bank_port_local_condition\", ";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            sOut += "1";
//            if (i != vecPorts.size()-1) sOut += " ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//
//    sOut += "//; my @bank_port_local_addrs =
// param_array_define(\"local_bank_port_local_addr\", ";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            sOut += "req_addr";
//            if (i != vecPorts.size()-1) sOut += " ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//
//    sOut += "//; my @bank_port_remote_addrs =
// param_array_define(\"local_bank_port_remote_addr\", ";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            sOut += "req_addr";
//            if (i != vecPorts.size()-1) sOut += " ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//
//    sOut += "//; my @bank_size = param_array_define(\"local_bank_size\", ";
//    for (it = mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            sOut += "1024";
//            if (i != vecPorts.size()-1) sOut += " ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//    sOut += "//; my @bank_bus_slave_addr =
// param_array_define(\"local_bank_slave_addr\", ";     for (it =
// mapBankToPorts.begin(); it != mapBankToPorts.end(); it++)
//    {
//        sOut += "\"";
//        vector<string> vecPorts = it->second;
//        for (i = 0; i < vecPorts.size(); i++)
//        {
//            sOut += "req_addr";
//            if (i != vecPorts.size()-1) sOut += " ";
//        }
//        sOut += "\"";
//        map<string, vector<string> >::iterator it1 = it; it1++;
//        if (it1 != mapBankToPorts.end()) sOut += ", ";
//    }
//    sOut += ");\n";
//
//    sOut += string("//; my $bus_data_width = param_define(\"bus_dwidth\",
// \"32\");\n");     sOut += string("//; my $bus_addr_width =
// param_define(\"bus_awidth\", \"32\");\n");
//
//    string sTopFileName = "task_" + sTaskName + "_p";
//    write_string_into_file(sOut, sTopFileName);
//}
//
