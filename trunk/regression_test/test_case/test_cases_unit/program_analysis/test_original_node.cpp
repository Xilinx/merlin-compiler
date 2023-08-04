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
template<int I>
void f() {
  int a[I];
}

template<>
void f<2>() {
  int b[2];
}

template<int I> 
class A {
  public:
    int f2() { 
      int sum = 0;
      for (int i = 0; i < 100; ++i) {
        sum += i;
      }
      return sum;
    }
    template<int W>
      int f3(int A[10]) {
        int sum = 0;
L3: for (int j = 0; j < W; ++j) {
      sum += A[j];
    }
    return sum;
      }
};
class B {
  public:
    template<int I>
      int f4() {
        int sum = 0;
        for (int k = 0; k < I; ++k) {
          sum += k;
        }
        return sum;
      }
};

void top(int c[10]) {
  f<3>();
  f<2>();
  A<2> a;
  a.f2();
  a.f3<10>(c);
  B b;
  b.f4<20>();
}

  
