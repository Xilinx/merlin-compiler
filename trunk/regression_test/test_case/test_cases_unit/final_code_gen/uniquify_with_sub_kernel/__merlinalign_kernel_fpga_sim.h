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
/************************************************************************************
 Merlin Compiler (TM) Version 2018.3.dev (8340c6448.8452)
 Built Mon Jul 9 09:31:07 2018 -0700
 Copyright (C) 2015-2018 Xilinx, Inc. All Rights Reserved.
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
extern int __merlin_init(const char * bitstream);
extern int __merlin_release();
extern void __merlin_align_kernel_fpga(char rf_vec[2048 * 10000],int rf_len_vec[10000],int profbuf_size_vec[10000],struct m128i profbuf_vec[128 * 10000],long results[10000],int N);
#ifdef __cplusplus
}
#endif
