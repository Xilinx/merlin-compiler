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
#include<stdio.h>
void foo_loop(int n) {
  int a[100];
  int k;
  k = 0;
  for (; k < 10; ++k) {
	a[k] = k;
	printf("index = %d\n", k);
  }

  k = 10;
  for (; k >= 0; --k) {
	a[k] = k;
	printf("index = %d\n", k);
  }
  if (a[0] > 0)
    k = 10;
  else 
	k = 11;
  for (; k >= 0; --k) {
	a[k] = k;
	printf("index = %d\n", k);
  }
  k = n;
  for (; k <= 10; ++k) {
	a[k] = k;
	printf("index = %d\n", k);
  }

  for (unsigned int i = 100; i >= 1; --i) {
    unsigned int ic = i; 
    a[ic] = ic;
    printf("index = %d\n", ic);
  }

}
int main() {
  foo_loop(2);
  return 0;
}
