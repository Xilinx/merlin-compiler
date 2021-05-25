// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.



#undef __SYNTHESIS__

#if (WINOGRAD == 1)
void PE_dot_product_and_acc(vec_input_t a, vec_input_t b, data_bitwidth *accum_shift_reg, char new_row_col_pair)
{
#pragma HLS inline // very important!!!

	data_bitwidth sum = 0;
	data_bitwidth accum = 0;
#pragma HLS RESOURCE variable=sum core=DSP48
#pragma HLS RESOURCE variable=accum core=AddSub_DSP

	new_row_col_pair = new_row_col_pair & 1;

//	sum += a * b;
	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		sum += a.data[i2] * b.data[i2];
	}

	accum = sum + (new_row_col_pair ? 0 : accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1]);

	for (int i = ACCUM_SHIFT_REG_SIZE  - 1; i >= 1; i--)
	{
#pragma HLS UNROLL
		accum_shift_reg[i] = accum_shift_reg[i - 1];
	}

	accum_shift_reg[0] = accum;
}
#else
void PE_dot_product_and_acc(vec_input_t *a, vec_input_t *b, data_bitwidth *accum_shift_reg, char new_row_col_pair)
{
#pragma HLS inline // very important!!!

	data_bitwidth sum = 0;
	data_bitwidth accum = 0;
#pragma HLS RESOURCE variable=sum core=DSP48
#pragma HLS RESOURCE variable=accum core=AddSub_DSP

	new_row_col_pair = new_row_col_pair & 1;

//	sum += a * b;
	for (int q = 0; q < K2; q++)
	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		sum += a[q].data[i2] * b[q].data[i2];
	}

	accum = sum + (new_row_col_pair ? 0 : accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1]);

	for (int i = ACCUM_SHIFT_REG_SIZE  - 1; i >= 1; i--)
	{
#pragma HLS UNROLL
		accum_shift_reg[i] = accum_shift_reg[i - 1];
	}

	accum_shift_reg[0] = accum;
}
#endif // WINOGRAD

static void	PE_0_kernel(hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<o_data_from_pe> &ch_data_o_out)
{
#if (WINOGRAD == 1)
	data_bitwidth PE_accum_shift_reg[C + K2 - 1][ACCUM_SHIFT_REG_SIZE];
#else
	data_bitwidth PE_accum_shift_reg[C][ACCUM_SHIFT_REG_SIZE];
#endif

#pragma HLS array_partition variable=PE_accum_shift_reg dim=2 complete 

#ifndef __SYNTHESIS__
#if (WINOGRAD == 1)
	for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
#else
	for (int c2 = 0; c2 < C; c2++)
#endif
#pragma HLS UNROLL
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
#if (WINOGRAD == 1)
		PE_accum_shift_reg[c2_q][i] = 0;
#else
		PE_accum_shift_reg[c2][i] = 0;
#endif
	}
#endif // __SYNTHESIS__

	// outer loops
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	{
		// middle loops
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int p = 0; p < K1; p++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int it = 0; it < (TOTAL_NUM_BLOCKS * K1 * BS_I * BS_R * BS_C * BS_O); it++)
	{
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false
//#pragma HLS dependence variable=PE_accum_shift_reg[0][0][0] array inter false

#pragma HLS PIPELINE
		struct in_data_feeder_to_pe a_local;
		char new_row_col_pair_local;

		struct wt_data_feeder_to_pe b_local;

		struct o_data_from_pe c_local_out;	

		a_local = ch_data_in_in.read();

		//	i0 = blk % BN_I;
		//	ro0 = blk / BN_I;
		//	if (ro0 < (BN_R * BN_O))
		//	{
		//		o0 = ro0 % BN_O;
		//		r0 = ro0 / BN_O;
		//	}
		//	else
		//	{
		//		o0 = BN_O - 1;
		//		r0 = BN_R - 1;
		//	}

		new_row_col_pair_local = a_local.new_row_col_pair;

		//	if (((r0 > 0) || (ro0 >= (BN_R * BN_O))) && (i0 == 0) && (i1 == 0))
		//	{
		//		new_row_col_pair_local = 1;
		//	}
		//	else
		//	{
		//		new_row_col_pair_local = 0;
		//	}

		b_local = ch_data_wt_in.read();

		ch_data_in_out << a_local;
		//	ch_data_wt_out << b_local;


		// 2. fill O_out_shift_reg
		if (new_row_col_pair_local)
		{
#if (WINOGRAD == 1)
			for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
			{
#pragma HLS UNROLL
				c_local_out.data[c2_q] = PE_accum_shift_reg[c2_q][ACCUM_SHIFT_REG_SIZE - 1];
			//	c_local_out.data[c2_q] = 0;
			}
#else
			for (int c2 = 0; c2 < C; c2++)
			{
#pragma HLS UNROLL
				c_local_out.data[c2] = PE_accum_shift_reg[c2][ACCUM_SHIFT_REG_SIZE - 1];
			}
#endif

			ch_data_o_out << c_local_out;
		}

		// Computation
#if (WINOGRAD == 1)
		for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
		{
#pragma HLS UNROLL
			vec_input_t vec_in;
			vec_input_t vec_wt;

			ap_ifmap_data_to_vec_in_data(a_local.pe_data, vec_in.data, c2_q);
			ap_filter_data_to_vec_in_data(b_local.pe_data, vec_wt.data, c2_q);

		//	for (int i2 = 0; i2 < I; i2++)
		//	{
		//		vec_in.data[i2] = 0;
		//		vec_wt.data[i2] = 0;
		//	}

			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg[c2_q], new_row_col_pair_local);
		}
#else
		// inner loops
		for (int c2 = 0; c2 < C; c2++)
		{
#pragma HLS UNROLL
			vec_input_t vec_in[K2];
			vec_input_t vec_wt[K2];

			for (int q = 0; q < K2; q++)
			{
#pragma HLS UNROLL
				ap_ifmap_data_to_vec_in_data(a_local.pe_data, vec_in[q].data, c2 + q);
				ap_filter_data_to_vec_in_data(b_local.pe_data, vec_wt[q].data, q);
			}

		//	ap_ifmap_data_to_vec_in_data(a_local.pe_data, vec_in.data, c2 + q);
		//	ap_filter_data_to_vec_in_data(b_local.pe_data, vec_wt.data, q);
		//	for (int i2 = 0; i2 < I; i2++)
		//	{
		//		vec_in.data[i2] = 1;
		//		vec_wt.data[i2] = 1;
		//	}
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg[c2], new_row_col_pair_local);
		}
#endif

		//	o1++;
		//	if (o1 == BS_O)

		//	o1++;
		//	if (o1 == BS_O)
		//	{
		//		o1 = 0;
		//		r1++;
		//		if (r1 == BS_R)
		//		{
		//			r1 = 0;
		//			i1++;
		//			if (i1 == BS_I)
		//			{
		//				i1 = 0;
		//				blk++;
		//				if (blk == TOTAL_NUM_BLOCKS)
		//				{
		//					blk = 0;
		//				}
		//			}
		//		}
		//	}
	}
	}
}

static void	PE_1_kernel(hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<o_data_from_pe> &ch_data_o_out)
{
#if (WINOGRAD == 1)
	data_bitwidth PE_accum_shift_reg[C + K2 - 1][ACCUM_SHIFT_REG_SIZE];
#else
	data_bitwidth PE_accum_shift_reg[C][ACCUM_SHIFT_REG_SIZE];
#endif

#pragma HLS array_partition variable=PE_accum_shift_reg dim=2 complete 

#ifndef __SYNTHESIS__
#if (WINOGRAD == 1)
	for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
#else
	for (int c2 = 0; c2 < C; c2++)
#endif
#pragma HLS UNROLL
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
#if (WINOGRAD == 1)
		PE_accum_shift_reg[c2_q][i] = 0;
#else
		PE_accum_shift_reg[c2][i] = 0;
#endif // WINOGRAD
	}
#endif // __SYNTHESIS__

	// outer loops
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	{
		// middle loops
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int p = 0; p < K1; p++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int it = 0; it < (TOTAL_NUM_BLOCKS * K1 * BS_I * BS_R * BS_C * BS_O); it++)
	{
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false
//#pragma HLS dependence variable=PE_accum_shift_reg[0][0][0] array inter false

#pragma HLS PIPELINE
		struct in_data_feeder_to_pe a_local;
		char new_row_col_pair_local;

		struct wt_data_feeder_to_pe b_local;

		struct o_data_from_pe c_local_out;	

		a_local = ch_data_in_in.read();

		//	i0 = blk % BN_I;
		//	ro0 = blk / BN_I;
		//	if (ro0 < (BN_R * BN_O))
		//	{
		//		o0 = ro0 % BN_O;
		//		r0 = ro0 / BN_O;
		//	}
		//	else
		//	{
		//		o0 = BN_O - 1;
		//		r0 = BN_R - 1;
		//	}

		new_row_col_pair_local = a_local.new_row_col_pair;

		//	if (((r0 > 0) || (ro0 >= (BN_R * BN_O))) && (i0 == 0) && (i1 == 0))
		//	{
		//		new_row_col_pair_local = 1;
		//	}
		//	else
		//	{
		//		new_row_col_pair_local = 0;
		//	}

		b_local = ch_data_wt_in.read();

		//ch_data_in_out << a_local;
		//ch_data_wt_out << b_local;


		// 2. fill O_out_shift_reg
		if (new_row_col_pair_local)
		{
#if (WINOGRAD == 1)
			for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
			{
#pragma HLS UNROLL
				c_local_out.data[c2_q] = PE_accum_shift_reg[c2_q][ACCUM_SHIFT_REG_SIZE - 1];
			//	c_local_out.data[c2_q] = 1;
			}
#else
			for (int c2 = 0; c2 < C; c2++)
			{
#pragma HLS UNROLL
				c_local_out.data[c2] = PE_accum_shift_reg[c2][ACCUM_SHIFT_REG_SIZE - 1];
			}
#endif

			ch_data_o_out << c_local_out;
		}

		// computation
#if (WINOGRAD == 1)
		for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
		{
#pragma HLS UNROLL
			vec_input_t vec_in;
			vec_input_t vec_wt;

			ap_ifmap_data_to_vec_in_data(a_local.pe_data, vec_in.data, c2_q);
			ap_filter_data_to_vec_in_data(b_local.pe_data, vec_wt.data, c2_q);

			for (int i2 = 0; i2 < I; i2++)
			{
			//	vec_in.data[i2] = 1;
			//	vec_wt.data[i2] = 1;
			}

			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg[c2_q], new_row_col_pair_local);
		}
#else
		// inner loops
		for (int c2 = 0; c2 < C; c2++)
		{
#pragma HLS UNROLL
			vec_input_t vec_in[K2];
			vec_input_t vec_wt[K2];

			for (int q = 0; q < K2; q++)
			{
#pragma HLS UNROLL
				ap_ifmap_data_to_vec_in_data(a_local.pe_data, vec_in[q].data, c2 + q);
				ap_filter_data_to_vec_in_data(b_local.pe_data, vec_wt[q].data, q);
			}

		//	for (int i2 = 0; i2 < I; i2++)
		//	{
		//		vec_in.data[i2] = 1;
		//		vec_wt.data[i2] = 1;
		//	}
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg[c2], new_row_col_pair_local);
		}
#endif

		//	o1++;
		//	if (o1 == BS_O)
		//	{
		//		o1 = 0;
		//		r1++;
		//		if (r1 == BS_R)
		//		{
		//			r1 = 0;
		//			i1++;
		//			if (i1 == BS_I)
		//			{
		//				i1 = 0;
		//				blk++;
		//				if (blk == TOTAL_NUM_BLOCKS)
		//				{
		//					blk = 0;
		//				}
		//			}
		//		}
		//	}
	}
	}
}

static void PE_0_wrapper(hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<o_data_from_pe> &ch_data_o_out)
{
	PE_0_kernel(ch_data_in_in, ch_data_wt_in, ch_data_in_out, ch_data_o_out);
}

static void PE_1_wrapper(hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<o_data_from_pe> &ch_data_o_out)
{
	PE_1_kernel(ch_data_in_in, ch_data_wt_in, ch_data_o_out);
}
