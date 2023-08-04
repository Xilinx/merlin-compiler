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
void func_top_1(int i, int n) {

    float m_buf[400];
  #pragma HLS array_partition variable=m_buf cyclic factor = 16 dim=1
    float tmp;
    int k = 1;
    int l;
    int j;
    for (j = i; j < n - 1; j++) {
      tmp = -(m_buf[i * 21L + k * 20L] / m_buf[i * 21L]);
      for (l = 0; l <= n; l++) {
 
  #pragma ACCEL PIPELINE AUTO
        m_buf[i * 20L + k * 20L + l] = m_buf[i * 20L + k * 20L + l] + m_buf[i * 20L + l] * tmp;
      }
      k++;
    }
    m_buf[0] = ((int )m_buf[420]);
}
