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

"""
Module for generating the final JSON file by merging all separate
JSON files in the stage of data extraction.

"""

import glob
import os.path
import utility
import merlin_log

#pylint: disable=invalid-name
class Merge:
    """ the Merge class """
    def __init__(self, spec):
        self._id = 0
        self.spec = spec
        self.logging = merlin_log.MerlinLog("DEBUG")
        self.sources_ignored = [
            os.path.basename(x) for x in glob.iglob("../../*.h")
        ]

    def id(self):
        """ make a unique number for internal use """
        self._id += 1
        return self._id

    def _transform(self, obj):
        res = {}
        d = {}
        key = ""
        for k, v in obj.items():
            if k == "topo_id":
                if v == "is_skipped_node":
                    key = "%s,%d" % (v, self.id())
                else:
                    key = v
            elif isinstance(v, list):
                for x in v:
                    d2 = self._transform(x)
                    while d2 != {}:
                        k2, v2 = d2.popitem()
                        if k2 in res:
                            res[k2].update(v2)
                        else:
                            res[k2] = v2
            else:
                d[k] = v

        if key in res:
            res[key].update(d)
        else:
            res[key] = d
        return res

    #pylint: disable=unused-variable
    @staticmethod
    def _index_array(res):
        index = {}
        for k, v in res.items():
            try:
                if v["type"] == "array":
                    index[v["name"]] = {
                        "topo_id": k,
                    }
            except KeyError as e:
                pass

        return index

    #pylint: disable=unused-variable
    @staticmethod
    def _index_loop(res):
        index = {}
        for k, v in res.items():
            #try:
            if k[:2] == "L_":
                index[k] = {
                    "topo_id": k,
                    "source": v.get("source", None),
                    "line": v.get("line", None),
                    "col": v.get("col", None),
                    "begin": v.get("begin", None),
                    "end": v.get("end", None),
                    "loop_label": v.get("loop_label", None),
                    "loop_body_line": v.get("loop_body_line", None),
                    "loop_body_col": v.get("loop_body_col", None),
                }
            #except KeyError as e:
            #    pass

        return index

    @staticmethod
    def _index_function(res):
        index = {}
        for k, v in res.items():
            try:
                if v["type"] == "kernel" or v["type"] == "function":
                    index[v["name"]] = {
                        "topo_id": k,
                    }
            except KeyError as e:
                pass

        return index

    def _read_loop_location(self):
        loops = utility.load_json(self.spec.json_loop_location)

        res = {}
        for d in loops:
            res.update(d)
        return res

    #pylint: disable=too-many-locals, unused-argument,too-many-statements, too-many-branches, too-many-arguments
    def _merge_csynth_rpt_xml(self, res, index_array, index_function, index_loop, \
                              merged_node_list, loop_name_list):
        loop_location = self._read_loop_location()

        def find_loop_location(loop_key):
            loop = loop_key.split(",")[1]
            loop = loop.replace(".", " ")
            if loop in loop_location:
                filename = os.path.basename(loop_location[loop]["filename"])
                line_num = loop_location[loop]["linenum"]
                return (filename, line_num)
            return (None, None)

        def find_loop_tid_by_line_num(filename, line_num):
            for k, v in index_loop.items():
                if filename == v["source"]:
                    if (line_num == v["line"] or
                        line_num == v["loop_body_line"]):
                        return k
            return None

        def find_loop_tid_by_label(label):
            label = label.split("_")[0]
            for k, v in index_loop.items():
                if label == v["loop_label"]:
                    return k
            return None

        def fuzzy_match_function(key):
            guess = key.rpartition("_")[0]
            post_name = key.rpartition("_")[2]
            tmp = str(post_name)
            if not tmp.isdigit():
                return ""
            while guess and guess not in index_function:
                guess = guess.rpartition("_")[0]

            if guess:
                return guess

            guess = key[:-1]
            while guess and guess not in index_function:
                guess = guess[:-1]

            return guess

        #pylint: disable=C0111
        class SkipException(Exception):
            pass

        def find_loop_tid(loop_key):
            funcname, loopname = loop_key[2:].split("-", 1)
            filename, line_num = find_loop_location(loop_key)
            if not loopname.lower().startswith("loop "):
                res = find_loop_tid_by_label(loopname)
                if res is not None:
                    return res
            if filename is None or line_num is None:
                return None
            elif filename in self.sources_ignored:
                raise SkipException
            else:
                self.logging.merlin_warning("line-number merging %s" % loop_key)
                res = find_loop_tid_by_line_num(filename, line_num)
                return None if res is None else res

        def judge_merge(key, guess, h):
            sub_module = h.get(guess)
            if sub_module:
                for module_name in sub_module:
                    if key == module_name:
                        return False
                return True
            return True
        #pylint: disable=too-many-nested-blocks
        def data_merge(tid, v, res):
            merge_resource_list = ["total-BRAM", "total-DSP", \
                                   "total-FF", "total-LUT", "total-URAM",\
                                   "util-BRAM", "util-DSP",\
                                   "util-FF", "util-LUT", "util-URAM"]
            merge_latency_list = ["interval-max", "interval-min",\
                                  "latency-max", "latency-min"]
            for merge_k, merge_data in v.items():
                if merge_k in res[tid]:
                    if merge_k in merge_resource_list:
                        value = res[tid][merge_k]
                        value_t = value
                        t = merge_data
                        if str(merge_data).startswith("~"):
                            t = merge_data.strip("~")
                        if str(value).startswith("~"):
                            value_t = value.strip("~")
                        if str(value_t).isdigit() and str(t).isdigit():
                            merged_value = int(value_t) + int(t)
                        else:
                            merged_value = value_t
                        res[tid].update({merge_k:int(merged_value)})
                    else:
                        if merge_k in merge_latency_list:
                            if str(merge_data).isdigit() and str(res[tid][merge_k]).isdigit():
                                if int(merge_data) >= int(res[tid][merge_k]):
                                    res[tid].update({merge_k:merge_data})
                        else:
                            res[tid].update({merge_k:merge_data})
                else:
                    res[tid].update({merge_k:merge_data})
                ##return res


        module_hierachy = {}
        module_list = []
        #pylint: disable=too-many-nested-blocks
        for path in sorted(glob.glob("*_csynth.json")):
            self.logging.merlin_info("merging vendor json file %s" % path)
            d = utility.load_json(path)
            module_name = path.rsplit(".", 1)[0]
            module_name = module_name.rsplit("_", 1)[0]
            module_list = []
            for k, v in d.items():
                #if v.has_key('instance_name') and v.has_key('module_name'):
                if 'instance_name' in v and 'module_name' in v:
                    sub_module_name = v.get('module_name')
                    sub_instance_name = v.get('instance_name')
                    module_list.append(sub_module_name)
            module_hierachy[module_name] = module_list
            for k, v in d.items():
                self.logging.merlin_info("--k %s" % k)
                if k[:2] == "L,":
                    try:
                        tid = find_loop_tid(k)
                        self.logging.merlin_info("--tid = %s" % tid)
                    except SkipException:
                        pass
                    else:
                        if tid is None:
                            self.logging.merlin_warning("--Not merging %s" % k)
                        else:
                            funcname, loopname = k[2:].split("-", 1)
                            loop_name_list.append(loopname)
                            self.logging.merlin_info("--loopname = %s" % loopname)
                            skip_key = ["II", "pipelined", "unrolled", \
                                       "trip-count", \
                                       "iteration-latency", \
                                       "iteration-latency-min", \
                                       "latency-max", \
                                       "latency-min"]
                            #pylint: disable=line-too-long
                            if funcname.lower().startswith("memcpy") \
                                or funcname.lower().startswith("memcpy_wide_bus_") \
                                or funcname.lower().startswith("merlin_memcpy"):
                                for i in skip_key:
                                    if v.get(i):
                                        del v[i]
                            res[tid].update(v)
                            merged_node_list.append(tid)
                else:
                    if k in index_function:
                        tid = index_function[k]["topo_id"]
                        if tid in res:
                            data_merge(tid, v, res)
                            merged_node_list.append(tid)
                    else:
                        guess = fuzzy_match_function(k)
                        if guess:
                            tid = index_function[guess]["topo_id"]
                            if judge_merge(k, guess, module_hierachy):
                                if tid in res:
                                    merged_node_list.append(tid)
                                    data_merge(tid, v, res)
                                    merged_node_list.append(tid)
                                    self.logging.merlin_warning(
                                        "Unable to find %s, so merging into %s" % (k, guess)
                            )
                        else:
                            self.logging.merlin_warning("Not merging %s" % k)

        return res

    #pylint: disable=too-many-arguments, too-many-nested-blocks
    def _merge_vivado_hls_log(self, res, index_array, index_function, index_loop, \
                              merged_node_list, loop_name_list):
        d = utility.load_json(self.spec.json_vivado_hls, "nosort")

        #pylint: disable=singleton-comparison
        def find_loop_tid_by_line_num(filename, line_num):
            for k, v in index_loop.items():
                if filename == v["source"]:
                    if (line_num == v["line"] or
                        line_num == v["loop_body_line"]):
                        return k
            return None

        def find_loop_tid_by_label(label):
            label = label.split("_")[0]
            for k, v in index_loop.items():
                if label == v["loop_label"]:
                    return k
            return None

        #pylint: disable=C0111
        class SkipException(Exception):
            pass

        def find_loop_tid(loop_key, loop):
            if loop.get("name") is not None:
                if not loop["name"].lower().startswith("loop-"):
                    res = find_loop_tid_by_label(loop["name"])
                    if res is not None:
                        return res

            if loop.get("filename") is None or loop.get("line") is None:
                return None
            elif loop.get("filename") in self.sources_ignored:
                raise SkipException
            else:
                self.logging.merlin_warning("line-number merging %s" % loop_key)
                res = find_loop_tid_by_line_num(loop["filename"], loop["line"])
                return None if res is None else res

        # find flatten son by name
        def find_flatten_son(name, loop_name_list):
            flatten_son = ""
            for one_name in loop_name_list:
                self.logging.merlin_info("name  = %s" % name)
                self.logging.merlin_info("one name = %s" % one_name)
                split_name = one_name.rpartition("_")
                self.logging.merlin_info("split name =")
                self.logging.merlin_info(split_name)
                if name in split_name:
                    name_index = split_name.index(name)
                    self.logging.merlin_info("name index  = %d" % name_index)
                    if len(split_name) > name_index+1:
                        flatten_son = split_name[name_index+2]
                        break
            self.logging.merlin_info("flatten_son  = %s" % flatten_son)
            return flatten_son

        for k, v in d.items():
            try:
                if k.split(",")[0] == "A":
                    tid = index_array[v["name"]]["topo_id"]
                    res[tid].update(v)

                elif k.split(",")[0] == "F":
                    tid = index_function[v["name"]]["topo_id"]
                    res[tid].update(v)
                    merged_node_list.append(tid)

                elif k.split(",")[0] == "L":
                    try:
                        tid = find_loop_tid(k, v)
                        if self.spec.attr_flatten in v and v[self.spec.attr_flatten] == "yes":
                            self.logging.merlin_info("find flatten loop")
                            flatten_son = find_flatten_son(v["name"], loop_name_list)
                            flatten_id = find_loop_tid_by_label(flatten_son)
                            if flatten_id is not None:
                                self.logging.merlin_info("flatten_id = %s" % flatten_id)
                                node = {}
                                node[self.spec.attr_flatten_son] = self.spec.attr_flatten_son
                                res[flatten_id].update(node)
                                merged_node_list.append(flatten_id)
                    except SkipException:
                        pass
                    else:
                        if tid is None:
                            self.logging.merlin_warning("Not merging %s" % k)
                        else:
                            res[tid].update(v)
                            merged_node_list.append(tid)
            except KeyError as e:
                self.logging.merlin_warning("Not merging %s" % k)
                self.logging.merlin_warning(" KeyError: %s" % e)

        return res

    def _merge_vendor(self, res):
        index_array = Merge._index_array(res)
        index_function = Merge._index_function(res)
        index_loop = Merge._index_loop(res)
        merged_node_list = []
        opt_remove_node_list = []
        loop_name_list = []
        res = self._merge_csynth_rpt_xml(res, index_array, index_function, index_loop, \
                                         merged_node_list, loop_name_list)
        res = self._merge_vivado_hls_log(res, index_array, index_function, index_loop, \
                                         merged_node_list, loop_name_list)
        self.logging.merlin_info("merged_node_list: ")
        self.logging.merlin_info(merged_node_list)
        all_func_list = []
        for k in index_function:
            all_func_list.append(index_function[k][self.spec.attr_topo_id])
        all_loop_list = []
        for k in index_loop:
            all_loop_list.append(k)
        for key_id in all_loop_list:
            if key_id not in merged_node_list:
                opt_remove_node_list.append(key_id)
        for key_id in all_func_list:
            if key_id not in merged_node_list:
                opt_remove_node_list.append(key_id)


        dead_code = {self.spec.attr_dead_code:self.spec.value_dead_code}
        for loop_id in opt_remove_node_list:
            res[loop_id].update(dead_code)
        return res

    #pylint: disable=no-self-use
    def _remove_skipped(self, res):
        ls = filter(lambda x: x.startswith("is_skipped_node,"), list(res.keys()))
        for key in ls:
            del res[key]
        return res

    def _merge_topo(self):
        kernels = utility.load_json(self.spec.json_topo_info)

        res = {}
        for x in kernels:
            res.update(self._transform(x))
        return res

    #pylint: disable=no-self-use, redefined-builtin
    def filter_json(self, input):
        '''
        filter json file, change all int type value to string type
        '''
        output = {}
        for one_node in input:
            one_dict = input[one_node]
            output_one_node = {}
            for one_key in one_dict:
                value = one_dict[one_key]
                output_one_node[one_key] = str(value)
            output[one_node] = output_one_node
        return output

    def merge(self):
        """ the entry to the merge process """
        res = self._merge_topo()
        res = self._merge_vendor(res)
        res = self._remove_skipped(res)
        res = self.filter_json(res)
        utility.dump_to_json(self.spec.json_gen_token, res)

#if __name__ == "__main__":
#    settings = merlin_report_setting.MerlinReportSetting()
#    res = Merge(settings).merge()
#    pass
