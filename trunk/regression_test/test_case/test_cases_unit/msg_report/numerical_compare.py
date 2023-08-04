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
#!/bin/python
# -*- coding:utf-8 -*-

import sys
import os
sys.path.append(os.path.join(os.environ['MERLIN_COMPILER_HOME'], 'regression_test/script/msg_report'))
from msg_report import report_data_compare
import json
import subprocess

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print 'not enough arguments'
        sys.exit(1)
    lhs_path = sys.argv[-2]
    rhs_path = sys.argv[-1]
    with open(lhs_path, 'r') as lhs_f:
        lhs = json.load(lhs_f)
        with open(rhs_path, 'r') as rhs_f:
            rhs = json.load(rhs_f)
            if report_data_compare(lhs, rhs):
                sys.exit(0)
            else:
                with open('err.log', 'a') as f:
                    output = subprocess.Popen(['diff', lhs_path, rhs_path], stdout=subprocess.PIPE).communicate()[0]
                    output = 'diff ' + lhs_path + ' ' + rhs_path + '\n' + output
                    f.write(output)
                sys.exit(1)
