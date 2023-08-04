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
#include <string.h> 

void sub(int *a) {
  for (int i = 0; i < 10; ++i) 
    merlin_L2:for (int j = 0; j < 10; ++j) {
      a[i *10 + j] = i *j;
    }

  for (int i = 0; i < 10; ++i) {
    int j = 0; 
merlin_L1:while (j < 10) {
      a[i *10 + j] = i *j;
      j++;
    }
  }
  int i = 0;
merlin_L3: do {
    int j = 0;
    while (j < 10) {
      a[i * 10 + j] = i;
      j++;
    }
    i++;
  } while (i < 10);

  i = 0;
L0: do {
      a[i]++;
    } while (i < 10);

}
#pragma ACCEL kernel
void top(int *a)
{
  for (int i = 0; i < 10; ++i) 
    for (int j = 0; j < 10; ++j) {
      a[i *10 + j] = i *j;
    }

  for (int i = 0; i < 10; ++i) {
    int j = 0; 
L0:while (j < 10) {
      a[i *10 + j] = i *j;
      j++;
      goto L0;
    }
  }
  int i = 0;
  do {
    int j = 0;
    while (j < 10) {
      a[i * 10 + j] = i;
      j++;
    }
    i++;
  } while (i < 10);
  sub(a);
  switch (a[0]) {
    case 1:
      for (int i = 0; i < 10; ++i) {}
      break;
    default:
      for (int i = 0; i < 10; ++i) {}
      break;
  }
}


void __merlinwrapper_top(int *a) {
  top(a);
}
