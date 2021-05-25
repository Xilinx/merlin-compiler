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

"""Python Script in Merlin Compiler

Filename    : optimizers/autodse/scripts/dse_engine.py
Description : This is the script for the DSE that accepts 1) the kernel code
              from Merlin frontend with design space pragmas; 2) an optinal
              design space partition file, performs DSE for that partition
              and outputs the optimal design configuration.
Usage       : Called directly in the dse directory of a Merlin project.
              Look at "Argument parsing" section in this script for the
              detail configuration.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-24-2017 First pull request (Cody Hao Yu)
"""
import argparse
import os
import sys
import json
import random

from typing import Dict, List, Union, Any # pylint: disable=unused-import
from typing import Optional # pylint: disable=unused-import
from dse_type import ( # pylint: disable=unused-import
    CFGType, ModelAccuracyType)
from opentuner.resultsdb.models import ( # pylint: disable=unused-import
    Configuration)

from dse_objective import ThresholdResourceMinimizeLatency
from opentuner import argparsers
from opentuner import MeasurementInterface
from opentuner.search.manipulator import MetadataParameter, DynamicParameter
from opentuner.search.manipulator import ConfigurationManipulator
from opentuner.resultsdb.models import Result

from util import get_cmd_output, get_src_files, run_command, to_int
from dse_util import write_json_to_file, prepare_case_dir, config_to_string
from dse_util import transform_kernel
from dse_mock import mock_load, mock_save
from merlin_logger import get_logger
from dse_run_hls import HLSRetCode, HLSResult
from dse_proc_eval import DSEQoREvaluator

MLOGGER = get_logger('DSE_Engine')


# Argument setting
PARSER = argparse.ArgumentParser(parents=argparsers(),
                                 description='DesignSpaceExplorer')
PARSER.add_argument('--hls-time-limit', action='store',
                    default=900,
                    help='HLS time limit (default 15 minutes)')
PARSER.add_argument('--impl-tool', action='store',
                    default='sdaccel',
                    help='Vendor tool (default sdaccel')
PARSER.add_argument('--random-seed', action='store',
                    default=0,
                    help='Customized random seed for searching')
PARSER.add_argument('--engine', action='store',
                    default="HLS-only",
                    help='engine mode: HLS-only (default), HLS-n-model')
PARSER.add_argument('--debug', action='store',
                    default=None,
                    help='enable debug mode (on/verbose)')
PARSER.add_argument('--encrypt', action='store_true',
                    default=False,
                    help='source code encryption')
PARSER.add_argument('--assign-design-space', action='store',
                    default=None,
                    help='assign a specific design space using a json file')
PARSER.add_argument('path',
                    help='kernel source file path')

def exit_dse_engine(exit_code):
    # type: (int) -> None
    """Leave DSE engine with assigned code"""
    sys.exit(exit_code)

class DesignSpaceExplorer(MeasurementInterface):
    """
    The main interface of the DSE system
    """

    def __init__(self, args):
        # type: (argparse.Namespace) -> None
        super(DesignSpaceExplorer, self).__init__(args)
        self.parallel_compile = True
        self.root_dir = os.getcwd()
        self._manipulator = None # type: Optional[ConfigurationManipulator]

        # Initialize parameters for terminal criteria
        self.config_score = None
        self.iter_count = 0
        self.elapsed_time = 0
        self.prev_result = None # type: Optional[Result]
        self.best_result = None # type: Optional[Result]
        self.best_unchange = 0
        self.ranking = float('inf')
        self.rank_unchange = 0
        self.rank_low = 0

        # Initialize seeds
        self.seeds = [] # type: List[CFGType]

        # Setup HLS time limit and vendor tool
        self.hls_time_limit = int(args.hls_time_limit)
        self.impl_tool = args.impl_tool
        if self.impl_tool != 'sdaccel' and self.impl_tool != 'aocl':
            MLOGGER.error('Unknown vendor tool %s', self.impl_tool)
            exit_dse_engine(1)

        self.mode = args.engine
        if not self.mode == 'HLS-n-model' and not self.mode == 'HLS-only':
            MLOGGER.error('Unrecognized DSE engine mode %s', self.mode)
            exit_dse_engine(1)

        # Setup source file and parameter lists
        self.args = args
        self.is_encrypt = args.encrypt
        self.file_list = get_src_files(args.path)
        self.file_list[:] = ['{0}/{1}'.format(args.path, f)
                             for f in self.file_list]

        # Setup the evaluator
        self.evaluator = DSEQoREvaluator(
            self.impl_tool, self.hls_time_limit, self.mode, self.args.debug)

        # Setup working directory
        self.work_dir = 'work'
        run_command('mkdir work')

        # Setup log and report
        self.report_dir = '{0}/report'.format(self.work_dir)
        if os.path.isdir(self.report_dir):
            run_command('rm -rf {0}'.format(self.report_dir))
        run_command('mkdir {0}'.format(self.report_dir))
        self.debug_dir = '{0}/debug'.format(self.work_dir)
        if os.path.isdir(self.debug_dir):
            run_command('rm -rf {0}'.format(self.debug_dir))
        run_command('mkdir {0}'.format(self.debug_dir))
        self.logfile_name = '{0}/history.log'.format(self.report_dir)
        self.bestfile_name = '{0}/best.log'.format(self.report_dir)
        self.accuracy_map = {} # type: Dict[int, ModelAccuracyType]

        # Initial design space
        self.params = {} # type: Dict[str, DynamicParameter]
        self.metadata = [] # type: List[MetadataParameter]
        self.prev_eval_cfgs = {} # type: Dict[str, str]

        # Load design space
        self.load_design_space(args.assign_design_space)

    def finalize(self):
        # type: () -> None
        """The method called at the end of the DSE process"""

        # Zip the cases with errors for debugging
        MLOGGER.fcs_lv3('Compressing temporary files for debugging')
        run_command('tar -czf {0}/debug.tar.gz {1} &> /dev/null'
                    .format(self.report_dir, self.debug_dir))

        # Dump cache
        self.evaluator.dump_cache(self.report_dir)

        # Write model accuracy log to a JSON file
        if self.accuracy_map:
            write_json_to_file(self.accuracy_map, '{0}/model_accuracy.json'
                               .format(self.report_dir))

        # Summarize the DSE process
        total = int(get_cmd_output('wc {0}/history.log'.format(self.report_dir))
                    .strip().split(' ')[0])
        ml_error = int(get_cmd_output('grep "merlin_error" {0}/history.log | wc'
                                      .format(self.report_dir))
                       .strip().split(' ')[0])
        hls_error = int(get_cmd_output('grep "hls_error" {0}/history.log | wc'
                                       .format(self.report_dir))
                        .strip().split(' ')[0])
        time_out = int(get_cmd_output('grep "timeout" {0}/history.log | wc'
                                      .format(self.report_dir))
                       .strip().split(' ')[0])
        tr_error = int(get_cmd_output('grep "trans_failed" {0}/history.log | wc'
                                      .format(self.report_dir))
                       .strip().split(' ')[0])
        dup_cfg = int(get_cmd_output('grep "duplicate" {0}/history.log | wc'
                                     .format(self.report_dir))
                      .strip().split(' ')[0])
        skip_hls = self.evaluator.get_skip_hls_count()
        if self._manipulator is not None:
            self.logging('Design space: {0:.2E}\n'
                         .format(self._manipulator.search_space_size()))
        self.logging('DSE Summary: Explored {0} designs'
                     .format(total) +
                     ' with {0} HLS errors, {1} Merlin errors, '
                     .format(hls_error, ml_error) +
                     'and {0} timeout\n'.format(time_out))
        self.logging('Merlin transformation failure: {0}\n'.format(tr_error))
        self.logging('Duplicated kernels: {0}\n'.format(dup_cfg))
        self.logging('Cache hit count, Skip HLS count: {0}\n'
                     .format(self.evaluator.get_cache_hit_counts()))
        self.logging('Overall skipped HLS run {0}, {1:.2f}%\n'
                     .format(skip_hls, 100.0 * skip_hls / total))

    def logging(self, msg):
        # type: (str) -> None
        """Write log to file using echo"""
        run_command('echo -n "{0}" >> {1}'.format(msg, self.logfile_name))

    def log_model_accuracy(self, config_id, obj):
        # type: (int, ModelAccuracyType) -> None
        """Log model accuracy to a JSON format file"""
        self.accuracy_map[config_id] = obj

    def log_best(self, iter_count, elapsed, latency):
        # type: (int, float, str) -> None
        """Log the best latency"""
        run_command('echo "{0}\t{1}\t{2}" >> {3}'.format(
            iter_count, elapsed, latency, self.bestfile_name))

    # pylint: disable=R0201,arguments-differ
    def run_precompiled(self, desired_result, inputs, limit,
                        compile_result, cfg_id):
        # type: (Any, Any, Any, Any, Any) -> Any
        """We do not use this method so just bypass the result"""
        del desired_result, inputs, limit, cfg_id
        return compile_result

    # pylint: disable=arguments-differ
    def run(self, desired_result, inputs, limit):
        # type: (Any, Any, Any) -> None
        """
        Pass this function since we're using the parallel mode
        """
        pass # pylint: disable=unnecessary-pass

    def manipulator(self):
        # type: () -> ConfigurationManipulator
        """
        Define the search space by creating a
        ConfigurationManipulator
        """
        cfg_manipulator = ConfigurationManipulator()

        for param in self.params.values():
            cfg_manipulator.add_parameter(param)

        for mdata in self.metadata:
            cfg_manipulator.add_parameter(mdata)

        self._manipulator = cfg_manipulator
        return cfg_manipulator

    def seed_configurations(self):
        # type: () -> List[CFGType]
        """
        Extra seed configuration objects to add to those given on
        the command line.
        Configuration objects (typically dictionaries) not database objects.
        """
        return self.seeds

    def objective(self): # pylint: disable=no-self-use
        # type: () -> ThresholdResourceMinimizeLatency
        """
        called once to create the search.objective.SearchObjective
        """
        return ThresholdResourceMinimizeLatency()

    def load_design_space(self, design_space_file):
        # type: (str) -> None
        """
        Load the design space from json file
        """
        filep = open(design_space_file, 'r')
        data = json.loads(filep.read())
        filep.close()
        design_space = data[0] # The first json object is the design space
        if len(data) > 1:
            self.seeds = data[1:] # The rest are seeds

        # Create design space parameters
        for label, data in design_space.items():
            if label.startswith('metadata'):
                # "metadata" is a preserve prefix to indicate
                # that this is not a part of the design space
                # but just info
                if label == 'metadata_prev-eval-cfg':
                    cfg_key = config_to_string(data['cfg'])
                    self.prev_eval_cfgs[cfg_key] = data['latency']
                else:
                    self.metadata.append(MetadataParameter(label, data))
            else:
                self.params[label] = DynamicParameter.create(
                    label, data['default'], data['options'], data['scope'],
                    {'expr': data['order'], 'var':  data['order_var']}
                    if 'order' in data else None)
                MLOGGER.fcs_lv2('Create %s', str(self.params[label]))

        # Specify dependencies
        for label, data in design_space.items():
            if label.startswith('metadata'):
                continue
            param = self.params[label]
            for dep in data['deps']:
                param.add_dependent(self.params[dep])

    # pylint: disable=arguments-differ
    def save_final_config(self, configuration):
        # type: (Configuration) -> None
        """
        Called at the end of tuning
        Write the final configure to a json file.
        """
        cfg = configuration.data
        write_json_to_file(cfg, 'best_cfg.json')

        # Clean or organize all temporary files.
        # Since OpenTuner doesn't explicitly delete objects,
        # destructor is not guaranteed to be invoked.
        # This is the simplest way to "destruct" this object.
        self.finalize()

    # pylint: disable=arguments-differ
    def compile(self, cfg, cfg_id):
        # type: (CFGType, int) -> Optional[Result]
        """
        The only function that needs to be implemented.
        Since we're using parallel compilation, this function
        also needs to evaluate each design option.
        """
        cfg_key = config_to_string(cfg)
        valid, obj = mock_load('compile', cfg_key)
        if valid:
            if obj is not None:
                history_log, result = obj
                self.logging(history_log)
                return result
            else:
                return None

        # Create a temporary working dir
        tmp_dir = prepare_case_dir(self.root_dir, self.work_dir, 'tmp')

        # Transform kernel
        (success, cfg_log_str) = transform_kernel(tmp_dir, self.is_encrypt, cfg)

        log_str = '{0}\t{1}'.format(cfg_id, cfg_log_str)

        hls_result = None
        if not success:
            # Midend has error, don't run HLS
            MLOGGER.fcs_lv2('%d midend error, do not run HLS', cfg_id)
            hls_result = HLSResult()
            hls_result.error_code = HLSRetCode.MERLIN_ERROR
            qor_log = None
            keep_dir = None
        else:
            MLOGGER.fcs_lv2('%d pass to QoR evaluator', cfg_id)
            hls_result, keep_dir, qor_log = self.evaluator.evaluate(
                cfg_id, tmp_dir)

        if hls_result is None:
            return Result(time=float('inf'), size=float('inf'), elapsed=0.0)

        history_log = '{0}{1}\n'.format(log_str, qor_log)
        self.logging(history_log)
        if qor_log:
            try:
                elapsed_time = float(qor_log.split('\t')[-1])
            except ValueError:
                elapsed_time = 0.0
        else:
            elapsed_time = 0.0

        if keep_dir:
            run_command('mv {0} {1}/{2}'.format(
                keep_dir, self.debug_dir, tmp_dir[tmp_dir.rfind('/') + 1:]))

            if hls_result.model_accuracy:
                hls_result.model_accuracy['path'] = \
                        keep_dir[keep_dir.rfind('/') + 1:]
                self.log_model_accuracy(cfg_id, hls_result.model_accuracy)

        if (hls_result.res_detail and
                hls_result.error_code != HLSRetCode.HLS_NO_RESOURCE):
            res_values = hls_result.res_detail.values()
            res_util = max(max(res_values), 0.99)
        else:
            res_util = float('inf')

        result = Result(time=hls_result.latency, size=res_util,
                        elapsed=elapsed_time, memo=hls_result.memo)

        mock_save('compile', cfg_key, (history_log, result))
        return result

    # pylint: disable=arguments-differ
    def extra_convergence_criteria(self, results):
        # type: (List[Result]) -> bool
        """
        The third stopping criteria than the DSE time
        """
        self.iter_count += len(results)

        # Skip the setup period
        if not results:
            return False

        # Update the best local
        crtl_elapsed_time = 0
        self.best_unchange += len(results)
        valid_result = 0
        iter_best_result = None
        for result in results:
            latency = result.time
            resource = result.size
            crtl_elapsed_time = max(crtl_elapsed_time, result.elapsed)
            if latency == float('inf'):
                continue
            elif resource >= 1.0:
                continue

            valid_result += 1
            if (not iter_best_result) or latency < iter_best_result.time:
                iter_best_result = result

        self.elapsed_time += crtl_elapsed_time

        # Terminate this partition if we cannot get a valid
        # result from the seed design point.
        #if (not self.best_result and self.iter_count >= len(self.seeds)
        #        and valid_result == 0):
        #    return True

        # Update the history
        if iter_best_result:
            if ((not self.best_result) or
                    iter_best_result.time < self.best_result.time):
                self.best_result = iter_best_result
                self.best_unchange = -1
                if self.best_result is not None:
                    self.log_best(self.iter_count, self.elapsed_time,
                                  self.best_result.time)

            # Keep the best result of current iteration for computing
            # the entropy at the next iteration.
            self.prev_result = iter_best_result

        # Update the best global
        count = 1
        new_ranking = float('inf')
        if os.path.exists('.who_is_the_best'):
            with open('.who_is_the_best') as filep:
                for line in filep:
                    latency = float(line.replace('\n', ''))
                    if self.best_result:
                        if latency == float(self.best_result.time):
                            # Found itself
                            new_ranking = count
                    count += 1
            new_ranking /= count

        # Add one thread as the manager requested
        if os.path.exists('.add_threads'):
            with open('.add_threads') as filep:
                line = filep.readline()
                addition_thread = int(line.replace('\n', ''))
                self.more_parallelism(addition_thread)
            run_command('rm -f .add_threads')

        # Skip the warm up period
        if new_ranking == float('inf') or count < 4:
            return False

        # Skip the rest criteria to let partitions run as long as possible
        return False

    # pylint: disable=R0201,arguments-differ
    def fast_eval(self, cfg, cfg_id):
        # type: (CFGType, int) -> None
        """
        Return config ID that we can reuse; otherwise None
        """
        pass # pylint: disable=unnecessary-pass

if __name__ == "__main__":
    MAIN_ARGS = PARSER.parse_args()
    if to_int(MAIN_ARGS.random_seed) != 0:
        MLOGGER.info(18, MAIN_ARGS.random_seed)
        random.seed(to_int(MAIN_ARGS.random_seed))
    else:
        random.seed(0)
    DesignSpaceExplorer.main(MAIN_ARGS)
