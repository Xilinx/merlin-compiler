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


#ifndef __AP_FIXED_REF_H__
#define __AP_FIXED_REF_H__

#ifndef __cplusplus
#error "C++ is required to include this header file"

#else

template <int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,
          int _AP_N>
struct af_bit_ref;

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
struct af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> {
  typedef ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> ref_type;

 public:
  af_bit_ref(
      const af_bit_ref<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N>& ref);

  af_bit_ref(ref_type* bv, int index = 0);

  af_bit_ref(const ref_type* bv, int index = 0);

  /// convert operators.
  operator bool() const;

  /// @name assign operators
  //  @{
  af_bit_ref& operator=(bool val);

  // Be explicit to prevent it from being deleted, as field d_bv
  // is of reference type.
  af_bit_ref& operator=(const af_bit_ref& val);

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  af_bit_ref& operator=(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& val);

  template <int _AP_W2, bool _AP_S2>
  af_bit_ref& operator=(const ap_bit_ref<_AP_W2, _AP_S2>& val);

  template <int _AP_W2>
  af_bit_ref& operator=(const ap_int<_AP_W2>& val);
  template <int _AP_W2>
  af_bit_ref& operator=(const ap_uint<_AP_W2>& val);

  //  @}


  /// @name comparison
  //  @{
  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  bool operator==(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op);

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  bool operator!=(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op);
  //  @}

  bool operator~() const;

  bool get() const;

  int length() const;

#ifndef __SYNTHESIS__
#else
  // XXX HLS will delete this in synthesis
  char* to_string() const;
#endif
}; // struct af_bit_ref

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
struct af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> {
  typedef ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> ref_type;

 public:
  af_bit_ref(
      const af_bit_ref<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N>& ref);

  af_bit_ref(ref_type* bv, int index = 0);

  af_bit_ref(const ref_type* bv, int index = 0);

  /// convert operators.
  operator bool() const;

  /// @name assign operators
  //  @{
  af_bit_ref& operator=(bool val);

  // Be explicit to prevent it from being deleted, as field d_bv
  // is of reference type.
  af_bit_ref& operator=(const af_bit_ref& val);

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  af_bit_ref& operator=(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& val);

  template <int _AP_W2, bool _AP_S2>
  af_bit_ref& operator=(const ap_bit_ref<_AP_W2, _AP_S2>& val);

  template <int _AP_W2>
  af_bit_ref& operator=(const ap_int<_AP_W2>& val);
  template <int _AP_W2>
  af_bit_ref& operator=(const ap_uint<_AP_W2>& val);

  //  @}


  /// @name comparison
  //  @{
  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  bool operator==(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op);

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  bool operator!=(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op);
  //  @}

  bool operator~() const;

  bool get() const;

  int length() const;

#ifndef __SYNTHESIS__
#else
  // XXX HLS will delete this in synthesis
  char* to_string() const;
#endif
}; // struct af_bit_ref
#endif // ifndef __cplusplus
#endif // ifndef __AP_FIXED_REF_H__
