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
#include "header.h"
struct DATA32{unsigned int data[32];};
class type_a {
  private:
    int a;
  public:
    void write(int val) {a=val;}
};
typedef union {int b;short s[4];}bus_to_short_t;
type_a c_a;

struct st {
  int data[4];
};

static void sub1(struct st_0 *this_) {
  long state=1;
  for (int round_counter = 1; round_counter<=14; round_counter ++ )
  {
#pragma ACCEL PARALLEL
    state^=this_ -> key_list[(long )round_counter];
  }
}

#pragma ACCEL kernel
void func_top_0(int a[32]) {
  DATA32 burst;
  st_0 cipher;
#pragma HLS array_partition variable=cipher complete dim=0
  type_a *c_b = &c_a;
  for (int i = 0; i < 32; i++)
  {
#pragma ACCEL PARALLEL
    burst.data[i] = a[i];
    c_b->write(a[i]);
  }
  sub1(&cipher);
  bus_to_short_t val;
  short out[4];
for (int i = 0; i < 4; i++)
  {
#pragma ACCEL PARALLEL
    out[i] = val.s[i];
  }
  struct st st_a, st_b[3], st_c[2][3];
  for (int j = 0; j < 3; j++) {
#pragma ACCEL parallel
    for (int i = 0; i < 4; ++i) {
#pragma ACCEL parallel
      st_a.data[i] = 0;
      st_a.data[i] += 1;
      st_b[j].data[i] = 0;
      for (int k = 0; k < 2; ++k) {
#pragma ACCEL parallel
        st_c[k][j].data[i] = 0;
      }
    }
  }
}
