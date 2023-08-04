#(C) Copyright 2016-2021 Xilinx, Inc.
#All Rights Reserved.
#
#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License. (edited)
import sys
import json
import glob
from util import run_command
from ds_proc_profiler import profile_design_space_list
from ds_proc_builder import prune_design_space_partition

try:
    sys.path.insert(0, '../common/')
    from test_util import eprint
    from check_json import check_json
except:
    pass

DS_PART_FILES = ['ds_part{0}.json'.format(i) for i in range(6)]

class ARGS(object):
    def __init__(self):
        self.path = '.'
        self.profile_dir = '.'
        self.platform = 'xilinx_vcu1525_dynamic_5_0'
        self.hls_time_limit = '480'
        self.impl_tool = 'sdaccel'
        self.max_thread = 8
        self.type = 'full'
        self.encrypt = False

def check_result():
    # Check design space file with profiling result
    for res in glob.glob('refs/ds_part*.json'):
        name = res[res.find('/') + 1:]
        check_json('temp/{0}'.format(name), 'refs/{0}'.format(name),
                   ignore_values=['latency', 'metadata_profiling'])

if __name__ == "__main__":
    mode = sys.argv[1]
    if mode != 'run' and mode != 'check':
        eprint('Unit test mode error. Expect run/check but {0}'.format(mode))
        sys.exit(1)
    elif mode == 'run':
        profile_design_space_list(DS_PART_FILES, ARGS())
        prune_design_space_partition(DS_PART_FILES, 4)
    else:
        check_result()
