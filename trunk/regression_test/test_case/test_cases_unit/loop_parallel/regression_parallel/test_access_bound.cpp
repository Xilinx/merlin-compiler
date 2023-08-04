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
int cc[10];

#pragma ACCEL kernel
void func_top_0(int in[50], int out[50], int m_return[1], int var) {
  int a[50];
  int i;
  int b[10];
  int c1[1];
  for (i = 0; i < 100; i++)
  {
#pragma ACCEL PARALLEL complete
    int aa[50];
    aa[i] = in[i];
    b[i>9?0:i] = cc[11];
  }
  for (i = 0; i < 100; i++)
  {
#pragma ACCEL PARALLEL complete
    c1[i] = a[i]++;
  }
}

