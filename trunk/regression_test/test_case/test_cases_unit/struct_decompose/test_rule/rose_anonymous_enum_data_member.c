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
#include<stdlib.h>
#include "t.h"
#define N 100
static void __merlin_dummy_g();
const AN_EM g_c[10];
const char g_b[10];
const int g_a[10];
static void __merlin_dummy_g_2();
AN_EM g_2_c[10];
char g_2_b[10];
int g_2_a[10];
static void __merlin_dummy_g_3();
AN_EM *g_3_c;
char *g_3_b;
int *g_3_a;
#pragma ACCEL kernel

void top(int *a_a,char *a_b,AN_EM *a_c)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_c max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=100
  for (int i = 0; i < 100; ++i) 
    a_b[i] = ((char )(((unsigned int )a_a[i]) + ((unsigned int )a_c[i])));
  a_a[0] = g_a[0];
  a_b[0] = g_b[0];
  a_c[0] = g_c[0];
  a_a[1] = g_2_a[1];
  a_b[1] = g_2_b[1];
  a_c[1] = g_2_c[1];
  a_a[2] = g_3_a[2];
  a_b[2] = g_3_b[2];
  a_c[2] = g_3_c[2];
  return ;
}

int main()
{
  AN_EM *a_c;
  char *a_b;
  int *a_a;
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a->c port=a_c data_type=AN_EM
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type=st
  st *a;
  a = ((st *)(malloc(sizeof(st ) * 100)));
  for (int i = 0; i < 100; ++i) {
    a[i] . a = i;
    a[i] . c = INT;
  }
  top(a_a,a_b,a_c);
  return 0;
}
