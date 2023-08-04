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
#include <math.h>
#include "merlin_type_define.h"
extern int __merlin_include__GB_merlin_type_define_h_1;
extern int __merlin_include__GB_math_h_0;

static float my_exp(float x)
{
  int sign = x >= ((float )0)?1 : 0;
  float absx = (float )(fabs(((double )x)));
  float int_part = (float )1.0;
  float y = (float )(((double )absx) - floor(((double )absx)));
  float frac_part = ((float )1) + y + y * y / ((float )2) + y * y * y / ((float )6) + y * y * y * y / ((float )24);
  return ((bool )sign)?((float )1) / (int_part * frac_part) : int_part * frac_part;
}

static float CNDF(float InputX)
{
  int sign;
  float OutputX;
  float xInput;
  float xNPrimeofX;
  float expValues;
  float xK2;
  float xK2_2;
  float xK2_3;
  float xK2_4;
  float xK2_5;
  float xLocal;
  float xLocal_1;
  float xLocal_2;
  float xLocal_3;
// Check for negative value of InputX
  if (((double )InputX) < 0.0) {
    InputX = -InputX;
    sign = 1;
  }
   else 
    sign = 0;
  xInput = InputX;
// Compute NPrimeX term common to both four & six decimal accuracy calcs
  expValues = my_exp(((float )(- 0.5f * InputX * InputX)));
  xNPrimeofX = expValues;
  xNPrimeofX = ((float )(((double )xNPrimeofX) * 0.39894228040143270286));
  xK2 = ((float )(0.2316419 * ((double )xInput)));
  xK2 = ((float )(1.0 + ((double )xK2)));
  xK2 = ((float )(1.0 / ((double )xK2)));
  xK2_2 = xK2 * xK2;
  xK2_3 = xK2_2 * xK2;
  xK2_4 = xK2_3 * xK2;
  xK2_5 = xK2_4 * xK2;
  xLocal_1 = ((float )(((double )xK2) * 0.319381530));
  xLocal_2 = ((float )(((double )xK2_2) * - 0.356563782));
  xLocal_3 = ((float )(((double )xK2_3) * 1.781477937));
  xLocal_2 = xLocal_2 + xLocal_3;
  xLocal_3 = ((float )(((double )xK2_4) * - 1.821255978));
  xLocal_2 = xLocal_2 + xLocal_3;
  xLocal_3 = ((float )(((double )xK2_5) * 1.330274429));
  xLocal_2 = xLocal_2 + xLocal_3;
  xLocal_1 = xLocal_2 + xLocal_1;
  xLocal = xLocal_1 * xNPrimeofX;
  xLocal = ((float )(1.0 - ((double )xLocal)));
  OutputX = xLocal;
  if ((bool )sign) {
    OutputX = ((float )(1.0 - ((double )OutputX)));
  }
  return OutputX;
}

static float BlkSchlsEqEuroNoDiv(float sptprice,float strike,float rate,float volatility,float time,int otype,float timet)
{
  float OptionPrice;
// local private working variables for the calculation
  float xStockPrice;
  float xStrikePrice;
  float xRiskFreeRate;
  float xVolatility;
  float xTime;
  float xSqrtTime;
  float logValues;
  float xLogTerm;
  float xD1;
  float xD2;
  float xPowerTerm;
  float xDen;
  float d1;
  float d2;
  float FutureValueX;
  float NofXd1;
  float NofXd2;
  float NegNofXd1;
  float NegNofXd2;
  xStockPrice = sptprice;
  xStrikePrice = strike;
  xRiskFreeRate = rate;
  xVolatility = volatility;
  xTime = time;
  xSqrtTime = ((float )(sqrt(((double )xTime))));
//logValues = log( sptprice / strike );
  logValues = sptprice / strike;
  xLogTerm = logValues;
  xPowerTerm = xVolatility * xVolatility;
  xPowerTerm = ((float )(((double )xPowerTerm) * 0.5));
  xD1 = xRiskFreeRate + xPowerTerm;
  xD1 = xD1 * xTime;
  xD1 = xD1 + xLogTerm;
  xDen = xVolatility * xSqrtTime;
  xD1 = xD1 / xDen;
  xD2 = xD1 - xDen;
  d1 = xD1;
  d2 = xD2;
  NofXd1 = CNDF(d1);
  NofXd2 = CNDF(d2);
  FutureValueX = strike * my_exp(((float )(-rate * time)));
  if (otype == 0) {
    OptionPrice = sptprice * NofXd1 - FutureValueX * NofXd2;
  }
   else {
    NegNofXd1 = ((float )(1.0 - ((double )NofXd1)));
    NegNofXd2 = ((float )(1.0 - ((double )NofXd2)));
    OptionPrice = FutureValueX * NegNofXd2 - sptprice * NegNofXd1;
  }
  return OptionPrice;
}
#pragma ACCEL kernel

void bs_kernel(float *sptprice,float *strike,float *rate,float *volatility,float *otime,float *otype,float *prices)
{
  
#pragma ACCEL interface variable=prices depth=0
  
#pragma ACCEL interface variable=otype depth=0
  
#pragma ACCEL interface variable=otime depth=0
  
#pragma ACCEL interface variable=volatility depth=0
  
#pragma ACCEL interface variable=rate depth=0
  
#pragma ACCEL interface variable=strike depth=0
  
#pragma ACCEL interface variable=sptprice depth=0
  float priceDelta;
  int BLOCK_SIZE = 128;
  int TOTAL_OPTION = 128;
  int k;
  for (k = 0; k < TOTAL_OPTION; k++) {
    int ii;
//#pragma ACCEL pipeline
    for (ii = 0; ii < BLOCK_SIZE; ii++) {
      float price;
      float curr_sptprice = sptprice[k * BLOCK_SIZE + ii];
      float curr_strike = strike[k * BLOCK_SIZE + ii];
      float curr_rate = rate[k * BLOCK_SIZE + ii];
      float curr_volatility = volatility[k * BLOCK_SIZE + ii];
      float curr_otime = otime[k * BLOCK_SIZE + ii];
      float curr_otype = otype[k * BLOCK_SIZE + ii];
      price = BlkSchlsEqEuroNoDiv(curr_sptprice,curr_strike,curr_rate,curr_volatility,curr_otime,((int )curr_otype),((float )0));
      prices[k * BLOCK_SIZE + ii] = price;
    }
  }
}
