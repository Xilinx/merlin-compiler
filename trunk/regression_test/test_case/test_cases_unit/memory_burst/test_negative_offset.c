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

void sub(int *a, int i) {
  int j = 0; 
  for (j = 0; j < 10000; j++) {
     a[i * 10000 + j] = 0;
  }
}
#pragma ACCEL kernel
void top(int *a) {
  int i;
  for (i = 0; i < 100; ++i) {
    sub(a, i);
    sub(a, i);
  }
}

void sub_0_0(int *a, int i) {
  int j = 0; 
  for (j = 0; j < 10000; j++) {
     a[i * 10000 + j] = 0;
  }
}
void sub_0(int *a, int i) {
  sub_0_0(a, i);
}
#pragma ACCEL kernel
void top_0(int *a) {
  int i;
  for (i = 0; i < 100; ++i) {
    sub_0(a, i);
  }
}

void sub_2(int *a) {
  int j = 0; 
  for (j = 0; j < 10000; j++) {
     a[j] = 0;
  }
}
#pragma ACCEL kernel
void top_2(int *a, int *b) {
  int i;
  for (i = 0; i < 100; ++i) {
    sub_2(&a[i * 10000]);
  }
}
