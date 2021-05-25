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


#ifndef _CNN_H_
#define _CNN_H_

#define IMG_IN_NUM 256
#define IMG_OUT_NUM 256
#define IMG_ROW 56
#define IMG_COL 56
#define IMG_IN_ROW (IMG_ROW + 2)
#define IMG_IN_COL (IMG_COL + 2)
#define KERNEL  3

/*
#define HA    (SCALING_FACTOR * MAT_A_BLOCK_HEIGHT)       // Matrix A height
#define WA    (SCALING_FACTOR * MAT_A_BLOCK_WIDTH)    // Matrix A width

#define HB    WA                                          // Matrix B height
#define WB    (SCALING_FACTOR * MAT_B_BLOCK_WIDTH)       // Matrix B width

#define HC HA                                             // Matrix C height
#define WC WB                                             // Matrix C width 
*/

#endif // _CNN_H_

