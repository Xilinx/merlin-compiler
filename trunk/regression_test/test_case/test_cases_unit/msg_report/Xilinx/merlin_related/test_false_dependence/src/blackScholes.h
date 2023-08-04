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
* This class is called "blackScholes" which implements the BS model.
*
* simulation() is launched by top function.
* It defines and initializes some objects of RNG and then launch the simulation.
*
* sampleSIM performs the entire simulation and passes the results to top
function.
*
*----------------------------------------------------------------------------
*/
#ifndef __BLACKSCHOLES__
#define __BLACKSCHOLES__

#include "defTypes.h"
#include "RNG.h"
#include "stockData.h"

class blackScholes {
  stockData data;
  static const int NUM_RNGS;
  static const int NUM_SIMS;
  static const int NUM_SIMGROUPS;
  static const int NUM_STEPS;

public:
  blackScholes(stockData);
  void simulation(data_t *, data_t *);
  void sampleSIM(RNG *, data_t *, data_t *);
};

#endif
