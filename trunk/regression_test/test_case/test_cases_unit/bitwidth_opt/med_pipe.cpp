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



//#include "cmost.h"
#include "header.h"


void denoise(float *u,float *g,float *f,float *u_out);


#define SIZE IMG_SIZE
#define RUN_DENOISE 1
#define RUN_REG 1
#define RUN_SEG 1

#define DEFINE_P 8
#define DEFINE_N 512
#define DEFINE_M 512 

#define DEFINE_PNM_PITCH 2641960

int main()
{

    int dim0 = DEFINE_P;
    int dim1 = DEFINE_N;
    int dim2 = DEFINE_M;
    
    float * u		; //cmost_malloc_1d(&u    , "u_in.dat" ,  4, DEFINE_PNM_PITCH);
    float * u_out	; //cmost_malloc_1d(&u_out, "0",          4, DEFINE_PNM_PITCH);
    float * g		; //cmost_malloc_1d(&g    , "0" ,         4, DEFINE_PNM_PITCH);
    float * f		; //cmost_malloc_1d(&f    , "0" ,         4, DEFINE_PNM_PITCH);
    
    denoise(u, g, f, u_out);
    
    //cmost_dump_1d(g, "g_out.dat");
    //cmost_dump_1d(u_out, "u_out.dat");
    
    return 0;
}


