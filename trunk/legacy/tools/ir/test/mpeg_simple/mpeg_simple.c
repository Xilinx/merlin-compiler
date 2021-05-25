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




void idct_row_kernel(int *data_idct_row) {
#pragma tldm iteration_domain iterator = "img, mby, mbx, block" max_range =    \
    "100, 36, 28, 6"
#pragma tldm tuning iterator = "img, mby, mbx, block" order =                  \
    "img, mby, mbx, block"
#pragma tldm access_pattern name = "ref0" port = "data_idct_row" dir =         \
    "w" iterator = "img, mby, mbx, block" pattern =                            \
        "(img*36*28*6+mby*28*6+mbx*6+block)*64 + 8*i + j: 0<=i; i<8; 0<=j; j<8"
}

void idct_col_kernel(int *data_idct_row, int *data_idct_col) {
#pragma tldm iteration_domain iterator = "img, mby, mbx, block" max_range =    \
    "100, 36, 28, 6"
#pragma tldm tuning iterator = "img, mby, mbx, block" order =                  \
    "img, mby, mbx, block"
#pragma tldm access_pattern name = "ref0" port = "data_idct_row" dir =         \
    "r" iterator = "img, mby, mbx, block" pattern =                            \
        "(img*36*28*6+mby*28*6+mbx*6+block)*64 + i + 8*j: 0<=i; i<8; 0<=j; j<8"
#pragma tldm access_pattern name = "ref1" port = "data_idct_col" dir =         \
    "w" iterator = "img, mby, mbx, block" pattern =                            \
        "(img*36*28*6+mby*28*6+mbx*6+block)*64 + i + 8*j: 0<=i; i<8; 0<=j; j<8"
}

void tu_kernel(int *data_idct_col, int *data_tu) {
#pragma tldm iteration_domain iterator = "img, mby, mbx" max_range =           \
    "100, 36, 28"
#pragma tldm tuning iterator = "img, mby, mbx" order = "img, mby, mbx"
#pragma tldm access_pattern name = "ref0" port = "data_idct_col" dir =         \
    "r" iterator = "img, mby, mbx" pattern =                                   \
        "(img*36*28*6+mby*28*6+mbx*6+block)*64 + i + 8*j: 0<=i; i<8; 0<=j; j<8; block>=0; block <8"
#pragma tldm access_pattern name = "ref1" port = "data_tu" dir =               \
    "w" iterator = "img, mby, mbx" pattern =                                   \
        "(img*36*28*6+mby*28*6+mbx*6+block)*64 + i + 8*j: 0<=i; i<8; 0<=j; j<8; block>=0; block<8"
}
