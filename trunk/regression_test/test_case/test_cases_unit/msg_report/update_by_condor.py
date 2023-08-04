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
import stat
import shutil
import getpass
import subprocess

#work_dir_name = 'update'
work_dir_name = ''

def update_case(path):
    work_dir = os.path.join(path, work_dir_name)
    if os.path.exists(work_dir):
        shutil.rmtree(work_dir)
    os.mkdir(work_dir)
    for f in os.listdir(path):
        if f in ['run', 'update', work_dir_name + '.condor_output']:
            continue
        if f.startswith('mars'):
            continue
        src = os.path.join(path, f)
        if os.path.isdir(src):
            shutil.copytree(src, os.path.join(work_dir, f))
        else:
            shutil.copy(src, work_dir)
    script_content = '''#! /bin/bash
source /curr/{user}/fcs_setting64.sh
pwd
make -C {work_dir_name} test
make -C {work_dir_name} update'''.format(user = getpass.getuser(),
        work_dir_name = work_dir_name)
    script_name = 'run.sh' 
    script_file = os.path.join(work_dir, script_name)
    with open(script_file, 'w') as f:
        f.write(script_content)
    st = os.stat(script_file)
    os.chmod(script_file, st.st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    input_content = work_dir_name
    output_content = work_dir_name
    job_title = '{0}'.format(os.path.abspath(path)[-5:])
    print(' '.join(['mars_exec_cloud', '-i', input_content, '-o', output_content, '-t', job_title, os.path.join(work_dir_name, script_name)]))
    subprocess.call(['mars_exec_cloud', '-i', input_content, '-o', output_content, '-t', job_title, os.path.join(work_dir_name, script_name)], cwd = path)

def update(file_list):
    with open(file_list) as f:
        for p in f:
            if p.startswith('#') or not p.strip():
                continue
            path = os.path.join(os.curdir, p.strip())
            print(path)
            update_case(path)

def copy_back_case(src, dst):
    for f in os.listdir(os.path.join(src, work_dir_name)):
        if not f.endswith('.ref') and not f == 'run.tgz':
            continue
        print(os.path.join(src, work_dir_name, f))
        print(dst)
        shutil.copy(os.path.join(src, work_dir_name, f), dst)

def copy_back(file_list):
    with open(file_list) as f:
        for p in f:
            if p.startswith('#') or not p.strip():
                continue
            src = os.path.join(os.curdir, p.strip())
            dst = os.path.join(os.environ.get('MERLIN_COMPILER_HOME'), 'regression_test/test_case/test_cases_unit/msg_report', p.strip())
            copy_back_case(src, dst);

if __name__ == '__main__':
    file_list = os.curdir
    if len(sys.argv) >=3:
        file_list = sys.argv[-1]
    if sys.argv[1] == 'update':
        update(file_list)
    elif sys.argv[1] == 'copy_back':
        copy_back(file_list)
