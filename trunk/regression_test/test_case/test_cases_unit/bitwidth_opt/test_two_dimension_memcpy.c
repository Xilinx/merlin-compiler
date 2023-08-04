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
#include <string.h> 
static int add_1(int a,int b)
{
  return a + b;
}
const int DIM2 = 4;
#pragma ACCEL kernel 

void top_kernel(int aa[1][8],int bb[4],int *c)
{
  int bb_buf[4];
  int c_buf[4];
  
#pragma ACCEL interface variable=aa depth=2,4 max_depth=2,4
  
#pragma ACCEL interface variable=bb depth=4 max_depth=4
  
#pragma ACCEL interface variable=c depth=4 max_depth=4
  int i;
  int tmp[4];
  memcpy((void *)(&bb_buf[0]),(const void *)(&bb[0]),sizeof(int ) * 4);
  for (i = 0; i < 4; ++i) {
    
#pragma ACCEL PARALLEL COMPLETE
    memcpy((void *)tmp,(const void *)(&aa[0]),((unsigned long )4) * 4);
    c_buf[i] = add_1(tmp[i],bb_buf[i]);
  }
  memcpy((void *)(&c[0]),(const void *)(&c_buf[0]),sizeof(int ) * 4);
  return ;
}
