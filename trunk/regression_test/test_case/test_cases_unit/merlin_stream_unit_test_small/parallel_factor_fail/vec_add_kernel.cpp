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
merlin_stream ch_a[2];
merlin_stream ch_b[2];
merlin_stream ch_c[2];

void compute(int id)
{
  int j;
  for (j = 0; j < 10240; j++) {
    int tmp_a;
    int tmp_b;
    int tmp_c;
    merlin_stream_read(&ch_a[id],(&tmp_a));
    merlin_stream_read(&ch_b[id],(&tmp_b));
    tmp_c = tmp_a + tmp_b + 1;
    merlin_stream_write(&ch_c[id],(&tmp_c));
  }
}
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel(int a[10240 * 100 * 2],int b[10240 * 100 * 2],int c[10240 * 100 * 2],int inc)
{
  
#pragma ACCEL interface variable=c depth=2048000
  
#pragma ACCEL interface variable=b depth=2048000
  
#pragma ACCEL interface variable=a depth=2048000
  int i1;
  int i2;
  int j;
  for (j = 0; j < 2; j++) {
    merlin_stream_init(&ch_a[j],10240,"int",sizeof(int ));
    merlin_stream_init(&ch_b[j],10240,"int",sizeof(int ));
    merlin_stream_init(&ch_c[j],10240,"int",sizeof(int ));
  }
  for (i1 = 0; i1 < 100; i1++) {
    for (i2 = 0; i2 < 10240; i2++) {
      
#pragma ACCEL parallel
      for (j = 0; j < 2; j++) {
        merlin_stream_write(&ch_a[j],(&a[i1 * 2 * 10240 + i2 * 2 + j]));
      }
    }
    for (i2 = 0; i2 < 10240; i2++) {
      
#pragma ACCEL parallel
      for (j = 0; j < 2; j++) {
        merlin_stream_write(&ch_b[j],(&b[i1 * 2 * 10240 + i2 * 2 + j]));
      }
    }
    for (i2 = 0; i2 < 2; i2++) {
      
#pragma ACCEL spawn_kernel factor=2
      compute(i2);
    }
    for (i2 = 0; i2 < 10240; i2++) {
      
#pragma ACCEL parallel
      for (j = 0; j < 2; j++) {
        merlin_stream_read(&ch_c[j],(&c[i1 * 2 * 10240 + i2 * 2 + j]));
      }
    }
  }
  for (j = 0; j < 2; j++) {
    merlin_stream_reset(&ch_a[j]);
    merlin_stream_reset(&ch_b[j]);
    merlin_stream_reset(&ch_c[j]);
  }
}
