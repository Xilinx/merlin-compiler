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
performance estimate module
'''

import sys
import os
import merlin_log
import merlin_report_setting

#pylint: disable=too-few-public-methods, too-many-instance-attributes, line-too-long, print-statement
class PerformanceEstimate:
    '''
    Feature:
        this class used to handle performance estimation module
    Input:
        1. refer_induct.json which filled all required data and original_id on each topoID
        2. lc code, used read to ROSE to calculate cycles
    Output:
        perf_est.json
    '''
    def __init__(self):
        self.logger = merlin_log.MerlinLog()
        self.handle1, self.handle2 = self.logger.set_merlin_log()
        self.settings = merlin_report_setting.MerlinReportSetting()
        self.json_gen_token = self.settings.json_gen_token
        self.json_perf_est = self.settings.json_perf_est
        self.json_refer_induct = self.settings.json_refer_induct
        self.log_total_message = self.settings.log_total_message
        self.log_message_report = self.settings.log_message_report
    def performance_estimate_top(self, work_dir):
        '''
        top level function for performance estimate
        '''
        self.logger.merlin_info("================================================================")
        self.logger.merlin_info("3. Start performance estimate: ")
        self.logger.merlin_info("================================================================")

        # need to transfer perl to python code too later
        # run perl pass
        merlin_home = os.environ.get('MERLIN_COMPILER_HOME', '.')
        cmd = "mars_perl " + merlin_home + \
            "/mars-gen/scripts/msg_report/perf_est.pl \
             --ann_rpt " + self.json_refer_induct \
             + " --perf_rpt " + self.json_perf_est + " > perf_est.log"
        self.logger.merlin_info("cmd = " + cmd)
        os.system("cd " + work_dir + "; " + cmd)
        os.system("echo " + cmd + " > " + work_dir + "/run_cmd")
        #merge perf_est.log to total_message.log
        curr_path = os.path.abspath((os.getcwd()))

        try:
            open(self.json_perf_est, "r")
        except IOError:
            print("Failed to generate : " + self.json_perf_est)
            self.logger.merlin_warning("Failed to generate : " + self.json_perf_est)
            if os.path.exists(os.path.join(self.settings.dir_perf_est, self.log_message_report)):
                self.logger.merge_log(curr_path, self.settings.dir_perf_est, self.log_message_report, self.log_total_message, 1)
            sys.exit(1)
        if os.path.exists(os.path.join(self.settings.dir_perf_est, "perf_est.log")):
            self.logger.merge_log(curr_path, self.settings.dir_perf_est, self.log_message_report, self.log_total_message, 1)
            self.logger.merge_log(curr_path, self.settings.dir_perf_est, "perf_est.log", self.log_total_message, 1)
        # check output
        self.logger.reset_merlin_log(self.handle1, self.handle2)
