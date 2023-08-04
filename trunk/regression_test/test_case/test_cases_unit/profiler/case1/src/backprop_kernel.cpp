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
#include <string.h>
#include "backprop.h"
#define ETA 0.3       //eta value
#define MOMENTUM 0.3  //momentum value

//extern "C" {
#pragma ACCEL kernel
void workload(float delta[17], float ly[65537], float w[65537 * 17], float oldw[65537 * 17]) {
#pragma ACCEL coalescing variable=delta force=on
#pragma ACCEL coalescing variable=ly force=on
#pragma ACCEL coalescing variable=w force=on
#pragma ACCEL coalescing variable=oldw force=on
    float new_dw;
    int k, kk, j;

    ly[0] = 1.0;
    
    #ifndef MC_BASELINE
    #pragma ACCEL pipeline
    #pragma ACCEL tiling factor=256
    #endif
    for (k = 0; k <= 65536; k++) {
        #ifndef MC_BASELINE
        #pragma ACCEL parallel factor=16
        #endif
        for (j = 0; j < 16; j++) {
            new_dw = ((ETA * delta[j] * ly[k]) + (MOMENTUM * oldw[k * 16 + j]));
            w[k * 16 + j] += new_dw;
            oldw[k * 16 + j] = new_dw;
        }
    }
}

//}
