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

struct ::xf_A_1_ 
{
}
;

struct ::xf_A_8_ 
{
}
;

struct ::xf_A_7_ 
{
}
;

struct ::xf_A_6_ 
{
}
;

struct ::xf_A_5_ 
{
}
;

struct ::xf_A_4_ 
{
}
;

struct ::xf_A_3_ 
{
}
;

struct ::xf_A_2_ 
{
}
;

int p_ZN2xf4A_1_1fIiEEPi(int *a)
{
  return a[0];
}

int p_ZN2xf4A_2_1fIiEEPi(int *a)
{
  return p_ZN2xf4A_1_1fIiEEPi(a) + a[2];
}

int p_ZN2xf4A_3_1fIiEEPi(int *a)
{
  return p_ZN2xf4A_2_1fIiEEPi(a) + a[3];
}

int p_ZN2xf4A_4_1fIiEEPi(int *a)
{
  return p_ZN2xf4A_3_1fIiEEPi(a) + a[4];
}

int p_ZN2xf4A_5_1fIiEEPi(int *a)
{
  return p_ZN2xf4A_4_1fIiEEPi(a) + a[5];
}

int p_ZN2xf4A_6_1fIiEEPi(int *a)
{
  return p_ZN2xf4A_5_1fIiEEPi(a) + a[6];
}

int p_ZN2xf4A_7_1fIiEEPi(int *a)
{
  return p_ZN2xf4A_6_1fIiEEPi(a) + a[7];
}

int p_ZN2xf4A_8_1fIiEEPi(int *a)
{
  return p_ZN2xf4A_7_1fIiEEPi(a) + a[8];
}
namespace xf
{
static void __merlin_dummy_SgTemplateClassDeclaration_template_();
static void __merlin_dummy_SgTemplateInstantiationDecl_template_();
}
static void __merlin_dummy_kernel_pragma();

void top(int *a)
{
p_ZN2xf4A_8_1fIiEEPi(a);
}
static void __merlin_dummy_SgTemplateInstantiationMemberFunctionDecl_int_f_in_();
