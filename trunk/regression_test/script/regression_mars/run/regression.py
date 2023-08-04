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
import os
import sys
import re


if len(sys.argv) != 4 and len(sys.argv) != 5:
    print "Usage: regression.py case.list test.py cfg.xml"
    exit()

case_list = sys.argv[1]
test_script = sys.argv[2]
cfg_file = sys.argv[3]

keep_dir = 0
if (len(sys.argv) == 5):
    if (sys.argv[4] == "-k"):
        keep_dir = 1


def run_command(cmd):
    "print and execute the command"
    os.system(cmd)
    return


case_list_file = open(case_list)
case_lines = case_list_file.readlines()

work_dir = "work"

if (keep_dir == 0):
    os.system("rm -rf " + work_dir)

if (not os.path.isdir(work_dir)):
    os.mkdir(work_dir)

for one_case_p in case_lines:

    # 1. remove comments
    one_case = one_case_p.strip()
    one_case = one_case.split("#")[0]
    one_case = one_case.strip()

    if len(one_case) == 0:
        continue

    # 2. print case info
    curr_time = os.popen("date +'%b%d %H:%M:%S'").read()
    sys.stdout.write(one_case + " [" + curr_time.strip() + "] ... ")
    sys.stdout.flush()

    source_dir = one_case
    one_case = one_case.split("/")[-1]
    case_dir = work_dir + "/" + one_case
    one_case_log = case_dir + "/" + one_case + ".log"

    # 3. Create directory for regression test
    if (keep_dir == 0 or not os.path.isdir(case_dir)):
        run_command("cp -r " + source_dir + " " + work_dir)

    run_command("cp " + test_script + " " + case_dir)
    run_command("cp mars_common.py " + case_dir)
    if os.path.isfile(cfg_file):
        run_command("cp " + cfg_file + " " + case_dir)
    run_command("rm -rf " + case_dir + "/pass.o")
    curr_log = work_dir + "/curr_case.log"
    os.system("rm -rf " + curr_log)
    os.system("ln -s " + one_case + "/" + one_case + ".log " + curr_log)

    # 4. execution regression
    case_tag = one_case
    case_tag = re.sub("no_bit_test", 'csim', case_tag)
    case_tag = re.sub("config", 'c',         case_tag)
    run_command("cd " + case_dir + "; python " + test_script + " " +
                case_tag + " " + cfg_file + " >& " + one_case + ".log")
    ret = os.system("grep ERROR: " + one_case_log + " >> " + work_dir + "/error.log")
    if os.path.isfile(case_dir + "/pass.o") and ret:
        if (re.match(".*_cloud", test_script)):
            print "  Submitted"
        else:
            print "  Pass"
    else:
        print "  Failed"
