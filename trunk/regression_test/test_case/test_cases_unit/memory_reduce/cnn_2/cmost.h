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

// This file is located in legacy/drivers/pure_cpu_headers/cmost.h

#ifndef __CMOST_H_INCLUDED__
#define __CMOST_H_INCLUDED__
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#else
#include <stddef.h>
#endif
//#include "cmost_test_common.h"
//#include "cl_platform.h"

#define CMOST_CC
#define CMOSTCC 
#define _MARS_COMPILER_FLOW_


#define FPGA_DDR_USER_BASE (0xc0000000)

#ifdef __cplusplus
extern "C" {
#endif
int cmost_malloc(void ** a, int size );
int cmost_malloc_1d(void ** a, const char * filename, int unit_size, int d0 );

int cmost_malloc_2d(void *** a, const char * filename, int unit_size, int d0, int d1 );

int cmost_malloc_3d(void **** a, const char * filename, int unit_size, int d0, int d1, int d2 );

int cmost_free_1d(void * a);
int cmost_free_2d(void ** a);
int cmost_free_3d( void *** a);

int cmost_dump_1d(void * a, const char * filename);
int cmost_dump_2d(void ** a, const char * filename);
int cmost_dump_3d(void *** a, const char * filename);
void cmost_break_point();

void cmost_write_file(void * buffer, const char * file_name, int size);
void cmost_load_file(void * buffer, const char * file_name, int size);

void  cmost_start_timer(int i);
void  cmost_read_timer_new(int i, float * sec);   // return the time in seconds

int cmost_free(void * a);
int cmost_dump(void * a, const char * filename);
//int user_main();
int user_main_wrapper(int argc, char ** argv);
void __merlin_access_range(
#ifdef __cplusplus
    ...
#endif
    );
#ifdef __cplusplus
}
#endif
#endif //_CMOST_H_INCLUDED_



