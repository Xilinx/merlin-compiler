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

import xml.etree.ElementTree as ET
import os
import sys
import re

###########################################################################
# Function Declaration
###########################################################################
def indent(elem, level=0):
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level + 1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def is_abs_dir(dir):
    return dir[0] == '/'

def xml_gen(src_dir, aux_dir, spec_dir, project_name):
    root_elem = ET.Element("project")
    root_elem.attrib["name"] = project_name
    src_elem = ET.SubElement(root_elem, "source_file_set")
    aux_elem = ET.SubElement(root_elem, "aux_file_set")

    if (src_dir != ""):
        prefix = "" if is_abs_dir(src_dir) else "../"
        for root, dirs, files in os.walk(src_dir):
            for file in files:
                if (file.startswith("#") or file.startswith(".") or file.endswith("~")
                    or file.endswith(".c") or file.endswith(".cpp")
                    or file.endswith(".C") or file.endswith(".cxx")
                    or file.endswith(".c++") or file.endswith(".cc")
                    or file.endswith(".H") or file.endswith(".h++")
                    or file.endswith(".hxx") or file.endswith(".hh")
                        or file.endswith(".h") or file.endswith(".hpp")):
                    ext = os.path.splitext(file)[-1]
                    fileelem = ET.SubElement(src_elem, "file")
                    fileelem.text = prefix + root + "/" + file
                    fileelem.attrib["type"] = ext[1:]
                else:
                    fileelem = ET.SubElement(aux_elem, "file")
                    fileelem.text = prefix + root + "/" + file
    else:
        fileelem = ET.SubElement(src_elem, "file")
        fileelem.text = "(src_file_to_add)"

    if (src_dir != aux_dir and aux_dir != ""):
        prefix = "" if is_abs_dir(aux_dir) else "../"
        for root, dirs, files in os.walk(aux_dir):
            for file in files:
                if not (file.startswith("#") or file.startswith(".") or file.endswith("~")
                        or file.endswith(".c") or file.endswith(".cpp")
                        or file.endswith(".C") or file.endswith(".cxx")
                        or file.endswith(".c++") or file.endswith(".cc")
                        or file.endswith(".H") or file.endswith(".h++")
                        or file.endswith(".hxx") or file.endswith(".hh")
                        or file.endswith(".h") or file.endswith(".hpp")):
                    fileelem = ET.SubElement(aux_elem, "file")
                    fileelem.text = prefix + root + "/" + file

    spec_file = ""
    if (spec_dir != ""):
        prefix = "" if is_abs_dir(spec_dir) else "../"
        for root, dirs, files in os.walk(spec_dir):
            for file in files:
                if file.endswith(".xml") and re.match(r'.*directive.*', file):
                    if (spec_file != ""):
                        print (
                            "[merlin_create_project] ERROR: Multiple directive found in spec directory.")
                        exit()
                    spec_file = prefix + root + "/" + file
    spec_elem = ET.SubElement(root_elem, "spec_file")
    spec_elem.text = spec_file

    indent(root_elem)
    ET.ElementTree(root_elem).write(project_name + "/" + project_name + ".prj")

def print_usage_exit():
    print "Usage: merlin_crate_project.py project_name [-s src_dir] [-p platform] [-copy]"
    print "       project_name : name of the project file and directory."
    print "       src_dir      : source code directory, including the data files to execute."
    print "       platform     : platform name, e.g. kintex7_leopard."
    print "       -copy        : indicate the source code is copied into the project."
    exit()

###########################################################################
# Main process
###########################################################################
# 1. Argument analysis
# 2. Generate project direcotry
# 3. Copy user files if specified
# 4. Copy default files
# 5. Create xml project file

# 1. Argument analysis
arg_src_dir = ""
arg_prj_name = ""
arg_is_copy = 0

all_argv = ""
for i in range(len(sys.argv)):
    curr_arg = sys.argv[i]
    if (i == 1):
        arg_prj_name = curr_arg
    if curr_arg == "-s":
        curr_value = sys.argv[i + 1]
        arg_src_dir = curr_value
    if curr_arg == "-copy":
        arg_is_copy = 1
    if i > 0:
        all_argv += " " + sys.argv[i]

if (arg_prj_name == ""):
    print_usage_exit()

# 2. Generate project direcotry
target_dir = arg_prj_name
if os.path.isdir(target_dir):
    print "[ERROR] Target directory \"" + target_dir + "\" exists."
    exit()
os.mkdir(target_dir)

# 3. Copy user files if specified
src_dir = arg_src_dir
aux_dir = arg_src_dir
if (arg_is_copy and arg_src_dir != ""):
    src_dir_new = target_dir + "/src"
    aux_dir_new = target_dir + "/aux"
    os.mkdir(src_dir_new)
    os.mkdir(aux_dir_new)
    os.system("cp " + arg_src_dir + "/*.c   " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.cpp " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.c++ " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.C " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.cxx " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.cc " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.h   " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.H   " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.hh   " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.hxx  " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.h++  " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.hpp " + src_dir_new + " >& /dev/null")
    os.system("cp " + arg_src_dir + "/*.dat* " + aux_dir_new + " >& /dev/null")
    src_dir = src_dir_new
    aux_dir = aux_dir_new

spec_dir = target_dir + "/spec"
os.system("mkdir " + spec_dir)
os.system("cd " + target_dir +
          "; cp $MERLIN_COMPILER_HOME/mars-gen/default/directive_mars_basic.xml spec/directive.xml")

curr_dir = os.getcwd()
os.system("cd " + target_dir + "/spec; mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/project/parse_project_command.pl " +
          curr_dir + "\/" + target_dir + "/spec/directive.xml \"" + all_argv + "\" " + curr_dir + " " + arg_src_dir)

# 5. Create xml project file
xml_gen(src_dir, aux_dir, spec_dir, arg_prj_name)
