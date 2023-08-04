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
#define VEC_SIZE 1024000
#define STEP_SIZE 53
void vec_add_kernel(double *a, double *b, double*c, double inc);


int main()
{   
    int i;

    double inc = 0;
    double * a = (double *)malloc(VEC_SIZE * sizeof(double)); 
    double * b = (double *)malloc(VEC_SIZE * sizeof(double));
    double * c = (double *)malloc(VEC_SIZE * sizeof(double));
    if (!(a && b && c)) while(1);

    for (i = 0; i < VEC_SIZE; i++)
    {
        a[i] = i;
        b[i] = i*i;
    }

#pragma cmost task name="vec_add"
    vec_add_kernel(a, b, c, inc);

    FILE* out = fopen( "c_out.dat", "w"); // added
    fwrite(c, sizeof(double), VEC_SIZE, out);
    fclose(out);

    int err = 0;
    for (i = 0; i < VEC_SIZE/STEP_SIZE * STEP_SIZE; i++)
    { 
        double ret = i * i;
        ret += i;
        if (fabs(c[i] / ret  - 1) > 0.001 && err < 10)
        {
            err ++;
            printf("ERROR: [%d] out=%lf exp=%lf \n", i, c[i], (double)i*i+i);
        }
    }
    if (err == 0)
    {
        printf("Success.\n");
    }

    free(a);
    free(b);
    free(c);




    return 0;
}


