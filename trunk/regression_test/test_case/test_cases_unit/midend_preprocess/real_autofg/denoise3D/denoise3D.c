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
#include <math.h>
#define IMG_P 62 
#define IMG_N 62 
#define IMG_M 62 

void initial(float u[64][64][64]){
	
	int p, n, m;
	for (p = 0; p < IMG_P + 2; p++) 
		for (n = 0; n < IMG_N + 2; n++) 
			for (m = 0; m < IMG_M + 2; m++) {
				u[p][n][m]=p+n+m;
			}

}

void calculate(float u[64][64][64],float g[64][64][64],float f[64][64][64],float u_out[64][64][64]){
	float sigma = 0.05f;
	float DT = 5.0f;
	float lambda = 0.065f;
	float EPSILON = 5e-5;
	float sigma2 = (sigma * sigma);
	float gamma1 = (lambda / sigma2);

	int p, n, m;
	float u_1, u_2, u_3, u_4, u_5, u_6, u_7;
	float r, g_1, g_2, g_3, g_4, g_5, g_6, g_7;
 
       	register short lbv, ubv, lb, ub, lb1, ub1, lb2, ub2;
  	register short c1, c2, c3, c4, c5, c6;
	
  
	for (p = 1; p < IMG_P + 1; p++) 
		for (n = 1; n < IMG_N + 1; n++) 
			for (m = 1; m < IMG_M + 1; m++) {
				u_1=u[p][n][m];
				u_2=u[p][n][m - 1];
				u_3=u[p][n][m + 1];
				u_4=u[p][n - 1][m];
				u_5=u[p][n + 1][m];
				u_6=u[p - 1][n][m];
				u_7=u[p + 1][n][m];

				g_2=g[p][n][m - 1];
				g_3=g[p][n][m + 1];
				g_4=g[p][n - 1][m];
				g_5=g[p][n + 1][m];
				g_6=g[p - 1][n][m];
				g_7=g[p + 1][n][m];

				r = ((u_1 * f[p][n][m]) / sigma2);
				r = ((r * (2.38944f + (r * (0.950037f + r)))) / (4.65314f + (r * (2.57541f + (r * (1.48937f + r))))));
				u_out[p][n][m] = ((u_1 + (DT * (((((((u_2 * g_2) + (u_3 * g_3)) + (u_4 * g_4)) + (u_5 * g_5)) + (u_6 * g_6)) + (u_7 * g_7)) + ((gamma1 * f[p][n][m]) * r)))) / (1.0f + (DT * (g_2 + g_3 + g_4 + g_5 + g_6 + g_7 + gamma1))));
			}
}
#pragma ACCEL kernel
void kernel_denoise(/*float f[64][64][64],float u_out[64][64][64],int IMG_P,int IMG_N,int IMG_M*/)
{

    float u_out[64+2][64+2][64+2];
	/* computation */
	float sigma = 0.05f;
	float DT = 5.0f;
	float lambda = 0.065f;
	float EPSILON = 5e-5;
	float sigma2 = (sigma * sigma);
	float gamma1 = (lambda / sigma2);
	int p, n, m;
	//float u[p][n][m];
	float u_1, u_2, u_3, u_4, u_5, u_6, u_7;
	float r, g_1, g_2, g_3, g_4, g_5, g_6, g_7;
	float u[64][64][64], g[64][64][64];
	float f[64][64][64];
	int q=0;

	initial(u);

	for (p = 1; p < IMG_P + 1; p++) 
		for (n = 1; n < IMG_N + 1; n++) 
			for (m = 1; m < IMG_M + 1; m++) {
				u_1=u[p][n][m];
				u_2=u[p][n][m - 1];
				u_3=u[p][n][m + 1];
				u_4=u[p][n - 1][m];
				u_5=u[p][n + 1][m];
				u_6=u[p - 1][n][m];
				u_7=u[p + 1][n][m];
				g[p][n][m] = (1.0f / sqrtf(((((((EPSILON + ((u_1 - u_2) * (u_1 - u_2))) + ((u_1 - u_3) * (u_1 - u_3))) + ((u_1 - u_4) * (u_1 - u_4))) + ((u_1 - u_5) * (u_1 - u_5))) + ((u_1 - u_6) * (u_1 - u_6))) + ((u_1 - u_7) * (u_1 - u_7)))));
			}

        calculate(u,g,f,u_out);
}


