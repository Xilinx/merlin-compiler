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
user_h = re.compile(".*#\s*include\s*\"(.*)\".*")
system_h = re.compile(".*#\s*include\s*<(.*)>.*")
begin_prefix = "// @__merlin_begin ";
end_prefix = "// @__merlin_end ";
first_line = "// @__merlin_first_line";
begin_patten = re.compile(begin_prefix + "(.*)")
end_patten = re.compile(end_prefix + "(.*)")
first_patten = re.compile(first_line)
line_directive = re.compile("# [0-9].*")
tmp_file_name = "__merlin_preprocess_tmp.tt"
compiler_option = ""
for arg in sys.argv[1:]:
    if not os.path.isfile(arg):
      compiler_option = compiler_option + " " + arg
      continue
    if re.match(r".*\.(c|cc|cpp|C|c\+\+|cxx)$", arg) == None:
      compiler_option = compiler_option + " " + arg
      continue

for arg in sys.argv[1:]:
    if not os.path.isfile(arg):
      continue
    if re.match(r".*\.(c|cc|cpp|C|c\+\+|cxx)$", arg) == None:
      continue
    #insert begin/end to mark the begin and end of header file
    tmp_file = open(tmp_file_name, "w")
    file_name = arg
    tmp_file.write(first_line + "\n");
    with open(file_name) as fileobject:
        for line in fileobject:
            if user_h.match(line) != None or system_h.match(line) != None :
              tmp_file.write( begin_prefix+ line)
              tmp_file.write(line);
              tmp_file.write( end_prefix + line)
            else:
              tmp_file.write(line);
    tmp_file.close()
    #use gcc to remove marcro directive
    os.rename(tmp_file_name, file_name)
    os.system("gcc -E -fdirectives-only -dDI -C " + compiler_option + " " + file_name + " -o " + tmp_file_name)
    os.rename(tmp_file_name, file_name)
    #wrap header files again
    tmp_file = open(tmp_file_name, "w")
    meet_begin_patten = False
    meet_first_patten = False
    with open(file_name) as fileobject:
        for line in fileobject:
            if not meet_first_patten:
              m = first_patten.match(line)
              if m != None:
                meet_first_patten = True
              continue

            m = begin_patten.match(line)
            if m != None:
               meet_begin_patten = True
               tmp_file.write(m.group(1) + "\n")
               continue
            if end_patten.match(line):
              meet_begin_patten = False
              continue
            elif meet_begin_patten:
               continue
            elif line_directive.match(line) != None:
              continue
            else:
              tmp_file.write(line)
    tmp_file.close()
    os.rename(tmp_file_name, file_name)
    
if os.path.isfile(tmp_file_name) :
    os.remove(tmp_file_name)
