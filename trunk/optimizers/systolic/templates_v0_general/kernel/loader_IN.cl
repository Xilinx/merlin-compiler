
//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");

//; my $block_dim_h = $cfg_root->{Loader_H}->{block_dim};
//; my $block_size_h = $cfg_root->{Loader_H}->{block_size};

//; my $block_dim_v = $cfg_root->{Loader_V}->{block_dim};

//; my $pe_dsize = $cfg_root->{PE}->{PE_DSIZE};

//;# my $debug_loader_in = "on";
//; my $debug_loader_in = "off";
	
__attribute__((max_global_work_dim(0)))
__kernel void load_IN_and_forward( __global vec_data_precision_t *restrict input_fm_block_wise)
{
	//; if ($debug_loader_in eq "on") {
	printf("Loader_IN started!\n");
	//; }
	
//; my @v_block_dim_h = split(',', $block_dim_h);
//; my $num_blocks_in_col_h = 1;
//; for (my $i = 0; $i < @v_block_dim_h - 1; $i++){
//;    $num_blocks_in_col_h *= $v_block_dim_h[$i];
//; }
//;
//; my @v_block_dim_v = split(',', $block_dim_v);
//; my $block_reuse_num_h = $v_block_dim_v[0];

//; my @v_block_size_h = split(',', $block_size_h);
//; my $block_num_vectors_h = 1;
//; for (my $i = 0; $i < @v_block_size_h; $i++){
//;    $block_num_vectors_h *= $v_block_size_h[$i];
//; }
//; $block_num_vectors_h /= $pe_dsize;
//; my $num_vectors_in_row_h = $block_num_vectors_h * $v_block_dim_h[@v_block_dim_h - 1];

//	unsigned int in_num_vectors_in_rxc_of_blocks = BN_I * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I; // number of vectors in one block line in the (r,c) direction
	unsigned int in_num_vectors_in_rxc_of_blocks = `$num_vectors_in_row_h`; // number of vectors in one block line in the (r,c) direction
//	unsigned char in_num_blocks_in_pxqxi = BN_R * BN_C;
	unsigned int in_num_blocks_in_pxqxi = `$num_blocks_in_col_h`;
//	unsigned char w_num_blocks_in_pxqxi = BN_O;
	unsigned int w_num_blocks_in_pxqxi = `$block_reuse_num_h`;
	
	//; if ($debug_loader_in eq "on") {
	int wr_cnt = 0;
	//; }

    // 32K x 32K largest matrix size supported
    unsigned int block_col_id = 1;

    unsigned int vector_id_in_block = 0;
    unsigned int vector_id_in_row_of_blocks = 1;
    unsigned int vector_id_global = 0;
    unsigned int vector_id_global_row_of_blocks_start = 0;

    unsigned int row_of_blocks_reuse_counter = 1;

    bool new_row_col_pair = false;
    bool more_vectors_to_load_and_forward = true;
    bool feed_zeros_to_flush_last_C_block = false;

    int cnt = 0;
    int last = 0;

    while (more_vectors_to_load_and_forward) {
        if (new_row_col_pair && !last) {
            last = 1;
        }
        if (!new_row_col_pair) last = 0;
        cnt ++;

        struct ch_data_row_feeder_chain_struct IN_local;

        if (feed_zeros_to_flush_last_C_block) {
            IN_local.data = VECTOR_ZERO;
        } else {
            IN_local.data = input_fm_block_wise[vector_id_global];
        }

        IN_local.new_row_col_pair = new_row_col_pair;
#if (ALTERA_FPGA_SDK_16_1 == 1)
        write_channel_altera(ch_data_row_loader_to_first_feeder, IN_local);
#endif
#if (INTEL_FPGA_SDK_17_1 == 1)
        write_channel_intel(ch_data_row_loader_to_first_feeder, IN_local);
#endif

		//; if ($debug_loader_in eq "on") {
        if (wr_cnt % 100 == 0)
		{
			int total = in_num_vectors_in_rxc_of_blocks * (1 + in_num_blocks_in_pxqxi * w_num_blocks_in_pxqxi);
		//	printf("in_num_blocks_in_pxqxi=%d\n", in_num_blocks_in_pxqxi);
		//	printf("in_num_vectors_in_rxc_of_blocks=%d\n", in_num_vectors_in_rxc_of_blocks);
		//	printf("w_num_blocks_in_pxqxi=%d\n", w_num_blocks_in_pxqxi);
			printf("Loader_IN wr=%d (total=%d)\n",
					wr_cnt, total);
		}
		wr_cnt++;
	//;}


        if (vector_id_in_block == (IN_BLOCK_NUM_VECTORS - 1)) {
            vector_id_in_block = 0;
            new_row_col_pair = false; // we keep new_row_col_pair=true for only the first block in the row of blocks (feeders in the daisy chain expect this functionality)
        } else {
            vector_id_in_block++;
        }


        vector_id_global++;
        if (vector_id_in_row_of_blocks == in_num_vectors_in_rxc_of_blocks) {
            vector_id_in_row_of_blocks = 1;
            new_row_col_pair = true;                                 // anytime we start feeding a row of blocks we set new_row_col_pair=true, kept true for the first block only (see above)

            if (feed_zeros_to_flush_last_C_block) {
                more_vectors_to_load_and_forward = false; // we feed only one row of blocks full of zeros to flush the last C block
            }

            if (row_of_blocks_reuse_counter == w_num_blocks_in_pxqxi) { // done reusing this row of blocks?
                row_of_blocks_reuse_counter = 1;
                vector_id_global_row_of_blocks_start = vector_id_global; // mark the new start of the row of blocks

                if (block_col_id == in_num_blocks_in_pxqxi) {    // done loading and forwarding the matrix? start feeding zeros to flush last C block
                    feed_zeros_to_flush_last_C_block = true;
                }
                block_col_id++;                                          // increment the block_id in the column of blocks

            } else {
                vector_id_global = vector_id_global_row_of_blocks_start;  // not done reusing, reset the vector_id_global to the start of row of blocks
                row_of_blocks_reuse_counter++;
            }
        } else {
            vector_id_in_row_of_blocks++;
        }

    }

	//; if ($debug_loader_in eq "on") {
	printf("Loader_IN finished!\n");
	//; }
	
}
