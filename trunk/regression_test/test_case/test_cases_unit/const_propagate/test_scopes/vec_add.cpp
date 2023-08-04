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
#define SIZE 10

#pragma ACCEL kernel
void top() {
  int i = 10;
  int ii = 10;
  int j = 0;
  int k1;
  int a[100];
  int inc1, inc2, inc3 = 3, inc4, inc5 = 3;

  for (int i = 0, inc = 0; i < 100; ++i) {
    a[i] = i + inc;
    for (int j = 0, inc = 5; j < 100; ++j)
      a[j] = j + inc;
  }

  int inc = 0;
  {
    int inc1 = inc;
    for (inc = 1; ii < 10; ++ii) {
      a[ii] = inc;
      inc++;
    }
  }
  {
    int inc = 0;
    int inc1 = inc;
    for (; ii < 10; ++ii) {
      a[ii] = inc;
    }
  }

  int a1[10] = {0};

  {
    int inc1 = inc;
    for (inc4 = 1; ii < 10; ++ii) {
      a[ii] = inc4;
    }
    do {
      inc1++;
    } while (0);

    for (; ii < 10; --ii)
      a[ii] = inc4 + inc5;
  }

  {
    int inc1 = 99;
    int k = inc1;
    int b = 100;
    int b1 = 100;

    while (b1, inc3 = 1) {
      b1 -= inc3;
      ;
    }
  }
  k1 = inc3;
}

int main() {
  int a = 1;
  int b = 1;
  int c = 1;

  top();
  return 0;
}
