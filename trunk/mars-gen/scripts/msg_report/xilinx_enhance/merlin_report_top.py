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
merlin report top file
current only support Xilnx
'''
import os
import shutil
import merlin_report_setting
from utility import rm_and_mkdir
import performance_estimate
import reference_induction
import final_report
import data_extraction

#pylint: disable=too-many-locals
def merlin_report_top(src, dst, xml, metadata, report):
    '''
    Feature:
        top level merlin report function, only prepare required files and
        call second level blocks
    Input:
        1. src: source code directory, can be any step of merlin frontend passes
        2. dst: last C code of merlin output
        3. xml: directive.xml
        4. metadata: histroy json for all frontend passes and pass list
        5. report: if not fast DSE, input cache json file of fast DSE
                if normal mode, input all vendor reports
    Output:
        merlin.rpt
    '''
    work_dir = os.getcwd()
    settings = merlin_report_setting.MerlinReportSetting()
    #merlin_home = os.environ.get('MERLIN_COMPILER_HOME', '.')
    #script_dir = "mars-gen/scripts/msg_report/xilinx_enhance"

    # 1 step: data extraction
    rm_and_mkdir(settings.dir_gen_token)
    shutil.copyfile(xml, os.path.join(settings.dir_gen_token, settings.file_directive))
    shutil.copytree(dst, os.path.join(settings.dir_gen_token, settings.dir_target_code))
    vdir = os.path.join(settings.dir_gen_token, settings.dir_vendor_report)
    shutil.copytree(report, vdir)
#    os.system("cd " + settings.dir_gen_token + "; python " + merlin_home \
#        + "/" + script_dir + "/data_extraction.py")
    os.chdir(work_dir + "/gen_token")
    data_ext = data_extraction.DataExtraction()
    data_ext.data_extraction_top()
    os.chdir(work_dir)
    output_gen_token = os.path.join(settings.dir_gen_token, settings.json_gen_token)

    # 2 step: refer induct
    #rm_and_mkdir(settings.dir_refer_induct)
    #shutil.copytree(metadata, os.path.join(settings.dir_refer_induct, settings.dir_metadata))
    os.system("rm -rf " + settings.dir_refer_induct)
    shutil.copytree(metadata, settings.dir_refer_induct)
    shutil.copyfile(xml, os.path.join(settings.dir_refer_induct, settings.file_directive))
    shutil.copyfile(output_gen_token, \
        os.path.join(settings.dir_refer_induct, settings.json_gen_token))
    os.chdir(work_dir + "/refer_induct")
    refer_induct = reference_induction.ReferenceInduction()
    os.chdir(work_dir)
    refer_induct.reference_induction_top()
    output_refer_induct = os.path.join(settings.dir_refer_induct, settings.json_refer_induct)

    # 3 step: performance estimation
    rm_and_mkdir(settings.dir_perf_est)
    shutil.copyfile(xml, os.path.join(settings.dir_perf_est, settings.file_directive))
    shutil.copytree(dst, os.path.join(settings.dir_perf_est, settings.dir_target_code))
    shutil.copyfile(output_refer_induct, os.path.join(settings.dir_perf_est, \
                                                      settings.json_refer_induct))
    os.chdir(work_dir + "/perf_est")
    perf_est = performance_estimate.PerformanceEstimate()
    os.chdir(work_dir)
    perf_est.performance_estimate_top(settings.dir_perf_est)
    output_perf_est = os.path.join(settings.dir_perf_est, settings.json_perf_est)

    # 4 step: final report generation
    rm_and_mkdir(settings.dir_final_report)
    shutil.copyfile(xml, os.path.join(settings.dir_final_report, settings.file_directive))
    shutil.copytree(src, os.path.join(settings.dir_final_report, settings.dir_source_code))
    shutil.copyfile(output_perf_est, \
        os.path.join(settings.dir_final_report, settings.json_perf_est))
    shutil.copyfile(
        os.path.join(settings.dir_gen_token, settings.json_topo_info),
        os.path.join(settings.dir_final_report, settings.json_topo_info))
    shutil.copyfile(
        os.path.join(settings.dir_gen_token, settings.json_adb),
        os.path.join(settings.dir_final_report, settings.json_adb))
    #os.system("cd " + settings.dir_final_report + "; python " + merlin_home \
    #    + "/" + script_dir + "/final_report.py")
    os.chdir(work_dir + "/final_report")
    #pylint: disable=no-value-for-parameter, no-member
    final_re = final_report.FinalReport()
    final_re.final_report_top()
    os.chdir(work_dir)
    output_final_report = os.path.join(settings.dir_final_report, settings.json_merlin_rpt)
    shutil.copyfile(output_final_report, settings.json_merlin_rpt)

#if __name__ == '__main__':
#    merlin_report_top("src", "lc", "directive.xml", "metadata", "vendor_report")
