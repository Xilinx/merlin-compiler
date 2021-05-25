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


#ifndef STD_HLS_COMPLEX_UTILS_H
#define STD_HLS_COMPLEX_UTILS_H

#include <complex>
#include "ap_fixed.h"
#include "utils/x_hls_defines.h"
#include "utils/x_hls_traits.h"
#ifndef __SYNTHESIS__
#include <iostream>
#endif

namespace hls {
/*
*******************************************************************************
* Vivado HLS complex data type
* ----------------------------
*
* std::complex
* x_traits for std::complex
* set_to_one
*
*******************************************************************************
*/
 

  template<typename T1, typename T2>
    struct x_traits<std::complex<T1>,std::complex<T2> > {
      typedef std::complex<T1>                  ADD_T;
      typedef std::complex<T1>                  SAT_T;
      typedef std::complex<T1>                  MULT_T;
      typedef std::complex<T1>                  MADD_T;
      typedef std::complex<int>                 INT_T;
      typedef std::complex<float_struct<24,8> > FLOAT_STRUCT_T;
    };
 
#define X_COMPLEX_MAX(a, b) ((a) > (b) ? (a) : (b))

  template<int W1, int I1, int W2, int I2>
    struct x_traits<std::complex<ap_fixed<W1,I1> >,std::complex<ap_fixed<W2,I2> > > {
      typedef std::complex<ap_fixed<X_COMPLEX_MAX(I1,I2)+1+X_COMPLEX_MAX(W1-I1,W2-I2),X_COMPLEX_MAX(I1,I2)+1> >  ADD_T;
      typedef std::complex<ap_fixed<W1,I1,AP_RND,AP_SAT> >   SAT_T;
      typedef std::complex<ap_fixed<W1+W2+1,I1+I2+1> >       MULT_T;
      typedef std::complex<ap_fixed<W1+W2+2,I1+I2+2> >       MADD_T;
      typedef std::complex<ap_fixed<(2*W1)+1,(2*I1)+1> >     CONJ_SQ_T;
      typedef std::complex<int>                              INT_T;
      typedef std::complex<float_struct<24,8> >              FLOAT_STRUCT_T;
    };

  template<int W1, int W2>
    struct x_traits<std::complex<ap_int<W1> >,std::complex<ap_int<W2> > > {
      typedef std::complex<ap_int<X_COMPLEX_MAX(W1,W2)+1> >  ADD_T;
      typedef std::complex<ap_int<W1> >                      SAT_T;
      typedef std::complex<ap_int<W1+W2+1> >                 MULT_T;
      typedef std::complex<ap_int<W1+W2+2> >                 MADD_T;
      typedef std::complex<ap_int<(2*W1)+1> >                CONJ_SQ_T;
      typedef std::complex<int>                              INT_T;
      typedef std::complex<float_struct<24,8> >              FLOAT_STRUCT_T;
    };

#undef X_COMPLEX_MAX

  template<int DIM, typename T>
    struct x_traits_d <std::complex<T>,DIM> {
      typedef std::complex<T>               ACCUM_T;
      typedef std::complex<T>               ACCUM2_T;
      typedef std::complex<ap_int<25+DIM> > ADD_TREE_T;
    };

  template<int DIM, int W, int I>
    struct x_traits_d <std::complex<ap_fixed<W,I> >,DIM> {
      typedef std::complex<ap_fixed<W+BitWidth<DIM>::Value,I+BitWidth<DIM>::Value> >         ACCUM_T;
      typedef std::complex<ap_fixed<(2*W)+BitWidth<DIM>::Value,(2*I)+BitWidth<DIM>::Value> > ACCUM2_T;
    };

  template<typename T> T x_neg(T &din); 

  static float x_neg(float din); 

  static double x_neg(double din); 

  template<typename T> std::complex<T> x_neg(std::complex<T> &din); 


  template<typename T> typename x_traits<T,T>::MULT_T x_conj_sq(T &din); 

  template<typename T> typename x_traits<T,T>::MADD_T x_conj_sq(std::complex<T> &din); 

  template<typename T> T x_conj(T &din); 

  template<typename T> std::complex<T> x_conj(const std::complex<T> &din); 

  // This function is called when a non-const std::complex is conjugated.  It calls the
  // version of x_conj() which takes a constant parameter
  template<typename T> std::complex<T> x_conj(std::complex<T> &din); 

  template<int W, int I> void set_to_one(std::complex<ap_fixed<W,I> > &a); 

  // Functions to extract real and imaginary components
  template<typename T> T x_real(const T &din); 
  template<typename T> T x_real(const std::complex<T> &din); 
  template<typename T> T x_imag(const T &din); 
  template<typename T> T x_imag(const std::complex<T> &din); 

} // end namespace hls

#endif




