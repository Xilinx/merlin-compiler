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
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "file_parser.h"
#include "stdio.h"

// char key[] = "falconisgreat";

using std::string;

// #define IS_ENC 1  // defined outside

int main(int argc, char **argv) {
  if (argc < 3) {
    return -1;
  }

  string keys = string(argv[1]);
  string file = string(argv[2]);

  // cout << keys << " " << file << endl;

  string content = read_string_from_file(file);

  vector<char> vec_key;
  size_t i;
  for (i = 0; i < keys.length(); i++) {
    char a = keys[i];
    char b = static_cast<char>((a >= '0' && a <= '9')
                                   ? (a - '0')
                                   : (a >= 'a' && a <= 'z')
                                         ? (a - 'a' + static_cast<char>(10))
                                         : static_cast<char>(0));
    // cout << string(my_itoa(b)) << endl;

    vec_key.push_back(b);
  }
  // cout << keys << endl;

  for (i = 0; i < content.length(); i++) {
    size_t idx = i % vec_key.size();
    char a = content[i];
    char org_offset;
    char b;
    char add_on;
    if (a >= 'a' && a <= 'z') {
      b = 'a';
      org_offset = static_cast<char>(a - 'a');
    } else if (a >= 'A' && a <= 'Z') {
      b = 'A';
      org_offset = static_cast<char>(a - 'A');
    } else {
      continue;
    }

    if (IS_ENC) {
      add_on = static_cast<char>(org_offset +
                                 (vec_key[idx] % static_cast<char>(26)));
    } else {
      add_on = static_cast<char>(org_offset -
                                 (vec_key[idx] % static_cast<char>(26)));
    }
    add_on = static_cast<char>(add_on % ('z' - 'a' + static_cast<char>(1)));
    if (add_on < 0) {
      add_on += 26;
    }

    char d;
    d = content[i] = b + add_on;

    ///////////////////////////////////////
    if (a >= 'a' && a <= 'z') {
      org_offset = static_cast<char>(d - 'a');
    } else if (a >= 'A' && a <= 'Z') {
      org_offset = static_cast<char>(d - 'A');
    }

    if (IS_ENC) {
      add_on = static_cast<char>(org_offset -
                                 (vec_key[idx] % static_cast<char>(26)));
    } else {
      add_on = static_cast<char>(org_offset +
                                 (vec_key[idx] % static_cast<char>(26)));
    }
    add_on = static_cast<char>(add_on % ('z' - 'a' + static_cast<char>(1)));
    if (add_on < 0) {
      add_on += 26;
    }
    char e = static_cast<char>(b + add_on);

    // printf("%c:%d %c:%d %d:%d %d %c:%d %c:%d\n", a,a, b,b, c,c1, add_on, d,
    // d, e, e); getchar();

    assert(e == a);
  }
  write_string_into_file(content, file);

  return 0;
}
