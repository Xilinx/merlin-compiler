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
#include "cmost.h"
#include <ap_int.h>
#include <hls_stream.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static int p_Z11ReinterpretIimERKm_1(const unsigned long &val)
{
  
#pragma HLS INLINE
  return (int &)val;
}

static float p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(const class ap_uint< 32 > &val)
{
  
#pragma HLS INLINE
  return (float &)val;
}

static void stream_in_weights_1(class ap_uint< 512 > *in_weights,class hls::stream< ap_uint< 256 >  > &fifo_weights,class hls::stream< float  > &fifo_bias,class ap_uint< 512 > *in_config,class hls::stream< ap_uint< 512 >  > &out_config,int w_offset,int bias_offset,int config_offset)
{
  int conv_en = 0;
  int in_h = 0;
  int in_c = 0;
  int size = 0;
  int n = 0;
  int burst_length = 0;
  int inc = 0;
  int in_h_13 = 0;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = in_config[i + config_offset];
    if (i == 0) {
      conv_en = p_Z11ReinterpretIimERKm_1((x(31,0)));
      in_h = p_Z11ReinterpretIimERKm_1((x(95,64)));
      in_c = p_Z11ReinterpretIimERKm_1((x(127,96)));
      size = p_Z11ReinterpretIimERKm_1((x(287,256)));
      n = p_Z11ReinterpretIimERKm_1((x(383,352)));
    }
    if (i == 1) {
      burst_length = p_Z11ReinterpretIimERKm_1((x(287,256)));
      inc = p_Z11ReinterpretIimERKm_1((x(351,320)));
      in_h_13 = p_Z11ReinterpretIimERKm_1((x(383,352)));
    }
    out_config . write(x);
  }
  int trans_cnt = 0;
  if (conv_en == 1 || conv_en == 3) {
    trans_cnt = n / 8;
  }
  class ap_uint< 512 > buf_in_weights[2304];
  int index = w_offset;
  for (int k = 0; k < trans_cnt; k++) {
    
#pragma HLS loop_tripcount min=4 max=4
    for (int z = 0; z < in_h_13; z++) {
      
#pragma HLS loop_tripcount min = 32 max =32
      memcpy((void *)buf_in_weights,(const void *)((void *)(&in_weights[index])),64UL * ((unsigned long )burst_length));
      for (int j = 0; j < burst_length; j++) {
        
#pragma HLS pipeline
        
#pragma HLS loop_tripcount min=36 max=36
        class ap_uint< 512 > buf_input = buf_in_weights[j];
        class ap_uint< 256 > tmp;
        tmp = buf_input(((0 + 1) * 32 * 8 - 1),(0 * 32 * 8));
        fifo_weights . write(tmp);
        tmp = buf_input(((1 + 1) * 32 * 8 - 1),(1 * 32 * 8));
        fifo_weights . write(tmp);
      }
    }
    index += inc;
  }
  for (int i = 0; i < n * 4 * 32 / 512; i++) {
    
#pragma HLS pipeline
    
#pragma HLS loop_tripcount min=1024 max=4096
    class ap_uint< 512 > buf_input = in_weights[i + bias_offset + 1024 * 1024 * 13 / 16];
    class ap_uint< 32 > tmp;
    float tmp_o;
    tmp = buf_input(((0 + 1) * 32 - 1),(0 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((1 + 1) * 32 - 1),(1 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((2 + 1) * 32 - 1),(2 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((3 + 1) * 32 - 1),(3 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((4 + 1) * 32 - 1),(4 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((5 + 1) * 32 - 1),(5 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((6 + 1) * 32 - 1),(6 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((7 + 1) * 32 - 1),(7 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((8 + 1) * 32 - 1),(8 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((9 + 1) * 32 - 1),(9 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((10 + 1) * 32 - 1),(10 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((11 + 1) * 32 - 1),(11 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((12 + 1) * 32 - 1),(12 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((13 + 1) * 32 - 1),(13 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((14 + 1) * 32 - 1),(14 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
    tmp = buf_input(((15 + 1) * 32 - 1),(15 * 32));
    tmp_o = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    fifo_bias . write(tmp_o);
  }
}

static void write_fifo_1(class ap_uint< 512 > *in_image,int number,int index,class hls::stream< ap_uint< 128 >  > fifo_image[8])
{
  __merlin_access_range(fifo_image,0,7UL);
  __merlin_access_range(in_image,0,3119UL);
  int M = number / (512 / 8 / 32);
  int N = number % (512 / 8 / 32);
  int Y = index % (512 / 8 / 32);
  class ap_uint< 128 > image_data_left[6240];
  class ap_uint< 512 > buf_input;
  class ap_uint< 256 > tmp;
  if (Y != 0) {
    buf_input = in_image[0];
    for (int j = 0; j < Y; j++) {
      
#pragma HLS dependence variable=image_data_left array inter false
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min=3 max=3
      tmp = buf_input(((j + Y + 2) * 32 * 4 - 1),((j + Y + 1) * 32 * 4));
      for (int p = 0; p < 8; p++) {
        
#pragma HLS unroll
        
#pragma HLS unroll
        fifo_image[p] . write(tmp);
      }
      tmp = buf_input(((j + Y + 2 + 1) * 32 * 4 - 1),((j + Y + 2) * 32 * 4));
      image_data_left[j] = tmp;
    }
  }
  for (int j = 0; j < M; j++) {
    
#pragma HLS dependence variable=image_data_left array inter false
    
#pragma HLS pipeline
    
#pragma HLS loop_tripcount min=104 max=104
    buf_input = in_image[j + (Y + 8 - 1) / 8];
    tmp = buf_input(((0 + 1) * 32 * 4 - 1),(0 * 32 * 4));
    for (int p = 0; p < 8; p++) {
      
#pragma HLS unroll
      
#pragma HLS unroll
      fifo_image[p] . write(tmp);
    }
    tmp = buf_input(((2 + 1) * 32 * 4 - 1),(2 * 32 * 4));
    for (int p = 0; p < 8; p++) {
      
#pragma HLS unroll
      
#pragma HLS unroll
      fifo_image[p] . write(tmp);
    }
    tmp = buf_input(((1 + 1) * 32 * 4 - 1),(1 * 32 * 4));
    image_data_left[2 * j + 0 + (Y + 8 - 1) / 8] = tmp;
    tmp = buf_input(((3 + 1) * 32 * 4 - 1),(3 * 32 * 4));
    image_data_left[2 * j + 1 + (Y + 8 - 1) / 8] = tmp;
  }
  if (Y == 0) {
    buf_input = in_image[M];
    for (int j = 0; j < N; j++) {
      
#pragma HLS dependence variable=image_data_left array inter false
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min=3 max=3
      class ap_uint< 256 > tmp;
      class ap_uint< 32 > tmp_x;
      tmp = buf_input(((2 * j + 1) * 32 * 4 - 1),(2 * j * 32 * 4));
      for (int p = 0; p < 8; p++) {
        
#pragma HLS unroll
        
#pragma HLS unroll
        fifo_image[p] . write(tmp);
      }
      tmp = buf_input(((2 * j + 1 + 1) * 32 * 4 - 1),((2 * j + 1) * 32 * 4));
      image_data_left[2 * M + j] = tmp;
    }
  }
  for (int j = 0; j < 2 * M + N; j++) {
    
#pragma HLS pipeline
    
#pragma HLS loop_tripcount min=104 max=104
    tmp = image_data_left[j];
    for (int p = 0; p < 8; p++) {
      
#pragma HLS unroll
      
#pragma HLS unroll
      fifo_image[p] . write(tmp);
    }
  }
}

static void stream_in_image_1(class ap_uint< 512 > *in_image,class hls::stream< ap_uint< 128 >  > fifo_image[8],class hls::stream< ap_uint< 512 >  > &in_config,class hls::stream< ap_uint< 512 >  > &out_config,int input_idx)
{
  __merlin_access_range(fifo_image,0,7UL);
  int conv_en = 0;
  int in_w = 0;
  int in_h = 0;
  int in_c = 0;
  int size = 0;
  int pad = 0;
  int n = 0;
  int in_wh = 0;
  int burst_length = 0;
  int in_h_13 = 0;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = in_config . read();
    if (i == 0) {
      conv_en = p_Z11ReinterpretIimERKm_1((x(31,0)));
      in_w = p_Z11ReinterpretIimERKm_1((x(63,32)));
      in_h = p_Z11ReinterpretIimERKm_1((x(95,64)));
      in_c = p_Z11ReinterpretIimERKm_1((x(127,96)));
      size = p_Z11ReinterpretIimERKm_1((x(287,256)));
      pad = p_Z11ReinterpretIimERKm_1((x(351,320)));
      n = p_Z11ReinterpretIimERKm_1((x(383,352)));
    }
    if (i == 1) {
      in_wh = p_Z11ReinterpretIimERKm_1((x(31,0)));
      in_h_13 = p_Z11ReinterpretIimERKm_1((x(383,352)));
      burst_length = p_Z11ReinterpretIimERKm_1((x(319,288)));
    }
    out_config . write(x);
  }
  int trans_cnt = 1;
  if (conv_en == 1 || conv_en == 3) {
    trans_cnt = n / 8;
  }
  int flag = 0;
  class ap_uint< 512 > buf_ping_in_image[3120];
  class ap_uint< 512 > buf_pang_in_image[3120];
  memcpy((void *)buf_ping_in_image,(const void *)((void *)(&in_image[input_idx])),64UL * ((unsigned long )burst_length));
  int index = 0;
  for (int t = 0; t < trans_cnt; t++) {
    
#pragma HLS loop_tripcount min=4 max=4
    merlinL9:
    for (int z = 0; z < in_h_13; z++) {
      
#pragma HLS loop_tripcount min = 32 max =32
      merlinL8:
      for (int s = 0; s < in_c / 8; s++) {
        
#pragma HLS loop_tripcount min=1 max=1
        flag += 1;
        int write_length;
        if (z > 0 && z < in_h_13 - 1) {
          write_length = in_w * (13 + 2 * pad);
        }
         else {
          if (in_w == 13) {
            write_length = in_w * 13;
          }
           else {
            write_length = in_w * (13 + pad);
          }
        }
        int index_before = index;
        if (in_w == 13) {
          if (s + 1 == in_c / 8) {
            index = 0;
          }
           else {
            index = (s + 1) * in_wh;
          }
        }
         else {
          if (s + 1 == in_c / 8) {
            if (z + 1 == in_h_13) {
              index = 0;
            }
             else {
              index = ((z + 1) * 13 - pad) * in_w;
            }
          }
           else {
            if (z == 0) {
              index = (s + 1) * in_wh;
            }
             else {
              index = (s + 1) * in_wh + (z * 13 - pad) * in_w;
            }
          }
        }
        if (flag % 2 == 1) {
          if (t + 1 == trans_cnt && z + 1 == in_h_13 && s + 1 == in_c / 8) {
            write_fifo_1(buf_ping_in_image,write_length,index_before,fifo_image);
          }
           else {
            memcpy((void *)buf_pang_in_image,(const void *)((void *)(&in_image[index * 8 * 32 / 512 + input_idx])),64UL * ((unsigned long )burst_length));
            write_fifo_1(buf_ping_in_image,write_length,index_before,fifo_image);
          }
        }
         else {
          if (t + 1 == trans_cnt && z + 1 == in_h_13 && s + 1 == in_c / 8) {
            write_fifo_1(buf_pang_in_image,write_length,index_before,fifo_image);
          }
           else {
            memcpy((void *)buf_ping_in_image,(const void *)((void *)(&in_image[index * 8 * 32 / 512 + input_idx])),64UL * ((unsigned long )burst_length));
            write_fifo_1(buf_pang_in_image,write_length,index_before,fifo_image);
          }
        }
      }
    }
  }
}

static void conv_1x1_weights_read_1(class hls::stream< ap_uint< 256 >  > &weights_in,float weights_out[8][1024],int c_out)
{
  __merlin_access_range(weights_out,0,7UL,0,1023UL);
  int i;
  int p;
  for (i = 0; i < 8 * c_out / 8; i++) {
    
#pragma HLS dependence variable=weights_out array inter false
    
#pragma HLS pipeline
    
#pragma HLS loop_tripcount min=1024 max=1024
    class ap_uint< 256 > w_buf;
    w_buf = weights_in . read();
    class ap_uint< 32 > tmp[8];
    for (p = 0; p < 8; p++) {
      
#pragma HLS unroll
      
#pragma HLS loop_tripcount min=8 max=8
      class ap_uint< 32 > tmp;
      tmp = w_buf(((p + 1) * 32 - 1),(p * 32));
      weights_out[p][i] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
    }
  }
}

void mars_kernel_0_50_node_0_stage_0(int k,int exec,float A_PART_0[4],float weights_in[1024])
{
  
#pragma HLS INLINE OFF
  if (exec == 1) {
    for (int p = 0; p < 4; p++) {
      
#pragma HLS unroll
      
#pragma HLS loop_tripcount min=8 max=8
      A_PART_0[p] = weights_in[p + k * 4];
    }
  }
}

void mars_kernel_0_50_node_1_stage_1(int k,int exec,float A_PART_0[4],class hls::stream< ap_uint< 128 >  > *data_in,float *output,int size)
{
  
#pragma HLS INLINE OFF
  if (exec == 1) {
    for (int j = 0; j < size; ++j) {
      
#pragma HLS dependence variable=output array inter false
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min=169 max=169
      class ap_uint< 128 > d_buf;
      d_buf = ( *data_in) . read();
      class ap_uint< 32 > d_tmp;
      float d[4];
      
#pragma HLS array_partition variable=d complete dim=1
      for (int p = 0; p < 4; p++) {
        
#pragma HLS unroll
        
#pragma HLS unroll
        
#pragma HLS loop_tripcount min=8 max=8
        d_tmp = d_buf(((p + 1) * 32 - 1),(p * 32));
        d[p] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(d_tmp);
      }
      float tmp[4];
      
#pragma HLS array_partition variable=tmp complete dim=1
      for (int p = 0; p < 4; p++) {
        
#pragma HLS unroll
        
#pragma HLS unroll
        tmp[p] = A_PART_0[p] * d[p];
      }
      float sum = (float )0.0;
      for (int p = 0; p < 4; p++) {
        
#pragma HLS unroll
        
#pragma HLS unroll
        sum += tmp[p];
      }
      output[j] += sum;
    }
  }
}

void mars_kernel_0_50(int mars_k,int mars_init,int mars_cond,float mars_A_PART_0_0[4],float mars_A_PART_0_1[4],class hls::stream< ap_uint< 128 >  > *mars_data_in_1,float *mars_output_1,int size,float mars_weights_in_0[1024])
{
  
#pragma HLS INLINE OFF
  mars_kernel_0_50_node_0_stage_0(mars_k - 0,(int )((mars_k >= mars_init + 0) & (mars_k <= mars_cond + 0)),mars_A_PART_0_0,mars_weights_in_0);
  mars_kernel_0_50_node_1_stage_1(mars_k - 1,(int )((mars_k >= mars_init + 1) & (mars_k <= mars_cond + 1)),mars_A_PART_0_1,mars_data_in_1,mars_output_1,size);
}

static void conv_1x1_core_1(class hls::stream< ap_uint< 128 >  > &data_in,float weights_in[1024],float *output,int in_c,int size)
{
  __merlin_access_range(output,0,337UL);
  __merlin_access_range(weights_in,0,1023UL);
  
#pragma HLS inline off
  float A_PART_0[4];
  int mars_count_0_50 = 0;
  float mars_kernel_0_50_A_PART_0_0[4];
  
#pragma HLS array_partition variable=mars_kernel_0_50_A_PART_0_0 complete dim=1
  float mars_kernel_0_50_A_PART_0_1[4];
  
#pragma HLS array_partition variable=mars_kernel_0_50_A_PART_0_1 complete dim=1
  for (int k = 0; k < in_c + 1; ++k) {
    
#pragma HLS loop_tripcount min=256 max=256
    if (mars_count_0_50 == 0) 
      mars_kernel_0_50(k,0,- 1 + in_c,mars_kernel_0_50_A_PART_0_0,mars_kernel_0_50_A_PART_0_1,&data_in,output,size,weights_in);
     else 
      mars_kernel_0_50(k,0,- 1 + in_c,mars_kernel_0_50_A_PART_0_1,mars_kernel_0_50_A_PART_0_0,&data_in,output,size,weights_in);
    mars_count_0_50++;
    if (mars_count_0_50 == 2) 
      mars_count_0_50 = 0;
  }
}

void conv_1x1_stream_1_L_0_0_para_sub(class hls::stream< ap_uint< 128 >  > *data_in,int p_c,int p_out_w,float local_buffer[338],float weights_buf[1024],int i_sub)
{
  
#pragma HLS function_instantiate variable=i_sub
  
#pragma HLS inline off
  
#pragma HLS loop_tripcount min=8 max=8
  for (int l = 0; l < p_out_w * 13; l++) {
    
#pragma HLS dependence variable=local_buffer array inter false
    
#pragma HLS pipeline
    
#pragma HLS loop_tripcount min=169 max=169
    local_buffer[l] = ((float )0.0);
  }
  conv_1x1_core_1(data_in[0],weights_buf,local_buffer,p_c / 4,p_out_w * 13);
}

void conv_1x1_stream_1_L_0_0_para(class hls::stream< ap_uint< 128 >  > data_in[8],int p_c,int p_out_w,float local_buffer[8][338],float weights_buf[8][1024])
{
  
#pragma HLS inline off
  int i_sub;
  for (i_sub = 0; i_sub < 8; ++i_sub) {
    
#pragma HLS unroll complete
    conv_1x1_stream_1_L_0_0_para_sub(&data_in[i_sub],p_c,p_out_w,local_buffer[i_sub],weights_buf[i_sub],i_sub);
  }
}

static void conv_1x1_stream_1(class hls::stream< ap_uint< 128 >  > data_in[8],class hls::stream< ap_uint< 256 >  > &weights_in,class hls::stream< float  > data_out[8],int config[14])
{
  __merlin_access_range(config,0,13UL);
  __merlin_access_range(data_out,0,7UL);
  __merlin_access_range(data_in,0,7UL);
  int p_c = config[2];
  int p_out_w = config[4];
  int p_out_h = config[5];
  int p_size = config[7];
  int p_n = config[10];
  int p_groups = config[12];
  int M = p_n / p_groups;
  int K = p_size * p_size * p_c / p_groups;
  float local_buffer[8][338];
  
#pragma HLS array_partition variable=local_buffer complete dim=1
  float weights_buf[8][1024];
  
#pragma HLS array_partition variable=weights_buf complete dim=1
  M = M * p_out_h / 13;
  for (int i = 0; i < M / 8; ++i) {
    
#pragma HLS loop_tripcount min=64 max=64
    conv_1x1_weights_read_1(weights_in,weights_buf,K);
    conv_1x1_stream_1_L_0_0_para(data_in,p_c,p_out_w,local_buffer,weights_buf);
    for (int i_sub = 0; i_sub < 8; i_sub++) {
    }
    for (int k = 0; k < p_out_w * 13; k++) {
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min = 169 max =169
      for (int p = 0; p < 8; p++) {
        
#pragma HLS unroll
        data_out[p] << local_buffer[p][k];
      }
    }
  }
}

static void burst_in_weights_1(class hls::stream< ap_uint< 256 >  > &stream_weights,float weights_in_n[8][4][3][3])
{
  __merlin_access_range(weights_in_n,0,7UL,0,35UL);
  for (int i = 0; i < 4; i++) {
    
#pragma HLS unroll
    for (int j = 0; j < 3; j++) {
      
#pragma HLS unroll
      for (int k = 0; k < 3; k++) {
        
#pragma HLS unroll
        class ap_uint< 256 > input_buf = stream_weights . read();
        for (int p = 0; p < 8; p++) {
          
#pragma HLS unroll
          class ap_uint< 32 > tmp = (input_buf(((p + 1) * 32 - 1),(p * 32)));
          weights_in_n[p][i][j][k] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
        }
      }
    }
  }
}

static void memory_burst_in_1(float data_in_tmp[4][3][418],class hls::stream< ap_uint< 128 >  > &data_in_st,int z,int in_w)
{
  __merlin_access_range(data_in_tmp,0,3UL,0,2UL,0,417UL);
  merlinL21:
  for (int i = 0; i < 3 - 1; i++) {
    merlinL20:
    for (int j = 0; j < in_w + 2 * 1; j++) {
      
#pragma HLS dependence variable=data_in_tmp array inter false
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min = 418 max =418
      int in_h_pad = z * 13 + i - 1;
      int in_w_pad = j - 1;
      bool pad_en = in_w_pad < 0 || in_w_pad >= in_w || in_h_pad < 0;
      class ap_uint< 128 > input_buf;
      if (pad_en) {
        input_buf = 0;
      }
       else {
        input_buf = data_in_st . read();
      }
      class ap_uint< 32 > tmp_value;
      for (int l = 0; l < 4; l++) {
        
#pragma HLS unroll
        tmp_value = input_buf(((l + 1) * 32 - 1),(l * 32));
        data_in_tmp[l][i + 1][j] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp_value);
      }
    }
  }
}

static void shift_in_data_1(class hls::stream< ap_uint< 128 >  > &data_in_st,class hls::stream< ap_uint< 128 >  > &stream_data_in,int z,int in_w,int in_h)
{
  STREAM1:
  for (int i = 0; i < 13 - 3 + 1 + 2 * 1; i++) {
    
#pragma HLS loop_tripcount min = 416 max = 416
    STREAM2:
    for (int j = 0; j < in_w + 2 * 1; j++) {
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min = 418 max = 418
      int in_w_pad = j - 1;
      int in_h_pad = z * 13 + i - 1;
      class ap_uint< 128 > buf;
      if (in_w_pad < 0 || in_w_pad >= in_w || in_h_pad >= in_h - 3 + 1) {
        buf = 0;
      }
       else {
        buf = data_in_st . read();
      }
      stream_data_in . write(buf);
    }
  }
}

static void compute_one_plan_sub_1(float input[4][3][418],class hls::stream< ap_uint< 128 >  > &stream_data_in,float filter[4][3][3],class hls::stream< float  > &stream_data_out,int in_w)
{
  __merlin_access_range(filter,0,35UL);
  __merlin_access_range(input,0,3UL,0,2UL,0,417UL);
  int i;
  int w;
  int h;
  float shift[4][3][3];
  
#pragma HLS array_partition variable=shift complete dim=3
  
#pragma HLS array_partition variable=shift complete dim=2
  
#pragma HLS array_partition variable=shift complete dim=1
  for (int t = 0; t < 13 - 3 + 1 + 2 * 1; t++) {
    
#pragma HLS loop_tripcount min = 416 max = 416
    for (i = 0; i < in_w + 2 * 1; i++) {
      
#pragma HLS dependence variable=input array inter false
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min = 418 max = 418
      class ap_uint< 128 > input_buf = stream_data_in . read();
      for (int l = 0; l < 4; l++) {
        
#pragma HLS unroll
        for (h = 0; h < 3 - 1; h++) {
          
#pragma HLS unroll
          input[l][h][i] = input[l][h + 1][i];
        }
        class ap_uint< 32 > tmp_buf = (input_buf(((l + 1) * 32 - 1),(l * 32)));
        input[l][3 - 1][i] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp_buf);
        for (w = 0; w < 3 - 1; w++) {
          
#pragma HLS unroll
          int k = 3 - 1 - w;
          for (h = 0; h < 3; h++) {
            
#pragma HLS unroll
            shift[l][k][h] = shift[l][k - 1][h];
          }
        }
        for (h = 0; h < 3; h++) {
          
#pragma HLS unroll
          shift[l][0][h] = input[l][h][i];
        }
      }
      if (i >= 3 - 1) {
        float tmp3[4];
        
#pragma HLS array_partition variable=tmp3 complete dim=1
        for (int l = 0; l < 4; l++) {
          
#pragma HLS unroll
          float result_tmp[9];
          
#pragma HLS array_partition variable=result_tmp complete dim=1
          for (w = 0; w < 3; w++) {
            
#pragma HLS unroll
            for (h = 0; h < 3; h++) {
              
#pragma HLS unroll
              result_tmp[w * 3 + h] = shift[l][3 - w - 1][h] * filter[l][h][w];
            }
          }
          float tmp00 = result_tmp[0] + result_tmp[1];
          float tmp01 = result_tmp[2] + result_tmp[3];
          float tmp02 = result_tmp[4] + result_tmp[5];
          float tmp03 = result_tmp[6] + result_tmp[7];
          float tmp10 = tmp00 + tmp01;
          float tmp11 = tmp02 + tmp03;
          float tmp2 = tmp10 + tmp11;
          tmp3[l] = tmp2 + result_tmp[8];
        }
        float tmp_sum00 = tmp3[0] + tmp3[1];
        float tmp_sum01 = tmp3[2] + tmp3[3];
        float tmp_sum = tmp_sum00 + tmp_sum01;
        stream_data_out . write(tmp_sum);
      }
    }
  }
}

static void adder_out_1(float *conv_sum,class hls::stream< float  > &stream_data_in,class hls::stream< float  > &stream_data_out,int s,int in_w,int in_c)
{
  __merlin_access_range(conv_sum,0,5407UL);
  for (int i = 0; i < (13 - 3 + 1 + 2 * 1) * (in_w - 3 + 1 + 2 * 1); i++) {
    
#pragma HLS dependence variable=conv_sum array inter false
    
#pragma HLS pipeline
    
#pragma HLS loop_tripcount min = 5408 max = 5408
    float tmp1 = stream_data_in . read();
    if (s == 0) {
      conv_sum[i] = tmp1;
    }
     else {
      conv_sum[i] += tmp1;
    }
    if (s == in_c / 4 - 1) {
      stream_data_out . write(conv_sum[i]);
    }
  }
}

static void compute_one_plan_1(float data_in_tmp[4][3][418],class hls::stream< ap_uint< 128 >  > &data_in_st,float filter[4][3][3],float *conv_sum,class hls::stream< float  > &conv_out_st,int z,int s,int in_w,int in_h,int in_c)
{
  __merlin_access_range(conv_sum,0,5407UL);
  __merlin_access_range(filter,0,35UL);
  __merlin_access_range(data_in_tmp,0,3UL,0,2UL,0,417UL);
  
#pragma HLS inline off
  
#pragma HLS dataflow
  class hls::stream< ap_uint< 128 >  > stream_data_in("streaming_data_in");
  
#pragma HLS stream variable = stream_data_in depth = 32
  class hls::stream< float  > stream_data_out("streaming_data_out");
  
#pragma HLS stream variable = stream_data_out depth = 32
  shift_in_data_1(data_in_st,stream_data_in,z,in_w,in_h);
  compute_one_plan_sub_1(data_in_tmp,stream_data_in,filter,stream_data_out,in_w);
  adder_out_1(conv_sum,stream_data_out,conv_out_st,s,in_w,in_c);
}

static void conv_core_1(class hls::stream< ap_uint< 128 >  > &data_in_st,float filter[4][3][3],float *conv_sum,class hls::stream< float  > &conv_out_st,int z,int s,int in_w,int in_h,int in_c)
{
  __merlin_access_range(conv_sum,0,5407UL);
  __merlin_access_range(filter,0,35UL);
  
#pragma HLS inline off
  float data_in_tmp[4][3][418];
  
#pragma HLS array_partition variable=data_in_tmp complete dim=2
  
#pragma HLS array_partition variable=data_in_tmp complete dim=1
  memory_burst_in_1(data_in_tmp,data_in_st,z,in_w);
  compute_one_plan_1(data_in_tmp,data_in_st,filter,conv_sum,conv_out_st,z,s,in_w,in_h,in_c);
}

void compute_cube_1_L_0_0_para_sub(class hls::stream< ap_uint< 128 >  > *stream_input,class hls::stream< float  > *conv_out_st,int z,int in_w,int in_h,int in_c,int s,float weights_in_n[4][3][3],float conv_sum[5408],int p)
{
  
#pragma HLS function_instantiate variable=p
  
#pragma HLS inline off
  conv_core_1(stream_input[0],weights_in_n,conv_sum,conv_out_st[0],z,s,in_w,in_h,in_c);
}

void compute_cube_1_L_0_0_para(class hls::stream< ap_uint< 128 >  > stream_input[8],class hls::stream< float  > conv_out_st[8],int z,int in_w,int in_h,int in_c,int s,float weights_in_n[8][4][3][3],float conv_sum[8][5408])
{
  
#pragma HLS inline off
  int p;
  for (p = 0; p < 8; ++p) {
    
#pragma HLS unroll complete
    compute_cube_1_L_0_0_para_sub(&stream_input[p],&conv_out_st[p],z,in_w,in_h,in_c,s,weights_in_n[p],conv_sum[p],p);
  }
}

static void compute_cube_1(class hls::stream< ap_uint< 128 >  > stream_input[8],class hls::stream< ap_uint< 256 >  > &stream_weights,class hls::stream< float  > conv_out_st[8],int z,int in_w,int in_h,int in_c)
{
  __merlin_access_range(conv_out_st,0,7UL);
  __merlin_access_range(stream_input,0,7UL);
  
#pragma HLS inline off
  for (int s = 0; s < in_c / 4; s++) {
    
#pragma HLS loop_tripcount min = 1 max =1
    float weights_in_n[8][4][3][3];
    
#pragma HLS array_partition variable=weights_in_n complete dim=4
    
#pragma HLS array_partition variable=weights_in_n complete dim=3
    
#pragma HLS array_partition variable=weights_in_n complete dim=2
    
#pragma HLS array_partition variable=weights_in_n complete dim=1
    burst_in_weights_1(stream_weights,weights_in_n);
    float conv_sum[8][5408];
    
#pragma HLS array_partition variable=conv_sum complete dim=1
    compute_cube_1_L_0_0_para(stream_input,conv_out_st,z,in_w,in_h,in_c,s,weights_in_n,conv_sum);
    for (int p = 0; p < 8; p++) {
    }
  }
}

static void conv_3x3_stream_1(class hls::stream< ap_uint< 128 >  > stream_input[8],class hls::stream< ap_uint< 256 >  > &stream_weights,class hls::stream< float  > stream_output[8],int config[14])
{
  __merlin_access_range(config,0,13UL);
  __merlin_access_range(stream_output,0,7UL);
  __merlin_access_range(stream_input,0,7UL);
  int in_w = config[0];
  int in_h = config[1];
  int in_c = config[2];
  int n = config[10];
  int in_h_13 = config[13];
  cal_core:
  for (int t = 0; t < n / 8; t++) {
    
#pragma HLS loop_tripcount min = 4 max = 4
    for (int z = 0; z < in_h_13; z++) {
      
#pragma HLS loop_tripcount min = 32 max = 32
      compute_cube_1(stream_input,stream_weights,stream_output,z,in_w,in_h,in_c);
    }
  }
}

static void conv_switch_1(class hls::stream< ap_uint< 128 >  > data_in[8],class hls::stream< ap_uint< 256 >  > &weights_in,class hls::stream< ap_uint< 512 >  > &config_in,class hls::stream< float  > data_out[8],class hls::stream< ap_uint< 512 >  > &config_out,const int config_list[32])
{
// should not inline because it is called by dataflow region
  __merlin_access_range(config_list,0,31UL);
  __merlin_access_range(data_out,0,7UL);
  __merlin_access_range(data_in,0,7UL);
  int config[14];
  
#pragma HLS array_partition variable=config complete dim=1
  int conv_en = 0;
  class ap_uint< 512 > tmp1;
  class ap_uint< 512 > tmp2;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = config_in . read();
    if (i == 0) {
      tmp1 = x;
      conv_en = p_Z11ReinterpretIimERKm_1((x(31,0)));
    }
    if (i == 1) {
      tmp2 = x;
    }
    config_out . write(x);
  }
  for (int i = 1; i < 15; i++) {
    
#pragma HLS unroll
    config[i - 1] = p_Z11ReinterpretIimERKm_1((tmp1(((i + 1) * 32 - 1),(i * 32))));
  }
  config[13] = p_Z11ReinterpretIimERKm_1((tmp2(383,352)));
  for (int i = 1; i < 15; i++) {
    
#pragma HLS unroll
    config[i - 1] = config_list[i];
  }
  config[13] = config_list[27];
  conv_en = config_list[0];
  if (conv_en == 1) {
    conv_1x1_stream_1(data_in,weights_in,data_out,config);
  }
   else {
    if (conv_en == 3) {
      conv_3x3_stream_1(data_in,weights_in,data_out,config);
    }
  }
}

static void bias_stream_1(class hls::stream< float  > stream_input[8],class hls::stream< float  > &fifo_bias,class hls::stream< ap_uint< 256 >  > &stream_output,int conv_out_en,class hls::stream< ap_uint< 256 >  > &data_out_st,int config[14])
{
  __merlin_access_range(config,0,13UL);
  __merlin_access_range(stream_input,0,7UL);
  int in_wh = config[2];
  int n = config[10];
  int batch_normalize = config[11];
  int activation = config[13];
  float biases[8];
  
#pragma HLS array_partition variable=biases complete dim=1
  float scales[8];
  
#pragma HLS array_partition variable=scales complete dim=1
  float rolling_mean[8];
  
#pragma HLS array_partition variable=rolling_mean complete dim=1
  float rolling_variance[8];
  
#pragma HLS array_partition variable=rolling_variance complete dim=1
  for (int t = 0; t < n / 8; t++) {
    
#pragma HLS loop_tripcount min = 4 max = 4
    for (int p = 0; p < 8; p++) {
      
#pragma HLS dependence variable=rolling_variance array inter false
      
#pragma HLS dependence variable=rolling_mean array inter false
      
#pragma HLS dependence variable=scales array inter false
      
#pragma HLS dependence variable=biases array inter false
      
#pragma HLS pipeline
      biases[p] = fifo_bias . read();
      scales[p] = fifo_bias . read();
      rolling_mean[p] = fifo_bias . read();
      rolling_variance[p] = fifo_bias . read();
    }
    for (int i = 0; i < in_wh; i++) {
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min = 173056 max =173056
      class ap_uint< 32 > tmp_a[8];
      
#pragma HLS array_partition variable=tmp_a complete dim=1
      for (int p = 0; p < 8; p++) {
        
#pragma HLS unroll
        float tmp_o1 = stream_input[p] . read();
        float tmp_o2;
        if (batch_normalize == 1) {
          float tmp_o20 = tmp_o1 - rolling_mean[p];
          float tmp_o21 = sqrtf(rolling_variance[p]) + .000001f;
          float tmp_o22 = tmp_o20 / tmp_o21;
          tmp_o2 = tmp_o22 * scales[p];
        }
         else {
          tmp_o2 = tmp_o1;
        }
        float tmp_o3 = tmp_o2 + biases[p];
        float tmp_o4;
        if (activation == 0) {
          float x = tmp_o3;
          if (x > ((float )0)) {
            tmp_o4 = x;
          }
           else {
            tmp_o4 = ((float ).1) * x;
          }
        }
         else {
          tmp_o4 = tmp_o3;
        }
        tmp_a[p] =  *((class ap_uint< 32 > *)(&tmp_o4));
      }
      class ap_uint< 256 > output_buf = ((((((((tmp_a[7] , tmp_a[6] , tmp_a[5])) , tmp_a[4] , tmp_a[3])) , tmp_a[2] , tmp_a[1])) , tmp_a[0]));
      class ap_uint< 256 > output_buf1 = output_buf;
      class ap_uint< 256 > output_buf2 = output_buf;
      stream_output . write(output_buf1);
      if (conv_out_en == 1) {
        data_out_st . write(output_buf2);
      }
    }
  }
}

static void bias_switch_1(class hls::stream< float  > stream_input[8],class hls::stream< float  > &fifo_bias,class hls::stream< ap_uint< 256 >  > &stream_output,class hls::stream< ap_uint< 256 >  > &data_out_st,class hls::stream< ap_uint< 512 >  > &config_in,class hls::stream< ap_uint< 512 >  > &config_out)
{
  __merlin_access_range(stream_input,0,7UL);
  int config[14];
  
#pragma HLS array_partition variable=config complete dim=1
  int conv_en = 0;
  int conv_out_en = 0;
  class ap_uint< 512 > tmp1;
  class ap_uint< 512 > tmp2;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = config_in . read();
    if (i == 0) {
      tmp1 = x;
      conv_en = p_Z11ReinterpretIimERKm_1((x(31,0)));
      conv_out_en = p_Z11ReinterpretIimERKm_1((x(511,480)));
    }
    if (i == 1) {
      tmp2 = x;
    }
    config_out . write(x);
  }
  for (int i = 1; i < 15; i++) {
    
#pragma HLS unroll
    config[i - 1] = p_Z11ReinterpretIimERKm_1((tmp1(((i + 1) * 32 - 1),(i * 32))));
  }
  config[2] = p_Z11ReinterpretIimERKm_1((tmp2(31,0)));
  if (conv_en == 1 || conv_en == 3) {
    bias_stream_1(stream_input,fifo_bias,stream_output,conv_out_en,data_out_st,config);
  }
}

static void maxpool_merlin_data_flow_1(class hls::stream< ap_uint< 256 >  > &stream_input,class hls::stream< ap_uint< 256 >  > &stream_output,int config[14])
{
  __merlin_access_range(config,0,13UL);
  int i;
  int j;
  int k;
  int m;
  int n;
  int s;
  int p;
  int in_w = config[0];
  int w = config[4];
  int h = config[5];
  int c = config[6];
  int size = config[7];
  int stride = config[8];
  int pad = config[9];
  int w_offset = -pad / 2;
  int h_offset = -pad / 2;
  int h_1 = h - 1;
  class ap_uint< 256 > input_buf;
  class ap_uint< 256 > output_buf;
  size = 2;
  float line_buffer[8][2][416];
  
#pragma HLS array_partition variable=line_buffer complete dim=2
  
#pragma HLS array_partition variable=line_buffer complete dim=1
  for (j = 0; j < 416; j++) {
    
#pragma HLS dependence variable=line_buffer array inter false
    
#pragma HLS pipeline
    for (s = 0; s < 2; s++) {
      
#pragma HLS unroll
      for (p = 0; p < 8; p++) {
        
#pragma HLS unroll
        line_buffer[p][s][j] = - 3.40282346638528859812e+38F;
      }
    }
  }
  for (k = 0; k < c / 8; ++k) {
    
#pragma HLS loop_tripcount min = 2 max = 2
    if (stride == 1) {
      for (j = 0; j < in_w; j++) {
        
#pragma HLS dependence variable=line_buffer array inter false
        
#pragma HLS pipeline
        
#pragma HLS loop_tripcount min = 416 max = 416
        input_buf = stream_input . read();
        for (p = 0; p < 8; p++) {
          
#pragma HLS unroll
          class ap_uint< 32 > tmp = (input_buf(((p + 1) * 32 - 1),(p * 32)));
          line_buffer[p][0][j] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
        }
      }
    }
    for (i = 0; i < h; ++i) {
      
#pragma HLS loop_tripcount min = 208 max = 208
      for (j = 0; j < in_w; j++) {
        
#pragma HLS dependence variable=line_buffer array inter false
        
#pragma HLS pipeline
        
#pragma HLS loop_tripcount min = 416 max = 416
        if (stride == 1) {
          for (p = 0; p < 8; p++) {
            
#pragma HLS unroll
            line_buffer[p][1][j] = line_buffer[p][0][j];
          }
        }
         else {
          if (stride == 2) {
            input_buf = stream_input . read();
            for (p = 0; p < 8; p++) {
              
#pragma HLS unroll
              class ap_uint< 32 > tmp = (input_buf(((p + 1) * 32 - 1),(p * 32)));
              line_buffer[p][1][j] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
            }
          }
        }
      }
      if (stride == 1 && i < h_1) {
        input_buf = stream_input . read();
        for (p = 0; p < 8; p++) {
          
#pragma HLS unroll
          class ap_uint< 32 > tmp = (input_buf(((p + 1) * 32 - 1),(p * 32)));
          line_buffer[p][0][0] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
        }
      }
      for (j = 0; j < w; ++j) {
        
#pragma HLS pipeline
        
#pragma HLS loop_tripcount min = 208 max = 208
        float max[8];
        
#pragma HLS array_partition variable=max complete dim=1
        class ap_uint< 32 > tmp_a[8];
        
#pragma HLS array_partition variable=tmp_a complete dim=1
        if (stride == 1 && i < h_1) {
          if (j < w - 1) {
            input_buf = stream_input . read();
            for (p = 0; p < 8; p++) {
              
#pragma HLS unroll
              class ap_uint< 32 > tmp = (input_buf(((p + 1) * 32 - 1),(p * 32)));
              line_buffer[p][0][j + 1] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
            }
          }
        }
         else {
          if (stride == 2) {
            input_buf = stream_input . read();
            for (p = 0; p < 8; p++) {
              
#pragma HLS unroll
              class ap_uint< 32 > tmp = (input_buf(((p + 1) * 32 - 1),(p * 32)));
              line_buffer[p][0][j * stride] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
            }
            input_buf = stream_input . read();
            for (p = 0; p < 8; p++) {
              
#pragma HLS unroll
              class ap_uint< 32 > tmp = (input_buf(((p + 1) * 32 - 1),(p * 32)));
              line_buffer[p][0][j * stride + 1] = p_Z11ReinterpretIf7ap_uintILi32EEERK7ap_uintILi32EE_1(tmp);
            }
          }
        }
        for (p = 0; p < 8; p++) {
          
#pragma HLS unroll
          max[p] = - 3.40282346638528859812e+38F;
          for (n = 0; n < size; ++n) {
            
#pragma HLS unroll
            
#pragma HLS loop_tripcount min = 2 max = 2
            for (m = 0; m < size; ++m) {
              
#pragma HLS unroll
              
#pragma HLS loop_tripcount min = 2 max = 2
              int cur_h = h_offset + n;
              int cur_w = w_offset + j * stride + m;
              int index1 = cur_h;
              int index2 = cur_w;
              int valid = (int )(cur_h >= 0 && cur_h < size && cur_w >= 0 && cur_w < in_w);
              float rose_temp;
              if (valid != 0) {
                rose_temp = line_buffer[p][index1][index2];
              }
               else {
                rose_temp = - 3.40282346638528859812e+38F;
              }
              float val = rose_temp;
              if (val > max[p]) {
                max[p] = val;
              }
               else {
                max[p] = max[p];
              }
              tmp_a[p] =  *((class ap_uint< 32 > *)(&max[p]));
            }
          }
        }
        output_buf = (((((((tmp_a[7] , tmp_a[6] , tmp_a[5])) , tmp_a[4] , tmp_a[3])) , tmp_a[2] , tmp_a[1])) , tmp_a[0]);
        stream_output . write(output_buf);
      }
    }
  }
}

static void maxpool_switch_1(class hls::stream< ap_uint< 256 >  > &data_input,class hls::stream< ap_uint< 256 >  > &data_output,class hls::stream< ap_uint< 512 >  > &config_in,class hls::stream< ap_uint< 512 >  > &config_out)
{
  int config[14];
  
#pragma HLS array_partition variable=config complete dim=1
  int maxpool_sel = 0;
  class ap_uint< 512 > tmp1;
  class ap_uint< 512 > tmp2;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = config_in . read();
    if (i == 2) {
      tmp1 = x;
      maxpool_sel = p_Z11ReinterpretIimERKm_1((x(31,0)));
    }
    if (i == 3) {
      tmp2 = x;
    }
    config_out . write(x);
  }
  for (int i = 1; i < 15; i++) {
    
#pragma HLS unroll
    config[i - 1] = p_Z11ReinterpretIimERKm_1((tmp1(((i + 1) * 32 - 1),(i * 32))));
  }
  if (maxpool_sel == 1) {
    maxpool_merlin_data_flow_1(data_input,data_output,config);
  }
   else {
    int data_size = p_Z11ReinterpretIimERKm_1((tmp2(127,96)));
    for (int j = 0; j < data_size / 8; j++) {
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min=21632 max=21632
      class ap_uint< 256 > tmp;
      tmp = data_input . read();
      data_output << tmp;
    }
  }
}

static void upsample_stream_1(class hls::stream< ap_uint< 256 >  > &input_data,int config[32],class hls::stream< ap_uint< 256 >  > &output_data)
{
  __merlin_access_range(config,0,31UL);
  int w = config[1];
  int h = config[2];
  int ch = config[3];
  int w_h_out = config[18];
  class ap_uint< 256 > tmp;
  class ap_uint< 32 > tmp_d[8];
  
#pragma HLS array_partition variable=tmp_d complete dim=1
  class ap_uint< 32 > tmp_line[8][676];
  
#pragma HLS array_partition variable=tmp_line complete dim=1
  for (int k = 0; k < ch / 8; ++k) {
    
#pragma HLS loop_tripcount min=16 max=16
    int index1 = 0;
    for (int j = 0; j < h; ++j) {
      
#pragma HLS loop_tripcount min=13 max=13
      
#pragma HLS loop_flatten off
      for (int i = 0; i < w; ++i) {
        
#pragma HLS pipeline
        
#pragma HLS loop_tripcount min=13 max=13
        tmp = input_data . read();
        for (int p = 0; p < 8; p++) {
          
#pragma HLS unroll
          
#pragma HLS loop_tripcount min=8 max=8
          tmp_d[p] = tmp(((p + 1) * 32 - 1),(p * 32));
        }
        for (int p = 0; p < 8; p++) {
          
#pragma HLS unroll
          
#pragma HLS loop_tripcount min=8 max=8
          tmp_line[p][index1 + i * 2] = tmp_d[p];
          tmp_line[p][index1 + i * 2 + 1] = tmp_d[p];
          tmp_line[p][index1 + w * 2 + i * 2] = tmp_d[p];
          tmp_line[p][index1 + w * 2 + i * 2 + 1] = tmp_d[p];
        }
      }
      index1 += w * 4;
    }
    for (int i = 0; i < w_h_out; i++) {
      
#pragma HLS dependence variable=tmp_line array inter false
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min=676 max=676
      class ap_uint< 256 > out_buf;
      out_buf = (((((((tmp_line[7][i] , tmp_line[6][i] , tmp_line[5][i])) , tmp_line[4][i] , tmp_line[3][i])) , tmp_line[2][i] , tmp_line[1][i])) , tmp_line[0][i]);
      output_data << out_buf;
    }
  }
}

static void upsample_switch_1(class hls::stream< ap_uint< 256 >  > &data_input,class hls::stream< ap_uint< 256 >  > &data_output,class hls::stream< ap_uint< 512 >  > &config_in,class hls::stream< ap_uint< 512 >  > &config_out)
{
  int config[32];
  
#pragma HLS array_partition variable=config cyclic factor=8 dim=1
  int upsample_sel = 0;
  class ap_uint< 512 > tmp1;
  class ap_uint< 512 > tmp2;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = config_in . read();
    if (i == 4) {
      tmp1 = x;
      upsample_sel = p_Z11ReinterpretIimERKm_1((x(31,0)));
    }
    if (i == 5) {
      tmp2 = x;
    }
    config_out . write(x);
  }
  for (int i = 0; i < 15; i++) {
    
#pragma HLS unroll
    config[i] = p_Z11ReinterpretIimERKm_1((tmp1(((i + 1) * 32 - 1),(i * 32))));
  }
  for (int i = 0; i < 15; i++) {
    
#pragma HLS unroll
    config[i + 16] = p_Z11ReinterpretIimERKm_1((tmp2(((i + 1) * 32 - 1),(i * 32))));
  }
  if (upsample_sel == 1) {
    upsample_stream_1(data_input,config,data_output);
  }
   else {
    int data_size = p_Z11ReinterpretIimERKm_1((tmp2(127,96)));
    for (int j = 0; j < data_size / 8; j++) {
      
#pragma HLS pipeline
      
#pragma HLS loop_tripcount min=21632 max=21632
      class ap_uint< 256 > tmp;
      tmp = data_input . read();
      data_output << tmp;
    }
  }
}

static void read_fifo_1(class hls::stream< ap_uint< 256 >  > &data_in,class ap_uint< 512 > *data_out)
{
  __merlin_access_range(data_out,0,337UL);
  for (int k = 0; k < 338; k++) {
    
#pragma HLS dependence variable=data_out array inter false
    
#pragma HLS pipeline
    class ap_uint< 256 > tmp_buf[2];
    
#pragma HLS array_partition variable=tmp_buf complete dim=1
    tmp_buf[0] = data_in . read();
    tmp_buf[1] = data_in . read();
    data_out[k] = (tmp_buf[1] , tmp_buf[0]);
  }
}

static void stream_out_image_1(class hls::stream< ap_uint< 256 >  > &data_in,class ap_uint< 512 > *data_out,class hls::stream< ap_uint< 512 >  > &config_in,class hls::stream< ap_uint< 512 >  > &config_out,int output_idx)
{
  int config[14];
  
#pragma HLS array_partition variable=config complete dim=1
  int loop_bound = 0;
  class ap_uint< 512 > tmp1;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = config_in . read();
    if (i == 4) {
      tmp1 = x;
    }
    if (i == 5) {
      loop_bound = p_Z11ReinterpretIimERKm_1((x(255,224)));
    }
    config_out . write(x);
  }
  for (int i = 1; i < 15; i++) {
    
#pragma HLS unroll
    config[i - 1] = p_Z11ReinterpretIimERKm_1((tmp1(((i + 1) * 32 - 1),(i * 32))));
  }
  class ap_uint< 512 > buf_ping_data_out[338];
  class ap_uint< 512 > buf_pang_data_out[338];
  read_fifo_1(data_in,buf_ping_data_out);
  int index = output_idx;
  for (int j = 0; j < loop_bound - 1; j++) {
    
#pragma HLS loop_tripcount min=128 max=128
    if (j % 2 == 0) {
      read_fifo_1(data_in,buf_pang_data_out);
      memcpy((void *)(&data_out[index]),(const void *)((void *)buf_ping_data_out),64UL * ((unsigned long )338));
    }
     else {
      read_fifo_1(data_in,buf_ping_data_out);
      memcpy((void *)(&data_out[index]),(const void *)((void *)buf_pang_data_out),64UL * ((unsigned long )338));
    }
    index += 338;
  }
  if (loop_bound % 2 == 0) {
    memcpy((void *)(&data_out[index]),(const void *)((void *)buf_pang_data_out),64UL * ((unsigned long )338));
  }
   else {
    memcpy((void *)(&data_out[index]),(const void *)((void *)buf_ping_data_out),64UL * ((unsigned long )338));
  }
}

static void stream_out_conv_1(class hls::stream< ap_uint< 256 >  > &conv_in,class ap_uint< 512 > *conv_out,class hls::stream< ap_uint< 512 >  > &config_in)
{
  int config[14];
  
#pragma HLS array_partition variable=config complete dim=1
  int conv_out_en = 0;
  class ap_uint< 512 > tmp1;
  class ap_uint< 512 > tmp2;
  for (int i = 0; i < 6; i++) {
    
#pragma HLS pipeline
    class ap_uint< 512 > x = config_in . read();
    if (i == 0) {
      tmp1 = x;
      conv_out_en = p_Z11ReinterpretIimERKm_1((x(511,480)));
    }
    if (i == 1) {
      tmp2 = x;
    }
  }
  for (int i = 1; i < 15; i++) {
    
#pragma HLS unroll
    config[i - 1] = p_Z11ReinterpretIimERKm_1((tmp1(((i + 1) * 32 - 1),(i * 32))));
  }
  if (conv_out_en == 1) {
    class ap_uint< 512 > buf_ping_conv_out[338];
    class ap_uint< 512 > buf_pang_conv_out[338];
    int loop_bound = p_Z11ReinterpretIimERKm_1((tmp2(255,224)));
    read_fifo_1(conv_in,buf_ping_conv_out);
    int index = 202800;
    for (int j = 0; j < loop_bound - 1; j++) {
      
#pragma HLS loop_tripcount min=128 max=128
      if (j % 2 == 0) {
        read_fifo_1(conv_in,buf_pang_conv_out);
        memcpy((void *)(&conv_out[index]),(const void *)((void *)buf_ping_conv_out),64UL * ((unsigned long )338));
      }
       else {
        read_fifo_1(conv_in,buf_ping_conv_out);
        memcpy((void *)(&conv_out[index]),(const void *)((void *)buf_pang_conv_out),64UL * ((unsigned long )338));
      }
      index += 338;
    }
    if (loop_bound % 2 == 0) {
      memcpy((void *)(&conv_out[index]),(const void *)((void *)buf_pang_conv_out),64UL * ((unsigned long )338));
    }
     else {
      memcpy((void *)(&conv_out[index]),(const void *)((void *)buf_ping_conv_out),64UL * ((unsigned long )338));
    }
  }
}
static void __merlin_dummy_kernel_pragma();

void top_kernel(class ap_uint< 512 > *merlin_input,class ap_uint< 512 > *merlin_output,class ap_uint< 512 > *conv_out,class ap_uint< 512 > *weights,class ap_uint< 512 > *config)
{
  __merlin_access_range(config,0,83UL);
  __merlin_access_range(weights,0,262143UL);
  __merlin_access_range(conv_out,0,173055UL);
  __merlin_access_range(merlin_output,0,172379UL);
  __merlin_access_range(merlin_input,0,173055UL);
  
#pragma ACCEL interface variable=merlin_input depth=173056 max_depth=173056
  
#pragma ACCEL interface variable=merlin_output depth=172380 max_depth=172380
  
#pragma ACCEL interface variable=conv_out depth=173056 max_depth=173056
  
#pragma ACCEL interface variable=weights depth=262144 max_depth=262144
  
#pragma ACCEL interface variable=config depth=84 max_depth=84
  printf("start kernel !!! \n");
  const int offset_list[13][4] = {{(0), (86528), (0), (0)}, {(86528), (129792), (72), (256)}, {(129792), (151424), (392), (512)}, {(151424), (162240), (1544), (768)}, {(162240), (167648), (6152), (1024)}, {(167648), (170352), (24584), (1280)}, {(170352), (175760), (319496), (1536)}, {(175760), (186576), (614408), (1792)}, {(186576), (189280), (630792), (2048)}, {(189280), (194688), (704520), (2304)}, {(186576), (197392), (712712), (2560)}, {(197392), (213616), (714760), (2816)}, {(213616), (224432), (770056), (3072)}};
  const int config_list[13][3][32] = {{{(3), (416), (416), (8), (1), (416), (416), (16), (3), (1), (1), (16), (1), (1), (0), (0), (173056), (519168), (173056), (2768896), (9), (27), (432), (512), (36), (3120), (36), (32), (0), (0), (0), (0)}, {(1), (416), (416), (16), (1), (208), (208), (16), (2), (2), (1), (0), (0), (0), (1), (0), (173056), (2768896), (43264), (692224), (4), (64), (0), (128), (32), (3120), (32), (32), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (208), (208), (16), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (692224), (0), (0), (0), (128), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(3), (208), (208), (16), (1), (208), (208), (32), (3), (1), (1), (32), (1), (1), (0), (0), (43264), (692224), (43264), (1384448), (9), (144), (4608), (256), (72), (1560), (72), (16), (0), (0), (0), (0)}, {(1), (208), (208), (32), (1), (104), (104), (32), (2), (2), (1), (0), (0), (0), (1), (0), (43264), (1384448), (10816), (346112), (4), (128), (0), (64), (64), (1560), (64), (16), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (104), (104), (32), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (346112), (0), (0), (0), (64), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(3), (104), (104), (32), (1), (104), (104), (64), (3), (1), (1), (64), (1), (1), (0), (0), (10816), (346112), (10816), (692224), (9), (288), (18432), (128), (144), (780), (144), (8), (0), (0), (0), (0)}, {(1), (104), (104), (64), (1), (52), (52), (64), (2), (2), (1), (0), (0), (0), (1), (0), (10816), (692224), (2704), (173056), (4), (256), (0), (32), (128), (780), (128), (8), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (52), (52), (64), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (173056), (0), (0), (0), (32), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(3), (52), (52), (64), (1), (52), (52), (128), (3), (1), (1), (128), (1), (1), (0), (0), (2704), (173056), (2704), (346112), (9), (576), (73728), (64), (288), (390), (288), (4), (0), (0), (0), (0)}, {(1), (52), (52), (128), (1), (26), (26), (128), (2), (2), (1), (0), (0), (0), (1), (0), (2704), (346112), (676), (86528), (4), (512), (0), (16), (256), (390), (256), (4), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (26), (26), (128), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (86528), (0), (0), (0), (16), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(3), (26), (26), (128), (1), (26), (26), (256), (3), (1), (1), (256), (1), (1), (0), (1), (676), (86528), (676), (173056), (9), (1152), (294912), (32), (576), (182), (576), (2), (0), (0), (0), (0)}, {(1), (26), (26), (256), (1), (13), (13), (256), (2), (2), (1), (0), (0), (0), (1), (0), (676), (173056), (169), (43264), (4), (1024), (0), (8), (512), (182), (512), (2), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (256), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (43264), (0), (0), (0), (8), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(3), (13), (13), (256), (1), (13), (13), (512), (3), (1), (1), (512), (1), (1), (0), (0), (169), (43264), (169), (86528), (9), (2304), (1179648), (16), (1152), (91), (1152), (1), (0), (0), (0), (0)}, {(1), (13), (13), (512), (1), (13), (13), (512), (2), (1), (1), (0), (0), (0), (1), (0), (169), (86528), (169), (86528), (4), (2048), (0), (16), (1024), (91), (1024), (1), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (512), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (86528), (0), (0), (0), (16), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(3), (13), (13), (512), (1), (13), (13), (1024), (3), (1), (1), (1024), (1), (1), (0), (0), (169), (86528), (169), (173056), (9), (4608), (4718592), (32), (2304), (91), (2304), (1), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (1024), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (173056), (0), (0), (0), (16), (0), (0), (0), (0), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (1024), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (173056), (0), (0), (0), (32), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(1), (13), (13), (1024), (1), (13), (13), (256), (1), (1), (0), (256), (1), (1), (0), (0), (169), (173056), (169), (43264), (1), (1024), (262144), (8), (512), (91), (512), (1), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (256), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (43264), (0), (0), (0), (32), (0), (0), (0), (0), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (256), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (43264), (0), (0), (0), (8), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(3), (13), (13), (256), (1), (13), (13), (512), (3), (1), (1), (512), (1), (1), (0), (0), (169), (43264), (169), (86528), (9), (2304), (1179648), (16), (1152), (91), (1152), (1), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (512), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (86528), (0), (0), (0), (8), (0), (0), (0), (0), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (512), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (86528), (0), (0), (0), (16), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(1), (13), (13), (512), (1), (13), (13), (256), (1), (1), (0), (256), (0), (1), (1), (0), (169), (86528), (169), (43264), (1), (512), (130560), (8), (256), (91), (256), (1), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (256), (0), (0), (0), (1), (0), (0), (0), (0), (0), (0), (0), (43264), (0), (0), (0), (16), (0), (0), (0), (0), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (256), (0), (0), (0), (1), (0), (0), (0), (0), (0), (0), (0), (43264), (0), (0), (0), (8), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(1), (13), (13), (256), (1), (13), (13), (128), (1), (1), (0), (128), (1), (1), (0), (0), (169), (43264), (169), (21632), (1), (256), (32768), (4), (128), (91), (128), (1), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (13), (13), (128), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (21632), (0), (0), (0), (8), (0), (0), (0), (0), (0), (0), (0), (0)}, {(1), (13), (13), (128), (1), (26), (26), (128), (0), (2), (0), (0), (0), (0), (1), (0), (169), (21632), (676), (86528), (0), (0), (0), (16), (0), (91), (0), (1), (0), (0), (0), (0)}}, {{(3), (26), (26), (384), (1), (26), (26), (256), (3), (1), (1), (256), (1), (1), (0), (0), (676), (259584), (676), (173056), (9), (3456), (884736), (32), (1728), (182), (1728), (2), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (26), (26), (256), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (173056), (0), (0), (0), (8), (0), (0), (0), (0), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (26), (26), (256), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (0), (173056), (0), (0), (0), (32), (0), (0), (0), (0), (0), (0), (0), (0)}}, {{(1), (26), (26), (256), (1), (26), (26), (256), (1), (1), (0), (256), (0), (1), (1), (0), (676), (173056), (676), (173056), (1), (256), (65280), (32), (128), (169), (128), (2), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (26), (26), (256), (0), (0), (0), (1), (0), (0), (0), (0), (0), (0), (0), (173056), (0), (0), (0), (32), (0), (0), (0), (0), (0), (0), (0), (0)}, {(0), (0), (0), (0), (0), (26), (26), (256), (0), (0), (0), (1), (0), (0), (0), (0), (0), (0), (0), (173056), (0), (0), (0), (32), (0), (0), (0), (0), (0), (0), (0), (0)}}};
  
#pragma HLS array_partition variable=config_list cyclic factor=8 dim=3
  class hls::stream< ap_uint< 512 >  > config2("c2");
  
#pragma HLS stream variable =config2 depth = 6
  class hls::stream< ap_uint< 512 >  > config3("c3");
  
#pragma HLS stream variable =config3 depth = 6
  class hls::stream< ap_uint< 512 >  > config4("c4");
  
#pragma HLS stream variable =config4 depth = 6
  class hls::stream< ap_uint< 512 >  > config5("c5");
  
#pragma HLS stream variable =config5 depth = 6
  class hls::stream< ap_uint< 512 >  > config6("c6");
  
#pragma HLS stream variable =config6 depth = 6
  class hls::stream< ap_uint< 512 >  > config7("c7");
  
#pragma HLS stream variable =config7 depth = 6
  class hls::stream< ap_uint< 512 >  > config8("c8");
  
#pragma HLS stream variable =config8 depth = 6
  
#pragma HLS RESOURCE variable=config2 core=FIFO_LUTRAM
  
#pragma HLS RESOURCE variable=config3 core=FIFO_LUTRAM
  
#pragma HLS RESOURCE variable=config4 core=FIFO_LUTRAM
  
#pragma HLS RESOURCE variable=config5 core=FIFO_LUTRAM
  
#pragma HLS RESOURCE variable=config6 core=FIFO_LUTRAM
  
#pragma HLS RESOURCE variable=config7 core=FIFO_LUTRAM
  
#pragma HLS RESOURCE variable=config8 core=FIFO_LUTRAM
  class hls::stream< ap_uint< 256 >  > conv_out_st("conv_out_st");
  
#pragma HLS stream variable =conv_out_st depth = 128
  class hls::stream< ap_uint< 128 >  > fifo_image1[8];
  
#pragma HLS array_partition variable=fifo_image1 complete dim=1
  
#pragma HLS stream variable = fifo_image1 depth = 128
  class hls::stream< float  > fifo_image2[8];
  
#pragma HLS array_partition variable=fifo_image2 complete dim=1
  
#pragma HLS stream variable = fifo_image2 depth = 128
  class hls::stream< ap_uint< 256 >  > fifo_image3("fifo_image3");
  
#pragma HLS stream variable = fifo_image3 depth = 256
  class hls::stream< ap_uint< 256 >  > fifo_image4("fifo_image4");
  
#pragma HLS stream variable = fifo_image4 depth = 32
  class hls::stream< ap_uint< 256 >  > fifo_image5("fifo_image5");
  
#pragma HLS stream variable = fifo_image5 depth = 32
  class hls::stream< float  > fifo_bias("fifo_bias");
  
#pragma HLS stream variable = fifo_bias depth = 16
  class hls::stream< ap_uint< 256 >  > fifo_weights("fifo_weights");
  
#pragma HLS stream variable = fifo_weights depth = 16
  int input_idx[16];
  int output_idx[16];
  int w_offset[16];
  int config_offset[16];
  int bias_offset[16];
  class ap_uint< 512 > config_array[84];
  memcpy((void *)config_array,(const void *)((void *)(&config[0])),64UL * ((unsigned long )(6 * 13 + 6)));
  class ap_uint< 512 > wide_i = config_array[0];
  class ap_uint< 512 > wide_o = config_array[1];
  class ap_uint< 512 > wide_w = config_array[2];
  class ap_uint< 512 > wide_c = config_array[3];
  class ap_uint< 512 > wide_b = config_array[4];
  for (int i = 0; i < 16; i++) {
    
#pragma HLS dependence variable=bias_offset array inter false
    
#pragma HLS dependence variable=config_offset array inter false
    
#pragma HLS dependence variable=w_offset array inter false
    
#pragma HLS dependence variable=output_idx array inter false
    
#pragma HLS dependence variable=input_idx array inter false
    
#pragma HLS pipeline
    input_idx[i] = p_Z11ReinterpretIimERKm_1((wide_i(((i + 1) * 32 - 1),(i * 32))));
    output_idx[i] = p_Z11ReinterpretIimERKm_1((wide_o(((i + 1) * 32 - 1),(i * 32))));
    w_offset[i] = p_Z11ReinterpretIimERKm_1((wide_w(((i + 1) * 32 - 1),(i * 32))));
    config_offset[i] = p_Z11ReinterpretIimERKm_1((wide_c(((i + 1) * 32 - 1),(i * 32))));
    bias_offset[i] = p_Z11ReinterpretIimERKm_1((wide_b(((i + 1) * 32 - 1),(i * 32))));
  }
  for (int layer_cnt = 0; layer_cnt < 13; layer_cnt++) {
    
#pragma HLS loop_tripcount min=1 max=1
    printf("layer %d \n",layer_cnt);
    
#pragma HLS dataflow
    stream_in_weights_1(weights,fifo_weights,fifo_bias,config_array + 6,config2,offset_list[layer_cnt][2],offset_list[layer_cnt][3],config_offset[layer_cnt]);
    stream_in_image_1(merlin_input,fifo_image1,config2,config3,offset_list[layer_cnt][0]);
    conv_switch_1(fifo_image1,fifo_weights,config3,fifo_image2,config4,config_list[layer_cnt][0]);
    bias_switch_1(fifo_image2,fifo_bias,fifo_image3,conv_out_st,config4,config5);
    maxpool_switch_1(fifo_image3,fifo_image4,config5,config6);
    upsample_switch_1(fifo_image4,fifo_image5,config6,config7);
    stream_out_image_1(fifo_image5,merlin_output,config7,config8,offset_list[layer_cnt][1]);
    stream_out_conv_1(conv_out_st,conv_out,config8);
  }
}
