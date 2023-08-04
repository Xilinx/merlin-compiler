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
struct st {
  int a;
  char b;
  char c[12];
};

struct st g;

void sub(struct st *a) {
#pragma HLS array_partition variable=a->c dim=1
}

#pragma ACCEL kernel
void top(struct st a[10], struct st *b, int *c) {
#pragma ACCEL interface variable=a depth=100
#pragma ACCEL interface variable=b depth=100
  struct st tmp;
  sub(&tmp);
  for (int i = 0; i < 12; ++ i) {
    tmp.c[i] = c[i];
  }
  sub(a);
  for (int i = 0; i < N; ++i)
#pragma ACCEL FALSE_DEPENDENCE variable=a
    a[i].b = a[i].a + b[i].b + tmp.c[i % 12] + g.c[i % 12];
  return;
}

int main() {
  struct st *a;
  a = (struct st*) malloc(sizeof(struct st) * N);
  struct st *b;
  b = (struct st*) malloc(sizeof(struct st) * N);
  int *c = (int *) malloc(sizeof(int) * 12);
  for (int i = 0; i < N; ++i)
    a[i].a = i;
  top(a, b, c);
  free(a);
  free(b);
  free(c);
  return 0;

}
