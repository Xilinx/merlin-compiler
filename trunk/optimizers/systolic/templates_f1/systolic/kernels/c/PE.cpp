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



//#undef __SYNTHESIS__

void PE_dot_product_and_acc(vec_input_t a, vec_input_t b, data_bitwidth *accum_shift_reg, char new_row_col_pair)
{
#pragma HLS inline // very important!!!

//	data_bitwidth sum = 0;
	data_bitwidth sum = 1;
	data_bitwidth accum = 0;
#pragma HLS RESOURCE variable=sum core=DSP48
#pragma HLS RESOURCE variable=accum core=AddSub_DSP

	new_row_col_pair = new_row_col_pair & 1;

//	sum += a * b; 
	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		sum += (a.data[i2] * b.data[i2]);
	//	sum = OR(sum, AND(a.data[i2], b.data[i2]));
	}

	accum = sum + (new_row_col_pair ? 0 : accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1]);

	for (int i = ACCUM_SHIFT_REG_SIZE  - 1; i >= 1; i--)
	{
#pragma HLS UNROLL
		accum_shift_reg[i] = accum_shift_reg[i - 1];
	}

//	accum_shift_reg[0] = accum;
	accum_shift_reg[0] = (accum - 1);
}

static void	PE_0_kernel( 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
	//	hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	data_bitwidth PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];

#pragma HLS array_partition variable=PE_accum_shift_reg complete 

#ifndef __SYNTHESIS__

// The following code for shift register initialization is ignored for synthesis
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		PE_accum_shift_reg[i] = 0;
	}

#endif

//	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int i = 0; i < (TOTAL_NUM_BLOCKS * BS_I * BS_R * BS_O); i++)
	{
#pragma HLS PIPELINE
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false

//		for (int i1 = 0; i1 < BS_I; i1++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct in_data_feeder_to_pe a_local;
			char new_row_col_pair_local;

			struct wt_data_feeder_to_pe b_local;

			struct o_data_in_pe c_local_out;	
			data_bitwidth c_local_out_data;
			bool c_local_out_valid;

			a_local = ch_data_in_in.read();

			new_row_col_pair_local = a_local.new_row_col_pair;

			b_local = ch_data_wt_in.read();

		//	if (pe_col < O - 1)
			{
				ch_data_in_out << a_local;
			}
		//	if (pe_row < R - 1)
			{
				ch_data_wt_out << b_local;
			}

			// 2. fill O_out_shift_reg
			if (new_row_col_pair_local)
			{
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = true;
			
				c_local_out.data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
				c_local_out_valid = true;
			}
			else
			{
				c_local_out_valid = false;
			}

			if (c_local_out_valid)
			{
				ch_data_o_out << c_local_out;
			}

#if (DEBUG_AP_INT_PACK == 1)
			vec_input_t vec_in, vec_wt;
			ap_in_data_to_vec_in_data(a_local.pe_data, vec_in.data);
			ap_in_data_to_vec_in_data(b_local.pe_data, vec_wt.data);
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg, new_row_col_pair_local);
#else
			PE_dot_product_and_acc(a_local.pe_data, b_local.pe_data, PE_accum_shift_reg, new_row_col_pair_local);
#endif
		}
	}
}

static void	PE_1_kernel( 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
	//	hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
	//	hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	data_bitwidth PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];

#pragma HLS array_partition variable=PE_accum_shift_reg complete 

#ifndef __SYNTHESIS__

// The following code for shift register initialization is ignored for synthesis
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		PE_accum_shift_reg[i] = 0;
	}

#endif

//	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int i = 0; i < (TOTAL_NUM_BLOCKS * BS_I * BS_R * BS_O); i++)
	{
#pragma HLS PIPELINE
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false

//		for (int i1 = 0; i1 < BS_I; i1++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct in_data_feeder_to_pe a_local;
			char new_row_col_pair_local;

			struct wt_data_feeder_to_pe b_local;

			struct o_data_in_pe c_local_out;	
			data_bitwidth c_local_out_data;
			bool c_local_out_valid;

			a_local = ch_data_in_in.read();

			new_row_col_pair_local = a_local.new_row_col_pair;

			b_local = ch_data_wt_in.read();

		//	if (pe_col < O - 1)
			{
			//	ch_data_in_out << a_local;
			}
		//	if (pe_row < R - 1)
			{
				ch_data_wt_out << b_local;
			}

			// 2. fill O_out_shift_reg
			if (new_row_col_pair_local)
			{
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = true;
			
				c_local_out.data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
				c_local_out_valid = true;
			}
			else
			{
				c_local_out_valid = false;
			}
			if (c_local_out_valid)
			{
				ch_data_o_out << c_local_out;
			}

#if (DEBUG_AP_INT_PACK == 1)
			vec_input_t vec_in, vec_wt;
			ap_in_data_to_vec_in_data(a_local.pe_data, vec_in.data);
			ap_in_data_to_vec_in_data(b_local.pe_data, vec_wt.data);
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg, new_row_col_pair_local);
#else
			PE_dot_product_and_acc(a_local.pe_data, b_local.pe_data, PE_accum_shift_reg, new_row_col_pair_local);
#endif
		}
	}
}

static void	PE_2_kernel( 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	data_bitwidth PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];

//	struct O_out_shift_reg_struct O_out_shift_reg[O_OUT_SHIFT_REG_SIZE];

#pragma HLS array_partition variable=PE_accum_shift_reg complete 
//#pragma HLS array_partition variable=O_out_shift_reg complete 

#ifndef __SYNTHESIS__

// The following code for shift register initialization is ignored for synthesis
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		PE_accum_shift_reg[i] = 0;
	}

//	for (int i = 0; i < O_OUT_SHIFT_REG_SIZE; i++)
//	{
//#pragma HLS UNROLL
//		O_out_shift_reg[i].data = 0;
//		O_out_shift_reg[i].valid = 0;
//	}

#endif

//	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int i = 0; i < (TOTAL_NUM_BLOCKS * BS_I * BS_R * BS_O); i++)
	{
#pragma HLS PIPELINE
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false

//		for (int i1 = 0; i1 < BS_I; i1++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct in_data_feeder_to_pe a_local;
			char new_row_col_pair_local;

			struct wt_data_feeder_to_pe b_local;

		//	struct o_data_in_pe c_local_in;	
		//	data_bitwidth c_local_in_data;
		//	bool c_local_in_valid;

			struct o_data_in_pe c_local_out;	
			data_bitwidth c_local_out_data;
			bool c_local_out_valid;

			a_local = ch_data_in_in.read();

			new_row_col_pair_local = a_local.new_row_col_pair;

			b_local = ch_data_wt_in.read();

		//	if (pe_col < O - 1)
			{
				ch_data_in_out << a_local;
			}
		//	if (pe_row < R - 1)
			{
				ch_data_wt_out << b_local;
			}

			// 2. fill O_out_shift_reg
			if (new_row_col_pair_local)
			{
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = true;
			
				c_local_out.data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
				c_local_out_valid = true;
			}
			else
			{
				// get data from PE above
			//	if (pe_row > 0)
				{
					// non-blocking read
					bool success;

				//	success = ch_data_o_in.read_nb(c_local_in);
				//	c_local_in_data = c_local_in.data;
				//	c_local_in_valid = success;
				
					success = ch_data_o_in.read_nb(c_local_out);
					c_local_out_valid = success;
				}
			//	else
			//	{
			//		c_local_out_valid = false;
			//	}

			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = c_local_in_data;
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = c_local_in_valid;
			}
			if (c_local_out_valid)
			{
				ch_data_o_out << c_local_out;
			}

			// computation
#if (DEBUG_AP_INT_PACK == 1)
			vec_input_t vec_in, vec_wt;
			ap_in_data_to_vec_in_data(a_local.pe_data, vec_in.data);
			ap_in_data_to_vec_in_data(b_local.pe_data, vec_wt.data);
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg, new_row_col_pair_local);
#else
			PE_dot_product_and_acc(a_local.pe_data, b_local.pe_data, PE_accum_shift_reg, new_row_col_pair_local);
#endif
		}
	}
}

static void	PE_3_kernel( 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
	//	hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	data_bitwidth PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];

#pragma HLS array_partition variable=PE_accum_shift_reg complete 

#ifndef __SYNTHESIS__

// The following code for shift register initialization is ignored for synthesis
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		PE_accum_shift_reg[i] = 0;
	}

#endif

//	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int i = 0; i < (TOTAL_NUM_BLOCKS * BS_I * BS_R * BS_O); i++)
	{
#pragma HLS PIPELINE
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false

//		for (int i1 = 0; i1 < BS_I; i1++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct in_data_feeder_to_pe a_local;
			char new_row_col_pair_local;

			struct wt_data_feeder_to_pe b_local;

			struct o_data_in_pe c_local_out;	
			data_bitwidth c_local_out_data;
			bool c_local_out_valid;

			a_local = ch_data_in_in.read();

			new_row_col_pair_local = a_local.new_row_col_pair;

			b_local = ch_data_wt_in.read();

		//	if (pe_col < O - 1)
			{
			//	ch_data_in_out << a_local;
			}
		//	if (pe_row < R - 1)
			{
				ch_data_wt_out << b_local;
			}

			// 2. fill O_out_shift_reg
			if (new_row_col_pair_local)
			{
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = true;
			
				c_local_out.data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
				c_local_out_valid = true;
			}
			else
			{
				// get data from PE above
			//	if (pe_row > 0)
				{
					// non-blocking read
					bool success;

				//	success = ch_data_o_in.read_nb(c_local_in);
				//	c_local_in_data = c_local_in.data;
				//	c_local_in_valid = success;
				
					success = ch_data_o_in.read_nb(c_local_out);
					c_local_out_valid = success;
				}
			//	else
			//	{
			//		c_local_out_valid = false;
			//	}

			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = c_local_in_data;
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = c_local_in_valid;
			}
			if (c_local_out_valid)
			{
				ch_data_o_out << c_local_out;
			}

#if (DEBUG_AP_INT_PACK == 1)
			vec_input_t vec_in, vec_wt;
			ap_in_data_to_vec_in_data(a_local.pe_data, vec_in.data);
			ap_in_data_to_vec_in_data(b_local.pe_data, vec_wt.data);
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg, new_row_col_pair_local);
#else
			PE_dot_product_and_acc(a_local.pe_data, b_local.pe_data, PE_accum_shift_reg, new_row_col_pair_local);
#endif
		}
	}
}

static void	PE_4_kernel( 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
	//	hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	data_bitwidth PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];

#pragma HLS array_partition variable=PE_accum_shift_reg complete 

#ifndef __SYNTHESIS__

// The following code for shift register initialization is ignored for synthesis
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		PE_accum_shift_reg[i] = 0;
	}

#endif

//	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int i = 0; i < (TOTAL_NUM_BLOCKS * BS_I * BS_R * BS_O); i++)
	{
#pragma HLS PIPELINE
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false

//		for (int i1 = 0; i1 < BS_I; i1++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct in_data_feeder_to_pe a_local;
			char new_row_col_pair_local;

			struct wt_data_feeder_to_pe b_local;

			struct o_data_in_pe c_local_out;	
			data_bitwidth c_local_out_data;
			bool c_local_out_valid;

			a_local = ch_data_in_in.read();

			new_row_col_pair_local = a_local.new_row_col_pair;

			b_local = ch_data_wt_in.read();

		//	if (pe_col < O - 1)
			{
				ch_data_in_out << a_local;
			}
		//	if (pe_row < R - 1)
			{
			//	ch_data_wt_out << b_local;
			}

			// 2. fill O_out_shift_reg
			if (new_row_col_pair_local)
			{
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = true;
			
				c_local_out.data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
				c_local_out_valid = true;
			}
			else
			{
				// get data from PE above
			//	if (pe_row > 0)
				{
					// non-blocking read
					bool success;

				//	success = ch_data_o_in.read_nb(c_local_in);
				//	c_local_in_data = c_local_in.data;
				//	c_local_in_valid = success;
				
					success = ch_data_o_in.read_nb(c_local_out);
					c_local_out_valid = success;
				}
			//	else
			//	{
			//		c_local_out_valid = false;
			//	}

			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = c_local_in_data;
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = c_local_in_valid;
			}
			if (c_local_out_valid)
			{
				ch_data_o_out << c_local_out;
			}

#if (DEBUG_AP_INT_PACK == 1)
			vec_input_t vec_in, vec_wt;
			ap_in_data_to_vec_in_data(a_local.pe_data, vec_in.data);
			ap_in_data_to_vec_in_data(b_local.pe_data, vec_wt.data);
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg, new_row_col_pair_local);
#else
			PE_dot_product_and_acc(a_local.pe_data, b_local.pe_data, PE_accum_shift_reg, new_row_col_pair_local);
#endif
		}
	}
}

static void	PE_5_kernel( 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
	//	hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
	//	hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	data_bitwidth PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];

#pragma HLS array_partition variable=PE_accum_shift_reg complete 

#ifndef __SYNTHESIS__

// The following code for shift register initialization is ignored for synthesis
	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		PE_accum_shift_reg[i] = 0;
	}

#endif

//	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int i = 0; i < (TOTAL_NUM_BLOCKS * BS_I * BS_R * BS_O); i++)
	{
#pragma HLS PIPELINE
//#pragma HLS dependence variable=PE_accum_shift_reg array inter false

//		for (int i1 = 0; i1 < BS_I; i1++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct in_data_feeder_to_pe a_local;
			char new_row_col_pair_local;

			struct wt_data_feeder_to_pe b_local;

			struct o_data_in_pe c_local_out;	
			data_bitwidth c_local_out_data;
			bool c_local_out_valid;

			a_local = ch_data_in_in.read();

			new_row_col_pair_local = a_local.new_row_col_pair;

			b_local = ch_data_wt_in.read();

		//	if (pe_col < O - 1)
			{
			//	ch_data_in_out << a_local;
			}
		//	if (pe_row < R - 1)
			{
			//	ch_data_wt_out << b_local;
			}

			// 2. fill O_out_shift_reg
			if (new_row_col_pair_local)
			{
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = true;
			
				c_local_out.data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
				c_local_out_valid = true;
			}
			else
			{
				// get data from PE above
			//	if (pe_row > 0)
				{
					// non-blocking read
					bool success;

				//	success = ch_data_o_in.read_nb(c_local_in);
				//	c_local_in_data = c_local_in.data;
				//	c_local_in_valid = success;
				
					success = ch_data_o_in.read_nb(c_local_out);
					c_local_out_valid = success;
				}
			//	else
			//	{
			//		c_local_out_valid = false;
			//	}

			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = c_local_in_data;
			//	O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = c_local_in_valid;
			}
			if (c_local_out_valid)
			{
				ch_data_o_out << c_local_out;
			}

#if (DEBUG_AP_INT_PACK == 1)
			vec_input_t vec_in, vec_wt;
			ap_in_data_to_vec_in_data(a_local.pe_data, vec_in.data);
			ap_in_data_to_vec_in_data(b_local.pe_data, vec_wt.data);
			PE_dot_product_and_acc(vec_in, vec_wt, PE_accum_shift_reg, new_row_col_pair_local);
#else
			PE_dot_product_and_acc(a_local.pe_data, b_local.pe_data, PE_accum_shift_reg, new_row_col_pair_local);
#endif
		}
	}
}

static void PE_0_wrapper(
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
	//	hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	PE_0_kernel(ch_data_in_in, ch_data_wt_in, ch_data_in_out, ch_data_wt_out, ch_data_o_out);
}


static void	PE_1_wrapper( 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
	//	hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
	//	hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	PE_1_kernel(ch_data_in_in, ch_data_wt_in, ch_data_wt_out, ch_data_o_out);
}


static void PE_2_wrapper(
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	PE_2_kernel(ch_data_in_in, ch_data_wt_in, ch_data_in_out, ch_data_wt_out, ch_data_o_in, ch_data_o_out);
}

static void PE_3_wrapper(
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
	//	hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	PE_3_kernel(ch_data_in_in, ch_data_wt_in, ch_data_wt_out, ch_data_o_in, ch_data_o_out);
}


static void PE_4_wrapper(
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
		hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
	//	hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	PE_4_kernel(ch_data_in_in, ch_data_wt_in, ch_data_in_out, ch_data_o_in, ch_data_o_out);
}

static void PE_5_wrapper(
		hls::stream<in_data_feeder_to_pe> &ch_data_in_in, 
		hls::stream<wt_data_feeder_to_pe> &ch_data_wt_in, 
	//	hls::stream<in_data_feeder_to_pe> &ch_data_in_out,
	//	hls::stream<wt_data_feeder_to_pe> &ch_data_wt_out, 
		hls::stream<o_data_in_pe> &ch_data_o_in, 
		hls::stream<o_data_in_pe> &ch_data_o_out)
{
	PE_5_kernel(ch_data_in_in, ch_data_wt_in, ch_data_o_in, ch_data_o_out);
}


