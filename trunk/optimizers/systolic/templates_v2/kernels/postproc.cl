
void relu_sw(int layer, float *out1_buf, float *out2_buf)
{
//	for (int r2 = 0; r2 < R; r2++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int c1 = 0; c1 < BS_C; c1++)
//	for (int o2 = 0; o2 < O; o2++)
//	for (int o1 = 0; o1 < BS_O; o1++)
//	{
//		int out1_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
//		int out2_buf_idx = out1_buf_idx;
//
//		out2_buf[out2_buf_idx] = out1_buf[out1_buf_idx];
//	}
	int ub = R * BS_R * BS_C * O * BS_O;

#pragma unroll 16
	for (int i = 0; i < ub; i++)
	{
		float data = out1_buf[i];
		out2_buf[i] = (data - 1e-9 > 0) ? data : 0;
	}
}

void pooling_sw(int layer,
		float *out2_buf, float *out3_buf, 
		int r0, int c0, int o0)
{
//	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
//	int BS_C_scaled = BS_C / cfg_pooling_size[layer];
	int BS_R_R_scaled = ((R * BS_R) >> (cfg_pooling_size[layer] - 1));
	int BS_C_scaled = (BS_C >> (cfg_pooling_size[layer] - 1));

	for (int r1 = 0; r1 < BS_R_R_scaled; r1++)
	for (int c1 = 0; c1 < BS_C_scaled; c1++)
	for (int o1 = 0; o1 < O * BS_O; o1++) 
	{
		float max = -1.0;
		int out3_buf_idx = (r1 * BS_C_scaled + c1) * O * BS_O + o1;

		for (int p = 0; p < cfg_pooling_size[layer]; p++)
		for (int q = 0; q < cfg_pooling_size[layer]; q++)
		{
			int out2_buf_idx = ((r1 * cfg_pooling_size[layer] + p) * BS_C + (c1 * cfg_pooling_size[layer] + q)) * O * BS_O + o1;

			if ((max + 1.0) < 1e-6 || (max - out2_buf[out2_buf_idx]) < 0)
				max = out2_buf[out2_buf_idx];
		}

		out3_buf[out3_buf_idx] = max;
	}
}

void zero_padding_sw(int layer,
		float *out4_buf, float *out5_buf,
		__global float *restrict out5, 
		int r0, int c0, int i0)
{
#if 0
	int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	int cfg_BN_I = (cfg_in_num[layer + 1] + BS_I * I - 1) / (BS_I * I);

	int pooled_BN_R = (cfg_BN_R + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
	int pooled_BN_C = (cfg_BN_C + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];

	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
	int BS_C_scaled = BS_C / cfg_pooling_size[layer];

	int blk_idx_r0 = r0 / cfg_pooling_size[layer];
	int blk_idx_c0 = c0 / cfg_pooling_size[layer];

	int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
	int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

	// Perform block merging at (r,c) on-chip
	for (int rr = 0; rr < BS_R_R_scaled; rr++)
	for (int cc = 0; cc < BS_C_scaled; cc++)
	for (int ii = 0; ii < BS_I * I; ii++)
	{
		int out4_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
		int out5_buf_idx = (((K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr) * (BS_C + K - 1) + ((K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc)) * BS_I * I + ii;

		out5_buf[out5_buf_idx] = out4_buf[out4_buf_idx];
	}

	// Write to DDR
	if (subblk_idx_c0 == (cfg_pooling_size[layer] - 1) || c0 == (cfg_BN_C - 1))
	{
		for (int rr = 0; rr < BS_R_R_scaled; rr++)
		for (int c1 = 0; c1 < BS_C + K - 1; c1++)
		{
//#pragma unroll O
			for (int ii = 0; ii < BS_I * I; ii++)
			{
				int out5_buf_idx = (((K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr) * (BS_C + K - 1) + c1) * BS_I * I + ii;
				int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + out5_buf_idx;

				out5[out5_idx] = out5_buf[out5_buf_idx];
			}
		}
	}
#endif
}

channel struct custom_float_array ch_debug_ddr_bandwidth __attribute__((depth(1)));

void write_to_ddr(int layer,
		float *out3_buf, float *out4_buf,
		int r0, int c0, int o0)
{
	int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	int pooled_BN_R = (cfg_BN_R + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
	int pooled_BN_C = (cfg_BN_C + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];


	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
	int BS_C_scaled = BS_C / cfg_pooling_size[layer];

	int blk_idx_r0 = r0 / cfg_pooling_size[layer];
	int blk_idx_c0 = c0 / cfg_pooling_size[layer];

	int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
	int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

	// Perform block merging at (r,c) on-chip
//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
//	for (int cc = 0; cc < BS_C_scaled; cc++)
//	for (int oo = 0; oo < O * BS_O; oo++)
//	{
//		int out3_buf_idx = (rr * BS_C_scaled + cc) * O * BS_O + oo;
//		int out4_buf_idx = ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + (subblk_idx_c0 * BS_C_scaled + cc)) * O * BS_O + oo;
//
//		out4_buf[out4_buf_idx] = out3_buf[out3_buf_idx];
//	}

	int out4_buf_idx_base = (subblk_idx_r0 * BS_R_R_scaled * BS_C + subblk_idx_c0 * BS_C_scaled) * O * BS_O;
	int ub = BS_C_scaled * O * BS_O;
	for (int rr = 0; rr < BS_R_R_scaled; rr++)
	{
#pragma unroll 16
	for (int i = 0; i < ub; i++)
	{
		int out3_buf_idx = rr * BS_C_scaled * O * BS_O + i;
		int out4_buf_idx = rr * BS_C * O * BS_O + i;
		out4_buf[out4_buf_idx_base + out4_buf_idx] = out3_buf[out3_buf_idx];
	}
	}

	// Write to DDR
	if (subblk_idx_c0 == (cfg_pooling_size[layer] - 1) || c0 == (cfg_BN_C - 1))
	{
		struct custom_float_array data;

		for (int rr = 0; rr < BS_R_R_scaled; rr++)
		for (int c1 = 0; c1 < BS_C; c1++)
		{
#pragma unroll 16
			for (int oo = 0; oo < O * BS_O; oo++)
			{
				int out4_buf_idx = ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + c1) * O * BS_O + oo;
			//	int out3_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + out4_buf_idx;

			//	out3[out3_idx] = out4_buf[out4_buf_idx];
				data.drain_data[oo] = out4_buf[out4_buf_idx];
			}
			write_channel_altera(ch_debug_ddr_bandwidth, data);
		}
	}
}

//__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__kernel void postproc_kernel(
		__global float *restrict out5, __global float *restrict out5_copy, 
		int num_conv) 
{
	float out0_buf[R * BS_R * BS_C * BS_O * O];
	float out1_buf[(R * BS_R + K - 1) * (BS_C + K - 1) * O * BS_O];

	for (int layer = 0; layer < /*num_conv*/ 1; layer++)
	{
		for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
		for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
		for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int r1 = 0; r1 < BS_R; r1++)
			for (int c1 = 0; c1 < BS_C; c1++)
			for (int o1 = 0; o1 < BS_O; o1++)
			{
				struct custom_float_array root_data;

				root_data = read_channel_altera(ch_drain_to_postproc);

				for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
				{
					int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
					out0_buf[out0_buf_idx] = root_data.drain_data[o2];
				}
			}

			// relu (out1->out2)
			relu_sw(layer, out0_buf, out0_buf);

			// pooling (out2->out3)
			if (cfg_pooling_size[layer] > 1)
			{
				pooling_sw(layer, out0_buf, out0_buf, r0, c0, o0);
			}

		//	if (layer < NUM_CONV - 1)
		//	{
		//		if ((layer & 1) == 0)
		//		{
		//			zero_padding_sw(layer, out0_buf, out1_buf, out5_copy, r0, c0, o0);
		//		}
		//		else
		//		{
		//			zero_padding_sw(layer, out0_buf, out1_buf, out5, r0, c0, o0);
		//		}
		//	}
		//	else
		//	{
		//		write_to_ddr(layer, out0_buf, out1_buf, out3, r0, c0, o0);
		//	}
			write_to_ddr(layer, out0_buf, out1_buf, r0, c0, o0);
		}

		//	if (layer < NUM_CONV - 1) 
		//	{
		mem_fence(CLK_CHANNEL_MEM_FENCE);
		write_channel_altera(ch_msg_loader_in_ready, layer);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
		write_channel_altera(ch_msg_loader_w_ready, layer);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
		//	}
	}
}
