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

#include "header.h"
#include "cmost.h"

//int get_global_id(int idx);
//void my_ap_wait() { ap_wait(); }


#define SIZE IMG_SIZE //IMG_M*IMG_N*IMG_P

#define RUN_DENOISE 1
#define RUN_REG 1
#define RUN_SEG 1

#define DEFINE_P 256
#define DEFINE_N 256
#define DEFINE_M 256

#define DEFINE_P_PITCH 258
#define DEFINE_N_PITCH 258
#define DEFINE_M_PITCH 258

#define DEFINE_size_PITCH 17173512
#define DEFINE_MULTI_PITCH 1000


void segmentation(float* interpt, float * curvature_motion_part_g,  float* phi, int IMG_P,int IMG_N,int IMG_M,int TS_P,int TS_N,int TS_M);

int main()
{
	int i;

#define nType 4

	int dim0 = DEFINE_P;
	int dim1 = DEFINE_N;
	int dim2 = DEFINE_M;

	float * interpt;                  cmost_malloc_1d(&interpt,                 "u_in.dat",   nType, DEFINE_size_PITCH);
	float * curvature_motion_part_g;  cmost_malloc_1d(&curvature_motion_part_g, "zero.dat",   nType, DEFINE_size_PITCH);
	float * phi;                      cmost_malloc_1d(&phi,                     "zero.dat" ,  nType, DEFINE_size_PITCH);

	segmentation(interpt, curvature_motion_part_g, phi, dim0, dim1, dim2, 8, 8, 256);

	cmost_dump_1d(curvature_motion_part_g, "g_out.dat");
	cmost_dump_1d(phi, "phi_out.dat");

	cmost_free_1d(interpt);
	cmost_free_1d(curvature_motion_part_g);
	cmost_free_1d(phi);

    return 0;
}

