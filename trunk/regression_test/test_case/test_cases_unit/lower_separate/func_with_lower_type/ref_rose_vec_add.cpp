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
static void __merlin_dummy_SgTemplateClassDeclaration_template_();
static void __merlin_dummy_SgTemplateMemberFunctionDeclaration_a_();
static void __merlin_dummy_SgTemplateMemberFunctionDeclaration_retu_();
static void __merlin_dummy_SgTemplateMemberFunctionDeclaration_a_();

struct ::A_int_ 
{
  int a;
}
;
void p_ZN6A_int_4incrIEEv(struct A_int_ *this_);
int p_ZNK6A_int_3getIEEv(const struct A_int_ *this_);
void p_ZN6A_int_3setIEEi(struct A_int_ *this_,int val);

void p_ZN6A_int_4incrIEEv(struct A_int_ *this_)
{
  this_ -> a++;
}

int p_ZNK6A_int_3getIEEv(const struct A_int_ *this_)
{
  return this_ -> a;
}

void p_ZN6A_int_3setIEEi(struct A_int_ *this_,int val)
{
  this_ -> a = val;
}

void sub(struct A_int_ *a)
{
  p_ZN6A_int_4incrIEEv(&a[0]);
}
static void __merlin_dummy_kernel_pragma();

void top(int *a)
{
  struct A_int_ tmp;
  p_ZN6A_int_3setIEEi(&tmp,a[1]);
  sub(&tmp);
  a[2] = p_ZNK6A_int_3getIEEv(&tmp);
}
