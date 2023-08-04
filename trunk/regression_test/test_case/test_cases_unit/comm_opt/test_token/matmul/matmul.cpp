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
/*Step1: loop exchange*/
#include <string.h>
#include <stdio.h>
#include <math.h>
#define VEC_SIZE 256
#define VEC_flat_SIZE (VEC_SIZE * VEC_SIZE)
void matmul_kernel(float *a,float *b,float *c);

int main()
{
  float a[256 * 256];
  float b[256 * 256];
  float c[256 * 256];
  for (int i = 0; i < 256 * 256; i++) {
    a[i] = i;
    b[i] = (i * i);
    c[i] = 0;
  }
  
  matmul_kernel(a,b,c);
{
    int i;
    int j;
    int k;
    float c_ref[256 * 256];
    memset(c_ref,0,sizeof(float ) * (256 * 256));
    for (k = 0; k < 256; k++) {
      for (i = 0; i < 256; i++) {
        for (j = 0; j < 256; j++) {
          c_ref[i * 256 + j] += a[k * 256 + i] * b[k * 256 + j];
        }
      }
    }
    for (i = 0; i < 256; i++) {
      for (j = 0; j < 256; j++) {
        if (fabs((c[i * 256 + j] - c_ref[i * 256 + j])) > 1e-15) {
          printf("ERROR: i=%d, j=%d\n",i,j);
          return - 1;
        }
      }
    }
  }
  printf("Matrix Multiplication Test Passed.\n");
  return 0;
}
