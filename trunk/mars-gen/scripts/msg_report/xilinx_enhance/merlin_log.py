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
Developer: Han Hu
Description: This module used to wrap up log related API to merlin API,
    so it will be more extendable
'''

import os
import sys
import logging
import traceback
from inspect import getframeinfo, stack

def log_file_except_hook(arg_t, arg_v, arg_tb):
    '''
    set msg traceback hook
    '''
    curr_path = os.path.abspath(os.getcwd())
    flag_exist_0 = 0
    flag_exist_1 = 0
    flag_exist_2 = 0
    flag_exist_3 = 0
    if os.path.exists(os.path.join(curr_path, "msg_report.log")):
        with open(os.path.join(curr_path, "msg_report.log")) as file_r:
            content1 = file_r.read()
            flag_exist_0 = 1
            file_r.close()
    if os.path.exists(os.path.join(curr_path, "refer_induct", "msg_report.log")):
        with open(os.path.join(curr_path, "refer_induct", "msg_report.log")) as file_r:
            content1 = file_r.read()
            flag_exist_3 = 1
            file_r.close()
    if os.path.exists(os.path.join(curr_path, "perf_est", "msg_report.log")):
        with open(os.path.join(curr_path, "perf_est", "msg_report.log")) as file_r:
            content1 = file_r.read()
            flag_exist_3 = 1
            file_r.close()
    if os.path.exists(os.path.join(curr_path, "perf_est", "perf_est.log")):
        with open(os.path.join(curr_path, "perf_est", "perf_est.log")) as file_r:
            content2 = file_r.read()
            flag_exist_1 = 1
            file_r.close()
    if os.path.exists(os.path.join(curr_path, "refer_induct", "refer_induct.log")):
        with open(os.path.join(curr_path, "refer_induct", "refer_induct.log")) as file_r:
            content3 = file_r.read()
            flag_exist_2 = 1
            file_r.close()
    if flag_exist_0 == 1:
        curr_path = os.path.abspath(os.path.dirname(os.getcwd()))
    with open('msg_traceback', 'w') as fileh:
        lines = traceback.format_exception(arg_t, arg_v, arg_tb)
        fileh.writelines(lines)
        with open(os.path.join(curr_path, "total_message.log"), 'a') as fileh1:
            if flag_exist_0 == 1 or flag_exist_3 == 1:
                fileh1.write(content1)
            if flag_exist_1 == 1:
                fileh1.write(content2)
            if flag_exist_2 == 1:
                fileh1.write(content3)
            fileh1.writelines(lines)
            fileh1.close()

class MerlinLog:
    '''
    Description: this class used to format merlin log
    Input: debug-level, default=ERROR, can be ERROR/INFO/WARNING
    Output: formatted message
    '''
    def __init__(self, debug_level="INFO"):
        '''
        Description: this function used to init the class
        Input: debug-level, default=ERROR, can be ERROR/INFO/WARNING
        Output:
        '''
        self.log_file_name = "msg_report.log"
        self.logger = logging.getLogger("logger")
        self.debug_level = debug_level

    def set_merlin_log(self):
        '''
        set debug level and log handler
        '''
        #set debug level
        if self.debug_level == "ERROR":
            self.logger.setLevel(logging.ERROR)
        elif self.debug_level == "WARNING":
            self.logger.setLevel(logging.WARNING)
        elif self.debug_level == "INFO":
            self.logger.setLevel(logging.INFO)
        elif self.debug_level == "DEBUG":
            self.logger.setLevel(logging.DEBUG)
        else:
            self.logger.setLevel(logging.DEBUG)
        #set handler
        handler1 = logging.StreamHandler()  #print to screen
        handler2 = logging.FileHandler(filename=self.log_file_name) #print to log
        #set format
        #formatter = logging.Formatter("%(asctime)s  [%(filename)s:%(lineno)d] %(message)s")
        formatter = logging.Formatter("%(asctime)s %(message)s")
        handler1.setFormatter(formatter)
        handler2.setFormatter(formatter)
        #add handler
        self.logger.addHandler(handler1)
        self.logger.addHandler(handler2)
        sys.excepthook = log_file_except_hook
        return (handler1, handler2)

    def reset_merlin_log(self, handler1, handler2):
        '''
        Description: this function used to remove the logging handler
        Input: logging handler
        '''
        self.logger.removeHandler(handler1)
        self.logger.removeHandler(handler2)
#pylint: disable= too-many-arguments, no-self-use
    def merge_log(self, curr_path, work_dir, filename_r, filename_w, dir_sel):
        '''
        Description: this function used to read the log file and write to another log file
        Input: log directory and log name
        '''
        if dir_sel == 0:
            with open(os.path.join(filename_r), 'r') as file_r_handle:
                content = file_r_handle.read()
                file_r_handle.close()
        else:
            with open(os.path.join(work_dir, filename_r), 'r') as file_r_handle:
                content = file_r_handle.read()
                file_r_handle.close()

        with open(os.path.join(curr_path, filename_w), 'a') as file_w_handle:
            file_w_handle.write(content)
            file_w_handle.close()

    def merlin_error(self, msg):
        '''
        Description: this function used to wrap up error information
        Input: string of message
        Output: formatted message
        '''
        caller = getframeinfo(stack()[1][0])
        filename = os.path.basename(caller.filename)
        linenumber = caller.lineno
        self.logger.error('[%s:%d] ERROR: %s', filename, linenumber, msg)
        sys.exit(1)

    def merlin_info(self, msg):
        '''
        Description: this function used to wrap up info information
        Input: string of message
        Output: formatted message
        '''
        caller = getframeinfo(stack()[1][0])
        filename = os.path.basename(caller.filename)
        linenumber = caller.lineno
        self.logger.info('[%s:%d] INFO: %s', filename, linenumber, msg)

    def merlin_warning(self, msg):
        '''
        Description: this function used to wrap up warning information
        Input: string of message
        Output: formatted message
        '''
        caller = getframeinfo(stack()[1][0])
        filename = os.path.basename(caller.filename)
        linenumber = caller.lineno
        self.logger.warning('[%s:%d] WARNING: %s', filename, linenumber, msg)

    def merlin_critical_warning(self, msg):
        '''
        Description: this function used to wrap up warning information
        Input: string of message
        Output: formatted message
        '''
        caller = getframeinfo(stack()[1][0])
        filename = os.path.basename(caller.filename)
        linenumber = caller.lineno
        self.logger.warning('[%s:%d] CRITICAL WARNING: %s', filename, linenumber, msg)

    def merlin_debug(self, msg):
        '''
        Description: this function used to wrap up debug information
        Input: string of message
        Output: formatted message
        '''
        caller = getframeinfo(stack()[1][0])
        filename = os.path.basename(caller.filename)
        linenumber = caller.lineno
        self.logger.debug('[%s:%d] DEBUG: %s', filename, linenumber, msg)

#if __name__ == '__main__':
#    AA = MerlinLog("ERROR")
#    AA.merlin_error("error\n")
#    AA.merlin_warning("warning\n")
#    AA.merlin_info("info\n")
