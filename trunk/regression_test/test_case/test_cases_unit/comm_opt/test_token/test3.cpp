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

#pragma ACCEL kernel
void foo1(int *a, int *b, int *c, int *d) {
  int i, j, k;
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 100; ++j) {
      for (k = 0; k < 100; ++k) {
        a[k + i + j] += k;
      }

      for (k = 0; k <100; ++k) {
        b[k] += a[k + i];
      }
    }
  }
  c[i] += a[i];
  for (j = 0; j < 100; ++j){
    d[i + j] += a[j];
	}
}

int main() {
  int a[300];
  int b[300];
  int c[300];
  int d[300];
  foo1(a, b, c, d);
  return 0;
}
