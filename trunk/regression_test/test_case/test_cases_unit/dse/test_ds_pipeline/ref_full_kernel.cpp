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

void vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  
#pragma ACCEL interface variable=c max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=b max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=a max_depth=10000 depth=10000
  int i;
  int j;
  int k;
  int m;
  int n;
  int p;
  int q;
  for (i = 0; i < 10000; i++) {
    
#pragma ACCEL PIPELINE auto{options:I0=[x for x in ["off","","flatten"]]; order:0 if x!="flatten" else 1; default:"off"}
    for (j = 0; j < 512; j++) {
      
#pragma ACCEL PIPELINE auto{options:I0_0=[x for x in ["off","","flatten"] if x=="off" or I0!="flatten"]; order:0 if x!="flatten" else 1; default:"off"}
      for (k = 0; k < 751; k++) {
        
#pragma ACCEL PIPELINE auto{options:I0_0_0=[x for x in ["off","","flatten"] if x=="off" or I0_0!="flatten" and I0!="flatten"]; order:0 if x!="flatten" else 1; default:"off"}
        for (m = 0; m < 127; m++) {
// Test if pipeline is ignored due to fine-grained
          c[i] = a[j] + b[k + m];
        }
      }
    }
    if (i > 0) {
// Test if-condition
      for (n = 0; n < 511; n++) {
        
#pragma ACCEL PIPELINE auto{options:I0_1_0=[x for x in ["off","","flatten"] if x=="off" or I0!="flatten"]; order:0 if x!="flatten" else 1; default:"off"}
        for (p = 0; p < 687; p++) {
          c[n] = a[n] + b[p];
        }
      }
      for (q = 0; q < 35; q++) {
        c[q] += q;
      }
    }
  }
// Test if pipeline is ignored due to conflict
  for (j = 0; j < 10000; j++) {
    
#pragma ACCEL PIPELINE
    c[j] += a[j] + b[j] + inc;
  }
}