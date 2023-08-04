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
#include<ap_int.h>
struct st {
  ap_int<10> a;
  int b;
};
#pragma ACCEL kernel
void top(struct st *a, int i) {
#pragma ACCEL interface variable=a depth=10
  struct st arr[3] = {{10, 20}, {10, 20}, {10,20}};
  struct st arr2[10];
  a[i].a = arr[i].a + arr2[i].a;
  
  a[i].b = arr[i].b + arr2[i].b;
}
