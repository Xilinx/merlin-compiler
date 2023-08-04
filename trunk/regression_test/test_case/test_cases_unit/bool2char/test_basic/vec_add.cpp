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
void f(bool *a, bool b, const bool c[10], const bool *d, const bool e[10][12], bool *f[10], bool (*g)[20]) {}
typedef bool * bool_p;
const int s = 10;
#include<string.h>
#pragma ACCEL kernel
void top(bool_p a, const bool &b, const bool c[10], const bool d[10 + 20], bool ee[], bool fa[s]) {
  bool tmp;
  bool tmp_2[100];
  bool *tmp_3;
  bool e[10][12];
  bool *ff[10];
  bool g[20];
  tmp;
  tmp_2[0];
  tmp_3;
  bool d_buf[30];
  memcpy(d_buf, d, sizeof(bool) * 30);

  for (bool i = true; i != false; i = 1 - i) {}
  f(a, b, c, &tmp, e, ff, &g);
  return;
}
