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

struct rect 
{
  struct point p1;
  struct point p2;
}
;
#pragma ACCEL kernel

void top(int *a_p1_x,char *a_p1_y,int *a_p2_x,char *a_p2_y)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_p2_y max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_p2_x max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_p1_y max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_p1_x max_depth=100
  int i;
  char *p_p2_y;
  int *p_p2_x;
  char *p_p1_y;
  int *p_p1_x;
  for (i = 0; i < 100 / 2; ++i) {
    a_p1_x[i] = a_p2_x[i];
    a_p1_y[i] = a_p2_y[i];
  }
  for (i = 100 / 2; i < 100; ++i) {
    p_p1_x = &a_p1_x[i];
    p_p1_y = &a_p1_y[i];
    p_p2_x = &a_p2_x[i];
    p_p2_y = &a_p2_y[i];
     *p_p1_x =  *p_p2_x;
     *p_p1_y =  *p_p2_y;
  }
  return ;
}

int main()
{
  char *a_p2_y;
  int *a_p2_x;
  char *a_p1_y;
  int *a_p1_x;
  
#pragma ACCEL wrapper VARIABLE=a->p1->x port=a_p1_x data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->p1->y port=a_p1_y data_type=char
  
#pragma ACCEL wrapper VARIABLE=a->p2->x port=a_p2_x data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->p2->y port=a_p2_y data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct rect"
  int i;
  struct rect *a;
  struct point p = {(0), (0)};
  a = ((struct rect *)(malloc(sizeof(struct rect ) * 100)));
  for (i = 0; i < 100; ++i) {
    a[i] . p1 = p;
    a[i] . p2 = p;
  }
  top(a_p1_x,a_p1_y,a_p2_x,a_p2_y);
  return 0;
}
