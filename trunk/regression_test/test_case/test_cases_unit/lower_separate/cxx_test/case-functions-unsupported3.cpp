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
#include "common.h"

int var;

void rec1(int i);
void rec2(int i);
void rec3(int i);

void rec1(int i) {
  if (i <= 0) {
    return;
  } else {
    var += i;
    rec2(i - 1);
  }
}

void rec2(int i) {
  var += i;
  rec3(i - 1);
}

void rec3(int i) {
  var += i;
  rec1(i - 1);
}

#pragma ACCEL kernel
void g() {
  rec1(100);
}
