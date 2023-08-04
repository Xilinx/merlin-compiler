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

int add_func_2(int a[2], int *v1, int *v2) {
  *v1 = a[0]; 
  *v2 = a[1]; 
}
#pragma ACCEL kernel
void func_top_0(int a1[2]) {
  int a[2];
  int v1;
  int v2;
  for (int i = 0; i < 2; i++) {
#pragma ACCEL pipeline
    for (int j = 0; j < 2; j++) {
#pragma ACCEL PARALLEL COMPLETE
      a[j] = a1[j];
    }
    add_func_2(a, &v1, &v2);
    a1[0] = v1 + v2;
  }
}

