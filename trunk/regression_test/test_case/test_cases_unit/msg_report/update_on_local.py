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

from __future__ import print_function
import os
import sys
import shutil
import subprocess
import multiprocessing

def worker(src):
    with open(os.devnull, 'w') as f:
        ret = subprocess.call(['make', 'fast_update'], stdout=f, stderr=subprocess.STDOUT, cwd=src)
        if ret == 0:
            print('{0} is done'.format(src))
            dst = os.path.join(os.environ.get('MERLIN_COMPILER_HOME'), 'regression_test/test_case/test_cases_unit/msg_report', src)
            for f in os.listdir(dst):
                if f == 'msg_report.ref':
                    continue
                if not f.endswith('.ref'):
                    continue
                if 'cycle_related' in src and not 'msg_report' in f:
                    continue
                print(os.path.join(src, f[:-len('.ref')]))
                print(os.path.join(dst, f))
                shutil.copy(os.path.join(src, f[:-len('.ref')]), os.path.join(dst, f))
        else:
            print('{0} is failed'.format(src))

def fast_update(file_list):
    run_path = []
    with open(file_list) as f:
        for p in f:
            if p.startswith('#') or not p.strip():
                continue
            p = p.strip()
            if p.endswith(' is failed.'):
                p = p[:-len(' is failed.')]
            src = os.path.join(os.curdir, p.strip())
            run_path.append(src)
    pool = multiprocessing.Pool()
    pool.map(worker, run_path)

if __name__ == '__main__':
    file_list = os.curdir
    fast_update('err.log')
