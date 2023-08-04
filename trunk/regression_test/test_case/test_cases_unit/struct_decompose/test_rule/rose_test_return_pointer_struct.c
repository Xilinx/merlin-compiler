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
static void __merlin_dummy_a();
char *a_b[8];
int *a_a[8];

void sub(int **ret_var_a,char **ret_var_b,int *b_a)
{
   *ret_var_a = a_a[ *b_a];
   *ret_var_b = a_b[ *b_a];
  return ;
}
#pragma ACCEL kernel

void top(int *b_a)
{
  
#pragma ACCEL INTERFACE DEPTH=1 VARIABLE=b_a max_depth=1
  char *ret_var_b;
  int *ret_var_a;
  sub(&ret_var_a,&ret_var_b,b_a);
  char *res_b = ret_var_b;
  int *res_a = ret_var_a;
   *res_a = 0;
   *res_b = ((char )1);
}
