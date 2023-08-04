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
// Testing NULL loop initial/testing/increment
#pragma ACCEL kernel

void func_loop(float fa,float fb)
{
  int a[10];
  int b[10];
  int c[10];
  int i = 0;
  int j = 0;
  int x;
  int y;
  int z;
  int N = 100;
  for (i = 0; ; i++) {
/* block 1*/
    c[i] = a[i] + b[i];
    if (i < 100) {
      break; 
    }
  }
  for (i = 0; ; ) {
/* block 2*/
    c[i] = a[i] + b[i];
    i++;
  }
  for (; i < 100; i++) {
/* block 3*/
    c[i] = a[i] + b[i];
  }
  for (; ; i++) {
/* block 4*/
    c[i] = a[i] + b[i];
    if (i < 100) {
      break; 
    }
  }
  for (i = 0; ; ) {
/* block 5*/
    c[i] = a[i] + b[i];
    if (i < 100) {
      break; 
    }
    i++;
  }
  for (; i < 100; ) {
/* block 6*/
    c[i] = a[i] + b[i];
    i++;
  }
  for (; ; ) {
/* block 7*/
    c[i] = a[i] + b[i];
    if (i < 100) {
      break; 
    }
    i++;
  }
  for (i = 0; i < 10; i++) {
    i++;
  }
  goto L0;
  for (i = 0; i < 10; i++) {
    L0:;
  }
  int sum = 0;
  short i0 = 0;
  int _s_ii = 0;
  char _l_ii = 0;
  
#pragma unroll
// Canonicalized from: for(char ii = 0;_s_ii < 10;_s_ii++) {...}
  for (_s_ii = ((char )0); _s_ii <= '\t'; ++_s_ii) {
    sum++;
  }
  
#pragma unroll
  for (i0 = 0; i0 < 10; i0++) {
    sum++;
  }
  int _s_i_3 = 0;
  char _l_i_3 = 0;
// Canonicalized from: for(char i = 0;_s_i_3 < 10;_s_i_3++) {...}
  for (_s_i_3 = ((char )0); _s_i_3 <= '\t'; ++_s_i_3) 
// Original: #pragma ACCEL parallel
{
    
#pragma ACCEL PARALLEL COMPLETE 
    sum++;
  }
  int _s_i_2 = 0;
  char _l_i_2 = 0;
// Canonicalized from: for(char i = 0;_s_i_2 < 10;_s_i_2++) {...}
  for (_s_i_2 = ((char )0); _s_i_2 <= '\t'; ++_s_i_2) {
    if ((bool )(((int )_s_i_2) % 2)) {
    }
     else {
    }
  }
  int _s_i_1 = 0;
  char _l_i_1 = 0;
// Canonicalized from: for(char i = 0;_s_i_1 < 10;_s_i_1++) {...}
  for (_s_i_1 = ((char )0); _s_i_1 <= '\t'; ++_s_i_1) {
    if ((bool )(((int )_s_i_1) % 2)) {
    }
     else {
      break; 
    }
  }
  for (char i = 0; i < 10; i++) {
    if ((i % 2)) {
      continue; 
    }
    i++;
  }
  int _s_i_0 = 0;
  char _l_i_0 = 0;
// Canonicalized from: for(char i = 0;_s_i_0 < 10;_s_i_0++) {...}
  for (_s_i_0 = ((char )0); _s_i_0 <= '\t'; ++_s_i_0) {
    if ((bool )(((int )_s_i_0) % 2)) {
    }
  }
  int _s_i = 0;
  char _l_i = 0;
// Canonicalized from: for(char i = 0;_s_i < 10;_s_i++) {...}
  for (_s_i = ((char )0); _s_i <= '\t'; ++_s_i) {
    if ((bool )(((int )_s_i) % 2)) {
      if (((int )_s_i) == 2) {
      }
    }
  }
  for (; fa > fb; ) {
    c[0]++;
  }
}
