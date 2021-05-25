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




#include "cmdline_parser.h"
#include "xml_parser.h"
#include "file_parser.h"

// 1. default for environments
// 2. extract param_define calls
// 3. combined with input xml // TODO

map<string, map<string, string>>
    g_param_attr;  // param_name->attribute_name->value

map<string, string> search_params_in_string(string sContent) {
  map<string, string> Params;

  size_t j = 0;
  size_t i;
  while (true) {
    if (j >= static_cast<int>(sContent.size())) {
      break;
    }
    int j0 = j;
    j = sContent.find("param_define", j0);
    if (string::npos == j) {
      break;
    }

    // fixed by zhangpeng, 2013-12-28
    {
      // marker is //;
      int j_newline = sContent.rfind("\n", j);
      int j_marker = sContent.find("//;", j_newline + 1);
      int j_comment = sContent.find("//; #", j_newline + 1);

      if (j_marker == -1 || j_marker > j) {
        j += strlen("param_define");
        continue;
      }
      if (j_comment != -1 && j_comment < j) {
        j += strlen("param_define");
        continue;
      }
    }

    string header = get_sub_delimited(sContent, j, '(');
    j += header.size() + 1;
    string sParam = get_sub_delimited(sContent, j, ',');
    j += sParam.size() + 1;
    string sValue = get_sub_delimited(sContent, j, ')');
    j += sValue.size() + 1;

    vector<string> sValue_Attr = str_split(sValue, ",");
    assert(!sValue_Attr.empty());

    // ZP: added 2013-12-25
    map<string, string> mapAttr;
    for (i = 1; i < sValue_Attr.size(); i++) {
      string sAttr = sValue_Attr[i];
      vector<string> vecAttr = str_split(sAttr, "=");
      if (vecAttr.size() >= 2) {
        mapAttr[FormatSpace(vecAttr[0])] = FormatSpace(vecAttr[1]);
      }
    }
    g_param_attr[FormatSpace(sParam)] = mapAttr;

    Params[FormatSpace(sParam)] = FormatSpace(sValue_Attr[0]);
  }

  return Params;
}

map<string, vector<string>> search_params_array_in_string(string sContent) {
  map<string, vector<string>> Params;

  size_t j = 0;
  while (true) {
    if (j >= static_cast<int>(sContent.size())) {
      break;
    }
    j = sContent.find("param_array_define", j);
    if (string::npos == j) {
      break;
    }

    string header = get_sub_delimited(sContent, j, '(');
    j += header.size() + 1;
    string sParam = get_sub_delimited(sContent, j, ',');
    j += sParam.size() + 1;
    string sValues = get_sub_delimited(sContent, j, ')');
    j += sValues.size() + 1;

    size_t jj = 0;
    vector<string> vecValues;
    while (jj < sValues.size()) {
      string sOneValue = get_sub_delimited(sValues, jj, ',');
      jj += sOneValue.size() + 1;
      vecValues.push_back(FormatSpace(sOneValue));
    }

    Params[FormatSpace(sParam)] = vecValues;  // FormatSpace(sValue);
  }

  return Params;
}

void cfg_gen(const CInputOptions &options) {
  int i;
  string sFileName_xml_in = options.get_option("-x");
  string sDirName_p_list = options.get_option("-d");
  string sFileName_xml_out = options.get_option("-o");

  // 1. if xml_in not exist and "-i" flag, create empty xml_in
  CXMLParser xml_in_parser;  // memory container for xml tree
  CXMLNode *pRoot_in;

  // 1.1 test if xml_in exist or not
  int xml_in_exist;
  {
    FILE *fp_tmp;
    fp_tmp = fopen(sFileName_xml_in.c_str(), "rb");
    xml_in_exist = static_cast<int>(nullptr != fp_tmp);
    if (fp_tmp != nullptr) {
      fclose(fp_tmp);
    }
  }

  // 1.2 parse xml file
  if (xml_in_exist == 0) {
#ifdef DEBUG_PRINT
    printf("[cfg_gen] INFO: XML input file does not exist: %s.\n",
           sFileName_xml_in.c_str());
#endif

    if (string("") != options.get_option("-i")) {
      exit(0);
    } else {
#ifdef DEBUG_PRINT
      printf("[cfg_gen] INFO: Use empty file instead.\n");
#endif
      pRoot_in = xml_in_parser.parse_empty("configuration_top");
    }
  } else {
    pRoot_in = xml_in_parser.parse_from_file(sFileName_xml_in);
  }

  // 2. if <p_list> not exist, parse dir for p_list
  vector<CXMLNode *> vecPList = pRoot_in->TraverseByName("p_file_list");
  vector<string> p_file_list;
  if (vecPList.empty()) {
    if (0 == options.get_option_num("-f")) {
      if (sDirName_p_list.empty()) {
        printf("[cfg_gen] ERROR: both p_file_list and working dir are not "
               "specified.\n");
        exit(0);
      }
      p_file_list = get_files_in_dir(sDirName_p_list);
      // <p_file_list>
      // <p_file>filename</p_file>
      // </p_file_list>
      string sXML = "<p_file_list>";
      for (i = 0; i < static_cast<int>(p_file_list.size()); i++) {
        sXML += "<p_file>" + p_file_list[i] + "</p_file>";
      }
      sXML += "</p_file_list>";
      CXMLNode *pNewSubTree = xml_in_parser.build_element_from_string(sXML);
      pRoot_in->AppendChild(pNewSubTree);
    } else {
      string sXML = "<p_file_list>";
      for (i = 0; i < options.get_option_num("-f"); i++) {
        string p_file = options.get_option("-f", i);
        p_file_list.push_back(p_file);
        sXML += "<p_file>" + p_file + "</p_file>";
      }
      sXML += "</p_file_list>";
      CXMLNode *pNewSubTree = xml_in_parser.build_element_from_string(sXML);
      pRoot_in->AppendChild(pNewSubTree);
    }
  } else {
    assert(1 == vecPList.size());  // only have one p_file_list
    CXMLNode *pList = vecPList[0];
    for (i = 0; i < pList->GetChildNum(); i++) {
      CXMLNode *pFile = pList->GetChildByIndex(i);
      assert(pFile->GetName() == "p_file");
      p_file_list.push_back(pFile->GetValue());
    }
  }

  // 3. open each p_list and search for param_define calls
  map<string, map<string, string>> mapParams;  // file * (name * default)
  map<string, map<string, vector<string>>>
      mapParams_array;  // file * (name * default_vec)
  for (i = 0; i < static_cast<int>(p_file_list.size()); i++) {
    map<string, string> tmp;
    mapParams.insert(pair<string, map<string, string>>(p_file_list[i], tmp));
    string sPathName = p_file_list[i];
    // string sPathName = sDirName_p_list + "/" + p_file_list[i];
    string sContent = read_string_from_file(sPathName);
    mapParams[p_file_list[i]] = search_params_in_string(sContent);
    mapParams_array[p_file_list[i]] = search_params_array_in_string(sContent);
  }

  // 4. add segment into xml_out
  // <configuratoin file="xxx">
  //    <param name="">value</param>
  // </configuration>
  map<string, map<string, string>>::iterator file_it;
  map<string, string>::iterator param_it;
  for (file_it = mapParams.begin(); file_it != mapParams.end(); file_it++) {
    string sXML;
    string sFileName = file_it->first;
    map<string, string> mapOneFile = file_it->second;
    sXML += "<configuration file=" + sFileName + ">";
    for (param_it = mapOneFile.begin(); param_it != mapOneFile.end();
         param_it++) {
      string sParam = param_it->first;
      string sValue = param_it->second;

      string sAttr;
      map<string, string> mapAttr = g_param_attr[sParam];
      map<string, string>::iterator attr_it;
      for (attr_it = mapAttr.begin(); attr_it != mapAttr.end(); attr_it++) {
        sAttr += " " + attr_it->first + "=\"" + attr_it->second + "\"";
      }

      sXML += "<param name=" + sParam + sAttr + ">" + sValue + "</param>";
    }

    map<string, vector<string>> mapOneFile_array =
        mapParams_array[file_it->first];
    map<string, vector<string>>::iterator param_it_array;
    for (param_it_array = mapOneFile_array.begin();
         param_it_array != mapOneFile_array.end(); param_it_array++) {
      string sParam = param_it_array->first;
      vector<string> sValues = param_it_array->second;

      sXML += "<param_array name=" + sParam + ">";
      for (size_t j = 0; j < sValues.size(); j++) {
        sXML += "<item>" + sValues[j] + "</item>";
      }
      sXML += "</param_array>";
    }

    sXML += "</configuration>";
    CXMLNode *pNewSubTree = xml_in_parser.build_element_from_string(sXML);
    pRoot_in->AppendChild(pNewSubTree);
  }

  //// 5. create node and insert to xml
  // CXMLNode * pNewSubTree = xml_in_parser.build_element_from_string(sXML);
  // vector<CXMLNode *> ConfigNodes = pRoot_in->TraverseByName("configuration",
  // 1); if (0 == ConfigNodes.size())
  //{
  //    pRoot_in->AppendChild(pNewSubTree);
  //}
  // else if (1 == ConfigNodes.size())
  //{
  //    // FIXME: do nothing here for now
  //    // MatchAndInsertDefault();
  //    // ... something like ConfigNodes[0]->ReplaceSubTree(pNewSubTree);
  //    assert(0);
  //}
  // else
  //{
  //    assert(0);
  //}

  // 7. dump to file
  xml_in_parser.write_into_file(sFileName_xml_out);
}

int main(int argc, char *argv[]) {
  CInputOptions options;
  options.set_flag("-d", 1, 0);
  options.set_flag("-f", 100, 0);
  options.set_flag("-x", 1, 0);
  options.set_flag("-o", 1, 0);
  options.set_flag("-i");
  options.set_flag("", 1);  // only cfg_gen (exe) is non-flag argument

  if ((options.parse(argc, argv) == 0) || (options.get_option_num("-d") == 0 &&
                                           options.get_option_num("-f") == 0)) {
    printf(
        "Usage: cfg_gen [-d dir] [-f p_file] [-x xml_in] [-o xml_out] [-i]\n");
    printf("    detect _p files in dir or xml_in, and extract parameters into "
           "xml_out.\n");
    printf("parameters: \n");
    printf("       -d // Directory of configurable source (_p) files, used "
           "when <p_file_list> does not exist in xml_in\n");
    printf("       -f // One specified _p file, used when <p_file_list> does "
           "not exist in xml_in\n");
    printf("       -i // exit if xml_in does not exist (by default, an empty "
           "xml_in is used)\n");
    exit(0);
  }

  // default values;
  if (0 == options.get_option_num("-d")) {
    options.add_option("-d", "./");
  }
  if (0 == options.get_option_num("-x")) {
    options.add_option("-x", "./cfg_in.xml");
  }
  if (0 == options.get_option_num("-o")) {
    options.add_option("-o", "./cfg.xml");
  }

  cfg_gen(options);

  return 1;
}
