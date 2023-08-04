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
#define VEC_SIZE 10000
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
void vec_add_kernel_0(int *a, int *b, int*c, int inc);
void vec_add_kernel_1(int *a, int *b, int*c, int inc);
int main() {   
    int i;
    int inc = 0;
    int a[VEC_SIZE]; 
    int b[VEC_SIZE]; 
    int c[VEC_SIZE];
    for(i = 0; i < VEC_SIZE; i++) {
        a[i] = i;
        b[i] = i * 2;
        c[i] = 0;
    }

#pragma ACCEL task parallel name="vec_add"
//#pragma ACCEL task name="vec_add"
{
    vec_add_kernel_0(a, b, c, inc);
    vec_add_kernel_1(a, b, c, inc);
}
    int cnt = 0;
    for(i = 0; i < VEC_SIZE; i++) {
        if(c[i] != a[i] + b[i]) {
            cnt++;
        }
    }
    if(cnt==0) {
        printf("SUCCESS\n");
        char file_name_s[256] = "pass.o";
        FILE *fp_s=fopen(file_name_s,"ab+");
        if(fp_s==NULL) printf("can't create file!");
        fclose(fp_s);
    } else {
        printf("FAIL\n");
        printf("%d\n", cnt);
        char file_name_f[256] = "fail.o";
        FILE *fp_f=fopen(file_name_f,"ab+");
        if(fp_f==NULL) printf("can't create file!");
        fclose(fp_f);
    }
    return 0;
}


