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



#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "config.h"

 float a[SIZE_I][SIZE_K];
 float b[SIZE_J][SIZE_K];
 float c[SIZE_I][SIZE_J];
 float c1[SIZE_I][SIZE_J];


void mm_kernel( float a[SIZE_I][SIZE_K], 
                float b[SIZE_J][SIZE_K],
                float c[SIZE_I][SIZE_J]);

void mm_kernel_sw( float a[SIZE_I][SIZE_K], 
                   float b[SIZE_J][SIZE_K],
                   float c[SIZE_I][SIZE_J])
{

    for (int i = 0; i < SIZE_I; i++)
    for (int j = 0; j < SIZE_J; j++)
    for (int k = 0; k < SIZE_K; k++)
        c[i][j] += a[i][k] * b[k][j];
}



void main()
{
    memset(a[0], 0, SIZE_I*SIZE_K*sizeof(float));
    memset(b[0], 0, SIZE_J*SIZE_K*sizeof(float));
    memset(c[0], 0, SIZE_I*SIZE_J*sizeof(float));
    memset(c1[0], 0, SIZE_I*SIZE_J*sizeof(float));

    float * p = a[0];
    for (int i = 0; i < SIZE_I*SIZE_K; i++)
    {
        *(p++) = rand()*1.0/RAND_MAX;
    }
    p = b[0];
    for (int i = 0; i < SIZE_J*SIZE_K; i++)
    {
        *(p++) = rand()*1.0/RAND_MAX;
    }

#pragma ACCEL task
    mm_kernel(a, b, c);

    mm_kernel_sw(a, b, c1);

    int err = 0;
    for (int i = 0; i < SIZE_I; i++)
    for (int j = 0; j < SIZE_J; j++)
    {
        int larger = fmax(fabs(c[i][j]), fabs( c1[i][j] ) ); 
        if (larger > 0)
        if (fabs((c[i][j] - c1[i][j])) / larger > 1e-5) 
        {
            printf("[ERROR] i=%d j=%d : out=%lf exp=%lf\n", i, j, c[i][j], c1[i][j]);
            err++;
        }

        if (err >= 10) return ;
    }

    printf("Success.\n");
}




