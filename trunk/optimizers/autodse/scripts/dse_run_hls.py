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

Filename    : optimizers/autodse/scripts/dse_run_hls.py
Description : This is the script for running HLS for the DSE that accepts
              the kernel code with a specific configuration and outputs
              the HLS results.
Usage       : New a class object and invoke buildin functions.
              Kernel source file path.
              HLS working directory.
              Platform name.
              HLS time limit (default 15 minutes).
              Keep temporary files or not (default not).
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 01-10-2018 Created (Cody Hao Yu)
"""
from typing import Dict, Union # pylint: disable=unused-import
from collections import OrderedDict, defaultdict
import os
import re
import signal
import subprocess
import traceback
import time
import json

from dse_util import TRANS_DIR, DPT_DIR
from dse_util import SRC_DIR, OCL_DIR, HLS_DIR, EXPORT_DIR
from dse_util import transform_kernel, config_to_string, write_json_to_file
from dse_mock import mock_load, mock_save
from util import get_src_files, run_command
from util import try_to_int, try_to_float
import merlin_logger

# Environment setup
MERLIN_COMPILER_HOME = os.environ['MERLIN_COMPILER_HOME']
OCL_CMD = ('mars_perl {0}/mars-gen/scripts/merlin_flow/final_code_gen.pl'
           .format(MERLIN_COMPILER_HOME))
HLS_CMD = ('mars_perl {0}/mars-gen/scripts/merlin_flow/merlin_backend.pl hls'
           .format(MERLIN_COMPILER_HOME))

BURST_THRESHOLD = 1024
RES_THRESHOLD = 0.8
INTEREST_CYCLE_THRESHOLD = 1

###############################################
# Xilinx Vivado HLS Dependent Settings Start
###############################################

# Extempt (QoR related) HLS errors
QOR_HLS_ERROR = ['XOCC 203-504']

# Ignored (no info) HLS errors
# XOCC 60-300: Failed to build kernel(ip)
# XOCC 60-599: Kernel compilation failed to complete
# XOCC 60-592: Failed to finish compilation
IGNORE_HLS_ERROR = ['XOCC 60-300', 'XOCC 60-599', 'XOCC 60-592']

###############################################
# Xilinx Vivado HLS Dependent Settings End
###############################################

MLOGGER = merlin_logger.get_logger('DSE_Run_HLS')

class HLSRetCode(object):  # pylint: disable=R0903
    """
    Evaluation return code
    """

    def __init__(self):
        pass

    EVAL_PASS = 0
    HLS_TIME_OUT = -1
    HLS_ERROR = -2
    HLS_RESULT_UNDEF = -3
    HLS_NO_RESOURCE = -4
    MERLIN_ERROR = -5
    DUPLICATE = -6

class HLSResult(object):  # pylint: disable=R0903
    """
    HLS result set
    """
    def __init__(self):
        self.error_code = HLSRetCode.EVAL_PASS
        self.source = 'hls' # Indicate the source of QoR numbers
                            # (hls or est)
        self.estimate = {'perf': False, 'res': False}
        self.res_detail = None
        self.res_abs_detail = None
        self.latency = 0.0
        self.time = 0.0
        self.memo = ''
        self.res_breakdown = dict()
        self.res_singleton = dict()
        self.name_topoid_map = defaultdict(list)
        self.kernel_func = ""
        self.parsed_hls_info = None
        self.model_accuracy = None

        # A list of crtitical tuples (scope topo id, is compute bounded)
        self.perf_btn = [] # type: List[Tuple[str, float]]

    def add_memo(self, key, msg):
        """
        Add memo in string type to the result
        Each message is separated by ;
        """
        full_msg = '{0}:{1}'.format(key, msg)

        if self.memo:
            self.memo += ';'
        if 1024 - len(self.memo) < len(full_msg):
            MLOGGER.debug('ERROR: Available memo space is less than'
                          '%d to add "%s"', len(full_msg), full_msg)
            MLOGGER.debug('Current memo: %s', self.memo)
            raise RuntimeError
        self.memo += full_msg

    def __str__(self):
        return ('Error_Code {0} Latency {1} '
                .format(self.error_code, self.latency) +
                'Time {0} Resource {1}'
                .format(self.time, self.res_detail))

class DSERunHLS(object):
    """
    The class for running HLS for DSE QoR
    """

    def set_time_out(self, timeout):
        """Set the timeout"""
        self.hls_time_limit = timeout

    def get_src_dir(self):
        """Get the directory for the kernel files"""
        return self.src_dir

    def get_work_dir(self):
        """Return the working directory"""
        return self.work_dir

    def get_ocl_dir(self):
        """Get the OpenCL directiory"""
        return self.ocl_dir

    def get_hls_dir(self):
        """Get the HLS running directiony"""
        return self.hls_dir

    def get_export_dir(self):
        """Get the export directory for HLS-ready code"""
        return self.exp_dir

    def is_setup(self):
        """Return if this object is initialized"""
        return self.setup

    def __init__(self, path, work_dir, hls_time_limit=900, impl_tool='sdaccel'):
        """Initialize the environment for running HLS"""

        self.setup = True

        # Setup working directory
        run_command('mkdir -p {0}/{{spec,{1}}}'.format(work_dir, SRC_DIR))
        run_command('cp -rf {0}/* {1}/{2}/ &> /dev/null'
                    .format(path, work_dir, SRC_DIR))

        # direcive.xml needs to be everywhre...
        if not os.path.exists('{0}/directive.xml'.format(path)):
            MLOGGER.debug('ERROR: We are at %s', os.getcwd())
            MLOGGER.debug('Cannot find directive.xml at %s', path)
            self.setup = False
            return

        run_command('cp {0}/directive.xml {1}/spec'.format(path, work_dir))
        run_command('cp {0}/directive.xml {1}'.format(path, work_dir))

        self.work_dir = work_dir
        self.src_dir = '{0}/{1}'.format(work_dir, SRC_DIR)
        self.ocl_dir = '{0}/{1}'.format(work_dir, OCL_DIR)
        self.hls_dir = '{0}/{1}'.format(work_dir, HLS_DIR)
        self.exp_dir = '{0}/{1}'.format(work_dir, EXPORT_DIR)

        self.pids = []
        self.result = None

        # Initialize kernel file list
        self.kernel_list = []

        # Setup HLS time limit and vendor tool
        self.hls_time_limit = int(hls_time_limit)
        self.impl_tool = impl_tool

        # Setup source files
        self.file_list = get_src_files(self.src_dir)
        self.file_list[:] = ['{0}/{1}'.format(self.src_dir, f)
                             for f in self.file_list]

        # Identify the kernel file
        if not os.path.exists('{0}/kernel.list'.format(self.src_dir)):
            MLOGGER.debug('ERROR: find kernel.list')
            self.setup = False
            return
        with open('{0}/kernel.list'.format(self.src_dir), 'r') as filep:
            for line in filep:
                if line.find('__merlinkernel') != -1:
                    self.kernel_list.append(line.rstrip())

        if not self.kernel_list:
            MLOGGER.debug('ERROR: Cannot find __merlinkernel in kernel.list')
            self.setup = False
            return

        return

    def gen_opencl(self):
        """
        Generate the HLS-ready code using the Merlin pass
        """

        src_dir = self.src_dir
        ocl_dir = self.ocl_dir

        # Make working folder
        if os.path.exists(self.ocl_dir):
            run_command('rm -rf {0}'.format(ocl_dir))
        run_command('mkdir -p {0}'.format(ocl_dir))

        # Copy directive
        run_command('cp {0}/directive.xml {1}'.format(src_dir, ocl_dir))

        # Apply opencl generation for HLS
        full_ocl_cmd = '{0} {1} {2}'.format(OCL_CMD, src_dir, ocl_dir)
        run_command('echo {0} > {1}/opencl_gen.log'
                    .format(full_ocl_cmd, ocl_dir))
        run_command('{0} &>> {1}/opencl_gen.log'
                    .format(full_ocl_cmd, ocl_dir))

        # Test if successd
        return os.path.exists('{0}/export'.format(ocl_dir))

    def run(self, metadata=None):
        """
        Run high-level synthesis
        metadata: A dict for reference results:
                  {perf, res}: est_info, code_change
        """
        self.result = HLSResult()

        src_dir = self.src_dir
        work_dir = self.work_dir
        ocl_dir = self.ocl_dir
        hls_dir = self.hls_dir
        exp_dir = self.exp_dir

        if not os.path.exists('{0}/export'.format(ocl_dir)):
            if not self.gen_opencl():
                MLOGGER.debug('ERROR: Fail to generate OpenCL: '
                              '%s/export is missing', ocl_dir)
                self.result.error_code = HLSRetCode.MERLIN_ERROR
                return HLSRetCode.MERLIN_ERROR

        run_command('cp -rf {0}/export {1}'.format(ocl_dir, exp_dir))
        run_command('touch {0}/is_dse_project'.format(exp_dir))

        # Make working folder
        run_command('mkdir -p {0}'.format(hls_dir))

        # Copy directive
        run_command('cp {0}/directive.xml {1}'.format(src_dir, hls_dir))

        # Generate estimated metadata if available
        metadata = {} if metadata is None else metadata
        for c_type in metadata:
            if metadata[c_type] is None or metadata[c_type]['est_info'] is None:
                continue
            est_dir = '{0}/fast_dse/{1}'.format(exp_dir, c_type)
            run_command('mkdir -p {0}'.format(est_dir))
            if (not write_json_to_file(metadata[c_type]['est_info'],
                                       '{0}/org.json'.format(est_dir))):
                MLOGGER.debug('ERROR: Failed to write org.json to %s', est_dir)
            if (not write_json_to_file(metadata[c_type]['code_change'],
                                       '{0}/code_change.json'.format(est_dir))):
                MLOGGER.debug('ERROR: Failed to write code_change.json to %s',
                              est_dir)

        # Run HLS
        # Note: Don't put the running log inside the HLS working directory
        # since the backend script will clean it in advance.
        # Note: Use relative paths since we will change directory when running
        # the backend flow.
        full_hls_cmd = '{0} {1} {2}'.format(HLS_CMD, EXPORT_DIR, HLS_DIR)
        run_command('echo {0} > {1}/hls.log'.format(full_hls_cmd, work_dir))
        exit_code = self.run_command_in_time(
            '{0} &>> hls.log'.format(full_hls_cmd), cwd=work_dir,
            limit=self.hls_time_limit)
        run_command('mv {0}/hls.log {1}'.format(work_dir, hls_dir))

        if exit_code['timeout']:
            self.result.error_code = HLSRetCode.HLS_TIME_OUT
        else:
            # Check if performance estimation flow generated the report
            if not os.path.exists('{0}/report_merlin/perf_est.json'
                                  .format(hls_dir)):
                self.result.error_code = HLSRetCode.HLS_ERROR
            self.result.time = exit_code['time']
        return self.result.error_code

    def evaluate(self):
        """
        Parse reports of Merlin midend and HLS
        Store the result in the object
        """
        if (self.result.error_code == HLSRetCode.MERLIN_ERROR or
                self.result.error_code == HLSRetCode.HLS_TIME_OUT or
                self.result.error_code == HLSRetCode.HLS_ERROR):
            self.result.latency = float('inf')
        else:
            tmp_time = self.result.time
            self.result = self.parse_merlin_hls_report()
            self.result.time = tmp_time

            # Read complete HLS info
            token_file_path = ('{0}/{1}/report_merlin/perf_est.json'
                               .format(self.work_dir, HLS_DIR))
            if os.path.exists(token_file_path):
                with open(token_file_path, 'r') as token_file:
                    self.result.parsed_hls_info = json.load(token_file)
            else:
                MLOGGER.debug('perf_est.json not found. No HLS info parsed')

        # Read the QoR source (hls or estimation)
        if os.path.exists('{0}/result_from_est.o'.format(self.hls_dir)):
            self.result.source = 'est'
        self.result.estimate['perf'] = (
            os.path.exists('{0}/estimate_perf_pass.o'.format(self.hls_dir)))
        self.result.estimate['res'] = (
            os.path.exists('{0}/estimate_res_pass.o'.format(self.hls_dir)))

        # Put the resource util to memo for search algorithm to refer
        if self.result.res_detail:
            self.result.add_memo(
                'util', ','.join([str(r)
                                  for _, r in self.result.res_detail.items()]))

        # Put the statement of performance bottleneck
        # to memo for search algorithm to refer
        if self.result.perf_btn:
            self.result.add_memo('btn', ','.join(
                ['{0}/{1}'.format(t, b) for t, b in self.result.perf_btn]))
        return self.result

    def parse_merlin_hls_report(self, report_dir='report_merlin'):
        """
        Parse Merlin HLS report for required information
        Return: HLSResult
        NOTE: This function won't change self.result!
        """
        # Parse the overall info (e.g., latency, resource util. and breakdown)
        rpt_file = '{0}/{1}/perf_est.json'.format(self.hls_dir, report_dir)
        if not os.path.exists(rpt_file):
            run_command('echo "Cannot find {0}" > {1}/eval.log'
                        .format(rpt_file, self.work_dir))
            return None

        result = HLSResult()
        res_detail = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
        res_abs_detail = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
        
        res_info_detail = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
        res_info_abs_detail = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
        kernel_detail = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
        kernel_abs_detail = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
        
        result.latency = float('-inf')
        with open(rpt_file) as filep:
            data = json.loads(filep.read())
            # Extract detailed resource util breakdown for every function
            for elt in data.values():
                if 'type' not in elt:
                    continue
                elif elt['type'] == 'res_info':
                    for res in res_info_detail:
                        res_info_util_ptg = try_to_float(elt['util-{0}'.format(res)])
                        if res_info_util_ptg is not None:
                            res_info_detail[res] = res_info_util_ptg/100.0
                        res_info_abs_detail[res] = try_to_int(elt['total-{0}'.format(res)])
                elif elt['type'] == 'kernel' or elt['type'] == 'function' or elt['type'] == 'loop':
                    if  elt['type'] == 'kernel':
                        result.kernel_func = elt['name']
                        """
                        In case of Xilinx, the resource count doesn't include the BSP resource, so the original 
                        max resource will give the updated kernel resource, the following fix is not required. 
                        """
                        if self.impl_tool == 'aocl':
                            ptg_s = elt['total_percent']
                            ptg_list = ptg_s.strip('[').strip(']').split(',')
                            ptg = try_to_float(ptg_list[1])
                            kernel_detail['LUT'] = ptg/100.0 if ptg is not None else -1
                            ptg = try_to_float(ptg_list[2])
                            kernel_detail['FF'] = ptg/100.0 if ptg is not None else -1
                            ptg = try_to_float(ptg_list[3])
                            kernel_detail['BRAM'] = ptg/100.0 if ptg is not None else -1
                            ptg = try_to_float(ptg_list[4])
                            kernel_detail['DSP'] = ptg/100.0 if ptg is not None else -1
                            res_s = elt['total_resources']
                            res_list = res_s.strip('[').strip(']').split(',')
                            kernel_abs_detail['LUT'] = try_to_int(res_list[0])
                            kernel_abs_detail['FF'] = try_to_int(res_list[1])
                            kernel_abs_detail['BRAM'] = try_to_int(res_list[2])
                            kernel_abs_detail['DSP'] = try_to_int(res_list[3])
                    res_elt = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
                    singleton_elt = {'FF': -1, 'LUT': -1, 'DSP': -1, 'BRAM': -1}
                    for res in res_elt:
                        res_key = 'total-{0}'.format(res)
                        singleton_key = 'singleton-{0}'.format(res)
                        if not res_key in elt:
                            res_elt[res] = 0
                        else:
                            util_num = try_to_int(elt[res_key])
                            if util_num is None:
                                res_elt[res] = 0
                            else:
                                res_elt[res] = util_num
                        if not singleton_key in elt:
                            singleton_elt[res] = 0
                        else:
                            util_num = try_to_int(elt[singleton_key])
                            if util_num is None:
                                singleton_elt[res] = 0
                            else:
                                singleton_elt[res] = util_num
                    result.res_breakdown[elt['name']] = res_elt
                    result.res_singleton [elt['name']] = singleton_elt
                    result.name_topoid_map[elt['name']].append(elt['topo_id'])
            for elt in data:
                if data[elt]['type'] == 'res_info':
                    continue
                # Extract latency
                if 'CYCLE_TOT' in data[elt]:
                    lat = try_to_int(data[elt]['CYCLE_TOT'])
                    if lat is not None:
                        result.latency = max(result.latency, lat)
                # Extract resource
                for res in res_detail:
                    # Extract utilization
                    if not 'util-{0}'.format(res) in data[elt]:
                        continue
                    util_ptg = try_to_float(data[elt]['util-{0}'.format(res)])
                    if util_ptg is None:
                        continue
                    res_detail[res] = max(res_detail[res], util_ptg / 100.0)

                    # Extract the absolute number
                    if not 'total-{0}'.format(res) in data[elt]:
                        continue
                    util_num = try_to_int(data[elt]['total-{0}'.format(res)])
                    if util_num is None:
                        continue
                    res_abs_detail[res] = max(res_abs_detail[res], util_num)
            for res in res_detail:
                if res_abs_detail[res] >= 0 and kernel_abs_detail[res] >= 0:
                    res_abs_detail[res] += res_info_abs_detail[res] - kernel_abs_detail[res]
                if res_detail[res] >= 0 and kernel_detail[res] >= 0:
                    res_detail[res] += res_info_detail[res] - kernel_detail[res]

        if result.latency <= 0:
            MLOGGER.debug('HLS reports %d design latency, '
                          'which may have problems', result.latency)
            result.latency = float('inf')

        if any([res_detail[i] == -1 for i in res_detail]):
            run_command('echo "Fail to parse Merlin HLS report' +
                        ' for resource util" > {0}/eval.log'
                        .format(self.work_dir))
            return result
        if any([res_detail[i] > RES_THRESHOLD for i in res_detail]):
            result.error_code = HLSRetCode.HLS_NO_RESOURCE

        result.res_detail = res_detail
        result.res_abs_detail = res_abs_detail
        # Analyze the design hotspot
        result.perf_btn = self.analyze_hotspot(report_dir)

        return result

    def analyze_hotspot(self, report_dir):
        """
        Analyze the design hotspot
        Return: a list of hotspot scope topo IDs and their bottlenecks
        """
        # Load performance report
        rpt_file = '{0}/{1}/final_info.json'.format(self.hls_dir, report_dir)
        if not os.path.exists(rpt_file):
            run_command('echo "Cannot find {0}" > {1}/eval.log'
                        .format(rpt_file, self.work_dir))
            return []
        else:
            with open(rpt_file) as filep:
                cycles = json.loads(filep.read())

        # Load hierarchy
        hier_file = ('{0}/{1}/gen_token/topo_info.json'
                     .format(self.hls_dir, report_dir))
        if not os.path.exists(hier_file):
            run_command('echo "Cannot find {0}" >> {1}/eval.log'
                        .format(hier_file, self.work_dir))
            return []
        else:
            with open(hier_file) as filep:
                hier = json.loads(filep.read())

        # Fina all critical pathes
        criticals = OrderedDict()
        for kernel in hier:
            subfuncs = {func['name']: func for func in kernel['sub_functions']}
            for path in self.find_all_hotspot_pathes(cycles, subfuncs, kernel):
                for topo_id, comp_bound in path:
                    criticals[topo_id] = comp_bound
        return [(tid, comp_bound) for tid, comp_bound in criticals.items()]

    def find_all_hotspot_pathes(self, cycles, subfuncs, node):
        """
        Traverse the hierarchy and find all performance critical pathes
        """
        if not node['childs']:
            if node['type'] == 'callfunction' and node['name'] in subfuncs:
                # Keep traversing into functions
                pathes = self.find_all_hotspot_pathes(
                    cycles, subfuncs, subfuncs[node['name']])
            else:
                # Innermost component, stop
                pathes = []
        else:
            # Sort child based on cycle count
            sorted_child = sorted(
                [child for child in node['childs']],
                key=lambda c: try_to_int(cycles[c['topo_id']]['CYCLE_TOT'], 0)
                if c['topo_id'] in cycles and
                'CYCLE_TOT' in cycles[c['topo_id']] else 0,
                reverse=True)
            pathes = []
            for child in sorted_child:
                pathes += self.find_all_hotspot_pathes(
                    cycles, subfuncs, child)

        if node['topo_id'] in cycles:
            org_id = cycles[node['topo_id']]['org_identifier'] #node['topo_id']
            total = (try_to_float(cycles[node['topo_id']]['CYCLE_TOT'], 0)
                     if 'CYCLE_TOT' in cycles[node['topo_id']] else 0)
            comm = (try_to_float(cycles[node['topo_id']]['CYCLE_BURST'], 0)
                    if 'CYCLE_BURST' in cycles[node['topo_id']] else 0)
            comp = (try_to_float(cycles[node['topo_id']]['CYCLE_COMPUTE'], 0)
                    if 'CYCLE_COMPUTE' in cycles[node['topo_id']] else 0)

            is_compute_bound = None
            if comm == 0 and comp > 0:
                is_compute_bound = True
            elif comm > 0 and comp == 0:
                is_compute_bound = False
            elif comm == 0 and comp == 0:
                pass # No data, set to an invalid value
            else:
                is_compute_bound = bool((comp / comm) >= 1.0)

            # Fitler out the components that we should not spend time on
            if (is_compute_bound is not None and
                    total > INTEREST_CYCLE_THRESHOLD and
                    not org_id.startswith('BuiltIn') and
                    not org_id.startswith('X')):
                if pathes:
                    for path in pathes:
                        path.append((org_id, is_compute_bound))
                else:
                    pathes = [[(org_id, is_compute_bound)]]
        else:
            run_command('echo "Hierarchy node {0} has no cycle info" >> '
                        '{1}/eval.log'.format(node['topo_id'], self.work_dir))
        return pathes

    def check_extempt(self):
        """
        Check if the HLS error is not what we care about
        """
        if not os.path.isfile('{0}/hls.log'.format(self.hls_dir)):
            MLOGGER.debug('ERROR: Cannot find hls.log in %s', self.hls_dir)
            return (False, 'NO_HLS_LOG')

        all_extempt = True
        error_set = set()
        with open('{0}/hls.log'.format(self.hls_dir), 'r') as hls_log:
            for line in hls_log:
                hls_code = re.search(r'ERROR: \[(.*)\]\ ', line)
                if hls_code:
                    hls_code = hls_code.group(1)
                    if hls_code in IGNORE_HLS_ERROR:
                        continue
                    if not hls_code in QOR_HLS_ERROR:
                        all_extempt = False
                        error_set.add(hls_code)

        return (all_extempt, ",".join(error_set))

    def clean(self, debug_mode, keep_code=False):
        """
        Debug modes:
         None    : Remove all
         on      : Only keep the config and reports if HLS success
         verbose : Keep all
        """
        keep = self.result is None # Duplicate HLS code or setup failed
        keep = keep or (self.result.error_code != HLSRetCode.MERLIN_ERROR and
                        self.result.error_code != HLSRetCode.HLS_TIME_OUT and
                        self.result.error_code != HLSRetCode.HLS_ERROR)

        if (debug_mode is None) or (debug_mode == 'on' and not keep):
            run_command('rm -rf {0}'.format(self.work_dir))
            return None
        if debug_mode == 'on' and keep:
            # Clean ori_src and only keep the Merlin kernel with pragmas
            run_command('cd {0}/{1}; '.format(self.work_dir, DPT_DIR) +
                        'rm -rf run_* *.rpt *.log opentuner.* *.json *.tmp')

            # Duplicated points don't have report_merlin
            # so we only keep lc for debugging
            if ((os.path.exists('{0}/{1}/export/lc'
                                .format(self.work_dir, OCL_DIR))) and
                    (not os.path.exists('{0}/{1}/report_merlin'
                                        .format(self.work_dir, HLS_DIR)))):
                run_command('mkdir -p {0}/{1}/report_merlin'
                            .format(self.work_dir, HLS_DIR))
                run_command('cp -rf {0}/{1}/export/lc {0}/{2}/report_merlin/'
                            .format(self.work_dir, OCL_DIR, HLS_DIR))

            # Clean everything but report_merlin, ori_src,
            # and code_transform (option)
            run_command('mkdir {0}/to_be_removed'.format(self.work_dir))
            run_command('mv -f {0}/* {0}/to_be_removed &>/dev/null'
                        .format(self.work_dir))
            run_command('mv -f ' +
                        '{0}/to_be_removed/implement/exec/hls/report_merlin* '
                        .format(self.work_dir) + self.work_dir +
                        ' &> /dev/null')
            run_command('mv -f ' +
                        '{0}/to_be_removed/implement/exec/hls/*.o '
                        .format(self.work_dir) + self.work_dir +
                        ' &> /dev/null')
            run_command('mv -f {0}/to_be_removed/ori_src {0}'
                        .format(self.work_dir))
            if keep_code:
                run_command('mv -f {0}/to_be_removed/code_transform* {0}'
                            .format(self.work_dir))
            run_command('rm -rf {0}/to_be_removed'.format(self.work_dir))

            # Clean report_merlin
            # FIXME: Check if we can safely remove this backslash
            # pylint: disable=anomalous-backslash-in-string
            run_command('find {0}/report_merlin*/ -name "history*.json" '
                        .format(self.work_dir) +
                        '-prune -exec rm -rf {} \; &> /dev/null')
            run_command('rm -rf {0}/report_merlin*/perf_est/lc_*'
                        .format(self.work_dir))
            run_command('rm -rf {0}/report_merlin*/perf_est/lc/lc'
                        .format(self.work_dir))
            run_command('rm -rf {0}/report_merlin*/gen_token'
                        .format(self.work_dir))
        return self.work_dir

    def run_command_in_time(self, cmd, cwd='./', limit=None, **kwargs):
        """
        Run a command with a time limitation
        """
        if limit is float('inf'):
            limit = None

        # Python3 compatible
        # basestring means "str" and "unicode" in Python2
        # but does not exists in Python3
        # pylint: disable=redefined-builtin
        try:
            basestring
        except NameError:
            basestring = str

        if isinstance(cmd, basestring):
            kwargs['shell'] = True
        killed = False
        start_time = time.time()
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                preexec_fn=os.setsid, stderr=subprocess.PIPE,
                                cwd=cwd, **kwargs)
        # Add p.pid to list of processes to kill in case of keyboardinterrupt
        self.pids.append(proc.pid)

        try:
            while proc.returncode is None:
                if limit is None:
                    goodwait(proc)
                elif limit and time.time() > start_time + limit:
                    killed = True
                    goodkillpg(proc.pid)
                    goodwait(proc)
                else:
                    time.sleep(3)
                proc.poll()
        except:
            if proc.returncode is None:
                goodkillpg(proc.pid)
            raise
        finally:
            # No longer need to kill proc
            if proc.pid in self.pids:
                self.pids.remove(proc.pid)

        end_time = time.time()
        return {'time': float('inf') if killed else (end_time - start_time),
                'timeout': killed,
                'returncode': proc.returncode}


# Util functions

def apply_cfg_and_run_hls(cfg, work_dir, args):
    """
    Apply the given config and run the HLS (analytical-based shortcut may apply)
    """
    cfg_key = config_to_string(cfg)
    valid, ret = mock_load('apply_cfg_and_run_hls', cfg_key)
    if valid:
        return ret

    # Recover arguments
    hls_time_limit = args['hls-time-limit']
    impl_tool = args['impl-tool']
    is_encrypt = args['encrypt']

    # Transform kernel
    (success, log_str) = transform_kernel(work_dir, is_encrypt, cfg)
    trans_dir = '{0}/{1}'.format(work_dir, TRANS_DIR)

    hls = DSERunHLS(trans_dir, work_dir, hls_time_limit, impl_tool)
    if not hls.is_setup:
        return (work_dir, None, log_str)

    if not success:
        # Midend has error, don't run HLS
        hls_result = HLSResult()
        hls_result.error_code = HLSRetCode.MERLIN_ERROR
    else:
        # Run HLS and get the result
        hls_result = None
        try:
            hls.run()
            hls_result = hls.evaluate()
        except Exception:
            hls_result = HLSResult()
            MLOGGER.debug(traceback.format_exc())
            hls_result.error_code = HLSRetCode.MERLIN_ERROR

    mock_save('apply_cfg_and_run_hls', cfg_key, (hls, hls_result, log_str))

    return (hls, hls_result, log_str)

def goodkillpg(pid):
    """Safely kill a process"""
    MLOGGER.fcs_lv3("killing pid %d", pid)
    try:
        if hasattr(os, 'killpg'):
            os.killpg(os.getpgid(pid), signal.SIGKILL)
        else:
            os.kill(os.getpgid(pid), signal.SIGKILL)
    except Exception:
        MLOGGER.debug('error killing process %s', pid, exc_info=True)

def goodwait(proc):
    """Safely wait for a process"""
    while True:
        try:
            ret_val = proc.wait()
            return ret_val
        except OSError as e:  # pylint: disable=C0103
            # pylint: disable=E0602
            if e.errno != errno.EINTR:
                raise
