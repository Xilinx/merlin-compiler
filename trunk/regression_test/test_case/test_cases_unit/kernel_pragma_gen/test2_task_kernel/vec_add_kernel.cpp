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
#define TILE 16
#pragma ACCEL kernel name="vec_add1"
void vec_add_kernel1(int *a, int *b, int*c, int inc)
{
    int j, j0, jj;
    for (j0 = 0 ;j0 < VEC_SIZE/TILE; j0++) { 
        #pragma ACCEL parallel
        for (jj = 0; jj < TILE; jj++) {
            j = j0 * TILE + jj;
            c[j] = a[j]+b[j]+inc;
        }
    }
}

void vec_add_kernel2(int *a, int *b, int*c, int inc)
{
    int j, j0, jj;
    for (j0 = 0 ;j0 < VEC_SIZE/TILE; j0++) { 
        #pragma ACCEL parallel
        for (jj = 0; jj < TILE; jj++) {
            j = j0 * TILE + jj;
            c[j] = a[j]+b[j]+inc+10000;
        }
    }
}
