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
void sub(int *aa) {
  aa[0] = 0;
}
int test(int a) { return a > 0 ; }
int test2(int b) { return b > 10; }
int test3(int c) { return c < 10; }
int test4(int d) { return 1; }
#pragma ACCEL kernel
void top(int *a) {
  sub(a);
  if (test(a[0])) {
  }
  for (int i = 0; i < test2(a[1]); ++i) {}
  while (test3(a[2])) {};
  do {} while (test4(a[3]));
}
