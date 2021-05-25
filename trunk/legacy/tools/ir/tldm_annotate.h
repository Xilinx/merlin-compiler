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



#ifndef TRUNK_LEGACY_TOOLS_IR_TLDM_ANNOTATE_H_
#define TRUNK_LEGACY_TOOLS_IR_TLDM_ANNOTATE_H_

#include <list>
#include <utility>
#include <map>
#include <vector>
#include <string>
#include "cmdline_parser.h"
#include "xml_parser.h"
#include "PolyModel.h"

using std::map;
using std::string;
using std::vector;

typedef pair<string, PolyMatrix> named_PolyMatrix;

class CXMLAnn_Dependence;

class reuse_chain_node {
 public:
  int from;
  int to;
  int buffer_size;
  string sArray;
};

class CXMLAnn_Polyhedral : public CXMLNodeAnnotationBase {
 public:
  virtual string GetClassString() { return "CXMLAnn_Polyhedral"; }
  PolyMatrix &GetDomain() { return m_domain; }
  PolyMatrix &GetOrder() {
    return m_order;
  }  // the scatter function: loop level = m_order.size(),
     // each row is a level of
     // ordering of one loop level, from outer loops to inner ones
  vector<named_PolyMatrix> &GetInputs() { return m_inputs; }
  vector<named_PolyMatrix> &GetOutputs() { return m_outputs; }
  vector<named_PolyMatrix> &GetDeps() { return m_deps; }
  string print();

  // vector<list<reuse_chain_node> > & GetReuseChain() { return m_reuse_chain; }

 protected:
  // 1. domain
  // 2. order
  // 3. io
  // 4. dept
  PolyMatrix m_domain;                /*!< \brief iteration domain */
  PolyMatrix m_order;                 /*!< \brief loop transformation matrix */
  vector<named_PolyMatrix> m_inputs;  /*!< \brief The list of input data */
  vector<named_PolyMatrix> m_outputs; /*!< \brief The list of output data */
  vector<named_PolyMatrix> m_deps;    /*!< \brief The list of dependent task */

  // vector<list<reuse_chain_node> > m_reuse_chain; //
  // m_reuse_chain[chain_idx][intra_chain_idx] -> {access reference index,
  // buffer size, array_name}
};

typedef struct CXMLAnn_WrapperGen_port_info__ {
  string array_name;
  string array_position;
  string port_id;  // array name
  int position;
  int io_type;  // 0 for output, 1 for input, 2 for iterator
  string port_type;
  string monitor;
  int rate;
  string unit_size;
  PolyMatrix addr_map;
  string addr_mapping;
  int to_sw;  // indicate it is a fifo I/F connected to software
  int sync_level;
  int matrix_index;  // used internally, indicate the position of polymatrix in
                     // the CXMLAnn_Polyhedral inputs/outputs
  int port_index;    // used internally, indicate the position of port
  map<string, string> properties;  // other annotations

  string ref;  // added on Feb3

  int is_bus() { return "bus" == port_type || "bus_e1" == port_type; }
  int is_fifo() { return "fifo" == port_type || "bfifo" == port_type; }

  // new representations for tldm information
  tldm_polyhedral_info poly_info;

  // store the dependence or reuse distance information between reference pairs
  vector<CXMLAnn_Dependence *> dependence;

  string print_string_format() {
    string str = poly_info.to_string();
    map<string, string>::iterator it;
    for (it = properties.begin(); it != properties.end(); it++) {
      str += it->first + "=\"" + it->second + "\" ";
    }
    return str;
  }
} CXMLAnn_WrapperGen_port_info;

class CXMLAnn_WrapperGen : public CXMLAnn_Polyhedral {
 public:
  enum {
    IOTYPE_OUTPUT = 0,  // DO not try to change the order!!!
    IOTYPE_INPUT,
    IOTYPE_ITERATOR
  };

 public:
  virtual string GetClassString() { return "CXMLAnn_WrapperGen"; }

  void SetParam(string key, string val) { m_mapParams[key] = val; }
  string GetParam(string key) {
    if (m_mapParams.end() == m_mapParams.find(key))
      return "";
    else
      return m_mapParams[key];
  }
  map<string, string> GetAllParams() { return m_mapParams; }

  void AppendPortInfo(CXMLAnn_WrapperGen_port_info port_info) {
    port_info.port_index = GetPortInfoNum();
    m_vecPortInfo.push_back(port_info);
  }
  int GetPortInfoNum() { return m_vecPortInfo.size(); }
  CXMLAnn_WrapperGen_port_info &GetPortInfo(int i) { return m_vecPortInfo[i]; }

  vector<string> &GetDebugPort() { return m_vecDebugPort; }

  string print();

  int GetPortInfoIdxFromPolyMatrix(int is_write, int matrix_idx) {
    int i;
    for (i = 0; i < GetPortInfoNum(); i++) {
      if (is_write != (GetPortInfo(i).io_type == IOTYPE_OUTPUT))
        continue;
      if (matrix_idx != GetPortInfo(i).matrix_index)
        continue;
      return i;
    }
    return -1;
  }
  CXMLAnn_WrapperGen_port_info *GetPortInfoFromPolyMatrix(int is_write,
                                                          int matrix_idx) {
    int i;
    for (i = 0; i < GetPortInfoNum(); i++) {
      if (is_write != (GetPortInfo(i).io_type == IOTYPE_OUTPUT))
        continue;
      if (matrix_idx != GetPortInfo(i).matrix_index)
        continue;
      return &GetPortInfo(i);
    }
    return nullptr;
  }
  named_PolyMatrix *GetPolyMatrixFromPortInfoIdx(int port_idx) {
    int is_write = GetPortInfo(port_idx).io_type == IOTYPE_OUTPUT;
    int matrix_idx = GetPortInfo(port_idx).matrix_index;
    vector<named_PolyMatrix> &vecInputsAll =
        is_write ? (GetOutputs()) : (GetInputs());
    if (matrix_idx < 0 || matrix_idx >= static_cast<int>(vecInputsAll.size()))
      return nullptr;
    return &(vecInputsAll[matrix_idx]);
  }

  void AppendPortPair(named_PolyMatrix one_access,
                      CXMLAnn_WrapperGen_port_info port_info) {
    if (port_info.io_type == IOTYPE_INPUT) {
      port_info.matrix_index = GetInputs().size();
      GetInputs().push_back(one_access);
    } else if (port_info.io_type == IOTYPE_OUTPUT) {
      port_info.matrix_index = GetOutputs().size();
      GetOutputs().push_back(one_access);
    } else {
      assert(0);
    }

    AppendPortInfo(port_info);
  }

  // These two functions are used together to apply domain offset in ldu and gdu
  // specifications
  PolyMatrix GetAccessDomain(int idx, CXMLNode *pTaskDef);
  PolyMatrix GetAccessOrder(int idx, CXMLNode *pTaskDef);
  PolyMatrix GetAccessPattern(int idx, CXMLNode *pTaskDef);
  PolyMatrix GetCombinedAccessPattern(int idx, CXMLNode *pTaskDef);

  // 2013-09-30
  // new representations for tldm information
  tldm_polyhedral_info poly_info;

  string print_string_format() {
    string str = poly_info.to_string();

    str += "\\\n";

    map<string, string>::iterator it;
    for (it = m_mapParams.begin(); it != m_mapParams.end(); it++) {
      str += it->first + "=\"" + it->second + "\" ";
    }
    return str;
  }

  CXMLAnn_WrapperGen *GetParent() { return m_parent; }
  void SetParent(CXMLAnn_WrapperGen *pParent) { m_parent = pParent; }
  string GetParentName() {
    if (m_parent)
      return m_parent->GetParam("name");
    else
      return "root";
  }
  // int  GetTaskType() { return m_nTaskType; }
  // void SetTaskType(int nTaskType) { m_nTaskType = nTaskType; }

  int get_outer_level_beyond_graph() {
    if (!GetParent())
      return 0;
    else
      return GetParent()->poly_info.get_loop_level();
  }

 protected:
  CXMLAnn_WrapperGen *m_parent;
  int m_nTaskType;  // 0: kernel; 1: graph; 2: host; 3: undefined

  // Scalar parameters
  // 1. body id
  // 2. translate type
  // 3. data_type data_def
  map<string, string> m_mapParams;

  // 3. io port port id, and address mapping
  vector<CXMLAnn_WrapperGen_port_info> m_vecPortInfo;

  // 4. debug ports
  vector<string> m_vecDebugPort;

  // 5. program utilities
  map<named_PolyMatrix *, CXMLAnn_WrapperGen_port_info *>
      m_mapPolyMatrix2PortInfo;
  map<CXMLAnn_WrapperGen_port_info *, named_PolyMatrix *>
      m_mapPortInfo2PolyMatrix;
};

class CXMLAnn_Dependence : public CXMLNodeAnnotationBase {
 public:
  CXMLAnn_Dependence(CXMLAnn_WrapperGen *pTask0, int nPort0,
                     CXMLAnn_WrapperGen *pTask1, int nPort1) {
    m_pTask0 = pTask0;
    m_pTask1 = pTask1;
    m_nPort0 = nPort0;
    m_nPort1 = nPort1;
    CXMLAnn_WrapperGen_port_info info0 = m_pTask0->GetPortInfo(nPort0);
    CXMLAnn_WrapperGen_port_info info1 = m_pTask1->GetPortInfo(nPort1);
    m_nIsWrite0 = (info0.io_type == CXMLAnn_WrapperGen::IOTYPE_OUTPUT);
    m_nIsWrite1 = (info1.io_type == CXMLAnn_WrapperGen::IOTYPE_OUTPUT);
    m_sDistance = "";  // initial value
  }
  int GetDependenceType() {  // return 0:RAW, 1:WAW, 2:RAR, 3:WAR
    return ((m_nIsWrite1 == 1) << 1) | (m_nIsWrite0 == 0);
  }
  string GetDistance(int is_reset = 0);
  string print_string_format();

  int is_match(CXMLAnn_WrapperGen *pTask0, int nPort0,
               CXMLAnn_WrapperGen *pTask1, int nPort1) {
    if (m_pTask0 != pTask0)
      return 0;
    if (m_pTask1 != pTask1)
      return 0;
    if (m_nPort0 != nPort0)
      return 0;
    if (m_nPort1 != nPort1)
      return 0;
    return 1;
  }

  int is_match(string sTask0, string sPort0, string sTask1, string sPort1) {
    if (m_pTask0->poly_info.name != sTask0)
      return 0;
    if (m_pTask1->poly_info.name != sTask1)
      return 0;
    if (m_pTask0->GetPortInfo(m_nPort0).poly_info.name != sPort0)
      return 0;
    if (m_pTask1->GetPortInfo(m_nPort1).poly_info.name != sPort1)
      return 0;
    return 1;
  }
  CXMLAnn_WrapperGen *GetTask0() { return m_pTask0; }
  CXMLAnn_WrapperGen *GetTask1() { return m_pTask1; }
  int GetPort0() { return m_nPort0; }
  int GetPort1() { return m_nPort1; }

 protected:
  // dependence from 0 to 1
  string m_sTask0, m_sTask1;
  int m_nPort0, m_nPort1;
  int m_nIsWrite0, m_nIsWrite1;
  CXMLAnn_WrapperGen *m_pTask0;
  CXMLAnn_WrapperGen *m_pTask1;

  string m_sDistance;
};

void get_connect_node_from_dependence(CXMLNode *pTLDMRoot, string proTaskName,
                                      string conTaskName, string dataName,
                                      CXMLNode **proConnect,
                                      CXMLNode **conConnect);
//////////////////////////////////////////////////////////////////////////
// 1. Data structure construction and deconstruction
//////////////////////////////////////////////////////////////////////////
int mark_tldm_annotation(CXMLNode *pTLDMRoot);
void print_tldm_annotation(CXMLNode *pTLDMRoot);
int mark_tldm_annotation_release_resource();
void tldm_clean_all_new_annotation();

//////////////////////////////////////////////////////////////////////////
// 2. XML attribute access functions
//////////////////////////////////////////////////////////////////////////
CXMLAnn_WrapperGen *get_tldm_task_ann_by_name(string sName);

vector<CXMLNode *> tldm_xml_get_data_ref_of_port(string task_name,
                                                 string sPortName,
                                                 CXMLNode *pTLDMRoot);
CXMLNode *tldm_xml_get_task_by_name(string task_name, CXMLNode *pRoot);
CXMLNode *tldm_xml_task_append_attribute(CXMLNode *pTaskNode, string sAttrName,
                                         string sAttrValue);
CXMLNode *tldm_xml_task_append_attribute_ex(CXMLNode *pTaskNode,
                                            string sAttrName, string sItemName,
                                            map<string, string> mapValue);
string tldm_xml_get_attribute(
    CXMLNode *pTaskNode,
    string sAttrName);  // pTaskNode can be pDataRef for connections
void tldm_xml_set_attribute(CXMLNode *pTaskNode, string sAttrName,
                            string sValue);

//////////////////////////////////////////////////////////////////////////
// 3. Annotation access functions 2013-09-30
//////////////////////////////////////////////////////////////////////////
typedef CXMLAnn_WrapperGen CTldmTaskAnn;
typedef CXMLAnn_WrapperGen_port_info CTldmPortAnn;
typedef CXMLAnn_Dependence CTldmDependenceAnn;

vector<CTldmTaskAnn *> &get_tldm_task_set();
vector<CTldmDependenceAnn *> &get_tldm_dependence_set();

CTldmDependenceAnn *create_tldm_dependence(CTldmTaskAnn *pTask0, int nPort0,
                                           CTldmTaskAnn *pTask1, int nPort1);
CTldmTaskAnn *create_tldm_task(string sTaskName,
                               tldm_polyhedral_info poly_info);
// void                 create_and_append_tldm_port(CTldmTaskAnn * pTask, string
// sPortName, tldm_polyhedral_info poly_info, map<string,string> mapAttribute);
void create_and_append_tldm_port(CTldmTaskAnn *pTask, string sPortName,
                                 tldm_polyhedral_info poly_info);

CTldmTaskAnn *get_tldm_task_by_name(string sName);
CTldmPortAnn *get_tldm_task_port(string sName, int nPort);
// CTldmDependenceAnn * get_tldm_dependence_by_task_and_port(string sTask0,
// string sPort0, string sTask1, string sPort1);
CTldmDependenceAnn *get_tldm_dependence_by_task_and_port(CTldmTaskAnn *pTask0,
                                                         int nPort0,
                                                         CTldmTaskAnn *pTask1,
                                                         int nPort1);
void rebuild_parent_outer_level();
int rebuild_dep_graph(
    string dep_filename = "");  // do not dump if filename is empty
int dump_task_pragma(string filename = "task_pragma.c");

// the followings are the suggested functions to access the new polyhedral
// format (2013.09.30)
// int    CTldmTaskAnn::GetPortInfoNum();
// CTldmPortAnn   CTldmTaskAnn::GetPortInfo(int idx);

// string CTldmTaskAnn::print_string_format();
// string CTldmPortAnn::print_string_format();
// string CTldmDependenceAnn::print_string_format();

// CTldmTaskAnn * CTldmTaskAnn::GetParent();
// void   CTldmTaskAnn::SetParent(CTldmTaskAnn * pParent) ;
// string CTldmTaskAnn::GetParentName() ;
// int    CTldmTaskAnn::GetTaskType();
// void   CTldmTaskAnn::SetTaskType(int nTaskType) ;

#endif  // TRUNK_LEGACY_TOOLS_IR_TLDM_ANNOTATE_H_
