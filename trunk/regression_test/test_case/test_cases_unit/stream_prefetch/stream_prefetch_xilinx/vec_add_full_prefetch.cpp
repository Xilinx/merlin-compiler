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
#include<hls_stream.h>
static class hls::stream< int  > top_a_ch[4];

void top_a_streaming(int *a)
{
  
#pragma HLS array_partition variable=top_a_ch complete dim=1
  
#pragma HLS INLINE off
  long merlin_it_0;
  long merlin_it_1_0;
  for (merlin_it_0 = 0L; merlin_it_0 < 625L; ++merlin_it_0) 
{
    for (merlin_it_1_0 = 0L; merlin_it_1_0 < 4; ++merlin_it_1_0) 
{
      long merlin_it_2;
      for (merlin_it_2 = 0L; merlin_it_2 < 4; ++merlin_it_2) {
        top_a_ch[merlin_it_2] . write(a[0 + (merlin_it_2 + (merlin_it_1_0 * 4 + merlin_it_0 * 16))]);
      }
    }
  }
}
static class hls::stream< int  > top_b_ch[4];

void top_b_streaming(int *b)
{
  
#pragma HLS array_partition variable=top_b_ch complete dim=1
  
#pragma HLS INLINE off
  long merlin_it_0;
  long merlin_it_1_0;
  for (merlin_it_0 = 0L; merlin_it_0 < 625L; ++merlin_it_0) 
{
    for (merlin_it_1_0 = 0L; merlin_it_1_0 < 4; ++merlin_it_1_0) 
{
      long merlin_it_2;
      for (merlin_it_2 = 0L; merlin_it_2 < 4; ++merlin_it_2) {
        top_b_ch[merlin_it_2] . write(b[0 + (merlin_it_2 + (merlin_it_1_0 * 4 + merlin_it_0 * 16))]);
      }
    }
  }
}
static class hls::stream< int  > top_c_ch[4];

void top_c_streaming(int *c)
{
  
#pragma HLS array_partition variable=top_c_ch complete dim=1
  
#pragma HLS INLINE off
  long merlin_it_0;
  long merlin_it_1_0;
  for (merlin_it_0 = 0L; merlin_it_0 < 625L; ++merlin_it_0) 
{
    for (merlin_it_1_0 = 0L; merlin_it_1_0 < 4; ++merlin_it_1_0) 
{
      long merlin_it_2;
      for (merlin_it_2 = 0L; merlin_it_2 < 4; ++merlin_it_2) {
        c[0 + (merlin_it_2 + (merlin_it_1_0 * 4 + merlin_it_0 * 16))] = top_c_ch[merlin_it_2] . read();
      }
    }
  }
}

void computation_top(int *a,int *b,int *c)
{
  
#pragma HLS inline off
  for (int i = 0; i < 10000 / 4; ++i) 
{
    
#pragma HLS pipeline
    for (int i0 = 0; i0 < 4; ++i0) 
{
      
#pragma HLS unroll
      top_c_ch[i0] . write((top_a_ch[i0] . read() + top_b_ch[i0] . read()));
    }
  }
}

#pragma ACCEL kernel
void top(int *a,int *b,int *c)
{
  
#pragma ACCEL interface variable=a depth=10000 max_depth=10000 
  
#pragma ACCEL interface variable=b depth=10000 max_depth=10000 
  
#pragma ACCEL interface variable=c depth=10000 max_depth=10000 
  
#pragma HLS dataflow
  
#pragma HLS stream variable=top_a_ch depth=1
  top_a_streaming(a);
  
#pragma HLS stream variable=top_b_ch depth=1
  top_b_streaming(b);
  computation_top(a,b,c);
  
#pragma HLS stream variable=top_c_ch depth=1
  top_c_streaming(c);
}
