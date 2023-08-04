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
    if 'src_topo_id' in node and compare_key in node and node['src_topo_id'] == src_topo_id:
        return str(node[compare_key]) == str(compare_value)
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
F_0 (org_identifier:F_1_0)
    L_0_1 (org_identifier:L_1_1)
    X_0_2 (org_identifier:X_1_2)
    X_0_3 (org_identifier:V_1_3)
""",

"""
{
}
"""
)

def test_preprocess(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_1_1

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])
    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    id_list = ["X_1_1", "L_1_2"]
    src_topo_id_map = {}
    src_topo_id_map["1_3"] = "V_1_3_5"
    res = fillSourceData.preprocess(vendor_data, id_list, src_topo_id_map)
    assert res == {'F_0': {'org_identifier': 'F_1_0', 'type': 'kernel'},
                   'L_0_1': {'org_identifier': 'X_1_1', 'type': 'loop'},
                   'X_0_2': {'org_identifier': 'L_1_2', 'type': 'callfunction'},
                   'X_0_3': {'org_identifier': 'V_1_3_5', 'type': 'callfunction'}} 

# test case 1_2
case_1_2 = (
"""
F_0 (org_identifier:F_1_0)
    L_0_1 (org_identifier:L_1_1)
        L_0_2 (org_identifier:X_1_2)
    X_0_3 (org_identifier:V_1_3)
""",

"""
{
}
"""
)

def test_postprocess(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_1_2

    hier_topo = make_hierarchy_data(case[0])
    vendor_data = make_vendor_data(case[0])
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])
    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.hier_topo_info.merlin_flatten_parent = ["L_0_1"]
    res = fillSourceData.post_process_for_output_data(hier_topo)
#    dump_to_json("ref_postprocess.json", res)
    ref = load_json("ref_postprocess.json") 
    assert res == ref 

# test case 1_3
case_1_3 = (
"""
F_0 (org_identifier:F_1_0)
    L_0_1 (org_identifier:L_1_1)
        L_0_2 (org_identifier:X_1_2, src_line:18, name:test, src_filename:test.cpp)
    X_0_3 (org_identifier:V_1_3)
""",

"""
{
}
"""
)

def test_get_note_info_for_ii(logger, settings, make_hierarchy_data,
        make_vendor_data, make_topo_info_data, make_schedule):
    case = case_1_3

    #vendor_data = make_vendor_data(case[0])
    vendor_data = load_json("test_ii.json")
    #topo = make_topo_from_json(vendor_data)
    hier_topo = make_hierarchy_data(case[0])
    dump_to_json("test.json",hier_topo)
    topo_info_data = make_topo_info_data(case[0])
    adb_data = json.loads(case[1])
    fillSourceData = fill_source_data.FillSourceData(logger, settings,
            hier_topo, vendor_data, topo_info_data, adb_data, False)
    fillSourceData.hier_topo_info.src_location= {"X_1_2": "\'location\' (xxx.cpp:77)"}
    violation = ""
    res = fillSourceData.get_note_info_for_ii(violation, "2", "yes", "",)
    assert res == "" 
    violation = "{u'dadd': {u'aa': u'xxx'}, u'reason': u'yyy', u'solution': u'zzz'}"
    res = fillSourceData.get_note_info_for_ii(violation, "2", "yes", "dadd")
    assert res == "yyy because of the access on variable \'location\' (xxx.cpp:77)\nzzz" 
    violation = "{u'dadd': {u'sum': u'xxx'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "2", "yes", "dadd")
    #assert res == "yyy because of the access on variable \"sum\"" 
    assert res == "" 
    violation = "{u'dadd': {u'gmem': u'xxx'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "2", "yes", "dadd")
    assert res == "yyy because of the access on kernel bus" 
    violation = "{u'dadd': {u'sum_s': u':'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "2", "yes", "sum abc")
    assert res == "yyy because of the access on variable \'sum\'" 
    violation = "{u'dadd': {u'sum_s': u':'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "2", "yes", "aa bb")
    assert res == "yyy because of the access on variable \'aa\'"
    violation = "{u'dadd': {u'sum_s': u':'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "1", "no", "aa bb")
    assert res == "yyy because of the access on variable \'aa\'" 
    violation = "{u'dadd': {u'sum_s': u':'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "1", "no", "aa bb")
    assert res == "yyy because of the access on variable \'aa\'" 
    violation = "{u'dadd': {u'sum_s': u':'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "1", "no", "s")
    #assert res == "yyy because of the access on variable \"aa\"" 
    assert res == "" 
    violation = "{u'dadd': {u'sum_s': u':'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "1", "no", "s aa um_s um")
    #assert res == "yyy because of the access on variable \"aa\"" 
    assert res == "yyy because of the access on variable \'um_s\'" 
    violation = "{u'dadd': {u'test_ln106': u'xxx'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "1", "no", "s aa um_s um test")
    #assert res == "yyy because of the access on variable \"aa\"" 
    assert res == "yyy because of the access on variable \'test\'" 
    violation = "{u'dadd': {u's_ln107': u'xxx'}, u'reason': u'yyy'}"
    res = fillSourceData.get_note_info_for_ii(violation, "1", "no", "s aa um_s um test")
    #assert res == "yyy because of the access on variable \"aa\"" 
    assert res == "yyy because of the access on variable \'location\' (xxx.cpp:77)" 

