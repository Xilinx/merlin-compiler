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
namespace ns {
  const int FC = 10;
  enum {DIM=1};
class A {
  int a[10];
  public:
  A() {
    // the following 'a' should be updated into "this_->a"
#pragma HLS array_patition variable=a factor=FC dim=DIM
    a[0] = 1;
  }
  int get(int i) {
    return a[i];
  }
  void set(int i, int val) {
    a[i] = val;
  }
};
}
enum _ram_type {
  RAM_1P_BRAM = 0,
};
#pragma ACCEL kernel
void top(int *a) {
// do not replace enumerator if its declaration is in the global scope
#pragma HLS interface variable=a core=RAM_1P_BRAM
  ns::A tmp;
  for (int i = 0; i < 10; ++i) {
    tmp.set(i, a[i]);
  }
}
