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

if __name__ == '__main__':
    file_list = 'case_list'
    if len(sys.argv) >=2:
        file_list = sys.argv[-1]
    with open(file_list) as f:
        for p in f:
            if p.startswith('#') or not p.strip():
                continue
            path = os.path.join(os.curdir, p.strip())
            if 'cycle_related' in path:
                shutil.copy('Makefile.example.cycle', os.path.join(path, 'Makefile'))
            elif 'extract_data' in path:
                shutil.copy('Makefile.example.extract_data', os.path.join(path, 'Makefile'))
            else:
                shutil.copy('Makefile.example.non-cycle', os.path.join(path, 'Makefile'))
