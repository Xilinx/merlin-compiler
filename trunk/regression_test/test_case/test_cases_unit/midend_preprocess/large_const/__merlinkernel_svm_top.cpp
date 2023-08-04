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
#include <assert.h>
extern int __merlin_include__GB_merlin_type_define_h_1;
extern int __merlin_include__GB_assert_h_0;
 const int x[123456789] = {1,2,3};

static void svm_kernel(double rho,int nwindows,int nblocks_,int blockHistogramSize,const float hist_cache[4293UL * 105UL * 36UL],const float svmDetector[3781UL],double sums[4293UL])
{
  nwindows >= 0 && ((unsigned long )nwindows) <= 4293UL?((void )0) : __assert_fail("nwindows >= 0 && nwindows <= 4293UL","svm_kernel.cpp",(unsigned int )42,__PRETTY_FUNCTION__);
//assert(nblocks >= 0 && nblocks <= NBLOCKS);
  blockHistogramSize >= 0 && ((unsigned long )blockHistogramSize) <= 36UL?((void )0) : __assert_fail("blockHistogramSize >= 0 && blockHistogramSize <= 36UL","svm_kernel.cpp",(unsigned int )44,__PRETTY_FUNCTION__);
  const int nblocks = 105;
  for (unsigned long i = (unsigned long )0; i < 4293UL; i++) {
    if (i < ((unsigned long )nwindows)) {
      double shift_reg[12 + 1];
      
#pragma ACCEL parallel
      for (int j = 0; j < 12 + 1; j++) 
        shift_reg[j] = ((double )0);
      double s = (double )0;
      for (int jk = 0; ((unsigned long )jk) < ((unsigned long )nblocks) * 36UL; jk++) {
        ((unsigned long )jk) + i * 105UL * 36UL < 4293UL * 105UL * 36UL?((void )0) : __assert_fail("jk+i*105UL*36UL < 4293UL*105UL*36UL","svm_kernel.cpp",(unsigned int )58,__PRETTY_FUNCTION__);
        ((unsigned long )jk) < 3781UL?((void )0) : __assert_fail("jk < 3781UL","svm_kernel.cpp",(unsigned int )59,__PRETTY_FUNCTION__);
        double r = (double )(hist_cache[((unsigned long )jk) + i * 105UL * 36UL] * svmDetector[jk]);
//s += r;
        shift_reg[12] = shift_reg[0] + r;
        
#pragma ACCEL parallel
        for (int x = 0; x < 12; ++x) 
          shift_reg[x] = shift_reg[x + 1];
      }
      
#pragma ACCEL parallel
      for (int x = 0; x < 12; ++x) 
        s += shift_reg[x];
      sums[i] = s + rho + x[1];
    }
  }
}
#pragma ACCEL kernel

void svm_top(double rho_buf[34UL],int nwindows_buf[34UL],int nblocks_buf[34UL],int blockHistogramSize_buf[34UL],const float blockHist_buf0[4293UL * 105UL * 64 * 34UL / 2],const float blockHist_buf1[4293UL * 105UL * 64 * 34UL / 2],const float svm_buf[3781UL * 34UL],double sums[4293UL * 34UL])
{
 const int y = 1;
#pragma ACCEL interface variable=sums depth=145962
  
#pragma ACCEL interface variable=svm_buf depth=128554
  
#pragma ACCEL interface variable=blockHist_buf1 depth=490432320
  
#pragma ACCEL interface variable=blockHist_buf0 depth=490432320
  
#pragma ACCEL interface variable=blockHistogramSize_buf depth=34
  
#pragma ACCEL interface variable=nblocks_buf depth=34
  
#pragma ACCEL interface variable=nwindows_buf depth=34
  
#pragma ACCEL interface variable=rho_buf depth=34
  for (int n = 0; ((unsigned long )n) < 34UL / ((unsigned long )2); n++) {
    svm_kernel(rho_buf[n],nwindows_buf[n],nblocks_buf[n],blockHistogramSize_buf[n],&blockHist_buf0[((unsigned long )n) * 4293UL * 105UL * ((unsigned long )64)],&svm_buf[((unsigned long )n) * 3781UL],&sums[((unsigned long )n) * 4293UL]);
  }
  for (int n = (int )(34UL / ((unsigned long )2)); ((unsigned long )n) < 34UL; n++) {
    svm_kernel(rho_buf[n],nwindows_buf[n],nblocks_buf[n],blockHistogramSize_buf[n],&blockHist_buf1[(((unsigned long )n) - 34UL / ((unsigned long )2)) * 4293UL * 105UL * ((unsigned long )64)],&svm_buf[((unsigned long )n) * 3781UL],&sums[((unsigned long )n) * 4293UL]);
  }
}
