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
#include<stdlib.h>
#define N 100
struct student {
  int id;
  char *name;
};

#pragma ACCEL kernel
int top(struct student *a) {
#pragma ACCEL interface variable=a depth=100
#pragma ACCEL interface variable=a->name depth=10
  int count;
  for (int i = 0; i < N; ++i)
  {
    a[i].id = a[i].id + 10;
    a[i].name[1] = a[i].name[0];
    char *temp = a[i].name; 


    //test for pointer cast
    //we should check this kind of pointer cast for OpenCL
    count += ((student*) temp)->id; 
  }

  return count;
}

int main() {
  int count = 0;
  struct student *a;
  a = (struct student*) malloc(sizeof(struct student) * N);
  for (int i = 0; i < N; ++i)
    a[i].id = i;
  count = top(a);
  return 0;

}
