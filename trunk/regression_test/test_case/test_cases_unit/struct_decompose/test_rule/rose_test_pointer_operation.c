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
char b;}st;
#pragma ACCEL kernel

void top(int *a_a,char *a_b,int size)
{
  
#pragma ACCEL INTERFACE DEPTH=1000 VARIABLE=a_b max_depth=1000
  
#pragma ACCEL INTERFACE DEPTH=1000 VARIABLE=a_a max_depth=1000
  char *ptr_b = a_b + 1;
  int *ptr_a = a_a + 1;
  ptr_a++;
  ptr_b++;
  ptr_a += 1;
  ptr_b += 1;
  ptr_a -= 2;
  ptr_b -= 2;
  ptr_a--;
  ptr_b--;
  ptr_a[0] = 0;
  ptr_b[0] = ((char )0);
  char *ptr2_b = a_b + size * 2 + 1;
  int *ptr2_a = a_a + size * 2 + 1;
  ptr2_a[0] = 1;
  ptr2_b[0] = ((char )1);
  char *ptr3_b = a_b - size * 2 + 1;
  int *ptr3_a = a_a - size * 2 + 1;
  ptr3_a[0] = 1;
  ptr3_b[0] = ((char )2);
  ptr3_a = a_a + size * 2 - 1;
  ptr3_b = a_b + size * 2 - 1;
  ptr3_a[0] = 3;
  ptr3_b[0] = ((char )4);
  char *ptr4_b = a_b + (size * 2 + 1);
  int *ptr4_a = a_a + (size * 2 + 1);
  ptr4_a[0] = 5;
  ptr4_b[0] = ((char )6);
  if (a_a != ((int *)0) && a_b != ((char *)0)) {
    ( *a_a)++;
  }
  return ;
}
