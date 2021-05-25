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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ID_UPDATE_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ID_UPDATE_H_
#include <string>
#include <vector>
#include <map>
#include "history.h"

struct UCodeInfo {
  const string name;
  const string file_location;
  explicit UCodeInfo(const string &na = "", const string &loc = "")
      : name(na), file_location(loc) {}
};

std::vector<std::map<std::string, HistoryMarker>>
loadHistories(const std::string &historyListLoc,
              std::vector<std::string> *p_historyNames,
              const std::string &historyLocPrefix = "");
std::string getUserCodeId(CSageCodeGen *ast, void *node,
                          bool ignoreError = false);
std::string getUserCodeFileLocation(CSageCodeGen *ast, void *node,
                                    bool include_brace = false);
UCodeInfo getUserCodeInfo(CSageCodeGen *cg, void *node);
std::string getQoRMergeMode(CSageCodeGen *ast, void *node);
void setFuncDeclUserCodeScopeId(CSageCodeGen *ast, void *funcDecl,
                                void *funcCall);

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ID_UPDATE_H_
