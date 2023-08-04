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
#include<ap_int.h>
#pragma ACCEL kernel
void func_top_0()
{
  int a[100];
  int b[100];
  int i;
  for (int cc = 0; cc < 1; cc++) {
    for (int jj = 0; jj<1 << 0; jj ++ ) {
      a[i] = b[i];
    }
  }
  for (int cc = 0; cc < 1; cc++) {
    for (int jj = 0; jj<1 << 0; jj ++ ) {
#pragma ACCEL parallel complete
      a[i] = b[i];
    }
  }

  for (int cc = 0; cc < 1; cc++) {
    for (int jj = 0; jj<64; jj ++ ) {
#pragma ACCEL parallel complete
      a[i] = b[i];
    }
  }
}

#pragma ACCEL kernel
void func_top_1() {
int sum;
  for (class ap_uint< 6 > i(0); i<((int )2) - ((int )2); i+=2) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=1
  }
  for (int i = 0; i < 3; i+=3) {
     sum = 0;
  }
  for (int i = 0; i < 3; i+=3) {
  }
}


