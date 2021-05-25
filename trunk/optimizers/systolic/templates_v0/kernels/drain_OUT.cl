

//;#   my $debug_drain_o = "on";
//;  my $debug_drain_o = "off";

#ifndef EMULATOR
__attribute__((packed))
#endif
struct custom_float_array { 
	float drain_data[SYS_ARRAY_NUM_COLS];
};

channel struct custom_float_array ch_drain_chain[SYS_ARRAY_NUM_COLS-1];

#define DRAIN_O_LAST_CHANNEL_DEPTH 1

channel struct custom_float_array ch_drain_O_tree_to_mem __attribute__((depth(DRAIN_O_LAST_CHANNEL_DEPTH)));

__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__attribute__((num_compute_units(SYS_ARRAY_NUM_COLS)))
__kernel void drain_OUT_chain_node_kernel() {
	unsigned drain_node_id = get_compute_id(0);

	//; if ($debug_drain_o eq "on") {
	int wr_cnt = 0;
	//; }
	
	//; if ($debug_drain_o eq "on") {
	printf("drain_O_chain[%d] started!\n", drain_node_id);
	//; }

	struct ch_data_o_struct PE_data_in;
	struct custom_float_array prev_node_data_in;
	struct custom_float_array data_out;

	while(1) {
	//	PE_data_in = read_channel_altera(ch_data_o_boundary[drain_node_id]);
		PE_data_in = read_channel_intel(ch_data_o_boundary[drain_node_id]);

		if (drain_node_id != SYS_ARRAY_NUM_COLS-1) {
		//	prev_node_data_in = read_channel_altera(ch_drain_chain[drain_node_id]);
			prev_node_data_in = read_channel_intel(ch_drain_chain[drain_node_id]);
		}

		#pragma unroll
		for (int i = SYS_ARRAY_NUM_COLS-1 ; i > drain_node_id; --i) {
			data_out.drain_data[i] = prev_node_data_in.drain_data[i];
		}

		data_out.drain_data[drain_node_id] = PE_data_in.data;
		
		if (drain_node_id == 0) {
		//	write_channel_altera(ch_drain_O_tree_to_mem, data_out);
			write_channel_intel(ch_drain_O_tree_to_mem, data_out);
		} else {
		//	write_channel_altera(ch_drain_chain[drain_node_id-1], data_out);
			write_channel_intel(ch_drain_chain[drain_node_id-1], data_out);
		}

	}
	//; if ($debug_drain_o eq "on") {
	printf("drain_O_chain[%d] finished!!!\n", drain_node_id);
	//; }
}

__attribute__((max_global_work_dim(0)))
//__kernel void drain_OUT_write_tree_root_to_mem_kernel(__global struct custom_float_array* 
//        restrict out, int o_num_coalesced_words) {
__kernel void drain_OUT_write_tree_root_to_mem_kernel(__global struct custom_float_array* 
        restrict out) {
//__kernel void drain_OUT_write_tree_root_to_mem_kernel(__global float* 
//        restrict out) {

	//; if ($debug_drain_o eq "on") {
	printf("drain_O started!\n");
	//; }
	//; if ($debug_drain_o eq "on") {
	int rd_cnt = 0; 
	int word_cnt = 0;
	//; }
	
	int o_num_coalesced_words = (BN_R * BN_C * BN_O * R * BS_R * BS_C * BS_O * O) / PE_COLS;

    bool more_words_to_write = true;

    int i = 0;
    int i_next = 1;

    while (more_words_to_write) {

      struct custom_float_array root_data;

     // root_data = read_channel_altera(ch_drain_O_tree_to_mem);
      root_data = read_channel_intel(ch_drain_O_tree_to_mem);

	//; if ($debug_drain_o eq "on") {
	  for (int t= 0; t < SYS_ARRAY_NUM_COLS; t++)
	  {
		//  printf("rd_cnt=%d %lf \n", rd_cnt, root_data.drain_data[t]);
		  rd_cnt++;
	  }
	//; }


	//; if ($debug_drain_o eq "on") {
	  {
		  int total = o_num_coalesced_words;
		  printf("Drain_O rd=%d word=%d (total_word=%d)\n",
				  word_cnt * SYS_ARRAY_NUM_COLS +
				  SYS_ARRAY_NUM_COLS-1, word_cnt, total);
		  word_cnt ++;
	  }
	//; }

	  // address mapping (out0->out1, in DDR for partial validation)
//	  {
//		  // out0[r0][c0][o0][c2][r1][c1][o1][o2]
//		  int o1 = i % BS_O;
//		  int c1 = (i / BS_O) % BS_C;
//		  int r1 = (i / (BS_C * BS_O)) % BS_R;
//		  int c2 = (i / (BS_R * BS_C * BS_O)) % C;
//		  int o0 = (i / (C * BS_R * BS_C * BS_O)) % cfg_BN_O[0];
//		  int c0 = (i / (cfg_BN_O[0] * C * BS_R * BS_C * BS_O)) % cfg_BN_C[0];
//		  int r0 = (i / (cfg_BN_C[0] * cfg_BN_O[0] * C * BS_R * BS_C * BS_O)) % cfg_BN_R[0];
//
//		  for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++) {
//			  int out0_idx = (r0 * cfg_BN_C[0] * cfg_BN_O[0] + c0 * cfg_BN_O[0] + o0) * C * BS_R * BS_C * BS_O * O + (c2 * BS_R * BS_C + r1 * BS_C + c1) * BS_O * O + o1 * O + o2;
//			  int out1_idx = (r0 * cfg_BN_C[0] * cfg_BN_O[0] + c0 * cfg_BN_O[0] + o0) * BS_R * C * BS_C * BS_O * O + (r1 * C * BS_C + c2 * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
//
////			  out[out1_idx] = root_data.drain_data[o2];
//			 // out[out0_idx] = root_data.drain_data[o2];
//			 // out0[out0_idx] = root_data.drain_data[o2];
//		  }
//		  i++;
//	  }

      out[i] = root_data;
      i++;

      if (o_num_coalesced_words == i_next) {
        more_words_to_write = false;
      } else {
        i_next++;
      }

    }

	//; if ($debug_drain_o eq "on") {
	printf("drain_O finished!!!\n");
	//; }
	
//	if (layer < NUM_CONV - 1) {
//	mem_fence(CLK_CHANNEL_MEM_FENCE);
//		write_channel_altera(ch_msg_loader_in_ready, layer);
//	mem_fence(CLK_CHANNEL_MEM_FENCE);
//		write_channel_altera(ch_msg_loader_w_ready, layer);
//	mem_fence(CLK_CHANNEL_MEM_FENCE);
//	}

}
