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

const int N = 100;

class Child {
	public:
	int a;
	char b;
} g_c;

class Parent {
	public:
  int a;
  char b;
  Child l_c;
};

#pragma ACCEL kernel
void top(Parent *a) {
#pragma ACCEL interface variable=a depth=100
	int i;

  for (i = 0; i < N; ++i) {
    a[i].a = a[i].a + a[i].l_c.a;
	a[i].b = a[i].b + a[i].l_c.b;
  }
  return;
}

int main() {
	int i;
  Parent *a = new Parent[N];

  for (i = 0; i < N; ++i) {
    a[i].a = i;
	a[i].b = i + '0';
	a[i].l_c.a = i + 1;
	a[i].l_c.b = i + '1';
  }
  top(a);

  delete [] a;

  return 0;

}
