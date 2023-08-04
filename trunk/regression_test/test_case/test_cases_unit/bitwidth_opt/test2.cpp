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

void func1_sub(int *a, int a_buf[1000], int k) {
    memcpy(a_buf, &(a[k]), sizeof(int) * 1000);
}

void func2_sub(int *a, int a_buf[1000]) {
    memcpy(a, &a_buf[2], sizeof(int) * 980);
}

extern "C" {
#pragma ACCEL kernel
void func1_kernel(int *a, int *b, int k) {
    int a_buf[1000];
    func1_sub(a, a_buf, k);
    func2_sub(b, a_buf);
}
}

int main() {
    int *a, *b, *c;
    int k;
    func1_kernel(a, b, k);
    return 0;
}
