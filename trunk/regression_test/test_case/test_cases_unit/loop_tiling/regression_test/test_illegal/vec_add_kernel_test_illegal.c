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
#include<assert.h>
#define VEC_SIZE 10000
#pragma ACCEL kernel
void vec_add_kernel(int *a, int *b, int*c, int inc, int n, int m)
{
    int j=0;
   #pragma ACCEL tile factor=2 
   for  (;j < n ; j++)
            c[j] = a[j]+b[j]+inc;
   #pragma ACCEL tile factor=3 
   for  (j = VEC_SIZE ;j > 0 ; j--)
            c[j] = a[j]+b[j]+inc;
   #pragma ACCEL tile factor=6 
   for  (j = 0 ;j <  VEC_SIZE  ; j+=n)
            c[j] = a[j]+b[j]+inc;
   #pragma ACCEL tile factor=3 
   for  (j = 0 ;j < VEC_SIZE  ; j++) {
     c[j] = a[j]+b[j]+inc;
     j=100;
   }
}
