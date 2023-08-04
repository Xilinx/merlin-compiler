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
/*
======================================================
 Copyright 2016 Liang Ma

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
======================================================
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* In the definition of Box_Muller(), sin/cos are used instead of sinf/cosf
* due to bugs in the synthesizer (C/RTL co-simulation is failing).
*
* macro__SINF_COSF_BUG_FIXED__ controls the switch between the two version.
*
*----------------------------------------------------------------------------
*/

#include "RNG.h"
#define RNG_M 397

#define RNG_R 31

#define RNG_U 11
#define RNG_A 0x9908B0DF
#define RNG_D 0xFFFFFFFF
#define RNG_S 7
#define RNG_B 0x9D2C5680
#define RNG_T 15
#define RNG_C 0xEFC60000
#define RNG_L 18
#define __SINF_COSF_BUG_FIXED__

#define lower_mask 0x7FFFFFFF //(1 << R) - 1
#define upper_mask 0x80000000 //~lower_mask

#define PI (data_t)3.14159265358979323846

#define MINI_RND (data_t)2.328306e-10

#include "my_fmaxf.h"

void RNG::init(uint seed) {
  this->index = 0;
  this->seed = seed;
  uint tmp = seed;

loop_seed_init:
  for (int i = 0; i < RNG_H; i++) {
    mt_e[i] = tmp;
    tmp = RNG_F * (tmp ^ (tmp >> (RNG_W - 2))) + i * 2 + 1;
    mt_o[i] = tmp;
    tmp = RNG_F * (tmp ^ (tmp >> (RNG_W - 2))) + i * 2 + 2;
  }
}

void RNG::init_array(RNG *rng, uint *seed, const int size) {
  // uint tmp[size];
  uint tmp[32];

#ifndef BASELINE
#pragma ACCEL parallel
#endif
// loop_set_seed:for(int i=0;i<size;i++)
loop_set_seed:
  for (int i = 0; i < 32; i++) {
    rng[i].index = 0;
    rng[i].seed = seed[i];
    tmp[i] = seed[i];
  }

loop_seed_init:
  for (int i = 0; i < RNG_H; i++) {
#ifndef BASELINE
#pragma ACCEL parallel
#endif
  // loop_group_init:for(int k=0;k<size;k++)
  loop_group_init:
    for (int k = 0; k < 32; k++) {
      rng[k].mt_e[i] = tmp[k];
      tmp[k] = RNG_F * (tmp[k] ^ (tmp[k] >> (RNG_W - 2))) + i * 2 + 1;
      rng[k].mt_o[i] = tmp[k];
      tmp[k] = RNG_F * (tmp[k] ^ (tmp[k] >> (RNG_W - 2))) + i * 2 + 2;
    }
  }
}

void RNG::extract_number(uint *num1, uint *num2) {
  int id1 = increase(1), idm = increase(RNG_MH), idm1 = increase(RNG_MHI);

  uint x = this->seed, x1 = this->mt_o[this->index], x2 = this->mt_e[id1],
       xm = this->mt_o[idm], xm1 = this->mt_e[idm1];

  x = (x & upper_mask) + (x1 & lower_mask);
  uint xp = x >> 1;
  if ((x & 0x01) != 0)
    xp ^= RNG_A;
  x = xm ^ xp;

  uint y = x;
  y ^= ((y >> RNG_U) & RNG_D);
  y ^= ((y << RNG_S) & RNG_B);
  y ^= ((y << RNG_T) & RNG_C);
  y ^= (y >> RNG_L);
  *num1 = y;
  mt_e[this->index] = x;

  x1 = (x1 & upper_mask) + (x2 & lower_mask);
  uint xt = x1 >> 1;
  if ((x1 & 0x01) != 0)
    xt ^= RNG_A;
  x1 = xm1 ^ xt;

  uint y1 = x1;
  y1 ^= ((y1 >> RNG_U) & RNG_D);
  y1 ^= ((y1 << RNG_S) & RNG_B);
  y1 ^= ((y1 << RNG_T) & RNG_C);
  y1 ^= (y1 >> RNG_L);
  *num2 = y1;
  mt_o[this->index] = x1;

  this->index = id1;
  this->seed = x2;
}

int RNG::increase(int k) {
  int tmp = this->index + k;
  return (tmp >= RNG_H) ? tmp - RNG_H : tmp;
}

void RNG::BOX_MULLER(data_t *data1, data_t *data2, data_t deviation) {
  data_t tp, tmp1, tmp2;
  uint num1, num2;

  extract_number(&num1, &num2);
  tmp1 = num1 * MINI_RND;
  tmp2 = num2 * MINI_RND;
  tp = sqrtf(my_fmaxf(-2 * logf(tmp1), 0) * deviation);
  *data1 = cosf(2 * PI * (tmp2)) * tp;
  *data2 = sinf(2 * PI * (tmp2)) * tp;
}

void RNG::BOX_MULLER(data_t *data1, data_t *data2, data_t ave,
                     data_t deviation) {
  data_t tp, tmp1, tmp2;
  uint num1, num2;

  extract_number(&num1, &num2);
  tmp1 = num1 * MINI_RND;
  tmp2 = num2 * MINI_RND;
  tp = sqrtf(my_fmaxf(-2 * logf(tmp1), 0) * deviation);
  *data1 = cosf(2 * PI * (tmp2)) * tp + ave;
  *data2 = sinf(2 * PI * (tmp2)) * tp + ave;
}
