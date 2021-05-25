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
#include <vector>

using std::vector;

#include "cmdline_parser.h"
#include "codegen.h"

//  / test objectives
#include "history.h"
#include "locations.h"

#if 0  //  control message log
#define MSG_INFO(x) cout << "[MSG TEST][INFO] " << x << endl;
#define MSG_WARNING(x) cout << "[MSG TEST][WARNING] " << x << endl;
#else
#define MSG_INFO(x)
#define MSG_WARNING(x)
#endif

#define MSG_ERROR(x) cerr << "[MSG TEST][ERROR] " << (x) << endl;

#define MSG_RESULT(x) cout << "[MSG TEST][RESULT] " << (x) << endl;
#define MSG_RESULT_WARNING(x)                                                  \
  cout << "[MSG TEST][RESULT][WARNING] " << (x) << endl;
#define MSG_RESULT_ERROR(x) cout << "[MSG TEST][RESULT][ERROR] " << (x) << endl;

int msg_locations_test(CSageCodeGen *ast, void *pTopFunc,
                       const CInputOptions &options) {
  MSG_INFO("Locations Test Start!");

  vector<void *> functions;
  ast->GetNodesByType(NULL, "preorder", "SgFunctionDeclaration", &functions,
                      true);
  vector<void *> statements;
  ast->GetNodesByType(NULL, "preorder", "SgStatement", &statements, true);

  vector<void *> testElements(functions);
  testElements.insert(testElements.end(), statements.begin(), statements.end());

  //  start from ast node
  MSG_RESULT(std::to_string(testElements.size()) + " entries will be checked.");
  for (size_t i = 0; i < testElements.size(); i++) {
    MSG_RESULT("== the " + std::to_string(i) + "-th check. ===========");
    SgNode *node = static_cast<SgNode *>(testElements[i]);
    if (isSkipped(ast, node)) {
      MSG_RESULT("skip an included node.");
      continue;
    }

    if (node == NULL) {
      MSG_RESULT("skip an empty node.");
      continue;
    }

    if (ast->is_floating_node(node) != 0) {
      MSG_RESULT("skip a floating node.");
      continue;
    }

    MSG_RESULT("Node: " + ast->_p(node));

    //  1. ast node <--> TopoLocation
    MSG_RESULT("1. ast node <--> topo "
               "location-------------------------------------------------");
    string topoLocation1 = getTopoLocation(ast, node);
    MSG_RESULT("TopoLocation: " + topoLocation1);
    SgNode *node1 = getSgNode(ast, topoLocation1);
    if (node == node1) {
      MSG_RESULT("CORRECT!");
    } else {
      MSG_RESULT_ERROR("NOT EQUAL: somewhere wrong!");
      MSG_RESULT_ERROR("New TopoLocation: " + getTopoLocation(ast, node1));
      continue;
    }

    //  //  2. ast node --> TopoLocation --> FileLocation --> ast node
    //  MSG_RESULT("2. ast node --> topo location --> file loation --> ast node
    //  "
    //            "------------------");
    //  string fileLocation2 = getFileLocation(ast, topoLocation1);
    //  MSG_RESULT("FileLocation: " + fileLocation2);
    //  SgNode *node2 = getSgNode(ast, fileLocation2, V_SgFunctionDeclaration);
    //  if (node == node2) {
    //   MSG_RESULT("CORRECT!");
    //  } else {
    //   MSG_RESULT_WARNING("NOT EQUAL: somewhere wrong!");
    //   MSG_RESULT_WARNING("New FileLocation: " + getFileLocation(ast,
    //   node2)); continue;
    //  }

    //  //  3. ast node <--> FileLocation
    //  MSG_RESULT("3. ast node <--> file "
    //            "location-------------------------------------------------");
    //  string fileLocation3 = getFileLocation(ast, node);
    //  MSG_RESULT("FileLocation: " + fileLocation3);
    //  MSG_INFO("Search ast node for type " +
    //  to_string(V_SgFunctionDeclaration)); SgNode *node3 = getSgNode(ast,
    //  fileLocation3, V_SgFunctionDeclaration); if (node == node3) {
    //   MSG_RESULT("CORRECT!");
    //  } else {
    //   MSG_RESULT_WARNING("NOT EQUAL: somewhere wrong!");
    //   MSG_RESULT_WARNING("New FileLocation: " + getFileLocation(ast,
    //   node3)); continue;
    //  }

    //  //  4. ast node --> FileLocation --> TopoLocation --> ast node
    //  MSG_RESULT("4. ast node --> file location --> topo loation --> ast node
    //  "
    //            "------------------");
    //  string topoLocation4 =
    //     getTopoLocation(ast, fileLocation3, V_SgFunctionDeclaration);
    //  MSG_RESULT("TopoLocation: " + topoLocation4);
    //  SgNode *node4 = getSgNode(ast, topoLocation4);
    //  if (node == node4) {
    //   MSG_RESULT("CORRECT!");
    //  } else {
    //   MSG_RESULT_ERROR("NOT EQUAL: somewhere wrong!");
    //   MSG_RESULT_ERROR("New TopoLocation: " + getTopoLocation(ast, node4));
    //   continue;
    //  }
  }

  MSG_INFO("Locations Test End!");

  return 0;
}

int msg_history_mark_test(CSageCodeGen *ast, void *pTopFunc,
                          const CInputOptions &options) {
  MSG_INFO("History Mark Start!");

  vector<void *> functions;
  ast->GetNodesByType(NULL, "preorder", "SgFunctionDeclaration", &functions);

  //  start from ast node
  for (size_t i = 0; i < functions.size(); i++) {
    SgNode *node = static_cast<SgNode *>(functions[i]);
    if (isSkipped(ast, node)) {
      continue;
    }

    //  1. Insert
    MSG_INFO("1. INSERT ------------"
             "---------------------------------------------------------");
    markInsert(ast, node, 8, "Insert ...");

    //  2. Delete
    MSG_INFO("2. DELETE ------------"
             "---------------------------------------------------------");
    markDelete(ast, node, 8, "Delete ...");

    //  3. Build
    MSG_INFO("3. BUILD -------------"
             "---------------------------------------------------------");
    markBuild(ast, node, "Build ...");
  }

  MSG_RESULT("History entry number: " +
             std::to_string(ast->getHistories()->size()));
  map<string, HistoryMarker> serializableHistories =
      getSerializableHistories(ast, ast->getHistories());
  MSG_RESULT("Serializable history entry number: " +
             std::to_string(ast->getHistories()->size()));

  //  Json API test
  writeHistoriesToJson("json_test.json", serializableHistories);
  map<string, HistoryMarker> serializableHistories1 =
      readHistoriesFromJson("json_test.json");
  writeHistoriesToJson("json_test_1.json", serializableHistories);

  for (auto &ele : *ast->getHistories()) {
    MSG_RESULT("==============================================================="
               "=================");
    SgNode *node = ele.first;
    string topoLocation = getTopoLocation(ast, node);
    MSG_RESULT(topoLocation);

    HistoryMarker &marker = ele.second;
    string markerString = marker.toString("[MSG TEST][RESULT] ");
    cout << markerString << endl;

    //  check the serializableHistories
    auto entryIt = serializableHistories.find(topoLocation);
    if (entryIt == serializableHistories.end()) {
      MSG_RESULT_ERROR(topoLocation + " is missing in serializable histories!");
    } else if (markerString !=
               entryIt->second.toString("[MSG TEST][RESULT] ")) {
      MSG_RESULT_ERROR("Content of " + topoLocation +
                       " in serializable histories is not correct.");
      cout << entryIt->second.toString("[MSG TEST][RESULT][WRONG] ") << endl;
    } else {
      MSG_RESULT("Correctly get serializable histories.");
    }
  }

  MSG_INFO("History Mark End!");

  return 0;
}

int msg_test_top(CSageCodeGen *ast, void *pTopFunc,
                 const CInputOptions &options) {
  msg_locations_test(ast, pTopFunc, options);
  //  msg_history_mark_test(ast, pTopFunc, options);
  return 0;
}
