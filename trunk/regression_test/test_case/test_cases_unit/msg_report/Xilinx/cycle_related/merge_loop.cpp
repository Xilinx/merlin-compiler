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
#define BUFSIZE 2048

#pragma ACCEL kernel
void mykernel(int orig[BUFSIZE], int len)
{
    int stage0[BUFSIZE];
    int i;
    int j;
#pragma ACCEL flatten
    for (i = 0; i < (BUFSIZE / 16); ++i) {
        for (j = 0; j < 16; ++j) {
            stage0[i * 16 + j] = orig[i * 16 + j] * 2;
        }
    }

    int stage1[BUFSIZE];
    for (i = 0; i < (BUFSIZE / 16); ++i) {
        for (j = 0; j < 16; ++j) {
            stage1[i * 16 + j] = orig[i * 16 + j] * 2;
        }
    }


    int stage2[BUFSIZE];
#pragma ACCEL trip_count=2048
#pragma ACCEL flatten
    for (i = 0; i < (len / 16); ++i) {
        for (j = 0; j < 16; ++j) {
            stage2[i * 16 + j] = orig[i * 16 + j] * 2;
        }
    }

    int stage3[BUFSIZE];
    stage3[0] = orig[0] * 3;
#pragma ACCEL false_dependence variable=stage3
#pragma ACCEL flatten
    for (i = 0; i < (len / 16); ++i) {
        for (j = 0; j < 16; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }
            stage3[i * 16 + j] = (orig[i * 16 + j] + 3 * orig[i * 16 + j - 1]) / 2;
        }
    }

    int stage4[BUFSIZE];
    int k;
#pragma ACCEL flatten
    for (i = 0; i < (BUFSIZE / 32); ++i) {
        for (j = 0; j < 2; ++j) {
#pragma ACCEL flatten
            for (k = 0; k < 16; ++k) {
                out[i * 32 + j * 2 + k] = orig[i * 32 + j * 2 + k] * 2;
            }
        }
    }
}
