
//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");

//; my $o_num_coalesced_words = $cfg_root->{Drain_O}->{num_coalesced_words};

//; my $outer_loop_ubs = $cfg_root->{Drain_O}->{outer_loop_ubs};
//; my $drain_out_order_ubs = $cfg_root->{Drain_O}->{drain_out_order_ubs};
//; my $drain_buffer_order_ubs = $cfg_root->{Drain_O}->{drain_buffer_order_ubs};
//; my $out_to_buffer_map = $cfg_root->{Drain_O}->{out_to_buffer_map};

//;#   my $debug_drain_o = "on";
//;  my $debug_drain_o = "off";

#ifndef EMULATOR
struct __attribute__((packed)) custom_float_array { 
#else
struct custom_float_array { 
#endif
	data_precision drain_data[SYS_ARRAY_NUM_COLS];
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

	while(1) 
	{
#if (ALTERA_FPGA_SDK_16_1 == 1)
		PE_data_in = read_channel_altera(ch_data_o_boundary[drain_node_id]);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
		PE_data_in = read_channel_intel(ch_data_o_boundary[drain_node_id]);
#endif

		if (drain_node_id != SYS_ARRAY_NUM_COLS-1)
		{
#if (ALTERA_FPGA_SDK_16_1 == 1)
			prev_node_data_in = read_channel_altera(ch_drain_chain[drain_node_id]);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
			prev_node_data_in = read_channel_intel(ch_drain_chain[drain_node_id]);
#endif
		}

		#pragma unroll
		for (int i = SYS_ARRAY_NUM_COLS-1 ; i > drain_node_id; --i) {
			data_out.drain_data[i] = prev_node_data_in.drain_data[i];
		}

		data_out.drain_data[drain_node_id] = PE_data_in.data;
		
		if (drain_node_id == 0) 
		{
#if (ALTERA_FPGA_SDK_16_1 == 1)
			write_channel_altera(ch_drain_O_tree_to_mem, data_out);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
			write_channel_intel(ch_drain_O_tree_to_mem, data_out);
#endif
		} 
		else 
		{
#if (ALTERA_FPGA_SDK_16_1 == 1)
			write_channel_altera(ch_drain_chain[drain_node_id-1], data_out);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
			write_channel_intel(ch_drain_chain[drain_node_id-1], data_out);
#endif
		}

	}
	//; if ($debug_drain_o eq "on") {
	printf("drain_O_chain[%d] finished!!!\n", drain_node_id);
	//; }
}

#define APPEND_TRANS 0
__attribute__((max_global_work_dim(0)))
#if APPEND_TRANS == 0
__kernel void drain_OUT_write_tree_root_to_mem_kernel(__global struct custom_float_array* 
        restrict out) {
#else
__kernel void drain_OUT_write_tree_root_to_mem_kernel(__global data_precision* 
        restrict out) {
#endif

	//; if ($debug_drain_o eq "on") {
	printf("drain_O started!\n");
	//; }
	//; if ($debug_drain_o eq "on") {
	int rd_cnt = 0; 
	int word_cnt = 0;
	//; }
	
	

#if APPEND_TRANS == 0
//	int o_num_coalesced_words = (cfg_BN_R[0] * cfg_BN_C[0] * cfg_BN_O[0] * R * BS_R * BS_C * BS_O * O) / PE_COLS;
	int o_num_coalesced_words = `$o_num_coalesced_words`;

    bool more_words_to_write = true;

    int i = 0;
    int i_next = 1;

	while (more_words_to_write)
	{
		struct custom_float_array root_data;

#if (ALTERA_FPGA_SDK_16_1 == 1)
		root_data = read_channel_altera(ch_drain_O_tree_to_mem);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
		root_data = read_channel_intel(ch_drain_O_tree_to_mem);
#endif

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


		out[i] = root_data;
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

#elif APPEND_TRANS == 1

#define BN_I 1
#define BN_J 2
#define BS_I 2
#define BS_J 2
#define PE_ROWS 2
#define PE_COLS 2

	data_precision out_buf[BS_I * PE_ROWS * BS_J * PE_COLS];

//	for (int i0 = 0; i0 < BN_I; i0++)
//	for (int j0 = 0; j0 < BN_J; j0++)
//	{
	int i0 = 0;
	int j0 = 0;
	while (1)
	{
		int i1 = 0;
		int j1 = 0;
		int i2 = 0;
		int j2 = 0;
		while (1)
		{
			struct custom_float_array root_data;

#if (ALTERA_FPGA_SDK_16_1 == 1)
			root_data = read_channel_altera(ch_drain_O_tree_to_mem);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
			root_data = read_channel_intel(ch_drain_O_tree_to_mem);
#endif

			for (j2 = 0; j2 < PE_COLS; j2++)
			{
				int out_buf_idx = ((i1 * PE_ROWS + i2) * BS_J + j1) * PE_COLS + j2;
			//	int out_buf_idx = ((i2 * BS_I + i1) * BS_J + j1) * PE_COLS + j2;

				out_buf[out_buf_idx] = root_data.drain_data[j2];

			}

			j1++;
			if (j1 == BS_J)
			{
				j1 = 0;
				i1++;
				if (i1 == BS_I)
				{
					i1 = 0;
					i2++;
					if (i2 == PE_ROWS)
					{
						i2 = 0;
						break;
					}
				}
			}
		}

	//	for (int i1 = 0; i1 < BS_I; i1++)
	//	for (int j1 = 0; j1 < BS_J; i1++)
	//	{
	//		for (int i2 = 0; i2 < PE_ROWS; i2++)
	//		for (int j2 = 0; j2 < PE_COLS; j2++)
		i1 = 0;
		j1 = 0;
		i2 = 0;
		j2 = 0;
		while (1)
		{
			int out_buf_idx = ((i1 * PE_ROWS + i2) * BS_J + j1) * PE_COLS + j2;
		//	int out_buf_idx = ((i2 * BS_I + i1) * BS_J + j1) * PE_COLS + j2;
			int out_idx = (i0 * BN_J + j0) * BS_I * PE_ROWS * BS_J * PE_COLS + out_buf_idx;

			out[out_idx] = out_buf[out_buf_idx];

			j2++;
			if (j2 == PE_COLS)
			{
				j2 = 0;
				j1++;
				if (j1 == BS_J)
				{
					j1 = 0;
					i1++;
					if (i1 == BS_I)
					{
						i1 = 0;
						i2++;
						if (i2 == PE_ROWS)
						{
							i2 = 0;
							break;
						}
					}
				}
			}
		}

		j0++;
		if (j0 == BN_J) 
		{
			j0 = 0;
			i0++;
			if (i0 == BN_I)
			{
				i0 = 0;
				break;
			}
		}
	}

#else

//; my @v_outer_loop_ubs = split(',', $outer_loop_ubs);
//; for (my $i = 0; $i < @v_outer_loop_ubs; $i++){
//;         $it_name = "_i".$i."_0";
	int `$it_name` = 0; 
//; }
	while (1)
	{
		//; my @v_drain_out_order_ubs = split(',', $drain_out_order_ubs);
		//; for (my $i = 0; $i < @v_drain_out_order_ubs; $i++){
		//;         $it_name = "_i".$i."_1";
		int `$it_name` = 0; 
		//; }
		while (1)
		{
			struct custom_float_array root_data;

#if (ALTERA_FPGA_SDK_16_1 == 1)
			root_data = read_channel_altera(ch_drain_O_tree_to_mem);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
			root_data = read_channel_intel(ch_drain_O_tree_to_mem);
#endif

			//; my $last = @v_drain_out_order_ubs - 1;
			//; my $pe_cols_var = "_i".$last."_1";
			for (`$pe_cols_var` = 0; `$pe_cols_var` < PE_COLS; `$pe_cols_var`++)
			{
				//; my $out_buf_idx_ap = "";
				//; my @v_drain_buffer_order_ubs = split(',', $drain_buffer_order_ubs);
				//; my @v_out_to_buffer_map = split(',', $out_to_buffer_map);
				//; for (my $i = 0; $i < @v_drain_buffer_order_ubs; $i++) {
				//;		my $factor = 1;
				//;		for (my $j = $i + 1; $j < @v_drain_buffer_order_ubs; $j++) {
				//;			$factor *= $v_drain_buffer_order_ubs[$j];
				//;		}
				//;		$out_buf_idx_ap .= "_i".$v_out_to_buffer_map[$i]."_1"."*".$factor;	
				//;		if ($i < @v_drain_buffer_order_ubs - 1) {
				//;			$out_buf_idx_ap .= "+";
				//;		}
				//; }
				int out_buf_idx = `$out_buf_idx_ap`; // access pattern

				out_buf[out_buf_idx] = root_data.drain_data[j2];

			}

			//; my @v_drain_out_order_ubs = split(',', $drain_out_order_ubs);
			//; for (my $i = @v_drain_out_order_ubs - 2; $i >=0 ; $i--){
			//;     $it_name = "_i".$i."_1";
			`$it_name`++;
			if (`$it_name` == `$v_drain_out_order_ubs[$i]`)
			{
			`$it_name` = 0;
			//; }
			//; for (my $i = @v_drain_out_order_ubs - 2; $i >=0 ; $i--){
			}
			//; }
		}

		//; my @v_drain_buffer_order_ubs = split(',', $drain_buffer_order_ubs);
		//; for (my $i = 0; $i < @v_drain_buffer_order_ubs; $i++){
		//;         $it_name = "_i".$i."_1";
		int `$it_name` = 0; 
		//; }
		while (1)
		{
			//; my $out_buf_idx_ap = "";
			//; for (my $i = 0; $i < @v_drain_buffer_order_ubs; $i++) {
			//;		my $factor = 1;
			//;		for (my $j = $i + 1; $j < @v_drain_buffer_order_ubs; $j++) {
			//;			$factor *= $v_drain_buffer_order_ubs[$j];
			//;		}
			//;		$out_buf_idx_ap .= "_i".$i."_1"."*".$factor;	
			//;		if ($i < @v_drain_buffer_order_ubs - 1) {
			//;			$out_buf_idx_ap .= "+";
			//;		}
			//; }
			int out_buf_idx = `$out_buf_idx_ap`;

			//; my $out_buf_idx_factor = 1;
			//;	for (my $i = 0; $i < @v_drain_buffer_order_ubs; $i++) {
			//;		$out_buf_idx_factor *= $v_drain_buffer_order_ubs[$i];
			//;	}
			//; my $out_idx_ap_prefix = "(";
			//; for (my $i = 0; $i < @v_outer_loop_ubs; $i++) {
			//;		my $factor = 1;
			//;		for (my $j = $i + 1; $j < @v_outer_loop_ubs; $j++) {
			//;			$factor *= $v_outer_loop_ubs[$j];
			//;		}
			//;		$out_idx_ap_prefix .= "_i".$i."_0"."*".$factor;	
			//;		if ($i < @v_outer_loop_ubs - 1) {
			//;			$out_idx_ap_prefix .= "+";
			//;		}
			//; }
			//; $out_idx_ap_prefix .= ")";
			//; $out_idx_ap_prefix .= "*".$out_buf_idx_factor;
			int out_idx = `$out_idx_ap_prefix` + out_buf_idx;

			out[out_idx] = out_buf[out_buf_idx];

			//; my @v_drain_buffer_order_ubs = split(',', $drain_buffer_order_ubs);
			//; for (my $i = @v_drain_buffer_order_ubs - 1; $i >=0 ; $i--){
			//;     $it_name = "_i".$i."_1";
			`$it_name`++;
			if (`$it_name` == `$v_drain_buffer_order_ubs[$i]`)
			{
			`$it_name` = 0;
			//; }
			//; for (my $i = @v_drain_buffer_order_ubs - 1; $i >=0 ; $i--){
			}
			//; }
		}

		//; my @v_outer_loop_ubs = split(',', $outer_loop_ubs);
		//; for (my $i = @v_outer_loop_ubs - 1; $i >=0 ; $i--){
		//;     $it_name = "_i".$i."_0";
		`$it_name`++;
		if (`$it_name` == `$v_outer_loop_ubs[$i]`)
		{
		`$it_name` = 0;
		//; }
		//; for (my $i = @v_outer_loop_ubs - 1; $i >=0 ; $i--){
		}
		//; }
	}

#endif

	//; if ($debug_drain_o eq "on") {
	printf("drain_O finished!!!\n");
	//; }
	
}
