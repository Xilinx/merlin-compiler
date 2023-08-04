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
struct S {
  const int *cp;
  int *p;
  const int x = 128;
  int y;
  volatile int z;
  S() : x(168) {}
};

typedef S I;
typedef const S (*C0)[100][100];
typedef const S (*C1[100])[100];
typedef const S *C2[100][100];
typedef const S (*&CR0)[100][100];
typedef const S (*(&CR1)[100])[100];
typedef const S *(&CR2)[100][100];

typedef S SS;

bool ff(const int *p) {
  return true;
}

bool ff(int *p) {
  return false;
}

#pragma ACCEL kernel
int f() {
  I i;
  i.z;
  ff(i.p);
  ff(i.cp);

  SS s;
  const S (*ci0)[100][100];
  const S (*ci1[100])[100];
  const S *ci2[100][100];
  C0 c0;
  C1 c1;
  C2 c2;
  CR0 cr0 = c0;
  CR1 cr1 = c1;
  CR2 cr2 = c2;
}
