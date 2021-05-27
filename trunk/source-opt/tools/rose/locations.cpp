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


#include "locations.h"
#include <cstring>
#include <iostream>
#include <map>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>

#include "codegen.h"
#include "common.h"
#include "mars_opt.h"
using std::map;
using std::set;
using std::stack;
using std::string;
using std::vector;
bool g_muteHISError = false;

extern char g_passName[MAX_PASS_LENGTH];
/******************************************************************************/
/* Utility functions **********************************************************/
/******************************************************************************/

bool isStartWith(string str, string substr) {
  if (str.size() < substr.size()) {
    return false;
  }
  return str.compare(0, substr.size(), substr) == 0;
}

/******************************************************************************/
/* isSkipped ******************************************************************/
/******************************************************************************/

string getSimpleFilename(string filename) {
  string sFilename = filename;
  int n = sFilename.rfind("/");
  if (n != -1) {
    sFilename = sFilename.substr(n + 1);
  }
  //  LOC_INFO("included file: " + sFilename);

  return sFilename;
}

bool isCompilerGenerated(CSageCodeGen *ast, SgNode *node) {
  if (node == nullptr || node->get_file_info() == nullptr) {
    return false;
  }
#if 0
  if (node == nullptr || node->get_file_info() == nullptr) {
    if (ast->IsCompilerGenerated(node)) {
      LOC_ERROR("some node is compiler generated but has no file info.");
    }
  }
  string filename = ast->GetFileInfo_filename(node);
  if (ast->IsCompilerGenerated(node) ^ !filename.compare("compilerGenerated")) {
    LOC_ERROR("IsCompilerGenerated and file compare has different result.");
  }
#endif
  return ast->IsCompilerGenerated(node);
}

bool isSkippedIncludeFile(string filename) {
  vector<string> skippedLibs = {"cmost.h", 
                                "rose_edg_required_macros_and_functions.h"};

  string sFilename = getSimpleFilename(filename);

  for (auto &lib : skippedLibs) {
    if (sFilename == lib) {
      return true;
    }
  }
  return false;
}

bool isSkipped(CSageCodeGen *ast, SgNode *node) {
  static std::map<SgNode *, bool> skippedNode;
  if (skippedNode.find(node) != skippedNode.end()) {
    return skippedNode[node];
  }

  if (node == nullptr || node->get_file_info() == nullptr) {
    return false;
  }

  string filename = ast->GetFileInfo_filename(node, 0);

  bool ret;
  if (isSkippedIncludeFile(filename)) {
    ret = true;
  } else if ((g_passName != string("none")) &&
             (g_passName != string("lower_separate")) &&
             (g_passName != string("gen_hierarchy")) &&
             (g_passName != string("program_analysis")) &&
             ast->isWithInHeaderFile(node)) {
    //  FIXME in gen_token_id and gen_hierarchy pass, we both have module to get
    //  code hierarchy in gen_token_id get hierarchy for lc code, in this case
    //  we do not need to care about header file in gen_hierarchy get hierarchy
    //  for user src code, in this case we need to care about header file,
    //  because some of the code may come from header file so if we want
    //  generate hierarchy for merlin generated code, we should consider use
    //  module in gen_token_id
    ret = true;
  } else if (ast->IsFromInputFile(node) != 0) {
    ret = false;
  } else if (ast->IsTransformation(node) || ast->IsNULLFile(node)) {
    ret = isSkipped(ast, static_cast<SgNode *>(ast->GetParent(node)));
  } else if (isCompilerGenerated(ast, node)) {
    if ((node->get_parent()->class_name() == "SgGlobal") &&
        (node->class_name() == "SgVariableDeclaration") &&
        (ast->GetVariableName(node) == "__PRETTY_FUNCTION__")) {
      ret = true;
    } else if ((node->get_parent()->class_name() == "SgGlobal") &&
               ((node->class_name() == "SgTemplateInstantiationFunctionDecl") ||
                (node->class_name() == "SgTemplateInstantiationDecl") ||
                (node->class_name() ==
                 "SgTemplateInstantiationMemberFunctionDecl"))) {
      ret = (g_passName != string("none")) &&
            (g_passName != string("lower_separate")) &&
            (g_passName != string("gen_hierarchy")) &&
            (g_passName != string("program_analysis"));
    } else if (ast->IsPragmaDecl(node) != 0) {
      //  FIXME: in the original template functions, pragma is ignored so in
      //  order to keep consistent we also ignore pragma in function
      //  instantiation
      ret = true;
    } else {
      ret = isSkipped(ast, static_cast<SgNode *>(ast->GetParent(node)));
    }
  } else {
    ret = true;
  }

  skippedNode[node] = ret;
  return ret;
}

/******************************************************************************/
/* getChildrenNumWithoutInclude ***********************************************/
/******************************************************************************/

int getChildrenNumWithoutInclude(CSageCodeGen *ast, SgNode *node) {
  if (node == nullptr) {
    return 0;
  }

  int num = 0;
  auto children = node->get_traversalSuccessorContainer();
  for (auto child : children) {
    if (!isSkipped(ast, child)) {
      ++num;
    }
  }
  return num;
}

/******************************************************************************/
/* getNodeByTopoIndex *********************************************************/
/******************************************************************************/

SgNode *getChild(vector<SgNode *> children, size_t index) {
  if (index >= children.size()) {
    LOC_ERROR("index out of bound when find the child.");
    return nullptr;
  }
  return children[index];
}

SgNode *getFileFromIndex(CSageCodeGen *ast, SgNode *parent, size_t index) {
  LOC_INFO("try to get file from index.");
  if (parent->class_name() != "SgFileList") {
    LOC_ERROR("must get file from filelist");
    return nullptr;
  }

  auto children = orderedByFile(ast, parent->get_traversalSuccessorContainer());
  return getChild(children, index);
}

SgNode *getNodeByTopoIndex(CSageCodeGen *ast, SgNode *node, size_t index) {
  if (node == nullptr) {
    LOC_ERROR("node is nullptr, cannot get its child.");
    return nullptr;
  }
  auto children = node->get_traversalSuccessorContainer();
  LOC_INFO("type: " + node->class_name());
  if (node->class_name() == "SgGlobal") {
    size_t i = 0;
    size_t real_i = 0;
    for (; i < children.size() && real_i <= index; ++i) {
      if (isSkipped(ast, children[i])) {
        continue;
      }
      ++real_i;
    }
    if (real_i > index) {
      return getChild(children, i - 1);
    }
    LOC_ERROR("index cannot found, max index(" + std::to_string(real_i - 1) +
              " is smaller than required index: " + std::to_string(index));
    return nullptr;
  }
  if (node->class_name() == "SgFileList") {
    return getFileFromIndex(ast, node, index);
  }
  return getChild(children, index);
}

/******************************************************************************/
/* getTopoIndex ***************************************************************/
/******************************************************************************/

bool isKernelFile(string fileName) {
  return isStartWith(fileName, "__merlinkernel");
}

bool isWrapperFile(string fileName) {
  return isStartWith(fileName, "__merlinwrapper");
}

vector<SgNode *> orderedByFile(CSageCodeGen *ast,
                               const vector<SgNode *> &children) {
  vector<SgNode *> kernels;
  vector<SgNode *> wrappers;
  vector<SgNode *> others;
  string kernelPrefix("__merlinkernel");
  string wrapperPrefix("__merlinwrapper");

  for (auto child : children) {
    if (child == nullptr || child->get_file_info() == nullptr) {
      continue;
    }
    string childFileName = ast->GetFileInfo_filename(child, 1);
    if (isKernelFile(childFileName)) {
      kernels.push_back(child);
      LOC_INFO("add kernel file: " + childFileName);
    } else if (isWrapperFile(childFileName)) {
      wrappers.push_back(child);
      LOC_INFO("add wrapper file: " + childFileName);
    } else {
      others.push_back(child);
      LOC_INFO("add other file: " + childFileName);
    }
  }

  sort(kernels.begin(), kernels.end(), [](SgNode *lhs, SgNode *rhs) {
    string file1 = lhs->get_file_info()->get_filename();
    string file2 = rhs->get_file_info()->get_filename();
    return file1 < file2;
  });
  sort(wrappers.begin(), wrappers.end(), [](SgNode *lhs, SgNode *rhs) {
    string file1 = lhs->get_file_info()->get_filename();
    string file2 = rhs->get_file_info()->get_filename();
    return file1 < file2;
  });
  sort(others.begin(), others.end(), [](SgNode *lhs, SgNode *rhs) {
    string file1 = lhs->get_file_info()->get_filename();
    string file2 = rhs->get_file_info()->get_filename();
    return file1 < file2;
  });
  kernels.insert(kernels.end(), wrappers.begin(), wrappers.end());
  kernels.insert(kernels.end(), others.begin(), others.end());

#if 0
  for (auto node : kernels) {
    LOC_INFO("filename: " << node->get_file_info()->get_filename());
  }
#endif
  return kernels;
}

int getFileIndex(CSageCodeGen *ast, SgNode *child) {
  if (ast->is_floating_node(child) != 0) {
    LOC_ERROR("get topology index for floating child.");
    return -1;
  }
  if (child->class_name() == "SgProject") {
    LOC_ERROR("get topology index for project.");
    return -1;
  }

  LOC_INFO("try to calculate file index.");

  auto parent = child->get_parent();
  auto children = orderedByFile(ast, parent->get_traversalSuccessorContainer());
  string filename = child->get_file_info()->get_filename();

  for (size_t i = 0; i < children.size(); ++i) {
    if (filename == children[i]->get_file_info()->get_filename()) {
      LOC_INFO("result index: " + std::to_string(i));
      return i;
    }
  }

  LOC_ERROR("result index: -1");
  return -1;
}

std::map<SgNode *, int> g_IndexCache;
int getTopoIndex(CSageCodeGen *ast, SgNode *child) {
  if (g_IndexCache.find(child) != g_IndexCache.end()) {
    return g_IndexCache[child];
  }
  if (child == nullptr || child->get_parent() == nullptr) {
    LOC_ERROR("get topology index for null pointer or its parent is null.");
    g_IndexCache[child] = -1;
    return -1;
  }
  if (child->class_name() == "SgProject") {
    LOC_ERROR("get topology index for project.");
    g_IndexCache[child] = -1;
    return -1;
  }

  DEFINE_START_END;
  STEMP(start);
  bool isSkip = isSkipped(ast, child);
  ACCTM(getTopoIndex_isSkipped, start, end);
  if (isSkip) {
    LOC_ERROR("this skipped node has no index: " + ast->_p(child, 80));
    g_IndexCache[child] = -1;
    return -1;
  }
  if (child->class_name() == "SgSourceFile") {
    STEMP(start);
    int index = getFileIndex(ast, child);
    ACCTM(GetFileIndex, start, end);
    LOC_INFO("file index: " + std::to_string(index));
    g_IndexCache[child] = index;
    return index;
  }
  auto parent = child->get_parent();
  STEMP(start);
  auto children = parent->get_traversalSuccessorContainer();
  size_t i = 0;
  size_t index = 0;
  for (; i < children.size() && child != children[i]; ++i) {
    if (isSkipped(ast, children[i])) {
      continue;
    }
    g_IndexCache[children[i]] = index;
    ++index;
  }
  ACCTM(GetGlobalIndex, start, end);
  if (i < children.size()) {
    LOC_INFO("global index: " + std::to_string(index));
    g_IndexCache[child] = index;
    return index;
  }
  LOC_ERROR("child should be found here.");
  g_IndexCache[child] = -1;
  return -1;
}

/******************************************************************************/
/* AST Location <-- Topo Location *********************************************/
/******************************************************************************/

SgNode *getRoot(CSageCodeGen *ast) {
  return static_cast<SgNode *>(ast->GetProject());
}

SgNode *getSgNode(CSageCodeGen *ast, const vector<int> &topoPosition,
                  SgNode *root) {
  SgNode *node = root != nullptr ? root : getRoot(ast);
  for (auto it = topoPosition.begin(); it != topoPosition.end(); ++it) {
    node = getNodeByTopoIndex(ast, node, *it);
    if (node == nullptr) {
      return nullptr;
    }
  }
  return node;
}

SgNode *getSgNode(CSageCodeGen *ast, const string &topoString, SgNode *root) {
  TopoLocation topoLocation(topoString);
  return getSgNode(ast, topoLocation.getTopoVector(), root);
}

/******************************************************************************/
/* AST <-- File Location ******************************************************/
/******************************************************************************/

#if KEEP_UNUSED
//  Fixit: rewrite this function
string getTypeStr(int type) {
  switch (type) {
  case V_SgVariableDeclaration:
    return "SgVariableDeclaration";
  case V_SgVariableDefinition:
    return "SgVariableDefinition";
  case V_SgFunctionDeclaration:
    return "SgFunctionDeclaration";
  case V_SgFunctionDefinition:
    return "SgFunctionDefinition";
  case V_SgForStatement:
    return "SgForStatement";
  default:
    LOC_ERROR("Cannot get type string from type " + std::to_string(type));
    return nullptr;
  }
}

vector<SgNode *> getAllSgNodes(CSageCodeGen *ast,
                               const FileLocation &fileLocation,
                               const int type) {
  //  input check
  if (fileLocation.getLineNum() < 0) {
    LOC_ERROR("Non-positive line number: " + fileLocation.toString());
  } else if (fileLocation.getLineNum() == 0) {
    LOC_WARNING("The location is in the line 0.");
    //  return nullptr;
  }

  vector<SgNode *> result;

  vector<void *> possible_results;
  string typeStr = getTypeStr(type);
  LOC_INFO("Finding type: " + typeStr);
  ast->GetNodesByType(nullptr, "preorder", typeStr, &possible_results);

  for (auto ele : possible_results) {
    SgNode *node = static_cast<SgNode *>(ele);
    if (fileLocation == getFileLocation(ast, node)) {
      LOC_INFO("New FileLocation: " + getFileLocation(ast, node));
      result.push_back(node);
    }
  }

  return result;
}

SgNode *getSgNode(CSageCodeGen *ast, const string &fileLocation,
                  const int type) {
  LOC_INFO("Search ast node for type " + std::to_string(type));
  vector<SgNode *> nodes = getAllSgNodes(ast, FileLocation(fileLocation), type);
  if (nodes.size() > 0) {
    return nodes[0];
  } else {
    LOC_ERROR("Ast node (" + std::to_string(type) +
              ") not found for FileLocation: " + fileLocation);
    return nullptr;
  }
}
#endif

/******************************************************************************/
/* File Location <-- AST ******************************************************/
/******************************************************************************/

string getFileLocation(CSageCodeGen *ast, SgNode *node, int simple) {
  if (node == nullptr || node->get_file_info() == nullptr) {
    return FileLocation("", -1).toString();
  }
  return FileLocation(ast->GetFileInfo_filename(node, simple),
                      ast->GetFileInfo_line(node))
      .toString();
}

/******************************************************************************/
/* File Location <-- Topo Location ********************************************/
/******************************************************************************/

string getFileLocation(CSageCodeGen *ast, const string &topoLocation,
                       int simple) {
  return getFileLocation(ast, getSgNode(ast, topoLocation), simple);
}

/******************************************************************************/
/* Topo Location <-- AST ******************************************************/
/******************************************************************************/
string getTopoLocation(CSageCodeGen *ast, SgNode *node, SgNode *root) {
  if (ast->is_floating_node(node) != 0) {
    LOC_ERROR("calculate topolocation for floating node.");
    if (node != nullptr) {
      LOC_ERROR("node type: " + node->class_name());
    } else {
      LOC_ERROR("node is null.");
    }
    return "";
  }

  if (isSkipped(ast, node)) {
    LOC_ERROR("calculate topolocation for a skipped node.");
    return "";
  }

  TopoLocation topoLocation;
  topoLocation.setType(node->variantT());

  DEFINE_START_END;
  stack<int> pos_stack;
  SgNode *parent;
  while ((parent = node->get_parent()) != root) {
    STEMP(start);
    int index = getTopoIndex(ast, node);
    ACCTM(getTopoLocation_body_1, start, end);
    if (index < 0) {
      LOC_ERROR("parent node: " + parent->class_name());
      LOC_ERROR("child node: " + node->unparseToString() + "(" +
                node->class_name() + ")");
      LOC_ERROR("File name: " << node->get_file_info()->get_filename());
      LOC_ERROR("File Loc: " +
                std::to_string(node->get_file_info()->get_line()));
      LOC_ERROR("negative index found: " + std::to_string(index));
    }
    pos_stack.push(index);
    node = parent;
  }
  if (node->class_name() != "SgProject") {
    LOC_ERROR("root node is not SgProject, root type: " + node->class_name());
  }
  STEMP(start);
  while (!pos_stack.empty()) {
    topoLocation.getTopoVector().push_back(pos_stack.top());
    pos_stack.pop();
  }
  ACCTM(getTopoLocation_body_2, start, end);

  return topoLocation.toString();
}

/******************************************************************************/
/* Topo Location <-- File Location ********************************************/
/******************************************************************************/

#if KEEP_UNUSED
string getTopoLocation(CSageCodeGen *ast, const string &fileLocation,
                       const int type) {
  return getTopoLocation(ast, getSgNode(ast, fileLocation, type));
}
#endif

/******************************************************************************/
/* Utility API ****************************************************************/
/******************************************************************************/

#if KEEP_UNUSED
vector<string> getTopoLocationBytype_compatible(CSageCodeGen *ast,
                                                void *sg_scope,
                                                string str_type) {
  vector<string> topoLocations;
  vector<void *> sg_nodes;
  ast->GetNodesByType_compatible(sg_scope, str_type, &sg_nodes);
  for (auto node : sg_nodes) {
    topoLocations.push_back(getTopoLocation(ast, static_cast<SgNode *>(node)));
  }
  return topoLocations;
}
FileLocation::FileLocation(const string &fileLocation) {
  char *fileLocationCStr = new char[fileLocation.length() + 1];
  snprintf(fileLocationCStr, fileLocation.size() + 1, "%s",
           fileLocation.c_str());
  char *rest = nullptr;
  //  file name
  this->file_name = strtok_r(fileLocationCStr, ":", &rest);
  this->line_num = std::stoi(strtok_r(nullptr, ",", &rest));
  char *col_str = strtok_r(nullptr, ",", &rest);
  if (col_str == nullptr || strlen(col_str) == 0) {
    this->col_num = -1;
  } else {
    this->col_num = std::stoi(col_str);
  }
  delete[] fileLocationCStr;
  fileLocationCStr = nullptr;
}
#endif

bool FileLocation::operator==(const FileLocation &rhs) const {
  return (this->file_name == rhs.file_name) && this->line_num == rhs.line_num &&
         this->col_num == rhs.col_num;
}

string FileLocation::toString() const {
  if (col_num <= 0) {
    return file_name + ":" + std::to_string(line_num);
  }
  return file_name + ":" + std::to_string(line_num) + ", " +
         std::to_string(col_num);
}
TopoLocation::TopoLocation(const string &topoLocation) {
  LOC_INFO("parse topo location:" + topoLocation);
  char *topoLocationCStr = new char[topoLocation.length() + 1];
  snprintf(topoLocationCStr, topoLocation.length() + 1, "%s",
           topoLocation.c_str());
  char *rest = nullptr;
  //  skip the type field
  char *token = strtok_r(topoLocationCStr, "_", &rest);
  if (strlen(token) != 1) {
    LOC_ERROR("type info has more than one bit: " << token);
  }
  switch (token[0]) {
  case 'v':
  case 'V':
    this->type = V_SgVariableDefinition;
    break;
  case 'f':
  case 'F':
    this->type = V_SgFunctionDefinition;
    break;
  case 'l':
  case 'L':
    this->type = V_SgForStatement;
    break;
  case 'x':
  case 'X':
    this->type = 0;
    break;
  default:
    dump_critical_message("Locations", "ERROR",
                          "Wrong topology location string.", -1, 0);
  }

  while ((token = strtok_r(nullptr, "_", &rest)) != nullptr) {
    try {
      this->location.push_back(std::stoi(token));
    } catch (std::exception &e) {
      LOC_ERROR("invalid id: " << topoLocation);
      delete[] topoLocationCStr;
      topoLocationCStr = nullptr;
      return;
    }
  }
  delete[] topoLocationCStr;
  topoLocationCStr = nullptr;
}

string TopoLocation::getTopoString() const {
  std::stringstream ss;
  for (auto val : this->location) {
    ss << "_" << std::to_string(val);
  }
  return ss.str();
}

char TopoLocation::getTypeString() const {
  switch (type) {
  case V_SgVariableDeclaration:
  case V_SgVariableDefinition:
    return 'V';
  case V_SgFunctionDeclaration:
  case V_SgFunctionDefinition:
    return 'F';
  case V_SgForStatement:
  case V_SgWhileStmt:
  case V_SgDoWhileStmt:
    return 'L';
  default:
    return 'X';
  }
}
