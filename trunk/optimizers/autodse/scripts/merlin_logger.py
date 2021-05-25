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

Filename    : optimizers/autodse/scripts/merlin_logger.py
Description : This is the log library for the DSE flow.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-24-2017 First pull request (Cody Hao Yu)
"""

from typing import Any # pylint: disable=unused-import
import os
import logging
from util import get_cmd_output

LOG_PATH = '.'
FILE_NAME = 'auto_dse'

# User message table
USER_MSG = {
    'INFO': {
        0: 'Maximum DSE time is set to {0} mins',
        1: 'HLS time for each design point is limited to {0} mins',
        2: 'Starting part {0} DSE for maximum {1} mins',
        3: 'Finished part {0} DSE in {1} mins',
        4: 'Stop partition {0} DSE due to key interrupt',
        5: 'Total explored {0} design points with {1:.1f}% '
           'timeout and {2:.1f}% HLS failed',
        6: 'Best QoR: {0} cycles',
        7: 'Total {0} design space partitions',
        8: 'Exploring {0} design space partitions using {1} threads',
        9: 'Target platform: {0}',
        10:'Stop launching partitions due to less than {0} mins',
        11:'Terminate partition {0} accordingly',
        12:'Finish {0}/{1} parts, best: inf',
        13:'Finish {0}/{1} parts, best: {2:.0f} cycles (part {3})',
        14:'Finished DSE process in {0} mins',
        15:'Total {0} design space parameters',
        16:'Design space includes {0:.2E} points',
        17:'Profiling {0} design space partitions using HLS',
        18:'Set random seed to {0}',
        19:'{0} parts HLS setup failed.',
        20:'{0} parts has Merlin error.',
        21:'{0} parts HLS timeout.',
        22:'{0} parts HLS error.',
        23:'{0} parts failed to fetch latency/resource info from HLS report.',
        24:'{0} parts are running out of the resource.',
        25:'{0} parts passed HLS.'
    },
    'WARNING': {
        0: 'Reset the DSE time for the minimum requirement',
        1: 'HLS time limit is reset to match the total DSE time',
        2: 'Fail to kill DSE process',
        3: 'Fail to summarize search process from history.log of part {0}',
        4: 'DSE didn not explore any design point',
        5: 'Timeout rate is too high (> 50%). '
           'Hint: Extend HLS time limit using --attribute dse_hls_time=?',
        6: 'HLS failure rate is too high (> 50%)',
        7: 'Merlin error rate {0:.1f}% is too high (> 50%)',
        8: 'DSE didn not find any valid design point',
        9: 'Maximum number of configs is limited to {0}',
        10:'Vendor HLS processes might still be running!',
        11:'The DSE engine did not generate the design, ',
        12:'Default config is running out of the resource. '
           'Please make sure the design uses less than 80% resources '
           'without Merlin pragmas.',
        13:'No design space to be explored. Stop.',
        15:'Skip dumping the Pareto-curve because "gnuplot" is not installed '
           'or encountered unexpected errors.'
    },
    'ERROR': {
        0: 'Invalid {0} time: {1}',
        1: 'Invalid thread number: {0}',
        2: 'Stop partition {0} DSE due to {1}',
        3: 'Fail to build and partition design space: {0}',
        4: 'Fail to write {0}: {1}',
        5: 'Failed to fetch ID from {0}',
        6: '{0} not found',
        7: 'Fail to read {0}: {1}',
        8: 'Missing attribute "options" in {0}',
        9: '"options" error: Incorrect ID or list syntax {0} in {1}',
        10:'"options" error: Illegal option list {0} in {1}',
        12: '"order" syntax error: {0}',
        13: '"order" should have only one argument, but already found two:'
            ' {0} and {1} in {2}',
        14:'{0} type mismatch: expected all options to be {1}, but found {2}',
        15:'Missing attribute "default" in {0}',
        16:'Unrecognized default value {0}. Note: double quotes (") '
           'are required for string options',
        17:'Cannot find the design space pass list',
        18:'Error when inserting design space pragmas',
        19:'Fail to parse pragmas',
        20:'Failed to find "auto{{...}}" in {0}. Please check the syntax.',
        21:'Failed to write out the design space',
        22:'Fail to write partition files',
        23:'Cannot find {0} for reading profiling result',
        24:'Stop the design space exploration.',
        25:'All parts encountered profiling failure. Please check '
           'the design space',
        26:'Fail to write the profiling result to a json file',
        27:'All parts encountered profiling failure. Please make sure '
           'the design can pass HLS without Merlin pragmas.',
        28:'Hint: Extend the HLS time limit using --attribute dse_hls_time=?',
        29:'Please use the non-DSE flow and make sure HLS report does not '
           'contain any "?"',
        30:'Please make sure the design uses less than 80% resources '
           'without Merlin pragmas.',
        31:'Redefine design space {0} in {1}',
        32:'Dependent parameter "{0}" is either not an allowable builtin '
           'function or not in the design space',
        33:'Missing definition of parameter {0}.',
        34:'Found more than one variables in list comprehension {0}'
    }
}

# Override logging functions

logging.addLevelName(4, "FCS-LV3")
def fcs_lv3(self, message, *args, **kws):
    # type: (Any, str, Any, Any) -> None
    """Add customized logging level (debug level 3) """
    # pylint: disable=protected-access
    if self.isEnabledFor(4):
        self._log(4, message, args, **kws)

logging.addLevelName(7, "FCS-LV2")
def fcs_lv2(self, message, *args, **kws):
    # type: (Any, str, Any, Any) -> None
    """Add customized logging level (debug level 2) """
    # pylint: disable=protected-access
    if self.isEnabledFor(7):
        self._log(7, message, args, **kws)

def info(self, code, *args, **kws):
    # type: (Any, Any, Any, Any) -> None
    """Override log function"""
    # pylint: disable=protected-access
    if self.isEnabledFor(20):
        if isinstance(code, str):
            self._log(20, code, args, **kws)
        elif code not in USER_MSG['INFO']:
            self._log(40, 'Log INFO code {0} is invalid'.format(code), None)
        else:
            self._log(20, encode_merlin_msg('INFO', code, *args), None)

def warning(self, code, *args, **kws):
    # type: (Any, Any, Any, Any) -> None
    """Override log function"""
    # pylint: disable=protected-access
    if self.isEnabledFor(30):
        if isinstance(code, str):
            self._log(30, code, args, **kws)
        elif code not in USER_MSG['WARNING']:
            self._log(40, 'Log WARNING code {0} is invalid'.format(code), None)
        else:
            self._log(30, encode_merlin_msg('WARNING', code, *args), None)

def error(self, code, *args, **kws):
    # type: (Any, Any, Any, Any) -> None
    """Override log function"""
    # pylint: disable=protected-access
    if self.isEnabledFor(40):
        if isinstance(code, str):
            self._log(40, code, args, **kws)
        elif code not in USER_MSG['ERROR']:
            self._log(40, 'Log ERROR code {0} is invalid'.format(code), None)
        else:
            self._log(40, encode_merlin_msg('ERROR', code, *args), None)

logging.Logger.fcs_lv3 = fcs_lv3 # type: ignore
logging.Logger.fcs_lv2 = fcs_lv2 # type: ignore
logging.Logger.info = info # type: ignore
logging.Logger.warning = warning # type: ignore
logging.Logger.error = error # type: ignore

# Set up logging format
FILE_LOG_FROMAT = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s",
                                    "%m-%d %H:%M")
CONS_LOG_FROMAT = logging.Formatter("%(levelname)s: %(message)s")
ROOT_LOGGER = logging.getLogger()

FILE_HANDLER = logging.FileHandler("{0}/{1}.log".format(LOG_PATH, FILE_NAME))
FILE_HANDLER.setFormatter(FILE_LOG_FROMAT)
ROOT_LOGGER.addHandler(FILE_HANDLER)

CONSOLE_HANDLER = logging.StreamHandler()
CONSOLE_HANDLER.setFormatter(CONS_LOG_FROMAT)
ROOT_LOGGER.addHandler(CONSOLE_HANDLER)

# Set up log level
try:
    TMP_LOG_NAME = (get_cmd_output('set_cfg -i get directive.xml ihatebug')
                    if os.path.exists('directive.xml') else 'INFO')
    LOG_LEVEL_NAME = TMP_LOG_NAME.upper()
    if LOG_LEVEL_NAME == 'DEBUG-LEVEL2':
        LOG_LEVEL_NAME = 'FCS-LV2'
    elif LOG_LEVEL_NAME == 'DEBUG-LEVEL3':
        LOG_LEVEL_NAME = 'FCS-LV3'
    LOG_LEVEL = logging.getLevelName(LOG_LEVEL_NAME)
    ROOT_LOGGER.setLevel(LOG_LEVEL)
except Exception:
    LOG_LEVEL = logging.getLevelName('INFO')
    ROOT_LOGGER.setLevel(LOG_LEVEL)

def encode_merlin_msg(level, code, *args):
    # type: (str, int, Any) -> str
    """Fromat the Merlin message with code"""
    if level == 'INFO':
        level_code = 1
    elif level == 'WARNING':
        level_code = 2
    else:
        level_code = 3
    message = USER_MSG[level][code].format(*args)
    return '[DSE-{0}{1:02d}] {2}'.format(level_code, code, message)

def get_logger(name):
    # type: (str) -> Any
    """Generate the logger for the caller"""
    this_logger = logging.getLogger(name)
    this_logger.setLevel(LOG_LEVEL)
    return this_logger
