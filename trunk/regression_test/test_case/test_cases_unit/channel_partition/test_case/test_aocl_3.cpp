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
#include "merlin_stream.h" 
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=200 NODE=__msm_node_0 PORT=__msm_port_a_buf_0___msm_node_0_0 
merlin_stream ch_a_buf_0_2;
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=200 NODE=__msm_node_0 PORT=__msm_port_c_buf_0___msm_node_0_0 
merlin_stream ch_c_buf_0_0;
#include <string.h> 
#define TILE_FACTOR 100
#define UNROLL_FACTOR 5

void __msm_node_0(int i,int c_buf_0[5][20][100],int a_buf_0[5][20][100]);

void __msm_node_0(int i,int c_buf_0[5][20][100],int a_buf_0[5][20][100])
{
  int a_buf_0_sn_tmp;
  int c_buf_0_sn_tmp;
  int j0;
  int k0;
  int kk;
  for (j0 = 0; j0 < 100 / 5; ++j0) {
    for (k0 = 0; k0 < 10; ++k0) {
      for (kk = 0; kk < 10; ++kk) {
        
#pragma ACCEL parallel
        merlin_stream_read(&ch_a_buf_0_2,&a_buf_0_sn_tmp);
        c_buf_0_sn_tmp = a_buf_0_sn_tmp ;
        merlin_stream_write(&ch_c_buf_0_0,&c_buf_0_sn_tmp);
      }
    }
  }
}

void __msm_port_c_buf_0___msm_node_0_0(int c_buf_0[5][20][100],int jj)
{
  int c_buf_0_sp_tmp;
  int _jj;
  _jj = jj;
  int _j0;
  int _k0;
  int _kk;
  for (_j0 = 0; _j0 < 100 / 5; ++_j0) {
    for (_k0 = 0; _k0 < 10; ++_k0) {
      for (_kk = 0; _kk < 10; ++_kk) {
        
#pragma ACCEL parallel
        merlin_stream_read(&ch_c_buf_0_0,&c_buf_0_sp_tmp);
        c_buf_0[_jj][_j0][_kk + _k0 * 10] = c_buf_0_sp_tmp;
      }
    }
  }
}

void __msm_port_a_buf_0___msm_node_0_0(int a_buf_0[5][20][100],int jj)
{
  int a_buf_0_sp_tmp;
  int _jj;
  _jj = jj;
  int _j0;
  int _k0;
  int _kk;
  for (_j0 = 0; _j0 < 100 / 5; ++_j0) {
    for (_k0 = 0; _k0 < 10; ++_k0) {
      for (_kk = 0; _kk < 10; ++_kk) {
        
//#pragma ACCEL parallel
        a_buf_0_sp_tmp = a_buf_0[_jj][_j0][_kk + _k0 * 10];
        merlin_stream_write(&ch_a_buf_0_2,&a_buf_0_sp_tmp);
      }
    }
  }
}

#pragma ACCEL kernel

void top(int *a,int *b,int *c)
{
  for (int i = 0; i < 10000 / 100; ++i) {
    int c_buf_0[5][20][100];
    int a_buf_0[5][20][100];
    memcpy(((void *)(&a_buf_0[0][0][0])),((const void *)(&a[i * 10000])),sizeof(int ) * 10000);
    for (int jj = 0; jj < 5; ++jj) {
      
#pragma ACCEL parallel
      merlin_stream_init(&ch_c_buf_0_0,10000,4);
      merlin_stream_init(&ch_a_buf_0_2,10000,4);
      
#pragma ACCEL stream_port access_type=read dim_num=3 node=__msm_node_0 variable=a_buf_0 
      __msm_port_a_buf_0___msm_node_0_0(a_buf_0,jj);
  
#pragma ACCEL stream_node parallel_factor=5 
      __msm_node_0(i,c_buf_0,a_buf_0);
      merlin_stream_reset(&ch_a_buf_0_2);
      
#pragma ACCEL stream_port access_type=write dim_num=3 node=__msm_node_0 variable=c_buf_0 
      __msm_port_c_buf_0___msm_node_0_0(c_buf_0,jj);
      merlin_stream_reset(&ch_c_buf_0_0); 
    }
    memcpy(((void *)(&c[i * 10000])),((const void *)(&c_buf_0[0][0][0])),sizeof(int ) * 10000);
  }
}
