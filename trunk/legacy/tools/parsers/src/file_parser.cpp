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


#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <limits>
#include <fstream>
#include "file_parser.h"

using std::numeric_limits;

string read_string_from_file(string sFilename) {
  FILE *fp;
  if ((fp = fopen(sFilename.c_str(), "rb")) == nullptr) {
    printf("[file_parser.cpp] file %s failed to open for read.\n",
           sFilename.c_str());
    exit(0);
  }

  fseek(fp, 0, SEEK_END);
  size_t file_size;
  file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *sAll = static_cast<char *>(malloc((file_size + 1) * sizeof(char)));
  fread(sAll, file_size, sizeof(char), fp);
  sAll[file_size] = 0;
  fclose(fp);
  return string(sAll);
}

void write_string_into_file(string str, string sFilename) {
  FILE *fp;
  if ((fp = fopen(sFilename.c_str(), "wb")) == nullptr) {
    printf("[file_parser.cpp] file %s failed to open for write.\n",
           sFilename.c_str());
    exit(0);
  }
  fwrite(str.c_str(), 1, str.size(), fp);  // fixed by zhangpeng, 2012-10-28

  fclose(fp);
}

void append_string_into_file(string str, string sFilename) {
  std::ofstream outfile;
  outfile.open(sFilename.c_str(), std::ios_base::app);
  outfile << str;
  outfile.close();
}

int test_file_for_read(string sFilename) {
#if 0
    FILE * fp;
    if (!(fp=fopen(sFilename.c_str(), "rb"))) {
        return 0;
    }
    fclose(fp);
    return 1;
#else
  struct stat buffer;
  return static_cast<int>(stat(sFilename.c_str(), &buffer) == 0);
#endif
}

int isSpace(char c) {
  return static_cast<int>(' ' == c || '\t' == c || '\r' == c || '\n' == c);
}
int isSpace(string str) {
  int i;
  for (i = 0; i < static_cast<int>(str.size()); i++) {
    if (str[i] != ' ' && str[i] != '\t' && str[i] != '\r' && str[i] != '\n') {
      return 0;
    }
  }
  return 1;
}

vector<string> get_files_in_dir(string sDir) {
  size_t i;
  size_t j0;
  size_t j1;
  vector<string> sFiles;

#ifdef LINUX_COMMANDS
  string sCmdLine = "ls " + sDir + "/*_p > tmp";
#endif
#ifdef WIN_COMMANDS
  string sCmdLine = "dir /B \"" + sDir + "/\" > tmp";
#endif
  // a compiler error happens if none of the two macros is defined
  // in VC: add additional command line /D "WIN_COMMANDS" into
  //        Project->Properties->Configuratoin Properties->C/C++->Command
  //        Line->Additional Options
  // in gcc: gcc/g++ -DWIN_COMMANDS ...

  system(sCmdLine.c_str());

  string sList = read_string_from_file("tmp");

  i = 0;
  while (true) {
    // find a non-space
    while (i < sList.size() && (isSpace(sList[i]) != 0)) {
      i++;
    }
    if (i == sList.size()) {
      break;
    }
    j0 = i;

    // find a space
    while (i < sList.size() && (isSpace(sList[i]) == 0)) {
      i++;
    }
    j1 = i;

    sFiles.push_back(sList.substr(j0, j1 - j0));
  }

#ifdef WIN_COMMANDS
  vector<string> sListNew;
  for (i = 0; i < sFiles.size(); i++) {
    int size = sFiles[i].size();
    string sFile = sFiles[i];
    if (sFile[size - 2] != '_' || sFile[size - 1] != 'p') {
      // skip
    } else {
      sFile = sDir + "/" + sFile;
      sListNew.push_back(sFile);
    }
  }
  sFiles = sListNew;
#endif

  return sFiles;
}

string get_sub_delimited(string str, int start, char delimited, int size) {
  int i;
  int quotation = 0;
  for (i = start; i < static_cast<int>(str.size()) && (size < 0 || i < size);
       i++) {
    if (str[i] == '"' && delimited != '\"') {
      quotation = 1 - quotation;
    }
    if (str[i] == delimited && quotation == 0) {
      break;
    }
  }

  if (start >= static_cast<int>(str.size())) {
    return "";
  }
  return str.substr(start, i - start);
}

string get_sub_delimited_sub(string str, int start, string delimited,
                             int size) {
  int i;
  int quotation = 0;
  for (i = start; i < static_cast<int>(str.size()) && (size < 0 || i < size);
       i++) {
    if (str[i] == '"' && delimited != "\"") {
      quotation = 1 - quotation;
    }
    if (quotation == 0 && delimited == str.substr(i, delimited.size())) {
      break;
    }
  }

  if (start >= static_cast<int>(str.size())) {
    return "";
  }
  return str.substr(start, i - start);
}

void str_split(string str, char delimited, vector<string> *sub_strs) {
  size_t j = 0;
  string sSub;
  bool back_is_delimited = false;
  while (j < str.size()) {
    sSub = get_sub_delimited(str, j, delimited);
    j += sSub.size() + 1;
    sSub = FormatSpace(sSub);
    sub_strs->push_back(sSub);
    back_is_delimited = true;
  }
  if (j == str.size() && back_is_delimited) {
    sub_strs->push_back("");
  }
}

void str_split(string str, string delimited, vector<string> *sub_strs) {
  size_t j = 0;
  string sSub;
  bool back_is_delimited = false;
  while (j < str.size()) {
    sSub = get_sub_delimited_sub(str, j, delimited);
    j += sSub.size() + delimited.size();
    sSub = FormatSpace(sSub);
    sub_strs->push_back(sSub);
    back_is_delimited = true;
  }
  if (j == str.size() && back_is_delimited) {
    sub_strs->push_back("");
  }
}

vector<string> str_split(string str, char delimited) {
  vector<string> sub_strs;
  str_split(str, delimited, &sub_strs);
  return sub_strs;
}

vector<string> str_split(string str, string delimited) {
  vector<string> sub_strs;
  str_split(str, delimited, &sub_strs);
  return sub_strs;
}

vector<string> str_split_2d(string str, string delimited) {
  vector<string> sub_strs;
  str_split(str, delimited, &sub_strs);
  return sub_strs;
}

vector<vector<string>> str_split_2d(string str, string outer_delimited,
                                    string inner_delimited) {
  size_t i;
  vector<vector<string>> sub_strs;

  vector<string> sub_strs_temp = str_split(str, outer_delimited);

  for (i = 0; i < sub_strs_temp.size(); i++) {
    vector<string> sub_strs_subs = str_split(sub_strs_temp[i], inner_delimited);
    sub_strs.push_back(sub_strs_subs);
  }
  return sub_strs;
}

string str_merge(char delimited, const vector<string> &sub_strs) {
  size_t i;
  string str;
  for (i = 0; i < sub_strs.size(); i++) {
    str += sub_strs[i];
    if (i != sub_strs.size() - 1) {
      str += string(1, delimited);
    }
  }
  return str;
}

string str_replace(string str, string org_sub, string new_sub) {
  string sOut = str;
  size_t j = sOut.find(org_sub, 0);
  while (j != string::npos) {
    sOut.replace(j, org_sub.size(), new_sub);
    j += new_sub.size();
    if (j < sOut.size()) {
      j = sOut.find(org_sub, j);
    } else {
      break;
    }
  }
  return sOut;
}

string str_remove_space(string str_in) {
  return str_replace(str_in, " ", "");
  //     int i;
  //     string str = str_in;
  //     for (i = str.size()-1; i>=0; i--)
  //     {
  //         if (str[i]==' ') str = str.substr(0, i) + str.substr(i+1);
  //     }
  //     return str;
}

string get_sub_delimited_multi(string str, int start, string delimited,
                               int size) {
  int i;
  int quotation = 0;
  for (i = start; i < static_cast<int>(str.size()) && (size < 0 || i < size);
       i++) {
    if (str[i] == '"') {
      quotation = 1 - quotation;
    }
    if (quotation == 0) {
      size_t j;
      for (j = 0; j < delimited.size(); j++) {
        if (delimited[j] == str[i]) {
          break;
        }
      }
      if (j != delimited.size()) {
        break;
      }
    }
  }

  // if (i == str.size() || i == size) return string("");

  if (start >= static_cast<int>(str.size())) {
    return "";
  }
  return str.substr(start, i - start);
}

string my_itoa(int64_t i) {
  char str[1024];
  if (i > numeric_limits<int>::max() || i < numeric_limits<int>::min()) {
    snprintf(str, sizeof(str), "%ldL", i);
  } else {
    snprintf(str, sizeof(str), "%d", static_cast<int>(i));
  }

  return string(str);
}

string my_itoa_hex(int i) {
  char str[1024];
  snprintf(str, sizeof(str), "0x%x", i);
  return string(str);
}
string my_itoa_hex(int64_t i) {
  char str[1024];
  snprintf(str, sizeof(str), "0x%lxL", i);
  return string(str);
}

int my_atoi(string s) { return atoi(s.c_str()); }
int my_atoi_hex(string s) {
  s = FormatSpace(s);
  if (s.substr(0, 2) == "0x") {
    s = s.substr(2);  // remove the 0x prefix
  }
  return static_cast<int>(strtol(s.c_str(), nullptr, 16));
}

double my_atod(string s) { return atof(s.c_str()); }
string my_dtoa(double d) {
  char str[1024];
  snprintf(str, sizeof(str), "%-lf", d);
  return string(str);
}

// Remove heading and tail spaces, and quotations
string FormatSpace(string str) {  // remove spaces and quotations
  int i;
  int j;

  for (i = 0; i < static_cast<int>(str.size()); i++) {
    if (str[i] != ' ' && str[i] != '\t' && str[i] != '\r' && str[i] != '\n') {
      break;
    }
  }
  if (i == static_cast<int>(str.size())) {
    return string("");
  }
  if (str[i] == '\"') {
    i++;
  }

  for (j = static_cast<int>(str.size()) - 1; j > i; j--) {
    if (str[j] != ' ' && str[j] != '\t' && str[j] != '\r' && str[j] != '\n') {
      break;
    }
  }
  if (str[j] == '\"') {
    j--;
  }

  return str.substr(i, j - i + 1);
}

string RemoveQuote(string str) {
  int i = 0;
  if (i < str.size() && str[i] == '\"') {
    i++;
  }

  int j = static_cast<int>(str.size()) - 1;
  if (j >= 0 && str[j] == '\"') {
    j--;
  }

  return str.substr(i, j - i + 1);
}
void RemoveSpace(string *str) { *str = str_replace(*str, " ", ""); }

// remove duplication
vector<string> str_vector_add(vector<string> vec0, vector<string> vec1) {
  size_t i;
  size_t j;
  vector<string> vec_ret = vec0;
  for (i = 0; i < vec1.size(); i++) {
    for (j = 0; j < vec0.size(); j++) {
      if (vec1[i] == vec0[j]) {
        break;
      }
    }
    if (j == vec0.size()) {
      vec_ret.push_back(vec1[i]);
    }
  }
  return vec_ret;
}

vector<string> str_vector_sub(vector<string> vec0, vector<string> vec1) {
  size_t i;
  size_t j;
  vector<string> vec_ret;
  for (i = 0; i < vec0.size(); i++) {
    for (j = 0; j < vec1.size(); j++) {
      if (vec0[i] == vec1[j]) {
        break;
      }
    }
    if (j == vec1.size()) {
      vec_ret.push_back(vec0[i]);
    }
  }
  return vec_ret;
}

vector<string> str_vector_and(vector<string> vec0, vector<string> vec1) {
  size_t i;
  size_t j;
  vector<string> vec_ret;
  for (i = 0; i < vec0.size(); i++) {
    for (j = 0; j < vec1.size(); j++) {
      if (vec0[i] == vec1[j]) {
        break;
      }
    }
    if (j != vec1.size()) {
      vec_ret.push_back(vec0[i]);
    }
  }
  return vec_ret;
}

int str_vector_find(vector<string> vec, string str) {
  size_t i;
  for (i = 0; i < vec.size(); i++) {
    if (vec[i] == str) {
      return i;
    }
  }
  return -1;
}

void str_vector_reverse(vector<string> *vec) {
  size_t i;
  vector<string> new_vec;
  for (i = 0; i < vec->size(); i++) {
    new_vec.push_back((*vec)[vec->size() - 1 - i]);
  }
  *vec = new_vec;
}

string get_lower_case(string str) {
  string str_out = str;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      str_out[i] = str[i] - 'A' + 'a';
    }
  }
  return str_out;
}
string get_upper_case(string str) {
  string str_out = str;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] >= 'a' && str[i] <= 'z') {
      str_out[i] = str[i] - 'a' + 'A';
    }
  }
  return str_out;
}

string get_filename_ext(string filename) {
  size_t j0 = filename.rfind(".");
  if (j0 != string::npos) {
    return filename.substr(j0 + 1);
  }
  return "";
}
