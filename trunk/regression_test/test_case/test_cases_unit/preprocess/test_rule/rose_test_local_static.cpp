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
static int sub_i = 0;

void sub(int *a)
{
  a[sub_i++] = 2;
}
static int top_i = 0;
static int top_i_0 = 100;
#pragma ACCEL kernel

void top(int *a)
{
{
    a[++top_i] = 0;
  }
{
    a[--top_i_0] = 1;
  }
  sub(a);
}
static int top2_s0 = 1;
static int top2_s1 = top2_s0 + 1;
static int top2_s2 = top2_s1 + top2_s0;
#pragma ACCEL kernel

void top2(int *a)
{
  int dim = top2_s0 + top2_s1 + top2_s2;
}

int sub3()
{
  return 10;
}
static int top3_s0 = sub3();
#pragma ACCEL kernel

void top3(int *a)
{
  int dim = top3_s0 * 2;
}
static int top4_s0 = 1;
static int top4_s1_flag;
static int top4_s1;
#pragma ACCEL kernel

void top4(int *a)
{
  int b = 10;
  if (!top4_s1_flag) {
    top4_s1 = top4_s0 + b;
    top4_s1_flag = 1;
  }
  int dim = top4_s0 * 2 + top4_s1 * 2;
}
static int top5_s0_flag;
static int top5_s0;
static int top5_s1_flag;
static int top5_s1;
#pragma ACCEL kernel

void top5(int *a)
{
  int b =  *a;
  int c =  *(a + 1);
  if (!top5_s0_flag) {
    top5_s0 = b;
    top5_s0_flag = 1;
  }
  if (!top5_s1_flag) {
    top5_s1 = top5_s0 + c;
    top5_s1_flag = 1;
  }
  int dim = top5_s0 * 2 + top5_s1 * 2;
}
