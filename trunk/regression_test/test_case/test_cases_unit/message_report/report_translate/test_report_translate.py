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
import os
import sys
import json
import pytest
import re

env_dict = os.environ
MERLIN_COMPILER_HOME = env_dict.get("MERLIN_COMPILER_HOME")
sys.path.append(MERLIN_COMPILER_HOME + '/mars-gen/scripts/msg_report/xilinx_enhance')

from collections import OrderedDict
import merlin_log
import merlin_report_setting
import separate_json_gen_top
import report_translate as rpt

def setup_module():
    os.chdir("report_translate")

def teardown_module():
    os.chdir("..")

@pytest.fixture()
def LOGGER():
    logger = merlin_log.MerlinLog()
    logger.set_merlin_log()
    return logger

@pytest.fixture()
def SETTINGS():
    return merlin_report_setting.MerlinReportSetting()

@pytest.fixture()
def reportTranslate(LOGGER, SETTINGS):
    return rpt.ReportTranslate(LOGGER, SETTINGS)

def test_get_equal_line():
    assert "==========" in rpt.get_equal_line()

def test_getmidline():
    assert '' == rpt.get_midline(0)
    assert '-' == rpt.get_midline(1)
    print(rpt.get_midline(-3)) # shouldn't be crashed

def test_get_spaceline():
    assert "  " == rpt.get_spaceline(2)
    assert "" == rpt.get_spaceline(0)
    print(rpt.get_spaceline(-5)) # shouldn't be crashed

def test_right_alignment():
    assert "  abc" == rpt.right_alignment("abc", 5)
    assert "abcde" == rpt.right_alignment("abcde", 5)
    assert "abcde" == rpt.right_alignment("abcde", 3)

def test_mid_alignment():
    assert "  abc  " == rpt.mid_alignment("abc", 7)
    assert "  abc   " == rpt.mid_alignment("abc", 8)
    assert "abc" == rpt.mid_alignment("abc", 2)

class TestReportTranslate:
    def test_get_pre_info(self, reportTranslate):
        assert "" == reportTranslate.get_pre_info("")

    def test_get_table_info(self, reportTranslate):
        assert "+\n+\n" == reportTranslate.get_table_info("")

    def test_get_note_info(self, reportTranslate):
        assert "" == reportTranslate.get_note_info("")

    def test_translate_summary(self, reportTranslate):
        string_rpt = reportTranslate.translate('summary.json')
        assert re.search(r'Target Fequency: 300MHz', string_rpt) is not None
        assert re.search(r'aes256_encrypt_ecb_kernel ', string_rpt) is not None
        assert re.search(r'Kernel.*Cycles.*BRAM', string_rpt) is not None

    def test_translate_interface(self, reportTranslate):
        string_rpt = reportTranslate.translate('interface.json')
        assert re.search(r'---.*---.*---', string_rpt) is not None
        assert re.search(r'key.*cpp:175.*memory burst', string_rpt) is not None

    def test_translate_performance(self, reportTranslate):
        string_rpt = reportTranslate.translate('performance.json')
        assert re.search(r'auto memory burst for.*\|.*2048', string_rpt) is not None

    def test_translate_resource(self, reportTranslate):
        string_rpt = reportTranslate.translate('resource.json')
        assert re.search(r'Hierarchy.*\|.*BRAM.*\|.*URAM', string_rpt) is not None
        assert re.search(r'aes_addRoundKey_cpy.*\|.*-\|.*', string_rpt) is not None

    def test_translate_top(self, reportTranslate):
        string_rpt = reportTranslate.translate_top()
        assert re.search(r'Kernel.*Cycles.*BRAM', string_rpt) is not None
        assert re.search(r'key.*cpp:175.*memory burst', string_rpt) is not None
        assert re.search(r'auto memory burst for.*\|.*2048', string_rpt) is not None
        assert re.search(r'Hierarchy.*\|.*BRAM.*\|.*URAM', string_rpt) is not None
        assert re.search(r'aes_addRoundKey_cpy.*\|.*-\|.*', string_rpt) is not None

    def test_translate_empty(self, reportTranslate):
        assert "" == reportTranslate.translate('empty.json')

    def test_translate_exception(self, reportTranslate):
        # corner input
        with pytest.raises(SystemExit):
            string_rpt = reportTranslate.translate('summary_1.json')

    def test_translate_corner(self, reportTranslate):
        with pytest.raises(SystemExit):
            string_rpt = reportTranslate.translate('corner.json')
        #assert re.search(r'Hierarchy.*\|.*BRAM.*\|.*URAM', string_rpt) is not None
        #assert re.search(r'aes_addRoundKey_cpy.*\|.*-\|.*', string_rpt) is not None
