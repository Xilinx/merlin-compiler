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
struct st {
  int a;
  int b;
  int c;
  int d;
  int e;
  int f;
  int h;
  int i;
  int j;
};
struct st2 {
  struct st a;
  struct st b;
  struct st c;
  struct st d;
};
struct st3 {
  struct st2 a;
  struct st2 b;
  struct st2 c;
  struct st2 d;
};

#pragma ACCEL kernel
void top(struct st3 *a) {
#pragma ACCEL interface variable=a depth=1
  a[0].a.a.a = 0;
  a[0].a.a.b = 0;
  a[0].a.a.c = 0;
  a[0].a.a.d = 0;
  a[0].a.a.e = 0;
  a[0].a.a.f = 0;
  a[0].a.a.h = 0;
  a[0].a.a.i = 0;
  a[0].a.a.j = 0;
  a[0].a.b.a = 0;
  a[0].a.b.b = 0;
  a[0].a.b.c = 0;
  a[0].a.b.d = 0;
  a[0].a.b.e = 0;
  a[0].a.b.f = 0;
  a[0].a.b.h = 0;
  a[0].a.b.i = 0;
  a[0].a.b.j = 0;
  a[0].a.c.a = 0;
  a[0].a.c.b = 0;
  a[0].a.c.c = 0;
  a[0].a.c.d = 0;
  a[0].a.c.e = 0;
  a[0].a.c.f = 0;
  a[0].a.c.h = 0;
  a[0].a.c.i = 0;
  a[0].a.c.j = 0;
  a[0].a.d.a = 0;
  a[0].a.d.b = 0;
  a[0].a.d.c = 0;
  a[0].a.d.d = 0;
  a[0].a.d.e = 0;
  a[0].a.d.f = 0;
  a[0].a.d.h = 0;
  a[0].a.d.i = 0;
  a[0].a.d.j = 0;
  a[0].b.a.a = 0;
  a[0].b.a.b = 0;
  a[0].b.a.c = 0;
  a[0].b.a.d = 0;
  a[0].b.a.e = 0;
  a[0].b.a.f = 0;
  a[0].b.a.h = 0;
  a[0].b.a.i = 0;
  a[0].b.a.j = 0;
  a[0].b.b.a = 0;
  a[0].b.b.b = 0;
  a[0].b.b.c = 0;
  a[0].b.b.d = 0;
  a[0].b.b.e = 0;
  a[0].b.b.f = 0;
  a[0].b.b.h = 0;
  a[0].b.b.i = 0;
  a[0].b.b.j = 0;
  a[0].b.c.a = 0;
  a[0].b.c.b = 0;
  a[0].b.c.c = 0;
  a[0].b.c.d = 0;
  a[0].b.c.e = 0;
  a[0].b.c.f = 0;
  a[0].b.c.h = 0;
  a[0].b.c.i = 0;
  a[0].b.c.j = 0;
  a[0].b.d.a = 0;
  a[0].b.d.b = 0;
  a[0].b.d.c = 0;
  a[0].b.d.d = 0;
  a[0].b.d.e = 0;
  a[0].b.d.f = 0;
  a[0].b.d.h = 0;
  a[0].b.d.i = 0;
  a[0].b.d.j = 0;
  a[0].c.a.a = 0;
  a[0].c.a.b = 0;
  a[0].c.a.c = 0;
  a[0].c.a.d = 0;
  a[0].c.a.e = 0;
  a[0].c.a.f = 0;
  a[0].c.a.h = 0;
  a[0].c.a.i = 0;
  a[0].c.a.j = 0;
  a[0].c.b.a = 0;
  a[0].c.b.b = 0;
  a[0].c.b.c = 0;
  a[0].c.b.d = 0;
  a[0].c.b.e = 0;
  a[0].c.b.f = 0;
  a[0].c.b.h = 0;
  a[0].c.b.i = 0;
  a[0].c.b.j = 0;
  a[0].c.c.a = 0;
  a[0].c.c.b = 0;
  a[0].c.c.c = 0;
  a[0].c.c.d = 0;
  a[0].c.c.e = 0;
  a[0].c.c.f = 0;
  a[0].c.c.h = 0;
  a[0].c.c.i = 0;
  a[0].c.c.j = 0;
  a[0].c.d.a = 0;
  a[0].c.d.b = 0;
  a[0].c.d.c = 0;
  a[0].c.d.d = 0;
  a[0].c.d.e = 0;
  a[0].c.d.f = 0;
  a[0].c.d.h = 0;
  a[0].c.d.i = 0;
  a[0].c.d.j = 0;
  a[0].d.a.a = 0;
  a[0].d.a.b = 0;
  a[0].d.a.c = 0;
  a[0].d.a.d = 0;
  a[0].d.a.e = 0;
  a[0].d.a.f = 0;
  a[0].d.a.h = 0;
  a[0].d.a.i = 0;
  a[0].d.a.j = 0;
  a[0].d.b.a = 0;
  a[0].d.b.b = 0;
  a[0].d.b.c = 0;
  a[0].d.b.d = 0;
  a[0].d.b.e = 0;
  a[0].d.b.f = 0;
  a[0].d.b.h = 0;
  a[0].d.b.i = 0;
  a[0].d.b.j = 0;
  a[0].d.c.a = 0;
  a[0].d.c.b = 0;
  a[0].d.c.c = 0;
  a[0].d.c.d = 0;
  a[0].d.c.e = 0;
  a[0].d.c.f = 0;
  a[0].d.c.h = 0;
  a[0].d.c.i = 0;
  a[0].d.c.j = 0;
  a[0].d.d.a = 0;
  a[0].d.d.b = 0;
  a[0].d.d.c = 0;
  a[0].d.d.d = 0;
  a[0].d.d.e = 0;
  a[0].d.d.f = 0;
  a[0].d.d.h = 0;
  a[0].d.d.i = 0;
  a[0].d.d.j = 0;
  return;
}
