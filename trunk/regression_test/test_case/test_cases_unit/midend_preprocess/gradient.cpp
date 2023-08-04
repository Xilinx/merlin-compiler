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
#include "memcpy_512.h"
#include <string.h> 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#define CHUNK_SIZE 16
#define L 10 //label_size;
#define D 784 //feature_size;
#define weight_size (L-1)*D
// to divide feature_size (D)
#define F_UR 4

void mars_kernel_0_12_0_node_0_stage_0(int k0,int exec)
{
  
#pragma HLS INLINE OFF
  if (exec == 1) {
  }
}

void mars_kernel_0_12_0_node_1_stage_0(int k0,int exec,double feature_buf[16][784],class ap_uint< 512 > *feature_samples,double feature_samples_9_0_buf[16][784],double label_buf[16],class ap_uint< 512 > *label_samples,double label_samples_6_0_buf[16],double marginY_buf[16],double margin_buf[16][10],int maxMarginIndex_buf[16],double maxMargin_buf[16],double multiplier_buf[16][10],double sum_buf[16])
{
  int jj;
  int j0;
  int kk;
  int t1;
  int t;
  
#pragma HLS INLINE OFF
  if (exec == 1) {
// combine these two nodes because they both access data
{
//memset(sum_buf, 0, sizeof(double) * CHUNK_SIZE);
//memset(margin_buf, 0, sizeof(double) * CHUNK_SIZE*L);
//memset(multiplier_buf, 0, sizeof(double) * CHUNK_SIZE*L);
      int t_size = 16;
      int t;
      for (t = 0; t < t_size; t++) {
        sum_buf[t] = ((double )0);
      }
      for (t = 0; t < t_size; t++) {
        for (int t1 = 0; t1 < 10; t1++) {
          margin_buf[t][t1] = ((double )0);
        }
      }
      for (t = 0; t < t_size; t++) {
        for (int t1 = 0; t1 < 10; t1++) {
          multiplier_buf[t][t1] = ((double )0);
        }
      }
/*INIT: */
      
#pragma HLS array_partition variable=label_samples_6_0_buf cyclic factor = 8 dim=1
      memcpy_wide_bus_read_double(&label_samples_6_0_buf[0],((class ap_uint< 512 > *)label_samples),((unsigned int )(((unsigned long )(0 + k0 * 16)) * sizeof(double ))),((unsigned int )(sizeof(double ) * ((unsigned long )16))));
      for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
        
//#pragma ACCEL PIPELINE II=1
        int k = k0 * 16 + kk;
//              if (k >= num_samples) break;
        marginY_buf[kk] = 0.0;
//-std::numeric_limits<double>::infinity();
        maxMargin_buf[kk] = ((double )(-__builtin_inff()));
        maxMarginIndex_buf[kk] = 0;
//label_buf[kk] = data[k*(D+1)];
        label_buf[kk] = label_samples_6_0_buf[kk];
      }
      
#pragma HLS array_partition variable=feature_samples_9_0_buf cyclic factor = 8 dim=2
      ::memcpy_wide_bus_read_double_2d(feature_samples_9_0_buf, 0, 0, ((class ap_uint< 512 > *)feature_samples),((unsigned int )(((unsigned long )(0 + k0 * 12544)) * sizeof(double ))),((unsigned int )(sizeof(double ) * ((unsigned long )12544))));
      for (int j0 = 0; j0 < 784 / 16; j0 += 1) {
        for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline flatten
{
          
//#pragma ACCEL PIPELINE II=1
          for (int jj = 0; jj < 16; jj += 1) {
            
//#pragma ACCEL PARALLEL COMPLETE
            int k = k0 * 16 + kk;
            int j = j0 * 16 + jj;
            feature_buf[kk][j] = feature_samples_9_0_buf[kk][j0 * 16 + jj];
          }
        }
      }
    }
  }
}

void mars_kernel_0_12_0_node_2_stage_1(int k0,int exec,double feature_buf[16][784],double margin_buf[16][10],double weights_13_0_buf[9][784])
{
  int j0;
  int i;
  int jj;
  int kk;
  
#pragma HLS INLINE OFF
  if (exec == 1) {{
      double margin_buf_reduc[16][4][9];
/*STAGE_1:*/
//for(j0 = 0; j0 < D; j0+=F_UR ) {
      for (kk = 0; kk < 16; kk++) {
        
        for (int jj = 0; jj < 4; jj += 1) {
          
          for (int i = 0; i < 10 - 1; i++) {
            
            margin_buf_reduc[kk][jj][i] = ((double )0);
          }
        }
      }
      for (j0 = 0; j0 < 784 / 4; j0 += 1) {
        for (kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline flatten
{
          
          for (int jj = 0; jj < 4; jj += 1) {
            
            for (int i = 0; i < 10 - 1; i++) {
              
              int j = j0 * 4 + jj;
              margin_buf_reduc[kk][jj][i] += weights_13_0_buf[i][j0 * 4 + jj] * feature_buf[kk][j];
            }
          }
        }
      }
      for (kk = 0; kk < 16; kk++) {
        
        for (int jj = 0; jj < 4; jj += 1) {
          
          for (int i = 0; i < 10 - 1; i++) {
            
            margin_buf[0 + 1 * kk][0 + 1 * i] += margin_buf_reduc[kk][jj][i];
          }
        }
      }
    }
  }
}

void mars_kernel_0_12_0_node_3_stage_2(int k0,int exec,double label_buf[16],double marginY_buf[16],double margin_buf[16][10],double margins[16][10 - 1],int maxMarginIndex_buf[16],double maxMargin_buf[16],double multiplier_buf[16][10],double sum_buf[16])
{
  int i;
  int kk;
  
#pragma HLS INLINE OFF
  if (exec == 1) {
// STAGE_2:      
{
      for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
        
        double label = label_buf[kk];
        marginY_buf[kk] = margin_buf[kk][((int )label) - 1];
      }
      for (i = 0; i < 10 - 1; i++) {
        for (kk = 0; kk < 16; kk++) {
//  #pragma ACCEL pipeline II=2
          if (margin_buf[kk][i] > maxMargin_buf[kk]) {
            maxMargin_buf[kk] = margin_buf[kk][i];
            maxMarginIndex_buf[kk] = i;
          }
          margins[kk][i] = margin_buf[kk][i];
        }
      }
/*STAGE_2: */
//double sum = 0.0;
      for (i = 0; i < 10 - 1; i++) {
        for (kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
          
          if (maxMargin_buf[kk] > ((double )0)) {
            margins[kk][i] -= maxMargin_buf[kk];
            if (i == maxMarginIndex_buf[kk]) {
              sum_buf[kk] += exp(-maxMargin_buf[kk]);
            }
             else {
              sum_buf[kk] += exp(margins[kk][i]);
            }
          }
           else {
            sum_buf[kk] += exp(margins[kk][i]);
          }
        }
      }
      for (int kk = 0; kk < 16; kk++) {
        double label = label_buf[kk];
// update gradient
        for (int i = 0; i < 10 - 1; i++) 
// Original: #pragma ACCEL pipeline
{
//FIXME:  
          
          multiplier_buf[kk][i] = exp(margins[kk][i]) / (sum_buf[kk] + 1.0);
          if (label != 0.0 && label == ((double )(i + 1))) {
            multiplier_buf[kk][i] -= 1.0;
          }
        }
      }
    }
  }
}

void mars_kernel_0_12_0_node_4_stage_3(int k0,int exec,double feature_buf[16][784],double multiplier_buf[16][10],double output_24_0_buf[9][784])
{
  int i;
  int jj;
  int j0;
  int kk;
  
#pragma HLS INLINE OFF
  if (exec == 1) {
    for (kk = 0; kk < 16; kk++) {
      for (j0 = 0; j0 < 784; j0 += 4) 
// Original: #pragma ACCEL pipeline flatten
{
        
        for (int jj = 0; jj < 4; jj++) 
// Original: #pragma ACCEL parallel copmlete
{
          
          for (int i = 0; i < 10 - 1; i++) 
// Original: #pragma ACCEL parallel copmlete
{
            
            int j = j0 + jj;
            output_24_0_buf[i][j0 + jj] += multiplier_buf[kk][i] * feature_buf[kk][j];
//FIXME:      //output[i*D+j] += multiplier_buf[kk][i]*feature_buf[kk][j];
          }
        }
      }
    }
//STAGE_3: 
  }
}

void mars_kernel_0_12_0_node_5_stage_3(int k0,int exec,double label_buf[16],double marginY_buf[16],double maxMargin_buf[16],double *mars_output_loss_buffer,double sum_buf[16])
{
  double output_loss_buffer =  *mars_output_loss_buffer;
  int kk;
  
#pragma HLS INLINE OFF
  if (exec == 1) {
    for (kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
      
      double label = label_buf[kk];
// compute loss
// FIXME:
// math.logip(sum)
      double loss = log(sum_buf[kk] + ((double )1));
      if (label > 0.0) {
        loss -= marginY_buf[kk];
      }
      if (maxMargin_buf[kk] > ((double )0)) {
        loss += maxMargin_buf[kk];
      }
//output[weight_size] += loss;
      output_loss_buffer += loss;
    }
//STAGE_4: 
  }
   *mars_output_loss_buffer = output_loss_buffer;
}

void mars_kernel_0_12_0_assign_feature_buf(double target[16][784],double orig[16][784])
{
  int i1;
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    for (i1 = 0; i1 < 784; i1++) {
      target[i0][i1] = orig[i0][i1];
    }
  }
}

void mars_kernel_0_12_0_assign_feature_samples_9_0_buf(double target[16][784],double orig[16][784])
{
  int i1;
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    for (i1 = 0; i1 < 784; i1++) {
      target[i0][i1] = orig[i0][i1];
    }
  }
}

void mars_kernel_0_12_0_assign_label_buf(double target[16],double orig[16])
{
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    target[i0] = orig[i0];
  }
}

void mars_kernel_0_12_0_assign_label_samples_6_0_buf(double target[16],double orig[16])
{
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    target[i0] = orig[i0];
  }
}

void mars_kernel_0_12_0_assign_marginY_buf(double target[16],double orig[16])
{
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    target[i0] = orig[i0];
  }
}

void mars_kernel_0_12_0_assign_margin_buf(double target[16][10],double orig[16][10])
{
  int i1;
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    for (i1 = 0; i1 < 10; i1++) {
      target[i0][i1] = orig[i0][i1];
    }
  }
}

void mars_kernel_0_12_0_assign_margins(double target[16][10 - 1],double orig[16][10 - 1])
{
  int i1;
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    for (i1 = 0; i1 < 9; i1++) {
      target[i0][i1] = orig[i0][i1];
    }
  }
}

void mars_kernel_0_12_0_assign_maxMarginIndex_buf(int target[16],int orig[16])
{
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    target[i0] = orig[i0];
  }
}

void mars_kernel_0_12_0_assign_maxMargin_buf(double target[16],double orig[16])
{
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    target[i0] = orig[i0];
  }
}

void mars_kernel_0_12_0_assign_multiplier_buf(double target[16][10],double orig[16][10])
{
  int i1;
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    for (i1 = 0; i1 < 10; i1++) {
      target[i0][i1] = orig[i0][i1];
    }
  }
}

void mars_kernel_0_12_0_assign_output_24_0_buf(double target[9][784],double orig[9][784])
{
  int i1;
  int i0;
  for (i0 = 0; i0 < 9; i0++) {
    for (i1 = 0; i1 < 784; i1++) {
      target[i0][i1] = orig[i0][i1];
    }
  }
}

void mars_kernel_0_12_0_assign_output_loss_buffer(double *target,double *orig)
{
   *target =  *orig;
}

void mars_kernel_0_12_0_assign_sum_buf(double target[16],double orig[16])
{
  int i0;
  for (i0 = 0; i0 < 16; i0++) {
    target[i0] = orig[i0];
  }
}

void mars_kernel_0_12_0_assign_weights_13_0_buf(double target[9][784],double orig[9][784])
{
  int i1;
  int i0;
  for (i0 = 0; i0 < 9; i0++) {
    for (i1 = 0; i1 < 784; i1++) {
      target[i0][i1] = orig[i0][i1];
    }
  }
}

void mars_kernel_0_12_0(int mars_k0,int mars_init,int mars_cond,double mars_feature_buf_0[16][784],double mars_feature_buf_1[16][784],double mars_feature_buf_2[16][784],double mars_feature_buf_3[16][784],class ap_uint< 512 > *feature_samples,double mars_feature_samples_9_0_buf_0[16][784],double mars_label_buf_0[16],double mars_label_buf_1[16],double mars_label_buf_2[16],double mars_label_buf_3[16],class ap_uint< 512 > *label_samples,double mars_label_samples_6_0_buf_0[16],double mars_marginY_buf_0[16],double mars_marginY_buf_1[16],double mars_marginY_buf_2[16],double mars_marginY_buf_3[16],double mars_margin_buf_0[16][10],double mars_margin_buf_1[16][10],double mars_margin_buf_2[16][10],double mars_margins_2[16][10 - 1],int mars_maxMarginIndex_buf_0[16],int mars_maxMarginIndex_buf_1[16],int mars_maxMarginIndex_buf_2[16],double mars_maxMargin_buf_0[16],double mars_maxMargin_buf_1[16],double mars_maxMargin_buf_2[16],double mars_maxMargin_buf_3[16],double mars_multiplier_buf_0[16][10],double mars_multiplier_buf_1[16][10],double mars_multiplier_buf_2[16][10],double mars_multiplier_buf_3[16][10],double mars_output_24_0_buf_3[9][784],double *mars_output_loss_buffer_3,double mars_sum_buf_0[16],double mars_sum_buf_1[16],double mars_sum_buf_2[16],double mars_sum_buf_3[16],double mars_weights_13_0_buf_1[9][784])
{
  
#pragma HLS dataflow
  mars_kernel_0_12_0_node_0_stage_0(mars_k0 - 0,mars_k0 >= mars_init + 0 & mars_k0 < mars_cond + 0);
  mars_kernel_0_12_0_node_1_stage_0(mars_k0 - 0,mars_k0 >= mars_init + 0 & mars_k0 < mars_cond + 0,mars_feature_buf_0,feature_samples,mars_feature_samples_9_0_buf_0,mars_label_buf_0,label_samples,mars_label_samples_6_0_buf_0,mars_marginY_buf_0,mars_margin_buf_0,mars_maxMarginIndex_buf_0,mars_maxMargin_buf_0,mars_multiplier_buf_0,mars_sum_buf_0);
  mars_kernel_0_12_0_node_2_stage_1(mars_k0 - 1,mars_k0 >= mars_init + 1 & mars_k0 < mars_cond + 1,mars_feature_buf_1,mars_margin_buf_1,mars_weights_13_0_buf_1);
  mars_kernel_0_12_0_node_3_stage_2(mars_k0 - 2,mars_k0 >= mars_init + 2 & mars_k0 < mars_cond + 2,mars_label_buf_2,mars_marginY_buf_2,mars_margin_buf_2,mars_margins_2,mars_maxMarginIndex_buf_2,mars_maxMargin_buf_2,mars_multiplier_buf_2,mars_sum_buf_2);
  mars_kernel_0_12_0_node_4_stage_3(mars_k0 - 3,mars_k0 >= mars_init + 3 & mars_k0 < mars_cond + 3,mars_feature_buf_3,mars_multiplier_buf_3,mars_output_24_0_buf_3);
  mars_kernel_0_12_0_node_5_stage_3(mars_k0 - 3,mars_k0 >= mars_init + 3 & mars_k0 < mars_cond + 3,mars_label_buf_3,mars_marginY_buf_3,mars_maxMargin_buf_3,mars_output_loss_buffer_3,mars_sum_buf_3);
}
#pragma ACCEL kernel
void gradient(int n_samples,int label_size,int feature_size,class ap_uint< 512 > *weights, double *data,class ap_uint< 512 > *output,class ap_uint< 512 > *feature_samples,class ap_uint< 512 > *label_samples)
//double   global_weights[9*784], 
//BUS_TYPE global_data[32*785], 
//double   global_output[9*784+1])
{
  double mars_kernel_0_12_0_maxMargin_buf_3[16];
  double mars_kernel_0_12_0_maxMargin_buf_2[16];
  double mars_kernel_0_12_0_maxMargin_buf_1[16];
  double mars_kernel_0_12_0_maxMargin_buf_0[16];
  int mars_kernel_0_12_0_maxMarginIndex_buf_3[16];
  int mars_kernel_0_12_0_maxMarginIndex_buf_2[16];
  int mars_kernel_0_12_0_maxMarginIndex_buf_1[16];
  int mars_kernel_0_12_0_maxMarginIndex_buf_0[16];
  double mars_kernel_0_12_0_marginY_buf_3[16];
  double mars_kernel_0_12_0_marginY_buf_2[16];
  double mars_kernel_0_12_0_marginY_buf_1[16];
  double mars_kernel_0_12_0_marginY_buf_0[16];
  double mars_kernel_0_12_0_label_buf_3[16];
  double mars_kernel_0_12_0_label_buf_2[16];
  double mars_kernel_0_12_0_label_buf_1[16];
  double mars_kernel_0_12_0_label_buf_0[16];
  double mars_kernel_0_12_0_feature_buf_3[16][784];
  double mars_kernel_0_12_0_feature_buf_2[16][784];
  double mars_kernel_0_12_0_feature_buf_1[16][784];
  double mars_kernel_0_12_0_feature_buf_0[16][784];
  static double output_24_0_buf[9][784];
  static double weights_13_0_buf[9][784];
  static double feature_samples_9_0_buf[16][784];
  static double label_samples_6_0_buf[16];
//FIXME:
// const int L = 10; //label_size;
// const int D = 784; //feature_size;
// const int weight_size = (L-1)*D;
  int num_samples = n_samples;
// ZP: 2015-11-15
//memset(output, 0, sizeof(double)*(weight_size+1));
//double* margins = new double[L-1];
  double margins[16][10 - 1];
  double label_buf[16];
  double feature_buf[16][784];
  double marginY_buf[16];
//-std::numeric_limits<double>::infinity();
  double maxMargin_buf[16];
  int maxMarginIndex_buf[16];
  int upper_bound = (n_samples + 16 - 1) / 16;
//for(int k = 0; k < num_samples; k++ ) {
  n_samples % 16 == 0?((void )0) : __assert_fail("(n_samples % 16) == 0","gradient.cpp",50,__PRETTY_FUNCTION__);
  int j0;
  int kk;
  int i;
  double output_loss_buffer = memcpy_wide_bus_single_read_double(((class ap_uint< 512 > *)output),(((10 - 1) * 784) * sizeof(double )));
  
#pragma HLS array_partition variable=weights_13_0_buf cyclic factor = 8 dim=2
  ::memcpy_wide_bus_read_double_2d(weights_13_0_buf, 0, 0, ((class ap_uint< 512 > *)weights),(0 * sizeof(double )),(sizeof(double ) * ((unsigned long )7056)));
  
#pragma HLS array_partition variable=output_24_0_buf cyclic factor = 8 dim=2
  ::memcpy_wide_bus_read_double_2d(output_24_0_buf, 0, 0, ((class ap_uint< 512 > *)output),(0 * sizeof(double )),(sizeof(double ) * ((unsigned long )7056)));
  int mars_count_0_12_0 = 0;
  mars_kernel_0_12_0_assign_feature_buf(mars_kernel_0_12_0_feature_buf_0,feature_buf);
  mars_kernel_0_12_0_assign_feature_buf(mars_kernel_0_12_0_feature_buf_1,feature_buf);
  mars_kernel_0_12_0_assign_feature_buf(mars_kernel_0_12_0_feature_buf_2,feature_buf);
  mars_kernel_0_12_0_assign_feature_buf(mars_kernel_0_12_0_feature_buf_3,feature_buf);
  mars_kernel_0_12_0_assign_label_buf(mars_kernel_0_12_0_label_buf_0,label_buf);
  mars_kernel_0_12_0_assign_label_buf(mars_kernel_0_12_0_label_buf_1,label_buf);
  mars_kernel_0_12_0_assign_label_buf(mars_kernel_0_12_0_label_buf_2,label_buf);
  mars_kernel_0_12_0_assign_label_buf(mars_kernel_0_12_0_label_buf_3,label_buf);
  mars_kernel_0_12_0_assign_marginY_buf(mars_kernel_0_12_0_marginY_buf_0,marginY_buf);
  mars_kernel_0_12_0_assign_marginY_buf(mars_kernel_0_12_0_marginY_buf_1,marginY_buf);
  mars_kernel_0_12_0_assign_marginY_buf(mars_kernel_0_12_0_marginY_buf_2,marginY_buf);
  mars_kernel_0_12_0_assign_marginY_buf(mars_kernel_0_12_0_marginY_buf_3,marginY_buf);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(mars_kernel_0_12_0_maxMarginIndex_buf_0,maxMarginIndex_buf);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(mars_kernel_0_12_0_maxMarginIndex_buf_1,maxMarginIndex_buf);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(mars_kernel_0_12_0_maxMarginIndex_buf_2,maxMarginIndex_buf);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(mars_kernel_0_12_0_maxMarginIndex_buf_3,maxMarginIndex_buf);
  mars_kernel_0_12_0_assign_maxMargin_buf(mars_kernel_0_12_0_maxMargin_buf_0,maxMargin_buf);
  mars_kernel_0_12_0_assign_maxMargin_buf(mars_kernel_0_12_0_maxMargin_buf_1,maxMargin_buf);
  mars_kernel_0_12_0_assign_maxMargin_buf(mars_kernel_0_12_0_maxMargin_buf_2,maxMargin_buf);
  mars_kernel_0_12_0_assign_maxMargin_buf(mars_kernel_0_12_0_maxMargin_buf_3,maxMargin_buf);
  for (int k0 = 0; k0 < upper_bound + 3; k0++) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL PIPELINE II=1
{
    
#pragma HLS loop_tripcount max=1872
    double sum_buf[16];
    double margin_buf[16][10];
    double multiplier_buf[16][10];
//STAGE_3: 
//STAGE_4: 
    static double mars_kernel_0_12_0_sum_buf_3[16];
    static double mars_kernel_0_12_0_sum_buf_2[16];
    static double mars_kernel_0_12_0_sum_buf_1[16];
    static double mars_kernel_0_12_0_sum_buf_0[16];
    static double mars_kernel_0_12_0_multiplier_buf_3[16][10];
    static double mars_kernel_0_12_0_multiplier_buf_2[16][10];
    static double mars_kernel_0_12_0_multiplier_buf_1[16][10];
    static double mars_kernel_0_12_0_multiplier_buf_0[16][10];
    static double mars_kernel_0_12_0_margin_buf_3[16][10];
    static double mars_kernel_0_12_0_margin_buf_2[16][10];
    static double mars_kernel_0_12_0_margin_buf_1[16][10];
    static double mars_kernel_0_12_0_margin_buf_0[16][10];
    if (mars_count_0_12_0 == 0) 
      mars_kernel_0_12_0(k0,0,upper_bound,mars_kernel_0_12_0_feature_buf_0,mars_kernel_0_12_0_feature_buf_1,mars_kernel_0_12_0_feature_buf_2,mars_kernel_0_12_0_feature_buf_3,feature_samples,feature_samples_9_0_buf,mars_kernel_0_12_0_label_buf_0,mars_kernel_0_12_0_label_buf_1,mars_kernel_0_12_0_label_buf_2,mars_kernel_0_12_0_label_buf_3,label_samples,label_samples_6_0_buf,mars_kernel_0_12_0_marginY_buf_0,mars_kernel_0_12_0_marginY_buf_1,mars_kernel_0_12_0_marginY_buf_2,mars_kernel_0_12_0_marginY_buf_3,mars_kernel_0_12_0_margin_buf_0,mars_kernel_0_12_0_margin_buf_1,mars_kernel_0_12_0_margin_buf_2,margins,mars_kernel_0_12_0_maxMarginIndex_buf_0,mars_kernel_0_12_0_maxMarginIndex_buf_1,mars_kernel_0_12_0_maxMarginIndex_buf_2,mars_kernel_0_12_0_maxMargin_buf_0,mars_kernel_0_12_0_maxMargin_buf_1,mars_kernel_0_12_0_maxMargin_buf_2,mars_kernel_0_12_0_maxMargin_buf_3,mars_kernel_0_12_0_multiplier_buf_0,mars_kernel_0_12_0_multiplier_buf_1,mars_kernel_0_12_0_multiplier_buf_2,mars_kernel_0_12_0_multiplier_buf_3,output_24_0_buf,&output_loss_buffer,mars_kernel_0_12_0_sum_buf_0,mars_kernel_0_12_0_sum_buf_1,mars_kernel_0_12_0_sum_buf_2,mars_kernel_0_12_0_sum_buf_3,weights_13_0_buf);
     else if (mars_count_0_12_0 == 1) 
      mars_kernel_0_12_0(k0,0,upper_bound,mars_kernel_0_12_0_feature_buf_3,mars_kernel_0_12_0_feature_buf_0,mars_kernel_0_12_0_feature_buf_1,mars_kernel_0_12_0_feature_buf_2,feature_samples,feature_samples_9_0_buf,mars_kernel_0_12_0_label_buf_3,mars_kernel_0_12_0_label_buf_0,mars_kernel_0_12_0_label_buf_1,mars_kernel_0_12_0_label_buf_2,label_samples,label_samples_6_0_buf,mars_kernel_0_12_0_marginY_buf_3,mars_kernel_0_12_0_marginY_buf_0,mars_kernel_0_12_0_marginY_buf_1,mars_kernel_0_12_0_marginY_buf_2,mars_kernel_0_12_0_margin_buf_3,mars_kernel_0_12_0_margin_buf_0,mars_kernel_0_12_0_margin_buf_1,margins,mars_kernel_0_12_0_maxMarginIndex_buf_3,mars_kernel_0_12_0_maxMarginIndex_buf_0,mars_kernel_0_12_0_maxMarginIndex_buf_1,mars_kernel_0_12_0_maxMargin_buf_3,mars_kernel_0_12_0_maxMargin_buf_0,mars_kernel_0_12_0_maxMargin_buf_1,mars_kernel_0_12_0_maxMargin_buf_2,mars_kernel_0_12_0_multiplier_buf_3,mars_kernel_0_12_0_multiplier_buf_0,mars_kernel_0_12_0_multiplier_buf_1,mars_kernel_0_12_0_multiplier_buf_2,output_24_0_buf,&output_loss_buffer,mars_kernel_0_12_0_sum_buf_3,mars_kernel_0_12_0_sum_buf_0,mars_kernel_0_12_0_sum_buf_1,mars_kernel_0_12_0_sum_buf_2,weights_13_0_buf);
     else if (mars_count_0_12_0 == 2) 
      mars_kernel_0_12_0(k0,0,upper_bound,mars_kernel_0_12_0_feature_buf_2,mars_kernel_0_12_0_feature_buf_3,mars_kernel_0_12_0_feature_buf_0,mars_kernel_0_12_0_feature_buf_1,feature_samples,feature_samples_9_0_buf,mars_kernel_0_12_0_label_buf_2,mars_kernel_0_12_0_label_buf_3,mars_kernel_0_12_0_label_buf_0,mars_kernel_0_12_0_label_buf_1,label_samples,label_samples_6_0_buf,mars_kernel_0_12_0_marginY_buf_2,mars_kernel_0_12_0_marginY_buf_3,mars_kernel_0_12_0_marginY_buf_0,mars_kernel_0_12_0_marginY_buf_1,mars_kernel_0_12_0_margin_buf_2,mars_kernel_0_12_0_margin_buf_3,mars_kernel_0_12_0_margin_buf_0,margins,mars_kernel_0_12_0_maxMarginIndex_buf_2,mars_kernel_0_12_0_maxMarginIndex_buf_3,mars_kernel_0_12_0_maxMarginIndex_buf_0,mars_kernel_0_12_0_maxMargin_buf_2,mars_kernel_0_12_0_maxMargin_buf_3,mars_kernel_0_12_0_maxMargin_buf_0,mars_kernel_0_12_0_maxMargin_buf_1,mars_kernel_0_12_0_multiplier_buf_2,mars_kernel_0_12_0_multiplier_buf_3,mars_kernel_0_12_0_multiplier_buf_0,mars_kernel_0_12_0_multiplier_buf_1,output_24_0_buf,&output_loss_buffer,mars_kernel_0_12_0_sum_buf_2,mars_kernel_0_12_0_sum_buf_3,mars_kernel_0_12_0_sum_buf_0,mars_kernel_0_12_0_sum_buf_1,weights_13_0_buf);
     else if (mars_count_0_12_0 == 3) 
      mars_kernel_0_12_0(k0,0,upper_bound,mars_kernel_0_12_0_feature_buf_1,mars_kernel_0_12_0_feature_buf_2,mars_kernel_0_12_0_feature_buf_3,mars_kernel_0_12_0_feature_buf_0,feature_samples,feature_samples_9_0_buf,mars_kernel_0_12_0_label_buf_1,mars_kernel_0_12_0_label_buf_2,mars_kernel_0_12_0_label_buf_3,mars_kernel_0_12_0_label_buf_0,label_samples,label_samples_6_0_buf,mars_kernel_0_12_0_marginY_buf_1,mars_kernel_0_12_0_marginY_buf_2,mars_kernel_0_12_0_marginY_buf_3,mars_kernel_0_12_0_marginY_buf_0,mars_kernel_0_12_0_margin_buf_1,mars_kernel_0_12_0_margin_buf_2,mars_kernel_0_12_0_margin_buf_3,margins,mars_kernel_0_12_0_maxMarginIndex_buf_1,mars_kernel_0_12_0_maxMarginIndex_buf_2,mars_kernel_0_12_0_maxMarginIndex_buf_3,mars_kernel_0_12_0_maxMargin_buf_1,mars_kernel_0_12_0_maxMargin_buf_2,mars_kernel_0_12_0_maxMargin_buf_3,mars_kernel_0_12_0_maxMargin_buf_0,mars_kernel_0_12_0_multiplier_buf_1,mars_kernel_0_12_0_multiplier_buf_2,mars_kernel_0_12_0_multiplier_buf_3,mars_kernel_0_12_0_multiplier_buf_0,output_24_0_buf,&output_loss_buffer,mars_kernel_0_12_0_sum_buf_1,mars_kernel_0_12_0_sum_buf_2,mars_kernel_0_12_0_sum_buf_3,mars_kernel_0_12_0_sum_buf_0,weights_13_0_buf);
    mars_count_0_12_0++;
    if (mars_count_0_12_0 == 4) 
      mars_count_0_12_0 = 0;
  }
  mars_kernel_0_12_0_assign_maxMargin_buf(maxMargin_buf,mars_kernel_0_12_0_maxMargin_buf_3);
  mars_kernel_0_12_0_assign_maxMargin_buf(maxMargin_buf,mars_kernel_0_12_0_maxMargin_buf_2);
  mars_kernel_0_12_0_assign_maxMargin_buf(maxMargin_buf,mars_kernel_0_12_0_maxMargin_buf_1);
  mars_kernel_0_12_0_assign_maxMargin_buf(maxMargin_buf,mars_kernel_0_12_0_maxMargin_buf_0);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(maxMarginIndex_buf,mars_kernel_0_12_0_maxMarginIndex_buf_3);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(maxMarginIndex_buf,mars_kernel_0_12_0_maxMarginIndex_buf_2);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(maxMarginIndex_buf,mars_kernel_0_12_0_maxMarginIndex_buf_1);
  mars_kernel_0_12_0_assign_maxMarginIndex_buf(maxMarginIndex_buf,mars_kernel_0_12_0_maxMarginIndex_buf_0);
  mars_kernel_0_12_0_assign_marginY_buf(marginY_buf,mars_kernel_0_12_0_marginY_buf_3);
  mars_kernel_0_12_0_assign_marginY_buf(marginY_buf,mars_kernel_0_12_0_marginY_buf_2);
  mars_kernel_0_12_0_assign_marginY_buf(marginY_buf,mars_kernel_0_12_0_marginY_buf_1);
  mars_kernel_0_12_0_assign_marginY_buf(marginY_buf,mars_kernel_0_12_0_marginY_buf_0);
  mars_kernel_0_12_0_assign_label_buf(label_buf,mars_kernel_0_12_0_label_buf_3);
  mars_kernel_0_12_0_assign_label_buf(label_buf,mars_kernel_0_12_0_label_buf_2);
  mars_kernel_0_12_0_assign_label_buf(label_buf,mars_kernel_0_12_0_label_buf_1);
  mars_kernel_0_12_0_assign_label_buf(label_buf,mars_kernel_0_12_0_label_buf_0);
  mars_kernel_0_12_0_assign_feature_buf(feature_buf,mars_kernel_0_12_0_feature_buf_3);
  mars_kernel_0_12_0_assign_feature_buf(feature_buf,mars_kernel_0_12_0_feature_buf_2);
  mars_kernel_0_12_0_assign_feature_buf(feature_buf,mars_kernel_0_12_0_feature_buf_1);
  mars_kernel_0_12_0_assign_feature_buf(feature_buf,mars_kernel_0_12_0_feature_buf_0);
  
#pragma HLS array_partition variable=output_24_0_buf cyclic factor = 8 dim=2
  ::memcpy_wide_bus_write_double_2d(((class ap_uint< 512 > *)output),output_24_0_buf, 0, 0, (sizeof(double ) * 0),(sizeof(double ) * ((unsigned long )7056)));
  memcpy_wide_bus_single_write_double(((class ap_uint< 512 > *)output),output_loss_buffer,(sizeof(double ) * ((10 - 1) * 784)));
}
