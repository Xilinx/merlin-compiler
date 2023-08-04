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
#include<stdio.h>

#define VEC_SIZE 10000

#pragma ACCEL kernel
void vec_add_kernel(int a[VEC_SIZE], int b[VEC_SIZE], int c[VEC_SIZE], int inc)
{
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) c[j] = a[j]+b[j]+inc;
    char* psrc = (char*)(&a[j]);
    char* pdst = (char*)(&c[j]);
    int ret = 0;
    if (psrc)
      ret += 1;
    if (pdst != NULL)
      ret += 1 << 1;
    if (psrc > pdst)
      ret += 1 << 2;
    if (psrc < pdst)
      ret += 1 << 3;
    if (psrc <= pdst)
      ret += 1 << 4;
    if (psrc >= pdst)
      ret += 1 << 5;
    if (psrc == pdst)
      ret += 1 << 6;
    while (pdst)
      pdst = NULL;
    for (;psrc;) {
      psrc = NULL;
    } 
    _Bool flag0 = pdst;
    if (flag0) {
      pdst = NULL;
    }
    _Bool flag1;
    flag1 = pdst;
    if (flag1) {
      pdst = NULL;
    }
    _Bool flag2 = (_Bool) pdst;
    if (flag2) {
      pdst = NULL;
    }
    if (a) {
      pdst = NULL;
    }
    if (b) {
      pdst = NULL;
    }
    while (a) {
      pdst = NULL;
    }
    for (j = (a ? 0 : 1); j < 10; j++) {
      pdst = NULL;
    }
}
