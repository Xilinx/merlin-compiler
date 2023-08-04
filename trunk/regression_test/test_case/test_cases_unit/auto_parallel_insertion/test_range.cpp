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
#include "ap_int.h"
#define VDTYPE_WIDTH  512
#define DTYPE_TWIDTH 16
#define DTYPE_IWIDTH 4
#define VFTYPE_WIDTH  512
typedef ap_uint<VDTYPE_WIDTH>                VectorDataType;
typedef ap_fixed<DTYPE_TWIDTH, DTYPE_IWIDTH>  DataType;
typedef ap_uint<VFTYPE_WIDTH>               VectorFeatureType;

class testClass
{
public:
  int& range(int i, int j) {
    return dummy;
  }
  int& range() {
    return dummy;
  }
  testClass() :dummy(111) { }
private:
  int dummy;
};

#pragma ACCEL kernel
void read_data(VectorDataType  data[144000],
  DataType        training_instance[1024],
  int training_id)
{
  testClass array[32];
  
  const int unroll_factor = 32;
  int temp = 0;
  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 1000; j++) {
      temp = i + 2 * j;
    }
  }
  //  The parameter of range is polynomial a*j+b
  READ_TRAINING_DATA1: for (int i = 0; i < 32U; i++)
  {
    VectorFeatureType tmp_data = data[training_id * 32 + i];
    READ_TRAINING_DATA_INNER1: for (int j = 0; j < 32U; j++)
    training_instance[i * 32U + j].range(16 - 1, 0) = tmp_data.range((j + 1) * 16 - 1, j * 16);
  }
  
  //  The parameter of range is polynomial j*j.
  READ_TRAINING_DATA2: for (int i = 0; i < 32U; i++)
  {
    VectorFeatureType tmp_data = data[training_id * 32 + i];
    READ_TRAINING_DATA_INNER2: for (int j = 0; j < 32U; j++)
    training_instance[i * 32U + j].range(16 - 1, 0) = tmp_data.range((j + 1) * j - 1, j * j);
  }
  
  //  The parameter of range is constant.
  READ_TRAINING_DATA3: for (int i = 0; i < 32U; i++)
  {
    VectorFeatureType tmp_data = data[training_id * 32 + i];
    READ_TRAINING_DATA_INNER3: for (int j = 0; j < 32U; j++)
    training_instance[i * 32U + j].range(16 - 1, 0) = tmp_data.range(16 - 1, 16);
  }
  
  //  The parameter of range is a*j+i or a*i+j.
  READ_TRAINING_DATA4: for (int i = 0; i < 32U; i++)
  {
    VectorFeatureType tmp_data = data[training_id * 32 + i];
    READ_TRAINING_DATA_INNER4: for (int j = 0; j < 32U; j++)
    training_instance[i * 32U + j].range(16 - 1, 0) = tmp_data.range((j + 1) * 16 - i, i * 16+j);
  }
  
  //  The parameter of range is a*j+training_id.
  READ_TRAINING_DATA5: for (int i = 0; i < 32U; i++)
  {
    VectorFeatureType tmp_data = data[training_id * 32 + i];
    READ_TRAINING_DATA_INNER5: for (int j = 0; j < 32U; j++)
    training_instance[i * 32U + j].range(16 - 1, 0) = tmp_data.range((j + 1) * 16 - training_id, j * 16+training_id);
  }
  
  //  Test of range non apint.
  TEST_DATA: for (int i = 0; i < 32U; i++)
  {
    VectorFeatureType tmp_data = data[training_id * 32 + i];
    TEST_DATA_INNER: for (int j = 0; j < 32U; j++)
    array[i * 32U + j].range(16 - 1, 0) = array[j].range((j + 1) * 16 - 1, j * 16);
  }
}

