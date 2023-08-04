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
void func_top_0(int a[100][100], int sum)
{
  int k;
  int j;
#pragma ACCEL parallel factor=5 reduction=sum
  for (k = 0; k < 100; k++) {
    for (j = 0; j < 100; j++)
    {
      sum += a[k][j];
    }
  }
#pragma ACCEL pipeline flatten reduction=sum reduction_scheme=cyclic
  for (k = 0; k < 100; k++) {
    for (j = 0; j < 100; j++)
    {
      sum += a[k][j];
    }
  }

  for (k = 0; k < 100; k++) {
#pragma ACCEL parallel complete
#pragma ACCEL reduction variable=sum
    for (j = 0; j < 100; j++)
    {
      sum += a[k][j];
    }
  }
}


#pragma ACCEL kernel
void func_top_1(int a[100][100], int b)
{
  int k;
  int j;

#pragma ACCEL pipeline flatten
#pragma ACCEL false_dependence variable=a
  for (k = 0; k < 100; k++) {
    for (j = 0; j < 100; j++)
    {
      a[k][j] = b;
    }
  }

}

#pragma ACCEL kernel
void func_top_2(int a[100][100], int sum)
{
  int k;
  int j;
#pragma ACCEL parallel factor=5 reduction=sum line_buffer=a
  for (k = 0; k < 99; k++) {
    for (j = 0; j < 99; j++)
    {
      sum += a[k][j] + a[k][j+1] + a[k+1][j];
    }
  }
#pragma ACCEL pipeline flatten line_buffer=a
  for (k = 0; k < 100; k++) {
    for (j = 0; j < 100; j++)
    {
      sum = a[k][j] + a[k][j+1] + a[k+1][j];
    }
  }
#pragma ACCEL pipeline flatten 
  for (k = 0; k < 100; k++) {
#pragma ACCEL line_buffer variable=a
    for (j = 0; j < 100; j++)
    {
      sum = a[k][j] + a[k][j+1] + a[k+1][j];
    }
  }


}

