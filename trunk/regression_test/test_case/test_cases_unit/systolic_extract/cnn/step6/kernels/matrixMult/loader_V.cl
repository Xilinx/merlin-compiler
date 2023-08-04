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


__attribute__((max_global_work_dim(0)))
__kernel void load_V_and_forward( __global vec_float_t* restrict B, 
        unsigned int v_num_vectors_in_col_of_blocks, 
        unsigned int v_num_vectors_in_matrix, 
        unsigned char h_num_blocks_in_col )
{


    // 32K x 32K largest matrix size supported
    unsigned int vector_id_in_col_of_blocks = 1;
    unsigned int vector_id_global = 1;
    unsigned char V_reuse_counter = 1;

    bool more_vectors_to_load_and_forward = true;
    bool feed_zeros_to_flush_last_C_block = false;

    while (more_vectors_to_load_and_forward) {

        struct ch_data_col_feeder_chain_struct B_local;

        if (feed_zeros_to_flush_last_C_block) {
            B_local.data = VECTOR_ZERO;
        } else {
            B_local.data = B[vector_id_global-1];
        }

        write_channel_altera(ch_data_col_loader_to_first_feeder, B_local);



        if (vector_id_in_col_of_blocks == v_num_vectors_in_col_of_blocks) {
            vector_id_in_col_of_blocks = 1;
            if (feed_zeros_to_flush_last_C_block) {
                more_vectors_to_load_and_forward = false; // we feed only one row of blocks full of zeros to flush the last C block
            }
        } else {
            vector_id_in_col_of_blocks++;
        }

        if (vector_id_global == v_num_vectors_in_matrix) {
            vector_id_global = 1;
            if (V_reuse_counter == h_num_blocks_in_col) { // done reload and forwarding the matrix data?
                feed_zeros_to_flush_last_C_block = true;
            }
            V_reuse_counter++;
        } else {
            vector_id_global++;
        }

    }


}

