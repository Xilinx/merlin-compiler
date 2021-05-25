
//channel struct custom_float_array ch_relu_to_pooling __attribute__((depth(1)));

//__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__kernel void relu_kernel(unsigned char num_conv) 
{
//	float out0_buf[R * BS_R * BS_C * BS_O * O];
	data_precision out0_buf[BS_O * O];

	for (unsigned char layer = 0; layer < num_conv; layer++)
	{
		for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
		for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
		for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int r1 = 0; r1 < BS_R; r1++)
			for (int c1 = 0; c1 < BS_C; c1++)
			{
				for (int o1 = 0; o1 < BS_O; o1++)
				{
					struct custom_float_array root_data;

					root_data = read_channel_altera(ch_drain_to_postproc);
#pragma unroll 32
					for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
					{
						// perform address transformation and relu
					//	int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
						int out0_buf_idx = o2 * BS_O + o1;
						data_precision data = root_data.drain_data[o2];
					//	out0_buf[out0_buf_idx] = root_data.drain_data[o2];
						out0_buf[out0_buf_idx] = (data - 1e-6) > 0 ? data : 0;
					}
				}
				for (int o1 = 0; o1 < BS_O; o1++)
				{
					struct custom_float_array root_data;
#pragma unroll 32
					for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
					{
						int out0_buf_idx_p = o1 * O + o2;

						root_data.drain_data[o2] = out0_buf[out0_buf_idx_p];
					}
					write_channel_altera(ch_relu_to_pooling, root_data);
				}
			}
		}

		//	if (layer < NUM_CONV - 1) 
		//	{
	//	mem_fence(CLK_CHANNEL_MEM_FENCE);
	//	write_channel_altera(ch_msg_loader_in_ready, layer);
	//	mem_fence(CLK_CHANNEL_MEM_FENCE);
	//	write_channel_altera(ch_msg_loader_w_ready, layer);
	//	mem_fence(CLK_CHANNEL_MEM_FENCE);
		//	}
	}
}
