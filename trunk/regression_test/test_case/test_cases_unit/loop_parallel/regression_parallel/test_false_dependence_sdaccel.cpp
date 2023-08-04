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
void sub_0 (int b[100][100]) {
  for (int i = 0; i < 100; i++)
  {
    for (int k = 0; k < 100; k++)
    {
#pragma ACCEL pipeline
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a
       b[i][k]=0;
    }
  }
}
#pragma ACCEL kernel
void func_top_0() {
  int a[100][100];
  int b[101][100];
  int c[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++)
  {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a
    for (k = 0; k < 10; k++)
    {
#pragma ACCEL PARALLEL COMPLETE
      for (j = 0; j < 99; j++)
      {
#pragma ACCEL PARALLEL COMPLETE
        a[i][j] = b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
  sub_0(c);
}
#pragma ACCEL kernel
void func_top_1() {
  int a[100][100];
  int b[102][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++)
  {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a
    for (k = 0; k < 10; k++) {
#pragma ACCEL PARALLEL COMPLETE
      for (j = 0; j < 99; j++) {
#pragma ACCEL PARALLEL COMPLETE
        a[k][j] += b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
}
#pragma ACCEL kernel
void func_top_2() {
  int a[100][100];
  int b[101][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++)
  {
#pragma ACCEL FALSE_DEPENDENCE
    for (k = 0; k < 10; k++) {
#pragma ACCEL PARALLEL COMPLETE
      for (j = 0; j < 99; j++) {
#pragma ACCEL PARALLEL COMPLETE
        a[i][j] = b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
}
#pragma ACCEL kernel
void func_top_3() {
  int a[100][100];
  int b[100][100];
  int k;
  int j;

  for (int i = 0; i < 10; i++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE = a_buf FORCE=off
    for (k = 0; k < 99; k++)
    {
#pragma ACCEL PIPELINE
      for (j = 0; j < 99; j++) {
#pragma ACCEL PARALLEL COMPLETE
        a[k][j] += b[k][j] + b[k + 1][j + 1];
      }
    }
  }
}
#pragma ACCEL kernel
void func_top_4() {
  int a[100][100];
  int bb[100][100];
  int k;
  int j;
  for (int i = 0; i < 10; i++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE = a
#pragma ACCEL FALSE_DEPENDENCE VARIABLE = bb
    for (k = 0; k < 9; k++) {
      for (j = 0; j < 99; j++)
      {

#pragma ACCEL PIPELINE
        a[k][j] += bb[i*k][j] + bb[i*k + 1][j + 1];
        bb[i*k][j] += a[k][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_5(int a[100]) {
  int j;
  for (j = 0; j < 99; j++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE = a
#pragma ACCEL PIPELINE

    a[j + 1] = a[j] + a[j + 1];
  }
  for (j = 0; j < 99; j++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE = a force = on
#pragma ACCEL PIPELINE

    a[j + 1] = a[j] + a[j + 1];
  }
}

// False dependency with alias
// FIXME: CMarsExpression cannot reconganize
// the alias access
#pragma ACCEL kernel
void func_top_6(int a[100]) {
  int j;
  int *b = &a[0];
  int *bb = a;
  for (j = 0; j < 99; j++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a
#pragma ACCEL PIPELINE
    b[j + 1] = a[j] + a[j + 1];
  }
  for (j = 0; j < 99; j++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=b
#pragma ACCEL PIPELINE
    b[j + 1] = b[j] + b[j + 1];
  }
  for (j = 0; j < 99; j++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a
#pragma ACCEL PIPELINE
    b[j + 1] = b[j] + b[j + 1];
  }
  for (j = 0; j < 99; j++) {
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a
#pragma ACCEL PIPELINE
    bb[j + 1] = a[j] + a[j + 1];
  }
}

#pragma ACCEL kernel
void func_top_7() {
  int b[100][100];
  int k;
  int i;
  int j;
  int sum=0;
  for (i = 0; i < 100; i++)
  {
#pragma ACCEL pipeline
    int a00[100][100];
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a00
    for (j = 0; j < 100; j++)
    {
#pragma ACCEL PARALLEL COMPLETE
      a00[i][j] = b[i][j];
    }
    sum+=a00[i][j];
  }
}
