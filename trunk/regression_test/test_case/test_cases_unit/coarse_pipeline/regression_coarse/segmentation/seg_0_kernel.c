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
#define PITCH_P 258
#define PITCH_N 258
#define PITCH_M 258

#define curvature_motion_part(i,j,k)  curvature_motion_part_g[((i)*PITCH_N+(j))*PITCH_M+k]
#define PPHI(i,j,k) phi[((i)*PITCH_N+(j))*PITCH_M+k]
#define PU0(i,j,k) interpt[((i)*PITCH_N+(j))*PITCH_M+k]

#pragma ACCEL kernel
void seg_0_kernel(float* interpt, float * curvature_motion_part_g,  float* phi, int IMG_P,int IMG_N,int IMG_M,int TS_P,int TS_N,int TS_M, float * num)
{
    int IMG_TS_P = IMG_P / TS_P;
    int IMG_TS_M = IMG_M / TS_M;
    int IMG_TS_N = IMG_N / TS_N;

    int tp, tn, tm;
    int l;
    float num1_local;
    float num2_local;
    float den1_local;
    float den2_local;

    int mm, nn, pp;
    for (tp = 0; tp < (IMG_TS_P); tp++) 
        for (tn = 0; tn < (IMG_TS_N); tn++) 
            for (tm = 0; tm < (IMG_TS_M); tm++) 
            {
#pragma ACCEL pipeline 
                l = 0;
                num1_local = 0;
                num2_local = 0;
                den1_local = 0;
                den2_local = 0;

                mm = nn = pp = 0;
                for(l=0; l<1; l++)
                {
                    num1_local = num[0];
                    num2_local = num[1];
                    den1_local = num[2];
                    den2_local = num[3];
                }

                for( pp = 0; pp < TS_P; pp++)  
                    for( nn = 0; nn < TS_N; nn++) {
                        int k, j, i;
                        float pphi_c, pu0_c;
                        for( mm = 0; mm < TS_M; mm++)
                        {
#pragma ACCEL  pipeline
                            //#pragma AP pipeline II = 1 // can not pipeline because bus_e1 bug in loop pipeline
                            k = tm * TS_M + mm + 1;
                            j = tn * TS_N + nn + 1;
                            i = tp * TS_P + pp + 1;

                            pphi_c = PPHI(i,j,k);
                            pu0_c = PU0(i,j,k);

                            if (pphi_c < 0)	
                            {
                                num1_local = num1_local + pu0_c;
                                den1_local = den1_local + 1;
                            }
                            else if (pphi_c > 0)	
                            {
                                num2_local = num2_local + pu0_c;
                                den2_local = den2_local + 1;
                            }
                        }

                        for(l=0; l<1; l++)
                        {
                            num[0] = num1_local;
                            num[1] = num2_local;
                            num[2] = den1_local;
                            num[3] = den2_local;
                        }

                    }
            }
}
