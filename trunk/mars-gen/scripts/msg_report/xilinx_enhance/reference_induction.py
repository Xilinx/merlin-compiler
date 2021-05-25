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
reference induction module
'''

import os
import sys
import shutil
import merlin_log
import merlin_report_setting
import utility

#pylint: disable=too-few-public-methods, too-many-instance-attributes, line-too-long, print-statement
class ReferenceInduction:
    '''
    Feature:
        this class used to read history and get org_id and org_scope_id
    Input:
        1. gen_token.json which filled all required data on each topoID
        2. metadata which have all history files
    Output:
        refer_induct.json
    '''
    def __init__(self):
        self.logger = merlin_log.MerlinLog()
        self.handle1, self.handle2 = self.logger.set_merlin_log()
        self.settings = merlin_report_setting.MerlinReportSetting()
        self.json_perf_est = self.settings.json_perf_est
        self.json_refer_induct = self.settings.json_refer_induct
        self.json_gen_token = self.settings.json_gen_token
        self.pass_name = self.settings.pass_id_update
        self.include_path = self.settings.xml_include_path
        self.log_total_message = self.settings.log_total_message
        self.log_message_report = self.settings.log_message_report
    def reference_induction_top(self):
        '''
        top level function for performance estimate
        '''
        self.logger.merlin_info("================================================================")
        self.logger.merlin_info("2. Start reference induct: ")
        self.logger.merlin_info("================================================================")
        curr_path = os.path.abspath((os.getcwd()))
        # run rose pass
        work_dir = self.settings.dir_refer_induct
        temp_file = "id_update_gen_token.json"
        empty_file = '__merlin.cpp'
        with open(os.path.join(work_dir, empty_file), 'w') as fileh:
            utility.exec_rose_pass(work_dir, self.pass_name, [empty_file], \
                self.include_path, '-a', 'history_list=history_passes.list', \
                '-a', 'inter_report=' + self.json_gen_token + " > refer_induct.log")
            fileh.close()
        with open(os.path.join(work_dir, temp_file), 'r') as fileh:
            shutil.copyfile(os.path.join(work_dir, temp_file), \
                os.path.join(work_dir, self.json_refer_induct))
            fileh.close()
        # check output
        self.logger.reset_merlin_log(self.handle1, self.handle2)
        try:
            open(os.path.join(work_dir, self.json_refer_induct), "r")
        except IOError:
            print("Failed to generate : " + self.json_refer_induct)
            self.logger.merlin_warning("Failed to generate : " + self.json_refer_induct)
            if os.path.exists(os.path.join(work_dir, "refer_induct.log")):
                self.logger.merge_log(curr_path, work_dir, self.log_message_report, self.log_total_message, 1)
                self.logger.merge_log(curr_path, work_dir, "refer_induct.log", self.log_total_message, 1)
            sys.exit(1)
        if os.path.exists(os.path.join(work_dir, "refer_induct.log")):
            self.logger.merge_log(curr_path, work_dir, self.log_message_report, self.log_total_message, 1)
            self.logger.merge_log(curr_path, work_dir, "refer_induct.log", self.log_total_message, 1)
