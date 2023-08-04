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

typedef struct {
  volatile int a;
#pragma ACCEL attribute variable=b depth=10
#pragma ACCEL attribute variable=c depth=10
  const int *b;
  volatile int *c;
} st;

struct st2 {
#pragma ACCEL attribute variable=a depth=10
  volatile st *a;
  const st c;
};
#pragma ACCEL kernel
void top(volatile st *const a, st *const b, volatile struct st2 c, volatile struct st2 d) {

#pragma ACCEL interface variable=a depth=100
#pragma ACCEL interface variable=b depth=100
#pragma ACCEL interface variable=c->a depth=100
  for (int i = 0; i < 100; ++i) {
    b[i] = ((st *)a)[i];	
  }
  st here;
  here = ((st *)(c.a))[0];
  st here2;
  here2 = ((st *)(((st2 *)(&c))->a))[0];
  b[10] = here;
  b[11] = here2;
  st *ptr;
  ptr = (st *)(c.a);
  b[0].a = b[1].b[0];
  b[0].a = b[1].c[0];
  b[0].a = c.a[0].a;
  b[0].a = c.c.a;
  b[0].a = d.a[0].a;
  b[12] = *ptr;
  return;
}

