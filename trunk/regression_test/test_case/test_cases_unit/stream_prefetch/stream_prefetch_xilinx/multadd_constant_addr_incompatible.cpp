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
#include "math.h"
#define READ_SIZE 3840000
#define WRITE_SIZE 30000
#pragma ACCEL kernel
void adder(float ina[640], float inb[READ_SIZE], float out[WRITE_SIZE]) {
#pragma ACCEL interface variable=out
#pragma ACCEL interface variable=ina
#pragma ACCEL interface variable=inb
    for(int i=0; i < 6000; i++) {
        int current_index = i;
        for(int j=0; j < 5; j++) {
            #pragma ACCEL parallel
            float temp = 0.0f;
            for(int k=0; k < 128; k++) {
            #pragma ACCEL parallel
                temp += ina[j*128+k] * inb[current_index*640+j*128+k];
            }
            //out[i*5+j] = 1.0f / (1.0f + exp(-temp));
            out[i*5+j] = 1.0f / (1.0f + exp(-temp));
        }
    }
}

