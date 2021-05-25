
//__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__kernel void write_to_ddr_kernel(__global data_precision *restrict out3, unsigned char num_conv) 
{
	data_precision out4_buf[R * BS_R * BS_C * BS_O * O];

	unsigned char layer = num_conv - 1;
//	for (int layer = 0; layer < /*num_conv*/ 1; layer++)
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
				struct custom_float_array root_data;

				root_data = read_channel_altera(ch_write_to_ddr);

#pragma unroll 32
				for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
				{
					// perform address transformation and relu
					int out4_buf_idx = ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + (subblk_idx_c0 * BS_C_scaled + cc)) * O * BS_O + o1 * O + o2;
					data_precision data = root_data.drain_data[o2];
					out4_buf[out4_buf_idx] = root_data.drain_data[o2];
				}
			}

			// Write to DDR
			if (subblk_idx_c0 == (cfg_pooling_size[layer] - 1) || c0 == (cfg_BN_C[layer] - 1))
			{
#if DEBUG_DDR_BANDWIDTH
				struct custom_float_array data;

				for (int rr = 0; rr < BS_R_R_scaled; rr++)
				for (int c1 = 0; c1 < BS_C; c1++)
				{
#pragma unroll 32
					for (int oo = 0; oo < O * BS_O; oo++)
					{
						int out4_buf_idx = ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + c1) * O * BS_O + oo;
						data.drain_data[oo] = out4_buf[out4_buf_idx];
					}
					write_channel_altera(ch_debug_ddr_bandwidth, data);
				}
#else
				int out4_buf_idx_base = subblk_idx_r0 * BS_R_R_scaled * BS_C * BS_O * O;
				int out3_idx_base = ((blk_idx_r0 * cfg_BN_C[layer] + blk_idx_c0) * cfg_BN_O[layer] + o0) * R * BS_R * BS_C * O * BS_O + out4_buf_idx_base;

#pragma unroll 32
				for (int i = 0; i < BS_R_R_scaled * BS_C * BS_O * O; i++)
				{
					out3[out3_idx_base + i] = out4_buf[out4_buf_idx_base + i];
				}
#endif
			}
		}

		//	if (layer < NUM_CONV - 1) 
		//	{
//		mem_fence(CLK_CHANNEL_MEM_FENCE);
//		write_channel_altera(ch_msg_loader_in_ready, layer);
//		mem_fence(CLK_CHANNEL_MEM_FENCE);
//		write_channel_altera(ch_msg_loader_w_ready, layer);
//		mem_fence(CLK_CHANNEL_MEM_FENCE);
		//	}
	}
}
