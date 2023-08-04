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
void test1() {
  if (0)
    0;
  // int i;
  for (int i = 0; i; i = i + 1) { // rose has a bug which leads to "for (int i =
                                  // 0; 0 ; i=i+1)" after const propagation
    int a[1][1] = {0};
  }
}

#pragma ACCEL kernel
void test2() {
  int b;
  {
    int a = (-5L);
    int a1 = (int)(-5L);
    if (b & a)
      ;
    if (b & a1)
      ;
  }

  {
    unsigned int a = (-5L);
    unsigned int a1 = (int)(-5L);
    if (b & a)
      ;
    if (b & a1)
      ;
  }

  {
    unsigned short a = (-5L);
    unsigned short a1 = (int)(-5L);
    if (b & a)
      ;
    if (b & a1)
      ;
  }

  {
    unsigned long long a = (-5L);
    unsigned long long a1 = (int)(-5L);
    if (b & a)
      ;
    if (b & a1)
      ;
  }

  {
    float b = 0;
    float a = (double)(-5L);
    float a1 = (float)(-4L);
    if (b + a)
      ;
    if (b + a1)
      ;
  }
}
