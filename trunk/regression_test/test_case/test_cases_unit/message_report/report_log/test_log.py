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
from collections import OrderedDict
from common.fixtures import *
import pytest
from unittest.mock import patch, DEFAULT

def check_if_in_log(info):
    with open('msg_report.log','r') as foo:
        for line in foo.readlines():
            if info in line:
                return True
    return False

def test_log(logger):
    from merlin_log import MerlinLog
    log_file = "msg_report.log"
    error = "MERLIN TESTING LOG ERROR"
    info = "MERLIN TESTING LOG INFO"
    warning = "MERLIN TESTING LOG INFO"
    debug = "MERLIN TESTING LOG DEBUG"
    log = MerlinLog()

    os.system("rm -rf " + log_file);
    log.set_merlin_log()
    with pytest.raises(SystemExit):
        log.merlin_error(error)
        assert check_if_in_log(error) == True
    log.merlin_warning(warning)
    assert check_if_in_log(warning) == True
    log.merlin_info(info)
    assert check_if_in_log(info) == True
    log.merlin_debug(debug)
    assert check_if_in_log(debug) == False

    os.system("rm -rf " + log_file);
    log.debug_level = "ERROR"
    log.set_merlin_log()
    with pytest.raises(SystemExit):
        log.merlin_error(error)
        assert check_if_in_log(error) == True
    log.merlin_warning(warning)
    assert check_if_in_log(warning) == False
    log.merlin_info(info)
    assert check_if_in_log(info) == False
    log.merlin_debug(debug)
    assert check_if_in_log(debug) == False

    os.system("rm -rf " + log_file);
    log.debug_level = "WARNING"
    log.set_merlin_log()
    with pytest.raises(SystemExit):
        log.merlin_error(error)
        assert check_if_in_log(error) == True
    log.merlin_warning(warning)
    assert check_if_in_log(warning) == True
    log.merlin_info(info)
    assert check_if_in_log(info) == True
    log.merlin_debug(debug)
    assert check_if_in_log(debug) == False

    os.system("rm -rf " + log_file);
    log.debug_level = "INFO"
    log.set_merlin_log()
    with pytest.raises(SystemExit):
        log.merlin_error(error)
        assert check_if_in_log(error) == True
    log.merlin_warning(warning)
    assert check_if_in_log(warning) == True
    log.merlin_info(info)
    assert check_if_in_log(info) == True
    log.merlin_debug(debug)
    assert check_if_in_log(debug) == False

    os.system("rm -rf " + log_file);
    log.debug_level = "DEBUG"
    log.set_merlin_log()
    with pytest.raises(SystemExit):
        log.merlin_error(error)
        assert check_if_in_log(error) == True
    log.merlin_warning(warning)
    assert check_if_in_log(warning) == True
    log.merlin_info(info)
    assert check_if_in_log(info) == True
    log.merlin_debug(debug)
    assert check_if_in_log(debug) == True 

    os.system("rm -rf " + log_file);
    log.debug_level = "XXXX"
    log.set_merlin_log()
    with pytest.raises(SystemExit):
        log.merlin_error(error)
        assert check_if_in_log(error) == True
    log.merlin_warning(warning)
    assert check_if_in_log(warning) == True
    log.merlin_info(info)
    assert check_if_in_log(info) == True
    log.merlin_debug(debug)
    assert check_if_in_log(debug) == True 

def test_log_2(logger):
    from merlin_log import MerlinLog
    from merlin_log import log_file_except_hook
    log = MerlinLog()
    log.set_merlin_log()
    exc_type, exc_value, exc_traceback = sys.exc_info()
    sys.excepthook = log_file_except_hook(exc_type, exc_value, exc_traceback)
    #a()
#    c = {"1" : "2"}
#    c["3"]
    # it is not work if we just write a new python which may tiger traceback
    # now just cover
