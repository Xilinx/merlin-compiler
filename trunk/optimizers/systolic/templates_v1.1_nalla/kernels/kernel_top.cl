#include "../host/inc/systolic_params.h"

#define CHANNEL_DEPTH_BOUNDARY_A_B       16
#define __ATTR_CHANNEL_DEPTH_BOUNDARY_A_B__  __attribute__((depth(CHANNEL_DEPTH_BOUNDARY_A_B)))

#define CHANNEL_DEPTH_BOUNDARY_C       1024
#define __ATTR_CHANNEL_DEPTH_BOUNDARY_C__  __attribute__((depth(CHANNEL_DEPTH_BOUNDARY_C)))

#define CHANNEL_DEPTH_INNER_A_B       0
#define __ATTR_CHANNEL_DEPTH_INNER_A_B__ __attribute__((depth(CHANNEL_DEPTH_INNER_A_B)))

#define CHANNEL_DEPTH_INNER_C       0
#define __ATTR_CHANNEL_DEPTH_INNER_C__ __attribute__((depth(CHANNEL_DEPTH_INNER_C)))

#pragma OPENCL EXTENSION cl_khr_fp16 : enable

//#define VECTOR_FLOAT_ZERO   (float)(0.0f)
//#define VECTOR_FLOAT2_ZERO   (float2)(0.0f, 0.0f)
//#define VECTOR_FLOAT4_ZERO   (float4)(0.0f, 0.0f, 0.0f, 0.0f)
//#define VECTOR_FLOAT8_ZERO   (float8)(VECTOR_FLOAT4_ZERO,VECTOR_FLOAT4_ZERO)
//#define VECTOR_FLOAT16_ZERO (float16)(VECTOR_FLOAT8_ZERO,VECTOR_FLOAT8_ZERO)

//typedef half data_precision;
//typedef float data_precision;
typedef char data_precision;

//typedef float2 data_precision2;
//typedef float4 data_precision4;
//typedef float8 data_precision8;
//typedef float16 data_precision16;

//typedef short2 data_precision2;
//typedef short4 data_precision4;
//typedef short8 data_precision8;
//typedef short16 data_precision16;

//typedef half2 data_precision2;
//typedef half4 data_precision4;
//typedef half8 data_precision8;
//typedef half16 data_precision16;

typedef char2 data_precision2;
typedef char4 data_precision4;
typedef char8 data_precision8;
typedef char16 data_precision16;

#define VECTOR_TYPE_ZERO   (data_precision)(0)
#define VECTOR_TYPE2_ZERO   (data_precision2)(0, 0)
#define VECTOR_TYPE4_ZERO   (data_precision4)(0, 0, 0, 0)
#define VECTOR_TYPE8_ZERO   (data_precision8)(VECTOR_TYPE4_ZERO,VECTOR_TYPE4_ZERO)
#define VECTOR_TYPE16_ZERO (data_precision16)(VECTOR_TYPE8_ZERO,VECTOR_TYPE8_ZERO)

#if DOT_PROD_VECTOR_SIZE==1
typedef data_precision vec_data_precision_t;
#define VECTOR_ZERO VECTOR_TYPE_ZERO
#elif DOT_PROD_VECTOR_SIZE==2
typedef data_precision2 vec_data_precision_t;
#define VECTOR_ZERO VECTOR_TYPE2_ZERO
#elif DOT_PROD_VECTOR_SIZE==4
typedef data_precision4 vec_data_precision_t;
#define VECTOR_ZERO VECTOR_TYPE4_ZERO
#elif DOT_PROD_VECTOR_SIZE==8
typedef data_precision8 vec_data_precision_t;
#define VECTOR_ZERO VECTOR_TYPE8_ZERO
#elif DOT_PROD_VECTOR_SIZE==16
typedef data_precision16 vec_data_precision_t;
#define VECTOR_ZERO VECTOR_TYPE16_ZERO
#else
#error Unsupported DOT_PROD_VECTOR_SIZE
#endif

#ifndef EMULATOR  // don't use packed in the emulator
__attribute__((packed))
#endif
struct ch_data_a_struct {
    vec_data_precision_t data;
    char new_row_col_pair;
};

#ifndef EMULATOR  // don't use packed in the emulator
__attribute__((packed))
#endif
struct ch_data_o_struct {
  //  float data;
    data_precision data;
};

#ifndef EMULATOR  // don't use packed in the emulator
__attribute__((packed))
#endif
struct ch_data_b_struct {
    vec_data_precision_t data;
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
    vec_data_precision_t data;
    char new_row_col_pair;
};

#ifndef EMULATOR
__attribute__((packed))
#endif
struct ch_data_col_feeder_chain_struct {
    vec_data_precision_t data;
};

channel struct ch_data_row_feeder_chain_struct  ch_data_row_feeder_chain [SYS_ARRAY_NUM_ROWS] __attribute__((depth(0)));
channel struct ch_data_col_feeder_chain_struct  ch_data_col_feeder_chain [SYS_ARRAY_NUM_COLS] __attribute__((depth(0)));

channel struct ch_data_row_feeder_chain_struct  ch_data_row_loader_to_first_feeder  __attribute__((depth(64)));
channel struct ch_data_col_feeder_chain_struct  ch_data_col_loader_to_first_feeder  __attribute__((depth(64)));

channel unsigned char ch_msg_loader_in_ready __attribute__((depth(1)));
channel unsigned char ch_msg_loader_w_ready __attribute__((depth(1)));

#ifndef EMULATOR
__attribute__((packed))
#endif
struct custom_float_array { 
//	float drain_data[SYS_ARRAY_NUM_COLS];
	data_precision drain_data[SYS_ARRAY_NUM_COLS];
};

channel struct custom_float_array ch_drain_chain[SYS_ARRAY_NUM_COLS-1];

#define DRAIN_O_LAST_CHANNEL_DEPTH 1

channel struct custom_float_array ch_drain_O_tree_to_mem __attribute__((depth(DRAIN_O_LAST_CHANNEL_DEPTH)));

channel struct custom_float_array ch_drain_to_postproc __attribute__((depth(1)));
channel struct custom_float_array ch_relu_to_pooling __attribute__((depth(1)));
channel struct custom_float_array ch_write_to_ddr __attribute__((depth(1)));
channel struct custom_float_array ch_zero_padding __attribute__((depth(1)));
channel struct custom_float_array ch_debug_ddr_bandwidth __attribute__((depth(1)));

// configs
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

//#include "postproc.cl"
#include "relu.cl"

#include "pooling.cl"

#include "zero_padding.cl"

#include "write_to_ddr.cl"

#if DEBUG_DDR_BANDWIDTH
#include "zero_padding_debug.cl"

#include "write_to_ddr_debug.cl"
#endif

