// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


/**
 * @file x_hls_defines.h
 */

#ifndef X_HLS_DEFINES_H
#define X_HLS_DEFINES_H

#include "ap_int.h"

/*
*******************************************************************************
*
* Contains global defines, macos, and structs.
*
*******************************************************************************
*/

#define PRAGMA(x) _Pragma(#x)
/*#define X_PIPELINE(ii) {\
	PRAGMA(AP PIPELINE II=ii)\
	}
*/
//#define NO_FORCING

//#define X_AP_Q_MODE AP_TRN
//#define X_AP_O_MODE AP_WRAP

/*
* May be used to specify all intantiated ap_fixed to have different options.
* Currently unused.



#define COMPARE_APPROX_SGEMM_BASE_FLOAT   0.01
#define COMPARE_APPROX_CGEMM_BASE_FLOAT   0.01
#define COMPARE_APPROX_SGEQRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_CGEQRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_SBSUT_BASE_FLOAT   0.01
#define COMPARE_APPROX_CBSUT_BASE_FLOAT   0.01
#define COMPARE_APPROX_SGETRI_BASE_FLOAT  10.0
#define COMPARE_APPROX_CGETRI_BASE_FLOAT  10.0
#define COMPARE_APPROX_SPBTRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_CPBTRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_CPBTRI_BASE_FLOAT  100.0
#define COMPARE_APPROX_SGESVD_BASE_FLOAT  0.01
#define COMPARE_APPROX_CGESVD_BASE_FLOAT  1.00
#define COMPARE_APPROX_CPOSV_BASE_FLOAT  200.00

#define COMPARE_APPROX_SGEMM_BASE_FIXED   0.01
#define COMPARE_APPROX_CGEMM_BASE_FIXED   0.01
#define COMPARE_APPROX_SGEQRF_BASE_FIXED  0.01
#define COMPARE_APPROX_CGEQRF_BASE_FIXED  0.01
#define COMPARE_APPROX_SBSUT_BASE_FIXED   0.01
#define COMPARE_APPROX_CBSUT_BASE_FIXED   0.01
#define COMPARE_APPROX_SGETRI_BASE_FIXED  10.0
#define COMPARE_APPROX_CGETRI_BASE_FIXED  10.0
#define COMPARE_APPROX_SPBTRF_BASE_FIXED  0.01
#define COMPARE_APPROX_CPBTRF_BASE_FIXED  100.00
#define COMPARE_APPROX_SGESVD_BASE_FIXED  0.01
#define COMPARE_APPROX_CGESVD_BASE_FIXED  1.00

#define COMPARE_APPROX_ERROR_FLOAT 0.10 // in percentage

#define COMPARE_APPROX_ERROR_FIXED 40.00
*/

/*
* Macros
*/
//#define __HLS_MAX(x,y) ((x) > (y) ? (x) : (y))
//#define __HLS_MIN(x,y) ((x) < (y) ? (x) : (y))

/*
* Floating point data structure (with unsigned mantissa)
*/
template<int M, int E>
struct float_struct {
    ap_uint<M>  mant; // 23
    ap_uint<E> exp; // 8
    ap_uint<1> sign;
};

/*
* Floating point data structure (with signed mantissa)
*/
template<int M, int E>
struct float_struct2 {
    ap_int<M>  mant; // 24
    ap_uint<E> exp; // 8
};


#endif



