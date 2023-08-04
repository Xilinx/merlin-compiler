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
]

def test_build_map_of_src_location(make_hiertopo_from_json):
    hier_data = load_json("input_set/test_get_location_recursive.json")
    hiertopo = make_hiertopo_from_json(hier_data)
#    hiertopo.build_map_of_src_location()
    assert hiertopo.get_src_location("X_0_0_0") == ""
    assert hiertopo.get_src_location("F_0_0_0_47") == "\'aes256_encrypt_ecb_kernel\' (aes.cpp:175)"
    assert hiertopo.get_src_location("X_0_0_0_47_0_0") == "\'key\' (aes.cpp:175)"
    assert hiertopo.get_src_location("X_0_0_0_47_2_0_2_3_1_3_1_0_1_0") == ""
    assert hiertopo.get_src_location("F_0_0_0_44") == ""
    assert hiertopo.get_src_location("L_0_0_0_47_2_0_2") == "loop \'i1\' (aes.cpp:180)"
    assert hiertopo.get_src_location("L_0_0_0_47_2_0_2_3_1") == "loop "
    assert hiertopo.get_src_location("F_0_0_0_45") == "\'aes256_encrypt_ecb\' (aes.cpp:140)"
    assert hiertopo.get_src_location("X_0_0_0_47_2_0_2_3_1_3_1_0") == ""
    assert hiertopo.get_src_location("X_0_0_0_49") == ""
    print(hiertopo.src_topo_id_set)
    assert hiertopo.src_topo_id_set == ['F_0_0_0_47', 'X_0_0_0_47_0_0', 'X_0_0_0_49_2', 'L_0_0_0_47_2_0_2', 'L_0_0_0_47_2_0_2_3_1', 'F_0_0_0_45']
    assert hiertopo.src_topo_id_map == {'0_0_0_44': 'F_0_0_0'}
