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
* Here declares the class RNG which is short for Random Number Generator.
* It implements 32bits Mersenne-Twist algorithm and Box-Muller transformation.
*
* RNG_N is the total size of the states
* mt_e/mt_o are two arrays that stores the states.
* extract_number produces two uniformly i.i.d. numbers
* init()/init_array() are two functions for state initialization.
*
*----------------------------------------------------------------------------
*/

#ifndef __RNG__
#define __RNG__

#include "defTypes.h"

#define RNG_N 624
#define RNG_H 312

#define RNG_MH 198
#define RNG_MHI 199

#define RNG_F 0x6C078965
#define RNG_W 32

class RNG {
public:
  int index;
  uint seed;
  uint mt_e[RNG_H], mt_o[RNG_H];

  void init(uint);
  void extract_number(uint *, uint *);
  void BOX_MULLER(data_t *, data_t *, data_t);
  void BOX_MULLER(data_t *, data_t *, data_t, data_t);
  int increase(int);
  int &operator++();

  static void init_array(RNG *, uint *, const int);
};

#endif
