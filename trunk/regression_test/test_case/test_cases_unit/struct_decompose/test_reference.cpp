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

class Test {
	public:
  int flag;
  short accu;
};

void sub(Test *a) {
  a->flag++;
  a->accu++;
}

void sub2(Test &a) {
  a.flag++;
  a.accu++;
}

#pragma ACCEL kernel
void top(Test &a, int *arr) {
#pragma ACCEL interface variable=a depth=100
	int i;
  Test &tmp = a;
  for (i = 0; i < N; ++i) {
	  if (1 == a.flag) {
		  tmp.accu += arr[i];
	  }
	  else {
		  tmp.accu -= arr[i];
	  }
  }
  sub(&a);
  sub2(a);
  return;
}

int main() {
	int i;
  Test a;
  int arr[N];
  for (i = 0; i < N; ++i) {
	  arr[N] = i;
  }
  a.flag = 1;
  a.accu = 0;
  top(a, arr);

  return 0;
}
