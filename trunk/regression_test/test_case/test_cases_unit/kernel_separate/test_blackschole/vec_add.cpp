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
//#include "cmost.h"
#include <stdio.h>
#include <math.h>

//Precision to use for calculations
#define fptype float

#define NUM_RUNS 1

#define PROBLEM_SIZE 16384
#define inv_sqrt_2xPI 0.39894228040143270286
#define CONST_E (2.71828182845904)
typedef struct OptionData_ {
        fptype s;          // spot price
        fptype strike;     // strike price
        fptype r;          // risk-free interest rate
        fptype divq;       // dividend rate
        fptype v;          // volatility
        fptype t;          // time to maturity or option expiration in years 
                           //     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)  
        char OptionType;   // Option type.  "P"=PUT, "C"=CALL
        fptype divs;       // dividend vals (not used in this test)
        fptype DGrefval;   // DerivaGem Reference Value
} OptionData;

OptionData *data;
int numOptions;

int numError = 0;
int nThreads;

// From vec_add_kernel

float my_exp(float x)
{
    int sign = (x>=0) ? 1 : 0;
    float absx = fabs(x);
    float int_part = 1.0;
    float y = absx - (floor(absx));
    float frac_part = 1 + y + y*y/2 + y*y*y/6 + y*y*y*y/24;
    return (sign) ? 1/(int_part * frac_part) : int_part * frac_part;
}



fptype CNDF ( fptype InputX ) 
{
    int sign;

    fptype OutputX;
    fptype xInput;
    fptype xNPrimeofX;
    fptype expValues;
    fptype xK2;
    fptype xK2_2, xK2_3;
    fptype xK2_4, xK2_5;
    fptype xLocal, xLocal_1;
    fptype xLocal_2, xLocal_3;

    // Check for negative value of InputX
    if (InputX < 0.0) {
        InputX = -InputX;
        sign = 1;
    } else 
        sign = 0;

    xInput = InputX;

    // Compute NPrimeX term common to both four & six decimal accuracy calcs
    expValues = my_exp((float)(-0.5f * InputX * InputX));
    xNPrimeofX = expValues;
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    xK2 = 0.2316419 * xInput;
    xK2 = 1.0 + xK2;
    xK2 = 1.0 / xK2;
    xK2_2 = xK2 * xK2;
    xK2_3 = xK2_2 * xK2;
    xK2_4 = xK2_3 * xK2;
    xK2_5 = xK2_4 * xK2;

    xLocal_1 = xK2 * 0.319381530;
    xLocal_2 = xK2_2 * (-0.356563782);
    xLocal_3 = xK2_3 * 1.781477937;
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_4 * (-1.821255978);
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_5 * 1.330274429;
    xLocal_2 = xLocal_2 + xLocal_3;

    xLocal_1 = xLocal_2 + xLocal_1;
    xLocal   = xLocal_1 * xNPrimeofX;
    xLocal   = 1.0 - xLocal;

    OutputX  = xLocal;

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    return OutputX;
} 

// For debugging
void print_xmm(fptype in, char* s) {
    printf("%s: %f\n", s, in);
}

fptype BlkSchlsEqEuroNoDiv( fptype sptprice,
        fptype strike, fptype rate, fptype volatility,
        fptype time, int otype, float timet )
{
    fptype OptionPrice;

    // local private working variables for the calculation
    fptype xStockPrice;
    fptype xStrikePrice;
    fptype xRiskFreeRate;
    fptype xVolatility;
    fptype xTime;
    fptype xSqrtTime;

    fptype logValues;
    fptype xLogTerm;
    fptype xD1; 
    fptype xD2;
    fptype xPowerTerm;
    fptype xDen;
    fptype d1;
    fptype d2;
    fptype FutureValueX;
    fptype NofXd1;
    fptype NofXd2;
    fptype NegNofXd1;
    fptype NegNofXd2;    

    xStockPrice = sptprice;
    xStrikePrice = strike;
    xRiskFreeRate = rate;
    xVolatility = volatility;

    xTime = time;
    xSqrtTime = sqrt(xTime);

    //logValues = log( sptprice / strike );
    logValues = ( sptprice / strike );

    xLogTerm = logValues;


    xPowerTerm = xVolatility * xVolatility;
    xPowerTerm = xPowerTerm * 0.5;

    xD1 = xRiskFreeRate + xPowerTerm;
    xD1 = xD1 * xTime;
    xD1 = xD1 + xLogTerm;

    xDen = xVolatility * xSqrtTime;
    xD1 = xD1 / xDen;
    xD2 = xD1 -  xDen;

    d1 = xD1;
    d2 = xD2;

    NofXd1 = CNDF( d1 );
    NofXd2 = CNDF( d2 );

    FutureValueX = strike * ( my_exp((float)( -(rate)*(time) )) );        
    if (otype == 0) {            
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else { 
        NegNofXd1 = (1.0 - NofXd1);
        NegNofXd2 = (1.0 - NofXd2);
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    return OptionPrice;
}

#pragma ACCEL kernel
void vec_add_kernel(float * sptprice, float * strike, float * rate, float * volatility, float * otime, float * otype, float * prices)
{
    fptype priceDelta;

    int BLOCK_SIZE = 128;
    int TOTAL_OPTION = 128;

    int k;
    for (k=0; k<TOTAL_OPTION; k++) {
        int ii;
//#pragma ACCEL pipeline
        for (ii=0; ii<BLOCK_SIZE; ii++) {
            fptype price;
            fptype curr_sptprice = sptprice[k*BLOCK_SIZE+ii];
            fptype curr_strike   = strike[k*BLOCK_SIZE+ii];
            fptype curr_rate   = rate[k*BLOCK_SIZE+ii];
            fptype curr_volatility   = volatility[k*BLOCK_SIZE+ii];
            fptype curr_otime   = otime[k*BLOCK_SIZE+ii];
            fptype curr_otype   = otype[k*BLOCK_SIZE+ii];
            price = BlkSchlsEqEuroNoDiv( curr_sptprice, curr_strike,
                    curr_rate , curr_volatility,curr_otime, 
                    curr_otype, 0);
            prices[k*BLOCK_SIZE+ii] = price;
        }

    }

}



int bs_thread(void *tid_ptr) {
    int j;
	int numOptions = PROBLEM_SIZE;

    fptype * otype;
    fptype * sptprice;
    fptype * strike;
    fptype * rate;
    fptype * volatility;
    fptype * otime;
    fptype *prices;
/*  cmost_malloc_1d((void**)&sptprice   ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d((void**)&strike     ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d((void**)&rate       ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d((void**)&volatility ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d((void**)&otime      ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d((void**)&otype      ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d((void**)&prices     ,"zero.dat",       4, PROBLEM_SIZE); 
*/
    
    for (j=0; j<NUM_RUNS; j++) {

        vec_add_kernel( sptprice,  strike,  rate,  volatility,  otime,  otype,  prices);

    }

    //cmost_dump_1d(prices, "prices_out.dat");

    //cmost_free_1d(sptprice   ); 
    //cmost_free_1d(strike     ); 
    //cmost_free_1d(rate       ); 
    //cmost_free_1d(volatility ); 
    //cmost_free_1d(otime      ); 
    //cmost_free_1d(otype      ); 
    //cmost_free_1d(prices     ); 

    return 0;
}



int main ()
{
	bs_thread(0);
    return 0;
}

