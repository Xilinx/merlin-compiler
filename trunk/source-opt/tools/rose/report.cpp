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


#include "report.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <map>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"

using std::fstream;
using std::ifstream;
using std::map;
using std::ofstream;
using std::string;

using rapidjson::Document;
using rapidjson::IStreamWrapper;
using rapidjson::OStreamWrapper;
using rapidjson::PrettyWriter;
using rapidjson::Value;

void readInterReport(string path, map<string, map<string, string>> *content) {
  assert(content != nullptr);
  content->clear();

  ifstream ifs(path);
  if (!ifs.good()) {
    fprintf(stderr, "cant read file: %s\n", path.c_str());
    assert(0);
    return;
  }
  IStreamWrapper isw(ifs);
  Document d;
  d.ParseStream(isw);

  for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
    string key = it->name.GetString();
    map<string, string> attributes;
    for (auto iter = it->value.MemberBegin(); iter != it->value.MemberEnd();
         ++iter) {
      attributes[iter->name.GetString()] = iter->value.GetString();
    }
    (*content)[key] = attributes;
  }
}

void writeInterReport(string path,
                      const map<string, map<string, string>> &content) {
  Document d;
  d.SetObject();
  Document::AllocatorType &allocator = d.GetAllocator();
  for (auto it = content.begin(); it != content.end(); ++it) {
    Value attributes_json;
    attributes_json.SetObject();
    for (auto iter = it->second.begin(); iter != it->second.end(); ++iter) {
      Value attribute_name;
      attribute_name.SetString(iter->first.c_str(), allocator);
      Value attribute_value;
      attribute_value.SetString(iter->second.c_str(), allocator);
      attributes_json.AddMember(attribute_name, attribute_value, allocator);
    }
    Value key;
    key.SetString(it->first.c_str(), allocator);
    d.AddMember(key, attributes_json, allocator);
  }

  ofstream ofs(path);
  if (!ofs.good()) {
    fprintf(stderr, "cant write file: %s\n", path.c_str());
    assert(0);
    return;
  }
  OStreamWrapper osw(ofs);
  PrettyWriter<OStreamWrapper> writer(osw);
  d.Accept(writer);
}
