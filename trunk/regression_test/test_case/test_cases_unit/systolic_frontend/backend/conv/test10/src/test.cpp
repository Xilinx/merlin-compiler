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

void systolic_array_kernel(const float ***merlin_in,float ***merlin_out,const float ****weight)
{
  
#pragma ACCEL interface variable=merlin_in depth=18,10,32
  
#pragma ACCEL interface variable=weight depth=137,3,3,32
  
#pragma ACCEL interface variable=merlin_out depth=16,8,137
  
#pragma ACCEL systolic
  for (int q_outer = 0; q_outer < 1; ++q_outer) 
    for (int p_outer = 0; p_outer < 1; ++p_outer) 
      for (int c_outer = 0; c_outer < 1; ++c_outer) 
        for (int r_outer = 0; r_outer < 2; ++r_outer) 
          for (int i_outer = 0; i_outer < 4; ++i_outer) 
            for (int o_outer = 0; o_outer < 4; ++o_outer) {
              
#pragma ACCEL data_reuse scope
              for (int q_feeder = 0; q_feeder < 3; ++q_feeder) 
                for (int p_feeder = 0; p_feeder < 3; ++p_feeder) 
                  for (int i_feeder = 0; i_feeder < 1; ++i_feeder) 
                      for (int r_feeder = 0; r_feeder < 4; ++r_feeder) 
                    for (int c_feeder = 0; c_feeder < 8; ++c_feeder) 
                        for (int o_feeder = 0; o_feeder < 1; ++o_feeder) {
                          
#pragma ACCEL parallel scope
                          for (int r_row_pe = 0; r_row_pe < 3; ++r_row_pe) 
                            for (int o_col_pe = 0; o_col_pe < 35; ++o_col_pe) 
                              for (int i_simd_pe = 0; i_simd_pe < 8; ++i_simd_pe) {
                                int o_sys = o_outer * 35 + (o_col_pe * 1 + o_feeder);
                                int i_sys = i_outer * 8 + (i_simd_pe * 1 + i_feeder);
                                int r_sys = r_outer * 12 + (r_row_pe * 4 + r_feeder);
                                int c_sys = c_outer * 8 + c_feeder;
                                int p_sys = p_outer * 3 + p_feeder;
                                int q_sys = q_outer * 3 + q_feeder;
                                merlin_out[r_sys][c_sys][o_sys] += merlin_in[r_sys + p_sys][c_sys + q_sys][i_sys] * weight[o_sys][p_sys][q_sys][i_sys];
                              }
                        }
            }
}
