
//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");

//; my $block_dim_v = $cfg_root->{Loader_V}->{block_dim};
//; my $block_size_v = $cfg_root->{Loader_V}->{block_size};

//; my $block_dim_h = $cfg_root->{Loader_H}->{block_dim};

//; my $pe_dsize = $cfg_root->{PE}->{PE_DSIZE};

//;#   my $debug_loader_w = "on";
//; my $debug_loader_w = "off";

__attribute__((max_global_work_dim(0)))
__kernel void load_W_and_forward(__global vec_data_precision_t *restrict weights_block_wise)
{
//; if ($debug_loader_w eq "on") {
    printf("loader_W started!\n");
//; }

//; if ($debug_loader_w eq "on") {
	int wr_cnt = 0;
//; }

//; my @v_block_dim_v = split(',', $block_dim_v);


//; my @v_block_size_v = split(',', $block_size_v);
//; my $block_num_vectors_v = 1;
//; for (my $i = 0; $i < @v_block_size_v; $i++){
//;    $block_num_vectors_v *= $v_block_size_v[$i];
//; }
//; $block_num_vectors_v /= $pe_dsize;
//; my $num_vectors_in_row_v = $block_num_vectors_v * $v_block_dim_v[@v_block_dim_v - 1];

//; my $num_vectors_in_v = $num_vectors_in_row_v * $v_block_dim_v[0];

//; my @v_block_dim_h = split(',', $block_dim_h);
//; my $block_reuse_num_v = 1;
//; for (my $i = 0; $i < @v_block_dim_h - 1; $i++){
//;    $block_reuse_num_v *= $v_block_dim_h[$i];
//; }

//	unsigned int w_num_vectors_in_o_of_blocks = BN_I * O * BS_O * K * K * BS_I; // number of vectors in one block line in (p,q,i) direction
	unsigned int w_num_vectors_in_o_of_blocks = `$num_vectors_in_row_v`; // number of vectors in one block line in (p,q,i) direction
//	unsigned int w_num_vectors_in_weights = BN_O * w_num_vectors_in_o_of_blocks;
	unsigned int w_num_vectors_in_weights = `$num_vectors_in_v`;
//	unsigned char in_num_blocks_in_pxqxi = BN_R * BN_C;
	unsigned int in_num_blocks_in_pxqxi = `$block_reuse_num_v`;

    // 32K x 32K largest matrix size supported
    unsigned int vector_id_in_col_of_blocks = 1;
    unsigned int vector_id_global = 1;
    unsigned int V_reuse_counter = 1;

    bool more_vectors_to_load_and_forward = true;
    bool feed_zeros_to_flush_last_C_block = false;

    while (more_vectors_to_load_and_forward) {

        struct ch_data_col_feeder_chain_struct W_local;

        if (feed_zeros_to_flush_last_C_block) {
            W_local.data = VECTOR_ZERO;
        } else {
            W_local.data = weights_block_wise[vector_id_global-1];
        }

#if (ALTERA_FPGA_SDK_16_1 == 1)
        write_channel_altera(ch_data_col_loader_to_first_feeder, W_local);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
        write_channel_intel(ch_data_col_loader_to_first_feeder, W_local);
#endif

		//; if ($debug_loader_w eq "on") {
		{
			int total = w_num_vectors_in_weights * in_num_blocks_in_pxqxi + w_num_vectors_in_o_of_blocks ;
	//		printf("Loader_V wr=%d data=%.1lf (total=%d)\n",
	//				wr_cnt, W_local.data, total);
			printf("Loader_W wr=%d (total=%d)\n",
					wr_cnt, total);
			wr_cnt++;
		}
		//; }


        if (vector_id_in_col_of_blocks == w_num_vectors_in_o_of_blocks) {
            vector_id_in_col_of_blocks = 1;
            if (feed_zeros_to_flush_last_C_block) {
                more_vectors_to_load_and_forward = false; // we feed only one row of blocks full of zeros to flush the last C block
            }
        } else {
            vector_id_in_col_of_blocks++;
        }

        if (vector_id_global == w_num_vectors_in_weights) {
            vector_id_global = 1;
            if (V_reuse_counter == in_num_blocks_in_pxqxi) { // done reload and forwarding the matrix data?
                feed_zeros_to_flush_last_C_block = true;
            }
            V_reuse_counter++;
        } else {
            vector_id_global++;
        }

    }

	//; if ($debug_loader_w eq "on") {
	printf("Loader_W finished!!!\n");
	//; }

}

