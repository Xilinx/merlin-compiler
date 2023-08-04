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
#include <assert.h>
#include <string.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_521();
void CompressFragment_kernel(const char *input,unsigned long input_size,char *op,unsigned short *table,const int table_size,int merlin_return_value[1]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

int __merlinwrapper_CompressFragment_kernel(const char *input,unsigned long input_size,char *op,unsigned short *table,const int table_size)
{
  int merlin_return_value[1];
{
    
#pragma ACCEL wrapper variable=input depth=65536 max_depth=65536 data_type=char io=RO copy=false port=merlin_input
    
#pragma ACCEL wrapper variable=input_size port=input_size
    
#pragma ACCEL wrapper variable=op port=op depth=65536 max_depth=65536 data_type=char io=RW copy=false
    
#pragma ACCEL wrapper variable=table port=table depth=256 max_depth=256 data_type="unsigned short" io=RW copy=true
    
#pragma ACCEL wrapper variable=table_size port=table_size
    
#pragma ACCEL wrapper variable=merlin_return_value port=merlin_return_value depth=1 max_depth=1 data_type=int io=WO copy=true scope_type=return
  }
  static char __m_merlin_input[65536];
  static char __m_op[65536];
  static unsigned short __m_table[256];
  static int __m_merlin_return_value[1];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "merlin_input", "65536 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (input != 0) {
      memcpy((void *)(__m_merlin_input + 0),(const void *)input,65536 * sizeof(char ));
    }
     else {
      printf("Error! Detected null pointer 'input' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "merlin_input");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "op", "65536 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (op != 0) {
      memcpy((void *)(__m_op + 0),(const void *)op,65536 * sizeof(char ));
    }
     else {
      printf("Error! Detected null pointer 'op' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "op");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "table", "256 * sizeof(unsigned short )");
    
#pragma ACCEL debug fflush(stdout);
    if (table != 0) {
      memcpy((void *)(__m_table + 0),(const void *)table,256 * sizeof(unsigned short ));
    }
     else {
      printf("Error! Detected null pointer 'table' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "table");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL kernel name=CompressFrangment_kernel
  CompressFragment_kernel(__m_merlin_input,input_size,__m_op,__m_table,table_size,__m_merlin_return_value);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "op", "65536 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (op != 0) {
      memcpy((void *)op,(const void *)(__m_op + 0),65536 * sizeof(char ));
    }
     else {
      printf("Error! Detected null pointer 'op' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "op");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "table", "256 * sizeof(unsigned short )");
    
#pragma ACCEL debug fflush(stdout);
    if (table != 0) {
      memcpy((void *)table,(const void *)(__m_table + 0),256 * sizeof(unsigned short ));
    }
     else {
      printf("Error! Detected null pointer 'table' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "table");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_return_value", "1 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (merlin_return_value != 0) {
      memcpy((void *)merlin_return_value,(const void *)(__m_merlin_return_value + 0),1 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'merlin_return_value' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_return_value");
    
#pragma ACCEL debug fflush(stdout);
  }
  return  *merlin_return_value;
}

int __merlin_CompressFragment_kernel(const char *input,unsigned long input_size,char *op,unsigned short *table,const int table_size)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_CompressFragment_kernel();
  int __merlin_return = __merlinwrapper_CompressFragment_kernel(input,input_size,op,table,table_size);
  
#pragma ACCEL string __merlin_release_CompressFragment_kernel();
  return __merlin_return;
}

int __merlin_write_buffer_CompressFragment_kernel(const char *input,unsigned long input_size,char *op,unsigned short *table,const int table_size)
{
}

int __merlin_read_buffer_CompressFragment_kernel(const char *input,unsigned long input_size,char *op,unsigned short *table,const int table_size)
{
}

int __merlin_execute_CompressFragment_kernel(const char *input,unsigned long input_size,char *op,unsigned short *table,const int table_size)
{
}
