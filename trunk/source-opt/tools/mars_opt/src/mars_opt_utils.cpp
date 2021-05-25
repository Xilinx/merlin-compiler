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


#include <signal.h>
#include <sys/resource.h>
#include <execinfo.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"
#include "postwrap_process.h"
#include "program_analysis.h"
#include "tldm_annotate.h"
#include "ir_types.h"

#include "history.h"

#include "common.h"

#define PRINT_INFO 1

//  also defined in codegen->cpp
#define USE_LOWERCASE_NAME 0
#define USED_CODE_IN_COVERAGE_TEST 0

//  define version string
#ifndef BUILD_VERSION
#define BUILD_VERSION "internal"
#endif
#ifndef BUILD_DATE
#define BUILD_DATE ""
#endif

const char CRITICAL_MESSAGE_REPORT_FILE[] = "critical_message.rpt";

//  ///////////////////////////////////////////////////////////////////////////
//  /

void dump_critical_message(string pass_name, string level, string message,
                           int number, int user_display, CSageCodeGen *ast,
                           void *node) {
  const string file_name = CRITICAL_MESSAGE_REPORT_FILE;
  if ((node != nullptr) && (ast != nullptr)) {
    map<string, string> msg_map;
    msg_map["critical_warning"] = message;
    insertMessage(ast, node, msg_map);
  }
  string str_type = level.substr(0, 1);
  string str_num;
  if (number != -1) {
    str_num = "-" + my_itoa(number);
  }

  string new_msg = level + ": [" + pass_name + str_num + "] " + message;
  string format_msg;
  format_msg = new_msg + "\n";

  if (str_type != "I") {
    format_msg = "\n" + format_msg;
  }

  if (user_display == 0) {
    if (test_file_for_read(file_name) != 0) {
      string all_info = read_string_from_file(file_name);
      if (all_info.find(format_msg) != string::npos) {
        return;
      }
    }
    append_string_into_file(format_msg, file_name);
  } else {
    cout << format_msg << endl;
  }
}

set<string> dump_array;
set<string> graph_names;
map<string, string> mapArray2DimStr;
map<string, string> mapArray2TypeName;

#if USED_CODE_IN_COVERAGE_TEST
extern void opencl_gen(CSageCodeGen *codegen, void *pTopFunc,
                       CInputOptions options, int debug_gen, int keep_code);
extern int user_main_gen(CSageCodeGen *codegen, void *pTopFunc);
extern void prof_gen(CSageCodeGen *codegen, void *pTopFunc);
extern void cpu_timer_gen(CSageCodeGen *codegen, void *pTopFunc, int debug_gen);
#endif
// string dram_addr_base = "0xbe000000";

extern string order_vector_gen(CSageCodeGen *codegen, void *sg_node_scope,
                               void *sg_node,
                               int HGT);  //  task: sg_node = basicblock task
                                          //  access: sg_node = one reference
extern string order_vector_gen(CSageCodeGen *codegen, void *sg_node, int HGT);
extern string cond_vector_gen(string order_vec);
#if 0
vector<string> extract_host_task(string sTaskName, void *pTaskBegin) {
  vector<string> host_task_desc;
  return host_task_desc;
}
#endif

extern vector<string> kernel_invar_param;
extern map<string, string> mapAccess2PortType;

//  This is the simplified order vector, where iterator components are removed
//  from the orignal TLDM 2n+1 IR
map<void *, vector<int>> mapTask2OrderVec;
map<void *, vector<int>> mapGraph2OrderVec;

map<void *, set<void *, TaskOrderLT>> execute_seq;
set<void *, GraphOrderLT> sorted_execute_seq;
vector<string> graph_execute_seq;

#if USED_CODE_IN_COVERAGE_TEST
void GetTLDMInfo(void *sg_node, void *pArg) {
  CSageCodeGen sggen;
  string str = sggen.GetPragmaString(sg_node);

  if ("" != str) {
    vector<string> *pPragmaList = static_cast<vector<string> *>(pArg);
    pPragmaList->push_back(str);
  }
}

void GetTLDMInfo_withPointer(void *sg_node, void *pArg) {
  CSageCodeGen sggen;
  string str = sggen.GetPragmaString(sg_node);

  if ("" != str) {
    void *sg_pragma_decl = sg_node;  //  sggen.GetParent(sg_node);
    vector<pair<void *, string>> *pPragmaList =
        (vector<pair<void *, string>> *)pArg;
    pPragmaList->push_back(pair<void *, string>(sg_pragma_decl, str));
  }
}
#endif

int tldm_pragma_parse_one_term(string sTerm, size_t ptr, string *p_sParam,
                               vector<string> *p_vecList,
                               vector<string> *p_vecCond) {
  //  reset all variables to be updated (returned)
  *p_sParam = "";
  (*p_vecList).clear();
  (*p_vecCond).clear();

  static boost::regex auto_expr(std::string(CMOST_auto) + "\\{.+\\}.*",
                                boost::regex::icase);

  //  move to the first non-space
  while (sTerm[ptr] == ' ') {
    ptr++;
  }

  //  this is a single term and is for DSE
  if (boost::regex_match(sTerm.substr(ptr), auto_expr)) {
    string param = get_sub_delimited(sTerm, ptr, '}') + "}";
    ptr += param.size() + 1;
    *p_sParam = FormatSpace(param);
    return ptr;
  }

  //  try to parse key-value term
  string kvParam = get_sub_delimited(sTerm, ptr, '=');
  size_t kvPtr = ptr + kvParam.size() + 1;

  //  try to parse single term (must not for DSE)
  string stParam = get_sub_delimited(sTerm, ptr, ' ');
  size_t stPtr = ptr + stParam.size() + 1;

  if (kvPtr >= sTerm.size()) {
    //  single term (no "=" in the pragma)
    //  simply return an entire term
    *p_sParam = FormatSpace(stParam);
    return stPtr;
  }
  if (stPtr < kvPtr) {
    //  exist a space before "="
    //  case 1: "term1( +)term2=value2", we only need term1 (stParam)
    //  case 2: "term1( +)=( +)value1", we need both (ignore here)

    string s_mid = sTerm.substr(stPtr, kvPtr - stPtr - 1);
    s_mid = FormatSpace(s_mid);
    if (!s_mid.empty()) {
      //  if term2 exists, return term1 as a single term
      *p_sParam = FormatSpace(stParam);
      return stPtr;
    }
  }

  //  case: keystatic_cast< *>(=( *)value)

  //  keystatic_cast< *>()
  kvParam = FormatSpace(kvParam);

  //  move pointer to the first character of the value
  while (kvPtr < sTerm.size() && sTerm[kvPtr] == ' ') {
    kvPtr++;
  }

  string sList;  //  the value

  if (kvPtr < sTerm.size() && sTerm[kvPtr] == '\"') {
    //  if the value has quotation marks, remove them
    sList = get_sub_delimited(sTerm, ++kvPtr, '\"');

    if (kvPtr + sList.size() + 1 > sTerm.size()) {
      string msg = "Missing quotation mark \" in the pragma";
      dump_critical_message("PRAGM", "ERROR", msg, 301);
      cout << "ERROR: " << msg << endl;
      exit(0);
    }
  } else if (boost::regex_match(sTerm.substr(kvPtr), auto_expr)) {
    //  value is for DSE
    sList = get_sub_delimited(sTerm, kvPtr, '}') + "}";
    sList = FormatSpace(sList);
  } else {
    sList = get_sub_delimited(sTerm, kvPtr, ' ');
    sList = FormatSpace(sList);
  }
  kvPtr += sList.size() + 1;

  *p_sParam = kvParam;
  (*p_vecList).push_back(sList);
  return kvPtr;
}

#if USED_CODE_IN_COVERAGE_TEST
int tldm_pragma_parse_one_term_old(string sTerm, size_t j, string *p_sParam,
                                   vector<string> *p_vecList,
                                   vector<string> *p_vecCond) {
  *p_sParam = "";
  *p_vecList.clear();
  *p_vecCond.clear();

  string sList;
  *p_sParam = get_sub_delimited(sTerm, j, '=');
  j += *p_sParam.size() + 1;
  *p_sParam = FormatSpace(*p_sParam);
  if (j >= sTerm.size())
    return j;

  while (sTerm[j] == ' ')
    j++;

  size_t j0 = j;
  size_t j0_bak = j;
  sList = get_sub_delimited(sTerm, j, '\"');
  j += sList.size() + 1;
  j0 = j;
  sList = get_sub_delimited(sTerm, j, '\"');
  j += sList.size() + 1;
  if (j0 >= sTerm.size() || j > sTerm.size()) {
    //  ZP: 20150703: add support for no ""
    if (0) {
      //  printf("j0=%d, size=%d, j=%d.\n", j0, sTerm.size(), j);
      printf("[mars_opt] ERROR: tldm pragma passing failed (quotation marks "
             "\"\" are missing for parameter %s).\n",
             p_sParam->c_str());
      printf("\t#pragma %s\n\n", sTerm.c_str());
      assert(0);
      exit(0);
    } else {
      j = j0_bak;
      sList = get_sub_delimited(sTerm, j, ' ');
      j += sList.size() + 1;
      j0 = j;
    }
  }
  sList = FormatSpace(sList);

  //  2013-10-08
  if (0) {
    sList = RemoveQuote(sList);

    string sFirst = get_sub_delimited(sList, 0, ':');
    str_split(sFirst, ',', *p_vecList);
    if (sFirst.size() + 1 < sList.size()) {
      string sSecond = sList.substr(sFirst.size() + 1);
      str_split(sSecond, ';', vecCond);
    }
  } else {
    p_vecList->push_back(sList);
  }
  return j;
}
#endif

int is_cmost_pragma(string sFilter, bool allow_hls) {
  boost::algorithm::to_lower(sFilter);

  if (sFilter == "hls" && static_cast<int>(allow_hls) == 1) {
    return 1;
  }

  return static_cast<int>(sFilter == "tldm" || sFilter == "cmost" ||
                          sFilter == "accel");
}

int tldm_pragma_parse_whole(
    string sPragma, string *p_sFilter, string *p_sCmd,
    map<string, pair<vector<string>, vector<string>>> *p_mapParam,
    bool allow_hls) {
  boost::replace_all(sPragma, "\t", " ");

  p_mapParam->clear();
  size_t j = 0;
  *p_sFilter = get_sub_delimited(sPragma, j, ' ');
  j += (*p_sFilter).size() + 1;
  if (is_cmost_pragma(*p_sFilter, allow_hls) == 0) {
    return 0;
  }

  //  20190205 cody: support DSE auto pragma
  boost::regex autoType(".*ACCEL +auto\\{.+\\}.*", boost::regex::icase);
  if (boost::regex_match(sPragma, autoType)) {
    *p_sCmd = get_sub_delimited(sPragma, j, '}') + '}';
  } else {
    *p_sCmd = get_sub_delimited(sPragma, j, ' ');
  }
  j += (*p_sCmd).size() + 1;

  vector<string> vecList;
  vector<string> vecCond;
  while (j < sPragma.size()) {
    string sParam;
    j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vecList, &vecCond);

    pair<vector<string>, vector<string>> empty;
    (*p_mapParam)[sParam] = empty;

    (*p_mapParam)[sParam].first = vecList;
    (*p_mapParam)[sParam].second = vecCond;
  }

  return 1;
}

void all_pragma_parse_whole(
    string sPragma, string *p_sFilter, string *p_sCmd,
    map<string, pair<vector<string>, vector<string>>> *p_mapParam,
    vector<string> *p_mapKey) {
  boost::replace_all(sPragma, "\t", " ");

  p_mapParam->clear();
  size_t j = 0;
  *p_sFilter = get_sub_delimited(sPragma, j, ' ');
  j += (*p_sFilter).size() + 1;
  *p_sCmd = get_sub_delimited(sPragma, j, ' ');
  j += (*p_sCmd).size() + 1;

  vector<string> vec0;
  vector<string> vec1;
  while (j < sPragma.size()) {
    string sParam;
    j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vec0, &vec1);

    pair<vector<string>, vector<string>> empty;
    (*p_mapParam)[sParam] = empty;

    (*p_mapParam)[sParam].first = vec0;
    (*p_mapParam)[sParam].second = vec1;
    p_mapKey->push_back(sParam);
  }
}

#if USED_CODE_IN_COVERAGE_TEST
string add_tail(string str, int len) {
  int j;
  for (j = str.size(); j < len; j++) {
    str += " ";
  }
  return str;
}

string translate_pragma(string sTaskName, string sPragma, string prefix) {
  size_t j = 0, i;
  string str = "";
  string sParam, sList;
  string sFilter = get_sub_delimited(sPragma, j, ' ');
  j += sFilter.size() + 1;

  if (!is_cmost_pragma(sFilter))
    return "";

  if (USE_LOWERCASE_NAME) {
    sTaskName = get_lower_case(sTaskName);
  }

  string sCmd = get_sub_delimited(sPragma, j, ' ');
  j += sCmd.size() + 1;

  if ("iteration_domain" == sCmd) {
    vector<string> dummy, vecIter, vecMax;
    j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vecIter, &dummy);
    assert(sParam == "iterator");

    j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vecMax, &dummy);
    assert(sParam == "max_range");

    vector<string> vecCond;
    for (i = 0; i < vecMax.size(); i++) {
      vector<string> min_max;
      str_split(vecMax[i], '.', min_max);

      //  2013-10-08
      if (0) {
        vecCond.push_back(min_max[0] + " .LE " + vecIter[i]);
        vecCond.push_back(vecIter[i] + " .LE " + min_max[2]);
      } else {
        vecCond.push_back("0 .LE 1");
      }
    }

    if (0 == vecIter.size()) {  //  fixed by pengzhang 12:26 AM 3/29/2013
      //  do nothing, <> is the default iteration domain for the single instance
    } else {
      str += "<" + sTaskName + "_set : " + str_merge(',', vecIter) + ">";
      str += "{" + str_merge(';', vecCond) + ";};\n";
    }

#if PRINT_INFO
    {
      vector<string> vecCond_print;
      for (i = 0; i < vecMax.size(); i++) {
        vecCond_print.push_back("0<=" + vecIter[i]);
        vecCond_print.push_back(vecIter[i] + "<" + vecMax[i]);
      }

      string sTaskName1 =
          sTaskName;  //  .substr(0, sTaskName.size() - strlen("_kernel"));
      string str_print =
          "[tldm_gen] Detected a TLDM task  : " + add_tail(sTaskName1, 9) +
          "{" + str_merge(',', vecIter) + "|" + str_merge(',', vecCond_print) +
          "}\n";
      std::cout << str_print;
    }

#endif  //  PRINT_INFO

    return str;
  }

  if ("tuning" == sCmd) {
    vector<string> vecIter, vecOrder, vecCond, vecTile, dummy, vec0, vec1;
    list<pair<string, string>> vecAdditional;
    j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vecIter, &dummy);
    assert(sParam == "iterator");

    j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vecOrder, &vecCond);
    string sCond = str_merge(',', vecCond);
    sCond = str_replace(sCond, "<=", " .LE ");
    sCond = str_replace(sCond, ">=", " .GE ");
    sCond = str_replace(sCond, "=", " .EQ ");
    sCond = str_replace(sCond, ">", " .GT ");
    sCond = str_replace(sCond, "<", " .LT ");
    sCond = str_replace(sCond, "(", "((");
    sCond = str_replace(sCond, ")", "))");
    assert(sParam == "order");

    if (0 == vecIter.size())  //  fixed by pengzhang 12:26 AM 3/29/2013
      str += "<>::(" + sTaskName + ") ";
    else
      str += "<" + sTaskName + "_set>::(" + sTaskName + ": " +
             str_merge(',', vecIter) + ") ";

    size_t j0 = str.size();
    while (j < sPragma.size()) {
      j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vec0, &vec1);
      vecAdditional.push_back(
          pair<string, string>(sParam, str_merge(',', vec0)));
    }

    if ("" != sCond) {
      vecAdditional.push_front(pair<string, string>("condition", sCond));
    }
    vecAdditional.push_front(
        pair<string, string>("order", str_merge(',', vecOrder)));

    if (0 != vecAdditional.size())
      str += "{";
    list<pair<string, string>>::iterator it;
    int bfound = 0;
    for (it = vecAdditional.begin(); it != vecAdditional.end(); it++) {
      if (it->first == "module_type")
        bfound = 1;

      if ("" != it->second) {  //  fixed by pengzhang 12:26 AM 3/29/2013
        string value_exp = it->second;
        str += it->first + "(\"" + value_exp + "\"); ";
      }
    }
    size_t j1 = str.size();

    //  ZP: 2014-01-29, no wrapper type any more
    //  this type will be converted to wrapped if it is hardware implementation
    if (bfound == 0)
      str += "module_type(wrapper); ";

    if (0 != vecAdditional.size())
      str += "}";

    str += ";\n";

#if PRINT_INFO
    {
      string sTaskName1 =
          sTaskName;  //  .substr(0, sTaskName.size() - strlen("_kernel"));
      string str_print =
          "[tldm_gen] Detected a TLDM attrib: " + add_tail(sTaskName1, 9) +
          ":: " + str.substr(j0, j1 - j0) + "}\n";

      std::cout << str_print;
    }

#endif  //  PRINT_INFO

    return str;
  }

  if ("access" == sCmd) {
    string sRef = "", sDir = "r", sPort;
    vector<string> vecIter, vecDims, vecCond, vec0, vec1;
    list<pair<string, string>> vecAdditional;
    while (j < sPragma.size()) {
      j = tldm_pragma_parse_one_term(sPragma, j, &sParam, &vec0, &vec1);

      if (sParam == "name") {
        sRef = vec0[0];
      } else if (sParam == "dir") {
        sDir = vec0[0];
      } else if (sParam == "port") {
        sPort = vec0[0];
      } else if (sParam == "iterator") {
        vecIter = vec0;
      } else if (sParam == "pattern") {
        vecDims = vec0;
        vecCond = vec1;
      } else {
        pair<string, string> more;
        more.first = sParam;
        more.second = str_merge(',', vec0);
        vecAdditional.push_back(more);
      }
    }

    if (USE_LOWERCASE_NAME) {
      sPort = get_lower_case(sPort);
    }

    string data_ref = "[" + sPort;
    if (0 != vecDims.size())
      data_ref += " : " + str_merge(',', vecDims);
    data_ref += "]";
    data_ref = str_replace(data_ref, "(", "((");
    data_ref = str_replace(data_ref, ")", "))");
    string ann_str = "";
    if (0 != vecCond.size() || "" != sRef || 0 != vecAdditional.size()) {
      ann_str = "{ ";
      if ("" != sRef)
        ann_str += "name(" + sRef + "); ";
      if (0 != vecCond.size()) {
        string sCond = str_merge(',', vecCond);
        sCond = str_replace(sCond, "<=", " .LE ");
        sCond = str_replace(sCond, ">=", " .GE ");
        sCond = str_replace(sCond, "=", " .EQ ");
        sCond = str_replace(sCond, ">", " .GT ");
        sCond = str_replace(sCond, "<", " .LT ");
        sCond = str_replace(sCond, "(", "((");
        sCond = str_replace(sCond, ")", "))");
        ann_str += "condition(" + sCond + "); ";
      }
      list<pair<string, string>>::iterator it;
      for (it = vecAdditional.begin(); it != vecAdditional.end(); it++) {
        string value_exp = it->second;
        ann_str += it->first + "(\"" + value_exp + "\"); ";
      }
      ann_str += "}";
    }
    string task_iter = "";
    if (0 != vecIter.size()) {
      task_iter = ":" + str_merge(',', vecIter);
    }
    string task_ref = "(" + sTaskName + task_iter + ")";

    if ("r" == sDir) {
      str = data_ref + " " + ann_str + " -> " + task_ref + ";";
    } else if ("w" == sDir) {
      str = task_ref + " -> " + data_ref + " " + ann_str + ";";
    } else if ("rw" == sDir) {
      str = task_ref + " -> " + data_ref + " " + ann_str +
            ";";  //  for the unknown case, the type has already been set to
                  //  bus_e1
    }

#if PRINT_INFO
    {
      string strDir = (sDir == "r") ? "<-" : "->";
      string str_print =
          "[tldm_gen] Detected a TLDM access: " + add_tail(sTaskName, 9) +
          strDir + " " + sPort + "[" + str_merge(',', vecDims) + "]\n";
    }
#endif  //  PRINT_INFO

    return str;
  }

  //  ZP: 2015-04-12: Ignore other pragmas
  //  return sPragma;
  return "";
}

//  vector<string> extract_one_task(string sTaskName, void *pTaskBegin) {
//  vector<string> ret;
//  ret.push_back("tldm iteration_domain iterator=\"bx, by\" max_range=\"2,
//  2\""); ret.push_back(
//      "tldm tuning iterator=\"bx, by\" order=\"i, j, ii, jj : 0<=ii; ii<=2; "
//      "0<=jj; jj<=2; ii=bx-2*i; jj=by-2*j\" additional = \"val1\"");
//  return ret;
//  }

void one_region_to_basicblock(CSageCodeGen *codegen, string sComponentName,
                              void *pTaskBegin, string sComponent,
                              string sAdditional) {
  int i, i0 = -1, i1 = -1;

  //  1. find the pTaskEnd
  void *pParent = codegen->GetParent(pTaskBegin);
  void *pTaskEnd = 0;
  int found = 0;
  for (i = 0; i < codegen->GetChildStmtNum(pParent); i++) {
    void *sg_stmt = codegen->GetChildStmt(pParent, i);
    if (!found && sg_stmt != pTaskBegin) {
      continue;
    }
    if (!found && sg_stmt == pTaskBegin) {
      i0 = i;
    }
    found = 1;

    string str = codegen->GetPragmaString(sg_stmt);
    if ("" == str)
      continue;
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(str, sFilter, sCmd, mapParams);
    if (sComponent + "_end" == sCmd) {
      pTaskEnd = sg_stmt;
      i1 = i;
      //  found1 = 1;
      break;
    }
    //  std::cout << sComponent+"_end" << sCmd << endl;
  }

  if (i == codegen->GetChildStmtNum(pParent)) {
    printf("[mars_opt] ERROR: %s_end is not found in the basicblock of "
           "%s_begin %s\n",
           sComponent.c_str(), sComponent.c_str(), sComponentName.c_str());
    assert(0);
    return;
  }

  //  by zhangpeng 2013-05-23, to make sure the
  if (codegen->GetParent(pTaskBegin) != codegen->GetParent(pTaskEnd)) {
    printf("[mars_opt] ERROR: tldm task_begin (%s) and task_end are not in the "
           "same basicblock. \n",
           sComponent.c_str());
    assert(0);
    return;
  }

  //  2. create and replace the BB
  void *newBB = codegen->CreateBasicBlock();
  vector<void *> vecChildsToReplace;
  for (i = i0 + 1; i < i1 + 1; i++) {
    void *sg_stmt = codegen->GetChildStmt(pParent, i);
    if (i != i1)
      codegen->AppendChild(newBB, codegen->CopyStmt(sg_stmt));
    vecChildsToReplace.push_back(sg_stmt);
  }

  //  3. replace the old code
  void *sg_stmt = codegen->GetChildStmt(pParent, i0);
  codegen->ReplaceChild(sg_stmt, newBB);
  for (size_t i = 0; i < vecChildsToReplace.size(); i++) {
    codegen->RemoveChild(vecChildsToReplace[i]);
  }

  //  4. add new pragma
  void *sg_new_pragma = codegen->CreatePragma(
      "tldm " + sComponent + "_block" + sAdditional + "\n", pParent);
  codegen->InsertStmt(sg_new_pragma, newBB);
}

int tldm_item_exist(
    const map<string, pair<vector<string>, vector<string>>> &mapParams,
    string name) {
  if (mapParams.end() == mapParams.find(name))
    return 0;
  if (0 == mapParams[name].first.size())
    return 0;
  return 1;
}

//  added by Hui
void dump_cfg_xml(
    map<string, vector<int>> mapArray2DimLength,
    map<string, int> mapArray2Length, map<string, int> mapArray2Type,
    /*map<string, string> mapArray2TypeName, */ map<string, int> mapArray2Index,
    map<string, int> mapArray2IndexOut, map<string, string> mapArray2InFile,
    map<string, string> mapArray2OutFile) {
  //  ofstream cfg_out("testing.cfg.xml");
  FILE *cfg_out;
  cfg_out = fopen("testing.cfg.xml", "w");

  map<string, int> mapArray2AddrOffset;

  int curr_offset = 0;

  fprintf(cfg_out, "<configuration>\n");

  fprintf(cfg_out, "    <param_tree name=\"input\">\n");
  for (map<string, string>::iterator mi = mapArray2InFile.begin();
       mi != mapArray2InFile.end(); mi++) {
    string arr_name = mi->first;
    string infile_name = mi->second;
    if (DEBUG)
      cout << "DEBUG: arr_name" << arr_name << endl;
    assert(mapArray2Type.count(arr_name) > 0);
    int type_id = mapArray2Type[arr_name];
    assert(mapArray2TypeName.count(arr_name) > 0);
    string type = mapArray2TypeName[arr_name];
    assert(mapArray2Index.count(arr_name) > 0);
    int index = mapArray2Index[arr_name];
    assert(mapArray2Length.count(arr_name) > 0);
    int length = mapArray2Length[arr_name];

    //  ZP: use string is easier for debug
    //     pls refer to cl_platform.h for the enumeration of data_type
    string str_type = "";
    if (index == 0)
      str_type = "float";
    else if (index == 1)
      str_type = "int";
    else
      str_type = "unknown";

    fprintf(cfg_out, "        <item name = \"%s\">\n", arr_name.c_str());
    fprintf(cfg_out, "            <index>%d</index>\n", index);
    fprintf(cfg_out, "            <type>%s</type>\n", type.c_str());
    fprintf(cfg_out, "            <unit_size>%d</unit_size>\n", type_id);
    fprintf(cfg_out, "            <filename>%s</filename>\n",
            infile_name.c_str());
    fprintf(cfg_out, "            <length>%d</length>\n", length);

    vector<int> length_vector = mapArray2DimLength[arr_name];
    fprintf(cfg_out, "            <dim>");
    for (size_t dim_idx = 0; dim_idx < length_vector.size(); dim_idx++) {
      fprintf(cfg_out, "%d", length_vector[dim_idx]);
      if (dim_idx < length_vector.size() - 1)
        fprintf(cfg_out, ",");
    }
    fprintf(cfg_out, "</dim>\n");

    //  fprintf(cfg_out, "            <DRAM_addr>%s +
    //  0x%08x</DRAM_addr>\n", dram_addr_base.c_str(), curr_offset);
    fprintf(cfg_out, "            <addr_offset>0x%08x</addr_offset>\n",
            curr_offset);
    fprintf(cfg_out, "        </item>\n");

    mapArray2AddrOffset[arr_name] = curr_offset;
    curr_offset += length * 4;
  }
  fprintf(cfg_out, "    </param_tree>\n");

  fprintf(cfg_out, "    <param_tree name=\"output\">\n");
  for (map<string, string>::iterator mi = mapArray2OutFile.begin();
       mi != mapArray2OutFile.end(); mi++) {
    string arr_name = mi->first;
    string outfile_name = mi->second;

    //  ZP: 20160703
    //  assert(mapArray2Type.count(arr_name) > 0);
    if (mapArray2Type.count(arr_name) == 0)
      continue;

    int type_id = mapArray2Type[arr_name];

    assert(mapArray2TypeName.count(arr_name) > 0);
    string type = mapArray2TypeName[arr_name];

    assert(mapArray2Length.count(arr_name) > 0);
    assert(mapArray2Index.count(arr_name) > 0);
    int index = mapArray2IndexOut[arr_name];
    int length = mapArray2Length[arr_name];

    fprintf(cfg_out, "        <item name = \"%s\">\n", arr_name.c_str());
    fprintf(cfg_out, "            <index>%d</index>\n", index);
    fprintf(cfg_out, "            <type>%s</type>\n", type.c_str());
    fprintf(cfg_out, "            <unit_size>%d</unit_size>\n", type_id);
    fprintf(cfg_out, "            <filename>%s</filename>\n",
            outfile_name.c_str());
    fprintf(cfg_out, "            <length>%d</length>\n", length);
    vector<int> length_vector = mapArray2DimLength[arr_name];
    fprintf(cfg_out, "            <dim>");
    for (size_t dim_idx = 0; dim_idx < length_vector.size(); dim_idx++) {
      fprintf(cfg_out, "%d", length_vector[dim_idx]);
      if (dim_idx < length_vector.size() - 1)
        fprintf(cfg_out, ",");
    }
    fprintf(cfg_out, "</dim>\n");
    fprintf(cfg_out, "            <addr_offset>0x%08x</addr_offset>\n",
            mapArray2AddrOffset[arr_name]);
    fprintf(cfg_out, "        </item>\n");
  }
  fprintf(cfg_out, "    </param_tree>\n");

  fprintf(cfg_out, "    <param name=\"nof_iteration\">1</param>\n");
  fprintf(cfg_out, "</configuration>\n");

  fclose(cfg_out);
}

//  ZP: 2013-10-24: add the array index into the first arguemnt of the cmost_
//  function calls
void *create_call_of_cmost_func_by_adding_index(CSageCodeGen *codegen,
                                                int index,
                                                void *func_call_org) {
  string sFuncName = codegen->GetFuncNameByCall(func_call_org);

  vector<void *> param_list;

  param_list.push_back(codegen->CreateConst(index));
  for (int i = 0; i < codegen->GetFuncCallParamNum(func_call_org); i++) {
    param_list.push_back(
        codegen->CopyExp(codegen->GetFuncCallParam(func_call_org, i)));
  }
  void *return_type = codegen->GetFuncReturnTypeByCall(func_call_org);
  void *func_call_new = codegen->CreateFuncCall(
      sFuncName, return_type, param_list, codegen->GetScope(func_call_org));
  return func_call_new;
}

void testing_xml_gen(CSageCodeGen *codegen, void *pTopFunc, bool addIdx) {
  size_t i;

  //  analyze user-specified parameter range
  vector<string> kernel_invar_param;
  map<string, int> mapParam2MaxValue;

  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (is_cmost_pragma(sFilter) && "parameter" == sCmd) {
      //  added by Hui
      string sVarName;
      if (mapParams["variable"].first.size() > 0) {
        sVarName = mapParams["variable"].first[0];
        if (DEBUG)
          printf("Found kernel-invariant variable %s", sVarName.c_str());

        //  To be fixed: do not allow the kernel-invariant parameters share the
        //  same variable names
        for (size_t invar_i = 0; invar_i < kernel_invar_param.size();
             invar_i++) {
          if (kernel_invar_param[invar_i] == sVarName) {
            assert(0);
          }
        }
        kernel_invar_param.push_back(sVarName);

        string sRange;
        if (mapParams["range"].first.size() > 0) {
          sRange = mapParams["range"].first[0];
          assert(sRange.find("..") != string::npos);
          int pos = sRange.find("..");
          sRange = sRange.substr(pos + 2);
          mapParam2MaxValue[sVarName] = atoi(sRange.c_str());

          if (DEBUG)
            printf(" with maximal value %d\n", atoi(sRange.c_str()));
        }
      }
      continue;
    }
  }

  map<string, int> mapArray2Length;
  map<string, vector<int>> mapArray2DimLength;
  map<string, int> mapArray2Type;
  map<string, int> mapArray2Index;
  map<string, int> mapArray2IndexOut;  //  ZP: added for the index of output,
                                       //  which is different to the input
  map<string, string> mapArray2InFile;
  map<string, string> mapArray2OutFile;
  set<string> free_arrays;

  vector<void *> vecFuncs;
  void *sg_project = codegen->GetProject();
  codegen->GetNodesByType(sg_project, "preorder", "SgFunctionCallExp",
                          &vecFuncs);
  if (DEBUG)
    cout << "Debug: vecFuncs size = " << vecFuncs.size() << endl;
  int index_value = 0;
  int param_idx = 0;

  map<void *, void *> mapOld2NewCall;

  for (i = 0; i < vecFuncs.size(); i++) {
    void *func_decl = codegen->GetFuncDeclByCall(vecFuncs[i], 0);
    string sFuncName = codegen->GetFuncName(func_decl);

    if (sFuncName.find("cmost_malloc") != string::npos) {
      int is_brief = 0, dim = 1;
      if (sFuncName == "cmost_malloc") {
        is_brief = 1;
      } else {
        int pos = sFuncName.find('d');  //  cmost_data_type_'dim'd
        string dim_str = sFuncName.substr(13, pos - 13);
        dim = atoi(dim_str.c_str());
      }

      if (DEBUG)
        cout << "DEBUG: cmost malloc " << codegen->UnparseToString(vecFuncs[i])
             << endl;

      //  Argument 1: pointer to allocate
      param_idx = 0;

      //  ///////////////////////////////  /
      //  XP:20150623
      int is_cast = 0;
      void *array_init =
          ((codegen->GetFuncCallParam(vecFuncs[i], param_idx++)));
      void *sg_first_param = array_init;
      void *sg_first_param_no_cast = NULL;
      codegen->remove_cast(&array_init);
      sg_first_param_no_cast = array_init;
      if (codegen->IsAddressOfOp(array_init)) {
        array_init = (static_cast<SgUnaryOp *>(array_init)->get_operand_i());
      } else {
        assert(0);
      }
      //  void* array_init =
      //  ((SgUnaryOp*)(codegen->GetFuncCallParam(vecFuncs[i],
      //  param_idx++)))->get_operand_i();
      //  ///////////////////////////////  /

      string array_name = codegen->UnparseToString(array_init);
      void *array_init_exp = codegen->GetVariableInitializedName(array_init);
      vector<size_t> nSizes;
      void *base_type;
      codegen->get_pointer_dimension(
          (static_cast<SgInitializedName *>(array_init_exp)->get_type(),
           base_type, nSizes));
      string type_name =
          codegen->UnparseToString(codegen->GetOrigTypeByTypedef(base_type));
      int unit_size =
          (type_name.find("int64_t") != string::npos)
              ? 64
              : (type_name.find("double") != string::npos)
                    ? 64
                    : (type_name.find("float") != string::npos)
                          ? 32
                          : (type_name.find("int") != string::npos)
                                ? 32
                                : (type_name.find("short") != string::npos)
                                      ? 16
                                      : (type_name.find("char") != string::npos)
                                            ? 8
                                            : (type_name.find("byte") !=
                                               string::npos)
                                                  ? 8
                                                  : 32;

      //  ZP: 21050616: to make consistent of the rose output
      //  if (!is_cast) codegen->AddCastToExp(sg_first_param, "void**");
      if (is_cast) {
        //  remove the cast and add a new one again
        void *new_first_param = codegen->CopyExp(sg_first_param_no_cast);
        codegen->ReplaceExp(sg_first_param, new_first_param);
        sg_first_param = new_first_param;
      }
      codegen->AddCastToExp(sg_first_param, "void * *");

      //  Argument 2: file_name
      string file_name;
      if (!is_brief) {
        file_name = codegen->UnparseToString(
            codegen->GetFuncCallParam(vecFuncs[i], param_idx++));
        int pos1 = file_name.find("\"");
        int pos2 = file_name.rfind("\"");
        file_name = file_name.substr(pos1 + 1, pos2 - pos1 - 1);
      } else {
        file_name = "";
      }

      //  Argument 3: unit size
      int type_id;
      if (!is_brief) {
        string elem_size_str = codegen->UnparseToString(
            codegen->GetFuncCallParam(vecFuncs[i], param_idx++));
        type_id = atoi(elem_size_str.c_str());
        if (type_id != 4) {
          printf("\n[mars_opt] ERROR: The 3rd argument %s of %s is not 4 (the "
                 "unit data size).\n\n",
                 elem_size_str.c_str(), sFuncName.c_str());
          exit(1);
        }
      }
      type_id = unit_size / 8;

      //  Argument 3: unit size
      int numParam = codegen->GetFuncCallParamNum(vecFuncs[i]);
      int numParam_exp = (is_brief) ? 2 : (3 + dim);
      if (numParam != numParam_exp) {
        printf("\n[mars_opt] ERROR: cmost_malloc_%d_d() is specified with %d "
               "array dimensions.\n\n",
               dim, numParam - 3);
        exit(1);
      }
      int array_length = 1;
      string dim_set_str;

      //  calculate array size
      for (int nparam = param_idx; nparam < numParam; nparam++) {
        void *param_i = codegen->GetFuncCallParam(vecFuncs[i], nparam);
        string param_i_str = codegen->UnparseToString(param_i);

        //  ZP: 20150806, fix the problem on mhs_cfg.xml
        if (is_brief) {
          param_i_str = "(" + param_i_str + ")/" + my_itoa(unit_size / 8);
        }

        int ret = 1;

        //  ZP: 20150603
        int value = 0;
        if (0) {
          map<string, int> mapVar2Coeff;
          //  cout << "asdf:" << codegen->UnparseToString(param_i) << endl;
          ret &= codegen->analyze_linear_expression(param_i, mapParam2MaxValue);
          if (ret == 0) {
            cout << "0\n";
          }

          if (mapVar2Coeff.size() > 1) {
            ret = 0;
            cout << "1\n";
          } else if (mapParam2MaxValue.size() == 0) {
            value = 0;
          } else if (mapParam2MaxValue.find("1") == mapParam2MaxValue.end()) {
            ret = 0;
            cout << "2\n";
          } else {
            value = mapParam2MaxValue["1"];
          }
        } else {
          //  ZP: 20150730
          MarsProgramAnalysis::CMarsExpression me(param_i, codegen);

          if (!me.IsConstant()) {
            string sFileName = codegen->GetFileInfo_filename(param_i);
            printf("\n[mars_opt] ERROR: Array length %s in function call %s is "
                   "not an integer constant: \n%s, line %d:\n    Call: %s\n    "
                   "Size: %s\n\n",
                   param_i_str.c_str(), sFuncName.c_str(), sFileName.c_str(),
                   codegen->GetFileInfo_line(param_i),
                   codegen->UnparseToString(vecFuncs[i]).c_str(),
                   me.unparse().c_str());

            exit(1);
          }

          value = me.GetConstant();
        }

        //  void* ref_copy = codegen->CopyExp(param_i);

        dim_set_str += param_i_str;
        if (nparam < numParam - 1)
          dim_set_str += ",";

        if (DEBUG)
          cout << "DEBUG: arg_" << nparam << " = " << param_i_str << endl;
        //  cout<<"DEBUG: arg_"<<nparam<<" = "<<param_i_str<<endl;
        int length_at_curr_dim = 1;
        if (mapParam2MaxValue.count(param_i_str) > 0) {
          length_at_curr_dim = mapParam2MaxValue[param_i_str.c_str()];
        } else {
          length_at_curr_dim =
              value;  //  atoi(param_i_str.c_str()); //  ZP; 20150603
        }
        if (is_brief) {
          length_at_curr_dim /= (unit_size / 8);
        }
        array_length *= length_at_curr_dim;
        mapArray2DimLength[array_name].push_back(length_at_curr_dim);
      }
      mapArray2Length[array_name] = array_length;
      mapArray2InFile[array_name] = file_name;
      mapArray2Type[array_name] = type_id;
      mapArray2TypeName[array_name] = type_name;
      mapArray2DimStr[array_name] = dim_set_str;

      mapArray2Index[array_name] = index_value++;
      if (DEBUG)
        cout << "DEBUG: init " << type_name << " array " << array_name
             << " elem_size = " << type_id << " length = " << array_length
             << " file name = " << file_name << " dim = " << dim_set_str
             << endl;

      //  ZP: 2013-10-24, adddding the index argument:
      if (addIdx) {
        void *func_call_new = create_call_of_cmost_func_by_adding_index(
            codegen, mapArray2Index[array_name], vecFuncs[i]);
        mapOld2NewCall[vecFuncs[i]] = func_call_new;
      }
    } else if (sFuncName.find("cmost_dump") != string::npos) {
      if (DEBUG)
        cout << "DEBUG: cmost dump " << codegen->UnparseToString(vecFuncs[i])
             << endl;

      string array_name =
          codegen->UnparseToString(codegen->GetFuncCallParam(vecFuncs[i], 0));
      string file_name =
          codegen->UnparseToString(codegen->GetFuncCallParam(vecFuncs[i], 1));
      int pos1 = file_name.find("\"");
      int pos2 = file_name.rfind("\"");
      file_name = file_name.substr(pos1 + 1, pos2 - pos1 - 1);

      mapArray2OutFile[array_name] = file_name;
      mapArray2IndexOut[array_name] = index_value++;

      if (mapArray2Length.count(array_name) == 0) {
        cout << "[mars_opt] WARNING: Missing cmost_malloc() for array \""
             << array_name << "\" referred in cmost_dump()." << endl;
      }

      dump_array.insert(array_name);
      if (DEBUG)
        cout << "DEBUG: dump array " << array_name << " "
             << "length = " << mapArray2Length[array_name]
             << " file name = " << file_name << endl;

      //  ZP: 2013-10-24, adding the index argument:
      if (addIdx) {
        assert(mapArray2Index.find(array_name) != mapArray2Index.end());
        void *func_call_new = create_call_of_cmost_func_by_adding_index(
            codegen, mapArray2IndexOut[array_name], vecFuncs[i]);
        mapOld2NewCall[vecFuncs[i]] = func_call_new;
      }
    } else if (sFuncName.find("cmost_free") != string::npos) {
      string array_name =
          codegen->UnparseToString(codegen->GetFuncCallParam(vecFuncs[i], 0));

      free_arrays.insert(array_name);
      //  ZP: 2013-10-24, adding the index argument:
      if (addIdx) {
        assert(mapArray2Index.find(array_name) != mapArray2Index.end());
        void *func_call_new = create_call_of_cmost_func_by_adding_index(
            codegen, mapArray2Index[array_name], vecFuncs[i]);
        mapOld2NewCall[vecFuncs[i]] = func_call_new;
      }
    }
  }

  for (map<string, int>::iterator mi = mapArray2Length.begin();
       mi != mapArray2Length.end(); mi++) {
    if (free_arrays.count(mi->first) == 0) {
      cout << "[mars_opt] WARNING: Missing cmost_free() for array \""
           << mi->first.c_str() << "\".\n";
    }
  }

  //  check whether argument name remains unchanged in function call tree.
  //    map<string, map<void*, set<int> > > mapParam2FuncIdx;

  for (i = 0; i < vecFuncs.size(); i++) {
    void *func_decl = codegen->GetFuncDeclByCall(vecFuncs[i], 0);
    if (func_decl == NULL)
      continue;
    string sFuncName = codegen->GetFuncName(func_decl);

    if (sFuncName.find("cmost_malloc") != string::npos ||
        sFuncName.find("cmost_free") != string::npos ||
        sFuncName.find("cmost_dump") != string::npos)
      continue;
    func_decl = codegen->GetFuncDeclByCall(vecFuncs[i], 1);
    if (func_decl == NULL)
      continue;

    int numParam = codegen->GetFuncCallParamNum(vecFuncs[i]);

    for (int nparam = 0; nparam < numParam; nparam++) {
      void *param_i = codegen->GetFuncCallParam(vecFuncs[i], nparam);
      string array_name_in_func_call = codegen->UnparseToString(param_i);
      if (mapArray2Length.count(array_name_in_func_call) > 0) {
        //    mapParam2FuncIdx[mapArray2Length[param_i_str]][func_decl].insert(nparam);
        void *param_si = codegen->GetFuncParam(func_decl, nparam);
        string array_name_in_func_decl = codegen->UnparseToString(param_si);

        if (strcmp(array_name_in_func_decl.c_str(),
                   array_name_in_func_call.c_str()) != 0) {
          //  /                    cout<<"\n[mars_opt] ERROR:
          //  / Argument
          //  /"<<array_name_in_func_call<<" in function call
          //  \""<<sFuncName<<"\" / does not match the name
          //  "<<array_name_in_func_decl<<" used in its / function declaration.
          //  \n\n"; / exit(1);
        }
      }
    }
  }

  for (map<void *, void *>::iterator it = mapOld2NewCall.begin();
       it != mapOld2NewCall.end(); it++) {
    codegen->ReplaceExp(it->first, it->second);
  }

  dump_cfg_xml(mapArray2DimLength, mapArray2Length, mapArray2Type,
               /*mapArray2TypeName, */ mapArray2Index, mapArray2IndexOut,
               mapArray2InFile, mapArray2OutFile);
}

//  instrument_type can be "test", ...
void instrument_gen(CSageCodeGen *codegen, void *pTopFunc,
                    string instrument_type, vector<string> include_file) {
  size_t i, j;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (!is_cmost_pragma(sFilter) || ("instrument" != sCmd))
      continue;

    if (!tldm_item_exist(mapParams, "type")) {
      printf("[mars_opt] WARNING: type missing in instrument pragma: \n\t%s\n",
             vecTldmPragmas[i].second.c_str());
      continue;
    }
    string sInstrumentType = mapParams["type"].first[0];
    if (instrument_type != sInstrumentType)
      continue;

    if (!tldm_item_exist(mapParams, "func_name")) {
      printf("[mars_opt] WARNING: func_name missing in instrument pragma: "
             "\n\t%s\n",
             vecTldmPragmas[i].second.c_str());
      continue;
    }
    string sFuncName = mapParams["func_name"].first[0];

    string sFuncArg;
    if (!tldm_item_exist(mapParams, "func_args")) {
      sFuncArg = "";
    } else {
      sFuncArg = str_merge(',', mapParams["func_args"].first);
    }
    string statement = "\n" + sFuncName + "(" + sFuncArg + ");" + "\n";
    codegen->AddDirectives(statement, vecTldmPragmas[i].first,
                           0);  //  0 for after, 1 for before
  }

  set<void *> header_inserted;
  if (0 != include_file.size()) {
    void *global = codegen->GetGlobal(0);
    if (header_inserted.find(global) ==
        header_inserted.end()) {  //  if not exist previously
      for (j = 0; j < include_file.size(); j++) {
        codegen->AddHeader("\n#include \"" + include_file[j] + "\"\n", global);
      }

      header_inserted.insert(global);
    }
  }
}

void all_region_to_basicblock(CSageCodeGen *codegen, void *pTopFunc,
                              string sComponent) {
  size_t i, j;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);
    if (!is_cmost_pragma(sFilter) || (sComponent + "_begin" != sCmd))
      continue;
    string sComponentName;
    if (mapParams["name"].first.size() > 0) {
      sComponentName = mapParams["name"].first[0];
    } else {
      static int idx = 0;
      sComponentName = "unmaned_" + sComponent + my_itoa(idx++);
    }
    pair<vector<string>, vector<string>> pair_name;
    pair_name.first.push_back(sComponentName);
    mapParams["name"] = pair_name;

    string sAdditional;
    {
      map<string, pair<vector<string>, vector<string>>>::iterator it;
      for (it = mapParams.begin(); it != mapParams.end(); it++) {
        {
          sAdditional += " " + it->first;
          if (it->second.first.size()) {
            sAdditional += "=\"";
            for (j = 0; j < it->second.first.size(); j++) {
              if (j != 0)
                sAdditional += ",";
              sAdditional += it->second.first[j];
            }
            for (j = 0; j < it->second.second.size(); j++) {
              if (j != 0)
                sAdditional += ";";
              else
                sAdditional += ":";
              sAdditional += it->second.second[j];
            }
            sAdditional += "\"";
          }
          sAdditional += " ";
        }
      }
    }

    void *pTaskBegin = vecTldmPragmas[i].first;
    one_region_to_basicblock(codegen, sComponentName, pTaskBegin, sComponent,
                             sAdditional);
  }
}

void one_array_add_base(CSageCodeGen *codegen, string sArray, void *sg_scope) {
  // TODO(XXX) : Finish this function.
}

void one_graph_to_tldm(CSageCodeGen *codegen, void *pGraph) {}

void *trace_up_for_graph_block(CSageCodeGen *codegen, void *sg_node_) {
  void *sg_node = sg_node_;
  while (sg_node) {
    sg_node = codegen->GetParent(sg_node);

    if (!sg_node)
      break;
    if (!codegen->IsBasicBlock(sg_node))
      continue;

    void *sg_parent = codegen->GetParent(sg_node);

    size_t stmt_idx = codegen->GetChildStmtIdx(sg_parent, sg_node);
    if (stmt_idx == 0 || stmt_idx >= codegen->GetChildStmtNum(sg_parent))
      continue;
    void *previous = codegen->GetChildStmt(sg_parent, stmt_idx - 1);
    cout << "previous = " << codegen->UnparseToString(previous) << endl;
    string sPragma = codegen->GetPragmaString(previous);
    if ("" == sPragma)
      continue;

    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(sPragma, sFilter, sCmd, mapParams);
    if (!is_cmost_pragma(sFilter) ||
        (("graph_block" != sCmd) && ("graph_begin" != sCmd)))
      continue;  //  bugfix? LIPENG

    void *graph_bb = sg_node;
    return graph_bb;
  }
  return nullptr;
}

int get_map_additionals(CSageCodeGen *codegen, string sPort, void *sg_scope,
                        map<string, string> *p_mapAdditional) {
  size_t i, j;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(sg_scope, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);
    if (!is_cmost_pragma(sFilter) || ("access_ann" != sCmd))
      continue;
    if (mapParams.find("port") == mapParams.end())
      assert(0);
    if (mapParams["port"].first.size() != 1)
      assert(0);
    if (mapParams["port"].second.size() != 0)
      assert(0);
    if ((mapParams["port"].first)[0] != sPort)
      continue;

    map<string, pair<vector<string>, vector<string>>>::iterator it;
    for (it = mapParams.begin(); it != mapParams.end(); it++) {
      if (it->first != "port") {
        string sAddValue = "";
        for (j = 0; j < it->second.first.size(); j++) {
          if (j != 0)
            sAddValue += ",";
          sAddValue += it->second.first[j];
        }
        for (j = 0; j < it->second.second.size(); j++) {
          if (j != 0)
            sAddValue += ";";
          else
            sAddValue += ":";
          sAddValue += it->second.second[j];
        }

        *p_mapAdditional[it->first] = sAddValue;
      }
    }
  }

  return 1;
}

void func_decl_check(CSageCodeGen *codegen) {  //  , map<string, int> kname2Id)
  int numGlobal = codegen->GetGlobalNum();

  void *sg_project = codegen->GetProject();
  set<string> globalFuncNames;
  vector<void *> vecDecls_g;
  codegen->GetNodesByType(sg_project, "preorder", "SgFunctionDeclaration",
                          &vecDecls_g);

  for (size_t i = 0; i < vecDecls_g.size(); i++)
    globalFuncNames.insert(codegen->GetFuncName(vecDecls_g[i]));

  for (int i = 0; i < numGlobal; i++) {
    //  extract scope
    void *sg_scope_global = codegen->GetGlobal(i);

    vector<void *> vecDecls;
    vector<void *> vecCalls;
    set<string> localFuncNames;

    codegen->GetNodesByType(sg_scope_global, "preorder", "SgFunctionCallExp",
                            &vecCalls);
    codegen->GetNodesByType(sg_scope_global, "preorder",
                            "SgFunctionDeclaration", &vecDecls);
    for (size_t j = 0; j < vecDecls.size(); j++) {
      localFuncNames.insert(codegen->GetFuncName(vecDecls[j]));
      if (globalFuncNames.count(codegen->GetFuncName(vecDecls[j])) > 0)
        cout << codegen->GetFuncName(vecDecls[j]) << "\t";
    }
    cout << "\n ########################################" << endl;
    set<string> localCall;
    for (size_t j = 0; j < vecCalls.size(); j++) {
      void *associate_func_decl =
          codegen->GetAssociatedFuncDeclByCall(vecCalls[j]);

      if (associate_func_decl == NULL)
        continue;
      string func_name = codegen->GetFuncName(associate_func_decl);
      localCall.insert(func_name);

      if (localFuncNames.count(func_name) == 0 &&
          globalFuncNames.count(func_name) > 0) {
        cout << "[mars_opt] ERROR: Function " << func_name
             << " is called without local declaration." << endl;
        exit(1);
      }
    }
  }
}

string get_addition_annotation_string(
    const map<string, pair<vector<string>, vector<string>>> &mapParams) {
  string sAdditional;
  size_t j;
  {
    map<string, pair<vector<string>, vector<string>>>::iterator it;
    for (it = mapParams.begin(); it != mapParams.end(); it++) {
      if (it->first != "name") {
        sAdditional += " " + it->first + "=\"";
        for (j = 0; j < it->second.first.size(); j++) {
          if (j != 0)
            sAdditional += ",";
          sAdditional += it->second.first[j];
        }
        for (j = 0; j < it->second.second.size(); j++) {
          if (j != 0)
            sAdditional += ";";
          else
            sAdditional += ":";
          sAdditional += it->second.second[j];
        }
        sAdditional += "\" ";
      }
    }
  }
  return sAdditional;
}

int tldm_pragma_check(CSageCodeGen *codegen, void *pTopFunc) { return 1; }

void *get_surrounding_graph_from_task(void *task_bb, CSageCodeGen *codegen,
                                      string sTaskName,
                                      const string &graph_name,
                                      string *p_additional) {
  void *graph_bb = task_bb;
  string sFilter, sCmd, sPragma;

  while (graph_bb) {
    graph_bb =
        codegen->TraceUpByType(codegen->GetParent(graph_bb), "SgBasicBlock");

    if (!graph_bb) {
      printf("\n[mars_opt] ERROR: task %s is not in any cmost graph range.\n\n",
             sTaskName.c_str());
      exit(1);
    }

    void *sg_parent = codegen->GetParent(graph_bb);
    if (!codegen->IsBasicBlock(sg_parent))
      continue;
    if (!graph_bb)
      break;
    size_t child_index = codegen->GetChildStmtIdx(sg_parent, graph_bb);
    if (0 == child_index || child_index >= codegen->GetChildStmtNum(sg_parent))
      continue;
    //  void * pre_stmt = codegen->GetPreviousStmt(graph_bb); //  has bug in
    //  ROSE
    //  !!!
    void *pre_stmt = codegen->GetChildStmt(sg_parent, child_index - 1);

    string sPragma = codegen->GetPragmaString(pre_stmt);
    if ("" == sPragma)
      continue;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(sPragma, sFilter, sCmd, mapParams);
    //  if (!is_cmost_pragma(sFilter)) return 0;
    if (is_cmost_pragma(sFilter) && ("graph_block" == sCmd)) {
      graph_name = mapParams["name"].first[0];

      if (graph_bb)
        *p_additional = get_addition_annotation_string(mapParams);
      break;
    }
  }

  if (graph_name == "undefined") {
    printf("\n[mars_opt] ERROR: task %s is not in any cmost graph range.\n\n",
           sTaskName.c_str());
    exit(1);
  }

  return graph_bb;
}

int get_tasks_from_pragmas(CSageCodeGen *codegen, void *pTopFunc,
                           CInputOptions options,
                           map<string, void *> *p_ret_tasks) {
  size_t i;

  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    //  if (!is_cmost_pragma(sFilter)) return 0;
    //  if (("tldm" != sFilter && "cmost" != sFilter) || ("task_block" != sCmd))
    //  continue;
    if (!is_cmost_pragma(sFilter) || ("task_block" != sCmd))
      continue;

    string sTaskName;
    if (mapParams["name"].first.size() > 0) {
      sTaskName = mapParams["name"].first[0];
    } else {
      static int task_id = 0;
      sTaskName = "unnamed_task" + my_itoa(task_id++);
    }

    void *sg_pragma = vecTldmPragmas[i].first;
    void *task_bb = codegen->GetNextStmt(sg_pragma);
    assert(codegen->IsBasicBlock(task_bb));

    *p_ret_tasks[sTaskName] = task_bb;
  }

  return *p_ret_tasks.size();
}

int linearize_array_top(CSageCodeGen *codegen, void *pTopFunc) {
  size_t i;
  map<void *, void *> mapTask2Graph;

  //  replace graph_bb with tldm pragma and calls
  map<void *, int> mapGraphProcessed;

  //  1. get task_bb and graph_bb
  vector<pair<void *, string>> vecTldmPragmas;
  map<void *, string> mapTaskName;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);
    if (!is_cmost_pragma(sFilter) || ("task_block" != sCmd))
      continue;
    string sTaskName;
    if (mapParams["name"].first.size() > 0) {
      sTaskName = mapParams["name"].first[0];
    } else {
      static int task_id = 0;
      sTaskName = "unnamed_task" + my_itoa(task_id++);
    }

    void *sg_pragma = vecTldmPragmas[i].first;
    void *sg_parent = codegen->GetParent(sg_pragma);
    void *task_bb = codegen->GetChildStmt(
        sg_parent, codegen->GetChildStmtIdx(sg_parent, sg_pragma) + 1);
    void *graph_bb = trace_up_for_graph_block(codegen, sg_pragma);
    mapTask2Graph[task_bb] = graph_bb;
    mapTaskName[task_bb] = sTaskName;

    //  1.5 linearize array access
    if (mapGraphProcessed.find(graph_bb) == mapGraphProcessed.end()) {
      int ret;
      ret = codegen->linearize_arrays(graph_bb);
      if (!ret) {
        cerr << "faile to linearize arrays!";
      }

      mapGraphProcessed[graph_bb] = 1;
    }
  }
  return 0;
}

string tldm_gen_top(CSageCodeGen *codegen, void *pTopFunc) {
  size_t i, j;
  vector<void *> vecFuncs;
  void *sg_project = codegen->GetProject();
  codegen->GetNodesByType(sg_project, "preorder", "SgFunctionDeclaration",
                          &vecFuncs);

  string str = "<> :: (top__)\n";
  string prefix = "\t";
  str += "{\n";

  int num_tasks = 0;
  for (i = 0; i < vecFuncs.size(); i++) {
    string sFuncName = codegen->GetFuncName(vecFuncs[i]);
    if (sFuncName.find("_kernel") != string::npos &&
        sFuncName.find("_kernel") + strlen("_kernel") == sFuncName.size()) {
      //  str += sFuncName + " :\n";
      int len = sFuncName.size() - strlen("_kernel");
      string sTaskName = sFuncName.substr(0, len);

      vector<string> vecTldmPragmas;
      codegen->TraverseSimple(vecFuncs[i], "preorder", GetTLDMInfo,
                              &vecTldmPragmas);
      for (j = 0; j < vecTldmPragmas.size(); j++) {
        string sTLDMStmt =
            translate_pragma(sTaskName, vecTldmPragmas[j], prefix);
        if (sTLDMStmt != "") {
          str += prefix + sTLDMStmt + "\n";
          if (sTLDMStmt.find("_set>") != string::npos)
            num_tasks++;
        }
      }
    }

    if (sFuncName.find("_kernel_detected") != string::npos &&
        sFuncName.find("_kernel_detected") + strlen("_kernel_detected") ==
            sFuncName.size()) {
      int len = sFuncName.size() - strlen("_kernel_detected");
      string sTaskName = sFuncName.substr(0, len);

      vector<string> vecTldmPragmas;
      codegen->TraverseSimple(vecFuncs[i], "preorder", GetTLDMInfo,
                              &vecTldmPragmas);
      for (j = 0; j < vecTldmPragmas.size(); j++) {
        string sTLDMStmt =
            translate_pragma(sTaskName, vecTldmPragmas[j], prefix);
        if (sTLDMStmt != "") {
          str += prefix + sTLDMStmt + "\n";
          if (sTLDMStmt.find("_set>") != string::npos)
            num_tasks++;
        }
      }
    }
  }
  str += "};\n\n";

  if (num_tasks == 0) {
    string msg = "Finding no task pragma. Quit the optimizations.\n";
    msg += "    Hint: please add '#pragma ACCEL task name=\"...\"' ";
    msg += "right before the kernel function call";
    dump_critical_message("TLDMG", "ERROR", msg, 301, 1);

    printf("\n\n[mars_opt] INFO: no accelerator is specified, the flow is "
           "stopped.");
    exit(0);
  }

  return str;
}

//  This function parse the xml info and get the ref_rename_list
//  input : options: -a xml_file
//  output: mapRefRenameList;  //  array_name -> <ref list in source preorder>
int GetRefNameFromXML(void *pTopFunc, CInputOptions options,
                      map<string, vector<string>> *p_mapRefRenameList) {
  size_t i;
  //  Port separation
  //  1. find the bus_e2 to be separated, and its reference order
  //  map<string, vector<string> > & mapRefRenameList;
  {
    if (options.get_option_num("-a") == 0)
      return 0;
    string sXMLfile = options.get_option("-a");
    if (sXMLfile == "")
      return 0;
    if (!test_file_for_read(sXMLfile)) {
      printf("[mars_opt] ERROR: Can not open file %s for reading in "
             "GetRefNameFromXML().\n",
             sXMLfile.c_str());
      assert(0);
      exit(0);
    }

    string sRefOrderVars =
        get_xml_cfg_simple(sXMLfile, "param:name:ref_rename_ports");

    vector<string> vecVars;
    str_split(sRefOrderVars, ',', vecVars);
    for (i = 0; i < vecVars.size(); i++) {
      string sRefOrder = get_xml_cfg_simple(
          sXMLfile, "param:name:ref_rename_list_" + vecVars[i]);
      vector<string> vecRef;
      str_split(sRefOrder, ',', vecRef);
      *p_mapRefRenameList.insert(
          pair<string, vector<string>>(vecVars[i], vecRef));
    }
  }
}

void pipeline_insert(CSageCodeGen *codegen, void *pTopFunc,
                     CInputOptions options) {
  if ("" == options.get_option("-x")) {
    printf("[mars_opt] ERROR: -x flag is required in pass pipeline_insert\n");
    assert(0);
  }
  CXMLParser parser;
  CXMLNode *pTLDM_in = parser.parse_from_file(options.get_option("-x"));
  mark_tldm_annotation(pTLDM_in);
  dump_task_pragma("poly_info_pipeline_insert.rpt");

  vector<string> vec_bus_e1;
  {
    tldm_polyhedral_info access_poly_info;
    vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
    for (size_t k = 0; k < taskset.size(); k++) {
      for (int l = 0; l < taskset[k]->GetPortInfoNum(); l++) {
        access_poly_info = taskset[k]->GetPortInfo(l).poly_info;

        string sPortName = access_poly_info.properties["func_arg_name"];
        string sPortType = access_poly_info.properties["port_type"];

        if (USE_LOWERCASE_NAME) {
          sPortName = get_lower_case(sPortName);
        }

        if (sPortType == "bus_e1")
          vec_bus_e1.push_back(sPortName);
      }
    }
  }

  codegen->InsertPipelinePragma(pTopFunc, vec_bus_e1);

  mark_tldm_annotation_release_resource();
}

void access_wrapper_top(CSageCodeGen *codegen, void *pTopFunc,
                        CInputOptions options) {
  if ("" == options.get_option("-x")) {
    printf("[mars_opt] ERROR: -x flag is required in pass wrapper_gen\n");
    assert(0);
  }

  CXMLParser parser;
  CXMLNode *pTLDM_in = parser.parse_from_file(options.get_option("-x"));
  mark_tldm_annotation(pTLDM_in);
  dump_task_pragma("poly_info_wrapper_gen.rpt");

  map<string, vector<string>> mapArray2Dim;
  {
    tldm_polyhedral_info access_poly_info;
    vector<CTldmTaskAnn *> taskset = get_tldm_task_set();
    for (size_t k = 0; k < taskset.size(); k++) {
      for (int l = 0; l < taskset[k]->GetPortInfoNum(); l++) {
        access_poly_info = taskset[k]->GetPortInfo(l).poly_info;

        string sPortName = access_poly_info.properties["func_arg_name"];

        if (USE_LOWERCASE_NAME) {
          sPortName = get_lower_case(sPortName);
        }

        mapArray2Dim[sPortName] =
            str_split(access_poly_info.properties["array_dim"], ",");
      }
    }
  }

  assert(pTopFunc);
  map<string, vector<string>>
      mapRefRenameList;  //  map<string port, vector<string ref_name_list> >
  GetRefNameFromXML(pTopFunc, options, &mapRefRenameList);

  //  fixed by zhangpeng to avoid filename conflicts between tasks
  //  codegen->access_wrapper(pTopFunc, mapRefRenameList, "cmost");
  codegen->access_wrapper(pTopFunc, mapRefRenameList,
                          "__CMOST_TASK_NAME___cmost", mapArray2Dim);

  for (int i = 0; i < codegen->GetGlobalNum(); i++) {
    void *sg_scope_global = codegen->GetGlobal(i);
    codegen->AddHeader("\n#include \"cmost_access.h\"", sg_scope_global);
  }

  mark_tldm_annotation_release_resource();
}

void *one_region_to_basicblock_tiling(
    CSageCodeGen *codegen,
    map<string, pair<vector<string>, vector<string>>> mapParams,
    void *pTaskBegin, string sComponent) {
  int i, i0 = -1, i1 = -1;
  //  1. find the pTaskEnd
  void *pParent = codegen->GetParent(pTaskBegin);
  int found = 0;
  for (i = 0; i < codegen->GetChildStmtNum(pParent); i++) {
    void *sg_stmt = codegen->GetChildStmt(pParent, i);
    if (!found && sg_stmt != pTaskBegin) {
      continue;
    }
    if (!found && sg_stmt == pTaskBegin) {
      i0 = i;
    }
    found = 1;

    string str = codegen->GetPragmaString(sg_stmt);
    if ("" == str)
      continue;
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams2;
    tldm_pragma_parse_whole(str, sFilter, sCmd, mapParams2);
    if (sComponent + "_end" == sCmd) {
      i1 = i;
      break;
    }
  }

  if (i == codegen->GetChildStmtNum(pParent)) {
    printf("[mars_opt] ERROR: %s_end is not found in the bb of %s_begin %s\n",
           sComponent.c_str(), sComponent.c_str(),
           mapParams["name"].first[0].c_str());
    return NULL;
  }

  //  2. create and replace the BB
  void *newBB = codegen->CreateBasicBlock();
  vector<void *> vecChildsToReplace;
  for (i = i0 + 1; i < i1 + 1; i++) {
    void *sg_stmt = codegen->GetChildStmt(pParent, i);
    if (i != i1)
      codegen->AppendChild(newBB, codegen->CopyStmt(sg_stmt));
    vecChildsToReplace.push_back(sg_stmt);
  }

  //  3. replace the old code
  void *sg_stmt = codegen->GetChildStmt(pParent, i0);
  codegen->ReplaceChild(sg_stmt, newBB);
  for (size_t i = 0; i < vecChildsToReplace.size(); i++) {
    codegen->RemoveChild(vecChildsToReplace[i]);
  }

  //  4. add new pragma
  string pragma_str = "cmost " + sComponent + "_begin";
  for (map<string, pair<vector<string>, vector<string>>>::iterator iter =
           mapParams.begin();
       iter != mapParams.end(); iter++) {
    if (iter->first != "loop_tiling" && iter->first != "tile_size")
      pragma_str += " " + iter->first + "=\"" + iter->second.first[0] + "\"";
  }
  void *sg_new_pragma = codegen->CreatePragma(pragma_str, pParent);
  codegen->InsertStmt(sg_new_pragma, newBB);
  sg_new_pragma =
      codegen->CreatePragma("cmost " + sComponent + "_end\n", pParent);
  SgStatement *sg_stm = isSgStatement(static_cast<SgNode *>(sg_new_pragma));
  SgStatement *sg_after = isSgStatement(static_cast<SgNode *>(newBB));
  //  SageInterface::insertStatement(sg_after, sg_stm, false); //  false: insert
  //  after
  codegen->InsertAfterStmt(sg_stm, sg_after);
  return newBB;
}

int loop_tiling_code_gen(CSageCodeGen *codegen, void *pTopFunc,
                         CTldmTaskAnn *tiled_task, string orig_iterator_str,
                         string orig_iterator_ranges, string tile_sizes_str) {
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;

  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  tldm_variable_range range;
  vector<tldm_variable_range> ranges;
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (is_cmost_pragma(sFilter) && ("parameter" == sCmd)) {
      range.variable = mapParams["variable"].first[0];
      range.lb = str_split(mapParams["range"].first[0], "..")[0];
      range.ub = str_split(mapParams["range"].first[0], "..")[1];
      ranges.push_back(range);
    }
  }
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (is_cmost_pragma(sFilter) && ("task_begin" == sCmd)) {
      string loop_tiling_task_name = mapParams["name"].first[0];
      if (tiled_task->poly_info.name == loop_tiling_task_name) {
        if (DEBUG) {
          cout << "[INFO] Tiled task " << loop_tiling_task_name << " found!"
               << endl;
        }
        if (mapParams.size() <= 0 || mapParams.count("loop_tiling") <= 0) {
          cout << "[ERROR] Loop_tiling pragma not found!" << endl;
          exit(0);
        }
        void *sg_pragma = vecTldmPragmas[i].first;
        void *sg_parent = codegen->GetParent(sg_pragma);
        void *task_bb = codegen->GetChildStmt(
            sg_parent, codegen->GetChildStmtIdx(sg_parent, sg_pragma) + 1);
        void *graph_bb = trace_up_for_graph_block(codegen, sg_pragma);

        if (!graph_bb) {
          cout << "[ERROR] Graph pragma not found!" << endl;
          exit(0);
        }
        int tiling_depth = str_split(tile_sizes_str, ",").size();
        vector<string> tile_sizes = str_split(tile_sizes_str, ",");
        vector<string> tile_loop_iter_names;
        vector<string> point_loop_iter_names;
        vector<void *> tile_loop_iter_vars;
        vector<void *> point_loop_iter_vars;
        vector<void *> orig_loop_iter_vars;
        vector<void *> orig_loop_lb_exps;
        vector<void *> orig_loop_ub_exps;
        vector<int> tile_sizes_int;
        vector<string> tiled_iterators =
            str_split(tiled_task->poly_info.iterator_vector, ",");
        vector<string> orig_iterator_substr = str_split(orig_iterator_str, ",");
        vector<void *> tile_loop_iter_lbs;
        vector<void *> tile_loop_iter_ubs;
        void *sg_loop = task_bb;
        //  Filling loop information
        for (int j = 0; j < tiling_depth; j++) {
          istringstream convert(tile_sizes[j]);
          int tile_size;
          assert(convert >> tile_size);
          if (DEBUG) {
            cout << " tile_size = " << tile_size << endl;
          }
          tile_sizes_int.push_back(tile_size);
        }
        for (int j = 0; j < tiling_depth; j++) {
          string tile_loop_iter_name = tiled_iterators[j];
          string point_loop_iter_name = tiled_iterators[j + tiling_depth];
          tile_loop_iter_names.push_back(tile_loop_iter_name);
          point_loop_iter_names.push_back(point_loop_iter_name);
          void *tile_var_decl = SageBuilder::buildVariableDeclaration(
              tile_loop_iter_name, SageBuilder::buildIntType());
          void *point_var_decl = SageBuilder::buildVariableDeclaration(
              point_loop_iter_name, SageBuilder::buildIntType());
          tile_loop_iter_vars.push_back(tile_var_decl);
          point_loop_iter_vars.push_back(point_var_decl);
          while (!codegen->IsForStatement(sg_loop))
            sg_loop = codegen->GetParent(sg_loop);
          void *ivar = NULL;
          void *lb = NULL, *ub = NULL, *step = NULL;
          void *loop_body = NULL;
          void *cond_op;
          int ub_limit = 0;
          codegen->ParseOneForLoop(sg_loop, ivar, lb, ub, step, cond_op,
                                   loop_body, ub_limit);
          orig_loop_iter_vars.insert(orig_loop_iter_vars.begin(), ivar);
          orig_loop_lb_exps.insert(orig_loop_lb_exps.begin(), lb);
          orig_loop_ub_exps.insert(orig_loop_ub_exps.begin(), ub);
          void *lb_var_decl = codegen->CreateVariableDecl(
              "int",
              "tldm_task_" + loop_tiling_task_name + "_tile_loop_iter_lb_" +
                  codegen->UnparseToString(ivar),
              codegen->CreateExp(
                  V_SgDivideOp, lb,
                  codegen->CreateConst(tile_sizes_int[tiling_depth - j - 1])),
              graph_bb);
          void *ub_var_decl = codegen->CreateVariableDecl(
              "int",
              "tldm_task_" + loop_tiling_task_name + "_tile_loop_iter_ub_" +
                  codegen->UnparseToString(ivar),
              codegen->CreateExp(
                  V_SgDivideOp, ub,
                  codegen->CreateConst(tile_sizes_int[tiling_depth - j - 1])),
              graph_bb);
          tile_loop_iter_lbs.insert(tile_loop_iter_lbs.begin(), lb_var_decl);
          tile_loop_iter_ubs.insert(tile_loop_iter_ubs.begin(), ub_var_decl);
          if (j < tiling_depth - 1)
            sg_loop = codegen->GetParent(sg_loop);
        }
        //  Insert lb and ub declarations
        for (int j = tiling_depth - 1; j >= 0; j--) {
          void *sg_parent = codegen->GetParent(graph_bb);
          void *ins_pos = codegen->GetChildStmt(sg_parent, 0);
          codegen->InsertStmt(tile_loop_iter_ubs[j],
                              codegen->GetPreviousStmt(ins_pos));
        }
        for (int j = tiling_depth - 1; j >= 0; j--) {
          void *sg_parent = codegen->GetParent(graph_bb);
          void *ins_pos = codegen->GetChildStmt(sg_parent, 0);
          codegen->InsertStmt(tile_loop_iter_lbs[j],
                              codegen->GetPreviousStmt(ins_pos));
        }
        //  Insert tile loop iteration decls
        for (int j = tiling_depth - 1; j >= 0; j--) {
          codegen->InsertStmt(tile_loop_iter_vars[j],
                              codegen->GetChildStmt(graph_bb, 0));
        }
        //  Insert point loop iteration decls
        for (int j = 0; j < tiling_depth; j++) {
          codegen->InsertStmt(point_loop_iter_vars[j], task_bb);
        }
        //  Insert assign statements
        for (int j = tiling_depth - 1; j >= 0; j--) {
          void *new_exp = codegen->CreateExp(
              V_SgMultiplyOp, codegen->CreateConst(tile_sizes_int[j]),
              SageBuilder::buildInitializedName(tile_loop_iter_names[j],
                                                SageBuilder::buildIntType()));
          new_exp = codegen->CreateExp(
              V_SgAddOp, new_exp,
              SageBuilder::buildInitializedName(point_loop_iter_names[j],
                                                SageBuilder::buildIntType()));
          new_exp =
              codegen->CreateExp(V_SgAddOp, new_exp, orig_loop_lb_exps[j]);
          void *var_decl = codegen->CreateVariableDecl(
              "int", orig_iterator_substr[j], new_exp, task_bb);
          codegen->InsertStmt(var_decl, codegen->GetChildStmt(task_bb, 0));
        }
        void *old_level;
        void *new_level;
        //  Insert lb and ub pragmas
        vector<int> loop_tripcount_min;
        vector<int> loop_tripcount_max;
        for (int j = tiling_depth - 1; j >= 0; j--) {
          int lb, ub;
          string lb_string =
              str_split(str_split(orig_iterator_ranges, ",")[j], "..")[0];
          cout << lb_string << endl;
          tldm_polynomial access(lb_string, ranges);
          int ret = access.get_bound_const(lb, ub);
          assert(ret);
          lb /= tile_sizes_int[j];
          ub /= tile_sizes_int[j];
          loop_tripcount_min.insert(loop_tripcount_min.begin(), ub);
          loop_tripcount_max.insert(loop_tripcount_max.begin(), lb);
          stringstream pragma_stream;
          pragma_stream << "tldm parameter variable =  \"tldm_task_"
                        << loop_tiling_task_name << "_tile_loop_iter_lb_"
                        << orig_iterator_substr[j] + "\" range = \"" << lb
                        << ".." << ub << "\"";
          void *sg_new_pragma =
              codegen->CreatePragma(pragma_stream.str(), pTopFunc);
          cout << codegen->UnparseToString(graph_bb) << endl;
          void *sg_parent = codegen->GetParent(graph_bb);
          void *ins_pos = codegen->GetChildStmt(sg_parent, 0);
          //  codegen->GetChildStmtIdx(sg_parent, graph_bb) - 1);
          //  ins_pos = graph_bb;

          //  cout << codegen->UnparseToString(ins_pos) << endl;
          codegen->InsertStmt(sg_new_pragma, codegen->GetPreviousStmt(ins_pos));
        }
        for (int j = tiling_depth - 1; j >= 0; j--) {
          int lb, ub;
          string ub_string =
              str_split(str_split(orig_iterator_ranges, ",")[j], "..")[1];
          cout << ub_string << endl;
          tldm_polynomial access(ub_string, ranges);
          int ret = access.get_bound_const(lb, ub);
          assert(ret);
          lb /= tile_sizes_int[j];
          ub /= tile_sizes_int[j];
          loop_tripcount_min[j] = lb - loop_tripcount_min[j] + 1;
          loop_tripcount_max[j] = ub - loop_tripcount_max[j] + 1;
          if (loop_tripcount_min[j] <= 0)
            loop_tripcount_min[j] = 1;
          stringstream pragma_stream;
          pragma_stream << "tldm parameter variable =  \"tldm_task_"
                        << loop_tiling_task_name << "_tile_loop_iter_ub_"
                        << orig_iterator_substr[j] + "\" range = \"" << lb
                        << ".." << ub << "\"";
          void *sg_new_pragma =
              codegen->CreatePragma(pragma_stream.str(), pTopFunc);
          cout << codegen->UnparseToString(graph_bb) << endl;
          void *sg_parent = codegen->GetParent(graph_bb);
          void *ins_pos = codegen->GetChildStmt(sg_parent, 0);
          codegen->InsertStmt(sg_new_pragma, codegen->GetPreviousStmt(ins_pos));
        }
        //  Insert point loops
        old_level = codegen->CopyStmt(task_bb);
        for (int j = tiling_depth - 1; j >= 0; j--) {
          new_level = codegen->CreateStmt(
              V_SgForStatement,
              SageBuilder::buildInitializedName(point_loop_iter_names[j],
                                                SageBuilder::buildIntType()),
              codegen->CreateConst(0), codegen->CreateConst(tile_sizes_int[j]),
              old_level);
          old_level = new_level;
        }
        codegen->InsertStmt(new_level, task_bb);
        codegen->RemoveChild(task_bb);
        //  Insert tile loops
        old_level = codegen->GetParent(old_level);
        for (int j = tiling_depth - 1; j >= 0; j--) {
          new_level = codegen->CreateStmt(
              V_SgForStatement,
              SageBuilder::buildInitializedName(tile_loop_iter_names[j],
                                                SageBuilder::buildIntType()),
              codegen->CreateVariableRef(tile_loop_iter_lbs[j]),
              codegen->CreateVariableRef(tile_loop_iter_ubs[j]), old_level);
          old_level = new_level;
        }
        codegen->InsertStmt(new_level, sg_loop);
        for (int j = 0; j < tiling_depth; j++) {
          stringstream pragma_stream;
          pragma_stream << "HLS loop_tripcount min=" << loop_tripcount_min[j]
                        << " avg="
                        << (loop_tripcount_min[j] + loop_tripcount_max[j]) / 2
                        << " max=" << loop_tripcount_max[j];
          void *sg_new_pragma =
              codegen->CreatePragma(pragma_stream.str(), pTopFunc);
          codegen->InsertStmt(sg_new_pragma,
                              codegen->GetChildStmt((old_level), 0));
          old_level = codegen->GetChildStmt((old_level), 1);
        }
        codegen->RemoveChild(sg_loop);
      }
    }
  }
  return 0;
}

bool check_tiling_legality(tldm_polyhedral_info poly_info,
                           vector<string> tiling_size_info) {
  //  FIXME: TBD
  //  cout << "[WARNING:] Legality check for loop tiling is not implemented
  //  yet!"
  //  << endl;

  vector<CTldmDependenceAnn *> vectorDepAnn = get_tldm_dependence_set();
  if (DEBUG) {
    cout << "[INFO] Number of dependences is " << vectorDepAnn.size() << endl;
  }
  if (!vectorDepAnn.size()) {
    cout << "[INFO] No dependence in the tiled loop " << endl;
    return true;
  }
  for (size_t i = 0; i < vectorDepAnn.size(); i++) {
    CTldmDependenceAnn *depAnn = vectorDepAnn[i];
    CXMLAnn_WrapperGen *pTask0 = depAnn->GetTask0();
    CXMLAnn_WrapperGen *pTask1 = depAnn->GetTask1();
    assert(pTask0);
    assert(pTask1);
    if ((pTask0->poly_info.name == poly_info.name) &&
        (pTask1->poly_info.name == poly_info.name)) {
      string distance = depAnn->GetDistance();
      //  cout << distance << endl;
      vector<string> sub_dis = str_split(distance, ',');

      int firstNoneZeroVal = -1;
      for (size_t j = 0; j < sub_dis.size(); j++) {
        if (my_atoi(sub_dis[j]) != 0) {
          firstNoneZeroVal = my_atoi(sub_dis[j]);
          break;
        }
      }
      //  cout << firstNoneZeroVal << endl;
      if (firstNoneZeroVal < 0)
        return false;
    }
  }
  return true;
}

string replace_var_string(string orig_string, string orig_var, string new_var,
                          size_t pos) {
  if (pos >= orig_string.length())
    return orig_string;
  size_t found_pos = orig_string.find(orig_var, pos);
  if (found_pos == string::npos)
    return orig_string;
  if (found_pos > 0 && (isalpha(orig_string[found_pos - 1]) ||
                        orig_string[found_pos - 1] == '_'))
    return replace_var_string(orig_string, orig_var, new_var, found_pos + 1);
  if (found_pos < orig_string.length() - 1 &&
      (isalpha(orig_string[found_pos + 1]) ||
       orig_string[found_pos + 1] == '_'))
    return replace_var_string(orig_string, orig_var, new_var, found_pos + 1);
  string new_string =
      orig_string.replace(found_pos, orig_var.length(), new_var);
  return replace_var_string(new_string, orig_var, new_var, found_pos + 1);
}

void insert_tripcount_pragma_by_statement(void *sg_loop, CSageCodeGen *codegen,
                                          vector<tldm_variable_range> ranges,
                                          void *pTopFunc) {
  if (!sg_loop)
    return;
  void *ivar = NULL;
  void *lb = NULL, *ub = NULL, *step = NULL;
  void *loop_body = NULL;
  void *cond_op;
  int ub_limit = 0;
  if (isSgForStatement(static_cast<SgNode *>(sg_loop)) {)
    codegen->ParseOneForLoop(sg_loop, ivar, lb, ub, step, cond_op, loop_body,
                            ub_limit);
    if (DEBUG) {
      cout << "Parsing loop "
           << (static_cast<SgNode *>(sg_loop)->unparseToString() << endl);
    }
    cout << codegen->UnparseToString(lb) << endl;
    cout << codegen->UnparseToString(ub) << endl;
    int lb_val, ub_val;
    int lb_val2, ub_val2;
    int ret, ret2;
    tldm_polynomial access(codegen->UnparseToString(lb), ranges);
    ret = access.get_bound_const(lb_val, ub_val);
    tldm_polynomial access2(codegen->UnparseToString(ub), ranges);
    ret2 = access2.get_bound_const(lb_val2, ub_val2);
    cout << ub_val2 - lb_val << endl;
    stringstream pragma_stream;
    pragma_stream << "HLS loop_tripcount min=" << ub_val2 - lb_val
                  << " avg=" << ub_val2 - lb_val << " max=" << ub_val2 - lb_val;
    void *sg_new_pragma = codegen->CreatePragma(pragma_stream.str(), pTopFunc);
    if (ret && ret2)
      codegen->InsertStmt(sg_new_pragma, codegen->GetChildStmt((sg_loop), 0));
  }
  if (isSgScopeStatement(static_cast<SgNode *>(sg_loop)
      && !isSgIfStmt(static_cast<SgNode *>sg_loop))) {
    for (int i = 0; i < codegen->GetChildStmtNum(sg_loop); i++) {
      insert_tripcount_pragma_by_statement(codegen->GetChildStmt(sg_loop, i),
                                           codegen, ranges, pTopFunc);
    }
  }
}

void create_task_BB(CXMLNode *pTLDMRoot, CSageCodeGen *codegen,
                    void *pTopFunc) {
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  tldm_variable_range range;
  vector<tldm_variable_range> ranges;
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    //  if (!is_cmost_pragma(sFilter)) return 0;
    //  if(("tldm" == sFilter || "cmost" == sFilter) && ("parameter" == sCmd))
    if (is_cmost_pragma(sFilter) && ("parameter" == sCmd)) {
      range.variable = mapParams["variable"].first[0];
      range.lb = str_split(mapParams["range"].first[0], "..")[0];
      range.ub = str_split(mapParams["range"].first[0], "..")[1];
      ranges.push_back(range);
    }
  }
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (is_cmost_pragma(sFilter) && ("task_begin" == sCmd)) {
      void *sg_pragma = vecTldmPragmas[i].first;
      one_region_to_basicblock_tiling(codegen, mapParams, sg_pragma, "task");
    }
  }
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (is_cmost_pragma(sFilter) && ("graph_begin" == sCmd)) {
      void *sg_pragma = vecTldmPragmas[i].first;
      one_region_to_basicblock_tiling(codegen, mapParams, sg_pragma, "graph");
    }
  }
}

void insert_tripcount_pragma(CXMLNode *pTLDMRoot, CSageCodeGen *codegen,
                             void *pTopFunc) {
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer,
                          &vecTldmPragmas);
  tldm_variable_range range;
  vector<tldm_variable_range> ranges;
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (is_cmost_pragma(sFilter) && ("parameter" == sCmd)) {
      range.variable = mapParams["variable"].first[0];
      range.lb = str_split(mapParams["range"].first[0], "..")[0];
      range.ub = str_split(mapParams["range"].first[0], "..")[1];
      ranges.push_back(range);
    }
  }
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);

    if (is_cmost_pragma(sFilter) && ("task_begin" == sCmd)) {
      void *sg_pragma = vecTldmPragmas[i].first;
      //  cout << (static_cast<SgNode *>(sg_pragma)->unparseToString() << endl);
      void *sg_parent = codegen->GetParent(sg_pragma);
      void *task_bb = codegen->GetChildStmt(
          sg_parent, codegen->GetChildStmtIdx(sg_parent, sg_pragma) + 1);
      //  cout << "task_bb = " << (static_cast<SgNode
      //  *>(task_bb)->unparseToString() << endl);

      //  task_bb = one_region_to_basicblock_tiling(codegen,
      //  mapParams["name"].first[0], sg_pragma, "task");

      //  cout << "task_bb = " << (static_cast<SgNode
      //  *>(task_bb)->unparseToString() << endl);

      void *graph_bb = trace_up_for_graph_block(codegen, sg_pragma);
      if (!graph_bb) {
        cout << "[ERROR] Graph pragma not found!" << endl;
        exit(0);
      }
      insert_tripcount_pragma_by_statement(task_bb, codegen, ranges, pTopFunc);
    }
  }
}

int update_tldm_annotation(CXMLNode *pTLDMRoot, CSageCodeGen *codegen,
                           void *pTopFunc) {
  size_t task_iter;

  string str_info;
  vector<CTldmTaskAnn *> vecTasks = get_tldm_task_set();
  for (task_iter = 0; task_iter < vecTasks.size(); task_iter++) {
    tldm_polyhedral_info poly_info = vecTasks[task_iter]->poly_info;
    vector<string> tiling_size_info =
        str_split(poly_info.properties["loop_tiling"], ',');
    if (tiling_size_info.empty())
      tiling_size_info = str_split(poly_info.properties["tile_size"], ',');
    if (DEBUG) {
      cout << "[INFO] Checking tiling parameter for " << poly_info.name << endl;
      //  cout << "Tiling pragma is " << poly_info.properties["loop_tiling"] <<
      //  endl; cout << "Tiling size_info.size() = " << tiling_size_info.size()
      //  << endl;; cout << tiling_size_info << endl;
    }
    if (!tiling_size_info.empty()) {
      CXMLNode *pTopModule = pTLDMRoot->GetChildByIndex(0);
      CXMLNode *pTaskDef =
          SearchNodeNameUnique(pTopModule, "task_def", poly_info.name);
      assert(pTaskDef);
      if (DEBUG) {
        cout << "Found tiling parameters" << tiling_size_info << endl;
      }
      if (str_split(poly_info.iterator_vector, ",").size() !=
          tiling_size_info.size()) {
        //  cout << poly_info.iterator_vector << endl;
        //  cout << poly_info.properties["loop_tiling"] << endl;

        cout << "[ERROR] Tiling size numbers are not equal to iterator numbers"
             << endl;
        assert(str_split(poly_info.iterator_vector, ",").size() ==
               tiling_size_info.size());
      }
      if (!check_tiling_legality(
              poly_info,
              tiling_size_info)) {  //  FIXME: what are the parameters
        cout << "[ERROR] Tiling pragmas found, but not legal" << endl;
        continue;
      }
      string orig_iterator_str = poly_info.iterator_vector;
      string orig_iterator_ranges = poly_info.iterator_range;
      //  Tiling step 1: add iterators
      //  string iterator_vector;
      //  "m,n,p" ->
      //    "__tldm_tile_loop_iter_m, __tldm_tile_loop_iter_n,
      //  __tldm_tile_loop_iter_p,
      //    __tldm_point_loop_iter_m, __tldm_point_loop_iter_n,
      //  __tldm_point_loop_iter_p"
      if (DEBUG) {
        cout << "[INFO] Orig iterator vector " << poly_info.iterator_vector
             << endl;
      }

      string tile_loop_iterator_str, point_loop_iterator_str;
      vector<string> iterator_info = str_split(poly_info.iterator_vector, ",");
      for (vector<string>::iterator iter = iterator_info.begin();
           iter != iterator_info.end(); ++iter) {
        //  tile_loop_iterator_str += "__tldm_tile_loop_iter_" + *iter;
        //  point_loop_iterator_str += "__tldm_point_loop_iter_" + *iter;
        tile_loop_iterator_str += "tldm_tile_loop_iter_" + *iter;
        point_loop_iterator_str += "tldm_point_loop_iter_" + *iter;
        if (iter + 1 != iterator_info.end()) {
          tile_loop_iterator_str += ",";
          point_loop_iterator_str += ",";
        }
      }
      poly_info.iterator_vector =
          tile_loop_iterator_str + "," + point_loop_iterator_str;

      tldm_xml_set_attribute(pTaskDef, "iterator_vector",
                             poly_info.iterator_vector);
      if (DEBUG) {
        cout << "[INFO] Modified iterator vector " << poly_info.iterator_vector
             << endl;
      }
      //  Tiling step 2: update the iterator ranges
      //  string iterator_range;
      //  "LB_M..UB_M,LB_N..UB_N,LB_P..UB_P" ->
      //  "LB_M/TILE_SIZE_M..UB_M/TILE_SIZE_M,
      //  LB_N/TILE_SIZE_N..UB_N/TILE_SIZE_N,
      //  LB_P/TILE_SIZE_P..UB_P/TILE_SIZE_P, 0..TILE_SIZE_M-1,
      //  0..TILE_SIZE_N-1, 0..TILE_SIZE_P-1"
      if (DEBUG) {
        cout << "[INFO] Orig iterator range " << poly_info.iterator_range
             << endl;
      }
      string orig_lb_str, tile_sizes_str;
      string tile_loop_iterator_range_str, point_loop_iterator_range_str;
      vector<string> orig_iterator_range_strs =
          str_split(poly_info.iterator_range, ",");
      int iter_iter = 0;
      for (vector<string>::iterator range_iter =
               orig_iterator_range_strs.begin();
           range_iter != orig_iterator_range_strs.end(); ++range_iter) {
        vector<string> range_bound_strs = str_split(*range_iter, "..");
        assert(range_bound_strs.size() == 2);
        string lb_str, ub_str;
        lb_str = range_bound_strs[0];
        orig_lb_str += lb_str + ",";
        tile_sizes_str += tiling_size_info[iter_iter] + ",";
        ub_str = range_bound_strs[1];
        tile_loop_iterator_range_str +=
            "((" + lb_str + ")/" + tiling_size_info[iter_iter] + ").." + "((" +
            ub_str + ")" + "/(" + tiling_size_info[iter_iter] + "))";
        point_loop_iterator_range_str +=
            "(0)..(" + tiling_size_info[iter_iter] + "-1)";
        if (range_iter + 1 != orig_iterator_range_strs.end()) {
          tile_loop_iterator_range_str += ",";
          point_loop_iterator_range_str += ",";
        }
        iter_iter++;
      }
      poly_info.iterator_range =
          tile_loop_iterator_range_str + "," + point_loop_iterator_range_str;
      tldm_xml_set_attribute(pTaskDef, "iterator_range",
                             poly_info.iterator_range);
      if (DEBUG) {
        cout << "[INFO] Modified iterator range " << poly_info.iterator_range
             << endl;
      }

      //  Tiling step 3: update the order vectors
      //  string order_vector;
      //  "0,m,0,n,0,p,0" ->
      //  "0,__tldm_tile_loop_iter_m,0,__tldm_tile_loop_iter_n,0,__tldm_tile_loop_iter_p,
      //  0,__tldm_point_loop_iter_m,0,__tldm_point_loop_iter_n,0,__tldm_point_loop_iter_p,0"
      if (DEBUG) {
        cout << "[INFO] Orig order vectors " << poly_info.order_vector << endl;
      }

      vector<string> order_vector_strs = str_split(poly_info.order_vector, ",");
      string new_order_vector_str;
      int i = 0;
      for (vector<string>::iterator iter = order_vector_strs.begin();
           iter != order_vector_strs.end(); ++iter, ++i) {
        if (i % 2) {
          new_order_vector_str += "__tldm_tile_loop_iter_" + *iter + ",";
        } else {
          new_order_vector_str += *iter + ",";
        }
      }
      for (vector<string>::iterator iter = iterator_info.begin();
           iter != iterator_info.end(); ++iter) {
        new_order_vector_str += "__tldm_point_loop_iter_" + *iter + ",0";
        if (iter + 1 != iterator_info.end()) {
          new_order_vector_str += ",";
        }
      }
      poly_info.order_vector = new_order_vector_str;
      tldm_xml_set_attribute(pTaskDef, "order_vector", poly_info.order_vector);
      tldm_xml_set_attribute(pTaskDef, "order", poly_info.order_vector);
      if (DEBUG) {
        cout << "[INFO] Modified order vectors " << poly_info.order_vector
             << endl;
      }
      //  Tiling step 4: udpate condition_vector, padding the string to "1"
      //  string condition_vector;
      //  "1,1,1,1,1,1,1" ->
      //  "1,1,1,1,1,1,1,1,1,1,1,1,1"
      if (DEBUG) {
        cout << "[INFO] Orig condition vectors " << poly_info.condition_vector
             << endl;
      }

      for (size_t i = 0; i < tiling_size_info.size(); i++)
        poly_info.condition_vector += ",1,1";

      tldm_xml_set_attribute(pTaskDef, "condition_vector",
                             poly_info.condition_vector);
      if (DEBUG) {
        cout << "[INFO] Modified condition range " << poly_info.condition_vector
             << endl;
      }
      //  Tiling step 5: update access_pattern
      //  string access_pattern;  //  "r:expression" or "w:expression"
      tldm_xml_set_attribute(pTaskDef, "access_pattern",
                             poly_info.access_pattern);
      if (DEBUG) {
        cout << "[INFO] Orig access_patterns " << poly_info.access_pattern
             << endl;
      }

      vecTasks[task_iter]->poly_info = poly_info;

      vector<CXMLNode *> vec_connects =
          pTopModule->TraverseByName("data_ref", -1);
      if (DEBUG) {
        cout << "[INFO] Connects in xml file: " << vec_connects.size() << endl;
      }

      for (size_t j = 0; j < vec_connects.size(); j++) {
        if (tldm_xml_get_attribute(vec_connects[j], "func_name") ==
                poly_info.name &&
            tldm_xml_get_attribute(vec_connects[j], "type") == "access") {
          if (DEBUG) {
            //  cout << tldm_xml_get_attribute(vec_connects[j],
            //  "access_pattern")
            //  << endl;
          }

          vector<string> orig_iterators = str_split(orig_iterator_str, ",");
          vector<string> tile_loop_iterators =
              str_split(tile_loop_iterator_str, ",");
          vector<string> point_loop_iterators =
              str_split(point_loop_iterator_str, ",");
          vector<string> tile_sizes = str_split(tile_sizes_str, ",");
          vector<string> orig_lbs = str_split(orig_lb_str, ",");
          string access_str =
              tldm_xml_get_attribute(vec_connects[j], "access_pattern");
          for (size_t k = 0; k < orig_iterators.size(); k++) {
            string tmp_str = "(" + tile_loop_iterators[k] + "*" +
                             tile_sizes[k] + "+" + point_loop_iterators[k] +
                             "+" + orig_lbs[k] + ")";
            access_str =
                replace_var_string(access_str, orig_iterators[k], tmp_str, 0);
            //  cout << access_str << endl;
          }
          tldm_xml_set_attribute(vec_connects[j], "access_pattern", access_str);
        }
      }

      loop_tiling_code_gen(codegen, pTopFunc, vecTasks[task_iter],
                           orig_iterator_str, orig_iterator_ranges,
                           tile_sizes_str);
    }
  }

  return 1;
}
#endif
