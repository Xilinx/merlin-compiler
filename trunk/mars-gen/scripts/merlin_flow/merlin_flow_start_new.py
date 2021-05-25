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

from multiprocessing import Process
import os
import sys
import shutil
import time
import resource

space = " "
arg_list = sys.argv

frontground_execution = 1
arg_str = ""
for arg in arg_list[2:]:
    if arg == "-regression" or arg == "-front" or arg == "-f":
        frontground_execution = 1
    elif arg == "-background" or arg == "-b":
        frontground_execution = 0
    elif arg == "-hls":
        arg_str += " hls"
    elif arg == "-impl":
        arg_str += " bit"
    elif arg == "impl":
        arg_str += " bit"
    elif arg == "-bit":
        arg_str += " bit"
    elif arg == "-test":
        arg_str += " test"
    elif arg == "-host":
        arg_str += " host"
    elif arg == "-sim":
        arg_str += " sim"
    else:
        arg_str += " " + arg

if (arg_list[1] == "-v"):
    os.system(
        "mars_perl ${MARS_BUILD_DIR}/scripts/merlin_flow/merlin_flow_top.pl -v |& tee merlin.log ")
    sys.exit()

################################
# The project argumnet can be a directory or a prj file

project_arg = arg_list[1]
flow_arg = "opt"
if (len(arg_list) >= 3):
    flow_arg = arg_list[2]
    if (flow_arg[0] == "-"):
        flow_arg = "opt"
project_file = ""
if (os.path.isfile(project_arg)):
    project_file = project_arg
elif (os.path.isdir(project_arg)):
    project_abs_dir = os.path.abspath(project_arg)
    project_name = os.path.basename(project_abs_dir)
    project_file = os.path.join(project_arg, project_name + ".prj")

if (project_file == "" or (not os.path.isfile(project_file))):
    print("[merlin_flow] ERROR: project file or directory does not exist: " + project_arg + " \n")
#    print("[merlin_flow]        project file : "+project_file+" \n");
    sys.exit()

full_file = project_file
project_dir, project_file = os.path.split(full_file)

flow_log_file = "merlin_" + flow_arg + ".log"

os.system("rm -rf .merlin.finish")

def call_merlin_flow():
    start_time = time.time()
    if not os.path.exists(project_dir + "/report"):
        os.system("mkdir " + project_dir + "/report")
    if frontground_execution:
        os.system("cd " + project_dir + "; mars_perl ${MARS_BUILD_DIR}/scripts/merlin_flow/merlin_flow_top.pl " +
                  project_file + " " + arg_str + " |& tee " + flow_log_file)
    else:
        os.system("cd " + project_dir + "; mars_perl ${MARS_BUILD_DIR}/scripts/merlin_flow/merlin_flow_top.pl " +
                  project_file + " " + arg_str + " |& tee report/merlin.log &")
    # hack, add time and memory usage info here, because perl is not good at calling getrusage API
    ru = resource.getrusage(resource.RUSAGE_CHILDREN)
#*** Compilation finished
#    end_msg = '''
#
#Total time: {0:.2f} seconds
#Peak memory usage: {1:.2f} Mbyte\n'''.format(time.time() - start_time, ru.ru_maxrss / 1024.0)
#    print end_msg
#    if not os.path.exists(project_dir + "/report"):
#        os.system("mkdir " + project_dir + "/report")
#    with open(os.path.join(project_dir, 'report', 'perf_msg.log'), 'w') as f:
#        f.write(end_msg)
#    if os.path.isfile(os.path.join(project_dir, flow_log_file)):
#        with open(os.path.join(project_dir, flow_log_file), 'r') as src:
#            with open(os.path.join(project_dir, "report", "merlin.log"), 'a') as dst:
#                dst.write(src.read())
#                dst.write(end_msg)
##    os.system("rm -rf "+project_dir+"/*.log");


p1 = Process(target=call_merlin_flow)
p1.start()
#p2 = Process(target = func2)
# p2.start()

xml_file = project_dir + "/spec/directive.xml"
# print(xml_file)
import xml.etree.cElementTree as ET
tree = ET.parse(open(xml_file, "r"))
root = tree.getroot()
debug_flag = root.findall("ihatebug")
# print(debug_flag)
if debug_flag:
    print("")
#    print("Debug mode enabled\n")
else:
    try:
        while p1.is_alive():
            time.sleep(1)
    except KeyboardInterrupt:
        time.sleep(0.5)
        if os.path.exists(os.path.join(project_dir, 'implement', 'code_transform')):
            shutil.rmtree(os.path.join(project_dir, 'implement', 'code_transform'))
        if os.path.exists(os.path.join(project_dir, 'implement', 'opencl_gen')):
            shutil.rmtree(os.path.join(project_dir, 'implement', 'opencl_gen'))
        print('\nPressed Ctrl+C! Merlin Flow end!\n')
        sys.exit()
