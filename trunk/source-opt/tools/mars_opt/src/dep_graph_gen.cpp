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
#include <set>
#include <vector>

#include "file_parser.h"
#include "tldm_annotate.h"
extern vector<string> graph_execute_seq;
extern std::set<string> dump_array;
extern std::set<string> graph_names;
//  Edit by Peng
//  int dep_graph_gen(map<string, map<string, vector<poly_access_pattern> > >
//  mapTaskName2VarMatrix, map<string, string> mapTaskName2OrderVec)
int dep_graph_gen() {
  rebuild_parent_outer_level();
  rebuild_dep_graph("dep_pragma.c");
  return 1;
}

//  added by Hui
int io_cfg_gen() {
  FILE *cfg_out;
  cfg_out = fopen("io_cfg.xml", "w");
  size_t i;

  map<string, std::set<string>> mapGraph2PortIn;
  map<string, std::set<string>> mapGraph2PortOut;
  map<string, std::set<string>> mapGraph2PortAccess;

  map<string, std::set<string>> mapGraph2ArrayUsedLater;
  map<string, std::set<string>> mapGraph2ArrayUsedBefore;

  //  vector<CTldmDependenceAnn *> dep_vector = get_tldm_dependence_set();
  vector<CTldmTaskAnn *> task_vector = get_tldm_task_set();

  for (i = 0; i < task_vector.size(); i++) {
    CTldmTaskAnn *curr_task = task_vector[i];
    string curr_graph = curr_task->GetParentName();
    if (graph_names.count(curr_graph) == 0) {
      continue;
    }

    //  void* curr_graph = mapTask2Graph[curr_task];
    for (int j = 0; j < curr_task->GetPortInfoNum(); j++) {
      CXMLAnn_WrapperGen_port_info curr_port = curr_task->GetPortInfo(j);
      tldm_polyhedral_info poly_info = curr_port.poly_info;
      string io_type = poly_info.properties["access_pattern"];
      string port_type = poly_info.properties["port_type"];
      //  printf("%s access array %s\n", curr_graph.c_str(),
      //  poly_info.properties["name"].c_str());
      if (port_type == "iterator" || port_type == "param") {
        continue;
      }
      //  printf("port type %s\n", io_type.c_str());
      if (io_type[0] == 'r') {
        mapGraph2PortIn[curr_graph].insert(poly_info.properties["name"]);
        mapGraph2PortAccess[curr_graph].insert(poly_info.properties["name"]);
      }
      if (io_type[0] == 'w') {
        mapGraph2PortOut[curr_graph].insert(poly_info.properties["name"]);
        mapGraph2PortAccess[curr_graph].insert(poly_info.properties["name"]);
      }
    }
  }
  std::set<string> array_onboard;

  std::set<string> array_used;
  string graph_next = "undef";

  for (vector<string>::reverse_iterator mi = graph_execute_seq.rbegin();
       mi != graph_execute_seq.rend(); mi++) {
    if (graph_next != "undef") {
      array_used.insert(mapGraph2PortIn[graph_next].begin(),
                        mapGraph2PortIn[graph_next].end());
      array_used.insert(mapGraph2PortOut[graph_next].begin(),
                        mapGraph2PortOut[graph_next].end());
      mapGraph2ArrayUsedLater[*mi].insert(array_used.begin(), array_used.end());
    }
    graph_next = *mi;
  }

  string graph_prev = "undef";
  array_used.clear();
  for (vector<string>::iterator mi = graph_execute_seq.begin();
       mi != graph_execute_seq.end(); mi++) {
    if (graph_prev != "undef") {
      array_used.insert(mapGraph2PortIn[graph_prev].begin(),
                        mapGraph2PortIn[graph_prev].end());
      array_used.insert(mapGraph2PortOut[graph_prev].begin(),
                        mapGraph2PortOut[graph_prev].end());
      mapGraph2ArrayUsedBefore[*mi].insert(array_used.begin(),
                                           array_used.end());
    }
    graph_prev = *mi;
  }

  fprintf(cfg_out, "<io>\n");
  //  fprintf(cfg_out, "    <param_tree name = \"%s\">\n", curr_graph.c_str());
  for (vector<string>::iterator vi = graph_execute_seq.begin();
       vi != graph_execute_seq.end(); vi++) {
    string curr_graph = *vi;
    fprintf(cfg_out, "    <node graph_name = \"%s\">\n", curr_graph.c_str());

    for (std::set<string>::iterator mi =
             mapGraph2PortAccess[curr_graph].begin();
         mi != mapGraph2PortAccess[curr_graph].end(); mi++) {
      if (array_onboard.count(*mi) == 0 ||
          mapGraph2ArrayUsedBefore[curr_graph].count(*mi) == 0 ||
          mapGraph2ArrayUsedBefore.count(curr_graph) == 0) {
        array_onboard.insert(*mi);
        fprintf(cfg_out, "        <port name = \"%s\">\n", (*mi).c_str());
        fprintf(cfg_out, "            <direction>read</direction>\n");
        fprintf(cfg_out, "        </port>\n");
      }
    }
    for (std::set<string>::iterator mi =
             mapGraph2PortAccess[curr_graph].begin();
         mi != mapGraph2PortAccess[curr_graph].end(); mi++) {
      if (dump_array.count(*mi) > 0 &&
          (mapGraph2ArrayUsedLater.count(curr_graph) == 0 ||
           mapGraph2ArrayUsedLater[curr_graph].count(*mi) == 0)) {
        fprintf(cfg_out, "        <port name = \"%s\">\n", (*mi).c_str());
        fprintf(cfg_out, "            <direction>write</direction>\n");
        fprintf(cfg_out, "        </port>\n");
      }
    }

    fprintf(cfg_out, "    </node>\n");
  }
  //  fprintf(cfg_out, "    </param_tree>\n");

  //  collect array reference info of each task

  fprintf(cfg_out, "</io>\n");

  return 1;
}
