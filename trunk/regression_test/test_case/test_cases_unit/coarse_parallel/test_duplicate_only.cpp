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
#include<string.h>
#define FACTOR 100
void func(int *a, int *b, int *c) {
  for (int i = 0; i < 10000/FACTOR; ++i) {
    c[i] = a[i] + b[i];
  }
}

#pragma ACCEL kernel
void top(int *a, int *b, int *c) {
  int a_buf[10000 / FACTOR];
  memcpy(a_buf, a, sizeof(int) * 10000 / FACTOR);
  int b_buf[FACTOR][10000/FACTOR];
  memcpy(b_buf, b, sizeof(int) * 10000);
  int c_buf[FACTOR][10000/FACTOR];
  for (int j = 0; j < FACTOR; ++j)  {
#pragma ACCEL parallel  
      func(a_buf, b_buf[j], c_buf[j]);
  }
  memcpy(c_buf, c, sizeof(int) * 10000);
}


