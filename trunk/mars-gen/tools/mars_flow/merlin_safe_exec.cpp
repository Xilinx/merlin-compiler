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


#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include "stdio.h"
#include "stdlib.h"

#include "file_parser.h"

// #define DEBUG
#define NUM_THREADS 5
const char *key = "falconisgreat";
int wait_time = 60;
static int do_not_encrypt_back = 0;

void *exec_cmd1(void *cmd_string) {
  std::string &cmd = *(static_cast<std::string *>(cmd_string));
  const char *char_s = cmd.c_str();
  std::string str = std::string(char_s);
  system("rm -rf .finish_cmd");
#ifdef DEBUG
  printf("start cmd = %s\n", str.c_str());
  fflush(stdout);
#endif
  system(str.c_str());
#ifdef DEBUG
  printf("finish cmd = %s\n", str.c_str());
  fflush(stdout);
#endif
  system("touch .finish_cmd;");
#ifdef DEBUG
  printf("touch file\n");
  fflush(stdout);
#endif
  pthread_exit(nullptr);
}

void *exec_cmd2(void *cmd_string) {
  // check file list
  std::string &file_list_s = *(static_cast<std::string *>(cmd_string));
  // #ifdef DEBUG
  //   printf("get interrupt, file_string_org = %s\n", file_list_s.c_str());
  //   fflush(stdout);
  // #endif
  // check file list
  for (int i = 0; i < wait_time; i++) {
    int end_flag = 0;
    sleep(1);
    // #ifdef DEBUG
    //     printf("sleep = %d\n", i);
    //     fflush(stdout);
    //     printf("get interrupt, file_string_org 2 = %s\n",
    //     file_list_s.c_str()); fflush(stdout);
    // #endif
    if (access(".finish_read_files", F_OK) != -1 ||
        access(".finish_cmd", F_OK) != -1 || i == wait_time - 1) {
      // #ifdef DEBUG
      //       printf("get interrupt, file_string_org 3 = %s\n",
      //       file_list_s.c_str()); fflush(stdout);
      // #endif
      const char *char_s = file_list_s.c_str();
      // #ifdef DEBUG
      //       printf("get interrupt, file_string 1 = %s %s\n",
      //       file_list_s.c_str(),
      //              char_s);
      //       fflush(stdout);
      // #endif
      std::string str = std::string(char_s);
      // #ifdef DEBUG
      //       printf("get interrupt, file_string 2 = %s\n", str.c_str());
      //       fflush(stdout);
      // #endif
      vector<std::string> vec_s = str_split(str, " ");
      for (auto &one_file : vec_s) {
        if (one_file != " " && !one_file.empty()) {
          if (FILE *file = fopen(one_file.c_str(), "r")) {
            fclose(file);
            std::string dec_cmd =
                std::string("script_enc ") + key + " " + one_file;
            if (do_not_encrypt_back == 0) {
              system(dec_cmd.c_str());
            }
#ifdef DEBUG
            printf("enc one file = %s\n", one_file.c_str());
            fflush(stdout);
#endif
          } else {
            printf("ERROR: File %s not exist\n", one_file.c_str());
          }
          end_flag = 1;
        }
      }
      system("rm -rf .finish_cmd");
      system("rm -rf .finish_read_files");
      if (end_flag != 0) {
#ifdef DEBUG
        printf("unsafe time = %d\n", i + 1);
        fflush(stdout);
#endif
        break;
      }
    }
  }
  return nullptr;
}

bool is_number(const std::string &s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && (std::isdigit(*it) != 0)) {
    ++it;
  }
  return !s.empty() && it == s.end();
}

void encrypt_back_files(string file_list) {
  std::istringstream buf(file_list);
  std::istream_iterator<std::string> beg(buf);
  std::istream_iterator<std::string> end;
  std::vector<std::string> tokens(beg, end);
  for (auto &one_file : tokens) {
    // std::cout << '"' << one_file << '"' << '\n';
    if (FILE *file = fopen(one_file.c_str(), "r")) {
      fclose(file);
      string dec_cmd = string("script_enc ") + key + " " + one_file;
      system(dec_cmd.c_str());
#ifdef DEBUG
      printf("enc one file = %s\n", one_file.c_str());
      fflush(stdout);
#endif
    } else {
      printf("ERROR: File %s not exist\n", one_file.c_str());
    }
  }
}

int main(int argc, char **argv) {
  /****************************************************************************/
  // parse argument list
  /****************************************************************************/
  if (argc == 1) {
    printf("Correct input argument format: \n");
    printf("    arg1 :: arg2 :: arg3: \n");
    printf("    arg1: commmandline need to run \n");
    printf("    arg2: file list need to encrypt and decrypt \n");
    printf("    arg3: 0 means no encrupt,  >0 mean need to encrypt, the number "
           "is wait time\n");
    return EXIT_SUCCESS;
  }

  // Check if in dse mode
  bool merlin_dse = false;
  char *dse_env = getenv("MERLIN_DSE");
  if (dse_env != nullptr) {
    if (strcmp(dse_env, "ON") == 0) {
      merlin_dse = true;
    }
  }

  int count_arg = 0;
  int count_arg_1 = 0;
  string str_args;
  string arg_1;
  string arg_2;
  string arg_2_org_str;
  string arg_3;
  for (int i = 1; i < argc; i++) {
    str_args += " \"" + string(argv[i]) + "\" ";
    if (std::string(argv[i]) == "::") {
      count_arg++;
      if (count_arg_1 == 1 && count_arg == 1) {
        // if command line is only one argument(wrappered with "" outside), need
        // pure arg
        arg_1 = string(argv[i - 1]);
      }
    } else {
      if (count_arg == 0) {
        arg_1 += " \"" + string(argv[i]) + "\"";
        // if command line are more than one argument, need wrapper with ""
        count_arg_1++;
      } else if (count_arg == 1) {
        arg_2 += " \"" + string(argv[i]) + "\"";
        arg_2_org_str += string(argv[i]) + " ";
      } else if (count_arg == 2) {
        arg_3 += string(argv[i]);
      }
    }
  }
  if (count_arg != 2) {
    printf("ERROR: correct input argument format: \n");
    printf("    arg1 :: arg2 :: arg3 \n");
    return EXIT_FAILURE;
  }
  // #ifdef DEBUG
  //   printf("arg1 = %s\n", arg_1.c_str());
  //   fflush(stdout);
  //   printf("arg2 = %s\n", arg_2.c_str());
  //   fflush(stdout);
  //   printf("arg3 = %s\n", arg_3.c_str());
  //   fflush(stdout);
  // #endif
  /****************************************************************************/
  // prepare command lines
  /****************************************************************************/
  string cmd1 = arg_1;
  string file_list = arg_2_org_str;
  int flag = atoi(arg_3.c_str());
  if (flag < 0 || !is_number(arg_3)) {
    printf("ERROR:  arg3 can only be positive integer.\n");
    return EXIT_FAILURE;
  }
  if (flag > 100) {
    do_not_encrypt_back = 1;
  }
  wait_time = flag;

  // check file list
  std::istringstream buf(file_list);
  std::istream_iterator<std::string> beg(buf);
  std::istream_iterator<std::string> end;
  std::vector<std::string> tokens(beg, end);
  for (auto &one_file : tokens) {
    // std::cout << '"' << one_file << '"' << '\n';
    if (FILE *file = fopen(one_file.c_str(), "r")) {
      fclose(file);
      if (flag > 0) {
        string dec_cmd = string("script_dec ") + key + " " + one_file;
        system(dec_cmd.c_str());
#ifdef DEBUG
        printf("dec one file = %s\n", one_file.c_str());
        fflush(stdout);
#endif
      }
    } else {
      printf("ERROR: File %s not exist\n", one_file.c_str());
      return EXIT_FAILURE;
    }
  }
  string cmd2 = file_list;
  // #ifdef DEBUG
  //   printf("file list = %s\n", cmd2.c_str());
  //   fflush(stdout);
  // #endif
  /****************************************************************************/
  // execute command lines
  /****************************************************************************/
  // if flag equal 1need to encrypt and decrypt and execute with multi thread
  // else just run command
  if (flag > 0 && !merlin_dse) {
#ifdef DEBUG
    printf("Execute in non-dse encryption mode.\n");
    fflush(stdout);
#endif
    pthread_t threads[NUM_THREADS];
    pthread_create(&threads[0], nullptr, exec_cmd1, static_cast<void *>(&cmd1));
    pthread_create(&threads[1], nullptr, exec_cmd2, static_cast<void *>(&cmd2));
    for (int i = 0; i < 2; ++i) {
      pthread_join(threads[i], nullptr);
    }
    pthread_exit(nullptr);
  } else if (flag > 0 && merlin_dse) {
#ifdef DEBUG
    printf("Execute in dse encryption mode.\n");
    fflush(stdout);
#endif
    system(cmd1.c_str());
    encrypt_back_files(file_list);
  } else {
#ifdef DEBUG
    printf("Execute in non-encryption mode.\n");
    fflush(stdout);
#endif
    system(cmd1.c_str());
  }

  return EXIT_SUCCESS;
}
