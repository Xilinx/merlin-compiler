
__attribute__((max_global_work_dim(0)))
__kernel void fast_conv_kernel(__global float *restrict out5, __global float *restrict out5_copy, int num_conv) 
{
	float in1_buf[(R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I];
	float out0_buf[R * BS_R * BS_C * O * BS_O];

	for (int layer = 0; layer < num_conv; layer++)
	{
		for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
		for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
		for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
		{
		//	for (int r2 = 0; r2 < R; r2++)
		//	for (int r1 = 0; r1 < BS_R; r1++)
		//	for (int c1 = 0; c1 < BS_C; c1++)
		//	for (int o2 = 0; o2 < O; o2++)
		//	for (int o1 = 0; o1 < BS_O; o1++)
		//	{
		//		int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
		//		out0_buf[out0_buf_idx] = 0.0;
		//	}
#pragma unroll 32
			for (int j = 0; j < R * BS_R * BS_C * O * BS_O; j++)
			{
				out0_buf[j] = 0;
			}

			for (int i0 = 0; i0 < cfg_BN_I[layer]; i0++)
			{
				// read
				for (int r_p = 0; r_p < R * BS_R + K - 1; r_p++)
				for (int c_q = 0; c_q < BS_C + K - 1; c_q++)
			//	for (int i1 = 0; i1 < BS_I; i1++)
			//	for (int i2 = 0; i2 < I; i2++)
#pragma unroll 32
				for (int ii = 0; ii < BS_I * I; ii++)
				{
				//	int out5_idx = ((r0 * cfg_BN_C[layer] + c0) * cfg_BN_I[i0]) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (r_p * (BS_C + K - 1) + c_q) * BS_I * I + i1 * I + i2;
				//	int in1_buf_idx = (r_p * (BS_C + K - 1) + c_q) * BS_I * I + i1 * I + i2;
					int out5_idx = ((r0 * cfg_BN_C[layer] + c0) * cfg_BN_I[i0]) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (r_p * (BS_C + K - 1) + c_q) * BS_I * I + ii;
					int in1_buf_idx = (r_p * (BS_C + K - 1) + c_q) * BS_I * I + ii;
					if ((layer & 1) == 0)
						in1_buf[in1_buf_idx] = out5[out5_idx];
					else
						in1_buf[in1_buf_idx] = out5_copy[out5_idx];
				}

				// compute
				for (int r2 = 0; r2 < R; r2++)
				for (int r1 = 0; r1 < BS_R; r1++)
				for (int c1 = 0; c1 < BS_C; c1++)
				for (int o2 = 0; o2 < O; o2++)
				for (int o1 = 0; o1 < BS_O; o1++)
				{
					int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
					for (int p = 0; p < cfg_kernel[layer]; p++)
					for (int q = 0; q < cfg_kernel[layer]; q++)
				//	for (int i1 = 0; i1 < BS_I; i1++)
				//	for (int i2 = 0; i2 < I; i2++)
#pragma unroll 32
					for (int ii = 0; ii < BS_I * I; ii++)
					{
					//	int in1_buf_idx = ((r2 * BS_R + r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + i1 * I + i2;
						int in1_buf_idx = ((r2 * BS_R + r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + ii;
						out0_buf[out0_buf_idx] += in1_buf[in1_buf_idx];

					//	printf("%f\n", out0_buf[out0_buf_idx]);
					}
				}
			}

			// write
			for (int r2 = 0; r2 < R; r2++)
			for (int r1 = 0; r1 < BS_R; r1++)
			for (int c1 = 0; c1 < BS_C; c1++)
			for (int o1 = 0; o1 < BS_O; o1++)
			{
				struct custom_float_array root_data;
#pragma unroll 32
				for (int o2 = 0; o2 < O; o2++)
				{
					int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o1 * O + o2;
					root_data.drain_data[o2] = out0_buf[out0_buf_idx];
				}
				write_channel_altera(ch_drain_to_postproc, root_data);
			}
		}
	}
}
