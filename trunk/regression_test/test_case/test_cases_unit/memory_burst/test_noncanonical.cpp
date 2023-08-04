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
#include<assert.h>

#pragma ACCEL kernel

void verify(int merlin_return_value[1],int *i,int *j,float *c,float *c2) {
#pragma ACCEL interface variable=c2 scope_type=global depth=1,1024,1024 max_depth=1,1024,1024
  
#pragma ACCEL interface variable=c scope_type=global depth=1,1024,1024 max_depth=1,1024,1024
  
#pragma ACCEL interface variable=j scope_type=global depth=1 max_depth=1
  
#pragma ACCEL interface variable=i scope_type=global depth=1 max_depth=1
 
  float sum = (float )0.0;
  float sum2 = (float )0.0;
  for ( *i = 0;  *i < 1024; ( *i)++) {
    for ( *j = 0;  *j < 1024; ( *j)++) {
      
#pragma ACCEL PIPELINE AUTO
      sum += c[( *i) * 1024L + ( *j)];
      sum2 += c2[( *i) * 1024L + ( *j)];
    }
  }
}


