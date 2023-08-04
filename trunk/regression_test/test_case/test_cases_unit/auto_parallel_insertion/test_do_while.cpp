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
void foo_top_8() {
  int tmp;
  for (int j = 0; j < 64; j++) {
    tmp++;
  }
}

#pragma ACCEL kernel
void func_top_8()
{
  int k;
  int i;
  int j;
  for (int k = 0; k < 100; k++) { 
    int j = 0;
    do {
      j++;
      foo_top_8();
    } while (j < 500); 

    do {
#pragma ACCEL loop_tripcount max=500
      for (int i = 0; i < 15; i++) {
        j++;
      }
    } while (j < 7500);
  }
}
