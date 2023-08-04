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
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_439();
const unsigned char sbox[256] = {(0x63), (0x7c), (0x77), (0x7b), (0xf2), (0x6b), (0x6f), (0xc5), (0x30), (0x01), (0x67), (0x2b), (0xfe), (0xd7), (0xab), (0x76), (0xca), (0x82), (0xc9), (0x7d), (0xfa), (0x59), (0x47), (0xf0), (0xad), (0xd4), (0xa2), (0xaf), (0x9c), (0xa4), (0x72), (0xc0), (0xb7), (0xfd), (0x93), (0x26), (0x36), (0x3f), (0xf7), (0xcc), (0x34), (0xa5), (0xe5), (0xf1), (0x71), (0xd8), (0x31), (0x15), (0x04), (0xc7), (0x23), (0xc3), (0x18), (0x96), (0x05), (0x9a), (0x07), (0x12), (0x80), (0xe2), (0xeb), (0x27), (0xb2), (0x75), (0x09), (0x83), (0x2c), (0x1a), (0x1b), (0x6e), (0x5a), (0xa0), (0x52), (0x3b), (0xd6), (0xb3), (0x29), (0xe3), (0x2f), (0x84), (0x53), (0xd1), (0x00), (0xed), (0x20), (0xfc), (0xb1), (0x5b), (0x6a), (0xcb), (0xbe), (0x39), (0x4a), (0x4c), (0x58), (0xcf), (0xd0), (0xef), (0xaa), (0xfb), (0x43), (0x4d), (0x33), (0x85), (0x45), (0xf9), (0x02), (0x7f), (0x50), (0x3c), (0x9f), (0xa8), (0x51), (0xa3), (0x40), (0x8f), (0x92), (0x9d), (0x38), (0xf5), (0xbc), (0xb6), (0xda), (0x21), (0x10), (0xff), (0xf3), (0xd2), (0xcd), (0x0c), (0x13), (0xec), (0x5f), (0x97), (0x44), (0x17), (0xc4), (0xa7), (0x7e), (0x3d), (0x64), (0x5d), (0x19), (0x73), (0x60), (0x81), (0x4f), (0xdc), (0x22), (0x2a), (0x90), (0x88), (0x46), (0xee), (0xb8), (0x14), (0xde), (0x5e), (0x0b), (0xdb), (0xe0), (0x32), (0x3a), (0x0a), (0x49), (0x06), (0x24), (0x5c), (0xc2), (0xd3), (0xac), (0x62), (0x91), (0x95), (0xe4), (0x79), (0xe7), (0xc8), (0x37), (0x6d), (0x8d), (0xd5), (0x4e), (0xa9), (0x6c), (0x56), (0xf4), (0xea), (0x65), (0x7a), (0xae), (0x08), (0xba), (0x78), (0x25), (0x2e), (0x1c), (0xa6), (0xb4), (0xc6), (0xe8), (0xdd), (0x74), (0x1f), (0x4b), (0xbd), (0x8b), (0x8a), (0x70), (0x3e), (0xb5), (0x66), (0x48), (0x03), (0xf6), (0x0e), (0x61), (0x35), (0x57), (0xb9), (0x86), (0xc1), (0x1d), (0x9e), (0xe1), (0xf8), (0x98), (0x11), (0x69), (0xd9), (0x8e), (0x94), (0x9b), (0x1e), (0x87), (0xe9), (0xce), (0x55), (0x28), (0xdf), (0x8c), (0xa1), (0x89), (0x0d), (0xbf), (0xe6), (0x42), (0x68), (0x41), (0x99), (0x2d), (0x0f), (0xb0), (0x54), (0xbb), (0x16)};
void aes256_encrypt_ecb_kernel(unsigned char *ctx_key,unsigned char *ctx_enckey,unsigned char *ctx_deckey,unsigned char k[32],unsigned char buf[16]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_aes256_encrypt_ecb_kernel(aes256_context *ctx,unsigned char k[32],unsigned char buf[16])
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=ctx->key port=ctx_key depth=1,32 max_depth=1,32 data_type="unsigned char" io=RW copy=true
    
#pragma ACCEL wrapper variable=ctx->enckey port=ctx_enckey depth=1,32 max_depth=1,32 data_type="unsigned char" io=RW copy=true
    
#pragma ACCEL wrapper variable=ctx->deckey port=ctx_deckey depth=1,32 max_depth=1,32 data_type="unsigned char" io=RW copy=true
    
#pragma ACCEL wrapper variable=k port=k depth=32 max_depth=32 data_type="unsigned char" io=RO copy=true
    
#pragma ACCEL wrapper variable=buf port=buf depth=16 max_depth=16 data_type="unsigned char" io=RW copy=true
  }
  static __CMOST_COMMENT__ __m_ctx_key;
  static __CMOST_COMMENT__ __m_ctx_enckey;
  static __CMOST_COMMENT__ __m_ctx_deckey;
  static __CMOST_COMMENT__ __m_k;
  static __CMOST_COMMENT__ __m_buf;
{
    int _i0;
/*
    static unsigned char __m_ctx_key[32];
    static unsigned char __m_ctx_enckey[32];
    static unsigned char __m_ctx_deckey[32];
    static unsigned char __m_k[32];
    static unsigned char __m_buf[16];
*/
    for (_i0 = 0; _i0 < 1; ++_i0) {
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "ctx_key", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (ctx != 0 && ctx[_i0] . key != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_key_buffer;
        opencl_write_buffer(__aes256_encrypt_ecb_kernel__ctx_key_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . key,((unsigned long )32) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'ctx[_i0] . key' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "ctx_key");
      
#pragma ACCEL debug fflush(stdout);
    }
    for (_i0 = 0; _i0 < 1; ++_i0) {
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "ctx_enckey", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (ctx != 0 && ctx[_i0] . enckey != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
        opencl_write_buffer(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . enckey,((unsigned long )32) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'ctx[_i0] . enckey' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "ctx_enckey");
      
#pragma ACCEL debug fflush(stdout);
    }
    for (_i0 = 0; _i0 < 1; ++_i0) {
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "ctx_deckey", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (ctx != 0 && ctx[_i0] . deckey != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
        opencl_write_buffer(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . deckey,((unsigned long )32) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'ctx[_i0] . deckey' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "ctx_deckey");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "k", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (k != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__k_buffer;
        opencl_write_buffer(__aes256_encrypt_ecb_kernel__k_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)k,((unsigned long )32) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'k' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "k");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "buf", "16 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (buf != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__buf_buffer;
        opencl_write_buffer(__aes256_encrypt_ecb_kernel__buf_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)buf,((unsigned long )16) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'buf' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "buf");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 0, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__ctx_key_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 1, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 2, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 3, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__k_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 4, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__buf_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__aes256_encrypt_ecb_kernel_kernel, commands[0], 1, __gid, &__event_aes256_encrypt_ecb_kernel);
}
{
	clWaitForEvents(1, &__event_aes256_encrypt_ecb_kernel);
	if(__event_aes256_encrypt_ecb_kernel) {
		opencl_release_event(__event_aes256_encrypt_ecb_kernel);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
    int _i0;
/*
    static unsigned char __m_ctx_key[32];
    static unsigned char __m_ctx_enckey[32];
    static unsigned char __m_ctx_deckey[32];
    static unsigned char __m_k[32];
    static unsigned char __m_buf[16];
*/
    for (_i0 = 0; _i0 < 1; ++_i0) {
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "ctx_key", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (ctx != 0 && ctx[_i0] . key != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_key_buffer;
        opencl_read_buffer(__aes256_encrypt_ecb_kernel__ctx_key_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . key,((unsigned long )32) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'ctx[_i0] . key' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "ctx_key");
      
#pragma ACCEL debug fflush(stdout);
    }
    for (_i0 = 0; _i0 < 1; ++_i0) {
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "ctx_enckey", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (ctx != 0 && ctx[_i0] . enckey != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
        opencl_read_buffer(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . enckey,((unsigned long )32) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'ctx[_i0] . enckey' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "ctx_enckey");
      
#pragma ACCEL debug fflush(stdout);
    }
    for (_i0 = 0; _i0 < 1; ++_i0) {
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "ctx_deckey", "32 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (ctx != 0 && ctx[_i0] . deckey != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
        opencl_read_buffer(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . deckey,((unsigned long )32) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'ctx[_i0] . deckey' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "ctx_deckey");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "buf", "16 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (buf != 0) {
        __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__buf_buffer;
        opencl_read_buffer(__aes256_encrypt_ecb_kernel__buf_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)buf,((unsigned long )16) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'buf' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "buf");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_aes256_encrypt_ecb_kernel(aes256_context *ctx,unsigned char k[32],unsigned char buf[16])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_aes256_encrypt_ecb_kernel();
  __merlinwrapper_aes256_encrypt_ecb_kernel(ctx,k,buf);
  
#pragma ACCEL string __merlin_release_aes256_encrypt_ecb_kernel();
}

void __merlin_write_buffer_aes256_encrypt_ecb_kernel(aes256_context *ctx,unsigned char k[32],unsigned char buf[16])
{
  int _i0;
  static unsigned char __m_ctx_key[32];
  static unsigned char __m_ctx_enckey[32];
  static unsigned char __m_ctx_deckey[32];
  static unsigned char __m_k[32];
  static unsigned char __m_buf[16];
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "ctx_key", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (ctx != 0 && ctx[_i0] . key != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_key_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__ctx_key_buffer;
/* __MERLIN_EVENT_WRITE__ __event___aes256_encrypt_ecb_kernel__ctx_key_buffer */
      int __MERLIN_EVENT_WRITE____event___aes256_encrypt_ecb_kernel__ctx_key_buffer;
      opencl_write_buffer_nb(__aes256_encrypt_ecb_kernel__ctx_key_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . key,((unsigned long )32) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__ctx_key_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'ctx[_i0] . key' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "ctx_key");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "ctx_enckey", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (ctx != 0 && ctx[_i0] . enckey != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
/* __MERLIN_EVENT_WRITE__ __event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer */
      int __MERLIN_EVENT_WRITE____event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
      opencl_write_buffer_nb(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . enckey,((unsigned long )32) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'ctx[_i0] . enckey' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "ctx_enckey");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "ctx_deckey", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (ctx != 0 && ctx[_i0] . deckey != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
/* __MERLIN_EVENT_WRITE__ __event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer */
      int __MERLIN_EVENT_WRITE____event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
      opencl_write_buffer_nb(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . deckey,((unsigned long )32) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'ctx[_i0] . deckey' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "ctx_deckey");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "k", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (k != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__k_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__k_buffer;
/* __MERLIN_EVENT_WRITE__ __event___aes256_encrypt_ecb_kernel__k_buffer */
      int __MERLIN_EVENT_WRITE____event___aes256_encrypt_ecb_kernel__k_buffer;
      opencl_write_buffer_nb(__aes256_encrypt_ecb_kernel__k_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)k,((unsigned long )32) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__k_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'k' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "k");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "buf", "16 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (buf != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__buf_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__buf_buffer;
/* __MERLIN_EVENT_WRITE__ __event___aes256_encrypt_ecb_kernel__buf_buffer */
      int __MERLIN_EVENT_WRITE____event___aes256_encrypt_ecb_kernel__buf_buffer;
      opencl_write_buffer_nb(__aes256_encrypt_ecb_kernel__buf_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)buf,((unsigned long )16) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__buf_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'buf' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "buf");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_aes256_encrypt_ecb_kernel(aes256_context *ctx,unsigned char k[32],unsigned char buf[16])
{
  int _i0;
  static unsigned char __m_ctx_key[32];
  static unsigned char __m_ctx_enckey[32];
  static unsigned char __m_ctx_deckey[32];
  static unsigned char __m_k[32];
  static unsigned char __m_buf[16];
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "ctx_key", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (ctx != 0 && ctx[_i0] . key != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_key_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__ctx_key_buffer;
/* __MERLIN_EVENT_READ__ __event___aes256_encrypt_ecb_kernel__ctx_key_buffer */
      int __MERLIN_EVENT_READ____event___aes256_encrypt_ecb_kernel__ctx_key_buffer;
      opencl_read_buffer_nb(__aes256_encrypt_ecb_kernel__ctx_key_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . key,((unsigned long )32) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__ctx_key_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'ctx[_i0] . key' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "ctx_key");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "ctx_enckey", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (ctx != 0 && ctx[_i0] . enckey != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
/* __MERLIN_EVENT_READ__ __event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer */
      int __MERLIN_EVENT_READ____event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
      opencl_read_buffer_nb(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . enckey,((unsigned long )32) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'ctx[_i0] . enckey' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "ctx_enckey");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "ctx_deckey", "32 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (ctx != 0 && ctx[_i0] . deckey != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
/* __MERLIN_EVENT_READ__ __event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer */
      int __MERLIN_EVENT_READ____event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
      opencl_read_buffer_nb(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer,commands[0],_i0 * 32 * sizeof(unsigned char ),(unsigned char *)ctx[_i0] . deckey,((unsigned long )32) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'ctx[_i0] . deckey' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "ctx_deckey");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "buf", "16 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (buf != 0) {
      __CMOST_COMMENT__ __aes256_encrypt_ecb_kernel__buf_buffer;
      __CMOST_COMMENT__ __event___aes256_encrypt_ecb_kernel__buf_buffer;
/* __MERLIN_EVENT_READ__ __event___aes256_encrypt_ecb_kernel__buf_buffer */
      int __MERLIN_EVENT_READ____event___aes256_encrypt_ecb_kernel__buf_buffer;
      opencl_read_buffer_nb(__aes256_encrypt_ecb_kernel__buf_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)buf,((unsigned long )16) * sizeof(unsigned char ),&__event___aes256_encrypt_ecb_kernel__buf_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'buf' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "buf");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_aes256_encrypt_ecb_kernel(aes256_context *ctx,unsigned char k[32],unsigned char buf[16])
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 0, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__ctx_key_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 1, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 2, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 3, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__k_buffer);
	opencl_set_kernel_arg(__aes256_encrypt_ecb_kernel_kernel, 4, sizeof(opencl_mem), &__aes256_encrypt_ecb_kernel__buf_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__aes256_encrypt_ecb_kernel_kernel, commands[0], 1, __gid, &__event_aes256_encrypt_ecb_kernel);
}
}
