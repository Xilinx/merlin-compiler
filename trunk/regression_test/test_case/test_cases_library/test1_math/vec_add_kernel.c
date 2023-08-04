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
void vec_add_kernel(int *a, int *b, int*c, int inc)
{
    float aa = 4.0;
    float x = 1.0;
    double y = 2.0;
    double z[2];
    z[0] = 1.0;
    z[1] = 2.0;
    int m = 3;
    short n = 4;
    int j, j0, jj;
    for (j0 = 0 ;j0 < VEC_SIZE/TILE; j0++) { 
        #pragma ACCEL parallel
        for (jj = 0; jj < TILE; jj++) {
            j = j0 * TILE + jj;
            c[j] = a[j]+b[j]+inc;
            c[j] += cos(x) + sin(x) + tan(y) + acos(x) + asin(y) + atan(x) + atan2(x,y) + cosh(x) +sinh(x) + tanh(y);
            c[j] += exp(x) + ldexp(x,y) + log(x) + log10(y) + modf(x,z) + exp2(y) + expm1(x) + ilogb(y) + log2(x);
            c[j] += pow(x,y) + sqrt(y) + cbrt(x) + hypot(x,y) + ceil(x) + floor(y) + fmod(x,y) + trunc(y) + round(x) + rint(y);
            c[j] += fdim(x,y) + fmin(x,x) + fmax(y,y) + fabs(x) + isfinite(y) + isinf(x) + isnan(y);
            // + frexp(x,z)
        }
    }
}
