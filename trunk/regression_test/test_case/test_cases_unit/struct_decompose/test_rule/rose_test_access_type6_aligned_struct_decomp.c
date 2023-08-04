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
  int y;
}
;

struct rect 
{
  struct point p1;
  struct point p2;
}
;
#pragma ACCEL kernel

void top(struct rect *a)
{
  
#pragma ACCEL interface variable=a depth=100
  int i;
  struct rect *p;
  for (i = 0; i < 100 / 2; ++i) {
    a[i] . p1 . x = a[i] . p2 . x;
    a[i] . p1 . y = a[i] . p2 . y;
  }
  for (i = 100 / 2; i < 100; ++i) {
    p = &a[i];
    p -> p1 . x = p -> p2 . x;
    p -> p1 . y = p -> p2 . y;
  }
  return ;
}

int main()
{
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct rect"
  int i;
  struct rect *a;
  struct point p = {(0), (0)};
  a = ((struct rect *)(malloc(sizeof(struct rect ) * 100)));
  for (i = 0; i < 100; ++i) {
    a[i] . p1 = p;
    a[i] . p2 = p;
  }
  top(a);
  return 0;
}
