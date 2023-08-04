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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_431();
void vec_add_kernel(int *a, int *b);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

int __merlinwrapper_vec_add_kernel(int *a, int *b) {
  {

#pragma ACCEL wrapper variable = a port = a depth = 10000 max_depth =          \
    10000 data_type = int io = RO copy = false

#pragma ACCEL wrapper variable = b port = b depth = 10000 max_depth =          \
    10000 data_type = int io = WO copy = false
  }
  static int __m_a[10000];
  static int __m_b[10000];
  /*
      static int __m_a[10000];
      static int __m_b[10000];
  */
  {

#pragma ACCEL debug printf(                                                    \
    "[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", \
    "10000 * sizeof(int )");

#pragma ACCEL debug fflush(stdout);
    if (a != ((int *)0)) {
      memcpy((void *)(__m_a + 0), (const void *)a,
             ((unsigned long)10000) * sizeof(int));
    } else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }

#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n",           \
                           "copy in", "a");

#pragma ACCEL debug fflush(stdout);
  }

#pragma ACCEL kernel
  vec_add_kernel(__m_a, __m_b);
  /*
      static int __m_a[10000];
      static int __m_b[10000];
  */
  {

#pragma ACCEL debug printf(                                                    \
    "[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out",     \
    "b", "10000 * sizeof(int )");

#pragma ACCEL debug fflush(stdout);
    if (b != ((int *)0)) {
      memcpy((void *)b, (const void *)(__m_b + 0),
             ((unsigned long)10000) * sizeof(int));
    } else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }

#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n",           \
                           "copy out", "b");

#pragma ACCEL debug fflush(stdout);
  }
}

int __merlin_vec_add_kernel(int *a, int *b) {

#pragma ACCEL string __merlin_check_opencl();

#pragma ACCEL string __merlin_init_vec_add_kernel();
  int __merlin_return = __merlinwrapper_vec_add_kernel(a, b);

#pragma ACCEL string __merlin_release_vec_add_kernel();
  return __merlin_return;
}

int __merlin_write_buffer_vec_add_kernel(int *a, int *b) {
  static int __m_a[10000];
  static int __m_b[10000];
  {

#pragma ACCEL debug printf(                                                    \
    "[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", \
    "10000 * sizeof(int )");

#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      memcpy((void *)(__m_a + 0), (const void *)a, 10000 * sizeof(int));
    } else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }

#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n",           \
                           "copy in", "a");

#pragma ACCEL debug fflush(stdout);
  }
}

int __merlin_read_buffer_vec_add_kernel(int *a, int *b) {
  static int __m_a[10000];
  static int __m_b[10000];
  {

#pragma ACCEL debug printf(                                                    \
    "[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out",     \
    "b", "10000 * sizeof(int )");

#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      memcpy((void *)b, (const void *)(__m_b + 0), 10000 * sizeof(int));
    } else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }

#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n",           \
                           "copy out", "b");

#pragma ACCEL debug fflush(stdout);
  }
}

int __merlin_execute_vec_add_kernel(int *a, int *b) {}
