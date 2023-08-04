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
import subprocess
import multiprocessing

run_path = []
with open('file_list') as f:
    for p in f:
        if p.startswith('#') or not p.strip():
            continue
        run_path.append(os.path.join(os.curdir, p.strip()))

write_err = multiprocessing.Lock()

def worker(path):
    with open(os.devnull, 'w') as f:
        ret = subprocess.call(['make', 'test'], stdout=f, stderr=subprocess.STDOUT, cwd=path)
        if ret == 0:
            print('{0} is done'.format(path))
        else:
            print('{0} is failed'.format(path))
            write_err.acquire()
            with open('err.log', 'a') as err_log:
                err_log.write('{0} is failed.\n'.format(path))
            write_err.release()

if '18.1' in os.environ.get('AOCL_BOARD_PACKAGE_ROOT', ''):
    pool = multiprocessing.Pool()
    pool.map(worker, run_path)
else:
    with open('err.log', 'a') as f:
        f.write('not supported AOCL_BOARD_PACKAGE_ROOT version: ' + os.environ.get('AOCL_BOARD_PACKAGE_ROOT'))

