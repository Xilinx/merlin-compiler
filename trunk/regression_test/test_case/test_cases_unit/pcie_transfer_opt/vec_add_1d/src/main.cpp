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
/****************************************************************************/
// main.cpp
// Copyright (C) 2017 Xilinx, Inc. - All rights reserved.
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "vec_add.h"
#ifdef MCC_ACC
#include "__merlinvec_add_kernel_sim.h"
#endif

#define AOCL_ALIGNMENT 64

void *alignedMalloc(size_t size) {
  void *result = NULL;
  posix_memalign (&result, AOCL_ALIGNMENT, size);
  return result;
}

int main(int argc, char* argv[])
{   
#ifdef MCC_ACC
    char *bin_file = argv[1];
    __merlin_init(bin_file);
#endif

    int i=0;
    int inc = 1;
    int * a = (int *)alignedMalloc(sizeof(int)*VEC_SIZE); 
    int * b = (int *)alignedMalloc(sizeof(int)*VEC_SIZE); 
    int * c = (int *)alignedMalloc(sizeof(int)*VEC_SIZE); 
    int * c_acc = (int *)alignedMalloc(sizeof(int)*VEC_SIZE); 

    // init
    for(i=0; i<VEC_SIZE; i++) {
	    a[i] = i;
	    b[i] = 2*i;
	    c[i] = 0;
	    c_acc[i] = 0;
    }    

    printf("Starting Vector Addition Example \n");

    // execute original reference code
    vec_add(a, b, c, inc);
    // execute kernel code 
#ifdef MCC_ACC
    __merlin_vec_add_kernel(a, b, c_acc, inc);
#else
     vec_add_kernel(a, b, c_acc, inc);
#endif
    // test results
    int err = 0;
    for(i=0; i<VEC_SIZE; i++) {
        if(c[i] != c_acc[i]) {
            err++;
            printf("yes = %d, no = %d\n", c[i], c_acc[i]);
        }
    }

    free(a);
    free(b);
    free(c);
    free(c_acc);

#ifdef MCC_ACC
    __merlin_release();
#endif

    if (err) {
        printf("Test failed %d\n",err);
        char file_name[256] = "fail.o";
        FILE *fp=fopen(file_name,"ab+");
        if(fp==NULL) printf("can't create file!");
        return 1;
    }
    else {
        printf("Test passed\n");
        char file_name_s[256] = "pass.o";
        FILE *fp_s=fopen(file_name_s,"ab+");
        if(fp_s==NULL) printf("can't create file!");
        return 0;
    }
}


