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

#define N 2

struct st {
  int a[2];
  char b;
};


struct st g[N] = {{{1,2}, 'a'}, {{3, 4}, 'b'}};

#pragma ACCEL kernel
void top(struct st *a) {
#pragma ACCEL interface variable=a depth=100
	int i, j;
  struct st f[N] = {{{5,6}, 'c'}, {{7, 8}, 'd'}}; 
  for (i = 0; i < N; ++i) {
    for (j = 0; j < 2; ++j)
    a[i].a[j] = a[i].a[j] + g[i].a[j] + g[i].b + f[i].a[j] + f[i].b ;
  }
  return;
}

int main() {
	int i;
  struct st *a;
  a = (struct st*) malloc(sizeof(struct st) * N);
  for (i = 0; i < N; ++i) {
    a[i].a[0] = a[i].a[1] = i;
  }
  top(a);
  return 0;

}
