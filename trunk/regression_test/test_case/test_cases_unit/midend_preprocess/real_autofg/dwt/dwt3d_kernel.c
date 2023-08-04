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

#define a1 (-1.586134342) 
#define a2 (-0.05298011854)
#define a3 (0.8829110762)
#define a4 (0.4435068522)

#define k1 1.1496043988602418
#define k2 (1.0/1.1496043988602418)

#define tmp(i, j)	tmp[(i)][(j)/8][((j)%8)]
#define tmp1(i, j)	tmp1[(i)/8][((i)%8)][(j)]
#define tmp_l(i, j)	tmp_l[(j)]


#define tmp1_i(i, j)	tmp1_i[(i)/8][(j)]
#define tmp_i(i, j)	    tmp_i [(i)][(j)/8]

void fwt97_pd_foo_one_row(float tmp1_i[32][256], float tmp_i[256][32], int i, int m, int n, int M, int N)
{
    int j;
    float tmp_l[256];

    float img_pre_e = 0.0; // reuse from previous even pixel
    float img_pre_o = 0.0; // reuse from previous even pixel

    // Predict&update 1.1
    img_pre_e = tmp1_i(i, 0);
    img_pre_o = tmp1_i(i, 1);
    for (j=1; j<m; j+=2) {
#pragma HLS loop_tripcount max=128
        //#pragma AP loop_tripcount max=max=128
        //#pragma AP pipeline II=1
        float img_o = tmp1_i(i, j);
        float img_e = tmp1_i(i, j+1);

        float img_i_j = img_o;
        float img_i_j_p = (j==m-1) ? img_pre_e : img_e;
        float img_i_j_n = img_pre_e;
        float img_i_j_up = img_i_j + (a1*(img_i_j_p + img_i_j_n));
        if (j==1) img_pre_o = img_i_j_up;

        tmp_l(i, j)   = img_i_j_up;
        tmp_l(i, j-1) = img_pre_e + a2*(img_i_j_up + img_pre_o);

        img_pre_o = img_i_j_up;
        img_pre_e = img_i_j_p;
    } 

    // Predict&update 1.2
    img_pre_e = tmp_l(i, 0);
    img_pre_o = tmp_l(i, 1);
    for (j=1; j<m; j+=2) {
#pragma HLS loop_tripcount max=128
        //#pragma AP loop_tripcount max=max=128
        //#pragma AP pipeline II=1
        float img_i_j = tmp_l(i, j);
        float img_i_j_p = (j==m-1) ? img_pre_e : tmp_l(i, j+1);
        float img_i_j_n = img_pre_e;
        float img_i_j_up = img_i_j + (a3*(img_i_j_p + img_i_j_n));
        if (j==1) img_pre_o = img_i_j_up;

        float img_i_j_up_1 = img_pre_e + a4*(img_i_j_up + img_pre_o);

        img_pre_o = img_i_j_up;
        img_pre_e = img_i_j_p;

        tmp_i(j/2+m/2, i) = k2*img_i_j_up;
        tmp_i(j/2    , i) = k1*img_i_j_up_1;
        //tmp(j/2+m/2, i) = k2*img_i_j_up;
        //tmp(j/2    , i) = k1*img_i_j_up_1;
    }
} 


void fwt97_pd_foo_one_pair(double aa, double bb, float img_o, float img_e, float * out_j, float * out_j_1, int i, int j, 
        float img_pre_o_in, float * img_pre_o, float img_pre_e_in, float * img_pre_e,
        int m, int n, int M, int N)
{

#pragma AP inline
    //float img_o = tmp(i, j);
    //float img_e = tmp(i, j+1);

    float img_i_j = img_o;
    float img_i_j_p = (j==m-1) ? img_pre_e_in : img_e;
    float img_i_j_n = img_pre_e_in;
    float img_i_j_up = img_i_j + (aa*(img_i_j_p + img_i_j_n));
    float img_pre_o_in_1 = img_pre_o_in;
    if (j==1) img_pre_o_in_1 = img_i_j_up;

    //tmp_l(i, j)   = img_i_j_up;
    //tmp_l(i, j-1) = img_pre_e + a2*(img_i_j_up + *img_pre_o);
    *out_j = img_i_j_up;
    *out_j_1 = img_pre_e_in + bb*(img_i_j_up + img_pre_o_in_1);

    *img_pre_o = img_i_j_up;
    *img_pre_e = img_i_j_p;
}


void fwt97_pd_foo_one_pic(
        float * img,
        float* img_0,
        float* img_1,
        float* img_2,
        float* img_3,
        float* img_4,
        float* img_5,
        float* img_6,
        float* img_7,
        int ip,
        int m,int n, 
        int M,int N) 
{
    //float img[256][256];
    float tmp[256][32][8];
    float tmp1[32][8][256];

    float tmp1_0[32][256];
    float tmp1_1[32][256];
    float tmp1_2[32][256];
    float tmp1_3[32][256];
    float tmp1_4[32][256];
    float tmp1_5[32][256];
    float tmp1_6[32][256];
    float tmp1_7[32][256];

    float tmp_0[256][32];
    float tmp_1[256][32];
    float tmp_2[256][32];
    float tmp_3[256][32];
    float tmp_4[256][32];
    float tmp_5[256][32];
    float tmp_6[256][32];
    float tmp_7[256][32];

    int i, j;

    double k1_a4 = k1*a4;

    float img_pre_e = 0.0; // reuse from previous even pixel
    float img_pre_o = 0.0; // reuse from previous even pixel

    for (i=0; i<32; i++) {
        for (j = 0; j < 256; j++) {
            tmp1_0[i][j] = img[ip*256*256+(8*i+0)*256+j];
            tmp1_1[i][j] = img[ip*256*256+(8*i+1)*256+j];
            tmp1_2[i][j] = img[ip*256*256+(8*i+2)*256+j];
            tmp1_3[i][j] = img[ip*256*256+(8*i+3)*256+j];
            tmp1_4[i][j] = img[ip*256*256+(8*i+4)*256+j];
            tmp1_5[i][j] = img[ip*256*256+(8*i+5)*256+j];
            tmp1_6[i][j] = img[ip*256*256+(8*i+6)*256+j];
            tmp1_7[i][j] = img[ip*256*256+(8*i+7)*256+j];
        }
    }

    for (i=0; i<n/8; i++) {
#pragma HLS loop_tripcount max=32
        fwt97_pd_foo_one_row(tmp1_0, tmp_0, (8*i+0), m, n, M, N);
        fwt97_pd_foo_one_row(tmp1_1, tmp_1, (8*i+1), m, n, M, N);
        fwt97_pd_foo_one_row(tmp1_2, tmp_2, (8*i+2), m, n, M, N);
        fwt97_pd_foo_one_row(tmp1_3, tmp_3, (8*i+3), m, n, M, N);
        fwt97_pd_foo_one_row(tmp1_4, tmp_4, (8*i+4), m, n, M, N);
        fwt97_pd_foo_one_row(tmp1_5, tmp_5, (8*i+5), m, n, M, N);
        fwt97_pd_foo_one_row(tmp1_6, tmp_6, (8*i+6), m, n, M, N);
        fwt97_pd_foo_one_row(tmp1_7, tmp_7, (8*i+7), m, n, M, N);
    }

    for (i=0; i<n; i++) {
#pragma HLS loop_tripcount max=256
        float tmp_l[256];

        float tmp_l_0[32];
        float tmp_l_1[32];
        float tmp_l_2[32];
        float tmp_l_3[32];
        float tmp_l_4[32];
        float tmp_l_5[32];
        float tmp_l_6[32];
        float tmp_l_7[32];

        // Predict&update 2.1
        img_pre_e = tmp_0[i][0]; //tmp[i][0][0]; //tmp(i, 0);
        img_pre_o = tmp_1[i][0]; //tmp[i][0][1]; //tmp(i, 1);
        for (j=1; j<m; j+=2*8) {
#pragma HLS loop_tripcount max=16
            int jj = j / (2*8);
            {
                float img_o_0 = tmp_1[i][jj*2+0]; float img_e_0 = tmp_2[i][jj*2+0];
                float img_o_1 = tmp_3[i][jj*2+0]; float img_e_1 = tmp_4[i][jj*2+0];
                float img_o_2 = tmp_5[i][jj*2+0]; float img_e_2 = tmp_6[i][jj*2+0];
                float img_o_3 = tmp_7[i][jj*2+0]; float img_e_3 = tmp_0[i][jj*2+1];
                float img_o_4 = tmp_1[i][jj*2+1]; float img_e_4 = tmp_2[i][jj*2+1];
                float img_o_5 = tmp_3[i][jj*2+1]; float img_e_5 = tmp_4[i][jj*2+1];
                float img_o_6 = tmp_5[i][jj*2+1]; float img_e_6 = tmp_6[i][jj*2+1];
                float img_o_7 = tmp_7[i][jj*2+1]; float img_e_7 = tmp_0[i][jj*2+2];
                float out_j_0, out_j_1_0, img_pre_o_0, img_pre_e_0;
                float out_j_1, out_j_1_1, img_pre_o_1, img_pre_e_1;
                float out_j_2, out_j_1_2, img_pre_o_2, img_pre_e_2;
                float out_j_3, out_j_1_3, img_pre_o_3, img_pre_e_3;
                float out_j_4, out_j_1_4, img_pre_o_4, img_pre_e_4;
                float out_j_5, out_j_1_5, img_pre_o_5, img_pre_e_5;
                float out_j_6, out_j_1_6, img_pre_o_6, img_pre_e_6;
                float out_j_7, out_j_1_7, img_pre_o_7, img_pre_e_7;
                float out_j_8, out_j_1_8, img_pre_o_8, img_pre_e_8;
                img_pre_o_0 = img_pre_o, img_pre_e_0 = img_pre_e;
                fwt97_pd_foo_one_pair(a1, a2, img_o_0, img_e_0, &out_j_0, &out_j_1_0, i, j+2*0, img_pre_o_0, &img_pre_o_1, img_pre_e_0, &img_pre_e_1,	m, n, M, N);
                fwt97_pd_foo_one_pair(a1, a2, img_o_1, img_e_1, &out_j_1, &out_j_1_1, i, j+2*1, img_pre_o_1, &img_pre_o_2, img_pre_e_1, &img_pre_e_2,	m, n, M, N);
                fwt97_pd_foo_one_pair(a1, a2, img_o_2, img_e_2, &out_j_2, &out_j_1_2, i, j+2*2, img_pre_o_2, &img_pre_o_3, img_pre_e_2, &img_pre_e_3,	m, n, M, N);
                fwt97_pd_foo_one_pair(a1, a2, img_o_3, img_e_3, &out_j_3, &out_j_1_3, i, j+2*3, img_pre_o_3, &img_pre_o_4, img_pre_e_3, &img_pre_e_4,	m, n, M, N);
                fwt97_pd_foo_one_pair(a1, a2, img_o_4, img_e_4, &out_j_4, &out_j_1_4, i, j+2*4, img_pre_o_4, &img_pre_o_5, img_pre_e_4, &img_pre_e_5,	m, n, M, N);
                fwt97_pd_foo_one_pair(a1, a2, img_o_5, img_e_5, &out_j_5, &out_j_1_5, i, j+2*5, img_pre_o_5, &img_pre_o_6, img_pre_e_5, &img_pre_e_6,	m, n, M, N);
                fwt97_pd_foo_one_pair(a1, a2, img_o_6, img_e_6, &out_j_6, &out_j_1_6, i, j+2*6, img_pre_o_6, &img_pre_o_7, img_pre_e_6, &img_pre_e_7,	m, n, M, N);
                fwt97_pd_foo_one_pair(a1, a2, img_o_7, img_e_7, &out_j_7, &out_j_1_7, i, j+2*7, img_pre_o_7, &img_pre_o_8, img_pre_e_7, &img_pre_e_8,	m, n, M, N);
                //tmp_l[jj*2*8+0*2+1] = out_j_0; tmp_l[jj*2*8+0*2+0] = out_j_1_0;
                tmp_l_1[jj*2+0] = out_j_0; tmp_l_0[jj*2+0] = out_j_1_0;
                //tmp_l[jj*2*8+1*2+1] = out_j_1; tmp_l[jj*2*8+1*2+0] = out_j_1_1;
                tmp_l_3[jj*2+0] = out_j_1; tmp_l_2[jj*2+0] = out_j_1_1;
                //tmp_l[jj*2*8+2*2+1] = out_j_2; tmp_l[jj*2*8+2*2+0] = out_j_1_2;
                tmp_l_5[jj*2+0] = out_j_2; tmp_l_4[jj*2+0] = out_j_1_2;
                //tmp_l[jj*2*8+3*2+1] = out_j_3; tmp_l[jj*2*8+3*2+0] = out_j_1_3;
                tmp_l_7[jj*2+0] = out_j_3; tmp_l_6[jj*2+0] = out_j_1_3;
                //tmp_l[jj*2*8+4*2+1] = out_j_4; tmp_l[jj*2*8+4*2+0] = out_j_1_4;
                tmp_l_1[jj*2+1] = out_j_4; tmp_l_0[jj*2+1] = out_j_1_4;
                //tmp_l[jj*2*8+5*2+1] = out_j_5; tmp_l[jj*2*8+5*2+0] = out_j_1_5;
                tmp_l_3[jj*2+1] = out_j_5; tmp_l_2[jj*2+1] = out_j_1_5;
                //tmp_l[jj*2*8+6*2+1] = out_j_6; tmp_l[jj*2*8+6*2+0] = out_j_1_6;
                tmp_l_5[jj*2+1] = out_j_6; tmp_l_4[jj*2+1] = out_j_1_6;
                //tmp_l[jj*2*8+7*2+1] = out_j_7; tmp_l[jj*2*8+7*2+0] = out_j_1_7;
                tmp_l_7[jj*2+1] = out_j_7; tmp_l_6[jj*2+1] = out_j_1_7;
                img_pre_o = img_pre_o_8; img_pre_e = img_e_7; //img_pre_e_8;
            }
        } 

        // Predict&update 2.2
        img_pre_e = tmp_l_0[0]; //tmp_l(i, 0);
        img_pre_o = tmp_l_1[0]; //tmp_l(i, 1);
        for (j=1; j<m; j+=2*8) {
#pragma HLS loop_tripcount max=32
            int jj = j / (2*8);
            if (1)
            {
                float img_o_0 = tmp_l_1[jj*2+0]; float img_e_0 = tmp_l_2[jj*2+0];
                float img_o_1 = tmp_l_3[jj*2+0]; float img_e_1 = tmp_l_4[jj*2+0];
                float img_o_2 = tmp_l_5[jj*2+0]; float img_e_2 = tmp_l_6[jj*2+0];
                float img_o_3 = tmp_l_7[jj*2+0]; float img_e_3 = tmp_l_0[jj*2+1];
                float img_o_4 = tmp_l_1[jj*2+1]; float img_e_4 = tmp_l_2[jj*2+1];
                float img_o_5 = tmp_l_3[jj*2+1]; float img_e_5 = tmp_l_4[jj*2+1];
                float img_o_6 = tmp_l_5[jj*2+1]; float img_e_6 = tmp_l_6[jj*2+1];
                float img_o_7 = tmp_l_7[jj*2+1]; float img_e_7 = tmp_l_0[jj*2+2];
                float out_j_0, out_j_1_0, img_pre_o_0, img_pre_e_0;
                float out_j_1, out_j_1_1, img_pre_o_1, img_pre_e_1;
                float out_j_2, out_j_1_2, img_pre_o_2, img_pre_e_2;
                float out_j_3, out_j_1_3, img_pre_o_3, img_pre_e_3;
                float out_j_4, out_j_1_4, img_pre_o_4, img_pre_e_4;
                float out_j_5, out_j_1_5, img_pre_o_5, img_pre_e_5;
                float out_j_6, out_j_1_6, img_pre_o_6, img_pre_e_6;
                float out_j_7, out_j_1_7, img_pre_o_7, img_pre_e_7;
                float out_j_8, out_j_1_8, img_pre_o_8, img_pre_e_8;
                img_pre_o_0 = img_pre_o, img_pre_e_0 = img_pre_e;
                fwt97_pd_foo_one_pair(a3, a4, img_o_0, img_e_0, &out_j_0, &out_j_1_0, i, j+2*0, img_pre_o_0, &img_pre_o_1, img_pre_e_0, &img_pre_e_1,	m, n, M, N);
                fwt97_pd_foo_one_pair(a3, a4, img_o_1, img_e_1, &out_j_1, &out_j_1_1, i, j+2*1, img_pre_o_1, &img_pre_o_2, img_pre_e_1, &img_pre_e_2,	m, n, M, N);
                fwt97_pd_foo_one_pair(a3, a4, img_o_2, img_e_2, &out_j_2, &out_j_1_2, i, j+2*2, img_pre_o_2, &img_pre_o_3, img_pre_e_2, &img_pre_e_3,	m, n, M, N);
                fwt97_pd_foo_one_pair(a3, a4, img_o_3, img_e_3, &out_j_3, &out_j_1_3, i, j+2*3, img_pre_o_3, &img_pre_o_4, img_pre_e_3, &img_pre_e_4,	m, n, M, N);
                fwt97_pd_foo_one_pair(a3, a4, img_o_4, img_e_4, &out_j_4, &out_j_1_4, i, j+2*4, img_pre_o_4, &img_pre_o_5, img_pre_e_4, &img_pre_e_5,	m, n, M, N);
                fwt97_pd_foo_one_pair(a3, a4, img_o_5, img_e_5, &out_j_5, &out_j_1_5, i, j+2*5, img_pre_o_5, &img_pre_o_6, img_pre_e_5, &img_pre_e_6,	m, n, M, N);
                fwt97_pd_foo_one_pair(a3, a4, img_o_6, img_e_6, &out_j_6, &out_j_1_6, i, j+2*6, img_pre_o_6, &img_pre_o_7, img_pre_e_6, &img_pre_e_7,	m, n, M, N);
                fwt97_pd_foo_one_pair(a3, a4, img_o_7, img_e_7, &out_j_7, &out_j_1_7, i, j+2*7, img_pre_o_7, &img_pre_o_8, img_pre_e_7, &img_pre_e_8,	m, n, M, N);
                //tmp_l[jj*2*8+0*2+1] = out_j_0; tmp_l[jj*2*8+0*2+0] = out_j_1_0;
                //tmp1(j+2*0, i)   = k2*out_j_0; tmp1(j+2*0-1, i) = k1*out_j_1_0;
                tmp1_1[jj*2+0][i] = k2*out_j_0; tmp1_0[jj*2+0][i] = k1*out_j_1_0;
                //tmp_l_1[jj*2+0] = out_j_0; tmp_l_0[jj*2+0] = out_j_1_0;
                //tmp_l[jj*2*8+1*2+1] = out_j_1; tmp_l[jj*2*8+1*2+0] = out_j_1_1;
                //tmp1(j+2*1, i)   = k2*out_j_1; tmp1(j+2*1-1, i) = k1*out_j_1_1;
                tmp1_3[jj*2+0][i] = k2*out_j_1; tmp1_2[jj*2+0][i] = k1*out_j_1_1;
                //tmp_l_3[jj*2+0] = out_j_1; tmp_l_2[jj*2+0] = out_j_1_1;
                //tmp_l[jj*2*8+2*2+1] = out_j_2; tmp_l[jj*2*8+2*2+0] = out_j_1_2;
                //tmp1(j+2*2, i)   = k2*out_j_2; tmp1(j+2*2-1, i) = k1*out_j_1_2;
                tmp1_5[jj*2+0][i] = k2*out_j_2; tmp1_4[jj*2+0][i] = k1*out_j_1_2;
                //tmp_l_5[jj*2+0] = out_j_2; tmp_l_4[jj*2+0] = out_j_1_2;
                //tmp_l[jj*2*8+3*2+1] = out_j_3; tmp_l[jj*2*8+3*2+0] = out_j_1_3;
                //tmp1(j+2*3, i)   = k2*out_j_3; tmp1(j+2*3-1, i) = k1*out_j_1_3;
                tmp1_7[jj*2+0][i] = k2*out_j_3; tmp1_6[jj*2+0][i] = k1*out_j_1_3;
                //tmp_l_7[jj*2+0] = out_j_3; tmp_l_6[jj*2+0] = out_j_1_3;
                //tmp_l[jj*2*8+4*2+1] = out_j_4; tmp_l[jj*2*8+4*2+0] = out_j_1_4;
                //tmp1(j+2*4, i)   = k2*out_j_4; tmp1(j+2*4-1, i) = k1*out_j_1_4;
                tmp1_1[jj*2+1][i] = k2*out_j_4; tmp1_0[jj*2+1][i] = k1*out_j_1_4;
                //tmp_l_1[jj*2+1] = out_j_4; tmp_l_0[jj*2+1] = out_j_1_4;
                //tmp_l[jj*2*8+5*2+1] = out_j_5; tmp_l[jj*2*8+5*2+0] = out_j_1_5;
                //tmp1(j+2*5, i)   = k2*out_j_5; tmp1(j+2*5-1, i) = k1*out_j_1_5;
                tmp1_3[jj*2+1][i] = k2*out_j_5; tmp1_2[jj*2+1][i] = k1*out_j_1_5;
                //tmp_l_3[jj*2+1] = out_j_5; tmp_l_2[jj*2+1] = out_j_1_5;
                //tmp_l[jj*2*8+6*2+1] = out_j_6; tmp_l[jj*2*8+6*2+0] = out_j_1_6;
                //tmp1(j+2*6, i)   = k2*out_j_6; tmp1(j+2*6-1, i) = k1*out_j_1_6;
                tmp1_5[jj*2+1][i] = k2*out_j_6; tmp1_4[jj*2+1][i] = k1*out_j_1_6;
                //tmp_l_5[jj*2+1] = out_j_6; tmp_l_4[jj*2+1] = out_j_1_6;
                //tmp_l[jj*2*8+7*2+1] = out_j_7; tmp_l[jj*2*8+7*2+0] = out_j_1_7;
                //tmp1(j+2*7, i)   = k2*out_j_7; tmp1(j+2*7-1, i) = k1*out_j_1_7;
                tmp1_7[jj*2+1][i] = k2*out_j_7; tmp1_6[jj*2+1][i] = k1*out_j_1_7;
                //tmp_l_7[jj*2+1] = out_j_7; tmp_l_6[jj*2+1] = out_j_1_7;
                img_pre_o = img_pre_o_8; img_pre_e = img_e_7; //img_pre_e_8;
            }

            //tmp1(j/2+m/2, i) = k2*img_i_j_up;
            //tmp1(j/2    , i) = k1*img_i_j_up_1;
        }
    }

    for (i=0; i<32; i++) {
        //#pragma AP loop_tripcount max=max=32
        for (j = 0; j < 256; j++) {
#pragma HLS loop_tripcount max=256
            //img[ip*256*256+i*256+j] = (i<128) ? tmp1(2*i, j) : tmp1(2*(i-128)+1, j); // tmp1(i,j);
            //img[ip*256*256+i*256+j] = tmp1(i,j);

            img_0[ip*32*256+i*256+j] = (8*i<128) ? tmp1_0[2*i][j]   : tmp1_1[2*i-32][j]; // tmp1(2*((8*i+0)-128)+1, j); //tmp1_0[i][j];
            img_1[ip*32*256+i*256+j] = (8*i<128) ? tmp1_2[2*i][j]   : tmp1_3[2*i-32][j]; // tmp1(2*((8*i+1)-128)+1, j); //tmp1_0[i][j];
            img_2[ip*32*256+i*256+j] = (8*i<128) ? tmp1_4[2*i][j]   : tmp1_5[2*i-32][j]; // tmp1(2*((8*i+2)-128)+1, j); //tmp1_0[i][j];
            img_3[ip*32*256+i*256+j] = (8*i<128) ? tmp1_6[2*i][j]   : tmp1_7[2*i-32][j]; // tmp1(2*((8*i+3)-128)+1, j); //tmp1_0[i][j];
            img_4[ip*32*256+i*256+j] = (8*i<128) ? tmp1_0[2*i+1][j] : tmp1_1[2*i-32+1][j]; // tmp1(2*((8*i+4)-128)+1, j); //tmp1_0[i][j];
            img_5[ip*32*256+i*256+j] = (8*i<128) ? tmp1_2[2*i+1][j] : tmp1_3[2*i-32+1][j]; // tmp1(2*((8*i+5)-128)+1, j); //tmp1_0[i][j];
            img_6[ip*32*256+i*256+j] = (8*i<128) ? tmp1_4[2*i+1][j] : tmp1_5[2*i-32+1][j]; // tmp1(2*((8*i+6)-128)+1, j); //tmp1_0[i][j];
            img_7[ip*32*256+i*256+j] = (8*i<128) ? tmp1_6[2*i+1][j] : tmp1_7[2*i-32+1][j]; // tmp1(2*((8*i+7)-128)+1, j); //tmp1_0[i][j];
        }
    }
}
#pragma ACCEL kernel
void dwt3d_kernel(
        float* img,
        float* img_0,
        float* img_1,
        float* img_2,
        float* img_3,
        float* img_4,
        float* img_5,
        float* img_6,
        float* img_7,
        int m, int n, 
        int MM, int NN) 
{

    int ip;
    for (ip = 0; ip < 16; ip++) {

        fwt97_pd_foo_one_pic(
                img,
                img_0, 
                img_1, 
                img_2, 
                img_3, 
                img_4, 
                img_5, 
                img_6, 
                img_7, 
                ip,
                m, n, MM, NN);

    }

}

