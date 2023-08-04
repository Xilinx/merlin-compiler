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
void func_top_0(int *a)
{
  int i0, i1, i2;
  for (i0 = 0; i0 < 1000; ++i0) {
    for (i1 = 0; i1 < 1000; ++i1) {
#pragma ACCEL TILED_TAG
      for (i2 = 0; i2 < 1000; ++i2) {
        int i = i0 * 1000000 + i1 * 1000 + i2;
        a[i] = i;
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_1(int *b)
{
  int i0, i1, i2;
  for (i0 = 0; i0 < 10000; ++i0) {
    for (i1 = 0; i1 < 10; ++i1) {
#pragma ACCEL TILED_TAG
      for (i2 = 0; i2 < 10; ++i2) {
        int i = i0 * 100 + i1 * 10 + i2;
        b[i] = i;
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_2(int *b)
{
  int i0, i1, i2;
  for (i0 = 0; i0 < 10000; ++i0) {
    for (i1 = 0; i1 < 10; ++i1) {
#pragma ACCEL TILED_TAG
      for (i2 = 0; i2 < 10; ++i2) {
        int i = i0 * 100 + i1 * 10 + i2;
        b[i + 1000000];
        b[i + 2000000];

      }
    }
  }
}
