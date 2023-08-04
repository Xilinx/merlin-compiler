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
import json
import subprocess

def try_to_get_float(s, default=0):
    try:
        r = float(s)
        return (True, r)
    except:
        return (False, default)

def json_compare(lhs, rhs):
    if len(lhs) != len(rhs):
        return False
    for k, v in lhs.items():
        if k not in rhs:
            return False
        for key in v:
            if key in ('unparse_info', 'messages', 'total_percent', 'burst', 'filename', 'stmts', 'var_refs', 'var_defs', 'name'):
                continue
            lhs_v = v[key]
            if key not in rhs[k]:
                return False
            rhs_v = rhs[k][key]
            float_keys = ( 'total-BRAM', 'total-DSP', 'total-FF', 'total-LUT', 'util-BRAM', 'util-DSP', 'util-FF', 'util-LUT')
            cyc_keys = ('CYCLE_TOT', 'CYCLE_UNIT')
            num_keys = ('trip-count', )
            if key in float_keys or key in cyc_keys or key in num_keys:
                lhs_is_float, lhs_f = try_to_get_float(lhs_v)
                rhs_is_float, rhs_f = try_to_get_float(rhs_v)
                if lhs_is_float and rhs_is_float:
                    rate = (lhs_f + 0.001)  / (rhs_f + 0.001)
                    if rate < 0.95 or rate > 1.05:
                        return False
                else:
                    if lhs_v != rhs_v:
                        return False
            else:
                if lhs_v != rhs_v:
                    return False
    return True

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
            if json_compare(lhs, rhs):
                sys.exit(0)
            else:
                with open('err.log', 'a') as f:
                    output = subprocess.Popen(['diff', lhs_path, rhs_path], stdout=subprocess.PIPE).communicate()[0]
                    output = 'diff ' + lhs_path + ' ' + rhs_path + '\n' + output
                    f.write(output)
                sys.exit(1)
