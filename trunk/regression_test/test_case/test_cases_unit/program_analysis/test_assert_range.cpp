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
#include <assert.h>

void f() {
  (bool) 0;
  (bool) 8;
  (bool) -8;

  int a, b;
  assert(a > -8);
  assert(b < 8);
  (bool) a;
  (bool) b;

  int x, y;
  assert(x > 8);
  assert(y < -8);
  (bool) x;
  (bool) y;
}

void f0(int c) {
  assert (c >= 0 && c <= 15);
  for (int i0 = (c+ 1) / 4; i0 < 4; ++i0) {
	i0;
  }
}

void f2(int c) {
  assert (c < 100);
L1:
  for (int i0 = 0; i0 < c; ++i0) {
	i0;
  }
}

void f3(int c) {
  {
	assert(c < 200);
  }
  if (1) {
L1:
  for (int i1 = 0; i1 < c; ++i1) {
	i1;
  }
  }
}
void f4(int c) {
  if (1) {
	assert(c < 100);
L1:
	for (int i2 = 0; i2 < c; ++i2) {
	  i2;
	}
  }
  goto L1;
}
