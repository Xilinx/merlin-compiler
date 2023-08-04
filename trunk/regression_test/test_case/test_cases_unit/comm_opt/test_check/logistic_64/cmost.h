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
//#ifndef _CMOST_H_INCLUDED_
//#define _CMOST_H_INCLUDED_

#include <stdio.h>
//#include <stdlib.h>
#include <assert.h>
#include <math.h>


//extern void * malloc(int size);
//extern void free(void*) __THROW ;
//__BEGIN_NAMESPACE_STD
//extern void free (void *__ptr) __THROW;
//__END_NAMESPACE_STD


//#include "cmost_test_common.h"
//#include "cl_platform.h"

#define CMOST_CC
#define CMOSTCC 


#define FPGA_DDR_USER_BASE (0xc0000000)


int cmost_malloc(void ** a, int size );
int cmost_malloc_1d(void ** a, char * filename, int unit_size, int d0 );
int cmost_malloc_2d(void *** a, char * filename, int unit_size, int d0, int d1 );
int cmost_malloc_3d(void **** a, char * filename, int unit_size, int d0, int d1, int d2 );
int altera_malloc(void ** a, char * filename, int unit_size, int d0 );

int cmost_free_1d(void * a);
int cmost_free_2d(void ** a);
int cmost_free_3d( void *** a);

int cmost_dump_1d(void * a, char * filename);
int cmost_dump_2d(void ** a, char * filename);
int cmost_dump_3d(void *** a, char * filename);
void cmost_break_point();

void cmost_write_file(void * buffer, char * file_name, int size);
void cmost_load_file(void * buffer, char * file_name, int size);

void  cmost_start_timer(int i);
void  cmost_read_timer_new(int i, float * sec);   // return the time in seconds

int cmost_free(void * a);
int cmost_dump(void * a, char * filename);


//#endif //_CMOST_H_INCLUDED_


