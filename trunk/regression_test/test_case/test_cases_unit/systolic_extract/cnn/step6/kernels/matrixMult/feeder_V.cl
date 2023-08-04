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

// instantiate V feeders
__attribute__((max_global_work_dim(0)))
__attribute__((autorun))
__attribute__((num_compute_units(SYS_ARRAY_NUM_COLS)))
__kernel void feed_V_kernel()
{
    const int feeder_id = get_compute_id(0);



    vec_float_t block_double_buffer[2][19][4][32];
    

    const bool LAST_FEEDER_IN_CHAIN             = (feeder_id == (SYS_ARRAY_NUM_COLS-1));

    int write_side_idx = 0;
    int forward_idx = 0;
    int read_side_idx = 0;

    bool more_to_write_to_buffer          = true;     // the feeder will first fill up one of its buffers before starting the feed the array
    bool more_to_feed_to_sysarr           = false;    // there is nothing to feed to sys array in the begining, wait for the buffer to fill up and then swap!
    bool more_data_to_forward             = false; // this is always false for the LAST_FEEDER_IN_CHAIN
    bool buffer_id_to_write_to            = 0;        // 0 .. 1
    bool buffer_id_to_feed_to_sysarr      = 1;        // 0 .. 1

	int _i2_w = 0; 
	int _i1_w = 0; 
	int _i0_w = 0; 

	int _i3_r = 0; 
	int _i2_r = 0; 
	int _i1_r = 0; 
	int _i0_r = 0; 



    while (1) {

//////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////
//#define FB_IN_DIM (BSIZE_K * PE_DSIZE)
//#define FB_IN_DIM (BSIZE_K )
//#define FB_IN_DIM (BSIZE_K )
//#define FB_Q_DIM K
//#define FB_R_DIM (BSIZE_J + K-1)

//#define FB_BLOCK_SIZE BSIZE_J * K * FB_IN_DIM // not including padding
//#define FB_PAD_SIZE   (K-1) * K * FB_IN_DIM
#define FB_PAD_SIZE   384
#define FB_BLOCK_SIZE (2432 - 384)
#define FB_FORWARD_SIZE ((FB_BLOCK_SIZE*PE_COLS+FB_PAD_SIZE) - (FB_BLOCK_SIZE * (feeder_id+1)))  // total_b_block_size - already_stored
#define FB_READ_SIZE	8192

//////////////////////////////////////////////////////////////
// WRITE SIDE:
// 1. reading from chain
// 2. update buffee
// 3. forward into chain
//////////////////////////////////////////////////////////////

        // Note:
        // more_to_write_to_buffer: writing into local buffer (including the padding part)

        struct ch_data_col_feeder_chain_struct data_read_from_channel;
        bool read_channel_success;

        //bool data_is_mine_to_buffer = (LAST_FEEDER_IN_CHAIN || (!LAST_FEEDER_IN_CHAIN && !more_data_to_forward));
        bool data_is_mine_to_buffer = more_to_write_to_buffer;

        // 1. reading from chain
        if (more_to_write_to_buffer || (!LAST_FEEDER_IN_CHAIN && more_data_to_forward)) {
            if (feeder_id == 0) {
                data_read_from_channel = read_channel_nb_altera(ch_data_col_loader_to_first_feeder, &read_channel_success);
            } else {
                data_read_from_channel = read_channel_nb_altera(ch_data_col_feeder_chain[feeder_id], &read_channel_success);
            }
        } else {
            read_channel_success = false;
        }

        if (data_is_mine_to_buffer && read_channel_success)
        {
            vec_float_t data        = data_read_from_channel.data;

            block_double_buffer[buffer_id_to_write_to][_i0_w][_i1_w][_i2_w] = data;

        }

        if (!LAST_FEEDER_IN_CHAIN) { // last feeder does not forward data
            if (more_data_to_forward && read_channel_success)
            {
                write_channel_altera(ch_data_col_feeder_chain[feeder_id+1], data_read_from_channel);
                
            }
        }

        if (more_to_write_to_buffer && read_channel_success)
        {
            write_side_idx ++;

			_i2_w++;
			if (_i2_w == 32)
			{
				_i2_w = 0;
			_i1_w++;
			if (_i1_w == 4)
			{
				_i1_w = 0;
			_i0_w++;
			if (_i0_w == 19)
			{
				_i0_w = 0;
		}
		}
		}
            if (write_side_idx == FB_BLOCK_SIZE + FB_PAD_SIZE)
            {
                more_to_write_to_buffer= false; 
            }
            if (write_side_idx == FB_BLOCK_SIZE && !LAST_FEEDER_IN_CHAIN) 
            {
                more_data_to_forward = true;
                forward_idx = -1; // to enable forwarding from next cycle
            }


            if (write_side_idx == FB_BLOCK_SIZE + FB_PAD_SIZE){
                write_side_idx = 0;
				_i2_w = 0;
				_i1_w = 0;
				_i0_w = 0;
            }
        }

        if (more_data_to_forward && read_channel_success)
        {
            forward_idx ++;
            if (forward_idx == FB_FORWARD_SIZE)
            {
                more_data_to_forward = 0;
                forward_idx = 0;
            }
        }

//////////////////////////////////////////////////////////////
// READ SIDE:
// 1. loop structure, including a reuse loop
// 2. update the addressing
//////////////////////////////////////////////////////////////

        if (more_to_feed_to_sysarr) {
            struct ch_data_b_struct sys_arr_feed_data;

            sys_arr_feed_data.data = 
                block_double_buffer[buffer_id_to_feed_to_sysarr][_i1_r+_i3_r][_i2_r][_i0_r];

            bool write_sys_arr_channel_success = write_channel_nb_altera(ch_data_b_boundary[feeder_id], sys_arr_feed_data);

            if (write_sys_arr_channel_success) {


		        read_side_idx++;
				_i3_r++;
				if (_i3_r == 16)
				{
					_i3_r = 0;
				_i2_r++;
				if (_i2_r == 4)
				{
					_i2_r = 0;
				_i1_r++;
				if (_i1_r == 4)
				{
					_i1_r = 0;
				_i0_r++;
				if (_i0_r == 32)
				{
					_i0_r = 0;
				}
				}
				}
				}
                if (read_side_idx == /*FB_IN_DIM * K * K *  BSIZE_J*/ FB_READ_SIZE)
                {
		            read_side_idx = 0;
                   
					_i3_r = 0;
					_i2_r = 0;
					_i1_r = 0;
					_i0_r = 0;
                    more_to_feed_to_sysarr = false;
                }
            }
        }


        if (!more_to_write_to_buffer && !more_to_feed_to_sysarr && !more_data_to_forward) {
            more_to_write_to_buffer = true;
            more_to_feed_to_sysarr  = true;
            buffer_id_to_feed_to_sysarr = !buffer_id_to_feed_to_sysarr;
            buffer_id_to_write_to       = !buffer_id_to_write_to;
        }

    }

}

