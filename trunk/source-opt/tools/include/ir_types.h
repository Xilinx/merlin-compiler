// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#pragma once

#include <map>
#include <string>
#include <vector>
#include <unordered_set>
typedef std::map<int, int> index_expr_map;  //  FIXME:  (variable, coefficient)
typedef std::map<int, int>
    range_table_map;  //  FIXME: Table of iteration space, correct only when the
                      //  itartion domain is affine

/*cmost pragma*/
static const char ACCEL_PRAGMA[] = "ACCEL";  //  current version
static const char CMOST_PRAGMA[] = "CMOST";  //  previous version
static const char TLDM_PRAGMA[] = "TLDM";    //  previous previous version
static const char CMOST_TASK[] = "TASK";     //  previous previous version
static const char CMOST_KERNEL[] = "KERNEL";
static const char CMOST_die[] = "DIE";
static const char CMOST_TASK_BEGIN[] = "TASK_BEGIN";
static const char CMOST_TASK_END[] = "TASK_END";
static const char CMOST_TASK_WAIT[] = "TASKWAIT";
static const char CMOST_sync_SUBPRAGMA[] = "ASYNC";
static const char CMOST_PARALLEL[] = "PARALLEL";
static const char CMOST_coalescing[] = "COALESCING";
static const char CMOST_coalescing_width[] = "COALESCING_WIDTH";
//  static const char  CMOST_LOOP_TILING[] = "LOOP_TILING";
static const char CMOST_INLINE[] = "INLINE";
static const char CMOST_LOOP_TILE[] = "TILE";
static const char CMOST_LOOP_TILED_TAG[] = "TILED_TAG";
static const char CMOST_PARTIAL_PARALLEL_TAG[] = "PARTIAL_PARALLEL_TAG";
static const char CMOST_cache[] = "CACHE";
static const char CMOST_LOOP_TILING[] = "TILING";
static const char CMOST_loop_tiling_option[] = "TILING_OPTION";
static const char CMOST_REDUCTION[] = "REDUCTION";
static const char CMOST_LINE_BUFFER[] = "LINE_BUFFER";
static const char CMOST_INTERFACE[] = "INTERFACE";
static const char CMOST_complete[] = "COMPLETE";
static const char CMOST_parallel_option[] = "PARALLEL_OPTION";
static const char CMOST_parallel_factor[] = "FACTOR";
static const char CMOST_parallel_multi_level_factor[] = "PARALLEL_FACTOR";
static const char CMOST_parallel[] = "PARALLEL";
static const char CMOST_PIPELINE[] = "PIPELINE";
static const char CMOST_pipe_parallel_factor[] = "FACTOR";
static const char CMOST_BURST[] = "BURST";
static const char CMOST_bus_bitwidth[] = "BUS_BITWIDTH";
static const char CMOST_FLATTEN[] = "FLATTEN";
static const char CMOST_flatten_option[] = "FLATTEN";
static const char CMOST_factor[] = "FACTOR";
static const char CMOST_factor_low[] = "factor";
static const char CMOST_prior[] = "PRIORITY";
static const char CMOST_ii[] = "II";
static const char CMOST_variable[] = "VARIABLE";
static const char CMOST_variable_low[] = "variable";
static const char CMOST_new_variable[] = "NEW_VARIABLE";
static const char CMOST_name[] = "NAME";
static const char CMOST_api_name[] = "API_NAME";
static const char CMOST_dim[] = "DIM";
static const char CMOST_dim_low[] = "dim";
static const char CMOST_type[] = "TYPE";
static const char CMOST_type_low[] = "type";
static const char CMOST_data_type[] = "DATA_TYPE";
static const char CMOST_depth[] = "DEPTH";
static const char CMOST_max_depth[] = "MAX_DEPTH";
static const char CMOST_migrate_mode[] = "MIGRATE_MODE";
static const char CMOST_bank[] = "BANK";
static const char CMOST_exclusive_option[] = "EXCLUSIVE_OPTION";
static const char CMOST_exclusive[] = "EXCLUSIVE";
static const char CMOST_fifo[] = "FIFO";
static const std::unordered_set<std::string> HLS_GLOBAL_MEM_MODE = {"M_AXI",
                                                                    "AP_BUS"};
static const char CMOST_global_memory[] = "GLOBALMEMORY";
static const char CMOST_bus[] = "M_AXI";
static const char CMOST_interface[] = "INTERFACE";
static const char CMOST_attribute[] = "ATTRIBUTE";
static const char CMOST_systolic[] = "SYSTOLIC";
static const char CMOST_data_reuse[] = "DATA_REUSE";
static const char CMOST_write_only[] = "WRITE_ONLY";
static const char CMOST_scope[] = "SCOPE";
static const char CMOST_scope_type[] = "SCOPE_TYPE";
static const char CMOST_io[] = "IO";
static const char CMOST_SKIP_SYNC[] = "SKIP_SYNC";
static const char CMOST_SKIP[] = "SKIP";
static const char CMOST_OFF[] = "OFF";
static const char CMOST_ON[] = "ON";
static const char CMOST_force[] = "FORCE";
static const char CMOST_off_option[] = "OFF_OPTION";
static const char CMOST_SPAWN[] = "SPAWN";
static const char CMOST_SPAWN_KERNEL[] = "SPAWN_KERNEL";
static const char CMOST_continue[] = "CONTINUE";
static const char CMOST_auto[] = "AUTO";
static const char AP_PRAGMA[] = "AP";  // previous vivado pragma
static const char CMOST_ARR_PARTITION[] = "ARRAY_PARTITION";
static const char CMOST_ARR_PARTITION_low[] = "array_partition";
static const char HLS_PRAGMA[] = "HLS";  // vivado pragma
static const char HLS_ARR_PARTITION[] = "ARRAY_PARTITION";
static const char HLS_RESOURCE[] = "RESOURCE";
static const char HLS_CORE[] = "CORE";
static const char HLS_ARRAY[] = "ARRAY";
static const char HLS_PARTITION[] = "PARTITION";
static const char HLS_TRIPCOUNT[] = "LOOP_TRIPCOUNT";
static const char HLS_LOOP[] = "LOOP";
static const char HLS_TC[] = "TRIPCOUNT";
static const char HLS_MAX[] = "max";
static const char HLS_MIN[] = "min";
static const char HLS_PIPELINE[] = "PIPELINE";
static const char HLS_PIPELINE_low[] = "pipeline";
static const char HLS_PIPELINE_II[] = "II";
static const char HLS_UNROLL[] = "UNROLL";
static const char HLS_UNROLL_low[] = "unroll";
static const char HLS_interface[] = "INTERFACE";
static const char HLS_port[] = "PORT";
static const char HLS_register[] = "REGISTER";
static const char CMOST_copy_in[] = "COPY_IN";
static const char CMOST_copy_out[] = "COPY_OUT";
static const char CMOST_copy[] = "COPY";
static const char CMOST_burst_off[] = "BURST_OFF";
static const char CMOST_explict_bundle[] = "EXPLICIT_BUNDLE";
static const char CMOST_memory_burst[] = "MEMORY_BURST";
static const char CMOST_burst_max_size[] = "MAX_BURST_SIZE";
static const char CMOST_false_dep[] = "FALSE_DEPENDENCE";
static const char CMOST_loop_flatten[] = "LOOP_FLATTEN";
static const char CMOST_stream_prefetch[] = "STREAM_PREFETCH";
static const char CMOST_bundle[] = "BUNDLE";
static const char CMOST_stream[] = "STREAM";
static const char CMOST_scheme[] = "SCHEME";
static const char CMOST_block[] = "block";
static const char CMOST_cyclic[] = "cyclic";
static const char CMOST_reduction_scheme[] = "REDUCTION_SCHEME";
static const char CMOST_wrapper[] = "WRAPPER";
static const char CMOST_register[] = "REGISTER";
static const char CMOST_mode[] = "MODE";
static const char PRAGMA_TYPE_NODE[] = "STREAM_NODE";  // sub kernel
static const char PRAGMA_TYPE_PORT[] =
    "STREAM_PORT";  //  single direction operation FIFO<->Array
static const char PRAGMA_TYPE_CHANNEL[] = "STREAM_CHANNEL";  //  FIFO
static const char PRAGMA_TYPE_BUFFER[] =
    "STREAM_BUFFER";  //  connect two ports via Array
static const char PRAGMA_NODE_ATTRIBUTE[] = "NODE";
static const char PRAGMA_PORT_ATTRIBUTE[] = "PORT";
static const char PRAGMA_SPAWN_ATTRIBUTE[] = "SPAWN";
static const char PRAGMA_SPAWN_KERNEL_ATTRIBUTE[] = "SPAWN_KERNEL";
static const char PRAGMA_SPAWN_FACTOR_ATTRIBUTE[] = "FACTOR";
static const char PRAGMA_PREFETCH_ATTRIBUTE[] = "PREFETCH";
static const char PRAGMA_READ_NODE_ATTRIBUTE[] = "READ_NODE";
static const char PRAGMA_WRITE_NODE_ATTRIBUTE[] = "WRITE_NODE";
static const char PRAGMA_VARIABLE_ATTRIBUTE[] = "VARIABLE";
static const char PRAGMA_PARALLEL_FACTOR_ATTRIBUTE[] = "PARALLEL_FACTOR";
static const char PRAGMA_PARALLEL_ITERATOR_ATTRIBUTE[] = "PARALLEL_ITERATOR";
static const char PRAGMA_DIM_NUM_ATTRIBUTE[] = "DIM_NUM";
static const char PRAGMA_ACCESS_TYPE_ATTRIBUTE[] = "ACCESS_TYPE";
static const char PRAGMA_CHANNEL_DEPTH_ATTRIBUTE[] = "CHANNEL_DEPTH";
static const char PRAGMA_CHANNEL_DEPTH_ATTRIBUTE_DEFAULT[] = "32";
static const char CMOST_streaming_access[] = "streaming_access";
