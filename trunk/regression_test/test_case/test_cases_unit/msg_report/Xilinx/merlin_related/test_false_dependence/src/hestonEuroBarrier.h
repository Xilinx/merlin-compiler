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
----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* This class is called "heston" which implements the Heston model.
*
* simulation() is launched by top function.
* It defines and initializes some objects of RNG and then launch the simulation.
*
* sampleSIM performs the entire simulation and passes the results to top
function.
*
*----------------------------------------------------------------------------
*/
#ifndef __HESTON_H__
#define __HESTON_H__

#include "barrierData.h"
#include "stockData.h"
#include "volatilityData.h"
#include "RNG.h"

class heston {
  const stockData data;
  const volData vol;
  const barrierData bData;
  static const int NUM_RNGS;
  static const int NUM_SIMGROUPS;
  static const int NUM_STEPS;
  static const int NUM_SIMS;

public:
  heston(stockData, volData, barrierData);
  void simulation(data_t *, data_t *);
  void sampleSIM(RNG *, data_t *, data_t *);
};
extern "C" void hestonEuroBarrier(data_t *pCall, data_t *pPut, data_t expect,
                                  data_t kappa, data_t variance,
                                  data_t correlation, data_t timeT,
                                  data_t freeRate, data_t volatility,
                                  data_t initPrice, data_t strikePrice,
                                  data_t upB, data_t lowB);
#endif
