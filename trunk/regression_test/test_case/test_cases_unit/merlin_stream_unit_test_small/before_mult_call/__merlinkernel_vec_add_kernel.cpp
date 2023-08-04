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
#include "cmost.h"
#include <merlin_stream.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
merlin_stream ch_a;

static void prefetch_a_org(int a[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    merlin_stream_write(&ch_a,(void *)(&a[j]));
  }
}

int prefetch_a(int a[1024 * 1024])
{
  int i;
  for (i = 0; i < 1024; i++) {
    prefetch_a_org(a + i * 1024);
  }
}
merlin_stream ch_b;

static void prefetch_b_org(int b[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    merlin_stream_write(&ch_b,(void *)(&b[j]));
  }
}

static void prefetch_b(int b[1024 * 1024])
{
  int i;
  for (i = 0; i < 1024; i++) {
    prefetch_b_org(b + i * 1024);
  }
}
merlin_stream ch_c;

static void compute_org(int inc)
{
  int j;
  for (j = 0; j < 1024; j++) {
    int tmp_a;
    int tmp_b;
    int tmp_c;
    merlin_stream_read(&ch_a,(void *)(&tmp_a));
    merlin_stream_read(&ch_b,(void *)(&tmp_b));
    tmp_c = tmp_a + tmp_b + inc;
    merlin_stream_write(&ch_c,(void *)(&tmp_c));
  }
}

static void compute(int inc)
{
  int i;
#pragma ACCEL spawn_kernel
  for (i = 0; i < 1024; i++) {
    compute_org(inc);
  }
}

static void prefetch_c_org(int c[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    merlin_stream_read(&ch_c,(void *)(&c[j]));
  }
}

static void prefetch_c(int c[1024 * 1024])
{
  int i;
  for (i = 0; i < 1024; i++) {
    prefetch_c_org(c + i * 1024);
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

int adder() {
    return 1;
}

void vec_add_kernel(int a[1024 * 1024],int b[1024 * 1024],int c[1024 * 1024],int inc)
{
  
#pragma ACCEL interface variable=c max_depth=1048576 depth=1048576
  
#pragma ACCEL interface variable=b max_depth=1048576 depth=1048576
  
#pragma ACCEL interface variable=a max_depth=1048576 depth=1048576
  merlin_stream_init(&ch_a,(unsigned long )1024,"int",4);
  merlin_stream_init(&ch_b,(unsigned long )1024,"int",4);
  merlin_stream_init(&ch_c,(unsigned long )1024,"int",4);

#pragma ACCEL spawn_kernel
  int m = prefetch_a(a) + adder();

  
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
