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

"""
Python Script in Merlin Compiler

Filename    : optimizers/autodse/scripts/util.py
Description : This is the general utilization library for the DSE flow.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-24-2017 First pull request (Cody Hao Yu)
"""
from __future__ import print_function
from typing import List, Optional # pylint: disable=unused-import
from subprocess import Popen, PIPE, STDOUT
import os

def must_use_pyc(path):
    # type: (str) -> str
    """Find the pyc file by given the py file"""
    pyc_path = path + 'c'
    if not os.path.exists(pyc_path):
        print('Cannot find {0} for execution'.format(pyc_path))
        raise_merlin_error()
    return pyc_path

def get_cmd_output(cmd):
    # type: (str) -> str
    """Run the command and return the output"""
    try:
        proc = Popen([cmd], stdin=PIPE, stdout=PIPE, stderr=PIPE, shell=True)
        out, _ = proc.communicate()
        if out != '':
            out = out[:-1]
        return out
    except Exception as err:
        print('Error when running command %s: %s', cmd, str(err))
        raise

def run_command(cmd, no_print=False):
    # type: (str, bool) -> int
    """Run the command"""
    if no_print is True:
        fnull = open(os.devnull, 'w')
        proc = Popen([cmd], shell=True, stdout=fnull, stderr=STDOUT,
                     preexec_fn=os.setsid)
        proc.communicate()
    else:
        proc = Popen([cmd], shell=True, preexec_fn=os.setsid)
        proc.communicate()
    return proc.returncode

def raise_merlin_error():
    # type: () -> None
    """Mark as the Merlin flow error"""
    run_command('touch .merlin_flow_has_error')

def has_merlin_error(path='.'):
    # type: (str) -> bool
    """Check if it has Merlin flow error"""
    return (os.path.exists('{0}/.cmost_flow_has_error'.format(path)) or
            os.path.exists('{0}/.merlin_flow_has_error'.format(path)))

def get_src_files(path):
    # type: (str) -> List[str]
    """Get kernel source files in the path"""
    return [f for f in os.listdir(path)
            if os.path.splitext(f)[1] in ['.c', '.cc', '.cpp', '.h', '.hpp']]

def get_c_files(path):
    # type: (str) -> List[str]
    """Get C/C++ files in the path"""
    return [f for f in os.listdir(path)
            if os.path.splitext(f)[1] in ['.c', '.cc', '.cpp']]

def try_to_int(string, default=None):
    # type: (str, Optional[int]) -> Optional[int]
    """Try to convert a string to an integer. Return None when fail"""
    try:
        return int(string)
    except ValueError:
        return default

def to_int(string):
    # type: (str) -> int
    """Convert a string to an integer. Throw exception when fail"""
    try:
        num = int(string)
    except ValueError:
        print('Cannot covert {0} to an integer'.format(string))
        raise_merlin_error()
    return num

def try_to_float(string, default=None):
    # type: (str, Optional[float]) -> Optional[float]
    """Try to convert a string to a floating point. Return None when fail"""
    try:
        return float(string)
    except Exception:
        return default

def to_float(string):
    # type: (str) -> float
    """Convert a string to a floating point. Throw exception when fail"""
    try:
        num = float(string)
    except ValueError:
        print('Cannot covert {0} to a float'.format(string))
        raise_merlin_error()
    return num
