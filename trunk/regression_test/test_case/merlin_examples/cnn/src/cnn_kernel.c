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
#include "config.h"
#include <string.h>

void cnn_kernel(float Cout[IMROW][IMCOL][NUM1], float Cout1[IMROW][IMCOL][NUM1], 
        float Cin[INIMROW][INIMCOL][NUM], 
        float weight[KERNEL][KERNEL][NUM1][NUM]) {

    for (int r = 0; r < IMROW; r++)
    for (int c = 0; c < IMCOL; c++)
    for (int p = 0; p < KERNEL; p++) 
    for (int q = 0; q < KERNEL; q++) 
    #pragma ACCEL parallel factor=32
    for (int i = 0; i < NUM; i++)
    #pragma ACCEL parallel factor=8
    for (int o = 0; o < NUM1; o++) {
    {
        float one_sub= weight[p][q][o][i] * Cin[r+p][c+q][i];

        #pragma ACCEL reduction
        Cout[r][c][o] += one_sub;
    }}

   
}


