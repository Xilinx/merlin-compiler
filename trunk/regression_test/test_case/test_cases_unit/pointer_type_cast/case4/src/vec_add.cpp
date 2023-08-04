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

#ifdef MCC_ACC
#include MCC_ACC_H_FILE
#endif


#include <stdio.h>
#include <stdlib.h>

#define VEC_SIZE 10000

void vec_add_kernel(int *a, int *b, int*c, int inc);


void vec_add_kernel_sw(int *a, int *b, int*c, int inc)
{
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) c[j] = a[j]+b[j]+inc;
}

int main(int argc, char * argv[])
{   
#ifdef MCC_ACC
    __merlin_init(argv[1]);
#endif
    int i;

    int inc = 0;
    int a[VEC_SIZE];
    int b[VEC_SIZE];
    int c[VEC_SIZE];
    int c_sw[VEC_SIZE];
    for (int i = 0; i < VEC_SIZE; i++)
    {
        a[i] = i;
        b[i] = (i*i) % 100;
        c[i] = c_sw[i] = 0;
    }


    vec_add_kernel_sw(a, b, c_sw, inc);

    char *str = (char*)malloc(sizeof(char)*100);

#ifdef MCC_ACC
    __merlin_vec_add_kernel(a, b, c, inc);
#else
    vec_add_kernel(a, b, c, inc);
#endif

    int err = 0;
    for (int i = 0; i < VEC_SIZE; i++)
    {
        if (c[i] != c_sw[i]) printf("ERROR %d: out=%d exp=%d\n",
                i, c[i], c_sw[i]);
    }
    if (err == 0)
    {
        printf("Success\n");
    }
    free(str);
#ifdef MCC_ACC
    __merlin_release();
#endif
    return err;
}


