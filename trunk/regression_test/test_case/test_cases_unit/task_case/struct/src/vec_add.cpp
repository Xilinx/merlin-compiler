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
#define AOCL_ALIGNMENT 64
#define VEC_SIZE  10000
#define VEC_SIZE2 10000
#ifdef MCC_ACC
#include MCC_ACC_H_FILE
#endif
struct st {
	int a;
} m;
void vec_add_top(struct st *a, int *b, struct st *d, int*c1, int *c2, int inc);
int main(int argc, char **argv) {   
    int i;
    int inc = 1;
    struct st a[VEC_SIZE2]; 
    int b[VEC_SIZE2]; 
    struct st d[VEC_SIZE2]; 
    int c1[VEC_SIZE2]; 
    int c2[VEC_SIZE2]; 
    for(i=0; i<VEC_SIZE2; i++) {
	    a[i].a = i;
	    b[i] = 2*i;
	    d[i].a = 3*i;
	    c1[i] = 1;
	    c2[i] = 1;
    }    
#ifdef MCC_ACC
    char *kernel_binary_file = argv[1];
    __merlin_init(kernel_binary_file);
    __merlin_vec_add_top(a, b, d, c1, c2, inc);
    __merlin_release();
#else
    vec_add_top(a, b, d, c1, c2, inc);
#endif
    int err = 0;
    for(i=0; i<VEC_SIZE; i++) {
        if(c1[i] != a[i].a + b[i] + inc) {
            printf("Test1 failed\n");
            err = 1;
            break;
        }
    }
    for(i=0; i<VEC_SIZE2; i++) {
        if(c2[i] != d[i].a + b[i] + inc + 10000) {
            printf("Test2 failed\n");
            err = 1;
            break;
        }
    }    
    if (0 == err) {
      printf ("Success! %d\n",err);
      char file_name_s[256] = "pass.o";
      FILE *fp_s=fopen(file_name_s,"ab+");
      if(fp_s==NULL) printf("can't create file!");
    } else	 {
      printf ("Fail! %d\n",err);
      char file_name[256] = "fail.o";
      FILE *fp=fopen(file_name,"ab+");
      if(fp==NULL) printf("can't create file!");
    }
    return 0;
}


