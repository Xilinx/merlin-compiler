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
#include "common.h"

int gl_var1 = 168;
int gl_var2 = 168;
int gl_var3 = 168;
const int c = 0;
struct LS;

void gl_func();

#pragma ACCEL kernel
void f1() {
  gl_var1 += 1 + c;
  HideDef *h;
  LS *ls;
  gl_func();
}

#pragma ACCEL kernel
void f2() {
  gl_var2 += 2 + c;
  HideDef *h;
  LS *ls;
  gl_func();
}

#pragma ACCEL kernel
void f3() {
  gl_var3 += 3 + c;
  HideDef *h;
  LS *ls;
  gl_func();
}

struct LS {
  int x;
};

void gl_func() {}
