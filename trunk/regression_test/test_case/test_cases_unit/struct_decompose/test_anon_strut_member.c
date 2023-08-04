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
#include<stdlib.h>

#define N 100

struct st_parent {
  int a;
  char b;
  struct {
	int a;
	int b;
  } l_c;
  struct {
    int c;
    int d;
  };
};

#pragma ACCEL kernel
void top(struct st_parent *a) {
#pragma ACCEL interface variable=a depth=100
	int i;

  for (i = 0; i < N; ++i) {
    a[i].a = a[i].a + a[i].l_c.a;
	a[i].b = a[i].b + a[i].l_c.b;
    a[i].c = i + 1;
    a[i].d = i * i;
  }
  return;
}

int main() {
	int i;
  struct st_parent *a;
  a = (struct st_parent*) malloc(sizeof(struct st_parent) * N);
  for (i = 0; i < N; ++i) {
    a[i].a = i;
	a[i].b = i + '0';
	a[i].l_c.a = i + 1;
	a[i].l_c.b = i + '1';
  }
  top(a);
  return 0;

}
