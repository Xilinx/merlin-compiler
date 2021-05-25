
//;#   my $debug_pe = "on";
//;  my $debug_pe = "off";
//;
//;#   my $debug_pe_idx = "on";
//;  my $debug_pe_idx = "off";


#define K 3

// PE function (dot product, accumulate and interleave). It is the core part of the PE kernel.
void PE_dot_product_and_acc(vec_float_t A, vec_float_t B, float* accum_shift_reg, char new_row_col_pair)
{
    float sum = 0;
    new_row_col_pair = new_row_col_pair & 1;

#pragma unroll
    for(int d=0; d < DOT_PROD_VECTOR_SIZE; ++d) {
#if DOT_PROD_VECTOR_SIZE==1
        sum += A * B;
#else
        sum += A[d] * B[d];
#endif
    }

    float accum = sum + (new_row_col_pair ? 0.0f : accum_shift_reg[ACCUM_SHIFT_REG_SIZE-1]);

#pragma unroll
    for (int i = ACCUM_SHIFT_REG_SIZE-1; i >= 1; i--) {
        accum_shift_reg[i] = accum_shift_reg[i - 1];
    }

    accum_shift_reg[0] = accum;
}


__attribute__((max_global_work_dim(0)))
__attribute__((autorun))
__attribute__((num_compute_units(SYS_ARRAY_NUM_ROWS,SYS_ARRAY_NUM_COLS)))
__kernel void PE_kernel()
{
    const int row = get_compute_id(0);
    const int col = get_compute_id(1);

//; if ($debug_pe eq "on") {
	printf("PE(%d,%d) started!\n", row, col);
//;}

//; if ($debug_pe eq "on") {
	int wr_out = 0;
//; }

//; if ($debug_pe_idx eq "on") {
	int acc_idx = 0;
	int s_first = 1;
//; }

#ifndef EMULATOR  // don't use packed in the emulator
    __attribute__((packed))
#endif
    struct C_out_shift_reg_struct {
        float data;  // results are scalar floats
        bool valid;  // indicates a valid result in the "C_out_shift_reg" shift register
    };

    // a drain buffer to which we copy results from the PE_accum_shift_reg
    // PE can then compute next set of results concurrently as we drain C_out_shift_reg down the PE column
    struct C_out_shift_reg_struct C_out_shift_reg[C_OUT_SHIFT_REG_SIZE];
    float PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE];

#pragma unroll
    for (int i=0; i < C_OUT_SHIFT_REG_SIZE ; i++) {
        C_out_shift_reg[i].data  = 0.0f;
        C_out_shift_reg[i].valid = false;
    }

#pragma unroll
    for (int i=0; i < ACCUM_SHIFT_REG_SIZE  ; i++) {
        PE_accum_shift_reg[i] = 0.0f;
    }

    while (1) {

        // private variables which are used to read-in / write-out the channels
        struct ch_data_a_struct   A_local;
        vec_float_t               A_local_data;
        char                      new_row_col_pair_local;

        struct ch_data_b_struct   B_local;
        vec_float_t               B_local_data;

        float                     C_local_in_data;
        bool                      C_local_in_valid;
        struct ch_data_o_struct   C_local_in;

        float                     C_local_out_data;
        bool                      C_local_out_valid;
        struct ch_data_o_struct   C_local_out;

        if (col==0) {
            A_local                = read_channel_altera(ch_data_a_boundary[row]);
        } else {
            A_local                = read_channel_altera(ch_data_a[row][col]);
        }
        A_local_data           = A_local.data;
        new_row_col_pair_local = A_local.new_row_col_pair & 1;

        if (row==0) {
            B_local               = read_channel_altera(ch_data_b_boundary[col]);
        } else {
            B_local               = read_channel_altera(ch_data_b[row][col]);
        }
        B_local_data          = B_local.data;

        // if not the PE on the right boundary of systolic array then forward data to the PE on the right
        if (col < (SYS_ARRAY_NUM_COLS-1)) {
            write_channel_altera(ch_data_a[row][col+1], A_local);
        }

        // if not the PE on the top boundary of systolic array then forward data to the PE above
        if (row < (SYS_ARRAY_NUM_ROWS-1)) {
            write_channel_altera(ch_data_b[row+1][col], B_local);
        }

        // shift the C matrix results from the accumulator (PE_accum_shift_reg) into C_out_shift_reg and then down the PE column
        C_local_out_data  = C_out_shift_reg[0].data;
        C_local_out_valid = C_out_shift_reg[0].valid;

        // drain C data to to the PE below only if C_local_out is valid (the bottom of the C_out_shift_reg)
        if (C_local_out_valid) {
            C_local_out.data = C_local_out_data;
            if (row==0) {
                write_channel_altera( ch_data_o_boundary[col], C_local_out);
            } else  {
                write_channel_altera( ch_data_c[row][col], C_local_out);
            }
			//; if ($debug_pe eq "on") {
			int total = (PE_ROWS - row) * BS_R * BS_C * BS_O * cfg_BN_ * BNUM_J;
			if (row == (PE_ROWS - 22) && col == 0) {
				printf("PE(%d,%d): wr_out=%d total=%d\n", row, col, wr_out, total);
			}
			wr_out++;
			//; }

        }

#pragma unroll
        for (int local_shift_slot = 0; local_shift_slot < C_OUT_SHIFT_REG_SIZE-1; ++local_shift_slot) {
            C_out_shift_reg[local_shift_slot] = C_out_shift_reg[local_shift_slot+1];
        }

        if (new_row_col_pair_local) {     // copy a result from the PE_accum_shift_reg into C_out_shift_reg
            C_out_shift_reg[C_OUT_SHIFT_REG_SIZE-1].data  = PE_accum_shift_reg[ACCUM_SHIFT_REG_SIZE-1];
            C_out_shift_reg[C_OUT_SHIFT_REG_SIZE-1].valid = true;


        } else {

            if (row < (SYS_ARRAY_NUM_ROWS-1)) {         // pass the results from the PE above through the C_out_shift_reg to the PE below (draining down the PE column)
                bool success;
                C_local_in = read_channel_nb_altera(ch_data_c[row+1][col],&success);
                C_local_in_data  = C_local_in.data;
                C_local_in_valid = success;
            } else {
                C_local_in_valid = false;
            }

            C_out_shift_reg[C_OUT_SHIFT_REG_SIZE-1].data  = C_local_in_data;
            C_out_shift_reg[C_OUT_SHIFT_REG_SIZE-1].valid = C_local_in_valid;

        }

        PE_dot_product_and_acc(A_local_data, B_local_data, PE_accum_shift_reg, new_row_col_pair_local);

		//; if ($debug_pe_idx eq "on") {
//#if DEBUG_PE
        {
			// (BN_R * BN_C * BN_O + 1) * BN_I * K * K * BS_I * BS_R * BS_C * BS_O
			int vec_idx = acc_idx / (cfg_BN_I[0] * K * K * BS_I *  BS_R * BS_C * BS_O);
			int r0 = -1;
			int c0 = -1;
			int o0 = -1;
			int i0 = (acc_idx / (K * K * BS_I * BS_R * BS_C * BS_O)) % cfg_BN_I[0];
			int r2 = row;
			int o2 = col;
			int o1 = acc_idx % BS_O;
			int c1 = (acc_idx / BS_O) % BS_C;
			int r1 = (acc_idx / (BS_C * BS_O)) % BS_R;
			int i1 = (acc_idx / (BS_R * BS_C * BS_O)) % BS_I;
			int q = (acc_idx / (BS_R * BS_C * BS_O * BS_I)) % K;
			int p = (acc_idx / (BS_R * BS_C * BS_O * BS_I * K)) % K;

			int pe_idx = -1;

			if (vec_idx >= 0 && vec_idx < cfg_BN_R[0] * cfg_BN_C[0] * cfg_BN_O[0]) {
				r0 = vec_idx / (cfg_BN_C[0] * cfg_BN_O[0]);
				c0 = (vec_idx / cfg_BN_O[0]) % cfg_BN_C[0];
				o0 = vec_idx % cfg_BN_O[0];
				pe_idx = (r0 * cfg_BN_C[0] * cfg_BN_O[0] + c0 * cfg_BN_O[0] + o0) * BS_R * BS_C * BS_O + (r1 * BS_C + c1) * BS_O + o1; 
			}
		//	if (row == 0 && col == 0) {
		//		printf("acc_idx=%d\n", acc_idx);
		//	}

			/*
			if (r2 == 0 && o2 == 0 && o1 == 0 && c1 == 0 && r1 == 0 && i1 == 0 && p == 0 && q == 0) {
				printf("row=%d col=%d pe_idx=%d\n", row, col, pe_idx);
				printf("AAAAAA{");
				for (int i2 = 0; i2 < PE_DSIZE; i2++) {
					printf(" %.2lf", A_local_data[i2]);
				}
				printf("}\n");
				printf("BBBBBB{");
				for (int i2 = 0; i2 < PE_DSIZE; i2++) {
					printf(" %.2lf", B_local_data[i2]);
				}
				printf("}\n");
			}
			*/

            if (row==DEBUG_PE_I && col==DEBUG_PE_J && pe_idx == DEBUG_PE_IDX)
            {
                if (s_first)
                {
                 //   printf("\nDump PE for c[%d,%d] ... \n", i_idx, j_idx);
                 //   s_first = 0;
                }
                printf("PE[%d,%d]@%d %.2lf <+ {", 
                        row,col,pe_idx, PE_accum_shift_reg[0]);
                for(int d=0; d < DOT_PROD_VECTOR_SIZE; ++d) {

#if DOT_PROD_VECTOR_SIZE==1
                    printf(" %.1lf", A_local_data);
#else
                    printf(" %.1lf", A_local_data[d]);
#endif
                }
                printf("}.*{");
                for(int d=0; d < DOT_PROD_VECTOR_SIZE; ++d) {
#if DOT_PROD_VECTOR_SIZE==1
                    printf(" %.1lf", B_local_data);
#else
                    printf(" %.1lf", B_local_data[d]);
#endif
                }
              //  printf("} k=%d i1=%d p=%d q=%d o1=%d r1=%d c1=%d\n", k1, (k1 % 8), p, q, i1, (j1 / 4), (j1 % 4));
				printf("} r0=%d c0=%d o0=%d i0=%d p=%d q=%d i1=%d r1=%d c1=%d o1=%d\n", r0, c0, o0, i0, p, q, i1, r1, c1, o1);
            }

            acc_idx ++;
        }
//#endif
//; }
    }
}

