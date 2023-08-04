#(C) Copyright 2016-2021 Xilinx, Inc.
#All Rights Reserved.
#
#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License. (edited)
import json
from collections import OrderedDict
from common.fixtures import *

def load_json(json_file, mode="default"): 
    data_set = {}
    with open(json_file, 'r') as fileh:
        if mode == "default":
            data_set = json.load(fileh, object_pairs_hook=OrderedDict)
        return data_set
    return data_set

cases = [
#0
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_1 (name:loop1, org_identifier:L_0_3)
    L_0_2 (name:loop2, org_identifier:L_0_3)
    L_0_3 (name:loop3, org_identifier:L_0_4)
"""
),

#1
(
# merlin output code
"""
F_0 (name:foo)
    L_0_1 (name:loop1, trip-count:10)
    L_0_2 (name:loop2, trip-count:30)
    L_0_3 (name:loop3, trip-count:20)
"""
),

#2
(
# merlin output code
"""
F_0 (name:foo)
    X_0_4 (name:a, messages: "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9994 for 139860643716640\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)key_buf_0[i1_sub - 0L],(const void *)(&key[0]),sizeof(unsigned ch...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionParameterList:unsigned char [32] dst,int dst_idx_1,unsigned char [32] src,int src_idx_0,unsign...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_2_0_6_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_2_0_6_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_2_0_6_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_2_0_5_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_18_2_0_5_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_18_2_0_5_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_16_2_0_4_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_14_2_0_3_0_3_1_0\\\",\\n    \\\"passname\\\": \\\"id_update\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359344\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"burst\\\": \\\"on\\\",\\n    \\\"burst_length_max\\\": \\\"32\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'key'\\\",\\n    \\\"is_read\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)key_buf_0[i1_sub - 0L],(const void *)(&key[0]),sizeof(unsigned ch...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676088\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]")
"""
),


#13
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
        X_0_0_0 (name:merlin_memcpy, org_identifier:X_0_0)
        L_0_0_1 (org_identifier:L_0_0)
        X_0_0_2 (name:bar, org_identifier:X_0_0_0)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
F_2 (name:bar, org_identifier:F_1)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
        X_0_0_0 (name:bar, src_filename:null, src_line:null)
F_1 (name:bar, src_filename:null, src_line:null)
"""
),
]

def test_method_get_src_id(make_topo):
    topo = make_topo(cases[0], cases[0])
    dst_id = "F_0"
    res = topo.get_src_id(dst_id)
    assert res == "F_0"
    dst_id = "F_1"
    res = topo.get_src_id(dst_id)
    assert res == ""
    dst_id = "L_0_1"
    res = topo.get_src_id(dst_id)
    assert res == "L_0_3"

def test_method_get_dst_id(make_topo):
    topo = make_topo(cases[0], cases[0])
    src_id = "F_0"
    res = topo.get_dst_id(src_id)
    assert res == ["F_0"]
    src_id = "F_1"
    res = topo.get_dst_id(src_id)
    assert res == []
    src_id = "L_0_3"
    res = topo.get_dst_id(src_id)
    assert res == ["L_0_1", "L_0_2"]

def test_method_get_dest_nodes(make_topo):
    topo = make_topo(cases[0], cases[0])
    src_node = {"src_topo_id":"F_0"}
    res = topo.get_dest_nodes(src_node)
    assert res == [{'name':'foo', 'org_identifier':'F_0', 'type':'kernel', 'topo_id':'F_0'}]
    src_node = {"src_topo_id":"L_0_3"}
    res = topo.get_dest_nodes(src_node)
    assert res == [{'name':'loop1', 'org_identifier':'L_0_3', 'type':'loop', 'topo_id':'L_0_1'}
                , {'name':'loop2', 'org_identifier':'L_0_3', 'type':'loop', 'topo_id':'L_0_2'}]
    src_node = {"src_topo_id":"L_0_9"}
    res = topo.get_dest_nodes(src_node)
    assert res == []
    
#def test_get_info_from_message_from_node(make_topo):
#    topo = make_topo(cases[0])
#    node = {"a":"b"}
#    res = topo.get_info_from_message_from_node(node, "x")
#    assert res == ""
#    node = {"a":"b",
#        "messages": "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_22_3_1\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_22_3_1\\\",\\n    \\\"passname\\\": \\\"altera_postprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"altera_postprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_18_3_1\\\",\\n    \\\"passname\\\": \\\"loop_parallel\\\"\\n}\",\n    \"{\\n    \\\"fine_grained_parallel\\\": \\\"on\\\",\\n    \\\"fine_grained_parallel_complete\\\": \\\"on\\\",\\n    \\\"fine_grained_parallel_factor\\\": \\\"16\\\",\\n    \\\"passname\\\": \\\"loop_parallel\\\"\\n}\",\n    \"{\\n    \\\"fine_grained_parallel\\\": \\\"on\\\",\\n    \\\"fine_grained_parallel_complete\\\": \\\"on\\\",\\n    \\\"fine_grained_parallel_factor\\\": \\\"16\\\",\\n    \\\"passname\\\": \\\"loop_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"loop_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_18_3_1\\\",\\n    \\\"passname\\\": \\\"reduction_general\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"reduction_general\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_18_3_1\\\",\\n    \\\"passname\\\": \\\"channel_partition\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"channel_partition\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_18_3_1\\\",\\n    \\\"passname\\\": \\\"coarse_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"coarse_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_18_3_1\\\",\\n    \\\"passname\\\": \\\"altera_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"altera_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_12_3_1\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_12_3_1\\\",\\n    \\\"passname\\\": \\\"auto_dim_interchange\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"auto_dim_interchange\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_3_2_0_12_3_1\\\",\\n    \\\"passname\\\": \\\"delinearization\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"delinearization\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_2_2_0_7_3_1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother L_0_0_0_2_2_0_7_3_1\\\",\\n    \\\"passname\\\": \\\"stream_prefetch\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"stream_prefetch\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 12002 for 139830622385032\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_6\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"passname\\\": \\\"midend_preprocess\\\",\\n    \\\"tiling_loop\\\": \\\"yes\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(int j_sub = 0;j_sub < 16;++j_sub) {...}\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_6\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_6\\\",\\n    \\\"passname\\\": \\\"altera_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"altera_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_3\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_3\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_3\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_3\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_3\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"passname\\\": \\\"preprocess\\\",\\n    \\\"src_trip_count\\\": \\\"10000\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother V_0_0_0_2_2_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgForStatement:for(j = 0;j < 10000;j++) {...}\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]",
#    }
#    res = topo.get_info_from_message_from_node(node, "tiling_loop")
#    assert res == "yes"
#    node = {"a":"b",
#        "messages": "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_0_0\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_0_0\\\",\\n    \\\"passname\\\": \\\"function_inline_by_pragma_HLS\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"function_inline_by_pragma_HLS\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_0_0\\\",\\n    \\\"passname\\\": \\\"auto_func_inline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"auto_func_inline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_25_0_0\\\",\\n    \\\"passname\\\": \\\"loop_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"loop_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_18_0_0\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_18_0_0\\\",\\n    \\\"passname\\\": \\\"reduction_general\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"reduction_general\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_16_0_0\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"off\\\",\\n    \\\"memory_coalescing_off_reason\\\": \\\"burst length is less           than threshold (1024)\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"critical_warning\\\": \\\"Memory coalescing NOT inferred: variable 'key' (aes.cpp:175)\\\\n  Reason: burst length is less than threshold (1024 bytes).\\\\n  Hint: reset threshold by using --attribute bus_length_threshold=?\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_14_0_0\\\",\\n    \\\"passname\\\": \\\"coarse_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"coarse_parallel\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_14_0_0\\\",\\n    \\\"passname\\\": \\\"delinearization\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"delinearization\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"burst\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"critical_warning\\\": \\\"Low bandwidth efficiency of memory burst: variable 'key' (aes.cpp:175) in scope (aes.cpp:186).\\\\n  Reason: short cached: size=64 bytes\\\\n\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9947 for 139679009499832\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:key\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
#    }
#    res = topo.get_info_from_message_from_node(node, "critical_warning")
#    assert "NOT inferred" in res
#    assert "Reason" in res
#    assert "Hint" in res

def test_get_value_from_key(make_topo):
    topo = make_topo(cases[0], cases[0])
    node = "F_0"    
    res = topo.get_value_from_key(node, "name")
    assert res == "foo"
    node = "F_1"    
    res = topo.get_value_from_key(node, "name")
    assert res == ""

def test_append_attribute_to_dict(make_topo):
    topo = make_topo(cases[0], cases[0])
    node = {"a":"b"}    
    node_output = {}
    res = topo.append_attribute_to_dict(node, node_output, "a")
    assert node_output == {"a":"b"}
    node = {"a":"b"}    
    node_output = {}
    res = topo.append_attribute_to_dict(node, node_output, "x")
    assert node_output == {"x":""}
    node = {"a":"b"}    
    node_output = {"a":"c"}
    res = topo.append_attribute_to_dict(node, node_output, "a")
    assert node_output == {"a":"c"}

def test_get_src_common_attributes(make_topo):
    topo = make_topo(cases[0], cases[0])
    node = {}
    node["src_line"] = "a"
    node["src_col"] = "b"
    node["src_filename"] = "c"
    node["src_topo_id"] = "L_1"
    node["type"] = "x"
    node["src_func_name"] = "d"
    node["trip-count"] = "e"
    node["name"] = "z"
    res = {}
    res = topo.get_src_common_attributes(node)
    assert res["src_line"] == "a"
    assert res["src_col"] == "b"
    assert res["src_filename"] == "c"
    assert res["src_topo_id"] == "L_1"
    assert res["type"] == "x"
    assert res["src_func_name"] == "d"
    assert res["trip-count"] == "e"
    assert node["name"] == "z"
    node["loop_label"] = "ppp"
    res = topo.get_src_common_attributes(node)
    assert res["name"] == "ppp"


def test_get_trip_count(make_topo):
    topo = make_topo(cases[1], cases[1])
    node = {"a":"b"}    
    node_output = {"c":"d"}    
    dst_list = ["L_0_1"]
    res = topo.get_trip_count(node, node_output, dst_list)
    assert node_output == {"c":"d", "trip-count":"10"}
    node = {"a":"b"}    
    node_output = {"c":"d", "trip-count":"1"}    
    dst_list = ["L_0_1", "L_0_2", "L_0_3"]
    res = topo.get_trip_count(node, node_output, dst_list)
    print(res)
    assert node_output == {"c":"d", "trip-count":"30"}
    node = {"a":"b", "trip-count":"15"}    
    node_output = {"c":"d", "trip-count":"8"}    
    dst_list = ["L_0_1", "L_0_2", "L_0_3"]
    res = topo.get_trip_count(node, node_output, dst_list)
    print(res)
    assert node_output == {"c":"d", "trip-count":"8"}

def test_get_insert_node_info(make_topo_from_json):
    vendor_data = load_json("input_set/test_get_insert_node_info1.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    topo.hier_topo_info.topo_id_name_map = {}
    node = "X_0_1"
    cross_list = []
    res = {}
    res = topo.get_insert_node_info(node, cross_list)
    assert res == {}
    vendor_data = load_json("input_set/test_get_insert_node_info1.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    topo.hier_topo_info.topo_id_name_map = {}
    node = "X_0_4"
    cross_list = []
    res = {}
    res = topo.get_insert_node_info(node, cross_list)
    assert res["name"] == "auto memory burst for \'data\'(read)"
    assert res["comment"] == "cache size=13x072B"
    assert res["total-LUT"] == "-"
    assert res["total-FF"] == "-"
    assert res["total-BRAM"] == "-"
    assert res["total-DSP"] == "-"
    assert res["total-URAM"] == "-"
    assert res["CYCLE_UNIT"] == 100
    vendor_data = load_json("input_set/test_get_insert_node_info2.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    topo.hier_topo_info.topo_id_name_map = {}
    node = "X_0_4"
    cross_list = {"X_0_4": ["L_0_9"]}
    res = {}
    res = topo.get_insert_node_info(node, cross_list)
    assert res["name"] == "auto memory burst for \'data\'(write)"
    assert res["comment"] == "cache size=131072B"
    assert res["total-LUT"] == "-"
    assert res["total-FF"] == "-"
    assert res["total-BRAM"] == "-"
    assert res["total-DSP"] == "-"
    assert res["total-URAM"] == "-"
    vendor_data = load_json("input_set/test_get_insert_node_info3.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    topo.hier_topo_info.topo_id_name_map = {}
    node = "X_0_4"
    cross_list = {"X_0_4": ["L_0_9"]}
    res = {}
    res = topo.get_insert_node_info(node, cross_list)
    assert res["name"] == "auto memory burst for \'data\'(read)"
    assert res["comment"] == "cache size=32768B"
    assert res["total-LUT"] == "-"
    assert res["total-FF"] == "-"
    assert res["total-BRAM"] == "-"
    assert res["total-DSP"] == "-"
    assert res["total-URAM"] == "-"
    assert res["CYCLE_UNIT"] == 25
    vendor_data = load_json("input_set/test_get_insert_node_info4.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    topo.hier_topo_info.topo_id_name_map = {}
    node = "X_0_4"
    cross_list = {"X_0_4": ["L_0_9", "L_0_10"]}
    res = {}
    res = topo.get_insert_node_info(node, cross_list)
    assert res["name"] == "auto memory burst for \'data\'(read)"
    assert res["comment"] == ""
    assert res["total-LUT"] == "-"
    assert res["total-FF"] == "-"
    assert res["total-BRAM"] == "-"
    assert res["total-DSP"] == "-"
    assert res["total-URAM"] == "-"
    assert res["CYCLE_UNIT"] == 3
    vendor_data = load_json("input_set/test_get_insert_node_info5.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    topo.hier_topo_info.topo_id_name_map = {}
    node = "X_0_4"
    cross_list = {"X_0_4": ["L_0_10"]}
    res = {}
    res = topo.get_insert_node_info(node, cross_list)
    assert res["name"] == "auto memory burst for \'data\'(write)"
    assert res["comment"] == "cache size=131072B"
    assert res["total-LUT"] == "-"
    assert res["total-FF"] == "-"
    assert res["total-BRAM"] == "-"
    assert res["total-DSP"] == "-"
    assert res["total-URAM"] == "-"
    assert res["CYCLE_UNIT"] == 320
    vendor_data = load_json("input_set/test_get_insert_node_info6.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    topo.hier_topo_info.topo_id_name_map = {"X_0_0_0_47_0_1": "new_data"}
    node = "X_0_4"
    cross_list = {"X_0_4": ["L_0_10"]}
    res = {}
    res = topo.get_insert_node_info(node, cross_list)
    assert res["name"] == "auto memory burst for \'new_data\'(write)"
    assert res["comment"] == "cache size=13x072/10B"
    assert res["total-LUT"] == "-"
    assert res["total-FF"] == "-"
    assert res["total-BRAM"] == "-"
    assert res["total-DSP"] == "-"
    assert res["total-URAM"] == "-"
    assert res["CYCLE_UNIT"] == 32

def test_get_array_decl_topoid_by_name(make_topo_from_json):
    vendor_data = load_json("input_set/test_get_array_decl_topoid_by_name.json")
    topo = make_topo_from_json(vendor_data, vendor_data)
    res = ""
    var = "aa"
    res = topo.get_array_decl_topoid_by_name(var)
    assert res == "X_0_4"
    var = "bb"
    res = topo.get_array_decl_topoid_by_name(var)
    assert res == "X_0_5"
    var = "cc"
    res = topo.get_array_decl_topoid_by_name(var)
    assert res == ""
    var = "dd"
    res = topo.get_array_decl_topoid_by_name(var)
    assert res == ""
