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
#pragma ACCEL kernel
void top(int *a, int *b, int *c) {
#pragma ACCEl interface variable=a depth=1000
#pragma ACCEl interface variable=b depth=1000
#pragma ACCEl interface variable=c depth=1000
  for (int i = 0; i < 10; ++i) {
    int *ptr;
    if (i % 2) {
      ptr = a;
    } else {
      ptr = b;
    }
    //memcpy(buf, &ptr[i * 100], sizeof(int) * 100);
    for (int j = 0; j < 100; ++j) {
      c[i * 100 + j] = ptr[i * 100 + j] + 2;
    }
  }
  static int a_buf[100];
  for (int i = 0; i < 10; ++i) {
    int *ptr;
    if (i % 2) {
      ptr = a;
    } else {
      ptr = a_buf;
    }
    for (int j = 0; j < 100; ++j) {
      c[i * 100 + j] = ptr[i * 100 + j] + 3;
    }
  }
}
