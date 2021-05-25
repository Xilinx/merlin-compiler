

#include "systolic.h"

#define FIFO_DEPTH 32

#define DEBUG_EXCEPTION 0

/*
 * ofmap[o]+=weight[o][i]*ifmap[i]
 * */


struct in_data_to_feeder
{
	data_bitwidth data;
	char new_row_col_pair;
};

struct wt_data_to_feeder
{
	data_bitwidth data;
};

struct o_data_in_pe
{
	data_bitwidth data;
};

struct O_out_shift_reg_struct
{
	data_bitwidth data;
	bool valid;
};

struct custom_float_array
{
	data_bitwidth drain_data[SYS_ARRAY_NUM_COLS];
};

void	load_in_and_forward(__global data_bitwidth *ifmap, struct in_data_to_feeder *ch_data_row_loader_to_first_feeder)
{
	// FIXME: multiple blocks
	int fifo_idx = 0;
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	{
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int i1 = 0; i1 < BS_I; i1++)
		{
			int ifmap_idx = r1 * BS_I + i1;
	
			struct in_data_to_feeder in_data;
	
			if (blk < NUM_BLOCKS)
			{
				in_data.new_row_col_pair = false;
				in_data.data = ifmap[ifmap_idx];
			}
			else
			{
				// one more block to push effective data
				in_data.new_row_col_pair = true;
				in_data.data = 0;
			}
	
			ch_data_row_loader_to_first_feeder[fifo_idx++] = in_data;
		}
	}
}

void	load_wt_and_forward(__global data_bitwidth *weight, struct wt_data_to_feeder *ch_data_loader_wt_to_first_feeder)
{
	int fifo_idx = 0;
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	{
		for (int o2 = 0; o2 < O; o2++)
		for (int o1 = 0; o1 < BS_O; o1++)
		for (int i1 = 0; i1 < BS_I; i1++)
		{
			int weight_idx = (o2 * BS_O + o1) * BS_I + i1;

			struct wt_data_to_feeder wt_data;
			if (blk < NUM_BLOCKS)
			{
				wt_data.data = weight[weight_idx];
			}
			else
			{
				wt_data.data = 0;
			}

			ch_data_loader_wt_to_first_feeder[fifo_idx++] = wt_data;
		}

	}
}

void feed_in(struct in_data_to_feeder *ch_data_row_feeder, struct in_data_to_feeder *ch_data_feeder_to_pe)
{
	data_bitwidth in_buf[BS_R][BS_I];

	bool new_row_col_pair_feed = false;
//	bool new_row_col_pair_buffer = false;

	// TODO: double buffer
	int fifo_feeder_idx = 0;
	int fifo_pe_idx = 0;
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	{
		// write
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int i1 = 0; i1 < BS_I; i1++)
		{
			struct in_data_to_feeder in_data;
			in_data = ch_data_row_feeder[fifo_feeder_idx];
			fifo_feeder_idx++;

			in_buf[r1][i1] = in_data.data;
			new_row_col_pair_feed = in_data.new_row_col_pair;
		}

		int read_idx = 0;
		// read
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int o1 = 0; o1 < BS_O; o1++) // interleaving
		{
			struct in_data_to_feeder in_data;

			in_data.data = in_buf[r1][i1];
			in_data.new_row_col_pair = new_row_col_pair_feed;

			ch_data_feeder_to_pe[fifo_pe_idx] = in_data;
			fifo_pe_idx++;

			if (++read_idx == (BS_R * BS_O))
				new_row_col_pair_feed = false;
		}
	}
}

void feed_wt(struct wt_data_to_feeder *ch_data_col_feeder, struct wt_data_to_feeder *ch_data_wt, struct wt_data_to_feeder *ch_data_col_feeder_next, int feeder_id)	
{
	data_bitwidth w_buf[BS_O][BS_I];

	int fifo_feeder_in_idx = 0;
	int fifo_feeder_out_idx = 0;
	int fifo_pe_idx = 0;
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	{
		// write
		for (int o2 = 0; o2 < (O - feeder_id); o2++)
		for (int o1 = 0; o1 < BS_O; o1++)
		for (int i1 = 0; i1 < BS_I; i1++)
		{
			struct wt_data_to_feeder wt_data;

		//	printf("%d: fifo_feeder_in_idx=%d\n", feeder_id, fifo_feeder_in_idx);
			wt_data = ch_data_col_feeder[fifo_feeder_in_idx++];

			if (0 == o2)
			{
				// write buffer
				w_buf[o1][i1] = wt_data.data;
			}
			else
			{
				// forward
				if (feeder_id < SYS_ARRAY_NUM_COLS - 1)
				{
				//	printf("%d: fifo_feeder_out_idx=%d\n", feeder_id, fifo_feeder_out_idx);
					ch_data_col_feeder_next[fifo_feeder_out_idx++] = wt_data;
				}
			}
		}
		
		// read
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++) // interleaving
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct wt_data_to_feeder wt_data;

			wt_data.data = w_buf[o1][i1];
			ch_data_wt[fifo_pe_idx++] =  wt_data;
		}
	}
}

void PE_dot_product_and_acc(data_bitwidth a, data_bitwidth b, data_bitwidth *accum_shift_reg)
{
	data_bitwidth sum = 0;
	data_bitwidth accum = 0;

	sum += a * b;

	// TODO: accumulation on blocks
	accum = sum + accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];

	for (int i = ACCUM_SHIFT_REG_SIZE - 1; i >= 1; i--)
	{
#pragma HLS UNROLL
		accum_shift_reg[i] = accum_shift_reg[i - 1];
	}

	accum_shift_reg[0] = accum;
}

void	PE(int pe_col, struct in_data_to_feeder *ch_data_in, struct wt_data_to_feeder *ch_data_wt, struct in_data_to_feeder *ch_data_in_right, struct o_data_in_pe *ch_data_o)
{
	data_bitwidth PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];
	struct O_out_shift_reg_struct O_out_shift_reg[O_OUT_SHIFT_REG_SIZE];

	for (int i = 0; i < ACCUM_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		PE_accum_shift_reg[i] = 0;
	}

	for (int i = 0; i < O_OUT_SHIFT_REG_SIZE; i++)
	{
#pragma HLS UNROLL
		O_out_shift_reg[i].data = 0;
		O_out_shift_reg[i].valid = 0;
	}

	int fifo_in_p_idx = 0;
	int fifo_wt_p_idx = 0;
	int fifo_in_c_idx = 0;
	int fifo_o_idx = 0;
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS; blk++)
	{
#pragma HLS PIPELINE

		for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct in_data_to_feeder a_local;
			char new_row_col_pair_local;

			struct wt_data_to_feeder b_local;
			struct o_data_in_pe c_local;	

			a_local = ch_data_in[fifo_in_p_idx];
			fifo_in_p_idx++;

			new_row_col_pair_local = a_local.new_row_col_pair;

			b_local = ch_data_wt[fifo_wt_p_idx];
			fifo_wt_p_idx++;

			if (pe_col < O - 1)
			{
				ch_data_in_right[fifo_in_c_idx] =  a_local;
				fifo_in_c_idx++;
			}

			// data propagation
			// 1. drain O_out_shift_reg
		//	data_bitwidth c_local_data = O_out_shift_reg[o2][0].data;
		//	bool c_local_valid = O_out_shift_reg[o2][0].valid;
			data_bitwidth c_local_data = O_out_shift_reg[0].data;
			bool c_local_valid = O_out_shift_reg[0].valid;

			if (c_local_valid)
			{
				c_local.data = c_local_data;

				ch_data_o[fifo_o_idx] = c_local;
				fifo_o_idx++;
			}

			for (int local_shift_slot = 0; local_shift_slot < O_OUT_SHIFT_REG_SIZE - 1; local_shift_slot++)
			{
#pragma HLS UNROLL
			//	O_out_shift_reg[o2][local_shift_slot] = O_out_shift_reg[o2][local_shift_slot + 1];
				O_out_shift_reg[local_shift_slot] = O_out_shift_reg[local_shift_slot + 1];
			}
				// 2. fill O_out_shift_reg
			if (new_row_col_pair_local)
			{
			//	O_out_shift_reg[o2][O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[o2][ACCUM_SHIFT_REG_SIZE - 1];
			//	O_out_shift_reg[o2][O_OUT_SHIFT_REG_SIZE - 1].valid = true;
				O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE - 1];
				O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = true;
			}
			else
			{
				// TODO: get data from PE above
			//	O_out_shift_reg[o2][O_OUT_SHIFT_REG_SIZE - 1].data = 0;
			//	O_out_shift_reg[o2][O_OUT_SHIFT_REG_SIZE - 1].valid = false;
				O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].data = 0;
				O_out_shift_reg[O_OUT_SHIFT_REG_SIZE - 1].valid = false;
			}

			// computation
		//	PE_dot_product_and_acc(a_local.data, b_local.data, PE_accum_shift_reg[o2]);
			PE_dot_product_and_acc(a_local.data, b_local.data, PE_accum_shift_reg);
		}
	}
}

void	drain_chain_node(struct o_data_in_pe *ch_data_o, struct custom_float_array *ch_drain_chain_in, struct custom_float_array *ch_drain_chain_out, int drain_node_id)
{
	struct o_data_in_pe PE_data_in;
	struct custom_float_array prev_node_data_in;
	struct custom_float_array data_out;

	int fifo_o_in_idx = 0;
	int fifo_drain_chain_in_idx = 0;
	int fifo_drain_chain_out_idx = 0;
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		PE_data_in = ch_data_o[fifo_o_in_idx];
		fifo_o_in_idx++;

		if (drain_node_id < SYS_ARRAY_NUM_COLS - 1)
		{
			prev_node_data_in = ch_drain_chain_in[fifo_drain_chain_in_idx];
			fifo_drain_chain_in_idx++;
		}

		for (int i = SYS_ARRAY_NUM_COLS - 1; i > drain_node_id; i--)
		{
			data_out.drain_data[i] = prev_node_data_in.drain_data[i];
		}

		data_out.drain_data[drain_node_id] = PE_data_in.data;

		ch_drain_chain_out[fifo_drain_chain_out_idx] = data_out;
		fifo_drain_chain_out_idx++;
	}
}

void drain_write_tree_root_to_mem(struct custom_float_array *ch_drain_chain_0, __global data_bitwidth *ofmap)
{
	data_bitwidth o_buf[BS_R * BS_O * O];

	int fifo_drain_chain_in_idx = 0;
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		struct custom_float_array root_data;
		
		root_data = ch_drain_chain_0[fifo_drain_chain_in_idx];
		fifo_drain_chain_in_idx++;

		for (int o2 = 0; o2 < O; o2++)
		{
			int o_buf_idx = (r1 * O + o2) * BS_O + o1;
			int o_idx = o_buf_idx;

			o_buf[o_buf_idx] = root_data.drain_data[o2];
			ofmap[o_idx] = o_buf[o_buf_idx];

		}
	}

	// Write to DDR
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int o2 = 0; o2 < O; o2++)
//	for (int o1 = 0; o1 < BS_O; o1++)
//	{
//		int o_idx = (r1 * O + o2) * BS_O + o1;
//
//		data_bitwidth data = ch_drain_to_write_to_ddr.read();
//		ofmap[o_idx] = data;
//	}
}


__kernel
__attribute__ ((reqd_work_group_size(1, 1, 1)))
__attribute__ ((xcl_dataflow))
void systolic_top(__global data_bitwidth *ifmap, __global data_bitwidth *weight, __global data_bitwidth *ofmap)
{

	struct in_data_to_feeder ch_data_row_feeder_chain[FIFO_DEPTH];
//	struct in_data_to_feeder ch_data_in[O + 1][FIFO_DEPTH];
	struct in_data_to_feeder ch_data_in_0[FIFO_DEPTH];
	struct in_data_to_feeder ch_data_in_1[FIFO_DEPTH];
	struct in_data_to_feeder ch_data_in_2[FIFO_DEPTH];

//	struct wt_data_to_feeder ch_data_col_feeder_chain[O + 1][FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_col_feeder_chain_0[FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_col_feeder_chain_1[FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_col_feeder_chain_2[FIFO_DEPTH];

//	struct wt_data_to_feeder ch_data_wt[O][FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_wt_0[FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_wt_1[FIFO_DEPTH];

//	struct o_data_in_pe ch_data_o[O][FIFO_DEPTH];
	struct o_data_in_pe ch_data_o_0[FIFO_DEPTH];
	struct o_data_in_pe ch_data_o_1[FIFO_DEPTH];

//	struct custom_float_array ch_drain_chain[O + 1][FIFO_DEPTH];
	struct custom_float_array ch_drain_chain_0[FIFO_DEPTH];
	struct custom_float_array ch_drain_chain_1[FIFO_DEPTH];
	struct custom_float_array ch_drain_chain_2[FIFO_DEPTH];

//	data_bitwidth ch_drain_to_write_to_ddr[FIFO_DEPTH];

	// Loader IN
	load_in_and_forward(ifmap, ch_data_row_feeder_chain);

	// Loader W
//	load_wt_and_forward(weight, ch_data_col_feeder_chain[0]);
	load_wt_and_forward(weight, ch_data_col_feeder_chain_0);

	// Feeder IN
//	feed_in(ch_data_row_feeder_chain, ch_data_in[0]);
	feed_in(ch_data_row_feeder_chain, ch_data_in_0);

	// Feeder W
//	for (int o2 = 0; o2 < O; o2++)
//	{
//#pragma HLS UNROLL
//
//		feed_wt(ch_data_col_feeder_chain[o2], ch_data_wt[o2], ch_data_col_feeder_chain[o2 + 1], o2);
//
//	}
	feed_wt(ch_data_col_feeder_chain_0, ch_data_wt_0, ch_data_col_feeder_chain_1, 0);
	feed_wt(ch_data_col_feeder_chain_1, ch_data_wt_1, ch_data_col_feeder_chain_2, 1);

	// PE: computation and data propogation
//	for (int o2 = 0; o2 < O; o2++)
//	{
//#pragma HLS UNROLL
//
//		PE(o2, ch_data_in[o2], ch_data_wt[o2], ch_data_in[o2 + 1], ch_data_o[o2]);
//	}
	PE(0, ch_data_in_0, ch_data_wt_0, ch_data_in_1, ch_data_o_0);
	PE(1, ch_data_in_1, ch_data_wt_1, ch_data_in_2, ch_data_o_1);

	// Drain: write to output buffer
//	for (int o2 = O - 1; o2 >= 0; o2--)
//	for (int o2 = 0; o2 < O; o2++)
//	{
//#pragma HLS UNROLL
//
//		drain_chain_node(ch_data_o[o2], ch_drain_chain[o2 + 1], ch_drain_chain[o2], o2);
//	}
	drain_chain_node(ch_data_o_1, ch_drain_chain_2, ch_drain_chain_1, 1);
	drain_chain_node(ch_data_o_0, ch_drain_chain_1, ch_drain_chain_0, 0);

//	drain_write_tree_root_to_mem(ch_drain_chain[0], ofmap);
	drain_write_tree_root_to_mem(ch_drain_chain_0, ofmap);
}
