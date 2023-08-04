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
#define SIZE 10000
#include "ap_int.h"
static void __merlin_dummy_SgTemplateClassDeclaration_template_();
typedef unsigned long retval_8_false_Type;
static void __merlin_dummy_SgTemplateMemberFunctionDeclaration_template_();
static void __merlin_dummy_SgTemplateMemberFunctionDeclaration_template_();
void p_Z5test2ILi100EEP7ap_uintILi32EEPi(class ap_uint< 32 > a[100],int b[100]);

void p_Z5test2ILi100EEP7ap_uintILi32EEPi(class ap_uint< 32 > a[100],int b[100])
{
  for (int i = 0; i < 100; i++) {
    b[i] = ((int )a[i]);
  }
}
static void __merlin_dummy_SgTemplateFunctionDeclaration_template_();
void p_Z5test1ILi100EEP7ap_uintILi32EEPi(class ap_uint< 32 > a[100],int b[100]);

void p_Z5test1ILi100EEP7ap_uintILi32EEPi(class ap_uint< 32 > a[100],int b[100])
{
  p_Z5test2ILi100EEP7ap_uintILi32EEPi(a,b);
}
static void __merlin_dummy_SgTemplateFunctionDeclaration_template_();

struct ::test_class_100_ 
{
}
;
void p_ZN15test_class_100_5test3IEEP7ap_uintILi32EEPi(struct test_class_100_ *this_,class ap_uint< 32 > a[100],int b[100]);

void p_ZN15test_class_100_5test3IEEP7ap_uintILi32EEPi(struct test_class_100_ *this_,class ap_uint< 32 > a[100],int b[100])
{
  for (int i = 0; i < 100; i++) {
    b[i] = ((int )a[i]);
  }
}

void p_ZN15test_class_100_5test4ILi100EEEP7ap_uintILi32EEPi(struct test_class_100_ *this_,class ap_uint< 32 > a[100],int b[100])
{
  for (int i = 0; i < 100; i++) {
    b[i] = ((int )a[i]);
  }
}
static void __merlin_dummy_kernel_pragma();

void test(class ap_uint< 32 > a[10000],int b[10000])
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem1
  
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem2
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  p_Z5test1ILi100EEP7ap_uintILi32EEPi(a,b);
  struct test_class_100_ new_class;
  p_ZN15test_class_100_5test3IEEP7ap_uintILi32EEPi(&new_class,a,b);
  p_ZN15test_class_100_5test4ILi100EEEP7ap_uintILi32EEPi(&new_class,a,b);
}
