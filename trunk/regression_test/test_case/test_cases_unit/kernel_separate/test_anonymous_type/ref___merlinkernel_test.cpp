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
#include "merlin_type_define.h"
extern int __merlin_include__GB_merlin_type_define_h_0;
int b[3] = {(8), (13), (21)};
struct c c;
static const char str[] = "text";
#pragma ACCEL kernel name="test"

void test()
{
// expected-warning {{address of array 'b' will always evaluate to 'true'}}
  if (!((bool )b)) {
  }
// expected-warning {{comparison of array 'b' equal to a null pointer is always false}}
  if (b == ((int *)0)) {
  }
// expected-warning {{address of array 'c.x' will always evaluate to 'true'}}
  if (!((bool )c . x)) {
  }
// expected-warning {{comparison of array 'c.x' equal to a null pointer is always false}}
  if (c . x == ((int *)0)) {
  }
// expected-warning {{address of array 'str' will always evaluate to 'true'}}
  if (!((bool )str)) {
  }
// expected-warning {{comparison of array 'str' equal to a null pointer is always false}}
  if (((const char *)0) == str) {
  }
}
