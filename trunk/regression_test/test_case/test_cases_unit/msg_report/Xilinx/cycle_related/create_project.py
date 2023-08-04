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
import shutil
import subprocess

MAKE_FILE = './Makefile'
CLEAN_REPORT = './clean_report.py'
CYCLE_COMPARE = './numerical_compare.py'

def create_project(target):
    wk_dir = target[:target.rfind('.')]
    src_dir = os.path.join(wk_dir, 'src')
    if os.path.isdir(src_dir):
        shutil.rmtree(src_dir)
    os.makedirs(src_dir)
    shutil.copy(target, src_dir)
    shutil.copy(MAKE_FILE, wk_dir)
    shutil.copy(CLEAN_REPORT, wk_dir)
    shutil.copy(CYCLE_COMPARE, wk_dir)
    subprocess.call(('merlin_create_project', 'run', '-s', 'src', '-x'), cwd=wk_dir)
    return subprocess.Popen(('make', 'update'), cwd=wk_dir)

if __name__ == '__main__':
    work_dir = '.'
    if os.path.isdir(sys.argv[-1]):
        print 'process dir'
        work_dir = sys.argv[-1]
    elif os.path.isfile(sys.argv[-1]):
        print 'process file'
        p = create_project(sys.argv[-1])
        p.wait()
        sys.exit()

    work_files = []
    for path, dirnames, filenames in os.walk(work_dir):
        for filename in filenames:
            if not filename.endswith('.cpp'):
                continue
            work_files.append(os.path.join(path, filename))

    work_process = []
    for fname in work_files:
        p = create_project(fname)
        work_process.append(p)

    for p in work_process:
        p.wait()
