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
#define fptype float
// Cumulative Normal Distribution Function
// See Hull, Section 11.8, P.243-244
#define inv_sqrt_2xPI 0.39894228040143270286
#include<string.h>
#include "math.h"
#include "stdio.h"
#define CONST_E (2.71828182845904)
// Begin function my_exp returning float
// Begin function my_exp returning float

float my_exp(float x)
{
  int sign = x >= 0?1 : 0;
  float absx = (fabs(x));
  float int_part = 1.0;
  float y = (absx - floor(absx));
  float frac_part = 1 + y + y * y / 2 + y * y * y / 6 + y * y * y * y / 24;
//for (i = 0; i < (int)(floor(absx)); i++)
//{
//	int_part *= CONST_E;
//}
  return sign?1 / (int_part * frac_part) : int_part * frac_part;
}
// End function my_exp
// End function my_exp

float CNDF(float InputX)
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
// the 'if' part
  if (InputX < 0.0) 
// the 'if' part
{
    InputX = -InputX;
    sign = 1;
  }
   else 
// the 'else' part
// the 'else' part
    sign = 0;
  xInput = InputX;
// Compute NPrimeX term common to both four & six decimal accuracy calcs
  expValues = my_exp(((float )(- 0.5f * InputX * InputX)));
  xNPrimeofX = expValues;
  xNPrimeofX = (xNPrimeofX * 0.39894228040143270286);
  xK2 = (0.2316419 * xInput);
  xK2 = (1.0 + xK2);
  xK2 = (1.0 / xK2);
  xK2_2 = xK2 * xK2;
  xK2_3 = xK2_2 * xK2;
  xK2_4 = xK2_3 * xK2;
  xK2_5 = xK2_4 * xK2;
  xLocal_1 = (xK2 * 0.319381530);
  xLocal_2 = (xK2_2 * - 0.356563782);
  xLocal_3 = (xK2_3 * 1.781477937);
  xLocal_2 = xLocal_2 + xLocal_3;
  xLocal_3 = (xK2_4 * - 1.821255978);
  xLocal_2 = xLocal_2 + xLocal_3;
  xLocal_3 = (xK2_5 * 1.330274429);
  xLocal_2 = xLocal_2 + xLocal_3;
  xLocal_1 = xLocal_2 + xLocal_1;
  xLocal = xLocal_1 * xNPrimeofX;
  xLocal = (1.0 - xLocal);
  OutputX = xLocal;
// the 'if' part
  if (sign) 
// the 'if' part
{
    OutputX = (1.0 - OutputX);
  }
  return OutputX;
}
// End function CNDF
// End function CNDF 
// For debugging
// Begin function print_xmm returning void
// Begin function print_xmm returning void

void print_xmm(float in,char *s)
{
  printf("%s: %f\n",s,in);
}
// End function print_xmm
// End function print_xmm

float BlkSchlsEqEuroNoDiv(float sptprice,float strike,float rate,float volatility,float time,int otype,float timet)
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
  xSqrtTime = (sqrt(xTime));
//logValues = log( sptprice / strike );
  logValues = sptprice / strike;
  xLogTerm = logValues;
  xPowerTerm = xVolatility * xVolatility;
  xPowerTerm = (xPowerTerm * 0.5);
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
// the 'if' part
  if (otype == 0) 
// the 'if' part
{
    OptionPrice = sptprice * NofXd1 - FutureValueX * NofXd2;
// the 'else' part
  }
   else 
// the 'else' part
{
    NegNofXd1 = (1.0 - NofXd1);
    NegNofXd2 = (1.0 - NofXd2);
    OptionPrice = FutureValueX * NegNofXd2 - sptprice * NegNofXd1;
  }
  return OptionPrice;
}
// End function BlkSchlsEqEuroNoDiv
// End function BlkSchlsEqEuroNoDiv
// Begin function bs_kernel returning void
// Begin function bs_kernel returning void

#pragma ACCEL kernel
void bs_kernel(float *sptprice,float *strike,float *rate,float *volatility,float *otime,float *otype,float *prices)
{
  float volatility_1_0_buf[1000];
  float strike_1_0_buf[1000];
  float sptprice_1_0_buf[1000];
  float rate_1_0_buf[1000];
  float prices_1_0_buf[1000];
  float otype_1_0_buf[1000];
  float otime_1_0_buf[1000];
  float priceDelta;
#define BLOCK_SIZE  1000
#define TOTAL_OPTION 1000
  int k;
  for (k = 0; k < 1000; k++) {
    int ii;
    memcpy(&otime_1_0_buf[0],&otime[1000 * k],sizeof(float ) * 1000);
    memcpy(&otype_1_0_buf[0],&otype[1000 * k],sizeof(float ) * 1000);
    memcpy(&rate_1_0_buf[0],&rate[1000 * k],sizeof(float ) * 1000);
    memcpy(&sptprice_1_0_buf[0],&sptprice[1000 * k],sizeof(float ) * 1000);
    memcpy(&strike_1_0_buf[0],&strike[1000 * k],sizeof(float ) * 1000);
    memcpy(&volatility_1_0_buf[0],&volatility[1000 * k],sizeof(float ) * 1000);
    for (ii = 0; ii < 1000; ii++) {
      
#pragma ACCEL pipeline
      float price;
      float curr_sptprice = sptprice_1_0_buf[ii];
      float curr_strike = strike_1_0_buf[ii];
      float curr_rate = rate_1_0_buf[ii];
      float curr_volatility = volatility_1_0_buf[ii];
      float curr_otime = otime_1_0_buf[ii];
      float curr_otype = otype_1_0_buf[ii];
      price = BlkSchlsEqEuroNoDiv(curr_sptprice,curr_strike,curr_rate,curr_volatility,curr_otime,curr_otype,0);
      prices_1_0_buf[ii] = price;
    }
    memcpy(&prices[1000 * k],&prices_1_0_buf[0],sizeof(float ) * 1000);
  }
}
// End function bs_kernel
// End function bs_kernel
// End function bs_kernel
// End function bs_kernel
