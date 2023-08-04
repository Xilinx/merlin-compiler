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
void mykernel(int orig[BUFSIZE])
{
    int i;

    int stage0[BUFSIZE];
    int j;
    for (i = 0; i < (BUFSIZE / 16); ++i) {
        for (j = 0; j < 16; ++j) {
            stage0[i * 16 + j] = orig[i * 16 + j] * 2;
        }
    }

    int stage1[BUFSIZE];
    stage1[0] = stage0[0];
#pragma ACCEL false_dependence variable=stage1
    for (i = 1; i < BUFSIZE; ++i) {
        stage1[i] = stage1[i - 1] + stage1[i];
    }

    int stage2[BUFSIZE];
    stage2[0] = stage0[0];
    for (i = 1; i < BUFSIZE; ++i) {
        stage2[i] = stage2[i - 1] + stage2[i];
    }

}
