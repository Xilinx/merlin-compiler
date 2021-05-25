//; 
//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");
//; 
//; my $buffer_dim= $cfg_root->{Feeder_H}->{buffer_dim};
//; my $overlap_size= $cfg_root->{Feeder_H}->{overlap_size};
//; my $iteration_domain= $cfg_root->{Feeder_H}->{iteration_domain};
//; my $access_pattern = $cfg_root->{Feeder_H}->{access_pattern};
//; 
//;#   my $debug_feeder_in = "on";
//; my $debug_feeder_in = "off";
//; 
//; 
//; 

// instantiate input feature map feeders
__attribute__((max_global_work_dim(0)))
__attribute__((autorun))
__attribute__((num_compute_units(SYS_ARRAY_NUM_ROWS)))
__kernel void feed_IN_kernel()
{
	
    const int feeder_id = get_compute_id(0);

	//; if ($debug_feeder_in eq "on") {
	int wr_cnt = 0;
	int rd_cnt = 0;
	int fw_cnt = -1 * (BN_R * BN_C * BN_O + 1) * BN_I;
	//; }


	//; if ($debug_feeder_in eq "on") {
	printf("feeder_IN[%d] started\n", feeder_id);
	//; }

//; my $buffer_define = "";
//; my $fb_total_size = 1;
//; my @v_buffer_dim = split(',', $buffer_dim);
//; foreach my $one_dim (@v_buffer_dim) {
//;    $buffer_define .= "[".$one_dim."]";
//;    $fb_total_size *= $one_dim;
//; }
//;
//; my $fb_read_size = 1;
//; my @v_iteration_domain = split(',', $iteration_domain);
//; foreach my $one_iter (@v_iteration_domain) {
//;		$fb_read_size *= $one_iter;
//; }
//;
//; my $num_first_vectors = 1;
//; my @v_iteration_domain = split(',', $iteration_domain);
//; for (my $i = @v_iteration_domain - 1; $i >=1 ; $i--){
//;         $num_first_vectors *= $v_iteration_domain[$i];
//; }


//    vec_float_t block_double_buffer[2]`$buffer_define`;
	vec_float_t block_double_buffer[2][BS_R + K - 1][BS_C + K - 1][BS_I];

    const bool LAST_FEEDER_IN_CHAIN             = (feeder_id == (SYS_ARRAY_NUM_ROWS-1));

    int write_side_idx = 0;
    int forward_idx = 0;
    int read_side_idx = 0;


    bool more_data_to_forward                   = false; // this is always false for the LAST_FEEDER_IN_CHAIN

    bool buffer_id_to_write_to            = 0;        // 0 .. 1
    bool buffer_id_to_feed_to_sysarr      = 1;        // 0 .. 1


    bool more_to_write_to_buffer          = true;     // the feeder will first fill up one of its buffers before starting the feed the array
    bool more_to_feed_to_sysarr           = false;    // there is nothing to feed to sys array in the begining, wait for the buffer to fill up and then swap!

  //  unsigned char reuse_counter = 0;

    bool new_row_col_pair_feed   = false;
    bool new_row_col_pair_buffer = false;

//; my @v_buf_dims = split(',', $buffer_dim);
//; for (my $i = @v_buffer_dim - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_w";
//	int `$it_name` = 0; 
//; }

//; my @v_iter_domain = split(',', $iteration_domain);
//; for (my $i = @v_iter_domain - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_r";
//	int `$it_name` = 0; 
//; }

	////////////////////////////////////////
	// Definitions
	////////////////////////////////////////
//#define FB_PAD_SIZE_IN   `$overlap_size`
//#define FB_BLOCK_SIZE_IN (`$fb_total_size - $overlap_size`)
//#define FB_FORWARD_SIZE_IN ((FB_BLOCK_SIZE_H * PE_ROWS + FB_PAD_SIZE_H) - FB_BLOCK_SIZE_H * (feeder_id + 1))
//#define FB_READ_SIZE_IN	`$fb_read_size`
//#define NUM_FIRST_VECTORS `$num_first_vectors`

#define FB_IN_DIM	BS_I
#define FB_PAD_SIZE_IN   (BS_C + K - 1) * (K - 1) * FB_IN_DIM
#define FB_BLOCK_SIZE_IN  ((BS_R + K - 1) * (BS_C + K - 1) * FB_IN_DIM - FB_PAD_SIZE_IN) // not including padding
#define FB_FORWARD_SIZE_IN ((FB_BLOCK_SIZE_IN * PE_ROWS + FB_PAD_SIZE_IN) - FB_BLOCK_SIZE_IN * (feeder_id + 1))
#define FB_READ_SIZE_IN	 BS_R * BS_C * BS_O * K * K * FB_IN_DIM
#define NUM_FIRST_VECTORS BS_R * BS_C * BS_O

	int _i2_w = 0; 
	int _i1_w = 0; 
	int _i0_w = 0; 

	int _i5_r = 0; 
	int _i4_r = 0; 
	int _i3_r = 0; 
	int _i2_r = 0; 
	int _i1_r = 0; 
	int _i0_r = 0; 

	int bf_cnt = 0;
	int fb_cnt = 0;
	int fb1_cnt = 0;

    while (1) {


        struct ch_data_row_feeder_chain_struct data_read_from_channel;
        bool read_channel_success;

//		bool data_is_mine_to_buffer = (LAST_FEEDER_IN_CHAIN || (!LAST_FEEDER_IN_CHAIN && !more_data_to_forward));
		bool data_is_mine_to_buffer = more_to_write_to_buffer;

		// 1. reading from chain
        if (more_to_write_to_buffer || (!LAST_FEEDER_IN_CHAIN && more_data_to_forward)) {
            if (feeder_id == 0) {
                //data_read_from_channel = read_channel_nb_altera(ch_data_row_loader_to_first_feeder,            &read_channel_success);
                data_read_from_channel = read_channel_nb_intel(ch_data_row_loader_to_first_feeder,            &read_channel_success);
            } else {
                //data_read_from_channel = read_channel_nb_altera(ch_data_row_feeder_chain[feeder_id], &read_channel_success);
                data_read_from_channel = read_channel_nb_intel(ch_data_row_feeder_chain[feeder_id], &read_channel_success);
            }
        } else {
            read_channel_success = false;
        }

        // if data is mine to buffer we will read from the channel only if there is more to write to the buffer (i.e., there is space in the buffer)
        // thus, is it guaranteed that the below condition will be satisfied only if there is more to write to the buffer
        if (data_is_mine_to_buffer /*more_to_write_to_buffer*/ && read_channel_success) {

            vec_float_t data        = data_read_from_channel.data;

            // new_row_col_pair read from the channel must have the same value throughout the entire block!
            // the load_A kernel guarantees this
            new_row_col_pair_buffer = data_read_from_channel.new_row_col_pair;

//; my @v_buf_dims = split(',', $buffer_dim);
//; my $idx_ref = "";
//; for (my $i = @v_buffer_dim - 1; $i >=0 ; $i--){
//;         $idx_ref = "[_i".$i."_w"."]".$idx_ref;
//; }

//           block_double_buffer[buffer_id_to_write_to]`$idx_ref` = data;
            block_double_buffer[buffer_id_to_write_to][_i0_w][_i1_w][_i2_w] = data;

			/*
			if (feeder_id == 0) {
			printf("{");
			for (int i2 = 0; i2 < PE_DSIZE; i2++) {
				printf(" %.2lf", data[i2]);
			}
			printf("}\n");
			}
			*/

			//; if ($debug_feeder_in eq "on") {
			{
				int total = (FB_BLOCK_SIZE_IN + FB_PAD_SIZE_IN) * (BN_R * BN_C * BN_O + 1) * BN_I;
				if (feeder_id == 0) {
					printf("Feeder_IN(%d): rd=%d (total=%d)\n",
							feeder_id, rd_cnt, total);
				}
				rd_cnt++;
			}
			//; }
        }

        if (!LAST_FEEDER_IN_CHAIN) { // last feeder does not forward data
            if (more_data_to_forward && read_channel_success) {
               // write_channel_altera(ch_data_row_feeder_chain[feeder_id+1], data_read_from_channel);
                write_channel_intel(ch_data_row_feeder_chain[feeder_id+1], data_read_from_channel);
            }
        }

        if (more_to_write_to_buffer && read_channel_success)
        {
            write_side_idx ++;

//; my @v_buf_dims = split(',', $buffer_dim);
//; for (my $i = @v_buffer_dim - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_w";
//			`$it_name`++;
//			if (`$it_name` == `$v_buffer_dim[$i]`)
//			{
//				`$it_name` = 0;
//; }
//; for (my $i = @v_buffer_dim - 1; $i >=0 ; $i--){
//		}
//; }
			_i2_w++;
			if (_i2_w == BS_I) {
				_i2_w = 0;
				_i1_w++;
				if (_i1_w == (BS_C + K - 1)) {
					_i1_w = 0;
					_i0_w++;
					if (_i0_w == (BS_R + K - 1)) {
						_i0_w = 0;
					}
				}
			}

            if (write_side_idx == FB_BLOCK_SIZE_IN + FB_PAD_SIZE_IN)
            {
                more_to_write_to_buffer = false; 
            }
            if (write_side_idx == FB_BLOCK_SIZE_IN && !LAST_FEEDER_IN_CHAIN) 
            {
                more_data_to_forward = true;
                forward_idx = -1; // to enable forwarding from next cycle

		//		if (feeder_id == 0)
		//			printf("fb1_cnt = %d\n", fb1_cnt++);
            }

            if (write_side_idx == FB_BLOCK_SIZE_IN + FB_PAD_SIZE_IN){
                write_side_idx = 0;

		//		if (feeder_id == 0)
		//			printf("bf_cnt = %d\n", bf_cnt++);
				
//; my @v_buf_dims = split(',', $buffer_dim);
//; for (my $i = @v_buffer_dim - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_w";
//				`$it_name` = 0;
//;}

				_i2_w = 0;
				_i1_w = 0;
				_i0_w = 0;
            }
        }

        if (more_data_to_forward && read_channel_success)
        {
            forward_idx ++;

	//		if (feeder_id == 0)
	//			printf("forward_idx = %d\n", forward_idx);

            if (forward_idx == FB_FORWARD_SIZE_IN)
            {
                more_data_to_forward = 0;
                forward_idx = 0;

//				if (feeder_id == 0) {
//					printf("fb_cnt = %d\n", fb_cnt++);
//				}
            }

			//; if ($debug_feeder_in eq "on") {
			{
				int total = FB_FORWARD_SIZE_IN * (BN_R * BN_C * BN_O + 1) * BN_I;
				if (feeder_id == 0) {
					printf("Feeder_IN(%d): fw=%d (total=%d)\n",
							feeder_id, fw_cnt, total);
				}
				fw_cnt++;
		//		if (feeder_id == 0) {
		//			printf("forward_idx=%d, fw_cnt=%d\n", forward_idx, fw_cnt);
		//		}
			}
			//; }
        }


//////////////////////////////////////////////////////////////
// READ SIDE:
// 1. loop structure, including a reuse loop
// 2. update the addressing
//////////////////////////////////////////////////////////////
		if (more_to_feed_to_sysarr) {
			struct ch_data_a_struct sys_arr_feed_data;

//;         my $idx_ref = "";
//; my @v_access_pattern= split(';', $access_pattern);
//; foreach my $idx_dim (@v_access_pattern) {
//;     my @v_coeff = split(',', $idx_dim);
//;     my $dim_ref = "";
//;     for (my $i = 0; $i < @v_coeff; $i++) {
//;         my $it_ref = "_i".$i."_r";
//;         if ($v_coeff[$i] == 0) {
//;         }
//;         elsif ($v_coeff[$i] == 1) {
//;            if ($dim_ref eq "") { $dim_ref = $it_ref; }
//;            else { $dim_ref .= "+".$it_ref; }
//;         }
//;         else {
//            $it_ref = "$v_coeff[$i]*".$it_ref;
//;            $it_ref = $it_ref;
//;            if ($dim_ref eq "") {$dim_ref = $it_ref;}
//;            else { $dim_ref .= "+".$it_ref;}
//;         }
//;     }
//;		if ($dim_ref eq "") { $dim_ref .= "0"; }
//;     $idx_ref .= "[".$dim_ref."]";
//; }

//            sys_arr_feed_data.data = 
//                block_double_buffer[buffer_id_to_feed_to_sysarr]`$idx_ref`;
            sys_arr_feed_data.data = 
                block_double_buffer[buffer_id_to_feed_to_sysarr][_i3_r + _i0_r][_i4_r + _i1_r][_i2_r];
            sys_arr_feed_data.new_row_col_pair = new_row_col_pair_feed;

           // bool write_sys_arr_channel_success = write_channel_nb_altera(ch_data_a_boundary[feeder_id], sys_arr_feed_data);
            bool write_sys_arr_channel_success = write_channel_nb_intel(ch_data_a_boundary[feeder_id], sys_arr_feed_data);

            if (write_sys_arr_channel_success) {

				/*
				if (feeder_id == 0) {
					if (_i5_r == 0 &&_i3_r == 0 && _i0_r == 0 && _i4_r == 0 && _i1_r == 0 && _i2_r == 0) {
						printf("HAHAHA{");
						for (int i2 = 0; i2 < PE_DSIZE; i2++) {
							printf(" %.2lf", sys_arr_feed_data.data[i2]);
						}
						printf("}\n");
					}
				}
				*/

				//; if ($debug_feeder_in eq "on") {
				{
					int total = BS_R * BS_C * BS_O * K * K * BS_I * (BN_R * BN_C * BN_O + 1) * BN_I;
					if (feeder_id == 0) {
						printf("Feeder_IN(%d): wr=%d (total=%d)\n",
								feeder_id, wr_cnt, total);
					}
					wr_cnt++;
				}
				//; }

		        read_side_idx++;
//; my @v_iter_domain = split(',', $iteration_domain);
//; for (my $i = @v_iter_domain - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_r";
//				`$it_name`++;
//				if (`$it_name` == `$v_iter_domain[$i]`)
//				{
//					`$it_name` = 0;
//; }
//; for (my $i = @v_iter_domain - 1; $i >=0 ; $i--){
//				}
//; }
				// (p,q,i,r1,c1,o1)
				_i5_r++;
				if (_i5_r == BS_O) { // o1
					_i5_r = 0;
					_i4_r++;
					if (_i4_r == BS_C) { // c1
						_i4_r = 0;
						_i3_r++;
						if (_i3_r == BS_R) { // r1
							_i3_r = 0;
							_i2_r++;
							if (_i2_r == BS_I) { // i1
								_i2_r = 0;
								_i1_r++;
								if (_i1_r == K) { // q
									_i1_r = 0;
									_i0_r++;
									if (_i0_r == K) { // p
										_i0_r = 0;
									}
								}
							}
						}
					}
				}

                if (read_side_idx == FB_READ_SIZE_IN)
                {
		            read_side_idx = 0;
                   
//; my @v_iter_domain = split(',', $iteration_domain);
//; for (my $i = @v_iter_domain - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_r";
//					`$it_name` = 0;
//; }

					_i0_r = 0;
					_i1_r = 0;
					_i2_r = 0;
					_i3_r = 0;
					_i4_r = 0;
					_i5_r = 0;

                    more_to_feed_to_sysarr = false;
                }

                // only the first vectors in the buffered block may need to reset the PEs accumulators \
                // (depending if this block is the first block in the matrix row)
                // hence, we feed the actual new_row_col_pair value only for the first vectors in the buffered rows
                // we always feed false for the other vectors
                bool done_feeding_first_vectors = (read_side_idx == NUM_FIRST_VECTORS);
                if (done_feeding_first_vectors) new_row_col_pair_feed = false;
            }
        }

        if (!more_to_write_to_buffer && !more_to_feed_to_sysarr && !more_data_to_forward) {
            more_to_write_to_buffer = true;
            more_to_feed_to_sysarr  = true;

            // swap_buffers, they have 0/1 IDs, can simply invert
            buffer_id_to_feed_to_sysarr = !buffer_id_to_feed_to_sysarr;
            buffer_id_to_write_to       = !buffer_id_to_write_to;

            new_row_col_pair_feed   = new_row_col_pair_buffer;
            new_row_col_pair_buffer = false; // reset to false
        }

    }

	//; if ($debug_feeder_in eq "on") {
	printf("feeder_IN[%d] finished\n", feeder_id);
	//; }
}
