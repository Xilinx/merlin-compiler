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

struct point {
  int x;
  char y;
};

struct point addpoint(struct point p1, struct point p2)
{
	struct point p;

	p.x = p1.x + p2.x;
	p.y = p1.y + p2.y;

	return p;
}

#pragma ACCEL kernel
void top(struct point *a) {
#pragma ACCEL interface variable=a depth=100
	int i;
	struct point p = {1, 1};

  for (i = 0; i < N; ++i) {
	  a[i] = addpoint(a[i], p);
  }

  return;
}

int main() {
	int i;
  struct point *a;
  a = (struct point*) malloc(sizeof(struct point) * N);
  for (i = 0; i < N; ++i) {
    a[i].x = i;
    a[i].y = i;
  }
  top(a);
  return 0;

}
