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
import re

def clean_report(path):
    with open(path, 'r+') as f:
        lines = f.readlines()
        pro_lines = []
        for line in lines:
            if line.startswith('Report time'):
                continue
            m = re.match(r'\|(.*)\|(\d|\s)+\|.+\|.+\|(\d|\s|\.)+\|(\d|\s|\.)+\|(\d|\s|\.)+\|(\d|\s|\.)+\|(.+)\|', line)
            if m:
                pro_line = '|{0}| | | | | | | |{1}|\n'.format(m.group(1), m.group(7))
                pro_lines.append(pro_line)
                continue
            m = re.match(r'\|(.*)\|(\d|\s)+\|(.*)\|', line)
            if m:
                digit = m.group(3)
                digit = digit[:digit.find('.')]
                pro_line = '|{0}|{1}|{2}|\n'.format(m.group(1), m.group(2), digit)
                pro_lines.append(pro_line)
                continue
            m = re.match(r'\|(.*)\|(\d|\s|\.)+\|(\d|\s|\.)+\|(\d|\s|\.)+\|(\d|\s|\.)+\|', line)
            if m:
                pro_line = '|{0}| | | |\n'.format(m.group(1))
                pro_lines.append(pro_line)
                continue
            pro_lines.append(line)
        f.truncate(0)
        f.seek(0)
        f.writelines(pro_lines)
              
if __name__ == '__main__':
    fpath = sys.argv[-1]
    clean_report(fpath)
