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
#include "vec_add.h"
#include "merlin_stream.h"

merlin_stream ch_a;
merlin_stream ch_b;
merlin_stream ch_c;

void prefetch_a_org(double a[VEC_SIZE]) {
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) { 
        merlin_stream_write(&ch_a, &a[j]);
    }
}
void prefetch_b_org(double b[VEC_SIZE]) {
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) { 
        merlin_stream_write(&ch_b, &b[j]);
    }
}
void prefetch_c_org(double c[VEC_SIZE]) {
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) { 
        merlin_stream_read(&ch_c, &c[j]);
    }
}
void compute_org() {
    int i, j;
     for (j = 0 ;j < VEC_SIZE; j++) {
         double tmp_a, tmp_b, tmp_c;
         merlin_stream_read(&ch_a, &tmp_a);
         merlin_stream_read(&ch_b, &tmp_b);
         tmp_c = tmp_a + tmp_b;
         merlin_stream_write(&ch_c, &tmp_c);
     }
}
void prefetch_a(double a[VEC_SIZE*TILE]) {
    int i, j;
    for(i=0; i<TILE; i++) {
        prefetch_a_org(a + i*VEC_SIZE);
    }
}
void prefetch_b(double b[VEC_SIZE*TILE]) {
    int i, j;
    for(i=0; i<TILE; i++) {
        prefetch_b_org(b + i*VEC_SIZE);
    }
}
void prefetch_c(double c[VEC_SIZE*TILE]) {
    int i, j;
    for(i=0; i<TILE; i++) {
        prefetch_c_org(c + i*VEC_SIZE);
    }
}
void compute() {
    compute_org();
}

#pragma ACCEL kernel
void vec_add_kernel(double a[VEC_SIZE*TILE], double b[VEC_SIZE*TILE], double c[VEC_SIZE*TILE], double inc) {
    merlin_stream_init(&ch_a, VEC_SIZE, "double", sizeof(double));
    merlin_stream_init(&ch_b, VEC_SIZE, "double", sizeof(double));
    merlin_stream_init(&ch_c, VEC_SIZE, "double", sizeof(double));
    #pragma ACCEL spawn_kernel
    prefetch_a(a);
    #pragma ACCEL spawn_kernel
    prefetch_b(b);
    int i, j;
    for(i=0; i<TILE; i++) {
        #pragma ACCEL spawn_kernel
        compute();
    }
    #pragma ACCEL spawn_kernel
    prefetch_c(c);
    merlin_stream_reset(&ch_a);
    merlin_stream_reset(&ch_b);
    merlin_stream_reset(&ch_c);
}
