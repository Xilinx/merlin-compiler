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
#include "__merlin_math.h"
int func_user(int x) {
    return x;
}
int func_user2() {
    return 3;
}
void vec_add_kernel(int *a, int *b, int*c, int inc)
{
    float aa = 4.0;
    float bb = sqrt(aa);
    float x = 1.0;
    double y = 2.0;
    long z = 8;
    int m = 3;
    short n = 4;
    int j, j0, jj;
    for (j0 = 0 ;j0 < VEC_SIZE/TILE; j0++) { 
        #pragma ACCEL parallel
        for (jj = 0; jj < TILE; jj++) {
            j = j0 * TILE + jj;
            c[j] = a[j]+b[j]+inc+ bb;
            c[j] += max(m,2) + abs(1) + abs_diff(1,m) + add_sat(m,2) + hadd(1,2) + rhadd(1,2);
            c[j] += clamp(1,2,3) + clz(1) + mad_hi(1,2,3) + mad_sat(1,m,3) + min(1,2);
            c[j] += mul_hi(1,m) + rotate(1,2) + sub_sat(1,func_user(2)) + popcount(99);
        }
    }
    for(j=0;j<abs(6);j++) {
    }
    func_user(abs(6));
}

