//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#define WRITE_OUTPUT 
#define CHECK_OUTPUT 
#include "support.h"

int main(int argc,char **argv)
{
// Parse command line.
  char *in_file;
  char *check_file;
  argc < 4 && "Usage: ./benchmark <input_file> <check_file>"?((void )0) : __assert_fail("argc<4 && \"Usage: ./benchmark <input_file> <check_file>\"","harness.c",21,__PRETTY_FUNCTION__);
  in_file = "input.data";
  check_file = "check.data";
  if (argc > 1) 
    in_file = argv[1];
  if (argc > 2) 
    check_file = argv[2];
// Load input data
  int in_fd;
  char *data;
  data = (malloc(INPUT_SIZE));
  data != ((void *)0) && "Out of memory"?((void )0) : __assert_fail("data!=((void *)0) && \"Out of memory\"","harness.c",37,__PRETTY_FUNCTION__);
  in_fd = open(in_file,00);
  in_fd > 0 && "Couldn't open input data file"?((void )0) : __assert_fail("in_fd>0 && \"Couldn't open input data file\"","harness.c",39,__PRETTY_FUNCTION__);
  input_to_data(in_fd,data);
// Unpack and call
  run_benchmark(data);
  int out_fd;
  out_fd = open("output.data",01 | 0100 | 01000,0400 | 0200 | 0400 >> 3 | 0200 >> 3 | 0400 >> 3 >> 3 | 0200 >> 3 >> 3);
  out_fd > 0 && "Couldn't open output data file"?((void )0) : __assert_fail("out_fd>0 && \"Couldn't open output data file\"","harness.c",48,__PRETTY_FUNCTION__);
  data_to_output(out_fd,data);
  close(out_fd);
// Load check data
  int check_fd;
  char *ref;
  ref = (malloc(INPUT_SIZE));
  ref != ((void *)0) && "Out of memory"?((void )0) : __assert_fail("ref!=((void *)0) && \"Out of memory\"","harness.c",58,__PRETTY_FUNCTION__);
  check_fd = open(check_file,00);
  check_fd > 0 && "Couldn't open check data file"?((void )0) : __assert_fail("check_fd>0 && \"Couldn't open check data file\"","harness.c",60,__PRETTY_FUNCTION__);
  output_to_data(check_fd,ref);
// Validate benchmark results
  if (!check_data(data,ref)) {
    fprintf(stderr,"Benchmark results are incorrect\n");
    char file_name[256] = "fail.o";
    struct _IO_FILE *fp = fopen(file_name,"ab+");
    if (fp == ((void *)0)) 
      printf("can't create file!");
    return - 1;
  }
  printf("Success.\n");
  char file_name_s[256] = "pass.o";
  struct _IO_FILE *fp_s = fopen(file_name_s,"ab+");
  if (fp_s == ((void *)0)) 
    printf("can't create file!");
  return 0;
}
