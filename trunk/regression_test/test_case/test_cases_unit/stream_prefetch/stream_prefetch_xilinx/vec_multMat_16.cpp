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
static void cmult_16_1(signed char *in_re,signed char *in_im,signed char *coeff_re,signed char *coeff_im,signed short *out_re,signed short *out_im)
{
  signed short p_l;
  signed short p_h;
  signed short p_l2;
  signed short p_h2;
  signed long b;
  signed long p1;
  signed long p2;
//(a+jb)*re{c}
  b = ((signed long )(((long long )( *in_re)) + (((long long )( *in_im)) << 2 * 8)));
  p1 = ((long )( *coeff_re)) * b;
  p_h = ((signed short )((long )(p1 >> 2 * 8)));
  p_l = ((signed short )(p1 & ((long )((1 << 8 * 2) - 1))));
//(a+jb)*im{c}
  p2 = ((long )( *coeff_im)) * b;
  p_h2 = ((signed short )(((long )p_h) + (p2 & ((long )((1 << 8 * 2) - 1)))));
  p_l2 = ((signed short )(((long )p_l) - ((long )(p2 >> 2 * 8))));
   *out_im = p_h2;
   *out_re = p_l2;
}
#pragma ACCEL kernel

void multMat_16(signed char *in_re,signed char *in_im,signed char *coeff_re,signed char *coeff_im,signed short *mult_re,signed short *mult_im)
{
  
#pragma ACCEL interface variable=mult_im depth=3300,64 max_depth=3300,64
  
#pragma ACCEL interface variable=mult_re depth=3300,64 max_depth=3300,64
  
#pragma ACCEL interface variable=coeff_im depth=64,16 max_depth=64,16
  
#pragma ACCEL interface variable=coeff_re depth=64,16 max_depth=64,16
  
#pragma ACCEL interface variable=in_im depth=3300,16 max_depth=3300,16
  
#pragma ACCEL interface variable=in_re depth=3300,16 max_depth=3300,16
  int i;
  int j;
  int k;
// multiplying first and second matrices and storing in mult.
//#pragma ACCEL parallel factor=2
  for (i = 0; i < 3300; i++) 
{
    
#pragma ACCEL PIPELINE AUTO 
    for (j = 0; j < 64; j++) 
{
      
#pragma ACCEL PARALLEL COMPLETE 
      signed short tmp_im;
      signed short tmp_re;
      signed short res_im;
      signed short res_re;
      res_re = ((signed short )0);
      res_im = ((signed short )0);
      for (k = 0; k < 16; k++) 
{
        
#pragma ACCEL PARALLEL COMPLETE 
        cmult_16_1(&in_re[i * 16L + k],&in_im[i * 16L + k],&coeff_re[j * 16L + k],&coeff_im[j * 16L + k],&tmp_re,&tmp_im);
        res_re += ((int )tmp_re);
        res_im += ((int )tmp_im);
      }
      mult_re[i * 64L + j] = ((signed short )(((int )res_re) >> 9));
      mult_im[i * 64L + j] = ((signed short )(((int )res_im) >> 9));
    }
  }
}
