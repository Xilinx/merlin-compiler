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
#include "cmost.h"
#pragma ACCEL kernel
void top(int *a, int *b) {
  __merlin_access_range(a, 0, 100);
  __merlin_access_range(b, 0, 2000);
  int i;
  while (i < 100) {
    a[i] = b[i];
    i++;
  }

  for (i = 0; i < 1000; ++i) {
    if (i * i  < 100) {
      a[i] = b[i];
    }
  }
  int t;
  for(t = 0;t < 4 *((25 - 6 + 1) / 4 +(25 - 6 + 1) % 4) / 4;t++) {
    a[t] = b[t];
  }
}
