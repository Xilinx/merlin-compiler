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

#pragma ACCEL kernel
void func_top_1() {
  int b[100][100];
  int t=10;
  int aa[100];
  int ab[100];
  int a[4];

  while(t > 1) {
#pragma ACCEL pipeline
    aa[t] += ab[t] + ab[t-1] + a[0] + ab[t+1] + (t--);
    aa[t] = a[1] + a[2];
  }

  do {
#pragma ACCEL parallel
    b[t][t] = ab[t] + ab[t+1] + ab[t-1];
    t++;
  } while(t < 99);
}
