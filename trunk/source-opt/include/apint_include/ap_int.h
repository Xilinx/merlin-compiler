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



#ifndef __AESL_AP_SIM_H__
#define __AESL_AP_SIM_H__


#include "string.h"

#ifdef _MSC_VER
typedef signed __int64 ap_slong;
typedef unsigned __int64 ap_ulong;
#else  // !defined(_MSC_VER)
typedef signed long long ap_slong;
typedef unsigned long long ap_ulong;
#endif // !defined(_MSC_VER)

//typedef int ap_q_mode; 
//typedef int ap_o_mode; 
//#define AP_TRN 0
//#define AP_SAT 0
//#define AP_WRAP 0
enum ap_q_mode { 
    AP_RND, // rounding to plus infinity
    AP_RND_ZERO,// rounding to zero
    AP_RND_MIN_INF,// rounding to minus infinity
    AP_RND_INF,// rounding to infinity
    AP_RND_CONV, // convergent rounding
    AP_TRN, // truncation
    AP_TRN_ZERO // truncation to zero

};
enum ap_o_mode { 
    AP_SAT,                // saturation
    AP_SAT_ZERO,        // saturation to zero
    AP_SAT_SYM,                // symmetrical saturation
    AP_WRAP,                // wrap-around (*)
    AP_WRAP_SM                // sign magnitude wrap-around (*)
};

#define AP_MAX(a, b) ((a) > (b) ? (a) : (b))
#define AP_MIN(a, b) ((a) < (b) ? (a) : (b))

////////////////////////////////////////////////////////////////////////////////////////
/* helper trait. Selecting the smallest C type that can hold the value,
 * return 64 bit C type if not possible.
 */
template <int _AP_N, bool _AP_S>
struct retval;

// at least 64 bit
template <int _AP_N>
struct retval<_AP_N, true> {
  typedef long Type;
};

template <int _AP_N>
struct retval<_AP_N, false> {
  typedef unsigned long Type;
};

// at least 8 bit
template <>
struct retval<1, true> {
  typedef signed char Type;
};

template <>
struct retval<1, false> {
  typedef unsigned char Type;
};

// at least 16 bit
template <>
struct retval<2, true> {
  typedef short Type;
};

template <>
struct retval<2, false> {
  typedef unsigned short Type;
};

// at least 32 bit
template <>
struct retval<3, true> {
  typedef long Type;
};

template <>
struct retval<3, false> {
  typedef unsigned long Type;
};

template <>
struct retval<4, true> {
  typedef long Type;
};

template <>
struct retval<4, false> {
  typedef unsigned long Type;
};


#ifndef __cplusplus
#error C++ is required to include this header file
#else

#ifdef __HLS_SYN__
#error ap_int simulation header file is not applicable for synthesis
#else


//Forward declaration
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q, 
         ap_o_mode _AP_O, int _AP_N> class ap_fixed;
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q, 
         ap_o_mode _AP_O, int _AP_N> class ap_ufixed;
template<int _AP_W> class ap_int;
template<int _AP_W> class ap_uint;

// trait for letting base class to return derived class.
// Notice that derived class template is incomplete, and we cannot use
// the member of the derived class.
template <int _AP_W2, bool _AP_S2>
struct _ap_int_factory;
template <int _AP_W2>
struct _ap_int_factory<_AP_W2,true> { typedef ap_int<_AP_W2> type; };
template <int _AP_W2>
struct _ap_int_factory<_AP_W2,false> { typedef ap_uint<_AP_W2> type; };

template <int _AP_W2, int _AP_I2, bool _AP_S2>
struct _ap_fixed_factory;

template <int _AP_W2, int _AP_I2>
struct _ap_fixed_factory<_AP_W2, _AP_I2, true> {
  typedef ap_fixed<_AP_W2, _AP_I2, AP_TRN, AP_WRAP, 0> type;
};

template <int _AP_W, bool _AP_S>
struct ap_int_base {
  template <int _AP_W2, bool _AP_S2>
  struct RType {
    enum {
      mult_w = _AP_W + _AP_W2,
      mult_s = _AP_S || _AP_S2,
      plus_w =
          AP_MAX(_AP_W + (_AP_S2 && !_AP_S), _AP_W2 + (_AP_S && !_AP_S2)) + 1,
      plus_s = _AP_S || _AP_S2,
      minus_w =
          AP_MAX(_AP_W + (_AP_S2 && !_AP_S), _AP_W2 + (_AP_S && !_AP_S2)) + 1,
      minus_s = true,
      div_w = _AP_W + _AP_S2,
      div_s = _AP_S || _AP_S2,
      mod_w = AP_MIN(_AP_W, _AP_W2 + (!_AP_S2 && _AP_S)),
      mod_s = _AP_S,
      logic_w = AP_MAX(_AP_W + (_AP_S2 && !_AP_S), _AP_W2 + (_AP_S && !_AP_S2)),
      logic_s = _AP_S || _AP_S2
    };

    typedef typename _ap_int_factory<mult_w, mult_s>::type mult;
    typedef typename _ap_int_factory<plus_w, plus_s>::type plus;
    typedef typename _ap_int_factory<minus_w, minus_s>::type minus;
    typedef typename _ap_int_factory<logic_w, logic_s>::type logic;
    typedef typename _ap_int_factory<div_w, div_s>::type div;
    typedef typename _ap_int_factory<mod_w, mod_s>::type mod;
    typedef typename _ap_int_factory<_AP_W, _AP_S>::type arg1;
    typedef bool reduce;
  };
};

/* Will uncomment soon
template <int _AP_W2, int _AP_I2>
struct _ap_fixed_factory<_AP_W2, _AP_I2, false> {
  typedef ap_ufixed<_AP_W2, _AP_I2> type;
};
*/
//AP_INT
//--------------------------------------------------------

#include "ap_int_ref.h"
#include "ap_fixed_ref.h"

template<int _AP_W>
class ap_int {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
    char dummy[(_AP_W + 7) >> 3];
public:
    typedef typename retval<AP_MAX((_AP_W + 7) / 8, 8), true>::Type RetType;

    //Constructor
     ap_int();
     //ap_int(const ap_int<_AP_W>& op) ;
     //ap_int(const volatile ap_int<_AP_W>& op);
    template<int _AP_W2>
     ap_int(const volatile ap_int<_AP_W2> &op);
    
    template<int _AP_W2>
     ap_int(const ap_int<_AP_W2> &op);

    template<int _AP_W2>
     ap_int(const ap_uint<_AP_W2> &op);

    template<int _AP_W2>
     ap_int(const volatile ap_uint<_AP_W2> &op);
    
//    template<int _AP_W2, bool _AP_S2>
//     ap_int(const ap_range_ref<_AP_W2, _AP_S2>& ref);
//
    template<int _AP_W2, bool _AP_S2>
     ap_int(const ap_bit_ref<_AP_W2, _AP_S2>& ref);

//    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
//     ap_int(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& ref);

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
     ap_int(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 ;
 
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
     ap_int(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                  ;

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
     ap_int(const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 ;
 
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
     ap_int(const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                  ;

   template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
            ap_o_mode _AP_O2, int _AP_N2>
    ap_int(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
                 _AP_N2>& op);

//    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
//             ap_o_mode _AP_O2, int _AP_N2>
//     ap_int(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
//                  _AP_N2>& op);

#define CTOR(TYPE) \
     ap_int(TYPE v);
    CTOR(bool)
    CTOR(signed char)
    CTOR(char)
    CTOR(unsigned char)
    CTOR(short)
    CTOR(unsigned short)
    CTOR(int)
    CTOR(unsigned int)
    CTOR(long)
    CTOR(unsigned long)
    CTOR(long long)
    CTOR(unsigned long long)
    CTOR(float)
    CTOR(double)
    CTOR(const char*)
    // CTOR(const std::string&)
#undef CTOR
     ap_int(const char* str, signed char rd);    
    ap_int<_AP_W> read() volatile;
    void write(const ap_int<_AP_W>& op2) volatile;
     //Assignment
     //Another form of "write"
     void operator = (const ap_int<_AP_W>& op2) volatile; 
     void operator = (const ap_uint<_AP_W>& op2) volatile; 
 
     void operator = (const volatile ap_int<_AP_W>& op2) volatile; 
     void operator = (const volatile ap_uint<_AP_W>& op2) volatile; 

    template <int _AP_W2>
     void operator = (const ap_int<_AP_W2>& op2) volatile;
    template <int _AP_W2>
     void operator = (const ap_uint<_AP_W2>& op2) volatile;

    template <int _AP_W2>
     void operator = (const volatile ap_int<_AP_W2>& op2) volatile;
    template <int _AP_W2>
     void operator = (const volatile ap_uint<_AP_W2>& op2) volatile;

     ap_int<_AP_W>& operator = (const volatile ap_int<_AP_W>& op2); 
     ap_int<_AP_W>& operator = (const volatile ap_uint<_AP_W>& op2); 

     ap_int<_AP_W>& operator = (const ap_int<_AP_W>& op2); 
     ap_int<_AP_W>& operator = (const ap_uint<_AP_W>& op2); 

    template <int _AP_W2>
     ap_int<_AP_W>& operator = (const volatile ap_int<_AP_W2>& op2);
    template <int _AP_W2>
     ap_int<_AP_W>& operator = (const volatile ap_uint<_AP_W2>& op2);

    template <int _AP_W2>
     ap_int<_AP_W>& operator = (const ap_int<_AP_W2>& op2);
    template <int _AP_W2>
     ap_int<_AP_W>& operator = (const ap_uint<_AP_W2>& op2);

    ap_int<_AP_W>& operator=(bool op2);
    ap_int<_AP_W>& operator=(char op2);
    ap_int<_AP_W>& operator=(signed char op2);
    ap_int<_AP_W>& operator=(unsigned char op2);
    ap_int<_AP_W>& operator=(short op2);
    ap_int<_AP_W>& operator=(unsigned short op2);
    ap_int<_AP_W>& operator=(int op2);
    ap_int<_AP_W>& operator=(unsigned int op2);
    ap_int<_AP_W>& operator=(long op2);
    ap_int<_AP_W>& operator=(unsigned long op2);
    ap_int<_AP_W>& operator=(ap_slong op2);
    ap_int<_AP_W>& operator=(ap_ulong op2);
    ap_int<_AP_W>& operator=(double op2);
    ap_int<_AP_W>& operator=(float op2);

  template <int _AP_W2, bool _AP_S2>
  ap_int<_AP_W>& operator=(const ap_bit_ref<_AP_W2, _AP_S2>& op2); 

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  ap_int<_AP_W>& operator=(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op);

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  ap_int<_AP_W>& operator=(
      const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op); 

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  ap_int<_AP_W>& operator=(
      const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op); 



  /* FIXME
  template <int _AP_W2, bool _AP_S2>
  INLINE ap_int_base& operator=(const ap_range_ref<_AP_W2, _AP_S2>& op2) {
    Base::V = (ap_int_base<_AP_W2, false>(op2)).V;
    return *this;
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base& operator=(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op2) {
    Base::V = op2.get().V;
    return *this;
  }


  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE ap_int_base& operator=(
      const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op) {
    Base::V = ((const ap_int_base<_AP_W2, false>)(op)).V;
    return *this;
  }
*/

    const ap_int<_AP_W> operator++();
    const ap_int<_AP_W> operator++(int);
    const ap_int<_AP_W> operator--();
    const ap_int<_AP_W> operator--(int);
    ap_int<_AP_W> operator+() const;
    const ap_int<_AP_W + 1> operator-() const;
    bool operator !() const;
    ap_int<_AP_W> operator~() const;

    /* all range() should have returned ap_range_ref<> */
    long & range(int Hi, int Lo);
    long range(int Hi, int Lo) const;
    template <int _AP_W2, int _AP_W3>
     long & range(const ap_int<_AP_W2>& HiIdx, const ap_int<_AP_W3>& LoIdx);

    template <int _AP_W2, int _AP_W3>
     long range(const ap_int<_AP_W2>& HiIdx, const ap_int<_AP_W3>& LoIdx) const;

    long & range();
    long range() const;

    /* both should have returned ap_range_ref<> */
    long & operator()(int Hi, int Lo);
    long operator()(int Hi, int Lo) const;

    /* should have returned ap_range_ref<> */
    template <int _AP_W2, int _AP_W3>
     long & operator()(const ap_int<_AP_W2>& HiIdx, const ap_int<_AP_W3>& LoIdx);

    /* should have returned ap_range_ref<> */
    template <int _AP_W2, int _AP_W3>
     long operator()(const ap_int<_AP_W2>& HiIdx, const ap_int<_AP_W3>& LoIdx) const;

    template <int _AP_W2>
     ap_bit_ref<_AP_W, true> operator[](const ap_int<_AP_W2>& index);
    template <int _AP_W2>
     ap_bit_ref<_AP_W, true> operator[](const ap_uint<_AP_W2>& index);

    template <int _AP_W2>
     bool operator[](const ap_int<_AP_W2>& index) const;
    template <int _AP_W2>
     bool operator[](const ap_uint<_AP_W2>& index) const;

    bool operator[](int index) const;

    ap_bit_ref<_AP_W, true> operator[] (int index);

    operator RetType() const;

    int countLeadingZeros() const;

    ap_bit_ref<_AP_W, true> bit(int index);

    template <int _AP_W2>
     ap_bit_ref<_AP_W, true> bit(const ap_int<_AP_W2>& index);
    template <int _AP_W2>
     ap_bit_ref<_AP_W, true> bit(const ap_uint<_AP_W2>& index);

    bool bit(int index) const;

    template <int _AP_W2>
     bool bit(const ap_int<_AP_W2>& index) const;
    template <int _AP_W2>
     bool bit(const ap_uint<_AP_W2>& index) const;

    /* should have returned ap_concat_ref<> */
    template <int _AP_W2>
     long concat(const ap_int<_AP_W2>& a2) const;

    /* should have returned ap_concat_ref<> */
    template <int _AP_W2>
     long & concat(ap_int<_AP_W2>& a2);

/*
  template <int _AP_W2, bool _AP_S2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, _AP_W2, ap_range_ref<_AP_W2, _AP_S2> >
      operator,(const ap_range_ref<_AP_W2, _AP_S2> &a2) const {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2,
                         ap_range_ref<_AP_W2, _AP_S2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<ap_range_ref<_AP_W2, _AP_S2>&>(a2));
  }

  template <int _AP_W2, bool _AP_S2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, _AP_W2, ap_range_ref<_AP_W2, _AP_S2> >
      operator,(ap_range_ref<_AP_W2, _AP_S2> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2,
                         ap_range_ref<_AP_W2, _AP_S2> >(*this, a2);
  }
 */

    /* should have returned ap_concat_ref<> */
    template <int _AP_W2>
     long & operator,(const ap_int<_AP_W2> &a2);

    /* should have returned ap_concat_ref<> */
    template <int _AP_W2>
     long operator,(ap_int<_AP_W2> &a2) const;

    /* should have returned ap_concat_ref<> */
    template <int _AP_W2>
     long operator,(const ap_int<_AP_W2> &a2) const;

    /* should have returned ap_concat_ref<> */
    template <int _AP_W2>
     long & operator,(ap_int<_AP_W2> &a2);

/*
  template <int _AP_W2, bool _AP_S2>
  INLINE ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >
  operator,(const ap_bit_ref<_AP_W2, _AP_S2> &a2) const {
    return ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<ap_bit_ref<_AP_W2, _AP_S2>&>(a2));
  }

  template <int _AP_W2, bool _AP_S2>
  INLINE ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >
  operator,(ap_bit_ref<_AP_W2, _AP_S2> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >(
        *this, a2);
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                       ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >
  operator,(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                         ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>&>(a2));
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                       ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >
  operator,(ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                         ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >(*this,
                                                                         a2);
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE ap_concat_ref<
      _AP_W, ap_int_base, _AP_W2,
      af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
  operator,(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>
                &a2) const {
    return ap_concat_ref<
        _AP_W, ap_int_base, _AP_W2,
        af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<
            af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>&>(a2));
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE ap_concat_ref<
      _AP_W, ap_int_base, _AP_W2,
      af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
  operator,(af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> &a2) {
    return ap_concat_ref<
        _AP_W, ap_int_base, _AP_W2,
        af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this,
                                                                       a2);
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, 1,
                    af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
      operator,(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>
                    &a2) const {
    return ap_concat_ref<
        _AP_W, ap_int_base, 1,
        af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>&>(
            a2));
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, 1,
                    af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
      operator,(
          af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> &a2) {
    return ap_concat_ref<
        _AP_W, ap_int_base, 1,
        af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this, a2);
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base<AP_MAX(_AP_W2 + _AP_W3, _AP_W), _AP_S> operator&(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& a2) {
    return *this & a2.get();
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base<AP_MAX(_AP_W2 + _AP_W3, _AP_W), _AP_S> operator|(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& a2) {
    return *this | a2.get();
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base<AP_MAX(_AP_W2 + _AP_W3, _AP_W), _AP_S> operator^(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& a2) {
    return *this ^ a2.get();
  }
*/
    int length() const volatile;
    bool iszero() const;
    bool is_zero() const;
    bool sign() const;
    void clear(int i);
    void invert(int i);
    bool test(int i) const;
    ap_int<_AP_W>& get();
    void set(int i);
    void set(int i, bool v);

    template <int _AP_W3>
     void set(const ap_int<_AP_W3>& val);

    bool and_reduce() const;
    bool nand_reduce() const;
    bool or_reduce() const;
    bool nor_reduce() const;
    bool xor_reduce() const;
    bool xnor_reduce() const;

    ap_int<_AP_W>& lrotate(int n);
    ap_int<_AP_W>& rrotate(int n);
    ap_int<_AP_W>& reverse();
    void set_bit(int i, bool v);
    bool get_bit(int i) const;
    void b_not();

    bool to_bool() const;
    char to_char() const;
    signed char to_schar() const;
    unsigned char to_uchar() const;
    short to_short() const;
    unsigned short to_ushort() const;
    int to_int() const;
    unsigned to_uint() const;
    long to_long() const;
    unsigned long to_ulong() const;
    ap_slong to_int64() const;
    ap_ulong to_uint64() const;
    float to_float() const;
    double to_double() const;

#ifndef __SYNTHESIS__
/* Uncomment once Merlin supports 'std::string'
  std::string to_string(signed char rd = 2, bool sign = false) const;
 */
#else
  char* to_string(signed char rd = 2, bool sign = false) const;
#endif
};

//AP_UINT
//---------------------------------------------------------------
template<int _AP_W>
class ap_uint {
    char dummy[(_AP_W + 7) >> 3];
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
public:
    typedef typename retval<AP_MAX((_AP_W + 7) / 8, 8), false>::Type RetType;
  //Constructor
   ap_uint();
   //ap_uint(const ap_uint<_AP_W>& op) ;
   //ap_uint(const volatile ap_uint<_AP_W>& op);
  template<int _AP_W2>
   ap_uint(const volatile ap_uint<_AP_W2> &op);

  template<int _AP_W2>
   ap_uint(const ap_uint<_AP_W2> &op) ;

  template<int _AP_W2>
   ap_uint(const ap_int<_AP_W2> &op) ;

  template<int _AP_W2>
   ap_uint(const volatile ap_int<_AP_W2> &op) ;

//    template<int _AP_W2, bool _AP_S2>
//     ap_uint(const ap_range_ref<_AP_W2, _AP_S2>& ref);
//
    template<int _AP_W2, bool _AP_S2>
     ap_uint(const ap_bit_ref<_AP_W2, _AP_S2>& ref);

//    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
//     ap_uint(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& ref);

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_uint(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 ;

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_uint(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 ;

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_uint(const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 ;

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_uint(const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 ;


  template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_uint(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
                _AP_N2>& op);

//    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
//             ap_o_mode _AP_O2, int _AP_N2>
//     ap_uint(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
//                  _AP_N2>& op);

#define CTOR(TYPE) \
   ap_uint(TYPE v);
  CTOR(bool)
  CTOR(char)
  CTOR(signed char)
  CTOR(unsigned char)
  CTOR(short)
  CTOR(unsigned short)
  CTOR(int)
  CTOR(unsigned int)
  CTOR(long)
  CTOR(unsigned long)
  CTOR(long long)
  CTOR(unsigned long long)
  CTOR(float)
  CTOR(double)
  CTOR(const char*)
  // CTOR(const std::string&)
#undef CTOR    
   ap_uint(const char* str, signed char rd);    
   ap_uint<_AP_W> read() volatile;
   void write(const ap_uint<_AP_W>& op2) volatile;
   //Assignment
   //Another form of "write"
   void operator = (const ap_uint<_AP_W>& op2) volatile; 
   void operator = (const ap_int<_AP_W>& op2) volatile;

   void operator = (const volatile ap_uint<_AP_W>& op2) volatile; 
   void operator = (const volatile ap_int<_AP_W>& op2) volatile;

   template <int _AP_W2>
    void operator = (const ap_uint<_AP_W2>& op2) volatile;
   template <int _AP_W2>
    void operator = (const ap_int<_AP_W2>& op2) volatile;

   template <int _AP_W2>
    void operator = (const volatile ap_uint<_AP_W2>& op2) volatile;
   template <int _AP_W2>
    void operator = (const volatile ap_int<_AP_W2>& op2) volatile;

   ap_uint<_AP_W>& operator = (const volatile ap_uint<_AP_W>& op2); 
   ap_uint<_AP_W>& operator = (const volatile ap_int<_AP_W>& op2);

   ap_uint<_AP_W>& operator = (const ap_uint<_AP_W>& op2); 
   ap_uint<_AP_W>& operator = (const ap_int<_AP_W>& op2);

   template <int _AP_W2>
    ap_uint<_AP_W>& operator = (const volatile ap_uint<_AP_W2>& op2);
   template <int _AP_W2>
    ap_uint<_AP_W>& operator = (const volatile ap_int<_AP_W2>& op2);

   template <int _AP_W2>
    ap_uint<_AP_W>& operator = (const ap_uint<_AP_W2>& op2);
   template <int _AP_W2>
    ap_uint<_AP_W>& operator = (const ap_int<_AP_W2>& op2);

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
   ap_uint& operator=(
       const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op);
  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
   ap_uint& operator=(
       const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op);

   ap_uint<_AP_W>& operator=(bool op2);
   ap_uint<_AP_W>& operator=(char op2);
   ap_uint<_AP_W>& operator=(signed char op2);
   ap_uint<_AP_W>& operator=(unsigned char op2);
   ap_uint<_AP_W>& operator=(short op2);
   ap_uint<_AP_W>& operator=(unsigned short op2);
   ap_uint<_AP_W>& operator=(int op2);
   ap_uint<_AP_W>& operator=(unsigned int op2);
   ap_uint<_AP_W>& operator=(long op2);
   ap_uint<_AP_W>& operator=(unsigned long op2);
   ap_uint<_AP_W>& operator=(ap_slong op2);
   ap_uint<_AP_W>& operator=(ap_ulong op2);
   ap_uint<_AP_W>& operator=(double op2);
   ap_uint<_AP_W>& operator=(float op2);

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  ap_uint<_AP_W>& operator=(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op); 

  template <int _AP_W2, bool _AP_S2>
  ap_uint<_AP_W>& operator=(const ap_bit_ref<_AP_W2, _AP_S2>& op2); 

/* FIXME
  template <int _AP_W2, bool _AP_S2>
  INLINE ap_int_base& operator=(const ap_range_ref<_AP_W2, _AP_S2>& op2) {
    Base::V = (ap_int_base<_AP_W2, false>(op2)).V;
    return *this;
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base& operator=(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op2) {
    Base::V = op2.get().V;
    return *this;
  }


  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE ap_int_base& operator=(
      const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op) {
    Base::V = ((const ap_int_base<_AP_W2, false>)(op)).V;
    return *this;
  }
*/
  const ap_uint<_AP_W> operator++();
  const ap_uint<_AP_W> operator++(int);
  const ap_uint<_AP_W> operator--();
  const ap_uint<_AP_W> operator--(int);
  ap_uint<_AP_W> operator+() const;
  const ap_int<_AP_W + 1> operator-() const;
  bool operator !() const;
  ap_uint<_AP_W> operator~() const;

  /* all range() should have returned ap_range_ref<> */
  unsigned long & range(int Hi, int Lo);
  unsigned long range(int Hi, int Lo) const;
  template <int _AP_W2, int _AP_W3>
   unsigned long & range(const ap_uint<_AP_W2>& HiIdx, const ap_uint<_AP_W3>& LoIdx);

  template <int _AP_W2, int _AP_W3>
   unsigned long range(const ap_uint<_AP_W2>& HiIdx, const ap_uint<_AP_W3>& LoIdx) const;

  unsigned long & range();
  unsigned long range() const;

  /* both should have returned ap_range_ref<> */
  unsigned long & operator()(int Hi, int Lo);
  unsigned long operator()(int Hi, int Lo) const;

  /* should have returned ap_range_ref<> */
  template <int _AP_W2, int _AP_W3>
   unsigned long & operator()(const ap_uint<_AP_W2>& HiIdx, const ap_uint<_AP_W3>& LoIdx);

  /* should have returned ap_range_ref<> */
  template <int _AP_W2, int _AP_W3>
   unsigned long operator()(const ap_uint<_AP_W2>& HiIdx, const ap_uint<_AP_W3>& LoIdx) const;

  template <int _AP_W2>
   ap_bit_ref<_AP_W, false> operator[](const ap_int<_AP_W2>& index);
  template <int _AP_W2>
   ap_bit_ref<_AP_W, false> operator[](const ap_uint<_AP_W2>& index);

  template <int _AP_W2>
   bool operator[](const ap_int<_AP_W2>& index) const;
  template <int _AP_W2>
   bool operator[](const ap_uint<_AP_W2>& index) const;

  bool operator[](int index) const;

  ap_bit_ref<_AP_W, false> operator[](int index);

    operator RetType() const;
    int countLeadingZeros() const;

  ap_bit_ref<_AP_W, false> bit(int index);

  template <int _AP_W2>
   ap_bit_ref<_AP_W, false> bit(const ap_int<_AP_W2>& index);
  template <int _AP_W2>
   ap_bit_ref<_AP_W, false> bit(const ap_uint<_AP_W2>& index);

  bool bit(int index) const;

  template <int _AP_W2>
   bool bit(const ap_int<_AP_W2>& index) const;
  template <int _AP_W2>
   bool bit(const ap_uint<_AP_W2>& index) const;

  /* should have returned ap_concat_ref<> */
  template <int _AP_W2>
   unsigned long concat(const ap_uint<_AP_W2>& a2) const;

  /* should have returned ap_concat_ref<> */
  template <int _AP_W2>
   unsigned long & concat(ap_uint<_AP_W2>& a2);

/*
  template <int _AP_W2, bool _AP_S2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, _AP_W2, ap_range_ref<_AP_W2, _AP_S2> >
      operator,(const ap_range_ref<_AP_W2, _AP_S2> &a2) const {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2,
                         ap_range_ref<_AP_W2, _AP_S2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<ap_range_ref<_AP_W2, _AP_S2>&>(a2));
  }

  template <int _AP_W2, bool _AP_S2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, _AP_W2, ap_range_ref<_AP_W2, _AP_S2> >
      operator,(ap_range_ref<_AP_W2, _AP_S2> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2,
                         ap_range_ref<_AP_W2, _AP_S2> >(*this, a2);
  }
 */

  /* should have returned ap_concat_ref<> */
  template <int _AP_W2>
   unsigned long & operator,(const ap_uint<_AP_W2> &a2);

  /* should have returned ap_concat_ref<> */
  template <int _AP_W2>
   unsigned long operator,(ap_uint<_AP_W2> &a2) const;

  /* should have returned ap_concat_ref<> */
  template <int _AP_W2>
   unsigned long operator,(const ap_uint<_AP_W2> &a2) const;

  /* should have returned ap_concat_ref<> */
  template <int _AP_W2>
   unsigned long & operator,(ap_uint<_AP_W2> &a2);

/*
  template <int _AP_W2, bool _AP_S2>
  INLINE ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >
  operator,(const ap_bit_ref<_AP_W2, _AP_S2> &a2) const {
    return ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<ap_bit_ref<_AP_W2, _AP_S2>&>(a2));
  }

  template <int _AP_W2, bool _AP_S2>
  INLINE ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >
  operator,(ap_bit_ref<_AP_W2, _AP_S2> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, 1, ap_bit_ref<_AP_W2, _AP_S2> >(
        *this, a2);
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                       ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >
  operator,(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                         ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>&>(a2));
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                       ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >
  operator,(ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> &a2) {
    return ap_concat_ref<_AP_W, ap_int_base, _AP_W2 + _AP_W3,
                         ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >(*this,
                                                                         a2);
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE ap_concat_ref<
      _AP_W, ap_int_base, _AP_W2,
      af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
  operator,(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>
                &a2) const {
    return ap_concat_ref<
        _AP_W, ap_int_base, _AP_W2,
        af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<
            af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>&>(a2));
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE ap_concat_ref<
      _AP_W, ap_int_base, _AP_W2,
      af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
  operator,(af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> &a2) {
    return ap_concat_ref<
        _AP_W, ap_int_base, _AP_W2,
        af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this,
                                                                       a2);
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, 1,
                    af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
      operator,(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>
                    &a2) const {
    return ap_concat_ref<
        _AP_W, ap_int_base, 1,
        af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(
        const_cast<ap_int_base<_AP_W, _AP_S>&>(*this),
        const_cast<af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>&>(
            a2));
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE
      ap_concat_ref<_AP_W, ap_int_base, 1,
                    af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
      operator,(
          af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> &a2) {
    return ap_concat_ref<
        _AP_W, ap_int_base, 1,
        af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this, a2);
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base<AP_MAX(_AP_W2 + _AP_W3, _AP_W), _AP_S> operator&(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& a2) {
    return *this & a2.get();
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base<AP_MAX(_AP_W2 + _AP_W3, _AP_W), _AP_S> operator|(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& a2) {
    return *this | a2.get();
  }

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE ap_int_base<AP_MAX(_AP_W2 + _AP_W3, _AP_W), _AP_S> operator^(
      const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& a2) {
    return *this ^ a2.get();
  }
*/
  int length() const volatile;
  bool iszero() const;
  bool is_zero() const;
  bool sign() const;
  void clear(int i);
  void invert(int i);
  bool test(int i) const;
  ap_uint<_AP_W>& get();
  void set(int i);
  void set(int i, bool v);

  template <int _AP_W3>
   void set(const ap_uint<_AP_W3>& val);

  bool and_reduce() const;
  bool nand_reduce() const;
  bool or_reduce() const;
  bool nor_reduce() const;
  bool xor_reduce() const;
  bool xnor_reduce() const;

  ap_uint<_AP_W>& lrotate(int n);
  ap_uint<_AP_W>& rrotate(int n);
  ap_uint<_AP_W>& reverse();
  void set_bit(int i, bool v);
  bool get_bit(int i) const;
    void b_not();

  bool to_bool() const;
  char to_char() const;
  signed char to_schar() const;
  unsigned char to_uchar() const;
  short to_short() const;
  unsigned short to_ushort() const;
  int to_int() const;
  unsigned to_uint() const;
  long to_long() const;
  unsigned long to_ulong() const;
  ap_slong to_int64() const;
  ap_ulong to_uint64() const;
  float to_float() const;
  double to_double() const;

#ifndef __SYNTHESIS__
/* Uncomment once Merlin supports 'std::string'
  std::string to_string(signed char rd = 2, bool sign = false) const;
 */
#else
  char* to_string(signed char rd = 2, bool sign = false) const;
#endif
};

#define ap_bigint ap_int
#define ap_biguint ap_uint

//AP_FIXED
//---------------------------------------------------------------------  
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q = AP_TRN, 
       ap_o_mode _AP_O = AP_WRAP, int _AP_N = 0>
class ap_fixed {
    char dummy[(_AP_W + 7) >> 3];
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
public:
#define _AP_S true
  template <int _AP_W2, int _AP_I2, bool _AP_S2>
  struct RType {
    enum {
      _AP_F = _AP_W - _AP_I,
      F2 = _AP_W2 - _AP_I2,
      mult_w = _AP_W + _AP_W2,
      mult_i = _AP_I + _AP_I2,
      mult_s = _AP_S || _AP_S2,
      plus_w = AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) +
	       1 + AP_MAX(_AP_F, F2),
      plus_i =
	  AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) + 1,
      plus_s = _AP_S || _AP_S2,
      minus_w =
	  AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) + 1 +
	  AP_MAX(_AP_F, F2),
      minus_i =
	  AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) + 1,
      minus_s = true,
#ifndef __SC_COMPATIBLE__
      div_w = _AP_S2 + _AP_W + AP_MAX(F2, 0),
#else
      div_w = _AP_S2 + _AP_W + AP_MAX(F2, 0) + AP_MAX(_AP_I2, 0),
#endif
      div_i = _AP_S2 + _AP_I + F2,
      div_s = _AP_S || _AP_S2,
      logic_w =
	  AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) +
	  AP_MAX(_AP_F, F2),
      logic_i = AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)),
      logic_s = _AP_S || _AP_S2
    };

    typedef ap_fixed<_AP_W, _AP_I> lhs;
    typedef ap_fixed<_AP_W2, _AP_I2> rhs;

    typedef ap_fixed<mult_w, mult_i> mult_base;
    typedef ap_fixed<plus_w, plus_i> plus_base;
    typedef ap_fixed<minus_w, minus_i> minus_base;
    typedef ap_fixed<logic_w, logic_i> logic_base;
    typedef ap_fixed<div_w, div_i> div_base;
    typedef ap_fixed<_AP_W, _AP_I> arg1_base;

    typedef typename _ap_fixed_factory<mult_w, mult_i, mult_s>::type mult;
    typedef typename _ap_fixed_factory<plus_w, plus_i, plus_s>::type plus;
    typedef typename _ap_fixed_factory<minus_w, minus_i, minus_s>::type minus;
    typedef typename _ap_fixed_factory<logic_w, logic_i, logic_s>::type logic;
    typedef typename _ap_fixed_factory<div_w, div_i, div_s>::type div;
    typedef typename _ap_fixed_factory<_AP_W, _AP_I, _AP_S>::type arg1;
  };
#undef _AP_S


   ap_fixed();
   //ap_fixed(const volatile ap_fixed<_AP_W, _AP_I,  _AP_Q, _AP_O, 
   //               _AP_N>& op);
   //ap_fixed(const ap_fixed<_AP_W, _AP_I,  _AP_Q, _AP_O, 
   //               _AP_N>& op);

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
          ap_o_mode _AP_O2, int _AP_N2>
   ap_fixed(const ap_fixed<_AP_W2, _AP_I2,  _AP_Q2, _AP_O2, 
                  _AP_N2>& op);

  
  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_fixed(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2,
                  _AP_N2>& op);

  template<int _AP_W2>
   ap_fixed(const ap_int<_AP_W2>& op);

  template<int _AP_W2>
   ap_fixed(const ap_uint<_AP_W2>& op);

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
          ap_o_mode _AP_O2, int _AP_N2>
   ap_fixed(const volatile ap_fixed<_AP_W2, _AP_I2,  _AP_Q2, _AP_O2, 
                  _AP_N2>& op);

  
  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_fixed(const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2,
                  _AP_N2>& op);

  template<int _AP_W2>
   ap_fixed(const volatile ap_int<_AP_W2>& op);

  template<int _AP_W2>
   ap_fixed(const volatile ap_uint<_AP_W2>& op);

//    template<int _AP_W2, bool _AP_S2>
//     ap_fixed(const ap_bit_ref<_AP_W2, _AP_S2>& op);
//
//    template<int _AP_W2, bool _AP_S2>
//     ap_fixed(const ap_range_ref<_AP_W2, _AP_S2>& op);
//
//    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
//     ap_fixed(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op);
//
   template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
            ap_o_mode _AP_O2, int _AP_N2>
    ap_fixed(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, 
                   _AP_Q2, _AP_O2, _AP_N2>& op);

//    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
//             ap_o_mode _AP_O2, int _AP_N2>
//     ap_fixed(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, 
//                    _AP_Q2, _AP_O2, _AP_N2>& op);


#define CTOR(TYPE) \
   ap_fixed(TYPE v);
  CTOR(bool)
  CTOR(char)
  CTOR(signed char)
  CTOR(unsigned char)
  CTOR(short)
  CTOR(unsigned short)
  CTOR(int)
  CTOR(unsigned int)
  CTOR(long)
  CTOR(unsigned long)
  CTOR(long long)
  CTOR(unsigned long long)
  CTOR(float)
  CTOR(double)
  CTOR(const char*)
  // CTOR(const std::string&)
#undef CTOR    
   ap_fixed(const char* str, signed char rd);    

  //Assignment
    template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
	      ap_o_mode _AP_O2, int _AP_N2>
     ap_fixed& operator=(
       const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op);

    template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
	      ap_o_mode _AP_O2, int _AP_N2>
     ap_fixed& operator=(
	const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op);

    ap_fixed& operator = (const ap_fixed<_AP_W, _AP_I, 
		   _AP_Q, _AP_O, _AP_N>& op); 

   ap_fixed& operator = (const volatile ap_fixed<_AP_W, _AP_I,
                  _AP_Q, _AP_O, _AP_N>& op); 

   void operator = (const ap_fixed<_AP_W, _AP_I, 
                  _AP_Q, _AP_O, _AP_N>& op) volatile; 

   void operator = (const volatile ap_fixed<_AP_W, _AP_I,
                  _AP_Q, _AP_O, _AP_N>& op) volatile; 

  long & range(int i, int j) ;
  long & range() ;
  long & operator()(int i, int j) ;

  const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator++();
  const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator++(int);
  const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator--();
  const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator--(int);
  ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator+();
  const ap_fixed<_AP_W + 1, _AP_I, _AP_Q, _AP_O, _AP_N> operator-() const;
  ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> getNeg();
  bool operator !() const;
  ap_fixed<_AP_W, _AP_I> operator~() const;

  template <int _AP_SHIFT>
   ap_fixed<_AP_W, _AP_I + _AP_SHIFT> lshift() const;

  template <int _AP_SHIFT>
   ap_fixed<_AP_W, _AP_I - _AP_SHIFT> rshift() const;

  af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> operator[](unsigned index);

  template <int _AP_W2, bool _AP_S2>
   af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> operator[](const ap_int<_AP_W2>& index);
  template <int _AP_W2, bool _AP_S2>
   af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> operator[](const ap_uint<_AP_W2>& index);

  af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> bit(unsigned index);

  template <int _AP_W2, bool _AP_S2>
   af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> bit(const ap_int<_AP_W2>& index);
  template <int _AP_W2, bool _AP_S2>
   af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> bit(const ap_uint<_AP_W2>& index);

  template <int _AP_W2>
   af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> get_bit(const ap_int<_AP_W2>& index);

/* FIXME
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(int Hi, int Lo);

  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(int Hi, int Lo) const;

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
   INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(
       const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
       const ap_int_base<_AP_W3, _AP_S3>& LoIdx);

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
   INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(
       const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
       const ap_int_base<_AP_W3, _AP_S3>& LoIdx) const;

  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range();
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range() const;

  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(int Hi, int Lo);
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(int Hi, int Lo) const;

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
   INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(
       const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
       const ap_int_base<_AP_W3, _AP_S3>& LoIdx);

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
   INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(
       const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
       const ap_int_base<_AP_W3, _AP_S3>& LoIdx) const;
 */

  bool operator[](unsigned index) const;
  bool bit(unsigned index) const;
  bool get_bit(int index) const;

  template <int _AP_W2>
   bool get_bit(const ap_int<_AP_W2>& index) const;

  bool is_zero() const;
  bool is_neg() const;
  int wl() const;
  int iwl() const;
  ap_q_mode q_mode() const;
  ap_o_mode o_mode() const;
  int n_bits() const;

#ifndef __SYNTHESIS__
/* Uncomment once Merlin supports 'std::string'
  std::string to_string(unsigned char radix = 2, bool sign = false) const;
 */
#else
  char* to_string(unsigned char radix = 2, bool sign = false) const;
#endif

  operator float() const;
  float to_float() const;
  double to_double() const;
    void b_not();
    ap_fixed& setBits(ap_ulong bv);
    static ap_fixed bitsToFixed(ap_ulong bv);

/*FIXME
  template <int _AP_W2, bool _AP_S2>
  INLINE operator ap_int_base<_AP_W2, _AP_S2>() const {
    return ap_int_base<_AP_W2, _AP_S2>(to_ap_int_base());
  }
 */

    char to_char() const;
    int to_int() const;
    unsigned to_uint() const;
    ap_slong to_int64() const;
    ap_ulong to_uint64() const;
/* FIXME
    half to_half() const;
    operator half() const;
 */
    operator long double() const;
    operator double() const;
    operator bool() const;
    operator char() const;
    operator signed char() const;
    operator unsigned char() const;
    operator short() const;
    operator unsigned short() const;
    operator int() const;
    operator unsigned int() const;
#ifdef __x86_64__
    operator long() const;
    operator unsigned long() const;
#else
    operator long() const;
    operator unsigned long() const;
#endif // ifdef __x86_64__ else

    operator ap_ulong() const;
    operator ap_slong() const;
    int length() const;

#ifndef __SYNTHESIS__
    ap_ulong bits_to_uint64() const;
#endif

    int countLeadingZeros();
};    
//AP_ UFIXED
//--- ----------------------------------------------------------------
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q = AP_TRN,
       ap_o_mode _AP_O = AP_WRAP, int _AP_N = 0>
class ap_ufixed{
    char dummy[(_AP_W + 7) >> 3];
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
public:
#define _AP_S false
  template <int _AP_W2, int _AP_I2, bool _AP_S2>
  struct RType {
    enum {
      _AP_F = _AP_W - _AP_I,
      F2 = _AP_W2 - _AP_I2,
      mult_w = _AP_W + _AP_W2,
      mult_i = _AP_I + _AP_I2,
      mult_s = _AP_S || _AP_S2,
      plus_w = AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) +
               1 + AP_MAX(_AP_F, F2),
      plus_i =
          AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) + 1,
      plus_s = _AP_S || _AP_S2,
      minus_w =
          AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) + 1 +
          AP_MAX(_AP_F, F2),
      minus_i =
          AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) + 1,
      minus_s = true,
#ifndef __SC_COMPATIBLE__
      div_w = _AP_S2 + _AP_W + AP_MAX(F2, 0),
#else
      div_w = _AP_S2 + _AP_W + AP_MAX(F2, 0) + AP_MAX(_AP_I2, 0),
#endif
      div_i = _AP_S2 + _AP_I + F2,
      div_s = _AP_S || _AP_S2,
      logic_w =
          AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)) +
          AP_MAX(_AP_F, F2),
      logic_i = AP_MAX(_AP_I + (_AP_S2 && !_AP_S), _AP_I2 + (_AP_S && !_AP_S2)),
      logic_s = _AP_S || _AP_S2
    };

    typedef ap_ufixed<_AP_W, _AP_I> lhs;
    typedef ap_ufixed<_AP_W2, _AP_I2> rhs;

    typedef ap_ufixed<mult_w, mult_i> mult_base;
    typedef ap_ufixed<plus_w, plus_i> plus_base;
    typedef ap_ufixed<minus_w, minus_i> minus_base;
    typedef ap_ufixed<logic_w, logic_i> logic_base;
    typedef ap_ufixed<div_w, div_i> div_base;
    typedef ap_ufixed<_AP_W, _AP_I> arg1_base;

    typedef typename _ap_fixed_factory<mult_w, mult_i, mult_s>::type mult;
    typedef typename _ap_fixed_factory<plus_w, plus_i, plus_s>::type plus;
    typedef typename _ap_fixed_factory<minus_w, minus_i, minus_s>::type minus;
    typedef typename _ap_fixed_factory<logic_w, logic_i, logic_s>::type logic;
    typedef typename _ap_fixed_factory<div_w, div_i, div_s>::type div;
    typedef typename _ap_fixed_factory<_AP_W, _AP_I, _AP_S>::type arg1;
  };
#undef _AP_S

   //Constructor
   ap_ufixed();
   
   //ap_ufixed(const volatile ap_ufixed<_AP_W, _AP_I,  _AP_Q, _AP_O, 
   //               _AP_N>& op);
   //ap_ufixed(const ap_ufixed<_AP_W, _AP_I,  _AP_Q, _AP_O, 
   //               _AP_N>& op);

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_ufixed(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, 
                   _AP_O2, _AP_N2>& op);
   
   
  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_ufixed(const ap_ufixed<_AP_W2, _AP_I2,  _AP_Q2, 
                   _AP_O2, _AP_N2>& op);
   
  template<int _AP_W2>
   ap_ufixed(const ap_int<_AP_W2>& op);

  template<int _AP_W2>
   ap_ufixed(const ap_uint<_AP_W2>& op);

  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_ufixed(const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, 
                   _AP_O2, _AP_N2>& op);
   
   
  template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_ufixed(const volatile ap_ufixed<_AP_W2, _AP_I2,  _AP_Q2, 
                   _AP_O2, _AP_N2>& op) ;
   
  template<int _AP_W2>
   ap_ufixed(const volatile ap_int<_AP_W2>& op);

  template<int _AP_W2>
   ap_ufixed(const volatile ap_uint<_AP_W2>& op);

//    template<int _AP_W2, bool _AP_S2>
//     ap_ufixed(const ap_bit_ref<_AP_W2, _AP_S2>& op);
//
//    template<int _AP_W2, bool _AP_S2>
//     ap_ufixed(const ap_range_ref<_AP_W2, _AP_S2>& op);
//
//    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
//     ap_ufixed(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op);
//
  template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
           ap_o_mode _AP_O2, int _AP_N2>
   ap_ufixed(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, 
                  _AP_Q2, _AP_O2, _AP_N2>& op);
//
//    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
//             ap_o_mode _AP_O2, int _AP_N2>
//     ap_ufixed(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, 
//                    _AP_Q2, _AP_O2, _AP_N2>& op);



#define CTOR(TYPE) \
   ap_ufixed(TYPE v);
  CTOR(bool)
  CTOR(char)
  CTOR(signed char)
  CTOR(unsigned char)
  CTOR(short)
  CTOR(unsigned short)
  CTOR(int)
  CTOR(unsigned int)
  CTOR(long)
  CTOR(unsigned long)
  CTOR(long long)
  CTOR(unsigned long long)
  CTOR(float)
  CTOR(double)
  CTOR(const char*)
  // CTOR(const std::string&)
#undef CTOR    
   ap_ufixed(const char* str, signed char rd);    
   
  //Assignment
  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
   ap_ufixed& operator=(
      const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op);

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
   ap_ufixed& operator=(
      const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op);

   ap_ufixed& operator = (const ap_ufixed<_AP_W, _AP_I, 
                  _AP_Q, _AP_O, _AP_N>& op); 

   ap_ufixed& operator = (const volatile ap_ufixed<_AP_W, _AP_I, 
                  _AP_Q, _AP_O, _AP_N>& op); 

   void operator = (const ap_ufixed<_AP_W, _AP_I, 
                  _AP_Q, _AP_O, _AP_N>& op) volatile; 

   void operator = (const volatile ap_ufixed<_AP_W, _AP_I, 
                  _AP_Q, _AP_O, _AP_N>& op) volatile; 

  template <int _AP_W2>
   operator ap_int<_AP_W2>() const;

  const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator++();
  const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator++(int);
  const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator--();
  const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> operator--(int);
  const ap_ufixed<_AP_W + 1, _AP_I, _AP_Q, _AP_O, _AP_N> operator-() const;
  ap_ufixed operator+();
  bool operator !() const;
  ap_ufixed<_AP_W, _AP_I> operator~() const;

  af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> operator[](unsigned index);

  template <int _AP_W2, bool _AP_S2>
  af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> operator[](
      const ap_int<_AP_W2>& index);
  template <int _AP_W2, bool _AP_S2>
  af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> operator[](
      const ap_uint<_AP_W2>& index);

  af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> bit(unsigned index);

  template <int _AP_W2, bool _AP_S2>
  af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> bit(
      const ap_int<_AP_W2>& index);
  template <int _AP_W2, bool _AP_S2>
  af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> bit(
      const ap_uint<_AP_W2>& index);

  template <int _AP_W2>
  af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> get_bit(
      const ap_int<_AP_W2>& index);

/* FIXME
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(int Hi, int Lo);

  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(int Hi, int Lo) const;

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(
      const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
      const ap_int_base<_AP_W3, _AP_S3>& LoIdx);

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range(
      const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
      const ap_int_base<_AP_W3, _AP_S3>& LoIdx) const;

  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range();
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> range() const;

  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(int Hi, int Lo);
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(int Hi, int Lo) const;

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(
      const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
      const ap_int_base<_AP_W3, _AP_S3>& LoIdx);

  template <int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
  INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> operator()(
      const ap_int_base<_AP_W2, _AP_S2>& HiIdx,
      const ap_int_base<_AP_W3, _AP_S3>& LoIdx) const;
 */

  bool bit(unsigned index) const;
  bool get_bit(int index) const;

  template <int _AP_W2>
   bool get_bit(const ap_int<_AP_W2>& index) const;

  bool operator[](unsigned index) const;

  unsigned long & range(int i, int j) ;
  unsigned long & range() ;
  unsigned long & operator()(int i, int j) ;

  operator float() const;
  operator long double() const;
  operator double() const;
/* FIXME
  operator half() const;
 */
  operator bool() const;
  operator char() const;
  operator signed char() const;
  operator unsigned char() const;
  operator short() const;
  operator unsigned short() const;
  operator int() const;
  operator unsigned int() const;
#ifdef __x86_64__
  operator long() const;
  operator unsigned long() const;
#else
  operator long() const;
  operator unsigned long() const;
#endif // ifdef __x86_64__ else

  operator ap_ulong() const;
  operator ap_slong() const;

  float to_float() const;
  double to_double() const;
/* FIXME
  half to_half() const;
 */

  char to_char() const;
  int to_int() const;
  unsigned to_uint() const;
  ap_slong to_int64() const;
  ap_ulong to_uint64() const;


    void b_not();
#ifndef __SYNTHESIS__
    ap_ulong bits_to_uint64() const;
#endif
    int countLeadingZeros();
    ap_ufixed& setBits(ap_ulong bv);
    static ap_ufixed bitsToFixed(ap_ulong bv);
    ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> getNeg();
    int length() const;

    template <int _AP_SHIFT>
     ap_ufixed<_AP_W, _AP_I + _AP_SHIFT> lshift() const;

    template <int _AP_SHIFT>
     ap_ufixed<_AP_W, _AP_I - _AP_SHIFT> rshift() const;

    bool is_zero() const;
    bool is_neg() const;
    int wl() const;
    int iwl() const;
    ap_q_mode q_mode() const;
    ap_o_mode o_mode() const;
    int n_bits() const;

#ifndef __SYNTHESIS__
/* Uncomment once Merlin supports 'std::string'
  std::string to_string(unsigned char radix = 2, bool sign = false) const;
 */
#else
  char* to_string(unsigned char radix = 2, bool sign = false) const;
#endif
};

#define AP_SHIFT_OP(sym, c_type) \
template<int W1> \
ap_int<W1> operator sym (ap_int<W1> a, c_type b); \
template<int W1> \
ap_uint<W1> operator sym (ap_uint<W1> a, c_type b); \
template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1> \
ap_fixed<W1, I1, Q1, O1, N1> operator sym (ap_fixed<W1, I1, Q1, O1, N1> a, c_type b); \
template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1> \
ap_ufixed<W1, I1, Q1, O1, N1> operator sym (ap_ufixed<W1, I1, Q1, O1, N1> a, c_type b); 
#define AP_SHIFT_OP_C_TYPE(c_type) \
AP_SHIFT_OP(>>, c_type) \
AP_SHIFT_OP(<<, c_type)

AP_SHIFT_OP_C_TYPE(char)
AP_SHIFT_OP_C_TYPE(signed char)
AP_SHIFT_OP_C_TYPE(unsigned char)
AP_SHIFT_OP_C_TYPE(short)
AP_SHIFT_OP_C_TYPE(unsigned short)
AP_SHIFT_OP_C_TYPE(int)
AP_SHIFT_OP_C_TYPE(unsigned int)
AP_SHIFT_OP_C_TYPE(long)
AP_SHIFT_OP_C_TYPE(unsigned long)
AP_SHIFT_OP_C_TYPE(long long)
AP_SHIFT_OP_C_TYPE(unsigned long long)
#undef AP_SHIFT_OP
#undef AP_SHIFT_OP_C_TYPE

#define AP_SHIFT_ASSIGN_OP(sym, c_type) \
  template<int W1> \
  ap_int<W1>& operator sym (ap_int<W1> &a, c_type b); \
  template<int W1> \
  ap_uint<W1>& operator sym (ap_uint<W1> &a, c_type b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1, I1, Q1, O1, N1>& operator sym (ap_fixed<W1, I1, Q1, O1, N1> &a, c_type b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_ufixed<W1, I1, Q1, O1, N1>& operator sym (ap_ufixed<W1, I1, Q1, O1, N1> &a, c_type b); 
#define AP_SHIFT_ASSIGN_OP_C_TYPE(c_type) \
AP_SHIFT_ASSIGN_OP(>>=, c_type) \
AP_SHIFT_ASSIGN_OP(<<=, c_type)

AP_SHIFT_ASSIGN_OP_C_TYPE(char)
AP_SHIFT_ASSIGN_OP_C_TYPE(signed char)
AP_SHIFT_ASSIGN_OP_C_TYPE(unsigned char)
AP_SHIFT_ASSIGN_OP_C_TYPE(short)
AP_SHIFT_ASSIGN_OP_C_TYPE(unsigned short)
AP_SHIFT_ASSIGN_OP_C_TYPE(int)
AP_SHIFT_ASSIGN_OP_C_TYPE(unsigned int)
AP_SHIFT_ASSIGN_OP_C_TYPE(long)
AP_SHIFT_ASSIGN_OP_C_TYPE(unsigned long)
AP_SHIFT_ASSIGN_OP_C_TYPE(long long)
AP_SHIFT_ASSIGN_OP_C_TYPE(unsigned long long)
#undef AP_SHIFT_ASSIGN_OP
#undef AP_SHIFT_ASSIGN_OP_C_TYPE

#define AP_BINARY_OP(sym) \
  template<int W1, int W2> \
  ap_int<W1 + W2> operator sym (ap_int<W1> a, ap_int<W2> b); \
  template<int W1, int W2> \
  ap_int<W1 + W2> operator sym (ap_int<W1> a, ap_uint<W2> b); \
  template<int W1, int W2> \
  ap_int<W1 + W2> operator sym (ap_uint<W1> a, ap_uint<W2> b); \
  template<int W1, int W2> \
  ap_int<W1 + W2> operator sym (ap_uint<W1> a, ap_int<W2> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_uint<W1> a, ap_fixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_int<W1> a, ap_fixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_uint<W1> a, ap_ufixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_int<W1> a, ap_ufixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_fixed<W2, I2, Q1, O1, N1> b, ap_uint<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_fixed<W2, I2, Q1, O1, N1> b, ap_int<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_ufixed<W2, I2, Q1, O1, N1> b, ap_uint<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_ufixed<W2, I2, Q1, O1, N1> b, ap_int<W1> a); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_fixed<W1, I1, Q1, O1, N1> a, ap_ufixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_ufixed<W1, I1, Q1, O1, N1> a, ap_ufixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_fixed<W1, I1, Q1, O1, N1> a, ap_fixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_fixed<W1 + W2, I2, Q1, O1, N1> operator sym (ap_ufixed<W1, I1, Q1, O1, N1> a, ap_fixed<W2, I2, Q2, O2, N2> b);
AP_BINARY_OP(+)
AP_BINARY_OP(-)
AP_BINARY_OP(*)
AP_BINARY_OP(/)
AP_BINARY_OP(%) \
AP_BINARY_OP(|) \
AP_BINARY_OP(^) \
AP_BINARY_OP(&) 
#undef AP_BINARY_OP

#define AP_ASSIGN_OP(sym) \
  template<int W1, int W2> \
  ap_int<W1>& operator sym (ap_int<W1> &a, ap_int<W2> b); \
  template<int W1, int W2> \
  ap_int<W1>& operator sym (ap_int<W1> &a, ap_uint<W2> b); \
  template<int W1, int W2> \
  ap_uint<W1>& operator sym (ap_uint<W1> &a, ap_uint<W2> b); \
  template<int W1, int W2> \
  ap_uint<W1>& operator sym (ap_uint<W1> &a, ap_int<W2> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_uint<W1>& operator sym (ap_uint<W1> &a, ap_fixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_int<W1>& operator sym (ap_int<W1> &a, ap_fixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_uint<W1>& operator sym (ap_uint<W1> &a, ap_ufixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_int<W1>& operator sym (ap_int<W1>& a, ap_ufixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W2, I2, Q1, O1, N1>& operator sym (ap_fixed<W2, I2, Q1, O1, N1>& b, ap_uint<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W2, I2, Q1, O1, N1>& operator sym (ap_fixed<W2, I2, Q1, O1, N1>& b, ap_int<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_ufixed<W2, I2, Q1, O1, N1>& operator sym (ap_ufixed<W2, I2, Q1, O1, N1>& b, ap_uint<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_ufixed<W2, I2, Q1, O1, N1>& operator sym (ap_ufixed<W2, I2, Q1, O1, N1> &b, ap_int<W1> a); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_fixed<W1, I1, Q1, O1, N1>& operator sym (ap_fixed<W1, I1, Q1, O1, N1>& a, ap_ufixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_ufixed<W1, I2, Q1, O1, N1> operator sym (ap_ufixed<W1, I1, Q1, O1, N1> &a, ap_ufixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_fixed<W1, I2, Q1, O1, N1> operator sym (ap_fixed<W1, I1, Q1, O1, N1> &a, ap_fixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  ap_ufixed<W1, I2, Q1, O1, N1> operator sym (ap_ufixed<W1, I1, Q1, O1, N1> &a, ap_fixed<W2, I2, Q2, O2, N2> b);
  AP_ASSIGN_OP(+=)
  AP_ASSIGN_OP(-=)
  AP_ASSIGN_OP(*=)
  AP_ASSIGN_OP(/=)
  AP_ASSIGN_OP(%=)
  AP_ASSIGN_OP(|=)
  AP_ASSIGN_OP(^=)
  AP_ASSIGN_OP(&=)
#undef AP_ASSIGN_OP
#define AP_ASSIGN_OP(sym, c_type) \
  template<int W1> \
  ap_int<W1>& operator sym (ap_int<W1> &a, c_type b); \
  template<int W1> \
  ap_uint<W1>& operator sym (ap_uint<W1> &a, c_type b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_fixed<W1, I1, Q1, O1, N1>& operator sym (ap_fixed<W1, I1, Q1, O1, N1> &a, c_type b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1> \
  ap_ufixed<W1, I1, Q1, O1, N1>& operator sym (ap_ufixed<W1, I1, Q1, O1, N1> &a, c_type b); 

#define AP_ASSIGN_OP_C_TYPE(c_type) \
AP_ASSIGN_OP(+=, c_type) \
AP_ASSIGN_OP(-=, c_type) \
AP_ASSIGN_OP(*=, c_type) \
AP_ASSIGN_OP(/=, c_type) \
AP_ASSIGN_OP(%=, c_type) \
AP_ASSIGN_OP(|=, c_type) \
AP_ASSIGN_OP(^=, c_type) \
AP_ASSIGN_OP(&=, c_type)

AP_ASSIGN_OP_C_TYPE(char)
AP_ASSIGN_OP_C_TYPE(signed char)
AP_ASSIGN_OP_C_TYPE(unsigned char)
AP_ASSIGN_OP_C_TYPE(short)
AP_ASSIGN_OP_C_TYPE(unsigned short)
AP_ASSIGN_OP_C_TYPE(int)
AP_ASSIGN_OP_C_TYPE(unsigned int)
AP_ASSIGN_OP_C_TYPE(long)
AP_ASSIGN_OP_C_TYPE(unsigned long)
AP_ASSIGN_OP_C_TYPE(long long)
AP_ASSIGN_OP_C_TYPE(unsigned long long)
#undef AP_ASSIGN_OP
#undef AP_ASSIGN_OP_C_TYPE

#define AP_REL_OP(sym) \
  template<int W1, int W2> \
  bool operator sym (ap_int<W1> a, ap_int<W2> b); \
  template<int W1, int W2> \
  bool operator sym (ap_int<W1> a, ap_uint<W2> b); \
  template<int W1, int W2> \
  bool operator sym (ap_uint<W1> a, ap_uint<W2> b); \
  template<int W1, int W2> \
  bool operator sym (ap_uint<W1> a, ap_int<W2> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_uint<W1> a, ap_fixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_int<W1> a, ap_fixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_uint<W1> a, ap_ufixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_int<W1> a, ap_ufixed<W2, I2, Q1, O1, N1> b); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_fixed<W2, I2, Q1, O1, N1> b, ap_uint<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_fixed<W2, I2, Q1, O1, N1> b, ap_int<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_ufixed<W2, I2, Q1, O1, N1> b, ap_uint<W1> a); \
  template<int W1, int W2, int I2, ap_q_mode Q1, ap_o_mode O1, int N1> \
  bool operator sym (ap_ufixed<W2, I2, Q1, O1, N1> b, ap_int<W1> a); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  bool operator sym (ap_fixed<W1, I1, Q1, O1, N1> a, ap_ufixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  bool operator sym (ap_ufixed<W1, I1, Q1, O1, N1> a, ap_ufixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  bool operator sym (ap_fixed<W1, I1, Q1, O1, N1> a, ap_fixed<W2, I2, Q2, O2, N2> b); \
  template<int W1, int I1, ap_q_mode Q1, ap_o_mode O1, int N1, int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2> \
  bool operator sym (ap_ufixed<W1, I1, Q1, O1, N1> a, ap_fixed<W2, I2, Q2, O2, N2> b);
 
  AP_REL_OP(>)
  AP_REL_OP(>=)
  AP_REL_OP(<)
  AP_REL_OP(<=)
  AP_REL_OP(==)
  AP_REL_OP(!=)

#undef AP_REL_OP
#define AP_REL_OP(sym, c_type) \
  template<int W> \
  bool operator sym (ap_int<W> a, c_type b); \
  template<int W> \
  bool operator sym (c_type a, ap_int<W> b); \
  template<int W> \
  bool operator sym (ap_uint<W> a, c_type b); \
  template<int W> \
  bool operator sym (c_type a, ap_uint<W> b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  bool operator sym (c_type a, ap_fixed<W, I, Q, O, N> b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  bool operator sym (ap_fixed<W, I, Q, O, N> a, c_type b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  bool operator sym (c_type a, ap_ufixed<W, I, Q, O, N> b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  bool operator sym (ap_ufixed<W, I, Q, O, N> a, c_type b); 
#define AP_REL_OP_C_TYPE(c_type) \
  AP_REL_OP(>, c_type) \
  AP_REL_OP(>=, c_type) \
  AP_REL_OP(<, c_type)  \
  AP_REL_OP(<=, c_type) \
  AP_REL_OP(==, c_type) \
  AP_REL_OP(!=, c_type) 
AP_REL_OP_C_TYPE(char)
AP_REL_OP_C_TYPE(unsigned char)
AP_REL_OP_C_TYPE(signed char)
AP_REL_OP_C_TYPE(short)
AP_REL_OP_C_TYPE(unsigned short)
AP_REL_OP_C_TYPE(int)
AP_REL_OP_C_TYPE(unsigned int)
AP_REL_OP_C_TYPE(long)
AP_REL_OP_C_TYPE(unsigned long)
AP_REL_OP_C_TYPE(long long)
AP_REL_OP_C_TYPE(unsigned long long)
#undef AP_REL_OP
#undef AP_REL_OP_C_TYPE
#define AP_BINARY_OP(sym, c_type) \
  template<int W> \
  ap_int<W> operator sym (const ap_int<W> &a, c_type b); \
  template<int W> \
  ap_int<W> operator sym (c_type a, const ap_int<W> &b); \
  template<int W> \
  ap_uint<W> operator sym (const ap_uint<W> &a, c_type b); \
  template<int W> \
  ap_uint<W> operator sym (c_type a, const ap_uint<W> &b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  ap_fixed<W, I, Q, O, N> operator sym (c_type a, const ap_fixed<W, I, Q, O, N> &b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  ap_fixed<W, I, Q, O, N>  operator sym (const ap_fixed<W, I, Q, O, N> &a, c_type b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  ap_ufixed<W, I, Q, O, N>  operator sym (c_type a, const ap_ufixed<W, I, Q, O, N> &b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  ap_ufixed<W, I, Q, O, N>  operator sym (const ap_ufixed<W, I, Q, O, N> &a, c_type b); 
#define AP_BINARY_OP_C_TYPE(c_type) \
    AP_BINARY_OP(+, c_type) \
    AP_BINARY_OP(-, c_type) \
    AP_BINARY_OP(*, c_type) \
    AP_BINARY_OP(/, c_type) \
    AP_BINARY_OP(%, c_type) \
    AP_BINARY_OP(|, c_type) \
    AP_BINARY_OP(^, c_type) \
    AP_BINARY_OP(&, c_type) 
AP_BINARY_OP_C_TYPE(char)
AP_BINARY_OP_C_TYPE(unsigned char)
AP_BINARY_OP_C_TYPE(signed char)
AP_BINARY_OP_C_TYPE(short)
AP_BINARY_OP_C_TYPE(unsigned short)
AP_BINARY_OP_C_TYPE(int)
AP_BINARY_OP_C_TYPE(unsigned int)
AP_BINARY_OP_C_TYPE(long)
AP_BINARY_OP_C_TYPE(unsigned long)
AP_BINARY_OP_C_TYPE(long long)
AP_BINARY_OP_C_TYPE(unsigned long long)
#undef AP_BINARY_OP
#undef AP_BINARY_OP_C_TYPE

#define AP_BINARY_OP_FLOAT(sym, c_type) \
  template<int W> \
  c_type operator sym (const ap_int<W> &a, c_type b); \
  template<int W> \
  c_type operator sym (c_type a, const ap_int<W> &b); \
  template<int W> \
  c_type operator sym (const ap_uint<W> &a, c_type b); \
  template<int W> \
  c_type operator sym (c_type a, const ap_uint<W> &b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  c_type operator sym (c_type a, const ap_fixed<W, I, Q, O, N> &b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  c_type  operator sym (const ap_fixed<W, I, Q, O, N> &a, c_type b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  c_type  operator sym (c_type a, const ap_ufixed<W, I, Q, O, N> &b); \
  template<int W, int I, ap_q_mode Q, ap_o_mode O, int N> \
  c_type  operator sym (const ap_ufixed<W, I, Q, O, N> &a, c_type b); 
#define AP_BINARY_OP_FLOAT_C_TYPE(c_type) \
    AP_BINARY_OP_FLOAT(+, c_type) \
    AP_BINARY_OP_FLOAT(-, c_type) \
    AP_BINARY_OP_FLOAT(*, c_type) \
    AP_BINARY_OP_FLOAT(/, c_type) 
AP_BINARY_OP_FLOAT_C_TYPE(float)
AP_BINARY_OP_FLOAT_C_TYPE(double)
#undef AP_BINARY_OP_FLOAT
#undef AP_BINARY_OP_FLOAT_C_TYPE


/* Operators with ap_bit_ref.
 * ------------------------------------------------------------
 */
// arithmetic, bitwise and shift operators.
#define OP_BIN_WITH_BIT(BIN_OP, RTYPE)                                         \
  template <int _AP_W1, int _AP_W2, bool _AP_S2>                               \
  typename ap_int_base<_AP_W1, true>::template RType<1, false>::RTYPE          \
  operator BIN_OP(const ap_int<_AP_W1>& op1,                                   \
                  const ap_bit_ref<_AP_W2, _AP_S2>& op2);                      \
  template <int _AP_W1, int _AP_W2, bool _AP_S2>                               \
  typename ap_int_base<_AP_W1, false>::template RType<1, false>::RTYPE         \
  operator BIN_OP(const ap_uint<_AP_W1>& op1,                                  \
                  const ap_bit_ref<_AP_W2, _AP_S2>& op2);                      \
  template <int _AP_W1, bool _AP_S1, int _AP_W2>                               \
  typename ap_int_base<1, false>::template RType<_AP_W2, true>::RTYPE          \
  operator BIN_OP(const ap_bit_ref<_AP_W1, _AP_S1>& op1,                       \
                  const ap_int<_AP_W2>& op2);                                  \
  template <int _AP_W1, bool _AP_S1, int _AP_W2>                               \
  typename ap_int_base<1, false>::template RType<_AP_W2, false>::RTYPE         \
  operator BIN_OP(const ap_bit_ref<_AP_W1, _AP_S1>& op1,                       \
                  const ap_uint<_AP_W2>& op2);

OP_BIN_WITH_BIT(+, plus)
OP_BIN_WITH_BIT(-, minus)
OP_BIN_WITH_BIT(*, mult)
OP_BIN_WITH_BIT(/, div)
OP_BIN_WITH_BIT(%, mod)
OP_BIN_WITH_BIT(&, logic)
OP_BIN_WITH_BIT(|, logic)
OP_BIN_WITH_BIT(^, logic)
OP_BIN_WITH_BIT(>>, arg1)
OP_BIN_WITH_BIT(<<, arg1)

#undef OP_BIN_WITH_BIT

// compound assignment operators.
#define OP_ASSIGN_WITH_BIT(ASSIGN_OP)                                      \
  template <int _AP_W1, int _AP_W2, bool _AP_S2>                           \
  ap_int<_AP_W1>& operator ASSIGN_OP(                                      \
      ap_int<_AP_W1>& op1, ap_bit_ref<_AP_W2, _AP_S2>& op2);               \
  template <int _AP_W1, int _AP_W2, bool _AP_S2>                           \
  ap_uint<_AP_W1>& operator ASSIGN_OP(                                     \
      ap_uint<_AP_W1>& op1, ap_bit_ref<_AP_W2, _AP_S2>& op2);              \
  template <int _AP_W1, bool _AP_S1, int _AP_W2>                           \
  ap_bit_ref<_AP_W1, _AP_S1>& operator ASSIGN_OP(                          \
      ap_bit_ref<_AP_W1, _AP_S1>& op1, ap_int<_AP_W2>& op2);               \
  template <int _AP_W1, bool _AP_S1, int _AP_W2>                           \
  ap_bit_ref<_AP_W1, _AP_S1>& operator ASSIGN_OP(                          \
      ap_bit_ref<_AP_W1, _AP_S1>& op1, ap_uint<_AP_W2>& op2);

OP_ASSIGN_WITH_BIT(+=)
OP_ASSIGN_WITH_BIT(-=)
OP_ASSIGN_WITH_BIT(*=)

#undef OP_ASSIGN_WITH_BIT

#if defined(SYSTEMC_H) || defined(SYSTEMC_INCLUDED) 
template<int _AP_W>
 void sc_trace(sc_core::sc_trace_file *tf, const ap_int<_AP_W> &op,
                     const std::string &name); 

template<int _AP_W>
 void sc_trace(sc_core::sc_trace_file *tf, const ap_uint<_AP_W> &op,
                     const std::string &name); 

template<int _AP_W, int _AP_I, ap_q_mode _AP_Q,
         ap_o_mode _AP_O, int _AP_N>
 void sc_trace(sc_core::sc_trace_file *tf, const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N >&op, const std::string &name) ;

template<int _AP_W, int _AP_I, ap_q_mode _AP_Q,
         ap_o_mode _AP_O, int _AP_N>
 void sc_trace(sc_core::sc_trace_file *tf, const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N >&op, const std::string &name) ;
#endif

#endif // __SYNTHESIS__

#endif // __cplusplus

#endif

