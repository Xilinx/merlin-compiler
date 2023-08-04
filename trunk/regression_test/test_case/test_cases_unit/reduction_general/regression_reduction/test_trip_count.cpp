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
#include<assert.h>
#pragma ACCEL kernel
void add_kernel1(int *m, int data[512][512], int size) {
#pragma ACCEL interface variable=m depth=1
#pragma ACCEL interface variable=data depth=512,512
  int tmp = 0;
  for(int j = 0; j < size; j++) {
    for(int i = j+1; i < 512; i++) {
      for(int k=0; k<j; k++) {
#pragma ACCEL parallel factor=64 
#pragma ACCEL reduction variable=tmp
        tmp += data[i][k] * data[j][k];
      }
    }
  }
  *m = tmp;
}

#pragma ACCEL kernel
void add_kernel2(int *m, int data[512][512], int size) {
#pragma ACCEL interface variable=m depth=1
#pragma ACCEL interface variable=data depth=512,512
  int tmp = 0;
  for(int j = 0; j < 512; j++) {
    for(int i = j+1; i < 512; i++) {
      for(int k=0; k<j; k++) {
#pragma ACCEL parallel factor=64
#pragma ACCEL reduction variable=tmp
#pragma HLS loop_tripcount max=511
        tmp += data[i][k] * data[j][k];
      }
    }
  }
  *m = tmp;
}

#pragma ACCEL kernel
void add_kernel3(int *m, int data[512][512], int size) {
#pragma ACCEL interface variable=m depth=1
#pragma ACCEL interface variable=data depth=512,512
  int tmp = 0;
  for(int j = 0; j < 512; j++) {
    for(int i = j+1; i < 512; i++) {
      for(int k=0; k<512; k++) {
#pragma ACCEL parallel factor=64
#pragma ACCEL reduction variable=tmp
        if(k<j)
          tmp += data[i][k] * data[j][k];
      }
    }
  }
  *m = tmp;
}

#pragma ACCEL kernel
void add_kernel4(int *m, int data[512][512], int size) {
#pragma ACCEL interface variable=m depth=1
#pragma ACCEL interface variable=data depth=512,512
    int tmp = 0;
    for(int j = 0; j < size; j++) {
#pragma ACCEL pipeline
#pragma ACCEL reduction variable=tmp
        for(int k=0; k<j; k++) {
#pragma ACCEL parallel
            tmp += data[j][k];
        }
    }
    *m = tmp;
}
