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
#include<string.h>
#include<assert.h>


#pragma ACCEL kernel
void foo1(int a) {
  unsigned long off = ((unsigned long )a) * ((unsigned long )2);
  assert(off >= 0 && off <= 24);
  int arr[10][5664];
  for (int k = 0; k < 10; k++) {
    for (int j = 0; j < 6; j++) {
      arr[1][j * 16 + (k * 576) + off * 16]++;
    }
  } 
}
//////
void bar2(int *arr) {

}

#pragma ACCEL kernel
void foo2(int a) {
  unsigned long off = ((unsigned long )a) * ((unsigned long )2);
  assert(off >= 0 && off <= 24);
  int arr[10][5664];
  for (int k = 0; k < 10; k++) {
    for (int j = 0; j < 6; j++) {
      // potential bugs if I put 10 instead of a
      bar2(&arr[a][j * 16 + (k * 576) + off * 16]);
    }
  } 
}
//////
void bar3_1(int *arr) {

}

void bar3(int *arr, int a) {
  unsigned long off = ((unsigned long )a) * ((unsigned long )2);
  assert(off >= 0 && off <= 24);
  for (int k = 0; k < 10; k++) {
    for (int j = 0; j < 6; j++) {
      bar3_1(&arr[j * 16 + (k * 576) + off * 16]);
    }
  } 
}

#pragma ACCEL kernel
void foo3(int a, int *profbuf_vec_data) {
  int arr[10][5664];
  for (int i_sub = 0; i_sub < 10; i_sub++) {
    memcpy((void *)arr[i_sub - 0LL],(const void *)(&profbuf_vec_data[i_sub * 57600 + i_sub * 5760]),sizeof(int) * 5664);
    bar3(arr[i_sub], a);
  }
}

////
void bar4(int a) {

}
#pragma ACCEL kernel
void foo4(int a, int *profbuf_vec_data) {
  int arr[5664];
  unsigned long off = ((unsigned long )a) * ((unsigned long )2);
  assert(off >= 0 && off <= 24);
  memcpy((void *)arr,(const void *)(profbuf_vec_data),sizeof(int) * 5664);
  for (int k = 0; k < 10; k++) {
    for (int j = 0; j < 6; j++) {
      bar4(arr[j * 16 + (k * 576) + off * 16]);
    }
  } 
}

////
#pragma ACCEL kernel
void foo5(int a, int *profbuf_vec_data) {
  int arr[10][5664];
  unsigned long off = ((unsigned long )a) * ((unsigned long )2);
  assert(off >= 0 && off <= 24);
  for (int i_sub = 0; i_sub < 10; i_sub++) {
    memcpy((void *)arr[i_sub - 0LL],(const void *)(&profbuf_vec_data[i_sub * 57600 + i_sub * 5760]),sizeof(int) * 5664);
    for (int k = 0; k < 10; k++) {
      for (int j = 0; j < 6; j++) {
        arr[i_sub][j * 16 + (k * 576) + off * 16]++;
      }
    }
  } 
}

////
#pragma ACCEL kernel
void foo6(int a, int *profbuf_vec_data) {
  int arr[10][5664];
  unsigned long off = ((unsigned long )a) * ((unsigned long )2);
  assert(off >= 0 && off <= 24);
  for (int i_sub = 0; i_sub < 10; i_sub++) {
    for (int k = 0; k < 10; k++) {
      for (int j = 0; j < 6; j++) {
        arr[i_sub][j * 16 + (k * 576) + off * 16]++;
      }
    }
  } 
}
