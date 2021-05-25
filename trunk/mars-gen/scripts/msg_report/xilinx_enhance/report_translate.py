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
this file used to translater json file to merlin.rpt
'''

import json
from collections import OrderedDict
#import merlin_log
#import merlin_report_setting
import separate_json_gen_top

LINE_LENGTH = 84
TAB_SPACE = 4

def get_equal_line():
    '''
    get eaque line for report
    '''
    return "=" * LINE_LENGTH + "\n"

def get_midline(length):
    '''
    get - line for report
    '''
    return "-" * length

def get_spaceline(length):
    '''
    get - line for report
    '''
    return " " * length

def right_alignment(data, length):
    '''
    right alignment for data
    '''
    length_data = len(data)
    return get_spaceline(length - length_data) + data

def left_alignment(data, length):
    '''
    left alignment for data
    '''
    length_data = len(data)
    return data + get_spaceline(length - length_data)

def mid_alignment(data, length):
    '''
    middle alignment for data
    '''
    final_data = ""
    length_data = len(data)
    delta = length - length_data
    if delta <= 0:
        final_data = data
    else:
        left = int(delta / 2)
        right = delta - left
        final_data = get_spaceline(left) + data + get_spaceline(right)
    return final_data

class ReportTranslate:
    '''
    a class to generate merlin.rpt
    '''
    def __init__(self, logger, settings):
        '''
        init class
        '''
        self.logger = logger
        self.settings = settings
        self.json_summary = settings.json_summary
        self.json_interface = settings.json_interface
        self.json_performance = settings.json_performance
        self.json_resource = settings.json_resource
        self.json_gen = separate_json_gen_top.OutputJsonGeneration(logger, settings)

    def get_head_info(self, top_head, comment_info):
        '''
        get header information
        '''
        self.logger.merlin_info("Start to translate head report:")
        string_head = ""
        top_head_comment = comment_info
        string_head += get_equal_line()
        string_head += top_head
        if top_head_comment != "":
            string_head += " (" + top_head_comment + ")"
        string_head += "\n"
        string_head += get_equal_line()
        return string_head

    def get_pre_info(self, pre_info):
        '''
        get pre_info information
        '''
        self.logger.merlin_info("Start to translate pre info report:")
        string_pre_info = ""
        if isinstance(pre_info, list):
            for one_node in pre_info:
                string_pre_info += one_node
                string_pre_info += '\n'
        return string_pre_info

    # pylint: disable=too-many-branches
    # pylint: disable=too-many-statements
    def get_table_info(self, table_data):
        '''
        get table information
        '''
        self.logger.merlin_info("Start to translate table report:")
        string_table = ""
        column_number = 1
        # check if cloumn number equals for each line of table
        for key in table_data:
            if isinstance(table_data[key], list):
                if key == self.json_gen.tab_column0:
                    column_number = len(table_data[key])
                else:
                    if column_number != len(table_data[key]):
                        self.logger.merlin_error("Table data column number \
                        not eaqal to head number")
            else:
                self.logger.merlin_error("Table data must be a list")
        # get max length for each column
        column_length = [1] * column_number
        for key in table_data:
            data_list = table_data[key]
            tab_depth = 0
            i = 0
            for data in data_list:
                len_data = len(data)
                if i == 0:
                    tab_depth = int(data)
                if i == 1:
                    len_data += tab_depth * TAB_SPACE
                column_length[i] = max(len_data, column_length[i])
                i += 1
        # get split line
        split_line = "+"
        for number in range(1, column_number):
            split_line += get_midline(column_length[number])
            split_line += "+"
        string_table += split_line + "\n"
        # get data line
        for key in table_data:
            data_list = table_data[key]
            if data_list and data_list[0] == "-1":
                string_table += split_line + "\n"
                continue
            string_column = "|"
            if key == self.json_gen.tab_column0:
                # table head, mid alignment
                i = 0
                for data in data_list:
                    if i > 0:
                        string_column += mid_alignment(data, column_length[i])
                        string_column += "|"
                    i += 1
                string_table += string_column + "\n"
#                string_table += split_line + "\n"
            else:
                i = 0
                tab_depth = 0
                for data in data_list:
                    if i == 0:
                        tab_depth = int(data)
                    else:
                        # first column need to consider the indent hierarchy
                        if i == 1:
                            # first column, left alignment
                            data = get_spaceline(tab_depth * TAB_SPACE) + data
                            string_column += left_alignment(data, column_length[i])
                        elif i == len(data_list) - 1:
                            # last column, left alignment
                            string_column += left_alignment(data, column_length[i])
                        else:
                            # other columns, right alignment
                            string_column += right_alignment(data, column_length[i])
                        string_column += "|"
                    i += 1
                string_table += string_column + "\n"
        string_table += split_line + "\n"
        return string_table

    def get_note_info(self, note_data):
        '''
        get note information
        '''
        self.logger.merlin_info("Start to translate note report:")
        string_note = ""
        for key in note_data:
            string_note += key + ": " + note_data[key] + "\n"
        return string_note

    def translate(self, json_file):
        '''
        translate data for one json file
        '''
        self.logger.merlin_info("Start report translate for " + json_file)
        string_json = ""
        with open(json_file, 'r') as fileh:
            # read dict in order
            first_level_data = json.load(fileh, object_pairs_hook=OrderedDict)
            if isinstance(first_level_data, dict): # check if dictionary
                # parser top level json key
                for key1 in first_level_data:
                    top_head = key1
                    second_level_data = first_level_data[key1]
                    # parser second level json key
                    for key2 in second_level_data:
                        # coomment add after top level head
                        if key2 == self.json_gen.tab_comment:
                            string_head = self.get_head_info( \
                            top_head, second_level_data[key2])
                            string_json += "\n"
                            string_json += string_head
                            string_json += "\n"
                        if key2 == self.json_gen.tab_pre_info:
                            string_pre = self.get_pre_info( \
                            second_level_data[key2])
                            string_json += string_pre
                        if key2 == self.json_gen.tab_table \
                           and isinstance(second_level_data[key2], dict):
                            string_table = self.get_table_info( \
                            second_level_data[key2])
                            string_json += string_table
                        if key2 == self.json_gen.tab_note \
                           and isinstance(second_level_data[key2], dict):
                            string_note = self.get_note_info( \
                            second_level_data[key2])
                            string_json += string_note
        return string_json

    def translate_top(self):
        '''
        translate data top function
        '''
        self.logger.merlin_info("Start report translate top")
        string_merlin_rpt = ""
        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating summary")
        self.logger.merlin_info("------------------------------")
        string_merlin_rpt += self.translate(self.json_summary)
        string_merlin_rpt += "\n"
        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating interface")
        self.logger.merlin_info("------------------------------")
        string_merlin_rpt += self.translate(self.json_interface)
        string_merlin_rpt += "\n"
        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating performance")
        self.logger.merlin_info("------------------------------")
        string_merlin_rpt += self.translate(self.json_performance)
        string_merlin_rpt += "\n"
        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating resource")
        self.logger.merlin_info("------------------------------")
        string_merlin_rpt += self.translate(self.json_resource)
        return string_merlin_rpt

# if __name__ == '__main__':
#     LOGGER = merlin_log.MerlinLog()
#     LOGGER.set_merlin_log()
#     SETTINGS = merlin_report_setting.MerlinReportSetting()
#     AA = ReportTranslate(LOGGER, SETTINGS)
#     STRING_RPT = AA.translate_top()
#     print(STRING_RPT)
#     FILEH = open('merlin.rpt', 'w')
#     FILEH.write(STRING_RPT)
#     FILEH.close()
