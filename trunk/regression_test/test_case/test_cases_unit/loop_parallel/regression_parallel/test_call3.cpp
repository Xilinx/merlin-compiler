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
int add_func_1(int a, int b) { return a + b; }

int add_func_2(int a[100], int b[100], int i, int j) { return a[i] + b[j]; }
#pragma ACCEL kernel
void func_top_0() {
  int a[100];
  int b[100];
  int i;
  int j;
  for (i = 0; i < 99; i++) {
    for (j = 0; j < 90; j++) {
#pragma ACCEL PIPELINE II = 1
      a[i] = add_func_1(b[j], b[j + 1]);
      a[i] += add_func_2(b, b, i, i + 1);
    }
  }

  for (i = 0; i < 99; i++) {
#pragma ACCEL PIPELINE II = 1
    for (j = 0; j < 99; j++) {
#pragma ACCEL PARALLEL COMPLETE
      a[i] = add_func_2(b, b, j, j + 1);
    }
    a[99] += add_func_1(b[99], b[98]);
  }
}

int main() {
  func_top_0();
}
