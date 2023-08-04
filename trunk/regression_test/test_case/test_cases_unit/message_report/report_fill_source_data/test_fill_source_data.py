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
# to run this test manually: pytest -v report_fill_source_data
# current directory should be the parent directory to this script file

import pdb
import os
import sys
import json
import pytest
import re
sys.path.append(os.getcwd())
from common.fixtures import *

assert os.getenv("MERLIN_COMPILER_HOME") is not None
path = os.path.join(
    os.getenv("MERLIN_COMPILER_HOME"),
    "mars-gen", "scripts", "msg_report", "xilinx_enhance"
)
sys.path.insert(0, path)

from collections import OrderedDict
import merlin_log
import merlin_report_setting
import separate_json_gen_top
import utility
import fill_source_data

def setup_module():
    os.chdir("report_fill_source_data")

def teardown_module():
    os.chdir("..")

def do_compare_result(node, src_topo_id, compare_key, compare_value):
    if 'src_topo_id' in node and node['src_topo_id'] == src_topo_id:
        if compare_key in node:
            return str(node[compare_key]) == str(compare_value)
        else:
            if compare_value == "":
                return True

    sub_keys = ['sub_functions', 'interfaces', 'childs']
    for sub_key in sub_keys:
        if sub_key in node:
            for sub_node in node[sub_key]:
                if do_compare_result(sub_node, src_topo_id, compare_key, compare_value):
                    return True
    return False

def compare_result(json_file, src_topo_id, compare_key, compare_value):
    for kernel in json_file:
        if do_compare_result(kernel, src_topo_id, compare_key, compare_value):
            return True
    return False

# test case 1_1
case_1_1 = (
"""
F_0 (name:kernel)
    L_0_0 (name:loop1)
""",

"""
{
}
"""
)

def test_fill_source_data_1_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_1_1

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')

# test case 1_2
case_1_2 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
""",

"""
{
}
"""
)

def test_fill_source_data_1_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_1_2

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 1_3
case_1_3 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:?)
    L_0_0_1 (name:loop1, org_identifier:L_0_0_1, CYCLE_UNIT:?)
""",

"""
{
}
"""
)

def test_fill_source_data_1_3(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_1_3

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '?')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '?')

# test case 2_1
case_2_1 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
""",

"""
{
}
"""
)

def test_fill_source_data_2_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_1

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 2_2
case_2_2 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
""",

"""
{
}
"""
)

def test_fill_source_data_2_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_2

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 2_3
case_2_3 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, pipelined:yes, II:5, CYCLE_UNIT:1645)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
""",

"""
{
}
"""
)

def test_fill_source_data_2_3(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_3

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '1645')

# test case 2_4
case_2_4 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:16000)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, flatten:yes, flatten-id: L_0_0_1, CYCLE_UNIT:1600)
""",

"""
{
}
"""
)

def test_fill_source_data_2_4(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_4

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 2_5
case_2_5 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        L_0_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
            X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:-)
F_1 (name:func1, inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:-)
    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, CYCLE_UNIT:800)
        L_0_1_2_2_1_3 (name:loop4, trip-count:40, org_identifier:L_0_0_1, unrolled:yes, unroll-factor:10, CYCLE_UNIT:4)
""",

"""
{
}
"""
)

def test_fill_source_data_2_5(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_5

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 2_6
case_2_6 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        L_0_0_1_2 (name:loop2, trip-count:8, unrolled:yes, unroll-factor:8, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
""",

"""
{
}
"""
)

def test_fill_source_data_2_6(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_6

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 2_7
case_2_7 = (
"""
X_0_0 (name:a, var_type:array, org_identifier:X_0_0)
X_0_1 (name:a, var_type:array, org_identifier:X_0_1)
X_0_2 (name:a, var_type:scalar, org_identifier:X_0_2)
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
""",

"""
{
}
"""
)

def test_fill_source_data_2_7(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_7

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'X_0_1', 'type', 'interface')

# test case 2_8
case_2_8 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        L_0_0_1_2 (name:loop2, org_identifier:L_0_0_1_2, CYCLE_UNIT:1600, parent_loop:L_0_0_1_1)
""",

"""
{
}
"""
)
def test_fill_source_data_2_8(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_8

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    vendor_data["L_0_0_1"]["messages"] = "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_write\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    vendor_data["L_0_0_1_2"]["messages"] = "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_write\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')
    assert compare_result(json_result, 'L_0_0_1_2', 'flatten', 'yes')
    assert fillSourceData.hier_topo_info.merlin_flatten_parent == ['L_0_0_1_1']

# test case 2_9
case_2_9 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, pipelined:no, CYCLE_UNIT:20000)
""",

"""
{
}
"""
)

def test_fill_source_data_2_9(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_9

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    vendor_data["L_0_0_1"]["II_violation"] = "{u'dadd': {u'gmem': u'xxx'}, u'reason': u'yyy', u'solution': u'zzz'}"
    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '20000')
    assert compare_result(json_result, 'L_0_0_1', 'table_note', 'yyy because of the access on kernel bus\nzzz')

# test case 2_10
case_2_10 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        L_0_0_1_2 (name:loop2, org_identifier:L_0_0_1_2, CYCLE_UNIT:1600, parent_loop:L_0_0_1_1)
""",

"""
{
}
"""
)
def test_fill_source_data_2_10(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_2_10

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    vendor_data["L_0_0_1"]["messages"] = "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_write\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    vendor_data["L_0_0_1_2"]["messages"] = "[\n    \"{\\n    \\\"node_priority\\\": \\\"minor\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_write\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')
    assert compare_result(json_result, 'L_0_0_1_2', 'flatten', '')

# test case 3_1
case_3_1 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, pipelined:yes, II:5, CYCLE_UNIT:1645)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, trip-count:8, unrolled:yes, unroll-factor:8, CYCLE_UNIT:1600)
""",

"""
{
}
"""
)

def test_fill_source_data_3_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_3_1

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '1645')

# test case 3_2
case_3_2 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:1645)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, trip-count:8, flatten:yes, flatten-id: L_0_0_1, CYCLE_UNIT:1600)
            L_0_1_2_1 (name:loop2, org_identifier:L_0_0_1, trip-count:8,  unrolled:yes, unroll-factor:5, CYCLE_UNIT:200)
""",

"""
{
}
"""
)

def test_fill_source_data_3_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_3_2

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '1645')

# test case 3_3
#case_3_3 = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
#    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
#        L_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
#            X_0_1_2_2(name:func1, inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:-)
#F_1(name:func1,, inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:-)
#    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, , CYCLE_UNIT:800)
#        L_0_1_2_2_1_3(name:loop4, trip-count:40, org_identifier:L_0_0_1, CYCLE_UNIT:4)
#""",
#
#"""
#{
#}
#"""
#)
#
#def test_fill_source_data_3_3(logger, settings, make_hierarchy_data,
#        make_vendor_data, make_topo_info_data, make_schedule):
#    case = case_3_3
#
#    hier_topo = make_hierarchy_data(case[0])
#    vendor_data = make_vendor_data(case[0])
#    topo_info_data = make_topo_info_data(case[0])
#    adb_data = json.loads(case[1])
#
#    fillSourceData = fill_source_data.FillSourceData(logger, settings,
#            hier_topo, vendor_data, topo_info_data, adb_data, False)
#    fillSourceData.fill_source_data()
#    with open('fill_source_data.json', 'r') as f:
#        json_result = json.load(f)
#    assert compare_result(json_result, 'F_0', 'type', 'kernel')
#    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 3_4
case_3_4 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, unrolled:yes, unroll-factor:2, CYCLE_UNIT:8000)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, trip-count:8, CYCLE_UNIT:1600)
            L_0_1_2_1 (name:loop2, org_identifier:L_0_0_1, trip-count:8,  unrolled:yes, unroll-factor:5, CYCLE_UNIT:200)
""",

"""
{
}
"""
)

def test_fill_source_data_3_4(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_3_4

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '8000')

# test case 3_5
case_3_5 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, unrolled:yes, unroll-factor:2, CYCLE_UNIT:8000)
        L_0_1_2_1 (name:loop2, org_identifier:L_0_0_1, trip-count:?,  unrolled:yes, unroll-factor:5, CYCLE_UNIT:200)
    L_0_0_2 (name:loop2, org_identifier:L_0_0_1, trip-count:?, CYCLE_UNIT:?)
""",

"""
{
}
"""
)

def test_fill_source_data_3_5(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_3_5

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '?')

# test case 4_1
case_4_1 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
    L_0_0_2 (name:loop2, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
        X_0_0_2_2_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
F_1 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, , CYCLE_UNIT:800)
F_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
""",

"""
{
    "kernel":{
        "1":"func1 func2"
    }
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:func)
"""
)

def test_fill_source_data_4_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_1

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 4_1_1
case_4_1_1 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
        L_0_0_1_1 (name:loop3, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
    L_0_0_2 (name:loop2, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
        X_0_0_2_2_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
F_1 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, , CYCLE_UNIT:800)
F_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
""",

"""
{
    "kernel":{
        "1":"func1 func2"
    }
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:func)
"""
)

def test_fill_source_data_4_1_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_1_1

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')


# test case 4_1_2
case_4_1_2 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        X_0_0_1_2_3 (name:func3, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
        X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
        L_0_0_1_1 (name:loop3, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
    L_0_0_2 (name:loop2, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
        X_0_0_2_2_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
F_1 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, , CYCLE_UNIT:800)
F_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
F_3 (name:func3, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
""",

"""
{
    "kernel":{
        "1":"func1 func2"
    }
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:func)
"""
)

def test_fill_source_data_4_1_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_1_2

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')



# test case 4_1_3
case_4_1_3 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
        L_0_0_1_1 (name:loop3, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
            X_0_0_1_2_3 (name:func3, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
    L_0_0_2 (name:loop2, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
        X_0_0_2_2_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
F_1 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, , CYCLE_UNIT:800)
F_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
F_3 (name:func3, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
""",

"""
{
    "kernel":{
        "1":"func1 func2"
    }
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:func)
"""
)

def test_fill_source_data_4_1_3(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_1_3

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

# test case 4_1_4
case_4_1_4 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        X_0_0_1_2_2 (name:func1, inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:-)
    L_0_0_2 (name:loop2, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
        X_0_0_2_2_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
F_1 (name:func1,inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:-)
    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, , CYCLE_UNIT:800)
        X_0_0_1_2_2_2_2 (name:func3, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
F_2 (name:func2, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
F_3 (name:func3, org_identifier:L_0_0_1, CYCLE_UNIT:600)
    L_2_2_4 (name:loop4, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
""",

"""
{
    "kernel":{
        "1":"func3 func2"
    }
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:func)
"""
)

def test_fill_source_data_4_1_4(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_1_4

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')


# test case 4_1_5

def test_fill_source_data_4_1_5(logger, settings):
    with open("./call_function_json/hierarchy.json") as f:
        hier_topo = json.load(f)
    with open("./call_function_json/perf_est.json") as f:
        vendor_data = json.load(f)
    with open("./call_function_json/topo_info.json") as f:
        topo_info_data = json.load(f)
    with open("./call_function_json/adb.json") as f:
        adb_data = json.load(f)
    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_0_0_2_2_0_4_3_4_0_1', 'unroll-factor', '25')
# test case 4_2
case_4_2 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    X_0_0_1 (name:func1, org_identifier:F_0_0_1, CYCLE_UNIT:16000, function_id:F_1)
    X_0_0_2 (name:func2, org_identifier:F_0_0_1, CYCLE_UNIT:3000, function_id:F_2)
F_1 (name:func1, org_identifier:F_0_0_1)
F_2 (name:func2, org_identifier:F_0_0_1)
""",

"""
{
    "kernel":{
        "1":"func1",
        "2":"func2"
    }
}
""",
"""
F_0 (name:kernel)
    F_0_0_1 (name:func)
"""
)

def test_fill_source_data_4_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_2

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0_0_1', 'CYCLE_UNIT', '19000')

## test case 4_3
#case_4_3 = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:16000)
#    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, pipelined:yes, II:5, CYCLE_UNIT:16000)
#        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, trip-count:8, unrolled:yes, unroll-factor:8, CYCLE_UNIT:1600)
#    L_0_0_2 (name:loop1, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
#""",
#
#"""
#{
#}
#"""
#)
#
#def test_fill_source_data_4_3(logger, settings, make_hierarchy_data,
#        make_vendor_data, make_topo_info_data, make_schedule):
#    case = case_4_3
#
#    hier_topo = make_hierarchy_data(case[0])
#    vendor_data = make_vendor_data(case[0])
#    topo_info_data = make_topo_info_data(case[0])
#    adb_data = json.loads(case[1])
#
#    fillSourceData = fill_source_data.FillSourceData(logger, settings,
#            hier_topo, vendor_data, topo_info_data, adb_data, False)
#    fillSourceData.fill_source_data()
#    with open('fill_source_data.json', 'r') as f:
#        json_result = json.load(f)
#    assert compare_result(json_result, 'F_0', 'type', 'kernel')
#    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '19000')

## test case 4_4
case_4_4 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:16000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, pipelined:yes, II:5, CYCLE_UNIT:16000)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, trip-count:8, unrolled:yes, unroll-factor:8, CYCLE_UNIT:1600)
    L_0_0_2 (name:loop1, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
""",

"""
{
}
"""
)

def test_fill_source_data_4_4(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_4

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '19000')

## test case 4_5
#case_4_5 = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
#    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
#        L_0_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
#            X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:800)
#""",
#
#"""
#{
#}
#"""
#)
#
#def test_fill_source_data_4_5(logger, settings, make_hierarchy_data,
#        make_vendor_data, make_topo_info_data, make_schedule):
#    case = case_4_5
#
#    hier_topo = make_hierarchy_data(case[0])
#    vendor_data = make_vendor_data(case[0])
#    topo_info_data = make_topo_info_data(case[0])
#    adb_data = json.loads(case[1])
#
#    fillSourceData = fill_source_data.FillSourceData(logger, settings,
#            hier_topo, vendor_data, topo_info_data, adb_data, False)
#    fillSourceData.fill_source_data()
#    with open('fill_source_data.json', 'r') as f:
#        json_result = json.load(f)
#    assert compare_result(json_result, 'F_0', 'type', 'kernel')

# test case 4_6
case_4_6 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:4645)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:1645)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_1, trip-count:8, flatten:yes, flatten-id: L_0_0_1, CYCLE_UNIT:1600)
            L_0_1_2_1 (name:loop2, org_identifier:L_0_0_1, trip-count:8,  unrolled:yes, unroll-factor:5, CYCLE_UNIT:200)
    L_0_0_2 (name:loop1, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
""",

"""
{
}
"""
)

def test_fill_source_data_4_6(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_6

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '4645')

## test case 4_7
#case_4_7 = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
#    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
#        L_0_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
#            X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:800)
#""",
#
#"""
#{
#}
#"""
#)
#
#def test_fill_source_data_4_7(logger, settings, make_hierarchy_data,
#        make_vendor_data, make_topo_info_data, make_schedule):
#    case = case_4_7
#
#    hier_topo = make_hierarchy_data(case[0])
#    vendor_data = make_vendor_data(case[0])
#    topo_info_data = make_topo_info_data(case[0])
#    adb_data = json.loads(case[1])
#
#    fillSourceData = fill_source_data.FillSourceData(logger, settings,
#            hier_topo, vendor_data, topo_info_data, adb_data, False)
#    fillSourceData.fill_source_data()
#    with open('fill_source_data.json', 'r') as f:
#        json_result = json.load(f)
#    assert compare_result(json_result, 'F_0', 'type', 'kernel')

# test case aes (4_8)
def test_fill_source_data_aes(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    hierarchy_file = "aes_data/hierarchy.json"
    vendor_file = "aes_data/perf_est.json"
    topo_info_file = "aes_data/topo_info.json"
    adb_file = "aes_data/adb.json"

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hierarchy_file, vendor_file, topo_info_file, adb_file, True)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, "F_0_0_0_2", 'type', 'kernel')

# test case aes (4_8)
def test_fill_source_data_aes_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    hierarchy_file = "aes_data_2/hierarchy.json"
    vendor_file = "aes_data_2/perf_est.json"
    topo_info_file = "aes_data_2/topo_info.json"
    adb_file = "aes_data_2/adb.json"

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hierarchy_file, vendor_file, topo_info_file, adb_file, True)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, "F_0_0_0_2", 'type', 'kernel')

# test case 4_9
case_4_9 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1_1, CYCLE_UNIT:1600)
#    L_0_0_2 (name:loop2, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
        X_0_0_1_3_2 (name:func2, org_identifier:L_0_0_1_1, CYCLE_UNIT:600)
F_1 (name:func1, org_identifier:L_0_0_1_1, CYCLE_UNIT:1600)
    L_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1_1, CYCLE_UNIT:800)
F_2 (name:func2, org_identifier:L_0_0_1_1, CYCLE_UNIT:600)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:ignore)
""",

"""
{
    "kernel":{
        "1":"func1 func2"
    }
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:func)
        L_0_0_1_1 (name:sub_loop)
"""
)

def test_fill_source_data_4_9(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_4_9

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'L_0_0_1_1', 'CYCLE_UNIT', '1600')

## test case 5_1
#case_5_1 = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
#    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
#        L_0_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
#            X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:800)
#""",
#
#"""
#{
#}
#"""
#)
#
#def test_fill_source_data_5_1(logger, settings, make_hierarchy_data,
#        make_vendor_data, make_topo_info_data, make_schedule):
#    case = case_5_1
#
#    hier_topo = make_hierarchy_data(case[0])
#    vendor_data = make_vendor_data(case[0])
#    topo_info_data = make_topo_info_data(case[0])
#    adb_data = json.loads(case[1])
#
#    fillSourceData = fill_source_data.FillSourceData(logger, settings,
#            hier_topo, vendor_data, topo_info_data, adb_data, False)
#    fillSourceData.fill_source_data()
#    with open('fill_source_data.json', 'r') as f:
#        json_result = json.load(f)
#    assert compare_result(json_result, 'F_0', 'type', 'kernel')

# test case 5_2
case_5_2 = (
"""
X_0_0 (name:a, var_type:array, org_identifier:X_0_0)
X_0_1 (name:a, var_type:array, org_identifier:X_0_1)
X_0_2 (name:a, var_type:scalar, org_identifier:X_0_2)
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        L_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
            X_0_1_2_2(name:func1, inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:-)
    L_0_0_2 (name:loop1, trip-count:5, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:3000)
        X_0_2_2_2(name:func1, inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:0)
    X_0_0_7(name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:15200)
F_1(name:func1,, inline:yes, org_identifier:L_0_0_1, CYCLE_UNIT:-)
    L_0_1_2_2_1 (name:loop3, trip-count:200, org_identifier:L_0_0_1, CYCLE_UNIT:800)
        L_0_1_2_2_1_3(name:loop4, trip-count:40, org_identifier:L_0_0_1, unrolled:yes, unroll-factor:10, CYCLE_UNIT:4)
F_2(name:func1,, inline:yes, CYCLE_UNIT:-)
    L_2_2_3 (name:loop1, trip-count:20, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_0_2, CYCLE_UNIT:600)
        L_2_2_3_1(name:loop1, trip-count:10, org_identifier:L_0_0_1, flatten:yes, flatten-id:L_2_2_3, CYCLE_UNIT:30)
F_3(name:func1, CYCLE_UNIT:15200)
    L_3_2 (name:loop1, trip-count:10, org_identifier:L_0_0_1, , CYCLE_UNIT:15100)
        L_3_2_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:15100)
            L_3_2_1_1 (name:loop1, trip-count:30, org_identifier:L_0_0_1, flatten:yes, CYCLE_UNIT:1510)
                L_3_2_1_1_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, flatten:yes, flatten-id: L_3_2_1_1, pipelined:yes, II:t, CYCLE_UNIT:1510)
                    L_3_2_1_1_1_2 (name:loop1, trip-count:8, org_identifier:L_0_0_1, unrolled:yes, unroll-factor:t, CYCLE_UNIT:-)
""",

"""
{
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:func)
"""
)

def test_fill_source_data_5_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_5_2

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')

## test case 5_3
#case_5_3 = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
#    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
#        L_0_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
#            X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:800)
#""",
#
#"""
#{
#}
#"""
#)
#
#def test_fill_source_data_5_3(logger, settings, make_hierarchy_data,
#        make_vendor_data, make_topo_info_data, make_schedule):
#    case = case_5_3
#
#    hier_topo = make_hierarchy_data(case[0])
#    vendor_data = make_vendor_data(case[0])
#    topo_info_data = make_topo_info_data(case[0])
#    adb_data = json.loads(case[1])
#
#    fillSourceData = fill_source_data.FillSourceData(logger, settings,
#            hier_topo, vendor_data, topo_info_data, adb_data, False)
#    fillSourceData.fill_source_data()
#    with open('fill_source_data.json', 'r') as f:
#        json_result = json.load(f)
#    assert compare_result(json_result, 'F_0', 'type', 'kernel')
#
## test case 5_4
#case_5_4 = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
#    L_0_0_1 (name:loop1, trip-count:10, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
#        L_0_0_1_2 (name:loop2, trip-count:2, org_identifier:L_0_0_1, CYCLE_UNIT:1600)
#            X_0_0_1_2_2 (name:func1, org_identifier:L_0_0_1, CYCLE_UNIT:800)
#""",
#
#"""
#{
#}
#"""
#)
#
#def test_fill_source_data_5_4(logger, settings, make_hierarchy_data,
#        make_vendor_data, make_topo_info_data, make_schedule):
#    case = case_5_4
#
#    hier_topo = make_hierarchy_data(case[0])
#    vendor_data = make_vendor_data(case[0])
#    topo_info_data = make_topo_info_data(case[0])
#    adb_data = json.loads(case[1])
#
#    fillSourceData = fill_source_data.FillSourceData(logger, settings,
#            hier_topo, vendor_data, topo_info_data, adb_data, False)
#    fillSourceData.fill_source_data()
#    with open('fill_source_data.json', 'r') as f:
#        json_result = json.load(f)
#    assert compare_result(json_result, 'F_0', 'type', 'kernel')
#
#case_1_xxxx = (
#"""
#F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
#    L_0_0_1 (name:loop1, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
#F_0 (name:kernel)
#    L_0_0 (name:loop1)
##F_1 (name:bar)
##    L_1_0 (name:a)
##        X_1_0_0 (name:foobar)
##    X_1_1 (name:foobar)
##F_2 (name:foobar)
#""",
#
#"""
#{
#    "foo":{
#        "2":"bar"
#    },
#    "bar":{
#        "10":"foo foobar"
#    },
#    "foobar":{
#    }
#}
#"""
#)

# test case 6_1 (test callfunction)
case_6_1 = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    X_0_0_1 (name:func1, org_identifier:X_0_0_1, CYCLE_UNIT:16000, function_id:F_1)
F_1 (name:func1, org_identifier:X_0_0_1)
""",

"""
{
    "kernel":{
        "1":"func1",
        "2":"func2"
    }
}
""",
"""
F_0 (name:kernel)
    X_0_0_1 (name:func)
"""
)

def test_fill_source_data_6_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_6_1

    hier_topo = make_hierarchy_data(case[2])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'X_0_0_1', 'CYCLE_UNIT', '16000')

# test case exception
case_exception = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:ignore)
    L_0_0_1 (name:loop1, trip-count:a, org_identifier:L_0_0_1, pipelined:yes, II:5, CYCLE_UNIT:0, unrolled:yes, unroll-factor:abc)
    L_0_0_2 (name:loop1, trip-count:10, org_identifier:L_0_0_1, pipelined:yes, CYCLE_UNIT:16000)
    X_0_0_1 (name:func1, org_identifier:X_0_0_1, CYCLE_UNIT:16000, function_id:F_1)
F_1 (name:func1)
""",

"""
{
}
""",
"""
F_0 (name:kernel)
    L_0_0_1 (name:loop1)
        F_0_1 (name:no_exist_test)
F_ (name:except)
"""
)

def test_fill_source_data_exception(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_exception

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    vendor_data["L_0_0_1"]["II_violation"] = "{u'dadd': {u'aa': u'xxx'}, u'reason': u'yyy', u'solution': u'zzz'}"
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')

# test case exception 1
case_exception_1 = (
"""
F_0 (type:kernel, org_identifier:F_0, CYCLE_UNIT:ignore)
""",

"""
{
}
""",
"""
F_0 (name:kernel, type:kernel)
"""
)

def test_fill_source_data_exception_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_exception_1

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    assert not fillSourceData.fill_function_loop_info(adb_data)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)

# test case memory burst no siblings
case_memory_burst_no_siblings = (
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
    L_0_1 (name:merlin_loop, org_identifier:L_0_0)
        X_0_1_1 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
        L_0_1_2 (name:merlin_loop, org_identifier:L_0_0)
            X_0_1_2_4 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
            L_0_1_2_5 (name:merlin_loop, org_identifier:L_0_0)
                X_0_1_2_5_6 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
                L_0_1_2_5_7 (name:merlin_loop, org_identifier:L_0_0_1)
                    X_0_1_2_5_7_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
                    X_0_1_2_5_7_1 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
                X_0_1_2_5_8 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
            X_0_1_2_6 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
        X_0_1_3 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
    X_0_2 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
        L_0_0_1 (name:null, src_filename:null, src_line:null)
""",
"""
{
}
"""
)

def test_fill_source_data_memory_burst_no_siblings(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_memory_burst_no_siblings

    hier_topo = make_hierarchy_data(case[1])
    vendor_data = make_vendor_data(case[0])
    vendor_data["X_0_1_2_5_7_0"]["messages"] = "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_read\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    vendor_data["X_0_1_2_5_7_1"]["messages"] = "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_write\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[2])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    assert not fillSourceData.fill_function_loop_info(adb_data)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)

# test case 9_1
case_9_1 = (
"""
F_0 (name:kernel)
    L_0_0 (name:loop1)
""",

"""
{
}
"""
)

def test_get_all_first_node(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_9_1

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    input = [[u'F_0_0_0_5', [u'F_0_0_0_5_1', u'F_0_0_0_5_2']], [u'F_0_0_0_6'], [u'F_0_0_0_7']]
    res = fillSourceData.get_all_first_node(input)
    assert res == [u'F_0_0_0_5', u'F_0_0_0_5_1']
    input = [[u'F_0_0_0_5']]
    res = fillSourceData.get_all_first_node(input)
    assert res == [u'F_0_0_0_5']
    input = [[u'F_0_0_0_5']]
    res = fillSourceData.get_all_first_node(input)
    assert res == [u'F_0_0_0_5']
    input = {}
    res = fillSourceData.get_all_first_node(input)
    assert res == []



# test case dead code
case_dead_code = (
"""
F_0 (name:kernel, org_identifier:F_0, CYCLE_UNIT:50000)
    L_0_0_1 (name:loop1, org_identifier:L_0_0_1, CYCLE_UNIT:16000)
        X_0_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
        L_0_1_2 (name:loop2, org_identifier:L_0_0_2, CYCLE_UNIT:16000)
            L_0_2_2 (name:loop3, org_identifier:L_0_0_3, CYCLE_UNIT:16000)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

"""
{
}
"""
)

def test_fill_source_data_dead_code_1(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_dead_code

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])
    vendor_data["L_0_0_1"]["dead_code"] = "is_dead_code"
    hier_topo[0]["childs"][0]["childs"][0]["parent_loop"] = "L_0_0_1"
    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '-')

def test_fill_source_data_dead_code_2(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_dead_code

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])
    vendor_data["L_0_0_1"]["dead_code"] = "is_dead_code"
    vendor_data["L_0_0_1"]["flatten"] = "flattened"

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'type', 'kernel')
    assert compare_result(json_result, 'F_0', 'CYCLE_UNIT', '50000')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '16000')

case_memory_burst_with_dead_code = (
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_1 (name:merlin_loop, org_identifier:L_0_0)
        X_0_1_2_5_6 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
        L_0_1_2_5_7 (name:merlin_loop, org_identifier:L_0_0_1)
        X_0_1_2_5_7_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
        L_0_0_1 (name:null, src_filename:null, src_line:null, parent_loop:L_0_0)
""",
"""
{
}
"""
)

def test_fill_source_data_memory_burst_with_dead_code(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_memory_burst_with_dead_code

    hier_topo = make_hierarchy_data(case[1])
    vendor_data = make_vendor_data(case[0])
    #vendor_data["X_0_1_2_5_7_0"]["messages"] = "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_read\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    #vendor_data["X_0_1_2_5_7_1"]["messages"] = "[\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"final_code_gen\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"coarse_pipeline\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140198154781904\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),data_buf_0[0][0],sizeof(unsigned char ) * 13...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"memory_coalescing\\\": \\\"on\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy_wide_bus_write_char_3d_4096_16_128((merlin_uint_128 *)data,(char(*)[4096]...\\\",\\n    \\\"passname\\\": \\\"bitwidth_opt\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 10613 for 140228746359568\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"flatten_loop\\\": \\\"on\\\",\\n    \\\"burst_length\\\": \\\"131072\\\",\\n    \\\"display_name\\\": \\\"auto memory burst for 'data'\\\",\\n    \\\"is_write\\\": \\\"1\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgFunctionCallExp:memcpy((void *)(&data[i1 * 131072]),(const void *)data_buf_0[0],sizeof(unsigned ...\\\",\\n    \\\"passname\\\": \\\"memory_burst\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"midend_preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_13_0_0\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"line_buffer\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"interface_transform\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"postwrap_process\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"kernel_wrapper\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_11_0_0\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"preprocess\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"Build in 9893 for 140628143676416\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"cached when calculate its brother X_0_0_0_47_0_0\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"copy message from bind node\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\",\n    \"{\\n    \\\"DebugInfo\\\": \\\"SgInitializedName:data\\\",\\n    \\\"passname\\\": \\\"lower_separate\\\"\\n}\"\n]"
    vendor_data["L_0_1_2_5_7"]["dead_code"] = "is_dead_code"
    vendor_data["L_0_1"]["dead_code"] = "is_dead_code"
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[2])

    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.fill_source_data()
    with open('fill_source_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_0', 'CYCLE_UNIT', '-')
    assert compare_result(json_result, 'L_0_0_1', 'CYCLE_UNIT', '-')
    assert json_result[0]["sub_functions"] == []





