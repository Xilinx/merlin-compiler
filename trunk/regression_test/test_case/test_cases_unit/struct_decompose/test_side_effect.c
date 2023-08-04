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
  short b;
};

int index() {
  static int i = 0;
  ++i;
  return i;
}

#pragma ACCEL kernel
void top(struct st *a) {
#pragma ACCEL interface variable=a depth=100
  int i = 0;
  struct st tmp;
  tmp.a = 0;
  tmp.b = 1;
  while (i < 100) {
    a[i++] = tmp;
  }

  i = 0;
  while ( i < 100) {
    a[index()] = tmp;
  }
  while (i < 100) {
    a[i++].a++;
    a[i++].b++;
  }
  i = 0;
  while (i < 100) {
  if (a[i++].a == tmp.a) break;
  }
  i = 0;
  while (i < 100 && a[i++].a == tmp.a);
  do {
  } while (a[i++].a == tmp.b);

  for (i = 0; a[i++].b != tmp.b;) {
  }
}
