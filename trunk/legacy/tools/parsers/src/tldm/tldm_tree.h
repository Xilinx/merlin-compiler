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



#ifndef TRUNK_LEGACY_TOOLS_PARSERS_SRC_TLDM_TLDM_TREE_H_
#define TRUNK_LEGACY_TOOLS_PARSERS_SRC_TLDM_TLDM_TREE_H_

#include <assert.h>
#include <stdlib.h>
#include <string>
#include "xml_parser.h"

using std::string;

class CTLDMTree : public CXMLTree {
 public:
  CXMLNode *CreateLeave(string type, string sValue) {
    CXMLNode *pDef;

    if ("identifier" == type) {
      pDef = CreateNode("identifier");
      pDef->SetValue(sValue);
    } else if ("int" == type) {
      pDef = CreateNode("integer");
      pDef->SetValue(sValue);
    } else if ("comment" == type) {
      pDef = CreateNode("comment");
      pDef->SetValue(sValue);
    } else if ("string" == type) {
      pDef = CreateNode("string");
      pDef->SetValue(sValue);
    } else {
      assert(0);
    }

    return pDef;
  }

  CXMLNode *AppendList(CXMLNode *pList, CXMLNode *pNode) {
    pList->AppendChild(pNode);
    return pList;
  }

  CXMLNode *CreateTaskDef(CXMLNode *pDomainRef, CXMLNode *pTaskRef) {
    pTaskRef->SetName("task_def");
    if (pDomainRef)
      pTaskRef->AppendChild(pDomainRef);
    return pTaskRef;
  }

  CXMLNode *AppendComment(CXMLNode *pNode, CXMLNode *pComment) {
    pNode->AppendChild(pComment);
    return pNode;
  }

  CXMLNode *AppendBody(CXMLNode *pNode, CXMLNode *pBody) {
    if (pBody) {
      unsigned int i;
      for (i = 0; i < pBody->GetChildNum(); i++) {
        pNode->AppendChild(pBody->GetChildByIndex(i));
      }
    }
    return pNode;
  }

  CXMLNode *CreateDomainDef(CXMLNode *pDomainRef, CXMLNode *pBody) {
    assert(pDomainRef);
    pDomainRef->SetName("iteration_domain_def");
    if (pBody)
      AppendBody(pDomainRef, pBody);
    return pDomainRef;
  }

  CXMLNode *CreateDataDef(CXMLNode *pDomainRef, CXMLNode *pType,
                          CXMLNode *pVarPos) {
    unsigned int i;
    pVarPos->SetName("data_def");

    // assert("" != pVarPos->GetValue()); // make sure varpos is from var, not
    // exp
    if (0 ==
        pVarPos->GetValue().size()) {  // make sure varpos is from var, not exp
      printf("var_pos->value = %s.\n", pVarPos->GetValue().c_str());
      exit(0);
    }
    if (0 != pVarPos->GetValue().size()) {  //
      pVarPos->SetParam("name", pVarPos->GetValue());
      pVarPos->SetValue("");
    }

    if (pDomainRef)
      pVarPos->AppendChild(pDomainRef);

    pType->SetName("data_type");
    pVarPos->AppendChild(pType);
    return pVarPos;
  }

  CXMLNode *CreateConnectList(CXMLNode *data_ref_in, CXMLNode *task_ref,
                              CXMLNode *data_ref_out) {
    CXMLNode *pNode = CreateNode("connect_list");
    if (data_ref_in) {
      CreateConnectFromList(pNode, "in", data_ref_in, task_ref);
    }
    if (data_ref_out) {
      CreateConnectFromList(pNode, "out", data_ref_out, task_ref);
    }
    return pNode;
  }

  CXMLNode *CreateDependence(string sType, CXMLNode *task_ref1,
                             CXMLNode *task_ref2) {
    CXMLNode *pNode = CreateNode("dependence");
    pNode->AppendChild(task_ref1);
    pNode->AppendChild(task_ref2);

    CXMLNode *pType = pNode->CreateAppendChild();
    pType->SetName("type");
    pType->SetValue(sType);
    return pNode;
  }

  CXMLNode *CreateRef(string name, CXMLNode *pVarPos, CXMLNode *pVarListExp) {
    if (name != "")
      pVarPos->SetName(name);
    else
      pVarPos->SetName("reference");

    // assert("" != pVarPos->GetValue()); // make sure varpos is from var, not
    // exp
    if (0 ==
        pVarPos->GetValue().size()) {  // make sure varpos is from var, not exp
      printf("var_pos->value = %s.\n", pVarPos->GetValue().c_str());
      exit(0);
    }
    if (0 != pVarPos->GetValue().size()) {  //
      pVarPos->SetParam("name", pVarPos->GetValue());
      pVarPos->SetValue("");
    }

    if (pVarListExp)
      pVarPos->AppendChild(pVarListExp);
    return pVarPos;
  }

  CXMLNode *CreateVarPos(CXMLNode *pVar, CXMLNode *pPos) {
    pVar->SetName("var_pos");
    if (pPos)
      pVar->SetParam("position", pPos->GetValue());
    return pVar;
  }

  CXMLNode *CreateList(string sName, CXMLNode *pItem) {
    CXMLNode *pNode = CreateNode(sName);
    pNode->AppendChild(pItem);
    return pNode;
  }

  // CXMLNode * CreateExp(CXMLNode * pOpcode, CXMLNode * pOprand1, CXMLNode *
  // pOprand2)
  // {
  // CXMLNode * pNode = CreateNode("expression");
  //  pNode->AppendChild(pOpcode);
  // pNode->AppendChild(pOprand1);
  // pNode->AppendChild(pOprand2);
  // return pNode;
  // }
  CXMLNode *CreateExp(string sOpcode, CXMLNode *pOprand1, CXMLNode *pOprand2) {
    CXMLNode *pNode = CreateNode("expression");
    pNode->SetParam("opcode", sOpcode);
    pNode->AppendChild(pOprand1);
    pNode->AppendChild(pOprand2);
    return pNode;
  }
  CXMLNode *CreateRangeExp(CXMLNode *pOprand1, CXMLNode *pOprand2) {
    if (!pOprand1)
      pOprand1 = CreateNode("negative_infinite");
    if (!pOprand2)
      pOprand2 = CreateNode("positive_infinite");
    return CreateExp("range", pOprand1, pOprand2);
  }

  CXMLNode *CreateAttrib(CXMLNode *pAttrName, CXMLNode *pArgList) {
    CXMLNode *pNode = CreateNode("attribute");
    pNode->SetParam("name", pAttrName->GetValue());
    AppendBody(pNode, pArgList);
    return pNode;
  }

 protected:
  void CreateConnectFromList(CXMLNode *pList, string sDirection,
                             CXMLNode *data_refs, CXMLNode *task_ref) {
    unsigned int i;
    for (i = 0; i < data_refs->GetChildNum(); i++) {
      CXMLNode *pNodeItem = CreateNode("connect");
      pNodeItem->AppendChild(data_refs->GetChildByIndex(i));
      pNodeItem->AppendChild(task_ref);
      CXMLNode *pDirection = pNodeItem->CreateAppendChild();
      pDirection->SetName("direction");
      pDirection->SetValue(sDirection);

      pList->AppendChild(pNodeItem);
    }
  }
};

class CTLDMParser : public CXMLParser {
 public:
  CTLDMTree *get_xml_tree() { return &m_XMLTree; }

 protected:
  CTLDMTree m_XMLTree;
};

#endif  // TRUNK_LEGACY_TOOLS_PARSERS_SRC_TLDM_TLDM_TREE_H_
