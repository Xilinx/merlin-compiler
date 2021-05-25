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
#include <algorithm>

#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "tldm_annotate.h"
#include "PolyModel.h"

// #define DEBUG_PRINT
// #define DEBUG_PRINT_NEW

XML_Object_Collector g_tldm_collector;

vector<CTldmDependenceAnn *> g_tldm_dependence_collector;
vector<CTldmTaskAnn *> g_tldm_task_collector;

map<string, CXMLAnn_WrapperGen *> g_map_name2task;

#define PRINT_COMBINED 1
#define PRINT_DEP 1
#define PRINT_BOUND 1

void get_connect_node_from_dependence(CXMLNode *pTLDMRoot, string proTaskName,
                                      string conTaskName, string dataName,
                                      CXMLNode **proConnect,
                                      CXMLNode **conConnect) {
  assert(pTLDMRoot);
  vector<CXMLNode *> vecConnect = pTLDMRoot->TraverseByName("connect");

  // proConnect
  for (size_t i = 0; i < vecConnect.size(); i++) {
    CXMLNode *nodeDir = vecConnect[i]->GetChildByName("direction");
    assert(nodeDir);

    if (nodeDir->GetValue() != "out") {
      continue;
    }

    CXMLNode *nodeTask = vecConnect[i]->GetChildByName("task_ref");
    assert(nodeTask);

    if (nodeTask->GetParam("name") != proTaskName) {
      continue;
    }

    CXMLNode *nodeDataRef = vecConnect[i]->GetChildByName("data_ref");
    assert(nodeDataRef);

    if (nodeDataRef->GetParam("name") != dataName) {
      continue;
    }

    *proConnect = vecConnect[i];
    break;
  }

  // conConnect
  for (size_t i = 0; i < vecConnect.size(); i++) {
    CXMLNode *nodeDir = vecConnect[i]->GetChildByName("direction");
    assert(nodeDir);

    if (nodeDir->GetValue() != "in") {
      continue;
    }

    CXMLNode *nodeTask = vecConnect[i]->GetChildByName("task_ref");
    assert(nodeTask);

    if (nodeTask->GetParam("name") != conTaskName) {
      continue;
    }

    CXMLNode *nodeDataRef = vecConnect[i]->GetChildByName("data_ref");
    assert(nodeDataRef);

    if (nodeDataRef->GetParam("name") != dataName) {
      continue;
    }

    *conConnect = vecConnect[i];
    break;
  }

  assert(proConnect && conConnect);
}

string CXMLAnn_Polyhedral::print() {
  size_t i;
  string str;

  string str_domain = "Domain :\n" + m_domain.print() + "\n";
  string str_sched = "Schedule :\n" + m_order.print() + "\n";

  string str_input = "Inputs : \n";
  for (i = 0; i < m_inputs.size(); i++) {
    str_input = str_input + "[" + m_inputs[i].first + "] \n";
    str_input = str_input + m_inputs[i].second.print();

#if PRINT_COMBINED
    {
      PolyMatrix combined = CombineAccessConstraints(
          m_domain, m_order, m_inputs[i].second, m_inputs[i].first, "_0");
      str_input += "Combined : \n" + combined.print() + "\n";
    }
#endif

#if PRINT_DEP
    {
      PolyMatrix dep_reduced = GetDependence(
          m_inputs[i].first, m_domain, m_order, m_inputs[i].second, m_domain,
          m_order, m_inputs[i].second);
      str_input += "Reduced dep : \n" + dep_reduced.print() + "\n";
      // printf("\n\n\n%s\n", str_input.c_str());
    }
#endif

#if PRINT_BOUND
    {
      PolyMatrix dep_bounds = GetLoopBounds(m_domain, m_order);
      str_input += "Loop Bounds : \n" + dep_bounds.print() + "\n";
      // printf("\n\n\n%s\n", str_input.c_str());
    }
#endif
  }

  string str_output = "Outputs : \n";
  for (i = 0; i < m_outputs.size(); i++) {
    str_output = str_output + "[" + m_outputs[i].first + "] \n";
    str_output = str_output + m_outputs[i].second.print();

#if PRINT_COMBINED
    {
      PolyMatrix combined = CombineAccessConstraints(
          m_domain, m_order, m_outputs[i].second, m_outputs[i].first, "_0");
      str_output += "Combined : \n" + combined.print() + "\n";
    }
#endif
  }

  string str_dep = "Deps : \n";
  for (i = 0; i < m_deps.size(); i++) {
    str_dep = str_dep + "[" + m_deps[i].first + "] \n";
    str_dep = str_dep + m_deps[i].second.print();
  }

  return str_domain + str_sched + str_input + str_output + str_dep;
}

string CXMLAnn_WrapperGen::print() {
  size_t i;
  string str_polyhrd = CXMLAnn_Polyhedral::print();

  string str_param = "\n\nParams : \n";
  map<string, string>::iterator it;
  for (it = m_mapParams.begin(); it != m_mapParams.end(); it++) {
    str_param += "[" + it->first + "]=" + it->second + "\n";
  }

  string str_port = "\nPorts : \n";
  for (i = 0; i < m_vecPortInfo.size(); i++) {
    char io_type_to_string[][1024] = {"out", "in", "iterator"};
    CXMLAnn_WrapperGen_port_info port_info = m_vecPortInfo[i];
    str_port = str_port + "array=" + port_info.array_name + " ";
    str_port = str_port + "array_position=" + port_info.array_position + " ";
    str_port = str_port + "port=" + port_info.port_id + " ";
    str_port = str_port + "pos=" + my_itoa(port_info.position) + " ";
    str_port = str_port +
               "io_dir=" + string(io_type_to_string[port_info.io_type]) + " ";
    str_port = str_port + "type=" + port_info.port_type + "\n";
    str_port = str_port + port_info.addr_map.print();
  }

  return str_polyhrd + str_param + str_port;
}

PolyVector BuildConditionFromXMLNode(CXMLNode *exp);
void BuildConditionFromXMLNode_withRange(CXMLNode *exp, PolyVector *lb,
                                         PolyVector *ub) {
  lb->clear();
  ub->clear();

  string sName = exp->GetName();

  if ("expression" == sName && "range" == exp->GetParam("opcode")) {
    assert(2 == exp->GetChildNum());
    *lb = BuildConditionFromXMLNode(exp->GetChildByIndex(0));
    *ub = BuildConditionFromXMLNode(exp->GetChildByIndex(1));
  } else {
    *lb = *ub = BuildConditionFromXMLNode(exp);
  }
}

PolyVector BuildConditionFromXMLNode(CXMLNode *exp) {
  PolyVector vec;

  string sName = exp->GetName();

  if ("integer" == sName) {
    vec = PolyVector(atoi(exp->GetValue().c_str()));
  } else if ("identifier" == sName) {
    vec = PolyVector(exp->GetValue());
  } else if ("negative_infinite" == sName) {
    vec = PolyVector(PolyVector::POSITIVE_INFINITE, -1);
  } else if ("positive_infinite" == sName) {
    vec = PolyVector(PolyVector::POSITIVE_INFINITE, 1);
  } else if ("expression" == sName) {
    string opcode = exp->GetParam("opcode");
    PolyVector exp0 = BuildConditionFromXMLNode(exp->GetChildByIndex(0));
    PolyVector exp1 = BuildConditionFromXMLNode(exp->GetChildByIndex(1));
    PolyVector one(1);
    if ("<" == opcode) {
      PolyVector flag =
          PolyVector(string(PolyVector::OP_FLAG_POINTER), PolyVector::OP_GE);
      vec = exp1 - exp0 + flag - one;
    } else if ("<=" == opcode) {
      PolyVector flag =
          PolyVector(string(PolyVector::OP_FLAG_POINTER), PolyVector::OP_GE);
      vec = exp1 - exp0 + flag;
    } else if (">" == opcode) {
      PolyVector flag =
          PolyVector(string(PolyVector::OP_FLAG_POINTER), PolyVector::OP_GE);
      vec = exp0 - exp1 + flag - one;
    } else if (">=" == opcode) {
      PolyVector flag =
          PolyVector(string(PolyVector::OP_FLAG_POINTER), PolyVector::OP_GE);
      vec = exp0 - exp1 + flag;
    } else if ("==" == opcode) {
      PolyVector flag =
          PolyVector(string(PolyVector::OP_FLAG_POINTER), PolyVector::OP_EQ);
      vec = exp0 - exp1 + flag;
    } else if ("+" == opcode) {
      vec = exp0 + exp1;
    } else if ("-" == opcode) {
      vec = exp0 - exp1;
    } else if ("*" == opcode) {
      vec = exp0 * exp1;
    } else if ("/" == opcode) {
      assert(0);
      // be not supported yet by PolyVector
    } else {
      printf("[WrapperGen] Error opcode in Function BuildConditionFromXMLNode "
             "(wrapper_gen.cpp).\n");
      exit(-1);
    }
  }

  return vec;
}

int AnalyzeDomain(CXMLNode *pDomain, PolyMatrix *domain) {
  size_t i;

  //// task node
  // CXMLNode * pTaskDef = SearchNodeNameUnque(pScope, "task_def", sTask);

  //// domain node
  // string sDomainName = SearchNodeNameUnique(pTaskDef,
  // "domain_ref")->GetParam("name"); CXMLNode * pDomain =
  // SearchNodeNameUnique(pScope, "iteration_domain_def", sDomainName);

  // iterators
  vector<CXMLNode *> vecNodes = pDomain->TraverseByName("vector", 1);

  // ZP: 2013-10-29 fixed for cases that has no iterators
  if (vecNodes.empty()) {
    return 1;
  }
  if (vecNodes.size() > 1) {
    assert(0);
  }
  //////////////////////////////////////////////////////////////////////////

  CXMLNode *pIteratorList = vecNodes[0];
  vector<string> vecIterators;
  for (int i = 0; i < pIteratorList->GetChildNum(); i++) {
    vecIterators.push_back(pIteratorList->GetChildByIndex(i)->GetValue());
  }

  // Expressions
  vecNodes = pDomain->TraverseByName("expression", 1);
  for (i = 0; i < vecNodes.size(); i++) {
    CXMLNode *pCond = vecNodes[i];
    PolyVector condition = BuildConditionFromXMLNode(pCond);
    domain->append_vector(condition);
  }
  domain->set_iterators(&vecIterators);
  // the order of coefficients are changed
  // in matrix according to vecIterators

  return 1;
}

int GetDefaultOrder(PolyMatrix *scatter, vector<string> vecIters) {
  size_t i;

  // iterators
  vector<string> vecIterators = vecIters;

  // attribute order node
  {
    for (i = 0; i < vecIterators.size(); i++) {
      PolyVector dim(vecIterators[i]);
      scatter->append_vector(dim);
    }
  }

  // set iterators
  scatter->set_iterators(&vecIterators);

  return 1;
}

int AnalyzeOrder(CXMLNode *pTaskDef, PolyMatrix *scatter,
                 const vector<string> &domain_iterators,
                 PolyMatrix *more_conditions) {
  // iterators
  vector<CXMLNode *> vecNodes = pTaskDef->TraverseByName("vector", 1);
  vector<string> vecIterators;
  if (1 == vecNodes.size()) {
    CXMLNode *pIteratorList = vecNodes[0];
    for (int i = 0; i < pIteratorList->GetChildNum(); i++) {
      vecIterators.push_back(pIteratorList->GetChildByIndex(i)->GetValue());
    }
  } else {
    assert(vecNodes.empty());  // should not have multiple "vector" sections
    vecIterators = domain_iterators;
  }

  // attribute order node
  vector<string> vecArg;
  vecArg.clear();
  vecArg.push_back("attribute");
  vecArg.push_back("order");
  vecNodes = pTaskDef->TraverseByCondition(CheckXMLNodeName, vecArg, 1);
  assert(vecNodes.size() <= 1);  // if not exist, identity matrix is used

  if (vecNodes.size() == 1) {
    CXMLNode *pOrderNode = vecNodes[0];
    // get each dimension
    for (int i = 0; i < pOrderNode->GetChildNum(); i++) {
      PolyVector dim =
          BuildConditionFromXMLNode(pOrderNode->GetChildByIndex(i));
      scatter->append_vector(dim);
    }
  } else {
    for (size_t i = 0; i < vecIterators.size(); i++) {
      PolyVector dim(vecIterators[i]);
      scatter->append_vector(dim);
    }
  }

  // Added by zhangpeng, 2013-01-06
  {
    CXMLNode *pParamConds =
        SearchNodeNameUnique(pTaskDef, "attribute", "condition");
    if (pParamConds != nullptr) {
      for (int j = 0;
           (pParamConds != nullptr) && j < pParamConds->GetChildNum(); j++) {
        CXMLNode *pCond = pParamConds->GetChildByIndex(j);
        PolyVector cond = BuildConditionFromXMLNode(pCond);
        more_conditions->append_vector(cond);
      }

      // FIXME: add all variables to the iterator for tiling loop generation
      vector<string> vec_iter;
      vector<string> vec_iter1;
      more_conditions->get_vars(&vec_iter);
      for (size_t j = 0; j < vec_iter.size(); j++) {
        if (vec_iter[j] != PolyVector::OP_FLAG_POINTER &&
            vec_iter[j] != PolyVector::CONST_POINTER) {
          vec_iter1.push_back(vec_iter[j]);
        }
      }
      more_conditions->set_iterators(&vec_iter1);
    }
  }

  // Added by zhangpeng, 2013-01-10
  {
    CXMLNode *pParamTileSize =
        SearchNodeNameUnique(pTaskDef, "attribute", "tile_size");
    if (pParamTileSize != nullptr) {
      assert((size_t)pParamTileSize->GetChildNum() == scatter->size());

      for (int j = 0; j < pParamTileSize->GetChildNum(); j++) {
        string sSize = pParamTileSize->GetChildByIndex(j)->GetValue();
        int nSize = my_atoi(sSize);
        PolyVector vecScatterDim = scatter->GetVector(j);

        string sDimName_t = "__tile_iter_" + my_itoa(j);
        string sDimName_i = "__intra_iter_" + my_itoa(j);

        PolyVector dim_i(sDimName_i);  // intra-tile index

        // 0 <= iter_i
        PolyVector cond = dim_i;
        {
          PolyVector vec0(PolyVector::OP_FLAG_POINTER, PolyVector::OP_GE);
          cond = cond + vec0;
        }
        more_conditions->append_vector(cond);

        // iter_i < tile_size
        cond = -(dim_i);
        {
          PolyVector vec0(PolyVector::CONST_POINTER, nSize - 1);
          cond = cond + vec0;
        }
        {
          PolyVector vec0(PolyVector::OP_FLAG_POINTER, PolyVector::OP_GE);
          cond = cond + vec0;
        }
        more_conditions->append_vector(cond);

        // iter = iter_t*tile_size + tile_i
        cond = dim_i;
        {
          PolyVector vec0(sDimName_t, nSize);
          cond = cond + vec0;
        }
        { cond = cond - vecScatterDim; }
        {
          PolyVector vec0(PolyVector::OP_FLAG_POINTER, PolyVector::OP_EQ);
          cond = cond + vec0;
        }
        more_conditions->append_vector(cond);
      }

      // FIXME: add all variables to the iterator for tiling loop generation
      vector<string> vec_iter;
      vector<string> vec_iter1;
      more_conditions->get_vars(&vec_iter);
      for (size_t j = 0; j < vec_iter.size(); j++) {
        if (vec_iter[j] != PolyVector::OP_FLAG_POINTER &&
            vec_iter[j] != PolyVector::CONST_POINTER) {
          vec_iter1.push_back(vec_iter[j]);
        }
      }
      more_conditions->set_iterators(&vec_iter1);

      // modify the scatter matrix
      {
        *scatter = PolyMatrix();
        for (int j = 0; j < pParamTileSize->GetChildNum(); j++) {
          string sDimName_t = "__tile_iter_" + my_itoa(j);
          PolyVector dim(sDimName_t);
          scatter->append_vector(dim);
        }
        for (int j = 0; j < pParamTileSize->GetChildNum(); j++) {
          string sDimName_i = "__intra_iter_" + my_itoa(j);
          PolyVector dim(sDimName_i);
          scatter->append_vector(dim);
        }
      }
    }
  }

  // set iterators
  scatter->set_iterators(&vecIterators);

  // #if PRINT_DEBUG
  //     string str = scatter.print();
  //     std::cout << "Scatter of task " << pTaskDef->GetParam("name") << ":" <<
  //  endl;     std::cout << str << endl; #endif

  return 1;
}

int AnalyzePort(CXMLNode *pPort, vector<string> iter_default,
                string *sArrayName, PolyMatrix *matAccess, string *sDir,
                CXMLAnn_WrapperGen_port_info *port_info_out) {
  int j;

  // 2.1 task_ref
  string sTaskRef;
  string sPosRef;
  vector<string> vecIters;
  {
    CXMLNode *pTaskRef = pPort->GetChildByName("task_ref");
    sTaskRef = pTaskRef->GetParam("name");
    sPosRef = pTaskRef->GetParam("pos");

    CXMLNode *pItersList = pTaskRef->GetChildByName("vector");
    if (nullptr != pItersList) {
      for (j = 0; j < pItersList->GetChildNum(); j++) {
        vecIters.push_back(pItersList->GetChildByIndex(j)->GetValue());
      }
    } else {  // default iter list
      vecIters = iter_default;
    }
  }

  // 2.2 data ref
  CXMLNode *pDataRef;
  {
    vector<string> vecVars = vecIters;
    pDataRef = pPort->GetChildByName("data_ref");
    *sArrayName = pDataRef->GetParam("name");
    CXMLNode *pDimensionList = pDataRef->GetChildByName("vector");
    for (j = 0;
         (pDimensionList != nullptr) && j < pDimensionList->GetChildNum();
         j++) {
      string sDim = *sArrayName + string("__d") + my_itoa(j);
      vecVars.push_back(sDim);

      CXMLNode *pDim = pDimensionList->GetChildByIndex(j);
      PolyVector lb;
      PolyVector ub;
      BuildConditionFromXMLNode_withRange(pDim, &lb, &ub);
      if ("expression" == pDim->GetName() &&
          "range" == pDim->GetParam("opcode")) {
        if (0 <= lb.get_coeff(PolyVector::POSITIVE_INFINITE)) {
          PolyVector vec = PolyVector(sDim);
          vec = vec - lb;
          PolyVector op = PolyVector(string(PolyVector::OP_FLAG_POINTER),
                                     PolyVector::OP_GE);
          vec = vec + op;
          matAccess->append_vector(vec);
        }
        if (0 >= ub.get_coeff(PolyVector::POSITIVE_INFINITE)) {
          PolyVector vec = PolyVector(sDim);
          vec = ub - vec;
          PolyVector op = PolyVector(string(PolyVector::OP_FLAG_POINTER),
                                     PolyVector::OP_GE);
          vec = vec + op;
          matAccess->append_vector(vec);
        }
      } else {  // not range
        PolyVector vec = PolyVector(sDim);
        vec = vec - lb;
        PolyVector op =
            PolyVector(string(PolyVector::OP_FLAG_POINTER), PolyVector::OP_EQ);
        vec = vec + op;
        matAccess->append_vector(vec);
      }
    }

    // Added by zhangpeng, 2013-01-06
    {
      CXMLNode *pParamConds =
          SearchNodeNameUnique(pDataRef, "attribute", "condition");
      for (j = 0; (pParamConds != nullptr) && j < pParamConds->GetChildNum();
           j++) {
        CXMLNode *pCond = pParamConds->GetChildByIndex(j);
        PolyVector cond = BuildConditionFromXMLNode(pCond);
        matAccess->append_vector(cond);
      }
    }

    // reordering the variables in matAccess
    matAccess->set_iterators(&vecVars);
  }

  // 2.3 direction
  *sDir = pPort->GetChildByName("direction")->GetValue();

  // 2.3.1 port info
  {
    CXMLAnn_WrapperGen_port_info port_info;
    port_info.array_name = pDataRef->GetParam("name");
    port_info.array_position = pDataRef->GetParam("position");
    CXMLNode *pAttr = SearchNodeNameUnique(pDataRef, "attribute", "port_id");
    if (pAttr != nullptr) {
      port_info.port_id = pAttr->GetChildByIndex(0)->GetValue();
      if (pAttr->GetChildNum() > 1) {
        port_info.position = my_atoi(pAttr->GetChildByIndex(1)->GetValue());
      }
    } else {
      port_info.port_id = port_info.array_name;
    }
    pAttr = SearchNodeNameUnique(pDataRef, "attribute", "port_type");
    if (pAttr != nullptr) {
      port_info.port_type = pAttr->GetChildByIndex(0)->GetValue();
    } else {
      port_info.port_type = string("fifo");
    }
    pAttr = SearchNodeNameUnique(pDataRef, "attribute", "rate");
    if (pAttr != nullptr) {
      port_info.rate = my_atoi(pAttr->GetChildByIndex(0)->GetValue());
    } else {
      port_info.rate = 1;
    }
    pAttr = SearchNodeNameUnique(pDataRef, "attribute", "unit_size");
    if (pAttr != nullptr) {
      port_info.unit_size = pAttr->GetChildByIndex(0)->GetValue();
    } else {
      port_info.unit_size = string("1");
    }
    pAttr = SearchNodeNameUnique(pDataRef, "attribute", "monitor");
    if (pAttr != nullptr) {
      port_info.monitor = (pAttr->GetChildByIndex(0)->GetValue() == "1")
                              ? string("yes")
                              : string("no");
    } else {
      port_info.monitor = string("no");
    }
    pAttr = SearchNodeNameUnique(pDataRef, "attribute", "sync_level");
    if (pAttr != nullptr) {
      port_info.sync_level = my_atoi(pAttr->GetChildByIndex(0)->GetValue());
    } else {
      port_info.sync_level = -1;
    }
    pAttr = SearchNodeNameUnique(pDataRef, "attribute", "addr_mapping");
    if (pAttr != nullptr) {
      port_info.addr_mapping = (pAttr->GetChildByIndex(0)->GetValue());
    } else {
      port_info.addr_mapping = "";
    }
    pAttr = SearchNodeNameUnique(pDataRef, "attribute", "ref");
    if (pAttr != nullptr) {
      port_info.ref = pAttr->GetChildByIndex(0)->GetValue();
    } else {
      port_info.ref = "";
    }

    vector<CXMLNode *> vecAttrs = pDataRef->TraverseByName("attribute");
    for (size_t i = 0; i < vecAttrs.size(); i++) {
      string sAttrName = vecAttrs[i]->GetParam("name");
      if (vecAttrs[i]->GetChildNum() == 0) {
        printf("[tldm_annotate] ERROR: A port attribute has no child with type "
               "define. (task=%s,port=%s,attr=%s)\n\n",
               sTaskRef.c_str(), port_info.port_id.c_str(), sAttrName.c_str());
        assert(0);
      }
      string sAttrValue;
      sAttrValue = vecAttrs[i]->GetChildByIndex(0)->GetValue();
      for (int jj = 1; jj < vecAttrs[i]->GetChildNum(); jj++) {
        sAttrValue += "," + vecAttrs[i]->GetChildByIndex(jj)->GetValue();
      }
      port_info.properties[sAttrName] = sAttrValue;
    }

    port_info.io_type = (*sDir == "out") ? CXMLAnn_WrapperGen::IOTYPE_OUTPUT
                                         : CXMLAnn_WrapperGen::IOTYPE_INPUT;
    *port_info_out = port_info;

    // added by zhangpeng 2013-06-20
    str_split(port_info.properties["inner_iterator"], ',',
              &matAccess->get_inner_iterators());
  }

  return 1;
}

void AnalyzeDepRef(CXMLNode *pDepRef, PolyMatrix *matDep,
                   vector<string> vecIter) {
  // (idct_row : it, i .SUB 0, 2 ) -> (CH_coeff : it, 0, i)
  // DELTA = (0,2), const_cond_d2 = 0;

  int j;
  for (j = 0; j < pDepRef->GetChildNum(); j++) {
    string sDepIter = string("d_iter") + my_itoa(j);
    PolyVector vec(sDepIter);
    PolyVector val;

    CXMLNode *pDim = pDepRef->GetChildByIndex(j);
    if ("identifier" == pDim->GetName()) {
      size_t k;
      for (k = 0; k < vecIter.size(); k++) {
        if (pDim->GetValue() == vecIter[k]) {
          break;
        }
      }
      assert(k < vecIter.size());  // must be the iterator in (target) task_ref
      string sIter("iter");
      sIter = sIter + my_itoa(k);
      val = PolyVector(sIter);
    } else if ("integer" == pDim->GetName()) {
      val = PolyVector(PolyVector::CONST_POINTER, my_atoi(pDim->GetValue()));
    } else {
      assert(0);
    }

    PolyVector op(PolyVector::OP_FLAG_POINTER, PolyVector::OP_EQ);
    vec = vec - val;
    vec = vec + op;
    matDep->append_vector(vec);
  }
}

int AnalyzeDep(CXMLNode *pDep, PolyMatrix *matDep, string *sDepTask) {
  vector<CXMLNode *> vecTaskRef = pDep->TraverseByName("task_ref", 1);
  assert(vecTaskRef.size() == 2);
  CXMLNode *pDepRef = vecTaskRef[0]->GetChildByIndex(0);
  CXMLNode *pTaskRef = vecTaskRef[1]->GetChildByIndex(0);

  *sDepTask = vecTaskRef[0]->GetParam("name");

  vector<string> vecIter;
  vector<string> vecConstCond;
  for (int j = 0; j < pTaskRef->GetChildNum(); j++) {
    CXMLNode *pDim = pTaskRef->GetChildByIndex(j);
    if ("identifier" == pDim->GetName()) {
      vecIter.push_back(pDim->GetValue());
    } else if ("integer" == pDim->GetName()) {
      PolyVector vec(PolyVector::FIXED_CONDITION + my_itoa(j));
      PolyVector val(PolyVector::CONST_POINTER, my_atoi(pDim->GetValue()));
      PolyVector op(PolyVector::OP_FLAG_POINTER, PolyVector::OP_EQ);
      vec = vec - val;
      vec = vec + op;
      matDep->append_vector(vec);

      vecConstCond.push_back(PolyVector::FIXED_CONDITION + my_itoa(j));
    } else {
      assert(0);
    }
  }

  // dependence
  vector<string> vecDelta;
  if ("vector" == pDepRef->GetName()) {
    AnalyzeDepRef(pDepRef, matDep, vecIter);
  } else if ("expression" == pDepRef->GetName()) {
    assert(pDepRef->GetParam("opcode") == "LSUB");
    AnalyzeDepRef(pDepRef->GetChildByIndex(0), matDep, vecIter);
    CXMLNode *pDelta = pDepRef->GetChildByIndex(1);
    for (int j = 0; j < pDelta->GetChildNum(); j++) {
      CXMLNode *pDim = pDelta->GetChildByIndex(j);
      assert(pDim->GetName() == "integer");
      PolyVector vec(PolyVector::LEXIGRAPHIC_DELTA + my_itoa(j));
      PolyVector val(PolyVector::CONST_POINTER, my_atoi(pDim->GetValue()));
      PolyVector op(PolyVector::OP_FLAG_POINTER, PolyVector::OP_EQ);
      vec = vec - val;
      vec = vec + op;
      matDep->append_vector(vec);

      vecDelta.push_back(PolyVector::LEXIGRAPHIC_DELTA + my_itoa(j));
    }
  } else {
    assert(0);
  }

  // vecDepdim, vecIter, vecDelta, vecConstCond
  {
    vector<string> order;
    for (int j = 0; j < SearchNodeNameFirst(pDepRef, "vector")->GetChildNum();
         j++) {
      string sDepIter = string("d_iter") + my_itoa(j);
      order.push_back(sDepIter);
    }
    for (int j = 0; j < SearchNodeNameFirst(pTaskRef, "vector")->GetChildNum();
         j++) {
      string sTaskIter = string("iter") + my_itoa(j);
      order.push_back(sTaskIter);
    }
    order.insert(order.end(), vecDelta.begin(), vecDelta.end());
    order.insert(order.end(), vecConstCond.begin(), vecConstCond.end());
    matDep->set_iterators(&order);
  }

  return 1;
}

void tldm_clean_all_new_annotation() {
  size_t i;
  for (i = 0; i < g_tldm_dependence_collector.size(); i++) {
    if (g_tldm_dependence_collector[i] != nullptr) {
      delete g_tldm_dependence_collector[i];
      g_tldm_dependence_collector[i] = nullptr;
    }
  }
  for (i = 0; i < g_tldm_task_collector.size(); i++) {
    if (g_tldm_task_collector[i] != nullptr) {
      delete g_tldm_task_collector[i];
      g_tldm_task_collector[i] = nullptr;
    }
  }
  g_tldm_dependence_collector.clear();
  g_tldm_task_collector.clear();
}

int mark_tldm_annotation_release_resource() {
  g_tldm_collector.CleanAll();
  g_map_name2task.clear();

  tldm_clean_all_new_annotation();
  return 1;
}

void print_tldm_annotation(CXMLNode *pTLDMRoot) {
  size_t i;
  size_t j;
  CXMLAnn_WrapperGen *pAnnTemp;
  // 0.1 parse all domain_def if not parsed
  {
    vector<CXMLNode *> vecNodes =
        pTLDMRoot->TraverseByName("iteration_domain_def");
    for (i = 0; i < vecNodes.size(); i++) {
      CXMLNode *pDomain = vecNodes[i];
      pAnnTemp = dynamic_cast<CXMLAnn_WrapperGen *>(
          pDomain->GetAnnotation("CXMLAnn_WrapperGen"));
      std::cout << "\n\n\nIterationDomain: " << pDomain->GetParam("name")
                << endl;
      if (pAnnTemp != nullptr) {
        std::cout << pAnnTemp->print();
      }
    }
  }

  // 0.2 parse all data_def if not parsed
  {
    vector<CXMLNode *> vecNodes = pTLDMRoot->TraverseByName("data_def");
    for (i = 0; i < vecNodes.size(); i++) {
      CXMLNode *pDataDef = vecNodes[i];
      pAnnTemp = dynamic_cast<CXMLAnn_WrapperGen *>(
          pDataDef->GetAnnotation("CXMLAnn_WrapperGen"));
      std::cout << "\n\n\nDataset: " << pDataDef->GetParam("name") << endl;
      if (pAnnTemp != nullptr) {
        std::cout << pAnnTemp->print() << endl;
      }
    }
  }

  // 1. Task annotation with IO and dependence

  vector<CXMLNode *> vecTaskNodes = pTLDMRoot->TraverseByName("task_def", 2);
  for (j = 0; j < vecTaskNodes.size(); j++) {
    if (vecTaskNodes[j]->GetDepth() < 2) {
      continue;
    }
    string sTaskName = vecTaskNodes[j]->GetParam("name");

    // 1. task_def: domain, order
    {
      // 1.1 task_def
      CXMLNode *pTaskDef =
          SearchNodeNameUnique(pTLDMRoot, "task_def", sTaskName);

      pAnnTemp = dynamic_cast<CXMLAnn_WrapperGen *>(
          pTaskDef->GetAnnotation("CXMLAnn_WrapperGen"));
      std::cout << "\n\n\nTask: " << pTaskDef->GetParam("name") << endl;
      if (pAnnTemp != nullptr) {
        std::cout << pAnnTemp->print() << endl;
      }
    }
  }
}

int mark_tldm_annotation(CXMLNode *pTLDMRoot) {
  size_t i;
  size_t j;
  // 0.1 parse all domain_def if not parsed
  {
    vector<CXMLNode *> vecNodes =
        pTLDMRoot->TraverseByName("iteration_domain_def");
    for (i = 0; i < vecNodes.size(); i++) {
      CXMLNode *pDomain = vecNodes[i];
      if (pDomain->GetAnnotation("CXMLAnn_WrapperGen") == nullptr) {
        CXMLAnn_WrapperGen *pAnnTemp = new CXMLAnn_WrapperGen;
        g_tldm_collector.Append(pAnnTemp);
        AnalyzeDomain(pDomain, &pAnnTemp->GetDomain());
        pDomain->SetAnnotation(pAnnTemp);
      }
    }
  }

  // 0.2 parse all data_def if not parsed
  {
    vector<CXMLNode *> vecNodes = pTLDMRoot->TraverseByName("data_def");
    for (i = 0; i < vecNodes.size(); i++) {
      CXMLNode *pDataDef = vecNodes[i];
      if (pDataDef->GetAnnotation("CXMLAnn_WrapperGen") == nullptr) {
        CXMLAnn_WrapperGen *pAnnTemp = new CXMLAnn_WrapperGen;
        g_tldm_collector.Append(pAnnTemp);
        CXMLNode *pDomainRef = SearchNodeNameUnique(pDataDef, "domain_ref");
        CXMLNode *pDomain = nullptr;
        if (pDomainRef != nullptr) {
          string sDomainName = pDomainRef->GetParam("name");
          pDomain = SearchNodeNameUnique(pTLDMRoot, "iteration_domain_def",
                                         sDomainName);
        }

        PolyMatrix domain;
        if (pDomain != nullptr) {
          CXMLAnn_WrapperGen *pAnnDomain = dynamic_cast<CXMLAnn_WrapperGen *>(
              pDomain->GetAnnotation("CXMLAnn_WrapperGen"));
          assert(pAnnDomain);
          domain = pAnnDomain->GetDomain();
        }
        pAnnTemp->GetDomain() = domain;
        pDataDef->SetAnnotation(pAnnTemp);

        CXMLNode *pType = pDataDef->GetChildByName("data_type");
        pDataDef->SetParam("data_type", pType->GetValue());
      }
    }
  }

  // 1. Task annotation with IO and dependence
  vector<CXMLNode *> vecTaskNodes = pTLDMRoot->TraverseByName("task_def", 2);
  for (j = 0; j < vecTaskNodes.size(); j++) {
    if (vecTaskNodes[j]->GetDepth() < 2) {
      continue;
    }
    string sTaskName = vecTaskNodes[j]->GetParam("name");
    CXMLNode *pTaskDefNode = vecTaskNodes[j];

    // 1. task_def: domain, order
    CXMLAnn_WrapperGen *pAnnTaskNode;
    {
      // 1.1 task_def
      CXMLNode *pTaskDef =
          SearchNodeNameUnique(pTLDMRoot, "task_def", sTaskName);
      if (pTaskDef->GetAnnotation("CXMLAnn_WrapperGen") != nullptr) {
        continue;
      }

      // Get required information from XML structure
      pAnnTaskNode = new CXMLAnn_WrapperGen;
      g_tldm_collector.Append(pAnnTaskNode);
      pAnnTaskNode->SetParent(nullptr);
      pTaskDef->SetAnnotation(pAnnTaskNode);
      g_map_name2task.insert(
          pair<string, CXMLAnn_WrapperGen *>(sTaskName, pAnnTaskNode));

      // 1.2 domain_def => pAnn->domain
      {
        CXMLNode *pDomainRef = SearchNodeNameUnique(pTaskDef, "domain_ref");
        CXMLNode *pDomain = nullptr;
        if (pDomainRef != nullptr) {
          string sDomainName = pDomainRef->GetParam("name");
          pDomain = SearchNodeNameUnique(pTLDMRoot, "iteration_domain_def",
                                         sDomainName);
        }

        PolyMatrix domain;
        if (pDomain != nullptr) {
          CXMLAnn_WrapperGen *pAnnTemp = dynamic_cast<CXMLAnn_WrapperGen *>(
              pDomain->GetAnnotation("CXMLAnn_WrapperGen"));
          assert(pAnnTemp);
          domain = pAnnTemp->GetDomain();
        }
        pAnnTaskNode->GetDomain() = domain;

        //////////////////////////////////////////////////////////////////////////
        // Added by zhangpeng, 2013-01-29 : get global_id information
        {
          vector<string> vec_gid = pAnnTaskNode->GetDomain().get_iterators();
          string str = str_merge(',', vec_gid);
          pAnnTaskNode->SetParam("global_id", str);
        }
      }

      // 1.3 parameters: position, body_id, translate_type, port_id, order
      {
        pAnnTaskNode->SetParam("position", pTaskDef->GetParam("position"));
        pAnnTaskNode->SetParam("loop_depth", "0");

        vector<CXMLNode *> vecNodes = pTaskDef->TraverseByName("attribute");
        int have_schedule = 0;
        for (i = 0; i < vecNodes.size(); i++) {
          CXMLNode *pAttr = vecNodes[i];
          string sName = pAttr->GetParam("name");
          if ("body_id" == sName) {
            pAnnTaskNode->SetParam("body_id",
                                   pAttr->GetChildByIndex(0)->GetValue());
          } else if ("translate_type" == sName) {
            pAnnTaskNode->SetParam("translate_type",
                                   pAttr->GetChildByIndex(0)->GetValue());
          } else if ("port_id" == sName) {
            CXMLAnn_WrapperGen_port_info port_info;
            port_info.array_name = "";
            port_info.array_position = "";
            port_info.port_id = pAttr->GetChildByIndex(0)->GetValue();
            port_info.position = my_atoi(pAttr->GetChildByIndex(1)->GetValue());
            port_info.io_type = CXMLAnn_WrapperGen::IOTYPE_ITERATOR;
            pAnnTaskNode->AppendPortInfo(port_info);
          } else if ("order" == sName) {
            vector<string> default_iterators =
                pAnnTaskNode->GetDomain().get_iterators();
            PolyMatrix more_conditions;
            AnalyzeOrder(pTaskDef, &pAnnTaskNode->GetOrder(), default_iterators,
                         &more_conditions);
            PolyMatrix &domain_temp = pAnnTaskNode->GetDomain();
            domain_temp.append_matrix(more_conditions);
            vector<string> merged_iter = merge_iterator(
                default_iterators, more_conditions.get_iterators());
            domain_temp.set_iterators(&merged_iter);
            have_schedule = 1;

            pAnnTaskNode->SetParam("loop_depth",
                                   my_itoa(pAnnTaskNode->GetOrder().size()));
          } else if ("dup_factor" == sName) {
            pAnnTaskNode->SetParam("dup_factor",
                                   pAttr->GetChildByIndex(0)->GetValue());
          } else {
            pAnnTaskNode->SetParam(sName,
                                   pAttr->GetChildByIndex(0)->GetValue());
          }
        }
        pAnnTaskNode->SetParam("name", sTaskName);
        if (pAnnTaskNode->GetParam("body_id") == string("")) {
          pAnnTaskNode->SetParam("body_id", sTaskName);
        }
        if (pAnnTaskNode->GetParam("dup_factor") == string("")) {
          pAnnTaskNode->SetParam("dup_factor", "1");
        }
        if (have_schedule == 0) {
          vector<string> default_iterators =
              pAnnTaskNode->GetDomain().get_iterators();
          GetDefaultOrder(&pAnnTaskNode->GetOrder(), default_iterators);
        }
      }
    }

    // 2 io port
    {
      vector<CXMLNode *> vecNodes = pTLDMRoot->TraverseByName("connect");
      for (i = 0; i < vecNodes.size(); i++) {
        CXMLNode *pPort = vecNodes[i];
        CXMLNode *pTaskRef = pPort->GetChildByName("task_ref");
        string sTaskRef = pTaskRef->GetParam("name");
        if (sTaskName == sTaskRef) {
          PolyMatrix matAccess;
          string sArrayName;
          CXMLAnn_WrapperGen_port_info port_info;
          string sDir;
          vector<string> task_iter = pAnnTaskNode->GetDomain().get_iterators();
          AnalyzePort(pPort, task_iter, &sArrayName, &matAccess, &sDir,
                      &port_info);

          named_PolyMatrix one_access = named_PolyMatrix(sArrayName, matAccess);
          pAnnTaskNode->AppendPortPair(one_access, port_info);

#ifdef DEBUG_PRINT
          assert(vecPorts.size() == 1);
          printf("[TLDM_annotate] task=%s, port_id=%s, array=%s\n",
                 sTaskName.c_str(), vecPorts[0].port_id.c_str(),
                 sArrayName.c_str());
#endif
        }
      }
    }

    // 3. dependence
    {
      vector<CXMLNode *> vecNodes = pTLDMRoot->TraverseByName("dependence");
      for (i = 0; i < vecNodes.size(); i++) {
        CXMLNode *pDep = vecNodes[i];
        vector<CXMLNode *> vecTaskRef = pDep->TraverseByName("task_ref", 1);
        assert(vecTaskRef.size() == 2);
        CXMLNode *pTaskRef = vecTaskRef[1];

        if (sTaskName == pTaskRef->GetParam("name")) {
          PolyMatrix matDep;
          string sDepTask;

          AnalyzeDep(pDep, &matDep, &sDepTask);

          named_PolyMatrix one_dep = named_PolyMatrix(sDepTask, matDep);
          pAnnTaskNode->GetDeps().push_back(one_dep);
        }
      }
    }

#ifdef DEBUG_PRINT
    std::cout << "\n\n\nTask: " << sTaskName << endl;
    std::cout << pAnnTaskNode->print() << endl;
#endif
    //////////////////////////////////////////////////////////////////////////
    // 2013-10-08 zhangpeng
    {
      tldm_polyhedral_info &task_poly_info = pAnnTaskNode->poly_info;
      // tldm_polyhedral_info task_poly_info;// = pAnnTaskNode->poly_info;

#ifdef DEBUG_PRINT_NEW
      printf("[mark_tldm] Task: %s\n", pAnnTaskNode->GetParam("name").c_str());
#endif

      task_poly_info.name = pAnnTaskNode->GetParam("name");
      task_poly_info.type = pAnnTaskNode->GetParam("type");
      task_poly_info.outer_level = -1;
      task_poly_info.iterator_vector =
          pAnnTaskNode->GetParam("iterator_vector");
      task_poly_info.iterator_range = pAnnTaskNode->GetParam("iterator_range");
      RemoveSpace(
          &task_poly_info.iterator_range);  // Do not why there is space there
      RemoveSpace(
          &task_poly_info.iterator_vector);  // Do not why there is space there
      task_poly_info.parameter_vector =
          pAnnTaskNode->GetParam("parameter_vector");
      task_poly_info.parameter_range =
          pAnnTaskNode->GetParam("parameter_range");
      task_poly_info.order_vector = pAnnTaskNode->GetParam("order_vector");
      task_poly_info.condition_vector =
          pAnnTaskNode->GetParam("condition_vector");
      task_poly_info.access_pattern = string("0");
      task_poly_info.gid_level = task_poly_info.get_loop_level();

      task_poly_info.properties = pAnnTaskNode->GetAllParams();

      CTldmTaskAnn *newTask =
          create_tldm_task(task_poly_info.name, task_poly_info);

      for (int k = 0; k < pAnnTaskNode->GetPortInfoNum(); k++) {
        CTldmPortAnn &port_info = pAnnTaskNode->GetPortInfo(k);
        tldm_polyhedral_info &port_poly_info = port_info.poly_info;
        // tldm_polyhedral_info port_poly_info;

#ifdef DEBUG_PRINT_NEW
        printf("[mark_tldm] Port: %s\n", port_info.array_name.c_str());
#endif

        port_poly_info.name = port_info.array_name;
        port_poly_info.type = "access";
        port_poly_info.outer_level = -1;
        port_poly_info.iterator_vector =
            port_info.properties["iterator_vector"];
        port_poly_info.iterator_range = port_info.properties["iterator_range"];
        RemoveSpace(
            &port_poly_info
                 .iterator_range);  // Do not know why there is space there
        RemoveSpace(
            &port_poly_info
                 .iterator_vector);  // Do not know why there is space there
        port_poly_info.parameter_vector =
            port_info.properties["parameter_vector"];
        port_poly_info.parameter_range =
            port_info.properties["parameter_range"];
        port_poly_info.order_vector = port_info.properties["order_vector"];
        port_poly_info.condition_vector =
            port_info.properties["condition_vector"];
        port_poly_info.access_pattern = port_info.properties["access_pattern"];
        port_poly_info.gid_level = task_poly_info.gid_level;

        port_poly_info.properties = port_info.properties;

        // ZP: 2013-10-13: add channel_offset into poly_info
        {
          port_poly_info.properties["channel_offset"] = "";
          {
            CXMLNode *pTaskNode = pTaskDefNode;
            vector<CXMLNode *> vecLDU = pTaskNode->TraverseByName("attribute");
            for (size_t kk = 0; kk < vecLDU.size(); kk++) {
              if ("access_pattern" != vecLDU[kk]->GetParam("name") &&
                  "gdu_info" != vecLDU[kk]->GetParam("name")) {
                continue;
              }

              CXMLNode *pChannelAttr = vecLDU[kk]->GetChildByIndex(0);

              if (port_poly_info.name !=
                  pChannelAttr->GetChildByName("port_name")->GetValue()) {
                continue;
              }

              string channel_offset =
                  pChannelAttr->GetChildByName("channel_offset")->GetValue();

              port_poly_info.properties["channel_offset"] = channel_offset;

              // printf("channel_offset: [%s] %s\n",
              // port_poly_info.name.c_str(), channel_offset.c_str());
              break;
            }
          }
        }
        //////////////////////////////////////////////////////////////////////////

        create_and_append_tldm_port(newTask, port_poly_info.name,
                                    port_poly_info);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // 2013-10-08 parse parent and outer_level
  { rebuild_parent_outer_level(); }

  if (true) {
    //////////////////////////////////////////////////////////////////////////
    // 2013-10-08 update distance
    { rebuild_dep_graph(); }
  }

  return 1;
}

CXMLAnn_WrapperGen *get_tldm_task_ann_by_name(string sName) {
  return g_map_name2task[sName];
}

CXMLNode *tldm_xml_task_append_attribute(CXMLNode *pTaskNode, string sAttrName,
                                         string sAttrValue) {
  assert(pTaskNode->GetName() == "task_def");
  CXMLNode *attr = pTaskNode->CreateAppendChild("attribute");
  attr->SetParam("name", sAttrName);
  CXMLNode *iden = attr->CreateAppendChild("identifier");
  iden->SetValue(sAttrValue);
  return attr;
}

CXMLNode *tldm_xml_task_append_attribute_ex(CXMLNode *pTaskNode,
                                            string sAttrName, string sItemName,
                                            map<string, string> mapValue) {
  assert(pTaskNode->GetName() == "task_def");
  CXMLNode *attr = pTaskNode->CreateAppendChild("attribute");
  attr->SetParam("name", sAttrName);
  CXMLNode *group = attr->CreateAppendChild("group");
  group->SetParam("name", sItemName);

  map<string, string>::iterator it;
  for (it = mapValue.begin(); it != mapValue.end(); it++) {
    CXMLNode *item = group->CreateAppendChild(it->first);
    item->SetValue(it->second);
  }
  return attr;
}

string tldm_xml_get_attribute(CXMLNode *pTaskNode, string sAttrName) {
  // CXMLNode * pAttr = SearchNodeNameUnique(pTaskNode, "attribute", sAttrName);
  CXMLNode *pAttr = SearchNodeNameFirst(pTaskNode, "attribute", sAttrName);
  // assert(pAttr);
  if (pAttr == nullptr) {
    return "";
  }

  assert(
      "group" !=
      pAttr->GetChildByIndex(0)->GetParam(
          "name"));  // do not support hierarchical attribute, like ldu_channel
  return pAttr->GetChildByIndex(0)->GetValue();
}

void tldm_xml_set_attribute(CXMLNode *pTaskNode, string sAttrName,
                            string sValue) {
  CXMLNode *pAttr = SearchNodeNameUnique(pTaskNode, "attribute", sAttrName);
  // assert(pAttr);

  if (pAttr == nullptr) {
    tldm_xml_task_append_attribute(pTaskNode, sAttrName, sValue);
  } else {
    assert("group" != pAttr->GetChildByIndex(0)->GetParam(
                          "name"));  // do not support hierarchical attribute,
                                     // like access_pattern
    pAttr->GetChildByIndex(0)->SetValue(sValue);
  }
}

const char c_id_offset_prefix[] = "_id_offset";

PolyMatrix CXMLAnn_WrapperGen::GetAccessDomain(int idx, CXMLNode *pTaskDef) {
  size_t i;
  assert(idx >= 0 && idx < GetPortInfoNum());
  PolyMatrix domain_task = GetDomain();

  assert(GetParam("body_id") == pTaskDef->GetParam("name"));
  string sTaskName = GetParam("body_id");

  CXMLAnn_WrapperGen_port_info port_info = GetPortInfo(idx);
  string sArray = port_info.array_name;

  vector<int> vecLBOffset;
  vector<int> vecUBOffset;
  get_xml_channel_offset(pTaskDef, sTaskName, sArray, &vecLBOffset,
                         &vecUBOffset);

  if (vecLBOffset.empty()) {
    return domain_task;
  }
  assert(vecLBOffset.size() == vecUBOffset.size());

  PolyMatrix domain_access = domain_task;

  // 1. Add some new iteration domain offset variables (_id_offset) with the
  // range of channel offsets
  for (i = 0; i < vecLBOffset.size(); i++) {
    if (vecLBOffset[i] == vecUBOffset[i]) {
      continue;
    }

    string new_var_name = c_id_offset_prefix + my_itoa(i);
    vector<string> vars = domain_access.get_iterators();
    vars.push_back(new_var_name);

    // _id_offsetx >= lb
    {
      PolyVector row(new_var_name);
      PolyVector const_term(-vecLBOffset[i]);
      PolyVector flag(PolyVector::OP_FLAG_POINTER, PolyVector::OP_GE);
      row = row + const_term;
      row = row + flag;
      domain_access.append_vector(row);
    }

    // _id_offsetx <= ub
    {
      PolyVector row(new_var_name, -1);
      PolyVector const_term(vecUBOffset[i]);
      PolyVector flag(PolyVector::OP_FLAG_POINTER, PolyVector::OP_GE);
      row = row + const_term;
      row = row + flag;
      domain_access.append_vector(row);
    }
    domain_access.set_iterators(&vars);
  }

  return domain_access;
}

PolyMatrix CXMLAnn_WrapperGen::GetAccessOrder(int idx, CXMLNode *pTaskDef) {
  return GetOrder();
}

//////////////////////////////////////////////////////////////////////////
// GetAccessDomain(), GetAccessOrder(), and GetAccessPattern() are used
// for the updated access pattern after data reuse optimization
// After data reuse, the iteration domain of the access reference is updated,
// which means a channel offset is applied to the iterators.
//
// These functions contains these steps:
// 1. add new offset variables into domain matrix, according to the info of
// channel_offset
// 2. keep order matrix unchanged (nothing to do with the scan order)
// 3. in the address expression, add the new offset variables
//
// Note that:
// 1. the loop level whose offset_LB == offset_UB, no new variable is
// introduced, but the offset value is also added to address
// 2. the order matrix and the inequalities in access matrix are used to match
// the corresponding iterators in the address expression
// 3. assumption: all rows in order matrix has only one iterator, otherwise the
// matching is difficult
// 4. assumption: only one row (first row) in the access matrix has the array
// variable
// 5. assumption: the inner loop iterators are expressed in access matrix in a
// standard format
PolyMatrix CXMLAnn_WrapperGen::GetAccessPattern(int idx, CXMLNode *pTaskDef) {
  size_t i;
  assert(idx >= 0 && idx < GetPortInfoNum());
  PolyMatrix access_mat = GetPolyMatrixFromPortInfoIdx(idx)->second;

  assert(GetParam("body_id") == pTaskDef->GetParam("name"));
  string sTaskName = GetParam("body_id");

  CXMLAnn_WrapperGen_port_info port_info = GetPortInfo(idx);
  string sArray = port_info.array_name;

  vector<int> vecLBOffset;
  vector<int> vecUBOffset;
  get_xml_channel_offset(pTaskDef, sTaskName, sArray, &vecLBOffset,
                         &vecUBOffset);

  if (vecLBOffset.empty()) {
    return access_mat;
  }
  assert(vecLBOffset.size() == vecUBOffset.size());

  // 0. find the unique row which contains the array address
  int addr_row_idx = -1;
  {
    string array_addr_name =
        sArray + string("__d0");  // FIXME: array is one-dimensional

    for (i = 0; i < access_mat.size(); i++) {
      if (access_mat.GetVector(i).get_coeff(array_addr_name) != 0) {
        assert(addr_row_idx == -1);
        addr_row_idx = i;
      }
    }
  }
  assert(addr_row_idx >= 0 && (size_t)addr_row_idx < access_mat.size());

  // 1. match iterators in order matrix and access matrix
  vector<string> loop_iterators;
  {
    for (i = 0; i < GetOrder().size();
         i++) {  // cloog generated loop depth (expected)
      PolyVector order_row = GetOrder().GetVector(i);
      vector<string> nonzero = order_row.get_nonzero_vars();
      assert(nonzero.size() == 1);

      loop_iterators.push_back(nonzero[0]);
    }
    vector<string> vecInnerIter = access_mat.get_parameters();
    str_vector_reverse(&vecInnerIter);  // the original order of the parameters
                                        // are from innermost to outermost
    for (i = 0; i < vecInnerIter.size(); i++) {
      loop_iterators.push_back(vecInnerIter[i]);
    }
  }

  // 2. apply the offset into the addr_row
  {
    PolyMatrix order_task = GetOrder();
    assert(vecLBOffset.size() == loop_iterators.size());
    vector<string> vars = access_mat.GetVector(addr_row_idx).get_iterators();

    for (i = 0; i < vecLBOffset.size(); i++) {
      PolyVector addr_row = access_mat.GetVector(addr_row_idx);

      string offset_var_name = c_id_offset_prefix + my_itoa(i);

      int coeff = addr_row.get_coeff(loop_iterators[i]);
      if (coeff != 0) {
        if (vecLBOffset[i] == vecUBOffset[i]) {  // add to contant term
          PolyVector zero(0);
          PolyVector &addr_row_mod = access_mat.GetVector(addr_row_idx);
          addr_row_mod = addr_row_mod + zero;
          int new_const = addr_row_mod.get_const() + vecUBOffset[i] * coeff;
          addr_row_mod.set_coeff(PolyVector::CONST_POINTER, new_const);
        } else {  // add a new variable in the new row and replace the original
                  // one
          string sNewIter = loop_iterators[i] + "__offset";

          // 1. new_it = it + offset
          PolyVector new_equation(sNewIter, -1);
          {
            PolyVector vec(loop_iterators[i]);
            new_equation = new_equation + vec;
            PolyVector offset(offset_var_name);
            new_equation = new_equation + offset;
            PolyVector flag(PolyVector::OP_FLAG_POINTER, PolyVector::OP_EQ);
            new_equation = new_equation + flag;
            access_mat.append_vector(new_equation);
          }

          // 2. it -> new_it
          {
            PolyVector iter_old(loop_iterators[i], -coeff);
            PolyVector iter_new(sNewIter, coeff);

            PolyVector &addr_row_mod = access_mat.GetVector(addr_row_idx);
            addr_row_mod = addr_row_mod + iter_old;
            addr_row_mod = addr_row_mod + iter_new;

            // cout << addr_row_mod.print() << endl;
          }
          vars.push_back(offset_var_name);
          vars.push_back(sNewIter);
        }
      }
    }
    access_mat.set_iterators(&vars);
  }

  return access_mat;
}

PolyMatrix CXMLAnn_WrapperGen::GetCombinedAccessPattern(int idx,
                                                        CXMLNode *pTaskDef) {
  CXMLAnn_WrapperGen_port_info port_info = GetPortInfo(idx);
  string sArray = port_info.array_name;
  return CombineAccessConstraints(GetAccessDomain(idx, pTaskDef),
                                  GetAccessOrder(idx, pTaskDef),
                                  GetAccessPattern(idx, pTaskDef), sArray, "");
}

//////////////////////////////////////////////////////////////////////////
// The functions for polyhedral format 2013-09-30
//////////////////////////////////////////////////////////////////////////

string CXMLAnn_Dependence::GetDistance(int is_reset) {
  if (!m_sDistance.empty() && is_reset == 0) {
    return m_sDistance;
  }

  tldm_polyhedral_info task0 = m_pTask0->poly_info;
  tldm_polyhedral_info task1 = m_pTask1->poly_info;
  tldm_polyhedral_info access0 = m_pTask0->GetPortInfo(m_nPort0).poly_info;
  tldm_polyhedral_info access1 = m_pTask1->GetPortInfo(m_nPort1).poly_info;

#ifdef DEBUG_PRINT_NEW
  printf("CalDist: (%s:%s)->(%s:%s)\n", task0.name.c_str(),
         access0.properties["ref_name"].c_str(), task1.name.c_str(),
         access1.properties["ref_name"].c_str());

  printf("\t%s\n", access0.access_pattern.c_str());
  printf("\t%s\n", access1.access_pattern.c_str());
#endif

  // 2013-10-08 zhangpeng: only support one level of graph
  tldm_polyhedral_info graph0;
  if (m_pTask0->GetParent() != nullptr) {
    graph0 = m_pTask0->GetParent()->poly_info;
  }
  tldm_polyhedral_info graph1;
  if (m_pTask1->GetParent() != nullptr) {
    graph1 = m_pTask1->GetParent()->poly_info;
  }

  // 1. merge iterator, iterator range, order vector
  tldm_polyhedral_info merge_info0 = tldm_polyinfo_merge(task0, access0);
  merge_info0 = tldm_polyinfo_merge(graph0, merge_info0);
  tldm_polyhedral_info merge_info1 = tldm_polyinfo_merge(task1, access1);
  merge_info1 = tldm_polyinfo_merge(graph1, merge_info1);

  // 2. out_level and gid_level
  int out_level0 = graph0.get_loop_level();
  int tsk_level0 = task0.get_loop_level();

  int out_level1 = graph1.get_loop_level();
  int tsk_level1 = task1.get_loop_level();

  int outer_level;
  int gid_level;

  if (out_level0 != out_level1) {
    outer_level = gid_level = 0;
  } else {
    outer_level = out_level1;
    gid_level = min(tsk_level0, tsk_level1);
  }

  // 3. call DependenceAnalysisTLDMFormat
  m_sDistance = DependenceAnalysisTLDMFormat(merge_info0, merge_info1,
                                             outer_level, gid_level);

  return m_sDistance;
}

string CXMLAnn_Dependence::print_string_format() {
  string str;

  str += "task0=\"" + m_pTask0->poly_info.name + "\" ";
  str += "port0=\"" + m_pTask0->GetPortInfo(m_nPort0).poly_info.name + "\" ";
  str += "port_idx0=\"" + my_itoa(m_nPort0) + "\" ";
  str += "task1=\"" + m_pTask1->poly_info.name + "\" ";
  str += "port1=\"" + m_pTask1->GetPortInfo(m_nPort1).poly_info.name + "\" ";
  str += "port_idx1=\"" + my_itoa(m_nPort1) + "\" ";
  str += "distance=\"" + GetDistance() + "\" ";

  return str;
}

vector<CTldmTaskAnn *> &get_tldm_task_set() { return g_tldm_task_collector; }

vector<CTldmDependenceAnn *> &get_tldm_dependence_set() {
  return g_tldm_dependence_collector;
}

CTldmTaskAnn *get_tldm_task_by_name(string sName) {
  size_t i;
  for (i = 0; i < g_tldm_task_collector.size(); i++) {
    if (g_tldm_task_collector[i]->GetParam("name") == sName) {
      return g_tldm_task_collector[i];
    }
  }
  return nullptr;
}

CXMLAnn_Dependence *create_tldm_dependence(CTldmTaskAnn *pTask0, int nPort0,
                                           CTldmTaskAnn *pTask1, int nPort1) {
  CXMLAnn_Dependence *dep =
      new CXMLAnn_Dependence(pTask0, nPort0, pTask1, nPort1);
  g_tldm_dependence_collector.push_back(dep);
  return dep;
}
CTldmTaskAnn *create_tldm_task(string sTaskName,
                               tldm_polyhedral_info poly_info) {
  CTldmTaskAnn *pNewTask = new CTldmTaskAnn;
  pNewTask->poly_info = poly_info;
  pNewTask->SetParam("name", sTaskName);
  pNewTask->SetParent(nullptr);
  g_tldm_task_collector.push_back(pNewTask);
  return pNewTask;
}
// void create_and_append_tldm_port(CTldmTaskAnn * pTask, string sPortName,
// tldm_polyhedral_info poly_info, map<string,string> mapAttribute)
void create_and_append_tldm_port(CTldmTaskAnn *pTask, string sPortName,
                                 tldm_polyhedral_info poly_info) {
  CTldmPortAnn port_info;
  port_info.array_name = sPortName;
  // port_info.properties = mapAttribute;
  port_info.poly_info = poly_info;
  pTask->AppendPortInfo(port_info);
}

CTldmPortAnn *get_tldm_task_port(string sName, int nPort) {
  CTldmTaskAnn *pTask = get_tldm_task_by_name(sName);
  return &(pTask->GetPortInfo(nPort));
}

CTldmDependenceAnn *get_tldm_dependence_by_task_and_port(string sTask0,
                                                         string sPort0,
                                                         string sTask1,
                                                         string sPort1) {
  size_t i;
  for (i = 0; i < g_tldm_dependence_collector.size(); i++) {
    CTldmDependenceAnn *dep = g_tldm_dependence_collector[i];
    if (dep->is_match(sTask0, sPort0, sTask1, sPort1) != 0) {
      return dep;
    }
  }

  return nullptr;
}
CTldmDependenceAnn *get_tldm_dependence_by_task_and_port(CTldmTaskAnn *pTask0,
                                                         int nPort0,
                                                         CTldmTaskAnn *pTask1,
                                                         int nPort1) {
  size_t i;
  for (i = 0; i < g_tldm_dependence_collector.size(); i++) {
    CTldmDependenceAnn *dep = g_tldm_dependence_collector[i];
    if (dep->is_match(pTask0, nPort0, pTask1, nPort1) != 0) {
      return dep;
    }
  }

  return nullptr;
}

void rebuild_parent_outer_level() {
  size_t i;
  vector<CTldmTaskAnn *> vecTasks = get_tldm_task_set();
  for (i = 0; i < vecTasks.size(); i++) {
    tldm_polyhedral_info poly_info = vecTasks[i]->poly_info;
#ifdef DEBUG_PRINT_NEW
    printf("[Mark Parent] %s : %s\n", poly_info.name.c_str(),
           poly_info.type.c_str());
#endif
    if (poly_info.type != "task") {
      continue;
    }

    string graph_name = poly_info.properties["graph_name"];
    CTldmTaskAnn *graph = get_tldm_task_by_name(graph_name);
    assert(graph);
    CTldmTaskAnn *pOldTask = get_tldm_task_ann_by_name(poly_info.name);
    if (pOldTask != nullptr) {
      pOldTask->SetParent(graph);
    }
    vecTasks[i]->SetParent(graph);

    int out_level = graph->poly_info.get_loop_level();
    poly_info.outer_level = out_level;

    for (int j = 0; j < vecTasks[i]->GetPortInfoNum(); j++) {
      vecTasks[i]->GetPortInfo(j).poly_info.outer_level = out_level;
      if (pOldTask != nullptr) {
        pOldTask->GetPortInfo(j).poly_info.outer_level = out_level;
      }
    }
  }
}

void rebuild_dep_graph_link() {
  size_t i;

  // 1. clean all the links first
  vector<CTldmTaskAnn *> vecTasks = get_tldm_task_set();
  for (i = 0; i < vecTasks.size(); i++) {
    CTldmTaskAnn *pTaskAnn = vecTasks[i];
    for (int j = 0; j < pTaskAnn->GetPortInfoNum(); j++) {
      pTaskAnn->GetPortInfo(j).dependence.clear();
    }
  }

  // 2. rebuild the link list
  vector<CTldmDependenceAnn *> dep_set = get_tldm_dependence_set();
  for (i = 0; i < dep_set.size(); i++) {
    CTldmDependenceAnn *pDepAnn = dep_set[i];

    CTldmTaskAnn *pTaskAnn0 = pDepAnn->GetTask0();
    CTldmTaskAnn *pTaskAnn1 = pDepAnn->GetTask1();
    int nPort0 = pDepAnn->GetPort0();
    int nPort1 = pDepAnn->GetPort1();

    pTaskAnn0->GetPortInfo(nPort0).dependence.push_back(pDepAnn);
    pTaskAnn1->GetPortInfo(nPort1).dependence.push_back(pDepAnn);
  }
}

//////////////////////////////////////////////////////////////////////////
// 2013-10-08, zhangpeng
// For this version, we do not support multiple write references for an array
//////////////////////////////////////////////////////////////////////////
int rebuild_dep_graph(string dep_filename /* = ""*/) {
  size_t i;
  size_t ii;
  vector<CTldmTaskAnn *> vecTasks = get_tldm_task_set();

  string sDepPragmas;

  // for each producer port, search its consumers
  for (i = 0; i < vecTasks.size(); i++) {
    CTldmTaskAnn *pTaskAnn = vecTasks[i];
    string sTaskName = pTaskAnn->GetParam("name");

    for (int j = 0; j < pTaskAnn->GetPortInfoNum(); j++) {
      // 1. find a write port
      CTldmPortAnn *port_info = get_tldm_task_port(sTaskName, j);
      tldm_polyhedral_info poly_info = port_info->poly_info;
      string sDir = get_sub_delimited(poly_info.access_pattern, 0, ':');
      if ("w" != sDir) {
        continue;
      }

      //////////////////////////////////////////////////////////////////////////
      // TO BE FIXED
      if (poly_info.properties["port_type"].empty()) {
        continue;
      }
      if (poly_info.properties["port_type"] == "iterator") {
        continue;
      }
      if (poly_info.properties["port_type"] == "param") {
        continue;
      }

      string sArray = poly_info.name;

      // 2. find all the read reference of sArray
      for (ii = 0; ii < vecTasks.size(); ii++) {
        CTldmTaskAnn *pTaskAnnR = vecTasks[ii];
        string sTaskNameR = pTaskAnnR->GetParam("name");

        for (int jj = 0; jj < pTaskAnnR->GetPortInfoNum(); jj++) {
          // 1. find a write port
          CTldmPortAnn *port_infoR = get_tldm_task_port(sTaskNameR, jj);
          tldm_polyhedral_info poly_infoR = port_infoR->poly_info;
          if (sArray != poly_infoR.name) {
            continue;
          }
          string sDir = get_sub_delimited(poly_infoR.access_pattern, 0, ':');
          if ("r" != sDir) {
            continue;
          }
          if (poly_infoR.properties["port_type"].empty()) {
            continue;
          }

          CTldmDependenceAnn *dep =
              create_tldm_dependence(pTaskAnn, j, pTaskAnnR, jj);

          sDepPragmas +=
              "#pragma tldm dependence " + dep->print_string_format() + "\n";
        }
      }
    }
  }

  if (!dep_filename.empty()) {
    write_string_into_file(sDepPragmas, dep_filename);
  }

  rebuild_dep_graph_link();
  return 1;
}

int dump_task_pragma(string filename /* ="task_pragma.c" */) {
  size_t i;
  string str_info;
  vector<CTldmTaskAnn *> vecTasks = get_tldm_task_set();
  for (i = 0; i < vecTasks.size(); i++) {
    tldm_polyhedral_info poly_info = vecTasks[i]->poly_info;
    // if (poly_info.type != "task") continue;

    str_info += "void " + poly_info.name + "_kernel_detected() \n{ \n";

    str_info += "#pragma tldm iteration_domain " + string("") + "iterator=\"" +
                poly_info.iterator_vector + "\" " + string("") +
                // "max_range=\""+poly_info.iterator_range+"\" " + string("") +
                "max_range=\"0..1\" " +
                string("") +  // TOFIX: we do not use this info later.
                "\n";

    str_info += "#pragma tldm tuning " + string("") + "iterator=\"" +
                poly_info.iterator_vector + "\" " + "order=\"" +
                poly_info.order_vector + "\" " + poly_info.to_string() + "\n";

    for (int j = 0; j < vecTasks[i]->GetPortInfoNum(); j++) {
      poly_info = vecTasks[i]->GetPortInfo(j).poly_info;

      if (poly_info.condition_vector.empty()) {
        poly_info.condition_vector = "1";
      }
      if (poly_info.type == "task access") {
        poly_info.type = "access";
      }

      str_info += "#pragma tldm access " + string("") + "port=\"" +
                  poly_info.name + "\" " + string("") + "dir=\"" +
                  string(poly_info.access_pattern[0] == 'w' ? "w" : "r") +
                  "\" " + poly_info.to_string() + "\n";
    }

    str_info += "}\n\n";
  }

  // write_string_into_file(str_info, "task_pragma.c");
  write_string_into_file(str_info, filename);
  return 1;
}
