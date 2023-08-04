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
typedef struct st {
  int a;
  char b;
} st;
#pragma ACCEL kernel
void top(st *a, int size) {
#pragma accel interface variable=a depth=1000
  st *ptr = a + 1;
  ptr++;
  ptr += 1;
  ptr -= 2;
  ptr--;
  ptr[0].a = 0;
  ptr[0].b = 0;
  st *ptr2 = a + (size *  2) + 1;
  ptr2[0].a = 1;
  ptr2[0].b = 1;
  st *ptr3 = a - (size *  2) + 1;
  ptr3[0].a = 1;
  ptr3[0].b = 2;
  ptr3 = a + (size *  2) - 1;
  ptr3[0].a = 3;
  ptr3[0].b = 4;
  st *ptr4 = (size *  2) + 1 + a;
  ptr4[0].a = 5;
  ptr4[0].b = 6;
  if (a != 0) {
	a->a++;
  }
	
  return;
}
