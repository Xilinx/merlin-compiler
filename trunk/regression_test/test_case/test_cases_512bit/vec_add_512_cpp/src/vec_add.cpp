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

#include<fstream>
#include <stdio.h>
#define VEC_SIZE 1024000

void vec_add_kernel(int *a, int *b, int*c, int inc);


int main()
{   
    int i;

    int inc = 0;
    int * a = new int[VEC_SIZE];
    int * b = new int[VEC_SIZE];
    int * c = new int[VEC_SIZE];
    if (!(a && b && c)) while(1);

    for (i = 0; i < VEC_SIZE; i++)
    {
        a[i] = i;
        b[i] = i*i;
    }

#pragma cmost task name="vec_add"
    vec_add_kernel(a, b, c, inc);
    FILE* out_file = fopen("c_out.dat", "w");
     
    fwrite(c, sizeof(int), VEC_SIZE, out_file);
    fclose(out_file);

    int err = 0;
    for (i = 0; i < VEC_SIZE; i++)
    {
        if (c[i] != i+i*i && err < 10)
        {
            err ++;
            printf("ERROR: [%d] out=%d exp=%d \n", c[i], i*i+i);
        }
    }
    if (err == 0)
    {
        printf("Success.\n");
    }

    delete [] a;
    delete [] b;
    delete [] c;
    return 0;
}

