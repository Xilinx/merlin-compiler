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



#include <assert.h>
#include <string>
#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

using std::string;

string remove_linemark(string str) {  // remove spaces and quotations
  int j;

  for (j = static_cast<int>(str.size()) - 1; j >= 0; j--) {
    if (str[j] != '\r' && str[j] != '\n') {
      break;
    }
  }
  return str.substr(0, j + 1);
}

string translate(string str, int *warning, int line_idx) {
  *warning = 0;
  string newstr = str;
  string newstr1;  // temp variable

  int j = 0;

  // 1. \ => \\;
  while (true) {
    j = newstr.find("\\", j);
    if (-1 == j) {
      break;
    }

    string newstr1 =
        newstr.substr(0, j) + string("\\\\");  // newstr[j] is replaced.
    if (j + 1 < static_cast<int>(newstr.size())) {
      newstr1 += newstr.substr(j + 1);
    }
    newstr = newstr1;
    j += 2;
  }

  // 2. ' => '."'".' // keep true '
  j = 0;
  while (true) {
    j = newstr.find("'", j);
    if (-1 == j) {
      break;
    }

    string newstr1 =
        newstr.substr(0, j) + string("'.\"'\".'");  // newstr[j] is replaced.
    if (j + 1 < static_cast<int>(newstr.size())) {
      newstr1 += newstr.substr(j + 1);
    }
    newstr = newstr1;
    j += 7;
  }

  // 3. `perl` => '."perl".'
  j = 0;
  while (true) {
    // 1. ` => '."
    j = newstr.find("`", j);
    if (-1 == j) {
      break;
    }

    if (newstr[j + 1] != '`' && newstr[j + 1] != '$') {
      *warning = 1;
    }

    if (j + 1 < static_cast<int>(newstr.size()) &&
        newstr[j + 1] == '`') {       // `` => `  // keep true ` in source code
      newstr1 = newstr.substr(0, j);  // newstr[j] is skipped.
      if (j + 1 < static_cast<int>(newstr.size())) {
        newstr1 += newstr.substr(j + 1);
      }
      newstr = newstr1;
      j++;
      continue;
    }
    string newstr1 =
        newstr.substr(0, j) + string("'.\"");  // newstr[j] is replaced.
    if (j + 1 < static_cast<int>(newstr.size())) {
      newstr1 += newstr.substr(j + 1);
    }
    newstr = newstr1;
    j += 3;

    // 2. ` => ".'
    int jj_old = j;
    j = newstr.find("`", j);
    if (-1 == j) {
      printf("[src_cfg] ERROR: quote ` mismatch at line %d, col %d :\n",
             line_idx, jj_old);
      printf("\"%s\"\n", newstr.c_str());
      printf("Please check temp.pl for the details.\n");
      assert(0);  // `` is not matched.
    }

    if (j + 1 < static_cast<int>(newstr.size()) && newstr[j + 1] == '`') {
      printf("[src_cfg] ERROR: a `` is found in the embedded perl, which is "
             "not allowed. \n    [j=%d] %s\n",
             j, str.c_str());
      assert(0);  // do not allow true ` in embedded perl
    }

    newstr1 = newstr.substr(0, j) + string("\".'");  // newstr[j] is replaced.
    if (j + 1 < static_cast<int>(newstr.size())) {
      newstr1 += newstr.substr(j + 1);
    }
    newstr = newstr1;
    j += 3;
  }
  return newstr;
}

// return 1 if the front pos char is ' ' or '\t'
int trailing_empty(string str, int pos) {
  for (int i = 0; i < pos; i++) {
    if (str[i] != ' ' && str[i] != '\t') {
      return 0;
    }
  }
  return 1;
}

string preprocess_one_line(string sLine) {
  string sOut = sLine;
  sOut = FormatSpace(sOut);
  string sHeader = get_sub_delimited_multi(sOut, 0, "\r\n\t ");
  sOut = sOut.substr(sHeader.size());

  sOut = FormatSpace(sOut);
  string sCmd = get_sub_delimited_multi(sOut, 0, "\r\n\t ");
  sOut = sOut.substr(sCmd.size());

  if (sHeader != "\\g:" && sHeader != "//;") {
    return sLine;
  }

  if ("#include" == sCmd) {
    sOut = FormatSpace(sOut);
    string sFileName = get_sub_delimited_multi(sOut, 0, "\r\n\t ");
    sOut = sOut.substr(sFileName.size());

    string sLineOut = read_string_from_file(sFileName);
    return sLineOut;
  }
  // else if ("#xxx" == sCmd) {}

  return sLine;

  return sLine;
}

int preprocessing(string sFile, string sFile_int) {
  string sLines = read_string_from_file(sFile);
  string sLinesOut;

  size_t j = 0;

  while (j < sLines.size()) {
    string sLine = get_sub_delimited(sLines, j, '\n');
    j += sLine.size() + 1;
    string sLineOut = remove_linemark(sLine) + "\n";

    sLineOut = preprocess_one_line(sLineOut);

    // printf("%s\n", sLineOut.c_str());

    sLinesOut += sLineOut;
  }

  write_string_into_file(sLinesOut, sFile_int);
  return 1;
}

void src_cfg(const CInputOptions &options) {
  string sFile_p = options.get_option("", 1);
  string sFile = options.get_option("-o");
  string sDefines = options.get_option("-p");
  string sQuote = options.get_option("-q");

  string sFile_p_int = sFile_p + "_int";
  preprocessing(
      sFile_p,
      sFile_p_int);  // transform input file into input_int for preprocessing

  // ZP: 2014-01-01
  // DO not configure if no "//;" or "\\g:"
  {
    string sContent = read_string_from_file(sFile_p_int);
    if (-1 == sContent.find("//;") && -1 == sContent.find("\\g:")) {
      write_string_into_file(sContent, sFile);
      return;
    }
  }

  FILE *fp_in;
  if ((fp_in = fopen(sFile_p_int.c_str(), "rb")) == nullptr) {
    printf("Error in open file %s\n", sFile_p.c_str());
    assert(0);
  }
  FILE *fp_out;
  if ((fp_out = fopen("./temp.pl", "wb")) == nullptr) {
    printf("Error in open file %s\n", "temp.pl");
    assert(0);
  }

  string sElement;
  string sLine;
  int i;

  // require "defines.pl";
  sLine = string("require \"") + sDefines + string("\";\n");
  fwrite(sLine.c_str(), 1, sLine.size(), fp_out);

  // open (OUTFILE, '>file_out');
  sLine = string("open (OUTFILE, '>") + sFile + string("');\n");
  fwrite(sLine.c_str(), 1, sLine.size(), fp_out);

  char sLineIn[40960];
  int line_idx = 1;  // line number starts from 1
  while (fgets(sLineIn, 40960, fp_in) != nullptr) {
    sElement = string(sLineIn);
    sElement = remove_linemark(sElement);

    int j = sElement.find("//;");
    int j0 = sElement.find("\\g:");

    // NOTE: a real perl line should start by "//;", with spaces allowed
    int fake_marker = static_cast<int>(
        (j > 0 && (trailing_empty(sElement, j) == 0)) ||
        (j0 > 0 && (trailing_empty(sElement, j0) ==
                    0)));  // the content of source file has "//;"

    if (fake_marker != 0) {
      string marker = (j > 0) ? "//;" : "\\g:";
      printf("[src_cfg] Warning (line %d): Perl marker %s should be in the "
             "front of a line (spaces are allowed). \n",
             line_idx, marker.c_str());

      // fixed by Peng Zhang 2013-01-07
      // assert(0);
    }

    if ((fake_marker == 0) && (-1 != j || -1 != j0)) {  // perl line
      int j_j0 = (j >= 0) ? j : j0;

      static int msg_shown = 0;
      if (msg_shown == 0 && -1 != j0) {
        printf("[src_cfg] Warning (line %d): Perl marker \\g: is deprecate, "
               "use //; instead. \n",
               line_idx);
        msg_shown = 1;
      }

      for (i = 0; i < j; i++) {
        if (sElement[i] != ' ' && sElement[i] != '\t') {
          printf("ERROR: can not have text before perl prefix //; \n");
          assert(0);
        }
      }
      sLine = sElement.substr(j_j0 + 3) + string("\n");
    } else {  // normal line
      if (sQuote != "simple") {
        int warning = 0;
        sElement = translate(sElement, &warning, line_idx);
        if (1 == warning) {
          printf("[src_cfg] Warning (line %d): perl quote ` is not followed by "
                 "perl variable $, potentially missing de-emulation ``.\n",
                 line_idx);
        }
        sLine = string("print OUTFILE '") + sElement + string("'.\"\\n\";\n");
      } else {
        sLine = string("print OUTFILE \"") + sElement + string("\\n\";\n");
      }
    }

    fwrite(sLine.c_str(), 1, sLine.size(), fp_out);
    line_idx++;
  }

  // close( OUTFILE);
  sLine = string("close (OUTFILE);\n");
  fwrite(sLine.c_str(), 1, sLine.size(), fp_out);

  fclose(fp_in);
  fclose(fp_out);

  system("perl ./temp.pl");
}

// defines.pl is needed, which is generated by cfg2def from cfg.xml
int main(int argc, char *argv[]) {
  system("rm -rf src_cfg_succeed");

  CInputOptions options;
  options.set_flag("-o", 1, 0);
  options.set_flag("-p", 1, 0);
  options.set_flag("-q", 1, 0);
  options.set_flag("", 2);

  if (options.parse(argc, argv) == 0) {
    printf("Usage: src_cfg src_p [-p defines] [-o src] [-q quote_style]\n");
    printf("   Configure source file src_p by the definition perl file \n");
    printf("parameters:\n");
    printf("   defines: a Perl file generated by cfg2def. \n");
    printf("   quote_style [default|simple]: default uses ` to quote perl, and "
           "simple does not. \n");
    exit(1);
  }

  // default values;
  string sFile_p = options.get_option("", 1);
  string sFile = sFile_p.substr(0, sFile_p.size() - 2);
  if (0 == options.get_option_num("-o")) {
    options.add_option("-o", sFile);
  }
  if (0 == options.get_option_num("-p")) {
    options.add_option("-p", "./defines.pl");
  }
  if (0 == options.get_option_num("-p")) {
    options.add_option("-q", "default");
  }

  src_cfg(options);

  system("touch src_cfg_succeed");
  return 0;
}
