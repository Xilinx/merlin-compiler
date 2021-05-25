
// for debugging DDR bandwidth for zero padding (1st layer, or layers w/o pooling)
__attribute__((max_global_work_dim(0)))
__kernel void zero_padding_debug_kernel(__global data_precision *restrict out5_copy, unsigned char num_conv) 
{
	for (int o0 = 0; o0 < cfg_BN_O[0]; o0++)
	for (int r0 = 0; r0 < cfg_BN_R[0]; r0++)
	for (int c0 = 0; c0 < cfg_BN_C[0]; c0++)
	{
		struct custom_float_array root_data;

	//	int out5_idx_base = ((r0 * cfg_BN_C[0] + c0) * cfg_BN_O[0] + o0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_O * O + (((K - 1) / 2 + rr) * (BS_C + K - 1) + c1) * BS_O * O + o1 * O;
		int out5_idx_base = ((r0 * cfg_BN_C[0] + c0) * cfg_BN_O[0] + o0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_O * O + ((K - 1) / 2) * (BS_C + K - 1) * BS_O * O;

		for (int i = 0; i < R * BS_R * (BS_C + K - 1) * BS_O; i)
		{
			root_data = read_channel_altera(ch_zero_padding_debug);

#pragma unroll 32
			for (int o2 = 0; o2 < O; o2++)
			{
			//	out5[out5_idx] = out5_buf[out5_buf_idx];
				out5_copy[out5_idx_base + i * O + o2] = root_data.data[o2];
			}
		}
	}
}
