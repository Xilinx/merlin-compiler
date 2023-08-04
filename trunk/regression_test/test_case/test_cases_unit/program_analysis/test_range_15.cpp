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
void k(int sol[16384000]) {
#pragma ACCEL interface variable = sol max_depth = 16384000 depth = 16384000

  for (int i = 0; i < 32000 + 2; i++) {
    for (int j = 0; j < 32 + 2; j++) {
#pragma ACCEL PIPELINE
      if (j >= 1 && j <= 32 && i >= 1 && i <= 32000) {
        for (int k = 0; k < 16; k++) {
#pragma ACCEL PARALLEL COMPLETE
          sol[k + 16 * (j - 1 + 32 * (i - 1))];
        }
      }
    }
  }
}
