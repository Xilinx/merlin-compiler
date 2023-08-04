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

void mm_kernel( float a[SIZE_I][SIZE_K], 
                float b[SIZE_J][SIZE_K],
                float c[SIZE_I][SIZE_J])
{

    for (int i = 0; i < SIZE_I; i++) 
        #pragma ACCEL parallel factor=8
        for (int k = 0; k < SIZE_K; k++) 
            #pragma ACCEL parallel factor=8
            for (int j = 0; j < SIZE_J; j++) 
                #pragma ACCEL reduction
                c[i][j] += a[i][k] * b[k][j];
}


