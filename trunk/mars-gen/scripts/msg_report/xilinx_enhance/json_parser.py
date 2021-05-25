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
This module contains useful utilities in JSON analysis.
'''

import re
#import json

#pylint: disable=invalid-name, too-many-lines

class NestedLevel:
    """ Helper class for analyzing nested level """
    #pylint: disable=too-many-arguments
    def __init__(self, settings, logger, topo, func_for_name, vendor_data, inlining=True):
        self.logger = logger
        self.settings = settings
        self._map = NestedLevel._start(self, topo, func_for_name, inlining)
        self.functions = vendor_data

    def _start(self, topo, func_for_name, inlining):
        functions = []
        for kernel in topo:
            functions.extend(kernel.get(self.settings.attr_sub_functions, []))
            functions.append(kernel)

        def get_function_body(name):
            res = []
            for func in functions:
                if func.get(self.settings.attr_name) == name:
                    res = func.get(self.settings.attr_childs, [])
                    break
            return res

        def build(obj, scope):
            try:
                if obj[self.settings.attr_topo_id] \
                   == self.settings.value_is_skipped_node:
                    pass

                elif obj[self.settings.attr_type] in \
                        (self.settings.value_kernel, \
                         self.settings.value_function, \
                         self.settings.value_loop):
                    res = [obj[self.settings.attr_topo_id], []]
                    for x in obj[self.settings.attr_childs]:
                        build(x, res[1])
                    scope.append(res)

                elif obj[self.settings.attr_type] == self.settings.value_call:
                    if inlining:
                        callee = func_for_name[obj[self.settings.attr_name]]
                        if callee.get(self.settings.attr_inline) == "yes":
                            for x in get_function_body( \
                                        obj[self.settings.attr_name]):
                                build(x, scope)
                        else:
                            scope.append(obj[self.settings.attr_topo_id])
                    else:
                        scope.append(obj[self.settings.attr_topo_id])

            except (KeyError, IndexError) as e:
                self.logger.merlin_critical_warning(e)

        def hashing(compound, res):
            res[compound[0]] = compound[1]
            for x in compound[1]:
                if isinstance(x, list):
                    hashing(x, res)
            return res

        res = {}
        for func in functions:
            try:
                ls = []
                build(func, ls)
                hashing(ls[0], res)
            except IndexError as e:
                self.logger.merlin_critical_warning(e)
        return res

    def find_all_functions(self):
        """ Returns all topo IDs for function definition """
        return list(filter(lambda x: x[:2] == "F_", self._map))

    #pylint: disable=too-many-nested-blocks
    def find_childs(self, topo_id, mode_sel):
        """ Find all objects in the scope of the given node

        Returns:
            A list of the objects or [] if it contains none
        mode_sel == 1: direct search
        mode_sel == 0: recursion search
        """
        res = []
        def find_call_function(topo_id):
            res = []
            for x in topo_id:
                if isinstance(x, list):
                    res.extend(find_call_function(x))
                else:
                    if x in self.functions and self.functions[x].get(self.settings.attr_type)\
                                                        == self.settings.value_call:
                        callee_func_id = self.functions[x].get(self.settings.attr_function_id)
                        sub_func_id = self._map.get(callee_func_id, [])
                        if sub_func_id == []:
                            res.append(callee_func_id)
                        res.extend(find_call_function(sub_func_id))
                    else:
                        res.append(x)
            return [res]
        if mode_sel == 1:
            return self._map.get(topo_id, [])
        else:
            if topo_id in self.functions:
                if self.functions[topo_id].get(self.settings.attr_type) == self.settings.value_call:
                    callee_function_id = self.functions[topo_id].get(self.settings.attr_function_id)
                    sub_id = self._map.get(callee_function_id, [])
                    res = find_call_function(sub_id)
                    return res
                else:
                    #return self._map.get(topo_id, [])
                    sub_id = self._map.get(topo_id, [])
                    # filter memcpy id remove dead code loop
                    sub_id_tmp = []
                    for i in sub_id:
                        if isinstance(i, list):
                            tmp_dict = {}
                            tmp_dict = self.functions.get(i[0], {})
                            if "dead_code" not in tmp_dict:
                                sub_id_tmp.append(i)
                        else:
                            if "dead_code" not in self.functions[i]:
                                sub_id_tmp.append(i)

                    res = find_call_function(sub_id_tmp)
                    return res



    def find_child_ids(self, topo_id):
        """ Returns a list of IDs of the direct children of the specified
            node in the argument.
        """
        res = []
        for x in self._map.get(topo_id, []):
            try:
                if isinstance(x, list):
                    res.append(x[0])
                else:
                    res.append(x)
            except IndexError as e:
                self.logger.merlin_critical_warning(e)
        return res

    def filter_outermost(self, topo_ids):
        """ Returns a list of topo IDs representing the outermost nodes
            from the given topo IDs.
        """
        def nested_into(tid, scope):
            for x in filter(lambda x: not isinstance(x, list), scope):
                if x == tid:
                    return True
            for x in filter(lambda x: isinstance(x, list), scope):
                try:
                    if x[0] == tid:
                        return True
                    elif nested_into(tid, x[1]):
                        return True
                except IndexError as e:
                    self.logger.merlin_critical_warning(e)
                    return False
            return False

        res = []
        s = set(topo_ids)
        for topo_id in s:
            for other in s-set([topo_id]):
                if nested_into(topo_id, self._map.get(other, [])):
                    break
            else:
                res.append(topo_id)
        return res

    def get_scope_id(self, topo_id):
        """ Returns the topo ID of the parent node of the node specified in
            the argument.

            This is an alternate way in case "scope_org_identifier" gives the
            value "ignore".
        """
        for k, v in self._map.items():
            for x in v:
                try:
                    if isinstance(x, list):
                        if x[0] == topo_id:
                            return k
                    elif x == topo_id:
                        return k
                except IndexError as e:
                    self.logger.merlin_critical_warning(e)
        return None

    def get_top_scope_id(self, topo_id):
        """ Returns the topo ID of the outermost function definition to which
            the node specified in the argument belongs.
        """
        sid = self.get_scope_id(topo_id)
        if sid is None:
            return None

        while sid is not None:
            topo_id = sid
            sid = self.get_scope_id(topo_id)
        return topo_id

#pylint: disable=too-few-public-methods, too-many-arguments
class Schedule:
    """ Helper class for analyzing the schedule of function invocation """
    def __init__(self, settings, logger, adb_data, nest_info, vendor_data):
        self.logger = logger
        self.settings = settings
        self._map = Schedule._start(self, adb_data, nest_info, vendor_data)

    def _start(self, adb_data, nest_info, vendor_data):
        def read_adb_data(adb_data):
            res = {}
            for k, v in adb_data.items():
                res[k] = {}
                for state, string in v.items():
                    for name in set([re.sub(r"\.\d+", "", name)
                                     for name in string.split()]):
                        res[k][name] = state
            return res

        def find(scope):
            res = []
            for x in scope:
                if isinstance(x, list):
                    if len(x) > 1:
                        res.extend(find(x[1]))
                else:
                    res.append(x)
            return res

        state = {}
        adb = read_adb_data(adb_data)

        for fid in nest_info.find_all_functions():
            unknown = -1
            if fid in vendor_data \
                and self.settings.attr_name in vendor_data[fid]:
                func_name = vendor_data[fid][self.settings.attr_name]
                for cid in find(nest_info.find_childs(fid, 1)):
                    if cid in vendor_data \
                        and self.settings.attr_name in vendor_data[cid]:
                        callee_name = vendor_data[cid][self.settings.attr_name]
                        if callee_name in adb.get(func_name, {}):
                            state[cid] = adb[func_name][callee_name]
                        else:
                            state[cid] = str(unknown)
                            unknown -= 1
        return state

    #pylint: disable=too-many-branches, too-many-nested-blocks
    def get_sched_info(self, nest_info, call_ids):
        """ Returns a list in which each element is also a list of function
            call topo IDs from the given argument. The calls in the same
            sub-list are in parallel execution while they are executed in
            serial within different sub-lists.
        """
        d = {}
        def find_all_sub_functions(search_id):
            res = []
            for x in search_id:
                if isinstance(x, list):
                    if len(x) >= 1:
                        res.extend(find_all_sub_functions(x))
                else:
                    if not x.startswith("L_"):
                        res.append(x)
            return res

        def dict_filter_remove_duplicate(dict_in):
            res = {}
            values = []
            for k, v in dict_in.items():
                if v not in values:
                    res[k] = v
                    values.append(v)
            return res

        for cid in call_ids:
            if cid.startswith('L_'):
                search_ids = nest_info.find_childs(cid, 1)
                cid_sub_func = find_all_sub_functions(search_ids)
                finish_flag = 0
                if cid_sub_func:
                    for x in cid_sub_func:
                        state = self._map.get(x)
                        if state:
                            if state in d:
                                d[state].append(cid)
                                finish_flag = 1
                                break
                            if state not in d:
                                if state.lstrip('-').isdigit():
                                    if int(state) > 0:
                                        d[state] = [cid]
                                        finish_flag = 1
                                        break
                                    if int(state) <= 0:
                                        d[cid] = [cid]
                    if finish_flag == 1:
                        if cid in d:
                            del d[cid]
                else:
                    state = self._map.get(cid, cid)
                    d[state] = [cid]
            else:
                state = self._map.get(cid, cid)
                if state in d:
                    d[state].append(cid)
                else:
                    d[state] = [cid]
        d = dict_filter_remove_duplicate(d)
        return list(d.values())

class Topo:
    """ Helper class for accessing nodes from vendor.json """
    def __init__(self, settings, logger, vendor_data, hier_topo_info):
        self.settings = settings
        self.logger = logger
        self.vendor_data = vendor_data
        self.hier_topo_info = hier_topo_info

        self.src_id_to_dst_id = {}
        self.dst_id_to_src_id = {}
        self.array_list = []
        self._build()

    def _build(self):
        '''
        Build the dict from src topo id to dst topo id
        '''
        self.logger.merlin_info("Building map for src_id and dst_id...")
        for dst_id, value in self.vendor_data.items():
            src_id = value.get(self.settings.attr_org_identifier)
            self.dst_id_to_src_id[dst_id] = src_id
            if src_id:
                if src_id in self.src_id_to_dst_id:
                    self.src_id_to_dst_id[src_id].append(dst_id)
                else:
                    self.src_id_to_dst_id[src_id] = [dst_id]
            # add interface to array list
            if self.settings.attr_type in value \
                and value[self.settings.attr_type] == "interface" \
                and self.settings.attr_var_type in value \
                and value[self.settings.attr_var_type] == "array" \
                and self.settings.attr_name in value:
                self.array_list.append(value[self.settings.attr_name])
            # add array to array list
            if self.settings.attr_type in value \
                and value[self.settings.attr_type] == "array" \
                and self.settings.attr_name in value:
                self.array_list.append(value[self.settings.attr_name])
        self.logger.merlin_info("Printing src_id to dst_id,,,")
        self.logger.merlin_info(self.src_id_to_dst_id)
        self.logger.merlin_info("Printing dst_id to src_id,,,")
        self.logger.merlin_info(self.dst_id_to_src_id)
        self.logger.merlin_info("Printing array list,,,")
        self.logger.merlin_info(self.array_list)

    def get_src_id(self, dst_id):
        """ Map topo id from merlin output code to that from merlin input code
        """
        if dst_id in self.dst_id_to_src_id:
            return self.dst_id_to_src_id[dst_id]
        else:
            return ""

    def get_dst_id(self, src_id):
        """ Map topo id from merlin input code to the ones from merlin output
            code
        """
        if src_id in self.src_id_to_dst_id:
            return self.src_id_to_dst_id[src_id]
        else:
            return []

    def get_dest_nodes(self, src_node):
        """ Returns a list of nodes in the merlin output code corresponding
            to the node in the merlin input code
        """
        res = []
        try:
            src_id = src_node[self.settings.attr_src_topo_id]
            dst_ids = self.src_id_to_dst_id[src_id]
            self.logger.merlin_info("dst_ids = ")
            self.logger.merlin_info(dst_ids)
            #self.logger.merlin_info("vendor_data = ")
            #self.logger.merlin_info(self.vendor_data)
            for one_id in dst_ids:
                node = self.vendor_data[one_id]
                #self.logger.merlin_info("one_node = ")
                #self.logger.merlin_info(node)
                node[self.settings.attr_topo_id] = one_id
                res.append(node)
        except KeyError as e:
            self.logger.merlin_critical_warning(e)
        return res


    def get_value_from_key(self, node, key):
        '''
        extract information from messages,
        messages is string, but can load as json file
        '''
        value = ""
        try:
            return self.vendor_data[node][key]
        except KeyError as e:
            self.logger.merlin_critical_warning(e)
        return value

    def append_attribute_to_dict(self, node, node_output, key):
        '''
        append specific attribute to dict
        '''
        if key in node:
            if key in node_output:
                self.logger.merlin_warning("Already have data: " + key)
            else:
                node_output[key] = node[key]
                #self.logger.merlin_info("Get data " + key + "=" + str(node[key]))
        else:
            node_output[key] = ""
            #self.logger.merlin_warning("Do not have data: " + key)

    def get_src_common_attributes(self, node):
        '''
        collect all common attribtues
        '''
        node_output = {}
        for one_key in node:
            if one_key not in (self.settings.attr_interfaces,
                               self.settings.attr_interfaces,
                               self.settings.attr_childs,
                               self.settings.attr_arrays,
                               self.settings.attr_sub_functions):
                node_output[one_key] = node[one_key]

        label_name = self.settings.attr_label_name
        if label_name in node:
            if node[label_name] != "":
                node_output[self.settings.attr_name] = node[label_name]
        return node_output

    def get_trip_count(self, node, output, dst_list):
        '''
        if one to one node, use ouput code tripcount
        it one to N node, use input code tripcount
        '''
        # should come from message to get accurate number
        is_one_to_one_node = len(dst_list) == 1
        tc = self.settings.attr_trip_count
        if is_one_to_one_node and tc in self.vendor_data[dst_list[0]]:
            output[tc] = self.vendor_data[dst_list[0]][tc]
        else:
            self.append_attribute_to_dict(node, output, tc)
            if tc in output:
                if output[tc] == "1" or output[tc] == "":
                    max_tc = 1
                    for dst in dst_list:
                        if tc in self.vendor_data[dst] and \
                            self.vendor_data[dst][tc].isnumeric():
                            max_tc = max(max_tc, int(self.vendor_data[dst][tc]))
                    output[tc] = str(max_tc)
                else:
                    self.append_attribute_to_dict(node, output, tc)
        self.logger.merlin_info("trip_count = " + output[tc])

    #pylint: disable=too-many-locals, too-many-branches, too-many-statements, too-many-nested-blocks
    def get_insert_node_info(self, node, cross_list):
        '''
        get inserted node info, including name, cycles, typee, etc.
        '''
        output = {}
        self.logger.merlin_info("node = " + node)
        self.logger.merlin_info("cross list = " + str(cross_list))
        if node not in self.vendor_data:
            return output
        # get cycls
        cycles = self.settings.attr_cycle_unit
        org_id = self.settings.attr_org_identifier
        # parse all crossed node, if cross loop, the cycles need to calibrate
        # cycles = org_cycles / factor
        # if factor *= trip_count / unroll_factor
        factor = 1
        self.logger.merlin_info("cross list = " + str(cross_list))
        self.logger.merlin_info("node = " + node)
        if node in cross_list:
            for one_cross_id in cross_list[node]:
                trip_count = self.get_value_from_key(
                    one_cross_id, self.settings.attr_lc_trip_count)
                if trip_count.isdigit():
                    factor = int(factor) * int(trip_count)
                    unroll_factor = self.get_value_from_key(
                        one_cross_id, self.settings.attr_unroll_factor)
                    if not unroll_factor.isdigit() or int(unroll_factor) <= 0:
                        unroll_factor = 1
                    factor = int(int(factor) / int(unroll_factor))
            self.logger.merlin_info("cycles factor = " + str(factor))

        try:
            output[cycles] = self.vendor_data[node][cycles]
            if output[cycles].isdigit():
                output[cycles] = int(int(output[cycles]) / int(factor))
            self.logger.merlin_info("cycles = " + str(output[cycles]))
        except KeyError as e:
            self.logger.merlin_critical_warning(e)

        # get name displayed in table
        display_name = ""
        is_write = ""
        is_read = ""
        burst_length = ""
        if node in self.vendor_data:
            #print("node = ")
            #print(self.vendor_data[node])
            if self.settings.attr_display_name in self.vendor_data[node]:
                display_name = self.vendor_data[node][self.settings.attr_display_name]
                self.logger.merlin_info("topo_id_map = ")
                self.logger.merlin_info(self.hier_topo_info.topo_id_name_map)
                if org_id in self.vendor_data[node] \
                    and self.vendor_data[node][org_id] in self.hier_topo_info.topo_id_name_map:
                    # if original C port name different with output name, use original name instead
                    org_name = self.hier_topo_info.topo_id_name_map[self.vendor_data[node][org_id]]
                    if org_name != "":
                        regexp = (r"auto memory burst.*'(\S+)'")
                        match = re.search(regexp, display_name)
                        if match:
                            (name, ) = match.groups()
                            if name != org_name:
                                self.logger.merlin_info("src org name = " + org_name)
                                self.logger.merlin_info("output new name = " + name)
                                display_name = "auto memory burst for \'" + org_name + "\'"
                self.logger.merlin_info("display_name = " + display_name)
            elif self.settings.attr_org_identifier in self.vendor_data[node]:
                # if no display name, directly find the org id name
                sid = self.vendor_data[node][self.settings.attr_org_identifier]
                src_location = self.hier_topo_info.get_src_location(sid)
                # delete line number
                regexp = (r"(.*) \(")
                match = re.search(regexp, src_location)
                if match:
                    (name, ) = match.groups()
                    src_location = name
                display_name = "auto memory burst for " + src_location
                self.logger.merlin_info("display_name = " + display_name)
            if self.settings.attr_is_write in self.vendor_data[node]:
                is_write = self.vendor_data[node][self.settings.attr_is_write]
            if self.settings.attr_is_read in self.vendor_data[node]:
                is_read = self.vendor_data[node][self.settings.attr_is_read]
            if self.settings.attr_burst_length_max in self.vendor_data[node]:
                burst_length = self.vendor_data[node][self.settings.attr_burst_length_max]
        direction = "read"
        if is_write == "1":
            direction = "write"
        elif is_read == "1":
            direction = "read"
        name = display_name + "(" + direction + ")"
        output[self.settings.attr_name] = name
        # give a type
        output[self.settings.attr_type] = self.settings.value_burst
        comment = self.settings.attr_comment
        if burst_length == "":
            output[comment] = ""
        elif burst_length.isdigit():
            burst_length = int(int(burst_length) / int(factor))
            output[comment] = "cache size=" + str(burst_length) + "B"
        else:
            if factor != 1:
                burst_length += "/" + str(factor)
            output[comment] = "cache size=" + str(burst_length) + "B"

        # do not show the resource for memory burst call
        output[self.settings.attr_total_lut] = '-'
        output[self.settings.attr_total_ff] = '-'
        output[self.settings.attr_total_bram] = '-'
        output[self.settings.attr_total_dsp] = '-'
        output[self.settings.attr_total_uram] = '-'
        output[self.settings.attr_comment_resource] = ""
        return output

    def index_function(self, key):
        '''
        Return mapping from function attribute specified in the argument
        to the function.
        '''
        res = {}
        for k, v in self.vendor_data.items():
            if v.get(self.settings.attr_type) in \
               (self.settings.value_function, self.settings.value_kernel):
                v[self.settings.attr_topo_id] = k
                if key in v:
                    res[v[key]] = v
        return res

    def index_function_call(self, key):
        '''
        Return mapping from function attribute specified in the argument
        to its calls.
        '''
        res = {}
        for k, v in self.vendor_data.items():
            if v.get(self.settings.attr_type) == self.settings.value_call:
                v[self.settings.attr_topo_id] = k
                if key in v:
                    res.setdefault(v[key], []).append(v)
        return res

    def get_array_decl_topoid_by_name(self, var):
        '''
        input an array name
        output the array declaration topoid
        '''
        topo_id = ""
        for one_id in self.vendor_data:
            try:
                one_node = self.vendor_data[one_id]
                attr_type = one_node[self.settings.attr_type]
                attr_name = one_node[self.settings.attr_name]
                if attr_name == var and attr_type in \
                   (self.settings.value_array, \
                   self.settings.value_interface):
                    return one_id
            except KeyError as e:
                self.logger.merlin_critical_warning(e)
                continue
        return topo_id

#pylint: disable=too-many-instance-attributes
class HierTopo:
    """ Helper class for accessing nodes from hierarchy.json """
    def __init__(self, settings, logger, hierarchy_data):
        self.settings = settings
        self.logger = logger
        self.hierarchy_data = hierarchy_data
        self.topo_info = HierTopo.index_hierarchy(self, hierarchy_data)
        self.src_topo_id_set = []
        self.topo_id_name_map = {}
        # src_topo_id_correct, now used to map loop and function body
        # to loop and function itself
        self.src_topo_id_map = {}
        self.src_location = {}
        self.build_map_of_src_location()
        self.logger.merlin_info("src topo id sets = ")
        self.logger.merlin_info(self.src_topo_id_set)
        self.merlin_flatten_parent = []

    def index_hierarchy(self, data):
        """ Hashing all hierarchies of nodes into dictionary """
        def index_obj(obj):
            res = {}
            #pylint: disable=unused-variable
            for k, v in obj.items():
                if isinstance(v, dict):
                    res.update(index_obj(v))
                elif isinstance(v, list):
                    for x in v:
                        res.update(index_obj(x))
                else:
                    if self.settings.attr_src_topo_id in obj:
                        res[obj[self.settings.attr_src_topo_id]] = obj
            return res

        res = {}
        for kernel in data:
            res.update(index_obj(kernel))
        return res

    def get_node(self, topo_id):
        """ Returns the node of the topo ID in the given argument """
        return self.topo_info[topo_id]

    def get_node_show_name(self, node):
        '''
        get show namne including file name, line number and node name
        '''
        show_name = ""
        if self.settings.attr_name in node \
            and self.settings.attr_src_filename in node \
            and self.settings.attr_src_line in node:
            show_name = ("\'" + node[self.settings.attr_name] + "\' (" \
                + node[self.settings.attr_src_filename] + ":" \
                + str(node[self.settings.attr_src_line]) + ")")
        return show_name

    #pylint: disable=line-too-long, too-many-branches
    def get_location_recursive(self, node, src_location):
        '''
        build the map for src id and it's location, including file name,
        line number and node name
        we only need loop and function name to show to user
        '''
        src_topo_id = self.settings.attr_src_topo_id
        attr_name = self.settings.attr_name
        if src_topo_id in node:
            # fill common data
            if self.settings.attr_type in node \
               and node[self.settings.attr_type] == self.settings.value_loop:
                src_location[node[src_topo_id]] = "loop " \
                    + self.get_node_show_name(node)
            else:
                src_location[node[src_topo_id]] = self.get_node_show_name(node)

            self.logger.merlin_info("append node id = " + node[src_topo_id])
            self.src_topo_id_set.append(node[src_topo_id])

            # merge interface information
            if self.settings.attr_interfaces in node:
                for one_interface in node[self.settings.attr_interfaces]:
                    if src_topo_id in one_interface:
                        if attr_name in one_interface:
                            # get the map of interface topo_id and name
                            self.topo_id_name_map[one_interface[src_topo_id]] = one_interface[attr_name]
                        src_location[one_interface[src_topo_id]] \
                            = self.get_node_show_name(one_interface)
                        self.src_topo_id_set.append(one_interface[src_topo_id])
                        self.logger.merlin_info("append interface id = " \
                                                + one_interface[src_topo_id])

            # merge array information
            if self.settings.attr_arrays in node:
                for one_array in node[self.settings.attr_arrays]:
                    src_location[one_array[self.settings.attr_src_topo_id]] = self.get_node_show_name(one_array)
                    if src_topo_id in one_array:
                        self.src_topo_id_set.append(one_array[self.settings.attr_src_topo_id])
                        self.logger.merlin_info("append array id = " + one_array[self.settings.attr_src_topo_id])

            # recursive compute childs
            if self.settings.attr_childs in node:
                for child in node[self.settings.attr_childs]:
                    self.get_location_recursive(child, src_location)

            # recursive compute functions
            if self.settings.attr_sub_functions in node:
                for sub_function in node[self.settings.attr_sub_functions]:
                    self.get_location_recursive(sub_function, src_location)

            # recursive compute functions
            if self.settings.attr_stmt in node:
                for one_stmt in node[self.settings.attr_stmt]:
                    if src_topo_id in one_stmt:
                        if self.settings.attr_map_to_id in one_stmt:
                            src_id = one_stmt[src_topo_id]
                            map_to_id = one_stmt[self.settings.attr_map_to_id]
                            self.src_topo_id_map[src_id[2:]] = map_to_id
        else:
            pass

    def build_map_of_src_location(self):
        '''
        build the map for src id and it's location, including file name,
        line number and node name
        output:
        {src_id : name(filename:linenumber)}
        '''
        self.logger.merlin_info("Building map for src_location...")
        src_location = {}
        for kernel in self.hierarchy_data:
            self.get_location_recursive(kernel, src_location)
        self.src_location = src_location
        self.logger.merlin_info("Printing src location,,")
        self.logger.merlin_info(self.src_location)

    def get_src_location(self, src_id):
        '''
        get src_location from dict
        '''
        if src_id in self.src_location:
            return self.src_location[src_id]
        else:
            return ""

class MemoryBurst:
    """ Helper class for analyzing memory burst interface """
    #pylint: disable=too-many-arguments
    def __init__(self, logger, settings, topo_info, hier_topo_info, nest_info):
        self.logger = logger
        self.settings = settings
        self.topo_info = topo_info
        self.hier_topo_info = hier_topo_info
        self.nest_info = nest_info
        self.func_call_for_id = \
                topo_info.index_function_call(self.settings.attr_function_id)
        self.function_for_id = topo_info.index_function(settings.attr_topo_id)
        self.location_merlin_memcpy = {}
        self.location_merlin_memcpy_src = {}
        self.parent_merlin_memcpy = {}
        self.parent_merlin_memcpy_src = {}
        self.location_cross_list = {}
        self.inserted = []

    #pylint: disable=too-many-statements, too-many-locals, too-many-branches
    def get_interface_info(self, interface):
        """ Returns a list of interface information from the given
            interface node from the merlin input code
        """
        output_info = {}
        nodes = self.topo_info.get_dest_nodes(interface)
        self.logger.merlin_info("interface = ")
        self.logger.merlin_info(interface)
        #self.logger.merlin_info("nodes = ")
        #self.logger.merlin_info(nodes)
        has_read = False
        has_write = False
        # set all default to -
        attr_type = self.settings.attr_type
        attr_var_type = self.settings.attr_var_type
        attr_burst = self.settings.attr_burst
        attr_streaming = self.settings.attr_streaming
        attr_coalesce = self.settings.attr_coalesce
        attr_memory_coalescing = self.settings.attr_memory_coalescing
        attr_note = self.settings.attr_note
        attr_coalesce_bitwidth = self.settings.attr_coalesce_bitwidth
        attr_burst_read_write = self.settings.attr_burst_read_write
        attr_comment_interface = self.settings.attr_comment_interface
        attr_memory_coalescing_bitwidth \
                = self.settings.attr_memory_coalescing_bitwidth
        attr_critical_warning = self.settings.attr_critical_warning
        output_info[attr_burst] = '-'
        output_info[attr_coalesce] = '-'
        output_info[attr_note] = '-'
        output_info[attr_coalesce_bitwidth] = '-'
        output_info[attr_comment_interface] = '-'
        for node in nodes:
            if attr_type in node \
                and node[attr_type] == self.settings.value_interface:
                self.logger.merlin_info("analyze interface node")
                streaming = ""
                burst = ""
                coalescing = ""
                bitwidth = ""
                note = ""
                if attr_streaming in node:
                    streaming = node[attr_streaming]
                if attr_burst in node:
                    burst = node[attr_burst]
                if attr_memory_coalescing in node:
                    coalescing = node[attr_memory_coalescing]
                if attr_memory_coalescing_bitwidth in node:
                    bitwidth = node[attr_memory_coalescing_bitwidth]
                if attr_critical_warning in node:
                    note = node[attr_critical_warning]
                coalesce_bitwidth = "-"
                comment = ""
                if (attr_var_type in node and
                        node[attr_var_type] == self.settings.value_array):
                    # separately show burst and coalesce in the report
                    if streaming == "on":
                        if coalescing == "on":
                            comment = "streamed & coalesced"
                            coalesce_bitwidth = bitwidth
                        else:
                            comment = "streamed"
                    elif burst == "on":
                        if coalescing == "on":
                            comment = "cached & coalesced"
                            coalesce_bitwidth = bitwidth
                        else:
                            comment = "cached"
                    else:
                        if coalescing == "on":
                            comment = "coalesced"
                            burst = "-"
                            coalesce_bitwidth = bitwidth
                        else:
                            comment = "-"
                            burst = "-"
                    is_read = ""
                    if self.settings.attr_is_read in node:
                        is_read = node[self.settings.attr_is_read]
                    is_write = ""
                    if self.settings.attr_is_write in node:
                        is_write = node[self.settings.attr_is_write]
                    if is_read == "1":
                        has_read = True
                    if is_write == "1":
                        has_write = True
                elif (attr_var_type in node and
                      node[attr_var_type] == self.settings.value_scalar):
                    comment = "scalar"
                    burst = "-"
                    coalescing = "-"
                    coalesce_bitwidth = "-"
                output_info[attr_burst] = burst
                output_info[attr_coalesce] = coalescing
                output_info[attr_note] = note
                output_info[attr_coalesce_bitwidth] = coalesce_bitwidth
                if note != "":
                    comment += self.settings.merlin_note
                output_info[attr_comment_interface] = comment
            elif attr_type in node \
                and node[attr_type] == self.settings.value_call:
                is_read = ""
                if self.settings.attr_is_read in node:
                    is_read = node[self.settings.attr_is_read]
                is_write = ""
                if self.settings.attr_is_write in node:
                    is_write = node[self.settings.attr_is_write]
                if is_read == "1":
                    has_read = True
                if is_write == "1":
                    has_write = True
        read_write = ""
        if has_read and has_write:
            read_write = "R/W"
        elif has_read:
            read_write = "RO"
        elif has_write:
            read_write = "WO"
        else:
            read_write = "-"
        output_info[attr_burst_read_write] = read_write

        self.logger.merlin_info("output_info = ")
        self.logger.merlin_info(output_info)
        return output_info

    def find_merlin_memcpy_calls(self, interface):
        """ Returns a list of function call nodes from the given
            interface node from the merlin input code
        """
        attr_type = self.settings.attr_type
        attr_var_type = self.settings.attr_var_type
        if attr_var_type in interface \
            and interface[attr_var_type] == self.settings.value_array:
            return list(filter(lambda node: \
                               attr_type in node and node[attr_type] \
                               == self.settings.value_call,
                               self.topo_info.get_dest_nodes(interface)))
        else:
            return []

    def locate_merlin_memcpy_call_site(self, call):
        """ Infer the merlin_memcpy call site in the merlin input code by
            returning the topo IDs of its predecessor and successor, any one
            of which can be None. The return value (None, None) indicates
            should be located by it's parent.
        """
        key = lambda x: [int(s) for s in x[2:].split("_")]

        def find_src_id(dest_ids, reverse, cross_list):
            res = None
            if reverse:
                dest_ids.reverse()

            for one_id in dest_ids:
                try:
                    sid = self.topo_info.get_src_id(one_id)
                    node = self.hier_topo_info.get_node(sid)
                    if (sid[:2] == "L_" or
                        node.get(self.settings.attr_type) \
                        == self.settings.value_call):
                        if (sid[:2] == "L_" and
                            self.topo_info.get_value_from_key(
                                self.nest_info.get_scope_id(one_id),
                                self.settings.attr_org_identifier
                            ) ==
                            self.topo_info.get_value_from_key(
                                one_id, self.settings.attr_org_identifier
                            )):
                            sid = find_src_id(
                                sorted(self.nest_info.find_child_ids(one_id),
                                       key=key),
                                reverse, cross_list
                            )
                            # record cross list for cycles computation
                            cross_list.append(one_id)
                            if sid is not None:
                                res = sid
                                break
                        else:
                            #if is_merlin_flatten and \
                            #    "parent_loop" in node:
                            #    res = node["parent_loop"]
                            #else:
                            #    res = sid
                            res = sid
                            break
                except KeyError as e:
                    self.logger.merlin_critical_warning(e)

            return res

        def locate(node_id):
            scope_id = self.nest_info.get_scope_id(node_id)

            dest_ids = self.nest_info.find_child_ids(scope_id)
            dest_ids.sort(key=key)

            ci = dest_ids.index(node_id)

            # a lost to record node which acrossed when find location
            cross_list = []
            if ci == 0 and ci == len(dest_ids)-1:
                if scope_id[:2] == "F_":
                    return (None, None, cross_list)
                else:
                    return locate(scope_id)
            elif ci == 0:
                sid = find_src_id(dest_ids[ci+1:], False, cross_list)
                if sid is None:
                    return (None, sid, \
                        cross_list)

                test_scope_id = sid
                test_dest_ids = self.topo_info.get_dst_id(test_scope_id)
                is_merlin_flatten = ""
                for one_id in test_dest_ids:
                    vendor_node = self.topo_info.vendor_data[one_id]
                    if "flatten_loop" in vendor_node and \
                        vendor_node["flatten_loop"] == "on":
                        is_merlin_flatten = "yes"
                        break
                node = self.hier_topo_info.get_node(sid)
                if is_merlin_flatten and \
                    "parent_loop" in node:
                    sid = node["parent_loop"]
                return (None, sid, \
                        cross_list)
            elif ci == len(dest_ids)-1:
                return (find_src_id(dest_ids[:ci], True, cross_list), \
                        None, cross_list)
            else:
                return (find_src_id(dest_ids[:ci], True, cross_list),
                        find_src_id(dest_ids[ci+1:], False, cross_list), \
                        cross_list)

        try:
            tid = call[self.settings.attr_topo_id]
            sid = self.nest_info.get_top_scope_id(tid)
            # we need to find a node from src, not generated by merlin
            while (self.function_for_id[sid][
                    self.settings.attr_org_identifier][:2] == "X_" or
                   self.function_for_id[sid][
                    self.settings.attr_org_identifier][:2] == "L_"):
                calls = self.func_call_for_id.get(sid, [])
                if len(calls) >= 1:
                    tid = calls[0][self.settings.attr_topo_id]
                    sid = self.nest_info.get_top_scope_id(tid)
                else:
                    return

            (pre_value, post_value, cross_list) = locate(tid)
            self.location_merlin_memcpy[call[self.settings.attr_topo_id]] \
                    = (pre_value, post_value)
            if pre_value is None and post_value is None:
                sid = self.nest_info.get_scope_id(tid)
                parent_id = self.topo_info.get_src_id(sid)
                self.parent_merlin_memcpy[call[self.settings.attr_topo_id]] \
                        = parent_id
                if self.settings.attr_name in call:
                    self.logger.merlin_info("parent id of memory burst "
                            + str(call[self.settings.attr_name]) + " is:"
                            + str(parent_id))
            self.location_cross_list[tid] = cross_list
            self.logger.merlin_info("cross list = " + str(cross_list))
        except KeyError as e:
            self.logger.merlin_critical_warning(e)

    def interchange_memcpy(self):
        '''
        interchange for location_merlin_memcpy
        to tell the src_id, if before or after it, we have extra nodes
        input:
        {dst_id: (before src id, after src id)}
        output:
        {src_id: {before:{[ids]}, after:{[ids]}}}
        '''
        str_after = self.settings.value_after
        str_before = self.settings.value_before
        self.logger.merlin_info("Print location_merlin_memcpy...")
        self.logger.merlin_info(self.location_merlin_memcpy)
        self.logger.merlin_info("Print parent_merlin_memcpy...")
        self.logger.merlin_info(self.parent_merlin_memcpy)
        location_merlin_memcpy_src = {}
        location_merlin_memcpy = self.location_merlin_memcpy
        for one_dst_id in location_merlin_memcpy:
            (before, after) = location_merlin_memcpy[one_dst_id]
            if before is not None:
                if before in location_merlin_memcpy_src:
                    if str_after in location_merlin_memcpy_src[before]:
                        location_merlin_memcpy_src[before][str_after].append(one_dst_id)
                    else:
                        location_merlin_memcpy_src[before][str_after] = [one_dst_id]
                else:
                    location_merlin_memcpy_src[before] = {}
                    location_merlin_memcpy_src[before][str_after] = [one_dst_id]
            if after is not None:
                if after in location_merlin_memcpy_src:
                    if str_before in location_merlin_memcpy_src[after]:
                        location_merlin_memcpy_src[after][str_before].append(one_dst_id)
                    else:
                        location_merlin_memcpy_src[after][str_before] = [one_dst_id]
                else:
                    location_merlin_memcpy_src[after] = {}
                    location_merlin_memcpy_src[after][str_before] = [one_dst_id]
            if before is None and after is None:
                if one_dst_id in self.parent_merlin_memcpy:
                    parent_id = self.parent_merlin_memcpy[one_dst_id]
                    if parent_id in self.parent_merlin_memcpy_src:
                        self.parent_merlin_memcpy_src[parent_id].append(
                                one_dst_id)
                    else:
                        self.parent_merlin_memcpy_src[parent_id] = [one_dst_id]
        self.location_merlin_memcpy_src = location_merlin_memcpy_src
        self.logger.merlin_info("Print location_merlin_memcpy_src...")
        self.logger.merlin_info(self.location_merlin_memcpy_src)
        self.logger.merlin_info("Print parent_merlin_memcpy_src...")
        self.logger.merlin_info(self.parent_merlin_memcpy_src)

    def get_childs_memory_burst(self, parent_id):
        '''
        Get all child memory burst node,
        and all the memory burst nodes have no other siblings.
        (otherwise they will have already been handled).
        '''
        self.logger.merlin_info("get_childs_memory_burst with parent id:" + str(parent_id))
        if parent_id in self.parent_merlin_memcpy_src:
            return self.parent_merlin_memcpy_src[parent_id]
        else:
            return []

    def get_before_and_after_list(self, child_id):
        '''
        get before and after dst_id list from src_id
        return (before_list, after_list)
        '''
        str_after = self.settings.value_after
        str_before = self.settings.value_before
        node_before_list = []
        node_after_list = []
        if child_id in self.location_merlin_memcpy_src:
            self.logger.merlin_info("find insert positoin")
            for one_dir in self.location_merlin_memcpy_src[child_id]:
                dir_id = self.location_merlin_memcpy_src[child_id][one_dir]
                if one_dir == str_after:
                    for one_id in dir_id:
                        if one_id not in self.inserted:
                            # create a dict to insert here
                            node_after_list.append(one_id)
                        else:
                            self.inserted.append(one_id)
                        self.logger.merlin_info("find after node = " + one_id)
                elif one_dir == str_before:
                    for one_id in dir_id:
                        if one_id not in self.inserted:
                            node_before_list.append(one_id)
                        else:
                            self.inserted.append(one_id)
                        self.logger.merlin_info("find before node = " + one_id)
        return (node_before_list, node_after_list)
