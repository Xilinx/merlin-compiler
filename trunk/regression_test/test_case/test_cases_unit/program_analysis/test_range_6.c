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
void test1(int a[100]) {
  int i1;
  for (i1 = 0; i1 < 100;  ++i1) {
    if (i1 > 10) {
      a[i1] = i1;
    }
  }
  return;
}

void test2(int a[100]) {
  int i2, j2;
  for (j2 = 0; j2 < 10; ++j2) {
    for (i2 = 0; i2 < 10;  ++i2) {
      if (j2 > 2)
        a[j2 * 10 + i2] = i2;
    }
  }
  return;
}

void test3(int a[100]) {
  int i3, j3;
  for (j3 = 0; j3 < 10; ++j3) {
    for (i3 = 0; i3 < 10;  ++i3) {
      if (i3 > 2) {
        a[j3 * 10 + i3] = i3;
      }
    }
  }
  return;
}

void test4(int a[100], int n) {
  int i4, j4;
  for (j4 = 0; j4 < 10; ++j4) {
    for (i4 = 0; i4 < 10;  ++i4) {
      if (j4 > n) {
        a[j4 * 10 + i4] = i4;
      }
    }
  }
  return;
}

void test5(int a[100]) {
  int i5, j5;
  for (j5 = 0; j5 < 10; ++j5) {
    for (i5 = 0; i5 < 10;  ++i5) {
      if (j5 == 0) {
        a[j5 * 10 + i5] = i5;
      }
    }
  }
  return;
}
