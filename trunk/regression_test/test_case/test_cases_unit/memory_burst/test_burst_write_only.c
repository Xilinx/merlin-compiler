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
// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes

#pragma ACCEL kernel
void func_top_0(int *a,int *b)
{
#pragma ACCEL interface variable=a write_only 
  int i;
  for (i = 0; i <  100; i += 2) {
    
#pragma ACCEL pipeline_parallel factor = 10
    a[i] = b[i];
  }
}
#pragma ACCEL kernel
void func_top_1(int *a,int *b)
{
  int i;
  for (i = 0; i <  100; i+= 2) {
    
#pragma ACCEL pipeline
    a[i] = b[i];
  }
}
void sub(int *a) {
#pragma ACCEL cache
  for (int i = 0; i < 100; ++i) {
    a[i]++;
  }
}
#pragma ACCEL kernel
void func_top_2(int *a, int *b) {
#pragma ACCEL interface variable=a write_only
  int i = 0;
  for(int i = 0; i < 100;++i) {
    a[i] = b[i];
  }
  sub(a); 
}
int main()
{
  int *a;
  int *b;
  int *c;
  int *aa;
  int *bb;
  int *cc;
  int a_s;
  int b_s;
  int c_s;
  

  func_top_0(a, b);
  

  func_top_1(a, b);
  
  return 0;
}
