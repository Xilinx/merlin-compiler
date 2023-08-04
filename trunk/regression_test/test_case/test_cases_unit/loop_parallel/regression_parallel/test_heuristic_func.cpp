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
void update(int src_height,int src_width,unsigned char *Data,int dest_height,int dest_width,unsigned char IMG1_data[240][320]){
  int j;
  int i;
  for (i = 0; i < 240; i++) {
    for (j = 0; j < 320; j++) {
#pragma ACCEL parallel
        Data[(j * (1 + 20971520 / dest_width) / 65536) + (i * (1 + 15728640 / dest_height) / 65536) * 320L] += i;
        IMG1_data[i][j] = Data[(j * (1 + 20971520 / dest_width) / 65536) + (i * (1 + 15728640 / dest_height) / 65536) * 320L];
        IMG1_data[i][j] = Data[(j * (2 + 20971520 / dest_width) / 65536) + (i * (1 + 15728641 / dest_height) / 65536) * 320L];
    }
  }
}
// update1 is the copy of update
void update1(int src_height,int src_width,unsigned char *Data,int dest_height,int dest_width,unsigned char IMG1_data[240][320]){
  int j;
  int i;
  for (i = 0; i < 240; i++) {
    for (j = 0; j < 320; j++) {
#pragma ACCEL pipeline
        Data[(j * (1 + 20971520 / dest_width) / 65536) + (i * (1 + 15728640 / dest_height) / 65536) * 320L] += i;
        IMG1_data[i][j] = Data[(j * (1 + 20971520 / dest_width) / 65536) + (i * (1 + 15728640 / dest_height) / 65536) * 320L];
        IMG1_data[i][j] = Data[(j * (2 + 20971520 / dest_width) / 65536) + (i * (1 + 15728641 / dest_height) / 65536) * 320L];
    }
  }
}
#pragma ACCEL kernel
void func_top() {
  unsigned char IMG1_data[240][320];
  unsigned char Data_buf_0[76800];
  unsigned char Data_buf_1[2][76800];
  update(240,320,(unsigned char *)Data_buf_0,240,320,IMG1_data);
  update1(240,320,(unsigned char *)Data_buf_1[0],240,320,IMG1_data);
}
