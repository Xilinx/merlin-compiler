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

Filename    : optimizers/autodse/scripts/dse_midend_checker.py
Description : This file contains a set of methods to check Merlin
              midend passes to see if it performs as expected
Usage       : Internal call.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 01-17-2019 Created (Cody Hao Yu)
"""
from typing import Dict, List # pylint: disable=unused-import
import sys
import json
import glob

def fetch_messages(data):
    # type: (Dict[str, Dict[str, str]]) -> List[str]
    """
    Fetch 'messages' from the pass log
    """
    ret = [] # type: List[str]
    if not data:
        return ret
    entries = data.values()
    for entry in entries:
        if 'messages' not in entry: # No debug info
            continue
        ret += [msg for msg in entry['messages'] if msg.find('DebugInfo') == -1]
    return ret

def collect_midend_failures(work_dir):
    # type: (str) -> List[str]
    """
    Parse the critical message report from midend
    and use all checker methods defined in this module
    to check all failures
    """
    fails = set()

    # Fetch messages from midend passes
    msgs = [] # type: List[str]
    history_list = glob.glob('{0}/history_*.json'.format(work_dir))
    for history in history_list:
        with open(history, 'r') as filep:
            data = json.load(filep)
        msgs += fetch_messages(data)

    # Get checker list from the module
    checker_list = [func for func in dir(sys.modules[__name__])
                    if func.startswith('check_')]

    for checker_name in checker_list:
        pass_name = checker_name[6:]
        checker = getattr(sys.modules[__name__], checker_name)
        if not checker(msgs):
            fails.add('{0}_failed'.format(pass_name))
    return list(fails)

def check_memory_burst(msgs):
    # type: (str) -> bool
    """
    Check if memory burst is inferred as expected
    FIXME: Now we only check if Merlin failed to infer
    burst for any array instead of further checking
    the burst length
    """
    #tracking = False
    for msg in msgs:
        # pylint: disable=no-else-return
        if msg.find('\"burst": \"off') != -1:
            return False
        elif msg.find('Memory burst NOT inferred') != -1:
            return False
        #elif msg.find('\"burst": \"on') != -1:
        #    tracking = True
        #elif tracking and msg.find('critical_warning') != -1:
        #    tracking = False
        #    if msg.find('due to small external memory') == -1:
        #        return False
    return True

def check_coarse_parallel(msgs):
    # type: (str) -> bool
    """
    Check if coarse parallel is enabled as expected
    """
    for msg in msgs:
        if msg.find('\"coarse_grained_parallel": \"off') != -1:
            return False
    return True

def check_coarse_pipeline(msgs):
    # type: (str) -> bool
    """
    Check if coarse pipeline is enabled as expected
    """
    for msg in msgs:
        if msg.find('\"coarse_grained_pipeline": \"off') != -1:
            return False
    return True

def check_loop_parallel(msgs):
    # type: (str) -> bool
    """
    Check if fine-grained parallel is enabled as expected
    """
    for msg in msgs:
        if msg.find('\"fine_grained_parallel": \"off') != -1:
            return False
    return True

def check_bitwidth_opt(msgs):
    # type: (str) -> bool
    # pylint: disable=unused-argument
    """
    Check if memory coalescing is enabled as expected
    FIXME Now we cannot judge if the memory coalescing
    failure is due to incorrect pragmas, since it may
    be caused by the kernel program characters
    """
    #for msg in msgs:
    #    if msg.find('\"memory_coalescing": \"off') != -1:
    #        return False
    return True
