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
__kernel void load_H_and_forward( __global vec_float_t* restrict A, 
        unsigned int h_num_vectors_in_row_of_blocks, 
        unsigned char h_num_blocks_in_col, 
        unsigned char v_num_blocks_in_row)
{
	

    // 32K x 32K largest matrix size supported
    unsigned char block_col_id = 1;

    unsigned int vector_id_in_block = 0;
    unsigned int vector_id_in_row_of_blocks = 1;
    unsigned int vector_id_global = 0;
    unsigned int vector_id_global_row_of_blocks_start = 0;

    unsigned char row_of_blocks_reuse_counter = 1;

    bool new_row_col_pair = false;
    bool more_vectors_to_load_and_forward = true;
    bool feed_zeros_to_flush_last_C_block = false;

    int cnt = 0;
    int last = 0;
    while(more_vectors_to_load_and_forward) {
        if (new_row_col_pair && !last) {
            last = 1;
        }
        if (!new_row_col_pair) last = 0;
        cnt ++;

        struct ch_data_row_feeder_chain_struct A_local;

        if (feed_zeros_to_flush_last_C_block) {
            A_local.data = VECTOR_ZERO;
        } else {
            A_local.data = A[vector_id_global];
        }

        A_local.new_row_col_pair = new_row_col_pair;
        write_channel_altera(ch_data_row_loader_to_first_feeder, A_local);




        if (vector_id_in_block == (MAT_A_BLOCK_NUM_VECTORS-1)) {
            vector_id_in_block = 0;
            new_row_col_pair = false; // we keep new_row_col_pair=true for only the first block in the row of blocks (feeders in the daisy chain expect this functionality)
        } else {
            vector_id_in_block++;
        }


        vector_id_global++;
        if (vector_id_in_row_of_blocks == h_num_vectors_in_row_of_blocks) {
            vector_id_in_row_of_blocks = 1;
            new_row_col_pair = true;                                 // anytime we start feeding a row of blocks we set new_row_col_pair=true, kept true for the first block only (see above)

            if (feed_zeros_to_flush_last_C_block) {
                more_vectors_to_load_and_forward = false; // we feed only one row of blocks full of zeros to flush the last C block
            }

            if (row_of_blocks_reuse_counter == v_num_blocks_in_row) { // done reusing this row of blocks?
                row_of_blocks_reuse_counter = 1;
                vector_id_global_row_of_blocks_start = vector_id_global; // mark the new start of the row of blocks

                if (block_col_id == h_num_blocks_in_col) {    // done loading and forwarding the matrix? start feeding zeros to flush last C block
                    feed_zeros_to_flush_last_C_block = true;
                }
                block_col_id++;                                          // increment the block_id in the column of blocks

            } else {
                vector_id_global = vector_id_global_row_of_blocks_start;  // not done reusing, reset the vector_id_global to the start of row of blocks
                row_of_blocks_reuse_counter++;
            }
        } else {
            vector_id_in_row_of_blocks++;
        }

    }

}


