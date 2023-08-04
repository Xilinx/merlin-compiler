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
#include<ap_int.h>
#define MIN(a, b) (a > b) ? (b) : (a)
#define MAX(a, b) (a > b) ? (a) : (b)

int sub(int i)
{
  int rose_temp;
  if (i > 0) {
    rose_temp = 1;
  }
   else {
    rose_temp = 0;
  }
  return rose_temp;
}
typedef class ap_int< 10 > int10_t;
#pragma ACCEL kernel

void test(double b[10])
{
  int a[100];
  int aa[100][50];
  int i;
  for (i = ((b[0] > b[1]?b[1] : b[0])); (i < (b[0] > b[1])?b[0] : b[1]); ++i) {
    double rose_temp;
    if (b[i] > ((double )0)) {
      rose_temp = b[i];
    }
     else {
      rose_temp = -b[i];
    }
    a[i] = rose_temp;
  }
  int &a_r = i > 0?a[0] : a[1];
  int rose_temp_0;
  if (i > 0) {
    rose_temp_0 = a[0];
  }
   else {
    rose_temp_0 = a[1];
  }
  const int a_c = rose_temp_0;
  int *rose_temp_1;
  if (i > 0) {
    rose_temp_1 = a;
  }
   else {
    rose_temp_1 = aa[0];
  }
  const int *a_ptr = rose_temp_1;
  (i > 0?a[0] : a[1]) = 1;
  int ret = sub(i);
  while(true){
    int rose_temp_2;
    if (i > 0) {
      rose_temp_2 = a[0];
    }
     else {
      rose_temp_2 = a[1];
    }
    bool rose__temp = (bool )((bool )rose_temp_2);
    if (!rose__temp) {
      break; 
    }
{
    }
  }
{
    bool rose__temp = true;
    do {{
      }
      rose_label__1:
{
      }
      int rose_temp_3;
      if (i < 10) {
        rose_temp_3 = a[0];
      }
       else {
        if (i > 5) {
          rose_temp_3 = a[1];
        }
         else {
          rose_temp_3 = a[2];
        }
      }
      rose__temp = ((bool )((bool )rose_temp_3));
    }while (rose__temp);
  }
{
    bool rose__temp = true;
    int rose_temp_4;
    if (i < 9) {
      rose_temp_4 = a[0];
    }
     else {
      rose_temp_4 = a[1];
    }
    rose__temp = ((bool )((bool )rose_temp_4));
    if (rose__temp) {
    }
  }{
    int rose_temp_5;
    if (i > 5) {
      rose_temp_5 = i;
    }
     else {
      rose_temp_5 = i + 10;
    }
    int rose__temp = rose_temp_5;
    switch(rose__temp){
      case 1:
      break; 
      default:
      break; 
    }
  }
  int10_t a0[10];
  int10_t rose_temp_6;
  if (i > 0) {
    rose_temp_6 = a0[0];
  }
   else {
    rose_temp_6 = ap_int< 10> ("100",((signed char )10));
  }
  a0[1] = rose_temp_6;
  int rose_temp_7;
  if (i > 0) {
    rose_temp_7 = 0;
  }
   else {
    rose_temp_7 = 1;
  }
  static int s_a = rose_temp_7;
  const int c_i = 10;
  static const int s_c_a = 1 >> ((c_i > 10?1 : 2));
  static const int s_c_a_a[2] = {(1 >> ((c_i > 5?1 : 2))), (2 << ((c_i > 2?2 : 3)))};
}
