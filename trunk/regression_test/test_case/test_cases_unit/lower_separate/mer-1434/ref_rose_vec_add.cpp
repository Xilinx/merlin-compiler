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
const unsigned int s1 = (10 * 4);
const unsigned int s2 = s1 * 2 + (10 / 2);

void sub(int a[40][85])
{
}

void sub_2(int b[10][20])
{
}
static void __merlin_dummy_kernel_pragma();

void top(int *a)
{
  int a_buf[40][85];
  int b_buf[10][20];
  sub(a_buf);
  sub_2(b_buf);
  return ;
}
