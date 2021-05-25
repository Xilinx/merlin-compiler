

//;#   my $debug_drain_o = "on";
//;  my $debug_drain_o = "off";


__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__attribute__((num_compute_units(SYS_ARRAY_NUM_COLS)))
__kernel void drain_OUT_chain_node_kernel() 
{
//#if 0
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

	while(1)
	{
		PE_data_in = read_channel_altera(ch_data_o_boundary[drain_node_id]);

		if (drain_node_id != SYS_ARRAY_NUM_COLS-1)
		{
			prev_node_data_in = read_channel_altera(ch_drain_chain[drain_node_id]);
		}

#pragma unroll
		for (int i = SYS_ARRAY_NUM_COLS-1 ; i > drain_node_id; --i)
		{
			data_out.drain_data[i] = prev_node_data_in.drain_data[i];
		}

		data_out.drain_data[drain_node_id] = PE_data_in.data;

		if (drain_node_id == 0)
		{
			write_channel_altera(ch_drain_O_tree_to_mem, data_out);
		}
		else
		{
			write_channel_altera(ch_drain_chain[drain_node_id-1], data_out);
		}

	}
	//; if ($debug_drain_o eq "on") {
	printf("drain_O_chain[%d] finished!!!\n", drain_node_id);
	//; }
//#endif
}


//__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__kernel void drain_OUT_write_tree_root_to_post_kernel(int num_conv) 
{
	for (int layer = 0; layer < num_conv; layer++)
	{
		//; if ($debug_drain_o eq "on") {
		printf("layer %d: drain_O started!\n", layer);
		//; }
		
		//; if ($debug_drain_o eq "on") {
		int rd_cnt = 0; 
		int word_cnt = 0;
		//; }
		
		int o_num_coalesced_words = (cfg_BN_R[layer] * cfg_BN_C[layer] * cfg_BN_O[layer] * R * BS_R * BS_C * BS_O * O) / PE_COLS;

		bool more_words_to_write = true;

		int i = 0;
		int i_next = 1;

		while (more_words_to_write)
		{
			struct custom_float_array root_data;

			root_data = read_channel_altera(ch_drain_O_tree_to_mem);

			//; if ($debug_drain_o eq "on") {
			for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
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

		//	for (int o2 = 0; o2 < SYS_ARRAY_NUM_COLS; o2++)
		//	{
		//		write_channel_altera(ch_drain_to_postproc, root_data.drain_data[o2]);
		//	}
			write_channel_altera(ch_drain_to_postproc, root_data);
			i++;

			if (o_num_coalesced_words == i_next)
			{
				more_words_to_write = false;
			}
			else
			{
				i_next++;
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

		//; if ($debug_drain_o eq "on") {
		printf("drain_O finished!!!\n");
		//; }
	}
}
