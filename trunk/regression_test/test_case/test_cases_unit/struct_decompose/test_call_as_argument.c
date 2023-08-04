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
struct st0 {
  int b;
  char a;
};
struct st {
  int a[1];
  struct st0 b;
};

int f(struct st a, int *b, int c) {
  b[0] = a.b.b;
  return c;
}

#pragma ACCEL kernel
void top(int *b) {
#pragma ACCEL interface variable=b depth=1
  struct st data[1][1] = {{{{1}, {2}}}};
  f(data[0][0], b, f(data[0][0], b, 0));
  return;

}
