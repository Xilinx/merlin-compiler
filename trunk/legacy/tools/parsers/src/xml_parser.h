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



#ifndef TRUNK_LEGACY_TOOLS_PARSERS_SRC_XML_PARSER_H_
#define TRUNK_LEGACY_TOOLS_PARSERS_SRC_XML_PARSER_H_

#include <stdio.h>
#include <assert.h>

#include <vector>
#include <map>
#include <string>
#include "cmdline_parser.h"

using std::map;
using std::string;
using std::vector;

class CXMLTree;

class CXMLNodeAnnotationBase {
 public:
  virtual string GetClassString() { return "CXMLNodeAnnotationBase"; }
  virtual ~CXMLNodeAnnotationBase() {}

 protected:
};

class XML_Object_Collector {
 public:
  void Append(CXMLNodeAnnotationBase *p) { m_vecPointers.push_back(p); }
  void CleanAll() {
    for (size_t i = 0; i < m_vecPointers.size(); i++) {
      CXMLNodeAnnotationBase *&p = m_vecPointers[i];
      if (p) {
        delete p;
        p = nullptr;
      }
    }
    m_vecPointers.clear();
  }

 protected:
  vector<CXMLNodeAnnotationBase *> m_vecPointers;
};

class CXMLNode {
 public:
  CXMLNode(CXMLTree *pXMLTree, int dummy) {
    assert(15479 == dummy);  // make sure only XMLTree can create object.
    m_pXMLTree = pXMLTree;
    m_Parent = nullptr;
  }

  CXMLNode(CXMLTree *pXMLTree, string sName, int dummy) {
    assert(15479 == dummy);  // make sure only XMLTree can create object.
    m_pXMLTree = pXMLTree;
    m_Parent = nullptr;
    SetName(sName);
  }

  int SetParam(string sParam, string sValue) {
    int found = m_Parameters.find(sParam) != m_Parameters.end();
    m_Parameters[sParam] = sValue;
    return found;
  }
  string GetParam(string sParam) { return m_Parameters[sParam]; }
  string DumpParams() {
    string dump;
    map<string, string>::iterator it;
    for (it = m_Parameters.begin(); it != m_Parameters.end(); it++) {
      if (it->first == "name" && it->second == "")
        continue;
      dump +=
          string(" ") + it->first + string("=\"") + it->second + string("\"");
    }
    return dump;
  }

  int GetDepth() {
    int nLevel = 0;
    CXMLNode *pNode = GetParent();
    while (pNode) {
      pNode = pNode->GetParent();
      nLevel++;
    }
    return nLevel;
  }
  int isLeaf() { return m_Children.size() == 0; }

  CXMLNode *CreateAppendChild();
  CXMLNode *CreateAppendChild(string sName) {
    CXMLNode *pNode = CreateAppendChild();
    pNode->SetName(sName);
    return pNode;
  }
  void AppendChild(CXMLNode *pNode) {
    m_Children.push_back(pNode);
    pNode->SetParent(this);
  }
  void AppendChild_attr_by_name(string attr_name, CXMLNode *pNode) {
    if (pNode) {
      AppendChild(pNode);
    } else {
      CXMLNode *pAttr = CreateAppendChild("attribute");
      pAttr->SetParam("name", attr_name);
      CXMLNode *pValue = pAttr->CreateAppendChild("string");
      pValue->SetValue("");
    }
  }
  int InsertChild(int idx, CXMLNode *pXMLNode) {
    int i;
    vector<CXMLNode *>::iterator it;
    for (it = m_Children.begin(), i = 0; i < idx; i++)
      it++;
    m_Children.insert(it, pXMLNode);
    pXMLNode->SetParent(this);
    return 1;
  }
  int RemoveChildByIndex(int idx) {
    int i;
    vector<CXMLNode *>::iterator it;
    for (it = m_Children.begin(), i = 0; i < idx; i++)
      it++;
    m_Children.erase(it);
    return 1;
  }
  CXMLNode *
  GetChildByName(string sName,
                 int idx = 0) {  // idx can be not zero when multiple hits
    size_t i;
    int j = 0;
    for (i = 0; i < m_Children.size(); i++) {
      if (m_Children[i]->GetName() == sName) {
        if (j >= idx)
          return m_Children[i];
        j++;
      }
    }

    return nullptr;
  }
  string GetFirstChildValue() {  // idx can be not zero when multiple hits
    return GetChildByIndex(0)->GetValue();
  }
  int GetChildNum() { return m_Children.size(); }
  CXMLNode *GetChildByIndex(int idx = 0) { return m_Children[idx]; }

  // level = -1 for all levels
  // level = 0 only for current level no child
  typedef int (*XMLTreeTraverseTestFunc)(CXMLNode *pNode,
                                         const vector<string> &sArguments);
  vector<CXMLNode *> TraverseByName(string sName, int level = -1);
  vector<CXMLNode *> TraverseByName1(string sName, int level = -1);
  vector<CXMLNode *> TraverseByCondition(XMLTreeTraverseTestFunc pfTestFunc,
                                         const vector<string> &sArgument,
                                         int level = -1);

  void SetParent(CXMLNode *pParent) { m_Parent = pParent; }
  CXMLNode *GetParent() { return m_Parent; }
  int GetLevel() {
    int i = 0;
    CXMLNode *pParent = GetParent();
    while (pParent) {
      pParent = pParent->GetParent();
      i++;
    }
    return i;
  }

  void SetName(string sName) { m_sName = sName; }
  string GetName() { return m_sName; }
  void SetValue(string sValue) { m_sValue = sValue; }
  string GetValue() { return m_sValue; }
  int isComment() { return m_sName == string(""); }

 public:
  CXMLNode *CloneSubTree(CXMLTree *newTree);
  void RemoveSubTree();
  void ReplaceSubTree(CXMLNode *pNewNode);
  CXMLTree *GetXMLTree() { return m_pXMLTree; }

 public:
  CXMLNodeAnnotationBase *GetAnnotation(string sAnnName) {
    if (m_mapAnnotations.end() == m_mapAnnotations.find(sAnnName))
      return nullptr;

    CXMLNodeAnnotationBase *pAnn = m_mapAnnotations[sAnnName];
    if (!pAnn)
      return nullptr;
    assert(pAnn->GetClassString() == sAnnName);
    return pAnn;
  }
  void SetAnnotation(CXMLNodeAnnotationBase *pAnn) {
    assert(pAnn);
    string sAnnName = pAnn->GetClassString();
    assert(sAnnName != "");
    assert(sAnnName != "CXMLNodeAnnotationBase");
    assert(nullptr ==
           GetAnnotation(sAnnName));  // must clear the content outside before
                                      // setting new value
    m_mapAnnotations[sAnnName] = pAnn;
  }

 protected:
  void TraverseByName(string sName, vector<CXMLNode *> *vecNodes, int depth);
  void TraverseByCondition(XMLTreeTraverseTestFunc pfTestFunc,
                           const vector<string> &sArgument,
                           vector<CXMLNode *> *vecNodes, int depth);

  CXMLNode *CloneSubTree_in(CXMLTree *newTree, CXMLNode *pParent = nullptr);

 protected:
  map<string, string> m_Parameters;
  CXMLNode *m_Parent;
  vector<CXMLNode *> m_Children;
  string m_sName;
  string m_sValue;

  map<string, CXMLNodeAnnotationBase *> m_mapAnnotations;

  CXMLTree *m_pXMLTree;
};

class CXMLTree {
 public:
  CXMLTree() {}
  ~CXMLTree() { Clean(); }
  CXMLNode *CreateNode(string sName = string("")) {
    CXMLNode *newNode = new CXMLNode(this, sName, 15479);
    m_NodeList.push_back(newNode);
    return newNode;
  }
  void Clean() {
    size_t i;
    for (i = 0; i < m_NodeList.size(); i++) {
      if (m_NodeList[i]) {
        delete m_NodeList[i];
        m_NodeList[i] = nullptr;
      }
    }
  }

  void setRoot(CXMLNode *pRoot) {
    m_pRoot = pRoot;
    if (m_pRoot)
      m_pRoot->SetParent(nullptr);
  }
  CXMLNode *getRoot() { return m_pRoot; }

 protected:
  CXMLNode *m_pRoot;
  vector<CXMLNode *> m_NodeList;
};

class CXMLParser {
 public:
  CXMLNode *parse_from_file(string sFilename);
  CXMLNode *parse_empty(string sName = "top");
  void write_into_file(string sFilename);
  CXMLTree *get_xml_tree() { return &m_XMLTree; }

  CXMLNode *build_element_from_string(string sElement);
  string dump_element_to_string(CXMLNode *pNode);

 protected:
  void FormatXMLString(string *str);
  string dump_parameter_to_string(CXMLNode *pNode);

 protected:
  CXMLTree m_XMLTree;
};

int CheckXMLNodeName(CXMLNode *pNode, const vector<string> &vecArg);
CXMLNode *SearchNodeNameUnique(CXMLNode *pScope, string sNode,
                               string sName = "");
CXMLNode *SearchNodeNameFirst(CXMLNode *pScope, string sNode,
                              string sName = "");
void MergeNodeWithPriority(CXMLNode *pOrgMerged, CXMLNode *pLowPriority);

// The interface to read or set the configurations in CMOST configuration format
// printf("Usage: set_cfg [-i get|set] cfg_xml hier1 hier2 ... value\n");
// printf("    Set/Get the configuration xxx/hier1/xxx/hier2/.../hierx into
// value.\n"); printf("    each hier can be \"name\" or
// \"name:param:param_value\"\n");
string set_xml_cfg(const CInputOptions &options);

void set_xml_cfg_simple(string xml_file, string node, string _value);
string get_xml_cfg_simple(string xml_file, string node);

void get_xml_channel_offset(CXMLNode *pRoot, string sTaskName, string sPortName,
                            vector<int> *vecLBOffset, vector<int> *vecUBOffset);

#endif  // TRUNK_LEGACY_TOOLS_PARSERS_SRC_XML_PARSER_H_
