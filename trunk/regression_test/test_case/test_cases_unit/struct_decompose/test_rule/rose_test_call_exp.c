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

struct f4 
{
  float f[4];
}
;

static void func_f4(float (*ret_var_f)[4],float x_f[4])
{
  int i_1_0;
  for (i_1_0 = 0; i_1_0 < 4UL; ++i_1_0) {
    ( *ret_var_f)[i_1_0] = x_f[i_1_0];
  }
  return ;
}
#pragma ACCEL kernel

void call_f4(float l_f4_f[4])
{
  float ret_var_f[4];
  func_f4(&ret_var_f,l_f4_f);
}
