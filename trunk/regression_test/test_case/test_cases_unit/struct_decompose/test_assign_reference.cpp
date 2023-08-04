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
  char b;
} st;
void sub(st &a, const st &tmp) {
   a = tmp;
}
void sub_0(st &a, st tmp) {
   a = tmp;
}
void sub_1(st &a, const st tmp) {
   a = tmp;
}
void sub_2(st &a, st &tmp) {
   a = tmp;
}
#pragma ACCEL kernel
void top(st *a) {
#pragma ACCEL interface variable=a depth=10
  st tmp;
  sub(a[0], tmp);
  sub_0(a[1], tmp);
  sub_1(a[2], tmp);
  sub_2(a[3], tmp);
}
