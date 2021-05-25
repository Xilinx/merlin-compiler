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
#include <iostream>
#include "file_parser.h"

char str[1024 * 1024];

char key[] = "falconisgreat";

#define DEBUG_PRINT 0
#define text_to_string(str) #str

string generate_random_filename() {
#define FILENAME_LENGTH 15
  int i;
  string str;
  str.resize(FILENAME_LENGTH);

  unsigned int seed;
  {
    int tmp = 0;
    struct timespec t_start;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    seed = time(nullptr) * (((int64_t)&tmp) % 16384) + t_start.tv_nsec;
  }

  for (i = 0; i < FILENAME_LENGTH; i++) {
    char a = static_cast<char>((rand_r(&seed) % 26) + 'a');
    str[i] = a;
  }
#if DEBUG_PRINT
  cout << str << endl;
#endif
  return str;
}

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define TOOL_NAME1 STR(TOOL_NAME)

int main(int argc, char **argv) {
  int ret;
  int i;

  if (argc < 2) {
    return EXIT_FAILURE;
  }
  int is_decode = IS_DEC;
  int idx = 1;
  string script_file = string(argv[idx++]);

#if DEBUG_PRINT
  cout << "script: " << script_file << endl;
#endif

  if (is_decode != 0) {
    string tmp_file = generate_random_filename();
    string new_script_file = "/tmp/" + tmp_file + ".dat";

    //////////////////////////////////////////////
    // ZP: 20150911  to avoid filename conflicts
    int count_name = 0;
    while (true) {
      std::fstream tmpfile(new_script_file.c_str());
      count_name++;
      char count_name_tmp[100];
      snprintf(count_name_tmp, sizeof(count_name_tmp), "%d", count_name);
      if (tmpfile) {
        // printf ("ok\n");
        new_script_file = "/tmp/" + tmp_file + count_name_tmp + ".dat";
      } else {
        break;
      }
    }
    //////////////////////////////////////////////

    snprintf(str, sizeof(str), "cp %s %s", script_file.c_str(),
             new_script_file.c_str());
    int ret = system(str);
    if (ret != 0) {
      fprintf(stderr, "ERROR: /tmp is not writable.\n");
      exit(EXIT_FAILURE);
    }

    script_file = new_script_file;
#if DEBUG_PRINT
    cout << "script: " << script_file << endl;
#endif

    snprintf(str, sizeof(str), "script_dec %s %s", key, script_file.c_str());
    ret = system(str);
    if (ret != 0) {
      fprintf(stderr, "ERROR: script_dec failed.\n");
      exit(EXIT_FAILURE);
    }
  }

  string cmd = string(TOOL_NAME1) + " " + script_file;

  for (i = idx; i < argc; i++) {
    cmd += " \"" + string(argv[i]) + "\" ";
  }
  // printf("cmd = %s\n", cmd.c_str());
  int cmd_ret = system(cmd.c_str());
  if (cmd_ret != 0) {
    // ZP: 20160703
    // fprintf(stderr, "ERROR: %s failed.\n", cmd.c_str());
    exit(EXIT_FAILURE);
  }

  if (is_decode != 0) {
    snprintf(str, sizeof(str), "rm -rf %s", script_file.c_str());
#if DEBUG_PRINT
    cout << "del : " << script_file << endl;
#endif
    ret = system(str);
    if (ret != 0) {
      exit(EXIT_FAILURE);
    }
  }

  return EXIT_SUCCESS;
}
