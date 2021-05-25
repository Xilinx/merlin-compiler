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
this file used to generate topo id json file
'''

import os
import sys
import shutil
import utility
#import merlin_log
#import merlin_report_setting

#pylint: disable=too-few-public-methods, too-many-instance-attributes, line-too-long, print-statement
class TopoIDGeneration:
    '''
    this class used to generte topp id of one code
    '''
    def __init__(self, logger, settings):
        self.logger = logger
        self.settings = settings
        #self.xml = self.settings.file_directive
        self.json_topo = self.settings.json_topo_info
        self.pass_name = self.settings.pass_gen_token_id
        self.include_path = self.settings.xml_include_path
        self.kernel_list = self.settings.json_kernel_list
        self.work_dir = self.settings.dir_target_code
        self.log_message_report = self.settings.log_message_report
        self.log_total_message = self.settings.log_total_message

    def topo_id_generation_top(self):
        '''
        top level function to generate topo id
        '''
        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start to generate " + self.json_topo)
        self.logger.merlin_info("------------------------------")
        curr_path = os.path.abspath(os.path.dirname(os.getcwd()))
        # get src list
        srcs = utility.get_src_files_from_dir(self.work_dir)

        # execute rose pass
        utility.exec_rose_pass(self.work_dir, self.pass_name, srcs, \
            self.include_path, '-a', 'kernel_list=' + self.kernel_list, \
            '-a', 'output=' + self.json_topo)

        # check output
        try:
            open(os.path.join(self.work_dir, self.json_topo), "r")
        except IOError:

            print("Failed to generate : " + self.json_topo)
            self.logger.merlin_warning("Failed to generate : " + self.json_topo)
            if os.path.exists(os.path.join(self.log_message_report)):
                self.logger.merge_log(curr_path, "", self.log_message_report, self.log_total_message, 0)
            sys.exit(1)
        shutil.copyfile(os.path.join(self.work_dir, self.json_topo),
                        self.json_topo)

# if __name__ == '__main__':
#     LOGGER = merlin_log.MerlinLog()
#     LOGGER.set_merlin_log()
#     SETTINGS = merlin_report_setting.MerlinReportSetting()
#     AA = TopoIDGeneration(LOGGER, SETTINGS)
#     AA.topo_id_generation_top()
