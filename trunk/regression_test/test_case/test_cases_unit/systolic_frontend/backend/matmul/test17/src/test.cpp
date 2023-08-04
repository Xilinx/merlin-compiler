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
void systolic_array_kernel(const float **A,const float **B,float **C)
{
  
#pragma ACCEL interface variable=A depth=43,75
  
#pragma ACCEL interface variable=B depth=35,75
  
#pragma ACCEL interface variable=C depth=43,35
  
#pragma ACCEL systolic
  for (int k_outer = 0; k_outer < 2; ++k_outer) 
    for (int j_outer = 0; j_outer < 1; ++j_outer) 
      for (int i_outer = 0; i_outer < 2; ++i_outer) {
        
#pragma ACCEL data_reuse scope
        for (int k_feeder = 0; k_feeder < 5; ++k_feeder) 
          for (int i_feeder = 0; i_feeder < 10; ++i_feeder) 
            for (int j_feeder = 0; j_feeder < 1; ++j_feeder) {
              
#pragma ACCEL parallel scope
              for (int i_row_pe = 0; i_row_pe < 4; ++i_row_pe) 
                for (int j_col_pe = 0; j_col_pe < 40; ++j_col_pe) 
                  for (int k_simd_pe = 0; k_simd_pe < 8; ++k_simd_pe) {
                    int i_sys = i_outer * 40 + (i_row_pe * 10 + i_feeder);
                    int j_sys = j_outer * 40 + (j_col_pe * 1 + j_feeder);
                    int k_sys = k_outer * 40 + (k_simd_pe * 5 + k_feeder);
                    C[i_sys][j_sys] += A[i_sys][k_sys] * B[j_sys][k_sys];
                  }
            }
      }
}
