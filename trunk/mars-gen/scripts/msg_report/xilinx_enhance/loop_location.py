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
this file used to generate loop_location.json
'''
import os
import sys
import shutil
import subprocess
#import merlin_log
#import merlin_report_setting
#pylint: disable = line-too-long, print-statement
def get_loop_location(logger, settings):
    '''
    generate loop_location.json
    '''
    logger.merlin_info("------------------------------")
    logger.merlin_info("Start to generate " + settings.json_loop_location)
    logger.merlin_info("------------------------------")

    curr_path = os.path.abspath(os.path.dirname(os.getcwd()))
    # check bc file exsit
    file_bc = settings.file_bc
    try:
        open(os.path.join(settings.dir_vendor_report, file_bc), "r")
    except IOError:
        print("Failed to generate : " + file_bc)
        logger.merlin_warning("Failed to generate : " + file_bc)
        if os.path.exists(os.path.join(settings.log_message_report)):
            logger.merge_log(curr_path, "", settings.log_message_report, settings.log_total_message, 0)
        sys.exit(1)
    shutil.copyfile(os.path.join(settings.dir_vendor_report, file_bc),
                    file_bc)

    # generate loop_location.json
    merlin_home = os.environ.get('MERLIN_COMPILER_HOME', '.')
    opt_path = os.path.join(merlin_home, 'mars-gen/tools/llvm/opt')
    process = subprocess.Popen([opt_path, '-print-dbginfo-merlin', file_bc], \
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=".")
    process.communicate()

    # check if output file exist
    try:
        open(settings.json_loop_location, "r")
    except IOError:
        print("Failed to generate : " + settings.json_loop_location)
        logger.merlin_warning("Failed to generate : " + settings.json_loop_location)
        if os.path.exists(os.path.join(settings.log_message_report)):
            logger.merge_log(curr_path, "", settings.log_message_report, settings.log_total_message, 0)
        sys.exit(1)

    # format output json file
    #with open(settings.json_loop_location, 'r+') as f:
    #    ret = {}
    #    raw_json = json.load(f)
    #    f.seek(0)
    #    f.truncate()
    #    json.dump(raw_json, f, indent = 4)
    #    for m in raw_json:
    #        for k, v in m.iteritems():
    #            split_pos = k.rfind('-')
    #            if split_pos == -1:
    #                continue
    #            func_name = k[:split_pos]
    #            loop_name = k[split_pos + 1:]
    #            ret[(func_name, loop_name)] = v
    #            v['filename'] = os.path.basename(v['filename'])
    #    return ret

# if __name__ == '__main__':
#     LOGGER = merlin_log.MerlinLog()
#     LOGGER.set_merlin_log()
#     SETTINGS = merlin_report_setting.MerlinReportSetting()
#     get_loop_location(LOGGER, SETTINGS)
