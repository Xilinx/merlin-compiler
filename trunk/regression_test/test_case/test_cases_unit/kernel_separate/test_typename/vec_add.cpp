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
template<int N>
class B {
  public:
  typedef int name;
};
template<int N>
class A {
  public:
  typedef typename B<N>::name data_t;
  //data_t a;
  A(const A &a);
  A();
};

template<int N>
class BB {};
template<>
class BB<1> {
  public:
  typedef int name;
};
template<int N>
class AA {
  public:
  typedef typename BB<N>::name data_t;
  AA(const AA & t);
  AA();
};

#pragma ACCEL kernel
void top(A<1> a) {
   typename B<1>::name  b;
   AA<1> aa;
}
