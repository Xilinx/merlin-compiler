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
#include <ap_int.h>
#include "merlin_type_define.h"
extern int __merlin_include__GB_merlin_type_define_h_1;
extern int __merlin_include__GB_ap_int_h_0;

static void p_Z5test2ILi100EEP7ap_uintILi32EEPi_1(class ap_uint< 32 > a[100],int b[100])
{
  for (int i = 0; i < 100; i++) {
    b[i] = ((int )a[i]);
  }
}

static void p_Z5test1ILi100EEP7ap_uintILi32EEPi_1(class ap_uint< 32 > a[100],int b[100])
{
  p_Z5test2ILi100EEP7ap_uintILi32EEPi_1(a,b);
}

static void p_ZN15test_class_100_5test3IEEP7ap_uintILi32EEPi_1(struct test_class_100_ *this_,class ap_uint< 32 > a[100],int b[100])
{
  for (int i = 0; i < 100; i++) {
    b[i] = ((int )a[i]);
  }
}

static void p_ZN15test_class_100_5test4ILi100EEEP7ap_uintILi32EEPi_1(struct test_class_100_ *this_,class ap_uint< 32 > a[100],int b[100])
{
  for (int i = 0; i < 100; i++) {
    b[i] = ((int )a[i]);
  }
}
#pragma ACCEL kernel

void test(class ap_uint< 32 > a[10000],int b[10000])
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem1
  
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem2
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  p_Z5test1ILi100EEP7ap_uintILi32EEPi_1(a,b);
  struct test_class_100_ new_class;
  p_ZN15test_class_100_5test3IEEP7ap_uintILi32EEPi_1(&new_class,a,b);
  p_ZN15test_class_100_5test4ILi100EEEP7ap_uintILi32EEPi_1(&new_class,a,b);
}
