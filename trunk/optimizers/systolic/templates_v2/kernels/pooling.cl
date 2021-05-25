
__attribute__((max_global_work_dim(0)))
__kernel void pooling_kernel(unsigned char num_conv) 
{
	data_precision out2_buf[R * BS_R * BS_C * BS_O * O];

	for (unsigned char layer = 0; layer < num_conv; layer++)
	{
	//	if (cfg_pooling_size[layer] == 1)
	//	{
	//		// bypass
	//		struct custom_float_array root_data;

	//		root_data = read_channel_altera(ch_relu_to_pooling);

	//		if (layer == num_conv - 1)
	//		{
	//			write_channel_altera(ch_write_to_ddr, root_data);
	//		}
	//		else
	//		{
	//			write_channel_altera(ch_zero_padding, root_data);
	//		}
	//	}
	//	else
		{
			for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
			for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
			for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
			{
			//	for (int r2 = 0; r2 < R; r2++)
			//	for (int r1 = 0; r1 < BS_R; r1++)
			//	for (int c1 = 0; c1 < BS_C; c1++)
			//	for (int o1 = 0; o1 < BS_O; o1++)
				for (int i = 0; i < R * BS_R * BS_C * BS_O; i++)
				{
					struct custom_float_array root_data;

					root_data = read_channel_altera(ch_relu_to_pooling);
#pragma unroll 32
					for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
					{
					//	int out2_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o1 * O + o2;
					//	out2_buf[out2_buf_idx] = root_data.drain_data[o2];
						out2_buf[i * O + o2] = root_data.drain_data[o2];
					}
				}

				int BS_R_R_scaled = ((R * BS_R) >> (cfg_pooling_size[layer] - 1));
				int BS_C_scaled = (BS_C >> (cfg_pooling_size[layer] - 1));
			
				for (int r1 = 0; r1 < BS_R_R_scaled; r1++)
				for (int c1 = 0; c1 < BS_C_scaled; c1++)
				for (int o1 = 0; o1 < BS_O; o1++)
				{
					struct custom_float_array root_data;

					for (int p = 0; p < cfg_pooling_size[layer]; p++)
					for (int q = 0; q < cfg_pooling_size[layer]; q++)
					{
						int out2_buf_idx_base = ((r1 * cfg_pooling_size[layer] + p) * BS_C + (c1 * cfg_pooling_size[layer] + q)) * O * BS_O + o1 * O;
#pragma unroll 32
						for (int o2 = 0; o2 < O; o2++)
						{
						//	int out2_buf_idx = ((r1 * cfg_pooling_size[layer] + p) * BS_C + (c1 * cfg_pooling_size[layer] + q)) * O * BS_O + o1 * O + o2;
				
						//	if ((p || q) == 0)
						//		root_data.drain_data[o2] = out2_buf[out2_buf_idx_base + o2];
						//	else if ((root_data.drain_data[o2] - out2_buf[out2_buf_idx_base + o2]) < 0)
						//		root_data.drain_data[o2] = out2_buf[out2_buf_idx_base + o2];
							root_data.drain_data[o2] = out2_buf[out2_buf_idx_base + o2];
						}
					}

					if (layer == num_conv - 1)
					{
						write_channel_altera(ch_write_to_ddr, root_data);
					}
					else
					{
						write_channel_altera(ch_zero_padding, root_data);
					}
				}
			}
		}
	}
}
