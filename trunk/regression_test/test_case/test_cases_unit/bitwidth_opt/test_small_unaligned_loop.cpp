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
#include<string.h>
#pragma ACCEL kernel
void func1_kernel(int *a) {
    int a_buf_0[34];
    int a_buf_1[65];
    int a_buf_2[1021];
    int a_buf_3[10][3][15];
    int a_buf_4[3][20][7];
    memcpy(a_buf_0, a, sizeof(int) * 34);
    memcpy(a_buf_1, a, sizeof(int) * 65);
    memcpy(a_buf_2, a, sizeof(int) * 1021);
    memcpy(a_buf_3, a, sizeof(int) * 450);
    memcpy(a_buf_4, a, sizeof(int) * 420);
}
