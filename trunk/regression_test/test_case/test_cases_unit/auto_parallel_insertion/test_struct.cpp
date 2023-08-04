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
struct st {
  int data[4];
};


#pragma ACCEL kernel
void func_top_0() {
  st T1;
  int i,j,k;
  int sum = 0;
  for (i = 0; i < 20000; i++) {
    for (k = 0; k < 32; k++) {
      for (j = 0; j < 4; j++) {
        T1.data[j] = j;
      }
    }
  }
  for (j = 0; j < 4; j++) {
    sum += T1.data[j];
  }
  st T2;
  for (j = 0; j < 4; j++) {
    T2.data[j] = sum;
  }
}
