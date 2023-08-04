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
struct S {
  int y;
  S(): y(100) {
  }
};

#pragma ACCEL kernel
int top() 
{
  static S s;
  static S s1[10];
  static S s2[2][3];
  S s3;
  S s4[10];
  S s5[2][3];
  return 0;
}

struct SS {
  int y;
  void set(int a) {
    y = a;
  }
};

#pragma ACCEL kernel
int top2()
{
  static SS s;
  static SS s1[10];
  static SS s2[1][2];
  SS s3;
  SS s4[10];
  SS s5[1][2];
  return 0;
}
