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
#include "memcpy_512.h"
#define SIZE_1 10
#include "memcpy_256_2d.h"
#undef SIZE_1
#define SIZE_1 30
#define SIZE_2 20
#include "memcpy_64_3d.h"
#undef SIZE_1
#undef SIZE_2
#define SIZE_1 30
#define SIZE_2 20
#define SIZE_3 128
#include "memcpy_128_4d.h"
#undef SIZE_1
#undef SIZE_2
#undef SIZE_3


void func(ap_uint<512> *a, ap_uint<256> *b, ap_uint<64> *c, ap_uint<128> *d)
{
  int a_buf[1028];
  memcpy_wide_bus_read_int(a_buf, a, 20, 1028 * sizeof(int));

  char b_buf[30][10];
  memcpy_wide_bus_read_char_2d_10(b_buf, 0, 0, b, 10, 300 *sizeof(char));

  float c_buf[40][30][20];
  memcpy_wide_bus_write_float_3d_30_20(c, c_buf, 0, 0, 0, 20, 1200 * sizeof (float));

  double d_buf[5][30][20][128];
  memcpy_wide_bus_write_double_4d_30_20_128(d, d_buf, 0, 0, 0, 0, 16,  128 * 30 * 100 * sizeof(double));


}
