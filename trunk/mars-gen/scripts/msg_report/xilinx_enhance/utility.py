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

'''
This is used for common function
'''
import os
#import re
import shutil
import json
from collections import OrderedDict
#import subprocess
import merlin_report_setting

#pylint: disable=print-statement
def get_src_files_from_dir(directory):
    '''
    get source code file list from directory
    '''
    srcs = []
    for filename in os.listdir(directory):
        if filename.endswith(('.c', '.cpp')):
            srcs.append(filename)
    return srcs

def dump_to_json(json_file, content, mode="default"):
    '''
    dump json file
    '''
    with open(json_file, 'w') as fileh:
        if mode == "default":
            json.dump(content, fileh, sort_keys=True, indent=4)
        elif mode == "nosort":
            json.dump(content, fileh, sort_keys=False, indent=4)

def load_json(json_file, mode="default"):

    '''
    load json file
    '''
    # if json file have illegal content, need to transfet to utf-8 format
    # else, it will not load json file correctly
    file1 = json_file
    file2 = "tmp_load.json"
    with open(file1, "rb") as source:
        with open(file2, "wb") as target:
            source_encoding = "iso-8859-1"
            target_encoding = "utf-8"
            target.write(source.read().decode(source_encoding).encode(target_encoding))
    shutil.copyfile(file2, file1)

    data_set = {}
    with open(json_file, 'r') as fileh:
        if mode == "default":
            data_set = json.load(fileh, object_pairs_hook=OrderedDict)
        elif mode == "nosort":
            data_set = json.load(fileh)
        return data_set

def rm_and_mkdir(path):
    '''
    delete and create directory
    '''
    shutil.rmtree(path, ignore_errors=True)
    os.makedirs(path)

def exec_rose_pass(work_dir, pass_name, srcs, * args):
    '''
    this function give a common way to call rose pass
    '''
    settings = merlin_report_setting.MerlinReportSetting()
    # get macro options
    macro_flags = []
    if settings.xml_macro_define is not None:
        #macro_flags.append(settings.xml_macro_define)
        macro_flags = ['-a', 'cflags="{0}"'.format(settings.xml_macro_define)]
    # the order of source file will affact the order of node in AST
    # should keep all source file in same order
    srcs.sort()
    # include directory
    include_path = ["-I."]
    #includes = [os.path.dirname(s) for s in srcs]
    # for one_include in includes:
    #     if os.path.isdir(one_include):
    #         include_path.append("-I")
    #         include_path.append(one_include)
    # #include_path = include_path + settings.xml_include_path

    # get commandlines
    cmd = [settings.bin_mars_opt, "-e", "-c", "-p", pass_name]
    cmd += srcs
    cmd += ["-a", "impl_tool=" + settings.xml_impl_tool]
    cmd += macro_flags
    cmd += include_path
    for arg in args:
        if arg:
            cmd.append(arg)
    print("cmd = " + ' '.join(cmd))
    with open(os.path.join(work_dir, settings.run), 'w') as file_handler:
        file_handler.write(' '.join(cmd))
    cmd += [" > " + pass_name + ".log"]
    cmd = ["merlin_safe_exec"] + cmd
    cmd += [" :: "]
    cmd += srcs
    cmd += [" :: "]
    if settings.xml_encrypt != "on" or pass_name == settings.pass_gen_hierarchy:
        cmd.append("0")
    else:
        cmd.append(settings.rose_timeout_read_files)

    # execute commandline and check output
    # subprocess.Popen only support list as command input
    os.system("cd " + work_dir + ";" + " ".join(cmd))
    # if use pOpen, then cflags setting issues
    #with open(settings.log_id_update, 'w') as file_handler:
    #    #process = subprocess.Popen(cmd, stdout=file_handler, stderr=file_handler)
    #    #process.communicate()
    #    file_handler.seek(0)
    if not os.path.isfile(os.path.join(work_dir, settings.rose_succeed)):
        return False
    return True

def is_number(string):
    '''
    the fucntion is to judge a string is number or not
    the float format is included
    '''
    tmp = str(string)
    if tmp.count('.') == 1:
        tmp_s = tmp.split('.')
        left = tmp_s[0]
        right = tmp_s[1]
        return left.isdigit() and right.isdigit()
    else:
        return tmp.isdigit()
#if __name__ == '__main__':
#    SRC_FILES = []
#    for f in os.listdir("lc"):
#        if f.endswith(('.c', '.cpp')):
#            SRC_FILES.append("lc/" + f)
#    exec_rose_pass(".", "gen_token_id", SRC_FILES, \
#                   "-a kernel_list=lc/kernel_list.json", \
#                   "-a output=output.json")
