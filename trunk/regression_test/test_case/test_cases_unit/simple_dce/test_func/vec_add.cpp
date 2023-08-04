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
void sub1(int *a, int n) {}
static void sub3(int *a, int n) {}
static void sub2(int *a, int n) {
  sub3(a, n);
}
void sub4(int *a, int n) {}
void sub(int *a, int n) {
  sub(a, n);
  if (0 == 0) {
    a[0] = 0;
  } else {
    sub4(a, n);
  }

  if (0 != 0) {
    sub1(a, n);
  }

  if (0 == 0) {
    a[2] = 2;
  } else {
    sub2(a, n);
  }

}

#pragma ACCEL kernel
void top(int *a, int n) {
  sub(a, n);
  if (0 == 0) {
    sub1(a, n);
  }
}
