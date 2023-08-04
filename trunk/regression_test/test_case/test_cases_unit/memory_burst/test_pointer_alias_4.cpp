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
void top(int *a, int *b) {
#pragma ACCEl interface variable=a depth=1000
#pragma ACCEl interface variable=b depth=1000
  int *a_ptr = &a[10];
  int *b_ptr = &b[10];
  for (int i = 0; i < 100; ++i) {
    a_ptr[i] = b_ptr[i] + 2;
  }
  if (b_ptr[100] > 0) {
    a_ptr[100] = 100;
  }
}
