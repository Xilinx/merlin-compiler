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
#include <stdlib.h>

int g_8 = 99;

#pragma ACCEL kernel
void g8(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9,int a10,
        int a11,int a12,int a13,int a14,int a15,int a16,int a17,int a18,int a19,
        int a20,int a21,int a22,int a23,int a24,int a25,int a26,int a27,int a28,int a29,
        int a30,int a31,int a32)
{
    g_8 = 22;
    return;
}
