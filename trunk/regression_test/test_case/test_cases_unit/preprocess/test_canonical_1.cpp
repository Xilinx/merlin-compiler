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
// Testing NULL loop initial/testing/increment
#pragma ACCEL kernel
void func_loop(float fa, float fb) {
  int a[10];
  int b[10];
  int c[10];

  int i = 0;
  int j = 0;
  int x, y, z;
  int N = 100;

  for (i = 0;; i++) {
    /* block 1*/
    c[i] = a[i] + b[i];
    if (i < 100)
      break;
  }

  for (i = 0;;) {
    /* block 2*/
    c[i] = a[i] + b[i];
    i++;
  }

  for (; i < 100; i++) {
    /* block 3*/
    c[i] = a[i] + b[i];
  }

  for (;; i++) {
    /* block 4*/
    c[i] = a[i] + b[i];
    if (i < 100)
      break;
  }

  for (i = 0;;) {
    /* block 5*/
    c[i] = a[i] + b[i];
    if (i < 100)
      break;
    i++;
  }

  for (; i < 100;) {
    /* block 6*/
    c[i] = a[i] + b[i];
    i++;
  }

  for (;;) {
    /* block 7*/
    c[i] = a[i] + b[i];
    if (i < 100)
      break;
    i++;
  }

  for (i = 0; i < 10; i++) { i++;}

  goto L0;
  for (i = 0; i < 10; i++) {
L0:
  }

  int sum=0;
  short i0=0;
#pragma unroll
  for (char ii = 0; ii < 10; ii++) { sum++;}

#pragma unroll
  for (i0 = 0; i0 < 10; i0++) { sum++;}

#pragma ACCEL parallel
  for (char i = 0; i < 10; i++) { sum++;}

  for (char i = 0; i < 10; i++) {
    if (i % 2 ) { continue; }
    else continue;
    continue;
  }
  for (char i = 0; i < 10; i++) {
    if (i % 2 ) { continue; }
    else break;
    continue;
  }
  
  for (char i = 0; i < 10; i++) {
    if (i % 2 ) continue; 
    i++;
  }
  
  for (char i = 0; i < 10; i++) {
    if (i % 2 ) continue; 
  }
  
  for (char i = 0; i < 10; i++) {
    if (i % 2 ) {
      if (i == 2) {
        continue; 
      }
      continue;
    }
    continue;
  }
  for (; fa > fb; ) {
    c[0]++;
  }
}

