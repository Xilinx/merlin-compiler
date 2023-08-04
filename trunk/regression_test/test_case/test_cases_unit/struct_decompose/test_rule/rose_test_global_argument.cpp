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

struct st 
{
  int a[12];
}
;

struct st2 
{
  int *a;
  int b;
  
#pragma ACCEL attribute variable=a depth=b max_depth=10
}
;

struct st3 
{
  int *a[7];
  
#pragma ACCEL attribute variable=a depth=,3
}
;
#pragma ACCEL kernel

void top(int (*a_a)[10][11][12],int (*b_a)[8][9][12],int n,int (*c_a[8])[12])
{
  
#pragma ACCEL INTERFACE DEPTH=8,n,12 MAX_DEPTH=8,10,12 VARIABLE=c_a 
  
#pragma ACCEL INTERFACE DEPTH=n,8,9,12 MAX_DEPTH=10,8,9,12 VARIABLE=b_a 
  
#pragma ACCEL INTERFACE DEPTH=1,10,11,12 VARIABLE=a_a max_depth=1,10,11,12
  a_a[0][1][2][3] = 0;
  b_a[0][0][0][2] = 1;
  c_a[0][0][1] = 2;
}
#pragma ACCEL kernel

void top2(int *(*a_a)[10][11],int *(*b_a)[8][9],int n,int **c_a[8])
{
  
#pragma ACCEL INTERFACE DEPTH=8,n,c->a->b MAX_DEPTH=8,10,10 VARIABLE=c_a 
  
#pragma ACCEL INTERFACE DEPTH=n,8,9,b->a->b MAX_DEPTH=10,8,9,10 VARIABLE=b_a 
  
#pragma ACCEL INTERFACE DEPTH=1,10,11,a->a->b VARIABLE=a_a max_depth=1,10,11,10
  a_a[0][1][2][3] = 0;
  b_a[0][0][0][2] = 1;
  c_a[0][0][1] = 2;
}
#pragma ACCEL kernel

void top3(int *(*a_a)[10][11][7],int *(*b_a)[8][9][7],int n,int *(*c_a[8])[7])
{
  
#pragma ACCEL INTERFACE DEPTH=8,n,7,3 MAX_DEPTH=8,10,7,3 VARIABLE=c_a 
  
#pragma ACCEL INTERFACE DEPTH=n,8,9,7,3 MAX_DEPTH=10,8,9,7,3 VARIABLE=b_a 
  
#pragma ACCEL INTERFACE DEPTH=1,10,11,7,3 VARIABLE=a_a max_depth=1,10,11,7,3
  a_a[0][1][2][3][0] = 0;
  b_a[0][0][0][2][1] = 1;
  c_a[0][0][1][2] = 2;
}
