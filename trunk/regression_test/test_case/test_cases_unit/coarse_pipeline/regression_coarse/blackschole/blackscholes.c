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
// Copyright (c) 2007 Intel Corp.

// Black-Scholes
// Analytical method for calculating European Options
//
// 
// Reference Source: Options, Futures, and Other Derivatives, 3rd Edition, Prentice 
// Hall, John C. Hull,

//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
//#include <string.h>
//#include <sys/time.h>

//Precision to use for calculations
#define fptype float

//#define NUM_RUNS 100
#define NUM_RUNS 1

#define PROBLEM_SIZE 10000

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void bs_kernel(float * sptprice, float * strike, float * rate, float * volatility, float * otime, float * otype, float * prices);

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
    cmost_malloc_1d(&sptprice   ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d(&strike     ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d(&rate       ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d(&volatility ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d(&otime      ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d(&otype      ,"u_in.dat",       4, PROBLEM_SIZE); 
    cmost_malloc_1d(&prices     ,"zero.dat",       4, PROBLEM_SIZE); 

    
    for (j=0; j<NUM_RUNS; j++) {

        bs_kernel( sptprice,  strike,  rate,  volatility,  otime,  otype,  prices);

    }

    cmost_dump_1d(prices, "prices_out.dat");

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

