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
import os
import pytest
import sys
import shutil
import subprocess

assert os.getenv("MERLIN_COMPILER_HOME") is not None
path = os.path.join(
    os.getenv("MERLIN_COMPILER_HOME"),
    "mars-gen", "scripts", "msg_report", "xilinx_enhance"
)
sys.path.insert(0, path)

def setup_module():
    os.chdir("report_data_extraction")

def teardown_module():
    os.chdir("..")

#@pytest.fixture(scope="session")
#def spec():
#    from merlin_report_setting import MerlinReportSetting
#    return MerlinReportSetting()

#@pytest.fixture(scope="session")
#def make_token_merge( test):
#    from merge_token import Merge
#    def make(test):
#        return Merge(
#        spec
#        )
#    return make

def test_merge():
    from merge_token import Merge
    from merlin_report_setting import MerlinReportSetting
    subprocess.call(["mkdir -p ./test_exe/test_exe"],shell=True)
    subprocess.call(["cp *.json ./test_exe/test_exe"],shell=True)
    os.chdir('./test_exe/test_exe')
    spec = MerlinReportSetting()
    token_merge = Merge(spec)
    #token_merge.__init__( spec)
    #test = "./topo_info.json" 
    #token_merge = make_token_merge(test)  
    #print("test \n")
    #spec.json_topo_info = "./topo_info.json"
    #print(spec.json_topo_info)
    res = token_merge.merge()
    res =  os.path.isfile("gen_token.json")
    os.chdir('../../')
    subprocess.call(["rm -r ./test_exe"],shell=True)
    assert res == True

def test_merge_1():
    from merge_token import Merge
    from merlin_report_setting import MerlinReportSetting
    #subprocess.call(["mkdir -p ./test_exe/test_exe"],shell=True)
    #subprocess.call(["cp *.json ./test_exe/test_exe"],shell=True)
    os.chdir('./test_merge_json')
    spec = MerlinReportSetting()
    token_merge = Merge(spec)
    res = token_merge.merge()
    res =  os.path.isfile("gen_token.json")
    os.chdir('../')
    subprocess.call(["rm -r ./test_exe"],shell=True)
    assert res == True

def test_merge_step():
    from merge_token import Merge
    from merlin_report_setting import MerlinReportSetting
    spec = MerlinReportSetting()
    token_merge = Merge(spec)

    res1 = token_merge._merge_topo()
    topo_count = 0;
    t_l_cnt = 0
    t_x_cnt = 0
    t_f_cnt = 0
    t_skip_cnt = 0
    for i in res1:
        #print(i)
        topo_count += 1
        if i.startswith("L"):
            t_l_cnt += 1
        elif i.startswith("F"):
            t_f_cnt += 1
        elif i.startswith("X"):
            t_x_cnt += 1
        elif i.startswith("i"):
            t_skip_cnt += 1
    #print(topo_count)
    #print(t_l_cnt)
    #print(t_f_cnt)
    #print(t_x_cnt)
    #print(t_skip_cnt)
    assert t_l_cnt == 19
    assert t_f_cnt == 22
    assert t_x_cnt == 282
    assert t_skip_cnt == 94

    res2 = token_merge._merge_vendor(res1)
    
    #for i in res2:
        #if res2[i] != res1[i]:
            #print(res[i])
            #print(res1[i])
    assert res2 == res1
    vendor_cnt = 0
    for i in res2:
        #print(i)
        vendor_cnt += 1
    #print(vendor_cnt)
    res3 = token_merge._remove_skipped(res2)
    remove_cnt = 0
    for i in res3:
        #print(i)
        remove_cnt += 1
    #print(remove_cnt)
    for i in res3:
        assert res3[i] == res2[i]

    filter_json_cnt = 0

    res4 = token_merge.filter_json(res3)
    for i in res4:
        filter_json_cnt += 1
    #print(filter_json_cnt)

def test_return_none_1():
    from merge_token import Merge
    from merlin_report_setting import MerlinReportSetting
    spec = MerlinReportSetting()
    token_merge = Merge(spec)
    line_num = '110'
    filename = "test"
    res = {}
    index_array = {}
    index_function = {}
    index_loop = {}
    merged_loop_list = []
    loop_name_list = []
    res = token_merge._merge_csynth_rpt_xml(res,index_array,index_function,index_loop, merged_loop_list, loop_name_list)
    assert res == {}


def test_return_none_2():
    from merge_token import Merge
    from merlin_report_setting import MerlinReportSetting
    spec = MerlinReportSetting()
    token_merge = Merge(spec)
    line_num = '110'
    filename = "test"
    res = {}
    index_array = {}
    index_function = {}
    index_loop = {}
    merged_loop_list = []
    loop_name_list = []
    res = token_merge._merge_vivado_hls_log(res,index_array,index_function,index_loop, merged_loop_list, loop_name_list)
    assert res == {}

def test_index_array():
    from merge_token import Merge
    from merlin_report_setting import MerlinReportSetting
    spec = MerlinReportSetting()
    token_merge = Merge(spec)
    res = {'test':{'name' : 'loop'}}
    rt = token_merge._index_array(res)
    #print(rt)
    assert rt == {}

#def test_index_loop():
#    from merge_token import Merge
#    from merlin_report_setting import MerlinReportSetting
#    spec = MerlinReportSetting()
#    token_merge = Merge(spec)
#    res = {'L':{'name' : 'loop'}}
#    rt = token_merge._index_loop(res)
#    print(rt)
#    assert rt == {}

def test_index_func():
    from merge_token import Merge
    from merlin_report_setting import MerlinReportSetting
    spec = MerlinReportSetting()
    token_merge = Merge(spec)
    res = {'test':{'name' : 'loop'}}
    rt = token_merge._index_function(res)
    #print(rt)
    assert rt == {}


