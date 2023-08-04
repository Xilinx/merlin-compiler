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
// Testing labels
#include<stdio.h>
#pragma ACCEL kernel
void func_loop() {
  int a[100]={0};
  int b[100];
  int c[100];
  for(int pp=0; pp<100; pp++) {
    a[pp]=1;
    b[pp]=5;
    c[pp]=2;
  }

  int N = 100;

l1:
  for (unsigned char ic = N; ((char)ic) >= 0; ic--) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
    }
l2:
  for (unsigned char ic = N; ((char)ic) >= 0; ic += 255) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
    }
l3:
  for (unsigned short ic = N; ((short)ic) >= 0; ic += 65535) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
  }
l4:
  for (unsigned int ic = N; ((int)ic) >= 0; ic += 0xffffffff) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
  }
l5:
  for (unsigned long ic = N; ((long)ic) >= 0; ic += 0xffffffffffffffffl) {
      c[ic] = a[ic] + b[ic];
      printf("index = %ld\n", ic);
  }
l6:
  for (unsigned char ic = N; ((char)ic) >= 0; ic += 253) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
    }
l7:
  for (unsigned short ic = N; ((short)ic) >= 0; ic += 65534) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
  }
l8:
  for (unsigned int ic = N; ((int)ic) >= 0; ic += 0xfffffff0) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
  }
l9:
  for (unsigned long ic = N; ((long)ic) >= 0; ic += 0xfffffffffffffff2l) {
      c[ic] = a[ic] + b[ic];
      printf("index = %d\n", ic);
  }

}

int main() {

  func_loop();

  return 0;
}
