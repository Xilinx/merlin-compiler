#include "systolic_params.h"
#include "opencl_sdk_version.h"

#define CHANNEL_DEPTH_BOUNDARY_A_B       16
#define __ATTR_CHANNEL_DEPTH_BOUNDARY_A_B__  __attribute__((depth(CHANNEL_DEPTH_BOUNDARY_A_B)))

#define CHANNEL_DEPTH_BOUNDARY_C       1024
#define __ATTR_CHANNEL_DEPTH_BOUNDARY_C__  __attribute__((depth(CHANNEL_DEPTH_BOUNDARY_C)))

#define CHANNEL_DEPTH_INNER_A_B       0
#define __ATTR_CHANNEL_DEPTH_INNER_A_B__ __attribute__((depth(CHANNEL_DEPTH_INNER_A_B)))

#define CHANNEL_DEPTH_INNER_C       0
#define __ATTR_CHANNEL_DEPTH_INNER_C__ __attribute__((depth(CHANNEL_DEPTH_INNER_C)))

#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#pragma OPENCL EXTENSION cl_intel_channels : enable

typedef float data_precision;
//typedef half data_precision;
//typedef char data_precision;

typedef float2 data_precision2;
typedef float4 data_precision4;
typedef float8 data_precision8;
typedef float16 data_precision16;

//typedef half2 data_precision2;
//typedef half4 data_precision4;
//typedef half8 data_precision8;
//typedef half16 data_precision16;

//typedef int2 data_precision2;
//typedef int4 data_precision4;
//typedef int8 data_precision8;
//typedef int16 data_precision16;

//typedef char2 data_precision2;
//typedef char4 data_precision4;
//typedef char8 data_precision8;
//typedef char16 data_precision16;

//#define VECTOR_FLOAT_ZERO   (float)(0.0f)
//#define VECTOR_FLOAT2_ZERO   (float2)(0.0f, 0.0f)
//#define VECTOR_FLOAT4_ZERO   (float4)(0.0f, 0.0f, 0.0f, 0.0f)
//#define VECTOR_FLOAT8_ZERO   (float8)(VECTOR_FLOAT4_ZERO,VECTOR_FLOAT4_ZERO)
//#define VECTOR_FLOAT16_ZERO (float16)(VECTOR_FLOAT8_ZERO,VECTOR_FLOAT8_ZERO)

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
struct __attribute__((packed)) ch_data_a_struct {
#else
struct ch_data_a_struct {
#endif
   // vec_float_t data;
    vec_data_precision_t data;
    char new_row_col_pair;
};

#ifndef EMULATOR  // don't use packed in the emulator
struct __attribute__((packed)) ch_data_o_struct {
#else
struct ch_data_o_struct {
#endif
   // float data;
    data_precision data;
};

#ifndef EMULATOR  // don't use packed in the emulator
struct __attribute__((packed)) ch_data_b_struct {
#else
struct ch_data_b_struct {
#endif
   // vec_float_t data;
    vec_data_precision_t data;
};

channel struct ch_data_a_struct  ch_data_a  [SYS_ARRAY_NUM_ROWS][SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_INNER_A_B__;
channel struct ch_data_b_struct  ch_data_b  [SYS_ARRAY_NUM_ROWS][SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_INNER_A_B__;
channel struct ch_data_o_struct  ch_data_c  [SYS_ARRAY_NUM_ROWS][SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_INNER_C__;

channel struct ch_data_a_struct  ch_data_a_boundary  [SYS_ARRAY_NUM_ROWS] __ATTR_CHANNEL_DEPTH_BOUNDARY_A_B__;
channel struct ch_data_b_struct  ch_data_b_boundary  [SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_BOUNDARY_A_B__;
channel struct ch_data_o_struct  ch_data_o_boundary  [SYS_ARRAY_NUM_COLS] __ATTR_CHANNEL_DEPTH_BOUNDARY_C__;

#ifndef EMULATOR
struct __attribute__((packed)) ch_data_row_feeder_chain_struct {
#else
struct ch_data_row_feeder_chain_struct {
#endif
   // vec_float_t data;
    vec_data_precision_t data;
    char new_row_col_pair;
};

#ifndef EMULATOR
struct __attribute__((packed)) ch_data_col_feeder_chain_struct {
#else
struct ch_data_col_feeder_chain_struct {
#endif
   // vec_float_t data;
    vec_data_precision_t data;
};

channel struct ch_data_row_feeder_chain_struct  ch_data_row_feeder_chain [SYS_ARRAY_NUM_ROWS] __attribute__((depth(0)));
channel struct ch_data_col_feeder_chain_struct  ch_data_col_feeder_chain [SYS_ARRAY_NUM_COLS] __attribute__((depth(0)));

channel struct ch_data_row_feeder_chain_struct  ch_data_row_loader_to_first_feeder  __attribute__((depth(64)));
channel struct ch_data_col_feeder_chain_struct  ch_data_col_loader_to_first_feeder  __attribute__((depth(64)));

//channel int ch_msg_loader_in_ready __attribute__((depth(1)));
//channel int ch_msg_loader_w_ready __attribute__((depth(1)));
//channel int ch_msg_loader_in_ready;
//channel int ch_msg_loader_w_ready;

#include "PE.cl"

#include "feeder_IN.cl"
//#include "feeder_IN_general.cl"

#include "feeder_W.cl"
//#include "feeder_W_general.cl"

#include "loader_IN.cl"

#include "loader_W.cl"

#include "drain_OUT.cl"

