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

#pragma ACCEL kernel

void svm_top(double rho,int nwindows_buf[34L / 2],float hist_cache_buf[34L / 2 * 4352L * 105L * 36L],float svmDetector_buf[3780L * 34L / 2],double sums_buf[4352L * 34L / 2])
{
  
#pragma ACCEL interface variable=sums_buf depth=73984
  
#pragma ACCEL interface variable=svmDetector_buf depth=64260
  
#pragma ACCEL interface variable=hist_cache_buf depth=279659520
  
#pragma ACCEL interface variable=nwindows_buf depth=17
  
#pragma ACCEL interface variable=hist_cache stream_prefetch=on
  for (int n = 0; ((long )n) < 34L / ((long )2); n++) {
     ;
  }
}
