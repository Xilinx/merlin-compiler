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
#define N 100

struct point 
{
  int x;
  char y;
}
;

void addpoint(int *ret_var_x,char *ret_var_y,int p1_x,char p1_y,int p2_x,char p2_y)
{
  char p_y;
  int p_x;
  p_x = p1_x + p2_x;
  p_y = ((char )(((int )p1_y) + ((int )p2_y)));
   *ret_var_x = p_x;
   *ret_var_y = p_y;
  return ;
}
#pragma ACCEL kernel

void top(int *a_x,char *a_y)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_y max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_x max_depth=100
  int i;
  char p_y = (char )1;
  int p_x = 1;
  for (i = 0; i < 100; ++i) {
    char ret_var_y;
    int ret_var_x;
    addpoint(&ret_var_x,&ret_var_y,a_x[i],a_y[i],p_x,p_y);
    a_x[i] = ret_var_x;
    a_y[i] = ret_var_y;
  }
  return ;
}

int main()
{
  char *a_y;
  int *a_x;
  
#pragma ACCEL wrapper VARIABLE=a->x port=a_x data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->y port=a_y data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct point"
  int i;
  struct point *a;
  a = ((struct point *)(malloc(sizeof(struct point ) * 100)));
  for (i = 0; i < 100; ++i) {
    a[i] . x = i;
    a[i] . y = i;
  }
  top(a_x,a_y);
  return 0;
}
