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

Filename    : optimizers/autodse/scripts/ds_proc_applier.py
Description : This is the script for the DSE that apply a design config to
              the kernel code.
Usage       : This script should only be used by ds_processor.
              Look at "Argument parsing" section in this script for the
              testing configuration.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 03-19-2018 First created (Cody Hao Yu)
"""
from typing import List, Dict # pylint: disable=unused-import
import argparse
import os
import sys
import re
import json
from util import run_command
from dse_util import get_ds_id_from_auto
import merlin_logger

MLOGGER = merlin_logger.get_logger('DS_Proc_Applier')

##############################################################
# Argument parsing
##############################################################

PARSER = argparse.ArgumentParser(description='Design Config Applier')
PARSER.add_argument('--encrypt', action='store_true',
                    default=False,
                    help='source code encryption')
PARSER.add_argument('design_config', action='store',
                    help='design config file in json')
PARSER.add_argument('path', action='store',
                    help='input kernel path')

#####################################
# Functions
#####################################

def exit_ds_proc_applier(exit_code):
    # type: (int) -> None
    """Perform postprocess to kernel files (e.g. encryption) and exit"""
    postproc_kernel_files(ARGS.path, ARGS.encrypt)
    sys.exit(exit_code)

def preproc_kernel_files(src_path, is_encrypted):
    # type: (str, bool) -> List[str]
    """Encrypt kernel files if needed"""
    kernel_list = '{0}/kernel.list'.format(src_path)

    if not os.path.exists(kernel_list):
        MLOGGER.error(6, kernel_list)
        return []

    # Create kernel file list (decrypt if needed)
    kernel_file_list = []
    with open(kernel_list, 'r') as kernel_list_file:
        for kernel_file in kernel_list_file:
            kernel_file = ('{0}/{1}'
                           .format(src_path, kernel_file.replace('\n', '')))
            if not os.path.exists(kernel_file):
                MLOGGER.error(6, kernel_file)
                return []

            if is_encrypted:
                run_command('merlin_safe_exec cp {0} {0}.tmp :: {0} :: 1'
                            .format(kernel_file))
            else:
                run_command('cp {0} {0}.tmp'.format(kernel_file))
            kernel_file_list.append('{0}.tmp'.format(kernel_file))

    return kernel_file_list

def postproc_kernel_files(src_path, is_encrypted):
    # type: (str, bool) -> None
    """Decrypt kernel files if needed"""
    kernel_list = '{0}/kernel.list'.format(src_path)

    if not os.path.exists(kernel_list):
        MLOGGER.error(6, kernel_list)
        return

    # Recover kernel file list (encrypt if needed)
    with open(kernel_list, 'r') as kernel_list_file:
        for kernel_file in kernel_list_file:
            kernel_file = ('{0}/{1}'
                           .format(src_path, kernel_file.replace('\n', '')))
            if not os.path.exists('{0}.tmp'.format(kernel_file)):
                MLOGGER.error(6, '{0}.tmp'.format(kernel_file))
                return

            if is_encrypted:
                run_command('merlin_safe_exec cp {0}.tmp {0} :: {0} :: 1'
                            .format(kernel_file))
            else:
                run_command('cp {0}.tmp {0}'.format(kernel_file))
    return

def apply_auto(line, target, value):
    # type: (str, str, str) -> str
    """
    Replace the target string with the value in line
    """
    start_idx = line.find(target)
    if start_idx == -1:
        return line
    end_idx = start_idx + len(target)
    return line[:start_idx] + str(value) + line[end_idx:]

def apply_design_config(work_dir, file_list, ds_config):
    # type: (str, List[str], Dict[str, str]) -> None
    """
    Apply the design config to the kernel files
    """
    for kernel_file in file_list:
        ori_file = open(kernel_file, 'r')
        new_file = open('{0}/.applier_tmp_kernel'.format(work_dir), 'w')

        line = ''
        for tmp_line in ori_file:
            tmp_line = tmp_line.replace('\n', '').rstrip()
            if not tmp_line:
                new_file.write('\n')
                continue
            elif tmp_line[-1] == '\\':
                # This line has not finished. Append it and keep reading
                if not line:
                    line = tmp_line[:-1]
                else:
                    line += ' ' + tmp_line[:-1]
                continue
            else:
                line += tmp_line

            if not line.startswith('//'): # Ignore comments
                autos = re.findall(r'(auto{.*?})', line, re.IGNORECASE)
                for auto in autos:
                    ds_id = get_ds_id_from_auto(auto)
                    if ds_id is None:
                        continue
                    line = apply_auto(line, auto, ds_config[ds_id])
            new_file.write(line + '\n')
            line = '' # Clean the line

        ori_file.close()
        new_file.close()
        run_command('mv {0}/.applier_tmp_kernel {1}'
                    .format(work_dir, kernel_file))

def load_design_config(cfg_file):
    # type: (str) -> Dict[str, str]
    """
    Read the design config file in json
    """
    ds_config = {} # type: Dict[str, str]
    with open(cfg_file, 'r') as filep:
        try:
            ds_config = json.loads(filep.read())
        except Exception as e: # pylint: disable=C0103
            MLOGGER.error(7, ARGS.design_config, e)
    return ds_config

##############################################################
# Main process
##############################################################

if __name__ == "__main__":
    ARGS = PARSER.parse_args()

    # Read inputs
    DS_CFG = load_design_config(ARGS.design_config)
    if DS_CFG is None:
        exit_ds_proc_applier(1)

    # Extract kernel file list (decrypt if needed)
    FILE_LIST = preproc_kernel_files(ARGS.path, ARGS.encrypt)

    # Apply config
    apply_design_config(ARGS.path, FILE_LIST, DS_CFG)

    # Encrypt back if needed
    postproc_kernel_files(ARGS.path, ARGS.encrypt)
