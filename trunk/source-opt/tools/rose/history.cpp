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


#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include "common.h"

#include "codegen.h"
#include "id_update.h"
#include "locations.h"
#include "mars_opt.h"
#include "report.h"

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"

using boost::filesystem::path;
using rapidjson::Document;
using rapidjson::IStreamWrapper;
using rapidjson::kArrayType;
using rapidjson::OStreamWrapper;
using rapidjson::PrettyWriter;
using rapidjson::SizeType;
using rapidjson::StringBuffer;
using rapidjson::Value;
using std::stack;
using std::unordered_map;
#if 0  //  control history log
#define HIS_INFO(x)                                                            \
  cout << "[HIS INFO][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define HIS_INFO(x)
#endif

#if 0  //  control history log
#define HIS_ALGO(x)                                                            \
  cout << "[HIS ALGO][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define HIS_ALGO(x)
#endif

#if 0  //  control history test
#define HIS_TEST(x)                                                            \
  cout << "[HIS TEST][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#define HIS_RIGHT(x)                                                           \
  cout << "[HIS CHECK][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#define HIS_WRONG(x)                                                           \
  cout << "[HIS WRONG][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define HIS_TEST(x)
#define HIS_RIGHT(x)
#define HIS_WRONG(x)
#endif

#define HIS_CHECK_TMP 0
/******************************************************************************/
/* Utility functions **********************************************************/
/******************************************************************************/

string printNodeInfo(const CSageCodeGen *ast, void *node, int len) {
  if (node == nullptr ||
      (static_cast<SgNode *>(node))->get_file_info() == nullptr ||
      ((static_cast<SgNode *>(node))->class_name() == "SgFileList") ||
      ((static_cast<SgNode *>(node))->class_name() == "SgSourceFile") ||
      ((static_cast<SgNode *>(node))->class_name() == "SgGlobal")) {
    return "top level nodes, do not unparse.";
  }

  return ast->is_floating_node(node) != 0 ? "a floating node"
                                          : ast->_p(node, len);
}

map<SgNode *, string> orgIdMap;

void insertDebugInfo(CSageCodeGen *ast, SgNode *node, const string &debugInfo) {
  map<string, string> message;
  message["DebugInfo"] = debugInfo;
  insertMessage(ast, node, message);
}

//  insert empty history for parent nodes
//  the API calucates org id requires all the parent nodes has history
void insertParent(CSageCodeGen *ast, SgNode *node) {
  auto *histories = ast->getHistories();
  while ((node = node->get_parent()) != nullptr) {
    if (histories->find(node) == histories->end()) {
      HistoryMarker parentMarker;
      HIS_ALGO("mark/insert parent: " << node
                                      << " (" + node->class_name() + ")");
      histories->insert(pair<SgNode *, HistoryMarker>(node, parentMarker));
      insertDebugInfo(ast, node, printNodeInfo(ast, node, 80));
    }
  }
}

/******************************************************************************/
/* Helper functions ***********************************************************/
/******************************************************************************/

bool isEmpty(SgNode *node) { return node == nullptr; }

bool isEmpty(string id) { return id.empty(); }

string getEmpty(const string & /*unused*/) { return ""; }

SgNode *getEmpty(const SgNode * /*unused*/) { return nullptr; }

bool isEqual(const string &id1, const string &id2) { return id1 == id2; }

bool isEqual(SgNode *node1, SgNode *node2) { return node1 == node2; }

template <typename T>
void printTopoOrder(const CSageCodeGen *ast, vector<T> topoOrder) {
#if 0
  cout << "[HIS INFO]topology order: ";
  for (auto ele : topoOrder) {
    if (isEmpty(ele)) {
      cout << "empty, ";
    } else {
      cout << getIdentifier(ast, ele) + ", ";
    }
  }
  cout << endl;
#endif
}

bool isRoot(SgNode *node) {
  if (isEmpty(node)) {
    HIS_ERROR("it is a nullptr pointer, not a root.");
    return true;
  }
  return node->class_name() == "SgProject";
}

bool isRoot(const string &id) {
  if (isEmpty(id)) {
    HIS_ERROR("it is an empty string, not a root.");
    return false;
  }
#if 0
  if (id.compare(0, 2, "X_")) {
    return false;
  }
  TopoLocation topoLocation(id);
  return topoLocation.getTopoVector().size() == 0;
#else
  return id == "X";
#endif
}

string getIdentifier(CSageCodeGen *ast, SgNode *node) {
  if (ast->is_floating_node(node) != 0) {
    return "";
  }
  return getTopoLocation(ast, node);
}

string getIdentifier(const CSageCodeGen *ast, string id) { return id; }

string getParent(const string &id) {
  TopoLocation topo(id);
  if (topo.getTopoVector().empty()) {
    return "";
  }

  topo.getTopoVector().pop_back();
  return topo.toString();
}

SgNode *getParent(SgNode *node) { return node->get_parent(); }

template <typename T>
void tryInsertRoot(CSageCodeGen *ast, map<T, HistoryMarker> *histories, T ele) {
  if (isRoot(ele)) {
    auto markerIt = histories->find(ele);
    if (markerIt == histories->end()) {
      HIS_ALGO("insert root with orginal id.");
      HistoryMarker marker;
      marker.setOrgIdentifier(getIdentifier(ast, ele));
      HIS_ALGO("mark/insert root: " + getIdentifier(ast, ele));
      histories->insert(pair<T, HistoryMarker>(ele, marker));
    } else {
      HIS_ALGO("update original id of root.");
      markerIt->second.setOrgIdentifier(getIdentifier(ast, ele));
    }
  } else {
    HIS_ERROR("try to insert a node that is not a root.");
    return;
  }
}

/******************************************************************************/
/* Reference induction ********************************************************/
/******************************************************************************/

template <typename T>
void getCalculatedRoot(CSageCodeGen *ast, map<T, HistoryMarker> *histories,
                       T key, stack<T> *path) {
  if (!path->empty()) {
    HIS_ERROR("not an empty path in the beginning.");
  }

  if (isEmpty(key)) {
    HIS_ERROR("the key is empty.");
    return;
  }

  HIS_ALGO("start from node: " + getIdentifier(ast, key));
  while (!isRoot(key)) {
    if (histories->find(key) != histories->end() &&
        !histories->find(key)->second.getOrgIdentifier().empty()) {
      HIS_ALGO("find a calculated parent found with org id: " +
               histories->find(key)->second.getOrgIdentifier());
      break;
    }
    path->push(key);
    key = getParent(key);
    HIS_ALGO("try: " + getIdentifier(ast, key));
  }
  HIS_ALGO("Path depth: " + std::to_string(path->size()));

  if (isRoot(key)) {
    tryInsertRoot(ast, histories, key);
  }
}

vector<string> getInitTopoOrder(const CSageCodeGen *ast, const string &pid,
                                int size) {
  if (size <= 0) {
    HIS_ERROR("children " + pid +
              " size is not positive: " + std::to_string(size));
    return {};
  }
  vector<string> topoOrder;
  for (int i = 0; i < size; ++i) {
    topoOrder.push_back(pid + "_" + std::to_string(i));
  }
  return topoOrder;
}

vector<SgNode *> getInitTopoOrder(CSageCodeGen *ast, SgNode *parent, int size) {
  if (parent->class_name() == "SgFileList") {
    return orderedByFile(ast, parent->get_traversalSuccessorContainer());
  }
  vector<SgNode *> result;
  for (auto child : parent->get_traversalSuccessorContainer()) {
    if (!isSkipped(ast, child)) {
      result.push_back(child);
    }
  }
  return result;
}

string getJsonString(const map<string, string> &msg);

template <typename T>
string calculateOrgIdentifierFromParent(CSageCodeGen *ast,
                                        map<T, HistoryMarker> *histories, T key,
                                        bool isCache = true) {
  HIS_ALGO("calculate " + getIdentifier(ast, key) + " from its parent.");

  if (isEmpty(key)) {
    HIS_ERROR("empty key.");
    return "";
  }

  if (isRoot(key)) {
    HIS_ERROR("Calculating SgProject: should not happen.");
    return "";
  }

  //  calculate its parent
  T parent = getParent(key);

  if (isEmpty(parent)) {
    HIS_ERROR("empty parent.");
    return "";
  }
  HIS_ALGO("its parent is " + getIdentifier(ast, parent));

  auto parentMarkerIt = histories->find(parent);
  if (parentMarkerIt == histories->end()) {
    HIS_ERROR("Parent should exist in history map.");
    HIS_ERROR("map size: " + std::to_string(histories->size()));
    HIS_INFO("Node ID: " + getIdentifier(ast, key));
    HIS_INFO("parent ID: " + getIdentifier(ast, parent));
    return "";
  }
  auto &parentMarker = parentMarkerIt->second;

  vector<T> topoOrder =
      getInitTopoOrder(ast, parent, parentMarker.getChildrenNum());
  vector<HistoryEntry> &historyEntries = parentMarker.getHistories();

  //  calculate original list
  HIS_ALGO("its parent has " + std::to_string(historyEntries.size()) +
           " history entries.");
  for (int i = historyEntries.size() - 1; i > parentMarker.getAppliedIndex();
       --i) {
    auto it = historyEntries.begin() + i;

    HIS_ALGO("Deal with action: " + it->toString());

    //  T theKey;
    switch (it->getAction()) {
    case INSERT:
      if (it->getLocation() >= topoOrder.size()) {
        HIS_ERROR("Action location is greater than topology order size.");
        return "";
      }

      //  not nessary
      //  theKey = topoOrder[it->getLocation()];
      //  if (!isEmpty(theKey)) {
      //   auto markerIt = histories->find(theKey);
      //   if (histories->find(topoOrder[it->getLocation()]) ==
      //   histories->end())
      //   {
      //     HIS_ERROR("Inserted key is not in history.")
      //   }
      //  }

      topoOrder.erase(topoOrder.begin() + it->getLocation());
      break;
    case DELETE:
      topoOrder.insert(topoOrder.begin() + it->getLocation(), getEmpty(key));
      break;
    default:
      HIS_ERROR("Should not find COPY or BUILD.");
    }

    //  resulted list
    HIS_ALGO("resulted topology order: ");
    printTopoOrder(ast, topoOrder);
  }

  string parentOrgId = parentMarker.getOrgIdentifier();
  HIS_ALGO("parent original id: " + parentOrgId);
  if (parentOrgId.empty()) {
    HIS_ERROR("Parent original identifier should not be empty.");
    return "";
  }

  HIS_ALGO("calculate based on the topology order: ");
  printTopoOrder(ast, topoOrder);
  //  write result into history
  string theTopoLocation;
  for (size_t i = 0; i < topoOrder.size(); ++i) {
    auto child = topoOrder[i];
    if (isEmpty(child)) {
      continue;
    }

    auto childHistoryMarkerIt = histories->find(child);
    //  HIS_ALGO("replace type " << parentOrgId[0] << " with "
    //                          << getIdentifier(ast, child)[0]);
    //  parentOrgId[0] = getIdentifier(ast, child)[0];

    if (childHistoryMarkerIt == histories->end()) {
      if (isCache) {
        HistoryMarker childMarker;
        map<string, string> msg;
        msg["DebugInfo"] =
            "cached when calculate its brother " + getIdentifier(ast, key);
        string msgString = getJsonString(msg);
        childMarker.getMessages().push_back(msgString);

        childMarker.setOrgIdentifier(parentOrgId + "_" + std::to_string(i));
        histories->insert(pair<T, HistoryMarker>(child, childMarker));
        HIS_ALGO("mark/insert a new entry for " + getIdentifier(ast, child) +
                 " with original id " + parentOrgId + "_" + std::to_string(i));
      }
    } else if (childHistoryMarkerIt->second.getOrgIdentifier().empty()) {
      childHistoryMarkerIt->second.setOrgIdentifier(parentOrgId + "_" +
                                                    std::to_string(i));
      HIS_ALGO("update " + getIdentifier(ast, child) + " with original id " +
               parentOrgId + "_" + std::to_string(i));
    }

    if (isEqual(key, child)) {
      theTopoLocation = parentOrgId + "_" + std::to_string(i);
      HIS_ALGO("update the result: " + theTopoLocation);
    }
  }

  HIS_ALGO("the result is " + theTopoLocation);
  return theTopoLocation;
}

int getTopoIndex(const CSageCodeGen *ast, string topo) {
  TopoLocation topoLocation(topo);
  return topoLocation.getTopoVector().back();
}

bool isUnknownChildrenNum(map<SgNode *, HistoryMarker> *histories,
                          SgNode *key) {
  return false;
}

bool isUnknownChildrenNum(map<string, HistoryMarker> *histories, string key) {
  return histories->find(key) == histories->end() ||
         (*histories)[key].getChildrenNum() < 0;
}

template <typename T>
string getOriginalTopoLocation(CSageCodeGen *ast,
                               map<T, HistoryMarker> *histories, T key) {
  HIS_ALGO("Calculate org ID for topology location: " +
           getIdentifier(ast, key));

  if (isEmpty(key)) {
    HIS_ERROR("try to get org id for an empty key.");
    return "";
  }

  if (histories->empty()) {
    HIS_ALGO("empty history.");
    return getIdentifier(ast, key);
  }

  string orgTopoLocation;

  if (!isRoot(key) &&
      (histories->find(key) == histories->end() ||
       histories->find(key)->second.getOrgIdentifier().empty()) &&
      (histories->find(getParent(key)) == histories->end() ||
       isUnknownChildrenNum(histories, getParent(key)))) {
    HIS_ALGO(getIdentifier(ast, key) + "'s parent is not found in histories");
    HIS_ALGO("calculate from its parent by call getOriginalTopoLocation.");
    int index = getTopoIndex(ast, key);
    orgTopoLocation = getOriginalTopoLocation(ast, histories, getParent(key));
    if (orgTopoLocation.empty()) {
      return "";
    }
    orgTopoLocation += "_" + std::to_string(index);
    //  orgTopoLocation[0] = getIdentifier(ast, key)[0];
    return orgTopoLocation;
  }

  stack<T> path;
  getCalculatedRoot(ast, histories, key, &path);

  if (path.empty()) {
    HIS_ALGO("empty path, return org id directly.");
    auto markerIt = histories->find(key);
    if (markerIt == histories->end()) {
      HIS_ERROR("a root or calculated node that is not in histories.");
      return "";
    }
    HIS_ALGO("directly get from org id field: " +
             markerIt->second.getOrgIdentifier());
    return markerIt->second.getOrgIdentifier();
  }

  if (histories->find(getParent(key)) == histories->end()) {
    HIS_ERROR("calculated parent is empty.");
    return "";
  }
  if ((*histories)[getParent(key)].getPropagate() == 0) {
    return (*histories)[getParent(key)].getOrgIdentifier();
  }

  while (!path.empty()) {
    orgTopoLocation =
        calculateOrgIdentifierFromParent(ast, histories, path.top());
    HIS_INFO("Current ID: " + getIdentifier(ast, path.top()));
    HIS_INFO("Original ID: " + orgTopoLocation);
    path.pop();
  }

  return orgTopoLocation;
}

string getOriginalTopoLocation(map<string, HistoryMarker> *histories,
                               string key, bool ignoreError /*= false*/) {
  if (key.empty()) {
    HIS_ERROR("empty key.");
    return "";
  }
  if (key.size() > 5 && (key.compare(0, 5, "Built") == 0)) {
    return key;
  }

  char type = key[0];
  key[0] = 'X';

  string id = getOriginalTopoLocation(nullptr, histories, key);

  if (id.empty()) {
    HIS_ERROR_COND(!ignoreError, "empty result.");
    return "";
  }
  if (id.size() > 5 && (id.compare(1, 4, "uilt") == 0)) {
    id[0] = 'B';
    return id;
  }

  id[0] = type;
  return id;
}

string getOriginalTopoLocation(CSageCodeGen *ast,
                               map<SgNode *, HistoryMarker> *histories,
                               string topoLocation,
                               bool ignoreError /*= false*/) {
  if (topoLocation.empty()) {
    HIS_ERROR("empty input.");
    return "";
  }

  string id = getOriginalTopoLocation(ast, ast->getHistories(),
                                      getSgNode(ast, topoLocation));
  if (id.empty()) {
    HIS_ERROR_COND(!ignoreError, "empty result.");
    return "";
  }
  if (id.size() > 5 && (id.compare(1, 4, "uilt") == 0)) {
    id[0] = 'B';
    return id;
  }
  id[0] = topoLocation[0];
  return id;
}

map<string, map<string, string>> g_userCodeFileInfo;
vector<map<string, HistoryMarker>> allHistoryList;
vector<string> passNames;
char g_passName[MAX_PASS_LENGTH];

inline bool exists_test(const string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

string getMetadataLocation(const string &passName) {
  if ((passName == "mars_gen_preproc") || (passName == "block") ||
      (passName == "outline")) {
    return "../../metadata/";
  }
  if (passName == "tldm_gen") {
    return "../../../metadata/";
  }
  if (passName == "opencl_gen_pcie") {
    return "../../../../metadata/";
  }
  if (passName == "final_code_gen") {
    //    return "../../metadata/";
    return "./metadata/";
  }
  //    return "../metadata/";
  return "./metadata/";
}

map<string, map<string, string>>
saveUserCodeFileInfo(CSageCodeGen *ast, const string &saveLocation) {
  map<string, map<string, string>> userCodeFileInfo;

  vector<void *> functions;
  ast->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration", &functions,
                      true);
  vector<void *> statements;
  ast->GetNodesByType(nullptr, "preorder", "SgStatement", &statements, true);
  vector<void *> initializedNames;
  ast->GetNodesByType(nullptr, "preorder", "SgInitializedName",
                      &initializedNames, true);
  vector<void *> exprs;
  ast->GetNodesByType(nullptr, "preorder", "SgExpression", &exprs, true);

  vector<void *> savedNodes(functions);
  savedNodes.insert(savedNodes.end(), statements.begin(), statements.end());
  savedNodes.insert(savedNodes.end(), initializedNames.begin(),
                    initializedNames.end());
  savedNodes.insert(savedNodes.end(), exprs.begin(), exprs.end());

  //  start from ast node
  for (size_t i = 0; i < savedNodes.size(); i++) {
    SgNode *node = static_cast<SgNode *>(savedNodes[i]);
    if (isSgValueExp(node)) {
      continue;
    }
    if (node == nullptr || node->get_file_info() == nullptr ||
        ast->IsCompilerGenerated(node)) {
      continue;
    }
    if (isSkipped(ast, node)) {
      continue;
    }
    auto id = getTopoLocation(ast, node);
    auto orgId = getOriginalTopoLocation(ast, ast->getHistories(), node);
    if (orgId.empty()) {
      HIS_WARNING("skip: " + printNodeInfo(ast, node, 80));
      continue;
    }
    orgId = getOriginalTopoLocation(ast, ast->getHistories(), id);
    if (!orgId.empty()) {
      orgId[0] = 'X';
    }
    string fileLocation = getFileLocation(ast, node, 1);
    userCodeFileInfo[orgId]["file_location"] = fileLocation;
    if (ast->IsFunctionDeclaration(node)) {
      userCodeFileInfo[orgId]["name"] = ast->GetFuncName(node);
    } else if (ast->IsFunctionCall(node)) {
      userCodeFileInfo[orgId]["name"] = ast->GetFuncNameByCall(node);
    } else if (ast->IsInitName(node) || ast->IsVariableDecl(node)) {
      userCodeFileInfo[orgId]["name"] = ast->GetVariableName(node);
    } else if (ast->IsClassDecl(node)) {
      userCodeFileInfo[orgId]["name"] = ast->GetClassName(node);
    }
    HIS_ALGO("insert to user code info map(" + orgId + ", " + fileLocation);
  }

  writeInterReport(saveLocation, userCodeFileInfo);
  return userCodeFileInfo;
}

bool isOptPass(const string &passName) {
  // TODO(youxiang): Currently, we consider every pass which can find metadata
  // as an opt
  //  pass to simplify pass managers' work (otherwise, one need to add/modify
  //  his/her passname here whenever he create/rename a pass). It may prove to
  //  be a problem later on.

  /*
  set<string> passes = {"none",
                        "preprocess",
                        "kernel_pragma_gen",
                        "kernel_separate",
                        "kernel_wrapper",
                        "postwrap_process",
                        "postwrap_process_1",
                        "interface_transform",
                        "altera_preprocess",
                        "midend_preprocess",
                        "memory_burst",
                        "delinearization",
                        "coarse_parallel",
                        "bitwidth_opt",
                        "reduction",
                        "coarse_pipeline",
                        "memory_partition",
                        "loop_parallel",
                        "altera_postprocess",
                        "mars_gen_preproc",
                        "block",
                        "outline",
                        "tldm_gen",
                        "opencl_gen_pcie"};
                        */
  string metadataLocation = getMetadataLocation(passName);
  path p(metadataLocation);
  if (is_directory(p)) {
    return true;
  }
  HIS_WARNING("it is not a opt pass, history is not guaranteed to work.");
  return false;
}

//  This acts as a modern replacement to ROSE's isSgXXX set of RTTI functions.
//  20181204 Youxiang: ROSE's isSgXXX has been implemented similarly like the
//  following
template <typename NodeTy> static const NodeTy *isa(const SgNode *node) {
  return static_cast<const NodeTy *>(isa<NodeTy>(const_cast<SgNode *>(node)));
}

template <typename NodeTy> static NodeTy *isa(SgNode *node) {
  assert(node && "Use isa_or_null for possibly nullptrs.");
  if (rose_ClassHierarchyCastTable[node->variantT()]
                                  [NodeTy::static_variant >> 3] &
      (1 << (NodeTy::static_variant & 7))) {
    return static_cast<NodeTy *>(node);
  }
  return nullptr;
}

//  Traverse the AST under `ast`, calling `handler` on all nodes of type
//  NodeTy. This will ignore nodes that aren't user-defined.
template <typename NodeTy, typename Fn>
vector<NodeTy *> trav(const CSageCodeGen &ast, SgNode *root, Fn &&handler) {
  return trav<NodeTy>(root, std::forward<Fn>(handler), [&](NodeTy *n) {
    if (n->isTransformation() || ast.IsFromInputFile(n)) {
      return true;
    }
    HIS_INFO("skipping non-user-defined node");
    return false;
  });
}

//  Traverse the AST under `ast` and call `handler` on all nodes of type
//  NodeTy for which `filter(node) == true`.
template <typename NodeTy, typename Fn, typename Gn>
vector<NodeTy *> trav(SgNode *root, Fn &&handler, Gn &&filter) {
  //  we need to first store the set of nodes on which to call `handler`
  //  because handler could insert into the AST (which would affect
  //  traversal).
  vector<NodeTy *> nodes;
  auto filt = [&](SgNode *node) {
    if (NodeTy *n = isa<NodeTy>(node)) {
      //  This assumes that NodeTy is a subclass of SgLocatedNode.
      if (filter(n)) {
        nodes.push_back(n);
      }
    }
  };
  CSageWalker<decltype(filt)> *walker =
      new CSageWalker<decltype(filt)>(std::move(filt));
  walker->traverse(root, preorder);
  delete walker;
  for (NodeTy *node : nodes) {
    handler(node);
  }
  return nodes;
}

//  Binds histories of template fn instance nodes to the history of their
//  respective template fn decl nodes. With this binding, we can trace to user
//  code, but not the compiler generated instantiation.
void bindTemplateInstantiation(CSageCodeGen *ast) {
  SgProject &root = *reinterpret_cast<SgProject *>(ast->GetProject());

  //  Build tfn_syms, which is needed to find function template for a function
  //  instantiation
  std::map<const SgTemplateFunctionSymbol *, SgTemplateFunctionDeclaration *>
      tfn_syms;
  trav<SgTemplateFunctionDeclaration>(
      *ast, &root, [&](SgTemplateFunctionDeclaration *tfn) {
        auto *tfnsym = isa<SgTemplateFunctionSymbol>(
            tfn->search_for_symbol_from_symbol_table());
        tfn_syms[tfnsym] = tfn;
      });

  //  Given an instantiation, return its unspecialized decl.
  auto orig_tfn = [&](const SgTemplateInstantiationFunctionDecl &tfinst) {
    auto *declsym = isa<SgTemplateFunctionSymbol>(
        tfinst.get_templateDeclaration()
            ->search_for_symbol_from_symbol_table());
    assert(declsym && "Expected a symbol to unspecialized template.");
    auto it = tfn_syms.find(declsym);
    return it != tfn_syms.end() ? it->second : nullptr;
  };

  vector<void *> functions;
  ast->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration", &functions,
                      true);

  for (auto node_ : functions) {
    SgNode *node = static_cast<SgNode *>(node_);
    if (isSkipped(ast, node) ||
        (node->class_name() != "SgTemplateInstantiationFunctionDecl")) {
      continue;
    }
    SgTemplateInstantiationFunctionDecl *funcTemplate =
        static_cast<SgTemplateInstantiationFunctionDecl *>(node);
    bindNodeOrgId(ast, node, orig_tfn(*funcTemplate), true);
  }
}

void historyModuleInit(CSageCodeGen *ast, const string &passName) {
  assert(passName.size() + 1 <= MAX_PASS_LENGTH);
  snprintf(g_passName, passName.size() + 1, "%s", passName.c_str());

  if (!isOptPass(passName)) {
    return;
  }

  string metadataLocation = getMetadataLocation(passName);
  string historyListLoc = metadataLocation + "history_passes.list";
  string userCodeFileInfoLoc = metadataLocation + "user_code_file_info.json";
  if (exists_test(historyListLoc)) {
    allHistoryList =
        loadHistories(historyListLoc, &passNames, metadataLocation);
    readInterReport(userCodeFileInfoLoc, &g_userCodeFileInfo);
  } else {
    if (passName == "none") {
      HIS_INFO("no history to load in the first pass.");
      g_userCodeFileInfo = saveUserCodeFileInfo(ast, userCodeFileInfoLoc);
      bindTemplateInstantiation(ast);
    } else {
      HIS_ERROR(
          "history passes list not found, cannot calculate user code id.");
    }
  }

#if HIS_CHECK
  map<string, map<string, string>> inputReferReport =
      saveForInterPasses(ast, passName);
#endif
  //  / save org id result
  markIncludeExpand(ast);
#if HIS_CHECK
  checkBetweenPasses(ast, inputReferReport);
  saveOrgId(ast, passName);
#endif
}

/******************************************************************************/
/* History markers ************************************************************/
/******************************************************************************/

extern map<SgNode *, int> g_IndexCache;

void markChange(CSageCodeGen *ast, SgNode *node, Action action, const int loc,
                const string &debugInfo /*= ""*/) {
  if (node == nullptr) {
    HIS_ERROR("Try to mark changes for the top level SgNode. (Not suppose to "
              "happen)");
    return;
  }
  HIS_ALGO("mark: " + std::to_string(action) + ", " + std::to_string(loc));
  auto *histories = ast->getHistories();
  //  check if project exists
  if (ast->GetProject() == nullptr) {
    HIS_ERROR("null project");
    return;
  }
  //  try to insert root
  HistoryMarker parentMarker;
  auto rootTopoLocation =
      getTopoLocation(ast, static_cast<SgNode *>(ast->GetProject()));
  parentMarker.setOrgIdentifier(rootTopoLocation);
  HIS_ALGO("mark/insert root: " + rootTopoLocation);
  histories->insert(pair<SgNode *, HistoryMarker>(
      static_cast<SgNode *>(ast->GetProject()), parentMarker));
  //  find the history marker
  auto historyMarkerIt = histories->find(node);
  if (historyMarkerIt == histories->end()) {
    if (ast->is_floating_node(node) == 0) {
      insertParent(ast, node);
    }
    HIS_INFO("Construct new marker.");
    HistoryMarker marker(action, loc);
    HIS_ALGO("Insert " + marker.getHistories().at(0).toString() +
             " to history map.");
    HIS_ALGO("mark/insert: " << node);
    histories->insert(pair<SgNode *, HistoryMarker>(node, marker));
  } else {
    HIS_INFO("Update entry to history map.");
    historyMarkerIt->second.getHistories().push_back(HistoryEntry(action, loc));
    HIS_ALGO("Add action " +
             historyMarkerIt->second.getHistories().back().toString() +
             " to history map.");
  }
  //  encode debugInfo into message
  if (!debugInfo.empty()) {
    map<string, string> message;
    message["DebugInfo"] = debugInfo;
    insertMessage(ast, node, message);
  }
}

void markInsert(CSageCodeGen *ast, SgNode *node, const int loc,
                const string &debugInfo /*= ""*/) {
  g_IndexCache.clear();
  DEFINE_START_END;
  STEMP(start);
  invalidateCache();
  HIS_ALGO("mark insert for " << node << ", " << (int64_t)node);
  if (isSkipped(ast, node)) {
    HIS_WARNING("mark history in skipped node, ignored");
    return;
  }
  if (loc < 0) {
    HIS_ERROR("insert a negative index.");
    return;
  }
  markChange(ast, node, INSERT, loc, debugInfo);
  ACCTM(markInsert, start, end);
}

void markDelete(CSageCodeGen *ast, SgNode *node, const int loc,
                const string &debugInfo /*= ""*/,
                SgNode *removeNode /*= nullptr*/) {
  g_IndexCache.clear();
  DEFINE_START_END;
  STEMP(start);
  invalidateCache();
  HIS_ALGO("mark delete for " << node << ", " << (int64_t)node);
  if (isSkipped(ast, node)) {
    HIS_WARNING("mark history in skipped node, ignored");
    return;
  }
  if (loc < 0) {
    HIS_ERROR("delete a negative index.");
    return;
  }

  auto *history = ast->getHistories();
  SgNode *child = getNodeByTopoIndex(ast, node, loc);
  if (removeNode != nullptr) {
    //  we do not delete all the histories in the subtree here because when use
    //  it for set_body we do not want to remove the histories in the sub-tree.
    //  Current solution is remove them when markBuild has the same pointer.
    //  Also, when we save history_*.json, we will not save anything that is not
    //  in the tree.
    history->erase(removeNode);
    orgIdMap.erase(removeNode);
  } else {
    HistoryMarker marker;
    history->insert(pair<SgNode *, HistoryMarker>(child, marker));
    auto it = history->find(child);
    if (it == history->end()) {
      HIS_ERROR("entry cannot be found after insert.");
    } else {
      auto orgId = getOriginalTopoLocation(ast, history, child);
      it->second.setOrgIdentifier(orgId);
    }
  }

  markChange(ast, node, DELETE, loc, debugInfo);
  ACCTM(markDelete, start, end);
}

void addSubnodeToHistory(CSageCodeGen *ast, SgNode *oNode, SgNode *node) {
  vector<SgNode *> oChildren = oNode->get_traversalSuccessorContainer();
  vector<SgNode *> children = node->get_traversalSuccessorContainer();
  if (oChildren.size() != children.size()) {
    HIS_ERROR("Copied tree has different size of original one.");
    return;
  }

  auto *histories = ast->getHistories();
  for (size_t i = 0; i < oChildren.size(); ++i) {
    auto markerIt = histories->find(oChildren[i]);
    if (markerIt != histories->end()) {
      if (histories->find(children[i]) != histories->end()) {
        HIS_WARNING("A Copied sub-node already in history.");
      } else {
        HIS_ALGO("mark/insert sub node from copy: " << oChildren[i]);
        histories->insert(
            pair<SgNode *, HistoryMarker>(children[i], markerIt->second));
      }
    }
    if (oChildren[i] != nullptr) {
      addSubnodeToHistory(ast, oChildren[i], children[i]);
    } else if (children[i] != nullptr) {
      HIS_ERROR("Original node is not null, but new copied node is null.");
    }
  }
}

void markCopy(CSageCodeGen *ast, SgNode *oNode, SgNode *node,
              const string &debugInfo /*= ""*/,
              const bool passMajor /*= false*/) {
  DEFINE_START_END;
  STEMP(start);
  auto *histories = ast->getHistories();
  if (histories->find(node) != histories->end()) {
    HIS_WARNING("mark copy for duplicated node: " << node);
    histories->erase(node);
    orgIdMap.erase(node);
  }

  HIS_ALGO("mark copy from " << oNode << " to " << node);
  markChange(ast, node, COPY, -1, debugInfo);
  insertParent(ast, oNode);

  auto historyMarkerIt = histories->find(node);
  if (historyMarkerIt == histories->end()) {
    HIS_ERROR("Cannot find copy mark after insert.");
    return;
  }
  string org_identifier;
  if (isSkipped(ast, oNode)) {
    org_identifier = "BuiltByCopyFromSkippedNode";
  } else {
    org_identifier = getOriginalTopoLocation(ast, ast->getHistories(), oNode);
  }
  //  / is floating node
  //  if (ast->is_floating_node(oNode)) {
  //   auto oMarkerIt = histories->find(oNode);
  //   if (oMarkerIt == histories->end()) {
  //     HIS_ERROR("Copy from a floating node that is not in histories.");
  //     return;
  //   }
  //   org_identifier = oMarkerIt->second.getOrgIdentifier();
  //   HIS_ALGO("borrow org id of " << oNode << ": " << org_identifier);
  //  } else {
  //   string topoLocation = getTopoLocation(ast, oNode);
  //   HIS_ALGO("Current topology location: " + topoLocation);
  //   HIS_ALGO("Calculating original topology location for copy node");
  //   org_identifier =
  //       getOriginalTopoLocation(ast, ast->getHistories(), topoLocation);
  //  }
  HIS_ALGO("  Original topology location: " + org_identifier);
  historyMarkerIt->second.setOrgIdentifier(org_identifier);
  historyMarkerIt->second.setAllApplied();
  if (oNode != node) {
    auto oHistoryMarkerIt = histories->find(oNode);
    if (oHistoryMarkerIt != histories->end()) {
      oHistoryMarkerIt->second.setOrgIdentifier(org_identifier);
      historyMarkerIt->second.increaseAppliedNum(
          oHistoryMarkerIt->second.getAppliedIndex() + 1);
      auto &oHistory = oHistoryMarkerIt->second.getHistories();
      auto &oMessage = oHistoryMarkerIt->second.getMessages();
      auto &history = historyMarkerIt->second.getHistories();
      auto &message = historyMarkerIt->second.getMessages();

      if (history.size() != 1) {
        HIS_ERROR("history size error: " << history.size());
        std::stringstream ss;
        for (auto val : history) {
          ss << ", " << val.toString();
        }
        HIS_ERROR("history: " << ss.str());
      }
      for (size_t i = 0; i < oHistory.size(); ++i) {
        history.push_back(oHistory[i]);
      }
      for (size_t i = 0; i < oMessage.size(); ++i) {
        message.push_back(oMessage[i]);
      }
    }
  }
  if (passMajor) {
    //  update qor_merge_mode
    string qorMergeMode = getQoRMergeMode(ast, oNode);
    setQoRMergeMode(ast, node, qorMergeMode);
  }

  //  also save to old node can cache more result

  addSubnodeToHistory(ast, oNode, node);

  ACCTM(markCopy, start, end);
}

void setParallel(CSageCodeGen *ast, void *node_, bool isParallel,
                 bool isRecursivelySet /*= false*/) {
  SgNode *node = static_cast<SgNode *>(node_);
  auto *histories = ast->getHistories();

  if (!isRecursivelySet) {
    SgNode *n = static_cast<SgNode *>(node_);
    if (histories->find(n) == histories->end()) {
      HistoryMarker marker;
      histories->insert(pair<SgNode *, HistoryMarker>(n, marker));
    }

    HIS_ALGO("mark parallel to " + std::to_string(isParallel));
    (*histories)[n].setParallel(isParallel);
    return;
  }

  vector<void *> statements;
  vector<void *> funcCallExprs;
  ast->GetNodesByType(node, "preorder", "SgStatement", &statements, true);
  ast->GetNodesByType(node, "preorder", "SgFunctionCallExp", &funcCallExprs,
                      true);

  vector<void *> savedNodes(statements);
  savedNodes.insert(savedNodes.end(), funcCallExprs.begin(),
                    funcCallExprs.end());

  for (void *n_ : savedNodes) {
    SgNode *n = static_cast<SgNode *>(n_);
    if (histories->find(n) == histories->end()) {
      HistoryMarker marker;
      histories->insert(pair<SgNode *, HistoryMarker>(n, marker));
    }

    HIS_ALGO("mark parallel to " + std::to_string(isParallel));
    (*histories)[n].setParallel(isParallel);
  }
}

void setNoHierarchy(CSageCodeGen *ast, void *node_, int isNoHierarchy) {
  SgNode *node = static_cast<SgNode *>(node_);
  auto *histories = ast->getHistories();
  if (histories->find(node) == histories->end()) {
    HistoryMarker marker;
    histories->insert(pair<SgNode *, HistoryMarker>(node, marker));
  }
  HIS_ALGO("mark no_hierarchy to " + std::to_string(isNoHierarchy));
  (*histories)[node].setNoHierarchy(isNoHierarchy);
}

void setQoRMergeMode(CSageCodeGen *ast, void *node_, string qorMergeMode,
                     bool isRecursivelySet /*= false*/) {
  SgNode *node = static_cast<SgNode *>(node_);
  auto *histories = ast->getHistories();

  if (!isRecursivelySet) {
    SgNode *n = static_cast<SgNode *>(node_);
    if (histories->find(n) == histories->end()) {
      HistoryMarker marker;
      histories->insert(pair<SgNode *, HistoryMarker>(n, marker));
    }

    HIS_ALGO("mark qor_merge_mode to " + qorMergeMode);
    (*histories)[n].setQoRMergeMode(qorMergeMode);
    return;
  }

  vector<void *> statements;
  vector<void *> funcCallExprs;
  ast->GetNodesByType(node, "preorder", "SgStatement", &statements, true);
  ast->GetNodesByType(node, "preorder", "SgFunctionCallExp", &funcCallExprs,
                      true);

  vector<void *> savedNodes(statements);
  savedNodes.insert(savedNodes.end(), funcCallExprs.begin(),
                    funcCallExprs.end());

  for (void *n_ : savedNodes) {
    SgNode *n = static_cast<SgNode *>(n_);
    if (histories->find(n) == histories->end()) {
      HistoryMarker marker;
      histories->insert(pair<SgNode *, HistoryMarker>(n, marker));
    }

    if (n != node &&
        (!isRecursivelySet || !(*histories)[n].getQoRMergeMode().empty())) {
      continue;
    }

    HIS_ALGO("mark qor_merge_mode to " + qorMergeMode);
    (*histories)[n].setQoRMergeMode(qorMergeMode);
  }
}

string bindNodeOrgId(CSageCodeGen *ast, void *node_, void *bindNode,
                     bool force /*= false*/) {
  DEFINE_START_END;
  if (bindNode == nullptr) {
    return "";
  }

  if (ast->is_floating_node(bindNode) != 0) {
    HIS_ERROR("bind to an floating node (not nullptr).");
    return "";
  }

  if (ast->is_floating_node(node_) == 0) {
    HIS_WARNING("bind on an node in the tree, usually has problem.");
  }

  auto *histories = ast->getHistories();
  STEMP(start);
  insertParent(ast, static_cast<SgNode *>(bindNode));
  ACCTM(bindNodeOrgId___insertParent, start, end);
  STEMP(start);
  string bindTopoLocation =
      getTopoLocation(ast, static_cast<SgNode *>(bindNode));
  ACCTM(bindNodeOrgId___getTopoLocation, start, end);
  STEMP(start);
  string orgId =
      getOriginalTopoLocation(ast, ast->getHistories(), bindTopoLocation);
  ACCTM(bindNodeOrgId___getOriginalTopoLocation, start, end);
  if (!orgId.empty()) {
    HIS_ALGO("[bind to node] node: " + printNodeInfo(ast, bindNode, 80) +
             " id: " + orgId + ", user id: " + getUserCodeId(ast, bindNode));
    orgId[0] = 'B';
  } else {
    HIS_ERROR("bind target has an empty org id");
  }

  if (histories->find(static_cast<SgNode *>(node_)) == histories->end()) {
    HistoryMarker marker;
    (*histories)[static_cast<SgNode *>(node_)] = marker;
  }

  //  copy the message and set no_hierarchy
  STEMP(start);
  if (histories->find(static_cast<SgNode *>(bindNode)) != histories->end()) {
    auto &bindMessage =
        (*histories)[static_cast<SgNode *>(bindNode)].getMessages();
    auto &message = (*histories)[static_cast<SgNode *>(node_)].getMessages();
    set<string> msgSet;
    msgSet.insert(bindMessage.begin(), bindMessage.end());
    msgSet.insert(message.begin(), message.end());
    message.clear();
    message.insert(message.end(), msgSet.begin(), msgSet.end());
    HIS_ALGO("insert " << bindMessage.size()
                       << " elements, not checked for empty");
    if (message.size() > 10) {
      HIS_ALGO("message is longer than 10, it is " << message.size());
    }
    (*histories)[static_cast<SgNode *>(node_)].setNoHierarchy(
        (*histories)[static_cast<SgNode *>(bindNode)].getNoHierarchy());
  }
  ACCTM(bindNodeOrgId___message, start, end);

  map<string, string> note;
  note["DebugInfo"] = "copy message from bind node";
  insertMessage(ast, node_, note);

  if (force ||
      (*histories)[static_cast<SgNode *>(node_)].getOrgIdentifier().empty()) {
    (*histories)[static_cast<SgNode *>(node_)].setOrgIdentifier(orgId);
  } else {
    HIS_WARNING("try to bind a node with org id");
  }

  return orgId;
}

void markBuild(CSageCodeGen *ast, void *node_, const string &debugInfo /*= ""*/,
               void *bindNode) {
  DEFINE_START_END;
  SgNode *node = static_cast<SgNode *>(node_);

  auto *histories = ast->getHistories();
  if (histories->find(node) != histories->end()) {
    HIS_WARNING("mark build for duplicated node.");
    histories->erase(node);
    orgIdMap.erase(node);
  }

  HIS_ALGO("mark build for " << node << ", " << (int64_t)node);
  STEMP(start);
  markChange(ast, node, BUILD, -1, debugInfo);
  ACCTM(markBuild___markChange, start, end);

  //  find the history marker
  auto historyMarkerIt = histories->find(node);
  if (historyMarkerIt == histories->end()) {
    HIS_ERROR("Cannot find copy mark after insert.");
    return;
  }
  if (historyMarkerIt->second.getHistories().size() != 1) {
    HIS_ERROR("history size error: "
              << historyMarkerIt->second.getHistories().size());
    std::stringstream ss;
    for (auto val : historyMarkerIt->second.getHistories()) {
      ss << ", " << val.toString();
    }
    ss << endl;
    for (auto val : historyMarkerIt->second.getMessages()) {
      ss << ", " << val;
    }
    HIS_ERROR("history: " << ss.str());
  }
  static int id = 0;
  string orgId = string("BuiltIn") + g_passName + "_" + std::to_string(id++);
  if (bindNode != nullptr) {
    STEMP(start);
    orgId = bindNodeOrgId(ast, node_, bindNode);
    ACCTM(markBuild___bindNodeOrgId, start, end);
  }
  historyMarkerIt->second.setOrgIdentifier(orgId);
  historyMarkerIt->second.setAllApplied();
  historyMarkerIt->second.setPropagate(0);
  HIS_ALGO("set org id and applied index for build node: " + orgId);
}

string getJsonString(const map<string, string> &msg_) {
  Document document;
  Document::AllocatorType &allocator = document.GetAllocator();
  map<string, string> msg = msg_;
  const char json[] = "{}";
  if (document.Parse(json).HasParseError()) {
    return "";
  }

  msg.insert(pair<string, string>("passname", g_passName));

  for (auto &entry : msg) {
    Value attribute_name;
    attribute_name.SetString(entry.first.c_str(), allocator);
    Value attribute_value;
    attribute_value.SetString(entry.second.c_str(), allocator);
    document.AddMember(attribute_name, attribute_value, allocator);
  }

  StringBuffer SB;
  PrettyWriter<StringBuffer> writer(SB);
  document.Accept(writer);
  auto jsonString = SB.GetString();
  return jsonString;
}

void insertMessage(CSageCodeGen *ast, void *node_,
                   const map<string, string> &msg) {
  SgNode *node = static_cast<SgNode *>(node_);
  auto *history = ast->getHistories();
  HistoryMarker marker;
  auto markerIt = history->find(node);
  if (markerIt == history->end()) {
    (*history)[node] = marker;
    HIS_INFO("create an empty node for inserting message.");
  }
  markerIt = history->find(node);

  string msgString = getJsonString(msg);
  HIS_ALGO("inserting " + msgString);
  markerIt->second.getMessages().push_back(msgString);
}

/******************************************************************************/
/* Mark includ files **********************************************************/
/******************************************************************************/

SgNode *getPreviousNode(SgNode *node) {
  SgNode *parent = node->get_parent();
  int index = parent->get_childIndex(node);
  if (index > 0) {
    return parent->get_traversalSuccessorContainer()[index - 1];
  }
  return nullptr;
}

SgNode *getNextNode(SgNode *node) {
  SgNode *parent = node->get_parent();
  size_t index = parent->get_childIndex(node);
  if (index + 1 < parent->get_numberOfTraversalSuccessors()) {
    return parent->get_traversalSuccessorContainer()[index + 1];
  }
  return nullptr;
}

void markIncludeFile(CSageCodeGen *ast, SgNode *scope, string filename,
                     int before) {
  HIS_ALGO("mark include file for scope: " + scope->class_name());

  if (isSkippedIncludeFile(filename)) {
    return;
  }

  filename = getSimpleFilename(filename);
  replace(filename.begin(), filename.end(), '.', '_');

  static int index = 0;
  string varName = filename + "_" + std::to_string(index++);

  string leadingString = "__merlin_include";

  if ((scope->class_name() == "SgGlobal") && (before != 0)) {
    SgNode *node = getNodeByTopoIndex(ast, scope, 0);
    if (node == nullptr) {
      HIS_ERROR("first element is null.");
      return;
    }

    HIS_ALGO("create new variable: " + leadingString + "__GB_" + varName);
    void *vBegin = ast->CreateVariableDecl(
        "int", leadingString + "__GB_" + varName, nullptr, scope);
    ast->SetExtern(vBegin);

    ast->InsertStmt(vBegin, node);
  } else if (scope->class_name() == "SgGlobal") {
    int size = getChildrenNumWithoutInclude(ast, scope);
    SgNode *node = getNodeByTopoIndex(ast, scope, size - 1);
    if (node == nullptr) {
      HIS_ERROR("last element is null.");
      return;
    }

    HIS_ALGO("create new variable: " + leadingString + "__GE_" + varName);
    void *vEnd = ast->CreateVariableDecl(
        "int", leadingString + "__GE_" + varName, nullptr, scope);
    ast->SetExtern(vEnd);

    ast->InsertAfterStmt(vEnd, node);
  } else {
    SgNode *parent = scope->get_parent();
    if (parent == nullptr) {
      HIS_ERROR("parent is nullptr.");
      return;
    }

    if (before != 0) {
      SgNode *previousNode = getPreviousNode(scope);
      if (previousNode != nullptr) {
        HIS_ALGO("create new variable: " + leadingString + "__LB_" + varName);
        void *vBegin = ast->CreateVariableDecl(
            "int", leadingString + "__LB_" + varName, nullptr, parent);
        ast->SetExtern(vBegin);

        HIS_ALGO("create new variable: " + leadingString + "__LE_" + varName);
        void *vEnd = ast->CreateVariableDecl(
            "int", leadingString + "__LE_" + varName, nullptr, parent);
        ast->SetExtern(vEnd);

        ast->InsertAfterStmt(vBegin, previousNode);
        ast->InsertStmt(vEnd, scope);
      } else {
        HIS_ALGO("create new variable: " + leadingString + "__GE_" + varName);
        void *vGEnd = ast->CreateVariableDecl(
            "int", leadingString + "__GE_" + varName, nullptr, parent);
        ast->SetExtern(vGEnd);

        ast->InsertStmt(vGEnd, scope);
      }
    } else {
      SgNode *nextNode = getNextNode(scope);
      if (nextNode != nullptr) {
        HIS_ALGO("create new variable: " + leadingString + "__LB_" + varName);
        void *vBegin = ast->CreateVariableDecl(
            "int", leadingString + "__LB_" + varName, nullptr, parent);
        ast->SetExtern(vBegin);

        HIS_ALGO("create new variable: " + leadingString + "__LE_" + varName);
        void *vEnd = ast->CreateVariableDecl(
            "int", leadingString + "__LE_" + varName, nullptr, parent);
        ast->SetExtern(vEnd);

        ast->InsertAfterStmt(vBegin, scope);
        ast->InsertStmt(vEnd, nextNode);
      } else {
        HIS_ALGO("create new variable: " + leadingString + "__GB_" + varName);
        void *vGBegin = ast->CreateVariableDecl(
            "int", leadingString + "__GB_" + varName, nullptr, parent);
        ast->SetExtern(vGBegin);

        ast->InsertAfterStmt(vGBegin, scope);
      }
    }
  }
}

string getFilenameFromMacro(string includeMacro) {
  int left;
  int right;
  if (includeMacro.find("\"") != string::npos) {
    left = includeMacro.find("\"");
    right = includeMacro.rfind("\"");
  } else if (includeMacro.find("<") != string::npos) {
    left = includeMacro.find("<");
    right = includeMacro.rfind(">");
  } else {
    LOC_ERROR("wrong include macro: " + includeMacro);
    return "";
  }

  string filename = includeMacro.substr(left + 1, right - left - 1);
  LOC_INFO("included file loc: " + filename);

  return filename;
}

void markIncludeMacro(CSageCodeGen *ast, SgNode *scope, string macro,
                      int before) {
  string filename = getFilenameFromMacro(macro);
  HIS_ALGO("filename: " + filename);
  markIncludeFile(ast, scope, filename, before);
}

void tryMarkNode(CSageCodeGen *ast, SgNode *parent, int i, int loc,
                 string filename) {
  SgNode *node = getNodeByTopoIndex(ast, parent, i);
  if (isSkipped(ast, node)) {
    HIS_WARNING("null node or without file info");
    return;
  }
  static set<SgNode *> gExpandedIncludeNode;
  if (!ast->IsTransformation(node) &&
      gExpandedIncludeNode.find(node) == gExpandedIncludeNode.end()) {
    markBuild(ast, node, "include element: " + printNodeInfo(ast, node, 80));
    markInsert(ast, parent, loc,
               "macro expand: " + printNodeInfo(ast, node, 80));
    gExpandedIncludeNode.insert(node);
  }
}

void tryRemoveChild(CSageCodeGen *ast, SgNode *node, bool move_prep_info) {
  ast->replace_with_dummy_decl(node);
}

void markIncludeExpand(CSageCodeGen *ast) {
  HIS_ALGO("try to expand include macro.");
  vector<void *> variables;
  ast->GetNodesByType(nullptr, "preorder", "SgVariableDeclaration", &variables);
  for (auto var : variables) {
    SgNode *node = static_cast<SgNode *>(var);
    SgNode *parent = node->get_parent();

    string varDecl = ast->_up(node);
    string leadingString = "__merlin_include";
    size_t varNameIndex = varDecl.find(leadingString);
    if (varNameIndex == string::npos ||
        (node->class_name() != "SgVariableDeclaration")) {
      continue;
    }

    HIS_ALGO("type: " + node->class_name());
    string varName = varDecl.substr(varNameIndex + leadingString.size());
    string typeString = varName.substr(0, 5);
    string filename = varName.substr(5, varName.rfind("_") - 5);
    HIS_ALGO("found variable: " + varDecl);
    HIS_ALGO("filename: " + filename);

    if (typeString == "__GB_") {
      int index = getTopoIndex(ast, node);
      for (int i = index - 1; i >= 0; --i) {
        tryMarkNode(ast, parent, i, 0, filename);
      }
      tryRemoveChild(ast, node, true);
    } else if (typeString == "__GE_") {
      int index = getTopoIndex(ast, node);
      for (int i = getChildrenNumWithoutInclude(ast, parent) - 1; i > index;
           --i) {
        tryMarkNode(ast, parent, i, index + 1, filename);
      }
      tryRemoveChild(ast, node, true);
    } else if (typeString == "__LB_") {
      int index = getTopoIndex(ast, node);
      int size = getChildrenNumWithoutInclude(ast, parent);
      bool found = false;

      for (int i = index + 1; i < size; ++i) {
        auto brother = getNodeByTopoIndex(ast, parent, i);
        if (brother == nullptr || brother->get_file_info() == nullptr) {
          HIS_ERROR("null node or without file info");
          return;
        }
        string brotherName = ast->_up(brother);
        brotherName[varNameIndex + leadingString.size() + 3] = 'B';
        if (brotherName == varDecl) {
          found = true;
          tryRemoveChild(ast, brother, true);
          break;
        }
        tryMarkNode(ast, parent, i, i, filename);
      }
      tryRemoveChild(ast, node, true);
      if (!found) {
        HIS_ERROR("end not found for: " + varDecl);
        return;
      }
    }
  }
}

/******************************************************************************/
/* Testing ********************************************************************/
/******************************************************************************/

#if HIS_CHECK_TMP
const int interPassesTestSize = 240;
#endif

map<SgNode *, string> saveOrgId(CSageCodeGen *ast, string passName) {
#if HIS_CHECK_TMP
  if (!isOptPass(g_passName)) {
    return orgIdMap;
  }

  vector<void *> functions;
  ast->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration", &functions,
                      true);
  vector<void *> statements;
  ast->GetNodesByType(nullptr, "preorder", "SgStatement", &statements, true);

  vector<void *> testElements(functions);
  testElements.insert(testElements.end(), statements.begin(), statements.end());

  //  start from ast node
  for (size_t i = 0; i < testElements.size(); i++) {
    SgNode *node = static_cast<SgNode *>(testElements[i]);
    if (isSkipped(ast, node)) {
      continue;
    }
    auto id = getTopoLocation(ast, node);
    auto orgId = getOriginalTopoLocation(ast, ast->getHistories(), node);
    if (orgId.empty()) {
      HIS_WARNING("skip: " + printNodeInfo(ast, node, 80));
      continue;
    }
    orgId = getOriginalTopoLocation(ast, ast->getHistories(), id);
    orgIdMap.insert(pair<SgNode *, string>(node, orgId));
    HIS_ALGO("insert to org id map(" + orgId + ", " +
             ast->GetFileInfo_filename(node) +
             "): " + printNodeInfo(ast, node, 80));
  }

#endif
  return orgIdMap;
}

map<string, map<string, string>>
saveForInterPasses(CSageCodeGen *ast, string passName, bool isForNextPass) {
  map<string, map<string, string>> referReport;
#if HIS_CHECK_TMP
  if (!isOptPass(g_passName)) {
    return referReport;
  }

  vector<void *> functions;
  ast->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration", &functions,
                      true);
  vector<void *> statements;
  ast->GetNodesByType(nullptr, "preorder", "SgStatement", &statements, true);

  vector<void *> testElements(functions);
  testElements.insert(testElements.end(), statements.begin(), statements.end());

  //  start from ast node
  for (size_t i = 0; i < testElements.size(); i++) {
    SgNode *node = static_cast<SgNode *>(testElements[i]);
    if (isSkipped(ast, node)) {
      HIS_INFO("skip a skiped node, should not be here.");
      continue;
    }
    if (node == nullptr || node->get_file_info() == nullptr) {
      HIS_INFO("skip top level nodes higher than file.");
      continue;
    } else {
      bool isInWrapperFile = false;
      SgNode *nodeWithFilename = node;
      string fileName;
      do {
        fileName = ast->GetFileInfo_filename(nodeWithFilename, 1);
        if (isWrapperFile(fileName)) {
          isInWrapperFile = true;
          break;
        }
        nodeWithFilename =
            static_cast<SgNode *>(ast->GetParent(nodeWithFilename));
      } while (nodeWithFilename != nullptr &&
               nodeWithFilename->get_file_info() != nullptr &&
               !fileName.compare("compilerGenerated"));

      if (isInWrapperFile) {
        HIS_INFO("skip node in wrapper file.");
        continue;
      }
    }

    auto id = getTopoLocation(ast, node);
    referReport["content"][id] = printNodeInfo(ast, node, interPassesTestSize);
  }

  if (isForNextPass) {
    writeInterReport("refer_report.json", referReport);
    writeInterReport("refer_report_" + passName + ".json", referReport);
  } else {
    string referReportURL = "input_refer_report_" + passName + ".json";
    writeInterReport(referReportURL, referReport);
  }
#endif
  return referReport;
}

#if HIS_CHECK_TMP
bool isIdenticalIgnoreWhiteSpace(const string &oldVal, const string &newVal,
                                 int size) {
  boost::regex spaceRegex("\\s+");
  string oldValNF = regex_replace(oldVal, spaceRegex, "");
  string newValNF = regex_replace(newVal, spaceRegex, "");

  if (oldValNF.size() > (size_t)size / 2 &&
      newValNF.size() > (size_t)size / 2) {
    oldValNF = oldValNF.substr(0, oldValNF.size() - 10);
    newValNF = newValNF.substr(0, newValNF.size() - 10);
  }

  int length =
      oldValNF.size() < newValNF.size() ? oldValNF.size() : newValNF.size();

  return !newValNF.compare(0, length, oldValNF.substr(0, length));
}

bool isIdenticalIgnoreMacro(const string &oldVal, const string &newVal,
                            int size) {
  boost::regex spaceRegex("\\s+");
  string oldValNF = regex_replace(oldVal, spaceRegex, "");
  string newValNF = regex_replace(newVal, spaceRegex, "");

  if (oldValNF.size() > (size_t)size / 2 &&
      newValNF.size() > (size_t)size / 2) {
    oldValNF = oldValNF.substr(0, oldValNF.size() - 10);
    newValNF = newValNF.substr(0, newValNF.size() - 10);
  }

  int length = newValNF.size() / 2;
  if (length < 0 || oldValNF.size() < (size_t)length) {
    return false;
  }
  string shortVal = oldValNF.substr(oldValNF.size() - length, length);
  return (newValNF.find(shortVal) != string::npos);
}

void checkContent(CSageCodeGen *ast, const map<string, string> &oldContent,
                  const map<string, string> &newContent) {
  auto *histories = ast->getHistories();
  for (auto &entry : newContent) {
    string id = entry.first;
    string newVal = entry.second;

    HIS_INFO("check: " + id);
    HIS_INFO("check: " + newVal);

    auto node = getSgNode(ast, id);
    auto orgId = getOriginalTopoLocation(ast, histories, node);
    if (orgId.empty()) {
      HIS_WARNING("skip: " + printNodeInfo(ast, node, 80));
      continue;
    }
    orgId = getOriginalTopoLocation(ast, histories, id);

    string buildPrefix = "Built";
    if (!orgId.compare(0, buildPrefix.size(), buildPrefix)) {
      HIS_INFO("ignore built node: " + orgId);
      continue;
    } else if (oldContent.find(orgId) == oldContent.end()) {
      HIS_WRONG(orgId + " not found in previous pass.");
      HIS_WRONG("\tafter: " + newVal);
      continue;
    }

    string oldVal = oldContent.find(orgId)->second;

    boost::regex spaceRegex("\\s+");
    string oldValNF = regex_replace(oldVal, spaceRegex, "");

    if (oldValNF[0] == '#' || oldValNF[0] == '/') {
      if (isIdenticalIgnoreMacro(oldVal, newVal, interPassesTestSize)) {
        HIS_RIGHT("correct(ignore macro)");
        HIS_RIGHT("\tbefore(ignore macro): " + oldVal);
        HIS_RIGHT("\t after(ignore macro): " + newVal);
      } else {
        HIS_WRONG(
            orgId +
            "(ignore macro): its content is not equivalent between passes.");
        HIS_WRONG("\tbefore(ignore macro): " + oldVal);
        HIS_WRONG("\t after(ignore macro): " + newVal);
      }
    } else if (isIdenticalIgnoreWhiteSpace(oldVal, newVal,
                                           interPassesTestSize)) {
      HIS_RIGHT("correct: " + newVal);
    } else {
      HIS_WRONG(orgId + ": its content is not equivalent between passes.");
      HIS_WRONG("\tbefore: " + oldVal);
      HIS_WRONG("\t after: " + newVal);
    }
  }
}
#endif

void checkBetweenPasses(
    CSageCodeGen *ast,
    const map<string, map<string, string>> &inputReferReport) {
#if HIS_CHECK_TMP
  if (!isOptPass(g_passName)) {
    return;
  }

  string metadataLocation = getMetadataLocation(g_passName);
  string oldReferReportURL = metadataLocation + "refer_report.json";
  bool isCheckNeeded = exists_test(oldReferReportURL);
  map<string, map<string, string>> oldReferReport;
  if (isCheckNeeded) {
    readInterReport(oldReferReportURL, &oldReferReport);
    checkContent(ast, oldReferReport["content"], inputReferReport["content"]);
  } else {
    HIS_RIGHT(
        "reference report not found, this only happens for the first pass.");
  }
#endif
}

void checkReferenceInduction(CSageCodeGen *ast,
                             const map<SgNode *, string> &orgIdMap,
                             string passName) {
#if HIS_CHECK_TMP
  if (!isOptPass(g_passName)) {
    return;
  }

  HIS_TEST("== TEST STARTED "
           "================================================================");
  auto *histories = ast->getHistories();
  auto sHistories = getSerializableHistories(ast, histories);

  HIS_TEST(std::to_string(orgIdMap.size()) + " entries will be checked.");

  for (auto &ele : orgIdMap) {
    auto node = ele.first;
    auto &orgId = ele.second;

    HIS_TEST("-- check for org id: " + orgId +
             "------------------------------------------------------");
    HIS_TEST("-- " + printNodeInfo(ast, node, 80));

    if (node == nullptr) {
      HIS_TEST("skip an empty node.");
      continue;
    }

    if (ast->is_floating_node(node)) {
      HIS_TEST("skip a floating node.");
      continue;
    }

    if (isSkipped(ast, node)) {
      HIS_TEST("skip a skiped node, should not be here.");
      continue;
    }

    auto topoLocation = getTopoLocation(ast, node);
    HIS_TEST("-- check for output id: " + topoLocation);

    //  check pure ast node version
    auto orgId0 = getOriginalTopoLocation(ast, histories, node);
    if (orgId0.compare(1, orgId.size() - 1, orgId.substr(1))) {
      HIS_WRONG("wrong org id, it should be " + orgId + ", not " + orgId0);
    } else {
      HIS_RIGHT("correct org id: " + orgId);
    }

    //  check ast node version
    auto orgId1 = getOriginalTopoLocation(ast, histories, topoLocation);
    if (orgId1.compare(orgId)) {
      HIS_WRONG("wrong org id, it should be " + orgId + ", not " + orgId1);
    } else {
      HIS_RIGHT("correct org id: " + orgId);
    }

    //  check string version
    auto orgId2 = getOriginalTopoLocation(sHistories, topoLocation);
    if (orgId2.compare(orgId)) {
      HIS_WRONG("wrong org id, it should be " + orgId + ", not " + orgId2);
    } else {
      HIS_RIGHT("correct org id: " + orgId);
    }

    //  if (printNodeInfo(ast, node, 80).find("__merlin_dummy") ==
    //  string::npos)
    //  {
    //   auto userCodeId = getUserCodeFileLocation(ast, node);
    //   HIS_RIGHT("user code id, node info: " + printNodeInfo(ast, node, 80));
    //   HIS_RIGHT("user code id: " + userCodeId);
    //  }
  }

  //  test for template function
  vector<void *> functions;
  ast->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration", &functions,
                      true);
  for (auto node_ : functions) {
    SgNode *node = static_cast<SgNode *>(node_);
    if (isSkipped(ast, node)) {
      continue;
    }
    HIS_TEST("template: " + printNodeInfo(ast, node));
    auto fileinfo = getUserCodeFileLocation(ast, node);
    if (fileinfo.empty()) {
      HIS_TEST("user file info empty for this template function");
    } else {
      HIS_TEST("user file info: " + getUserCodeFileLocation(ast, node));
    }
  }
#endif
}

/******************************************************************************/
/* History IO *****************************************************************/
/******************************************************************************/

map<string, HistoryMarker>
getSerializableHistories(CSageCodeGen *ast,
                         map<SgNode *, HistoryMarker> *histories) {
  DEFINE_START_END;
  map<string, HistoryMarker> serializableHistories;
  for (auto &ele : *histories) {
    if (ele.first == nullptr) {
      HIS_ERROR("The Node is nullptr.");
      continue;
    }
    if (ast->is_floating_node(ele.first) != 0) {
      HIS_INFO("Find a floating node of type " + ele.first->class_name() + ".");
      continue;
    }
    if (isSkipped(ast, ele.first)) {
      HIS_WARNING("History on skipped node.");
      continue;
    }
    //  HIS_INFO("SgNode file loc: " + getFileLocation(ast, ele.first));
    //  HIS_INFO("SgNode type: " + ele.first->class_name());
    //  HIS_INFO("SgNode children size: " +
    //          std::to_string(ele.first->get_traversalSuccessorContainer().size()));
    //  for (auto &child : ele.first->get_traversalSuccessorContainer()) {
    //   if (child) {
    //     HIS_INFO("  child: " + child->class_name());
    //     HIS_INFO("  child: " + getFileLocation(ast, child));
    //   } else {
    //     HIS_INFO("  empty child");
    //   }
    //  }
    STEMP(start);
    insertDebugInfo(ast, ele.first, printNodeInfo(ast, ele.first, 80));
    ACCTM(insertDebugInfo, start, end);
    STEMP(start);
    ele.second.setChildrenNum(getChildrenNumWithoutInclude(ast, ele.first));
    ACCTM(setchildnum, start, end);
    STEMP(start);
    string key = getTopoLocation(ast, ele.first);
    if (!key.empty()) {
      key[0] = 'X';
    }
    ACCTM(getkey, start, end);
    HIS_ALGO("mark/insert to string map: " + key + " (" << ele.first << ")");
    STEMP(start);
    serializableHistories.insert(pair<string, HistoryMarker>(key, ele.second));
    ACCTM(insertentry, start, end);

    //  check the key
#if HIS_CHECK_TMP
    auto nodeTest = getSgNode(ast, key);
    if (nodeTest != ele.first) {
      HIS_ERROR("the key is not calculated correctly," << nodeTest
                                                       << " != " << ele.first);
    }
#endif
  }
  return serializableHistories;
}

map<string, HistoryMarker> readHistoriesFromJson(const string histories_url) {
  std::ifstream ifs(histories_url);
  IStreamWrapper isw(ifs);
  Document d;
  d.ParseStream(isw);
  const Value &histories_json = d.GetObject();
  map<string, HistoryMarker> histories;
  for (auto it = histories_json.MemberBegin(); it != histories_json.MemberEnd();
       ++it) {
    HistoryMarker marker;
    marker.loadFromJson(it->value);
    histories[it->name.GetString()] = marker;
  }
  return histories;
}

void writeHistoriesToJson(const string histories_url,
                          const map<string, HistoryMarker> &histories) {
  Document d;
  d.SetObject();
  Document::AllocatorType &allocator = d.GetAllocator();
  for (auto it = histories.begin(); it != histories.end(); ++it) {
    Value marker;
    it->second.dumpToJson(&marker, &allocator);
    Value key(it->first.c_str(), allocator);
    d.AddMember(key, marker, allocator);
  }

  std::ofstream ofs(histories_url);
  OStreamWrapper osw(ofs);
  PrettyWriter<OStreamWrapper> writer(osw);
  d.Accept(writer);
}

string HistoryEntry::toString() {
  string str;
  switch (this->action) {
  case INSERT:
    str = "INSERT";
    break;
  case DELETE:
    str = "DELETE";
    break;
  case COPY:
    str = "COPY";
    break;
  case BUILD:
    str = "BUILD";
    break;
  default:
    str = "ERROR(" + std::to_string(action) + ")";
    break;
  }
  return str + ":" + std::to_string(this->location);
}

void HistoryEntry::dumpToJson(Value *value,
                              Document::AllocatorType *allocator) const {
  value->SetObject();
  Value action_value;
  action_value.SetInt(static_cast<int>(action));
  value->AddMember("action", action_value, *allocator);
  Value location_value;
  location_value.SetInt(location);
  value->AddMember("location", location_value, *allocator);
}

void HistoryEntry::loadFromJson(const Value &value) {
  action = static_cast<Action>(value["action"].GetInt());
  location = value["location"].GetInt();
}

void HistoryMarker::increaseAppliedNum(size_t num) {
  if (static_cast<size_t>(this->applied_index) >= this->histories.size()) {
    HIS_ERROR("invalid applied index: greater than size");
  }
  this->applied_index += num;
}

string HistoryMarker::toString(string leadingString) {
  std::stringstream ss;
  ss << leadingString << "Original ID: " << this->org_identifier << endl;
  if (histories.size() != messages.size()) {
    ss << leadingString << "ERROR: histories and messages have different size!"
       << endl;
  }
  for (size_t i = 0; i < histories.size() && i < messages.size(); ++i) {
    ss << leadingString << "  History: " << histories[i].toString() << endl;
    ss << leadingString << "  Message: " << messages[i] << endl;
  }
  ss << leadingString << "Applied Index: " << this->applied_index << endl;
  ss << leadingString << "Children Num: " << this->children_num << endl;
  return ss.str();
}

void HistoryMarker::dumpToJson(Value *d,
                               Document::AllocatorType *allocator) const {
  d->SetObject();
  Value org_identifier;
  org_identifier.SetString(this->org_identifier.c_str(), *allocator);
  d->AddMember("org_identifier", org_identifier, *allocator);
  Value user_code_scope_id;
  user_code_scope_id.SetString(this->user_code_scope_id.c_str(), *allocator);
  d->AddMember("user_code_scope_id", user_code_scope_id, *allocator);
  Value applied_index;
  applied_index.SetInt(this->applied_index);
  d->AddMember("applied_index", applied_index, *allocator);
  Value children_num;
  children_num.SetInt(this->children_num);
  d->AddMember("children_num", children_num, *allocator);
  Value propagate;
  propagate.SetInt(this->propagate);
  d->AddMember("propagate", propagate, *allocator);
  Value no_hierarchy;
  no_hierarchy.SetInt(this->no_hierarchy);
  d->AddMember("no_hierarchy", no_hierarchy, *allocator);
  Value qor_merge_mode;
  qor_merge_mode.SetString(this->qor_merge_mode.c_str(), *allocator);
  d->AddMember("qor_merge_mode", qor_merge_mode, *allocator);
  Value parallel;
  parallel.SetInt(this->parallel ? 1 : 0);
  d->AddMember("parallel", parallel, *allocator);
  Value histories(kArrayType);
  for (auto &historyEntry : this->histories) {
    Value history;
    historyEntry.dumpToJson(&history, allocator);
    histories.PushBack(history, *allocator);
  }
  d->AddMember("histories", histories, *allocator);
  Value messages(kArrayType);
  for (auto &message : this->messages) {
    Value messageValue;
    messageValue.SetString(message.c_str(), *allocator);
    messages.PushBack(messageValue, *allocator);
  }
  d->AddMember("messages", messages, *allocator);
}

void HistoryMarker::loadFromJson(const Value &d) {
  org_identifier = d["org_identifier"].GetString();
  user_code_scope_id = d["user_code_scope_id"].GetString();
  applied_index = d["applied_index"].GetInt();
  children_num = d["children_num"].GetInt();
  propagate = d["propagate"].GetInt();
  no_hierarchy = d["no_hierarchy"].GetInt();
  qor_merge_mode = d["qor_merge_mode"].GetString();
  parallel = (d["parallel"].GetInt() > 0);
  this->histories.clear();
  const Value &histories = d["histories"];
  for (SizeType i = 0; i < histories.Size(); ++i) {
    HistoryEntry entry;
    entry.loadFromJson(histories[i]);
    this->histories.push_back(entry);
  }
  this->messages.clear();
  const Value &messages = d["messages"];
  for (SizeType i = 0; i < messages.Size(); ++i) {
    this->messages.push_back(messages[i].GetString());
  }
}
