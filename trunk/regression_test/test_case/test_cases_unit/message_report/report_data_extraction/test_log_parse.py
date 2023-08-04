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
import shutil

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

@pytest.fixture(scope="session")
def spec():
    from merlin_report_setting import MerlinReportSetting
    return MerlinReportSetting()

@pytest.fixture(scope="session")
def make_log_parser( spec):
    from vivado_hls_log_parser import LogParser
    def make(test):
        return LogParser(
            spec,test
        )
    return make

def test_parse_1(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_1(i))
    t = 0
    for x in res:
        #print(x,":",res[x])
        t = t + 1
    print(t)
    assert t == 20

def test_parse_2(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    t = 0
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_2(i))
    for x in res:
        #print(x,":",res[x])
        t = t + 1
    assert t == 1

def test_parse_3(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_3(i))
    t = 0
    for x in res:
        #print(x,":",res[x])
        t = t + 1
    assert t == 3

def test_parse_4(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_4(i))
    t = 0
    for x in res:
        #print(x,":",res[x])
        t = t + 1
    assert t == 52

def test_parse_5(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_5(i))
    t = 0
    for x in res:
        #print(x,":",res[x])
        t = t + 1
    assert t == 3


def test_parse_8(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_8(i))
    t = 0
    for x in res:
        #print(x,":",res[x])
        t = t + 1
    assert t == 3

def test_parse_12(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    l = 0
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_12(i))
    t = 0
    for x in res:
        print(x,":",res[x])
        t = t + 1
    assert t == 12
    
def test_parse_13(make_log_parser):
    test_file = ("vivado_hls.log")
    log_parser = make_log_parser(test_file)
    res = {}
    l = 0
    for i in range(len(log_parser.lines)):
        res.update(log_parser.parse_13(i))
    t = 0
    for x in res:
        print(x,":",res[x])
        t = t + 1
    assert t == 2 

def test_parse(make_log_parser):
    test_file = ("vivado_hls.log")
    shutil.copy("vivado_hls.json","vivado_hls_bp.json")
    os.remove("vivado_hls.json")
    log_parser = make_log_parser(test_file) 
    log_parser.parse()
    
    res =  os.path.isfile("vivado_hls.json")
    shutil.copy("vivado_hls_bp.json","vivado_hls.json")
    assert res == True
