//; 
//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");
//; 
//; my $buffer_dim_v= $cfg_root->{Feeder_V}->{buffer_dim};
//;
//; my $block_dim_h= $cfg_root->{Loader_H}->{block_dim};
//; my $block_dim_v= $cfg_root->{Loader_V}->{block_dim};
//;
//; my $overlap_size= $cfg_root->{Feeder_V}->{overlap_size};
//; my $iteration_domain= $cfg_root->{Feeder_V}->{iteration_domain};
//; my $access_pattern = $cfg_root->{Feeder_V}->{access_pattern};
//; 
//; 
//;#   my $debug_feeder_w = "on";
//; my $debug_feeder_w = "off";
//; 
//; 

// instantiate W feeders
__attribute__((max_global_work_dim(0)))
__attribute__((autorun))
__attribute__((num_compute_units(SYS_ARRAY_NUM_COLS)))
__kernel void feed_W_kernel()
{
    const int feeder_id = get_compute_id(0);

//; if ($debug_feeder_w eq "on") {
    int rd_cnt = 0;
    int wr_cnt = 0;
    int fw_cnt = 0;
//; }

//; my $buffer_define = "";
//; my $fb_total_size = 1;
//; my @v_buffer_dim_v = split(',', $buffer_dim_v);
//; foreach my $one_dim (@v_buffer_dim_v) {
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

    vec_data_precision_t block_double_buffer[2]`$buffer_define`;
//	vec_float_t block_double_buffer[2][BS_O][K][K][BS_I];
    

    const bool LAST_FEEDER_IN_CHAIN             = (feeder_id == (SYS_ARRAY_NUM_COLS - 1));

    int write_side_idx = 0;
    int forward_idx = 0;
    int read_side_idx = 0;

    bool more_to_write_to_buffer          = true;     // the feeder will first fill up one of its buffers before starting the feed the array
    bool more_to_feed_to_sysarr           = false;    // there is nothing to feed to sys array in the begining, wait for the buffer to fill up and then swap!
    bool more_data_to_forward             = false; // this is always false for the LAST_FEEDER_IN_CHAIN
    bool buffer_id_to_write_to            = 0;        // 0 .. 1
    bool buffer_id_to_feed_to_sysarr      = 1;        // 0 .. 1

//; my @v_buffer_dim_v = split(',', $buffer_dim_v);
//; for (my $i = @v_buffer_dim_v - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_w";
	int `$it_name` = 0; 
//; }

//; my @v_iteration_domain = split(',', $iteration_domain);
//; for (my $i = @v_iteration_domain - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_r";
	int `$it_name` = 0; 
//; }

//	int _i3_w = 0; 
//	int _i2_w = 0; 
//	int _i1_w = 0; 
//	int _i0_w = 0; 
//
//	int _i5_r = 0; 
//	int _i4_r = 0; 
//	int _i3_r = 0; 
//	int _i2_r = 0; 
//	int _i1_r = 0; 
//	int _i0_r = 0; 

    while (1) {

//////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////

//#define FB_W_DIM	BS_I
//#define FB_PAD_SIZE_W   0
//#define FB_BLOCK_SIZE_W  (BS_O * K * K * FB_W_DIM - FB_PAD_SIZE_W) // not including padding
//#define FB_FORWARD_SIZE_W ((FB_BLOCK_SIZE_W * PE_COLS + FB_PAD_SIZE_W) - (FB_BLOCK_SIZE_W * (feeder_id + 1)))  // total_b_block_size - already_stored
//#define FB_READ_SIZE_W	 BS_R * BS_C * BS_O * K * K * FB_W_DIM

#define FB_PAD_SIZE_W   `$overlap_size`
#define FB_BLOCK_SIZE_W (`$fb_total_size - $overlap_size`)
#define FB_FORWARD_SIZE_W ((FB_BLOCK_SIZE_W * PE_COLS + FB_PAD_SIZE_W) - (FB_BLOCK_SIZE_W * (feeder_id + 1)))  // total_b_block_size - already_stored
#define FB_READ_SIZE_W	`$fb_read_size`

//////////////////////////////////////////////////////////////
// WRITE SIDE:
// 1. reading from chain
// 2. update buffee
// 3. forward into chain
//////////////////////////////////////////////////////////////

        // Note:
        // more_to_write_to_buffer: writing into local buffer (including the padding part)

        struct ch_data_col_feeder_chain_struct data_read_from_channel;
        bool read_channel_success;

        //bool data_is_mine_to_buffer = (LAST_FEEDER_IN_CHAIN || (!LAST_FEEDER_IN_CHAIN && !more_data_to_forward));
        bool data_is_mine_to_buffer = more_to_write_to_buffer;

        // 1. reading from chain
        if (more_to_write_to_buffer || (!LAST_FEEDER_IN_CHAIN && more_data_to_forward)) {
            if (feeder_id == 0) {
               // data_read_from_channel = read_channel_nb_altera(ch_data_col_loader_to_first_feeder, &read_channel_success);
                data_read_from_channel = read_channel_nb_intel(ch_data_col_loader_to_first_feeder, &read_channel_success);
            } else {
               // data_read_from_channel = read_channel_nb_altera(ch_data_col_feeder_chain[feeder_id], &read_channel_success);
                data_read_from_channel = read_channel_nb_intel(ch_data_col_feeder_chain[feeder_id], &read_channel_success);
            }
        } else {
            read_channel_success = false;
        }

        if (data_is_mine_to_buffer && read_channel_success)
        {
            vec_data_precision_t data        = data_read_from_channel.data;

//; my @v_buffer_dim_v = split(',', $buffer_dim_v);
//; my $idx_ref = "";
//; for (my $i = @v_buffer_dim_v - 1; $i >=0 ; $i--){
//;         $idx_ref = "[_i".$i."_w"."]".$idx_ref;
//; }
            block_double_buffer[buffer_id_to_write_to]`$idx_ref` = data;
           // block_double_buffer[buffer_id_to_write_to][_i0_w][_i1_w][_i2_w][_i3_w] = data;

			/*
			if (feeder_id == 0) {
			printf("{");
			for (int i2 = 0; i2 < PE_DSIZE; i2++) {
				printf(" %.2lf", data[i2]);
			}
			printf("}\n");
			}
			*/

//; my @v_block_dim_v = split(',', $block_dim_v);
//; my $num_blocks_to_pe_v = 1;
//; for (my $i = 0; $i < @v_block_dim_v; $i++){
//;    $num_blocks_to_pe_v *= $v_block_dim_v[$i];
//; }
//;
//; my @v_block_dim_h = split(',', $block_dim_h);
//; my $block_reuse_num_v = 1;
//; for (my $i = 0; $i < @v_block_dim_h - 1; $i++){
//;    $block_reuse_num_v *= $v_block_dim_h[$i];
//; }
//;
//; my $num_blocks_to_pe_v = $num_blocks_to_pe_v * $block_reuse_num_v;
//; my $num_blocks_to_pe_v = $num_blocks_to_pe_v + $v_block_dim_v[@v_block_dim_v - 1];
//;
//; if ($debug_feeder_w eq "on") {
			{
			//	int total = (FB_BLOCK_SIZE_W + FB_PAD_SIZE_W) * (cfg_BN_R[0] * cfg_BN_C[0] * cfg_BN_O[0] + 1) * cfg_BN_I[0];
				int total = (FB_BLOCK_SIZE_W + FB_PAD_SIZE_W) * `$num_blocks_to_pe_v`;
				if (feeder_id == 0)
					printf("Feeder_W(%d): rd=%d (total=%d)\n", feeder_id, rd_cnt++, total);
			}
//; }
        }

        if (!LAST_FEEDER_IN_CHAIN) { // last feeder does not forward data
            if (more_data_to_forward && read_channel_success)
            {
               // write_channel_altera(ch_data_col_feeder_chain[feeder_id+1], data_read_from_channel);
                write_channel_intel(ch_data_col_feeder_chain[feeder_id+1], data_read_from_channel);
            }
        }

        if (more_to_write_to_buffer && read_channel_success)
        {
            write_side_idx ++;

//; my @v_buffer_dim_v = split(',', $buffer_dim_v);
//; for (my $i = @v_buffer_dim_v - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_w";
			`$it_name`++;
			if (`$it_name` == `$v_buffer_dim_v[$i]`)
			{
				`$it_name` = 0;
//; }
//; for (my $i = @v_buffer_dim_v - 1; $i >=0 ; $i--){
		}
//; }
		//	_i3_w++;
		//	if (_i3_w == BS_I) {
		//		_i3_w = 0;
		//		_i2_w++;
		//		if (_i2_w == K) {
		//			_i2_w = 0;
		//			_i1_w++;
		//			if (_i1_w == K) {
		//				_i1_w = 0;
		//				_i0_w++;
		//				if (_i0_w == BS_O) {
		//					_i0_w = 0;
		//				}
		//			}
		//		}
		//	}

            if (write_side_idx == FB_BLOCK_SIZE_W + FB_PAD_SIZE_W)
            {
                more_to_write_to_buffer= false; 
            }
            if (write_side_idx == FB_BLOCK_SIZE_W && !LAST_FEEDER_IN_CHAIN) 
            {
                more_data_to_forward = true;
                forward_idx = -1; // to enable forwarding from next cycle
            }


            if (write_side_idx == FB_BLOCK_SIZE_W + FB_PAD_SIZE_W){
                write_side_idx = 0;
//; my @v_buffer_dim_v = split(',', $buffer_dim_v);
//; for (my $i = @v_buffer_dim_v - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_w";
				`$it_name` = 0;
//;}
			//	_i3_w = 0;
			//	_i2_w = 0;
			//	_i1_w = 0;
			//	_i0_w = 0;
            }
        }

        if (more_data_to_forward && read_channel_success)
        {
            forward_idx ++;
            if (forward_idx == FB_FORWARD_SIZE_W)
            {
                more_data_to_forward = 0;
                forward_idx = 0;
            }
        }

//////////////////////////////////////////////////////////////
// READ SIDE:
// 1. loop structure, including a reuse loop
// 2. update the addressing
//////////////////////////////////////////////////////////////

        if (more_to_feed_to_sysarr) {
            struct ch_data_b_struct sys_arr_feed_data;

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

            sys_arr_feed_data.data = 
                block_double_buffer[buffer_id_to_feed_to_sysarr]`$idx_ref`;
//            sys_arr_feed_data.data = 
//                block_double_buffer[buffer_id_to_feed_to_sysarr][_i5_r][_i0_r][_i1_r][_i2_r];

           // bool write_sys_arr_channel_success = write_channel_nb_altera(ch_data_b_boundary[feeder_id], sys_arr_feed_data);
            bool write_sys_arr_channel_success = write_channel_nb_intel(ch_data_b_boundary[feeder_id], sys_arr_feed_data);

            if (write_sys_arr_channel_success) {

				/*
				if (feeder_id == 0) {
					if (_i5_r == 0 &&_i3_r == 0 && _i0_r == 0 && _i4_r == 0 && _i1_r == 0 && _i2_r == 0) {
						printf("WWWWWW{");
						for (int i2 = 0; i2 < PE_DSIZE; i2++) {
							printf(" %.2lf", sys_arr_feed_data.data[i2]);
						}
						printf("}\n");
					}
				}
				*/

//; if ($debug_feeder_w eq "on") {
			//	int total = BS_R * BS_C * BS_O * K * K * BS_I * (cfg_BN_R[0] * cfg_BN_C[0] * cfg_BN_O[0] + 1) * cfg_BN_I[0];
				int total = `$num_blocks_to_pe_v` * FB_READ_SIZE_W;
            if (feeder_id == 0)
				printf("Feeder_W(%d): wr=%d (total=%d)\n", feeder_id, wr_cnt++, total);
//; }

		        read_side_idx++;
//; my @v_iter_domain = split(',', $iteration_domain);
//; for (my $i = @v_iter_domain - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_r";
				`$it_name`++;
				if (`$it_name` == `$v_iter_domain[$i]`)
				{
					`$it_name` = 0;
//; }
//; for (my $i = @v_iter_domain - 1; $i >=0 ; $i--){
				}
//; }

				//	_i5_r++;
				//	if (_i5_r == BS_O) {
				//		_i5_r = 0;
				//		_i4_r++;
				//		if (_i4_r == BS_C) {
				//			_i4_r = 0;
				//			_i3_r++;
				//			if (_i3_r == BS_R) {
				//				_i3_r = 0;
				//				_i2_r++;
				//				if (_i2_r == BS_I) {
				//					_i2_r = 0;
				//					_i1_r++;
				//					if (_i1_r == K) {
				//						_i1_r = 0;
				//						_i0_r++;
				//						if (_i0_r == K) {
				//							_i0_r = 0;
				//						}
				//					}
				//				}
				//			}
				//		}
				//	}

                if (read_side_idx == /*FB_IN_DIM * K * K *  BSIZE_J*/ FB_READ_SIZE_W)
                {
		            read_side_idx = 0;
                   
//; my @v_iter_domain = split(',', $iteration_domain);
//; for (my $i = @v_iter_domain - 1; $i >=0 ; $i--){
//;         $it_name = "_i".$i."_r";
					`$it_name` = 0;
//; }

				//	_i5_r = 0;
				//	_i4_r = 0;
				//	_i3_r = 0;
				//	_i2_r = 0;
				//	_i1_r = 0;
				//	_i0_r = 0;

                    more_to_feed_to_sysarr = false;
                }
            }
        }


        if (!more_to_write_to_buffer && !more_to_feed_to_sysarr && !more_data_to_forward) {
            more_to_write_to_buffer = true;
            more_to_feed_to_sysarr  = true;
            buffer_id_to_feed_to_sysarr = !buffer_id_to_feed_to_sysarr;
            buffer_id_to_write_to       = !buffer_id_to_write_to;
        }

    }

}

