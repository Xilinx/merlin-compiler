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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_HISTORY_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_HISTORY_H_

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "locations.h"
#include "mars_opt.h"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

#define HIS_CHECK 1

#if 0  //  control history log
#define HIS_WARNING(x)                                                         \
  cout << "[HIS WARNING][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define HIS_WARNING(x)
#endif

#if 0
#define HIS_ERROR(x)                                                           \
  if (!g_muteHISError) {                                                       \
    cerr << "[HIS ERROR][" << __func__ << ", " << __LINE__ << "]" << x         \
         << endl;                                                              \
  } else {                                                                     \
    cerr << "[HIS WARNING][" << __func__ << ", " << __LINE__ << "]" << x       \
         << endl;                                                              \
  }

#define HIS_ERROR_COND(cond, x)                                                \
  if (!g_muteHISError && cond) {                                               \
    cerr << "[HIS ERROR][" << __func__ << ", " << __LINE__ << "]" << x         \
         << endl;                                                              \
  } else {                                                                     \
    cerr << "[HIS WARNING][" << __func__ << ", " << __LINE__ << "]" << x       \
         << endl;                                                              \
  }
#else
#define HIS_ERROR(x)
#define HIS_ERROR_COND(cond, x)
#endif

int getIndex(SgNode *node);

class CSageCodeGen;

enum Action { INSERT, DELETE, COPY, BUILD };

class HistoryEntry {  //  all history are noted in their parents
 public:
  HistoryEntry(Action action, const int loc) : action(action), location(loc) {}

  std::string toString();

  Action getAction() { return this->action; }
  const Action getAction() const { return this->action; }

  int getLocation() { return this->location; }

  HistoryEntry() : action(INSERT), location(-1) {}

  void dumpToJson(rapidjson::Value *value,
                  rapidjson::Document::AllocatorType *allocator) const;

  void loadFromJson(const rapidjson::Value &value);

 private:
  Action action;
  int location;  //  topological location: the index of its parent
};

class HistoryMarker {
 public:
  HistoryMarker(Action action, const int &loc)
      : org_identifier(""), histories({HistoryEntry(action, loc)}),
        applied_index(-1), children_num(-1), propagate(1), no_hierarchy(0),
        qor_merge_mode(""), parallel(false) {}
  HistoryMarker()
      : org_identifier(""), applied_index(-1), children_num(-1), propagate(1),
        no_hierarchy(0), qor_merge_mode(""), parallel(false) {}

  std::vector<HistoryEntry> &getHistories() { return histories; }
  const std::vector<HistoryEntry> &getHistories() const { return histories; }
  std::vector<std::string> &getMessages() { return messages; }

  std::string &getOrgIdentifier() { return this->org_identifier; }

  void setOrgIdentifier(const std::string &org_identifier) {
    this->org_identifier = org_identifier;
  }
  void setParallel(bool isParallel) { this->parallel = isParallel; }

  int64_t getAppliedIndex() { return this->applied_index; }

  void setAllApplied() { this->applied_index = this->histories.size() - 1; }
  void increaseAppliedNum(size_t num);

  int64_t getChildrenNum() { return this->children_num; }

  void setChildrenNum(size_t children_num) {
    this->children_num = children_num;
  }

  std::string toString(std::string leadingString);

  void dumpToJson(rapidjson::Value *d,
                  rapidjson::Document::AllocatorType *allocator) const;

  void loadFromJson(const rapidjson::Value &d);

  void setUserCodeScopeId(std::string userCodeScopeId) {
    this->user_code_scope_id = userCodeScopeId;
  }

  std::string getUserCodeScopeId() const { return this->user_code_scope_id; }

  void setPropagate(int isPropagate) { this->propagate = isPropagate; }

  int getPropagate() const { return this->propagate; }

  void setNoHierarchy(int isNoHierarchy) { this->no_hierarchy = isNoHierarchy; }

  int getNoHierarchy() const { return this->no_hierarchy; }

  void setQoRMergeMode(std::string qorMergeMode) {
    this->qor_merge_mode = qorMergeMode;
  }

  std::string getQoRMergeMode() const { return this->qor_merge_mode; }

  bool getParallel() const { return this->parallel; }

 private:
  std::string org_identifier;  //  position std::string or “created_#”
  std::string user_code_scope_id;
  std::vector<HistoryEntry> histories;
  std::vector<std::string> messages;
  int64_t applied_index;
  int64_t children_num;
  int propagate;     //  the children will share the same org_identifier with it
  int no_hierarchy;  //  passed to Shan, not used now according to his opinion
  std::string qor_merge_mode;  //  set major for minor for node when lc has
                               //  multiple nodes corresponding to user node if
                               //  set major the node, the user node cycle will
                               //  be the cycle of this node
  bool parallel;
};

/******************************************************************************/
/* Helper functions ***********************************************************/
/******************************************************************************/

std::string getParent(const std::string &id);
std::string printNodeInfo(const CSageCodeGen *ast, void *node, int len = 80);

/******************************************************************************/
/* History APIs ***************************************************************/
/******************************************************************************/

bool isOptPass(const std::string &passName);

void markInsert(CSageCodeGen *ast, SgNode *, const int loc,
                const std::string &debugInfo = "");
void markDelete(CSageCodeGen *ast, SgNode *, const int loc,
                const std::string &debugInfo = "",
                SgNode *removeNode = nullptr);
void markCopy(CSageCodeGen *ast, SgNode *, SgNode *,
              const std::string &debugInfo = "", const bool passMajor = false);
void markBuild(CSageCodeGen *ast, void *, const std::string &debugInfo = "",
               void *bindNode = nullptr);
void markIncludeFile(CSageCodeGen *ast, SgNode *scope, std::string filename,
                     int before);
void markIncludeMacro(CSageCodeGen *ast, SgNode *scope, std::string macro,
                      int before);
void markIncludeExpand(CSageCodeGen *ast);

void setNoHierarchy(CSageCodeGen *ast, void *node_, int isNoHierarchy);
void setQoRMergeMode(CSageCodeGen *ast, void *node_, std::string qorMergeMode,
                     bool isRecursivelySet = false);
void setParallel(CSageCodeGen *ast, void *node_, bool isParallel,
                 bool isRecursivelySet = false);
std::string bindNodeOrgId(CSageCodeGen *ast, void *node_, void *bindNode,
                          bool force = false);
void insertMessage(CSageCodeGen *ast, void *node,
                   const std::map<std::string, std::string> &msg);

std::string
getOriginalTopoLocation(CSageCodeGen *ast,
                        std::map<SgNode *, HistoryMarker> *histories,
                        std::string topoLocation, bool ignoreError = false);
std::string
getOriginalTopoLocation(std::map<std::string, HistoryMarker> *histories,
                        std::string key, bool ignoreError = false);

void historyModuleInit(CSageCodeGen *ast, const std::string &passName);
std::map<SgNode *, std::string> saveOrgId(CSageCodeGen *ast,
                                          std::string passName);
void checkReferenceInduction(CSageCodeGen *ast,
                             const std::map<SgNode *, std::string> &orgIdMap,
                             std::string passName);
std::map<std::string, std::map<std::string, std::string>>
saveForInterPasses(CSageCodeGen *ast, std::string passName,
                   bool isForNextPass = false);
void checkBetweenPasses(
    CSageCodeGen *ast,
    const std::map<std::string, std::map<std::string, std::string>>
        &inputReferReport);

/******************************************************************************/
/* Json API *******************************************************************/
/******************************************************************************/

std::map<std::string, HistoryMarker>
readHistoriesFromJson(const std::string histories_url);
void writeHistoriesToJson(const std::string histories_url,
                          const std::map<std::string, HistoryMarker> &);
std::map<std::string, HistoryMarker>
getSerializableHistories(CSageCodeGen *ast,
                         std::map<SgNode *, HistoryMarker> *);

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_HISTORY_H_
