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
Input: fill_hotspot_data.json
Output; summary.json interface.json performance.json resource.json

This module generate separate module data
'''

import datetime
import utility
#import merlin_log
#import merlin_report_setting

FREQ = 300
# use global index to mark foot note
#pylint: disable=global-statement
#pylint: disable=global-at-module-level
global NOTE_INDEX
NOTE_INDEX = 0

def get_note_index():
    '''
    get global note index name
    '''
    #pylint: disable=global-statement
    global NOTE_INDEX
    NOTE_INDEX += 1
    return "[" + str(NOTE_INDEX) + "]"

def get_value_from_key(node, key):
    '''
    get value from key from dict
    if did not find value, report ?
    if value is empty, report -
    '''
    res = ""
    if key in node:
        res = str(node[key])
        if res.strip() == "":
            res = "-"
    else:
        res = "?"
    return res

def get_node_show_name(node):
    '''
    get node show name
    '''
    node_name = get_value_from_key(node, 'name')
    file_name = get_value_from_key(node, 'src_filename')
    line_num = get_value_from_key(node, 'src_line')
    name = node_name
    if line_num != "?" and file_name != "?":
        name = (node_name + " (" + file_name + ":" + line_num + ")")
    else:
        name = node_name
    return name

def extend_index(index):
    '''
    extened index to some length
    '''
    length_max = 10
    length_input = len(str(index))
    length_max = max(length_max, length_input)
    return "0" * (length_max - length_input) + str(index)

#pylint: disable=too-many-instance-attributes
class OutputJsonGeneration:
    '''
    this class used to generate output json file for merlin.rpt
    '''
    def __init__(self, logger, settings):
        self.logger = logger
        self.settings = settings
        self.index = 0
        self.tab_kernel = "Kernel"
        self.tab_cycle = "Cycles"
        self.tab_lut = "LUT"
        self.tab_ff = "FF"
        self.tab_bram = "BRAM"
        self.tab_dsp = "DSP"
        self.tab_uram = "URAM"
        self.tab_detail = "Detail"
        self.tab_interface = "Interface"
        self.tab_bitwidth = "SW Bitwidth"
        self.tab_coalesce_bitwidth = "HW Bitwidth"
        self.tab_read_write = "Type"
        self.tab_comment = "comment"
        self.tab_table = "table"
        self.tab_note = "table_note"
        self.tab_column0 = "column0"
        self.tab_column = "column"
        self.tab_hierarchy = "Hierarchy"
        self.tab_tc = "TC"
        self.tab_ac = "AC"
        self.tab_cpc = "CPC"
        self.tab_pre_info = "pre_info"
        self.title_summary = "Summary"
        self.title_interface = "Interface"
        self.title_performance = "Performance Estimate"
        self.title_resource = "Resource Estimate"
        self.explain_performance = 'TC: Trip Count, AC: Accumulated Cycles,'
        self.explain_performance += ' CPC: Cycles Per Call'

    def is_valid_type(self, data_type):
        '''
        check if valid type
        '''
        return data_type in (self.settings.value_kernel, \
            self.settings.value_function, self.settings.value_call, \
            self.settings.value_loop, self.settings.value_burst)
    #pylint: disable=too-many-locals, too-many-statements
    def get_summary_json(self, data_set):
        '''
        this function used to generate summary json file
        '''

        attr_type = self.settings.attr_type
        content = {}
        content[self.tab_comment] = ''
        pre_info = []
        pre_info.append("Date: " \
                + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'))
        pre_info.append("Target Device: " + self.settings.xml_platform_name)
        target_freq = "0"
        estimate_freq = "0"
        for one_node in data_set:
            if attr_type in one_node and \
                one_node[attr_type] == self.settings.value_kernel:
                target_freq = get_value_from_key(one_node, self.settings.attr_target_freq)
                estimate_freq = get_value_from_key(one_node, self.settings.attr_estimate_freq)
        pre_info.append("Target Frequency: " + str(target_freq) + "MHz")
        pre_info.append("Estimated Frequency: " + str(estimate_freq) + "MHz")
        content[self.tab_pre_info] = pre_info

        table = {}
        table[self.tab_column0] = ["0", self.tab_kernel, self.tab_cycle, \
                                   self.tab_lut, self.tab_ff, self.tab_bram, \
                                   self.tab_dsp, self.tab_uram, self.tab_detail]
        self.index = 0
        for one_node in data_set:
            if attr_type in one_node and \
                one_node[attr_type] == self.settings.value_kernel:
                # insert -1 indent before each kernel
                # will insert split line if indent is -1
                self.index += 1
                table[self.tab_column + extend_index(self.index)] = [
                    str(-1), "", "", "", "", "", "", "", ""]
                # calculate estimate time based on frequency FREQ
                # currently based on estimate frequency
                cycle = get_value_from_key(one_node, \
                                           self.settings.attr_cycle_unit)
                show_cycle = ""
                if cycle.isdigit():
                    time = round(float(cycle) / int(estimate_freq) / 1000, 3)
                    show_cycle = cycle + " (" + str(time) + "ms)"
                else:
                    show_cycle = "?"

                self.index += 1
                table[self.tab_column + extend_index(self.index)] = [
                    str(0),
                    get_node_show_name(one_node),
                    show_cycle,
                    get_value_from_key(one_node, self.settings.attr_total_lut),
                    get_value_from_key(one_node, self.settings.attr_total_ff),
                    get_value_from_key(one_node, self.settings.attr_total_bram),
                    get_value_from_key(one_node, self.settings.attr_total_dsp),
                    get_value_from_key(one_node, self.settings.attr_total_uram),
                    get_value_from_key(one_node, self.settings.attr_comment)]
        content[self.tab_table] = table
        empty_dict = {}
        content[self.tab_note] = empty_dict
        summary_top = {}
        summary_top[self.title_summary] = content
        utility.dump_to_json(self.settings.json_summary, summary_top)

    def get_interface_json(self, data_set):
        '''
        top of gnerate interface json file
        '''
        attr_type = self.settings.attr_type
        content = {}
        content[self.tab_comment] = ''
        content[self.tab_pre_info] = ''
        content[self.tab_note] = {}
        table = {}
        table[self.tab_column0] = ["0", self.tab_interface, self.tab_bitwidth, \
                                   self.tab_coalesce_bitwidth, \
                                   self.tab_read_write, self.tab_detail]
        self.index = 0
        for one_node in data_set:
            if attr_type in one_node and \
                one_node[attr_type] == self.settings.value_kernel \
                and self.settings.attr_interfaces in one_node:
                self.index += 1
                table[self.tab_column + extend_index(self.index)] = [
                    str(-1), "", "", "", "", ""]
                for one_interface in one_node[self.settings.attr_interfaces]:
                    self.index += 1
                    note = get_value_from_key(one_interface, \
                                              self.settings.attr_note)
                    coalesce_bitwidth = "-"
                    wr_type = "-"
                    comment_interface = "-"
                    access_bitwidth = \
                        get_value_from_key(one_interface, \
                                    self.settings.attr_interface_bitwidth)
                    # if user input argument is struct, the bitwidth is 0
                    # and the struct element maybe very complicate
                    # so currently we will set the data as "-"
                    if access_bitwidth != "0":
                        note_name = ""
                        if note not in ('', '-', '?'):
                            note_name = get_note_index()
                            content[self.tab_note][note_name] = note
                        comment_interface = get_value_from_key(one_interface, \
                                self.settings.attr_comment_interface)
                        comment_interface = comment_interface.replace( \
                                    self.settings.merlin_note, note_name)
                        coalesce_bitwidth = \
                            get_value_from_key(one_interface, \
                                        self.settings.attr_coalesce_bitwidth)
                        wr_type = get_value_from_key(one_interface, \
                                        self.settings.attr_burst_read_write)
                        if not coalesce_bitwidth.isdigit():
                            coalesce_bitwidth = access_bitwidth
                    else:
                        access_bitwidth = "-"

                    table[self.tab_column + extend_index(self.index)] = [
                        str(0),
                        get_node_show_name(one_interface),
                        access_bitwidth,
                        coalesce_bitwidth,
                        wr_type,
                        comment_interface]
        content[self.tab_table] = table
        interface_top = {}
        interface_top[self.tab_interface] = content
        utility.dump_to_json(self.settings.json_interface, interface_top)

    def do_build_performance_table(self, table, note_dict, node, indent=0):
        '''
        generate table data recursively
        '''
        attr_type = self.settings.attr_type
        self.index += 1
        show_name = get_node_show_name(node)
        show_trip_count = get_value_from_key(node, \
                                             self.settings.attr_trip_count)
        if attr_type in node and node[attr_type] == self.settings.value_loop:
            show_name = self.settings.value_loop + " " + show_name
        else:
            show_trip_count = ""
        if attr_type in node and self.is_valid_type(node[attr_type]):
            note = get_value_from_key(node, self.settings.attr_note)
            note_name = ""
            if note not in ('', '-', '?'):
                note_name = get_note_index()
                note_dict[note_name] = note
            comment_performance = get_value_from_key(node, \
                                                     self.settings.attr_comment)
            comment_performance = comment_performance.replace( \
                                self.settings.merlin_note, note_name)
#            print("note = " + note)
#            print("node = ")
#            print(node)
#            print("note name = " + note_name)
#            print("comment_performance = " + comment_performance)
            table[self.tab_column + extend_index(self.index)] = [
                str(indent),
                show_name,
                show_trip_count,
                get_value_from_key(node, self.settings.attr_cycle_total),
                get_value_from_key(node, self.settings.attr_cycle_unit),
                comment_performance]
        if self.settings.attr_childs in node:
            indent += 1
            for child in node[self.settings.attr_childs]:
                self.do_build_performance_table(table, note_dict, child, indent)
        if self.settings.attr_sub_functions in node:
            for sub_function in node[self.settings.attr_sub_functions]:
                indent = 0
                self.do_build_performance_table(table, note_dict, \
                                                sub_function, indent)

    def build_performance_table(self, data_set):
        '''
        generate table head
        '''
        table = {}
        note_dict = {}
        table[self.tab_column0] = ["0", self.tab_hierarchy, self.tab_tc, \
                                   self.tab_ac, self.tab_cpc, self.tab_detail]
        self.index = 0
        for one_node in data_set:
            self.index += 1
            table[self.tab_column + extend_index(self.index)] = [
                str(-1), "", "", "", "", ""]
            self.do_build_performance_table(table, note_dict, one_node, 0)
#        print("note_dict = ")
#        print(note_dict)
#        note_str = ""
#        for key in note_dict:
#            note_str += key + " : " + note_dict[key]
#            note_str += "\n"
        return (table, note_dict)

    def get_performance_json(self, data_set):
        '''
        top of gnerate performance json file
        '''
        content = {}
        content[self.tab_comment] = self.explain_performance
        content[self.tab_pre_info] = ''
        (content[self.tab_table], content[self.tab_note]) = self.build_performance_table(data_set)
        performance_top = {}
        performance_top[self.title_performance] = content
        utility.dump_to_json(self.settings.json_performance, performance_top)

    def do_build_resource_table(self, table, node, indent=0):
        '''
        generate table data recursively
        '''
        attr_type = self.settings.attr_type
        self.index += 1
        show_name = get_node_show_name(node)
        if attr_type in node and node[attr_type] == self.settings.value_loop:
            show_name = self.settings.value_loop + " " + show_name
        if attr_type in node and self.is_valid_type(node[attr_type]):
            table[self.tab_column + extend_index(self.index)] = [
                str(indent),
                show_name,
                get_value_from_key(node, self.settings.attr_total_lut),
                get_value_from_key(node, self.settings.attr_total_ff),
                get_value_from_key(node, self.settings.attr_total_bram),
                get_value_from_key(node, self.settings.attr_total_dsp),
                get_value_from_key(node, self.settings.attr_total_uram),
                get_value_from_key(node, self.settings.attr_comment_resource)]
        if self.settings.attr_childs in node:
            indent += 1
            for child in node[self.settings.attr_childs]:
                self.do_build_resource_table(table, child, indent)
        if self.settings.attr_sub_functions in node:
            for sub_function in node[self.settings.attr_sub_functions]:
                indent = 0
                self.do_build_resource_table(table, sub_function, indent)

    def build_resource_table(self, data_set):
        '''
        generate table head
        '''
        table = {}
        table[self.tab_column0] = ["0", self.tab_hierarchy, self.tab_lut, \
                                   self.tab_ff, self.tab_bram, self.tab_dsp, \
                                   self.tab_uram, self.tab_detail]
        self.index = 0
        for one_node in data_set:
            self.index += 1
            table[self.tab_column + extend_index(self.index)] = [
                str(-1), "", "", "", "", "", "", ""]
            self.do_build_resource_table(table, one_node, 0)
        return table

    def get_resource_json(self, data_set):
        '''
        top of gnerate resource json file
        '''
        content = {}
        content[self.tab_comment] = ''
        content[self.tab_pre_info] = ''
        content[self.tab_table] = self.build_resource_table(data_set)
        content[self.tab_note] = {}
        resource_top = {}
        resource_top[self.title_resource] = content
        utility.dump_to_json(self.settings.json_resource, resource_top)

    def separate_json_gen_top(self):
        '''
        top file for generate json files for merlin.rpt
        '''
        data_set = utility.load_json(self.settings.json_fill_hotspot_data)
        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating " + self.settings.json_summary)
        self.logger.merlin_info("------------------------------")
        self.get_summary_json(data_set)

        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating " + self.settings.json_interface)
        self.logger.merlin_info("------------------------------")
        self.get_interface_json(data_set)

        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating " + self.settings.json_performance)
        self.logger.merlin_info("------------------------------")
        self.get_performance_json(data_set)

        self.logger.merlin_info("------------------------------")
        self.logger.merlin_info("Start generating " + self.settings.json_resource)
        self.logger.merlin_info("------------------------------")
        self.get_resource_json(data_set)

#if __name__ == '__main__':
#    LOGGER = merlin_log.MerlinLog()
#    LOGGER.set_merlin_log()
#    SETTINGS = merlin_report_setting.MerlinReportSetting()
#    AA = OutputJsonGeneration(LOGGER, SETTINGS)
#    AA.separate_json_gen_top()
