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
import subprocess
import os
#import merlin_report_top
from common.fixtures import *
import pytest
import sys
from collections import OrderedDict
def setup_module():
    os.chdir("misc_modules/report_merlin_fail")

def teardown_module():
    os.chdir("../../../")

def test(make_merlin_report_top):
#def test():
#    subprocess.run(
#      ["msg_report", "--src src/",
#                     "--dst lc/",
#                     "--metadata metadata/",
#                     "--xml directive.xml",
#                     "--report vendor_report/"],
#      cwd="misc_modules/report_merlin",
#      check=True
#    )
#    os.chdir("misc_modules/report_merlin_fail")
    cwd = os.getcwd()
    print("cwd = " + cwd)
    with pytest.raises(SystemExit):
        make_merlin_report_top("src", "lc", "directive.xml", "metadata", "vendor_report")
        print("after cwd = " + cwd)
#    subprocess.run(
#      ["msg_report", "--src src/",
#                     "--dst lc/",
#                     "--metadata metadata/",
#                     "--xml directive.xml",
#                     "--report vendor_report/"],
#      cwd="misc_modules/report_merlin_fail",
#      check=False
#    )
