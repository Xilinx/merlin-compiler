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
// RUN: %clang_cc1 -triple x86_64-apple-darwin -fsyntax-only -verify  %s
// rdar://18716393
extern int a[];
int b[] = {(8), (13), (21)};
struct {
int x[10];}c;
const char str[] = "text";

void ignore()
{
  if (!a) {
  }
}
#pragma ACCEL kernel name="test"

void test()
{
// expected-warning {{address of array 'b' will always evaluate to 'true'}}
  if (!b) {
  }
// expected-warning {{comparison of array 'b' equal to a null pointer is always false}}
  if (b == 0) {
  }
// expected-warning {{address of array 'c.x' will always evaluate to 'true'}}
  if (!c . x) {
  }
// expected-warning {{comparison of array 'c.x' equal to a null pointer is always false}}
  if (c . x == 0) {
  }
// expected-warning {{address of array 'str' will always evaluate to 'true'}}
  if (!str) {
  }
// expected-warning {{comparison of array 'str' equal to a null pointer is always false}}
  if (0 == str) {
  }
}
int array[2];

int test1()
{
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
  if (!array) {
    return array[0];
// expected-warning {{comparison of array 'array' not equal to a null pointer is always true}}
  }
   else if (array != 0) {
    return array[1];
  }
// expected-warning {{comparison of array 'array' equal to a null pointer is always false}}
  if (array == 0)
    return 1;
  return 0;
}
#define NULL (void*)0

int test2(int *pointer,char ch,void *pv)
{
// expected-warning {{address of 'pointer' will always evaluate to 'true'}}
  if (!(&pointer)) {
    return 0;
  }
// expected-warning {{address of 'pointer' will always evaluate to 'true'}}
  if (&pointer) {
    return 0;
  }
// expected-warning {{comparison of address of 'pointer' equal to a null pointer is always false}}
  if (&pointer == ((void *)0)) {
  }
// expected-warning {{comparison of address of 'pointer' not equal to a null pointer is always true}}
  if (&pointer != ((void *)0)) {
  }
  return 1;
}

void test3()
{
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
  if (array) {
  }
// expected-warning {{comparison of array 'array' not equal to a null pointer is always true}}
  if (array != 0) {
  }
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
  if (!array) {
  }
// expected-warning {{comparison of array 'array' equal to a null pointer is always false}}
  if (array == 0) {
  }
  if (array[0] && array)
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
{
  }
  if (array[0] || array)
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
{
  }
  if (array[0] && !array)
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
{
  }
  if (array[0] || !array)
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
{
  }
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
  if (array && array[0]) {
  }
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
  if (!array || array[0]) {
  }
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
  if (array || !array && array[0])
// expected-warning {{address of array 'array' will always evaluate to 'true'}}
{
  }
}
// rdar://19256338
#define SAVE_READ(PTR) if( (PTR) && (&result) ) *result=*PTR;

void _HTTPClientErrorHandler(int me)
{
  int *result;
  if (&me && &result)
     *result = *(&me);
  ;
}

void test_conditional_operator()
{
  int x;
// expected-warning {{address of array}}
  x = (b?1 : 0);
// expected-warning {{address of array}}
  x = (c . x?1 : 0);
// expected-warning {{address of array}}
  x = (str?1 : 0);
// expected-warning {{address of array}}
  x = (array?1 : 0);
// expected-warning {{address of 'x'}}
  x = (&x?1 : 0);
}
