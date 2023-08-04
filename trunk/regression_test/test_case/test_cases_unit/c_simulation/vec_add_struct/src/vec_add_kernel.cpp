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
 
merlin_stream ch_input;
merlin_stream ch_c;

void prefetch_input_org(input input_ab[VEC_SIZE]) {
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) { 
        merlin_stream_write(&ch_input, &input_ab[j]);
    }
}
void prefetch_c_org(int c[VEC_SIZE]) {
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) { 
        merlin_stream_read(&ch_c, &c[j]);
    }
}
void compute_org(int inc) {
    int i, j;
     for (j = 0 ;j < VEC_SIZE; j++) {
         input tmp_input_ab;
         int tmp_c;
         merlin_stream_read(&ch_input, &tmp_input_ab);
         tmp_c = tmp_input_ab.a + tmp_input_ab.b + inc;
         merlin_stream_write(&ch_c, &tmp_c);
     }
}
void prefetch_input(input input_ab[VEC_SIZE*TILE]) {
    int i, j;
    for(i=0; i<TILE; i++) {
        prefetch_input_org(input_ab + i*VEC_SIZE);
    }
}
void prefetch_c(int c[VEC_SIZE*TILE]) {
    int i, j;
    for(i=0; i<TILE; i++) {
        prefetch_c_org(c + i*VEC_SIZE);
    }
}
void compute(int inc) {
    int i, j;
    for(i=0; i<TILE; i++) {
        compute_org(inc);
    }
}

#pragma ACCEL kernel
void vec_add_kernel(input input_ab[VEC_SIZE*TILE], int c[VEC_SIZE*TILE], int inc) {
    merlin_stream_init(&ch_input, VEC_SIZE, "int", sizeof(input));
    merlin_stream_init(&ch_c, VEC_SIZE, "int", sizeof(int));
    #pragma ACCEL spawn_kernel
    prefetch_input(input_ab);
    #pragma ACCEL spawn_kernel
    compute(inc);
    #pragma ACCEL spawn_kernel
    prefetch_c(c);
    merlin_stream_reset(&ch_input);
    merlin_stream_reset(&ch_c);
}
