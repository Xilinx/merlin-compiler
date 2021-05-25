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

#!/usr/bin/python
#youxiang 2016-09-23
#this tool is used to preprocess all the users' included files
#into *.c/cpp files to work around ROSE backend limitation
#
# Usage: 
# python preprocess.py file1 file2 ... filen
import re
import os
import sys
user_h = re.compile("\s*#\s*include\s*\"(.*)\"")
system_h = re.compile("\s*#\s*include\s*<(.*)>")
tmp_file_name = "__merlin_preprocess_tmp.tt"
for file_name in sys.argv[1:]:
    if not os.path.isfile(file_name):
      continue
    while 1 :
        flag = 0
        tmp_file = open(tmp_file_name, "w")
        with open(file_name) as fileobject:
            for line in fileobject:
                m = user_h.match(line)
                header = ""
                if m != None:
                  header = m.group(1)
                m = system_h.match(line)
                if m != None:
                    header = m.group(1)
                if header == "" or (not os.path.isfile(header)):
                    tmp_file.write(line)
                    continue;
                header_file = open(header)
                tmp_file.write(header_file.read())
                header_file.close()
                flag = 1
        tmp_file.close()
        if not flag : 
            break
        tmp_file = open(tmp_file_name);
        input_file = open(file_name, "w")
        input_file.write(tmp_file.read())
        input_file.close()
        tmp_file.close()
if os.path.isfile(tmp_file_name) :
    os.remove(tmp_file_name)
