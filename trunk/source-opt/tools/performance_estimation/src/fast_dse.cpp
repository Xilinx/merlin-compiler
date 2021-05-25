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


#include <utility>
#include <vector>
#include "fast_dse.h"
#define REQUIREJSON "require.json"
#define REQUIREFILLEDJSON "require_filled.json"
#define STEP1JSON "step1.json"
#define STEP2JSON "step2.json"
#define STEP3JSON "step3.json"
#define STEP4JSON "step4.json"
#define D_IL "iteration-latency"
#define D_ILI "intel-latency"
#define D_UF "unroll-factor"
#define D_UN "unrolled"
#define D_TC "trip-count"
#define D_II "II"
#define D_IIF "initiation-interval"

//  init the data of block
FastDseBlock::FastDseBlock(CSageCodeGen *codegen, BlockType Type,
                           void *AstPtr) {
  this->codegen = codegen;
  this->Type = Type;
  this->AstPtr = AstPtr;

  this->II = 1;
  this->TC = 1;
  this->UF = 1;
  TopoID = getTopoLocation(codegen, static_cast<SgNode *>(AstPtr));
}

//  FastDseBlock::~FastDseBlock() {}

//  transfer blocks attributes to json file
int ExportBlockToJson(CSageCodeGen *codegen, vector<FastDseBlock *> BlockList,
                      string OutputJson) {
  map<string, map<string, string>> require_attributes;
  for (auto OneBlock : BlockList) {
    if (OneBlock->Type == LOOP) {
      map<string, string> attributes;
      attributes.insert(
          pair<string, string>(D_II, std::to_string(OneBlock->II)));
      attributes.insert(
          pair<string, string>(D_IIF, std::to_string(OneBlock->II)));
      attributes.insert(
          pair<string, string>(D_TC, std::to_string(OneBlock->TC)));
      attributes.insert(
          pair<string, string>(D_UF, std::to_string(OneBlock->UF)));
      attributes.insert(
          pair<string, string>(D_IL, std::to_string(OneBlock->IL)));
      attributes.insert(
          pair<string, string>(D_ILI, std::to_string(OneBlock->ILI)));
      require_attributes.insert(
          pair<string, map<string, string>>(OneBlock->TopoID, attributes));
    }
  }
  writeInterReport(OutputJson, require_attributes);
  return 1;
}

//  print blocks attributes to log file
int PrintBlock(CSageCodeGen *codegen, vector<FastDseBlock *> BlockList) {
  for (auto OneBlock : BlockList) {
    if (OneBlock->Type == LOOP) {
      cout << "Type = LOOP "
           << ", TopoID = " << OneBlock->TopoID
           << ", AstPtr = " << codegen->UnparseToString(OneBlock->AstPtr)
           << ", II = " << OneBlock->II << ", TC = " << OneBlock->TC
           << ", UF = " << OneBlock->UF << ", IL = " << OneBlock->IL
           << ", ILI = " << OneBlock->ILI << endl;
    }
  }
  return 1;
}

//  generate required data structure
//  FIXME: Currently only consider loops II IL TC UF
int FastDseBlock::GenerateRequireDataStucture(
    void *pKernel, string Tool, vector<FastDseBlock *> *BlockList,
    map<void *, FastDseBlock *> *Ast_Block) {
  vector<void *> vec_loops;
  string kernel_name = codegen->GetFuncName(pKernel);
  cout << "#################################################" << endl;
  cout << "1. Require data structure file generation for " << kernel_name
       << endl;
  cout << "#################################################" << endl;
  vector<void *> loops_in_kernel;
  map<string, map<string, string>> require_attributes;
  codegen->GetNodesByType(pKernel, "preorder", "SgForStatement",
                          &loops_in_kernel, false, true);
  for (auto loop : loops_in_kernel) {
    if (!codegen->isWithInHeaderFile(loop)) {
      cout << "find one loop: " << codegen->UnparseToString(loop) << endl;
      FastDseBlock *LoopBlock = new FastDseBlock(codegen, LOOP, loop);
      //  get trip count fome source code
      int64_t trip_count = 1;
      int64_t CurrentLoopTC = 1;
      codegen->get_loop_trip_count(loop, &trip_count, &CurrentLoopTC);
      //  if non-canonical loop, we can not get data from this API
      cout << "trip_count = " << trip_count
           << ", CurrentLoopTC = " << CurrentLoopTC << endl;
      LoopBlock->TC = CurrentLoopTC;
      //  get unroll factor from source code
      string is_unroll_s = "no";
      int64_t factor = GetUnrollFactorFromVendorPragma(codegen, loop, Tool);
      // int64_t factor = 1;
      // GetUnrollFactorFromVendorPragma(codegen, loop, Tool, factor);
      string factor_s;
      bool is_unroll = factor > 1 ? true : false;
      if (static_cast<int>(is_unroll) == 1) {
        is_unroll_s = "yes";
      }
      //  FIXME: if trip-count <=8, auto unroll ?
      //  if(LoopBlock->TC <= 8) {
      //  }
      LoopBlock->UF = factor;
      LoopBlock->IsUnroll = is_unroll_s;
      //  Push to block list
      if (std::find(vec_loops.begin(), vec_loops.end(), loop) ==
          vec_loops.end()) {
        vec_loops.push_back(loop);
        BlockList->push_back(LoopBlock);
        Ast_Block->insert(pair<void *, FastDseBlock *>(loop, LoopBlock));
      }
    }
  }
  return 1;
}

//  generate attribute list, II IL TC UF
vector<string> GetAttributeList() {
  vector<string> AttributeList;
  AttributeList.push_back(D_II);
  AttributeList.push_back(D_IL);
  AttributeList.push_back(D_ILI);
  AttributeList.push_back(D_TC);
  AttributeList.push_back(D_UF);
  AttributeList.push_back(D_UN);  //  can ignore it later
  return AttributeList;
}

//  Update data structure with existing org data
//  Except TC
int UpdateRequireDataWithOrgData(CSageCodeGen *codegen,
                                 const vector<FastDseBlock *> &BlockList,
                                 string HistoryJson, string Tool) {
  //  bool check_failed = false;
  //  FIXME do we need to check if loop is tiling nested loop?
  cout << "#################################################" << endl;
  cout << "2. UpdateRequireDataWithOrgData." << endl;
  cout << "#################################################" << endl;
  cout << "Reading org map." << endl;
  map<string, map<string, string>> history_map;
  readInterReport(HistoryJson, &history_map);
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;

  vector<string> AttributeList = GetAttributeList();
  for (auto OneBlock : BlockList) {
    string ID = OneBlock->TopoID;
    //    cout << "ID = " << ID << endl;
    iter1 = history_map.find(ID);
    if (iter1 == history_map.end()) {
      //  if required Id not exist in org.json, then direrctoy use default data
      cout << "ERROR: require ID not exist in org.json" << endl;
    } else {
      cout << "original ID exist in org.json" << endl;
      map<string, string> attribute_org = iter1->second;
      for (auto OneAttribute : AttributeList) {
        iter2 = attribute_org.find(OneAttribute);
        if (iter2 == attribute_org.end()) {
          //  If attribute not eixst in org.json, use default value
          cout << "WARNING: attribute " << OneAttribute
               << " not exist in original json" << endl;
        } else {
          //  If value not empty, replace data from org.json
          //          cout << "attribute = " << OneAttribute
          //               << ", value = " << iter2->second << endl;
          if (!iter2->second.empty()) {
            //  If trip-count, we just use current exist data, not org data
            //  If II we use org data if possible
            if (OneAttribute == D_II) {
              OneBlock->II = atoi((iter2->second).c_str());
              //  If II equal "-", atoi result is 0, replace with default 1
              //  if (OneBlock->II == 0) {
              //  OneBlock->II = 1;
              //  }
              //  keep original II
              //  II will used for detect pipe parent
              OneBlock->II = OneBlock->II;
            }
            //  Ior unroll factor we use org data if possible
            if (OneAttribute == D_UF) {
              int64_t UF_org = atoi((iter2->second).c_str());
              if (UF_org < 1) {
                cout << "Find unroll factor < 1." << endl;
                OneBlock->UF = OneBlock->UF;
              } else {
                OneBlock->UF = UF_org;
              }
            }
            //  Ior if unrolled we use org data if possible
            //  useless now, check if can ignore
            if (OneAttribute == D_UN) {
              OneBlock->IsUnroll = iter2->second;
            }
            //  If iteration latency, we use org data if possible
            if (OneAttribute == D_IL) {
              OneBlock->IL = atoi((iter2->second).c_str());
              if (OneBlock->IL < 1) {
                cout << "Find iteration latency < 1." << endl;
                OneBlock->IL = 1;
              }
            }
            //  If intel iteration latency, we use org data if possible
            if (OneAttribute == D_ILI) {
              OneBlock->ILI = atoi((iter2->second).c_str());
              if (OneBlock->ILI < 1) {
                cout << "Find intel iteration latency < 1." << endl;
                OneBlock->ILI = 1;
              }
            }
            //  If intel iteration latency, we use org data if possible
            if (OneAttribute == D_TC) {
              void *loop = OneBlock->AstPtr;
              int64_t TC_org = atoi((iter2->second).c_str());
              cout << "loop = " << codegen->UnparseToString(loop)
                   << ", TC = " << TC_org << endl;
              //  If non-canonical loop, we can rely data from org.json
              if ((codegen->IsCanonicalForLoop(loop) == 0) && TC_org > 0) {
                cout << "Find a non-canonical loop." << endl;
                OneBlock->TC = TC_org;
              }
            }
          } else {
            //  If value empty, it means we did not find info in vendor report
            //  It is possible a fully unroll loop or empty loop
            //  so set II=0, IL=1, UF=TC
            if (OneAttribute == D_II) {
              OneBlock->II = 0;
            }
            if (OneAttribute == D_UF) {
              OneBlock->UF = OneBlock->TC;
            }
            if (OneAttribute == D_IL) {
              OneBlock->IL = 1;
            }
            if (OneAttribute == D_ILI) {
              OneBlock->ILI = 1;
            }
          }
        }
      }
    }
  }
  cout << "\n" << endl;
  cout << "\n" << endl;
  return 0;
}

FastDseBlock *GetDirectParentLoopBlock(CSageCodeGen *codegen,
                                       FastDseBlock *OneBlock,
                                       map<void *, FastDseBlock *> Ast_Block) {
  void *ChildLoop = OneBlock->AstPtr;
  if (ChildLoop != nullptr) {
    void *ParentStmt = codegen->GetParent(ChildLoop);
    while (true) {
      if (codegen->IsFunctionDeclaration(ParentStmt) != 0) {
        return nullptr;
      }
      if (static_cast<bool>(codegen->IsForStatement(ParentStmt)) ||
          static_cast<bool>(codegen->IsWhileStatement(ParentStmt)) ||
          static_cast<bool>(codegen->IsDoWhileStatement(ParentStmt))) {
        map<void *, FastDseBlock *>::iterator it = Ast_Block.find(ParentStmt);
        if (it != Ast_Block.end()) {
          FastDseBlock *ParentBlock = it->second;
          if (ParentBlock->II > 1) {
            cout << "Child Loop ID " << OneBlock->TopoID << ", Parent Loop ID "
                 << ParentBlock->TopoID << endl;
            return ParentBlock;
          }
          return nullptr;
        }
        cout << "Did not find existing parent in block list." << endl;
        return nullptr;
      }
      ParentStmt = codegen->GetParent(ParentStmt);
    }
  }
  return nullptr;
}

//  Predict some of the data from code_change.json
//  Now only consider UF and TC change
bool UpdateRequireDataWithCodeChangeData(
    CSageCodeGen *codegen, const vector<FastDseBlock *> &BlockList,
    map<void *, FastDseBlock *> Ast_Block, string ChangeJson,
    string HistoryJson, string Tool) {
  //  FIXME check if tiling loop
  //  record loop, and check on loop if also loop
  //  and A*B=C*D, then it is tiling loop
  //  and innermost loop fully unrolled
  bool ret = true;
  cout << "#################################################" << endl;
  cout << "3. UpdateRequireDataWithCodeChangeData" << endl;
  cout << "#################################################" << endl;
  cout << "Reading change map." << endl;
  map<string, map<string, string>> change_map;
  readInterReport(ChangeJson, &change_map);
  cout << "Reading org map." << endl;
  map<string, map<string, string>> history_map;
  readInterReport(HistoryJson, &history_map);
  map<string, map<string, string>>::iterator iter1_single;
  map<string, string>::iterator iter2_single;
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;

  for (auto OneBlock : BlockList) {
    string ID = OneBlock->TopoID;
    for (iter1 = change_map.begin(); iter1 != change_map.end(); iter1++) {
      string ChangeID = iter1->first;
      cout << "ChangeID = " << ChangeID << endl;
      if (ID == ChangeID) {
        //  try to get TC number from orignal org.json
        int64_t TC_org = OneBlock->TC;
        map<string, string> attribute_change = iter1->second;
        iter1_single = history_map.find(ID);
        if (iter1_single != history_map.end()) {
          map<string, string> tmp = iter1_single->second;
          iter2_single = tmp.find(D_TC);
          if (iter2_single != tmp.end()) {
            string value = iter2_single->second;
            TC_org = atoi(value.c_str());
          }
        }
        int64_t II_org = OneBlock->II;
        int64_t UF_org = OneBlock->UF;
        int64_t II_new = 1;
        int64_t UF_new = 1;
        bool from_or_to_fully_unroll = false;
        //  if original loop is fully unrolled loop, then we don't need to run
        //  HLS
        if ((TC_org > 1) && (UF_org > 1) && (TC_org == UF_org)) {
          from_or_to_fully_unroll = true;
          cout << "Warning: Find original loop II = UF = " << TC_org << endl;
        }
        for (iter2 = attribute_change.begin(); iter2 != attribute_change.end();
             iter2++) {
          string attribute = iter2->first;
          string value_changed = iter2->second;
          cout << "attribute = " << attribute
               << ",    value_changed = " << value_changed << endl;
          //  if UF changed, need update II, UF and IL
          if (attribute == D_UF) {
            if (!value_changed.empty()) {
              UF_new = atoi(value_changed.c_str());
            }
            cout << "UF_new = " << UF_new << ", UF_org = " << UF_org << endl;
            double ratio = (UF_new * 1.0) / (UF_org * 1.0);
            cout << "ratio = " << ratio << endl;
            //  unroll-factor impact loop latency
            double logdata = log2(ratio);
            cout << "logdata = " << logdata << endl;
            OneBlock->IL += log2(ratio);
            OneBlock->ILI = OneBlock->IL;
            if (OneBlock->IL < 1) {
              cout << "output IL = " << OneBlock->IL << endl;
              OneBlock->IL = 1;
            }
            if (OneBlock->ILI < 1) {
              OneBlock->ILI = 1;
            }
            //  unroll-factor impact II
            if (II_org == 1) {
              II_new = 1;
            } else if (II_org == 0) {
              II_new = 0;
            } else {
              II_new = static_cast<int64_t>((II_org - 1.0) * ratio);
            }
            OneBlock->II = II_new;
            //  Use the UF data from code_change.json
            OneBlock->UF = UF_new;
            cout << "Changed II to " << OneBlock->II << endl;
            cout << "Changed UF to " << OneBlock->UF << endl;
            cout << "Changed IL to " << OneBlock->IL << endl;
            cout << "Changed ILI to " << OneBlock->ILI << endl;
          }
          //  if Tc changed, do not impact other data
          if (attribute == D_TC) {
            //  Use the TC data from code_change.json
            if (!value_changed.empty()) {
              OneBlock->TC = atoi(value_changed.c_str());
              //              cout << "Changed TC to " << OneBlock->TC << endl;
              //              int64_t TC_new = OneBlock->TC;
              //              double ratio = (TC_new * 1.0) / (TC_org * 1.0);
              //              cout << "ratio = " << ratio << endl;
              //              double logdata = log2(ratio);
              //              cout << "logdata = " << logdata << endl;
              //              int64_t NewParentII += log2(ratio);
              //              if(NewParentII < 1) {
              //                  NewParentII = 1;
              //              }
              //              cout << "Change from parent II from " << ParentII
              //              << " to " << NewParentII << endl;
            }
          }
        }
        if ((OneBlock->TC > 1) && (OneBlock->UF > 1) &&
            (OneBlock->TC == OneBlock->UF)) {
          from_or_to_fully_unroll = !from_or_to_fully_unroll;
          cout << "Warning: Find new loop II = UF = " << OneBlock->TC << endl;
        }
        if (from_or_to_fully_unroll) {
          ret = false;
          cout << "Warning: Find a loop which from fully unroll to not fully "
                  "unroll"
               << "or from not fully unroll to fully unroll. Run HLS." << endl;
        }
        FastDseBlock *ParentLoopBlock =
            GetDirectParentLoopBlock(codegen, OneBlock, Ast_Block);
        if (ParentLoopBlock != nullptr) {
          int64_t ParentII = ParentLoopBlock->II;
          cout << "Parent II = " << ParentII << endl;
          //  if unroll factor > 1, update parent loop II if II not eaqul to 1
          if (UF_org > 1) {
            double ratio = (UF_new * 1.0) / (UF_org * 1.0);
            cout << "ratio = " << ratio << endl;
            int64_t NewParentII = ParentII * ratio;
            cout << "Change from parent II from " << ParentII << " to "
                 << NewParentII << endl;
            ParentLoopBlock->II = NewParentII;
          }
          //  if unroll factor > 1, update parent loop IL if II not eaqul to 1
          int64_t ParentIL = ParentLoopBlock->IL;
          cout << "Parent IL = " << ParentIL << endl;
          if (UF_org > 1) {
            double ratio = (UF_new * 1.0) / (UF_org * 1.0);
            cout << "ratio = " << ratio << endl;
            int64_t NewParentIL = ParentIL * ratio;
            cout << "Change from parent IL from " << ParentIL << " to "
                 << NewParentIL << endl;
            ParentLoopBlock->IL = NewParentIL;
          }
        }
        cout << "\n" << endl;
      }
    }
  }
  return ret;
}

//  check if all data structure fully filled with valid data
//  all data should >= 1
bool CheckIfFullyFilled(const vector<FastDseBlock *> &BlockList, string Tool) {
  cout << "#################################################" << endl;
  cout << "4. Check if data fully filled." << endl;
  cout << "#################################################" << endl;
  bool fully_filled = true;
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;
  for (auto OneBlock : BlockList) {
    string ID = OneBlock->TopoID;
    //    cout << "ID = " << ID << endl;
    vector<string> AttributeList = GetAttributeList();
    for (auto OneAttribute : AttributeList) {
      cout << "attribute = " << OneAttribute << endl;
      if (OneAttribute == D_TC) {
        if (OneBlock->TC < 1) {
          cout << "Error TC number " << OneBlock->TC << endl;
          fully_filled = false;
        } else {
          cout << "value = " << OneBlock->TC << endl;
        }
      }
      if (OneAttribute == D_II) {
        if (OneBlock->II < 0) {
          cout << "Error II number " << OneBlock->II << endl;
          fully_filled = false;
        } else {
          cout << "value = " << OneBlock->II << endl;
        }
      }
      if (OneAttribute == D_UF) {
        if (OneBlock->UF < 1) {
          cout << "Error UF number " << OneBlock->UF << endl;
          fully_filled = false;
        } else {
          cout << "value = " << OneBlock->UF << endl;
        }
      }
      if (OneAttribute == D_IL) {
        if (OneBlock->IL < 1) {
          //  If fully unrolled, then no need IL
          if (OneBlock->UF != OneBlock->TC) {
            cout << "Error IL number " << OneBlock->IL << endl;
            fully_filled = false;
          }
        } else {
          cout << "value = " << OneBlock->IL << endl;
        }
      }
    }
  }
  return fully_filled;
}

//  get attribute datafrom map
//  int get_attribute(map<string, string> attribute_value, string attribute,
//                  int64_t &data) {
//  data = 1;
//  map<string, string>::iterator iter;
//  for (iter = attribute_value.begin(); iter != attribute_value.end(); iter++)
//  {
//    string attribute_m = iter->first;
//    string value_m = iter->second;
//    if (attribute_m == attribute) {
//      cout << "get attribute: attribute = " << attribute
//           << ", value = " << value_m << endl;
//      if (value_m != "" && value_m != "n/a") {
//        data = atoi(value_m.c_str());
//        return 1;
//      }
//    }
//  }
//  return 0;
//  }

//  set attribute data to json map
void set_attribute_to_map(map<string, map<string, string>> *filled_map,
                          string ID, string attribute, string value) {
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;
  iter1 = (*filled_map).find(ID);
  if (iter1 != (*filled_map).end()) {
    map<string, string> attribute_value = iter1->second;
    iter2 = attribute_value.find(attribute);
    if (iter2 != attribute_value.end()) {
      (*filled_map)[ID][attribute] = value;
      cout << "set attribute: ID = " << ID << ", attribute = " << attribute
           << ", value = " << value << endl;
    } else {
      cout << "Did not find attribute = " << attribute << " in ID = " << ID
           << endl;
    }
  } else {
    cout << "Did not find ID = " << ID << endl;
  }
}

//  insert new attribtue to json map
void insert_attribute_to_map(map<string, map<string, string>> *filled_map,
                             string ID, string attribute, string value) {
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;
  iter1 = (*filled_map).find(ID);
  if (iter1 != (*filled_map).end()) {
    map<string, string> attribute_value = iter1->second;
    attribute_value.insert(pair<string, string>(attribute, value));
    cout << "insert attribute: ID = " << ID << ", attribute = " << attribute
         << ", value = " << value << endl;
    (*filled_map)[ID] = attribute_value;
  } else {
    cout << "Did not find ID = " << ID << endl;
  }
}

//  Merte data structure json file and org.json
bool MergeRequireJson(CSageCodeGen *codegen, string RequireJson,
                      string HistoryJson, string OutputJson, string Tool) {
  cout << "#################################################" << endl;
  cout << "5. Merge json file." << endl;
  cout << "#################################################" << endl;
  map<string, map<string, string>> require_map;
  readInterReport(RequireJson, &require_map);
  map<string, map<string, string>> history_map;
  readInterReport(HistoryJson, &history_map);

  map<string, map<string, string>>::iterator iter1_single;
  map<string, string>::iterator iter2_single;
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;
  for (iter1 = require_map.begin(); iter1 != require_map.end(); iter1++) {
    string change_ID = iter1->first;
    //    cout << "ID = " << change_ID << endl;
    iter1_single = history_map.find(change_ID);
    if (iter1_single != history_map.end()) {
      cout << "original ID exist in org.json" << endl;
      map<string, string> attribute_require = iter1->second;
      map<string, string> attribute_org = iter1_single->second;
      for (iter2 = attribute_require.begin(); iter2 != attribute_require.end();
           iter2++) {
        string attribute = iter2->first;
        iter2_single = attribute_org.find(attribute);
        //  if not exist, set new attribute number
        if (iter2_single != attribute_org.end()) {
          string value = iter2->second;
          set_attribute_to_map(&history_map, change_ID, attribute, value);
        } else {
          //  if not exist, insert new attribute
          string value = iter2->second;
          insert_attribute_to_map(&history_map, change_ID, attribute, value);
        }
      }
      cout << "\n" << endl;
    }
  }
  cout << "Write output.json file." << endl;
  writeInterReport(OutputJson, history_map);
  cout << "\n" << endl;
  return true;
}

//  Merte data structure json file and org.json
bool PostProcessForJson(CSageCodeGen *codegen, string OutputJson) {
  cout << "#################################################" << endl;
  cout << "6. Delete some attribtues from " << OutputJson << "." << endl;
  cout << "#################################################" << endl;
  map<string, map<string, string>> current_map;
  readInterReport(OutputJson, &current_map);
  map<string, map<string, string>> output_map;
  map<string, map<string, string>>::iterator iter1;
  map<string, string>::iterator iter2;
  for (iter1 = current_map.begin(); iter1 != current_map.end(); iter1++) {
    string ID = iter1->first;
    //    cout << "ID = " << ID << endl;
    map<string, string> attribute_value = iter1->second;
    map<string, string> attribute_temp;
    for (iter2 = attribute_value.begin(); iter2 != attribute_value.end();
         iter2++) {
      string attribute = iter2->first;
      string value = iter2->second;
      if (attribute != "CYCLE_TOT" && attribute != "CYCLE_UNIT" &&
          attribute != "DBW") {
        attribute_temp.insert(pair<string, string>(attribute, value));
      } else {
        cout << "Delete: attribute = " << attribute << ", value = " << value
             << endl;
      }
    }
    output_map.insert(pair<string, map<string, string>>(ID, attribute_temp));
  }
  cout << "Write output.json file." << endl;
  string cmd = "rm -rf " + OutputJson;
  system(cmd.c_str());
  writeInterReport(OutputJson, output_map);
  cout << "\n" << endl;
  return true;
}

void FastDse(CSageCodeGen *codegen,
             map<string, map<string, string>> KernelGroup, string ChangeJson,
             string HistoryJson, string OutputJson, string Tool) {
  cout << "#################################################" << endl;
  cout << "Enter fastdse." << endl;
  cout << "#################################################" << endl;
  vector<FastDseBlock *> BlockList;
  map<void *, FastDseBlock *> Ast_Block;
  vector<void *> KernelList;
  for (auto OneKernel : KernelGroup) {
    string TopKernelName = OneKernel.first;
    void *pKernel = codegen->GetFuncDeclByName(TopKernelName, 1);
    FastDseBlock *TopKernelBlock = new FastDseBlock(codegen, FNDECL, pKernel);
    TopKernelBlock->GenerateRequireDataStucture(pKernel, Tool, &BlockList,
                                                &Ast_Block);
  }
  //  1. Generate BLocks for each require statements
  PrintBlock(codegen, BlockList);
  ExportBlockToJson(codegen, BlockList, STEP1JSON);

  //  2. Filled data with data from org.json
  UpdateRequireDataWithOrgData(codegen, BlockList, HistoryJson, Tool);
  PrintBlock(codegen, BlockList);
  ExportBlockToJson(codegen, BlockList, STEP2JSON);

  //  3. Update requied data with code change
  if (!UpdateRequireDataWithCodeChangeData(codegen, BlockList, Ast_Block,
                                           ChangeJson, HistoryJson, Tool)) {
    return;
  }
  PrintBlock(codegen, BlockList);
  ExportBlockToJson(codegen, BlockList, STEP3JSON);

  //  4. Check of all required data filled
  if (!CheckIfFullyFilled(BlockList, Tool)) {
    return;
  }
  PrintBlock(codegen, BlockList);
  ExportBlockToJson(codegen, BlockList, REQUIREFILLEDJSON);

  //  5. Merge new data with original data
  MergeRequireJson(codegen, REQUIREFILLEDJSON, HistoryJson, OutputJson, Tool);

  //  6. Merge new data with original data
  PostProcessForJson(codegen, OutputJson);
}
