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


#include <stdlib.h>
#include <fstream>
#include <algorithm>

#include <string>
#include <set>
using std::string;

// #include "../src/xml_parser.h"
#include "xml_parser.h"
#include "tldm_annotate.h"

#include "iostream"
#include "header.h"

// application specific values
// #define GRAPH_UNROLL_FACTOR 8 // by default, unroll 8 copies
#define GRAPH_UNROLL_FACTOR 128  // by default, unroll 8 copies

// int test_main(char * szFile1, char * szFile2)

bool checkTaskIsWrapper(CXMLNode *xmlNode, string taskName) {
  vector<CXMLNode *> nodeVecTask = xmlNode->TraverseByName("task_def", 2);
  for (size_t i = 0; i < nodeVecTask.size(); i++) {
    // skip the top level task
    if (nodeVecTask[i]->GetLevel() == 1) {
      continue;
    }

    string sTaskName;
    sTaskName = nodeVecTask[i]->GetParam("name");
    if (sTaskName == taskName) {
      return tldm_xml_get_attribute(nodeVecTask[i], "module_type") == "wrapper";
    }
  }
  return false;
}

double getTaskIOTime(CXMLNode *pTLDMRoot, string sTaskName) {
  CXMLAnn_WrapperGen *pTaskAnn = get_tldm_task_ann_by_name(sTaskName);
  int nofPorts = pTaskAnn->GetPortInfoNum();
  double nofAccess = 0;

  //  cout << "Task " << sTaskName << ": " << endl; // <<
  //  mergedPolyInfo.access_pattern << endl;
  for (int i = 0; i < nofPorts; i++) {
    tldm_polyhedral_info portPolyInfo = pTaskAnn->GetPortInfo(i).poly_info;
    if (portPolyInfo.properties["port_type"] != "bfifo") {
      continue;
    }
    tldm_polyhedral_info taskAndPortPolyInfo =
        tldm_polyinfo_merge(pTaskAnn->poly_info, portPolyInfo);
    tldm_polyhedral_info mergedPolyInfo = taskAndPortPolyInfo;
    //    tldm_polyhedral_info mergedPolyInfo = tldm_polyinfo_merge(pTaskAnn ->
    //    GetParent() -> poly_info, taskAndPortPolyInfo);

    vector<tldm_variable_range> vecVarRange =
        mergedPolyInfo.get_variable_range();
    string iteratorVector = mergedPolyInfo.iterator_vector;
    vector<string> loopIterators = str_split(iteratorVector, ',');

    // cout << iteratorVector << endl;

    /////////////////////////////////////////////////////////////////
    // ZP: 2014-02-19 // exclude outer loop iterations in the access calculation
    vector<string> outer_iter =
        str_split(pTaskAnn->poly_info.iterator_vector, ',');
    vector<string> params = str_split(mergedPolyInfo.parameter_vector, ',');
    vector<string> total_param = str_vector_add(outer_iter, params);
    for (size_t k = 0; k < vecVarRange.size(); k++) {
      if (-1 != str_vector_find(total_param, vecVarRange[k].variable)) {
        vecVarRange[k].lb = "0";
        vecVarRange[k].ub = "0";
      }
    }
    /////////////////////////////////////////////////////////////////

    double mul_range = 1;
    for (size_t j = 0; j < loopIterators.size(); j++) {
      tldm_polynomial onePoly = tldm_polynomial(loopIterators[j], vecVarRange);
      int lb;
      int ub;
      assert(onePoly.get_bound_const(lb, ub));
      int range = ub - lb + 1;
      mul_range *= static_cast<double>(range);
      //      cout << loopIterators[j] << ", lb " << lb << ", ub " << ub << ",
      //      range " << ub - lb + 1 << " mul_range " << mul_range << endl;
    }
    //    cout << "number of access: " << mul_range << endl;

    nofAccess += mul_range;
  }
  return FPGACyclePerAccess * nofAccess;
}

int test_main(char *szApp, char *szPlatform, char *szUcf, char *szSDF) {
  // system("echo here is test_main");
  std::ofstream sdfFile;
  sdfFile.open(szSDF);

  bool mod_sel_flag = false;
  bool mod_dup_flag = false;
  string platform_name;

  //============= Parse User Directive =============================
  //================================================================
  CXMLParser ucf;
  ucf.parse_from_file(string(szUcf));

  CXMLNode *xmlNodeUcf = ucf.get_xml_tree()->getRoot();
  if (xmlNodeUcf->GetChildByName("module_selection")->GetValue() == "on") {
    mod_sel_flag = true;
  }
  if (xmlNodeUcf->GetChildByName("module_duplication")->GetValue() == "on") {
    mod_dup_flag = true;
  }
  platform_name = xmlNodeUcf->GetChildByName("platform_name")->GetValue();
  //================================================================

  CXMLParser application;
  application.parse_from_file(string(szApp));

  CXMLNode *xmlNode = application.get_xml_tree()->getRoot();
  mark_tldm_annotation(xmlNode);
  dump_task_pragma("polyinfo_modsel.rpt");

  // sdfFile << xmlNode -> GetName() << endl;
  // sdfFile << xmlNode -> GetValue() << endl;

  vector<CXMLNode *> nodeVecTask = xmlNode->TraverseByName("task_def", 2);

  vector<string> mTaskNameList;
  for (size_t i = 0; i < nodeVecTask.size(); i++) {
    string sTaskName;
    // skip the top level task
    if (nodeVecTask[i]->GetLevel() == 1) {
      continue;
    }

    sTaskName = nodeVecTask[i]->GetParam("name");
    // skip the input and output
    if (sTaskName == "INPUT" || sTaskName == "OUTPUT") {
      continue;
    }
    if (tldm_xml_get_attribute(nodeVecTask[i], "type") == "graph") {
      continue;
    }

    // skip gid, ldu, etc.
    if (tldm_xml_get_attribute(nodeVecTask[i], "module_type") != "wrapper") {
      continue;
    }
    mTaskNameList.push_back(sTaskName);
  }

  map<string, int>
      mTaskName2NofInstance;  // number of this task in the unrolled sdf graph
  map<string, int> mTaskName2MaxDup;
  map<string, int> mTaskName2PCF;  // performance compensate factor

  sdfFile << "@TASK_LIST {" << endl;

  int scaling_factor = 1;
  for (size_t i = 0; i < nodeVecTask.size(); i++) {
    string sTaskName;
    // skip the top level task
    if (nodeVecTask[i]->GetLevel() == 1) {
      continue;
    }

    // skip non-related tasks
    sTaskName = nodeVecTask[i]->GetParam("name");
    if (find(mTaskNameList.begin(), mTaskNameList.end(), sTaskName) ==
        mTaskNameList.end()) {
      continue;
    }

    // get number of task instances
    CXMLAnn_WrapperGen *tmpTaskAnn = get_tldm_task_ann_by_name(sTaskName);
    unsigned nof_instance = my_atoi(tmpTaskAnn->GetParam(
        "sdf_iter_num"));  // the graph needs to be executed
                           // that amount of iterations

    int unroll_factor = static_cast<int>(nof_instance < GRAPH_UNROLL_FACTOR
                                             ? nof_instance
                                             : GRAPH_UNROLL_FACTOR);
    mTaskName2NofInstance[sTaskName] = unroll_factor;
    mTaskName2PCF[sTaskName] =
        (nof_instance / unroll_factor) +
        ((nof_instance % unroll_factor) == 0
             ? 0
             : 1);  // ceiling(nof_instances/unroll_factor)

    // get maximal allowed number of replicas
    int max_dup = unroll_factor;  // default is the same as the unroll factor

    if (tldm_xml_get_attribute(nodeVecTask[i], "module_type") == "wrapper") {
      CXMLNode *nodeRate =
          SearchNodeNameUnique(nodeVecTask[i], "attribute",
                               "max_dup_factor");  // sdf_related parameter
      unsigned user_defined_max_dup;
      if (nodeRate != nullptr) {
        user_defined_max_dup =
            my_atoi(nodeRate->GetChildByIndex(0)->GetValue());
        if (user_defined_max_dup > 1 && !mod_dup_flag) {
          cout << "-------tldm_2_sdf parser ERROR:--------" << endl;
          cout << "module duplication is turned off" << endl;
          cout << "cannot specify maximum duplication factor of "
               << user_defined_max_dup << " for task " << sTaskName;
        }
        if (user_defined_max_dup > unroll_factor) {
          cout << "-------tldm_2_sdf parser ERROR:--------" << endl;
          cout << "Task: " << sTaskName << "\tuser spedicifed dup_factor"
               << user_defined_max_dup << endl;
          cout << "unroll factor: " << unroll_factor << endl;
          assert(0);
        }
        max_dup = static_cast<int>(user_defined_max_dup);
      }
    }

    if (!mod_dup_flag) {
      max_dup = 1;
    }
    mTaskName2MaxDup[sTaskName] = max_dup;

    sdfFile << "\tTASK " << sTaskName << " MAX_IMPL " << max_dup;

    // get user specified replicas
    if (tldm_xml_get_attribute(nodeVecTask[i], "module_type") == "wrapper") {
      CXMLNode *nodeRate = SearchNodeNameUnique(
          nodeVecTask[i], "attribute", "dup_factor");  // sdf_related parameter
      unsigned user_defined_dup;
      if (nodeRate != nullptr) {
        user_defined_dup = my_atoi(nodeRate->GetChildByIndex(0)->GetValue());
        if (user_defined_dup > 1 && !mod_dup_flag) {
          cout << "-------tldm_2_sdf parser ERROR:--------" << endl;
          cout << "module duplication is turned off" << endl;
          cout << "cannot specify duplication factor of " << user_defined_dup
               << " for task " << sTaskName;
        }
        if (user_defined_dup > unroll_factor) {
          cout << "-------tldm_2_sdf parser ERROR:--------" << endl;
          cout << "Task: " << sTaskName << "\tuser spedicifed dup_factor"
               << user_defined_dup << endl;
          cout << "unroll factor: " << unroll_factor << endl;
          assert(0);
        }
        sdfFile << " FIX_IMPL " << user_defined_dup;
      }
    }

    unsigned group_id = my_atoi(tmpTaskAnn->GetParam("sdf_group_id"));
    unsigned group_task_id = my_atoi(tmpTaskAnn->GetParam("sdf_task_id"));
    // unsigned iter_num  = my_atoi( tmpTaskAnn -> GetParam("sdf_iter_num") );
    unsigned iter_num = 10;  // my_atoi( tmpTaskAnn -> GetParam("sdf_iter_num")
                             // );

    // TODO(youxiang): ZP: calculate the ITER_NUM
    // 1. calculate the scaling factor
    // 2. scale the iter_num using the factor

    sdfFile << " GROUP_ID " << group_id << " GROUPINNER_ID " << group_task_id
            << " ITER_NUM " << iter_num;  // << nof_instance;

    sdfFile << endl;
  }
  sdfFile << "}" << endl;
  // exit(0);

  sdfFile << "@SCALING_FACTOR ";
  sdfFile << scaling_factor << endl;

  // parse fixed implementation
  map<string, string> mTaskName2FixImpl;
  for (size_t i = 0; i < nodeVecTask.size(); i++) {
    string sTaskName;
    // skip the top level task
    if (nodeVecTask[i]->GetLevel() == 1) {
      continue;
    }

    // skip non-related tasks
    sTaskName = nodeVecTask[i]->GetParam("name");
    if (find(mTaskNameList.begin(), mTaskNameList.end(), sTaskName) ==
        mTaskNameList.end()) {
      continue;
    }

    // add to map from name -> fixed implementation
    string fixImpl = nodeVecTask[i]->GetParam("position");
    if (!fixImpl.empty()) {
      mTaskName2FixImpl[sTaskName] = fixImpl;
    }
  }

#if 1

  map<string, int> mTaskName2Gdus;
  for (size_t i = 0; i < mTaskNameList.size(); i++) {
    mTaskName2Gdus[mTaskNameList[i]] = 0;
  }

  // dependency
  vector<CTldmDependenceAnn *> vectorDepAnn = get_tldm_dependence_set();

  // count number of gdus for each task
  for (size_t i = 0; i < vectorDepAnn.size(); i++) {
    CTldmDependenceAnn *depAnn = vectorDepAnn[i];

    // step 0: get polyhedral_info
    CXMLAnn_WrapperGen *pTask0 = depAnn->GetTask0();
    CXMLAnn_WrapperGen *pTask1 = depAnn->GetTask1();
    assert(pTask0);
    assert(pTask1);

    string proTaskName;
    string conTaskName;
    string dataName;
    tldm_polyhedral_info poly_info_0 = pTask0->poly_info;
    tldm_polyhedral_info poly_info_1 = pTask1->poly_info;

    proTaskName = poly_info_0.name;
    conTaskName = poly_info_1.name;
    // proTaskName = pTask0 -> poly_info.name;
    // conTaskName = pTask1 -> poly_info.name;
    // dataName = poly_info_0.name;
    //        if (nport0_info.port_type != "bfifo" || nport1_info.port_type !=
    // "bfifo")             continue; if (poly_info_0.properties["port_type"] !=
    // "bfifo" ||
    // poly_info_1.properties["port_type"] != "bfifo")     continue;

    cout << proTaskName << " --> " << conTaskName << endl;

    if (poly_info_0.properties["module_type"] != "wrapper" &&
        poly_info_1.properties["module_type"] == "wrapper") {
      mTaskName2Gdus[conTaskName]++;
    }
    if (poly_info_0.properties["module_type"] == "wrapper" &&
        poly_info_1.properties["module_type"] != "wrapper") {
      mTaskName2Gdus[proTaskName]++;
    }
    //        if (pTask0 ->)
    // cout << "dataName: " << dataName << endl;
  }
  for (map<string, int>::iterator iter = mTaskName2Gdus.begin();
       iter != mTaskName2Gdus.end(); iter++) {
    cout << iter->first << ": " << iter->second << endl;
  }

#endif

#if 1
  //============= Parse Platform Infomation =============================
  //============= Get implementation lib ================================
  CXMLParser platform;
  platform.parse_from_file(string(szPlatform));

  CXMLNode *xmlNodePlat = platform.get_xml_tree()->getRoot();
  vector<CXMLNode *> nodeVecComponent =
      xmlNodePlat->TraverseByName("component");

  // get IOTime of each task
  map<string, double> mTaskName2IOTime;
  for (size_t i = 0; i < mTaskNameList.size(); i++) {
    mTaskName2IOTime[mTaskNameList[i]] =
        getTaskIOTime(xmlNode, mTaskNameList[i]);
  }

  map<string, int> mTaskName2Version;
  sdfFile << "@LIBRARY {" << endl;
  for (size_t i = 0; i < nodeVecComponent.size(); i++) {
    // skip non-acc component
    string sType;
    if (nodeVecComponent[i]->GetChildByName("type") != NULL) {
      sType = nodeVecComponent[i]->GetChildByName("type")->GetValue();
    } else {
      continue;
    }
    if (sType != "Accelerator" && sType != "Computation") {
      continue;
    }
    string mComponentName = nodeVecComponent[i]->GetParam("name");

    vector<CXMLNode *> nodeVecImpl =
        nodeVecComponent[i]->TraverseByName("implementation");
    for (size_t j = 0; j < nodeVecImpl.size(); j++) {
      string mImplName = nodeVecImpl[j]->GetParam("name");
      string mTaskName = nodeVecImpl[j]->GetParam("task");

      // cannot find the implementation in current application
      if (find(mTaskNameList.begin(), mTaskNameList.end(), mTaskName) ==
          mTaskNameList.end()) {
        continue;
      }

      // if this task has fixed implementation, skip other implementations
      if (!mTaskName2FixImpl[mTaskName].empty()) {  // user specified selection
        if (mTaskName2FixImpl[mTaskName] != mImplName) {
          continue;
        }
      }

      if (!mod_sel_flag) {  // user doesn't specify, mod_sel off, then use hw0
        if (mImplName.substr(mImplName.size() - strlen("hw_imp0"),
                             mImplName.size()) != "hw_imp0") {
          continue;
        }
      }

      // FIXME: Peng Zhang
      if (platform_name == "zed" && mComponentName == "microblaze") {
        continue;
      }
      if (platform_name == "ml605" && mComponentName == "arm_a9") {
        continue;
      }

      // FIXME: Peng Zhang 2013-02-10
      if (sType == "Computation") {
        //                if (mod_dup_flag == false || platform_name !="zed") //
        // one core
        //                {
        if (mImplName.substr(mImplName.size() - 7, mImplName.size()) !=
            "sw_imp0") {
          continue;
        }
        //                }
        //
        //                else // the case we have two cores
        //                {
        //                    char * task_list[] = {"parser", "dequant",
        // "idct_row",
        // "idct_col", "cc_mc", "tu", "denoise"};                     int k;
        // for (k = 0; k < 7; k++)
        //                    {
        //                        string s_task_name = string(task_list[k]);
        //                        if (mImplName.find(s_task_name) == 0) break;
        //                    }
        //                    assert(k < 7);
        //                    string imp_suffix = (k&1) ? "sw_imp1" : "sw_imp0";
        //                    if (mImplName.substr(mImplName.size()-7,
        // mImplName.size())
        //!= imp_suffix)                         continue;
        //                }
      }

      int version;

      // get version, start from 0
      if (mTaskName2Version.find(mTaskName) == mTaskName2Version.end()) {
        version = 0;
        mTaskName2Version[mTaskName] = 0;
      } else {
        version = mTaskName2Version[mTaskName] + 1;
        mTaskName2Version[mTaskName] = version;
      }

      // get pcf
      int pcf = mTaskName2PCF[mTaskName];  // performance compensate factor

      sdfFile << "\tTASK " << mTaskName << " VERSION " << version << " ";

      CXMLNode *nodeMetric = nodeVecImpl[j]->GetChildByName("metrics");
      double exeTime;
      int areaCost;
      // int initInterval;

      exeTime = my_atoi((nodeMetric->GetChildByName("worst_lat")->GetValue()));
      double ioTime = mTaskName2IOTime[mTaskName];
      if (mImplName.substr(mImplName.size() - 7, mImplName.size()) !=
          "sw_imp0") {
        exeTime = exeTime > ioTime ? exeTime : ioTime;
      }

      exeTime *= pcf;
      sdfFile << "EXETIME " << exeTime << " ";

      sdfFile << "II " << exeTime << " ";

      int nofFF;
      int nofLUT;
      if (mImplName.substr(mImplName.size() - 7, mImplName.size()) ==
          "sw_imp0") {
        nofFF = my_atoi(nodeMetric->GetChildByName("num_FF")->GetValue());
        nofLUT = my_atoi(nodeMetric->GetChildByName("num_LUT")->GetValue());
      } else {
        nofFF = my_atoi(nodeMetric->GetChildByName("num_FF")->GetValue()) +
                mTaskName2Gdus[mTaskName] * GDU_FF;
        nofLUT = my_atoi(nodeMetric->GetChildByName("num_LUT")->GetValue()) +
                 mTaskName2Gdus[mTaskName] * GDU_LUT;
      }
      areaCost = (nofFF / 2 > nofLUT) ? nofFF / 2 : nofLUT;
      sdfFile << "AREA " << areaCost << " ";

      int bramSize =
          my_atoi((nodeMetric->GetChildByName("num_BRAM")->GetValue())) * 16 *
          1024 / 32;  // 18kb --> int
      // if (mImplName.substr(mImplName.size()-7, mImplName.size()) !=
      // "sw_imp0")     bramSize += mTaskName2Gdus[mTaskName] * GDU_BRAM;

      sdfFile << "BRAM " << bramSize << " ";

      int nofDSP = my_atoi((nodeMetric->GetChildByName("num_DSP")->GetValue()));
      if (mImplName.substr(mImplName.size() - 7, mImplName.size()) !=
          "sw_imp0") {
        nofDSP += mTaskName2Gdus[mTaskName] * GDU_DSP;
      }
      sdfFile << "DSP " << nofDSP << " ";

      sdfFile << "IMPLNAME " << mImplName << " ";
      sdfFile << endl;
    }
  }
  sdfFile << "}" << endl;

#endif

  //============= SDFG Infomation =============================
  //===========================================================
  sdfFile << "@HSDFG 0 {" << endl;
  vector<CXMLNode *> nodeVecConnect = xmlNode->TraverseByName("connect");

  // int nofEdges = 0;
  //==== SDFG NODES ===================
  for (size_t i = 0; i < nodeVecTask.size(); i++) {
    string sTaskName;
    string sNodeName;
    // skip the top level task
    if (nodeVecTask[i]->GetLevel() == 1) {
      continue;
    }

    sTaskName = nodeVecTask[i]->GetParam("name");
    if (find(mTaskNameList.begin(), mTaskNameList.end(), sTaskName) ==
        mTaskNameList.end()) {
      continue;
    }

    sNodeName = sTaskName;
    for (int j = 0; j < mTaskName2NofInstance[sTaskName]; j++) {
      sdfFile << "\tNODE " << sNodeName << "_" << j << " TASK " << sTaskName
              << " LABLE " << j << endl;
    }
  }
  sdfFile << endl << endl;

  //==== SDFG ARCs ===================
  // vector<string> argsRate; argsRate.push_back("rate");
  //
  //
  std::set<string> parsedConnect;
  std::set<string>::iterator ssIter;
  parsedConnect.clear();

  int edgeNum = 0;
  for (size_t i = 0; i < vectorDepAnn.size(); i++) {
    CTldmDependenceAnn *depAnn = vectorDepAnn[i];

    // step 0: get polyhedral_info
    CXMLAnn_WrapperGen *pTask0 = depAnn->GetTask0();
    CXMLAnn_WrapperGen *pTask1 = depAnn->GetTask1();
    assert(pTask0);
    assert(pTask1);

    string proTaskName;
    string conTaskName;
    string dataName;
    tldm_polyhedral_info poly_info_0 = pTask0->poly_info;
    tldm_polyhedral_info poly_info_1 = pTask1->poly_info;

    proTaskName = poly_info_0.name;
    conTaskName = poly_info_1.name;

    dataName = pTask0->GetPortInfo(depAnn->GetPort0()).poly_info.name;

    // proTaskName = pTask0 -> poly_info.name;
    // conTaskName = pTask1 -> poly_info.name;
    // dataName = poly_info_0.name;
    //        if (nport0_info.port_type != "bfifo" || nport1_info.port_type !=
    // "bfifo")             continue; if (poly_info_0.properties["port_type"] !=
    // "bfifo" ||
    // poly_info_1.properties["port_type"] != "bfifo")     continue;

    //        cout << proTaskName << " --> " << conTaskName << endl;

    if (proTaskName != conTaskName) {
      if (find(mTaskNameList.begin(), mTaskNameList.end(), proTaskName) ==
          mTaskNameList.end()) {
        continue;
      }
      if (find(mTaskNameList.begin(), mTaskNameList.end(), conTaskName) ==
          mTaskNameList.end()) {
        continue;
      }

      assert(mTaskName2NofInstance[proTaskName] ==
             mTaskName2NofInstance[conTaskName]);

      // check they should be in the same graph???

      for (int i = 0; i < mTaskName2NofInstance[proTaskName]; i++) {
        sdfFile << "\tARC " << dataName << "_" << edgeNum++ << " FROM "
                << proTaskName << "_" << i;
        sdfFile << " TO " << conTaskName << "_" << i;
        sdfFile << " TYPE 2" << endl;
      }
    }
  }
  // cross-Graph edges
  // has assume only one task in each graph
  for (size_t i = 0; i < mTaskNameList.size(); i++) {
    for (size_t j = 0; j < mTaskNameList.size(); j++) {
      if (i >= j) {
        continue;
      }
      string proTaskName = mTaskNameList[i];
      string conTaskName = mTaskNameList[j];

      CXMLAnn_WrapperGen *proTaskAnn;
      CXMLAnn_WrapperGen *conTaskAnn;
      proTaskAnn = get_tldm_task_ann_by_name(proTaskName);
      conTaskAnn = get_tldm_task_ann_by_name(conTaskName);

      unsigned proTask_group_id;
      unsigned conTask_group_id;
      // unsigned proTask_task_id, conTask_task_id;
      // unsigned proTask_iter_num, conTask_iter_num;

      proTask_group_id = my_atoi(proTaskAnn->GetParam("sdf_group_id"));
      // proTask_task_id  = my_atoi( proTaskAnn -> GetParam("sdf_task_id")  );
      // proTask_iter_num = my_atoi( proTaskAnn -> GetParam("sdf_iter_num") );
      conTask_group_id = my_atoi(conTaskAnn->GetParam("sdf_group_id"));
      // conTask_task_id  = my_atoi( conTaskAnn -> GetParam("sdf_task_id")  );
      // conTask_iter_num = my_atoi( conTaskAnn -> GetParam("sdf_iter_num") );

      // assert(proTask_iter_num == conTask_iter_num); // ?
      if (proTask_group_id >= conTask_group_id) {
        continue;
      }
      sdfFile << "\tARC edge_" << edgeNum++ << " FROM " << proTaskName << "_"
              << mTaskName2NofInstance[proTaskName] - 1;
      sdfFile << " TO " << conTaskName << "_0";
      sdfFile << " TYPE 2";
      // sdfFile << " ITERATION_FACTOR 1";
      sdfFile << endl;
    }
  }

  // find the task with group id of 0, and mTaskNameList.size()-1
  {
    // if (mTaskNameList.size() != 1) // skip ARC if only one group
    //{
    string proTaskName;
    string conTaskName;
    for (size_t i = 0; i < mTaskNameList.size(); i++) {
      string sTaskName = mTaskNameList[i];
      CXMLAnn_WrapperGen *taskAnn;
      taskAnn = get_tldm_task_ann_by_name(sTaskName);
      unsigned task_group_id = my_atoi(taskAnn->GetParam("sdf_group_id"));
      if (task_group_id == 0) {
        conTaskName = sTaskName;
      }
      if ((static_cast<unsigned int>(task_group_id ==
                                     mTaskNameList.size() - 1) != 0U)) {
        proTaskName = sTaskName;
      }
    }

    if (!proTaskName.empty()) {
      sdfFile << "\tARC edge_" << edgeNum++ << " FROM " << proTaskName << "_"
              << mTaskName2NofInstance[proTaskName] - 1;
      sdfFile << " TO " << conTaskName << "_0";
      sdfFile << " TYPE 3";
      // sdfFile << " ITERATION_FACTOR 1";
      sdfFile << endl;
    }
    //}
  }
  sdfFile << "}" << endl;

  //============= Parse Buffer Infomation =============================
  //=======================================================================
  sdfFile << "@BUFFER {" << endl;
  int bufId = 0;
  for (size_t i = 0; i < vectorDepAnn.size(); i++) {
    CTldmDependenceAnn *depAnn = vectorDepAnn[i];

    // step 0: get polyhedral_info
    CXMLAnn_WrapperGen *pTask0 = depAnn->GetTask0();
    CXMLAnn_WrapperGen *pTask1 = depAnn->GetTask1();
    assert(pTask0);
    assert(pTask1);

    string proTaskName;
    string conTaskName;
    string dataName;
    tldm_polyhedral_info poly_info_0 = pTask0->poly_info;
    tldm_polyhedral_info poly_info_1 = pTask1->poly_info;

    proTaskName = poly_info_0.name;
    conTaskName = poly_info_1.name;

    if (proTaskName == conTaskName) {
      continue;
    }

    dataName = pTask0->GetPortInfo(depAnn->GetPort0()).poly_info.name;

    // proTaskName = pTask0 -> poly_info.name;
    // conTaskName = pTask1 -> poly_info.name;
    // dataName = poly_info_0.name;
    //        if (nport0_info.port_type != "bfifo" || nport1_info.port_type !=
    // "bfifo")             continue; if (poly_info_0.properties["port_type"] !=
    // "bfifo" ||
    // poly_info_1.properties["port_type"] != "bfifo")     continue;

    //        cout << proTaskName << " --> " << conTaskName << endl;

    if (find(mTaskNameList.begin(), mTaskNameList.end(), proTaskName) ==
        mTaskNameList.end()) {
      continue;
    }
    if (find(mTaskNameList.begin(), mTaskNameList.end(), conTaskName) ==
        mTaskNameList.end()) {
      continue;
    }

    int tokenSize = 32768;
    int proRate = 1;
    int conRate = 1;

    sdfFile << "\tBUF buf_" << bufId++ << " FROMTASK " << proTaskName;
    sdfFile << " TOTASK " << conTaskName;
    sdfFile << " TOKENSIZE " << tokenSize;
    sdfFile << " PRORATE " << proRate;
    sdfFile << " CONRATE " << conRate << endl;
  }
  sdfFile << "}" << endl;

  return 1;
}

int main(int argc, char *argv[]) {
  // printf("Hello Starting...\n");
  // test_main(argv[1], argv[2]);
  if (argc < 4) {
    cout << "./xml2sdf_parser test.tldm_ann.xml design_metrics.xml "
            "directive.xml sdf.txt"
         << endl;
    exit(1);
  }
  test_main(argv[1], argv[2], argv[3], argv[4]);
  return 1;
}
