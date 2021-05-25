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


#ifndef __C_MODEL_H_
#define __C_MODEL_H_


void c_model(
    //   unsigned int *num_elem_weights,
    //   unsigned int num_elem_input_fm,
    //   unsigned int num_elem_output_fm,
       data_precision *weights_block_wise,
       data_precision *input_fm_block_wise,
       data_precision *output_fm_block_wise,
	   int *t_idx_addr,
	   int *t_idx_idx,
	   int *t_idx_i,
	   int *t_idx_j
		);

#endif // __C_MODEL_H_
