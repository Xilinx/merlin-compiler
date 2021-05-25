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

Filename    : optimizers/autodse/scripts/dse_util.py
Description : This is the library for the DSE that is called by
              the DSE Python scripts.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-24-2017 First pull request (Cody Hao Yu)
"""
import re
import os
import json
import tempfile
import numpy as np

from typing import Dict, Any, List, Tuple # pylint: disable=unused-import
from typing import Optional, Union # pylint: disable=unused-import
from dse_type import CFGType # pylint: disable=unused-import
from util import has_merlin_error, must_use_pyc, run_command
import merlin_logger

MLOGGER = merlin_logger.get_logger('DSE_Util')

# Constant values
MERLIN_COMPILER_HOME = os.environ['MERLIN_COMPILER_HOME']
APPLIER_PY = must_use_pyc('{0}/optimizers/'.format(MERLIN_COMPILER_HOME) +
                          'autodse/scripts/ds_proc_applier.py')
MERLIN_EXE_PASS_CMD = ('mars_perl {0}/mars-gen/scripts/merlin_flow/'
                       .format(MERLIN_COMPILER_HOME) +
                       'frontend_pass.pl . -src_list kernel.list')
DPT_DIR = 'ori_src/'
TRANS_DIR = 'code_transform_base/'
RPT_PATH = 'implement/exec/hls/report_merlin/perf_est.json'
SRC_DIR = 'implement/code_transform/'
OCL_DIR = 'implement/opencl_gen/'
HLS_DIR = 'implement/exec/hls/'
EXPORT_DIR = 'implement/export/'

def exit_dse_util(error_code):
    # type: (int) -> None
    """Exit dse util with assigned code"""
    import sys
    sys.exit(error_code)

def gen_merlin_pass_cmd(name, vendor='unknown'):
    # type: (str, str) -> str
    """Generate Merlin pass command"""
    return ('{0} -p {1} -a "-a impl_tool={2}"'
            .format(MERLIN_EXE_PASS_CMD, name, vendor))

def write_json_to_file(data, file_name):
    # type: (Dict[Any, Any], str) -> bool
    """Write a dict to a file in json. Return false if failed"""
    try:
        filep = open(file_name, 'w')
        filep.write(json.dumps(data, indent=2, sort_keys=True))
        filep.close()
    except Exception as e: # pylint: disable=C0103
        MLOGGER.error(4, file_name, str(e))
        return False
    return True

def get_ds_id_from_auto(auto):
    # type: (str) -> Optional[str]
    """
    Fetch the ID from an auto block
    """
    tokens = re.search(r'options *: *(.+?) *(=|$|\})', auto, re.IGNORECASE)
    ds_id = None # type: Optional[str]
    if tokens is not None:
        try:
            ds_id = tokens.group(1).strip()
        except IndexError:
            MLOGGER.error(5, auto)
    return ds_id

def prepare_case_dir(root_dir, work_dir, prefix='tmp'):
    # type: (str, str, str) -> str
    """
    Make a new temp dir and copy the source file in the current dir to it.
    Return the new temp dir path
    """

    # Make temp folder
    path = tempfile.mkdtemp(prefix=prefix, dir='{0}/'.format(work_dir))
    run_command('mkdir -p {0}/{1}'.format(path, DPT_DIR))

    # Copy files
    run_command('rsync -av --progress --exclude="work" --exclude="profile" . ' +
                '{0}/{1} &> /dev/null'.format(path, DPT_DIR))
    run_command('cp {0}/directive.xml {1}/{2}'
                .format(root_dir, path, DPT_DIR))
    return path

def apply_config(path, cfg, encrypt):
    # type: (str, CFGType, bool) -> bool
    """
    Write the config to a json file and call the applier.
    Return False if unsuccess
    """

    # Write config to json
    if not write_json_to_file(cfg, '{0}/cfg.json'.format(path)):
        return False

    # Set applier command
    applier_cmd = ('python {0} {1}/cfg.json {1}'
                   .format(APPLIER_PY, path))

    if encrypt:
        applier_cmd += ' --encrypt'

    # Apply config
    run_command(applier_cmd)

    return True

def apply_midend(path):
    # type: (str) -> bool
    """Apply Merlin midend passes"""
    midend_cmd = 'merlin_midend'

    run_command('cd {0}; {1} -src . -dst . &> midend.log'
                .format(path, midend_cmd))

    if has_merlin_error(path):
        return False

    # Fetch pass list
    if not os.path.exists('{0}/midend.log'.format(path)):
        MLOGGER.fcs_lv2('ERROR: midend.log not found')
        return True
    with open('{0}/midend.log'.format(path), 'r') as logfile:
        run_command('touch {0}/history_passes.list'.format(path))
        for line in logfile:
            if line.find('Starting optimization step') != -1:
                tokens = line.replace('\n', '').split(' ')
                if len(tokens) < 5:
                    MLOGGER.fcs_lv2('ERROR: Fail to parse midend.log '
                                    'for pass list %s', line)
                else:
                    run_command('echo "{0}" >> {1}/history_passes.list'
                                .format(tokens[4], path))
    run_command('mv {0}/history_* {0}/metadata'.format(path))
    if os.path.exists('{0}/user_code_file_info.json'.format(path)):
        run_command('mv {0}/user_code_file_info.json {0}/metadata/'.format(path))
    return True

def config_to_string(cfg):
    # type: (CFGType) -> str
    """
    Transform a config to a string excluding metadata
    """
    string = ''
    for label, value in sorted(cfg.iteritems(), key=lambda p: p[0]):
        if label.startswith('metadata'):
            continue
        string += ('{0}\t'.format(label))
        if str(value) == '':
            string += 'n/a\t'
        else:
            string += '{0}\t'.format(value)
    return string

def transform_kernel(path, is_encrypt, cfg=None):
    # type: (str, bool, CFGType) -> Tuple[bool, str]
    """
    Apply a given configuration. Return a tuple with the first
    element indicates if the transformation was success or not
    """

    log_str = ''
    if cfg is not None:
        apply_config('{0}/{1}'.format(path, DPT_DIR), cfg, is_encrypt)

        # Record config
        log_str += config_to_string(cfg)
    else:
        log_str += 'default\t'
    log_str += '{0}\t'.format(path[path.rfind('/') + 1:])

    # Apply transformations with a specific config
    run_command('cp -rf {0}/{1} {0}/{2} &> /dev/null'
                .format(path, DPT_DIR, TRANS_DIR))
    if not apply_midend('{0}/{1}'.format(path, TRANS_DIR)):
        return (False, log_str)

    return (True, log_str)

def find_pareto_points(_data):
    # type: (List[Tuple[int, float]]) -> List[Tuple[int, float]]
    """
    Identify Pareto points by given a set of 2-D points
    """
    if not _data:
        return []

    data = sorted(_data, key=lambda p: p[0])

    pareto = []
    pivot = data[0]
    for point in data:
        if pivot[0] == point[0]: # Same latency
            if point[1] < pivot[1]: # Samller area
                pivot = point
        else: # Larger latency
            if point[1] < pivot[1]:
                pareto.append(pivot)
                pivot = point
    if not pareto or pivot != pareto[-1]:
        pareto.append(pivot)
    return pareto

def draw_pareto_curve(points):
    # type: (List[Tuple[int, float]]) -> Tuple[Optional[str], Optional[str]]
    """
    Identify and draw Pareto curve to both TXT and PDF files
    """
    try:
        import gnuplotlib as gp
    except NameError:
        MLOGGER.warning(15)
        return (None, None)

    pareto_set = find_pareto_points(points)
    path = os.getcwd()

    try:
        # Print to TXT
        gp.plot((np.array([p[0] for p in pareto_set]),
                 np.array([p[1] for p in pareto_set]),
                 {'with': 'linespoints pt 15',
                  'legend': '  Pareto curve'}),
                (np.array([p[0] for p in points]),
                 np.array([p[1] for p in points]),
                 {'with': 'points pt 0', 'legend': 'Explored points'}),
                title='DSE Summary', xlabel='Latency', ylabel='Area',
                terminal='dumb 70, 35', output='{0}/summary.txt'.format(path),
                unset=['grid', 'xtics', 'ytics'])

        # Print to PDF
        gp.plot((np.array([p[0] for p in points]),
                 np.array([p[1] for p in points]),
                 {'with': 'points lc "black" pt 6 ps 0.5',
                  'legend': 'Explored points'}),
                (np.array([p[0] for p in pareto_set]),
                 np.array([p[1] for p in pareto_set]),
                 {'with': 'linespoints lc "black" pt 7 ps 0.5',
                  'legend': 'Pareto curve'}),
                title='DSE Summary', xlabel='Latency', ylabel='Area',
                terminal='pdf', output='{0}/summary.pdf'.format(path),
                unset=['grid', 'xtics', 'ytics'])
    except Exception:
        MLOGGER.warning(15)
        return (None, None)
    return ('{0}/summary.txt'.format(path), '{0}/summary.pdf'.format(path))
