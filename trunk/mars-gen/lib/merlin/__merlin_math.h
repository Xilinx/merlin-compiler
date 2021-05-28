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


//#ifndef __HLS_MATH_H__
//#define __HLS_MATH_H__

#ifdef _MATH_H
#include <math.h>
#endif

/****************************************************************
 * Function Name		Supported	Notes		*
 * cos(float/double)		Y				*
 * sin(float/double)		Y				*
 * tan(float/double)		Y				*
 * acos(float/double)		Y				*
 * asin(float/double)		Y				*
 * atan(float/double)		Y				*
 * atan2(float/double)		Y				*
 * cosh(float/double)		Y				*
 * sinh(float/double)		Y				*
 * tanh(float/double)		Y				*
 * acosh(float/double)		N		C++11		*
 * asinh(float/double)		N		C++11		*
 * atanh(float/double)		N		C++11		*
 * exp(float/double)		Y				*
 * frexp(float/double)		Y				*
 * ldexp(float/double)		Y				*
 * log(float/double)		Y				*
 * log10(float/double)		Y				*
 * modf(float/double)		Y				*
 * exp2(float/double)		Y		C++11		*
 * expm1(float/double)		Y		C++11		*
 * ilogb(float/double)		Y		C++11		*
 * log1p(float/double)		N		C++11		*
 * log2(float/double)		Y		C++11		*
 * logb(float/double)		N		C++11		*
 * scalbn(float/double)		N		C++11		*
 * scalbln(float/double)	N		C++11		*
 * pow(float/double)		Y				*
 * sqrt(float/double)		Y				*
 * cbrt(float/double)		Y		C++11		*
 * hypot(float/double)		Y		C++11		*
 * erf(float/double)		N		C++11		*
 * erfc(float/double)		N		C++11		*
 * tgamma(float/double)		N		C++11		*
 * lgamma(float/double)		N		C++11		*
 * ceil(float/double)		Y				*
 * floor(float/double)		Y				*
 * fmod(float/double)		Y				*
 * trunc(float/double)		Y		C++11		*
 * round(float/double)		Y		C++11		*
 * lround(float/double)		N		C++11		*
 * llround(float/double)	N		C++11		*
 * rint(float/double)		Y		C++11		*
 * lrint(float/double)		N		C++11		*
 * llrint(float/double)		N		C++11		*
 * nearbyint(float/double)	N		C++11		*
 * remainder(float/double)	N		C++11		*
 * remquo(float/double)		N		C++11		*
 * copysign(float/double)	N		C++11		*
 * nan(float/double)		N		C++11		*
 * nextafter(float/double)	N		C++11		*
 * nexttoward(float/double)	N		C++11		*
 * fdim(float/double)		Y		C++11		*
 * fmin(float/double)		Y		C++11		*
 * fmax(float/double)		Y		C++11		*
 * fabs(float/double)		Y				*
 * fma(float/double)		N		C++11		*
 * fpclassify(float/double)	N		C++11		*
 * isfinite(float/double)	Y		C++11		*
 * isinf(float/double)		Y		C++11		*
 * isnan(float/double)		Y		C++11		*
 * isnormal(float/double)	N		C++11		*
 * signbit(float/double)	N		C++11		*
 * isgreater(float/double)	N		C++11		*
 * isgreaterequal(float/double)	N		C++11		*
 * isless(float/double)		N		C++11		*
 * islessequal(float/double)	N		C++11		*
 * islessgreater(float/double)	N		C++11		*
 * isunordered(float/double)	N		C++11		*
 ****************************************************************/
/*unsigned long abs(long __x);*/
/* from OpenCL spec */
unsigned long abs_diff(long __x, long __y);
long add_sat(long __x, long __y);
long hadd(long __x, long __y);
long rhadd(long __x, long __y);
long clamp(long __x, long __m, long __n);
long clz(long __x);
long mad_hi(long __a, long __b, long __c);
long mad_sat(long __a, long __b, long __c);
long max(long __x, long __y);
long min(long __x, long __y);
long mul_hi(long __x, long __y);
long rotate(long __v, long __i);
long sub_sat(long __x, long __y);
int popcount(int __x);

/********** Supported float builtins ********/

/********** Classification ******************/
#define fpclassifyf(__x) __undefined_builtin

#define isnormalf(__x) __undefined_builtin

#define signbitf(__x) __undefined_builtin

// int _Z8isfinitef(float __x);
//#define __finitef(__x) _Z8isfinitef(__x)
int finitef(float __x);

// int _Z5isinff(float __x);
//#define __isinff(__x) _Z5isinff(__x)
int isinff(float __x);

// int _Z5isnanf(float __x);
//#define __isnanf(__x) _Z5isnanf(__x)
int isnanf(float __x);

/********** Min, Max, Diff ******************/
// float _Z4fmaxff(float __x, float __y);
//#define fmaxf(__x,__y) _Z4fmaxff(__x, __y)
float fmaxf(float __x, float __y);

// float _Z4fminff(float __x, float __y);
//#define fminf(__x,__y) _Z4fminff(__x, __y)
float fminf(float __x, float __y);

// float _Z9__tg_fdimff(float __x, float __y);
//#define fdimf(__x,__y) _Z9__tg_fdimff(__x, __y)
float fdimf(float __x, float __y);

/************** Power ***********************/
// float _Z9__tg_sqrtf(float __x);
//#define sqrtf(__x) _Z9__tg_sqrtf(__x)
float sqrtf(float __x);

// float  __acl__cbrtf(float);
//#define cbrtf(__x) __acl__cbrtf(__x)
float cbrtf(float);

// float __acl__hypotf(float, float);
//#define hypotf(__x,__y) __acl__hypotf(__x,__y)
float hypotf(float, float);

/******** Rounder & Remainder****************/
// fabsf(__x)  Supported as is. No fpga specific translation needed

#define remainderf(__x, __y) __undefined_builtin

// float _Z9__tg_fmodff(float __x, float __y);
//#define fmodf(__x,__y) _Z9__tg_fmodff(__x, __y)
float fmodf(float __x, float __y);

#define remquof(__x) __undefined_builtin

#define fmaf(__x) __undefined_builtin

// ceilf(__x) Supported as is. No fpga specific translation needed

// floorf(__x) Supported as is. No fpga specific translation needed

// float _Z10__tg_truncf(float __x);
//#define truncf(__x) _Z10__tg_truncf(__x)
float truncf(float __x);

// float _Z5roundf(float __x);
//#define roundf(__x) _Z5roundf(__x)
float roundf(float __x);

#define lroundf(__x) __undefined_builtin

#define llroundf(__x) __undefined_builtin

// float _Z4rintf(float __x);
//#define rintf(__x) _Z4rintf(__x)
float rintf(float __x);

#define lrintf(__x) __undefined_builtin

#define llrintf(__x) __undefined_builtin

#define nearbyintf(__x) __undefined_builtin

// float _Z4modffPf(float __x, float* __y);
//#define modff(__x, __y)   _Z4modffPf(__x, __y)
float modff(float __x, float *__y);

#define scalbnf(__x) __undefined_builtin
#define scalblnf(__x) __undefined_builtin

#define nextafterf(__x) __undefined_builtin

#define nexttowardf(__x) __undefined_builtin

#define copysignf(__x) __undefined_builtin

/*********** Exponential ********************/
// float _Z8__tg_powff(float __x, float __y);
//#define powf(__x,__y) _Z8__tg_powff(__x, __y)
float powf(float __x, float __y);

// float _Z8__tg_expf(float __x);
//#define expf(__x) _Z8__tg_expf(__x)
float expf(float __x);

// float _Z10__tg_exp2ff(float __x);
//#define exp2f(__x) _Z10__tg_exp2ff(__x)
float exp2f(float __x);

// float __acl__expm1f(float);
//#define expm1f(__x) __acl__expm1f(__x)
float expm1f(float);

// float __acl__exp10f(float __x);
//#define exp10f(__x) __acl__exp10f(__x)
float exp10f(float __x);

// float _Z5frexpfPi(float __x, int* __y);
//#define frexpf(__x, __y)  _Z5frexpfPi(__x, __y)
float frexpf(float __x, int *__y);

// ldexpf(__x) Supported as is. No fpga specific translation needed

// float _Z8__tg_logf(float __x);
//#define logf(__x) _Z8__tg_logf(__x)
float logf(float __x);

// float _Z10__tg_log2ff(float __x);
//#define log2f(__x) _Z10__tg_log2ff(__x)
float log2f(float __x);

// float _Z10__tg_log10f(float __x);
//#define log10f(__x) _Z10__tg_log10f(__x)
float log10f(float __x);

#define log1pf(__x) __undefined_builtin

// int _Z5ilogbf(float __x);
//#define ilogbf(__x) _Z5ilogbf(__x)
int ilogbf(float __x);

#define logbf(__x) __undefined_builtin

/************ Float Error & Gamma *********/
#define erff(__x) __undefined_builtin

#define erfcf(__x) __undefined_builtin

#define lgammaf(__x) __undefined_builtin

#define tgammaf(__x) __undefined_builtin

/********** Supported double builtins *******/
/********** Classification ******************/
#define fpclassifyf(__x) __undefined_builtin

#define isnormalf(__x) __undefined_builtin

#define signbitf(__x) __undefined_builtin

// long _Z8isfinited(double __x);
//#define __finite(__x) _Z8isfinited(__x)
//#define isfinite(__x) _Z8isfinited(__x)
//------------long isfinite(double __x);
int isfinite(double __x);

// long _Z5isinfd(double __x);
//#define __isinf(__x) _Z5isinfd(__x)
//#define isinf(__x) _Z5isinfd(__x)
//-----------long isinf(double __x);
int isinf(double __x);

// long _Z5isnand(double __x);
//#define __isnan(__x) _Z5isnand(__x)
//#define isnan(__x) _Z5isnand(__x)
//-----------long isnan(double __x);
int isnan(double __x);

/********** Min, Max, Diff ******************/
#define fma(__x) __undefined_builtin

// double _Z4fmaxdd(double __x, double __y);
//#define fmax(__x,__y) _Z4fmaxdd(__x, __y)
double fmax(double __x, double __y);

// double _Z4fmindd(double __x, double __y);
//#define fmin(__x,__y) _Z4fmindd(__x, __y)
double fmin(double __x, double __y);

// double _Z9__tg_fdimdd(double __x, double __y);
//#define fdim(__x,__y) _Z9__tg_fdimdd(__x, __y)
double fdim(double __x, double __y);

/************** Power ***********************/
// double __acl__sqrtfd(double  __x);
//#define sqrt(__x) __acl__sqrtfd(__x)
double sqrt(double __x);

// Only have the float version built-in.
#define cbrt(__x) (exp(log(__x) / 3.0))

#define hypot(__x, __y) sqrt(__x *__x + __y * __y)

/******** Rounder & Remainder****************/
// double __acl__fabsfd(double __x);
//#define fabs(__x) __acl__fabsfd(__x)
double fabs(double __x);

// double _Z9__tg_fmoddd(double __x, double __y);
//#define fmod(__x,__y) _Z9__tg_fmoddd(__x, __y)
double fmod(double __x, double __y);

#define remainder(__x, __y) __undefined_builtin

#define remquo(__x) __undefined_builtin

// double __acl__ceilfd(double __x);
//#define ceil(__x) __acl__ceilfd(__x)
double ceil(double __x);

// double __acl__floorfd(double __x);
//#define floor(__x) __acl__floorfd(__x)
double floor(double __x);

// double _Z10__tg_truncd(double __x);
//#define trunc(__x) _Z10__tg_truncd(__x)
double trunc(double __x);

// double _Z5roundd(double __x);
//#define round(__x) _Z5roundd(__x)
double round(double __x);

#define lround(__x) __undefined_builtin

#define llround(__x) __undefined_builtin

// double _Z4rintd(double __x);
//#define rint(__x) _Z4rintd(__x)
double rint(double __x);

#define lrint(__x) __undefined_builtin

#define llrint(__x) __undefined_builtin

#define nearbyint(__x) __undefined_builtin

// double _Z4modfdPd(double __x, double * __y);
//#define modf(__x, __y) _Z4modfdPd(__x, __y)
double modf(double __x, double *__y);

#define scalbn(__x) __undefined_builtin
#define scalbln(__x) __undefined_builtin

#define nextafter(__x) __undefined_builtin

#define nexttoward(__x) __undefined_builtin

#define copysign(__x, __y) __undefined_builtin

/*********** Exponential ********************/
// double _Z8__tg_powdd(double __x, double __y);
//#define pow(__x,__y) _Z8__tg_powdd(__x, __y)
double pow(double __x, double __y);

// double __acl__expfd(double __x);
//#define exp(__x) __acl__expfd(__x)
double exp(double __x);

// double _Z9__tg_exp2d(double __x);
//#define exp2(__x) _Z9__tg_exp2d(__x)
double exp2(double __x);

//#define expm1(__x) (__acl__expfd(__x) - 1.0)
#define expm1(__x) (exp(__x) - 1.0)

// double __acl__exp10fd(double __x);
//#define exp10(__x) __acl__exp10fd(__x)
double exp10(double __x);

// double _Z5frexpdPi(double __x, int* __y);
//#define frexp(__x, __y)  _Z5frexpdPi(__x, __y)
double frexp(double __x, int *__y);

// double __acl__ldexpfd(double __x, int __y);
//#define ldexp(__x, __y) __acl__ldexpfd(__x, __y)
double ldexp(double __x, int __y);

// double __acl__logfd(double __x);
//#define log(__x) __acl__logfd(__x)
double log(double __x);

// double _Z9__tg_log2d(double __x);
//#define log2(__x) _Z9__tg_log2d(__x)
double log2(double __x);

// double _Z10__tg_log10d(double __x);
//#define log10(__x) _Z10__tg_log10d(__x)
double log10(double __x);

// double _ZL10__tg_log1pd(double __x);
//#define log1p(__x) _ZL10__tg_log1pd(__x)
double log1p(double __x);

// int _Z5ilogbd(double __x);
//#define ilogb(__x) _Z5ilogbd(__x)
int ilogb(double __x);

#define logb(__x) __undefined_builtin

/************ Double Error & Gamma *********/
#define erf(__x) __undefined_builtin

#define erfc(__x) __undefined_builtin

#define lgamma(__x) __undefined_builtin

#define tgamma(__x) __undefined_builtin

/********** Supported trig builtins **********/

// float _Z8__tg_sinf(float __x);
//#define sinf(__x) _Z8__tg_sinf(__x)
float sinf(float __x);

// float _Z8__tg_cosf(float __x);
//#define cosf(__x) _Z8__tg_cosf(__x)
float cosf(float __x);

// float _Z8__tg_tanf(float __x);
//#define tanf(__x) _Z8__tg_tanf(__x)
float tanf(float __x);

// float _Z9__tg_asinf(float __x);
//#define asinf(__x) _Z9__tg_asinf(__x)
float asinf(float __x);

// float _Z9__tg_acosf(float __x);
//#define acosf(__x) _Z9__tg_acosf(__x)
float acosf(float __x);

// float _Z9__tg_atanf(float __x);
//#define atanf(__x) _Z9__tg_atanf(__x)
float atanf(float __x);

// float __acl__atan2f(float __x, float __y);
//#define atan2f(__x, __y) __acl__atan2f(__x, __y)
float atan2f(float __x, float __y);

// float _Z10__tg_sinhff(float __x);
//#define sinhf(__x) _Z10__tg_sinhff(__x)
float sinhf(float __x);

// float _Z10__tg_coshff(float __x);
//#define coshf(__x) _Z10__tg_coshff(__x)
float coshf(float __x);

// float _Z10__tg_tanhff(float __x);
//#define tanhf(__x) _Z10__tg_tanhff(__x)
float tanhf(float __x);

#define asinhf(__x) __undefined_builtin

#define acoshf(__x) __undefined_builtin

#define atanhf(__x) __undefined_builtin

/********* Double Trig ******/

// double __acl__sinfd(double __x);
//#define sin(__x) __acl__sinfd(__x)
double sin(double __x);

// double __acl__cosfd(double __x);
//#define cos(__x) __acl__cosfd(__x)
double cos(double __x);

// double _Z8__tg_tand(double __x);
//#define tan(__x) _Z8__tg_tand(__x)
double tan(double __x);

// double __acl__asinfd(double __x);
//#define asin(__x) __acl__asinfd(__x)
double asin(double __x);

// double __acl__acosfd(double __x);
//#define acos(__x) __acl__acosfd(__x)
double acos(double __x);

// double __acl__atanfd(double __x);
//#define atan(__x) __acl__atanfd(__x)
double atan(double __x);

// double __acl__atan2fd(double __x, double __y);
//#define atan2(__x, __y) __acl__atan2fd(__x, __y)
double atan2(double __x, double __y);

// double _Z9__tg_sinhd(double __x);
//#define sinh(__x) _Z9__tg_sinhd(__x)
double sinh(double __x);

// double _Z9__tg_coshd(double __x);
//#define cosh(__x) _Z9__tg_coshd(__x)
double cosh(double __x);

// double _Z9__tg_tanhd(double __x);
//#define tanh(__x) _Z9__tg_tanhd(__x)
double tanh(double __x);

#define asinh(__x) __undefined_builtin

#define acosh(__x) __undefined_builtin

#define atanh(__x) __undefined_builtin

/********* Extended Math Functions ******/

