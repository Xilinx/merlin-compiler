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
//test whether function 'f' has only two versions
void f(int *a) {
  *a = 1;
}

#pragma ACCEL kernel
void top(int *a) {
  f(&a[0]);
  f(&a[1]);
  int local;
  f(&local);
  int local2;
  f(&local2);
  int a_buf[2];
  f(&a_buf[0]);
  f(&a_buf[1]);
}

void f1(int *a) {
  *a = 1;
}

void f2(int *a) {
  f1(a);
  f1(a);
}

#pragma ACCEL kernel
void top2(int *a) {
  int local;
  f2(&local);
  int local2;
  f2(&local2);
  int a_buf[2];
  f2(&a_buf[0]);
}
