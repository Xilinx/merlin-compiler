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
typedef struct st2 {
int e;
int *f;
int g[1];}st2;

struct st 
{
  int a;
  int b;
  int *c;
  st2 *d;
  st2 h;
};

struct st g_a = {1, 2, 0, 0, {3, 0, {4}}};
#pragma ACCEL kernel
void top(struct st *a) {
#pragma ACCEL interface variable=a depth=2
  struct st tmp = a[0];
  struct st *ptr = a;
  a[0] = tmp;
  struct st tmp2 = {1, 2, 0, 0, {3, 0, {4}}};
  a[1] = tmp2;
  a[0] = g_a;
  ptr->a++;
  ptr->h.e++;

}
