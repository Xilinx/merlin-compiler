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
#define epsilon 10e-5
#define PITCH_P 258
#define PITCH_N 258
#define PITCH_M 258

#define curvature_motion_part(i,j,k)  curvature_motion_part_g[((i)*PITCH_N+(j))*PITCH_M+k]
#define PPHI(i,j,k) phi[((i)*PITCH_N+(j))*PITCH_M+k]
#define PU0(i,j,k) interpt[((i)*PITCH_N+(j))*PITCH_M+k]

void seg_1_kernel(float* interpt, float * curvature_motion_part_g,  float* phi, int IMG_P,int IMG_N,int IMG_M,int TS_P,int TS_N,int TS_M, float * num)
{
    int IMG_TS_P = IMG_P / TS_P;
    int IMG_TS_M = IMG_M / TS_M;
    int IMG_TS_N = IMG_N / TS_N;
    float lambda1=1;
    float lambda2=1;
    float mu=0.18f*255.0f*255.0f;
    float dt=0.15f/mu;


    int i, j, k;
    int tp, tn, tm;

    for (tp = 0; tp < (IMG_TS_P); tp++) 
        for (tn = 0; tn < (IMG_TS_N); tn++) 
            for (tm = 0; tm < (IMG_TS_M); tm++) 
            {
                int mm, nn, pp;
                float c1_tmp = num[0]/num[2];
                float c2_tmp = num[1]/num[3];
                for( pp = 0; pp < TS_P; pp++)  
                    for( nn = 0; nn < TS_N; nn++)
                        for( mm = 0; mm < TS_M; mm++)
                        {
                            //#pragma cmost  pipeline
                            int k = tm * TS_M + mm + 1;
                            int j = tn * TS_N + nn + 1;
                            int i = tp * TS_P + pp + 1;

                            float Grad;

                            float K;			// curvature

                            float Dx_p;
                            float Dx_m;
                            float Dy_p;
                            float Dy_m;
                            float Dz_p;
                            float Dz_m;
                            float Dx_0;
                            float Dy_0;
                            float Dz_0;

                            float Dxx, Dyy, Dzz;
                            float Dxy, Dxz, Dyz;


                            Dx_p = (PPHI(i+1,j,k) - PPHI(i,j,k));
                            Dx_m = (PPHI(i,j,k) - PPHI(i-1,j,k));
                            Dy_p = (PPHI(i,j+1,k) - PPHI(i,j,k));
                            Dy_m = (PPHI(i,j,k) - PPHI(i,j-1,k));
                            Dz_p = (PPHI(i,j,k+1) - PPHI(i,j,k));
                            Dz_m = (PPHI(i,j,k) - PPHI(i,j,k-1));

                            Dx_0 = (PPHI(i+1,j,k) - PPHI(i-1,j,k))/2;
                            Dy_0 = (PPHI(i,j+1,k) - PPHI(i,j-1,k))/2;
                            Dz_0 = (PPHI(i,j,k+1) - PPHI(i,j,k-1))/2;

                            Dxx = (Dx_p - Dx_m) ;
                            Dyy = (Dy_p - Dy_m) ;
                            Dzz = (Dz_p - Dz_m) ;

                            Dxy = ( PPHI(i+1,j+1,k) - PPHI(i+1,j-1,k) - PPHI(i-1,j+1,k) + PPHI(i-1,j-1,k) ) / (4.0f);
                            Dxz = ( PPHI(i+1,j,k+1) - PPHI(i+1,j,k-1) - PPHI(i-1,j,k+1) + PPHI(i-1,j,k-1) ) / (4.0f);
                            Dyz = ( PPHI(i,j+1,k+1) - PPHI(i,j+1,k-1) - PPHI(i,j-1,k+1) + PPHI(i,j-1,k-1) ) / (4.0f);

                            Grad = (Dx_0*Dx_0 + Dy_0*Dy_0 + Dz_0*Dz_0);
                            K  = (  Dx_0*Dx_0*Dyy - 2*Dx_0*Dy_0*Dxy + Dy_0*Dy_0*Dxx
                                    + Dx_0*Dx_0*Dzz - 2*Dx_0*Dz_0*Dxz + Dz_0*Dz_0*Dxx
                                    + Dy_0*Dy_0*Dzz - 2*Dy_0*Dz_0*Dyz + Dz_0*Dz_0*Dyy )
                                / ( (CUB(Grad)) + epsilon );


                            curvature_motion_part(i,j,k) = 
                                Grad * ( mu * K + lambda1*SQR(PU0(i,j,k)-c1_tmp)
                                        - lambda2*SQR(PU0(i,j,k)-c2_tmp) );
                        }
            }
}
