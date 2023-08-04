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
  #define MAT_A_NUM_BLOCKS_IN_ROW             (IMG_ROW / MAT_A_BLOCK_WIDTH)
  #define MAT_A_NUM_BLOCKS_IN_COL             (IMG_COL / MAT_A_BLOCK_HEIGHT)
  #define MAT_A_NUM_VECTORS_IN_ROW_OF_BLOCKS  (MAT_A_NUM_BLOCKS_IN_ROW * MAT_A_BLOCK_NUM_VECTORS)

//  #define MAT_B_NUM_BLOCKS_IN_ROW             (WB / MAT_B_BLOCK_WIDTH)
//  #define MAT_B_NUM_BLOCKS_IN_COL             (HB / MAT_B_BLOCK_HEIGHT)
//  #define MAT_B_NUM_VECTORS_IN_COL_OF_BLOCKS  (MAT_B_NUM_BLOCKS_IN_COL * MAT_B_BLOCK_NUM_VECTORS)
//  #define MAT_B_NUM_VECTORS_IN_MATRIX         (MAT_B_NUM_VECTORS_IN_COL_OF_BLOCKS * MAT_B_NUM_BLOCKS_IN_ROW)

