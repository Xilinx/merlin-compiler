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
void mykernel(int orig[BUFSIZE], int out[BUFSIZE])
{
    int i, j, k, l;
    for (l = 0; l < (BUFSIZE); ++l) {
middle:
        for (i = 0; i < (BUFSIZE / 32); ++i) {
            for (j = 0; j < 2; ++j) {
                for (k = 0; k < 16; ++k) {
                    out[i * 32 + j * 16 + k] = orig[i * 32 + j * 16 + k] * 2;
                }
            }
        }

        for (i = 0; i < BUFSIZE / 32; ++i) {
            for (j = 0; j < 32; ++j) {
                out[i * 32 + j] += 2;
            }
        }
    }
}
