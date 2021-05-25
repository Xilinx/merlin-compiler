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

# -*- coding: utf-8 -*-
'''
Input: ref_induct.json hierarchy.json
Output; fill_source_data.json

This module compute to final data, and fill into output file
'''
import re
import json
#import merlin_report_setting
#import merlin_log
import utility
import json_parser

#pylint: disable=too-many-lines
#pylint: disable=too-many-instance-attributes
#pylint: disable=line-too-long, too-many-lines, no-member
class FillSourceData:
    '''
    this class used to compute final data, and fill into output file
    '''
    #pylint: disable=too-many-arguments
    def __init__(self, logger, settings, hierarchy_file="", \
            vendor_file="", topo_info_file="", adb_file="", \
            use_file=True):
        '''
        Prepare input data (json format).
        When use_file is True, then all data are read from files,
        else all data are pass by parameters.
        '''
        self.logger = logger
        self.settings = settings
        self.logger.merlin_info("Start loading info")
        if use_file:
            if len(hierarchy_file) == 0:
                hierarchy_file = self.settings.json_hierarchy
            if len(vendor_file) == 0:
                vendor_file = self.settings.json_perf_est
            if len(topo_info_file) == 0:
                topo_info_file = self.settings.json_topo_info
            if len(adb_file) == 0:
                adb_file = self.settings.json_adb
            # load json file
            self.hierarchy_data = utility.load_json(hierarchy_file)
            self.vendor_data = utility.load_json(vendor_file)
            self.topo_info_data = utility.load_json(topo_info_file)
            self.adb_data = utility.load_json(adb_file)
        else:
            self.hierarchy_data = hierarchy_file
            self.vendor_data = vendor_file
            self.topo_info_data = topo_info_file
            self.adb_data = adb_file

        # call parser to get result
        self.logger.merlin_info("Loading hierarchy topo info")
        self.hier_topo_info = json_parser.HierTopo(self.settings,
                                                   self.logger,
                                                   self.hierarchy_data)
        # preprocess for json file
        self.vendor_data = self.preprocess(self.vendor_data,
                                           self.hier_topo_info.src_topo_id_set,
                                           self.hier_topo_info.src_topo_id_map)
        self.logger.merlin_info("Loading topo info")
        self.topo_info = json_parser.Topo(self.settings, self.logger,
                                          self.vendor_data,
                                          self.hier_topo_info)
        self.function_for_name = self.topo_info.index_function(self.settings.attr_name)
        self.logger.merlin_info("Loading nest info")
        self.nest_info = json_parser.NestedLevel(self.settings,
                                                 self.logger,
                                                 self.topo_info_data,
                                                 self.function_for_name,
                                                 self.vendor_data)
        self.logger.merlin_info("Loading schedule info")
        self.sched_info = json_parser.Schedule(self.settings,
                                               self.logger,
                                               self.adb_data,
                                               self.nest_info,
                                               self.vendor_data)

        self.logger.merlin_info("Loading burst info")
        self.burst_analyzer = json_parser.MemoryBurst(
            self.logger, self.settings, self.topo_info, self.hier_topo_info,
            json_parser.NestedLevel(self.settings,
                                    self.logger,
                                    self.topo_info_data,
                                    self.function_for_name, self.vendor_data, False)
        )

        self.json_filled_data = self.settings.json_fill_source_data
        self.filled_data = []
        self.logger.merlin_info("Finish loading info")
        self.curr_kernel_id = ""
        self.curr_kernel_num = "0"
        self.flatten_loop_info = {}
        self.dead_code = {}
    #pylint: disable=too-many-locals, too-many-branches
    def preprocess(self, vendor_data, id_list, src_topo_id_map):
        '''
        do preprocess for json files
        1. correct org_id in vendor_data
        2. if node id is loop body or function body, set it the same as loop or function
        '''
        self.logger.merlin_info("Do preprocess for vendor data")
        self.logger.merlin_info("get map list")
        self.logger.merlin_info(src_topo_id_map)
        #for one_id in id_list:
        #    self.logger.merlin_info("checking id list: " + one_id)
        new_data = {}
        #pylint: disable=too-many-nested-blocks
        for one_id in vendor_data:
            new_node = {}
            one_node = vendor_data[one_id]
            for key in one_node:
                new_node[key] = one_node[key]
                if self.settings.attr_org_identifier == key:
                    org_id = one_node[self.settings.attr_org_identifier]
                    if org_id[2:] in src_topo_id_map:
                        new_node[key] = src_topo_id_map[org_id[2:]]
                    elif org_id in id_list:
                        new_node[key] = one_node[key]
                    else:
                        #self.logger.merlin_info("checking org id: " + org_id)
                        if org_id[:2] == "L_" or org_id[:2] == "F_" \
                             or org_id[:2] == "X_" or org_id[:2] == "V_":
                            true_org_id = org_id[2:]
                            true_src_id = ""
                            for src_id in id_list:
                                true_src_id = src_id[2:]
                                #self.logger.merlin_info("true id  " + true_src_id + " and " + true_org_id)
                                if true_org_id == true_src_id:
                                    new_node[key] = src_id
                                    self.logger.merlin_info("correct id from " + org_id + " to " + src_id)
            new_data[one_id] = new_node
        key_word_list = [self.settings.msg_tile, self.settings.msg_flatten_loop, \
                        self.settings.attr_is_read, self.settings.attr_is_write, \
                        self.settings.attr_display_name, self.settings.attr_burst_length_max,\
                        self.settings.attr_burst, self.settings.attr_memory_coalescing, \
                        self.settings.attr_streaming, \
                        self.settings.attr_memory_coalescing_bitwidth, \
                        self.settings.attr_node_priority, \
                        self.settings.attr_critical_warning]
        for one_id in new_data:
            node = new_data[one_id]
            if self.settings.attr_messages in node:
                msgs = node[self.settings.attr_messages]
                msgs_list = [json.loads(m) for m in json.loads(msgs)]
                for one_type_info in msgs_list:
                    for one_info in one_type_info:
                        if one_info == self.settings.attr_coarse_pipeline:
                            info = one_type_info[one_info]
                            if info == "on":
                                new_data[one_id][self.settings.attr_coarse_pipeline] = "yes"
                        if one_info in key_word_list:
                            info = one_type_info[one_info]
                            new_data[one_id].update({one_info:info})
                            if one_info == self.settings.msg_tile and info == "yes":
                                if self.settings.attr_trip_count in node:
                                    tile_factor = node[self.settings.attr_trip_count]
                                    new_data[one_id].update({self.settings.msg_tile_factor:tile_factor})
        return new_data

    #pylint: disable=too-many-boolean-expressions, too-many-locals
    def node_filter(self, dst_list):
        '''
        only care the loop and function node
        '''
        def get_all_nodes_from_nest(nodes, node_list):
            for node in nodes:
                if not isinstance(node, list):
                    node_list.append(node)
                else:
                    get_all_nodes_from_nest(node, node_list)
            return node_list

        attr_name = self.settings.attr_name
        attr_type = self.settings.attr_type
        attr_inline = self.settings.attr_inline
        val_call = self.settings.value_call
        val_function = self.settings.value_function
        val_kernel = self.settings.value_kernel
        val_loop = self.settings.value_loop
        val_stmt = self.settings.value_stmt
        output_list = []
        decl_child_list = []
        # get all function decls children
        #pylint: disable=too-many-nested-blocks
        for node in dst_list:
            if node.startswith("F_"):
                self.logger.merlin_info("Checking function " + node)
                # check if children both have function decl and call,
                # if both have, then we should ignore the decl,
                # because the info included in call
                name_decl = self.topo_info.get_value_from_key(node, attr_name)
                type_decl = self.topo_info.get_value_from_key(node, attr_type)
                is_inline = self.topo_info.get_value_from_key(node, attr_inline)
                self.logger.merlin_info("is inline function " + is_inline)
                # if start with F and type is stmt not function, delete it
                if type_decl == val_stmt:
                    decl_child_list.append(node)
                    continue
                if name_decl != "":
                    for one_call in dst_list:
                        name_call = self.topo_info.get_value_from_key( \
                            one_call, attr_name)
                        call_type = self.topo_info.get_value_from_key( \
                            one_call, attr_type)
                        if name_call == name_decl \
                           and call_type == val_call:
                            decl_child_list.append(node)
                            # if inline func, both call and decl be filtered,
                            # because inline function do not have valid info
                            if is_inline == "yes":
                                decl_child_list.append(one_call)
                            children_nodes = self.nest_info.find_childs(node, 1)
                            self.logger.merlin_info("find all children nodes: ")
                            self.logger.merlin_info(children_nodes)
                            one_list = []
                            get_all_nodes_from_nest(children_nodes, one_list)
                            self.logger.merlin_info("function list = ")
                            self.logger.merlin_info(one_list)
                            decl_child_list.extend(one_list)
        self.logger.merlin_info("decl child list = ")
        self.logger.merlin_info(decl_child_list)
        for node in dst_list:
            if node in self.vendor_data and \
                    attr_type in self.vendor_data[node]:
                node_type = self.vendor_data[node][attr_type]
                # As some nodes' type aren't correct,
                # we use topo id's prefix instead
                if node_type in (val_call, val_function, val_kernel, val_loop) \
                        or node.startswith("L_") or node.startswith("F_"):
                    if node not in decl_child_list:
                        output_list.append(node)
        return output_list

    def fill_interface_info(self, interface):
        '''
        fill interface info
        '''
        if self.settings.attr_name in interface:
            self.logger.merlin_info("Start to merge interface: " \
                                    + interface[self.settings.attr_name])

        node_output = {}
        node_output.update(self.topo_info.get_src_common_attributes(interface))
        self.topo_info.append_attribute_to_dict(
            interface, node_output, self.settings.attr_interface_bitwidth
        )

        calls = self.burst_analyzer.find_merlin_memcpy_calls(interface)
        for call in calls:
            self.burst_analyzer.locate_merlin_memcpy_call_site(call)

        interface_info = self.burst_analyzer.get_interface_info(interface)
        node_output.update(interface_info)
        return node_output

    #pylint: disable=no-self-use
    def get_all_first_node(self, nest_nodes):
        '''
        Get all first nest node in a list
        e.g.input:[[u'F_0_0_0_5', [u'F_0_0_0_5_1', u'F_0_0_0_5_2']], [u'F_0_0_0_6'], [u'F_0_0_0_7']]
             output:[u'F_0_0_0_5', u'F_0_0_0_5_1']
        '''
        res = []
        if not isinstance(nest_nodes, list) or not nest_nodes:
            return res
        first_nodes = nest_nodes[0]
        while isinstance(first_nodes, list) and first_nodes:
            if not isinstance(first_nodes[0], list):
                res.append(first_nodes[0])
            if len(first_nodes) >= 2:
                first_nodes = first_nodes[1]
            elif len(first_nodes) == 1 and isinstance(first_nodes[0], list):
                first_nodes = first_nodes[0]
            else:
                break
        return res

    def get_tiling_info(self, nest):
        '''
        get info for tiling
        '''
        info = ""
        loop_name = ""
        for one_node in nest:
            is_tiling_loop = ""
            if one_node in self.vendor_data and self.settings.msg_tile in self.vendor_data[one_node]:
                is_tiling_loop = self.vendor_data[one_node][self.settings.msg_tile]
            if is_tiling_loop == "yes":
                break
            if one_node in self.vendor_data and self.settings.attr_name in self.vendor_data[one_node]:
                loop_name = self.vendor_data[one_node][self.settings.attr_name]
        if loop_name != "":
            info = "loop tiled"
        return info


    #pylint: disable=no-self-use
    def resource_show(self, first, second):
        '''
        get resource show string
        (should be merge with other name show function)
        '''
        return first + " (" + second + "%)"

    #pylint: disable=too-many-locals
    def compute_resource(self, sche_nodes, output, is_inline=False):
        '''
        compute resource for each node
        '''
        def sum_resource(node, key, part_sum):
            if key in node and node[key].isnumeric():
                return part_sum + int(node[key])
            return part_sum

        # Xilnx resource utilization is integer
        # but resource utilization may be float data
        # if float data, need change code
        def sum_resource_util(node, key, part_sum):
            value = "0"
            if key in node:
                value = node[key]
            # if ~=0, value is 0
            if value == "~0":
                value = "0"
            if value.isdigit():
                part_sum += int(value)
            return part_sum

        def do_compute_resource(sche_nodes, key1, key2, is_inline=False):
            '''
            return (total resource number, total resource utilization)
            '''
            (total, util) = ("-", "-")
            if is_inline:
                return (total, util)

            res_t = 0
            res_u = 0
            for one_serial in sche_nodes:
                for one_parallel_id in one_serial:
                    if one_parallel_id in self.vendor_data:
                        # add resource
                        res_t = sum_resource(self.vendor_data[one_parallel_id], key1, res_t)
                        res_u = sum_resource_util(self.vendor_data[one_parallel_id], key2, res_u)
            # set resource
            if res_u == 0:
                res_u = "~0"

            return (str(res_t), str(res_u))

        (total_0, util_0) = do_compute_resource(
            sche_nodes,
            self.settings.attr_total_lut,
            self.settings.attr_util_lut,
            is_inline)
        (total_1, util_1) = do_compute_resource(
            sche_nodes,
            self.settings.attr_total_ff,
            self.settings.attr_util_ff,
            is_inline)
        (total_2, util_2) = do_compute_resource(
            sche_nodes,
            self.settings.attr_total_bram,
            self.settings.attr_util_bram,
            is_inline)
        (total_3, util_3) = do_compute_resource(
            sche_nodes,
            self.settings.attr_total_dsp,
            self.settings.attr_util_dsp,
            is_inline)
        (total_4, util_4) = do_compute_resource(
            sche_nodes,
            self.settings.attr_total_uram,
            self.settings.attr_util_uram,
            is_inline)

        # if all data 0, return all "-"
        # if one of the data is "-", retuan all "-"
        if (total_0 == "0" and total_1 == "0" and total_2 == "0" \
            and total_3 == "0" and total_4 == "0") \
            or total_0 == "-" or total_1 == "-" or total_2 == "-" \
            or total_3 == "-" or total_4 == "-":
            output[self.settings.attr_total_lut] = "-"
            output[self.settings.attr_total_ff] = "-"
            output[self.settings.attr_total_bram] = "-"
            output[self.settings.attr_total_dsp] = "-"
            output[self.settings.attr_total_uram] = "-"
        else:
            output[self.settings.attr_total_lut] = total_0 + " (" + util_0 + "%)"
            output[self.settings.attr_total_ff] = total_1 + " (" + util_1 + "%)"
            output[self.settings.attr_total_bram] = total_2 + " (" + util_2 + "%)"
            output[self.settings.attr_total_dsp] = total_3 + " (" + util_3 + "%)"
            output[self.settings.attr_total_uram] = total_4 + " (" + util_4 + "%)"

    def get_matched_variable(self, var, dep_var_list):
        '''
        check if var match one var in dep_var_list
        '''
# this commented code can improve accuracy
# if the dependency check time issue solved, we will recover this code
#        def match(var1, var2):
#            if var2 != "" and (var1.startswith(var2) or var1.endswith(var2)):
#                self.logger.merlin_info("found matched var")
#                return True
#            return False
#
#        found_match = False
#        ret_var = var
#        var_list = dep_var_list.split(" ")
#        for one_var in var_list:
#            if match(var, one_var):
#                found_match = True
#                ret_var = one_var
#                self.logger.merlin_info("get var from dep var list = " + ret_var)
#                break
#        if not found_match:
#            # check if from array list, get the first var list in array
#            for one_var in var_list:
#                if one_var in self.topo_info.array_list:
#                    ret_var = one_var
#                    self.logger.merlin_info("get var from array= " + ret_var)
#                    break
        def try_to_match(var, var_list):
            '''
            compare with target varaible to all variable list
            find the matched ones, and select the longest match
            '''
            max_length = 0
            max_length_var = ""
            for one_var in var_list:
                if one_var in var:
                    self.logger.merlin_info("matched var = " + one_var)
                    if len(one_var) > max_length:
                        max_length_var = one_var
                        max_length = len(one_var)
                        self.logger.merlin_info("max_length_var = " + max_length_var)
                        self.logger.merlin_info("max_length = " + str(max_length))
            # if length = 1, too short to use
            if max_length > 1:
                return max_length_var
            return ""
        dep_var_list = dep_var_list.lstrip(" ")
        var_list = dep_var_list.split(" ")
        ret_var = try_to_match(var, var_list)
        self.logger.merlin_info("matched ret var = " + ret_var)
        if ret_var == "":
            # check if from array list, get the first var list in array
            for one_var in var_list:
                if one_var in self.topo_info.array_list:
                    ret_var = one_var
                    self.logger.merlin_info("get var from array= " + ret_var)
                    break
        return ret_var

    #pylint: disable=too-many-branches, too-many-statements
    def get_note_info_for_ii(self, violation, iteration_interval, is_pipeline, dep_var_list):
        '''
        get note info for II > 1
        '''
        def get_src_array_name_from_dst(var, dep_var_list):
            self.logger.merlin_info("Get src array name from dst for " + var)
            src_name = var
            topo_id = self.topo_info.get_array_decl_topoid_by_name(var)
            self.logger.merlin_info("initial var = " + var + ", topo_id = " + topo_id)
            # if topo id is not exist, then try to get var from array list
            if topo_id == "":
                var = self.get_matched_variable(var, dep_var_list)
                topo_id = self.topo_info.get_array_decl_topoid_by_name(var)
                self.logger.merlin_info("updated var = " + var + ", topo_id = " + topo_id)
                src_name = var

            org_id = ""
            if topo_id in self.vendor_data and self.settings.attr_org_identifier in self.vendor_data[topo_id]:
                org_id = self.vendor_data[topo_id][self.settings.attr_org_identifier]
                self.logger.merlin_info("Topo_id = " + topo_id + ", org_id = " + org_id)
                if org_id != "" and org_id in self.hier_topo_info.src_location:
                    src_name = self.hier_topo_info.get_src_location(org_id)
                    # we do not need to use loop location here, because loop location already in hierarchy table
                    # if we want to use reference location, need some work from start to end
                    #loop_location = self.hier_topo_info.get_src_location(src_id)
                    #if loop_location:
                    #    src_name_split = src_name.split(" ")
                    #    loop_location_split = loop_location.split("(")
                    #    if src_name_split and len(loop_location_split) > 1:
                    #        src_name ="\'" + src_name_split[0] + "\' (" + loop_location_split[1]
                    self.logger.merlin_info("Found src_name = " + src_name)
            return src_name

        def transform_name_with_line(var):
            regexp = r".*_ln(?P<line>\d+).*"
            match = re.search(regexp, var)
            if match:
                line, = match.groups()
                for i in self.vendor_data:
                    tmp = self.vendor_data[i]
                    if "line" in tmp and tmp["line"] == line and \
                       "type" in tmp and tmp["type"] == "var_ref" and\
                       "name" in tmp:
                        return tmp["name"]
                return var
            return var
        self.logger.merlin_info("Finding the reason for II != 1")
        self.logger.merlin_info("is pipeline = " + is_pipeline + ", dep_var_list = " + dep_var_list + ", violaiton = ")
        self.logger.merlin_info(violation)
        if violation == "":
            return ""
        note = ""
        #pylint: disable=too-many-nested-blocks
        if is_pipeline == "no" or \
            iteration_interval.isnumeric() and int(iteration_interval) > 1:
            #pylint: disable=eval-used
            violation = eval(violation)
            if isinstance(violation, dict):
                reason = ""
                solution = ""
                var_list = []
                for one_dict in violation:
                    self.logger.merlin_info(one_dict)
                    # find the reason of II issue
                    if one_dict == self.settings.attr_ii_reason:
                        reason = violation[one_dict]
                    # find the solution of II issue
                    if one_dict == self.settings.attr_ii_solution:
                        solution = violation[one_dict]
                    # find the variable of II issue
                    if one_dict not in (self.settings.attr_ii_reason, self.settings.attr_ii_solution):
                        for one_var in violation[one_dict]:
                            if violation[one_dict][one_var] != "":
                                self.logger.merlin_info("one_var = " + one_var)
                                self.logger.merlin_info(violation[one_dict][one_var])
                                # 1. if gmem, then bus access conflict
                                # 2. if no variable, find one from dependecy variable list
                                # 3. if have one, trace to real source code variable
                                if "gmem" in one_var:
                                    one_var = "kernel bus"
                                elif violation[one_dict][one_var] == ":":
                                    one_var = self.get_matched_variable(one_var, dep_var_list)
                                    if not one_var.startswith("\'"):
                                        one_var = "\'" + one_var + "\'"
                                    one_var = "variable " + one_var
                                else:
                                    one_var = transform_name_with_line(one_var)
                                    one_var = get_src_array_name_from_dst(one_var, dep_var_list)
                                    if not one_var.startswith("\'"):
                                        one_var = "\'" + one_var + "\'"
                                    one_var = "variable " + one_var
                                if one_var not in var_list and one_var != "variable \'\'":
                                    var_list.append(one_var)
                self.logger.merlin_info("reason = " + reason)
                self.logger.merlin_info(var_list)
                self.logger.merlin_info("solution = " + solution)
                length = len(var_list)
                if length > 0:
                    note = reason + " because of the access on "
                    note += " and ".join(var_list)
                    if solution != "":
                        note += "\n"
                        note += solution
                self.logger.merlin_info("note = " + note)
        return note

    #pylint: disable=too-many-statements, too-many-branches, too-many-nested-blocks
    #pylint: disable=too-many-locals, invalid-name
    def fill_function_loop_info(self, node):
        '''
        fill function and loop node info
        '''
        try:
            src_id = node[self.settings.attr_src_topo_id]
        except KeyError as e:
            self.logger.merlin_critical_warning(e)
            return {}
        #figure out the related which kernel
        if self.settings.attr_type in node and \
            node[self.settings.attr_type] == self.settings.value_kernel:
            kernel_dst_id = self.topo_info.get_dst_id(self.curr_kernel_id)
            if kernel_dst_id != "" and len(kernel_dst_id) >= 1:
                tmp = kernel_dst_id[0].split("_")
                if len(tmp) >= 3:
                    self.curr_kernel_num = tmp[2]
        self.logger.merlin_info("Start to compute for loop " + src_id)
        note = ""
        output = {}
        # set default value for comment
        output[self.settings.attr_comment] = "-"
        output[self.settings.attr_comment_resource] = "-"
        output[self.settings.attr_comment_interface] = "-"
        dst_list_before_filter = self.topo_info.get_dst_id(src_id)
        self.logger.merlin_info("Get dst nodes: ")
        self.logger.merlin_info(dst_list_before_filter)
        dst_list = self.node_filter(dst_list_before_filter)
        if self.settings.attr_parent_loop in node and \
            node[self.settings.attr_parent_loop] in self.dead_code:
            self.dead_code[src_id] = "removed"
            output[self.settings.attr_cycle_unit] = "-"
            output[self.settings.attr_total_lut] = "-"
            output[self.settings.attr_total_ff] = "-"
            output[self.settings.attr_total_bram] = "-"
            output[self.settings.attr_total_dsp] = "-"
            output[self.settings.attr_total_uram] = "-"
            self.logger.merlin_info("dead node or parent node")
            return output
        if len(dst_list) == 0:
            self.logger.merlin_warning("Did not find dst node")
            return output
        #clear other kernel related dst_id
        kernel_dst_list = []
        for dst in dst_list:
            if len(dst.split("_")) >= 3 and dst.split("_")[2] == self.curr_kernel_num:
                kernel_dst_list.append(dst)
        if kernel_dst_list != []:
            dst_list = kernel_dst_list
        self.logger.merlin_info("Get filtered nodes: ")
        self.logger.merlin_info(dst_list)
        self.logger.merlin_info("Get outermost nested nodes: ")
        self.logger.merlin_info(self.nest_info.filter_outermost(dst_list))
        sche_nodes = self.sched_info.get_sched_info(self.nest_info, self.nest_info.filter_outermost(dst_list))
        self.logger.merlin_info("Get schedule nodes: ")
        self.logger.merlin_info(sche_nodes)
        is_merlin_flatten = ""
        parent_loop = ""
        total_cycles = "0"
        total_unroll_factor = 1
        total_iteration_interval = 0
        tiling_info = ""
        flatten_info = ""
        merlin_flatten_info = ""
        unroll_info = ""
        is_inline = False
        no_dead_code_list = []
        flatten_flag = ""
        for dst in dst_list:
            if self.settings.attr_dead_code not in self.vendor_data[dst]:
                no_dead_code_list.append(dst)
            else:
                if self.settings.attr_flatten in self.vendor_data[dst]:
                    flatten_flag = "yes"

        dst_list = no_dead_code_list
        if len(dst_list) == 0 and flatten_flag == "":
            self.dead_code[src_id] = "removed"
            output[self.settings.attr_cycle_unit] = "-"
            output[self.settings.attr_total_lut] = "-"
            output[self.settings.attr_total_ff] = "-"
            output[self.settings.attr_total_bram] = "-"
            output[self.settings.attr_total_dsp] = "-"
            output[self.settings.attr_total_uram] = "-"
            self.logger.merlin_info("dead node or parent node 2")
            return output
        self.logger.merlin_info("prepare to collect data")
        for one_serial in sche_nodes:
            parallel_cycles = "0"
            for one_parallel_id in one_serial:
                nest_cycles = "0"
                nest_unroll_factor = 1
                nest_iteration_interval = 0
                one_parallel = [one_parallel_id]
                one_parallel_nest = self.nest_info.find_childs(one_parallel_id, 0)
                for one in self.get_all_first_node(one_parallel_nest):
                    if one in dst_list_before_filter:
                        one_parallel.append(one)
                for one_node in one_parallel:
                    # one_nest already be sorted, the first one is the outermost loop
                    self.logger.merlin_info("Compute dst node " + one_node)
                    # if nested loop, only need outer most cycles and resource
                    if one_node == one_parallel[0]:
                        cycle_str = self.topo_info.get_value_from_key(
                            one_node, self.settings.attr_cycle_unit)
                        nest_cycles = self.format_cycle(cycle_str)
                        node_priority = self.topo_info.get_value_from_key(one_node, self.settings.attr_node_priority)
                        if node_priority == self.settings.value_minor:
                            self.logger.merlin_info("find minor node_priority loop, will ignore everything except cycles")
                            break
                    if self.topo_info.get_value_from_key(one_node, self.settings.attr_type) == self.settings.value_kernel:
                        estimated = self.topo_info.get_value_from_key(one_node, self.settings.attr_perf_estimate)
                        target = self.topo_info.get_value_from_key(one_node, self.settings.attr_perf_target)
                        uncertainty = self.topo_info.get_value_from_key(one_node, self.settings.attr_perf_uncertain)
                        output[self.settings.attr_perf_estimate] = estimated
                        output[self.settings.attr_perf_target] = target
                        output[self.settings.attr_perf_uncertain] = uncertainty
                        # computation for estimated freq and target freq
                        target_freq = "0"
                        estimate_freq = "0"
                        estimate_period = "0"
                        target_period = "0"
                        uncertainty_period = "0"
                        target_period = target
                        estimate_period = estimated
                        uncertainty_period = uncertainty
                        if utility.is_number(target_period) and float(target_period) != 0:
                            target_freq = round(1000 / float(target_period))
                            target_freq = int(target_freq)
                        #pylint: disable=too-many-boolean-expressions
                        if utility.is_number(estimate_period) and float(estimate_period) != 0 and\
                            utility.is_number(uncertainty_period) and float(uncertainty_period) != 0 and\
                            utility.is_number(target_period) and float(target_period) != 0:
                            estimate_freq = target_freq
                            if (float(estimate_period) + float(uncertainty_period)) > float(target_period):
                                estimate_freq = round(1000 /(float(estimate_period) + float(uncertainty_period)))
                            estimate_freq = int(estimate_freq)
                        output[self.settings.attr_estimate_freq] = str(estimate_freq)
                        output[self.settings.attr_target_freq] = str(target_freq)
                    if node[self.settings.attr_type] == self.settings.value_kernel \
                        or node[self.settings.attr_type] == self.settings.value_function:
                        # if any node is inline, then mark inline
                        if self.settings.attr_inline not in output:
                            output[self.settings.attr_inline] = ""
                        if one_node in self.vendor_data \
                                and output[self.settings.attr_inline] != "yes":
                            self.topo_info.append_attribute_to_dict(
                                self.vendor_data[one_node], output, self.settings.attr_inline)
                        if one_node in self.vendor_data \
                                and self.settings.attr_inline in self.vendor_data[one_node]:
                            if self.vendor_data[one_node][self.settings.attr_inline] == "yes":
                                self.logger.merlin_info("Is inlined")
                                is_inline = True
                    elif node[self.settings.attr_type] == self.settings.value_loop:
                        # get tiling information
                        # ????? do we need show tiling factor?
                        is_tiling = ""
                        if one_node in self.vendor_data and self.settings.msg_tile in self.vendor_data[one_node]:
                            is_tiling = self.vendor_data[one_node][self.settings.msg_tile]
                        if is_tiling:
                            tiling_info = self.get_tiling_info(one_parallel)
                            self.logger.merlin_info("is tiling")
                            output[self.settings.msg_tile] = "yes"
                            tmp_tile_factor = 1
                            tmp_tile_factor = output.get(self.settings.msg_tile_factor, 1)
                            if utility.is_number(self.vendor_data[one_node][self.settings.msg_tile_factor]):
                                output[self.settings.msg_tile_factor] = tmp_tile_factor * int(self.vendor_data[one_node][self.settings.msg_tile_factor])

                        # check merlin flatten
                        #is_merlin_flatten = ""
                        if one_node in self.vendor_data and self.settings.msg_flatten_loop in self.vendor_data[one_node]:
                            is_merlin_flatten = self.vendor_data[one_node][self.settings.msg_flatten_loop]
                        if is_merlin_flatten:
                            self.logger.merlin_info("is merlin flatten loop")
                            if self.settings.attr_parent_loop in node:
                                parent_loop = node[self.settings.attr_parent_loop]
                                output[self.settings.attr_flatten] = "yes"
                                #output[self.settings.attr_flatten_id] = "merlin_flatten"
                                #location = self.hier_topo_info.get_src_location(parent_loop)
                                self.hier_topo_info.merlin_flatten_parent.append(parent_loop)
                                #merlin_flatten_info = "flattened"
                                flatten_info = "flattened"
                                self.logger.merlin_info("merlin_flatten info = " + merlin_flatten_info)
                            else:
                                self.logger.merlin_warning("but did not have loop flattened with")

                        # ????? what to do if have multiple flatten
                        # get flatten information
                        # if have flatten=yes and flatten-id is not empty,
                        # then it is not outermost flatten loop
                        is_flatten_outermost = True
                        is_flatten = self.topo_info.get_value_from_key(one_node, self.settings.attr_flatten)
                        flatten_id = self.topo_info.get_value_from_key(one_node, self.settings.attr_flatten_id)
                        if is_flatten == "yes":
                            output[self.settings.attr_flatten] = is_flatten
                            output[self.settings.attr_flatten_id] = flatten_id
                            if flatten_id != "":
                                is_flatten_outermost = False
                                flatten_info = self.settings.msg_flattened
                            self.logger.merlin_info("flatten_info = ")
                            self.logger.merlin_info(flatten_info)
                        # get unroll information
                        # ????? if unrolled and unroll factor is not iterger which > 1, should warning
                        is_unroll = self.topo_info.get_value_from_key(one_node, self.settings.attr_unrolled)
                        unroll_factor = self.topo_info.get_value_from_key(one_node, self.settings.attr_unroll_factor)
                        if is_unroll == "yes":
                            if unroll_factor != "" and unroll_factor.isnumeric():
                                nest_unroll_factor *= int(unroll_factor)
                            else:
                                self.logger.merlin_warning("Find a unroll loop, but no unroll factor")
                            self.logger.merlin_info("unroll_factor = " + unroll_factor)

                        # get pipeline information
                        # ????? if flattened loop, will not show pipeline
                        # if not pipeline, will not show not pipeline,
                        # instead, we will use foot note wot tell how to optimize
                        is_pipeline = self.topo_info.get_value_from_key(one_node, self.settings.attr_pipelined)
                        self.logger.merlin_info("is pipeline = " + is_pipeline)
                        iteration_interval = self.topo_info.get_value_from_key(one_node, self.settings.attr_iteration_interval)
                        self.logger.merlin_info("iteration_interval = " + iteration_interval)
                        dep_var_list = self.topo_info.get_value_from_key(one_node, self.settings.attr_dep_var)
                        self.logger.merlin_info("dep_var_list = " + dep_var_list)
                        violation = self.topo_info.get_value_from_key(one_node, self.settings.attr_ii_violation)
                        if violation == "":
                            one_node_nest = self.nest_info.find_childs(one_node, 0)
                            for one in self.get_all_first_node(one_node_nest):
                                one_type = self.topo_info.get_value_from_key(one, self.settings.attr_type)
                                if one_type == self.settings.value_function:
                                    violation = self.topo_info.get_value_from_key(one, self.settings.attr_ii_violation)
                        note_tmp = ""
                        if iteration_interval != "" or is_pipeline == "no":
                            note_tmp = self.get_note_info_for_ii(violation, iteration_interval, is_pipeline, dep_var_list)
                        if note_tmp != "":
                            note = note_tmp
                        # if not pipeline, set II = -1
                        # if pipeline, directly use vendor II
                        if is_pipeline == "yes":
                            if iteration_interval != "" and iteration_interval.isnumeric() \
                                and is_flatten_outermost:
                                nest_iteration_interval = int(iteration_interval)
                            else:
                                self.logger.merlin_warning("Find a pipeline loop without II")
                            self.logger.merlin_info("pipeline II = " + str(iteration_interval))
                        else:
                            if note_tmp != "":
                                self.logger.merlin_warning("Find a none pipeline loop")
                                nest_iteration_interval = -1
                        if self.settings.attr_coarse_pipeline in self.vendor_data[one_node]:
                            output[self.settings.attr_coarse_pipeline] = "yes"

                self.logger.merlin_info("Neste cycles = " + str(nest_cycles))
                # ????? currently, use max unroll factor as the whole factor
                # need discussion on how to show final unroll if split to multiple loops
                total_unroll_factor = max(total_unroll_factor, nest_unroll_factor)
                if total_unroll_factor > 1:
                    self.logger.merlin_info("parallel unroll_factor = " + str(total_unroll_factor))

                # ????? currently, use max II as the whole factor
                # need discussion on how to show final unroll if split to multiple loops
                # if II= -1, it means innermost loop not pipelined because of a reason
                if nest_iteration_interval == -1 or total_iteration_interval == -1:
                    total_iteration_interval = -1
                else:
                    total_iteration_interval = max(total_iteration_interval, nest_iteration_interval)
                if total_iteration_interval >= 1:
                    self.logger.merlin_info("pipeline II = " + str(total_iteration_interval))
                elif total_iteration_interval == -1:
                    self.logger.merlin_info("no pipeline")
                # compute cycles for all parallel nodes
                if nest_cycles == "?":
                    parallel_cycles = "?"
                else:
                    parallel_cycles = max(int(parallel_cycles), int(nest_cycles))
                self.logger.merlin_info("parallel cycles = " + str(parallel_cycles))

            # compute cycles for serial nodes
            self.logger.merlin_info("Parallel cycles = " + str(parallel_cycles))
            if parallel_cycles == "?" or total_cycles == "?":
                total_cycles = "?"
            else:
                total_cycles = sum([int(total_cycles), int(parallel_cycles)])
        if note != "":
            output[self.settings.attr_note] = note
        self.compute_resource(sche_nodes, output, is_inline)
        # add cycles ?????
        # output total cycles
        if total_cycles == 0:
            total_cycles = "-"
        self.logger.merlin_info("total cycles = " + str(total_cycles))
        output[self.settings.attr_cycle_unit] = total_cycles
        # set resource
        if is_inline:
            output[self.settings.attr_comment] = self.settings.msg_inlined
            output[self.settings.attr_comment_resource] = self.settings.msg_inlined
            output[self.settings.attr_cycle_unit] = "-"

        if node[self.settings.attr_type] == self.settings.value_loop:
            # add trip-count
            if self.settings.attr_trip_count in node:
                trip_count = node[self.settings.attr_trip_count]
                if trip_count.isdigit() and int(trip_count) > 0:
                    self.topo_info.append_attribute_to_dict(node, output, self.settings.attr_trip_count)
                else:
                    self.topo_info.get_trip_count(node, output, dst_list)
            str_comment = self.merge_comments(output, tiling_info, flatten_info, \
                total_unroll_factor, total_iteration_interval)
            output[self.settings.attr_comment] = str_comment
            if is_merlin_flatten:
                merlin_parent_pipe_info = ""
                if total_iteration_interval > 0:
                    merlin_parent_pipe_info = "pipeline II=" + str(total_iteration_interval)
                self.flatten_loop_info[parent_loop] = merlin_parent_pipe_info
            if note != "":
                output[self.settings.attr_comment] += self.settings.merlin_note
            output[self.settings.attr_comment_resource] = unroll_info
            self.logger.merlin_info("performance comment = " + str_comment)
            self.logger.merlin_info("resource comment = " + str_comment)
            self.logger.merlin_info("output for loop " + str(output))
        return output

    def merge_comments(self, output, tiling_info, \
                       flatten_info, total_unroll_factor, total_iteration_interval):
        '''
        merge and add comments
        ????? currently do simple merge, need review
        when find tiling, then report tiling
        when find flatten, then report flatten
        when find unroll, nested loop do *=, then get max unroll
        when find pipeline, get max II
        '''
        comments = []
        # show tiling info
        if tiling_info != "":
            comments.append(tiling_info)
        # show merlin flatten info
        #if merlin_flatten_info != "":
        #    comments.append(merlin_flatten_info)
        # show flatten info
        if flatten_info != "":
            comments.append(flatten_info)
        # show unroll info
        if total_unroll_factor > 1:
            output[self.settings.attr_unroll_factor] = total_unroll_factor
            unroll_info = "parallel factor=" + str(total_unroll_factor) + "x"
        else:
            unroll_info = ""
        if unroll_info != "":
            comments.append(unroll_info)
        # show pipeline and II info
        pipeline_info = ""
        if total_iteration_interval > 0:
            pipeline_info = "pipeline II=" + str(total_iteration_interval)
        elif total_iteration_interval == -1:
            pipeline_info = "no pipeline"
        if self.settings.attr_coarse_pipeline in output:
            pipeline_info = self.settings.attr_pipelined
        if flatten_info == self.settings.msg_flattened or flatten_info != "":
            # if flattened, should not be pipelined
            # pipeline should applied on flatten parent
            pipeline_info = ""
        if pipeline_info != "":
            comments.append(pipeline_info)
        # merge comments
        str_comment = ', '.join(comments)
        #str_comment = ""
        #for one_comment in comments:
        #    if one_comment != "":
        #        str_comment += one_comment
        #        if comments.index(one_comment) < len(comments) - 1:
        #            str_comment += ", "
        #        else:
        #            str_comment += ""
        return str_comment

    def get_resource_for_call(self, node):
        '''
        get resource data from declaration
        '''
        # ?????: temp directly copy last C code resource to source code resource
        # if src function split to several dst functions, this may have issues
        # we need first get all cycles of func decl resource in src code, then can
        # get the func call resource
        # or start a new loop to compute function call resource separately
        dict_output = {}
        dict_output[self.settings.attr_total_lut] = '-'
        dict_output[self.settings.attr_total_ff] = '-'
        dict_output[self.settings.attr_total_bram] = '-'
        dict_output[self.settings.attr_total_dsp] = '-'
        dict_output[self.settings.attr_total_uram] = '-'
        if self.settings.attr_function_id in node:
            function_decl_id = node[self.settings.attr_function_id]
            if function_decl_id in self.vendor_data:
                is_inline = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_inline)
                if not is_inline:
                    total_lut = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_total_lut)
                    total_bram = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_total_bram)
                    total_dsp = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_total_dsp)
                    total_ff = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_total_ff)
                    total_uram = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_total_uram)
                    util_lut = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_util_lut)
                    util_bram = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_util_bram)
                    util_dsp = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_util_dsp)
                    util_ff = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_util_ff)
                    util_uram = self.topo_info.get_value_from_key(function_decl_id, self.settings.attr_util_uram)
                    dict_output[self.settings.attr_total_lut] = self.resource_show(total_lut, util_lut)
                    dict_output[self.settings.attr_total_ff] = self.resource_show(total_ff, util_ff)
                    dict_output[self.settings.attr_total_bram] = self.resource_show(total_bram, util_bram)
                    dict_output[self.settings.attr_total_dsp] = self.resource_show(total_dsp, util_dsp)
                    dict_output[self.settings.attr_total_uram] = self.resource_show(total_uram, util_uram)
        return dict_output

    def format_cycle(self, cycle_str):
        '''
        format cycles number
        if integer and >0, directly use it
        if none or ignored, use 0
        others, use ?
        '''
        cycle_out = 0
        if cycle_str.isdigit() and int(cycle_str) >= 0:
            cycle_out = int(cycle_str)
        elif cycle_str in ("", "ignore"):
            cycle_out = 0
        else:
            cycle_out = "?"
        return cycle_out

    def fill_call_function_info(self, src_node):
        '''
        fill callfunction info
        '''
        name = ""
        if self.settings.attr_name in src_node:
            name = src_node[self.settings.attr_name]
        self.logger.merlin_info("Start to compute for call " + name)
        node_output = {}

        dst_nodes = self.topo_info.get_dest_nodes(src_node)
        if len(dst_nodes) == 0:
            self.logger.merlin_warning("Did not find dst node")
            return node_output
        dst_node = dst_nodes[0]
        node_output[self.settings.attr_comment] = "-"
        node_output[self.settings.attr_comment_resource] = "-"
        if self.settings.attr_name in dst_node and \
                dst_node[self.settings.attr_name] in self.function_for_name and \
                self.function_for_name[dst_node[self.settings.attr_name]].get(self.settings.attr_inline) == "yes":
            node_output[self.settings.attr_comment] = self.settings.msg_inlined
            node_output[self.settings.attr_comment_resource] = self.settings.msg_inlined
            node_output[self.settings.attr_comment_resource] = \
                    self.settings.msg_inlined
            node_output[self.settings.attr_cycle_unit] = "-"
        else:
            key = self.settings.attr_cycle_unit
            if key in dst_node:
                node_output[key] = self.format_cycle(dst_node[key])
        dict_resource = self.get_resource_for_call(dst_node)
        node_output.update(dict_resource)

        self.logger.merlin_info("call output: " + str(node_output))
        return node_output

#    #pylint: disable=unused-argument
#    #pylint: disable=no-self-use
#    def fill_array_info(self, node):
#        '''
#        fill array info
#        '''
#        return {}
#
    #pylint: disable=too-many-branches
    def merge_one_node(self, node):
        '''
        Recursively merge node, compute the final data.
        '''
        node_dict = {}
        if self.settings.attr_type in node:
            # fill common data
            node_dict.update(self.topo_info.get_src_common_attributes(node))
            #kernel topo id
            if node[self.settings.attr_type] == self.settings.value_kernel:
                self.curr_kernel_id = node[self.settings.attr_src_topo_id]
            # merge loop and function information
            if node[self.settings.attr_type] == self.settings.value_kernel \
                or node[self.settings.attr_type] == self.settings.value_loop \
                or node[self.settings.attr_type] == self.settings.value_function:
                node_dict.update(self.fill_function_loop_info(node))
            else:
                pass
            # merge interface information
            if (node[self.settings.attr_type] == self.settings.value_kernel and
                    self.settings.attr_interfaces in node):
                list_tmp = []
                for one_interface in node[self.settings.attr_interfaces]:
                    list_tmp.append(self.fill_interface_info(one_interface))
                node_dict[self.settings.attr_interfaces] = list_tmp
                self.burst_analyzer.interchange_memcpy()

            # merge call information
            if node[self.settings.attr_type] == self.settings.value_call:
                node_dict.update(self.fill_call_function_info(node))
            else:
                pass

#            # merge array information
#            if self.settings.attr_arrays in node:
#                list_tmp = []
#                for _ in node[self.settings.attr_arrays]:
#                    list_tmp.append(self.fill_array_info(node))
#                node_dict[self.settings.attr_arrays] = list_tmp
#
            list_tmp = []
            # add all read memory burst which parent is this node
            # this memory burst node have no siblings
            if self.settings.attr_src_topo_id in node:
                childs_memory_burst = self.burst_analyzer.get_childs_memory_burst(
                    node[self.settings.attr_src_topo_id])
                for child_memory_burst in childs_memory_burst:
                    if child_memory_burst in self.vendor_data and \
                       self.settings.attr_is_read in self.vendor_data[child_memory_burst] and \
                       self.vendor_data[child_memory_burst][self.settings.attr_is_read] == "1":
                        insert_dict = self.topo_info.get_insert_node_info(
                            child_memory_burst,
                            self.burst_analyzer.location_cross_list)
                        list_tmp.append(insert_dict)

            # recursive compute childs
            asserted_memcpy_node = []
            if self.settings.attr_childs in node:
                for child in node[self.settings.attr_childs]:
                    node_before_list = []
                    node_after_list = []
                    if self.settings.attr_src_topo_id in child:
                        child_id = child[self.settings.attr_src_topo_id]
                        self.logger.merlin_info("Compute child id = " + child_id)
                        (node_before_list, node_after_list) = self.burst_analyzer.get_before_and_after_list(child_id)
                        if node_before_list != [] and (child_id in self.dead_code or \
                            (self.settings.attr_parent_loop in child and  \
                            child[self.settings.attr_parent_loop] in self.dead_code)):
                            node_before_list = []
                        if node_after_list != [] and (child_id in self.dead_code or \
                            (self.settings.attr_parent_loop in child and \
                            child[self.settings.attr_parent_loop] in self.dead_code)):
                            node_after_list = []
                        for one_direction_node in node_before_list:
                            insert_dict = self.topo_info.get_insert_node_info(one_direction_node, \
                                                                             self.burst_analyzer.location_cross_list)
                            if one_direction_node not in asserted_memcpy_node:
                                list_tmp.append(insert_dict)
                                self.logger.merlin_info("insert info before node = " + str(insert_dict))
                                asserted_memcpy_node.append(one_direction_node)
                        list_tmp.append(self.merge_one_node(child))
                        for one_direction_node in node_after_list:
                            insert_dict = self.topo_info.get_insert_node_info(one_direction_node, \
                                                                             self.burst_analyzer.location_cross_list)
                            if one_direction_node not in asserted_memcpy_node:
                                list_tmp.append(insert_dict)
                                self.logger.merlin_info("insert info after node = " + str(insert_dict))
                                asserted_memcpy_node.append(one_direction_node)

            # add all write memory burst which parent is this node
            # this memory burst node have no siblings
            if self.settings.attr_src_topo_id in node:
                childs_memory_burst = self.burst_analyzer.get_childs_memory_burst(
                    node[self.settings.attr_src_topo_id])
                for child_memory_burst in childs_memory_burst:
                    if child_memory_burst in self.vendor_data and \
                       self.settings.attr_is_write in self.vendor_data[child_memory_burst] and \
                       self.vendor_data[child_memory_burst][self.settings.attr_is_write] == "1":
                        insert_dict = self.topo_info.get_insert_node_info(
                            child_memory_burst,
                            self.burst_analyzer.location_cross_list)
                        list_tmp.append(insert_dict)

            node_dict[self.settings.attr_childs] = list_tmp

            # recursive compute functions
            if self.settings.attr_sub_functions in node:
                list_tmp = []
                for sub_function in node[self.settings.attr_sub_functions]:
                    list_tmp.append(self.merge_one_node(sub_function))
                node_dict[self.settings.attr_sub_functions] = list_tmp

        return node_dict

    #pylint: disable=too-many-branches
    def scan_one_node(self, node, topoid, key, value):
        '''
        Recursively m find and update node, compute the final data.
        '''
        node_dict = {}
        if self.settings.attr_type in node:
            # fill common data
            for one_key in node:
                if one_key not in (self.settings.attr_interfaces, \
                                   self.settings.attr_interfaces, \
                                   self.settings.attr_childs, \
                                   self.settings.attr_arrays, \
                                   self.settings.attr_sub_functions):
                    node_dict[one_key] = node[one_key]
                    if one_key == self.settings.attr_src_topo_id:
                        self.logger.merlin_info("searching id " + node[one_key])
                        if node[one_key] == topoid:
                            self.logger.merlin_info("update key =  " + key + ", value = " + value)
                            node_dict[key] = value
                            if topoid in self.flatten_loop_info and \
                                self.flatten_loop_info[topoid] != "":
                                node_dict[self.settings.attr_comment] = \
                                self.flatten_loop_info[topoid]
                            else:
                                node_dict[self.settings.attr_comment] = \
                                    self.settings.msg_flattened
                            node_dict[self.settings.attr_flatten_id] = \
                                    self.settings.msg_merlin_flatten
                            node_dict[self.settings.attr_cycle_unit] = "-"
                            node_dict[self.settings.attr_total_lut] = "-"
                            node_dict[self.settings.attr_total_ff] = "-"
                            node_dict[self.settings.attr_total_bram] = "-"
                            node_dict[self.settings.attr_total_dsp] = "-"
                            node_dict[self.settings.attr_total_uram] = "-"
                else:
                    list_tmp = []
                    for one_child in node[one_key]:
                        list_tmp.append(self.scan_one_node(one_child, topoid, key, value))
                    node_dict[one_key] = list_tmp
        return node_dict

    def update_one_data_in_hierarchy(self, filled_data, topoid, key, value):
        '''
        update one data key value in json file
        '''
        new_list = []
        for kernel in filled_data:
            new_list.append(self.scan_one_node(kernel, topoid, key, value))
        return new_list

    def post_process_for_output_data(self, filled_data):
        '''
        Do post process for filled data
        1. filled info to merlin flattened loop, the parent loop of
        merlin flatten info not exist in vendor.json, must do here
        '''
        self.logger.merlin_info("Do post process for filled data")
        for one_id in self.hier_topo_info.merlin_flatten_parent:
            key = self.settings.attr_flatten
            value = "yes"
            filled_data = self.update_one_data_in_hierarchy(filled_data, \
                                                            one_id, key, value)
        return filled_data

    def fill_source_data(self):
        '''
        Compute to final data, and fill into output file
        '''
        self.logger.merlin_info("Start to merge input and output json files")
        for kernel in self.hierarchy_data:
            if not self.settings.attr_name in kernel:
                self.logger.merlin_critical_warning("kernel has no attribute 'name'")
                continue
            self.logger.merlin_info("Start to merge kernel: " + \
                                    kernel[self.settings.attr_name])
            self.filled_data.append(self.merge_one_node(kernel))
        self.filled_data = self.post_process_for_output_data(self.filled_data)
        utility.dump_to_json(self.json_filled_data, self.filled_data, "nosort")

#if __name__ == '__main__':
#    LOGGER = merlin_log.MerlinLog()
#    LOGGER.set_merlin_log()
#    SETTINGS = merlin_report_setting.MerlinReportSetting()
#    AA = FillSourceData(LOGGER, SETTINGS)
#    AA.fill_source_data()
