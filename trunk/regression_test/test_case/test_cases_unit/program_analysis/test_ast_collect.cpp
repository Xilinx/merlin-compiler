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
int a;
class b {
  int c;
};


template<int N> 
void f(int a[N]) {}

template<int N>
class d {
  int e[N];
};

#pragma ACCEL kernel
void foo(int *a, int b = 10) {
#pragma hls INLINE
  int c = 100;
  c += 10;
  int d[10];
  f<10>(d);
  b > 0 ? a[0] : a[1];
  while(true) {}
  for (;;) {}
  do {
  } while (1);
  return;
}



