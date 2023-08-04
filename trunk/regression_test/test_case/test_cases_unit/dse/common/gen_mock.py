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
import json
import glob
import sys
import re
import os

history_logs = glob.glob(sys.argv[1])
mock_data = {}

for log in history_logs:
    with open(log, 'r') as f:
        for line in f:
            match = re.search(r'(\d+)\t(.+)\ttmp(.+)\n', line)
            if not match:
                break
    
            cfg_str = match.group(2).replace('\t', '')
            mock_data[cfg_str] = {}
            result = match.group(3).split('\t')
            if len(result) < 7:
                if result[1].find('timeout') != -1:
                    mock_data[cfg_str]['code'] = -1 # Timeout code
                else:
                    mock_data[cfg_str]['code'] = result[1]
            else:
                if len(result) == 7: # Xilinx
                    lat, ff, lut, dsp, bram, time = result[1:]
                else: # Intel
                    lat, _, ff, lut, dsp, bram, time = result[1:]

                if lat.find('resource') != -1:
                    mock_data[cfg_str]['code'] = -4 # No resource code
                    mock_data[cfg_str]['latency'] = lat[:lat.find(' (')]
                else:
                    mock_data[cfg_str]['code'] = 0
                    mock_data[cfg_str]['latency'] = lat
                mock_data[cfg_str]['res_details'] = {}
                mock_data[cfg_str]['res_details']['FF'] = ff
                mock_data[cfg_str]['res_details']['LUT'] = lut
                mock_data[cfg_str]['res_details']['DSP'] = dsp
                mock_data[cfg_str]['res_details']['BRAM'] = bram
                mock_data[cfg_str]['time'] = time

with open('mock_hls_run.json', 'w') as f:
    f.write(json.dumps(mock_data, indent=2, sort_keys=True))
