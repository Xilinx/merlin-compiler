
//__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__kernel void write_to_ddr_kernel(__global float *restrict out3, int num_conv) 
{
//	float out4_buf[R * BS_R * BS_C * BS_O * O];

	int layer = num_conv - 1;

//	for (int layer = 0; layer < /*num_conv*/ 1; layer++)
	{
//		int pooled_BN_R = (cfg_BN_R[layer] + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
//		int pooled_BN_C = (cfg_BN_C[layer] + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
		int pooled_BN_R = cfg_BN_R[layer] >> (cfg_pooling_size[layer] - 1);
		int pooled_BN_C = cfg_BN_C[layer] >> (cfg_pooling_size[layer] - 1);

	//	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
	//	int BS_C_scaled = BS_C / cfg_pooling_size[layer];
		int BS_R_R_scaled = (R * BS_R) >> (cfg_pooling_size[layer] - 1);
		int BS_C_scaled = BS_C >> (cfg_pooling_size[layer] - 1);

	//	for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
	//	for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
	//	for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
		int r0 = 0;
		int c0 = 0;
		int o0 = 0;
	//	for (int i = 0; i < cfg_BN_O[layer] * cfg_BN_R[layer] * cfg_BN_C[layer]; i++)
		while (1)
		{
		//	int blk_idx_r0 = r0 / cfg_pooling_size[layer];
		//	int blk_idx_c0 = c0 / cfg_pooling_size[layer];
			int blk_idx_r0 = r0 >> (cfg_pooling_size[layer] - 1);
			int blk_idx_c0 = c0 >> (cfg_pooling_size[layer] - 1);

			int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
			int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

			// Perform block merging at (r,c) on-chip
		//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
		//	for (int cc = 0; cc < BS_C_scaled; cc++)
		//	for (int o1 = 0; o1 < BS_O; o1++)
			int rr = 0;
			int cc = 0;
			int o1 = 0;
		//	for (int j = 0; j < BS_R_R_scaled * BS_C_scaled * BS_O; j++)
			while (1)
			{
				struct custom_float_array root_data;

				root_data = read_channel_altera(ch_write_to_ddr);

#pragma unroll 32
				for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
				{
					int out3_idx = ((blk_idx_r0 * pooled_BN_C + blk_idx_c0) * cfg_BN_O[layer] + o0) * R * BS_R * BS_C * O * BS_O + ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + (subblk_idx_c0 * BS_C_scaled + cc)) * O * BS_O + o1 * O + o2;
					float data = root_data.drain_data[o2];

					out3[out3_idx] = root_data.drain_data[o2];
				}

				o1++;
				if (o1 == BS_O)
				{
					o1 = 0;
					cc++;
					if (cc == BS_C_scaled)
					{
						cc = 0;
						rr++;
						if (rr == BS_R_R_scaled) 
						{
							rr = 0;
							break;
						}
					}
				}
			}

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
						break;
					}
				}
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
