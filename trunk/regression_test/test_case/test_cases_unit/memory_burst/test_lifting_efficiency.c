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

#pragma ACCEL kernel
void func_top_0(int *a,int *b)
{
  int i;
  for (i = 100; i >= 0; i--) {
    
#pragma ACCEL pipeline
    a[i * 152] = b[i * 162];
  }
}

#pragma ACCEL kernel
void func_top_1(int *a, int *b)
{
  int i;
  int j;
  for (i = 0; i <= 10; i++) {
    for (j = 0; j <= 10; j++) {
      
#pragma ACCEL pipeline
      a[i * 20] = b[i* 2000];
    }
  }
}

void sub_func_2(int *a, int *b) {
  a[0] = b [0];
}

#pragma ACCEL kernel
void func_top_2(int *a, int *b)
{
  int i;
  int j;
  for (i = 0; i <= 100; i++) {
#pragma ACCEL pipeline
     sub_func_2(a + i * 150, b + i * 161);
  }
}

void sub_func_3(int *a, int *b) {
  int j;
  for (j = 0; j <= 10; j++) {
      
#pragma ACCEL pipeline
      a[0] = b[0];
    }
}
#pragma ACCEL kernel
void func_top_3(int *a, int *b)
{
  int i;
  int j;
  for (i = 0; i <= 10; i++) {
  //expected burst for 'b' here, so that data access is paralleled with
  //computation
    sub_func_3(a + i * 1000, b + i * 2000);
  //expected burst for 'a' here
  }
}


//Loop structure 
#pragma ACCEL kernel
void func_top_4(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i <= 10; i++) {
    for (j = 0; j <= 10; j++) {
      
#pragma ACCEL pipeline
      a[i* 128 + j] = 0;
    }
    for (j = 0; j <= 10; j++) {
      
#pragma ACCEL pipeline
      b[(i + 1) * 1024 + j] = 1;
    }
  }
}

//Loop with func call

void sub_5(char *a, int v) {
  for (int i = 0; i < 64; ++i) {
    a[i] = i + v;
  }
}

#pragma ACCEL kernel
void func_top_5(char *a) {
  for (int i = 0; i < 1024; ++i) {
    int a_0 = a[i * 256 + 256];
    int a_1 = a[i * 256 + 156];
    int a_2 = a[i * 256 + 56];
    int a_3 = a[i * 256 + 6];
    int a_4 = a[i * 256 + 0];
    int a_5 = a[i * 256 + 200];
    sub_5(a + i * 256, a_0 + a_1 + a_2 + a_3 + a_4 + a_5);
  }
}

int main()
{
  int *a;
  char *a_c;
  int *b;
  int *c;
  int a_s;
  int b_s;
  int c_s;

  func_top_0(a, b);

  func_top_1(a, b);
  

  func_top_2(a, b);


  func_top_3(a, b);


  func_top_4(a, b);


  func_top_5(a_c);

  return 0;
}

