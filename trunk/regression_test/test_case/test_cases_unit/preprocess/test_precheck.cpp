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
void func_top_1(float Cout[224][224][256],float Cout1[224][224][256],float Cin[226][226][128],float weight[3][3][256][128])
{
  for (int r = 0; r < 224; r++) 
    for (int c = 0; c < 224; c++) 
      for (int p = 0; p < 3; p++) 
        for (int q = 0; q < 3; q++) {
#pragma ACCEL pipeline
          for (int i = 0; i < 128; i++) {
#pragma ACCEL parallel
            for (int o = 0; o < 256; o++) { {
              float one_sub = weight[p][q][o][i] * Cin[r + p][c + q][i];
#pragma ACCEL reduction variable=Cout
              Cout[r][c][o] += one_sub;
            }
            }
          }
        }
}
