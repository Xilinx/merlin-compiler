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
final report module
'''
import os
import merlin_log
import merlin_report_setting
import gen_hierarchy
import fill_source_data
import separate_json_gen_top
import report_translate
import hotspot_gen

#pylint: disable=too-few-public-methods
class FinalReport:
    '''
    Feature:
        this class used to handle final report module
    Input:
        1. perf_est.json which filled all required data on each topoID
        2. src code, used to generate source code hierarchy
        3. last c code, used to generate TopoID and hierarchy in topo_info.json
        (could ignore, should fix later)
        4. adb.json: describe the schedule info for each function
        (could ignore, should fix later)
    Output:
        merlin.rpt
    '''
    def __init__(self):
        self.logger = merlin_log.MerlinLog()
        self.handle1, self.handle2 = self.logger.set_merlin_log()
        self.settings = merlin_report_setting.MerlinReportSetting()
        self.json_merlin_rpt = self.settings.json_merlin_rpt
        self.log_message_report = self.settings.log_message_report
        self.log_total_message = self.settings.log_total_message

    def final_report_top(self):
        '''
        top level function for final report
        '''
        curr_path = os.path.abspath(os.path.dirname(os.getcwd()))
        self.logger.merlin_info("================================================================")
        self.logger.merlin_info("4. Start final report generation: ")
        self.logger.merlin_info("================================================================")
        # generate hierarchy.json
        hierarchy = gen_hierarchy.GenHierarchy(self.logger, self.settings)
        hierarchy.gen_hierarchy_top()

        # analyze hierarchy json and refer_induct.json to get final map
        fill_data = fill_source_data.FillSourceData(self.logger, self.settings)
        fill_data.fill_source_data()

        # calculate hotspot
        hotspot = hotspot_gen.FillHotspotData(self.logger, self.settings)
        hotspot.fill_hotspot_data()

        # generate json for for separete report segment
        gen_json = separate_json_gen_top.OutputJsonGeneration(self.logger, self.settings)
        gen_json.separate_json_gen_top()

        # translate json file to merlin.rpt
        rpt_translate = report_translate.ReportTranslate(self.logger, self.settings)
        string_rpt = rpt_translate.translate_top()
        #merge msg_report.log to total_message.log
        if os.path.exists(os.path.join(self.log_message_report)):
            self.logger.merge_log(curr_path, "", self.log_message_report, self.log_total_message, 0)

        # dump to rpt file
        self.logger.reset_merlin_log(self.handle1, self.handle2)
        with open(self.json_merlin_rpt, 'w') as fileh:
            fileh.write(string_rpt)
            fileh.close()

#if __name__ == '__main__':
#    FINAL = FinalReport()
#    FINAL.final_report_top()
