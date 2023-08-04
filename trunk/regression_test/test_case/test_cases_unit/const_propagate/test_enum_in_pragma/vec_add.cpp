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
enum _ram_type {
   RAM_1P_BRAM = 2,
};

#pragma ACCEL kernel
void top(int a[10], int b[10]) {
#pragma ACCEL parallel factor=RAM_1P_BRAM
  for (int i = 0; i < 10; ++i) {
//#pragma HLS pipeline II=RAM_1P_BRAM
      b[i]=a[i]+RAM_1P_BRAM;
  }
}

#pragma ACCEL kernel
void top_acc(int a[10], int b[10]) {
enum _ram_type {
   RAM_2P_BRAM = 3,
};
#pragma ACCEL pipeline II=RAM_2P_BRAM
  for (int i = 0; i < 10; ++i) {
//#pragma HLS pipeline II=RAM_1P_BRAM
      b[i]=a[i]+RAM_1P_BRAM;
  }
}

enum _ram_top_type {
   RAM_3P_BRAM = 4,
};

#pragma ACCEL kernel
void top_top(int a[10], int b[10]) {
//#pragma ACCEL parallel factor=RAM_1P_BRAM
  for (int i = 0; i < 10; ++i) {
#pragma HLS pipeline II=RAM_3P_BRAM
      b[i]=a[i]+RAM_1P_BRAM;
  }
}

#pragma ACCEL kernel
void top_hls(int a[10], int b[10]) {
enum _ram_type {
   RAM_4P_BRAM = 5,
};
//#pragma ACCEL parallel factor=RAM_1P_BRAM
  for (int i = 0; i < 10; ++i) {
#pragma HLS pipeline II=RAM_4P_BRAM
      b[i]=a[i]+RAM_1P_BRAM;
  }
}

#pragma ACCEL kernel
void top_mer1969(int a[10], int b[10]) {
enum _ram_type {
   RAM_5P_BRAM = 5,
};
 const int II = (int)RAM_5P_BRAM;
//#pragma ACCEL parallel factor=RAM_1P_BRAM
  for (int i = 0; i < 10; ++i) {
#pragma HLS pipeline II=10*II
      b[i]=a[i]+RAM_5P_BRAM;
  }
}
