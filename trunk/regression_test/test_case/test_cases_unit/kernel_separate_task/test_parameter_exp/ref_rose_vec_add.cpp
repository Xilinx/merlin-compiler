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
//#include <stdio.h>
#define VEC_SIZE 10000
void vec_add_kernel(int *a,int *b,int *c,int inc);
int my_global_var;
double my_global_var_double;
extern int __merlin_include__LB___merlinhead_kernel_top_h_1;
#include "__merlinhead_kernel_top.h"
extern int __merlin_include__LE___merlinhead_kernel_top_h_1;

int main()
{
  
#pragma ACCEL string __merlin_init("kernel_top.aocx");
  int i;
  int inc = 0;
//cmost_malloc_1d( &a, "int_i.dat" , 4, VEC_SIZE);
  int *a;
//cmost_malloc_1d( &b, "int_i2.dat", 4, VEC_SIZE);
  int *b;
//cmost_malloc_1d( &c, "0"  , 4, VEC_SIZE);
  int *c;
  if (!(a && b && c)) 
    while(1)
      ;
//#pragma ACCEL kernel
  
#pragma ACCEL task name="vec_add"
  __merlin_vec_add_kernel(a,b,c,inc);
//cmost_dump_1d(c, "c_out.dat"); // added
//cmost_free_1d(a);
//cmost_free_1d(b);
//cmost_free_1d(c);
  return 0;
}

void sub_function(int *a,int *b,int *c)
{
  int j;
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j];
}
//#pragma ACCEL kernel

void vec_add_kernel(int *a,int *b,int *c,int inc)
{
  
#pragma ACCEL interface variable=c depth=0
  
#pragma ACCEL interface variable=b depth=0
  
#pragma ACCEL interface variable=a depth=0
  my_global_var++;
  sub_function(a,b,c);
  int j;
  double temp_double = my_global_var_double;
  
#pragma ACCEL pipeline
  
#pragma ACCEL parallel factor = 16
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + inc + ((int )temp_double);
}
