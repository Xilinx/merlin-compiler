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

Filename    : optimizers/autodse/scripts/ds_proc_profiler.py
Description : This is the script for the DSE that profiling the design space.
Usage       : This script should not be called durectly. It should only used be
              imported and used by ds_proc_builder
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 05-06-2018 First created (Cody Hao Yu)
"""
from typing import List, Tuple # pylint: disable=unused-import
from multiprocessing import Pool
import os
import sys
import json
import re
from argparse import Namespace # pylint: disable=unused-import
from util import run_command, get_cmd_output
from dse_util import write_json_to_file, prepare_case_dir
from dse_run_hls import DSERunHLS, HLSResult # pylint: disable=unused-import
from dse_run_hls import HLSRetCode, apply_cfg_and_run_hls
import merlin_logger

MLOGGER = merlin_logger.get_logger('DS_Proc_Profiler')

# User specific work space prefix
PROFILE_PREFIX = 'dse_{0}_profile'.format(get_cmd_output('whoami'))

##############################################################
# Environment setting
##############################################################

def exit_ds_proc_profiler(exit_code):
    # type: (int) -> None
    """Exit the DSE profiler with assigned code"""
    sys.exit(exit_code)

#####################################
# Functions
#####################################

def summarize_hls_results(codes):
    # type: (List[int]) -> int
    """
    Check HLS result and provide hints if all failed
    """
    err_msg_map = {21: 28, 23: 29, 24: 30}
    code_dict = {0: 0}
    for code in codes:
        if code not in code_dict:
            code_dict[code] = 0
        code_dict[code] += 1

    if code_dict[0] > 0:
        MLOGGER.info(25, code_dict[0])

    for code in range(19, 25):
        if code in code_dict and code_dict[code] > 0:
            MLOGGER.info(code, code_dict[code])

    for code in range(19, 25):
        if (code in code_dict and code_dict[code] == len(codes) and
                code in err_msg_map):
            MLOGGER.error(err_msg_map[code])
            MLOGGER.fcs_lv2('Check {0} for details'.format(
                '.merlin_prj/run/implement/dse/profile/ds_part*'))
    return code_dict[0]

def check_hls_result(hls, hls_result):
    # type: (DSERunHLS, HLSResult) -> int
    """
    Check HLS result, clean the folder and return
    message code or 0 if success
    """
    if hls_result is None:
        return 19
    if hls_result.error_code == HLSRetCode.MERLIN_ERROR:
        hls.clean(1)
        return 20
    if hls_result.error_code == HLSRetCode.HLS_TIME_OUT:
        hls.clean(1)
        return 21
    if hls_result.error_code == HLSRetCode.HLS_ERROR:
        hls.clean(1)
        return 22
    if hls_result.error_code == HLSRetCode.HLS_RESULT_UNDEF:
        hls.clean(1)
        return 23
    if hls_result.error_code == HLSRetCode.HLS_NO_RESOURCE:
        hls.clean(1)
        return 24
    return 0

def read_kernel_file(src_path, kernel_file, encrypt):
    # type: (str, str, bool) -> List[str]
    """
    Open a kernel file and read all contents
    """
    curr_file_data = ['']

    file_path = '{0}/lc/{1}'.format(src_path, kernel_file)
    if not os.path.exists(file_path):
        MLOGGER.error(23, file_path)
        exit_ds_proc_profiler(1)

    if encrypt:
        run_command('merlin_safe_exec cp {0} {0}.tmp :: {0} :: 1'
                    .format(file_path))
    else:
        run_command('cp {0} {0}.tmp'.format(file_path))

    curr_r_file = open('{0}.tmp'.format(file_path), 'r')
    for line in curr_r_file:
        curr_file_data.append(line)
    curr_r_file.close()
    return curr_file_data

def profile_design_space_list(ds_file_list, args):
    # type: (List[str], Namespace) -> None
    """
    Profile a design space list with multiple partitions in parallel
    """
    pool = Pool(processes=len(ds_file_list))
    ret = pool.map(profile_design_space, [(ds, args) for ds in ds_file_list])
    pool.close()

    # Check the result
    if summarize_hls_results(ret) == 0:
        if args.type == 'full':
            MLOGGER.error(27)
        else:
            MLOGGER.error(25)
        exit_ds_proc_profiler(1)

def profile_design_space(packed_args):
    # type: (Tuple[str, Namespace]) -> int
    """
    Profile the design space using HLS
    """
    design_space_file, args = packed_args

    with open(design_space_file, 'r') as filep:
        content = json.loads(filep.read())
        ds_dict, cfg = content[0:2]

    # Prepare the workspace
    work_dir = prepare_case_dir(args.path, args.profile_dir, PROFILE_PREFIX)

    # Run HLS
    tmp_args = {'hls-time-limit': args.hls_time_limit,
                'impl-tool': args.impl_tool,
                'encrypt': args.encrypt}
    (hls, hls_result, _) = apply_cfg_and_run_hls(cfg, work_dir, tmp_args)

    # Backup
    tokens = re.search(r'ds_(.+).json$', design_space_file)
    tag = tokens.group(1) if tokens is not None else 'unknown_part'
    run_command('mv -f {0} {1}/profile/ds_{2}'.format(work_dir, args.path, tag))

    # Check results
    ret = check_hls_result(hls, hls_result)
    if ret == 0:
        ds_dict['metadata_prev-eval-cfg'] = {'cfg': cfg,
                                             'latency': hls_result.latency,
                                             'resource': hls_result.res_detail}
        if not write_json_to_file(content, design_space_file):
            MLOGGER.error(26)
    return ret
