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
void foo1() {
  int tmp;
  for (int i = 0; i < 10; i++) {
    tmp++;
  }
}

void foo2() {
  int tmp;
  for (int i = 0; i < 10; i++) {
    #pragma HLS unroll
    tmp++;
  }
}

int foo3(int tmp) {
  for (int i = 0; i < 10; i++) {
    tmp++;
  }
  return tmp;
}

int foo4(int tmp) {
  for (int i = 0; i < 10; i++) {
    #pragma HLS unroll
    tmp++;
  }
  return tmp;
}

void foo5() {
  foo2();
}


void func_top_0_df() {
    #pragma HLS dataflow 
    foo1();
    foo2();
}

void foo9() {
  for (int i = 0; i < 100; i++) {
    i++;
  } 
}

void foo8() {
  foo9();
}

void foo7() {
  int tmp;
  tmp++;
}

void foo6() {
  foo7();
}


void foo12() {
  int tmp;
  tmp++;
}

void foo11() {
  foo12();
  int i = 0;
  while (i < 10) {
  #pragma ACCEL loop_tripcount max=50
  #pragma HLS unroll
    i++;
  }
}

void foo10() {
  foo11();
  foo12();
  for (int i = 0; i < 10; i++) {
    i++;
  }
}

#pragma ACCEL kernel
void func_top_2() {
  foo10();
  foo11();
  foo12(); 
}



