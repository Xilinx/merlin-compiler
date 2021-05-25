
__attribute__((max_global_work_dim(0)))
__kernel void write_to_ddr_debug_kernel(__global float *restrict out3, int num_conv) 
{

//	for (int layer = 0; layer < /*num_conv*/1; layer++)
	{
		for (int r0 = 0; r0 < cfg_BN_R[0]; r0++)
		for (int c0 = 0; c0 < cfg_BN_C[0]; c0++)
		for (int o0 = 0; o0 < cfg_BN_O[0]; o0++)
		{
		//	printf("layer=%d, r0=%d, c0=%d, o0=%d\n", layer, r0, c0, o0);
		
			int out3_idx_base = ((r0 * cfg_BN_C[0] + c0) * cfg_BN_O[0] + o0) * R * BS_R * BS_C * O * BS_O;
		
		
			for (int i = 0; i < (R * BS_R * BS_C); i++) // 280
			{
				struct custom_float_array data = read_channel_altera(ch_debug_ddr_bandwidth);
		
			//	printf("i=%d\n", i);
		
		#pragma unroll 32
				for (int oo = 0; oo < O * BS_O; oo++) // 32
				{
					out3[out3_idx_base + i * O * BS_O + oo] = data.drain_data[oo];
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
