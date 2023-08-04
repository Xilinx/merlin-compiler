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
  int a[12];
};
struct st2 {
  int *a;
  int b;
#pragma ACCEL attribute variable=a depth=b max_depth=10
};

struct st3 {
  int *a[7];
#pragma ACCEL attribute variable=a depth=,3 
};

#pragma ACCEL kernel
void top(struct st (*a)[10][11], struct st (*b)[8][9], int n, struct st *c[8]) {
#pragma ACCEL interface variable=a depth=1
#pragma ACCEL interface variable=b depth=n max_depth=10
#pragma ACCEL interface variable=c depth=,n max_depth=,10
  a[0][1][2].a[3] = 0;
  b[0][0][0].a[2] = 1;
  c[0][0].a[1] = 2;
}

#pragma ACCEL kernel
void top2(struct st2 (*a)[10][11], struct st2 (*b)[8][9], int n, struct st2 *c[8]) {
#pragma ACCEL interface variable=a depth=1
#pragma ACCEL interface variable=b depth=n max_depth=10
#pragma ACCEL interface variable=c depth=,n max_depth=,10
  a[0][1][2].a[3] = 0;
  b[0][0][0].a[2] = 1;
  c[0][0].a[1] = 2;
}

#pragma ACCEL kernel
void top3(struct st3 (*a)[10][11], struct st3 (*b)[8][9], int n, struct st3 *c[8]) {
#pragma ACCEL interface variable=a depth=1
#pragma ACCEL interface variable=b depth=n max_depth=10
#pragma ACCEL interface variable=c depth=,n max_depth=,10
  a[0][1][2].a[3][0] = 0;
  b[0][0][0].a[2][1] = 1;
  c[0][0].a[1][2] = 2;
}
