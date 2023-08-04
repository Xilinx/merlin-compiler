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

struct ::float2 
{
  float x;
  float y;
}
;

void p_ZN6float2C2Ev(struct float2 *this_)
{
  (this_ -> x = ((float )0) , this_ -> y = ((float )0));
}

void p_ZN6float2C2Ef(struct float2 *this_,float a)
{
  this_ -> x = a;
  this_ -> y = a;
}

struct float2 &p_ZN6float2aSERK6float2(struct float2 *this_,const struct float2 &other)
{
  this_ -> x = other . x;
  this_ -> y = other . y;
  return  *this_;
}
static void __merlin_dummy_SgClassDeclaration_struct_flo_();

struct float2x8 
{
  struct float2 i0;
  struct float2 i1;
}
;
static void __merlin_dummy_kernel_pragma();
/*float2x8 in*/

void foo()
{
  struct float2 a;
  p_ZN6float2C2Ev(&a);
  ;
  struct float2 b;
  p_ZN6float2C2Ev(&b);
  ;
  p_ZN6float2aSERK6float2(&b,a);
  struct float2x8 data;
  p_ZN6float2C2Ev(&data . i0);
  p_ZN6float2C2Ev(&data . i1);
  ;
  struct float2x8 data1;
  p_ZN6float2C2Ev(&data1 . i0);
  p_ZN6float2C2Ev(&data1 . i1);
  ;
  data1 = data;
  p_ZN6float2aSERK6float2(&data1 . i0,data . i0);
  p_ZN6float2aSERK6float2(&data1 . i1,data . i1);
}
