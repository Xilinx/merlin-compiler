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
void sub_func_1(int *a, int *b) {
  __merlin_access_range(a, 0, 15);
  a[b[0]] = 0;
}

void func_top_1(int *a, int *b, int i) {

  sub_func_1(a, b);

  a[i] = 0;
}

void sub_func_2(int *a) {
  int i;
  while ( i < 15)
    a[i++] = i;
}

void func_top_2(int *a) {
  __merlin_access_range(a, 0, 1500);
  int i;
  for (i = 0; i < 100; ++i) {
   __merlin_access_range(a, i * 15, i * 15 + 15);

   sub_func_2(a + i * 15);
  }
}

void sub_func_3(int *a_sub) {
  __merlin_access_range(a_sub, 0, 15);
  int i;
  while ( i < 15)
    a_sub[i++] = i;
}

void func_top_3(int *a) {
  int i;
  for (i = 0; i < 100; ++i) {

   sub_func_3(a + i * 15);
  }
}
