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
typedef struct st {
int a;
char b;}st;

void sub(int &a_a,char &a_b,const int &tmp_a,const char &tmp_b)
{
  a_a = tmp_a;
  a_b = tmp_b;
}

void sub_0(int &a_a,char &a_b,int tmp_a,char tmp_b)
{
  a_a = tmp_a;
  a_b = tmp_b;
}

void sub_1(int &a_a,char &a_b,const int tmp_a,const char tmp_b)
{
  a_a = tmp_a;
  a_b = tmp_b;
}

void sub_2(int &a_a,char &a_b,int &tmp_a,char &tmp_b)
{
  a_a = tmp_a;
  a_b = tmp_b;
}
#pragma ACCEL kernel

void top(int *a_a,char *a_b)
{
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=a_b max_depth=10
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=a_a max_depth=10
  char tmp_b;
  int tmp_a;
  sub(a_a[0],a_b[0],tmp_a,tmp_b);
  sub_0(a_a[1],a_b[1],tmp_a,tmp_b);
  sub_1(a_a[2],a_b[2],tmp_a,tmp_b);
  sub_2(a_a[3],a_b[3],tmp_a,tmp_b);
}
