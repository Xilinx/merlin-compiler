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
Input: fill_source_data.json
Output: fill_hotspot_data.json

This module compute AC, and fill into fill_hotspot_data.json
'''

import json
import re
import merlin_report_setting
import utility
#import merlin_log

class FillHotspotData:
    '''
    this class used to compute hotspot data, and fill into output file
    '''

    def __init__(self, logger, settings):
        '''
        prepare self.hierarchy and self.ref_induct
        '''
        self.logger = logger
        self.settings = settings
        self.filled_data = []
        self.func_trip_count = {}
        self.func_ii = {}
        self.func_inline = {}
        self.max_ac = 1

    def fill_hotspot_data(self, input_file=\
        merlin_report_setting.MerlinReportSetting().json_fill_source_data):
        '''
        Compute to hotspot data, and fill into output file
        '''
        with open(input_file, 'r') as data_file:
            json_source_data = json.load(data_file)
            for kernel in json_source_data:
                self.filled_data.append(self.handle_hotspot_data(kernel))
        utility.dump_to_json(self.settings.json_fill_hotspot_data, self.filled_data,
                             "nosort")

    def is_flatten_node(self, node):
        '''
        Is node flattened ?
        '''
        if self.settings.attr_flatten in node:
            if self.settings.attr_flatten_id in node:
                if node[self.settings.attr_flatten_id] != "":
                    return True
        return False

    def get_is_inlined_node(self, node):
        '''
        Is node inlined?
        '''
        if self.settings.attr_inline in node:
            return node[self.settings.attr_inline] == 'yes'
        if self.settings.attr_comment in node:
            comment = node[self.settings.attr_comment]
            if re.search('inlined', comment):
                return True
        if node[self.settings.attr_type] == self.settings.value_function:
            if self.settings.attr_name in node:
                if node[self.settings.attr_name] in self.func_inline:
                    return self.func_inline[node[self.settings.attr_name]]
        return False

    def get_pipeline_info(self, node):
        '''
        Get pipeline and II info
        '''
        if self.settings.attr_pipelined in node and self.settings.attr_iteration_interval in node \
            and node[self.settings.attr_iteration_interval].isdigit():
            return int(node[self.settings.attr_iteration_interval])
        return -1

    def compute_ac(self, node, trip_count, pipeline_ii=-1):
        '''
        Compute AC
        '''
        if trip_count != '?' and self.settings.attr_cycle_unit in node \
            and self.is_number(node[self.settings.attr_cycle_unit]):
            return self.do_compute_ac(node, trip_count, pipeline_ii)
        return '-'


    def do_compute_ac(self, node, trip_count, pipeline_ii=-1):
        '''
        Compute AC
        '''
        if not self.settings.attr_cycle_unit in node or self.is_flatten_node(node):
            return '-'

        if self.settings.attr_unroll_factor in node:
            if self.settings.attr_trip_count in node and \
                node[self.settings.attr_trip_count].isdigit():
                if (int(node[self.settings.attr_unroll_factor])
                        == int(node[self.settings.attr_trip_count])):
                    node[self.settings.attr_cycle_unit] = '-'
                    return '-'

        if self.settings.attr_type in node and \
            node[self.settings.attr_type] == self.settings.value_function:
            if node[self.settings.attr_name] in self.func_inline \
                and self.func_inline[node[self.settings.attr_name]]:
                return '-'

        if pipeline_ii >= 1 and trip_count >= 1:
            return int(trip_count - 1) * pipeline_ii + int(node[self.settings.attr_cycle_unit])

        # default
        return int(trip_count) * int(node[self.settings.attr_cycle_unit])

    def handle_sub_hotspot_data(self, node, trip_count, pipeline_ii, tile_factor):
        '''
        Compute sub node or sub function node's hotspot
        '''
        if self.settings.attr_childs in node:
            for sub_node in node[self.settings.attr_childs]:
                self.handle_hotspot_data(sub_node, trip_count, pipeline_ii, tile_factor)

        if self.settings.attr_sub_functions in node:
            for sub_node in node[self.settings.attr_sub_functions]:
                self.handle_hotspot_data(sub_node, trip_count, pipeline_ii, tile_factor)

    #pylint: disable=too-many-branches
    def handle_hotspot_data(self, node, trip_count=1, pipeline_ii=-1, tile_factor=1):
        '''
        Compute the accumulate_cycle of a node, and fill into output file
        trip_count is multiplicative multiplication of trip_count of it's ancestors
        '''
        if not self.settings.attr_name in node \
                or not self.settings.attr_type in node:
            self.logger.merlin_warning('Node have no "name" or "type" attribute')
            return node
        self.logger.merlin_info("start to handle node ["
                                + node[self.settings.attr_name] + "] 's hotspot")
        # record trip_count, pipeline_ii, inline in dict
        if node[self.settings.attr_type] == self.settings.value_call:
            self.func_trip_count[node[self.settings.attr_name]] = trip_count
            self.func_ii[node[self.settings.attr_name]] = pipeline_ii
            self.func_inline[node[self.settings.attr_name]] = self.get_is_inlined_node(node)
        # if burst under tiled loop and the trip count will divide the tile_factor
        if self.settings.attr_type in node and node[self.settings.attr_type] == \
                                                       self.settings.value_burst:
            if (str(trip_count)).isdigit() and (str(tile_factor)).isdigit():
                trip_count = int(trip_count)/int(tile_factor)
        if not self.is_flatten_node(node):
            if node[self.settings.attr_type] == self.settings.value_function:
                if self.settings.attr_name in node:
                    if node[self.settings.attr_name] in self.func_trip_count:
                        trip_count = self.func_trip_count[node[self.settings.attr_name]]
        # compute accumulate_cycle
        accumulate_cycle = self.compute_ac(node, trip_count, pipeline_ii)
        if self.is_flatten_node(node) or self.get_is_inlined_node(node):
            node[self.settings.attr_cycle_unit] = '-'

        if node[self.settings.attr_type] == self.settings.value_kernel:
            self.max_ac = accumulate_cycle
        # compute pecentage
        if self.is_number(accumulate_cycle) and self.is_number(self.max_ac) and self.max_ac > 0:
            percent = format(float(accumulate_cycle) / float(self.max_ac) * 100, '.1f')
            node[self.settings.attr_cycle_total] = \
              str(accumulate_cycle) + ' (' + str(percent).rjust(5, ' ') + '%)'
        else:
            node[self.settings.attr_cycle_total] = '-'
        # update trip_count
        if not self.is_flatten_node(node):
            if node[self.settings.attr_type] == self.settings.value_function:
                if self.settings.attr_name in node:
                    if node[self.settings.attr_name] in self.func_trip_count:
                        trip_count = self.func_trip_count[node[self.settings.attr_name]]
                    if node[self.settings.attr_name] in self.func_ii:
                        pipeline_ii = self.func_ii[node[self.settings.attr_name]]

            if self.settings.attr_trip_count in node:
                if trip_count == '?' or node[self.settings.attr_trip_count] == '?':
                    trip_count = '?'
                elif node[self.settings.attr_trip_count].isdigit() \
                        and int(node[self.settings.attr_trip_count]) > 0:
                    trip_count *= int(node[self.settings.attr_trip_count])

        # unroll
        if self.settings.attr_unroll_factor in node:
            if self.is_number(node[self.settings.attr_unroll_factor]) \
                and int(node[self.settings.attr_unroll_factor]) > 0 \
                and self.is_number(trip_count):
                trip_count = int(trip_count / int(node[self.settings.attr_unroll_factor]))
        if self.settings.msg_tile_factor in node and \
            self.is_number(node[self.settings.msg_tile_factor]) and \
            int(node[self.settings.msg_tile_factor]) > 0:
            tmp_tile_factor = int(node[self.settings.msg_tile_factor])
        else:
            tmp_tile_factor = tile_factor
        # compute sub nodes
        if pipeline_ii < 0:
            pipeline_ii = self.get_pipeline_info(node)
        self.logger.merlin_info("node [" + node[self.settings.attr_name] \
                + "] 's AC is: " + str(node[self.settings.attr_cycle_total]))
        self.logger.merlin_info("node [" + node[self.settings.attr_name] \
                + "] 's trip-count after update is: " + str(trip_count))
        self.handle_sub_hotspot_data(node, trip_count, pipeline_ii, tmp_tile_factor)

        return node

    @staticmethod
    def is_number(num):
        '''
        Judge whether an obj is a number
        '''
        try:
            float(num)
            return True
        except ValueError:
            return False
