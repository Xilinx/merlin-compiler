
__attribute__((max_global_work_dim(0)))
__kernel void zero_padding_kernel(__global data_precision *restrict out5, __global data_precision *restrict out5_copy, unsigned char num_conv) 
{
	data_precision out5_buf[(R * BS_R + K - 1) * (BS_C + K - 1) * O * BS_O];

	for (unsigned char layer = 0; layer < num_conv - 1; layer++)
	{
		int pooled_BN_R = (cfg_BN_R[layer] + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
		int pooled_BN_C = (cfg_BN_C[layer] + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];

		int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
		int BS_C_scaled = BS_C / cfg_pooling_size[layer];

		for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
		for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
		for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
		{
			int blk_idx_r0 = r0 / cfg_pooling_size[layer];
			int blk_idx_c0 = c0 / cfg_pooling_size[layer];

			int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
			int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

				// Perform block merging at (r,c) on-chip
			for (int rr = 0; rr < BS_R_R_scaled; rr++)
			for (int cc = 0; cc < BS_C_scaled; cc++)
			for (int o1 = 0; o1 < BS_O; o1++)
			{
				int out5_buf_idx_base = (((K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr) * (BS_C + K - 1) + ((K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc)) * BS_O * O + o1 * O;

				struct custom_float_array root_data;

				root_data = read_channel_altera(ch_zero_padding);
#pragma unroll 32
				for (int o2 = 0; o2 < O; o2++)
				{
					out5_buf[out5_buf_idx_base + o2] = root_data.drain_data[o2];
				}
			}
		
			// Write to DDR
			if (subblk_idx_c0 == (cfg_pooling_size[layer] - 1) || c0 == (cfg_BN_C[layer] - 1))
			{
#if DEBUG_DDR_BANDWIDTH
				for (int rr = 0; rr < BS_R_R_scaled; rr++)
				for (int c1 = 0; c1 < BS_C + K - 1; c1++)
				for (int o1 = 0; o1 < BS_O; o1++)
				{
					struct custom_float_array root_data;
#pragma unroll 32
					for (int o2 = 0; o2 < O; o2++)
					{
						int out5_buf_idx = (((K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr) * (BS_C + K - 1) + c1) * BS_O * O + o1 * O + o2;
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + out5_buf_idx;
		
					//	out5[out5_idx] = out5_buf[out5_buf_idx];
						root_data.drain_data[o2] = out5_buf[out5_buf_idx];
					}

					write_channel_altera(ch_zero_padding_debug, root_data);
				}
#else
				int out5_buf_idx_base = ((K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled) * (BS_C + K - 1) * BS_O * O;
				int out5_idx_base = ((blk_idx_r0 * cfg_BN_C[layer] + blk_idx_c0) * cfg_BN_O[layer] + o0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_O * O + out5_buf_idx_base;

#pragma unroll 32
				for (int i = 0; i < BS_R_R_scaled * (BS_C + K - 1) * BS_O * O; i++)
				{
					out5[out5_idx_base + i] = out5_buf[out5_buf_idx_base + i];
				}
#endif
			}
		}

		mem_fence(CLK_CHANNEL_MEM_FENCE);
		write_channel_altera(ch_msg_loader_in_ready, layer);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
		write_channel_altera(ch_msg_loader_w_ready, layer);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
	}
}
