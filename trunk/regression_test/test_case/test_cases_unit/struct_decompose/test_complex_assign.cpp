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
struct st {
  int a;
};

void f(struct st a, struct st *b) { b[0] = a;}

#pragma ACCEL kernel
void top(struct st *a) {
#pragma ACCEL interface variable=a depth=10
  struct st b;
  struct st c;
  struct st d;
  b = c = d = a[0];

  a[1] = a[2] = c;

  f(b = c = d, a);
  return;

}
