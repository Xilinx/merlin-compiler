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

import os, sys
import linecache
import re

is_display = 1;
if (len(sys.argv) < 2):	
    print("Usage: python addheader.py <relative path> [-s]\n");
    print("       -s: silent, no display                  \n");

if (len(sys.argv) > 2 and sys.argv[2] == "-s") :
    is_display = 0;

path = sys.argv[1]
if os.path.isdir(path):
    if (is_display == 1):
        print("process files in path: " + path)
else:
    print(path + " does not exist")
    exit(1)


scriptpath = os.path.dirname(os.path.realpath(sys.argv[0]))

def process(file, suffix):
    headerpath = scriptpath + "/" + file
    if os.path.isfile(headerpath) == False:
        print(headerpath + " does not exist.")
        exit(1)

    g = open(headerpath, 'r')
    g_content = g.readlines()
    for root, dirs, files in os.walk(path):
        for file in files: 
            if file.endswith(suffix):
				print(os.path.join(root, file))
				f = open(os.path.join(root, file), 'rw+')
				line1 = linecache.getline(os.path.join(root, file), 1).strip()
				line8 = linecache.getline(os.path.join(root, file), 8).strip()
				line9 = linecache.getline(os.path.join(root, file), 9).strip()
				f_content = f.readlines()
				f.seek(0,0)
				if(line1.find("/********") != -1 and line8.find("********/") != -1):
					#f_content = f.readlines()[8:]
					f.writelines(g_content + f_content[8:])
				elif(line1.find("/********") != -1 and line9.find("********/") != -1):
					f.writelines(g_content + f_content[9:])
				elif(line1.find("##########") != -1 and line8.find("#######") != -1):
					f.writelines(g_content + f_content[8:])
				else:
					#f_content = f.readlines()
					f.writelines(g_content + f_content)
#				f.writelines(g_content + f_content)
				f.close()
	g.close()

process("xilinx_script.txt",  ".pl")
process("xilinx_script.txt",  ".tcl")
process("xilinx_script.txt",  ".py")
process("xilinx_c.txt", ".c")
process("xilinx_c.txt", ".cpp")
process("xilinx_c.txt", ".h")
process("xilinx_c.txt", ".v")
