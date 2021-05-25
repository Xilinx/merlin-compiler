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


#include "id_update.h"

#include <iostream>
#include <unordered_map>
#include "cmdline_parser.h"
#include "codegen.h"
#include "history.h"
#include "locations.h"
#include "report.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"

using rapidjson::Document;
using rapidjson::PrettyWriter;
using rapidjson::StringBuffer;
using rapidjson::Value;
#if 0  //  control message log
#define IDU_INFO(x)                                                            \
  cout << "[IDU INFO][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define IDU_INFO(x)
#endif

#if 0  //  control message log
#define IDU_ALGO(x)                                                            \
  cout << "[IDU ALGO][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#define IDU_WARNING(x)                                                         \
  cout << "[IDU WARNING][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define IDU_ALGO(x)
#define IDU_WARNING(x)
#endif

#define IDU_ERROR(x)                                                           \
  if (!g_muteHISError) {                                                       \
    cout << "[IDU ERROR][" << __func__ << ", " << __LINE__ << "]" << (x)       \
         << endl;                                                              \
  } else {                                                                     \
    cout << "[IDU WARNING][" << __func__ << ", " << __LINE__ << "]" << (x)     \
         << endl;                                                              \
  }

#define IDU_ERROR_COND(cond, x)                                                \
  if (!g_muteHISError && (cond)) {                                             \
    cout << "[IDU ERROR][" << __func__ << ", " << __LINE__ << "]" << (x)       \
         << endl;                                                              \
  } else {                                                                     \
    IDU_WARNING(x)                                                             \
  }

#define IDU_RESULT(x)                                                          \
  cout << "[IDU RESULT][" << __func__ << ", " << __LINE__ << "]" << (x) << endl;
#define IDU_RESULT_WARNING(x)                                                  \
  cout << "[IDU RESULT][WARNING][" << __func__ << ", " << __LINE__ << "]"      \
       << (x) << endl;
#define IDU_RESULT_ERROR(x)                                                    \
  cout << "[IDU RESULT][ERROR][" << __func__ << ", " << __LINE__ << "]" << (x) \
       << endl;

extern map<string, map<string, string>> g_userCodeFileInfo;
extern vector<map<string, HistoryMarker>> allHistoryList;
extern vector<string> passNames;
extern char g_passName[MAX_PASS_LENGTH];

inline bool isValidId(const string &id) {
  return !(id.empty() || isStartWith(id, "Built"));
}

inline bool isInterestedType(const string &type) {
  if (  //  ! type.compare("function") ||
      (type == "kernel") || (type == "callfunction") || (type == "loop")) {
    return true;
  }
  return false;
}
#if 0
inline bool exists_test(const string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}
#endif

vector<map<string, HistoryMarker>> loadHistories(const string &historyListLoc,
                                                 vector<string> *p_historyNames,
                                                 const string &historyLocPfx) {
  //  open history list file
  std::ifstream fHistoryList;
  fHistoryList.open(historyListLoc);

  //  load the histories
  string history_url;
  vector<map<string, HistoryMarker>> historyList;
  while (fHistoryList >> history_url) {
    history_url = historyLocPfx + "history_" + history_url + ".json";
    IDU_INFO("load: " + history_url);
    if (test_file_for_read(history_url) == 0) {
      IDU_ERROR("file " + history_url + " not found");
      return historyList;
    }
    p_historyNames->push_back(history_url);
    historyList.push_back(readHistoriesFromJson(history_url));
  }
  fHistoryList.close();

  return historyList;
}

string getKey(string id) {
  if (isValidId(id)) {
    id[0] = 'X';
  }
  return id;
}

bool getInfo(const map<string, HistoryMarker> &histories, string id, string key,
             int *value) {
  auto markerIt = histories.find(id);
  if (markerIt == histories.end()) {
    return false;
  }

  HistoryMarker marker = markerIt->second;

  if (key == "no_hierarchy") {
    if (marker.getNoHierarchy() == 1) {
      IDU_ALGO("find no_hierarchy: " + std::to_string(marker.getNoHierarchy()));
      *value = marker.getNoHierarchy();
      return true;
    }
    return false;
  }
  IDU_ERROR("should not be here");

  return false;
}

bool isCreated(const map<string, HistoryMarker> &histories, string key) {
  IDU_INFO("is this node created: " + key);
  do {
    auto marker = histories.find(key);
    if (marker != histories.end()) {
      auto &actions = marker->second.getHistories();
      if (!actions.empty() &&
          (actions[0].getAction() == COPY || actions[0].getAction() == BUILD)) {
        IDU_ALGO("this id is created: " + key);
        return true;
      }
      IDU_ALGO("this id is not created: " + key);
      if (!actions.empty()) {
        //  IDU_ALGO("actions: \n" + actions[0].toString())
      }
    }
    key = getParent(key);
  } while (!key.empty());
  return false;
}

bool getInfo(const map<string, HistoryMarker> &histories, string id, string key,
             string *value) {
  auto markerIt = histories.find(id);
  if (markerIt == histories.end()) {
    if ((key == "qor_merge_mode") && isCreated(histories, id)) {
      IDU_ALGO("stop to find qor_merge_mode");
      return true;
    }
    return false;
  }

  HistoryMarker marker = markerIt->second;

  if (key == "user_code_scope_id") {
    if (value->empty() && !marker.getUserCodeScopeId().empty()) {
      IDU_ALGO("find user code scope id: " + marker.getUserCodeScopeId());
      *value = marker.getUserCodeScopeId();
      return true;
    }
  } else if (key == "qor_merge_mode") {
    IDU_ALGO("get info for key: " + id);
    if (value->empty() && !marker.getQoRMergeMode().empty()) {
      IDU_ALGO("find qor_merge_mode: " + marker.getQoRMergeMode());
      *value = marker.getQoRMergeMode();
      return true;
    }
    if (value->empty() && isCreated(histories, id)) {
      IDU_ALGO("stop to find qor_merge_mode: " + marker.getQoRMergeMode());
      return true;
    }
  } else {
    IDU_ERROR("should not be here");
  }

  return false;
}

bool getInfo(const map<string, HistoryMarker> &histories, string id, string key,
             bool *value) {
  auto markerIt = histories.find(id);
  if (markerIt == histories.end()) {
    return false;
  }

  HistoryMarker marker = markerIt->second;
  IDU_ALGO("trace parallel with: " + id);

  if (key == "parallel") {
    if (marker.getParallel()) {
      IDU_ALGO("stop trace parallel with: " + id);
      *value = marker.getParallel();
      return true;
    }
  } else {
    IDU_ERROR("should not be here");
  }

  return false;
}

template <typename T>
void getUserCodeId(CSageCodeGen *ast,
                   vector<map<string, HistoryMarker>> *p_historyList,
                   const vector<string> &historyNames, string id, string key,
                   T *value) {
  bool stop = false;

  for (auto it = p_historyList->rbegin(); it != p_historyList->rend(); ++it) {
    string hName =
        historyNames[historyNames.size() - 1 - (it - p_historyList->rbegin())];
    if ("none" == hName) {
      continue;
    }
    IDU_ALGO("id before: " + id);
    IDU_ALGO("calc with history: " + hName);
    if (!isValidId(id)) {
      IDU_WARNING("calculate to a invalid key, stop trace");
      return;
    }

    if (!stop) {
      stop = getInfo(*it, getKey(id), key, value);
    } else {
      IDU_ALGO("stop to trace");
    }

    id = getOriginalTopoLocation(&*it, id);
  }
}

string getQoRMergeMode(CSageCodeGen *ast, void *node) {
  string id = getTopoLocation(ast, static_cast<SgNode *>(node));
  string orgId = getOriginalTopoLocation(ast, ast->getHistories(), id);

  string qorMergeMode;
  getUserCodeId(ast, &allHistoryList, passNames, orgId, "qor_merge_mode",
                &qorMergeMode);
  IDU_ALGO("get qor_merge_mode: " + qorMergeMode);
  return qorMergeMode;
}

string getUserCodeScopeIdFromHistory(CSageCodeGen *ast, void *node_) {
  auto *histories = ast->getHistories();
  SgNode *node = static_cast<SgNode *>(node_);

  if (histories->find(node) != histories->end() &&
      !(*histories)[node].getUserCodeScopeId().empty()) {
    return (*histories)[node].getUserCodeScopeId();
  }
  auto id = getTopoLocation(ast, node);
  id = getOriginalTopoLocation(ast, ast->getHistories(), id, true);
  string userCodeScopeId;
  getUserCodeId(ast, &allHistoryList, passNames, id, "user_code_scope_id",
                &userCodeScopeId);
  return userCodeScopeId;
}

string getUserCodeId(CSageCodeGen *ast,
                     vector<map<string, HistoryMarker>> *p_historyList,
                     const vector<string> &historyNames, string id, string type,
                     vector<string> *messages, bool ignoreError) {
  if (messages != nullptr && !messages->empty()) {
    IDU_ERROR("initial message is not empty.");
    messages = nullptr;
  }

  //  calculate original id
  IDU_ALGO("calculate id: " + id);
  string oid = id;
  for (auto it = p_historyList->rbegin(); it != p_historyList->rend(); ++it) {
    string key = id;
    if (isStartWith(key, "Built")) {
      IDU_ERROR_COND(!ignoreError,
                     "calculate user code id for built node: " + key);
      return "";
    }
    if (!key.empty()) {
      key[0] = 'X';
    }
    auto markerIt = it->find(key);
    string nodeInfo;
    if (markerIt != it->end()) {
      vector<string> &curMessage = markerIt->second.getMessages();
      nodeInfo = curMessage.back();
      if (messages != nullptr) {
        messages->insert(messages->end(), curMessage.begin(), curMessage.end());
      }
    }
    string hName =
        historyNames[historyNames.size() - 1 - (it - p_historyList->rbegin())];
    if ("none" == hName) {
      continue;
    }
    IDU_ALGO("calculate with history: " + hName);
    auto org_id = getOriginalTopoLocation(&*it, id);
    if (!org_id.empty())
      id = org_id;
    IDU_ALGO("result is: " + id);
    if (!isValidId(id)) {
      if (isInterestedType(type)) {
        IDU_ERROR("missing node " + id + " is built");
        if (nodeInfo.empty()) {
          IDU_ERROR("missing node info: it is a child of a built node.");
          do {
            key = getParent(key);
            if (key.empty()) {
              HIS_ERROR("cannot find its parent in history");
              break;
            }
            if (it->find(key) != it->end()) {
              nodeInfo = (*it)[key].getMessages().back();
              break;
            }
          } while (nodeInfo.empty());
        }
        IDU_ERROR("missing node info: " + nodeInfo);
      } else {
        IDU_INFO("ignore type (" + type + "): " + id);
      }

      if (type == "function") {
        string userCodeScopeId;
        getUserCodeId(ast, p_historyList, historyNames, oid,
                      "user_code_scope_id", &userCodeScopeId);
        if (userCodeScopeId.empty()) {
          IDU_ERROR("missing scope id for built function " + id);
          IDU_ERROR("function info: " + nodeInfo);
        }
      }

      return id;
    }
  }

  return id;
}

string getUserCodeId(CSageCodeGen *ast, void *node,
                     bool ignoreError /*= false*/) {
  string id = getTopoLocation(ast, static_cast<SgNode *>(node));
  string orgId = getOriginalTopoLocation(ast, ast->getHistories(), id);
  string userCodeId = getUserCodeId(ast, &allHistoryList, passNames, orgId, "",
                                    nullptr, ignoreError);
  return userCodeId;
}

string getUserCodeScopeId(CSageCodeGen *ast, void *node) {
  if ((ast->is_floating_node(node) != 0) || ast->GetParent(node) == nullptr) {
    IDU_ERROR("calculate user code scope id for floating or top level node.");
    return "";
  }

  IDU_ALGO("start to calc user code scope id for " +
           printNodeInfo(ast, node, 80));

  string userCodeScopeId = getUserCodeScopeIdFromHistory(ast, node);
  if (!userCodeScopeId.empty()) {
    IDU_ALGO("directly get user code scope id from history: " +
             userCodeScopeId);
    return userCodeScopeId;
  }

  SgNode *scopeNode = static_cast<SgNode *>(
      ast->TraceUpByType(ast->GetParent(node), "SgForStatement"));
  if (scopeNode == nullptr) {
    scopeNode = static_cast<SgNode *>(
        ast->TraceUpByType(ast->GetParent(node), "SgFunctionDeclaration"));
    if (scopeNode == nullptr) {
      IDU_ERROR("the node is not in a function");
      return "";
    }
    IDU_ALGO("try its parent function: " + printNodeInfo(ast, scopeNode, 80));
    userCodeScopeId = getUserCodeId(ast, scopeNode, true);
    if (!isValidId(userCodeScopeId)) {
      userCodeScopeId = getUserCodeScopeIdFromHistory(ast, scopeNode);
    }
    IDU_ALGO("user code scope from parent function declaration is " +
             userCodeScopeId);
  } else {
    IDU_ALGO("try its parent loop: " + printNodeInfo(ast, scopeNode, 80));
    userCodeScopeId = getUserCodeId(ast, scopeNode, true);
    if (!isValidId(userCodeScopeId)) {
      userCodeScopeId = getUserCodeScopeIdFromHistory(ast, scopeNode);
      if (userCodeScopeId.empty()) {
        userCodeScopeId = getUserCodeScopeId(ast, scopeNode);
      }
    }
    IDU_ALGO("user code scope from parent loop is " + userCodeScopeId);
  }

  return userCodeScopeId;
}

void setFuncDeclUserCodeScopeId(CSageCodeGen *ast, void *funcDecl,
                                void *funcCall) {
  IDU_ALGO("start to set user code scope id");
  string userCodeScopeId = getUserCodeScopeId(ast, funcCall);
  if (!isValidId(userCodeScopeId)) {
    IDU_ERROR("user code scope id is not valid: " + userCodeScopeId);
    IDU_ERROR("function decl: " + printNodeInfo(ast, funcDecl, 80));
    IDU_ERROR("function call: " + printNodeInfo(ast, funcCall, 80));
  }

  auto *histories = ast->getHistories();
  if (histories->find(static_cast<SgNode *>(funcDecl)) == histories->end()) {
    HistoryMarker marker;
    histories->insert(
        pair<SgNode *, HistoryMarker>(static_cast<SgNode *>(funcDecl), marker));
  }
  IDU_ALGO("update user code scope id to " + userCodeScopeId);
  (*histories)[static_cast<SgNode *>(funcDecl)].setUserCodeScopeId(
      userCodeScopeId);
}

bool isGeneratedByMerlin(string filename) {
  vector<string> generated = {"cmost.h", "altera_const.h",
                              "rose_edg_required_macros_and_functions.h"};

  for (auto &file : generated) {
    if (filename == file) {
      return true;
    }
  }
  return false;
}

string getUserCodeFileLocation(CSageCodeGen *ast, void *node,
                               bool include_brace) {
  if (!isOptPass(g_passName) ||
      (((static_cast<SgNode *>(node)->get_file_info()) != nullptr) &&
       ast->isWithInHeaderFile(node))) {
    IDU_WARNING("not calling getUserCodeFileLocation in merlin_flow or the "
                "node is in a header file.");
    IDU_WARNING("return file location in the beginning of the pass instead.");

    if ((static_cast<SgNode *>(node)->get_file_info()) == nullptr) {
      IDU_WARNING("the file info is nullptr");
      return "";
    }

    if (isGeneratedByMerlin(ast->GetFileInfo_filename(node, 1))) {
      return "";
    }
    string file_name = ast->GetFileInfo_filename(node, 1);
    void *non_compiler_generated_node = nullptr;
    if (file_name.empty() || "NULL_FILE" == file_name ||
        ast->IsCompilerGenerated(node)) {
      void *curr = node;
      while (curr != nullptr) {
        if (!ast->IsCompilerGenerated(curr)) {
          if (non_compiler_generated_node == nullptr) {
            non_compiler_generated_node = curr;
          }
        } else {
          non_compiler_generated_node = nullptr;
        }
        curr = ast->GetParent(curr);
      }
    } else {
      non_compiler_generated_node = node;
    }
    if (non_compiler_generated_node == nullptr) {
      return "";
    }
    string location =
        ast->GetFileInfo_filename(non_compiler_generated_node, 1) + ":" +
        std::to_string(ast->GetFileInfo_line(non_compiler_generated_node));
    if (include_brace) {
      location = "(" + location + ")";
    }
    return location;
  }

  // if ((ast->IsFunctionDeclaration(node) == 0) &&
  //     (ast->IsStatement(node) == 0) && (ast->IsInitName(node) == 0) &&
  //     (ast->IsFunctionCall(node) == 0)) {
  //   node = ast->TraceUpToStatement(node);
  // }
  UCodeInfo info = getUserCodeInfo(ast, node);
  if (info.file_location.empty()) {
    return "";
  }
  string location = info.file_location;
  if (include_brace) {
    location = "(" + location + ")";
  }
  return location;
}

UCodeInfo getUserCodeInfo(CSageCodeGen *cg, void *node) {
  if (!isOptPass(g_passName) ||
      (((static_cast<SgNode *>(node)->get_file_info()) != nullptr) &&
       cg->isWithInHeaderFile(node))) {
    IDU_WARNING("not calling getUserCodeFileLocation in merlin_flow or the "
                "node is in a header file.");
    IDU_WARNING("return file location in the beginning of the pass instead.");

    if ((static_cast<SgNode *>(node)->get_file_info()) == nullptr) {
      IDU_WARNING("the file info is nullptr");
      return UCodeInfo();
    }

    if (isGeneratedByMerlin(cg->GetFileInfo_filename(node, 1))) {
      return UCodeInfo();
    }

    if (cg->IsCompilerGenerated(node)) {
      const string &type = static_cast<SgNode *>(node)->class_name();
      if (type == "SgTemplateVariableDeclaration") {
        void *inst = cg->TraceUpByType(node, "SgTemplateInstantiationDecl");
        vector<void *> vec;
        cg->GetNodesByType(cg->GetTemplateClassDecl(inst), "preorder",
                           "SgTemplateVariableDeclaration", &vec);
        node = *std::find_if(vec.begin(), vec.end(), [&](void *n) {
          return cg->GetVariableName(n) == cg->GetVariableName(node);
        });
      } else if (type == "SgTemplateInstantiationDecl") {
        node = cg->GetTemplateClassDecl(node);
      } else if (type == "SgTemplateInstantiationFunctionDecl") {
        node = cg->GetTemplateFuncDecl(node);
      }
    }

    string file_name = cg->GetFileInfo_filename(node, 1);
    void *non_compiler_generated_node = nullptr;
    if (file_name.empty() || "NULL_FILE" == file_name ||
        cg->IsCompilerGenerated(node)) {
      void *curr = node;
      while (curr != nullptr) {
        if (!cg->IsCompilerGenerated(curr)) {
          if (non_compiler_generated_node == nullptr) {
            non_compiler_generated_node = curr;
          }
        } else {
          non_compiler_generated_node = nullptr;
        }
        curr = cg->GetParent(curr);
      }
    } else {
      non_compiler_generated_node = node;
    }
    if (non_compiler_generated_node == nullptr) {
      return UCodeInfo();
    }

    std::stringstream loc;
    loc << cg->GetFileInfo_filename(non_compiler_generated_node, 1) << ":"
        << cg->GetFileInfo_line(non_compiler_generated_node);
    std::stringstream unparse;
    if (cg->IsFunctionDeclaration(non_compiler_generated_node)) {
      unparse << cg->GetFuncName(non_compiler_generated_node);
    } else if (cg->IsInitName(non_compiler_generated_node) ||
               cg->IsVariableDecl(non_compiler_generated_node)) {
      unparse << cg->GetVariableName(non_compiler_generated_node);
    } else if (cg->IsClassDecl(non_compiler_generated_node)) {
      unparse << cg->GetClassName(non_compiler_generated_node);
    } else {
      unparse << cg->_up(non_compiler_generated_node);
    }
    return UCodeInfo(unparse.str(), loc.str());
  }

  // if ((cg->IsFunctionDeclaration(node) == 0) &&
  //     (cg->IsStatement(node) == 0) && (cg->IsInitName(node) == 0) &&
  //     (cg->IsFunctionCall(node) == 0)) {
  //   node = cg->TraceUpToStatement(node);
  // }

  static std::unordered_map<void *, string> cached;
  string userCodeId;
  auto map2info = [](map<string, string> &m) {
    return UCodeInfo(m["name"], m["file_location"]);
  };

  if (cached.count(node) > 0) {
    userCodeId = cached[node];
    return map2info(g_userCodeFileInfo[userCodeId]);
  }

  g_muteHISError = true;
  userCodeId = getUserCodeId(cg, node);
  g_muteHISError = false;

  if (!isValidId(userCodeId)) {
    IDU_WARNING("Invalid user code id, the node does not come from user code");
    return UCodeInfo();
  }

  for (userCodeId[0] = 'X'; isValidId(userCodeId);
       userCodeId = getParent(userCodeId)) {
    if (g_userCodeFileInfo.find(userCodeId) != g_userCodeFileInfo.end()) {
      cached[node] = userCodeId;
      return map2info(g_userCodeFileInfo[userCodeId]);
    }
  }
  IDU_ERROR("User code info not found for id: " + userCodeId);
  return UCodeInfo();
}

string getJsonString(const vector<string> &messages) {
  Document document;
  const char json[] = "[]";
  if (document.Parse(json).HasParseError()) {
    return "";
  }

  for (auto &str : messages) {
    Value msgEntry;
    msgEntry.SetString(str.c_str(), str.size(), document.GetAllocator());
    document.PushBack(msgEntry, document.GetAllocator());
  }

  StringBuffer SB;
  PrettyWriter<StringBuffer> writer(SB);
  document.Accept(writer);
  auto jsonString = SB.GetString();
  return jsonString;
}

int id_update_top(CSageCodeGen *ast, void *pTopFunc,
                  const CInputOptions &options) {
  IDU_INFO("Start to update id.");

  string historyListLoc = options.get_option_key_value("-a", "history_list");
  IDU_INFO("history list: " + historyListLoc);

  if (historyListLoc.empty()) {
    IDU_ERROR("empty history list.");
  }

  //  load histories
  vector<string> historyNames;
  vector<map<string, HistoryMarker>> historyList =
      loadHistories(historyListLoc, &historyNames);

  //  load inter report
  string json_url = options.get_option_key_value("-a", "inter_report");
  IDU_INFO("inter report: " + json_url);
  map<string, map<string, string>> read_inter_report;
  IDU_INFO("read inter report: start.");
  readInterReport(json_url, &read_inter_report);
  IDU_INFO("read inter report: done.");
  map<string, map<string, string>> inter_report;
  for (const auto &it : read_inter_report) {
    if (it.first.empty()) {
      continue;
    }
    inter_report[it.first] = it.second;
  }

  for (auto &entry : inter_report) {
    string id = entry.first;
    string type = entry.second["type"];

    //  Han ignore TOP_ ID
    if (isStartWith(id, "TOP_")) {
      continue;
    }

    vector<string> messages;
    string org_id = getUserCodeId(ast, &historyList, historyNames, id, type,
                                  &messages, false);
    entry.second["org_identifier"] = org_id;
    if (isInterestedType(type) && !isValidId(org_id)) {
      IDU_ERROR("invalid org id: " + org_id + ", please check missing node");
    }

    string qorMergeMode;
    getUserCodeId(ast, &historyList, historyNames, id, "qor_merge_mode",
                  &qorMergeMode);
    entry.second["qor_merge_mode"] = qorMergeMode;

    string messagesString = getJsonString(messages);
    entry.second["messages"] = messagesString;

    bool parallel = false;
    getUserCodeId(ast, &historyList, historyNames, id, "parallel", &parallel);
    entry.second["parallel"] = parallel ? "yes" : "no";
  }

  for (auto &entry : inter_report) {
    string scopeId = entry.first;
    string scopeLoopId;
    string scopeFuncId;
    string type = entry.second["type"];

    //  Han ignore TOP_ ID
    if (isStartWith(scopeId, "TOP_")) {
      continue;
    }

    if ((type == "function") || (type == "kernel")) {
      IDU_ALGO("do not calculate scope id for function declaration");
      entry.second["scope_org_identifier"] = "";
      continue;
    }
    IDU_ALGO("calculate scope id for " + scopeId);

  CALC_SCOPE_ID:
    do {
      scopeId = getParent(scopeId);
      if (scopeId.empty()) {
        break;
      }
      scopeLoopId = scopeId;
      scopeLoopId[0] = 'L';
      scopeFuncId = scopeId;
      scopeFuncId[0] = 'F';
    } while (!scopeId.empty() &&
             inter_report.find(scopeFuncId) == inter_report.end() &&
             inter_report.find(scopeLoopId) == inter_report.end());

    if (scopeId.empty()) {
      IDU_ERROR("its parent cannot find in intermediate report.");
    } else {
      if (inter_report.find(scopeFuncId) != inter_report.end()) {
        int noHierarchy = 0;
        getUserCodeId(ast, &historyList, historyNames, scopeFuncId,
                      "no_hierarchy", &noHierarchy);
        if (noHierarchy == 1) {
          entry.second["scope_org_identifier"] = "ignore";
          entry.second["hierarchy_mode"] = "ignored";
          inter_report[scopeFuncId]["hierarchy_mode"] = "ignore_sub_hierarchy";
          continue;
        }

        string funcOrgId = inter_report[scopeFuncId]["org_identifier"];
        string funcScopeOrgId;
        getUserCodeId(ast, &historyList, historyNames, scopeFuncId,
                      "user_code_scope_id", &funcScopeOrgId);
        if (isValidId(funcOrgId)) {
          IDU_ALGO("scope id from org id of parent function: " + funcOrgId);
          entry.second["scope_org_identifier"] = funcOrgId;
        } else if (isValidId(funcScopeOrgId)) {
          IDU_ALGO("scope id from scope id of parent function: " + funcOrgId);
          entry.second["scope_org_identifier"] = funcScopeOrgId;
        } else {
          IDU_ERROR("scope id error: one of org id " + funcOrgId +
                    " or scope id " + funcOrgId +
                    " should be not empty or built.");
          entry.second["scope_org_identifier"] = "";
        }
      } else if (inter_report.find(scopeLoopId) != inter_report.end()) {
        int noHierarchy = 0;
        getUserCodeId(ast, &historyList, historyNames, scopeLoopId,
                      "no_hierarchy", &noHierarchy);
        if (noHierarchy == 1) {
          entry.second["scope_org_identifier"] = "ignore";
          entry.second["hierarchy_mode"] = "ignored";
          inter_report[scopeLoopId]["hierarchy_mode"] = "ignore_sub_hierarchy";
          continue;
        }

        string loopOrgId = inter_report[scopeLoopId]["org_identifier"];
        IDU_ALGO("scope id from parent loop: " + loopOrgId);
        if (!isValidId(loopOrgId)) {
          goto CALC_SCOPE_ID;
        } else {
          entry.second["scope_org_identifier"] = loopOrgId;
        }
      } else {
        IDU_ERROR("scope id error: id (" + scopeId +
                  ") not found in intermediate report.");
        entry.second["scope_org_identifier"] = "";
      }
    }
  }

  //  dump the inter report
  IDU_INFO("write inter report: start.");
  writeInterReport("id_update_" + json_url, inter_report);
  IDU_INFO("write inter report: done.");

  return 0;
}
