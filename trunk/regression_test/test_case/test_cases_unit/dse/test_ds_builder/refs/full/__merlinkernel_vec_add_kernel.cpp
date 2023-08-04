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
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void foo(int a[10000],int b[10000],int c[10000])
{
  int n;
  int p;
  for (n = 0; n < 511; n++) {
    
#pragma ACCEL PIPELINE auto{options:I2=[x for x in ["off","","flatten"]]; order:0 if x!="flatten" else 1; default:"off"}
    
#pragma ACCEL TILE FACTOR=auto{options:T2=[x for x in [1,2,4,8,16,32,64,128,256,511] ]; default:1}
    
#pragma ACCEL PARALLEL FACTOR=auto{options:L2=[x for x in range(1,10)+[16,32,64,128] if x*T2<=511]; default:1}
    for (p = 0; p < 687; p++) {
      
#pragma ACCEL PARALLEL FACTOR=auto{options:L2_0=[x for x in range(1,10)+[16,32,64,128] if x==1 or I2!="flatten"]; default:1}
      c[n] = a[n] + b[p];
    }
  }
  return ;
}

void vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  
#pragma ACCEL interface variable=c bus_bitwidth=auto{options:bw2=[32,512]; default:32}
  
#pragma ACCEL interface variable=b bus_bitwidth=auto{options:bw1=[32,512]; default:32}
  
#pragma ACCEL interface variable=a bus_bitwidth=auto{options:bw0=[32,512]; default:32}
  __merlin_access_range(c,0,9999UL);
  __merlin_access_range(b,0,9999UL);
  __merlin_access_range(a,0,9999UL);
  
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
    
#pragma ACCEL TILE FACTOR=auto{options:T0=[x for x in [1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,10000] if x==1 or T0_0==1]; default:1}
    
#pragma ACCEL PARALLEL FACTOR=auto{options:L0=[x for x in range(1,10)+[16,32,64,128] if x*T0<=10000]; default:1}
    for (j = 0; j < 512; j++) {
      
#pragma ACCEL PIPELINE auto{options:I0_0=[x for x in ["off","","flatten"] if x=="off" or I0!="flatten"]; order:0 if x!="flatten" else 1; default:"off"}
      
#pragma ACCEL TILE FACTOR=auto{options:T0_0=[x for x in [1,2,4,8,16,32,64,128,256] if x==1 or T0==1 and I0!="flatten"]; default:1}
      
#pragma ACCEL PARALLEL FACTOR=auto{options:L0_0=[x for x in range(1,10)+[16,32,64,128] if x*T0_0<=512 and (x==1 or I0!="flatten")]; default:1}
      foo(a,b,c);
    }
    foo(a,b,c);
    for (q = 0; q < 35; q++) {
      
#pragma ACCEL PARALLEL FACTOR=auto{options:L0_1=[x for x in range(1,10)+[16,32,35] if x==1 or I0!="flatten"]; default:1}
      c[q] += q;
    }
  }
  for (i = 0; i < 10000; i++) {
    
#pragma ACCEL PIPELINE auto{options:I1=[x for x in ["off","","flatten"]]; order:0 if x!="flatten" else 1; default:"off"}
    
#pragma ACCEL TILE FACTOR=auto{options:T1=[x for x in [1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,10000] if x==1 or T1_0==1 and T1_0_0==1]; default:1}
    
#pragma ACCEL PARALLEL FACTOR=auto{options:L1=[x for x in range(1,10)+[16,32,64,128] if x*T1<=10000]; default:1}
    for (j = 0; j < 512; j++) {
      
#pragma ACCEL PIPELINE auto{options:I1_0=[x for x in ["off","","flatten"] if x=="off" or I1!="flatten"]; order:0 if x!="flatten" else 1; default:"off"}
      
#pragma ACCEL TILE FACTOR=auto{options:T1_0=[x for x in [1,2,4,8,16,32,64,128,256] if x==1 or T1==1 and T1_0_0==1 and I1!="flatten"]; default:1}
      
#pragma ACCEL PARALLEL FACTOR=auto{options:L1_0=[x for x in range(1,10)+[16,32,64,128] if x*T1_0<=512 and (x==1 or I1!="flatten")]; default:1}
      for (k = 0; k < 751; k++) {
        
#pragma ACCEL PIPELINE auto{options:I1_0_0=[x for x in ["off","","flatten"] if x=="off" or I1_0!="flatten" and I1!="flatten"]; order:0 if x!="flatten" else 1; default:"off"}
        
#pragma ACCEL TILE FACTOR=auto{options:T1_0_0=[x for x in [1,2,4,8,16,32,64,128,256,512,751] if x==1 or T1==1 and T1_0==1 and I1_0!="flatten" and I1!="flatten"]; default:1}
        
#pragma ACCEL PARALLEL FACTOR=auto{options:L1_0_0=[x for x in range(1,10)+[16,32,64,128] if x*T1_0_0<=751 and (x==1 or I1_0!="flatten" and I1!="flatten")]; default:1}
        if (k < 300) {
          for (m = 0; m < 127; m++) {
            
#pragma ACCEL PARALLEL FACTOR=auto{options:L1_0_0_0_0=[x for x in range(1,10)+[16,32,64,127] if x==1 or I1_0_0!="flatten" and I1_0!="flatten" and I1!="flatten"]; default:1}
            c[i] = a[j] + b[k + m];
          }
        }
         else {
          for (n = 0; n < 63; n++) {
            
#pragma ACCEL PARALLEL FACTOR=auto{options:L1_0_0_0_1=[x for x in range(1,10)+[16,32,63] if x==1 or I1_0_0!="flatten" and I1_0!="flatten" and I1!="flatten"]; default:1}
            c[i] = a[j] + b[k + n];
          }
        }
      }
    }
  }
}
