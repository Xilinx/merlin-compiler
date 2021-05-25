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
this file used to extract vendor data
'''

import glob
import os
import sys
import shutil
import c_synth_rpt_parser
import vivado_hls_log_parser
import loop_location
import topo_id_gen
import merge_token
import merlin_log
import merlin_report_setting

# pylint: disable=too-few-public-methods, invalid-name, too-many-instance-attributes, line-too-long, print-statement
class DataExtraction():
    '''
    Feature:
        this class used to handle performance estimation module
    Input:
        1. last c code, used to generate TopoID and hierarchy in topo_info.json
        2. vendor rpt.xml report -> xxx.json
        3. vivado_hls.log -> vivado_hls.json
        4. be_file->loop_location.json: describe the loop line number for the last c code
        5. adb.json: describe the schedule info for each function
    Output:
        gen_token.json: it is a flatten json file which filled vendor data in
    '''
    def __init__(self):
        self.logger = merlin_log.MerlinLog()
        self.handle1, self.handle2 = self.logger.set_merlin_log()
        self.settings = merlin_report_setting.MerlinReportSetting()
        self.dir_vendor_report = self.settings.dir_vendor_report
        self.json_adb = self.settings.json_adb
        self.dir_gen_token = self.settings.dir_gen_token
        self.log_message_report = self.settings.log_message_report
        self.log_total_message = self.settings.log_total_message
    #pylint: disable=fixme, too-many-statements
    def data_extraction_top(self):
        '''
        top level function for performance estimate
        '''
        # build a total message log for all steps
        curr_path = os.path.abspath(os.path.dirname(os.getcwd()))
        with open(os.path.join(curr_path, self.log_total_message), 'w') as fileh:
            fileh.close()
        # build end
        self.logger.merlin_info("================================================================")
        self.logger.merlin_info("1. Start data extracton: ")
        self.logger.merlin_info("================================================================")
        # 1. extract rpt.xml file
        for one_file in sorted(glob.glob(os.path.join(self.dir_vendor_report, "*.rpt.xml"))):
            try:
                c_synth_rpt_parser.ReportExtractor(
                    self.settings, self.logger, one_file
                ).dump()
            except c_synth_rpt_parser.ReportParseError as e:
                self.logger.merlin_warning("Skipping %s, %s" % (one_file, e))
            except c_synth_rpt_parser.ReportExtractError as e:
                self.logger.merlin_warning("Skipping %s, %s" % (one_file, e))

        # 2. extract vovado_hls.log
        vivado_hls_log_parser.LogParser(
            self.settings,
            os.path.join(
                self.settings.dir_vendor_report,
                self.settings.log_vivado_hls)).parse()

        # 3. generate adb.json, currently directly copy, generated in perl script
        # TODO change to python later
        try:
            open(os.path.join(self.dir_vendor_report, self.json_adb), "r")
        except IOError:
            print("Failed to generate : " + self.json_adb)
            self.logger.merlin_warning("Failed to generate : " + self.json_adb)
            if os.path.exists(os.path.join(self.log_message_report)):
                self.logger.merge_log(curr_path, "", self.log_message_report, self.log_total_message, 0)
            sys.exit(1)
        shutil.copyfile(os.path.join(self.dir_vendor_report, self.json_adb),
                        self.json_adb)

        # 4. generate loop_location
        loop_location.get_loop_location(self.logger, self.settings)

        # 5. generate topo id json file
        topo_id_pass = topo_id_gen.TopoIDGeneration(self.logger, self.settings)
        topo_id_pass.topo_id_generation_top()

        # 6. merge topo id and the attributes
        merge_token.Merge(self.settings).merge()
        # check output
        try:
            open(self.settings.json_gen_token, "r")
        except IOError:
            print("Failed to generate : " + self.settings.json_gen_token)
            self.logger.merlin_warning("Failed to generate : " + self.settings.json_gen_token)
            if os.path.exists(os.path.join(self.log_message_report)):
                self.logger.merge_log(curr_path, "", self.log_message_report, self.log_total_message, 0)
            sys.exit(1)
        #remove log output handle, prevent the log information printing 2 times
        self.logger.reset_merlin_log(self.handle1, self.handle2)
        # merge msg_report.log to total_message.log
        if os.path.exists(os.path.join(self.log_message_report)):
            self.logger.merge_log(curr_path, "", self.log_message_report, self.log_total_message, 0)

#if __name__ == '__main__':
#    DATAEXTRACT = DataExtraction()
#    DATAEXTRACT.data_extraction_top()
