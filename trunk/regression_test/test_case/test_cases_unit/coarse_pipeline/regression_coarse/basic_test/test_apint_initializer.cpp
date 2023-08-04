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
#include <ap_int.h>
typedef class ap_uint< 8 > _DataType_0_1_name;

#pragma ACCEL kernel
void func_top_3_0(int bram_read_count)
{
  int i;
  int j;
  _DataType_0_1_name linebuffer[5][3840 >> 0];
  _DataType_0_1_name linebuffer1[5][3840 >> 0];
  for (j = 0; j < 100; j++) 
{
#pragma ACCEL pipeline
      _DataType_0_1_name packed_read1;
      _DataType_0_1_name packed_read2 = 1;
      for (i = 0; i < 5; i++) {
        packed_read1 = linebuffer[(long )i][bram_read_count];
      }
      for (i = 0; i < 5; i++) {
        packed_read2 = linebuffer1[(long )i][bram_read_count];
      }
  }
}


