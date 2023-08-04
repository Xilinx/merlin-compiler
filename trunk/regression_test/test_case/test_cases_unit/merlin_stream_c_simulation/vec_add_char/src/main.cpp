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
#include MCC_ACC_H_FILE
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
    char inc = 1;
    char * a = (char *)alignedMalloc(sizeof(char)*VEC_SIZE*TILE); 
    char * b = (char *)alignedMalloc(sizeof(char)*VEC_SIZE*TILE); 
    char * c = (char *)alignedMalloc(sizeof(char)*VEC_SIZE*TILE); 

    // init
    for(i=0; i<VEC_SIZE*TILE; i++) {
	    a[i] = (char)i;
	    b[i] = (char)2*i;
	    c[i] = (char)1;
    }    

    printf("Starting Vector Addition Example \n");

    // execute kernel code 
#ifdef MCC_ACC
    __merlin_vec_add_kernel(a, b, c, inc);
    __merlin_release();
#else
    vec_add_kernel(a, b, c, inc);
#endif

    // test results
    int err = 0;
    for(i=0; i<VEC_SIZE*TILE; i++) {
        char ref_c = a[i] + b[i];
        if(c[i] != ref_c) {
            printf("sum = %d, a = %d, b = %d, merlin = %d\n", a[i] + b[i] + inc, a[i], b[i], c[i]);
            err++;
        }
    }

    free(a);
    free(b);
    free(c);

    if (err) {
        printf("Test failed %d\n",err);
        return 1;
    }
    else {
        printf("Test passed\n");
        return 0;
    }
}


