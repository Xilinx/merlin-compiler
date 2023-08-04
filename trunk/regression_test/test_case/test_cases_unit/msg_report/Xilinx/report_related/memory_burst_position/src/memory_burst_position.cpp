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
void sub(int *c, int *b) {
#pragma hls inline off
  for (int i = 0; i < 1000; ++i)
    c[i] = b[i]; 
}
#pragma ACCEL kernel
void top(int *a, int *b, int *c) {
#pragma ACCEL interface variable=a depth=1000 bus_bitwidth=off
#pragma ACCEL interface variable=b depth=1000 bus_bitwidth=off
#pragma ACCEL interface variable=c depth=10000000 bus_bitwidth=off
 int i;

 for (i = 0; i < 1000; ++i) {
    a[i] = i;
 }

 int b_buf[1000];
 memcpy(b_buf, b, sizeof(int) * 1000);

 for (i = 0; i < 100; ++i)
    sub(&c[i * 100000] , b_buf);
 return;
}
