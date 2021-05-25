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


#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <map>
#include <string>

#include "codegen.h"
#include "mars_opt.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "mars_ir.h"
#include "stream_ir.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

#include "program_analysis.h"
// using namespace MarsProgramAnalysis;
// using namespace MerlinStreamModel;
static int tool_type = 0;   
static int count_memcpy = 0;
static int pcie_merge = 1;

// using namespace std;

map<string, map<int, string>> kname2idx2var;
map<string, map<string, string>> kname2var2offset;
map<string, int> kname2Id;
map<int, string> id2kname;
map<string, string> kname2base_addr;
map<string, string> kname2sok_addr;
map<string, set<string>> kname2fifo;

int header_gen(CSageCodeGen *codegen, void *sg_scope, int debug_gen);

extern int handle_single_assignment(CSageCodeGen *codegen, void *sg_array1,
                                    const vector<void *> &sg_idx1,
                                    const vector<size_t> &dim1, void *sg_array2,
                                    const vector<void *> &sg_idx2,
                                    const vector<size_t> &dim2, void *sg_length,
                                    int type_size, void *func_call);

extern int function_uniquify_top(CSageCodeGen *codegen, void *pTopFunc,
                                 const CInputOptions &options);

string space_elim(string input) {
  string output;
  for (size_t idx = 0; idx < input.size(); idx++) {
    //  if(strcmp(&(input[idx]), "1") == 0)
    if ((input[idx]) == ' ') {
      continue;
    }
    output.append(input, idx, 1);
  }
  return output;
}
void GetTLDMInfo_withPointer4(void *sg_node, void *pArg) {
  CSageCodeGen sggen;
  string str = sggen.GetPragmaString(sg_node);

  if (!str.empty()) {
    void *sg_pragma_decl = sg_node;  //  sggen.GetParent(sg_node);
    vector<pair<void *, string>> *pPragmaList =
        static_cast<vector<pair<void *, string>> *>(pArg);
    pPragmaList->push_back(pair<void *, string>(sg_pragma_decl, str));
  }
}
void GetTLDMInfo_withPointer3(void *sg_node, void *pArg) {
  CSageCodeGen sggen;
  string str = sggen.GetPragmaString(sg_node);

  if (!str.empty()) {
    void *sg_pragma_decl = sg_node;  //  sggen.GetParent(sg_node);
    vector<pair<void *, string>> *pPragmaList =
        static_cast<vector<pair<void *, string>> *>(pArg);
    pPragmaList->push_back(pair<void *, string>(sg_pragma_decl, str));
  }
}
//  extern void GetTLDMInfo_withPointer1(void * sg_node, void * pArg);
void GetTLDMInfo_withPointer1(void *sg_node, void *pArg) {
  CSageCodeGen sggen;
  string str = sggen.GetPragmaString(sg_node);

  if (!str.empty()) {
    void *sg_pragma_decl = sg_node;  //  sggen.GetParent(sg_node);
    vector<pair<void *, string>> *pPragmaList =
        static_cast<vector<pair<void *, string>> *>(pArg);
    pPragmaList->push_back(pair<void *, string>(sg_pragma_decl, str));
  }
}

static vector<void *> get_all_tasks(CSageCodeGen *codegen, void *pTopFunc) {
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  vector<void *> top_funcs;
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    top_funcs.push_back(func);
  }
  return top_funcs;
}

static int add_global_attribute_to_pntr_ref_by_ref(CSageCodeGen *codegen,
                                                   void *sg_ref) {
  void *sg_base_type;
  vector<size_t> sg_sizes;
  codegen->get_type_dimension(codegen->GetTypeByExp(sg_ref), sg_base_type,
                              sg_sizes, sg_ref);
  int dim = sg_sizes.size();

  if (dim == 0) {  //  scalar, not undetermined
    return 0;
  }
  int ret = 0;
  int curr_dim = 0;  //  the leftmost dim
  void *curr_exp = sg_ref;

  while (true) {
    void *parent = codegen->GetParent(curr_exp);

    if ((codegen->IsPntrArrRefExp(parent) != 0) &&
        curr_exp == codegen->GetExpLeftOperand(parent) && curr_dim >= 0 &&
        curr_dim < dim) {
      curr_dim++;
    } else if (codegen->IsAddressOfOp(parent) != 0) {
      curr_dim--;
    } else if ((codegen->IsCastExp(parent) != 0) &&
               (codegen->IsPointerType(codegen->GetTypeByExp(parent)) != 0)) {
      //  apply global attribute
      void *cast_type = codegen->GetTypeByExp(parent);
      string type_string = codegen->GetStringByType(cast_type);
      if (type_string.find("__global ") == string::npos) {
        string opencl_type = "__global " + type_string;
        codegen->RegisterType(opencl_type);
        void *new_cast_type = codegen->GetTypeByString(opencl_type);
        void *new_parent =
            codegen->CreateCastExp(codegen->CopyExp(curr_exp), new_cast_type);
        codegen->ReplaceExp(parent, new_parent);
        parent = new_parent;
        ret = 1;
      }
    } else if ((codegen->IsPointerDerefExp(parent) != 0) && curr_dim >= 0 &&
               curr_dim < dim) {
      curr_dim++;
    } else if ((codegen->IsAddOp(parent) != 0) && curr_dim < dim &&
               curr_dim >= 0) {
    } else if ((codegen->IsSubtractOp(parent) != 0) &&
               curr_exp == codegen->GetExpLeftOperand(parent) &&
               curr_dim < dim && curr_dim >= 0) {
    } else if ((codegen->IsPlusPlusOp(parent) != 0) ||
               (codegen->IsMinusMinusOp(parent) != 0)) {
    } else if (parent == nullptr) {
      break;
    } else {
      break;
    }

    curr_exp = parent;
  }
  return ret;
}

void graph_block_removal(CSageCodeGen *codegen, void *pTopFunc,
                         int keep_code = 0) {
  //  find "graph_end" pragma & insert "cl_flush()"
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer3,
                          &vecTldmPragmas);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    void *block = codegen->CreateBasicBlock();
    printf("Pragma: %s\n", vecTldmPragmas[i].second.c_str());
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    if (("tldm" != sFilter && "cmost" != sFilter) ||
        (sCmd.find("task_block") == string::npos)) {
      continue;
    }
    string pragma_str = vecTldmPragmas[i].second;

    if (pragma_str.find('"') == string::npos) {
      cerr << "Cannot extract graph_block name" << endl;
      exit(1);
    }

    int pos1 = static_cast<int>(pragma_str.find('"'));
    int pos2 = static_cast<int>(pragma_str.rfind('"'));
    string block_name = pragma_str.substr(pos1 + 1, pos2 - pos1 - 1);

    string prof_call = block_name + "_prof";

    void *sg_scope = vecTldmPragmas[i].first;
    void *blockStmt = codegen->GetNextStmt(sg_scope);
    if (codegen->IsBasicBlock(blockStmt) == 0) {
      cerr << "Pragma graph_block followed by non-basicblock" << endl;
      exit(1);
    }
    while (true) {
      if (codegen->IsScopeStatement(sg_scope) != 0) {
        break;
      }
      { sg_scope = codegen->GetParent(sg_scope); }
    }

    /*vector<void*> sg_init_list;

    void* profCallExp = codegen->CreateFuncCall(prof_call,
    codegen->GetTypeByString("void"), sg_init_list, sg_scope); void*
    profCallStmt = codegen->CreateStmt(V_SgExprStatement, profCallExp);

    //  to check
    codegen->InsertStmt(profCallStmt, blockStmt);
    //  create function declaration
    vector<void*> fp_list;

    //  To check: sg_scope is sg_global
    void* sg_global = codegen->GetGlobal(sg_scope);

    void* prof_func_decl = codegen->CreateFuncDecl("void", prof_call, fp_list,
    sg_global, true, nullptr); codegen->PrependChild(sg_global,
    prof_func_decl);*/

    //  remove the original statement
    if (keep_code != 0) {
      //  codegen->PrependChild(blockStmt, block); //  add block as the first
      //  child of blockStmt
    } else {
      codegen->ReplaceChild(blockStmt, block);  //  replace blockStmt with block
    }
  }
}

void dummy_param_flush_gen(CSageCodeGen *codegen, void *pTopFunc,
                           int debug_gen) {
  //  find "graph_end" pragma & insert "cl_flush()"
  size_t i;
  //  int task_total = 0;
  int count_task = 0;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer1,
                          &vecTldmPragmas);
  map<void *, bool> header_def;

  //    for (i = 0; i < vecTldmPragmas.size(); i++)
  //    {
  //        string sFilter, sCmd;
  //        map<string, pair<vector<string>, vector<string> > > mapParams;
  //        tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd,
  //  mapParams);         if (("tldm" != sFilter && "cmost" != sFilter) ||
  //  (sCmd.find("task_block") == string::npos)) continue;
  //        task_total++;
  //    }
  //    printf("\ntask_total = %d\n",task_total);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    //    printf("DEBUG: Pragma: %s\n", vecTldmPragmas[i].second.c_str());
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);

    if (("tldm" != sFilter && "cmost" != sFilter) ||
        (sCmd.find("task_block") == string::npos)) {
      continue;
    }

    count_task++;
    //        printf("\ncount_task = %d\n",count_task);

    string graph_name;
    if (vecTldmPragmas[i].second.find("task_block") != string::npos) {
      //  int pos1 = int(vecTldmPragmas[i].second.find('"'));
      //  int pos2 = int(vecTldmPragmas[i].second.rfind('"'));
      //  graph_name = vecTldmPragmas[i].second.substr(pos1+1, pos2-pos1-1);
      //
      graph_name = mapParams["name"].first[0];
    }
    void *sg_scope = vecTldmPragmas[i].first;
    void *blockStmt = codegen->GetNextStmt(sg_scope);
    if (codegen->IsBasicBlock(blockStmt) == 0) {
      cerr << "Pragama graph_block followed by non-basicblock" << endl;
      exit(1);
    }
    while (true) {
      if (codegen->IsScopeStatement(sg_scope) != 0) {
        break;
      }
      { sg_scope = codegen->GetParent(sg_scope); }
    }

    vector<void *> sg_init_list;

    //  void* flushCallExp = codegen->CreateFuncCall("cl_flush",
    //  codegen->GetTypeByString("void"), sg_init_list, sg_scope); void*
    //  flushCallStmt = codegen->CreateStmt(V_SgExprStatement, flushCallExp);

    //  codegen->InsertStmt(flushCallStmt, vecTldmPragmas[i].first);
    //  to check
    //    cerr<<"INSERT CMOST_OPENCL_TYPE"<<endl;
    codegen->RegisterType("__CMOST_OPENCL_TYPE__");

    if (debug_gen == RUNTIME_GEN) {
      void *mem_obj_gen_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_MEM_OBJ_GEN__", 0, sg_scope);
      codegen->PrependChild(blockStmt, mem_obj_gen_decl);
      void *init_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_INIT_GEN__", 0, sg_scope);
      codegen->PrependChild(blockStmt, init_decl);
    } else {
      //  //            void* timer_start_decl =
      //  / codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  /"__CMOST_OPENCL_TIMER_START__", 0, sg_scope); /
      //  / codegen->PrependChild(blockStmt, timer_start_decl);
    }
    if (debug_gen == PCIE_GEN) {
      //  void* timer_start_decl =
      //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  "__CMOST_OPENCL_CPU_TIMER_START__", 0, sg_scope);
      //  codegen->PrependChild(blockStmt, timer_start_decl);
      if (pcie_merge == 0 || tool_type == 0) {
        void *pcie_wr_obj_gen = codegen->CreateVariableDecl(
            "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_PCIE_WR_OBJ_GEN__", 0,
            sg_scope);
        codegen->PrependChild(blockStmt, pcie_wr_obj_gen);
      }
      if (count_task == 1) {
        if (pcie_merge == 1 && tool_type == 1) {
          void *pcie_merge_w = codegen->CreateVariableDecl(
              "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_PCIE_MERGE_W__", 0,
              sg_scope);
          codegen->PrependChild(blockStmt, pcie_merge_w);
        }
        void *total_timer = codegen->CreateVariableDecl(
            "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_TOTAL_TIMER_START__", 0,
            sg_scope);
        codegen->PrependChild(blockStmt, total_timer);
      }
      void *test_var_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_PARSER__", 0, sg_scope);
      codegen->PrependChild(blockStmt, test_var_decl);
    }

    void *var_decl = codegen->CreateVariableDecl(
        "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_GEN__", 0, sg_scope);
    //  void* declStmt = codegen->CreateStmt(V_SgExprStatement, var_decl);
    string prag = "\n//;     $curr_graph_name = __merlin_" + graph_name + ";\n";

    CTldmTaskAnn *pTaskAnn = get_tldm_task_by_name(graph_name);
    string async_mode = pTaskAnn->poly_info.properties["async"];
    if (async_mode == "1") {
      async_mode = "1";
    } else {
      async_mode = "0";
    }
    prag += "\n//;     $is_async = " + async_mode + ";\n";
    char temp[64];
    snprintf(temp, sizeof(temp), "%d", count_task);
    prag += "\n//;     $index_" + graph_name + " = " + temp + ";\n";
    codegen->AppendChild(blockStmt, var_decl);

    if (debug_gen == RUNTIME_GEN) {
      void *mem_obj_release_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_MEM_OBJ_RELEASE_GEN__", 0,
          sg_scope);
      codegen->AppendChild(blockStmt, mem_obj_release_decl);
      void *release_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_RELEASE_GEN__", 0, sg_scope);
      codegen->AppendChild(blockStmt, release_decl);
      //            void* mem_obj_gen_decl =
      //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  "__CMOST_OPENCL_MEM_OBJ_GEN__", 0, sg_scope);
      //            codegen->PrependChild(blockStmt, mem_obj_gen_decl);
      //            void* init_decl =
      //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  "__CMOST_OPENCL_INIT_GEN__", 0, sg_scope);
      //            codegen->PrependChild(blockStmt, init_decl);

      //            void* mem_obj_release_decl =
      //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  "__CMOST_OPENCL_MEM_OBJ_RELEASE_GEN__", 0, sg_scope);
      //            codegen->AppendChild(blockStmt, mem_obj_release_decl);
      //            void* release_decl =
      //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  "__CMOST_OPENCL_RELEASE_GEN__", 0, sg_scope);
      //            codegen->AppendChild(blockStmt, release_decl);
    } else {
      //  //            void* timer_end_decl =
      //  / codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  /"__CMOST_OPENCL_TIMER_END__", 0, sg_scope); /
      //  / codegen->AppendChild(blockStmt, timer_end_decl);
    }

    if (debug_gen == PCIE_GEN) {
      //    void* timer_end_decl =
      //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  "__CMOST_OPENCL_CPU_TIMER_END__", 0, sg_scope);
      //    codegen->AppendChild(blockStmt, timer_end_decl);
      if (async_mode == "1") {
        //  void* async_timer_start_decl =
        //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
        //  "__CMOST_OPENCL_TIMER_START__", 0, sg_scope);
        //  codegen->AppendChild(blockStmt, async_timer_start_decl);
      } else {
        void *pcie_rb_obj_gen = codegen->CreateVariableDecl(
            "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_PCIE_RB_OBJ_GEN__", 0,
            sg_scope);
        codegen->AppendChild(blockStmt, pcie_rb_obj_gen);
        void *total_timer_end = codegen->CreateVariableDecl(
            "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_TOTAL_TIMER_END__", 0,
            sg_scope);
        codegen->AppendChild(blockStmt, total_timer_end);
      }
      //  void* timer_end_decl =
      //  codegen->CreateVariableDecl("__CMOST_OPENCL_TYPE__",
      //  "__CMOST_OPENCL_PCIE_TIMER_END__", 0, sg_scope);
      //  codegen->AppendChild(blockStmt, timer_end_decl);
    }

    void *child = codegen->GetChildStmt(blockStmt, 0);
    codegen->AddDirectives(prag, child, 1);
    //  codegen->AppendChild(blockStmt, flushCallStmt);
    if (header_def.count(codegen->GetGlobal(sg_scope)) == 0 ||
        !header_def[codegen->GetGlobal(sg_scope)]) {
      header_gen(codegen, codegen->GetGlobal(sg_scope), debug_gen);
      header_def[codegen->GetGlobal(sg_scope)] = true;
    }
    void *func_decl_exec =
        codegen->GetFuncDeclByName("__merlin_execute_" + graph_name);
    if (func_decl_exec != nullptr) {
      prag += "\n//;     $no_wait = 1;\n";
      void *func_decl_body = codegen->GetFuncBody(func_decl_exec);
      void *var_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_GEN__", 0, sg_scope);
      codegen->AppendChild(func_decl_body, var_decl);
      codegen->AddDirectives(prag, var_decl, 1);
    }
  }
}

int header_gen(CSageCodeGen *codegen, void *sg_scope, int debug_gen) {
  if (debug_gen == RUNTIME_GEN) {
    codegen->AddHeader("\n#include \"kernel_cl.h\"", sg_scope);
    codegen->AddHeader("\n#include <CL/cl.h>", sg_scope);
  } else if (debug_gen == CPU_GEN) {
    codegen->AddHeader("\n#include \"cl_platform.h\"", sg_scope);
  } else {
    codegen->AddHeader("\n#include \"cl.h\"", sg_scope);
  }
  if (debug_gen == PCIE_GEN) {
    //  codegen->AddHeader("\n#define FPGA_DDR_USER_BASE 0xbe000000", sg_scope);
    //    codegen->AddHeader("\n struct timeval t_start, t_end, t_betw;",
    //  sg_scope);     codegen->AddHeader("\n float t_pcie = 0;", sg_scope);
    //    codegen->AddHeader("\n float t_comp = 0;", sg_scope);
  }
  return 1;
}

int header_gen(CSageCodeGen *codegen, void *sg_scope, string func_name) {
  string header_name = "\n#include \"x" + func_name + "_core_slv.h\"";
  codegen->AddHeader(header_name, sg_scope);
  return 1;
}

string cap_to_small(string input) {
  string output;
  //  char* outputc;
  output.clear();
  //  int debug;
  size_t i;
  //  cout<<"= "<<input<<" "<<input.length()<<endl;
  for (i = 0; i < input.length(); i++) {
    if (input[i] >= 'A' && input[i] <= 'Z') {
      input[i] += 32;
    }
  }
  return input;  //  .substr(0);
}

void cfg_gen() {  //  (map<string, int>& kname2Id, map<string, vector<string> >&
                  //  kname2base_addr, map<string, string>& kname2sok_addr)

  //  extract function calls from kernel.config
  std::ifstream cfg_in("driver_cfg.xml");

  if (!cfg_in.is_open()) {
    cout << "Error: driver_cfg.xml does not exist.." << endl;
    exit(1);
  }

  string line;
  //  set<string> kname2Id;
  string mod_name;
  string base_addr;
  string sok_addr;
  //  map<string, vector<string> > mod2base_addr;
  //  map<string, string> mod2sok_addr;

  kname2Id.clear();
  id2kname.clear();

  int kid = 0;

  /*while(fin.good())
      {
              getline(fin, line);
      if(line.find(' ') == string::npos)
      {
          break;
      }
      int pos1 = int(line.find(' '));
      string kname = line.substr(0, pos1);

      string kid_str = line.substr(pos1+1);

      int kid = atoi(kid_str.c_str());

      kname2Id[kname] = kid ++;

      cerr<<kname<<endl;
  }*/
  int port_idx = 0;
  while (cfg_in.good()) {
    getline(cfg_in, line);

    //  if(line.find("module name") == string::npos)
    //    continue;

    if (line.find("module name") != string::npos) {
      int pos1 = static_cast<int>(line.find('\"'));
      int pos2 = static_cast<int>(line.rfind('\"'));

      if (pos2 <= pos1) {
        continue;
      }
      mod_name = line.substr(pos1 + 1, pos2 - pos1 - 1);

      mod_name = mod_name + "_kernel";
      id2kname[kid] = mod_name;
      kname2Id[mod_name] = kid++;
      cout << mod_name << " " << kid - 1 << endl;

      port_idx = 0;
    }
    if (line.find("item") != string::npos) {
      int pos1 = static_cast<int>(line.find('>'));
      int pos2 = static_cast<int>(line.rfind('<'));
      if (pos2 <= pos1) {
        continue;
      }

      base_addr = line.substr(pos1 + 1, pos2 - pos1 - 1);
      kname2base_addr[mod_name] = base_addr;
      cout << mod_name << " base addr: " << base_addr << endl;
    }
    if (line.find("sok") != string::npos) {
      int pos1 = static_cast<int>(line.find('>'));
      int pos2 = static_cast<int>(line.rfind('<'));
      if (pos2 <= pos1) {
        continue;
      }

      sok_addr = line.substr(pos1 + 1, pos2 - pos1 - 1);
      kname2sok_addr[mod_name] = base_addr;
      cout << mod_name << " sok addr: " << sok_addr << endl;
    }
    if (line.find("port_type") != string::npos) {
      unsigned pos = line.find("port_type");
      //    cerr<<line<<" i"<<pos<<endl;
      string line1 = line.substr(0, pos);
      //    cerr<<"i2"<<endl;
      string line2 = line.substr(pos);

      int pos1 = static_cast<int>(line1.find('\"'));

      int pos2 = 0;
      //  if(line1.find('_') > 0)
      //        pos2 = int(line.find('_'));
      //    else
      pos2 = static_cast<int>(line1.rfind('\"'));

      if (pos2 <= pos1) {
        continue;
      }
      //    cerr<<"i3"<<endl;
      string var_name = line1.substr(pos1 + 1, pos2 - pos1 - 1);

      pos1 = static_cast<int>(line2.find('\"'));
      pos2 = static_cast<int>(line2.rfind('\"'));
      //    cerr<<"i1"<<endl;
      string type = line2.substr(pos1 + 1, pos2 - pos1 - 1);
      if (type == "bfifo") {
        kname2fifo[mod_name].insert(var_name);
      }
      //        var_name = var_name + "_g";

      kname2idx2var[mod_name][port_idx++] = var_name;
    }
  }

  for (map<string, string>::iterator mi = kname2sok_addr.begin();
       mi != kname2sok_addr.end(); mi++) {
    string kname = (*mi).first;
    string func_name = kname.substr(0, kname.length() - 7);
    string header_name = "x" + func_name + "_core_slv.h";

    std::ifstream h_in(header_name.c_str());

    if (!h_in.is_open()) {
      cout << "Error:" << header_name << " file does not exist.." << endl;
      exit(1);
    }
    while (h_in.good()) {
      getline(h_in, line);

      if (line.find("ADDR") != string::npos) {
        int pos1 = 0;
        if (line.find("BASE") != string::npos) {
          pos1 = static_cast<int>(line.find("BASE_"));
        } else {
          pos1 = static_cast<int>(line.find("ADDR_"));
        }

        int pos2 = static_cast<int>(line.find("_DATA"));

        string var_name_cap = line.substr(pos1 + 5, pos2 - pos1 - 5);
        string var_name = cap_to_small(var_name_cap);
        string offset = space_elim(line.substr(pos2 + 6));
        kname2var2offset[kname][var_name] = offset;
        cerr << "In " << kname << " " << var_name << " offset: " << offset
             << endl;
      }
    }
  }
}

void user_main_gen(CSageCodeGen *codegen,
                   void *pTopFunc) {  //  , map<string, int> kname2Id)
  int numGlobal = codegen->GetGlobalNum();
  for (int i = 0; i < numGlobal; i++) {
    //  extract scope
    void *sg_scope_global = codegen->GetGlobal(i);

    vector<void *> vec_decls;

    codegen->GetNodesByType(sg_scope_global, "preorder",
                            "SgFunctionDeclaration", &vec_decls);

    for (auto decl : vec_decls) {
      //  void* func_body = codegen->GetFuncBody(vecDecl[j]);
      //  void* func_decl = codegen->GetFuncDeclByCall(decl, 0);
      string func_name = codegen->GetFuncName(decl);
      if (codegen->GetFuncBody(decl) == nullptr) {
        continue;
      }
      if (func_name == "main") {
        codegen->SetFuncName(decl, "user_main");

        //  add a wrapper
        //  user_main_wrapper(int argc, char ** argv);

        vector<void *> arg_list;

        if (codegen->GetFuncParamNum(decl) == 2) {
          void *argc = codegen->GetFuncParam(decl, 0);
          arg_list.push_back(codegen->CreateVariable(
              codegen->GetTypebyVar(argc), codegen->GetVariableName(argc)));
          void *argv = codegen->GetFuncParam(decl, 1);
          arg_list.push_back(codegen->CreateVariable(
              codegen->GetTypebyVar(argv), codegen->GetVariableName(argv)));
        } else {
          arg_list.push_back(
              codegen->CreateVariable(codegen->GetTypeByString("int"), "argc"));
          arg_list.push_back(codegen->CreateVariable(
              codegen->GetTypeByString("char * *"), "argv"));
        }
        arg_list.push_back(codegen->CreateVariable(
            codegen->GetTypeByString("char *"), "bitstream"));

        //  3. create funciton declaration
        void *sg_decl_new;
        {
          sg_decl_new =
              codegen->CreateFuncDecl("int", "user_main_wrapper", arg_list,
                                      codegen->GetGlobal(decl), true, nullptr);

          void *sg_body_new = codegen->GetFuncBody(sg_decl_new);

          vector<void *> param_list;
          for (auto arg : arg_list) {
            param_list.push_back(codegen->CreateVariableRef(arg));
          }
          void *expr = codegen->CreateFuncCall(
              "user_main", codegen->GetTypeByString("int"), param_list,
              codegen->GetGlobal(decl));
          void *stmt = codegen->CreateStmt(V_SgExprStatement, expr);
          codegen->AppendChild(sg_body_new, stmt);
        }

        codegen->AppendChild(codegen->GetGlobal(decl), sg_decl_new);

        int arg_num = codegen->GetFuncParamNum(decl);
        int arg_index = 0;
        for (auto arg : arg_list) {
          if (arg_index >= arg_num) {
            codegen->insert_param_to_decl(
                decl, codegen->CreateVariable(codegen->GetTypebyVar(arg),
                                              codegen->GetVariableName(arg)));
          }
          arg_index++;
        }
      }
    }
  }
}

int NDRange_gen(CSageCodeGen *codegen,
                void *pTopFunc) {  //  , map<string, int> kname2Id)
  int numGlobal = codegen->GetGlobalNum();

  int addHeader = 1;

  for (int i = 0; i < numGlobal; i++) {
    //  extract scope
    void *sg_scope_global = codegen->GetGlobal(i);

    vector<void *> vecCalls;

    codegen->GetNodesByType(sg_scope_global, "preorder", "SgFunctionCallExp",
                            &vecCalls);

    for (size_t j = 0; j < vecCalls.size(); j++) {
      //  void* func_body = codegen->GetFuncBody(vecDecl[j]);
      void *func_decl = codegen->GetFuncDeclByCall(vecCalls[j], 0);
      string func_name = codegen->GetFuncName(func_decl);

      if (kname2Id.count(func_name) > 0) {
        //  cerr<<"Arrive 1"<<endl;
        string callName = "clEnqueueNDRangeKernel";
        //  string cbCallName = "clCreateBuffer";
        string skaCallName = "clSetKernelArg";

        //  find sg_scope
        void *sg_scope = vecCalls[j];

        while (true) {
          if (codegen->IsScopeStatement(sg_scope) != 0) {
            break;
          }
          { sg_scope = codegen->GetParent(sg_scope); }
        }

        string str5 = codegen->UnparseToString(sg_scope);
        //  cerr<<"sg_scope: "<<str5<<endl;
        //  cerr<<"Arrive 2"<<endl;

        int kid = kname2Id[func_name];

        void *zero_val = codegen->CreateConst(0);
        //  void* one_val = codegen->CreateConst(1);
        void *kid_val = codegen->CreateConst(kid);

        //  check:create return value
        void *ret_var =
            codegen->CreateVariableDecl("int", "err", zero_val, sg_scope);

        /*void* a1_var = codegen->CreateVariableDecl("int", "a1", zero_val,
        sg_scope); void* a2_var = codegen->CreateVariableDecl("int", "a2",
        kid_val, sg_scope); void* a3_var = codegen->CreateVariableDecl("int",
        "a3", zero_val, sg_scope); void* a4_var =
        codegen->CreateVariableDecl("int", "a4", zero_val, sg_scope); void*
        a5_var = codegen->CreateVariableDecl("int", "a5", zero_val, sg_scope);
        void* a6_var = codegen->CreateVariableDecl("int", "a6", zero_val,
        sg_scope); void* a7_var = codegen->CreateVariableDecl("int", "a7",
        zero_val, sg_scope); void* a8_var = codegen->CreateVariableDecl("int",
        "a8", zero_val, sg_scope); void* a9_var =
        codegen->CreateVariableDecl("int", "a9", zero_val, sg_scope);
*/
        //  create function call
        vector<void *> sg_init_list;
        sg_init_list.push_back(kid_val);
        /*sg_init_list.push_back(codegen->CreateVariableRef(a1_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a2_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a3_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a4_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a5_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a6_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a7_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a8_var));
        sg_init_list.push_back(codegen->CreateVariableRef(a9_var));
        *///  To check: sg_scope is func_body
        //  cerr<<"Arrive 2.5"<<endl;
        void *NDRangeCallExp = codegen->CreateFuncCall(
            callName, codegen->GetTypeByString("int"), sg_init_list, sg_scope);
        string str = codegen->UnparseToString(NDRangeCallExp);
        //  str = codegen->UnparseToString(sg_scope);
        void *NDRangeCallStmt = codegen->CreateStmt(
            V_SgAssignStatement, codegen->CreateVariableRef(ret_var),
            NDRangeCallExp);
        //  cerr<<"Arrive 2.6"<<endl;
        //                void* NDRangeCallStmt =
        //  codegen->CreateStmt(V_SgExprStatement, NDRangeCallExp);
        void *parentStmt =
            codegen->TraceUpByType(vecCalls[j], "SgExprStatement");
        codegen->InsertStmt(NDRangeCallStmt, parentStmt);
        //  cerr<<"Arrive 2.7"<<endl;

        //  insert clCreateBuffer
        //  insert clSetKernelArg

        int numParam = codegen->GetFuncParamNum(func_decl);

        //  cerr<<"Arrive 2.8"<<endl;

        vector<void *> sg_init_list_cb;
        vector<void *> sg_init_list_ska;
        for (int nparam = 0; nparam < numParam; nparam++) {
          //  string ret_var_i_name = "cl" + par;
          //  void* ret_var_i = codegen->CreateVariableDecl("int",
          //  ret_var_i_name, zero_val, sg_scope);

          //  cerr<<"Arrive 3"<<endl;
          void *param_i = codegen->GetFuncCallParam(vecCalls[j], nparam);
          void *ref_copy = codegen->CopyExp(param_i);
          string debug = codegen->UnparseToString(ref_copy);
          //  cerr<<"Debug:"<<debug<<endl;

          //  cerr<<"Arrive 4 j = "<<i<<endl;
          /*    sg_init_list_cb.clear();

              sg_init_list_cb.push_back(zero_val);
              sg_init_list_cb.push_back(zero_val);
              sg_init_list_cb.push_back(one_val);
              sg_init_list_cb.push_back(ref_copy);
              sg_init_list_cb.push_back(zero_val);*/
          //  void* CreateBufferCallExp = codegen->CreateFuncCall(cbCallName,
          //  codegen->GetTypeByString("int"), sg_init_list_cb, sg_scope); void*
          //  CreateBufferCallStmt = codegen->CreateStmt(V_SgAssignStatement,
          //  codegen->CreateVariableRef(ret_var_i), CreateBufferCallExp);

          //  codegen->InsertStmt(CreateBufferCallStmt, NDRangeCallStmt);

          void *nparam_val = codegen->CreateConst(nparam);
          sg_init_list_ska.clear();

          //    cerr<<"Arrive 4.2 j = "<<i<<endl;
          sg_init_list_ska.push_back(kid_val);
          sg_init_list_ska.push_back(nparam_val);
          sg_init_list_ska.push_back(ref_copy);
          //  sg_init_list_ska.push_back(one_val);
          //  sg_init_list_ska.push_back(ret_val_i);

          void *SetKernelArgCallExp = codegen->CreateFuncCall(
              skaCallName, codegen->GetTypeByString("int"), sg_init_list_ska,
              sg_scope);
          //    cerr<<"Arrive 5"<<endl;
          void *SetKernelArgCallStmt =
              codegen->CreateStmt(V_SgExprStatement, SetKernelArgCallExp);
          //    cerr<<"Arrive 6"<<endl;
          codegen->InsertStmt(SetKernelArgCallStmt, NDRangeCallStmt);
          //    cerr<<"Arrive 7"<<endl;
        }

        codegen->RemoveChild(parentStmt);
        if (addHeader == 1) {
          header_gen(codegen, codegen->GetGlobal(sg_scope), 0);

          /*    for(map<string, int>::iterator mi = kname2Id.begin(); mi !=
             kname2Id.end(); mi ++)
              {
                  string kname = (*mi).first;
                  //  unsigned pos = kname.find("_kernel");
                  string func_name = kname.substr(0, kname.length() - 7);
                  header_gen(codegen, codegen->GetGlobal(sg_scope), func_name);
              }*/
          addHeader = 0;
        }  //  codegen->ReplaceChild(vecCalls[j],NDRangeCallStmt);
      }
    }
  }
  return 1;
}

int mem_object_gen(CSageCodeGen *codegen, void *pTopFunc, int debug_gen) {
  size_t i;
  //  1. get task_bb and graph_bb
  std::ofstream mem_obj_out("opencl_mem_obj_gen_p");
  std::ofstream mem_obj_release_out("opencl_mem_obj_release_gen_p");
  std::ofstream mem_obj_pcie_w_out("opencl_mem_obj_pcie_w_gen_p");
  std::ofstream mem_obj_pcie_r_out("opencl_mem_obj_pcie_r_gen_p");
  std::ofstream io_out("io_cfg.xml");

  map<string, vector<int>> arr_name2dim;
  arr_name2dim.clear();
  map<string, string> arr_name2dim_exp;
  arr_name2dim_exp.clear();
  set<string> dump_data;
  dump_data.clear();
  set<string> init_data;
  init_data.clear();

  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);

    if ("tldm" != sFilter && "cmost" != sFilter) {
      continue;
    }

    void *sg_pragma = vecTldmPragmas[i].first;
    string pragma_str = vecTldmPragmas[i].second;
    if (pragma_str.find("cmost_data_dump") != string::npos) {
      int arg_pos = pragma_str.find("func_args");
      string dump_data_str = space_elim(pragma_str.substr(arg_pos));

      int start_pos = dump_data_str.find('"');

      dump_data_str = dump_data_str.substr(start_pos + 1);
      int end_pos = dump_data_str.rfind('"');

      int pos_1 = 0;  //  start_pos + 1;
      int pos_2 = 1;  //  pos_1 + 1;

      string dump_name;

      while (true) {
        if (dump_data_str.find(',', pos_1) == string::npos) {
          dump_name = dump_data_str.substr(pos_1, end_pos - pos_1);
          dump_data.insert(dump_name);
          break;
        }

        pos_2 = dump_data_str.find(',', pos_1);
        dump_name = dump_data_str.substr(pos_1, pos_2 - pos_1);
        dump_data.insert(dump_name);
        pos_1 = pos_2 + 1;
      }
    }
    if (pragma_str.find("cmost_data_init") != string::npos) {
      int arg_pos = pragma_str.find("func_args");
      string init_data_str = space_elim(pragma_str.substr(arg_pos));
      int start_pos = init_data_str.find('"');

      init_data_str = init_data_str.substr(start_pos + 1);
      int end_pos = init_data_str.rfind('"');
      //  mem_obj_pcie_r_out<<"DEBUG: init_data_str = "<<init_data_str<<endl;
      int pos_1 = 0;  //  start_pos + 1;
      int pos_2 = 1;  //  pos_1 + 1;

      string init_name;

      while (true) {
        if (init_data_str.find(',', pos_1) == string::npos) {
          init_name = init_data_str.substr(pos_1, end_pos - pos_1);
          init_data.insert(init_name);
          break;
        }

        pos_2 = init_data_str.find(',', pos_1);
        init_name = init_data_str.substr(pos_1, pos_2 - pos_1);
        init_data.insert(init_name);
        pos_1 = pos_2 + 1;
      }
    }
    if (("tldm" != sFilter && "cmost" != sFilter) || ("task_block" != sCmd)) {
      continue;
    }
    //  void * task_bb = codegen->GetNextStmt(sg_pragma);
    //  assert(codegen->IsBasicBlock(task_bb));
    //  if(!codegen->IsBasicBlock(task_bb)) continue;
    void *graph_bb = sg_pragma;

    while (graph_bb != nullptr) {
      graph_bb =
          codegen->TraceUpByType(codegen->GetParent(graph_bb), "SgBasicBlock");

      void *sg_parent = codegen->GetParent(graph_bb);
      if (codegen->IsBasicBlock(sg_parent) == 0) {
        continue;
      }
      if (graph_bb == nullptr) {
        break;
      }
      size_t child_index = codegen->GetChildStmtIdx(sg_parent, graph_bb);
      if (0 == child_index ||
          child_index >= codegen->GetChildStmtNum(sg_parent)) {
        continue;
      }

      void *pre_stmt = codegen->GetChildStmt(sg_parent, child_index - 1);

      string sPragma = codegen->GetPragmaString(pre_stmt);
      if (sPragma.empty()) {
        continue;
      }

      map<string, pair<vector<string>, vector<string>>> mapParams;
      tldm_pragma_parse_whole(sPragma, &sFilter, &sCmd, &mapParams);

      if (("tldm" == sFilter || "cmost" == sFilter) && "graph_block" == sCmd) {
        break;
      }
    }

    //  graph_bb is the basic block for one task
    assert(codegen->IsBasicBlock(graph_bb));

    codegen->get_arrays_dim(graph_bb, arr_name2dim);
    printf("size = %d", static_cast<int>(arr_name2dim.size()));
  }

  //  deal with malloc
  int numGlobal = codegen->GetGlobalNum();

  for (int i = 0; i < numGlobal; i++) {
    //  extract scope
    void *sg_scope_global = codegen->GetGlobal(i);

    vector<void *> vecCalls;

    codegen->GetNodesByType(sg_scope_global, "preorder", "SgFunctionCallExp",
                            &vecCalls);

    for (size_t j = 0; j < vecCalls.size(); j++) {
      //  void* func_body = codegen->GetFuncBody(vecDecl[j]);
      void *func_decl = codegen->GetFuncDeclByCall(vecCalls[j], 0);
      string func_name = codegen->GetFuncName(func_decl);
      //  cerr<<"func call name = "<<func_name<<endl;
      if (func_name == "malloc") {
        string elem_size;
        string total_size;

        string func_call_str = codegen->UnparseToString(vecCalls[j]);
        string func_decl_str = codegen->UnparseToString(func_decl);

        assert(codegen->GetFuncCallParamNum(vecCalls[j]) <= 1);
        //  for (int k = 0; k < codegen->GetFuncCallParamNum(vecCalls[j]); k++)
        void *arg_exp = codegen->GetFuncCallParam(vecCalls[j], 0);
        string size_exp = codegen->UnparseToString(arg_exp);
        //  cerr<<"param "<<k<<" = "<<param_k<<endl;

        string arr_name = codegen->GetMallocVarName(vecCalls[j]);
        arr_name2dim_exp[arr_name] = size_exp;

        //  cerr<<"malloc array name = "<<arr_name<<endl;
        //  cerr<<"malloc call : "<<func_call_str<<endl;
      }
    }
  }

  //  mem_obj_pcie_w_out<<"DEBUG: dump_data size = "<<dump_data.size()<<endl;

  if (debug_gen == PCIE_GEN) {
    /*io_out<<"<io>\n";
    for(set<string>::iterator msi = init_data.begin(); msi != init_data.end();
    msi ++)
    {
        string data_name = *msi;
        io_out<<"<node name = \""<<data_name<<"\">\n";
        io_out<<"<direction>write</direction>\n";
        io_out<<"</node>\n";
        //  mem_obj_pcie_w_out<<"
    clEnqueueWriteBuffer((void*)"<<data_name<<");\n";
    }*/
  }

  for (set<string>::iterator msi = dump_data.begin(); msi != dump_data.end();
       msi++) {
    string data_name = *msi;

    if (debug_gen == PCIE_GEN) {
      //    io_out<<"<node name = \""<<data_name<<"\">\n";
      //    io_out<<"<direction>read</direction>\n";
      //    io_out<<"</node>\n";
      //    mem_obj_pcie_r_out<<"
      //  clEnqueueReadBuffer((void*)"<<data_name<<");\n";
    }
    if (debug_gen == RUNTIME_GEN) {
      if (arr_name2dim.count(data_name) > 0) {
        vector<int> vec_size = arr_name2dim[data_name];
        int arr_size = 1;
        for (vector<int>::iterator vi = vec_size.begin(); vi != vec_size.end();
             vi++) {
          arr_size *= (*vi);
        }
        mem_obj_release_out << "    clEnqueueReadBuffer(cmdQueue, oclbuf_"
                            << data_name << ", CL_TRUE, 0, " << arr_size << ", "
                            << data_name << ", 0, NULL, NULL);\n";
      }
      if (arr_name2dim_exp.count(data_name) > 0) {
        string dim_exp = arr_name2dim_exp[data_name];
        mem_obj_release_out << "    clEnqueueReadBuffer(cmdQueue, oclbuf_"
                            << data_name << ", CL_TRUE, 0, " << dim_exp << ", "
                            << data_name << ", 0, NULL, NULL);\n";
      }
    }
  }
  if (debug_gen == PCIE_GEN) {
    io_out << "</io>\n";
  }

  if (debug_gen == RUNTIME_GEN) {
    mem_obj_out << "//  ; my $module_list = param_tree_define(\"module_list\", "
                   "\"default\", \"item module\");"
                << endl;
    mem_obj_out << "//  ; for($i = 0; $i < @{$module_list->{module}}; $i ++){"
                << endl;
    mem_obj_out << "//  ;        for($j = 0; $j < "
                   "@{$module_list->{module}[$i]->{port_list}->{port}}; $j ++){"
                << endl;
    mem_obj_out << "//  ;            $name = "
                   "$module_list->{module}[$i]->{port_list}->{port}[$j]->{func_"
                   "arg_name};"
                << endl;
    mem_obj_out
        << "//  ;            $port_type = "
           "$module_list->{module}[$i]->{port_list}->{port}[$j]->{port_type};"
        << endl;

    mem_obj_out << "//  ;     if($delcared{$name} ne 999){" << endl;

    mem_obj_release_out
        << "//  ; my $module_list = param_tree_define(\"module_list\", "
           "\"default\", \"item module\");"
        << endl;
    mem_obj_release_out
        << "//  ; for($i = 0; $i < @{$module_list->{module}}; $i ++){" << endl;
    mem_obj_release_out
        << "//  ;        for($j = 0; $j < "
           "@{$module_list->{module}[$i]->{port_list}->{port}}; $j ++){"
        << endl;
    mem_obj_release_out << "//  ;            $name = "
                           "$module_list->{module}[$i]->{port_list}->{port}[$j]"
                           "->{func_arg_name};"
                        << endl;
    mem_obj_release_out
        << "//  ;            $port_type = "
           "$module_list->{module}[$i]->{port_list}->{port}[$j]->{port_type};"
        << endl;

    mem_obj_release_out << "//  ;     if($delcared{$name} ne 9999){" << endl;
    //  : my $module_list = param_tree_define("module_list", "default", "item
    //  module"); : for($i = 0; $i < @{$module_list->{module}}; $i ++){ :
    //  for($j = 0; $j < @{$module_list->{module}[$i]->{port_list}->{port}}; $j
    //  ++){ :            $name =
    //  $module_list->{module}[$i]->{port_list}->{port}[$j]->{func_arg_name}; :
    //  $port_type =
    //  $module_list->{module}[$i]->{port_list}->{port}[$j]->{port_type};
    mem_obj_out << "//  ;             if($port_type eq \"param\"){" << endl;
    mem_obj_out << "    cl_mem oclbuf_`$name` = clCreateBuffer(context, "
                   "CL_MEM_READ_WRITE, 4, NULL, &status);"
                << endl;
    mem_obj_out << "    status = clEnqueueWriteBuffer(cmdQueue, "
                   "oclbuf_`$name`, CL_FALSE, 0, &`$name`, 4, 0, NULL, NULL);"
                << endl;
    mem_obj_out << "//  ;     $delcared{$name} = 999;" << endl;
    mem_obj_out << "//  ;             }" << endl;

    mem_obj_release_out << "//  ;             if($port_type eq \"param\"){"
                        << endl;
    mem_obj_release_out << "     clReleaseMemObject(oclbuf_`$name`);" << endl;
    mem_obj_release_out << "//  ;     $delcared{$name} = 9999;" << endl;
    mem_obj_release_out << "//  ;             }" << endl;

    for (map<string, vector<int>>::iterator mvi = arr_name2dim.begin();
         mvi != arr_name2dim.end(); mvi++) {
      vector<int> vec_size = (*mvi).second;

      int arr_size = 1;
      for (vector<int>::iterator vi = vec_size.begin(); vi != vec_size.end();
           vi++) {
        arr_size *= (*vi);
      }
      //  mem_obj_out<<"//  ; if($name eq \""<<(*mvi).first<<"\" && ( $port_type
      //  eq
      //  \"bus_e1\" || $port_type eq \"bus_e2\" || $port_type eq
      //  \"param\"))"<<endl;
      mem_obj_out
          << "//  ;        if($name eq \"" << (*mvi).first
          << "\" && ( $port_type eq \"bus_e1\" || $port_type eq \"bus_e2\" ))"
          << endl;
      mem_obj_out << "//  ;        {" << endl;
      mem_obj_out << "    cl_mem oclbuf_" << (*mvi).first
                  << " = clCreateBuffer(context, CL_MEM_READ_WRITE, "
                  << arr_size << ", NULL, &status);" << endl;
      mem_obj_out << "    status = clEnqueueWriteBuffer(cmdQueue, oclbuf_"
                  << (*mvi).first << ", CL_FALSE, 0, " << (*mvi).first << ", "
                  << arr_size << ", 0, NULL, NULL);" << endl;
      mem_obj_out << "//  ;     $delcared{$name} = 999;" << endl;
      mem_obj_out << "//  ;        }" << endl;

      mem_obj_release_out
          << "//  ;        if($name eq \"" << (*mvi).first
          << "\" && ( $port_type eq \"bus_e1\" || $port_type eq \"bus_e2\" ))"
          << endl;
      mem_obj_release_out << "//  ;        {" << endl;
      mem_obj_release_out << "     clReleaseMemObject(oclbuf_" << (*mvi).first
                          << ");" << endl;
      mem_obj_release_out << "//  ;     $delcared{$name} = 9999;" << endl;
      mem_obj_release_out << "//  ;        }" << endl;
    }

    for (map<string, string>::iterator mvi2 = arr_name2dim_exp.begin();
         mvi2 != arr_name2dim_exp.end(); mvi2++) {
      if (arr_name2dim.count((*mvi2).first) > 0) {
        continue;
      }

      //  mem_obj_out<<"//  ; if($name eq \""<<(*mvi2).first<<"\" && $port_type
      //  neq
      //  \"bfifo\")"<<endl;
      mem_obj_out
          << "//  ;        if($name eq \"" << (*mvi2).first
          << "\" && ( $port_type eq \"bus_e1\" || $port_type eq \"bus_e2\"))"
          << endl;
      mem_obj_out << "//  ;         {" << endl;
      mem_obj_out << "    cl_mem oclbuf_" << (*mvi2).first
                  << " = clCreateBuffer(context, CL_MEM_READ_WRITE, "
                  << (*mvi2).second << ", NULL, &status);" << endl;
      mem_obj_out << "    status = clEnqueueWriteBuffer(cmdQueue, oclbuf_"
                  << (*mvi2).first << ", CL_FALSE, 0, " << (*mvi2).first << ", "
                  << (*mvi2).second << ", 0, NULL, NULL);" << endl;
      mem_obj_out << "//  ;     $delcared{$name} = 999;" << endl;
      mem_obj_out << "//  ;         }" << endl;

      mem_obj_release_out
          << "//  ;        if($name eq \"" << (*mvi2).first
          << "\" && ( $port_type eq \"bus_e1\" || $port_type eq \"bus_e2\" ))"
          << endl;
      mem_obj_release_out << "//  ;        {" << endl;
      mem_obj_release_out << "     clReleaseMemObject(oclbuf_" << (*mvi2).first
                          << ");" << endl;
      mem_obj_release_out << "//  ;     $delcared{$name} = 9999;" << endl;
      mem_obj_release_out << "//  ;        }" << endl;
    }
    mem_obj_out << "//  ;    }" << endl;
    mem_obj_out << "//  ;}" << endl;
    mem_obj_out << "//  ;}" << endl;

    mem_obj_release_out << "//  ;    }" << endl;
    mem_obj_release_out << "//  ;}" << endl;
    mem_obj_release_out << "//  ;}" << endl;
    //  ;:    }
  }
  return 1;
}

void insert_interface_pragma(CSageCodeGen *codegen, void *pTopFunc) {
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  map<string, int> mapKernel;
  for (size_t k = 0; k < taskset.size(); k++) {
    //  int idx;
    CTldmTaskAnn *pTask = taskset[k];

    string sFuncName = pTask->GetParam("name") + "_kernel";
    //  ZP: 20151012
    sFuncName = pTask->poly_info.properties["kernel_name"];

    if (pTask->poly_info.type != "task") {
      continue;
    }

    string sKernel = pTask->poly_info.properties["kernel_name"];
    if (mapKernel.find(sKernel) == mapKernel.end()) {
      mapKernel[sKernel] = 1;
    } else {
      continue;
    }

    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    void *body = codegen->GetFuncBody(func);
    assert(func && body);

    void *first_stmt = codegen->GetChildStmt(body, 0);
    assert(first_stmt);

    codegen->AddDirectives("\n//; $curr_task_name=\"" +
                               pTask->GetParam("name") + "\";",
                           first_stmt);
    codegen->AddDirectives("\n//; #include \"interface_pragma.inc\"",
                           first_stmt);
  }
}
void opencl_kernel_add_extern_C(CSageCodeGen *codegen, void *pTopFunc) {
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  for (size_t k = 0; k < taskset.size(); k++) {
    //  int idx;
    CTldmTaskAnn *pTask = taskset[k];

    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];

    //  vector<void *> kernel_funcs = codegen->GetAllFuncDeclByName(sFuncName,
    //  1); if (kernel_funcs.size() < 1) continue;
    if (pTask->poly_info.type != "task") {
      continue;
    }
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    void *body = codegen->GetFuncBody(func);
    assert(func && body);

    if (codegen->isWithInCppFile(func)) {
      void *pos = codegen->CreateVariableDecl(
          "int", codegen->GetFuncName(func) + "_dummy_pos", NULL,
          codegen->GetParent(func));
      codegen->SetStatic(pos);
      codegen->InsertStmt(pos, func);
      codegen->AddDirectives("\nextern \"C\" { \n", func, 1);
      codegen->AddDirectives("}\n", func, 0);
    }
  }
}

//  ZP: 20151113
//  Problem:
//   There is a compatibility problem on ROSE and SDAccel
//   ROSE will add a cast of void* to the NULL pointer
//   But SDAccel does not accept this cast conversion, and fails in host
//   compilation
//  Solution:
//   We do the conversion to the correct type by automation
void fix_time_h_system_call(CSageCodeGen *codegen, void *pTopFunc) {
  //  1. find all the gettimeofday function calls
  //  2. add a cast to "struct timezone *" to the second argument

  vector<void *> vecCalls;

  codegen->GetNodesByType(pTopFunc, "preorder", "SgFunctionCallExp", &vecCalls);

  for (size_t i = 0; i < vecCalls.size(); i++) {
    void *sg_call = vecCalls[i];

    if (codegen->GetFuncNameByCall(sg_call) == "gettimeofday") {
      void *sg_arg_2 = codegen->GetFuncCallParam(sg_call, 1);
      codegen->RegisterType("struct timezone");
      void *sg_arg_2_new = codegen->CreateCastExp(codegen->CopyExp(sg_arg_2),
                                                  "struct timezone *");
      codegen->ReplaceExp(sg_arg_2, sg_arg_2_new);
    }
  }
}

void opencl_kernel_declaration_gen(CSageCodeGen *codegen, void *pTopFunc) {
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  for (size_t k = 0; k < taskset.size(); k++) {
    int idx;
    CTldmTaskAnn *pTask = taskset[k];

    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    //  printf("[Han Debug][opencl_gen.cpp]kernel_name =
    //  %s\n",sFuncName.c_str()); printf("[Han
    //  Debug][opencl_gen.cpp]poly_info.type =
    //  %s\n",pTask->poly_info.type.c_str()); vector<void *> kernel_funcs =
    //  codegen->GetAllFuncDeclByName(sFuncName, 1); if (kernel_funcs.size() <
    //  1) continue;
    if (pTask->poly_info.type != "task") {
      continue;
    }
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    void *body = codegen->GetFuncBody(func);
    assert(func && body);

    for (idx = 0; idx < codegen->GetFuncParamNum(func); idx++) {
      //  printf("[Han Debug][opencl_gen.cpp]Enter GetFuncParamNum\n");
      //  void * arg = codegen->GetFuncParam(func, idx);
      {
        //  void * sg_type = codegen->GetTypebyVar(arg);
        //  if (codegen->IsPointerType(sg_type)) {
        //    string sg_type_string = codegen->GetStringByType(sg_type);

        //    //  ZP: 20151001: simplify for ap_uint<512> {}
        //    //  This implementation is not solid
        //    if (sg_type_string.find("{") != -1)
        //    {
        //        int pos = sg_type_string.find("{");
        //        sg_type_string = sg_type_string.substr(0, pos) + " * ";
        //    }
        //    //  string opencl_type = "__global " + sg_type_string;
        //    string opencl_type;
        //    if(tool_type == 1) {
        //        //  opencl_type = "__global " + sg_type_string + " restrict";
        //    } else {
        //        opencl_type = "__global " + sg_type_string;
        //    }
        //    codegen->RegisterType(opencl_type);
        //    codegen->SetTypetoVar(arg, codegen->GetTypeByString(opencl_type));
        //  }
      }
    }
  }
}

void getGid_gen(CSageCodeGen *codegen, void *pTopFunc,
                int tool_type) {  //  , map<string, int> kname2Id)
  tldm_polyhedral_info access_poly_info;
  tldm_polyhedral_info task_poly_info;
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  map<string, int> mapKernel;
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];

    string sFuncName;  //  = pTask->GetParam("name") + "_kernel";

    task_poly_info = pTask->poly_info;

    if (pTask->poly_info.type != "task") {
      continue;
    }
    //  vector<void *> kernel_funcs = codegen->GetAllFuncDeclByName(sFuncName,
    //  1); if (kernel_funcs.size() < 1) continue;

    //  ZP: 20151012
    sFuncName = pTask->poly_info.properties["kernel_name"];
    //  printf("[Han Debug][opencl_gen.cpp]sFuncName = %s",sFuncName.c_str());

    string sKernel = sFuncName;
    if (mapKernel.find(sKernel) == mapKernel.end()) {
      mapKernel[sKernel] = 1;
    } else {
      continue;
    }

    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    void *body = codegen->GetFuncBody(func);
    assert(func && body);

    string iterator_vector = pTask->poly_info.iterator_vector;
    string iterator_range = pTask->poly_info.iterator_range;

    vector<string> iterators = str_split(iterator_vector, ",");
    vector<vector<string>> ranges = str_split_2d(iterator_range, ",", "..");
    assert(iterators.size() == ranges.size());

    vector<void *> vec_param;
    int i;
    for (i = 0; i < codegen->GetFuncParamNum(func); i++) {
      vec_param.push_back(codegen->GetFuncParam(func, i));
    }

    void *dummyAssignStmt = codegen->CreateVariableDecl(
        "int", string("__CMOST_KERNEL_ENTRY__") + sFuncName, 0, body);
    codegen->PrependChild(body, dummyAssignStmt);

    if (tool_type != 1) {
      codegen->AddDirectives("\n// __MERLIN_INTERFACE_BEGIN__",
                             dummyAssignStmt);
      codegen->AddDirectives("\n//; $curr_task_name=\"" +
                                 pTask->GetParam("name") + "\";",
                             dummyAssignStmt);
      codegen->AddDirectives("\n//; #include \"interface_pragma.inc\"",
                             dummyAssignStmt);
      codegen->AddDirectives("\n// __MERLIN_INTERFACE_END__", dummyAssignStmt);
    }

    for (size_t i = 0; i < iterators.size(); i++) {
      //  ////////////////////////////////////  /
      //  each iterator a get_global_id call
      //  ////////////////////////////////////  /

      //  1. search for the sgInitialName for the iterator in the function arg
      //  list
      void *sg_var = nullptr;
      for (size_t j = 0; j < vec_param.size(); j++) {
        if (codegen->UnparseToString(vec_param[j]) == iterators[i]) {
          sg_var = vec_param[j];
          break;
        }
      }

      //  2. create function call and assignment statement
      string callName = "get_global_id";
      vector<void *> sg_init_list;
      void *sg_index = codegen->CreateConst(i);
      sg_init_list.push_back(sg_index);
      void *gidCallExp = codegen->CreateFuncCall(
          callName, codegen->GetTypeByString("int"), sg_init_list, body);
      //  void* gidAddExp = codegen->CreateExp(V_SgAddOp,
      //  codegen->CreateConst(my_atoi(ranges[i][0])));
      void *gidAssignStmt = codegen->CreateStmt(
          V_SgAssignStatement, codegen->CreateVariableRef(sg_var), gidCallExp);
      codegen->PrependChild(body, gidAssignStmt);
    }
  }
}

void async_read_gen(CSageCodeGen *codegen, void *pTopFunc, int debug_gen) {
  //  find "graph_end" pragma & insert "cl_flush()"
  size_t i;
  size_t j;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer1,
                          &vecTldmPragmas);
  map<void *, bool> header_def;

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    //    printf("DEBUG: Pragma: %s\n", vecTldmPragmas[i].second.c_str());
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    void *sg_scope = vecTldmPragmas[i].first;

    if ("ACCEL" != sFilter) {
      continue;
    }

    string task_name;
    if (vecTldmPragmas[i].second.find("taskwait") != string::npos) {
      //            printf("\n%s\n",vecTldmPragmas[i].second.c_str());
      vector<string> taskwait_str = str_split(vecTldmPragmas[i].second, " ");
      for (j = taskwait_str.size() - 1; j >= 2; j--) {
        //            for(j = 2; j < taskwait_str.size(); j++) {
        //                printf("\n%s\n",taskwait_str[j].c_str());
        string wait_task_name =
            "\n//;     $wait_task_name = " + taskwait_str[j] +
            ";\n//;     $is_async = 1;\n";
        void *total_time_end = codegen->CreateVariableDecl(
            "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_TOTAL_TIMER_END__", 0,
            sg_scope);
        codegen->InsertAfterStmt(total_time_end, sg_scope);
        void *pcie_rb_obj_gen = codegen->CreateVariableDecl(
            "__CMOST_OPENCL_TYPE__", "__CMOST_OPENCL_PCIE_RB_OBJ_GEN__", 0,
            sg_scope);
        codegen->InsertAfterStmt(pcie_rb_obj_gen, sg_scope);
        codegen->AddDirectives(wait_task_name, pcie_rb_obj_gen);
      }
    }
  }
}

void add_global_attribute(CSageCodeGen *codegen, void *var_init, int is_top,
                          const set<void *> &readonly_or_writeonly_vars,
                          int add_volatile) {
  void *sg_type = codegen->GetTypebyVar(var_init);
  string sg_type_string = codegen->GetStringByType(sg_type);
  bool is_volatile = codegen->IsVolatileType(sg_type) != 0;

  int find_flag = 0;
  if (sg_type_string.find("__constant ") == 0) {
    return;
  }
  if (sg_type_string.find("__global ") == 0) {
    find_flag = 1;
  }
  //    return;

  //  if (codegen->IsArrayType(sg_type)) {
  if (is_top != 0) {
    void *basic_type;
    vector<size_t> basic_size;
    codegen->get_type_dimension(sg_type, &basic_type, &basic_size, var_init);
    basic_type = codegen->GetBaseType(basic_type, false);
    sg_type = codegen->CreatePointerType(basic_type);
  } else {
    sg_type = codegen->ArrayToPointerRecur(sg_type, true);
  }
  sg_type_string = codegen->GetStringByType(sg_type);
  //  }
  string opencl_type;

  //  ZP: 20170105
  if (add_volatile != 0) {
    if (readonly_or_writeonly_vars.count(var_init) <= 0) {  //  ZP: 20170105
      opencl_type = (find_flag != 0 ? "" : "__global volatile ") +
                    sg_type_string + (is_top != 0 ? " restrict" : "");
    } else {
      if (is_volatile) {
        sg_type_string = "volatile " + sg_type_string;
      }
      opencl_type = (find_flag != 0 ? "" : "__global ") + sg_type_string +
                    (is_top != 0 ? " restrict" : "");
    }
  } else {
    if (is_volatile) {
      sg_type_string = "volatile " + sg_type_string;
    }
    opencl_type = (find_flag != 0 ? "" : "__global ") + sg_type_string +
                  (is_top != 0 ? " restrict" : "");
  }
  //  printf("opencl_type = %s\n", opencl_type.c_str());
  codegen->RegisterType(opencl_type);
  codegen->SetTypetoVar(var_init, codegen->GetTypeByString(opencl_type));
}

void add_global_attribute_to_return_type(CSageCodeGen *codegen, void *func,
                                         int add_volatile) {
  void *sg_type = codegen->GetFuncReturnType(func);
  string sg_type_string = codegen->GetStringByType(sg_type);
  bool is_volatile = codegen->IsVolatileType(sg_type) != 0;

  int find_flag = 0;
  if (sg_type_string.find("__constant ") == 0) {
    return;
  }
  if (sg_type_string.find("__global ") == 0) {
    find_flag = 1;
  }

  string opencl_type;
  //  ZP: 20170105
  if (add_volatile != 0) {
    if (is_volatile) {
      sg_type_string = "volatile " + sg_type_string;
    }
    opencl_type = (find_flag != 0 ? "" : "__global ") + sg_type_string;
  } else {
    if (is_volatile) {
      sg_type_string = "volatile " + sg_type_string;
    }
    opencl_type = (find_flag != 0 ? "" : "__global ") + sg_type_string;
  }
  codegen->RegisterType(opencl_type);
  codegen->SetFuncReturnType(func, codegen->GetTypeByString(opencl_type));
}

int insert_global_in_func(CSageCodeGen *codegen, void *scope, void *var_init,
                          const set<void *> &readonly_or_writeonly_vars,
                          set<void *> *p_visited_func,
                          set<void *> *p_visited_pointer,
                          map<void *, set<void *>> *p_decls_map,
                          int add_volatile) {
  //  youxiang 20160923
  //  remove declarations without function body
  //  bug640
  int ret = 1;
  void *func = nullptr;
  if (codegen->IsFunctionDeclaration(scope) != 0) {
    func = scope;
    void *func_body = codegen->GetFuncBody(func);
    if (func_body == nullptr) {
      return 0;
    }
    if (p_visited_func->count(func) <= 0) {
      p_visited_func->insert(func);
      vector<void *> all_funcs;
      codegen->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                              &all_funcs);
      for (auto decl : all_funcs) {
        if (codegen->GetFuncBody(decl) != nullptr) {
          continue;
        }
        if (!codegen->isSameFunction(decl, func)) {
          continue;
        }
        if (codegen->isWithInHeaderFile(decl)) {
          string msg =
              "Cannot support function declaration in the header file.\n";
          msg += "Function: " + codegen->_up(decl) + "\n";
          msg += "File : " + codegen->get_file(decl);
          dump_critical_message("CLGEN", "ERROR", msg, 301);
          cout << "Error: " << msg << endl;
          ret = 0;
          continue;
        }
        //  codegen->RemoveStmt(decl);
        (*p_decls_map)[func].insert(decl);
      }
    }
  }
  if (p_visited_pointer->count(var_init) > 0) {
    return 0;
  }
  p_visited_pointer->insert(var_init);
  //  youxiang 20160928 support pointer alias
  vector<void *> vec_pntr;
  if (codegen->IsInitName(var_init) != 0) {
    vector<void *> refs =
        codegen->GetAllRefInScope(var_init, codegen->GetFuncBody(func));
    for (auto one_ref : refs) {
      void *sg_pntr = nullptr;
      void *sg_array;
      vector<void *> sg_indexes;
      int pointer_dim;
      codegen->parse_pntr_ref_by_array_ref(
          one_ref, &sg_array, &sg_pntr sg_indexes, pointer_dim, one_ref);
      if (sg_pntr == nullptr) {
        continue;
      }
      void *sg_type = codegen->GetTypeByExp(sg_pntr);
      if ((codegen->IsPointerType(sg_type) != 0) ||
          (codegen->IsArrayType(sg_type) != 0)) {
        vec_pntr.push_back(sg_pntr);
      } else {
        //  pointer cast
        add_global_attribute_to_pntr_ref_by_ref(codegen, one_ref);
      }
    }
  } else if (codegen->IsFunctionDeclaration(var_init) != 0) {
    vector<void *> vec_calls;
    codegen->GetFuncCallsFromDecl(var_init, scope, &vec_calls);
    for (auto call : vec_calls) {
      void *sg_pntr = nullptr;
      void *sg_func;
      vector<void *> sg_indexes;
      int pointer_dim;
      codegen->parse_pntr_ref_by_func_call(call, sg_func, sg_pntr, sg_indexes,
                                           pointer_dim, call);
      if (sg_pntr == nullptr) {
        continue;
      }
      void *sg_type = codegen->GetTypeByExp(sg_pntr);
      if ((codegen->IsPointerType(sg_type) != 0) ||
          (codegen->IsArrayType(sg_type) != 0)) {
        vec_pntr.push_back(sg_pntr);
      } else {
        //  pointer cast
        add_global_attribute_to_pntr_ref_by_ref(codegen, call);
      }
    }
  } else {
    assert(0);
  }

  for (auto sg_pntr : vec_pntr) {
    //  case 1: int ** a_lias = pntr(a);
    {
      void *var_name;
      void *value_exp;
      if (codegen->parse_assign(codegen->GetParent(sg_pntr), var_name,
                                value_exp) != 0) {
        if (value_exp == sg_pntr && (codegen->IsInitName(var_name) != 0)) {
          void *new_array = var_name;
          int local_ret = insert_global_in_func(
              codegen, func, new_array, readonly_or_writeonly_vars,
              p_visited_func, p_visited_pointer, p_decls_map, add_volatile);
          if (codegen->IsCastExp(sg_pntr) != 0) {
            void *new_pntr = sg_pntr;
            codegen->remove_cast(&new_pntr);
            codegen->ReplaceExp(sg_pntr, codegen->CopyExp(new_pntr));
          }
          if (local_ret != 0) {
            add_global_attribute(codegen, new_array, 0,
                                 readonly_or_writeonly_vars, add_volatile);
          }
          continue;
        }
      }
    }

    //  case 2: func_call(pntr(a));
    {
      int arg_idx = codegen->GetFuncCallParamIndex(sg_pntr);
      if (-1 != arg_idx) {
        void *func_call = codegen->TraceUpToFuncCall(sg_pntr);
        assert(codegen->IsFunctionCall(func_call));
        void *func_decl = codegen->GetFuncDeclByCall(func_call);
        if (func_decl != nullptr) {
          void *new_array = codegen->GetFuncParam(func_decl, arg_idx);
          int local_ret = insert_global_in_func(
              codegen, func_decl, new_array, readonly_or_writeonly_vars,
              p_visited_func, p_visited_pointer, p_decls_map, add_volatile);
          if (codegen->IsCastExp(sg_pntr) != 0) {
            void *new_pntr = sg_pntr;
            codegen->remove_cast(&new_pntr);
            codegen->ReplaceExp(sg_pntr, codegen->CopyExp(new_pntr));
          }
          if (local_ret != 0) {
            add_global_attribute(codegen, new_array, 0,
                                 readonly_or_writeonly_vars, add_volatile);
          }
        }
        continue;
      }
    }
    //  case 3: return pointer
    if (codegen->IsReturnStatement(codegen->GetParent(sg_pntr)) != 0) {
      void *new_array = codegen->TraceUpToFuncDecl(sg_pntr);
      int local_ret = insert_global_in_func(
          codegen, nullptr, new_array, readonly_or_writeonly_vars,
          p_visited_func, p_visited_pointer, p_decls_map, add_volatile);
      if (local_ret != 0) {
        add_global_attribute_to_return_type(codegen, new_array, add_volatile);
      }
      continue;
    }
  }
  return ret;
}

int check_access_type(CSageCodeGen *codegen, void *var_init) {
  int has_read = 0;
  int has_write = 0;
  int readonly_or_writeonly = 0;
  void *func_decl = codegen->TraceUpToFuncDecl(var_init);
  void *func_body = codegen->GetFuncBody(func_decl);
  MarsProgramAnalysis::ArrayRangeAnalysis ris(var_init, codegen, func_body,
                                              func_body, false, false);
  has_read = ris.has_read();
  has_write = ris.has_write();
  if (((has_read != 0) && (has_write == 0)) ||
      ((has_read == 0) && (has_write != 0))) {
    readonly_or_writeonly = 1;
  }
  return readonly_or_writeonly;
}

void check_access_in_func(CSageCodeGen *codegen, void *func, void *var_init,
                          set<void *> *p_readonly_or_writeonly_vars,
                          set<void *> *p_visited) {
  if (p_visited->count(var_init) > 0) {
    return;
  }
  p_visited->insert(var_init);
  void *func_body = codegen->GetFuncBody(func);
  if (func_body == nullptr) {
    return;
  }

  //  youxiang 20160928 support pointer alias
  vector<void *> refs = codegen->GetAllRefInScope(var_init, func_body);
  for (auto &one_ref : refs) {
    void *sg_pntr = nullptr;
    void *sg_array;
    vector<void *> sg_indexes;
    int pointer_dim;
    codegen->parse_pntr_ref_by_array_ref(one_ref, &sg_array, &sg_pntr,
                                         &sg_indexes pointer_dim, one_ref);
    if (sg_pntr == nullptr) {
      continue;
    }
    void *sg_type = codegen->GetTypeByExp(sg_pntr);
    if ((codegen->IsPointerType(sg_type) != 0) ||
        (codegen->IsArrayType(sg_type) != 0)) {
      //  case 1: int ** a_lias = pntr(a);
      {
        void *var_name;
        void *value_exp;
        if (codegen->parse_assign(codegen->GetParent(sg_pntr), var_name,
                                  value_exp) != 0) {
          if (value_exp == sg_pntr && (codegen->IsInitName(var_name) != 0)) {
            void *new_array = var_name;
            check_access_in_func(codegen, func, new_array,
                                 p_readonly_or_writeonly_vars, p_visited);
            if (check_access_type(codegen, new_array) != 0) {
              p_readonly_or_writeonly_vars->insert(new_array);
            }
          }
        }
      }

      //  case 2: func_call(pntr(a));
      {
        int arg_idx = codegen->GetFuncCallParamIndex(sg_pntr);
        if (-1 != arg_idx) {
          void *func_call = codegen->TraceUpToFuncCall(sg_pntr);
          assert(codegen->IsFunctionCall(func_call));
          void *func_decl = codegen->GetFuncDeclByCall(func_call);
          if (func_decl != nullptr) {
            void *new_array = codegen->GetFuncParam(func_decl, arg_idx);
            check_access_in_func(codegen, func_decl, new_array,
                                 p_readonly_or_writeonly_vars, p_visited);
            if (check_access_type(codegen, new_array) != 0) {
              p_readonly_or_writeonly_vars->insert(new_array);
            }
          }
        }
      }
    }
  }
}

int insert_global_attribute_top(CSageCodeGen *codegen, void *pTopFunc,
                                const CInputOptions &options) {
  printf("[opencl_gen.cpp]Enter insert global attribute top\n");
  set<string> set_file_name;
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  set<void *> visited;
  set<void *> readonly_or_writeonly_vars;
  string volatile_flag = options.get_option_key_value("-a", "volatile_flag");
  int add_volatile = static_cast<int>("on" == volatile_flag);
  int ret = 1;
  //  1. readonly port analysis
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    //  printf("[Han Debug][opencl_gen.cpp]sFuncName = %s\n",sFuncName.c_str());
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    void *body = codegen->GetFuncBody(func);
    assert(func && body);
    int idx;
    for (idx = 0; idx < codegen->GetFuncParamNum(func); idx++) {
      void *arg = codegen->GetFuncParam(func, idx);
      void *sg_type = codegen->GetTypebyVar(arg);
      string sg_type_string = codegen->GetStringByType(sg_type);
      //  printf("\n\n[Han Debug][opencl_gen.cpp]sg_type_string =
      //  %s\n",sg_type_string.c_str()); printf("arg = %s, sg_type =
      //  %s\n",codegen->UnparseToString(arg).c_str(),codegen->UnparseToString(sg_type).c_str());
      if (add_volatile != 0) {
        if ((codegen->IsPointerType(sg_type) != 0) ||
            (codegen->IsArrayType(sg_type) != 0)) {
          check_access_in_func(codegen, func, arg, &readonly_or_writeonly_vars,
                               &visited);
          if (check_access_type(codegen, arg) != 0) {
            readonly_or_writeonly_vars.insert(arg);
          }
        }
      }
    }
  }

  visited.clear();
  map<void *, set<void *>> decls_map;
  set<void *> top_set;
  set<void *> visited_func;
  set<void *> visited_pointer;
  //  2. add __global attribute
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    //  printf("[Han Debug][opencl_gen.cpp]sFuncName = %s\n",sFuncName.c_str());
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    set_file_name.insert(codegen->get_file(func));
    set<void *> sub_decls;
    set<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(func, &sub_decls, &sub_calls);
    for (auto sub_decl : sub_decls) {
      if (codegen->GetFuncBody(sub_decl) == nullptr) {
        continue;
      }
      set_file_name.insert(codegen->get_file(sub_decl));
    }
    void *body = codegen->GetFuncBody(func);
    assert(func && body);
    int idx;
    top_set.insert(func);
    for (idx = 0; idx < codegen->GetFuncParamNum(func); idx++) {
      void *arg = codegen->GetFuncParam(func, idx);
      void *sg_type = codegen->GetTypebyVar(arg);
      string sg_type_string = codegen->GetStringByType(sg_type);
      //  printf("\n\n[Han Debug][opencl_gen.cpp]sg_type_string =
      //  %s\n",sg_type_string.c_str()); printf("arg = %s, sg_type =
      //  %s\n",codegen->UnparseToString(arg).c_str(),codegen->UnparseToString(sg_type).c_str());
      if ((codegen->IsPointerType(sg_type) != 0) ||
          (codegen->IsArrayType(sg_type) != 0)) {
        ret &= insert_global_in_func(
            codegen, func, arg, readonly_or_writeonly_vars, &visited_func,
            &visited_pointer, &decls_map, add_volatile);
        add_global_attribute(codegen, arg, 1, readonly_or_writeonly_vars,
                             add_volatile);
      }
    }
  }
  //  update function declaration
  for (auto decl_pair : decls_map) {
    void *new_decl = decl_pair.first;
    //  do not touch top kernel declarations
    if (top_set.count(new_decl) > 0) {
      continue;
    }
    vector<void *> old_decls(decl_pair.second.begin(), decl_pair.second.end());
    for (auto old_decl : old_decls) {
      void *update_decl =
          codegen->CloneFuncDecl(new_decl, codegen->GetGlobal(old_decl), false);
      codegen->ReplaceStmt(old_decl, update_decl);
    }
  }
  //    string kernel_top_file = "kernel_top.cl";
  //    remove(kernel_top_file.c_str());
  //    //  void * kernel_top = codegen->CreateSourceFile(kernel_top_file);
  //    for (auto file_name : set_file_name) {
  //        if(file_name.find(".c") != string::npos) {
  //            string include_file = "#include \"" + file_name.substr(0,
  //                file_name.find(".c")) + ".cl\"";
  //            //  codegen->AddHeader(include_file, kernel_top);
  //            string cmd = "echo '" + include_file + "' >> " +
  //            kernel_top_file; system(cmd.c_str());
  //        }
  //    }
  if (ret == 0) {
    throw std::exception();
  }
  return ret;
}

static void add_const_attribute(CSageCodeGen *codegen, void *var_init) {
  void *sg_type = codegen->GetTypebyVar(var_init);
  string sg_type_string = codegen->GetStringByType(sg_type);

  if (sg_type_string.find("__constant ") == 0) {
    return;
  }
  string opencl_type;
  opencl_type = "__constant ";
  opencl_type += (codegen->IsConstType(sg_type) != 0 ? "" : "const ");
  opencl_type += sg_type_string;
  codegen->RegisterType(opencl_type);
  codegen->SetTypetoVar(var_init, codegen->GetTypeByString(opencl_type));
}

static int is_const_pointer_type(CSageCodeGen *codegen, void *sg_type) {
  if ((codegen->IsPointerType(sg_type) == 0) &&
      (codegen->IsArrayType(sg_type) == 0)) {
    return 0;
  }
  //  if (!codegen->IsConstType(sg_type))
  //  return 0;
  return 1;
}

static int add_const_attribute(CSageCodeGen *codegen, void *var_init,
                               const set<void *> &funcs) {
  int ret = 1;
  for (auto func : funcs) {
    void *func_body = codegen->GetFuncBody(func);
    if (func_body == nullptr) {
      return 0;
    }
    vector<void *> refs = codegen->GetAllRefInScope(var_init, func_body);
    for (auto &one_ref : refs) {
      void *sg_pntr = nullptr;
      void *sg_array;
      vector<void *> sg_indexes;
      int pointer_dim;
      codegen->parse_pntr_ref_by_array_ref(
          one_ref, &sg_array, &sg_pntr sg_indexes, pointer_dim, one_ref);
      if (sg_pntr == nullptr) {
        continue;
      }
      void *sg_type = codegen->GetTypeByExp(sg_pntr);
      if (is_const_pointer_type(codegen, sg_type) != 0) {
        //  case 1: int ** a_lias = pntr(a);
        {
          void *var_name;
          void *value_exp;
          if (codegen->parse_assign(codegen->GetParent(sg_pntr), var_name,
                                    value_exp) != 0) {
            if (value_exp == sg_pntr && (codegen->IsInitName(var_name) != 0)) {
              void *new_array = var_name;
              set<void *> sub_funcs;
              sub_funcs.insert(func);
              add_const_attribute(codegen, new_array, sub_funcs);
              if (codegen->IsCastExp(sg_pntr) != 0) {
                void *new_pntr = sg_pntr;
                codegen->remove_cast(&new_pntr);
                codegen->ReplaceExp(sg_pntr, codegen->CopyExp(new_pntr));
              }
              add_const_attribute(codegen, new_array);
            }
          }
        }

        //  case 2: func_call(pntr(a));
        {
          int arg_idx = codegen->GetFuncCallParamIndex(sg_pntr);
          if (-1 != arg_idx) {
            void *func_call = codegen->TraceUpToFuncCall(sg_pntr);
            assert(codegen->IsFunctionCall(func_call));
            void *func_decl = codegen->GetFuncDeclByCall(func_call);
            if (func_decl != nullptr) {
              void *new_array = codegen->GetFuncParam(func_decl, arg_idx);
              set<void *> sub_funcs;
              sub_funcs.insert(func_decl);
              add_const_attribute(codegen, new_array, sub_funcs);
              if (codegen->IsCastExp(sg_pntr) != 0) {
                void *new_pntr = sg_pntr;
                codegen->remove_cast(&new_pntr);
                codegen->ReplaceExp(sg_pntr, codegen->CopyExp(new_pntr));
              }
              add_const_attribute(codegen, new_array);
            }
          }
        }
      }
    }
  }
  return ret;
}

static void collect_const_global_variables(CSageCodeGen *codegen, void *func,
                                           const set<void *> &const_global_vars,
                                           const set<void *> &funcs) {
  if (funcs.count(func) > 0) {
    return;
  }
  void *body = codegen->GetFuncBody(func);
  if (body == nullptr) {
    return;
  }
  funcs.insert(func);
  vector<void *> vec_refs;
  codegen->GetNodesByType(body, "preorder", "SgVarRefExp", &vec_refs);
  for (auto ref : vec_refs) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    if (codegen->IsGlobalInitName(var_init) == 0) {
      continue;
    }
    void *var_type = codegen->GetTypebyVar(var_init);
    if (codegen->IsConstType(var_type) == 0) {
      continue;
    }
    const_global_vars.insert(var_init);
  }
  vector<void *> vec_calls;
  codegen->GetNodesByType(body, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto call : vec_calls) {
    void *callee = codegen->GetFuncDeclByCall(call);
    if (callee == nullptr) {
      continue;
    }
    collect_const_global_variables(codegen, callee, const_global_vars, funcs);
  }
}

static int add_const_attribute_top(CSageCodeGen *codegen, void *pTopFunc) {
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  set<void *> const_global_vars;
  set<void *> funcs;
  int ret = 1;
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    collect_const_global_variables(codegen, func, const_global_vars, funcs);
  }

  for (auto var : const_global_vars) {
    add_const_attribute(codegen, var, funcs);
  }
  return ret;
}

int change_register_in_func(CSageCodeGen *codegen, void *func,
                            set<void *> *p_visited) {
  if ((func == nullptr) || p_visited->count(func) > 0) {
    return 0;
  }
  p_visited->insert(func);
  void *body = codegen->GetFuncBody(func);
  if (body == nullptr) {
    return 0;
  }
  int Changed = 0;
  vector<void *> vec_vars;
  codegen->GetNodesByType(body, "preorder", "SgVariableDeclaration", &vec_vars);
  for (auto &var : vec_vars) {
    void *var_init = codegen->GetVariableInitializedName(var);
    void *type = codegen->GetTypebyVar(var_init);
    if (codegen->is_register(var) == 0) {
      continue;
    }
#if 0
      void *initializer = codegen->GetInitializer(var_init);
      if (initializer) {
        initializer = codegen->CopyExp(initializer);
      }
      string var_name = codegen->GetVariableName(var_init);
      string new_var_name = var_name;
      codegen->get_valid_local_name(func, &new_var_name);
      void *new_type = type;
      if (codegen->IsScalarType(type) || codegen->IsArrayType(type)) {
        vector<size_t> basic_size;
        void *basic_type;
        codegen->get_type_dimension(type, &basic_type, &basic_size, var);
        basic_type = codegen->GetBaseType(basic_type, false);
        string base_type_str = codegen->GetStringByType(basic_type);
        string new_type_str = base_type_str + " __attribute__((register))";
        codegen->RegisterType(new_type_str);
        new_type = codegen->GetTypeByString(new_type_str);
        if (!basic_size.empty()) {
            vector<int> dims;
            for (int i = basic_size.size() - 1; i >= 0; --i)
              dims.push_back(basic_size[i]);
            new_type = codegen->CreateArrayType(new_type, dims);
        }
      }
      void *new_var = codegen->CreateVariableDecl(new_type, new_var_name,
                      initializer, body);
      codegen->InsertStmt(new_var, var);
      void *new_var_init = codegen->GetVariableInitializedName(new_var);
      vector<void*> var_refs;
      codegen->GetNodesByType(body, "preorder", "SgVarRefExp", &var_refs);
      for (auto &ref : var_refs) {
        if (codegen->GetVariableInitializedName(ref) == var_init)
          codegen->ReplaceExp(ref, codegen->CreateVariableRef(new_var_init));
      }
      codegen->RemoveStmt(var);
      codegen->SetVariableName(new_var_init, var_name);
#else
    codegen->unset_register(var);
    if ((codegen->IsScalarType(type) == 0) &&
        (codegen->IsArrayType(type) == 0) &&
        (codegen->IsPointerType(type) == 0)) {
      continue;
    }
    //  youxiang :20161004 donot touch pointer alias
    string type_string = codegen->GetStringByType(type);
    if (type_string.find("__global ") == 0) {
      continue;
    }
    if (type_string.find("__constant ") == 0) {
      continue;
    }
    vector<size_t> basic_size;
    void *basic_type;
    codegen->get_type_dimension(type, &basic_type, &basic_size, var);
    basic_type = codegen->GetBaseType(basic_type, false);
    string base_type_str = codegen->GetStringByType(basic_type);
    string new_type_str = base_type_str + " __attribute__((register))";
    codegen->RegisterType(new_type_str);
    void *new_type = codegen->GetTypeByString(new_type_str);
    if (codegen->IsArrayType(type) != 0) {
      vector<size_t> dims;
      for (size_t i = basic_size.size(); i > 0; --i) {
        dims.push_back(basic_size[i - 1]);
      }
      new_type = codegen->CreateArrayType(new_type, dims);
    } else if (codegen->IsPointerType(type) != 0) {
      new_type = codegen->CreatePointerType(new_type);
    }
    codegen->SetTypetoVar(var_init, new_type);
#endif
    Changed = 1;
  }

  vector<void *> vecCalls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vecCalls);
  for (size_t j = 0; j < vecCalls.size(); j++) {
    void *sub_func = codegen->GetFuncDeclByCall(vecCalls[j]);
    Changed |= change_register_in_func(codegen, sub_func, p_visited);
  }
  return Changed;
}

int change_register_top(CSageCodeGen *codegen, void *pTopFunc) {
  vector<string> vec_file_name;
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  int Changed = 0;
  set<void *> visited;
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    string sFuncName = pTask->poly_info.properties["kernel_name"];
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    Changed |= change_register_in_func(codegen, func, &visited);
  }
  return Changed;
}

void memcpy_replace(CSageCodeGen *codegen, void *func, void *pTopFunc) {
  string func_name = codegen->GetFuncNameByCall(pTopFunc);
  //  printf("[2Han Debug][opencl_gen.cpp]func_name = %s\n",func_name.c_str());
  if (func_name ==
      "memcpy") {  //  if function name is memcpy, replace it with for loop
    void *parentStmt =
        codegen->TraceUpByTypeCompatible(pTopFunc, V_SgStatement);
    //  printf("[2Han Debug][opencl_gen.cpp]find memcpy function call\n");
    void *sg_array1 = nullptr;
    void *sg_pntr1 = nullptr;
    vector<void *> sg_idx1;
    int pointer_dim1 = 0;
    void *sg_array2 = nullptr;
    void *sg_pntr2 = nullptr;
    vector<void *> sg_idx2;
    int pointer_dim2 = 0;
    void *sg_length = nullptr;

    //  int ret = codegen->parse_memcpy(
    //    pTopFunc, sg_length, sg_array1, sg_pntr1, sg_idx1,
    //    pointer_dim1, sg_array2, sg_pntr2, sg_idx2, pointer_dim2);
    /***********************************************************************/
    /***************************check memcpy arguments
     * value*******************************/
    /***********************************************************************/
    //  read destination array
    //  printf("[2Han Debug][opencl_gen.cpp]dim1 = %d, dim2 =
    //  %d\n",pointer_dim1, pointer_dim2); string sg_length_string =
    //  codegen->UnparseToString(sg_length); printf("[2Han
    //  Debug][opencl_gen.cpp]sg_length = %s\n",sg_length_string.c_str());
    string array1_name = codegen->GetVariableName(sg_array1);
    string array1_type = codegen->GetVariableTypeName(sg_array1);
    //  printf("[2Han Debug][opencl_gen.cpp]array1_name = %s, array1_type =
    //  %s\n",array1_name.c_str(),array1_type.c_str());
    string array1_pntr = codegen->UnparseToString(sg_pntr1);
    //  printf("[2Han Debug][opencl_gen.cpp]array1_pntr =
    //  %s\n",array1_pntr.c_str()); printf("[2Han
    //  Debug][opencl_gen.cpp]array1_array_dim = %d\n",sg_idx1.size());
    for (size_t i = 0; i < sg_idx1.size(); i++) {
      string array1_index0 = codegen->UnparseToString(sg_idx1[i]);
      //  printf("[2Han Debug][opencl_gen.cpp]array1_array_idx1[%d] =
      //  %s\n",i,array1_index0.c_str());
    }
    //  read source array
    //  string array2_name = codegen->GetVariableName(sg_array2);
    string array2_type = codegen->GetVariableTypeName(sg_array2);
    int type_size = codegen->get_type_unit_size(
        codegen->GetTypebyVar(sg_array2), sg_array2);
    //  printf("[2Han Debug][opencl_gen.cpp]array2_name = %s, array2_type =
    //  %s\n, size = %d",array2_name.c_str(),array2_type.c_str(),type_size);
    string array2_pntr = codegen->UnparseToString(sg_pntr2);
    //  printf("[2Han Debug][opencl_gen.cpp]array2_pntr =
    //  %s\n",array2_pntr.c_str()); printf("[2Han
    //  Debug][opencl_gen.cpp]array2_array_dim = %d\n",sg_idx2.size());
    for (size_t i = 0; i < sg_idx2.size(); i++) {
      string array2_index0 = codegen->UnparseToString(sg_idx2[i]);
      //  printf("[2Han Debug][opencl_gen.cpp]array2_array_idx2[%d] =
      //  %s\n",i,array2_index0.c_str());
    }

    /***********************************************************************/
    /**********************************create for
     * loop*************************************/
    /***********************************************************************/
    assert((size_t)pointer_dim1 <= sg_idx1.size() - 1);  //  check pointer_dim
    assert((size_t)pointer_dim2 <= sg_idx2.size() - 1);
    string for_index = "__i_fcs_" + my_itoa(count_memcpy);
    void *sg_index_i = codegen->CreateVariableDecl("int", for_index, NULL,
                                                   codegen->GetFuncBody(func));
    codegen->InsertStmt(sg_index_i, parentStmt);
    void *index_name = codegen->GetVariableInitializedName(sg_index_i);
    //  void *index_type = codegen->GetTypebyVar(index_name);
    //  generate leftside of "="
    void *array1_index_left = codegen->CreateVariableRef(sg_index_i);
    vector<void *> array1_index;
    for (size_t i = 0; i < sg_idx1.size(); i++) {
      //  void *array1_left;
      if (i == sg_idx1.size() - 1) {
        array1_index.push_back(codegen->CreateExp(
            V_SgAddOp, array1_index_left, codegen->CopyExp(sg_idx1[i])));
      } else {
        array1_index.push_back(codegen->CopyExp(sg_idx1[i]));
      }
    }
    void *array1_ref = codegen->CreateArrayRef(
        codegen->CreateVariableRef(sg_array1), array1_index);
    string string_array1_exp = codegen->UnparseToString(array1_ref);
    //  printf("[2Han debug][opencl_gen.cpp]test array1 exp string =
    //  %s\n",string_array1_exp.c_str()); generate rightside of "="
    void *array2_index_left = codegen->CreateVariableRef(sg_index_i);
    vector<void *> array2_index;
    for (size_t i = 0; i < sg_idx2.size(); i++) {
      if (i == sg_idx2.size() - 1) {
        array2_index.push_back(codegen->CreateExp(
            V_SgAddOp, array2_index_left, codegen->CopyExp(sg_idx2[i])));
      } else {
        array2_index.push_back(codegen->CopyExp(sg_idx2[i]));
      }
    }
    void *array2_ref = codegen->CreateArrayRef(
        codegen->CreateVariableRef(sg_array2), array2_index);
    string string_array2_exp = codegen->UnparseToString(array2_ref);
    //  printf("[2Han debug][opencl_gen.cpp]test array2 exp string =
    //  %s\n",string_array2_exp.c_str()); generate "=" assignment expression
    void *transfer_exp =
        codegen->CreateExp(V_SgAssignOp, array1_ref, array2_ref);
    string string_transfer_exp = codegen->UnparseToString(transfer_exp);
    //  printf("[2Han Debug][opencl_gen.cpp]test transfer string =
    //  %s\n\n",string_transfer_exp.c_str()); generate for loop and replace old
    //  memcpy function call
    void *sg_length_c =
        codegen->CreateExp(V_SgDivideOp, codegen->CopyExp(sg_length),
                           codegen->CreateConst(type_size));
    void *for_body_stmt = codegen->CreateStmt(V_SgExprStatement, transfer_exp,
                                              NULL, NULL, NULL, NULL);
    void *for_loop_stmt = codegen->CreateStmt(
        V_SgForStatement, index_name,  //  index_type,
        codegen->CreateConst(0), sg_length_c, for_body_stmt, NULL);
    codegen->ReplaceStmt(parentStmt, for_loop_stmt);
    count_memcpy++;
  } else {  //  if function name is not memcpy, then get int he function body
            //  and search memcpy again
    string new_func_name = codegen->GetFuncNameByCall(pTopFunc);
    //  printf("[2Han Debug][opencl_gen.cpp]not memcpy call function string =
    //  %s\n",new_func_name.c_str());
    void *new_func = codegen->GetFuncDeclByCall(pTopFunc);
    //  void * new_func = codegen->GetFuncDeclByName(new_func_name);
    //  assert(new_func);
    if (codegen->IsFunctionDeclaration(new_func) != 0) {
      vector<void *> vecCalls;
      codegen->GetNodesByType(new_func, "preorder", "SgFunctionCallExp",
                              &vecCalls);
      for (size_t j = 0; j < vecCalls.size(); j++) {
        memcpy_replace(codegen, new_func, vecCalls[j]);
      }
    }
  }
}

int memcpy_replace_top(CSageCodeGen *codegen, void *pTopFunc) {
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  map<string, int> mapKernel;
  for (size_t k = 0; k < taskset.size(); k++) {
    tldm_polyhedral_info task_poly_info;
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName;  //  = pTask->GetParam("name") + "_kernel";
    task_poly_info = pTask->poly_info;
    if (pTask->poly_info.type != "task") {
      continue;
    }
    sFuncName = pTask->poly_info.properties["kernel_name"];
    //  printf("[2Han Debug][opencl_gen.cpp]sFuncName =
    //  %s\n",sFuncName.c_str());
    string sKernel = sFuncName;
    if (mapKernel.find(sKernel) == mapKernel.end()) {
      mapKernel[sKernel] = 1;
    } else {
      continue;
    }
    void *func = codegen->GetFuncDeclByName(sFuncName);
    assert(func);
    vector<void *> vecCalls;
    codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vecCalls);
    for (size_t j = 0; j < vecCalls.size(); j++) {
      //  printf("[2Han Debug][opencl_gen.cpp]vecCalls index = %d\n",j);
      memcpy_replace(codegen, func, vecCalls[j]);
    }
  }
  return 0;
}

void const_replace(CSageCodeGen *codegen, void *pTopFunc) {
  vector<void *> vec_initname;
  int count = 0;
  codegen->GetNodesByType(NULL, "preorder", "SgInitializedName", &vec_initname);
  for (size_t j = 0; j < vec_initname.size(); j++) {
    if (codegen->IsGlobalInitName(vec_initname[j]) != 0) {
      string file_name = codegen->get_file(vec_initname[j]);
      //  printf("[3Han Debug][opencl_gen.cpp]file_name_string =
      //  %s\n",file_name.c_str());
      if (codegen->IsFromInputFile(vec_initname[j]) != 0) {
        string init_name_string = codegen->UnparseToString(vec_initname[j]);
        void *init_type = codegen->GetTypebyVar(vec_initname[j]);
        string init_type_string = codegen->GetStringByType(init_type);
        if (init_type_string.find("const") != string::npos) {
          count++;
          if (codegen->IsArrayType(init_type) != 0) {
            void *base_type;
            vector<size_t> sizes;
            codegen->get_type_dimension(init_type, &base_type, &sizes, NULL);
            string base_type_string = codegen->UnparseToString(base_type);
            //  printf("[3Han Debug][opencl_gen.cpp]base_type_string =
            //  %s\n",base_type_string.c_str()); printf("[3Han
            //  Debug][opencl_gen.cpp]sizes.size() = %d\n",sizes.size());
            base_type_string = codegen->GetStringByType(base_type);
            base_type_string = "__constant " + base_type_string;
            codegen->RegisterType(base_type_string);
            void *new_array_type = codegen->CreateArrayType(
                codegen->GetTypeByString(base_type_string), sizes);
            codegen->SetTypetoVar(vec_initname[j], new_array_type);
            //  printf("[3Han Debug][opencl_gen.cpp]base_type_string =
            //  %s\n",base_type_string.c_str());
          } else {
            string no_cast_type_string = init_type_string.erase(0, 5);
            //  printf("[3Han Debug][opencl_gen.cpp]init_name = %s, no_cast_type
            //  = %s\n",init_name_string.c_str(),no_cast_type_string.c_str());
            string opencl_type = "__constant " + no_cast_type_string;
            //  printf("[3Han Debug][opencl_gen.cpp]opencl_type =
            //  %s\n",opencl_type.c_str());
            codegen->RegisterType(opencl_type);
            codegen->SetTypetoVar(vec_initname[j],
                                  codegen->GetTypeByString(opencl_type));
            //  void * new_init_type = codegen->GetTypebyVar(vec_initname[j]);
            //  string new_type_string =
            //  codegen->GetStringByType(new_init_type); printf("[3Han
            //  Debug][opencl_gen.cpp]new_type_string =
            //  %s\n",new_type_string.c_str());
          }
        }
      }
    }
  }
}

static int remove_const_global(CSageCodeGen *codegen, void *pTopFunc) {
  vector<void *> vec_initname;
  //  int count=0;
  set<string> set_file_name;
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  set<void *> visited;
  //  int ret = 1;
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    void *func = codegen->GetFuncDeclByName(sFuncName);
    assert(func);
    set_file_name.insert(codegen->get_file(func));
    set<void *> sub_decls;
    set<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(func, &sub_decls, &sub_calls);
    for (auto sub_decl : sub_decls) {
      if (codegen->GetFuncBody(sub_decl) == nullptr) {
        continue;
      }
      set_file_name.insert(codegen->get_file(sub_decl));
    }
  }
  codegen->GetNodesByType(NULL, "preorder", "SgInitializedName", &vec_initname);
  for (auto var_init : vec_initname) {
    if (codegen->IsGlobalInitName(var_init) == 0) {
      continue;
    }
    if (set_file_name.count(codegen->get_file(var_init)) <= 0) {
      continue;
    }
    if (codegen->IsConstType(codegen->GetTypebyVar(var_init)) == 0) {
      continue;
    }
    vector<void *> fp_list;
    void *dummy_decl = codegen->CreateFuncDecl(
        "void", "__merlin_dummy_" + codegen->GetVariableName(var_init), fp_list,
        codegen->GetGlobal(var_init), false, nullptr);
    codegen->SetStatic(dummy_decl);
    void *var_decl = codegen->GetVariableDecl(var_init);
    codegen->ReplaceStmt(var_decl, dummy_decl);
  }
  return 1;
}

//  1. find all the kernel calls
//  2. replace memory allocation for the kernel ports (pointer)
//  3. allocate additional single_element buffer for scalar
//  4. replace kernel call
//  5. add the include file "kestrel_app.h"
int host_app_wrapper_for_kestrel(CSageCodeGen *codegen, void *pTopFunc) {
  size_t i;
  //  printf("Hello kestrel! \n");

  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    //  /////////////////////////////////////////////////  /
    //  1. find all the kernel calls
    //  /////////////////////////////////////////////////  /
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);

    if ("tldm" != sFilter && "cmost" != sFilter) {
      continue;
    }

    if ("task_block" != sCmd) {
      continue;
    }

    void *sg_kernel_call = codegen->find_kernel_call(vecTldmPragmas[i].first);
    assert(sg_kernel_call);
    void *sg_task = sg_kernel_call;
    void *sg_task_stmt = codegen->TraceUpToStatement(sg_task);
    void *sg_kernel = codegen->GetFuncDeclByCall(sg_task);

    //  cout << "-- " << codegen->_p(sg_task) << " : " <<
    //  codegen->get_file(sg_task) << endl;
    cout << codegen->_p(sg_task) << " : " << codegen->_p(sg_kernel) << endl;

    //  /////////////////////////////////////////////////  /
    //  2. replace memory allocation for the kernel ports (pointer)
    //  3. allocate additional single_element buffer for scalar
    //  /////////////////////////////////////////////////  /
    int read_index = 0;
    int write_index = 0;
    int index_alloc = 0;
    for (int j = 0; j < codegen->GetFuncParamNum(sg_kernel); j++) {
      void *actual_arg = codegen->GetFuncCallParam(sg_task, j);
      void *formal_arg = codegen->GetFuncParam(sg_kernel, j);

      codegen->remove_cast(&actual_arg);

      //  cout << "  -- " << codegen->_p(actual_arg) << " : " <<
      //  codegen->_p(formal_arg) << endl;

      if (codegen->IsVarRefExp(actual_arg) == 0) {
        string sFile = codegen->get_file(actual_arg);
        int nLine = codegen->get_line(actual_arg);

        string msg = "Kernel argument can not be expression: " + string("") +
                     "\n\t Argument '" + codegen->UnparseToString(actual_arg) +
                     "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                     "' (" + sFile + ":" + my_itoa(nLine) + ")\n";
        dump_critical_message("RUNTM", "ERROR", msg, 301);
        cout << msg << endl;
        exit(-1);
      }

      //  get type for the actual argument
      //  int is_scalar;
      void *sg_array;
      void *base_type;
      vector<size_t> nSizes;
      codegen->get_type_dimension_by_ref(actual_arg, sg_array, base_type,
                                         nSizes);

      cout << "  -- " << codegen->_p(actual_arg) << " : "
           << codegen->_p(base_type) << " dim=" << nSizes.size() << endl;

      const string var_suffix =
          "_" + codegen->GetFuncName(sg_kernel) + "_kestrel";

      if (nSizes.empty()) {
        //  data_type * __var_t = kestrel_alloc(1, length,
        //  length*sizeof(data_type), 0);
        //  *__var_t = actual_arg;

        vector<void *> params;
        params.push_back(
            codegen->CreateStringExp(codegen->GetFuncName(sg_kernel)));
        params.push_back(codegen->CreateConst(read_index));
        params.push_back(codegen->CreateConst(1));
        params.push_back(codegen->CreateConst(1));
        params.push_back(codegen->CreateExp(V_SgSizeOfOp, base_type));
        params.push_back(codegen->CreateConst(0));
        void *sg_call = codegen->CreateFuncCall(
            "kestrel_alloc", base_type, params, codegen->GetScope(sg_task));
        sg_call = codegen->CreateCastExp(sg_call,
                                         codegen->CreatePointerType(base_type));
        string s_var = "__" + codegen->UnparseToString(actual_arg) + var_suffix;
        void *sg_var_decl = codegen->CreateVariableDecl(
            codegen->CreatePointerType(base_type), s_var, sg_call,
            codegen->GetScope(sg_task));
        codegen->InsertStmt(sg_var_decl, sg_task_stmt);

        void *sg_assign = codegen->CreateExp(
            V_SgAssignOp, codegen->CreateExp(V_SgPointerDerefExp, sg_var_decl),
            codegen->CopyExp(actual_arg));
        void *sg_stmt = codegen->CreateStmt(V_SgExprStatement, sg_assign);
        codegen->InsertStmt(sg_stmt, sg_task_stmt);

        cout << "  -->" << codegen->_p(sg_var_decl) << endl;
        cout << "  -->" << codegen->_p(sg_stmt) << endl;
        read_index++;
      } else if (nSizes.size() == 1) {
        int dim_size = nSizes[0];
        if (dim_size == 0) {
          string sFile = codegen->get_file(actual_arg);
          int nLine = codegen->get_line(actual_arg);
          string msg = "Unknown dimension size of array/pointer variables: " +
                       string("") + "\n\t variable '" +
                       codegen->UnparseToString(actual_arg) + "' for kernel '" +
                       codegen->GetFuncName(sg_kernel) + "' (" + sFile + ":" +
                       my_itoa(nLine) + ")\n";
          dump_critical_message("RUNTM", "ERROR", msg, 302);
          cout << msg << endl;
          exit(-1);
        }

        //  1. get the original declaration
        void *org_var_decl = codegen->get_actual_var_decl_by_ref(
            actual_arg);  //  by default, find the unique one
        if (org_var_decl == nullptr) {
          string sFile = codegen->get_file(actual_arg);
          int nLine = codegen->get_line(actual_arg);
          string msg =
              "Can not find the unique actual declaration for the variable: " +
              string("") + "\n\t variable '" +
              codegen->UnparseToString(actual_arg) + "' for kernel '" +
              codegen->GetFuncName(sg_kernel) + "' (" + sFile + ":" +
              my_itoa(nLine) + ")\n";
          dump_critical_message("RUNTM", "ERROR", msg, 303);
          cout << msg << endl;
          exit(-1);
        }

        if (codegen->IsGlobalInitName(
                codegen->GetVariableInitializedName(org_var_decl)) != 0) {
          string sFile = codegen->get_file(actual_arg);
          int nLine = codegen->get_line(actual_arg);
          string msg = "The actual declaration for the kernel interface port "
                       "can not be global variable: " +
                       string("") + "\n\t variable '" +
                       codegen->UnparseToString(actual_arg) + "' for kernel '" +
                       codegen->GetFuncName(sg_kernel) + "' (" + sFile + ":" +
                       my_itoa(nLine) + ")\n";
          dump_critical_message("RUNTM", "ERROR", msg, 303);
          cout << msg << endl;
          exit(-1);
        }

        //  is_write flag: indicating whether the kernel has write to the port
        //  variable
        int has_read = 0;
        int has_write = 0;
        {
          void *func_body = codegen->GetFuncBody(sg_kernel);
          MarsProgramAnalysis::ArrayRangeAnalysis ris(
              formal_arg, codegen, func_body, func_body, false, false);

          has_read = ris.has_read();
          has_write = ris.has_write();

          if ((has_read != 0) &&
              (has_write != 0)) {  //  currently Kestrel NAM does not support
                                   //  READ_WRITE port
            string sFile = codegen->get_file(actual_arg);
            int nLine = codegen->get_line(actual_arg);
            string msg = "Kernel interface variable with both read and write "
                         "accesses in the kernel is not allowed: " +
                         string("") + "\n\t variable '" +
                         codegen->UnparseToString(actual_arg) +
                         "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                         "' (" + sFile + ":" + my_itoa(nLine) + ")\n";
            dump_critical_message("RUNTM", "ERROR", msg, 303);
            cout << msg << endl;
            exit(-1);
          }
          if (has_write != 0) {
            index_alloc = write_index;
            write_index++;
          } else if (has_read != 0) {
            index_alloc = read_index;
            read_index++;
          }
        }

        //  2. insert new declaration
        vector<void *> params;
        params.push_back(
            codegen->CreateStringExp(codegen->GetFuncName(sg_kernel)));
        params.push_back(codegen->CreateConst(index_alloc));
        params.push_back(codegen->CreateConst(1));
        params.push_back(codegen->CreateConst(dim_size));
        //  params.push_back(codegen->CreateExp(V_SgMultiplyOp,
        //            codegen->CreateConst(dim_size),
        //            codegen->CreateExp(V_SgSizeOfOp, base_type)));
        params.push_back(codegen->CreateExp(V_SgSizeOfOp, base_type));
        params.push_back(codegen->CreateConst(has_write));
        void *sg_call =
            codegen->CreateFuncCall("kestrel_alloc", base_type, params,
                                    codegen->GetScope(org_var_decl));
        sg_call = codegen->CreateCastExp(sg_call,
                                         codegen->CreatePointerType(base_type));
        string s_var = "__" + codegen->UnparseToString(actual_arg) + var_suffix;
        void *sg_var_decl = codegen->CreateVariableDecl(
            codegen->CreatePointerType(base_type), s_var, sg_call,
            codegen->GetScope(org_var_decl));
        codegen->InsertAfterStmt(sg_var_decl, org_var_decl);

        //  3. replace all the original references with the new one, except
        //  malloc
        void *org_array = codegen->GetVariableInitializedName(org_var_decl);
        vector<void *> org_ref = codegen->GetAllRefInScope(
            org_array, codegen->GetScope(org_var_decl));

        for (size_t t = 0; t < org_ref.size(); t++) {
          void *curr_ref = org_ref[t];

          void *sg_pntr;
          {
            void *sg_array;
            vector<void *> sg_indexes;
            int pointer_dim;
            codegen->parse_pntr_ref_by_array_ref(
                curr_ref, &sg_array, &sg_pntr sg_indexes, pointer_dim);
          }

          int arg_idx = codegen->GetFuncCallParamIndex(sg_pntr);
          if (arg_idx != -1) {
            void *curr_call = codegen->TraceUpToFuncCall(sg_pntr);
            string curr_func_name = codegen->GetFuncNameByCall(curr_call);

            if (curr_func_name == "malloc") {
              continue;
            }
            if (curr_func_name == codegen->GetFuncName(sg_kernel)) {
              continue;
            }
            if (curr_func_name.find("cmost_dump") == 0) {
              continue;
            }
            if (curr_func_name.find("cmost_free") == 0) {
              continue;
            }
            if (curr_func_name == "cmost_malloc") {
              continue;
            }
            if (curr_func_name.find("cmost_malloc_") == 0) {
              void *input_file_name = codegen->GetFuncCallParam(curr_call, 1);

              //  create cmost_load_file call
              if (codegen->UnparseToString(input_file_name) != "\"\"") {
                vector<void *> curr_params;
                curr_params.push_back(codegen->CreateVariableRef(sg_var_decl));
                curr_params.push_back(codegen->CopyExp(input_file_name));
                curr_params.push_back(codegen->CreateExp(
                    V_SgMultiplyOp, codegen->CreateConst(dim_size),
                    codegen->CreateExp(V_SgSizeOfOp, base_type)));
                void *new_file_call = codegen->CreateFuncCall(
                    "cmost_load_file", codegen->GetTypeByString("void"),
                    curr_params, codegen->GetScope(curr_ref));
                void *new_stmt =
                    codegen->CreateStmt(V_SgExprStatement, new_file_call);
                codegen->InsertAfterStmt(
                    new_stmt, codegen->TraceUpToStatement(curr_call));
              }

              continue;
            }
          }

          codegen->ReplaceExp(curr_ref,
                              codegen->CreateVariableRef(sg_var_decl));
        }
      } else {
        string sFile = codegen->get_file(actual_arg);
        int nLine = codegen->get_line(actual_arg);
        string msg = "Multiple dimensional array is not supported in kernel "
                     "interface: " +
                     string("") + "\n\t variable '" +
                     codegen->UnparseToString(actual_arg) + "' for kernel '" +
                     codegen->GetFuncName(sg_kernel) + "' (" + sFile + ":" +
                     my_itoa(nLine) + ")\n";
        dump_critical_message("RUNTM", "ERROR", msg, 303);
        cout << msg << endl;
        exit(-1);
      }
    }

    //  /////////////////////////////////////////////////  /
    //  5. add the include file "kestrel_app.h"
    //  /////////////////////////////////////////////////  /

    codegen->AddHeader("#include \"kestrel_app.h\"", sg_task);

    //  /////////////////////////////////////////////////  /
    //  4. replace kernel call
    //  /////////////////////////////////////////////////  /
    {
      //  kestrel_start_kernel("kernel_name");
      string str_kernel_call = codegen->UnparseToString(sg_task_stmt);

      vector<void *> params;
      params.push_back(
          codegen->CreateStringExp(codegen->GetFuncName(sg_kernel)));
      void *sg_call = codegen->CreateFuncCall(
          "kestrel_start_kernel", codegen->GetTypeByString("void"), params,
          codegen->GetScope(sg_task));
      void *sg_stmt = codegen->CreateStmt(V_SgExprStatement, sg_call);
      codegen->ReplaceStmt(sg_task_stmt, sg_stmt);
      codegen->AddComment(str_kernel_call, sg_stmt);
    }
  }

  printf("\n");

  return 1;
}

void add_global(CSageCodeGen *codegen, void *pTopFunc,
                const CInputOptions &options) {
  //  printf("[Han Debug][opencl_gen.cpp]Enter insert global attribute top\n");
  set<string> set_file_name;
  vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
  set<void *> visited;
  set<void *> readonly_or_writeonly_vars;
  int ret = 1;
  string volatile_flag = options.get_option_key_value("-a", "volatile_flag");
  int add_volatile = static_cast<int>("on" == volatile_flag);
  //  1. readonly port analysis
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    //  printf("[Han Debug][opencl_gen.cpp]sFuncName = %s\n",sFuncName.c_str());
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    void *body = codegen->GetFuncBody(func);
    assert(func && body);
    int idx;
    for (idx = 0; idx < codegen->GetFuncParamNum(func); idx++) {
      void *arg = codegen->GetFuncParam(func, idx);
      void *sg_type = codegen->GetTypebyVar(arg);
      string sg_type_string = codegen->GetStringByType(sg_type);
      //  printf("\n\n[Han Debug][opencl_gen.cpp]sg_type_string =
      //  %s\n",sg_type_string.c_str()); printf("arg = %s, sg_type =
      //  %s\n",codegen->UnparseToString(arg).c_str(),codegen->UnparseToString(sg_type).c_str());
      if (add_volatile != 0) {
        if ((codegen->IsPointerType(sg_type) != 0) ||
            (codegen->IsArrayType(sg_type) != 0)) {
          check_access_in_func(codegen, func, arg, &readonly_or_writeonly_vars,
                               &visited);
          if (check_access_type(codegen, arg) != 0) {
            readonly_or_writeonly_vars.insert(arg);
          }
        }
      }
    }
  }

  visited.clear();
  set<void *> visited_func;
  set<void *> visited_pointer;
  map<void *, set<void *>> decls_map;
  set<void *> top_set;
  //  2. add __global attribute
  for (size_t k = 0; k < taskset.size(); k++) {
    CTldmTaskAnn *pTask = taskset[k];
    string sFuncName = pTask->GetParam("name") + "_kernel";
    sFuncName = pTask->poly_info.properties["kernel_name"];
    if (pTask->poly_info.type != "task") {
      continue;
    }
    //  printf("[Han Debug][opencl_gen.cpp]sFuncName = %s\n",sFuncName.c_str());
    void *func = codegen->GetFuncDeclByName(sFuncName);  //  kernel_funcs[0];
    assert(func);
    set_file_name.insert(codegen->get_file(func));
    set<void *> sub_decls;
    set<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(func, &sub_decls, &sub_calls);
    for (auto sub_decl : sub_decls) {
      if (codegen->GetFuncBody(sub_decl) == nullptr) {
        continue;
      }
      set_file_name.insert(codegen->get_file(sub_decl));
    }
    void *body = codegen->GetFuncBody(func);
    assert(func && body);
    top_set.insert(func);
    int idx;
    for (idx = 0; idx < codegen->GetFuncParamNum(func); idx++) {
      void *arg = codegen->GetFuncParam(func, idx);
      void *sg_type = codegen->GetTypebyVar(arg);
      string sg_type_string = codegen->GetStringByType(sg_type);
      //  printf("\n\n[Han Debug][opencl_gen.cpp]sg_type_string =
      //  %s\n",sg_type_string.c_str()); printf("arg = %s, sg_type =
      //  %s\n",codegen->UnparseToString(arg).c_str(),codegen->UnparseToString(sg_type).c_str());
      if ((codegen->IsPointerType(sg_type) != 0) ||
          (codegen->IsArrayType(sg_type) != 0)) {
        ret &= insert_global_in_func(
            codegen, func, arg, readonly_or_writeonly_vars, &visited_func,
            &visited_pointer, &decls_map, add_volatile);
        add_global_attribute(codegen, arg, 1, readonly_or_writeonly_vars,
                             add_volatile);
      }
    }
    void *return_type = codegen->GetFuncReturnType(func);
    string return_type_string = codegen->UnparseToString(return_type);

    string opencl_return_type = "__kernel " + return_type_string;
    //  printf("[Han Debug][opencl_gen.cpp]opencl_return_type =
    //  %s\n",opencl_return_type.c_str());
    codegen->RegisterType(opencl_return_type);
    void *new_return_type = codegen->GetTypeByString(opencl_return_type);
    codegen->SetFuncReturnType(func, new_return_type);
  }

  //  update function declaration
  for (auto decl_pair : decls_map) {
    void *new_decl = decl_pair.first;
    //  do not touch top kernel declarations
    if (top_set.count(new_decl) > 0) {
      continue;
    }
    vector<void *> old_decls(decl_pair.second.begin(), decl_pair.second.end());
    for (auto old_decl : old_decls) {
      void *update_decl =
          codegen->CloneFuncDecl(new_decl, codegen->GetGlobal(old_decl), false);
      codegen->ReplaceStmt(old_decl, update_decl);
    }
  }
}

//  This function is declared in array_linearize.cpp
int replace_memcpy_with_opencl_in_lib_wrapper(CSageCodeGen *codegen,
                                              void *pTopFunc,
                                              const CInputOptions &options);

static int remove_access_range_intrinsic(CSageCodeGen *codegen,
                                         void *pTopFunc) {
  int ret = 0;
  vector<void *> vec_decls;
  codegen->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                          &vec_decls);
  //  FIXME: verify
  //    for(auto decl : vec_decls) {
  //        string func_name = codegen->GetFuncName(decl);
  //        if (codegen->IsMerlinInternalIntrinsic(func_name)) continue;
  //        vector<void*> fp_list;
  //        void *dummy_decl = codegen->CreateFuncDecl("void", "__merlin_dummy_"
  //        +
  //                           codegen->GetFuncName(decl), fp_list,
  //                           codegen->GetGlobal(decl), false, nullptr);
  //        codegen->SetStatic(dummy_decl);
  //        codegen->ReplaceStmt(decl, dummy_decl);
  //        ret = 1;
  //    }
  vector<void *> vec_calls;
  codegen->GetNodesByType(nullptr, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto call : vec_calls) {
    string func_name = codegen->GetFuncNameByCall(call);
    if (codegen->IsMerlinInternalIntrinsic(func_name)) {
      continue;
    }
    void *call_stmt = codegen->TraceUpToStatement(call);
    codegen->RemoveStmt(call_stmt);
    ret = 1;
  }
  return ret;
}

int replace_memcpy_with_for_loop_in_function(CSageCodeGen *codegen,
                                             void *func_call, int suffix) {
  //  void * memcpy_scope = codegen->GetScope(func_call);
  void *func_decl = codegen->TraceUpToFuncDecl(func_call);
  void *sg_array1 = nullptr;
  void *sg_pntr1 = nullptr;
  vector<void *> sg_idx1;
  int pointer_dim1;
  void *sg_array2 = nullptr;
  void *sg_pntr2 = nullptr;
  vector<void *> sg_idx2;
  int pointer_dim2;
  void *sg_length;

  int ret = codegen->parse_memcpy(func_call, sg_length, sg_array1, sg_pntr1,
                                  sg_idx1, pointer_dim1, sg_array2, sg_pntr2,
                                  sg_idx2, pointer_dim2);
  if (ret == 0) {
#if PROJDEBUG
    cout << "cannot inline memcpy " << codegen->_p(func_call) << endl;
#endif
    return 0;
  }

  void *array1_type_var = codegen->GetTypebyVar(sg_array1);
  int type_size1 = codegen->get_type_unit_size(
      codegen->RemoveCast(array1_type_var), sg_array1);
  void *array1_basic_type = nullptr;
  vector<size_t> array1_basic_size;
  codegen->get_type_dimension(array1_type_var, array1_basic_type,
                              array1_basic_size, sg_array1);
  void *array2_type_var = codegen->GetTypebyVar(sg_array2);
  //  int type_size2 =
  //  codegen->get_type_unit_size(codegen->RemoveCast(array2_type_var),
  //  sg_array2);
  void *array2_basic_type = nullptr;
  vector<size_t> array2_basic_size;
  codegen->get_type_dimension(array2_type_var, array2_basic_type,
                              array2_basic_size, sg_array2);
  if ((array1_basic_type == nullptr) ||
      array1_basic_type != array2_basic_type) {
#if PROJDEBUG
    cout << "cannot inline memcpy " << codegen->_p(func_call)
         << " because of different types" << endl;
#endif
    return 0;
  }

  if (handle_single_assignment(codegen, sg_array1, sg_idx1, array1_basic_size,
                               sg_array2, sg_idx2, array2_basic_size, sg_length,
                               type_size1, func_call) != 0) {
    return 1;
  }
  int dimension1 = array1_basic_size.size();
  int opt_dimension1 = dimension1;
  {
    //  Youxiang 20170409
    //  if offset + length is less than the lowest dimension,
    //  we can pass partial access to be friendly to HLS
    int element_bitwidth =
        codegen->get_bitwidth_from_type(array1_basic_type, false);
    if (element_bitwidth != 0) {
      int curr_dim = 0;
      int element_bytes = element_bitwidth / 8;
      CMarsExpression me_total_offset =
          (CMarsExpression(sg_length, codegen, func_call) / element_bytes);
      int64_t curr_size = 1;
      while (curr_dim < dimension1) {
        void *curr_index = sg_idx1[dimension1 - 1 - curr_dim];
        CMarsExpression me_curr_offset =
            CMarsExpression(curr_index, codegen, func_call) * curr_size;
        me_total_offset = me_total_offset + me_curr_offset;
        CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
        int64_t c_ub = mr_total_offset.get_const_ub();
        curr_size *= array1_basic_size[curr_dim];
        if (c_ub <= curr_size) {
          opt_dimension1 = curr_dim + 1;
          break;
        }
        curr_dim++;
      }
    }
  }

  int dimension2 = array2_basic_size.size();
  int opt_dimension2 = dimension2;
  {
    //  Youxiang 20170409
    //  if offset + length is less than the lowest dimension,
    //  we can pass partial access to be friendely to HLS
    int element_bitwidth =
        codegen->get_bitwidth_from_type(array2_basic_type, false);
    if (element_bitwidth != 0) {
      int curr_dim = 0;
      int element_bytes = element_bitwidth / 8;
      CMarsExpression me_total_offset =
          (CMarsExpression(sg_length, codegen, func_call) / element_bytes);
      int64_t curr_size = 1;
      while (curr_dim < dimension2) {
        void *curr_index = sg_idx2[dimension2 - 1 - curr_dim];
        CMarsExpression me_curr_offset =
            CMarsExpression(curr_index, codegen, func_call) * curr_size;
        me_total_offset = me_total_offset + me_curr_offset;
        CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
        int64_t c_ub = mr_total_offset.get_const_ub();
        curr_size *= array2_basic_size[curr_dim];
        if (c_ub <= curr_size) {
          opt_dimension2 = curr_dim + 1;
          break;
        }
        curr_dim++;
      }
    }
  }

  string merlin_memcpy_name = "merlin_memcpy_" + my_itoa(suffix);
  string instance_var_list;
  vector<void *> fp_list;
  vector<void *> arg_list;
  vector<void *> fp_idx1;
  vector<void *> fp_idx2;
  vector<size_t> new_array1_basic_size;
  size_t max_len;
  size_t burst_size_1 = 1;
  size_t burst_size_2 = 1;
  for (int i = 0; i < opt_dimension1; ++i) {
    size_t curr_size = array1_basic_size[i];
    new_array1_basic_size.push_back(curr_size);
    burst_size_1 *= curr_size;
  }
  void *new_array1_type_var = codegen->CreateArrayType(
      array1_basic_type, vector<size_t>(new_array1_basic_size.rbegin(),
                                        new_array1_basic_size.rend()));
  vector<size_t> new_array2_basic_size;
  for (int i = 0; i < opt_dimension2; ++i) {
    size_t curr_size = array2_basic_size[i];
    new_array2_basic_size.push_back(curr_size);
    burst_size_2 *= curr_size;
  }
  max_len = std::max(burst_size_1, burst_size_2);
  void *new_array2_type_var = codegen->CreateArrayType(
      array2_basic_type, vector<size_t>(new_array2_basic_size.rbegin(),
                                        new_array2_basic_size.rend()));
  void *fp_dst = codegen->CreateVariable(new_array1_type_var, "dst");
  void *fp_src = codegen->CreateVariable(new_array2_type_var, "src");
  void *fp_len =
      codegen->CreateVariable(codegen->GetTypeByExp(sg_length), "len");
  CMarsExpression me_length(sg_length, codegen, func_call);
  size_t length_upper = me_length.get_range().get_const_ub();
  if (length_upper < max_len) {
    max_len = length_upper;
  }

  void *dst_arg = codegen->CreateVariableRef(sg_array1);
  for (int i = 0; i < dimension1 - opt_dimension1; ++i) {
    dst_arg = codegen->CreateExp(V_SgPntrArrRefExp, dst_arg,
                                 codegen->CopyExp(sg_idx1[i]));
  }
  fp_list.push_back(fp_dst);
  arg_list.push_back(dst_arg);
  for (int i = dimension1 - opt_dimension1; i < dimension1; ++i) {
    void *idx = sg_idx1[i];
    string offset_var_name = "dst_idx_" + my_itoa(i);
    void *fp_idx =
        codegen->CreateVariable(codegen->GetTypeByExp(idx), offset_var_name);
    fp_idx1.push_back(fp_idx);
    fp_list.push_back(fp_idx);
    if (!instance_var_list.empty()) {
      instance_var_list += ",";
    }
    instance_var_list += offset_var_name;
    arg_list.push_back(codegen->CopyExp(idx));
  }
  void *src_arg = codegen->CreateVariableRef(sg_array2);
  for (int i = 0; i < dimension2 - opt_dimension2; ++i) {
    src_arg = codegen->CreateExp(V_SgPntrArrRefExp, src_arg,
                                 codegen->CopyExp(sg_idx2[i]));
  }
  fp_list.push_back(fp_src);
  arg_list.push_back(src_arg);
  for (int i = dimension2 - opt_dimension2; i < dimension2; ++i) {
    void *idx = sg_idx2[i];
    string offset_var_name = "src_idx_" + my_itoa(i);
    void *fp_idx =
        codegen->CreateVariable(codegen->GetTypeByExp(idx), offset_var_name);
    fp_idx2.push_back(fp_idx);
    fp_list.push_back(fp_idx);
    if (!instance_var_list.empty()) {
      instance_var_list += ",";
    }
    instance_var_list += offset_var_name;
    arg_list.push_back(codegen->CopyExp(idx));
  }

  fp_list.push_back(fp_len);
  arg_list.push_back(codegen->CopyExp(sg_length));

  //  create function declaration
  void *merlin_memcpy_decl =
      codegen->CreateFuncDecl("void", merlin_memcpy_name, fp_list,
                              codegen->GetGlobal(func_call), true, func_call);
  void *new_func_body = codegen->GetFuncBody(merlin_memcpy_decl);
  //  implement function
  //  add inline off in the function
  string inline_off_pragma_str = "HLS inline off";
  void *inline_off_pragma =
      codegen->CreatePragma(inline_off_pragma_str, new_func_body);
  codegen->AppendChild(new_func_body, inline_off_pragma);
  //  YX: 20170711 Bug1163
  //  add function instantiation
  string function_instantiation_str =
      "HLS function_instantiate variable=" + instance_var_list;
  void *function_instantiation_pragma =
      codegen->CreatePragma(function_instantiation_str, new_func_body);
  codegen->AppendChild(new_func_body, function_instantiation_pragma);

  int type_size = type_size1;
  assert((size_t)pointer_dim1 <= sg_idx1.size() - 1);  //  check pointer_dim
  assert((size_t)pointer_dim2 <= sg_idx2.size() - 1);
  void *sg_index_i =
      codegen->CreateVariableDecl("int64_t", "i", NULL, new_func_body);
  codegen->AppendChild(new_func_body, sg_index_i);
  void *index_name = codegen->GetVariableInitializedName(sg_index_i);

  //  generate leftside of "="
  vector<void *> array1_index;
  void *sg_total_offset1 = codegen->CreateConst(0);
  for (int i = 0; i < opt_dimension1; i++) {
    sg_total_offset1 =
        codegen->CreateExp(V_SgMultiplyOp, sg_total_offset1,
                           codegen->CreateConst(static_cast<int>(
                               new_array1_basic_size[opt_dimension1 - 1 - i])));
    sg_total_offset1 = codegen->CreateExp(
        V_SgAddOp, sg_total_offset1, codegen->CreateVariableRef(fp_idx1[i]));
  }
  string s_total_offset1 = "total_offset1";
  void *sg_total_offset_decl1 = codegen->CreateVariableDecl(
      "int64_t", s_total_offset1, sg_total_offset1, new_func_body);
  codegen->AppendChild(new_func_body, sg_total_offset_decl1);
  for (int i = 0; i < opt_dimension1; i++) {
    array1_index.push_back(codegen->CreateExp(
        V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl1),
        codegen->CreateVariableRef(sg_index_i)));
  }

  for (int i = 0; i < opt_dimension1 - 1; i++) {
    for (int j = 0; j < opt_dimension1 - i; j++) {
      if (j == opt_dimension1 - i - 1) {
        array1_index[j] = codegen->CreateExp(
            V_SgModOp, array1_index[j],
            codegen->CreateConst(static_cast<int>(new_array1_basic_size[i])));
      } else {
        array1_index[j] = codegen->CreateExp(
            V_SgDivideOp, array1_index[j],
            codegen->CreateConst(static_cast<int>(new_array1_basic_size[i])));
      }
    }
  }

  void *array1_ref =
      codegen->CreateArrayRef(codegen->CreateVariableRef(fp_dst), array1_index);

  //  generate rightside of "="

  vector<void *> array2_index;
  void *sg_total_offset2 = codegen->CreateConst(0);
  for (int i = 0; i < opt_dimension2; i++) {
    sg_total_offset2 =
        codegen->CreateExp(V_SgMultiplyOp, sg_total_offset2,
                           codegen->CreateConst(static_cast<int>(
                               new_array2_basic_size[opt_dimension2 - i - 1])));
    sg_total_offset2 = codegen->CreateExp(
        V_SgAddOp, sg_total_offset2, codegen->CreateVariableRef(fp_idx2[i]));
  }
  string s_total_offset2 = "total_offset2";
  void *sg_total_offset_decl2 = codegen->CreateVariableDecl(
      "int64_t", s_total_offset2, sg_total_offset2, new_func_body);
  codegen->AppendChild(new_func_body, sg_total_offset_decl2);
  for (int i = 0; i < opt_dimension2; i++) {
    array2_index.push_back(codegen->CreateExp(
        V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl2),
        codegen->CreateVariableRef(sg_index_i)));
  }

  for (int i = 0; i < opt_dimension2 - 1; i++) {
    for (int j = 0; j < opt_dimension2 - i; j++) {
      if (j == opt_dimension2 - i - 1) {
        array2_index[j] = codegen->CreateExp(
            V_SgModOp, array2_index[j],
            codegen->CreateConst(static_cast<int>(new_array2_basic_size[i])));
      } else {
        array2_index[j] = codegen->CreateExp(
            V_SgDivideOp, array2_index[j],
            codegen->CreateConst(static_cast<int>(new_array2_basic_size[i])));
      }
    }
  }

  void *array2_ref =
      codegen->CreateArrayRef(codegen->CreateVariableRef(fp_src), array2_index);

  void *transfer_exp = codegen->CreateExp(V_SgAssignOp, array1_ref, array2_ref);
  void *sg_length_c =
      codegen->CreateExp(V_SgDivideOp, codegen->CreateVariableRef(fp_len),
                         codegen->CreateConst(type_size));
  void *for_body_stmt = codegen->CreateStmt(V_SgExprStatement, transfer_exp);
  void *new_loop_body = codegen->CreateBasicBlock();
  string pipeline_pragma_str =
      HLS_PRAGMA + " " + HLS_PIPELINE + " " + HLS_PIPELINE_II + "=1";
  void *pipeline_pragma =
      codegen->CreatePragma(pipeline_pragma_str, new_loop_body);
  codegen->AppendChild(new_loop_body, pipeline_pragma);
  string tripcount_pragma_str =
      HLS_PRAGMA + " " + HLS_TRIPCOUNT + " " + HLS_MAX + "=" + my_itoa(max_len);
  void *tripcount_pragma =
      codegen->CreatePragma(tripcount_pragma_str, new_loop_body);
  codegen->AppendChild(new_loop_body, tripcount_pragma);
  codegen->AppendChild(new_loop_body, for_body_stmt);
  void *for_loop_stmt =
      codegen->CreateStmt(V_SgForStatement, index_name,  //  index_type,
                          codegen->CreateConst(0), sg_length_c, new_loop_body,
                          nullptr, nullptr, func_call);
  codegen->AppendChild(new_func_body, for_loop_stmt);

  //  insert function declaration
  codegen->InsertStmt(merlin_memcpy_decl, func_decl);
  codegen->SetStatic(merlin_memcpy_decl);

  //  create new function call
  void *new_func_call = codegen->CreateFuncCall(
      merlin_memcpy_name, codegen->GetTypeByString("void"), arg_list,
      codegen->GetScope(func_call), func_call);
  codegen->ReplaceExp(func_call, new_func_call);
  setFuncDeclUserCodeScopeId(codegen, merlin_memcpy_decl, new_func_call);
  return 1;
}

int replace_memcpy_with_for_loop_in_function(CSageCodeGen *codegen,
                                             void *pTopFunc) {
  auto kernels = get_all_tasks(codegen, pTopFunc);
  set<void *> all_funcs;
  for (auto func : kernels) {
    set<void *> sub_decls;
    set<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(func, &sub_decls, &sub_calls);
    all_funcs.insert(sub_decls.begin(), sub_decls.end());
    all_funcs.insert(func);
  }

  bool Changed = false;
  int suffix = 0;
  for (auto decl : all_funcs) {
    vector<void *> vec_calls;
    vector<void *> vec_memcpy;
    codegen->GetNodesByType(decl, "preorder", "SgFunctionCallExp", &vec_calls);
    for (auto call : vec_calls) {
      string func_name = codegen->GetFuncNameByCall(call);
      if (func_name != "memcpy") {
        continue;
      }
      vec_memcpy.push_back(call);
    }
    for (auto call : vec_memcpy) {
      replace_memcpy_with_for_loop_in_function(codegen, call, suffix++);
      Changed = true;
    }
  }
  return static_cast<int>(Changed);
}

void bool2char(CSageCodeGen *codegen, void *pTopFunc) {
  printf("\n\nEnter bool2char...\n");
  auto kernels = get_all_tasks(codegen, pTopFunc);
  for (auto func : kernels) {
    vector<void *> params = codegen->GetFuncParams(func);
    for (auto param : params) {
      void *type = codegen->GetTypebyVar(param);
      void *base_type;
      void *new_type;
      string s_type;
      if ((codegen->IsArrayType(type) != 0) ||
          (codegen->IsPointerType(type) != 0)) {
        base_type = codegen->GetBaseType(type);
        s_type = codegen->UnparseToString(base_type);
        if (s_type == "bool") {
          s_type = "char * ";
          new_type = codegen->GetTypeByString("char * ");
          codegen->RegisterType(s_type);
          codegen->SetTypetoVar(param, new_type);
        }
      } else {
        s_type = codegen->UnparseToString(type);
        if (s_type == "bool") {
          s_type = "char ";
          new_type = codegen->GetTypeByString("char ");
          codegen->RegisterType(s_type);
          codegen->SetTypetoVar(param, new_type);
        }
      }
    }
  }
}

int opencl_gen(CSageCodeGen *codegen, void *pTopFunc, CInputOptions options,
               int debug_gen, int keep_code = 0) {
  if (options.get_option("-x").empty()) {
    printf("[mars_opt] ERROR: -x flag is required in pass opencl_gen\n");
    assert(0);
  }
  CXMLParser parser;
  CXMLNode *pTLDM_in = parser.parse_from_file(options.get_option("-x"));
  mark_tldm_annotation(pTLDM_in);
  dump_task_pragma("poly_info_pipeline_insert.rpt");

  if ("hls" == options.get_option("-a")) {
    insert_interface_pragma(codegen, pTopFunc);
    return 1;
  }
  if ("kestrel" == options.get_option("-a")) {
    host_app_wrapper_for_kestrel(codegen, pTopFunc);
    return 1;
  }

  if ("off" == options.get_option_key_value("-a", "pcie_merge")) {
    pcie_merge = 0;
  }

  bool standard_flow =
      "standard" == options.get_option_key_value("-a", "effort");
  //  TO BE FIXED
  /*cfg_gen(kname2Id, kname2base_addr, kname2sok_addr);

  flush_gen(codegen, pTopFunc);
  NDRange_gen(codegen, pTopFunc, kname2Id);
  getGid_gen(codegen, pTopFunc, kname2Id);
  */
  //  cfg_gen();//  (kname2Id, kname2base_addr, kname2sok_addr);

  //  NDRange_gen(codegen, pTopFunc);//  , kname2Id);
  if (debug_gen == RUNTIME_GEN) {
    mem_object_gen(codegen, pTopFunc, debug_gen);
  }
  user_main_gen(codegen, pTopFunc);                     //  , kname2Id);
  graph_block_removal(codegen, pTopFunc, keep_code);    //  , kname2Id);
  dummy_param_flush_gen(codegen, pTopFunc, debug_gen);  //  handling host
  async_read_gen(codegen, pTopFunc, debug_gen);
  getGid_gen(codegen, pTopFunc,
             tool_type);  //  , kname2Id);          //  handling kernel

  //  2017.03.07
  remove_access_range_intrinsic(codegen, pTopFunc);

  //    if (tool_type != 1) bool2char(codegen, pTopFunc);
  //  2016.02.08 Han
  //    if (tool_type == 1) const_replace(codegen, pTopFunc);

  //  2016.02.08 Han
  //    if (tool_type == 1) memcpy_replace_top(codegen, pTopFunc);

  //  2015.03.24 ZP
  opencl_kernel_declaration_gen(codegen, pTopFunc);

  //  2015.10.29 ZP
  if (tool_type != 1) {
    opencl_kernel_add_extern_C(codegen, pTopFunc);
  }

  //  2015.11.13 ZP
  fix_time_h_system_call(codegen, pTopFunc);

  //  2017.03.22
  if (0 == tool_type) {
    replace_memcpy_with_for_loop_in_function(codegen, pTopFunc);
  }

  mark_tldm_annotation_release_resource();

  //  2016.09.14
  //  This function is declared in array_linearize.cpp
  replace_memcpy_with_opencl_in_lib_wrapper(codegen, pTopFunc, options);

  return 1;
}
