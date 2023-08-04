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
from common.fixtures import *
import data_extraction
from c_synth_rpt_parser import ReportParseError
from c_synth_rpt_parser import ReportExtractError
import merlin_log 
import os
import sys
import pytest
from unittest.mock import patch, DEFAULT


def setup_module():
    os.chdir("misc_modules/report_merlin")

def teardown_module():
    os.chdir("../..")

@patch("data_extraction.print")
@patch("data_extraction.open")
def test_except_01(mock_open, mock_print):
    def side_effect(*args):
        if args[0] == 'vendor_report/adb.json':
            raise IOError
        else:
            return DEFAULT
    mock_open.side_effect = side_effect
    with pytest.raises(SystemExit):
        with pytest.raises(IOError):
            data_extraction.DataExtraction().data_extraction_top()
    mock_print.assert_called_with("Failed to generate : adb.json")

@patch("data_extraction.print")
@patch("data_extraction.open")
def test_except_02(mock_open, mock_print):
    def side_effect(*args):
        if args[0] == 'gen_token.json':
            raise IOError
        else:
            return DEFAULT
    mock_open.side_effect = side_effect
    with pytest.raises(SystemExit):
        with pytest.raises(IOError):
            data_extraction.DataExtraction().data_extraction_top()
    mock_print.assert_called_with("Failed to generate : gen_token.json")

@patch("merlin_log.MerlinLog.merlin_warning")
def test_except_03(mock_merlin_warning):
    def side_effect(*args):
        res = []
        res = args[0].split(",")
        if res[0] == "Skipping vendor_report/test.rpt.xml":
            raise ReportParseError("except parse error !")
        else:
            return DEFAULT
    mock_merlin_warning.side_effect = side_effect
    with pytest.raises(ReportParseError) as e:
        data_extraction.DataExtraction().data_extraction_top()
    assert str(e.value) == "[ReportParseError] except parse error !"


@patch("merlin_log.MerlinLog.merlin_warning")
def test_except_04(mock_merlin_warning):
    def side_effect(*args):
        res = []
        res = args[0].split(",")
        if res[0] == "Skipping vendor_report/test1.rpt.xml":
            raise ReportExtractError("except extract error !")
        else:
            return DEFAULT
    mock_merlin_warning.side_effect = side_effect
    with pytest.raises(ReportExtractError) as e:
        data_extraction.DataExtraction().data_extraction_top()
    assert str(e.value) == "[ReportExtractError] except extract error !"
