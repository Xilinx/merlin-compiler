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

int sub_0(int a[100],int b[100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL PARALLEL COMPLETE
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL PARALLEL COMPLETE
      a[i] = b[i];
    }
  }
  return 1024;
}
#pragma ACCEL kernel

void func_top_9()
{
  int a[100][100];
  int b[100][100];
  int i = 0;
  while(i < 100)
// Original: #pragma ACCEL PIPELINE FLATTEN II=2
{
    
#pragma ACCEL PIPELINE II=2 
    sub_0(a[i],b[i]);
    i++;
  }
}
