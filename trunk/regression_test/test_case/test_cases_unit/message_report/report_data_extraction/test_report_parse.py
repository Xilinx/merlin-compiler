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
import filecmp

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

#test report parse
@pytest.fixture(scope="function")
def logger():
    from merlin_log import MerlinLog
    return MerlinLog()

@pytest.fixture(scope="function")
def make_report_parse( logger):
    from c_synth_rpt_parser import ReportParser
    def make(test):
        return ReportParser(
            logger,test
        )
    return make


def test_report_parse_xml_empty(make_report_parse):
    test = ("./test_xml_empty.rpt.xml")
    from c_synth_rpt_parser import ReportParseError
    with pytest.raises(ReportParseError)as excinfo:
        report_parse = make_report_parse(test)
    #print(excinfo.type)
    #print(excinfo.match)
    #print(excinfo.value)
    assert str(excinfo.value) == "[ReportParseError] no element found: line 13, column 0"

def test_report_parse_item(make_report_parse):
    test = ("./test_item.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    t1 = tt.__getitem__(0)
    #print(t1.__len__());
    #print(t1.tag())
    result = []
    for x in t1:
        res = report_parse.parse_item(x)
        result.append(res)
    
    assert result[0][0] == 'Date'
    assert result[0][1] == 'Wed Sep  4 05:44:52 2019\n'
    assert result[1][0] == 'Version'
    assert result[1][1] == '2019.1 (Build 2552052 on Fri May 24 15:28:33 MDT 2019)'
    assert result[2][0] == 'Project'
    assert result[2][1] == 'aes256_encrypt_ecb_kernel'


def test_report_parse_item_empty(make_report_parse):
    test = ("./test_item_empty.rpt.xml")
    from c_synth_rpt_parser import ReportParseError
    report_parse = make_report_parse(test)
    tt = report_parse.root
    t1 = tt.__getitem__(0)
    with pytest.raises(ReportParseError)as excinfo:
        res = report_parse.parse_item(t1)
    #print(excinfo.type)
    #print(excinfo.match)
    #print(excinfo.value)
    assert str(excinfo.value) == "[ReportParseError] Found <item> node without 'name' attribute value!"

def test_report_parse_section(make_report_parse):
    test = ("./test_item.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    #print(tt.tag())
    t1 = tt.__getitem__(0)
    #print(t1.tag())
    res = report_parse.parse_section(t1)
    for i in res.keys():
        x = i
    assert x == "Vivado HLS Report for 'test xml report parse'"

def test_report_parse_section_empty(make_report_parse):
    test = ("./test_section_empty.rpt.xml")
    from c_synth_rpt_parser import ReportParseError
    report_parse = make_report_parse(test)
    tt = report_parse.root
    t1 = tt.__getitem__(0)
    #with pytest.raises(ReportParseError)as excinfo:
    res = report_parse.parse_section(t1)
    for i in res.keys():
        x = i
    assert x == "Target device"

def test_report_parse_table(make_report_parse):
    test = ("./test_table.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    #print(tt.tag())
    t1 = tt.__getitem__(0)
    #print(t1.tag())
    #print(t1.__len__())
    t2_1 = t1.__getitem__(0)
    t2_2 = t1.__getitem__(1)
    #print(t2_1.tag())
    #print(t2_2.tag())
    t3_1 = t2_1.__getitem__(0)
    #print(t3_1.tag())
    t4_1 = t3_1.__getitem__(0)
    #print(t4_1.tag())
    t5_1 = t4_1.__getitem__(0)
    #print(t5_1.tag())
    key = []
    result = {}
    list_1 = []
    ref = ['3.33','2.373','0.90']
    res = report_parse.parse_table(t5_1)
    for i in res:
        key = i
        result = res[i]
    #print(key)
    #print(result)
    assert key == "ap_clk"
    for i in result:
        list_1.append(result[i])
    for i in range(3):
        assert list_1[i] == ref[i]
   
def test_report_parse_table_empty(make_report_parse):
    test = ("./test_table_empty.rpt.xml")
    from c_synth_rpt_parser import ReportParseError
    report_parse = make_report_parse(test)
    tt = report_parse.root
    t1 = tt.__getitem__(0)
    t2_1 = t1.__getitem__(0)
    t2_2 = t1.__getitem__(1)
    t3_1 = t2_1.__getitem__(0)
    t4_1 = t3_1.__getitem__(0)
    t5_1 = t4_1.__getitem__(0)
    key = []
    resut = {}
    list_1 = []
    ref = ['3.33','2.373','0.90']
    with pytest.raises(ReportParseError)as excinfo:
        res = report_parse.parse_table(t5_1)
    assert str(excinfo.value) == "[ReportParseError] Found one row in <table> node without 'name' attribute value!"

def test_report_parse_table_3(make_report_parse):
    test = ("./test_table_1.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    t1 = tt.__getitem__(0)
    t2_1 = t1.__getitem__(0)
    t2_2 = t1.__getitem__(1)
    t3_1 = t2_1.__getitem__(0)
    t4_1 = t3_1.__getitem__(0)
    t5_1 = t4_1.__getitem__(0)
    key = []
    result = {}
    list_1 = []
    ref = ['1','1','1']
    res = report_parse.parse_table(t5_1)
    for i in res:
        result = res[i]
    for i in result:
        list_1.append(result[i])
    for i in range(3):
        assert list_1[i] == ref[i]

def test_report_parse_xml(make_report_parse):
    test = ("./test_table.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    res = tt.__repr__()
    ref = "<Element 'profile' at"
    for i in range(len(ref)):
        assert res[i] == ref[i]

def test_report_parse_xml(make_report_parse):
    test = ("./test_empty.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    res = tt.__repr__()
    txt = tt.text()
    #print(txt)
    assert txt == ''

def test_report_parse_row(make_report_parse):
    test = ("./test_table.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    #print(tt.tag())
    t1 = tt.__getitem__(0)
    #print(t1.tag())
    #print(t1.__len__())
    t2_1 = t1.__getitem__(0)
    t2_2 = t1.__getitem__(1)
    #print(t2_1.tag())
    #print(t2_2.tag())
    t3_1 = t2_1.__getitem__(0)
    #print(t3_1.tag())
    t4_1 = t3_1.__getitem__(0)
    #print(t4_1.tag())
    t5_1 = t4_1.__getitem__(0)
    #print(t5_1.tag())
    t6_1 = t5_1.__getitem__(0)
    #print(t6_1.tag())
    res = report_parse.parse_row(t6_1)
    ref = ['Clock', 'Target', 'Estimated', 'Uncertainty']
    for i in range(4):
        assert res[i] == ref[i]

def test_report_parse(make_report_parse):
    test = ("./test_item.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    res = report_parse.parse(tt)
    #print(res)
    result = {}
    list_1 = []
    ref = ['Wed Sep  4 05:44:52 2019\n', '2019.1 (Build 2552052 on Fri May 24 15:28:33 MDT 2019)', 'aes256_encrypt_ecb_kernel','solution', 'virtexuplus', 'xcu250-figd2104-2L-e']
    for i in res:
        result = res[i]
    for i in result:
        list_1.append(result[i])
    for i in range(len(ref)):
        assert list_1[i] == ref[i]

def test_report_rename_duplicate(make_report_parse):
    test = ("./test_item.rpt.xml")
    report_parse = make_report_parse(test)
    tt = report_parse.root
    keys_1 = ['test','test','test1']
    res = report_parse.rename_duplicate(keys_1)
    res.append(res[0])
    #print(res)
    #print(len(keys_1))
    #print(len(res))
    for i in range(len(res)-1):
        assert (res[i] != res[i+1]) == 1
        
    
    assert 1 == 1



#test report extraction
@pytest.fixture(scope="function")
def spec():
    from merlin_report_setting import MerlinReportSetting
    return MerlinReportSetting()

@pytest.fixture(scope="function")
def make_report_extractor( spec,logger):
    from c_synth_rpt_parser import ReportExtractor
    def make(test):
        return ReportExtractor(
            spec,logger,test
        )
    return make

def test_report_extractor_dump(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    report_extractor.dump()
    result = filecmp.cmp('test_ref.json','test_extractor_csynth.json')
    assert result == True


def test_report_extractor_perf_summ_empty(make_report_extractor):
    test_file = ("test_extractor_empty.rpt.xml")
    from c_synth_rpt_parser import ReportExtractError
    report_extractor = make_report_extractor(test_file);
    res = {}
    with pytest.raises(ReportExtractError)as excinfo:
        res = report_extractor._perf_summ(res)
    #print(excinfo.value)
    assert str(excinfo.value) == "[ReportExtractError] No 'min0' in Performance Estimates/Latency (clock cycles)/Summary"


def test_report_extractor_perf_summ(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = {}
    res = report_extractor._perf_freq(res)
    res = report_extractor._perf_summ(res)
    tt = {}
    result=[]
    ref = ['2.373','3.33','0.90','3','4156','3','4156']
    j = 0
    for i in res.keys():
        tt = res[i]
    for i in tt.keys():
        result.append(tt[i])
    for i in range(4):
        assert result[i] == ref[i]

def test_report_extractor_perf_instance_empty(make_report_extractor):
    test_file = ("test_extractor_empty_1.rpt.xml")
    from c_synth_rpt_parser import ReportExtractError
    report_extractor = make_report_extractor(test_file);
    res = {}
    #with pytest.raises(TypeError)as excinfo:
    res = report_extractor._perf_instance(res)
    #print(excinfo.value)
    #print(res)
    #assert str(excinfo.value) == "catching classes that do not inherit from BaseException is not allowed"
    assert res == {}


def test_report_extractor_perf_instance_empty_1(make_report_extractor):
    test_file = ("test_extractor_empty_2.rpt.xml")
    from c_synth_rpt_parser import ReportExtractError
    report_extractor = make_report_extractor(test_file);
    res = {}
    #with pytest.raises(TypeError)as excinfo:
    res = report_extractor._perf_instance(res)
    #print(excinfo.value)
    #print(res)
    #assert str(excinfo.value) == "catching classes that do not inherit from BaseException is not allowed"
    #assert res == {}

def test_report_extractor_perf_instance(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = {}
    res = report_extractor._perf_instance(res)
    tt = {}
    result=[]
    ref = ['grp_mars_kernel_0_2_node_1_stage_1_fu_788','mars_kernel_0_2_node_1_stage_1','1','4154','1','4154']
    for i in res.keys():
        tt = res[i]
    for i in tt.keys():
        result.append(tt[i])
    for i in range(len(result)):
        assert result[i] == ref[i]

def test_report_extractor_perf_loop_empty_1(make_report_extractor):
    test_file = ("test_extractor_loop_empty_1.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = {}
    res = report_extractor._perf_loop(res)
    assert res == {}

def test_report_extractor_perf_loop_empty(make_report_extractor):
    test_file = ("test_extractor_loop_empty.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = {}
    res = report_extractor._perf_loop(res)
    assert res == {}

def test_report_extractor_perf_loop(make_report_extractor):
    test_file = ("test_extractor_loop.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = {}
    res = report_extractor._perf_loop(res)
    tt = {}
    result=[]
    ref = ['merlinL2','4125','4125','31','31','1','4096','yes']
    for i in res.keys():
        tt = res[i]
    for i in tt.keys():
        result.append(tt[i])
    for i in range(len(result)):
        assert result[i] == ref[i]

def test_report_extractor_perf_loop_1(make_report_extractor):
    test_file = ("test_extractor_loop_1.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = {}
    res = report_extractor._perf_loop(res)
    tt = {}
    result=[]
    ref = ['merlinL2','4125','4125','','31','1','4096','yes']
    for i in res.keys():
        tt = res[i]
    for i in tt.keys():
        result.append(tt[i])
    for i in range(len(result)):
        assert result[i] == ref[i]

def test_report_extractor_util_summ_empty_1(make_report_extractor):
    test_file = ("test_extractor_util_empty_1.rpt.xml")
    from c_synth_rpt_parser import ReportExtractError
    report_extractor = make_report_extractor(test_file);
    res = {'test_extractor':{}}
    with pytest.raises(ReportExtractError)as excinfo:
        res = report_extractor._util_summ(res)
   
    print(excinfo.value)
    #assert str(excinfo.value) == "[ReportExtractError] No 'test_extractor_util_empty' in Utilization Estimates/Summary/Utilization (%)"
    assert 1 == 1

def test_report_extractor_util_summ_empty(make_report_extractor):
    test_file = ("test_extractor_util_empty.rpt.xml")
    from c_synth_rpt_parser import ReportExtractError
    report_extractor = make_report_extractor(test_file);
    res = {'test_extractor':{}}
    with pytest.raises(ReportExtractError)as excinfo:
        res = report_extractor._util_summ(res)
   
    #print(excinfo.value)
    assert str(excinfo.value) == "[ReportExtractError] No 'test_extractor_util_empty' in Utilization Estimates/Summary/Utilization (%)"

def test_report_extractor_util_summ(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = {'test_extractor':{}}
    res = report_extractor._util_summ(res)
    tt = {}
    result=[]
    #print(res)
    ref = [224,0,17024,30653,0,'4','0','~0','1','0']
    for i in res.keys():
        tt = res[i]
    for i in sorted(tt.keys()):
        result.append(tt[i])
    for i in range(len(result)):
        #print(result[i])
        assert result[i] == ref[i]

def test_report_extractor_strip_loop_name(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = "   + - - + test_loop_name"
    res = report_extractor.strip_loop_name(res)
    
    #print(res)
    assert res == "test_loop_name"

def test_report_extractor_process_tilde(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    report_extractor = make_report_extractor(test_file);
    res = "test1 ~ test2"
    res = report_extractor.process_tilde(res)
    assert res[0] == 'test1'
    assert res[1] == 'test2'

def test_report_extractor_get_to_dictionary(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    from c_synth_rpt_parser import ReportExtractor
    report_extractor = make_report_extractor(test_file);
    path = [ReportExtractor.PE, ReportExtractor.CC, ReportExtractor.SUMM]
    d = {}
    d = report_extractor.get_to_dictionary([],report_extractor.res,*path)
    #print(d)
    tt = {}
    result=[]
    ref = ['3','4156','3','4156','none']
    for i in d.keys():
        result.append(d[i])
    for i in range(len(result)):
        assert result[i] == ref[i]


def test_report_extractor_get_to_dictionary_empty(make_report_extractor):
    test_file = ("test_extractor.rpt.xml")
    from c_synth_rpt_parser import ReportExtractor
    from c_synth_rpt_parser import ReportExtractError
    report_extractor = make_report_extractor(test_file);
    path = ["tesasd","asdadd"]
    d = {}
    with pytest.raises(ReportExtractError)as excinfo:
        d = report_extractor.get_to_dictionary([],report_extractor.res,*path)
    #print(excinfo.value)
    assert str(excinfo.value) == "[ReportExtractError] Unable to get to dictionary 'tesasd/asdadd' for its data"

