
//;#   my $debug_loader_in = "on";
//; my $debug_loader_in = "off";
	
//__attribute__((max_global_work_dim(0)))
//__kernel void load_IN_and_forward( __global vec_float_t* restrict A, 
//        unsigned int h_num_vectors_in_row_of_blocks, 
//        unsigned char h_num_blocks_in_col, 
//        unsigned char v_num_blocks_in_row)
__attribute__((max_global_work_dim(0)))
__kernel void load_IN_and_forward( __global vec_float_t *restrict out5,
		__global vec_float_t *restrict out5_copy, int num_conv)
//__attribute__((max_global_work_dim(0)))
//__kernel void load_IN_and_forward( __global vec_float_t *restrict out5)
{
	for (int layer = 0; layer < num_conv; layer++)
	{
		//; if ($debug_loader_in eq "on") {
		printf("Loader_IN for layer %d started!\n", layer);
		//; }
		
		
		unsigned int in_num_vectors_in_rxc_of_blocks = cfg_BN_I[layer] * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I; // number of vectors in one block line in the (r,c) direction
	//	unsigned char in_num_blocks_in_pxqxi = cfg_BN_R[layer] * cfg_BN_C[layer];
		unsigned int in_num_blocks_in_pxqxi = cfg_BN_R[layer] * cfg_BN_C[layer];
	//	unsigned char w_num_blocks_in_pxqxi = cfg_BN_O[layer];
		unsigned int w_num_blocks_in_pxqxi = cfg_BN_O[layer];

		// r0,c0,o0,i0,(r2*BS_R+r1_p),c1_q,i1
		int rd_cnt = 0;
		int rd_total = in_num_vectors_in_rxc_of_blocks * in_num_blocks_in_pxqxi * w_num_blocks_in_pxqxi;
		int r0 = 0;
		int c0 = 0;
		int o0 = 0;
		int i0 = 0;

		int r2_r1_p = 0;
		int c1_q = 0;
		int i1 = 0;

		int r, c, i;
		
		//; if ($debug_loader_in eq "on") {
		int wr_cnt = 0;
		//; }
	
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
	
	    while (more_vectors_to_load_and_forward) 
		{
	        if (new_row_col_pair && !last)
			{
	            last = 1;
	        }
	        if (!new_row_col_pair) last = 0;
	        cnt ++;
	
	        struct ch_data_row_feeder_chain_struct IN_local;
	
	        if (feed_zeros_to_flush_last_C_block)
			{
	            IN_local.data = VECTOR_ZERO;
	        }
			else
			{
				if ((layer & 1) == 0)
				{
					IN_local.data = out5[vector_id_global];
				}
				else
				{
					IN_local.data = out5_copy[vector_id_global];
				}

				// zero padding for tiling
				r = r0 * R * BS_R + r2_r1_p;
				c = c0 * BS_C + c1_q;
				i = i0 * BS_I + i1;

				if (r >= cfg_img_row[layer] + cfg_kernel[layer] - 1 || c >= cfg_img_col[layer] + cfg_kernel[layer] - 1)
				{
					IN_local.data = VECTOR_ZERO;
				}
				else if (i * I + I - 1 >= cfg_in_num[layer])
				{
				//	if (i * I >= cfg_in_num[layer])
				//	{
				//		IN_local.data = VECTOR_ZERO;
				//	}
				//	else
				//	{
				//		for (int d = (cfg_in_num[layer] - i * I); d < I; b++)
				//		{
				//			(IN_local.data)[d] = 0;
				//		}
				//	}
					IN_local.data = VECTOR_ZERO;
				}
				else
				{}


				i1++;
				if (i1 == BS_I)
				{
					i1 = 0;
					c1_q++;
					if (c1_q == (BS_C + K - 1))
					{
						c1_q = 0;
						r2_r1_p++;
						if (r2_r1_p == R * BS_R + K - 1)
						{
							r2_r1_p = 0;
							i0++;
							if (i0 == cfg_BN_I[layer])
							{
								i0 = 0;
								o0++;
								if (o0 == cfg_BN_O[layer])
								{
									o0 = 0;
									c0++;
									if (c0 == cfg_BN_C[layer])
									{
										c0 = 0;
										r0++;
										if (r0 == cfg_BN_R[layer])
										{
											r0 = 0;
										}
									}
								}
							}
						}
					}
				}
	        }
	
	        IN_local.new_row_col_pair = new_row_col_pair;
	        write_channel_altera(ch_data_row_loader_to_first_feeder, IN_local);
	
			//; if ($debug_loader_in eq "on") {
			{
				int total = in_num_vectors_in_rxc_of_blocks * (1 + in_num_blocks_in_pxqxi * w_num_blocks_in_pxqxi);
				printf("Loader_IN wr=%d (total=%d)\n",
						wr_cnt, total);
				wr_cnt++;
			}
		//;}
	
	
	        if (vector_id_in_block == (IN_BLOCK_NUM_VECTORS - 1)) 
			{
	            vector_id_in_block = 0;
	            new_row_col_pair = false; // we keep new_row_col_pair=true for only the first block in the row of blocks (feeders in the daisy chain expect this functionality)
	        }
			else
			{
	            vector_id_in_block++;
	        }
	
	
	        vector_id_global++;
	        if (vector_id_in_row_of_blocks == in_num_vectors_in_rxc_of_blocks)
			{
	            vector_id_in_row_of_blocks = 1;
	            new_row_col_pair = true;                                 // anytime we start feeding a row of blocks we set new_row_col_pair=true, kept true for the first block only (see above)
	
	            if (feed_zeros_to_flush_last_C_block)
				{
	                more_vectors_to_load_and_forward = false; // we feed only one row of blocks full of zeros to flush the last C block
	            }
	
	            if (row_of_blocks_reuse_counter == w_num_blocks_in_pxqxi)
				{ // done reusing this row of blocks?
	                row_of_blocks_reuse_counter = 1;
	                vector_id_global_row_of_blocks_start = vector_id_global; // mark the new start of the row of blocks
	
	                if (block_col_id == in_num_blocks_in_pxqxi)
					{    // done loading and forwarding the matrix? start feeding zeros to flush last C block
	                    feed_zeros_to_flush_last_C_block = true;

						rd_cnt = 0;
						int r0 = 0;
						int c0 = 0;
						int o0 = 0;
						int i0 = 0;

						int r2_r1_p = 0;
						int c1_q = 0;
	                }
	                block_col_id++; // increment the block_id in the column of blocks
	
	            }
				else
				{
	                vector_id_global = vector_id_global_row_of_blocks_start;  // not done reusing, reset the vector_id_global to the start of row of blocks
	                row_of_blocks_reuse_counter++;
	            }
	        }
			else
			{
	            vector_id_in_row_of_blocks++;
	        }
		}
	
		//; if ($debug_loader_in eq "on") {
		printf("Loader_IN for layer %d finished!\n", layer);
		//; }
		
	//	if (layer < NUM_CONV - 1) {
		mem_fence(CLK_CHANNEL_MEM_FENCE);
		int layer_id = read_channel_altera(ch_msg_loader_in_ready);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
	//	printf("loader In: layer_id = %d\n", layer_id);
	//	}

	}
}
