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
// RUN: %clang_cc1 -emit-llvm -debug-info-kind=limited -triple x86_64-linux-gnu %s -o - | FileCheck %s
// Make sure that we emit a global variable for each of the members of the
// anonymous union.
static union {
int c;
int d;

union 
{
  int a;
}
;};

int test_it()
{
   c = 1;
   d = 2;
   a = 4;
  return ( c == 1);
}
#pragma ACCEL kernel name="foo"

void foo()
{
  union {
  int i;
  char c;};
   i = 8;
}

