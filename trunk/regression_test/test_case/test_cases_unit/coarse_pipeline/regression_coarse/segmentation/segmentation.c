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
#include <math.h>

//#define m 16
//#define n 16
//#define p 16
//#define SIZE m*n*p
#define PITCH_P 258
#define PITCH_N 258
#define PITCH_M 258

#define min(x,y) ((x<y)?x:y)

#define curvature_motion_part(i,j,k)  curvature_motion_part_g[((i)*PITCH_N+(j))*PITCH_M+k]
#define PPHI(i,j,k) phi[((i)*PITCH_N+(j))*PITCH_M+k]
#define PU0(i,j,k) interpt[((i)*PITCH_N+(j))*PITCH_M+k]
#define epsilon 10e-5

//#define curvature_motion_part(i,j,k)  curvature_motion_part_g[i][j][k]
//#define PPHI(i,j,k) phi[i][j][k]
//#define PU0(i,j,k) interpt[i][j][k]

//int* curvature_motion_part_g;// global array ; no larger than 256^3 :)

void seg_0_kernel(float* interpt, float * curvature_motion_part_g,  float* phi, int IMG_P,int IMG_N,int IMG_M,int TS_P,int TS_N,int TS_M, float * num);
void seg_1_kernel(float* interpt, float * curvature_motion_part_g,  float* phi, int IMG_P,int IMG_N,int IMG_M,int TS_P,int TS_N,int TS_M, float * num);
void seg_2_kernel(float* interpt, float * curvature_motion_part_g,  float* phi, int IMG_P,int IMG_N,int IMG_M,int TS_P,int TS_N,int TS_M, float * num);

void init_phi(float* phi, float* curvature_motion_part_g)
{

	//
	//  This routine inputs the initial condition for phi.
	//
	//int epsilon = 10e-5;

	int xcent = (int)(floor((PITCH_M-1) / 2));
	int ycent = (int)(floor((PITCH_N-1) / 2));
	int zcent = (int)(floor((PITCH_P-1) / 2));
	float r = (min( min(PITCH_M,PITCH_N), PITCH_P )) / 3;

	int xx; int yy; int zz;

	long i; 
	long j;
	long k;
	for(i=0; i < PITCH_P; i++)
	{
		for(j=0; j < PITCH_N; j++)
		{
			for(k=0; k < PITCH_M; k++)
			{
				xx = (int)(i);
				yy = (int)(j);
				zz = (int)(k);
				PPHI(i,j,k) = ( (xx-xcent)*(xx-xcent) + (yy-ycent)*(yy-ycent) + (zz-zcent)*(zz-zcent)) - r;
				curvature_motion_part(i,j,k)=0;

			}
		}
	}
}


void segmentation(float* interpt, float * curvature_motion_part_g,  float* phi, int IMG_P, int IMG_N, int IMG_M, int TS_P, int TS_N, int TS_M)
{

// this pragma reduce the size of the multi-dimensional array for 

    float * c1; cmost_malloc_1d(&c1, "zero.dat", 4, 4);
    float * c2; cmost_malloc_1d(&c2, "zero.dat", 4, 4);

	/*
	 * BE CAREFUL
	 *	remove below function for verification only
	 */
	//	init_phi(phi1, curvature_motion_part_g);	
	init_phi(phi, curvature_motion_part_g);

	float * num; cmost_malloc_1d(&num, "zero.dat", 4, 4);
	int iter;
	for(iter=0;iter<1;iter++)
	{
        	int IMG_TS_P = IMG_P / TS_P;
        	int IMG_TS_M = IMG_M / TS_M;
        	int IMG_TS_N = IMG_N / TS_N;

		//float * phi_1 = phi;
		int i, j, k;

		for (i = 0; i < 4; i++) num[i] = 0.0;


		seg_0_kernel(interpt, curvature_motion_part_g, phi, IMG_P, IMG_N, IMG_M, TS_P, TS_N, TS_M, num);

#if 0			
		seg_1_kernel(interpt, curvature_motion_part_g, phi, IMG_P, IMG_N, IMG_M, TS_P, TS_N, TS_M, num);
		
		seg_2_kernel(interpt, curvature_motion_part_g, phi, IMG_P, IMG_N, IMG_M, TS_P, TS_N, TS_M, num);
#endif		
	}
	//printf("c1 = %f\t c2 = %f\n\r", c1, c2);
    cmost_free_1d(c1);
    cmost_free_1d(c2);
    cmost_free_1d(num);
}

#if 0
int main( )
{
    return 0;
}
#endif
