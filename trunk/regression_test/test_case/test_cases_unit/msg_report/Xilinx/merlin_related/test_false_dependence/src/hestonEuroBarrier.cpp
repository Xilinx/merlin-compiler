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

*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Top function is defined here with interface specified as axi4.
* It creates an object of heston and launches the simulation.
*
*----------------------------------------------------------------------------
*/
#include "hestonEuroBarrier.h"
#include "stockData.h"
//#include "stockData.cpp"
//#include "RNG.cpp"
#include "RNG.h"
//#include "volatilityData.cpp"
#include "volatilityData.h"
//#include "barrierData.cpp"
#include "barrierData.h"

float my_fmaxf(float a, float b) { return (a > b) ? a : b; }

#pragma ACCEL kernel
extern "C" void
hestonEuroBarrier(data_t pCall[1], data_t pPut[1], // call price and put price
                  data_t expect,                   // theta
                  data_t kappa,                    // kappa
                  data_t variance,                 // xi
                  data_t correlation,              // rho
                  data_t timeT,                    // time period of options
                  data_t freeRate,    // interest rate of the riskless asset
                  data_t volatility,  // volatility of the risky asset
                  data_t initPrice,   // stock price at time 0
                  data_t strikePrice, // strike price
                  data_t upB,         // up barrier
                  data_t lowB         // low barrier
) {
  volData vol(expect, kappa, variance, volatility, correlation);
  stockData sd(timeT, freeRate, volatility, initPrice, strikePrice);
  barrierData bD(upB, lowB);
  heston bs(sd, vol, bD);
  data_t call, put;
  bs.simulation(&call, &put);

  *pCall = call;
  *pPut = put;
  return;
}

#define NUM_RNGS 32
#define NUM_SIMGROUPS 512
#define NUM_STEPS 256
#define NUM_SIMS 512

heston::heston(stockData data, volData vol, barrierData bData)
    : data(data), vol(vol), bData(bData) {}
void heston::simulation(data_t *pCall, data_t *pPut) {

  RNG mt_rng[NUM_RNGS];
  uint seeds[NUM_RNGS];

#ifndef BASELINE
#pragma ACCEL parallel
#endif
loop_seed:
  for (int i = 0; i < NUM_RNGS; i++) {
    seeds[i] = i;
  }
  // RNG_init_array(mt_rng,seeds,NUM_RNGS);
  // void RNG_init_array(RNG* rng, uint* seed, const int size)
  {
    uint tmp[32];

#ifndef BASELINE
#pragma ACCEL parallel
#endif
    for (int i = 0; i < 32; i++) {
      mt_rng[i].index = 0;
      mt_rng[i].seed = seeds[i];
      tmp[i] = seeds[i];
    }

    for (int i = 0; i < RNG_H; i++) {
#ifndef BASELINE
#pragma ACCEL parallel
#endif
      for (int k = 0; k < 32; k++) {
        mt_rng[k].mt_e[i] = tmp[k];
        tmp[k] = RNG_F * (tmp[k] ^ (tmp[k] >> (RNG_W - 2))) + i * 2 + 1;
        mt_rng[k].mt_o[i] = tmp[k];
        tmp[k] = RNG_F * (tmp[k] ^ (tmp[k] >> (RNG_W - 2))) + i * 2 + 2;
      }
    }
  }
  // return sampleSIM(mt_rng,pCall,pPut);
  // void heston::sampleSIM(RNG* mt_rng, data_t* call,data_t* put)
  {
    const data_t Dt = data.timeT / NUM_STEPS,
                 ratio1 = expf(-data.freeRate * data.timeT),
                 ratio2 = sqrtf(
                     my_fmaxf(1.0f - vol.correlation * vol.correlation, 0.0f)),
                 ratio3 = Dt * data.freeRate,
                 ratio4 = vol.kappa * vol.expect * Dt,
                 volInit = my_fmaxf(vol.initValue, 0.0f) * Dt;

    data_t fCall = 0.0f, fPut = 0.0f;
    data_t sCall[NUM_RNGS], sPut[NUM_RNGS];
    data_t stockPrice[NUM_RNGS][NUM_SIMGROUPS];
    data_t vols[NUM_RNGS][NUM_SIMGROUPS], pVols[NUM_RNGS][NUM_SIMGROUPS];
    data_t num1[NUM_RNGS][NUM_SIMGROUPS], num2[NUM_RNGS][NUM_SIMGROUPS];
    data_t bBarrier[NUM_RNGS][NUM_SIMGROUPS];

#ifndef BASELINE
#pragma ACCEL parallel
#endif
    for (int i = 0; i < NUM_RNGS; i++) {
      sCall[i] = 0.0f;
      sPut[i] = 0.0f;
    }
  loop_init:
    for (int s = 0; s < NUM_SIMGROUPS; s++) {
      for (int i = 0; i < NUM_RNGS; i++) {
        stockPrice[i][s] = data.initPrice;
        vols[i][s] = vol.initValue;
        pVols[i][s] = volInit;
        bBarrier[i][s] = true;
      }
    }
  loop_main:
    for (int j = 0; j < NUM_SIMS; j++) {
    loop_path:
      for (int path = 0; path < NUM_STEPS; path++) {
      loop_share:
        for (int s = 0; s < NUM_SIMGROUPS; s++) {
#ifndef BASELINE
#pragma ACCEL parallel factor = 8
#endif
        loop_parallel:
          for (uint i = 0; i < NUM_RNGS; i++) {
            if (!bBarrier[i][s])
              continue;
            mt_rng[i].BOX_MULLER(&num1[i][s], &num2[i][s], pVols[i][s]);

            stockPrice[i][s] *=
                expf(ratio3 - pVols[i][s] * 0.5f +
                     num1[i][s] * vol.correlation + num2[i][s] * ratio2);
            if (stockPrice[i][s] < bData.downBarrier ||
                stockPrice[i][s] > bData.upBarrier) {
              bBarrier[i][s] = false;
            }
            vols[i][s] +=
                ratio4 - vol.kappa * pVols[i][s] + vol.variance * num1[i][s];
            pVols[i][s] = my_fmaxf(vols[i][s], 0.0f) * Dt;
          }
        }
      }
    loop_sum:
      for (int s = 0; s < NUM_SIMGROUPS; s++) {
#ifndef BASELINE
#pragma ACCEL parallel
#endif
      loop_sum_R:
        for (int i = 0; i < NUM_RNGS; i++) {
          vols[i][s] = vol.initValue;
          pVols[i][s] = volInit;
          if (bBarrier[i][s]) {
            float payoff = stockPrice[i][s] - data.strikePrice;
            if (stockPrice[i][s] > data.strikePrice) {
              sCall[i] += payoff;
            } else {
              sPut[i] -= payoff;
            }
          }
          stockPrice[i][s] = data.initPrice;
          bBarrier[i][s] = true;
        }
      }
    }
#ifndef BASELINE
#pragma ACCEL parallel
#endif
  loop_final_sum:
    for (int i = 0; i < NUM_RNGS; i++) {
      fCall += sCall[i];
      fPut += sPut[i];
    }
    *pCall = ratio1 * fCall / NUM_RNGS / NUM_SIMS / NUM_SIMGROUPS;
    *pPut = ratio1 * fPut / NUM_RNGS / NUM_SIMS / NUM_SIMGROUPS;
  }
}
void heston::sampleSIM(RNG *mt_rng, data_t *call, data_t *put) {
  const data_t Dt = data.timeT / NUM_STEPS,
               ratio1 = expf(-data.freeRate * data.timeT),
               ratio2 = sqrtf(
                   my_fmaxf(1.0f - vol.correlation * vol.correlation, 0.0f)),
               ratio3 = Dt * data.freeRate,
               ratio4 = vol.kappa * vol.expect * Dt,
               volInit = my_fmaxf(vol.initValue, 0.0f) * Dt;

  data_t fCall = 0.0f, fPut = 0.0f;
  data_t sCall[NUM_RNGS], sPut[NUM_RNGS];
  data_t stockPrice[NUM_RNGS][NUM_SIMGROUPS];
  data_t vols[NUM_RNGS][NUM_SIMGROUPS], pVols[NUM_RNGS][NUM_SIMGROUPS];
  data_t num1[NUM_RNGS][NUM_SIMGROUPS], num2[NUM_RNGS][NUM_SIMGROUPS];
  data_t bBarrier[NUM_RNGS][NUM_SIMGROUPS];

#ifndef BASELINE
#pragma ACCEL parallel
#endif
  for (int i = 0; i < NUM_RNGS; i++) {
    sCall[i] = 0.0f;
    sPut[i] = 0.0f;
  }
loop_init:
  for (int s = 0; s < NUM_SIMGROUPS; s++) {
    for (int i = 0; i < NUM_RNGS; i++) {
      stockPrice[i][s] = data.initPrice;
      vols[i][s] = vol.initValue;
      pVols[i][s] = volInit;
      bBarrier[i][s] = true;
    }
  }
loop_main:
  for (int j = 0; j < NUM_SIMS; j++) {
  loop_path:
    for (int path = 0; path < NUM_STEPS; path++) {
    loop_share:
      for (int s = 0; s < NUM_SIMGROUPS; s++) {
#ifndef BASELINE
#pragma ACCEL parallel
#endif
      loop_parallel:
        for (uint i = 0; i < NUM_RNGS; i++) {
          if (!bBarrier[i][s])
            continue;
          mt_rng[i].BOX_MULLER(&num1[i][s], &num2[i][s], pVols[i][s]);

          stockPrice[i][s] *=
              expf(ratio3 - pVols[i][s] * 0.5f + num1[i][s] * vol.correlation +
                   num2[i][s] * ratio2);
          if (stockPrice[i][s] < bData.downBarrier ||
              stockPrice[i][s] > bData.upBarrier) {
            bBarrier[i][s] = false;
          }
          vols[i][s] +=
              ratio4 - vol.kappa * pVols[i][s] + vol.variance * num1[i][s];
          pVols[i][s] = my_fmaxf(vols[i][s], 0.0f) * Dt;
        }
      }
    }
  loop_sum:
    for (int s = 0; s < NUM_SIMGROUPS; s++) {
#ifndef BASELINE
#pragma ACCEL parallel
#endif
    loop_sum_R:
      for (int i = 0; i < NUM_RNGS; i++) {
        vols[i][s] = vol.initValue;
        pVols[i][s] = volInit;
        if (bBarrier[i][s]) {
          float payoff = stockPrice[i][s] - data.strikePrice;
          if (stockPrice[i][s] > data.strikePrice) {
            sCall[i] += payoff;
          } else {
            sPut[i] -= payoff;
          }
        }
        stockPrice[i][s] = data.initPrice;
        bBarrier[i][s] = true;
      }
    }
  }
#ifndef BASELINE
#pragma ACCEL parallel
#endif
loop_final_sum:
  for (int i = 0; i < NUM_RNGS; i++) {
    fCall += sCall[i];
    fPut += sPut[i];
  }
  *call = ratio1 * fCall / NUM_RNGS / NUM_SIMS / NUM_SIMGROUPS;
  *put = ratio1 * fPut / NUM_RNGS / NUM_SIMS / NUM_SIMGROUPS;
}
