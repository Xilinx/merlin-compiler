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

void top(int *merlin_in_0,int *merlin_out_0,int *merlin_input,int *merlin_output,int *merlin_bit,int merlin_pos,int merlin_label,float *merlin_value,double *merlin_val,char *merlin_feature,char *merlin_image,short merlin_parameter,int merlin_in,int x)
{
  
#pragma ACCEL interface variable=merlin_in_0 depth=100
  
#pragma ACCEL false_dependence variable=merlin_output
  int merlin_out;
}
