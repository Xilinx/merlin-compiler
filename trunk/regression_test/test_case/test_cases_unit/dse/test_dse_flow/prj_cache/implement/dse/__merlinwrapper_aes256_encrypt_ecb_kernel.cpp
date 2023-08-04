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
#include "merlin_type_define.h"
extern int __merlin_include__GB_stdlib_h_2;
extern int __merlin_include__GB_stdio_h_1;
extern int __merlin_include__GB_string_h_0;
static void __merlin_dummy_432();
void aes256_encrypt_ecb_kernel(unsigned char key[32],unsigned char data[16 * (16 * 1024 * 1024)]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_aes256_encrypt_ecb_kernel(unsigned char key[32],unsigned char data[16 * (16 * 1024 * 1024)])
{
{
    
#pragma ACCEL wrapper variable=key port=key depth=32 max_depth=32 data_type="unsigned char" io=RO copy=true
    
#pragma ACCEL wrapper variable=data port=data depth=268435456 max_depth=268435456 data_type="unsigned char" io=RW copy=false
  }
  static unsigned char __m_key[32];
  static unsigned char __m_data[268435456];
{
/*
    static unsigned char __m_key[32];
    static unsigned char __m_data[268435456];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "key", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (key != 0) {
        memcpy((void *)(__m_key + 0),(const void *)key,32 * sizeof(unsigned char ));
      }
       else {
        printf("Error! Detected null pointer 'key' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "key");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "data", "268435456 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (data != 0) {
        memcpy((void *)(__m_data + 0),(const void *)data,268435456 * sizeof(unsigned char ));
      }
       else {
        printf("Error! Detected null pointer 'data' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "data");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
  aes256_encrypt_ecb_kernel(__m_key,__m_data);
{
/*
    static unsigned char __m_key[32];
    static unsigned char __m_data[268435456];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "data", "268435456 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (data != 0) {
        memcpy((void *)data,(const void *)(__m_data + 0),268435456 * sizeof(unsigned char ));
      }
       else {
        printf("Error! Detected null pointer 'data' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "data");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_aes256_encrypt_ecb_kernel(unsigned char key[32],unsigned char data[16 * (16 * 1024 * 1024)])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_aes256_encrypt_ecb_kernel();
  __merlinwrapper_aes256_encrypt_ecb_kernel(key,data);
  
#pragma ACCEL string __merlin_release_aes256_encrypt_ecb_kernel();
}

void __merlin_write_buffer_aes256_encrypt_ecb_kernel(unsigned char key[32],unsigned char data[16 * (16 * 1024 * 1024)])
{
  static unsigned char __m_key[32];
  static unsigned char __m_data[268435456];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "key", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (key != 0) {
      memcpy((void *)(__m_key + 0),(const void *)key,32 * sizeof(unsigned char ));
    }
     else {
      printf("Error! Detected null pointer 'key' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "key");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "data", "268435456 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (data != 0) {
      memcpy((void *)(__m_data + 0),(const void *)data,268435456 * sizeof(unsigned char ));
    }
     else {
      printf("Error! Detected null pointer 'data' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "data");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_aes256_encrypt_ecb_kernel(unsigned char key[32],unsigned char data[16 * (16 * 1024 * 1024)])
{
  static unsigned char __m_key[32];
  static unsigned char __m_data[268435456];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "data", "268435456 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (data != 0) {
      memcpy((void *)data,(const void *)(__m_data + 0),268435456 * sizeof(unsigned char ));
    }
     else {
      printf("Error! Detected null pointer 'data' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "data");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_aes256_encrypt_ecb_kernel(unsigned char key[32],unsigned char data[16 * (16 * 1024 * 1024)])
{
}
