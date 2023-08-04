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
void foo(int a[10], int *b, int *c, int *d) {
  int index = 0;
  for (int i = 0; i < 10; ++i) { 
    if (a[i] > 0) {
    index  = i;
    }
  }
  b[index];
  index = index;
  c[index];
  index = index + 1;
  d[index];
}

void foo2(int a[10], int *b, int *d) {
  int index = 0;
  for (int i = 0; i < 10; ++i) { 
    if (a[i] > 0) {
    index  = i;
    }
  }
  b[index];
  index = index + 1;
  d[index];
}
  
void foo3(int a[10], int n, int *b, int *d) {
  int index = 0;
  for (int i = 0; i < 10; ++i) { 
    if (a[i] > 0) {
    index  = n;
    }
  }
  b[index];
  
  int i0 = 0;
  for (int j = 0; j < 10; ++j) {
    char tmp;
    if (j) {
      tmp = i0;
    } else {
      tmp = i0 + 1;
    }
    i0 = tmp;
  }
  
  d[i0];
}

void foo4_0(int *a, int i) {
  a[i] = 0;
}

void foo4_1(int *a, int j) {
  a[j] = 0;
}
void foo4(int a[10]) {
  foo4_0(a, 2);
  foo4_0(a, 3);
  foo4_1(a, 4);
}

void foo5(int *a) {
  int i;
  if (a[0]) {
    i = 10;
  }
  a[i];
}

void foo6(int *a) {
  int i, j;
  while (i < j) {
    j = i;
    i = j++;
  }
  a[i];
  a[j];
}
