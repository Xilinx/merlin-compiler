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
#!/bin/python
# -*- coding:utf-8 -*-

import sys
import os
import re
import json

def extract_data_from_report(path):
    cleaned_report = os.path.join(os.path.dirname(path), os.path.basename(path) + '.cleaned')
    report_data = os.path.join(os.path.dirname(path), os.path.basename(path) + '.data')
    cleaned_report_lines = []
    total_res = {}
    total_cycle = {}
    const_res = {}
    hierarchy_data = []
    hierarchy_cycle = []
    hierarchy_res = []
    cycle_hotspot = []
    res_hotspot = []
    with open(path, 'r') as report:
        lines = report.readlines()
        pattern_matched = {'old_h_sum': 0, 'old_c_a' : 0, 'old_r_h' : 0, 'hier_c' : 0, 'k_sum' : 0, 'hier_r' : 0}
        for line in lines:
            if line.startswith('Report time'):
                continue
            # ignore head line
            if '+' in line:
                continue

            for h in ('hierarchy', 'TC', 'AC', 'CPC', 'LUT', 'FF', 'BRAM', 'DSP', 'comment'):
                if h not in line:
                    break
            else:
                for k in pattern_matched:
                    pattern_matched[k] = 0
                pattern_matched['old_h_sum'] = 1
                continue
            for h in ('Name', 'Cycles', 'Activity'):
                if h not in line:
                    break
            else:
                for k in pattern_matched:
                    pattern_matched[k] = 0
                pattern_matched['old_c_a'] = 1
                continue
            for h in ('Name', 'LUT', 'FF', 'BRAM', 'DSP'):
                if h not in line:
                    break
            else:
                for k in pattern_matched:
                    pattern_matched[k] = 0
                pattern_matched['old_r_h'] = 1
                continue
            for h in ('Hierarchy', 'TC', 'AC', 'CPC', 'Detail'):
                if h not in line:
                    break
            else:
                for k in pattern_matched:
                    pattern_matched[k] = 0
                pattern_matched['hier_c'] = 1
                continue
            for h in ('Kernel', 'Cycles', 'LUT', 'FF', 'BRAM', 'DSP', 'URAM'):
			    if h not in line:
				    break
            else:
                for k in pattern_matched:
                    pattern_matched[k] = 0
                pattern_matched['k_sum'] = 1
                continue
            for h in ('Hierarchy', 'LUT', 'FF', 'BRAM', 'DSP', 'URAM'):
			    if h not in line:
				    break
            else:
                for k in pattern_matched:
                    pattern_matched[k] = 0
                pattern_matched['hier_r'] = 1
                continue
            if pattern_matched['k_sum']:
                #               |Kernel |Cycles |LUT| FF | BRAM | DSP| URAM| Detail| 
                #                1          2    4     6     8    10  12   |       
                m = re.match(r'\|(.*)\|(.+)\((.+)ms\)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\|', line)
                if m:
                    cleaned_line = '|{0}||||||{1}|\n'.format(m.group(1), m.group(14))
                    cleaned_report_lines.append(cleaned_line)
                    total_cycle['Cycles'] = m.group(2).strip().lstrip('~');
                    total_cycle['Time'] = m.group(3).strip().lstrip('~');
                    total_res['LUT'] = m.group(4).strip().lstrip('~')
                    total_res['LUTP'] = m.group(5).strip().lstrip('~')
                    total_res['FF'] = m.group(6).strip().lstrip('~')
                    total_res['FFP'] = m.group(7).strip().lstrip('~')
                    total_res['BRAM'] = m.group(8).strip().lstrip('~')
                    total_res['BRAMP'] = m.group(9).strip().lstrip('~')
                    total_res['DSP'] = m.group(10).strip().lstrip('~')
                    total_res['DSPP'] = m.group(11).strip().lstrip('~')
                    total_res['URAM'] = m.group(12).strip().lstrip('~')
                    total_res['URAMP'] = m.group(13).strip().lstrip('~')
                    continue

            if pattern_matched['hier_c']:
                #           |Hierarchy|TC         |AC   |CPC  | Detail
                #                1     2           3     5     6
                m = re.match(r'\|(.*)\|(\d|\s)+\|(.+)\((.+)\)\|(.+)\|(.+)\|', line)
                if m:
                    cleaned_line = '|{0}||||{1}|\n'.format(m.group(1), m.group(6))
                    cleaned_report_lines.append(cleaned_line)
                    data = {}
                    keys = {'TC':2, 'AC':3, 'ACP': 4, 'CPC':5}
                    for k, v in keys.items():
                        data[k] = m.group(v).strip().lstrip('~')
                    hierarchy_cycle.append(data)
                    continue
            
            if pattern_matched['hier_r']:
                #               |Hierarchy |LUT| FF | BRAM | DSP| URAM|Detail        
                #                1          2    4     6     8    10
                m = re.match(r'\|(.*)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\((.+)\)\|(.+)\|', line)
                if m:
                    cleaned_line = '|{0}||||||{1}|\n'.format(m.group(1), m.group(12))
                    cleaned_report_lines.append(cleaned_line)
                    data = {}
                    keys = {'LUT':2, 'LUTP': 3, 'FF':4, 'FFP': 5, 'BRAM':6, 'BRAMP':7, 'DSP':8, 'DSPP':9, 'URAM': 10, 'URAMP': 11}
                    for k, v in keys.items():
                        data[k] = m.group(v).strip().lstrip('~')
                    hierarchy_res.append(data)
                    continue
            if line.startswith('Total Resource Usage'):
                m = re.search(r'LUT:\s*(\S+)%, FF:\s*(\S+)%, BRAM:\s*(\S+)%, DSP:\s*(\S+)%', line)
                if m:
                    total_res['LUT'] = m.group(1).lstrip('~')
                    total_res['FF'] = m.group(2).lstrip('~')
                    total_res['BRAM'] = m.group(3).lstrip('~')
                    total_res['DSP'] = m.group(4).lstrip('~')
                continue
            if line.startswith('Total Constant Cache'):
                m = re.search(r'LUT:\s*(\S+)%, FF:\s*(\S+)%, BRAM:\s*(\d+)%, DSP:\s*(\d+)%', line)
                if m:
                    const_res['LUT'] = m.group(1).lstrip('~')
                    const_res['FF'] = m.group(2).lstrip('~')
                    const_res['BRAM'] = m.group(3).lstrip('~')
                    const_res['DSP'] = m.group(4).lstrip('~')
                continue
            
            if pattern_matched['old_h_sum']:
                #           |hierarchy|TC         |AC   |CPC  |LUT             |FF              |BRAM            |DSP             |comment
                #                1     2           4     5     6                8                10               12               14
                m = re.match(r'\|(.*)\|((\d|\s)+)\|(.+)\|(.+)\|((\d|\s|\.|~)+)\|((\d|\s|\.|~)+)\|((\d|\s|\.|~)+)\|((\d|\s|\.|~)+)\|(.+)\|', line)
                if m:
                    cleaned_line = '|{0}||||||||{1}|\n'.format(m.group(1), m.group(14))
                    cleaned_report_lines.append(cleaned_line)
                    data = {}
                    keys = {'TC':2, 'AC':4, 'CPC':5, 'LUT':6, 'FF':8, 'BRAM':10, 'DSP':12}
                    for k, v in keys.items():
                        data[k] = m.group(v).strip().lstrip('~')
                    hierarchy_data.append(data)
                    continue
            if pattern_matched['old_c_a']:
                #               |name |cycles     |activity
                #                1     2           4
                m = re.match(r'\|(.*)\|((\d|\s)+)\|(.*)%\s*\|', line)
                if m:
                    cleaned_line = '|{0}|||\n'.format(m.group(1))
                    cleaned_report_lines.append(cleaned_line)
                    data = {}
                    keys = {'cycles':2, 'activity':4}
                    for k, v in keys.items():
                        data[k] = m.group(v).strip()
                    cycle_hotspot.append(data)
                    continue
            if pattern_matched['old_r_h']:
                #               |name |BRAM        |DSP         |FF          |LUT        
                #                1     2            3            4            5
                m = re.match(r'\|(.*)\|(\d|\s|\.)+\|(\d|\s|\.)+\|(\d|\s|\.)+\|(\d|\s|\.)+\|', line)
                if m:
                    cleaned_line = '|{0}||||\n'.format(m.group(1))
                    cleaned_report_lines.append(cleaned_line)
                    data = {}
                    keys = {'BRAM':2, 'DSP':3, 'FF':4, 'LUT':5}
                    for k, v in keys.items():
                        data[k] = m.group(v).strip()
                    res_hotspot.append(data)
                    continue

            cleaned_report_lines.append(line)
    with open(cleaned_report, 'w') as f:
        f.writelines(cleaned_report_lines)
    json_data = {}
    if total_res:
        json_data['total_res'] = total_res
    if total_cycle:
        json_data['total_cycle'] = total_cycle
    if const_res:
        json_data['const_res'] = const_res
    json_data['hierarchy_data'] = hierarchy_data
    json_data['cycle_hotspot'] = cycle_hotspot
    json_data['res_hotspot'] = res_hotspot
    json_data['hierarchy_cycle'] = hierarchy_cycle
    json_data['hierarchy_res'] = hierarchy_res
    with open(report_data, 'w') as f:
        json.dump(json_data, f, indent=2)

def report_data_compare(lhs, rhs):
    for k in ('total_res', 'const_res', 'total_cycle'):
        lhs_total_res = lhs.get(k, {})
        rhs_total_res = rhs.get(k, {})
        for k, v in lhs_total_res.items():
            if not k in rhs_total_res:
                return False
            if not is_almost_same(lhs_total_res[k], rhs_total_res[k]):
                return False

    keys = ('hierarchy_data', 'cycle_hotspot', 'res_hotspot', 'hierarchy_cycle', 'hierarchy_res')
    for k in keys:
        lhs_data = lhs.get(k, [{}])
        rhs_data = rhs.get(k, [{}])
        if len(lhs_data) != len(rhs_data):
            return False
        for i in range(len(lhs_data)):
            for k, v in lhs_data[i].items():
                if not k in rhs_data[i]:
                    return False
                if not is_almost_same(lhs_data[i][k], rhs_data[i][k]):
                    return False
    return True

def is_almost_same(lhs, rhs):
    if '-' in lhs:
        return '-' in rhs
    elif '?' in lhs:
        return '?' in rhs
    elif '' == lhs:
        return '' == rhs
    else:
        lhs_f = float(lhs)
        rhs_f = float(rhs)
        if lhs_f < 100 and rhs_f < 100:
            return abs(lhs_f - rhs_f) < 5
        percent = (lhs_f + 0.001) / (rhs_f + 0.001)
        res = (percent < 1.05 and percent > 0.95)
        return res
