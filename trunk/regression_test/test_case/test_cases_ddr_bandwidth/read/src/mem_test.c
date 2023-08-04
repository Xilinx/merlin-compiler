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
#include <stdlib.h>

#define VEC_SIZE 1024000

void mem_test_kernel(int *a, int*c, int size, int iterations);


int main(int argc, char** argv)
{   
    int i;
    int size = 1;
    int iterations = 1024 * 1024;
    printf("program burst length (1 by default) iterations (1024 * 1024 by default)!\n");
    if (argc > 1)
      size = atoi(argv[1]);
    if (argc > 2)
      iterations = atoi(argv[2]);
    printf("test burst length = %d, with iterations = %d\n", size, iterations);
    int inc = 0;
    int * a = (int *) malloc(sizeof(int) * VEC_SIZE);
    int * c = (int *) malloc(sizeof(int) * VEC_SIZE);
    if (!(a && c)) while(1);

    for (i = 0; i < VEC_SIZE; i++)
    {
        a[i] = i;
        c[i] = 0;
    }
#pragma cmost task name="mem_test"
    mem_test_kernel(a, c, size, iterations);

    FILE *out = fopen("c_out.dat", "w");
    fwrite( c,sizeof(int),  VEC_SIZE, out); // added
    fclose(out);

    int err = 0;
    if (*c != a[size / 2]) {
            err ++;
            printf("ERROR: out=%d exp=%d \n", *c, a[size /2 ]);
    }
    if (err == 0)
    {
        printf("Success.\n");
    }

    free(a);
    free(c);
    return 0;
}


