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
this file used to generate target hierarcy code json file
'''

import os
import sys
import shutil
import utility

# pylint: disable=too-few-public-methods, too-many-instance-attributes, line-too-long, print-statement
class GenHierarchy:
    '''
    this class used to generte topp id of one code
    '''
    def __init__(self, logger, settings):
        self.logger = logger
        self.pass_name = settings.pass_gen_hierarchy
        self.include_path = settings.xml_include_path
        self.json_vendor = settings.json_perf_est
        self.json_hierarchy = settings.json_hierarchy
        self.json_final_info = settings.json_final_info
        self.work_dir = settings.dir_source_code
        self.log_total_message = settings.log_total_message
        self.log_message_report = settings.log_message_report
        self.source_file = settings.xml_source_file
        self.log_hierarchy = settings.log_hierarchy

    def gen_hierarchy_top(self):
        '''
        top level function to generate topo id
        '''
        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating " + self.json_hierarchy)
        self.logger.merlin_info("------------------------------")
        curr_path = os.path.abspath(os.path.dirname(os.getcwd()))

        # prepare input files
        shutil.copyfile(self.json_vendor,
                        os.path.join(self.work_dir, self.json_vendor))
        srcs = self.source_file.split(" ")
        is_c_file = False
        for file in srcs:
            if file.endswith('.c'):
                is_c_file = True
        if is_c_file:
            dummy_file_name = '__dummy__.c'
            dummy_file = os.path.abspath(os.path.join(self.work_dir, dummy_file_name))
            with open(dummy_file, 'w') as out:
                if is_c_file:
                    out.write("#include<stdio.h>\n")
                    out.write("#include<stdlib.h>\n")
                    out.write("#include<string.h>\n")
            srcs.insert(0, dummy_file)

        # call rose pass
        utility.exec_rose_pass(self.work_dir, self.pass_name, srcs, \
            self.include_path, '-a', 'input=' + self.json_vendor, \
            '-a', 'output=' + self.json_final_info, \
            '-a', "hierarchy=" + self.json_hierarchy, \
            '-a', 'switch=on')

        # check output
        try:
            open(os.path.join(self.work_dir, self.json_hierarchy), "r")
        except IOError:
            print("Failed to generate : " + self.json_hierarchy)
            self.logger.merlin_warning("Failed to generate : " + self.json_hierarchy)
            if os.path.exists(os.path.join(self.log_message_report)):
                self.logger.merge_log(curr_path, "", self.log_message_report, self.log_total_message, 0)

            sys.exit(1)
        if os.path.exists(os.path.join(self.work_dir, self.log_hierarchy)):
            self.logger.merge_log(curr_path, "",
                                  os.path.join(self.work_dir, self.log_hierarchy),
                                  self.log_total_message, 0)
        shutil.copyfile(os.path.join(self.work_dir, self.json_hierarchy),
                        self.json_hierarchy)

# if __name__ == '__main__':
#     LOGGER = merlin_log.MerlinLog()
#     LOGGER.set_merlin_log()
#     SETTINGS = merlin_report_setting.MerlinReportSetting()
#     AA = GenHierarchy(LOGGER, SETTINGS)
#     AA.gen_hierarchy_top()
