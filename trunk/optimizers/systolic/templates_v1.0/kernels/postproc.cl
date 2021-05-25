
/*
void addr_trans(float *out0_buf, float *out1_buf)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		int out1_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
		int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * BS_O * O + o1 * O + o2;

		out1_buf[out1_buf_idx] = out0_buf[out0_buf_idx];
	}
}
*/

/*
void relu_sw(int layer, float *out1_buf, float *out2_buf)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		int out1_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
		int out2_buf_idx = out1_buf_idx;

		out2_buf[out2_buf_idx] = out1_buf[out1_buf_idx];
	}
}
*/

void pooling_sw(int layer,
		float *out2_buf, float *out3_buf, 
		int r0, int c0, int o0)
{
//	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
//	int BS_C_scaled = BS_C / cfg_pooling_size[layer];
	int BS_R_R_scaled = ((R * BS_R) >> (cfg_pooling_size[layer] - 1));
	int BS_C_scaled = (BS_C >> (cfg_pooling_size[layer] - 1));

//	for (int r1 = 0; r1 < BS_R_R_scaled; r1++)
//	for (int c1 = 0; c1 < BS_C_scaled; c1++)
//	for (int o1 = 0; o1 < O * BS_O; o1++) 
//	{
//		float max = -1.0;
//		int out3_buf_idx = (r1 * BS_C_scaled + c1) * O * BS_O + o1;
//
//		for (int p = 0; p < cfg_pooling_size[layer]; p++)
//		for (int q = 0; q < cfg_pooling_size[layer]; q++)
//		{
//			int out2_buf_idx = ((r1 * cfg_pooling_size[layer] + p) * BS_C + (c1 * cfg_pooling_size[layer] + q)) * O * BS_O + o1;
//
//			if ((max + 1.0) < 1e-6 || (max - out2_buf[out2_buf_idx]) < 0)
//				max = out2_buf[out2_buf_idx];
//		}
//
//		out3_buf[out3_buf_idx] = max;
//	}

//	for (int r1 = 0; r1 < BS_R_R_scaled; r1++)
//	for (int c1 = 0; c1 < BS_C_scaled; c1++)
	int r1 = 0;
	int c1 = 0;
	while (1)
	{
		int out3_buf_idx_base = (r1 * BS_C_scaled + c1) * O * BS_O;

		for (int p = 0; p < cfg_pooling_size[layer]; p++)
		for (int q = 0; q < cfg_pooling_size[layer]; q++)
		{
			int out2_buf_idx_base = ((r1 * cfg_pooling_size[layer] + p) * BS_C + (c1 * cfg_pooling_size[layer] + q)) * O * BS_O;

#pragma unroll 32
			for (int o1 = 0; o1 < O * BS_O; o1++) 
			{
				int out3_buf_idx = out3_buf_idx_base + o1;
				int out2_buf_idx = out2_buf_idx_base + o1;

				if ((p | q) == 0)
				{
					out3_buf[out3_buf_idx] = out2_buf[out2_buf_idx];
				}
				else
				{
					if (out3_buf[out3_buf_idx] - out2_buf[out2_buf_idx] < 0)
						out3_buf[out3_buf_idx] = out2_buf[out3_buf_idx];
				}
			}
		}

		c1++;
		if (c1 == BS_C_scaled)
		{
			c1 = 0;
			r1++;
			if (r1 == BS_R_R_scaled)
			{
				r1 = 0;
				break;
			}
		}
	}
}

int zero_padding_sw_little_out5_idx(int layer, int r0_p, int c0_p, int i0, int rr_p, int cc_p, int ii)
{
	return ((((r0_p) * cfg_BN_C[layer] + (c0_p)) * cfg_BN_I[layer] + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr_p) * (BS_C + K - 1) + (cc_p)) * BS_I * I + ii);
}

void zero_padding_sw_little(int layer,
		float *out3_buf, __global float *restrict out5, 
		int r0, int c0, int i0)
{
//	const unsigned int cfg_BN_R = (cfg_img_row[layer + 1] + R * BS_R - 1) / (R * BS_R);
//	const unsigned int cfg_BN_C = (cfg_img_col[layer + 1] + BS_C - 1) / BS_C;
//	const unsigned int cfg_BN_I = (cfg_in_num[layer + 1] + BS_I * I - 1) / (BS_I * I);

//	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
//	int BS_C_scaled = BS_C / cfg_pooling_size[layer];
	int BS_R_R_scaled = ((R * BS_R) >> (cfg_pooling_size[layer] - 1));
	int BS_C_scaled = (BS_C >> (cfg_pooling_size[layer] - 1));

//	int blk_idx_r0 = r0 / cfg_pooling_size[layer];
//	int blk_idx_c0 = c0 / cfg_pooling_size[layer];
	int blk_idx_r0 = (r0 >> (cfg_pooling_size[layer] - 1));
	int blk_idx_c0 = (c0 >> (cfg_pooling_size[layer] - 1));
	int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
	int subblk_idx_c0 = c0 % cfg_pooling_size[layer];


	// part 1, 2, 3, 4
//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
//	for (int cc = 0; cc < BS_C_scaled; cc++)
//	for (int ii = 0; ii < (BS_I * I); ii++) 
//	{
//		int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
//		int rr_p = (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr;
//		int cc_p = (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc;
//	//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
//		int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
//
//		out5[out5_idx] = out3_buf[out3_buf_idx];
//	}

//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
//	for (int cc = 0; cc < BS_C_scaled; cc++)
	int rr = 0;
	int cc = 0;
	while (1)
	{
		int out3_buf_idx_base = (rr * BS_C_scaled + cc) * BS_I * I;
		int rr_p = (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr;
		int cc_p = (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc;
		int out5_idx_base = ((blk_idx_r0 * cfg_BN_C[layer] + blk_idx_c0) * cfg_BN_I[layer] + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I;
#pragma unroll 32
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			int out3_buf_idx = out3_buf_idx_base + ii;
			int out5_idx = out5_idx_base + ii;

			out5[out5_idx] = out3_buf[out3_buf_idx];
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
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 - 1, blk_idx_c0 - 1, i0, rr_p, cc_p, ii);

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr;
				int cc_p = cc;
			//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
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

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr;
				int cc_p = cc + ((K - 1) / 2) + subblk_idx_c0 * BS_C_scaled;
			//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
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
			if (blk_idx_r0 > 0 && blk_idx_c0 < cfg_BN_C[layer] - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = (K - 1) / 2 + R * BS_R + rr;
				int cc_p = cc - (BS_C_scaled - ((K - 1) / 2));
			//	int out5_idx = (((blk_idx_r0 - 1) * cfg_BN_C + blk_idx_c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 - 1, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr;
				int cc_p = cc + BS_C - BS_C_scaled + (K - 1);
			//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);


				out5[out5_idx] = 0;
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
				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
				int cc_p = cc;
			//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
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
			if (blk_idx_r0 < cfg_BN_R[layer] - 1 && blk_idx_c0 > 0)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr + (K - 1) / 2 - BS_R_R_scaled;
				int cc_p = cc + BS_C + ((K - 1) / 2);
			//	int out5_idx = (((blk_idx_r0 + 1) * cfg_BN_C + blk_idx_c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 + 1, blk_idx_c0 - 1, i0, rr_p, cc_p, ii);

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
				int cc_p = cc;
			//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
			//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
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
			if (blk_idx_r0 < cfg_BN_R[layer] - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr - (BS_R_R_scaled - (K - 1) / 2);
				int cc_p = cc + (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled;
			//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0 + 1, blk_idx_c0, i0, rr_p, cc_p, ii);
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 + 1, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
				int cc_p = cc + (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled;
			//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
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
			if (blk_idx_r0 < cfg_BN_R[layer] - 1 && blk_idx_c0 < cfg_BN_C[layer] - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr - (BS_R_R_scaled - (K - 1) / 2);
				int cc_p = cc + (K - 1) / 2 - BS_C_scaled;
		//		int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0 + 1, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0 + 1, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
				int cc_p = cc + BS_C + (K - 1) - BS_C_scaled;
			//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
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
			if (blk_idx_c0 < cfg_BN_C[layer] - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
				int cc_p = cc + (K - 1) / 2 - BS_C_scaled;
			//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
			//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);


				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
				int cc_p = cc + BS_C + (K - 1) - BS_C_scaled;
			//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);
				int out5_idx = zero_padding_sw_little_out5_idx(layer, blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
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
}

void write_to_ddr_little(int layer,
		float *out3_buf, __global float *restrict out3,
		int r0, int c0, int o0)
{
//	int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
//	int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
//	int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

//	int pooled_BN_R = (cfg_BN_R[layer] + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
//	int pooled_BN_C = (cfg_BN_C[layer] + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
	int pooled_BN_R = (cfg_BN_R[layer] + cfg_pooling_size[layer] - 1) >> (cfg_pooling_size[layer] - 1);
	int pooled_BN_C = (cfg_BN_C[layer] + cfg_pooling_size[layer] - 1) >> (cfg_pooling_size[layer] - 1);


//	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
//	int BS_C_scaled = BS_C / cfg_pooling_size[layer];
	int BS_R_R_scaled = (R * BS_R) >> (cfg_pooling_size[layer] - 1);
	int BS_C_scaled = BS_C >> (cfg_pooling_size[layer] - 1);

//	int blk_idx_r0 = r0 / cfg_pooling_size[layer];
//	int blk_idx_c0 = c0 / cfg_pooling_size[layer];
	int blk_idx_r0 = r0 >> (cfg_pooling_size[layer] - 1);
	int blk_idx_c0 = c0 >> (cfg_pooling_size[layer] - 1);

	int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
	int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
//	for (int cc = 0; cc < BS_C_scaled; cc++)
//	for (int oo = 0; oo < O * BS_O; oo++)
//	{
//		int out3_buf_idx = (rr * BS_C_scaled + cc) * O * BS_O + oo;
//		int out3_idx = ((blk_idx_r0 * pooled_BN_C + blk_idx_c0) * cfg_BN_O[layer] + o0) * R * BS_R * BS_C * O * BS_O + ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + (subblk_idx_c0 * BS_C_scaled + cc)) * O * BS_O + oo;
//
//		out3[out3_idx] = out3_buf[out3_buf_idx];
//	}

//	for (int rr = 0; rr < BS_R_R_scaled; rr++)
//	for (int cc = 0; cc < BS_C_scaled; cc++)
	int rr = 0;
	int cc = 0;
	while (1)
	{
		int out3_buf_idx_base = (rr * BS_C_scaled + cc) * O * BS_O;
		int out3_idx_base = ((blk_idx_r0 * pooled_BN_C + blk_idx_c0) * cfg_BN_O[layer] + o0) * R * BS_R * BS_C * O * BS_O + ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + (subblk_idx_c0 * BS_C_scaled + cc)) * O * BS_O;
#pragma unroll 32
		for (int oo = 0; oo < O * BS_O; oo++)
		{
			int out3_buf_idx = out3_buf_idx_base + oo;
			int out3_idx = out3_idx_base + oo;

			out3[out3_idx] = out3_buf[out3_buf_idx];
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
}

__attribute__((max_global_work_dim(0)))
__kernel void postproc_kernel(__global float *restrict out3, 
		__global float *restrict out5, __global float *restrict out5_copy, int num_conv) 
//__attribute__((max_global_work_dim(0)))
//__kernel void postproc(__global float *restrict out3) 
{
	float out0_buf[R * BS_R * BS_C * BS_O * O];
//	float out1_buf[R * BS_R * BS_C * O * BS_O];
//	float out2_buf[R * BS_R * BS_C * O * BS_O];
//	float out3_buf[R * BS_R * BS_C * O * BS_O];

	for (int layer = 0; layer < num_conv; layer++)
	{
	//	for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
	//	for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
	//	for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
		int r0 = 0;
		int c0 = 0;
		int o0 = 0;
		while (1)
		{
			// collect data within a block
		//	for (int r2 = 0; r2 < R; r2++)
		//	for (int r1 = 0; r1 < BS_R; r1++)
		//	for (int c1 = 0; c1 < BS_C; c1++)
			int r2 = 0;
			int r1 = 0;
			int c1 = 0;
			while (1)
			{
				for (int o1 = 0; o1 < BS_O; o1++)
				{
					struct custom_float_array root_data;

					root_data = read_channel_altera(ch_drain_to_postproc);
#pragma unroll 32
					for (int o2 = 0; o2 < O; o2++)
					{
						// perform address transformation immediately out of drain and relu
					//	int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * BS_O * O + o1 * O + o2;
						int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * BS_O * O + o2 * BS_O + o1;
						out0_buf[out0_buf_idx] = root_data.drain_data[o2];

					}
				}

				c1++;
				if (c1 == BS_C)
				{
					c1 = 0;
					r1++;
					if (r1 == BS_R)
					{
						r1 = 0;
						r2++;
						if (r2 == R)
						{
							r2 = 0;
							break;
						}
					}
				}
			}

			// address transform (out0->out1)
		//	addr_trans(out0_buf, out1_buf);

			// relu (out1->out2)
		//	relu_sw(layer, out1_buf, out2_buf);
		//	relu_sw(layer, out0_buf, out0_buf);

			// pooling (out2->out3)
			if (cfg_pooling_size[layer] > 1)
			{
			//	pooling_sw(layer, out2_buf, out3_buf, r0, c0, o0);
				pooling_sw(layer, out0_buf, out0_buf, r0, c0, o0);
			}

			if (layer < num_conv - 1)
			{
				// boundary padding
			//	if (cfg_pooling_size[layer] > 1)
			//	{
					if ((layer & 1) == 0)
					{
					//	zero_padding_sw_little(layer, out3_buf, out5_copy, r0, c0, o0);
						zero_padding_sw_little(layer, out0_buf, out5_copy, r0, c0, o0);
					}
					else
					{
					//	zero_padding_sw_little(layer, out3_buf, out5, r0, c0, o0);
						zero_padding_sw_little(layer, out0_buf, out5, r0, c0, o0);
					}
			//	}
			//	else
			//	{
			//		if ((layer & 1) == 0)
			//			zero_padding_sw_big(layer, out2_buf, out5_copy, r0, c0, o0);
			//		else
			//			zero_padding_sw_big(layer, out2_buf, out5, r0, c0, o0);
			//	}

			}
			if (layer == num_conv - 1)
			{
				// write to DDR
			//	if (cfg_pooling_size[layer] > 1)
			//	{
				//	write_to_ddr_little(layer, out3_buf, out3, r0, c0, o0);
					write_to_ddr_little(layer, out0_buf, out3, r0, c0, o0);
			//	}
			//	else
			//	{
			//		write_to_ddr_big(layer, out2_buf, out3, r0, c0, o0);
			//	}
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

		//	if (layer < NUM_CONV - 1) {
			mem_fence(CLK_CHANNEL_MEM_FENCE);
				write_channel_altera(ch_msg_loader_in_ready, layer);
			mem_fence(CLK_CHANNEL_MEM_FENCE);
				write_channel_altera(ch_msg_loader_w_ready, layer);
			mem_fence(CLK_CHANNEL_MEM_FENCE);
		//	}
	}
}
