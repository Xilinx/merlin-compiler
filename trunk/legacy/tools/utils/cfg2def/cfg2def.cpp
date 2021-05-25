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

string remove_file_suffix(string sFileName) {
  int i = sFileName.rfind('.');
  return sFileName.substr(0, i);
}

// 1. cfg.xml to perl definitions
// 2. definitions of param_define

void cfg2def(const CInputOptions &options) {
  int i;
  size_t ii;

  string sXMLFile = options.get_option("", 1);
  string sPerlFile = options.get_option("-o");

  string sOut;

  // configurations
  CXMLParser xml_parser;
  CXMLNode *pRoot = xml_parser.parse_from_file(sXMLFile);
  vector<CXMLNode *> ConfigNodes = pRoot->TraverseByName("configuration", 1);

  map<string, string> mapParamValue;
  map<string, vector<string>> mapParamValue_array;
  map<string, int> mapParamTree;
  if (ConfigNodes.empty()) {
    // no reconfiguration, use default values
  } else if (!ConfigNodes.empty()) {
    for (ii = 0; ii < ConfigNodes.size(); ii++) {
      CXMLNode *pConfig = ConfigNodes[ii];
      vector<CXMLNode *> Params = pConfig->TraverseByName("param", 2);

      for (i = 0; i < static_cast<int>(Params.size()); i++) {
        CXMLNode *pParam = Params[i];
        string sName = pParam->GetParam("name");
        string sValue = pParam->GetValue();

        if (mapParamValue.find(sName) != mapParamValue.end()) {
          printf("[cfg2def] ERROR: Parameter name redefined in %s. (ParamName: "
                 "%s)",
                 sXMLFile.c_str(), sName.c_str());
          exit(0);
        }
        mapParamValue[sName] = sValue;
      }

      vector<CXMLNode *> Params_array =
          pConfig->TraverseByName("param_array", 2);

      for (i = 0; i < static_cast<int>(Params_array.size()); i++) {
        CXMLNode *pParam = Params_array[i];
        string sName = pParam->GetParam("name");
        if (mapParamValue_array.find(sName) != mapParamValue_array.end()) {
          printf("[cfg2def] ERROR: Parameter array name redefined in %s. "
                 "(ArrayName: %s)",
                 sXMLFile.c_str(), sName.c_str());
          exit(0);
        }

        vector<CXMLNode *> vecItem = pParam->TraverseByName("item", 1);
        vector<string> vecValues;
        for (size_t j = 0; j < vecItem.size(); j++) {
          string sValue = vecItem[j]->GetValue();
          vecValues.push_back(sValue);
        }
        mapParamValue_array[sName] = vecValues;
      }

      vector<CXMLNode *> Params_tree = pConfig->TraverseByName("param_tree", 1);
      for (i = 0; i < static_cast<int>(Params_tree.size()); i++) {
        CXMLNode *pParam = Params_tree[i];
        string sName = pParam->GetParam("name");
        if (mapParamTree.find(sName) != mapParamTree.end()) {
          printf("[cfg2def] ERROR: Parameter tree name redefined in %s. "
                 "(TreeName: %s)",
                 sXMLFile.c_str(), sName.c_str());
          exit(0);
        }
        mapParamTree[sName] = i;
      }
      mapParamTree[""] = -1;
    }
  }

  // Write out parameter definitions
  // %parameters = ("key" => "value", "key" => "value", ...);
  if (!mapParamValue.empty()) {
    sOut += "%parameters = (\n";
    map<string, string>::iterator it;
    for (it = mapParamValue.begin(); it != mapParamValue.end(); it++) {
      // "para_name"=>"param_value",
      sOut += "\"" + it->first + "\"=>\"" + it->second + "\",\n";
    }
    sOut += ");\n";

    // param_define function:
    // sub param_define {
    //    ($name, $value) = @_;
    //    if ($parameters{$name} eq "") {
    //        return $value;
    //    }
    //    else {
    //        return $parameters{$name};
    //    }
    //}
    sOut += "sub param_define {\n";
    sOut += "    ($name, $value, @remains) = @_;\n";
    sOut += "    if (not exists $parameters{$name}) {\n";
    sOut += "        return $value;\n";
    sOut += "    }\n";
    sOut += "    else {\n";
    sOut += "        return $parameters{$name};\n";
    sOut += "    }\n";
    sOut += "}\n";
  } else {
    sOut += "%parameters = (\n";
    // "para_name"=>"param_value",
    sOut += "\" dummy \"=>\" dummy_valu\",\n";
    sOut += ");\n";
  }

  // sub param_array_define {
  //    ($name, @value) = @_;
  //    if ("dep_iter_name" eq $name) {
  //        return ("dep1", "dep2");
  //    }
  //    else if ("dep_II" eq $name) {
  //        return ("5", "5");
  //    }
  //    else if ("dep_iter_width" eq $name) {
  //        return ("11", "11");
  //    }
  //    else {
  //        return @value;
  //    }
  //}

  if (!mapParamValue_array.empty()) {
    sOut += "sub param_array_define {\n";
    sOut += "    ($name, @value) = @_;\n";

    map<string, vector<string>>::iterator it1;
    for (it1 = mapParamValue_array.begin(); it1 != mapParamValue_array.end();
         it1++) {
      string str = (mapParamValue_array.begin() == it1) ? "if" : "elsif";
      sOut += "  " + str + "(\"" + it1->first + "\" eq $name ) {\n";
      sOut += "    return (";
      vector<string> vecValues = it1->second;
      size_t j;
      for (j = 0; j < vecValues.size(); j++) {
        sOut += "\"" + vecValues[j] + "\"";
        if (j != vecValues.size() - 1) {
          sOut += ",";
        }
      }
      sOut += ");\n  }\n";
    }
    // sOut += "    if ("dep_iter_name" eq $name) {\n";
    // sOut += "        return ("dep1", "dep2");\n";
    // sOut += "    }\n";
    sOut += "  else {\n";
    sOut += "    return @value;\n";
    sOut += " }\n";
    sOut += "}\n\n";
  } else {
    sOut += "sub param_array_define {\n";
    sOut += "    ($name, @value) = @_;\n";
    sOut += "    return @value;\n";
    sOut += "}\n\n";
  }

  // load xxx_cfg.xml
  // sub param_tree_define {
  //    ($name, @xml_file.xml) = @_;
  //    if ("tree_name0" eq $name) {
  //        return (@($xxx_cfg->{configuration}->{param_tree}))[0];
  //    }
  //    else if ("dep_II" eq $name) {
  //        ...
  //    }
  //    else if ("dep_iter_width" eq $name) {
  //        ...
  //    }
  //    else if ($xml_file.xml > 0) {  // can not implement, because C do not
  // know the variable name of xml loaded file
  //    load $xml_file.xml[0];
  //        return $xml_file;
  //    }
  //}
  if (!mapParamTree.empty()) {
    sOut += "use POSIX;\n";
    sOut += "use XML::Simple;\n";
    sOut += "use Data::Dumper;\n";
    sOut += "sub param_tree_define {\n";
    sOut += "  my ($name, $xml_file, $str_array_reserve) = @_;\n";
    sOut += "  my @array_reserve = split(/ /, $str_array_reserve);\n";
    sOut += "  my $xml = new XML::Simple(KeyAttr=>'nan', suppressempty=>'', "
            "ForceArray=>\\@array_reserve);\n";
    sOut += "  my $test = $xml->XMLin(\"" + sXMLFile + "\");\n";
    sOut += "  if (-e \"$xml_file\") { $test = $xml->XMLin($xml_file); }\n";
    sOut += "  if ($name eq \"\" ) { return $test; }\n";
    sOut += "  if (ref $test->{param_tree} ne 'ARRAY')\n";
    sOut += "  {\n";
    sOut += "    $param_tree = $test->{param_tree};\n";
    sOut += "    if( $name eq $param_tree->{name} )\n";
    sOut += "    {\n";
    sOut += "      return $param_tree;\n";
    sOut += "    }\n";
    sOut += "  }\n";
    sOut += "  else {\n";
    sOut += "    foreach my $param_tree (@{$test->{param_tree}})\n";
    sOut += "    {\n";
    sOut += "      if( $name eq $param_tree->{name} )\n";
    sOut += "      {\n";
    sOut += "        return $param_tree;\n";
    sOut += "      }\n";
    sOut += "    }\n";
    sOut += "  }\n";
    sOut += "  if (\"\" eq $xml_file) { return 0; } \n";
    sOut += "  if (not -e \"$xml_file\") { print \"\\n param_tree_define: tree "
            "XML file ($xml_file) is not found.\\n\"; $input = <STDIN>; "
            "die(\"\\n\"); } \n";
    sOut += "  return $xml->XMLin($xml_file);\n";
    sOut += "}\n\n";
  } else {
    sOut += "use POSIX;\n";
    sOut += "use XML::Simple;\n";
    sOut += "use Data::Dumper;\n";
    sOut += "sub param_tree_define {\n";
    sOut += "  my ($name, $xml_file, $str_array_reserve) = @_;\n";
    sOut += "  my @array_reserve = split(/ /, $str_array_reserve);\n";
    // sOut += "  print @array_reserve; \n";
    sOut += "  my $xml = new XML::Simple(KeyAttr=>'nan', "
            "ForceArray=>[@array_reserve]);\n";
    sOut += "  my $test = $xml->XMLin(\"" + sXMLFile + "\");\n";
    sOut += "  if (-e \"$xml_file\") { $test = $xml->XMLin($xml_file); }\n";
    sOut += "  if ($name eq \"\") { return $test; }\n";
    sOut += "  if (\"\" eq $xml_file) { return 0; } \n";
    sOut += "  if (not -e \"$xml_file\") { print \"\\n param_tree_define: tree "
            "XML file ($xml_file) is not found.\\n\"; $input = <STDIN>; "
            "die(\"\\n\"); } \n";
    sOut += "  return $xml->XMLin($xml_file);\n";
    sOut += "}\n\n";
  }

  write_string_into_file(sOut, sPerlFile);
}

int main(int argc, char *argv[]) {
  CInputOptions options;
  options.set_flag("-o", 1, 0);
  options.set_flag("", 2);

  if (options.parse(argc, argv) == 0) {
    printf("Usage: cfg2def xml_in [-o perl_out]\n");
    printf("    Generate perl define files from configuration xml.\n");
    exit(0);
  }

  // default values;
  if (0 == options.get_option_num("-o")) {
    options.add_option("-o", "./defines.pl");
  }

  cfg2def(options);
  return 0;
}
