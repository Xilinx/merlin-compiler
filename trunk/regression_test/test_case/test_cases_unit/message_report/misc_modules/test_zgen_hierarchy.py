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
import gen_hierarchy
import os
import pytest
from unittest.mock import patch, DEFAULT

def setup_module():
    os.chdir("misc_modules/report_merlin")

def teardown_module():
    os.chdir("../..")

@patch("gen_hierarchy.print")
@patch("gen_hierarchy.open")
def test_except_01(mock_open, mock_print, logger, settings):
    def side_effect(*args):
        if args[0] == 'src/hierarchy.json':
            raise IOError
        else:
            return DEFAULT
    mock_open.side_effect = side_effect
    with pytest.raises(SystemExit):
        with pytest.raises(IOError):
            gen_hierarchy.GenHierarchy(logger, settings).gen_hierarchy_top()
    mock_print.assert_called_with("Failed to generate : hierarchy.json")
