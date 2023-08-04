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

#define VEC_SIZE 1024



void vec_add_kernel(int *a, int *b, int *c, int inc);
void vec_add_sw(int *a, int *b, int *c, int inc);

int main()
{   
    int i;

    int inc = 0;
    
    int a[VEC_SIZE];
    int  b[VEC_SIZE];
    int  c[VEC_SIZE];
    int a_sw[VEC_SIZE];
    int  b_sw[VEC_SIZE];
    int  c_sw[VEC_SIZE];

    for (i = 0; i < VEC_SIZE; i++)
    {
        a[i] = a_sw[i] = i;
        b[i] = b_sw[i] = i*i;
        c[i] = c_sw[i] = 0;
    }
    vec_add_sw(a_sw, b_sw, c_sw, inc);

    vec_add_kernel(a, b, c, inc);

//    cmost_dump_1d(c, "c_out.dat"); // added

    int err = 0;
    for (i = 0; i < VEC_SIZE; i++)
    {
        if (c[i] != c_sw[i]) 
        {
            if (err < 10)
            {
                printf("ERROR [%d]: out=%lf exp=%f\n", i, c[i], a[i] + b[i]);
            }
            else if (err == 10)
            {
                
                printf("...\n");
                break;
            }
            err ++;
        }
    }

    if (err == 0)
    {
        printf("Successful.\n");
    }

    return (err!=0);
}


