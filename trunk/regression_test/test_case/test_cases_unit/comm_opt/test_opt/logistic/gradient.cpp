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
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
typedef float BUS_TYPE;
// to divide feature_size (D)
static void __merlin_dummy_74();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel name="gradient"
#pragma ACCEL kernel
void gradient(int n_samples,int label_size,int feature_size,float *weights,float *data,float *output,float *output_last_element,float *feature_samples,float *label_samples)
//float   global_weights[9*784], 
//BUS_TYPE global_data[32*785], 
//float   global_output[9*784+1])
{
  int dummy_token_80;
// Token comes from node 20 and 21 for sync level 0
  int token_80;
  int dummy_token_79;
// Token comes from node 20 and 12 for sync level 2
  int token_79;
  int dummy_token_78;
// Token comes from node 20 and 9 for sync level 2
  int token_78;
  int dummy_token_77;
// Token comes from node 20 and 8 for sync level 2
  int token_77;
  int dummy_token_76;
// Token comes from node 20 and 7 for sync level 2
  int token_76;
  int dummy_token_75;
// Token comes from node 20 and 6 for sync level 2
  int token_75;
  int dummy_token_74;
// Token comes from node 20 and 5 for sync level 2
  int token_74;
  int dummy_token_73;
// Token comes from node 20 and 4 for sync level 2
  int token_73;
  int dummy_token_72;
// Token comes from node 20 and 3 for sync level 2
  int token_72;
  int dummy_token_71;
// Token comes from node 20 and 2 for sync level 2
  int token_71;
  int dummy_token_70;
// Token comes from node 20 and 1 for sync level 2
  int token_70;
  int dummy_token_69;
// Token comes from node 19 and 21 for sync level 0
  int token_69;
  int dummy_token_68;
// Token comes from node 18 and 21 for sync level 0
  int token_68;
  int dummy_token_67;
// Token comes from node 18 and 20 for sync level 2
  int token_67;
  int dummy_token_66;
// Token comes from node 17 and 21 for sync level 0
  int token_66;
  int dummy_token_65;
// Token comes from node 17 and 20 for sync level 2
  int token_65;
  int dummy_token_64;
// Token comes from node 16 and 21 for sync level 0
  int token_64;
  int dummy_token_63;
// Token comes from node 16 and 18 for sync level 2
  int token_63;
  int dummy_token_62;
// Token comes from node 16 and 17 for sync level 2
  int token_62;
  int dummy_token_61;
// Token comes from node 15 and 21 for sync level 0
  int token_61;
  int dummy_token_60;
// Token comes from node 15 and 16 for sync level 2
  int token_60;
  int dummy_token_59;
// Token comes from node 14 and 21 for sync level 0
  int token_59;
  int dummy_token_58;
// Token comes from node 14 and 19 for sync level 2
  int token_58;
  int dummy_token_57;
// Token comes from node 14 and 15 for sync level 2
  int token_57;
  int dummy_token_56;
// Token comes from node 13 and 21 for sync level 0
  int token_56;
  int dummy_token_55;
// Token comes from node 13 and 19 for sync level 2
  int token_55;
  int dummy_token_54;
// Token comes from node 13 and 15 for sync level 2
  int token_54;
  int dummy_token_53;
// Token comes from node 12 and 21 for sync level 0
  int token_53;
  int dummy_token_52;
// Token comes from node 11 and 21 for sync level 0
  int token_52;
  int dummy_token_51;
// Token comes from node 11 and 14 for sync level 2
  int token_51;
  int dummy_token_50;
// Token comes from node 11 and 13 for sync level 2
  int token_50;
  int dummy_token_49;
// Token comes from node 10 and 21 for sync level 0
  int token_49;
  int dummy_token_48;
// Token comes from node 10 and 14 for sync level 2
  int token_48;
  int dummy_token_47;
// Token comes from node 10 and 13 for sync level 2
  int token_47;
  int dummy_token_46;
// Token comes from node 9 and 21 for sync level 0
  int token_46;
  int dummy_token_45;
// Token comes from node 9 and 11 for sync level 2
  int token_45;
  int dummy_token_44;
// Token comes from node 9 and 10 for sync level 2
  int token_44;
  int dummy_token_43;
// Token comes from node 8 and 21 for sync level 0
  int token_43;
  int dummy_token_42;
// Token comes from node 8 and 11 for sync level 2
  int token_42;
  int dummy_token_41;
// Token comes from node 8 and 10 for sync level 2
  int token_41;
  int dummy_token_40;
// Token comes from node 7 and 21 for sync level 0
  int token_40;
  int dummy_token_39;
// Token comes from node 7 and 11 for sync level 2
  int token_39;
  int dummy_token_38;
// Token comes from node 7 and 10 for sync level 2
  int token_38;
  int dummy_token_37;
// Token comes from node 6 and 21 for sync level 0
  int token_37;
  int dummy_token_36;
// Token comes from node 6 and 11 for sync level 2
  int token_36;
  int dummy_token_35;
// Token comes from node 6 and 10 for sync level 2
  int token_35;
  int dummy_token_34;
// Token comes from node 5 and 21 for sync level 0
  int token_34;
  int dummy_token_33;
// Token comes from node 5 and 11 for sync level 2
  int token_33;
  int dummy_token_32;
// Token comes from node 5 and 10 for sync level 2
  int token_32;
  int dummy_token_31;
// Token comes from node 4 and 21 for sync level 0
  int token_31;
  int dummy_token_30;
// Token comes from node 4 and 11 for sync level 2
  int token_30;
  int dummy_token_29;
// Token comes from node 4 and 10 for sync level 2
  int token_29;
  int dummy_token_28;
// Token comes from node 3 and 21 for sync level 0
  int token_28;
  int dummy_token_27;
// Token comes from node 3 and 11 for sync level 2
  int token_27;
  int dummy_token_26;
// Token comes from node 3 and 10 for sync level 2
  int token_26;
  int dummy_token_25;
// Token comes from node 2 and 21 for sync level 0
  int token_25;
  int dummy_token_24;
// Token comes from node 2 and 11 for sync level 2
  int token_24;
  int dummy_token_23;
// Token comes from node 2 and 10 for sync level 2
  int token_23;
  int dummy_token_22;
// Token comes from node 1 and 21 for sync level 0
  int token_22;
  int dummy_token_21;
// Token comes from node 1 and 11 for sync level 2
  int token_21;
  int dummy_token_20;
// Token comes from node 1 and 10 for sync level 2
  int token_20;
  int dummy_token_19;
// Token comes from node 0 and 20 for sync level 0
  int token_19;
  int dummy_token_18;
// Token comes from node 0 and 19 for sync level 0
  int token_18;
  int dummy_token_17;
// Token comes from node 0 and 18 for sync level 0
  int token_17;
  int dummy_token_16;
// Token comes from node 0 and 17 for sync level 0
  int token_16;
  int dummy_token_15;
// Token comes from node 0 and 16 for sync level 0
  int token_15;
  int dummy_token_14;
// Token comes from node 0 and 15 for sync level 0
  int token_14;
  int dummy_token_13;
// Token comes from node 0 and 14 for sync level 0
  int token_13;
  int dummy_token_12;
// Token comes from node 0 and 13 for sync level 0
  int token_12;
  int dummy_token_11;
// Token comes from node 0 and 12 for sync level 0
  int token_11;
  int dummy_token_10;
// Token comes from node 0 and 11 for sync level 0
  int token_10;
  int dummy_token_9;
// Token comes from node 0 and 10 for sync level 0
  int token_9;
  int dummy_token_8;
// Token comes from node 0 and 9 for sync level 0
  int token_8;
  int dummy_token_7;
// Token comes from node 0 and 8 for sync level 0
  int token_7;
  int dummy_token_6;
// Token comes from node 0 and 7 for sync level 0
  int token_6;
  int dummy_token_5;
// Token comes from node 0 and 6 for sync level 0
  int token_5;
  int dummy_token_4;
// Token comes from node 0 and 5 for sync level 0
  int token_4;
  int dummy_token_3;
// Token comes from node 0 and 4 for sync level 0
  int token_3;
  int dummy_token_2;
// Token comes from node 0 and 3 for sync level 0
  int token_2;
  int dummy_token_1;
// Token comes from node 0 and 2 for sync level 0
  int token_1;
  int dummy_token_0;
// Token comes from node 0 and 1 for sync level 0
  int token_0;
  float output_buf_0_0_rn_rf10[9][196][4];
  float multiplier_buf_0_rf9[16][10];
  float sum_buf_0_rn_0_rf8[16];
  float sum_buf_0_rn_rf7[16];
  float sum_buf_0_rn_rf6[16];
  float margin_buf_0_rn_rf5[16][10];
  float margin_buf_0_rn_rf4[16][10];
  float label_buf_0_rn_1_rf3[16];
  float sum_buf_0_rf2[16];
  float weights_buf_0_0_rf1[196][4][9];
  float output_buf_0_0_rf0[9][196][4];
  float feature_buf_0_tmp_0;
  float feature_buf_0_rn_0[16][784];
  float feature_buf_0_tmp;
  float feature_buf_0_rn[16][784];
  float label_buf_0_tmp_2;
  float label_buf_0_rn_2[16];
  float label_buf_0_tmp_1;
  float label_buf_0_rn_1[16];
  float label_buf_0_tmp_0;
  float label_buf_0_rn_0[16];
  float label_buf_0_tmp;
  float label_buf_0_rn[16];
  float margin_buf_0_rn[16][10];
  float sum_buf_0_rn_0[16];
  float sum_buf_0_rn[16];
  float output_buf_0_0_rn[9][196][4];
  int _memcpy_i3_2;
  int _memcpy_i3_1;
  int _memcpy_i3_0;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  int num_samples_0;
  float margins_0[16][10 - 1];
  float label_buf_0[16];
  float feature_buf_0[16][784];
  float marginY_buf_0[16];
  float maxMargin_buf_0[16];
  int maxMarginIndex_buf_0[16];
  int upper_bound_0;
  int j0_0;
  int kk_0;
  int i_0;
  float output_loss_buffer_0_arr[1];
  float output_loss_buffer_0;
  float sum_buf_0[16];
  float margin_buf_0[16][10];
  float multiplier_buf_0[16][10];
  int t_size_0;
  int t_0;
  int k_0;
  int _in_j0_1_0;
  int _in_jj_0_0;
  int k_1;
  int j_0;
  int _in_j0_0_0;
  int _in_jj_1;
  int j_1;
  float label_0;
  float label_1;
  float tmp_0;
  int _in_j0_2;
  int j_2;
  float label_2;
  float loss_0;
  float output_buf_0_0[9][196][4];
  float weights_buf_0_0[196][4][9];
  float label_samples_buf_0_0[16];
  int __memcpy_i_2_0;
  
#pragma ACCEL interface variable=label_samples depth=2560 max_depth=2560
  
#pragma ACCEL interface variable=feature_samples depth=2007040 max_depth=2007040
  
#pragma ACCEL interface variable=output_last_element depth=1 max_depth=1
  
#pragma ACCEL interface variable=output depth=7056 max_depth=7056
  
#pragma ACCEL interface variable=data depth=2009600 max_depth=2009600
  
#pragma ACCEL interface variable=weights depth=7056 max_depth=7056
//FIXME:
// const int L = 10; //label_size;
// const int D = 784; //feature_size;
// const int weight_size = (L-1)*D;
// ZP: 2015-11-15
//memset(output, 0, sizeof(float)*(weight_size+1));
//float* margins = new float[L-1];
//-std::numeric_limits<float>::infinity();
  num_samples_0 = n_samples;
  upper_bound_0 = (n_samples + 16 - 1) / 16;
//for(int k = 0; k < num_samples; k++ ) {
  n_samples % 16 == 0?((void )0) : __assert_fail("(n_samples % 16) == 0","gradient.cpp",51,__PRETTY_FUNCTION__);
//float output_loss_buffer = output[weight_size];
  output_loss_buffer_0_arr[0] =  *output_last_element;
// Token gen: node 0 and 20 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_19 = 1;
// Token gen: node 0 and 19 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_18 = 1;
// Token gen: node 0 and 18 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_17 = 1;
// Token gen: node 0 and 17 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_16 = 1;
// Token gen: node 0 and 16 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_15 = 1;
// Token gen: node 0 and 15 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_14 = 1;
// Token gen: node 0 and 14 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_13 = 1;
// Token gen: node 0 and 13 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_12 = 1;
// Token gen: node 0 and 12 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_11 = 1;
// Token gen: node 0 and 11 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_10 = 1;
// Token gen: node 0 and 10 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_9 = 1;
// Token gen: node 0 and 9 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_8 = 1;
// Token gen: node 0 and 8 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_7 = 1;
// Token gen: node 0 and 7 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_6 = 1;
// Token gen: node 0 and 6 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_5 = 1;
// Token gen: node 0 and 5 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_4 = 1;
// Token gen: node 0 and 4 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_3 = 1;
// Token gen: node 0 and 3 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_2 = 1;
// Token gen: node 0 and 2 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_1 = 1;
// Token gen: node 0 and 1 for sync level 0(forward)
  
#pragma ACCEL mem_fence
  token_0 = 1;
  for (int k0 = 0; k0 < (n_samples + 16 - 1) / 16; k0++) 
// Original: #pragma ACCEL pipeline
{
    for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 9; ++_memcpy_i0_2) {
      for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 196; ++_memcpy_i0_1) {
        for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 4; ++_memcpy_i0_0) {
// Token gen: node 0 and 1 for sync level 0(forward)
          if (_memcpy_i0_0 == 0 && _memcpy_i0_1 == 0 && _memcpy_i0_2 == 0 && k0 == 0) {
            dummy_token_0 = token_0;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 20 and 1 for sync level 2(backward)
          if (_memcpy_i0_0 == 0 && _memcpy_i0_1 == 0 && _memcpy_i0_2 == 0 && k0 != 0) {
            dummy_token_70 = token_70;
            
#pragma ACCEL mem_fence
          }
          output_buf_0_0[_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = output[0 + (((0 * 9 + _memcpy_i0_2) * 196 + _memcpy_i0_1) * 4 + _memcpy_i0_0)];
// Token gen: node 1 and 21 for sync level 0(forward)
          if (_memcpy_i0_0 == 3 && _memcpy_i0_1 == 195 && _memcpy_i0_2 == 8 && k0 == -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_22 = 1;
          }
// Token gen: node 1 and 11 for sync level 2(forward)
          if (_memcpy_i0_0 == 3 && _memcpy_i0_1 == 195 && _memcpy_i0_2 == 8) {
            
#pragma ACCEL mem_fence
            token_21 = 1;
          }
// Token gen: node 1 and 10 for sync level 2(forward)
          if (_memcpy_i0_0 == 3 && _memcpy_i0_1 == 195 && _memcpy_i0_2 == 8) {
            
#pragma ACCEL mem_fence
            token_20 = 1;
          }
        }
      }
    }
    for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 196; ++_memcpy_i1_2) {
// Token gen: node 0 and 2 for sync level 0(forward)
      if (_memcpy_i1_2 == 0 && k0 == 0) {
        dummy_token_1 = token_1;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 20 and 2 for sync level 2(backward)
      if (_memcpy_i1_2 == 0 && k0 != 0) {
        dummy_token_71 = token_71;
        
#pragma ACCEL mem_fence
      }
      for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 4; ++_memcpy_i1_1) {
// Parallel pragma comes from SgInitializedName:weights_buf_0_0
        
#pragma ACCEL parallel
        for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 9; ++_memcpy_i1_0) {
// Parallel pragma comes from SgInitializedName:weights_buf_0_0
          
#pragma ACCEL parallel
          weights_buf_0_0[_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0] = weights[0 + (((0 * 196 + _memcpy_i1_2) * 4 + _memcpy_i1_1) * 9 + _memcpy_i1_0)];
        }
      }
// Token gen: node 2 and 21 for sync level 0(forward)
      if (_memcpy_i1_2 == 195 && k0 == -1 + (15 + n_samples) / 16) {
        
#pragma ACCEL mem_fence
        token_25 = 1;
      }
// Token gen: node 2 and 11 for sync level 2(forward)
      if (_memcpy_i1_2 == 195) {
        
#pragma ACCEL mem_fence
        token_24 = 1;
      }
// Token gen: node 2 and 10 for sync level 2(forward)
      if (_memcpy_i1_2 == 195) {
        
#pragma ACCEL mem_fence
        token_23 = 1;
      }
    }
    
#pragma ACCEL PIPELINE II=1
//#pragma HLS loop_tripcount max=3750
    
#pragma HLS loop_tripcount max=256
// combine these two nodes because they both access data
{
      for (__memcpy_i_2_0 = 0; __memcpy_i_2_0 < sizeof(float ) * ((unsigned long )16) / 4; ++__memcpy_i_2_0) {
// Token gen: node 0 and 3 for sync level 0(forward)
        if (__memcpy_i_2_0 == 0 && k0 == 0) {
          dummy_token_2 = token_2;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 20 and 3 for sync level 2(backward)
        if (__memcpy_i_2_0 == 0 && k0 != 0) {
          dummy_token_72 = token_72;
          
#pragma ACCEL mem_fence
        }
        label_samples_buf_0_0[__memcpy_i_2_0 + 0] = label_samples[__memcpy_i_2_0 + k0 * 16];
// Token gen: node 3 and 21 for sync level 0(forward)
        if (__memcpy_i_2_0 == 15 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_28 = 1;
        }
// Token gen: node 3 and 11 for sync level 2(forward)
        if (__memcpy_i_2_0 == 15) {
          
#pragma ACCEL mem_fence
          token_27 = 1;
        }
// Token gen: node 3 and 10 for sync level 2(forward)
        if (__memcpy_i_2_0 == 15) {
          
#pragma ACCEL mem_fence
          token_26 = 1;
        }
      }
// Token gen: node 0 and 4 for sync level 0(forward)
      if (k0 == 0) {
        dummy_token_3 = token_3;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 20 and 4 for sync level 2(backward)
      if (k0 != 0) {
        dummy_token_73 = token_73;
        
#pragma ACCEL mem_fence
      }
      t_size_0 = 16;
// Token gen: node 4 and 21 for sync level 0(forward)
      if (k0 == -1 + (15 + n_samples) / 16) {
        
#pragma ACCEL mem_fence
        token_31 = 1;
      }
// Token gen: node 4 and 11 for sync level 2(forward)
      
#pragma ACCEL mem_fence
      token_30 = 1;
// Token gen: node 4 and 10 for sync level 2(forward)
      
#pragma ACCEL mem_fence
      token_29 = 1;
      for (t_0 = 0; t_0 < 16; t_0++) {
// Token gen: node 0 and 5 for sync level 0(forward)
        if (t_0 == 0 && k0 == 0) {
          dummy_token_4 = token_4;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 20 and 5 for sync level 2(backward)
        if (t_0 == 0 && k0 != 0) {
          dummy_token_74 = token_74;
          
#pragma ACCEL mem_fence
        }
        sum_buf_0[t_0] = 0;
// Token gen: node 5 and 21 for sync level 0(forward)
        if (t_0 == 15 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_34 = 1;
        }
// Token gen: node 5 and 11 for sync level 2(forward)
        if (t_0 == 15) {
          
#pragma ACCEL mem_fence
          token_33 = 1;
        }
// Token gen: node 5 and 10 for sync level 2(forward)
        if (t_0 == 15) {
          
#pragma ACCEL mem_fence
          token_32 = 1;
        }
      }
      for (t_0 = 0; t_0 < 16; t_0++) {
        for (int t1 = 0; t1 < 10; t1++) {
// Token gen: node 0 and 6 for sync level 0(forward)
          if (t1 == 0 && t_0 == 0 && k0 == 0) {
            dummy_token_5 = token_5;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 20 and 6 for sync level 2(backward)
          if (t1 == 0 && t_0 == 0 && k0 != 0) {
            dummy_token_75 = token_75;
            
#pragma ACCEL mem_fence
          }
          margin_buf_0[t_0][t1] = 0;
// Token gen: node 6 and 21 for sync level 0(forward)
          if (t1 == 9 && t_0 == 15 && k0 == -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_37 = 1;
          }
// Token gen: node 6 and 11 for sync level 2(forward)
          if (t1 == 9 && t_0 == 15) {
            
#pragma ACCEL mem_fence
            token_36 = 1;
          }
// Token gen: node 6 and 10 for sync level 2(forward)
          if (t1 == 9 && t_0 == 15) {
            
#pragma ACCEL mem_fence
            token_35 = 1;
          }
        }
      }
//for (t = 0; t < t_size; t++) for (int t1 = 0; t1 < L; t1++)multiplier_buf[t][t1]= 0;
//#pragma ACCEL pipeline 
/*INIT: */
      for (int kk = 0; kk < 16; kk++) {
// Token gen: node 0 and 7 for sync level 0(forward)
        if (kk == 0 && k0 == 0) {
          dummy_token_6 = token_6;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 20 and 7 for sync level 2(backward)
        if (kk == 0 && k0 != 0) {
          dummy_token_76 = token_76;
          
#pragma ACCEL mem_fence
        }
        k_0 = k0 * 16 + kk;
//              if (k >= num_samples) break;
//marginY_buf[kk] = 0.0;
//-std::numeric_limits<float>::infinity();
        maxMargin_buf_0[kk] = -__builtin_inff();
        maxMarginIndex_buf_0[kk] = 0;
//label_buf[kk] = data[k*(D+1)];
        label_buf_0_tmp = label_samples_buf_0_0[kk];
        label_buf_0_tmp_0 = label_buf_0_tmp;
        label_buf_0_tmp_1 = label_buf_0_tmp_0;
        label_buf_0_tmp_2 = label_buf_0_tmp_1;
        label_buf_0[kk] = label_buf_0_tmp_2;
        label_buf_0_rn_2[kk] = label_buf_0_tmp_2;
        label_buf_0_rn_1[kk] = label_buf_0_tmp_1;
        label_buf_0_rn_0[kk] = label_buf_0_tmp_0;
        label_buf_0_rn[kk] = label_buf_0_tmp;
// Token gen: node 7 and 21 for sync level 0(forward)
        if (kk == 15 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_40 = 1;
        }
// Token gen: node 7 and 11 for sync level 2(forward)
        if (kk == 15) {
          
#pragma ACCEL mem_fence
          token_39 = 1;
        }
// Token gen: node 7 and 10 for sync level 2(forward)
        if (kk == 15) {
          
#pragma ACCEL mem_fence
          token_38 = 1;
        }
      }
//      }
//
//      {
// Stantardize from: for(int j0 = 0;j0 < 784 / 4;j0 += 1) {...}
      for (int j0 = 0; j0 <= 195; j0++) {
// Token gen: node 0 and 8 for sync level 0(forward)
        if (j0 == 0 && k0 == 0) {
          dummy_token_7 = token_7;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 20 and 8 for sync level 2(backward)
        if (j0 == 0 && k0 != 0) {
          dummy_token_77 = token_77;
          
#pragma ACCEL mem_fence
        }
        _in_j0_1_0 = 0 + ((int )1) * j0;
// Token gen: node 8 and 21 for sync level 0(forward)
        if (j0 == 195 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_43 = 1;
        }
// Token gen: node 8 and 11 for sync level 2(forward)
        if (j0 == 195) {
          
#pragma ACCEL mem_fence
          token_42 = 1;
        }
// Token gen: node 8 and 10 for sync level 2(forward)
        if (j0 == 195) {
          
#pragma ACCEL mem_fence
          token_41 = 1;
        }
        for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline flatten
{
// Token gen: node 0 and 9 for sync level 0(forward)
          if (kk == 0 && j0 == 0 && k0 == 0) {
            dummy_token_8 = token_8;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 20 and 9 for sync level 2(backward)
          if (kk == 0 && j0 == 0 && k0 != 0) {
            dummy_token_78 = token_78;
            
#pragma ACCEL mem_fence
          }
          
#pragma ACCEL PIPELINE II=1
// Stantardize from: for(int jj = 0;jj < 4;jj += 1) {...}
          for (int jj = 0; jj <= 3; jj++) {
            
#pragma ACCEL PARALLEL COMPLETE
            _in_jj_0_0 = 0 + ((int )1) * jj;
            k_1 = k0 * 16 + kk;
            j_0 = (0 + ((int )1) * j0) * 4 + _in_jj_0_0;
            feature_buf_0_tmp = feature_samples[k_1 * 784 + j_0];
            feature_buf_0_tmp_0 = feature_buf_0_tmp;
            feature_buf_0[kk][j_0] = feature_buf_0_tmp_0;
            feature_buf_0_rn_0[kk][j_0] = feature_buf_0_tmp_0;
            feature_buf_0_rn[kk][j_0] = feature_buf_0_tmp;
          }
// Token gen: node 9 and 21 for sync level 0(forward)
          if (kk == 15 && j0 == 195 && k0 == -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_46 = 1;
          }
// Token gen: node 9 and 11 for sync level 2(forward)
          if (kk == 15 && j0 == 195) {
            
#pragma ACCEL mem_fence
            token_45 = 1;
          }
// Token gen: node 9 and 10 for sync level 2(forward)
          if (kk == 15 && j0 == 195) {
            
#pragma ACCEL mem_fence
            token_44 = 1;
          }
        }
      }
    }
/*STAGE_1:*/
//for(j0 = 0; j0 < D; j0+=F_UR ) {
// Stantardize from: for(j0 = 0;j0 < 784 / 4;j0 += 1) {...}
    for (j0_0 = 0; j0_0 <= 195; j0_0++) {
// Token gen: node 0 and 10 for sync level 0(forward)
      if (j0_0 == 0 && k0 == 0) {
        dummy_token_9 = token_9;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 1 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_20 = token_20;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 2 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_23 = token_23;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 3 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_26 = token_26;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 4 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_29 = token_29;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 5 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_32 = token_32;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 6 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_35 = token_35;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 7 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_38 = token_38;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 8 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_41 = token_41;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 9 and 10 for sync level 2(forward)
      if (j0_0 == 0) {
        dummy_token_44 = token_44;
        
#pragma ACCEL mem_fence
      }
      _in_j0_0_0 = 0 + ((int )1) * j0_0;
// Token gen: node 10 and 21 for sync level 0(forward)
      if (j0_0 == 195 && k0 == -1 + (15 + n_samples) / 16) {
        
#pragma ACCEL mem_fence
        token_49 = 1;
      }
// Token gen: node 10 and 14 for sync level 2(forward)
      if (j0_0 == 195) {
        
#pragma ACCEL mem_fence
        token_48 = 1;
      }
// Token gen: node 10 and 13 for sync level 2(forward)
      if (j0_0 == 195) {
        
#pragma ACCEL mem_fence
        token_47 = 1;
      }
      for (kk_0 = 0; kk_0 < 16; kk_0++) 
// Original: #pragma ACCEL pipeline flatten
{
// Token gen: node 0 and 11 for sync level 0(forward)
        if (kk_0 == 0 && j0_0 == 0 && k0 == 0) {
          dummy_token_10 = token_10;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 1 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_21 = token_21;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 2 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_24 = token_24;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 3 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_27 = token_27;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 4 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_30 = token_30;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 5 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_33 = token_33;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 6 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_36 = token_36;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 7 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_39 = token_39;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 8 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_42 = token_42;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 9 and 11 for sync level 2(forward)
        if (kk_0 == 0 && j0_0 == 0) {
          dummy_token_45 = token_45;
          
#pragma ACCEL mem_fence
        }
        if (kk_0 == 0) {
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL parallel
            for (int jj = 0; jj <= 3; jj++) {
              
#pragma ACCEL parallel
              weights_buf_0_0_rf1[j0_0][jj][i] = weights_buf_0_0[j0_0][jj][i];
            }
          }
        }
        
#pragma ACCEL PIPELINE II=1
// Stantardize from: for(int jj = 0;jj < 4;jj += 1) {...}
        for (int jj = 0; jj <= 3; jj++) {
          
#pragma ACCEL PARALLEL COMPLETE
          _in_jj_1 = 0 + ((int )1) * jj;
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL PARALLEL COMPLETE
            j_1 = (0 + ((int )1) * j0_0) * 4 + _in_jj_1;
//margin_buf[kk][i] += weights[i*D+j] * feature_buf[kk][j];
            margin_buf_0[kk_0][i] += weights_buf_0_0_rf1[j0_0][jj][i] * feature_buf_0_rn[kk_0][j_1];
            margin_buf_0_rn_rf4[kk_0][i] = margin_buf_0[kk_0][i];
          }
        }
        if (j0_0 == 195) {
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL parallel
            margin_buf_0_rn[kk_0][i] = margin_buf_0_rn_rf4[kk_0][i];
          }
        }
// Token gen: node 11 and 21 for sync level 0(forward)
        if (kk_0 == 15 && j0_0 == 195 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_52 = 1;
        }
// Token gen: node 11 and 14 for sync level 2(forward)
        if (kk_0 == 15 && j0_0 == 195) {
          
#pragma ACCEL mem_fence
          token_51 = 1;
        }
// Token gen: node 11 and 13 for sync level 2(forward)
        if (kk_0 == 15 && j0_0 == 195) {
          
#pragma ACCEL mem_fence
          token_50 = 1;
        }
      }
    }
// Token gen: node 0 and 12 for sync level 0(forward)
    if (k0 == 0) {
      dummy_token_11 = token_11;
      
#pragma ACCEL mem_fence
    }
// Token gen: node 20 and 12 for sync level 2(backward)
    if (k0 != 0) {
      dummy_token_79 = token_79;
      
#pragma ACCEL mem_fence
    }
    j0_0 = 195 + ((int )1);
// Token gen: node 12 and 21 for sync level 0(forward)
    if (k0 == -1 + (15 + n_samples) / 16) {
      
#pragma ACCEL mem_fence
      token_53 = 1;
    }
// STAGE_2:      
{
//#pragma ACCEL pipeline
      for (kk_0 = 0; kk_0 < 16; kk_0++) {
// Token gen: node 0 and 13 for sync level 0(forward)
        if (kk_0 == 0 && k0 == 0) {
          dummy_token_12 = token_12;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 10 and 13 for sync level 2(forward)
        if (kk_0 == 0) {
          dummy_token_47 = token_47;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 11 and 13 for sync level 2(forward)
        if (kk_0 == 0) {
          dummy_token_50 = token_50;
          
#pragma ACCEL mem_fence
        }
        label_0 = label_buf_0_rn[kk_0];
        marginY_buf_0[kk_0] = margin_buf_0[kk_0][((int )label_0) - 1];
// Token gen: node 13 and 21 for sync level 0(forward)
        if (kk_0 == 15 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_56 = 1;
        }
// Token gen: node 13 and 19 for sync level 2(forward)
        if (kk_0 == 15) {
          
#pragma ACCEL mem_fence
          token_55 = 1;
        }
// Token gen: node 13 and 15 for sync level 2(forward)
        if (kk_0 == 15) {
          
#pragma ACCEL mem_fence
          token_54 = 1;
        }
      }
      for (i_0 = 0; i_0 < 10 - 1; i_0++) {
        for (kk_0 = 0; kk_0 < 16; kk_0++) {
//  #pragma ACCEL pipeline II=2
// Token gen: node 0 and 14 for sync level 0(forward)
          if (kk_0 == 0 && i_0 == 0 && k0 == 0) {
            dummy_token_13 = token_13;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 10 and 14 for sync level 2(forward)
          if (kk_0 == 0 && i_0 == 0) {
            dummy_token_48 = token_48;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 11 and 14 for sync level 2(forward)
          if (kk_0 == 0 && i_0 == 0) {
            dummy_token_51 = token_51;
            
#pragma ACCEL mem_fence
          }
          margin_buf_0_rn_rf5[kk_0][i_0] = margin_buf_0_rn[kk_0][i_0];
          if (margin_buf_0_rn_rf5[kk_0][i_0] > maxMargin_buf_0[kk_0]) {
            maxMargin_buf_0[kk_0] = margin_buf_0_rn_rf5[kk_0][i_0];
            maxMarginIndex_buf_0[kk_0] = i_0;
          }
          margins_0[kk_0][i_0] = margin_buf_0_rn_rf5[kk_0][i_0];
// Token gen: node 14 and 21 for sync level 0(forward)
          if (kk_0 == 15 && i_0 == 8 && k0 == -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_59 = 1;
          }
// Token gen: node 14 and 19 for sync level 2(forward)
          if (kk_0 == 15 && i_0 == 8) {
            
#pragma ACCEL mem_fence
            token_58 = 1;
          }
// Token gen: node 14 and 15 for sync level 2(forward)
          if (kk_0 == 15 && i_0 == 8) {
            
#pragma ACCEL mem_fence
            token_57 = 1;
          }
        }
      }
/*STAGE_2: */
//float sum = 0.0;
      for (i_0 = 0; i_0 < 10 - 1; i_0++) {
//#pragma ACCEL pipeline
        for (kk_0 = 0; kk_0 < 16; kk_0++) {
// Token gen: node 0 and 15 for sync level 0(forward)
          if (kk_0 == 0 && i_0 == 0 && k0 == 0) {
            dummy_token_14 = token_14;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 13 and 15 for sync level 2(forward)
          if (kk_0 == 0 && i_0 == 0) {
            dummy_token_54 = token_54;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 14 and 15 for sync level 2(forward)
          if (kk_0 == 0 && i_0 == 0) {
            dummy_token_57 = token_57;
            
#pragma ACCEL mem_fence
          }
          if (i_0 == 0) {
            sum_buf_0_rf2[kk_0] = sum_buf_0[kk_0];
          }
          if (maxMargin_buf_0[kk_0] > 0) {
            margins_0[kk_0][i_0] -= maxMargin_buf_0[kk_0];
            if (i_0 == maxMarginIndex_buf_0[kk_0]) {
              sum_buf_0_rf2[kk_0] += exp((-maxMargin_buf_0[kk_0]));
              sum_buf_0_rn_0_rf8[kk_0] = sum_buf_0_rf2[kk_0];
              sum_buf_0_rn_rf6[kk_0] = sum_buf_0_rf2[kk_0];
            }
             else {
              sum_buf_0_rf2[kk_0] += exp(margins_0[kk_0][i_0]);
              sum_buf_0_rn_0_rf8[kk_0] = sum_buf_0_rf2[kk_0];
              sum_buf_0_rn_rf6[kk_0] = sum_buf_0_rf2[kk_0];
            }
          }
           else {
            sum_buf_0_rf2[kk_0] += exp(margins_0[kk_0][i_0]);
            sum_buf_0_rn_0_rf8[kk_0] = sum_buf_0_rf2[kk_0];
            sum_buf_0_rn_rf6[kk_0] = sum_buf_0_rf2[kk_0];
          }
          if (i_0 == 8) {
            sum_buf_0_rn_0[kk_0] = sum_buf_0_rn_0_rf8[kk_0];
          }
          if (i_0 == 8) {
            sum_buf_0_rn[kk_0] = sum_buf_0_rn_rf6[kk_0];
          }
// Token gen: node 15 and 21 for sync level 0(forward)
          if (kk_0 == 15 && i_0 == 8 && k0 == -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_61 = 1;
          }
// Token gen: node 15 and 16 for sync level 2(forward)
          if (kk_0 == 15 && i_0 == 8) {
            
#pragma ACCEL mem_fence
            token_60 = 1;
          }
        }
      }
      for (int kk = 0; kk < 16; kk++) {
// Token gen: node 0 and 16 for sync level 0(forward)
        if (kk == 0 && k0 == 0) {
          dummy_token_15 = token_15;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 15 and 16 for sync level 2(forward)
        if (kk == 0) {
          dummy_token_60 = token_60;
          
#pragma ACCEL mem_fence
        }
        label_buf_0_rn_1_rf3[kk] = label_buf_0_rn_1[kk];
        sum_buf_0_rn_rf7[kk] = sum_buf_0_rn[kk];
        label_1 = label_buf_0_rn_0[kk];
// update gradient
//#pragma ACCEL pipeline 
        for (int i = 0; i < 10 - 1; i++) {
// Parallel pragma comes from SgInitializedName:multiplier_buf_0
          
#pragma ACCEL parallel
          tmp_0 = ((float )(exp(((double )margins_0[kk][i])) / (((double )sum_buf_0_rn_rf7[kk]) + 1.0)));
          if (label_buf_0_rn_1_rf3[kk] != 0.0 && label_buf_0_rn_1_rf3[kk] == (i + 1)) {
            tmp_0 -= 1.0;
          }
          multiplier_buf_0[kk][i] = tmp_0;
        }
// Token gen: node 16 and 21 for sync level 0(forward)
        if (kk == 15 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_64 = 1;
        }
// Token gen: node 16 and 18 for sync level 2(forward)
        if (kk == 15) {
          
#pragma ACCEL mem_fence
          token_63 = 1;
        }
// Token gen: node 16 and 17 for sync level 2(forward)
        if (kk == 15) {
          
#pragma ACCEL mem_fence
          token_62 = 1;
        }
//FIXME:  
      }
    }
//STAGE_3: 
    for (kk_0 = 0; kk_0 < 16; kk_0++) {
// Stantardize from: for(j0 = 0;j0 < 784;j0 += 4) {...}
      for (j0_0 = 0; j0_0 <= 195; j0_0++) 
// Original: #pragma ACCEL pipeline flatten
{
// Token gen: node 0 and 17 for sync level 0(forward)
        if (j0_0 == 0 && kk_0 == 0 && k0 == 0) {
          dummy_token_16 = token_16;
          
#pragma ACCEL mem_fence
        }
// Token gen: node 16 and 17 for sync level 2(forward)
        if (j0_0 == 0 && kk_0 == 0) {
          dummy_token_62 = token_62;
          
#pragma ACCEL mem_fence
        }
        if (kk_0 == 0) {
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL parallel
            for (int jj = 0; jj < 4; jj++) {
              
#pragma ACCEL parallel
              output_buf_0_0_rf0[i][j0_0][jj] = output_buf_0_0[i][j0_0][jj];
            }
          }
        }
        if (j0_0 == 0) {
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL parallel
            multiplier_buf_0_rf9[kk_0][i] = multiplier_buf_0[kk_0][i];
          }
        }
        
#pragma ACCEL PIPELINE II=1
        _in_j0_2 = 0 + ((int )4) * j0_0;
        for (int jj = 0; jj < 4; jj++) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL PARALLEL COMPLETE
            j_2 = _in_j0_2 + jj;
            output_buf_0_0_rf0[i][j0_0][jj] += multiplier_buf_0_rf9[kk_0][i] * feature_buf_0_rn_0[kk_0][j_2];
            output_buf_0_0_rn_rf10[i][j0_0][jj] = output_buf_0_0_rf0[i][j0_0][jj];
          }
        }
        if (kk_0 == 15) {
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL parallel
            for (int jj = 0; jj < 4; jj++) {
              
#pragma ACCEL parallel
              output_buf_0_0_rn[i][j0_0][jj] = output_buf_0_0_rn_rf10[i][j0_0][jj];
            }
          }
        }
// Token gen: node 17 and 21 for sync level 0(forward)
        if (j0_0 == 195 && kk_0 == 15 && k0 == -1 + (15 + n_samples) / 16) {
          
#pragma ACCEL mem_fence
          token_66 = 1;
        }
// Token gen: node 17 and 20 for sync level 2(forward)
        if (j0_0 == 195 && kk_0 == 15) {
          
#pragma ACCEL mem_fence
          token_65 = 1;
        }
      }
// Token gen: node 0 and 18 for sync level 0(forward)
      if (kk_0 == 0 && k0 == 0) {
        dummy_token_17 = token_17;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 16 and 18 for sync level 2(forward)
      if (kk_0 == 0) {
        dummy_token_63 = token_63;
        
#pragma ACCEL mem_fence
      }
      j0_0 = 780 + ((int )4);
// Token gen: node 18 and 21 for sync level 0(forward)
      if (kk_0 == 15 && k0 == -1 + (15 + n_samples) / 16) {
        
#pragma ACCEL mem_fence
        token_68 = 1;
      }
// Token gen: node 18 and 20 for sync level 2(forward)
      if (kk_0 == 15) {
        
#pragma ACCEL mem_fence
        token_67 = 1;
      }
    }
//STAGE_4: 
//#pragma ACCEL pipeline 
    for (kk_0 = 0; kk_0 < 16; kk_0++) {
// Token gen: node 0 and 19 for sync level 0(forward)
      if (kk_0 == 0 && k0 == 0) {
        dummy_token_18 = token_18;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 13 and 19 for sync level 2(forward)
      if (kk_0 == 0) {
        dummy_token_55 = token_55;
        
#pragma ACCEL mem_fence
      }
// Token gen: node 14 and 19 for sync level 2(forward)
      if (kk_0 == 0) {
        dummy_token_58 = token_58;
        
#pragma ACCEL mem_fence
      }
      label_2 = label_buf_0_rn_2[kk_0];
      loss_0 = ((float )(log(((double )(sum_buf_0_rn_0[kk_0] + ((float )1))))));
      if (label_2 > 0.0) {
        loss_0 -= marginY_buf_0[kk_0];
      }
      if (maxMargin_buf_0[kk_0] > 0) {
        loss_0 += maxMargin_buf_0[kk_0];
      }
//output[weight_size] += loss;
      output_loss_buffer_0_arr[0] += loss_0;
// Token gen: node 19 and 21 for sync level 0(forward)
      if (kk_0 == 15 && k0 == -1 + (15 + n_samples) / 16) {
        
#pragma ACCEL mem_fence
        token_69 = 1;
      }
    }
    for (_memcpy_i3_2 = 0; _memcpy_i3_2 < 9; ++_memcpy_i3_2) {
      for (_memcpy_i3_1 = 0; _memcpy_i3_1 < 196; ++_memcpy_i3_1) {
        for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 4; ++_memcpy_i3_0) {
// Token gen: node 0 and 20 for sync level 0(forward)
          if (_memcpy_i3_0 == 0 && _memcpy_i3_1 == 0 && _memcpy_i3_2 == 0 && k0 == 0) {
            dummy_token_19 = token_19;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 17 and 20 for sync level 2(forward)
          if (_memcpy_i3_0 == 0 && _memcpy_i3_1 == 0 && _memcpy_i3_2 == 0) {
            dummy_token_65 = token_65;
            
#pragma ACCEL mem_fence
          }
// Token gen: node 18 and 20 for sync level 2(forward)
          if (_memcpy_i3_0 == 0 && _memcpy_i3_1 == 0 && _memcpy_i3_2 == 0) {
            dummy_token_67 = token_67;
            
#pragma ACCEL mem_fence
          }
          output[0 + (((0 * 9 + _memcpy_i3_2) * 196 + _memcpy_i3_1) * 4 + _memcpy_i3_0)] = output_buf_0_0_rn[_memcpy_i3_2][_memcpy_i3_1][_memcpy_i3_0];
// Token gen: node 20 and 21 for sync level 0(forward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 == -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_80 = 1;
          }
// Token gen: node 20 and 12 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_79 = 1;
          }
// Token gen: node 20 and 9 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_78 = 1;
          }
// Token gen: node 20 and 8 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_77 = 1;
          }
// Token gen: node 20 and 7 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_76 = 1;
          }
// Token gen: node 20 and 6 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_75 = 1;
          }
// Token gen: node 20 and 5 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_74 = 1;
          }
// Token gen: node 20 and 4 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_73 = 1;
          }
// Token gen: node 20 and 3 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_72 = 1;
          }
// Token gen: node 20 and 2 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_71 = 1;
          }
// Token gen: node 20 and 1 for sync level 2(backward)
          if (_memcpy_i3_0 == 3 && _memcpy_i3_1 == 195 && _memcpy_i3_2 == 8 && k0 != -1 + (15 + n_samples) / 16) {
            
#pragma ACCEL mem_fence
            token_70 = 1;
          }
        }
      }
    }
// compute loss
// FIXME:
// math.logip(sum)
  }
//memset(sum_buf, 0, sizeof(float) * CHUNK_SIZE);
//memset(margin_buf, 0, sizeof(float) * CHUNK_SIZE*L);
//memset(multiplier_buf, 0, sizeof(float) * CHUNK_SIZE*L);
// Token gen: node 1 and 21 for sync level 0(forward)
  dummy_token_22 = token_22;
  
#pragma ACCEL mem_fence
// Token gen: node 2 and 21 for sync level 0(forward)
  dummy_token_25 = token_25;
  
#pragma ACCEL mem_fence
// Token gen: node 3 and 21 for sync level 0(forward)
  dummy_token_28 = token_28;
  
#pragma ACCEL mem_fence
// Token gen: node 4 and 21 for sync level 0(forward)
  dummy_token_31 = token_31;
  
#pragma ACCEL mem_fence
// Token gen: node 5 and 21 for sync level 0(forward)
  dummy_token_34 = token_34;
  
#pragma ACCEL mem_fence
// Token gen: node 6 and 21 for sync level 0(forward)
  dummy_token_37 = token_37;
  
#pragma ACCEL mem_fence
// Token gen: node 7 and 21 for sync level 0(forward)
  dummy_token_40 = token_40;
  
#pragma ACCEL mem_fence
// Token gen: node 8 and 21 for sync level 0(forward)
  dummy_token_43 = token_43;
  
#pragma ACCEL mem_fence
// Token gen: node 9 and 21 for sync level 0(forward)
  dummy_token_46 = token_46;
  
#pragma ACCEL mem_fence
// Token gen: node 10 and 21 for sync level 0(forward)
  dummy_token_49 = token_49;
  
#pragma ACCEL mem_fence
// Token gen: node 11 and 21 for sync level 0(forward)
  dummy_token_52 = token_52;
  
#pragma ACCEL mem_fence
// Token gen: node 12 and 21 for sync level 0(forward)
  dummy_token_53 = token_53;
  
#pragma ACCEL mem_fence
// Token gen: node 13 and 21 for sync level 0(forward)
  dummy_token_56 = token_56;
  
#pragma ACCEL mem_fence
// Token gen: node 14 and 21 for sync level 0(forward)
  dummy_token_59 = token_59;
  
#pragma ACCEL mem_fence
// Token gen: node 15 and 21 for sync level 0(forward)
  dummy_token_61 = token_61;
  
#pragma ACCEL mem_fence
// Token gen: node 16 and 21 for sync level 0(forward)
  dummy_token_64 = token_64;
  
#pragma ACCEL mem_fence
// Token gen: node 17 and 21 for sync level 0(forward)
  dummy_token_66 = token_66;
  
#pragma ACCEL mem_fence
// Token gen: node 18 and 21 for sync level 0(forward)
  dummy_token_68 = token_68;
  
#pragma ACCEL mem_fence
// Token gen: node 19 and 21 for sync level 0(forward)
  dummy_token_69 = token_69;
  
#pragma ACCEL mem_fence
// Token gen: node 20 and 21 for sync level 0(forward)
  dummy_token_80 = token_80;
  
#pragma ACCEL mem_fence
   *output_last_element = output_loss_buffer_0_arr[0];
//output[weight_size] = output_loss_buffer;
}
