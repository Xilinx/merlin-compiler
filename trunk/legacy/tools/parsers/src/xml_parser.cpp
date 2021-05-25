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



#include "xml_parser.h"
#include "file_parser.h"
#include "cmdline_parser.h"

CXMLNode *CXMLNode::CreateAppendChild() {
  CXMLNode *node = m_pXMLTree->CreateNode();
  AppendChild(node);
  return node;
}

vector<CXMLNode *>
CXMLNode::TraverseByCondition(XMLTreeTraverseTestFunc pfTestFunc,
                              const vector<string> &sArgument, int level) {
  vector<CXMLNode *> vecNodes;

  TraverseByCondition(pfTestFunc, sArgument, &vecNodes, level);

  return vecNodes;
}

void CXMLNode::TraverseByCondition(XMLTreeTraverseTestFunc pfTestFunc,
                                   const vector<string> &sArgument,
                                   vector<CXMLNode *> *vecNodes,
                                   int depth = -1) {
  if (pfTestFunc(this, sArgument) != 0) {
    vecNodes->push_back(this);
  }
  if (depth == -1 || depth > 0) {
    int i;
    for (i = 0; i < GetChildNum(); i++) {
      int sub_depth = (depth == -1) ? -1 : depth - 1;
      GetChildByIndex(i)->TraverseByCondition(pfTestFunc, sArgument, vecNodes,
                                              sub_depth);
    }
  }
}

int TestName(CXMLNode *pNode, const vector<string> &sArguments) {
  assert(sArguments.size() == 1);
  return static_cast<int>(pNode->GetName() == sArguments[0]);
}

// level = -1 for all levels
vector<CXMLNode *> CXMLNode::TraverseByName(string sName, int level) {
  vector<string> sArguments;
  sArguments.push_back(sName);

  return TraverseByCondition(TestName, sArguments, level);
}

// level = -1 for all levels
vector<CXMLNode *> CXMLNode::TraverseByName1(string sName, int level) {
  vector<CXMLNode *> vecNodes;

  TraverseByName(sName, &vecNodes, level);

  return vecNodes;
}

// level == -1 for all levels
void CXMLNode::TraverseByName(string sName, vector<CXMLNode *> *vecNodes,
                              int depth) {
  if (GetName() == sName || sName.empty()) {
    vecNodes->push_back(this);
  }
  if (depth == -1 || depth > 0) {
    int i;
    for (i = 0; i < GetChildNum(); i++) {
      int sub_depth = (depth == -1) ? -1 : depth - 1;
      GetChildByIndex(i)->TraverseByName(sName, vecNodes, sub_depth);
    }
  }
}

int CheckXMLNodeName(CXMLNode *pNode, const vector<string> &vecArg) {
  assert(vecArg.size() <= 2);
  int check_name = static_cast<int>(vecArg.size() == 2);
  int ret = static_cast<int>(
      pNode->GetName() == vecArg[0] &&
      ((check_name == 0) || pNode->GetParam("name") == vecArg[1]));
  return ret;
}

CXMLNode *SearchNodeNameUnique(CXMLNode *pScope, string sNode,
                               string sName /* = ""*/) {
  vector<string> vecArg;
  vecArg.push_back(sNode);
  if (!sName.empty()) {
    vecArg.push_back(sName);
  }
  vector<CXMLNode *> vecNodes =
      pScope->TraverseByCondition(CheckXMLNodeName, vecArg);
  assert(vecNodes.size() <= 1);  // avoid redefinition
  return (vecNodes.size()) != 0U ? vecNodes[0] : nullptr;
}

CXMLNode *SearchNodeNameFirst(CXMLNode *pScope, string sNode,
                              string sName /* = ""*/) {
  vector<string> vecArg;
  vecArg.push_back(sNode);
  if (!sName.empty()) {
    vecArg.push_back(sName);
  }
  vector<CXMLNode *> vecNodes =
      pScope->TraverseByCondition(CheckXMLNodeName, vecArg);
  // assert(vecNodes.size()>=1);
  return (vecNodes.size()) != 0U ? vecNodes[0] : nullptr;
}

CXMLNode *CXMLNode::CloneSubTree(CXMLTree *newTree) {
  return CloneSubTree_in(newTree);
}

CXMLNode *CXMLNode::CloneSubTree_in(CXMLTree *newTree, CXMLNode *pParent) {
  // map<string, string>    m_Parameters;
  // CXMLNode *                    m_Parent;
  // vector<CXMLNode *>    m_Children;
  // string                            m_sName;
  // string                            m_sValue;
  // CXMLTree * m_pXMLTree; // set when creating

  CXMLNode *newNode = newTree->CreateNode();
  newNode->m_Parameters = m_Parameters;
  newNode->m_sName = m_sName;
  newNode->m_sValue = m_sValue;
  newNode->m_Parent = pParent;

  int i;
  for (i = 0; i < GetChildNum(); i++) {
    CXMLNode *newChild = GetChildByIndex(i)->CloneSubTree_in(newTree, newNode);
    newNode->AppendChild(newChild);
  }
  return newNode;
}

void CXMLNode::RemoveSubTree() {
  // memory free is managed by m_NodeList in tree;
  if (GetParent() == nullptr) {
    return;
  }

  CXMLNode *pParent = GetParent();

  int i;
  for (i = 0; i < pParent->GetChildNum(); i++) {
    if (pParent->GetChildByIndex(i) == this) {
      break;
    }
  }

  assert(i < pParent->GetChildNum());

  pParent->RemoveChildByIndex(i);
}

void CXMLNode::ReplaceSubTree(CXMLNode *pNewNode) {
  assert(pNewNode->m_pXMLTree == m_pXMLTree);
  // memory free is managed by m_NodeList in tree;
  if (GetParent() == nullptr) {
    return;
  }

  CXMLNode *pParent = GetParent();

  int i;
  for (i = 0; i < pParent->GetChildNum(); i++) {
    if (pParent->GetChildByIndex(i) == this) {
      break;
    }
  }
  assert(i < pParent->GetChildNum());

  pParent->RemoveChildByIndex(i);
  pParent->InsertChild(i, pNewNode);
}

CXMLNode *CXMLParser::parse_from_file(string sFilename) {
  FILE *fp_in;
  if ((fp_in = fopen(sFilename.c_str(), "rb")) == nullptr) {
    printf("[xml_parser] ERROR: Fail to open file %s for read.\n",
           sFilename.c_str());
    fflush(stdout);
    assert(0);
  }

  string sElement;

  char sLine[4096];
  while (fgets(sLine, 4096, fp_in) != nullptr) {
    sElement += string(sLine);
  }

  FormatXMLString(&sElement);
  CXMLNode *pRoot = build_element_from_string(sElement);
  m_XMLTree.setRoot(pRoot);

  fclose(fp_in);
  return pRoot;
}

CXMLNode *CXMLParser::parse_empty(string sName) {
  assert(!sName.empty());
  string sElement = "<" + sName + "></" + sName + ">";

  FormatXMLString(&sElement);
  CXMLNode *pRoot = build_element_from_string(sElement);
  m_XMLTree.setRoot(pRoot);

  return pRoot;
}

// remove spaces after '=' and '<' and '>' and '/', except between quotations
void CXMLParser::FormatXMLString(string *str) {
  size_t size = str->size();
  int quotation = 0;

  // remove spaces after '=' and '<' and '>' and '</'
  for (size_t i = 0; i < size; i++) {
    if ((*str)[i] == '\"') {
      quotation = static_cast<int>(static_cast<int>(quotation) == 0);
    }
    if ((quotation == 0) &&
        ((*str)[i] == '=' || (*str)[i] == '<' || (*str)[i] == '>' ||
         (i > 0 && (*str)[i] == '/' && (*str)[i - 1] == '<'))) {
      size_t j;
      for (j = i + 1; j < str->size(); j++) {
        if ((*str)[j] != ' ' && (*str)[j] != '\t' && (*str)[j] != '\r' &&
            (*str)[j] != '\n') {
          break;
        }
      }
      // remove from i+1 to j-1
      if (j == str->size()) {
        *str = str->substr(0, i + 1);
      } else {
        *str = str->substr(0, i + 1) + str->substr(j);
      }
      size -= j - i - 1;
    }
  }

  size = str->size();
  assert(quotation == 0);

  for (size_t i = size; i > 0; i--) {
    if ((*str)[i - 1] == '\"') {
      quotation = static_cast<int>(static_cast<int>(quotation) == 0);
    }
    if ((quotation == 0) &&
        ((*str)[i - 1] == '=' || (*str)[i - 1] == '<' || (*str)[i - 1] == '>' ||
         (i - 1 > 0 && (*str)[i - 1] == '/' && (*str)[i - 2] == '<'))) {
      size_t j;
      for (j = i - 1; j > 0; j--) {
        if ((*str)[j - 1] != ' ' && (*str)[j - 1] != '\t' &&
            (*str)[j - 1] != '\r' && (*str)[j - 1] != '\n') {
          break;
        }
      }
      // remove from i-2 to j
      if (j < 1) {
        *str = str->substr(i - 1);
      } else {
        *str = str->substr(0, j) + str->substr(i - 1);
        i = j;
      }
    }
  }
}

// whether the name is valid for a component name
int check_xml_name(string sName) {
  // 1. All its characters are letters, numbers and underscores
  // 2. can not start with numbers
  size_t i;
  for (i = 0; i < sName.size(); i++) {
    int isNum = static_cast<int>(sName[i] >= '0' && sName[i] <= '9');
    int isLetter = static_cast<int>((sName[i] >= 'a' && sName[i] <= 'z') ||
                                    (sName[i] >= 'A' && sName[i] <= 'Z'));
    int isUnderScore = static_cast<int>((sName[i] == '_') ||
                                        (sName[i] == '-') || (sName[i] == '.'));
    if (0 == i && (isLetter == 0) && (isUnderScore == 0)) {
      return 0;
    }
    if ((isNum == 0) && (isLetter == 0) && (isUnderScore == 0)) {
      return 0;
    }
  }
  return 1;
}

string GetWholeXMLNode(string sXML, int start) {
  size_t j = start;

  string name = get_sub_delimited_multi(sXML, static_cast<int>(j + 1), " >");
  j += name.size() + 1;

  // printf("Node = %s\n", sXML.substr(0, 20).c_str());

  if (sXML[j - 1] == '/') {
    return sXML.substr(start, j - start + 1);
  }

  if (check_xml_name(name) == 0) {
    printf("[xml_parser] ERROR: Invalid element name(%c): %s \n", sXML[j - 2],
           name.c_str());
    printf("             @ %100s\n", sXML.c_str() + j - 1 - name.size());
    assert(0);
  }

  string s_simple = get_sub_delimited_sub(sXML, j, string("/>"));
  string s_simple_c = get_sub_delimited_sub(sXML, j, string("<"));

  if (s_simple.size() < s_simple_c.size()) {
    return sXML.substr(start, j - start + s_simple.size() + 2);
  }

  int num_of_begins = 1;
  int num_of_ends = 0;
  string sComponent;
  while (num_of_ends != num_of_begins) {
    string s_begin = get_sub_delimited_sub(sXML, j, string("<") + name);
    string s_end = get_sub_delimited_sub(sXML, j, string("</") + name);

    string s_simple = get_sub_delimited_sub(
        sXML, static_cast<int>(j + s_end.size() + 1), string("/>"));
    string s_simple_c = get_sub_delimited_sub(
        sXML, static_cast<int>(j + s_end.size() + 1), string("<"));
    if (s_simple.size() < s_simple_c.size()) {
      j += s_simple.size() + 2;
    } else if (s_begin.size() > s_end.size()) {
      num_of_ends++;
      j += s_end.size() + 1;
    } else if (s_begin.size() < s_end.size()) {
      num_of_begins++;
      j += s_begin.size() + 1;
    } else {  // (s_begin.size() == s_end.size())
      // assert(0); handled by the following check

      j += s_begin.size() + 1;
      if (j >= sXML.size()) {
        printf("[xml_parser] ERROR: Mismatch for </%s>\n", name.c_str());
        char str[1024];
        strncpy(str, sXML.c_str() + start, 100);
        str[100] = '.';
        str[101] = '.';
        str[102] = '.';
        str[103] = 0;
        printf("             @ %s\n", str);
        assert(0);
      }
    }

    if (j >= sXML.size()) {
      printf("[xml_parser] ERROR: Mismatch for </%s>\n", name.c_str());
      char str[1024];
      strncpy(str, sXML.c_str() + start, 100);
      str[100] = '.';
      str[101] = '.';
      str[102] = '.';
      str[103] = 0;
      printf("             @ %s\n", str);
      assert(0);
    }
  }

  string tailing_str = get_sub_delimited(sXML, j, '>');
  j += tailing_str.size();

  if (j >= sXML.size()) {
    printf("[xml_parser] ERROR: Mismatch for </%s>\n", name.c_str());
    char str[1024];
    strncpy(str, sXML.c_str() + start, 100);
    str[100] = '.';
    str[101] = '.';
    str[102] = '.';
    str[103] = 0;
    printf("             @ %s\n", str);
    assert(0);
  }

  return sXML.substr(start, j - start + 1);
}

CXMLNode *CXMLParser::build_element_from_string(string sElement) {
  // printf("build = %s\n", sElement.substr(0, 20).c_str());

  if (sElement.empty()) {
    return nullptr;
  }

  size_t i = 0;
  size_t j;
  // parse name
  string header = get_sub_delimited(sElement, 0, '<');
  // assert(isSpace(header)); // FIXME: TODO: if < is not found
  i += header.size() + 1;

  string name = get_sub_delimited_multi(sElement, i, " >");

  i += name.size() + 1;

  int n_name_size = name.size();
  if (name[n_name_size - 1] == '/') {
    name = name.substr(0, n_name_size - 1);
  }
  CXMLNode *pNode = m_XMLTree.CreateNode(FormatSpace(name));

  // parse parameters
  if (sElement[i - 1] != '>') {
    string paramlist = get_sub_delimited(sElement, i, '>');

    // paramlist.push_back('>');
    j = 0;
    while (true) {
      string param = get_sub_delimited(paramlist, j, '=');
      j += param.size() + 1;
      if (j > paramlist.size()) {
        break;
      }
      string value = get_sub_delimited_multi(paramlist, j, " />");
      j += value.size() + 1;
      pNode->SetParam(FormatSpace(param), FormatSpace(value));
      if (j > paramlist.size()) {
        break;
      }
    }
    i += paramlist.size() + 1;
  }

  if (sElement[i - 1] != '>') {
    printf("\nsElement = %s, i = %d\n", sElement.c_str(), static_cast<int>(i));
  }
  assert(sElement[i - 1] == '>');

  if (i >= 2 && sElement[i - 2] == '/') {  // "<.....    />"
    // Do nothing but return
  } else {
    // child / value

    // remove the last </name>
    size_t j_r = j = i;
    while (j < sElement.size()) {
      j_r = j;
      string tmp = get_sub_delimited_sub(sElement, j, string("</") + name);
      j += tmp.size() + 1;
    }
    string childlist = sElement.substr(i, j_r - i - 1);

    j = 0;
    while (true) {
      // int j0 = j;
      string header = get_sub_delimited(childlist, j, '<');
      j += header.size() + 1;
      int j0 = static_cast<int>(j - 1);
      if (j > childlist.size()) {
        // value branch or end of child branch
        if (pNode->isLeaf() != 0) {
          pNode->SetValue(FormatSpace(childlist));
        }
        break;
      }
      if (string("!--") == childlist.substr(j, 3)) {
        // comments
        string sComment = get_sub_delimited_sub(childlist, j, "-->");
        j += sComment.size() + 3;
        sComment = childlist.substr(j0, j - j0);
        CXMLNode *pChild = pNode->CreateAppendChild();
        pChild->SetName(string(""));
        pChild->SetValue(FormatSpace(sComment));
      } else {
        string child_element = GetWholeXMLNode(childlist, j0);
        j = j0 + child_element.size();
        CXMLNode *pChild = build_element_from_string(child_element);
        pNode->AppendChild(pChild);
      }
    }
  }

  return pNode;
}

string CXMLParser::dump_element_to_string(CXMLNode *pNode) {
  int i;

  string sElement;
  // name and parameters
  for (i = 0; i < pNode->GetLevel(); i++) {
    sElement += string("\t");
  }

  if (pNode->GetName() == string("")) {  // comment
    sElement += pNode->GetValue();
  } else if (pNode->isLeaf() != 0) {  // leaf
    sElement += string("<") + pNode->GetName() + pNode->DumpParams() +
                string(">") + pNode->GetValue() + string("</") +
                pNode->GetName() + string(">");
  } else {
    sElement +=
        string("<") + pNode->GetName() + pNode->DumpParams() + string(">\n");
    for (i = 0; i < pNode->GetChildNum(); i++) {
      sElement += dump_element_to_string(pNode->GetChildByIndex(i));
    }
    for (i = 0; i < pNode->GetLevel(); i++) {
      sElement += string("\t");
    }
    sElement += string("</") + pNode->GetName() + string(">");
  }

  sElement += string("\n");

  return sElement;
}

void CXMLParser::write_into_file(string sFilename) {
  string sElement;
  sElement = dump_element_to_string(m_XMLTree.getRoot());
  write_string_into_file(sElement, sFilename);
}

void MergeNodeWithPriority(CXMLNode *pOrgMerged, CXMLNode *pLowPriority) {
  int i;
  int j;
  for (j = 0; j < pLowPriority->GetChildNum(); j++) {
    CXMLNode *pChildLow = pLowPriority->GetChildByIndex(j);
    for (i = 0; i < pOrgMerged->GetChildNum(); i++) {
      CXMLNode *pChildOrg = pOrgMerged->GetChildByIndex(i);
      if (pChildOrg->GetName() == pChildLow->GetName() &&
          pChildOrg->GetParam("name") == pChildLow->GetParam("name")) {
        break;
      }
    }
    if (i == pOrgMerged->GetChildNum()) {
      CXMLNode *pNewChild = pChildLow->CloneSubTree(pOrgMerged->GetXMLTree());
      pOrgMerged->AppendChild(pNewChild);
    }
  }
}

// The interface to read or set the configurations in CMOST configuration format
// printf("Usage: set_cfg [-i get|set] cfg_xml hier1 hier2 ... value\n");
// printf("    Set/Get the configuration xxx/hier1/xxx/hier2/.../hierx into
// value.\n"); printf("    each hier can be \"name\" or
// \"name:param:param_value\"\n");
string set_xml_cfg(const CInputOptions &options) {
  int i;

  string sXMLFile = options.get_option("", 1);

  vector<string> vecFields;
  for (i = 2; i < options.get_option_num(""); i++) {
    vecFields.push_back(options.get_option("", i));
  }

  string sOut;

  // configurations
  CXMLParser xml_parser;
  CXMLNode *pRoot = xml_parser.parse_from_file(sXMLFile);

  CXMLNode *pNode = pRoot;
  size_t matched_depth = 0;
  while (true) {
    if (matched_depth >= vecFields.size()) {
      break;
    }
    size_t j = 0;
    string sName = get_sub_delimited(vecFields[matched_depth], j, ':');
    j += sName.size() + 1;
    string sParam = get_sub_delimited(vecFields[matched_depth], j, ':');
    j += sParam.size() + 1;
    string sParamValue = get_sub_delimited(vecFields[matched_depth], j, ':');
    j += sParamValue.size() + 1;

    sParamValue = RemoveQuote(sParamValue);

    vector<CXMLNode *> ConfigNodes = pNode->TraverseByName(sName);
    if (ConfigNodes.empty()) {
      break;
    }

    for (j = 0; j < ConfigNodes.size(); j++) {
      pNode = ConfigNodes[j];  // only care the first match in sub branches
      if (sParam.empty() || pNode->GetParam(sParam) == sParamValue) {
        break;
      }
    }
    if (j == ConfigNodes.size()) {
      break;
    }

    if (matched_depth == vecFields.size() - 1 &&
        options.get_option("-i", 0) == string("get")) {
      sOut = pNode->GetValue();
      break;
    }
    if (matched_depth == vecFields.size() - 2 &&
        options.get_option("-i", 0) == string("set")) {
      pNode->SetValue(vecFields[matched_depth + 1]);
      break;
    }
    if (matched_depth > vecFields.size() - 1 &&
        options.get_option("-i", 0) == string("get")) {
      break;
    } else if (matched_depth > vecFields.size() - 2 &&
               options.get_option("-i", 0) == string("set")) {
      break;
    }
    matched_depth++;
  }
  if (options.get_option("-i", 0) == string("set")) {
    xml_parser.write_into_file(sXMLFile);
  }

  return sOut;
}

void set_xml_cfg_simple(string xml_file, string node, string _value) {
  CInputOptions inputs;
  inputs.set_flag("", 10, 4);
  inputs.set_flag("-i", 1, 0);
  inputs.add_option("-i", "set");
  inputs.add_option("", "set_cfg");
  inputs.add_option("", xml_file);
  inputs.add_option("", node);
  inputs.add_option("", _value);
  set_xml_cfg(inputs);
}
string get_xml_cfg_simple(string xml_file, string node) {
  CInputOptions inputs;
  inputs.set_flag("", 10, 4);
  inputs.set_flag("-i", 1, 0);
  inputs.add_option("-i", "get");
  inputs.add_option("", "set_cfg");
  inputs.add_option("", xml_file);
  inputs.add_option("", node);
  return set_xml_cfg(inputs);
}

// return empty when not available
void get_xml_channel_offset(CXMLNode *pRoot, string sTaskName, string sPortName,
                            vector<int> *vecLBOffset,
                            vector<int> *vecUBOffset) {
  size_t i;
  size_t k;
  CXMLNode *pTaskDef = SearchNodeNameUnique(pRoot, "task_def", sTaskName);
  if (pTaskDef == nullptr) {
    return;
  }

  vector<CXMLNode *> vecLDU = pTaskDef->TraverseByName("attribute");
  int found = 0;
  for (k = 0; k < vecLDU.size(); k++) {
    if ("ldu_channel" != vecLDU[k]->GetParam("name") &&
        "gdu_info" != vecLDU[k]->GetParam("name")) {
      continue;
    }

    CXMLNode *pChannelAttr = vecLDU[k]->GetChildByIndex(0);
    string sArrayName = pChannelAttr->GetChildByName("port_name")->GetValue();
    string channel_offset =
        pChannelAttr->GetChildByName("channel_offset")->GetValue();
    if (sPortName != sArrayName) {
      continue;
    }

    assert(found == 0);  // if this assert fails, there is error in data reuse
                         // (node replication)
    found = 1;

    vector<string> vecOffset;
    str_split(channel_offset, ',', &vecOffset);

    for (i = 0; i < vecOffset.size(); i++) {
      size_t j = 0;
      string sLB = get_sub_delimited_sub(vecOffset[i], j, "..");
      j += sLB.size() + 2;
      string sUB = get_sub_delimited_sub(vecOffset[i], j, "..");
      j += sUB.size() + 2;
      vecLBOffset->push_back(my_atoi(sLB));
      vecUBOffset->push_back(my_atoi(sUB));
    }
  }
}
