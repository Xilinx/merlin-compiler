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

void systolic_array_kernel(const float **A,const float **B,float **C)
{
  
#pragma ACCEL systolic
  for (int i_outer = 0; i_outer < 1; ++i_outer) 
      for (int r_outer = 0; r_outer < 2; ++r_outer)
        for (int o_outer = 0; o_outer < 1; ++o_outer) {
        
#pragma ACCEL data_reuse scope
        for (int i_feeder = 0; i_feeder < 8; ++i_feeder) 
          for (int r_feeder = 0; r_feeder < 12; ++r_feeder)
		   for (int o_feeder = 0; o_feeder < 2; ++o_feeder) {
            
#pragma ACCEL parallel scope
            for (int r_row_pe = 0; r_row_pe < 5; ++r_row_pe) 
              for (int o_col_pe = 0; o_col_pe < 32; ++o_col_pe) 
                for (int i_simd_pe = 0; i_simd_pe < 8; ++i_simd_pe) {
                  int r_sys = r_outer * 60 + (r_row_pe * 12 + r_feeder);
                  //int o_sys = o_outer * 32 + o_col_pe * 1;
                  int o_sys = o_outer * 64 + (o_col_pe * 2 + o_feeder);
                  int i_sys = i_outer * 64 + (i_simd_pe * 8 + i_feeder);
                  C[r_sys][o_sys] +=  A[r_sys][i_sys] * B[o_sys][i_sys];
                }
          }
      }
}
