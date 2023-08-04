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
/*Loop kernel from denoise, medical imaging*/

void denoise2D(int b[64][64])
{
  int a[64][64];
  int i;
  int j;
/*Array Access*/
  for (j = 1; j < 64; j++) {
    for (i = 1; i < 64; i++) {
      
#pragma ACCEL parallel factor=6
      b[j][i] = (a[j + 1][i] + a[j - 1][i] + a[j][i + 1] + a[j][i - 1]) / 4;
    }
  }
}
