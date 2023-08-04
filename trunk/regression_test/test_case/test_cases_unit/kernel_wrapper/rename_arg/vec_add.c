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
#define VEC_SIZE 10000
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
void vec_add_kernel(int *vec_add_kernel,int *b,int *c,int inc);
extern int __merlin_include__LB___merlinhead_kernel_top_h_1;
#include "__merlinhead_kernel_top.h"
extern int __merlin_include__LE___merlinhead_kernel_top_h_1;

int main()
{
  
#pragma ACCEL string __merlin_init("kernel_top.aocx");
  int i;
  int inc = 0;
  int a[10000];
  int b[10000];
  int c[10000];
  for (i = 0; i < 10000; i++) {
    a[i] = i;
    b[i] = i * 2;
    c[i] = 0;
  }
  __merlin_vec_add_kernel(a,b,c,inc);
  int cnt = 0;
  for (i = 0; i < 10000; i++) {
    if (c[i] != a[i] + b[i]) {
      cnt++;
    }
  }
  if (cnt == 0) {
    printf("SUCCESS\n");
    char file_name_s[256] = "pass.o";
    struct _IO_FILE *fp_s = fopen(file_name_s,"ab+");
    if (fp_s == ((void *)0)) 
      printf("can't create file!");
    fclose(fp_s);
  }
   else {
    printf("FAIL\n");
    printf("%d\n",cnt);
    char file_name_f[256] = "fail.o";
    struct _IO_FILE *fp_f = fopen(file_name_f,"ab+");
    if (fp_f == ((void *)0)) 
      printf("can't create file!");
    fclose(fp_f);
  }
  return 0;
}
