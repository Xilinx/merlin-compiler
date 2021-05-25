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


#ifndef TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_TLDM_IF_H_
#define TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_TLDM_IF_H_

#include <assert.h>
#include <queue>

template <class DATA_TYPE> class tldm_fifo {
 public:
  explicit tldm_fifo(int size) { m_fifosize = size; }
  int is_empty() { return m_dat.size() == 0; }
  int is_full() { return m_dat.size() >= m_fifosize; }
  DATA_TYPE read() {
    m_dat.front();
    m_dat.pop();
  }
  void write(const DATA_TYPE &a) { m_dat.push(a); }

 protected:
  queue<DATA_TYPE> m_dat;
  int m_fifosize;
};

template <class DATA_TYPE> class tldm_array {
 public:
  tldm_array(int dims, int size0, int size1 = 1, int size2 = 1, int size3 = 1) {
    assert(dims <= 4);
    m_nDims = dims;
    m_pData = new DATA_TYPE[size0 * size1 * size2 * size3];
    m_nSize0 = size0;
    m_nSize1 = size1;
    m_nSize2 = size2;
    m_nSize3 = size3;
  }
  ~tldm_array() {
    if (m_pData) {
      delete[] m_pData;
      m_pData = NULL;
    }
  }
  DATA_TYPE read(int idx0) {
    assert(idx0 < m_nSize0);
    return m_pData[idx0];
  }
  DATA_TYPE read(int idx1, int idx0) {
    assert(idx0 < m_nSize0);
    assert(idx1 < m_nSize1);
    return m_pData[idx1 * m_nSize0 + idx0];
  }
  DATA_TYPE read(int idx2, int idx1, int idx0) {
    assert(idx0 < m_nSize0);
    assert(idx1 < m_nSize1);
    assert(idx2 < m_nSize2);
    return m_pData[(idx2 * m_nSize1 + idx1) * m_nSize0 + idx0];
  }
  DATA_TYPE read(int idx3, int idx2, int idx1, int idx0) {
    assert(idx0 < m_nSize0);
    assert(idx1 < m_nSize1);
    assert(idx2 < m_nSize2);
    assert(idx3 < m_nSize3);
    return m_pData[((idx3 * m_nSize2 + idx2) * m_nSize1 + idx1) * m_nSize0 +
                   idx0];
  }

  void write(int idx0, const DATA_TYPE &val) {
    assert(idx0 < m_nSize0);
    m_pData[idx0] = val;
  }
  void write(int idx1, int idx0, const DATA_TYPE &val) {
    assert(idx0 < m_nSize0);
    assert(idx1 < m_nSize1);
    m_pData[idx1 * m_nSize0 + idx0] = val;
  }
  void write(int idx2, int idx1, int idx0, const DATA_TYPE &val) {
    assert(idx0 < m_nSize0);
    assert(idx1 < m_nSize1);
    assert(idx2 < m_nSize2);
    m_pData[(idx2 * m_nSize1 + idx1) * m_nSize0 + idx0] = val;
  }
  void write(int idx3, int idx2, int idx1, int idx0, const DATA_TYPE &val) {
    assert(idx0 < m_nSize0);
    assert(idx1 < m_nSize1);
    assert(idx2 < m_nSize2);
    assert(idx3 < m_nSize3);
    m_pData[((idx3 * m_nSize2 + idx2) * m_nSize1 + idx1) * m_nSize0 + idx0] =
        val;
  }

 protected:
  DATA_TYPE *m_pData;
  int m_nDims;
  int m_nSize0;
  int m_nSize1;
  int m_nSize2;
  int m_nSize3;
};

#endif  // TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_TLDM_IF_H_
