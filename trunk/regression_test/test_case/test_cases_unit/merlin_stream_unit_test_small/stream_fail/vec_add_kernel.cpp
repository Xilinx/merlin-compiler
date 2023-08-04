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

void prefetch_a_org(int a[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    merlin_stream_write(&ch_a,(&a[j]));
  }
}

void prefetch_b_org(int b[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    merlin_stream_write(&ch_b,(&b[j]));
  }
}

void prefetch_c_org(int c[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    merlin_stream_read(&ch_c,(&c[j]));
  }
}

void compute_org(int inc)
{
  int i;
  int j;
  for (j = 0; j < 1024; j++) {
    int tmp_a;
    int tmp_b;
    int tmp_c;
    merlin_stream_read(&ch_a,(&tmp_a));
    merlin_stream_read(&ch_b,(&tmp_b));
    tmp_c = tmp_a + tmp_b + inc;
    merlin_stream_write(&ch_c,(&tmp_c));
  }
}

void prefetch_a(int a[1024 * 1024])
{
  int i;
  int j;
  for (i = 0; i < 1024; i++) {
    prefetch_a_org(a + i * 1024);
  }
}

void prefetch_b(int b[1024 * 1024])
{
  int i;
  int j;
  for (i = 0; i < 1024; i++) {
    prefetch_b_org(b + i * 1024);
  }
}

void prefetch_c(int c[1024 * 1024])
{
  int i;
  int j;
  for (i = 0; i < 1024; i++) {
    prefetch_c_org(c + i * 1024);
  }
}

void compute(int inc)
{
  int i;
  int j;
  for (i = 0; i < 1024; i++) {
    compute_org(inc);
  }
}
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel(int a[1024 * 1024],int b[1024 * 1024],int c[1024 * 1024],int inc)
{
  
#pragma ACCEL interface variable=c depth=1048576
  
#pragma ACCEL interface variable=b depth=1048576
  
#pragma ACCEL interface variable=a depth=1048576
  merlin_stream_init(&ch_a,1024,"int",sizeof(int ));
  merlin_stream_init(&ch_b,1024,"int",sizeof(int ));
  merlin_stream_init(&ch_c,1024,"int",sizeof(int ));
  
#pragma ACCEL spawn_kernel
  prefetch_a(a);
  
#pragma ACCEL spawn_kernel
  prefetch_b(b);
  
#pragma ACCEL spawn_kernel
  compute(inc);
  
#pragma ACCEL spawn_kernel
  prefetch_c(c);
  merlin_stream_reset(&ch_a);
  merlin_stream_reset(&ch_b);
  merlin_stream_reset(&ch_c);
}
