# (C) Copyright 2016-2021 Xilinx, Inc.
# All Rights Reserved.
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License. 

#!/bin/python
# -*- coding:utf-8 -*-
from __future__ import print_function
import os
import argparse
import shutil
import sys
import logging
import xml.etree.ElementTree
import traceback
import json
from xilinx_enhance.merlin_report_top import merlin_report_top

def log_file_except_hook(t, v, tb):
    with open('msg_traceback', 'w') as f:
        lines = traceback.format_exception(t, v, tb)
        f.writelines(lines)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--src", required=True, help="user source code directory")
    parser.add_argument("--dst", required=True, help="merlin output code directory")
    parser.add_argument("--xml", required=True, help="directive.xml")
    parser.add_argument("--metadata", required=True, help="metadata history file directory")
    parser.add_argument("--report", required=True, help="vendor report directory; if fast DSE, use stored json files")
    args = parser.parse_args()
    xml_file = args.xml
    spec = xml.etree.ElementTree.parse(xml_file)
    impl_tool = spec.getroot().find('implementation_tool').text.strip()
    print(impl_tool)
    if impl_tool == "sdaccel":
       merlin_report_top(args.src, args.dst, args.xml, args.metadata, args.report) 
    sys.exit(0)
