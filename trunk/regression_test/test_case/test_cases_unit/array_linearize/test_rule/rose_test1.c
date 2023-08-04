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
// 1. combination of * and []
// 2. alias
// 3. conversion
//
#pragma ACCEL kernel

void kernel_top(int a1[100],int a2[100],int a3[100],int a4[100])
{
  
#pragma ACCEL interface variable=a2 depth=10,10 max_depth=10,10
  
#pragma ACCEL interface variable=a1 depth=10,10 max_depth=10,10
  
#pragma ACCEL interface variable=a3 depth=10,10 max_depth=10,10
  
#pragma ACCEL interface variable=a4 depth=10,10 max_depth=10,10
  int i;
  int *p1 = a1;
  p1[0] = 0;
  a2[0] = 0;
  int *p3[10];
  for (i = 0; i < 10; i++) 
    p3[i] = &a3[i * 10L];
  p3[0][0] = 0;
  int *p4 = a4;
  p4[0] = 0;
  int *b1 = a1;
  b1[0];
  int *b2 = a2;
  b2[0];
  int *b3 = a3;
  b3[0];
  int *b4 = a4;
  b4[0];
  int *c1 = a1;
  c1[0];
  int *c2 = a2;
  c2[0];
  int *c3 = a3;
  c3[0];
  int *c4 = a4;
  c4[0];
  int *d1 = a1;
  d1[0];
  int *d2 = a2;
  d2[0];
  int *d3 = a3;
  d3[0];
  int *d4 = a4;
  d4[0];
  int *e1 = a1;
  e1[0];
  int *e2 = a2;
  e2[0];
  int *e3 = a3;
  e3[0];
  int *e4 = a4;
  e4[0];
  int *f1 = a1;
  f1[0];
  int *f2 = a2;
  f2[0];
  int *f3 = a3;
  f3[0];
  int *f4 = a4;
  f4[0];
//    int * g1[10] = a1; g1[0][0];  // this code can not pass gcc, but can pass ROSE
//    int * g2[10] = a2; g2[0][0];
//    int * g3[10] = a3; g3[0][0];
//    int * g4[10] = a4; g4[0][0];
}
