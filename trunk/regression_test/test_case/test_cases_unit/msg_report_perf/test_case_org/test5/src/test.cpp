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
void func(char *a, int *b, long *c) {
    for(int j=0; j<64; j++) {
        for(int i=0; i<128; i++) {
//            c[k*128*64+j*128+i] = a[k*128*64+j*128+i] + b[k*128*64+j*128+i];
            c[j*128+i] = a[j*128+i] + b[j*128+i];
        }
    }
    return;
}

#pragma ACCEL kernel
void test(char * a, int * b, long * c) {
    #pragma ACCEL interface variable=a depth=262144 
    #pragma ACCEL interface variable=b depth=262144 
    #pragma ACCEL interface variable=c depth=262144 
    for(int k=0; k<32; k++) {
        func(a + k*128*64, b + k*128*64, c + k*128*64);
    }
    return;
}
