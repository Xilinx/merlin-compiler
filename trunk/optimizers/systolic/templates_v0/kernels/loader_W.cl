
//;#   my $debug_loader_w = "on";
//; my $debug_loader_w = "off";

//__attribute__((max_global_work_dim(-1)))
//__kernel void load_V_and_forward( __global vec_float_t* restrict B, 
//        unsigned int v_num_vectors_in_col_of_blocks, 
//        unsigned int v_num_vectors_in_matrix, 
//        unsigned char h_num_blocks_in_col )
__attribute__((max_global_work_dim(0)))
__kernel void load_W_and_forward(__global vec_float_t *restrict weights_block_wise)
{
//; if ($debug_loader_w eq "on") {
    printf("loader_W started!\n");
//; }

//; if ($debug_loader_w eq "on") {
	int wr_cnt = 0;
//; }

	unsigned int w_num_vectors_in_o_of_blocks = BN_I * O * BS_O * K * K * BS_I; // number of vectors in one block line in (p,q,i) direction
	unsigned int w_num_vectors_in_weights = BN_O * w_num_vectors_in_o_of_blocks;
	unsigned char in_num_blocks_in_pxqxi = BN_R * BN_C;

    // 32K x 32K largest matrix size supported
    unsigned int vector_id_in_col_of_blocks = 1;
    unsigned int vector_id_global = 1;
    unsigned char V_reuse_counter = 1;

    bool more_vectors_to_load_and_forward = true;
    bool feed_zeros_to_flush_last_C_block = false;

    while (more_vectors_to_load_and_forward) {

        struct ch_data_col_feeder_chain_struct W_local;

        if (feed_zeros_to_flush_last_C_block) {
            W_local.data = VECTOR_ZERO;
        } else {
            W_local.data = weights_block_wise[vector_id_global-1];
        }

        //write_channel_altera(ch_data_col_loader_to_first_feeder, W_local);
        write_channel_intel(ch_data_col_loader_to_first_feeder, W_local);

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
	

//	if (layer < NUM_CONV - 1) {
//	mem_fence(CLK_CHANNEL_MEM_FENCE);
//	int layer_id = read_channel_altera(ch_msg_loader_w_ready);
//	mem_fence(CLK_CHANNEL_MEM_FENCE);
//	printf("Loader W: layer_id = %d\n", layer_id);
//	}
}

