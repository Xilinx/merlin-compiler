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
#include<string.h>
void sub1(int);
void sub2(int *);

#pragma ACCEL kernel
void foo(int *a, int *b, int *c, int *d) {
  int a_buf[1000];
  int b_buf[1000];
  int c_buf[1000];
  memcpy(a, a_buf, sizeof(int) * 1000);
  a[50]++;
  a[60]--;
  a[70]+=a[40];
  a[70]-=a[40];
  a[70]&=a[40];
  a[70]|=a[40];
  a[70]^=a[40];
  a[70]>>=a[40];
  a[70]<<=a[40];
  a[70]%=a[40];
  a[70]/=a[40];
  memcpy(b_buf, b, sizeof(int) * 1000);
  sub1(b[0]);
 
  memcpy(c_buf, c, sizeof(int) * 1000);
  sub2(&c[2]);

  d[0]++;
  switch (a[0]) {
    case 0:
      a[0] += b[0];
      break;
    default:
      a_buf[0] = b[0];
      break;
  }
}

int main() {
 int *a, *b, *c, *d;
  foo(a, b, c, d);
  return 0;
}
