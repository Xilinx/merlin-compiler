#include "../host/inc/systolic_params.h"

#define CHANNEL_DEPTH_BOUNDARY_A_B       16
#define __ATTR_CHANNEL_DEPTH_BOUNDARY_A_B__  __attribute__((depth(CHANNEL_DEPTH_BOUNDARY_A_B)))

#define CHANNEL_DEPTH_BOUNDARY_C       1024
#define __ATTR_CHANNEL_DEPTH_BOUNDARY_C__  __attribute__((depth(CHANNEL_DEPTH_BOUNDARY_C)))

#define CHANNEL_DEPTH_INNER_A_B       0
#define __ATTR_CHANNEL_DEPTH_INNER_A_B__ __attribute__((depth(CHANNEL_DEPTH_INNER_A_B)))

#define CHANNEL_DEPTH_INNER_C       0
#define __ATTR_CHANNEL_DEPTH_INNER_C__ __attribute__((depth(CHANNEL_DEPTH_INNER_C)))


#define VECTOR_FLOAT_ZERO   (float)(0.0f)
#define VECTOR_FLOAT2_ZERO   (float2)(0.0f, 0.0f)
#define VECTOR_FLOAT4_ZERO   (float4)(0.0f, 0.0f, 0.0f, 0.0f)
#define VECTOR_FLOAT8_ZERO   (float8)(VECTOR_FLOAT4_ZERO,VECTOR_FLOAT4_ZERO)
#define VECTOR_FLOAT16_ZERO (float16)(VECTOR_FLOAT8_ZERO,VECTOR_FLOAT8_ZERO)


#if DOT_PROD_VECTOR_SIZE==1
typedef float vec_float_t;
#define VECTOR_ZERO VECTOR_FLOAT_ZERO
#elif DOT_PROD_VECTOR_SIZE==2
typedef float2 vec_float_t;
#define VECTOR_ZERO VECTOR_FLOAT2_ZERO
#elif DOT_PROD_VECTOR_SIZE==4
typedef float4 vec_float_t;
#define VECTOR_ZERO VECTOR_FLOAT4_ZERO
#elif DOT_PROD_VECTOR_SIZE==8
typedef float8 vec_float_t;
#define VECTOR_ZERO VECTOR_FLOAT8_ZERO
#elif DOT_PROD_VECTOR_SIZE==16
typedef float16 vec_float_t;
#define VECTOR_ZERO VECTOR_FLOAT16_ZERO
#else
#error Unsupported DOT_PROD_VECTOR_SIZE
#endif

#ifndef EMULATOR  // don't use packed in the emulator
__attribute__((packed))
#endif
struct ch_data_a_struct {
    vec_float_t data;
    char new_row_col_pair;
};

#ifndef EMULATOR  // don't use packed in the emulator
__attribute__((packed))
#endif
struct ch_data_o_struct {
    float data;
};

#ifndef EMULATOR  // don't use packed in the emulator
__attribute__((packed))
#endif
struct ch_data_b_struct {
    vec_float_t data;
};

channel struct ch_data_a_struct  ch_data_a  [SYS_ARRAY_NUM_ROWS][SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_INNER_A_B__;
channel struct ch_data_b_struct  ch_data_b  [SYS_ARRAY_NUM_ROWS][SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_INNER_A_B__;
channel struct ch_data_o_struct  ch_data_c  [SYS_ARRAY_NUM_ROWS][SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_INNER_C__;

channel struct ch_data_a_struct  ch_data_a_boundary  [SYS_ARRAY_NUM_ROWS] __ATTR_CHANNEL_DEPTH_BOUNDARY_A_B__;
channel struct ch_data_b_struct  ch_data_b_boundary  [SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_BOUNDARY_A_B__;
channel struct ch_data_o_struct  ch_data_o_boundary  [SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_BOUNDARY_C__;

#ifndef EMULATOR
__attribute__((packed))
#endif
struct ch_data_row_feeder_chain_struct {
    vec_float_t data;
    char new_row_col_pair;
};

#ifndef EMULATOR
__attribute__((packed))
#endif
struct ch_data_col_feeder_chain_struct {
    vec_float_t data;
};

channel struct ch_data_row_feeder_chain_struct  ch_data_row_feeder_chain [SYS_ARRAY_NUM_ROWS] __attribute__((depth(0)));
channel struct ch_data_col_feeder_chain_struct  ch_data_col_feeder_chain [SYS_ARRAY_NUM_COLS] __attribute__((depth(0)));

channel struct ch_data_row_feeder_chain_struct  ch_data_row_loader_to_first_feeder  __attribute__((depth(64)));
channel struct ch_data_col_feeder_chain_struct  ch_data_col_loader_to_first_feeder  __attribute__((depth(64)));

channel int ch_msg_loader_in_ready __attribute__((depth(1)));
channel int ch_msg_loader_w_ready __attribute__((depth(1)));
//channel int ch_msg_loader_in_ready;
//channel int ch_msg_loader_w_ready;

channel struct custom_float_array ch_drain_to_postproc __attribute__((depth(1)));

constant unsigned int cfg_in_num[] = {16, 64, 64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512};
constant unsigned int cfg_out_num[] = {64, 64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512, 512};
constant unsigned int cfg_img_row[] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
constant unsigned int cfg_img_col[] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
constant unsigned int cfg_kernel[] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};

constant unsigned int cfg_BN_R[] = {12, 12, 6, 6, 3, 3, 3, 2, 2, 2, 1, 1, 1};
constant unsigned int cfg_BN_C[] = {16, 16, 8, 8, 4, 4, 4, 2, 2, 2, 1, 1, 1};
constant unsigned int cfg_BN_O[] = {2, 2, 4, 4, 8, 8, 8, 16, 16, 16, 16, 16, 16};
constant unsigned int cfg_BN_I[] = {1, 2, 2, 4, 4, 8, 8, 8, 16, 16, 16, 16, 16};

constant unsigned int cfg_pooling_size[] = {1, 2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1};

#include "PE.cl"

#include "feeder_IN.cl"

#include "feeder_W.cl"

#include "loader_IN.cl"

#include "loader_W.cl"

#include "drain_OUT.cl"

#include "postproc.cl"

