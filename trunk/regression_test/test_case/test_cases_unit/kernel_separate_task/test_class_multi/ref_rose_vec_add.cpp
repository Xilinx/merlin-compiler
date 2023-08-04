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

class class1 
{
  public: int a;
}
;

class class2 
{
  public: int a;
  class class1 myclass1;
}
;

class class3 
{
  public: int a;
  class class2 myclass2;
}
;

class class4 
{
  public: int a;
  class class1 myclass1;
  class class3 myclass3;
}
;

class class5 
{
  public: int a;
}
;

class counter_cls 
{
  public: int a;
  class class4 myclass4;
}
;
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel1(int *a,int *b,int *c,int inc,class counter_cls counter)
{
  for (int j = 0; j < 100; j++) {
    counter . a++;
    c[j] = a[j] + b[j] + inc + counter . a;
  }
}
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel2(int *a,int *b,int *c,int inc)
{
}
static void __merlin_dummy_kernel_pragma();

void vec_add(int *a,int *b,int *c,int inc,class counter_cls counter)
{
  vec_add_kernel1(a,b,c,inc,counter);
  vec_add_kernel2(a,b,c,inc);
}
