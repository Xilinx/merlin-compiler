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



#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stdlib.h"

#include "config.h"

void cnn_kernel(float Cout[IMROW][IMCOL][NUM1],float Cout1[IMROW][IMCOL][NUM1],  
        float Cin[INIMROW][INIMCOL][NUM], 
        float weight[KERNEL][KERNEL][NUM1][NUM]) ;

void cnn_kernel_sw(float Cout[IMROW][IMCOL][NUM1],
        float Cin[INIMROW][INIMCOL][NUM], 
        float weight[KERNEL][KERNEL][NUM1][NUM]) 
{

    for (int r = 0; r < IMROW; r++)
    for (int c = 0; c < IMCOL; c++)
    for (int p = 0; p < KERNEL; p++) 
    for (int q = 0; q < KERNEL; q++) {
    for (int i = 0; i < NUM; i++)
    for (int o = 0; o < NUM1; o++) {
    {
        float one_sub= weight[p][q][o][i] * Cin[r+p][c+q][i];
        Cout[r][c][o] += one_sub;
    }}}
}

float Cout[IMROW][IMCOL][NUM1]; 
float Cout1[IMROW][IMCOL][NUM1]; 
float Cin[INIMROW][INIMCOL][NUM]; 
float weight[KERNEL][KERNEL][NUM1][NUM] ;

void main()
{

    memset(Cout[0][0]  , 0, NUM1*IMROW*IMCOL*sizeof(float));
    memset(Cout1[0][0]  , 0, NUM1*IMROW*IMCOL*sizeof(float));
    memset(Cin[0][0]   , 0, NUM*INIMROW*INIMCOL*sizeof(float));
    memset(weight[0][0][0], 0, NUM1*NUM*KERNEL*KERNEL*sizeof(float));


    float * p = Cin[0][0];
    for (int i = 0; i < NUM*INIMROW*INIMCOL; i++)
    {
        *(p++) = rand()*1.0/RAND_MAX;
    }
    p = weight[0][0][0];
    for (int i = 0; i < NUM1*NUM*KERNEL*KERNEL; i++)
    {
        *(p++) = rand()*1.0/RAND_MAX;
    }

#pragma ACCEL task
    cnn_kernel(Cout, Cout, Cin, weight);

    cnn_kernel_sw(Cout1, Cin, weight);

    int err = 0;
    for (int r = 0; r < IMROW; r++)
    for (int c = 0; c < IMCOL; c++)
    for (int o = 0; o < NUM1; o++) 
    {
        int larger = fmax(fabs(Cout[r][c][o]), fabs( Cout[r][c][o]) ); 
        if (larger > 0)
        if (fabs((Cout[r][c][o] - Cout1[r][c][o])) / larger > 1e-5) 
        //if (fabs(Cout[r][c][o] - Cout1[r][c][o]) > 1e-5)
        {
            printf("[ERROR] r=%d c=%d o=%d: out=%lf exp=%lf\n", r, c, o, Cout[r][c][o], Cout1[r][c][o]);
            err++;
        }

        if (err >= 10) return ;
    }

    printf("Success.\n");
}




