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
// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes
#include<string.h>
#pragma ACCEL kernel
void func_top_0(int *a,int *b)
{
  int b_buf[100];
#pragma ACCEL interface variable=a 
  for (int i0 = 0; i0 <  10; i0++) {
    for (int i1 = 0; i1 < 10; i1++) {
      int i = i0 * 10 + i1;
      b_buf[i] = b[i];
    }
  }
  for (int i0 = 0; i0 <  10; i0++) {
    for (int i1 = 0; i1 < 10; i1++) {
      int i = i0 * 10 + i1;
      a[i] = b_buf[i];
    }
  }
}

// sub function

void sub_0(int a[100],int b[100], int b_buf[100])
{
  int i;
  int j;
  for (i = 0; i <  10; i++) {
    if (a[0 *  10 + 0]) {
      for (j = 0; j <  10; j++) {
        
#pragma ACCEL pipeline
        b_buf[i *  10 + j] = b[i *  10 + j];
      }
    }
  }
  a[0] = b_buf[0];
}

#pragma ACCEL kernel
void func_top_9(int a[100],int b[100])
{
#pragma ACCEL interface variable=a 
  int b_buf[100];
  sub_0(a,b, b_buf);
}

void sub_1(int a[100],int b[100])
{
  int i;
  int j;
  int b_buf[100];
  memcpy(b_buf, b, sizeof(int) * 100);
  for (i = 0; i <  10; i++) {
    if (a[0 *  10 + 0]) {
      for (j = 0; j <  10; j++) {
        a[i *  10 + j] = b_buf[i *  10 + j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_11(int a[100],int b[100])
{
#pragma ACCEL interface variable=a max_burst_size=10
  sub_1(a,b);
  sub_1(a,b);
}

//manual burst results in inefficent burst
#pragma ACCEL kernel
void func_top_2(int a[100]) {
  int a_buf[10];
  for (int i = 0; i < 10; ++i) {
    a_buf[i] = a[i];
  }
  for (int i = 0; i < 100;++i) {
    a[i] *= 2;
  }
  for (int i = 0; i < 100;++i) {
    a[i] = a[i] + a_buf[i % 10];
  }
}

int main()
{
  int *a;
  int *b;
  int *c;
  int *aa;
  int *bb;
  int *cc;
  int a_s;
  int b_s;
  int c_s;
  

  func_top_0(a, b);
  

  func_top_1(a, b);
 
  func_top_2(a);

  func_top_9(aa, bb);
  

  func_top_11(aa, bb);

  return 0;
}
