
int zero_padding_sw_little_out5_idx(int layer, int r0_p, int c0_p, int i0, int rr_p, int cc_p, int ii)
{
	return ((((r0_p) * cfg_BN_C[layer + 1] + (c0_p)) * cfg_BN_I[layer + 1] + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr_p) * (BS_C + K - 1) + (cc_p)) * BS_I * I + ii);
}

__attribute__((max_global_work_dim(0)))
__kernel void zero_padding_kernel(__global data_precision *restrict out5, __global data_precision *restrict out5_copy, int num_conv) 
{
//	data_precision out5_buf[(R * BS_R + K - 1) * (BS_C + K - 1) * O * BS_O];
	data_precision out3_buf[(R * BS_R) * (BS_C) * O * BS_O];

	__global data_precision *restrict out;

	for (unsigned char layer = 0; layer < num_conv - 1; layer++)
	{
		if ((layer & 1) == 0)
		{
			out = out5_copy;
		}
		else
		{
			out = out5;
		}

		int BS_R_R_scaled = ((R * BS_R) >> (cfg_pooling_size[layer] - 1));
		int BS_C_scaled = (BS_C >> (cfg_pooling_size[layer] - 1));

	//	for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
	//	for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
	//	for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
	//	for (int i0 = 0; i0 < cfg_BN_O[layer]; i0++)
		int r0 = 0;
		int c0 = 0;
		int i0 = 0;
		while (1)
		{
			int blk_idx_r0 = (r0 >> (cfg_pooling_size[layer] - 1));
			int blk_idx_c0 = (c0 >> (cfg_pooling_size[layer] - 1));
			int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
			int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

		//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
		//	for (int cc = 0; cc < BS_C_scaled; cc++)
		//	for (int o1 = 0; o1 < BS_O; o1++)
			int rr = 0;
			int cc = 0;
			int o1 = 0;
			while (1)
			{
				int out3_buf_idx_base = (rr * BS_C_scaled + cc) * BS_O * O + o1 * O;

				struct custom_float_array root_data;

				root_data = read_channel_altera(ch_zero_padding);
#pragma unroll 32
				for (int o2 = 0; o2 < O; o2++)
				{
					out3_buf[out3_buf_idx_base + o2] = root_data.drain_data[o2];
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

			// part 1, 2, 3, 4
		//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
		//	for (int cc = 0; cc < BS_C_scaled; cc++)
			rr = 0;
			cc = 0;
			while (1)
			{
				int rr_p = (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr;
				int cc_p = (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc;
#pragma unroll 32
				for (int ii = 0; ii < (BS_I * I); ii++) 
				{
					int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
					int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

					//	out5[out5_idx] = out3_buf[out3_buf_idx];
					out[out5_idx] = out3_buf[out3_buf_idx];
				}

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
		
			// part 5
			if (subblk_idx_r0 == 0 && subblk_idx_c0 == 0)
			{
			//	for (int rr = 0; rr < (K - 1) / 2; rr++)
			//	for (int cc = 0; cc < (K - 1) / 2; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = 0;
				int cc = 0;
				int ii = 0;
				while (1)
				{
					if (blk_idx_r0 > 0 && blk_idx_c0 > 0)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = (K - 1) / 2 + R * BS_R + rr;
						int cc_p = (K - 1) / 2 + BS_C + cc;
					//	int out5_idx = (((blk_idx_r0 - 1) * cfg_BN_C + blk_idx_c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer,  blk_idx_r0 - 1, blk_idx_c0 - 1, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr;
						int cc_p = cc;
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == (BS_I * I))
					{
						ii = 0;
						cc++;
						if (cc == ((K - 1) / 2))
						{
							cc = 0;
							rr++;
							if (rr == ((K - 1) / 2))
							{
								rr = 0;
								break;
							}
						}
					}
				}
			}
		
			// part 6.l, 6.r
			if (subblk_idx_r0 == 0)
			{
			//	for (int rr = 0; rr < (K - 1) / 2; rr++)
			//	for (int cc = 0; cc < BS_C_scaled; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = 0;
				int cc = 0;
				int ii = 0;
				while (1)
				{
					if (blk_idx_r0 > 0)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = (K - 1) / 2 + R * BS_R + rr;
						int cc_p = (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc;
					//	int out5_idx = (((blk_idx_r0 - 1) * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 - 1, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr;
						int cc_p = cc + ((K - 1) / 2) + subblk_idx_c0 * BS_C_scaled;
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == BS_I * I)
					{
						ii = 0;
						cc++;
						if (cc == BS_C_scaled)
						{
							cc = 0;
							rr++;
							if (rr == ((K - 1) / 2))
							{
								rr = 0;
								break;
							}
						}
					}
				}
			}
		
			// part 7
			if (subblk_idx_r0 == 0 && subblk_idx_c0 == cfg_pooling_size[layer] - 1)
			{
			//	for (int rr = 0; rr < (K - 1) / 2; rr++)
			//	for (int cc = BS_C_scaled - ((K - 1) / 2); cc < BS_C_scaled; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = 0;
				int cc = BS_C_scaled - ((K - 1) / 2);
				int ii = 0;
				while (1)
				{
					if (blk_idx_r0 > 0 && blk_idx_c0 < cfg_BN_C[layer + 1] - 1)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = (K - 1) / 2 + R * BS_R + rr;
						int cc_p = cc - (BS_C_scaled - ((K - 1) / 2));
					//	int out5_idx = (((blk_idx_r0 - 1) * cfg_BN_C[layer] + blk_idx_c0 + 1) * cfg_BN_I[layer] + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 - 1, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr;
						int cc_p = cc + BS_C - BS_C_scaled + (K - 1);
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
		
		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == (BS_I * I))
					{
						ii = 0;
						cc++;
						if (cc == BS_C_scaled)
						{
							cc = BS_C_scaled - ((K - 1) / 2);
							rr++;
							if (rr == ((K - 1) / 2))
							{
								rr = 0;
								break;
							}
						}
					}
				}
			}
		
			// part 8.u, 8.d
			if (subblk_idx_c0 == 0)
			{
			//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
			//	for (int cc = 0; cc < (K - 1) / 2; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = 0;
				int cc = 0;
				int ii = 0;
				while (1)
				{
					if (blk_idx_c0 > 0)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
						int cc_p = cc + ((K - 1) / 2) + BS_C;
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0 - 1, i0, rr_p, cc_p, ii);
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
						int cc_p = cc;
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == BS_I * I)
					{
						ii = 0;
						cc++;
						if (cc == ((K - 1) / 2))
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
			}
		
			// part 9
			if (subblk_idx_r0 == cfg_pooling_size[layer] - 1 && subblk_idx_c0 == 0)
			{
			//	for (int rr = BS_R_R_scaled - ((K - 1) / 2); rr < BS_R_R_scaled; rr++)
			//	for (int cc = 0; cc < (K - 1) / 2; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = BS_R_R_scaled - ((K - 1) / 2);
				int cc = 0;
				int ii = 0;
				while (1)
				{
					if (blk_idx_r0 < cfg_BN_R[layer + 1] - 1 && blk_idx_c0 > 0)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = rr + (K - 1) / 2 - BS_R_R_scaled;
						int cc_p = cc + BS_C + ((K - 1) / 2);
					//	int out5_idx = (((blk_idx_r0 + 1) * cfg_BN_C + blk_idx_c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 + 1, blk_idx_c0 - 1, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
						int cc_p = cc;
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
					//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == (BS_I * I))
					{
						ii = 0;
						cc++;
						if (cc == ((K - 1) / 2))
						{
							cc = 0;
							rr++;
							if (rr == BS_R_R_scaled)
							{
								rr = BS_R_R_scaled - ((K - 1) / 2);
								break;
							}
						}
					}
				}
			}
		
			// part 10.l, 10.r
			if (subblk_idx_r0 == cfg_pooling_size[layer] - 1)
			{
			//	for (int rr = BS_R_R_scaled - ((K - 1) / 2); rr < BS_R_R_scaled; rr++)
			//	for (int cc = 0; cc < BS_C_scaled; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = BS_R_R_scaled - ((K - 1) / 2);
				int cc = 0;
				int ii = 0;
				while (1)
				{
					if (blk_idx_r0 < cfg_BN_R[layer + 1] - 1)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = rr - (BS_R_R_scaled - (K - 1) / 2);
						int cc_p = cc + (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled;
					//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0 + 1, blk_idx_c0, i0, rr_p, cc_p, ii);
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 + 1, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
						int cc_p = cc + (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled;
					//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == (BS_I * I))
					{
						ii = 0;
						cc++;
						if (cc == BS_C_scaled)
						{
							cc = 0;
							rr++;
							if (rr == BS_R_R_scaled)
							{
								rr = BS_R_R_scaled - ((K - 1) / 2);
								break;
							}
						}
					}
				}
			}
		
			// part 11
			if (subblk_idx_r0 == cfg_pooling_size[layer] - 1 && subblk_idx_c0 == cfg_pooling_size[layer] - 1)
			{
			//	for (int rr = BS_R_R_scaled - ((K - 1) / 2); rr < BS_R_R_scaled; rr++)
			//	for (int cc = BS_C_scaled - ((K - 1) / 2); cc < BS_C_scaled; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = BS_R_R_scaled - ((K - 1) / 2);
				int cc = BS_C_scaled - ((K - 1) / 2);
				int ii = 0;
				while (1)
				{
					if (blk_idx_r0 < cfg_BN_R[layer + 1] - 1 && blk_idx_c0 < cfg_BN_C[layer + 1] - 1)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = rr - (BS_R_R_scaled - (K - 1) / 2);
						int cc_p = cc + (K - 1) / 2 - BS_C_scaled;
				//		int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0 + 1, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 + 1, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
						int cc_p = cc + BS_C + (K - 1) - BS_C_scaled;
					//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == (BS_I * I))
					{
						ii = 0;
						cc++;
						if (cc == BS_C_scaled)
						{
							cc = BS_C_scaled - ((K - 1) / 2);
							rr++;
							if (rr == BS_R_R_scaled)
							{
								rr = BS_R_R_scaled - ((K - 1) / 2);
								break;
							}
						}
					}
				}
			}
		
			// part 12.u, 12.d
			if (subblk_idx_c0 == cfg_pooling_size[layer] - 1)
			{
			//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
			//	for (int cc = BS_C_scaled - ((K - 1) / 2); cc < BS_C_scaled; cc++)
			//	for (int ii = 0; ii < (BS_I * I); ii++) 
				int rr = 0;
				int cc = BS_C_scaled - ((K - 1) / 2);
				int ii = 0;
				while (1)
				{
					if (blk_idx_c0 < cfg_BN_C[layer + 1] - 1)
					{
						int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
						int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
						int cc_p = cc + (K - 1) / 2 - BS_C_scaled;
					//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
					//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
		
		
					//	out5[out5_idx] = out3_buf[out3_buf_idx];
						out[out5_idx] = out3_buf[out3_buf_idx];
					}
					else
					{
						int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
						int cc_p = cc + BS_C + (K - 1) - BS_C_scaled;
					//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
						int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

		
					//	out5[out5_idx] = 0;
						out[out5_idx] = 0;
					}

					ii++;
					if (ii == (BS_I * I))
					{
						ii = 0;
						cc++;
						if (cc == BS_C_scaled)
						{
							cc = BS_C_scaled - ((K - 1) / 2);
							rr++;
							if (rr == BS_R_R_scaled)
							{
								rr = 0;
								break;
							}
						}
					}
				}
			}

			i0++;
			if (i0 == cfg_BN_O[layer])
			{
				i0 = 0;
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

	//	for (int i = 0; i < 1000; i++)
	//	{
	//		printf("OpenCL zero_padding %d: %.3f\n", i, out5_copy[i]);
	//	//	out5[i] = i;
	//	}

		mem_fence(CLK_CHANNEL_MEM_FENCE);
		write_channel_altera(ch_msg_loader_in_ready, layer);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
		write_channel_altera(ch_msg_loader_w_ready, layer);
		mem_fence(CLK_CHANNEL_MEM_FENCE);
	}
}
