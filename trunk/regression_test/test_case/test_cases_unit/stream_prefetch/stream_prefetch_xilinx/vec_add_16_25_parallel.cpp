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

#define TS 25

#pragma ACCEL kernel name="vec_add_kernel"
void vec_add_kernel(int *a, int *b, int*c, int inc)
{
    #pragma ACCEL interface variable=a depth=10000
    #pragma ACCEL interface variable=b depth=10000
    #pragma ACCEL interface variable=c depth=10000
    
    int j;
    int j0,j1, jj;
    for (j0 = 0 ;j0 < VEC_SIZE/TS/16; j0++)  
    for (j1 = 0 ;j1 < 16; j1++) { 
#pragma ACCEL parallel 
    for (jj = 0 ;jj < TS; jj++) { 
#pragma ACCEL parallel 
      j = j0*TS*16 + j1 * TS + jj;
        c[j] = a[j]+b[j] + inc;
    }
    }
}

