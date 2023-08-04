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
#define VEC_SIZE 10000
#define TILE_SIZE 10

#pragma ACCEL kernel
void vec_add_kernel(int a[VEC_SIZE],int b[VEC_SIZE],int c[VEC_SIZE],int inc)
{
    int i, j, k, p;
    for (i = 0; i < VEC_SIZE / TILE_SIZE; i++) {
        // Test if ignored due to innermost loop
        for (j = 0; j < TILE_SIZE; j++) {
            int idx = i * TILE_SIZE + j;
            c[idx] = a[idx] + b[idx] + inc;
        }
    }

    // Test imperfect loop
    // Test shadowed by pipeline flatten
    for (i = 0; i < VEC_SIZE / TILE_SIZE; i++) {
#pragma ACCEL PIPELINE auto{options: I1=["off","","flatten"]}
        for (j = 0; j < TILE_SIZE; j++) {
            for (k = 0; k < 100; ++k) {
                int idx = i * TILE_SIZE + j;
                c[idx] += a[idx] + b[idx] + inc;
            }
        }
        for (p = 0; p < 10; ++p) {
            for (k = 0; k > 100; ++k) {
              c[p] += a[p + k];
            }
        }
    }

    // Test if ignored due to conflict
    for (i = 0; i < VEC_SIZE / TILE_SIZE; i++) {
#pragma ACCEL tiling factor=4
        for (j = 0; j < TILE_SIZE; j++) {
            int idx = i * TILE_SIZE + j;
            c[idx] = a[idx] + b[idx] + inc;
        }
    }

    // Test if ignored due to unbound loop
    for (j = 0; j < inc; j++) {
        c[j] += j;
    }

    // Test if fully unrolled when tripcount < 10
    for (j = 0; j < 3; j++) {
        c[j] += a[j];
    }
}
