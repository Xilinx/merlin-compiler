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
  int k;
  int a[100];
  int inc1 = 1, inc2, inc3, inc5 = 2;

  for (int i = 0, inc = 0; i < 100; ++i)
    a[i] = i + inc;

  for (int inc = 1; ii < 10; ++ii)
    a[ii] = inc;

  for (int inc = 1; ii < 10; ++ii) {
    a[ii] = inc;
    inc++;
  }

  int inc4;
  for (inc4 = 1; ii < 10; ++ii) {
    a[ii] = inc4;
  }
  for (; ii < 10; --ii, inc1 = 1)
    a[ii] = inc1;

  for (; ii < 10; --ii, inc5)
    a[ii] = inc5;

  for (; ii < 10, inc2 = 1; --ii)
    a[ii] = inc2;

  k = inc1;
  int k1 = inc2;
  int b = 100;
  int b1 = 100;

  while (b) {
    b--;
  }

  while (b1, inc3 = 1) {
    b1 -= inc3;
    ;
  }
  k = b;
  k1 = b1;

  int x = 0, x1;
  do {
    x = x + 1;
  } while (0);

  do {
    x = x + 1;
  } while (0 && (x1 = 0));

  k = x + x1;
  {
    int x2;
    do {
      x = x + 1;
    } while (0, x2 = 0);

    k = x2;
  }
  {
    int x2;
    do {
      x = x + 1;
    } while (0 || (x2 = 0));

    k = x2;
  }
}

int main() {
  int a = 1;
  int b = 1;
  int c = 1;

  top();
  return 0;
}
