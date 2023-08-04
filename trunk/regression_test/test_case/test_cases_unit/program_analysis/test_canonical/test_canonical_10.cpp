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
void foo_loop(int n) {
  int a[100];
  int k;
  k = 0;
  for (; k < 10; ++k) {
	a[k] = k;
  }

  unsigned short k1 = 0;
  for (; k1 < 10; ++k1) {
	a[k1] = k1;
  }
  unsigned k2 = 10;
  for (; k2 >= 0; --k2) {
	a[k2] = k2;
  }
  
  if (a[0] > 0)
    k = 10;
  else 
	k = 11;
  for (; k >= 0; --k) {
	a[k] = k;
  }
  char k6 = n;
  for (; k6 <= 10; ++k6) {
	a[k6] = k6;
  }
  unsigned char k3 = n;
  for (; k3 <= 10; ++k3) {
	a[k3] = k3;
  }
  unsigned long k4 = n;
  for (; k4 <= 10; ++k4) {
	a[k4] = k4;
  }
  signed char k5 = n;
  for (; k5 <= 10; ++k5) {
	a[k5] = k5;
  }
}
int main() {
  foo_loop(2);
  return 0;
}
