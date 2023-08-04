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
const int N = 100;

class Test 
{
  public: int a;
  char b;
}
;
static void __merlin_dummy_b();
char b_b;
int b_a;
#pragma ACCEL kernel

void top(int *a_a,char *a_b)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=100
  int i;
  b_a = 1;
  b_b = 'b';
  for (i = 0; i < N; ++i) {
    a_a[i] = a_a[i] + b_a;
    a_b[i] = ((char )(((int )a_b[i]) + ((int )b_b)));
  }
  return ;
}

int main()
{
  char *a_b;
  int *a_a;
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="class Test"
  int i;
  class Test *a = new Test [100];
  for (i = 0; i < N; ++i) 
    a[i] . a = i;
  top(a_a,a_b);
  delete []a;
  return 0;
}
