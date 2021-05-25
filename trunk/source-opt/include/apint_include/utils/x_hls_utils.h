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


#ifndef X_HLS_UTILS_H
#define X_HLS_UTILS_H
#include <stdint.h>
#include "ap_fixed.h"
//#include "hls_half.h"
#include <limits>
#ifndef __SYNTHESIS__
#include <iostream>
#include <cmath>
#endif
class half;
/*
*******************************************************************************
* HLS utilities
* -----------------------
*
* General utilities usable by all functions. Mainly contains classes to
* generate static values from recursive class declarations.
*
* pow2
* sqrtf - C declaration
* to_float
*
* fp_struct<float>
* fp_struct<double>
* fp_struct<half>
* dumpDouble
*
* Power - B^N
* BitWidth = calculate minimum # of bits to store given integer value
*
* Table
*
*******************************************************************************
*/

namespace hls {

    template<typename T>
    class numeric_limits {
    public:
        static T max()    ; 
        static T min()    ; 
        static T epsilon(); 
    };

    template <int W, int I, ap_q_mode Q, ap_o_mode O>
    class numeric_limits<ap_fixed<W,I,Q,O> > {
    public:
        static ap_fixed<W,I,Q,O> max(); 
        static ap_fixed<W,I,Q,O> min(); 
        static ap_fixed<W,I,Q,O> epsilon(); 
    };

    template <int W, int I, ap_q_mode Q, ap_o_mode O>
    class numeric_limits<ap_ufixed<W,I,Q,O> > {
    public:
        static ap_ufixed<W,I,Q,O> max(); 
        static ap_ufixed<W,I,Q,O> min(); 
        static ap_ufixed<W,I,Q,O> epsilon(); 
    };

    template <int W>
    class numeric_limits<ap_int<W> > {
    public:
        static ap_int<W> max(); 
        static ap_int<W> min(); 
        static ap_int<W> epsilon(); 
    };

    template <int W>
    class numeric_limits<ap_uint<W> > {
    public:
        static ap_uint<W> max(); 
        static ap_uint<W> min(); 
        static ap_uint<W> epsilon(); 
    };
}


namespace hlstmp {

    template<typename T>
    class numeric_limits {
    public:
        static T max()    ; 
        static T min()    ; 
        static T epsilon(); 
    };

    template <int W, int I, ap_q_mode Q, ap_o_mode O>
    class numeric_limits<ap_fixed<W,I,Q,O> > {
    public:
        static ap_fixed<W,I,Q,O> max(); 
        static ap_fixed<W,I,Q,O> min(); 
        static ap_fixed<W,I,Q,O> epsilon(); 
    };

    template <int W, int I, ap_q_mode Q, ap_o_mode O>
    class numeric_limits<ap_ufixed<W,I,Q,O> > {
    public:
        static ap_ufixed<W,I,Q,O> max(); 
        static ap_ufixed<W,I,Q,O> min(); 
        static ap_ufixed<W,I,Q,O> epsilon(); 
    };

    template <int W>
    class numeric_limits<ap_int<W> > {
    public:
        static ap_int<W> max(); 
        static ap_int<W> min(); 
        static ap_int<W> epsilon(); 
    };

    template <int W>
    class numeric_limits<ap_uint<W> > {
    public:
        static ap_uint<W> max() ;
        static ap_uint<W> min() ;
        static ap_uint<W> epsilon(); 
    };
}


static inline
const
uint32_t pow2(uint32_t e)
;

template<class T>
T reg(T in)
;

/*
*------------------------------------------------------------------------------
* References C square root function for single precision square root.
* Maps to FPU sqrt core
*------------------------------------------------------------------------------
*/
// extern "C" float sqrtf(float);

/*
*------------------------------------------------------------------------------
* Converts data to float type
*------------------------------------------------------------------------------
*/
static inline
float to_float(float v)
{
    return v;
}

template<int _W, int _I>
float to_float(ap_fixed<_W, _I> v)
{
    return v.to_float();
}


/*
*------------------------------------------------------------------------------
* Single-precision floating point union to convert between 32-bit int and float
*------------------------------------------------------------------------------
*/

template <typename T>
class fp_struct
{
};

union single_cast {
    float f;
    uint32_t i;
};

template <>
class fp_struct<float>
{
public:
    const static int EXP_INFNAN = 255;
    const static int EXP_BIAS = 127;
    const static int EXP_BITS = 8;
    const static int SIG_BITS = 23;
    const static int BITS = 32;

    fp_struct() {
    }
    fp_struct(float f); 
    fp_struct(ap_uint<32> data); 
    fp_struct(uint32_t i); 
    inline ap_uint<32> data() const; 
    inline int expv() const; 
    inline int32_t to_int() const; 
    inline float to_float() const; 
    inline void set_mantissa(ap_ufixed<1+SIG_BITS,1> mantissa); 
    inline ap_ufixed<1+SIG_BITS,1> mantissa() const; 
    inline float to_ieee() const; 
    inline int __signbit() const; 

    static float infinity(); 

    static float minus_infinity(); 

    typedef uint32_t inttype;
    typedef ap_uint<32> data_type;
    ap_uint<1>  sign;
    ap_uint<EXP_BITS> exp;
    ap_uint<SIG_BITS> sig;
};

#ifndef __SYNTHESIS__
static inline
std::ostream &operator << (std::ostream &os, const fp_struct<float> &s)
;
#endif

static inline
void
castSingle(
    float din,
    fp_struct<float> &dout)
;

static inline
float
castSingle(
    fp_struct<float> din)
;

static inline
void
dumpSingle(
    float da,
    fp_struct<float> ds)
;


/*
*------------------------------------------------------------------------------
* Double-precision floating point union to convert between 64-bit int and double
*------------------------------------------------------------------------------
*/
union double_cast {
    double d;
    uint64_t i;
};

template <>
class fp_struct<double>
{
public:
    const static int EXP_INFNAN = 2047;
    const static int EXP_BIAS = 1023;
    const static int EXP_BITS = 11;
    const static int SIG_BITS = 52;
    const static int BITS = 64;

    fp_struct() {
    }
    fp_struct(double f); 
    fp_struct(ap_uint<64> data); 
    fp_struct(uint64_t i); 
    inline ap_uint<64> data() const; 
    inline int64_t to_int() const; 
    inline int expv() const; 
    inline ap_uint<20> sig_msb() const; 
    inline ap_uint<32> sig_lsb() const; 
    inline double to_double() const; 
    inline void set_mantissa(ap_ufixed<1+SIG_BITS,1> mantissa); 
    inline ap_ufixed<1+SIG_BITS,1> mantissa() const; 
    inline double to_ieee() const; 
    inline int __signbit() const; 

    static double infinity(); 

    static double minus_infinity(); 

    typedef uint64_t inttype;
    typedef ap_uint<64> data_type;
    ap_uint<1>  sign;
    ap_uint<EXP_BITS> exp;
    ap_uint<SIG_BITS> sig;
};

#ifndef __SYNTHESIS__
static inline
std::ostream &operator << (std::ostream &os, const fp_struct<double> &s)
;
#endif

static inline
void
castDouble(
    double din,
    fp_struct<double> &dout)
;

static inline
double
castDouble(
    fp_struct<double> din)
;

static inline
void
dumpDouble(
    double da,
    fp_struct<double> ds)
;

/*
*------------------------------------------------------------------------------
* Half-precision floating point union to convert between 16-bit int and half
*------------------------------------------------------------------------------
*/
#define HAS_NO_HALF_TYPE
#ifndef HAS_NO_HALF_TYPE
#ifdef __SYNTHESIS__
union half_cast {
    half d;
    uint16_t i;
};
#endif

template <>
class fp_struct<half>
{
public:
    const static int EXP_INFNAN = 31;
    const static int EXP_BIAS = 15;
    const static int EXP_BITS = 5;
    const static int SIG_BITS = 10;
    const static int BITS = 16;
    
    fp_struct(); 
    fp_struct(half f); 
    fp_struct(ap_uint<16> data); 
    fp_struct(uint16_t i); 
    inline ap_uint<16> data() const; 
    inline int expv() const; 
    inline int16_t to_int() const; 
    inline half to_half() const; 
    inline void set_mantissa(ap_ufixed<1+SIG_BITS,1> mantissa); 
    inline ap_ufixed<1+SIG_BITS,1> mantissa() const; 
    inline half to_ieee() const; 
    inline int __signbit() const; 

    static half infinity(); 

    static half minus_infinity(); 

    typedef uint16_t inttype;
    typedef ap_uint<16> data_type;
    ap_uint<1>  sign;
    ap_uint<EXP_BITS> exp;
    ap_uint<SIG_BITS> sig;
};

#ifndef __SYNTHESIS__
static inline
std::ostream &operator << (std::ostream &os, const fp_struct<half> &s)
;
#endif

static inline
void
castHalf(
    half din,
    fp_struct<half> &dout)
;

static inline
half
castHalf(
    fp_struct<half> din)
;

static inline
void
dumpHalf(
    half da,
    fp_struct<half> ds)
;
#endif


/*
*------------------------------------------------------------------------------
* Raise a value to a certain power (B^N).
*
* This class is necessary in template arguments due to its static const nature
* and the fact that it performs the power operation as part of the recursive
* class declaration. A standard recursive power function would not return
* a static const value.
*
*------------------------------------------------------------------------------
*/
template < unsigned int _Base, unsigned int _Num >
class Power
{
public:
    static const unsigned int Value = _Base * Power< _Base, _Num - 1 >::Value;
};

template < unsigned int _Base >
class Power< _Base, 0 >
{
public:
    static const unsigned int Value = 1;
};

/*
*------------------------------------------------------------------------------
* Calculate the minimum # of bits to store the given integer value.
* UnsignedBitWidth<2>::Value == 3
* UnsignedBitWidth<3>::Value == 3
* UnsignedBitWidth<4>::Value == 4
*------------------------------------------------------------------------------
*/
template  < unsigned int _Num, unsigned int _I=_Num/2>
class BitWidth
{
public:
    static const unsigned int Value = 1 + BitWidth<_Num,_I/2>::Value;
};

template <unsigned int _Num>
class BitWidth<_Num, 0>
{
public:
    static const unsigned int Value = 1;
};

/*
*------------------------------------------------------------------------------
* Calculate the minimum # of bits to store the given unsigned integer value.
* UnsignedBitWidth<2>::Value == 2
* UnsignedBitWidth<3>::Value == 2
* UnsignedBitWidth<4>::Value == 3
*------------------------------------------------------------------------------
*/
template  < unsigned int _Num, unsigned int _I=_Num/2>
class UnsignedBitWidth
{
public:
    static const unsigned int Value = 1 + UnsignedBitWidth<_Num,_I/2>::Value;
};

template <unsigned int _Num>
class UnsignedBitWidth<_Num, 0>
{
public:
    static const unsigned int Value = 0;
};

/*
*------------------------------------------------------------------------------
* return num of bits of a specific type
*
* This class returns the bitwidth result of a specific type
*------------------------------------------------------------------------------
*/
template  < typename T >
class Type_BitWidth
{
public:
    static const unsigned int Value = 8*sizeof(T);
};

template  <int W >
class Type_BitWidth< ap_uint<W> >
{
public:
    static const unsigned int Value = W;
};

template  < int W >
class Type_BitWidth< ap_int<W> >
{
public:
    static const unsigned int Value = W;
};

template  < int W, int I >
class Type_BitWidth< ap_ufixed<W, I> >
{
public:
    static const unsigned int Value = W;
};

template  < int W, int I >
class Type_BitWidth< ap_fixed<W, I> >
{
public:
    static const unsigned int Value = W;
};

/*
*******************************************************************************
* Table class
*
* T: table data type
* N: Depth of table entries
*
* public:
*   dummy    Table data type
*   array[N] Table array
*******************************************************************************
*/

template <typename _T, int _Num, int _I=_Num-1>
class Table : public Table<_T, _Num, _I-1>
{
public:
    typedef typename _T::TableType TableType;
    static const typename _T::TableType dummy;
    static const int size = _Num;

};

template <typename _T, int _Num>
class Table<_T, _Num, 0>
{
public:
    static const typename _T::TableType dummy;
    static typename _T::TableType array[_Num];
};

/*
* Declares the templatized and static Table dummy variable
*
* Assign the global static const Table::dummy variable to the
* associated Table entry by invoking T::apply, assigning it to array[I]
* and adding smaller templates of Table::dummy to it, thereby recursively
* assigning the smaller templates for Tables..
*/

template <typename _T, int _Num, int _I>
const typename _T::TableType Table<_T, _Num, _I>::dummy
    = Table<_T, _Num, 0>::array[_I] = _T::apply(_I,_Num) + 0*Table<_T, _Num, _I-1>::dummy;

template <typename _T, int _Num>
const typename _T::TableType Table<_T, _Num, 0>::dummy
    = Table<_T, _Num, 0>::array[0] = _T::apply(0,_Num);

/*
* Declares the templatized and static Table array
*/
template <typename _T, int _Num>
typename _T::TableType Table<_T, _Num, 0>::array[_Num];

// Some standard junk... this is in the new standard and in boost,
// but for a few definitions it's not worth the dependency
template <class T>
struct is_fptype { static const bool value = false; };
template <> struct is_fptype<float> { static const bool value = true; };
template <> struct is_fptype<double> { static const bool value = true; };
template <> struct is_fptype<half> { static const bool value = true; };

template <class T>
struct is_integraltype { static const bool value = false; };
template <> struct is_integraltype<int> { static const bool value = true; };
template <> struct is_integraltype<unsigned int> { static const bool value = true; };
template <> struct is_integraltype<char> { static const bool value = true; };
template <> struct is_integraltype<signed char> { static const bool value = true; };
template <> struct is_integraltype<unsigned char> { static const bool value = true; };
template <> struct is_integraltype<short> { static const bool value = true; };
template <> struct is_integraltype<unsigned short> { static const bool value = true; };
template <> struct is_integraltype<long> { static const bool value = true; };
template <> struct is_integraltype<unsigned long> { static const bool value = true; };
template <> struct is_integraltype<long long> { static const bool value = true; };
template <> struct is_integraltype<unsigned long long> { static const bool value = true; };
template <int W> struct is_integraltype<ap_int<W> > { static const bool value = true; };
template <int W> struct is_integraltype<ap_uint<W> > { static const bool value = true; };

template <class T>
struct is_fixedtype { static const bool value = false; };
template <int W, int I, ap_q_mode Q, ap_o_mode O> struct is_fixedtype<ap_fixed<W,I,Q,O> > { static const bool value = true; };
template <int W, int I, ap_q_mode Q, ap_o_mode O> struct is_fixedtype<ap_ufixed<W,I,Q,O> > { static const bool value = true; };

namespace hls {
    template<bool B, class T = void>
    struct enable_if {};

    template<class T>
    struct enable_if<true, T> { typedef T type; };
    template<typename T, T _v>
    struct integral_constant
    {
        static const T value = _v;
        typedef T value_type;
        typedef integral_constant<T,_v> type;
        operator value_type() { return value; }
    };
    
    typedef integral_constant<bool, true> true_type;
    typedef integral_constant<bool, false> false_type;
    
    template<typename T1, typename T2>
    struct is_same;
    
    template<typename T1, typename T2>
    struct is_same : public false_type { };
    
    template<typename T1>
    struct is_same<T1,T1> : public true_type { };
    
    template<typename T>
    struct is_arithmetic : public integral_constant<bool, (is_integraltype<T>::value || is_fptype<T>::value)> { };
}

template<typename T1, typename T2>
struct enable_or { static const bool value = T1::value || T2::value; };

template<typename T1, typename T2>
struct enable_and { static const bool value = T1::value && T2::value; };



template<typename T, bool = is_integraltype<T>::value>
struct __promote { typedef double type; };

template<typename T>
struct __promote<T, false> { };

template<>
struct __promote<double> { typedef double type; };

template<>
struct __promote<float> { typedef float type; };

template<>
struct __promote<half> { typedef half type; };



//End standard junk


#endif



