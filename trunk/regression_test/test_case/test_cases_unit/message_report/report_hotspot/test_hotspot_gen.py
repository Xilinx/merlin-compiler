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
# to run this test manually: pytest -v report_hotspot
# current directory should be the parent directory to this script file

import os
import sys
import json
import pytest
import re

assert os.getenv("MERLIN_COMPILER_HOME") is not None
path = os.path.join(
    os.getenv("MERLIN_COMPILER_HOME"),
    "mars-gen", "scripts", "msg_report", "xilinx_enhance"
)
sys.path.insert(0, path)

def setup_module():
    os.chdir("report_hotspot")

def teardown_module():
    os.chdir("..")

from collections import OrderedDict
import merlin_log
import merlin_report_setting
import separate_json_gen_top
import utility
import hotspot_gen

@pytest.fixture()
def LOGGER():
    logger = merlin_log.MerlinLog()
    logger.set_merlin_log()
    return logger

@pytest.fixture()
def SETTINGS():
    return merlin_report_setting.MerlinReportSetting()

@pytest.fixture()
def fillHotspotData(LOGGER, SETTINGS):
    return hotspot_gen.FillHotspotData(LOGGER, SETTINGS)

def test_is_number(fillHotspotData):
    assert fillHotspotData.is_number("23")
    assert not fillHotspotData.is_number("12.abc")
    assert not fillHotspotData.is_number("")

def do_compare_result(node, topo_id, compare_key, compare_value):
    if 'topo_id' in node and compare_key in node and node['topo_id'] == topo_id:
        return node[compare_key] == compare_value
    sub_keys = ['sub_functions', 'interfaces', 'childs']
    for sub_key in sub_keys:
        if sub_key in node:
            for sub_node in node[sub_key]:
                if do_compare_result(sub_node, topo_id, compare_key, compare_value):
                    return True
    return False

def compare_result(json_file, topo_id, compare_key, compare_value):
    for kernel in json_file:
        if do_compare_result(kernel, topo_id, compare_key, compare_value):
            return True
    return False

def test_fill_hotspot_data_1_1(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_1 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_1.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_2(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_2 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_2.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'F_0', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_3(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_3 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_3.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '-')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_4(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_4 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_4.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_5(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_5 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_5.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '12 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_6(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_6 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_6.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '30 ( 71.4%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_7(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_7 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_7.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_8(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_8 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_8.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_9(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_9 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_9.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_10(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_10 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_10.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_1_11(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_1_11 --dst ./")
    fillHotspotData.fill_hotspot_data('input_1_11.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_2_1(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_2_1 --dst ./")
    fillHotspotData.fill_hotspot_data('input_2_1.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_2_2(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_2_2 --dst ./")
    fillHotspotData.fill_hotspot_data('input_2_2.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_2_3(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_2_3 --dst ./")
    fillHotspotData.fill_hotspot_data('input_2_3.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '15 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_2_4(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_2_4 --dst ./")
    fillHotspotData.fill_hotspot_data('input_2_4.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '12 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_2_5(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_2_5 --dst ./")
    fillHotspotData.fill_hotspot_data('input_2_5.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '30 (100.0%)')
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '-')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_3_1(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_3_1 --dst ./")
    fillHotspotData.fill_hotspot_data('input_3_1.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    assert compare_result(json_result, 'L_0_1', 'CYCLE_TOT', '-')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_3_2(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_3_2 --dst ./")
    fillHotspotData.fill_hotspot_data('input_3_2.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_3_3(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_3_3 --dst ./")
    fillHotspotData.fill_hotspot_data('input_3_3.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    assert compare_result(json_result, 'X_1', 'CYCLE_TOT', '3 (100.0%)')
    assert compare_result(json_result, 'F_1', 'CYCLE_TOT', '-')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_4_1(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_4_1 --dst ./")
    fillHotspotData.fill_hotspot_data('input_4_1.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_4_2(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_4_2 --dst ./")
    fillHotspotData.fill_hotspot_data('input_4_2.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    assert compare_result(json_result, 'F_1', 'CYCLE_TOT', '-')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_4_3(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_4_3 --dst ./")
    fillHotspotData.fill_hotspot_data('input_4_3.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '288 (100.0%)')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_4_4(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_4_4 --dst ./")
    fillHotspotData.fill_hotspot_data('input_4_4.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    assert compare_result(json_result, 'L_3_1', 'CYCLE_TOT', '-')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_5_1(fillHotspotData):
    os.system("python ../common/generate_hierarchy.py --src data/input_5_1 --dst ./")
    fillHotspotData.fill_hotspot_data('input_5_1.json')
    with open('fill_hotspot_data.json', 'r') as f:
        json_result = json.load(f)
    assert compare_result(json_result, 'L_0', 'CYCLE_TOT', '3 (100.0%)')
    assert compare_result(json_result, 'L_3_1', 'CYCLE_TOT', '-')
    os.system("rm *.json *.log")

def test_fill_hotspot_data_6(fillHotspotData):
    os.system("cp ./data/input_6.json .")
    fillHotspotData.fill_hotspot_data('input_6.json')
    with open('fill_hotspot_data.json','r') as f:
        json_result = json.load(f)
    for node in json_result[0]["childs"]:
        if node["src_topo_id"] == 'L_0_0_0_2_2_0_3':
            trip_count = node["trip-count"]
    assert trip_count == '?'
    os.system("rm *.json *.log")

def test_fill_hotspot_data_7(fillHotspotData):
    os.system("cp ./data/input_7.json .")
    fillHotspotData.fill_hotspot_data('input_7.json')
    with open('fill_hotspot_data.json','r') as f:
        json_result = json.load(f)
    for node in json_result[0]["sub_functions"]:
        if node["name"] == 'multiple_bits_writer':
            cycle_tot = node["CYCLE_TOT"]
    assert cycle_tot == '6087752 ( 82.9%)'
