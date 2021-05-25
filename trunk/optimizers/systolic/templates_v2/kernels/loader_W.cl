
//;#   my $debug_loader_w = "on";
//; my $debug_loader_w = "off";

	__attribute__((max_global_work_dim(0)))
__kernel void load_W_and_forward(__global vec_data_precision_t *restrict w1, unsigned char num_conv)
{
	for (unsigned char layer = 0; layer < num_conv; layer++) 
	{
		//; if ($debug_loader_w eq "on") {
		printf("loader_W started!\n");
		//; }

		//; if ($debug_loader_w eq "on") {
		int wr_cnt = 0;
		//; }

		unsigned int w_num_vectors_in_o_of_blocks = cfg_BN_I[layer] * O * BS_O * K * K * BS_I; // number of vectors in one block line in (p,q,i) direction
		unsigned int w_num_vectors_in_weight = cfg_BN_O[layer] * w_num_vectors_in_o_of_blocks;
	//	unsigned char in_num_blocks_in_pxqxi = cfg_BN_R[layer] * cfg_BN_C[layer];
		unsigned int in_num_blocks_in_pxqxi = cfg_BN_R[layer] * cfg_BN_C[layer];

		// r0,c0,o0,i0,
		int rd_cnt = 0;
		int rd_total = w_num_vectors_in_weight * in_num_blocks_in_pxqxi;
		int r0 = 0;
		int c0 = 0;
		int o0 = 0;
		int i0 = 0;
		int o2 = 0;
		int o1 = 0;
		int p = 0;
		int q = 0;
		int i1 = 0;

		int o, i;

		unsigned int vector_id_in_col_of_blocks = 1;
		unsigned int vector_id_global = 1;
		unsigned char V_reuse_counter = 1;

		bool more_vectors_to_load_and_forward = true;
		bool feed_zeros_to_flush_last_C_block = false;

		while (more_vectors_to_load_and_forward)
		{
			struct ch_data_col_feeder_chain_struct W_local;

			if (feed_zeros_to_flush_last_C_block)
			{
				W_local.data = VECTOR_ZERO;
			}
			else
			{
			//	W_local.data = weights_block_wise[vector_id_global-1];
				W_local.data = w1[vector_id_global - 1];

				o = o0 * O * BS_O + o2 * BS_O + o1;
				i = i0 * BS_I + i1;

				if (o >= cfg_out_num[layer])
				{
					W_local.data = VECTOR_ZERO;
				}
				else if ((i * I + I - 1) >= cfg_in_num[layer])
				{
					// FIXME: Scalarizer can't handle insert element with non-consstant index
				//	if (i * I >= cfg_in_num[layer])
				//	{
				//		W_local.data = VECTOR_ZERO;
				//	}
				//	else
				//	{
				//		for (int d = (cfg_in_num[layer] - i * I); d < I; d++)
				//		{
				//			(W_local.data)[d] = 0;
				//		}
				//	}
					W_local.data = VECTOR_ZERO;
				}
				else
				{}

				i1++;
				if (i1 == BS_I)
				{
					i1 = 0;
					q++;
					if (q == K)
					{
						q = 0;
						p++;
						if (p == K)
						{
							p = 0;
							o1++;
							if (o1 == BS_O)
							{
								o1 = 0;
								o2++;
								if (o2 == O)
								{
									o2 = 0;
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
			}

			write_channel_altera(ch_data_col_loader_to_first_feeder, W_local);

			//; if ($debug_loader_w eq "on") {
			{
				int total = w_num_vectors_in_weight * in_num_blocks_in_pxqxi + w_num_vectors_in_o_of_blocks ;
				//		printf("Loader_V wr=%d data=%.1lf (total=%d)\n",
				//				wr_cnt, W_local.data, total);
				printf("Loader_W wr=%d (total=%d)\n",
						wr_cnt, total);
				wr_cnt++;
			}
			//; }

			if (vector_id_in_col_of_blocks == w_num_vectors_in_o_of_blocks) 
			{
				vector_id_in_col_of_blocks = 1;
				if (feed_zeros_to_flush_last_C_block) 
				{
					more_vectors_to_load_and_forward = false; // we feed only one row of blocks full of zeros to flush the last C block
				}
			}
			else
			{
				vector_id_in_col_of_blocks++;
			}

			if (vector_id_global == w_num_vectors_in_weight) 
			{
				vector_id_global = 1;
				if (V_reuse_counter == in_num_blocks_in_pxqxi)
				{ // done reload and forwarding the matrix data?
					feed_zeros_to_flush_last_C_block = true;

					rd_cnt = 0;
					int r0 = 0;
					int c0 = 0;
					int o0 = 0;
					int i0 = 0;
					int o2 = 0;
					int o1 = 0;
					int p = 0;
					int q = 0;
					int i1 = 0;
				}
				V_reuse_counter++;
			} 
			else
			{
				vector_id_global++;
			}
		}

		//; if ($debug_loader_w eq "on") {
		printf("Loader_W finished!!!\n");
		//; }

		w1 += w_num_vectors_in_weight;
	//	weights_block_wise += w_num_vectors_in_weight;

			if (layer < num_conv - 1) {
		mem_fence(CLK_CHANNEL_MEM_FENCE);
		unsigned char layer_id = read_channel_altera(ch_msg_loader_w_ready);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
	//	printf("Loader W: layer_id = %d\n", layer_id);
			}
	}
}

