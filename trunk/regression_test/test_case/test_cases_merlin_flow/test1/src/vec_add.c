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
#include "stdlib.h"

#define VEC_SIZE 10000

void vec_add_kernel(int *a, int*c, int inc);

int main(int argc, char ** argv)
{   
    int i;

    int inc = atoi(argv[1]);
    int a[100]; 
    int b[100]; 
    int c[100]; 
    for (i = 0; i < 100; i++) {
        a[i] = i;
        c[i] = 0;
    }

    vec_add_kernel(a, c, inc);

    if(c[1] == 1235) {
        FILE *fp1=fopen("pass.o","ab+");
        if(fp1==NULL) printf("can't create file!");
    } else {
        FILE *fp2=fopen("fail.o","ab+");
        if(fp2==NULL) printf("can't create file!");
        printf("c = %d\n", c[1]);
    }
    return 0;
}


