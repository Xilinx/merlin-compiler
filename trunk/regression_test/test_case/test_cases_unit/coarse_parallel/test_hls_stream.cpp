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
#include <string.h> 
#include <hls_stream.h>
#define TILE_FACTOR 100
#define UNROLL_FACTOR 5
#pragma ACCEL kernel
void top(hls::stream<int> a[5],int *b,int *c)
{
  for (int i = 0; i < 10000 / 100; ++i) {
    int c_buf_0[5][20][100];
    int b_buf_0[5][20][100];
    memcpy(((void *)(&b_buf_0[0][0][0])),((const void *)(&b[i * 10000])),sizeof(int ) * 10000);
    for (int jj = 0; jj < 5; ++jj) {
      
      int j0, k;
#pragma ACCEL parallel
      for (j0 = 0; j0 < 100 / 5; ++j0) {
        for (k = 0; k < 100; ++k) {
          int offset = (i * 100 + jj * (100 / 5) + j0) * 100 + k;
          c_buf_0[jj][j0][k] = a[jj].read() + b_buf_0[jj][j0][k];
        }
      }
    }
    memcpy(((void *)(&c[i * 10000])),((const void *)(&c_buf_0[0][0][0])),sizeof(int ) * 10000);
  }
}
