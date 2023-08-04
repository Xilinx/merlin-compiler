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
#include "block_wise_reformat.h"

void block_wise_reformat_input(data_precision *input,data_precision *input_block_wise)
{
  for (int i_outer = 0; i_outer < 1; ++i_outer) {
    for (int k_outer = 0; k_outer < 166; ++k_outer) {
      for (int i_feeder = 0; i_feeder < 1; ++i_feeder) {
        for (int k_feeder = 0; k_feeder < 1; ++k_feeder) {
          for (int i_row_pe = 0; i_row_pe < 10; ++i_row_pe) {
            for (int k_simd_pe = 0; k_simd_pe < 8; ++k_simd_pe) {
              int in_idx = (i_outer * 10 + i_row_pe + i_feeder) * 1327 + ((k_outer + k_feeder) * 8 + k_simd_pe);
              int in_bw_idx = ((i_outer * 166 + k_outer) * 10 + (i_row_pe + i_feeder)) * 8 + (k_feeder * 8 + k_simd_pe);
              if (i_outer * 10 + i_row_pe + i_feeder < 1 && (k_outer + k_feeder) * 8 + k_simd_pe < 1327) 
                input_block_wise[in_bw_idx] = input[in_idx];
               else 
                input_block_wise[in_bw_idx] = 0;
            }
          }
        }
      }
    }
  }
}

void block_wise_reformat_weight(data_precision *weight,data_precision *weight_block_wise)
{
  for (int j_outer = 0; j_outer < 1; ++j_outer) {
    for (int k_outer = 0; k_outer < 166; ++k_outer) {
      for (int j_feeder = 0; j_feeder < 1; ++j_feeder) {
        for (int k_feeder = 0; k_feeder < 1; ++k_feeder) {
          for (int j_col_pe = 0; j_col_pe < 12; ++j_col_pe) {
            for (int k_simd_pe = 0; k_simd_pe < 8; ++k_simd_pe) {
              int w_idx = (j_outer * 12 + j_col_pe + j_feeder) * 1327 + ((k_outer + k_feeder) * 8 + k_simd_pe);
              int w_bw_idx = ((j_outer * 166 + k_outer) * 12 + (j_col_pe + j_feeder)) * 8 + (k_feeder * 8 + k_simd_pe);
              if (j_outer * 12 + j_col_pe + j_feeder < 1 && (k_outer + k_feeder) * 8 + k_simd_pe < 1327) 
                weight_block_wise[w_bw_idx] = weight[w_idx];
               else 
                weight_block_wise[w_bw_idx] = 0;
            }
          }
        }
      }
    }
  }
}

void block_wise_reformat_output(data_precision *output_block_wise,data_precision *output)
{
  for (int i_outer = 0; i_outer < 1; ++i_outer) {
    for (int i_row_pe = 0; i_row_pe < 10; ++i_row_pe) {
      for (int i_feeder = 0; i_feeder < 1; ++i_feeder) {
        for (int j_outer = 0; j_outer < 1; ++j_outer) {
          for (int j_col_pe = 0; j_col_pe < 12; ++j_col_pe) {
            for (int j_feeder = 0; j_feeder < 1; ++j_feeder) {
              int out_idx = i_outer * 10 + i_row_pe + i_feeder + (j_outer * 12 + j_col_pe + j_feeder);
              int out_bw_idx = ((i_outer + j_outer) * 10 + (i_row_pe + i_feeder)) * 12 + (j_col_pe + j_feeder);
              if (i_outer * 10 + i_row_pe + i_feeder < 1 && j_outer * 12 + j_col_pe + j_feeder < 1) 
                output[out_idx] = output_block_wise[out_bw_idx];
            }
          }
        }
      }
    }
  }
}

void data_reorganization(data_precision *output_fm_drain,data_precision *output_fm_opencl_block_wise)
{
  for (int i_outer = 0; i_outer < 1; ++i_outer) {
    for (int i_row_pe = 0; i_row_pe < 10; ++i_row_pe) {
      for (int i_feeder = 0; i_feeder < 1; ++i_feeder) {
        for (int j_outer = 0; j_outer < 1; ++j_outer) {
          for (int j_col_pe = 0; j_col_pe < 12; ++j_col_pe) {
            for (int j_feeder = 0; j_feeder < 1; ++j_feeder) {
              int out_idx = ((i_outer + j_outer) * 10 + (i_row_pe + i_feeder)) * 12 + (j_col_pe + j_feeder);
              int out_bw_idx = ((i_outer + j_outer) * 10 + (i_row_pe + i_feeder)) * 12 + (j_feeder * 12 + j_col_pe);
              output_fm_opencl_block_wise[out_idx] = output_fm_drain[out_bw_idx];
            }
          }
        }
      }
    }
  }
}
