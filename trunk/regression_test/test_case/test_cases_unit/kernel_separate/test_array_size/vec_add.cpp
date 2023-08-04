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
int f() {
  return 10;
}
// int g[f()];
const int N = 10;
const int M = 20;
const int K = 2;
int g_a[N];
int g_b[M] = {0};
int g_c[][K] = {{1,2}, {3, 4}};
#pragma ACCEL kernel
void vec_add_kernel(int *a)
{
    int a_buf[f()];
    a_buf[0] = a[0];
    a[1] = a_buf[1] + g_a[0] + g_b[2] + g_c[0][1];
    // g[0] = a[0];
}
