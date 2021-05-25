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
Python Script in Merlin Compiler

Filename    : optimizers/autodse/scripts/dse_cache_res_xilinx.py
Description : This is the script for maintaining a cache to
              estimate the resource utilization of Xilinx HLS.
Usage       : Internal call
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-11-2018 Created (Cody Hao Yu)
"""
import os
import json
import math
import threading
from queue import Queue
import traceback
from copy import deepcopy

from dse_cache import CacheDataBase, DSECacheBase
from dse_run_hls import HLSRetCode
from dse_util import gen_merlin_pass_cmd, OCL_DIR
from util import run_command, try_to_int
import merlin_logger

MLOGGER = merlin_logger.get_logger('DSE_Cache_Res_Xilinx')
MODEL_RES_LIST = ['DSP', 'LUT']

class ResUtilEst(object):
    """
    The result data of the analytical model
    """
    def __init__(self):
        """
        BRAM, DSP, LUT, FF
        """
        self.usages = [-1] * 4

class QorGraphNode(object):
    """
    A call graph for storing QoR information
    TODO: Extend it to support interface and array
    """
    def __init__(self, name, skeleton, _type, parent, para_factor):
        self.name = name
        self.child_nodes = set()
        self.unpreds = set()
        # for function:
        #   overall = iteration = singleton + fix_struct + sum(child_nodes)
        # for loop:
        #   overall = para * iteration
        #   iteration = singleton + sum(child_nodes)
        self.singleton = {'DSP': -1, 'LUT': -1}
        self.overall = {'DSP': -1, 'LUT': -1}
        self.iteration = {'DSP': -1, 'LUT': -1}
        self.fix_struct = {'DSP': 0, 'LUT': 0}
        self.parent = parent
        self.skeleton = skeleton
        self.type = _type
        self.inline = False
        self.para = para_factor
        if self.type == 'F' and self.para != 1:
            MLOGGER.info("WARN: Fuction %s has para_factor %d",
                         self.name, self.para)
            self.para = 1

class CacheData(CacheDataBase):
    """
    Cache data for skeleton and QoR information
    """
    known_func_prefixes = {
        "merlin_memcpy",
        "memcpy_wide_bus",
    }
    def __init__(self, cfg_id, filename, bram_usage, lut_memctrl):
        with open(filename, 'r') as skeleton_file:
            jdata = json.load(skeleton_file)
            self.skeleton = jdata[u'skeleton']
        self.lock = threading.Lock()
        self.est_bram = bram_usage
        self.fix_res = dict()
        self.remove_known = True
        self.polish(lut_memctrl)
        self.top_func = ""
        self.qor_graph = dict()
        self.history = {}
        cache_key = self.gen_cache_key(self.skeleton)
        super(CacheData, self).__init__(cfg_id, cache_key)

    @staticmethod
    def gen_cache_key(skeleton):
        """
        Generate the cache key for a skeleton
        Key spec:
            * Take only the keys from the skeleton, regardless values
              which are unroll factors.
            * The structure is dict(key: set()).
            * Since dict and set are implemented using hash table,
              traversing this structure to generate a string would have
              the same order.
        """
        if not skeleton:
            return None

        key = dict()
        for item, subitems in skeleton.items():
            if not subitems:
                key[item] = set()
            else:
                key[item] = set(subitems.keys())
        return str(key)

    def dump(self):
        """Return a dict of this cache data"""
        jdata = {}
        jdata[u'skeleton'] = self.skeleton
        jdata[u'change_history'] = {}
        for key, value in self.history.items():
            jdata[u'change_history'][value] = key
        return jdata

    @staticmethod
    def dissolve_known_func(func_name):
        """
        Dissolve a known func by removing it from skeleton and
        calculate resource usage
        """
        resources = {'LUT': 0, 'DSP': 0}
        # FIXME: currently using a fixed estimation
        if func_name.startswith("merlin_memcpy"):
            resources['LUT'] = 500
        elif func_name.startswith("memcpy_wide_bus"):
            resources['LUT'] = 1000
        else:
            MLOGGER.info("WARN: Found non-dissolvable function: %s", func_name)
        return resources

    def polish(self, lut_memctrl):
        """
        Remove empty and system func calls, also
        if specified, remove known functions
        """
        skeleton = self.skeleton
        empty_nodes = set()
        for item, subitems in skeleton.items():
            for subitem in subitems:
                tokens = subitem.strip().split('@')
                if tokens[-2] == 'F':
                    item_name = tokens[0]
                else:
                    item_name = subitem
                if item_name not in skeleton:
                    empty_nodes.add(subitem)
        for item in skeleton:
            for empty_node in empty_nodes:
                if empty_node in skeleton[item]:
                    skeleton[item].pop(empty_node)

        if not self.remove_known:
            return

        known_nodes = set()
        nodes_queue = Queue()
        # 1st round: enqueue all funcs
        for item in skeleton:
            if '@' not in item: # function
                for prefix in CacheData.known_func_prefixes:
                    if item.startswith(prefix):
                        known_nodes.add(item)
                        nodes_queue.put(item)
        # 2nd round: enqueue all child nodes
        while not nodes_queue.empty():
            node_name = nodes_queue.get()
            if node_name in skeleton:
                for subitem in skeleton[node_name]:
                    tokens = subitem.strip().split('@')
                    if tokens[-2] == 'F':
                        item_name = tokens[0]
                    else:
                        item_name = subitem
                    if item_name not in known_nodes:
                        known_nodes.add(item_name)
                        nodes_queue.put(item_name)
        MLOGGER.debug("Known nodes: %s", known_nodes)

        new_skeleton = dict()
        for item in skeleton:
            if item in known_nodes:
                continue
            new_skeleton[item] = dict()
            for subitem in skeleton[item]:
                tokens = subitem.strip().split('@')
                if tokens[-2] == 'F':
                    func_name = tokens[0]
                    if func_name in known_nodes:
                        # Remove known functions and count their resources
                        resources = self.dissolve_known_func(func_name)
                        if item in self.fix_res:
                            for res in MODEL_RES_LIST:
                                self.fix_res[item][res] += resources[res]
                        else:
                            self.fix_res[item] = resources
                    else:
                        new_skeleton[item][subitem] = skeleton[item][subitem]
                elif subitem in known_nodes:
                    MLOGGER.info("WARN: Found known loop: %s", subitem)
                else:
                    new_skeleton[item][subitem] = skeleton[item][subitem]
        self.skeleton = new_skeleton
        for item in lut_memctrl:
            if item in self.fix_res:
                self.fix_res[item]['LUT'] += lut_memctrl[item]
            else:
                self.fix_res[item] = {'DSP': 0, 'LUT': lut_memctrl[item]}
        return

    def analyze_unpredictables(self, other, top_name, res):
        """Analyze the number of unpredictable nodes"""
        # The unpreds are all from loops of different trip counts
        unpreds_dict = dict()
        unpreds_count = 0
        self_graph = self.qor_graph[top_name]
        other_graph = other.qor_graph[top_name]

        for element in self_graph.child_nodes:
            node_name = element.name
            # if the same function is invoked multiple times, then
            # only one unpred is counted
            if node_name in unpreds_dict:
                continue
            graph_dict, count = self.analyze_unpredictables(
                other, node_name, res)
            if count != graph_dict[node_name]:
                MLOGGER.info("WARN: #Node %s: unpreds mismatched!", node_name)
            unpreds_dict.update(graph_dict)
            if element.overall[res] < 0: # unknown elements
                unpreds_count += count
        # new contribute to unpreds
        if self_graph.type == 'L' and self_graph.para != other_graph.para:
            unpreds_count += 1
        # record the graph node itself
        unpreds_dict[top_name] = unpreds_count
        return unpreds_dict, unpreds_count

    def update_graph(self, other, top_name, unpreds_dict, delta, res):
        """Updating the QoR of the graph via new input case"""
        self_graph = self.qor_graph[top_name] # type: QorGraphNode
        other_graph = other.qor_graph[top_name] # type: QorGraphNode

        unpreds_count = unpreds_dict[top_name]
        if unpreds_count >= 2 or unpreds_count <= 0:
            MLOGGER.debug("  %s: either not updatable or no need to update",
                          top_name)
            for self_ele in self_graph.child_nodes:
                other_ele = other.qor_graph[self_ele.name]
                if self_ele.overall[res] >= 0 and \
                        other_ele.overall[res] >= 0:
                    sub_delta = other_ele.overall[res] - \
                                self_ele.overall[res]
                    self.update_graph(
                        other, self_ele.name, unpreds_dict, sub_delta, res)
            return

        if self_graph.type == 'L' and self_graph.para != other_graph.para:
            # loop:
            #   overall = iteration * para
            #   iteration = singleton + sum(child_nodes)
            diff_para = other_graph.para - self_graph.para
            self_graph.iteration[res] = delta / diff_para
            self_graph.overall[res] = self_graph.iteration[res] * \
                                       self_graph.para
            MLOGGER.debug("   %s (delta = %d): the very loop to update",
                          top_name, delta)
            sum_child_nodes = 0
            for self_ele in self_graph.child_nodes:
                other_ele = other.qor_graph[self_ele.name]
                if self_ele.overall[res] >= 0 and \
                        other_ele.overall[res] >= 0:
                    sub_delta = other_ele.overall[res] - \
                                self_ele.overall[res]
                    self.update_graph(
                        other, self_ele.name, unpreds_dict, sub_delta, res)
                    if sum_child_nodes >= 0:
                        sum_child_nodes += self_ele.overall[res]
                else:
                    sum_child_nodes = -1
            if sum_child_nodes >= 0:
                self_graph.singleton[res] = self_graph.iteration[res] - \
                                           sum_child_nodes
            return

        if self_graph.type == 'L':
            MLOGGER.debug("   %s (delta = %d): loop unroll factor "
                          "remains unchanged", top_name, delta)
            diff_iter = delta / self_graph.para
            diff_known = 0
            unpred = None
            for self_ele in self_graph.child_nodes:
                other_ele = other.qor_graph[self_ele.name]
                if self_ele.overall[res] >= 0 and \
                        other_ele.overall[res] >= 0:
                    sub_delta = other_ele.overall[res] - \
                                self_ele.overall[res]
                    diff_known += sub_delta
                    self.update_graph(
                        other, self_ele.name, unpreds_dict, sub_delta, res)
                elif unpreds_dict[self_ele.name] <= 0:
                    pass
                else:
                    if unpred is not None:
                        MLOGGER.info("WARN: More than one "
                                     "unpreds found: %s and %s!",
                                     unpred.name, self_ele.name)
                    unpred = self_ele
            self.update_graph(
                other, unpred.name, unpreds_dict, diff_iter - diff_known, res)

            if unpred.overall[res] >= 0 and self_graph.iteration[res] >= 0:
                sub_dsp = 0
                for self_ele in self_graph.child_nodes:
                    if self_ele.overall[res] >= 0:
                        sub_dsp += self_ele.overall[res]
                    else:
                        sub_dsp = -1
                        break
                if sub_dsp >= 0:
                    self_graph.singleton[res] = self_graph.iteration[res] - \
                                               sub_dsp
            return

        # for function:
        #   overall = iteration = singleton + fix_struct + sum(child_nodes)
        diff_known = other_graph.fix_struct[res] - self_graph.fix_struct[res]
        MLOGGER.debug("   %s (delta = %s): function of known delta %s",
                      top_name, delta, diff_known)
        unpred = None
        for self_ele in self_graph.child_nodes:
            other_ele = other.qor_graph[self_ele.name]
            if self_ele.overall[res] >= 0 and \
                    other_ele.overall[res] >= 0:
                sub_delta = other_ele.overall[res] - \
                            self_ele.overall[res]
                diff_known += sub_delta
                self.update_graph(
                    other, self_ele.name, unpreds_dict, sub_delta, res)
            elif unpreds_dict[self_ele.name] <= 0:
                pass
            else:
                if unpred is not None:
                    MLOGGER.info("WARN: More than one "
                                 "unpreds found: %s and %s!",
                                 unpred.name, self_ele.name)
                unpred = self_ele
        self.update_graph(
            other, unpred.name, unpreds_dict, delta - diff_known, res)

        if unpred.overall[res] >= 0:
            sub_dsp = 0
            for self_ele in self_graph.child_nodes:
                if self_ele.overall[res] >= 0:
                    sub_dsp += self_ele.overall[res]
                else:
                    sub_dsp = -1
                    break
            if sub_dsp >= 0:
                if self_graph.inline:
                    self_graph.overall[res] = (sub_dsp +
                                               self_graph.fix_struct[res])
                    self_graph.iteration[res] = self_graph.overall[res]
                else:
                    self_graph.singleton[res] = self_graph.overall[res] - \
                                               self_graph.fix_struct[res] - \
                                               sub_dsp
        return

    def update_qor_graph(self, other):
        """Updating the entire usage graph via new input case"""
        self.lock.acquire()
        try:
            for res in MODEL_RES_LIST:
                self.update_resource_in_graph(other, res)
        except Exception:
            MLOGGER.info('ERROR: Error when updating QoR graph')
            MLOGGER.info(traceback.format_exc())
        self.lock.release()

    def update_resource_in_graph(self, other, res):
        """Updating the graph for a specific resource type"""
        unpreds_dict, _ = self.analyze_unpredictables(other, self.top_func, res)
        delta = other.qor_graph[self.top_func].overall[res] - \
                self.qor_graph[self.top_func].overall[res]
        MLOGGER.debug("Update resource type  %s:", res)
        MLOGGER.debug("  Reference skeleton before update:")
        self.print_qor_graph(self.top_func, 4, set())
        MLOGGER.debug("  New skeleton:")
        other.print_qor_graph(self.top_func, 4, set())
        self.update_graph(other, self.top_func, unpreds_dict, delta, res)
        MLOGGER.debug("  Reference skeleton after update:")
        self.print_qor_graph(self.top_func, 4, set())
        MLOGGER.debug("Done.")

    @staticmethod
    def dump_unpreds(unpreds_dict):
        """Print out all unpredictions"""
        MLOGGER.debug("Begin dumping unpreds:")
        for item in unpreds_dict:
            MLOGGER.debug("   %s: %s", item, unpreds_dict[item])
        MLOGGER.debug("Complete dumping.")

    def adjust_usage(self, node_name, dsp_budget):
        """Identify and zeroify node that uses no DSP at all"""
        node = self.qor_graph[node_name]
        dsp_unknown = dsp_budget
        if node.type == 'L':
            dsp_unknown = int(math.ceil(float(dsp_unknown)/node.para))
        for child_node in node.child_nodes: # type:QorGraphNode
            if child_node.overall['DSP'] >= 0:
                dsp_unknown -= child_node.overall['DSP']
                self.adjust_usage(child_node.name, child_node.overall['DSP'])
        if node.singleton['DSP'] >= 0:
            dsp_unknown -= node.singleton['DSP']
        if dsp_unknown < 0:
            MLOGGER.info("WARN: DSP usage of %s becomes negative (%d)!",
                         node_name, dsp_unknown)
            dsp_unknown = 0

        if dsp_unknown == 0:
            used_up = True
        else:
            used_up = False
            for child_node in node.child_nodes:
                if child_node.overall['DSP'] < 0:
                    if self.adjust_usage(child_node.name, dsp_unknown):
                        used_up = True
                    else:
                        pass
        if used_up:
            if node.singleton['DSP'] < 0:
                node.singleton['DSP'] = 0
            dsp_iter = node.singleton['DSP']
            for child_node in node.child_nodes:
                if child_node.overall['DSP'] < 0:
                    child_node.overall['DSP'] = 0
                    child_node.iteration['DSP'] = 0
                    child_node.singleton['DSP'] = 0
                    self.adjust_usage(child_node.name, 0)
                else:
                    dsp_iter += child_node.overall['DSP']
            node.iteration['DSP'] = dsp_iter
            if node.type == 'L':
                node.overall['DSP'] = dsp_iter * node.para
            elif node.inline:
                node.overall['DSP'] = dsp_iter
            else:
                if node.overall['DSP'] != dsp_iter:
                    MLOGGER.info("WARN: Adjustment error: mismatch happens!")
            return True
        else:
            return False

    def create_qor_graph(self, hls_result):
        """Create the usage graph for a new input case"""
        self.top_func = hls_result.kernel_func
        res_breakdown = hls_result.res_breakdown
        self.print_breakdown(res_breakdown)
        self.analyze_usage(self.top_func, None, 'F', res_breakdown, 1)
        budgets = self.qor_graph[self.top_func].overall
        # optimization specifically for DSP
        self.adjust_usage(self.top_func, budgets['DSP'])
        MLOGGER.debug("Begin printing usage graph:")
        self.print_qor_graph(self.top_func, 2, set())
        MLOGGER.debug("Complete printing.")

    @staticmethod
    def print_breakdown(res_breakdown):
        """Display the resource breakdown of a kernel"""
        MLOGGER.debug("Begin printing resource breakdown:")
        for comp in res_breakdown:
            MLOGGER.debug("  %s: %s", comp, res_breakdown[comp])
        MLOGGER.debug("Complete printing.")

    def print_qor_graph(self, top_func, indent, known_nodes):
        """Print out the entire QoR graph"""
        node = self.qor_graph[top_func] # type: QorGraphNode
        _type = node.type
        if node.inline:
            _type = "[I]F"
        indent_str = " " * indent
        debug_str = indent_str + top_func + ": " + _type + ", "
        debug_str += str(node.para) + ', '
        debug_str += str(node.overall) + ', '
        debug_str += str(node.iteration) + ', '
        debug_str += str(node.singleton) + ', '
        debug_str += str(node.fix_struct) + '.'
        MLOGGER.debug(debug_str)
        for child_node in node.child_nodes:
            if child_node.name not in known_nodes:
                known_nodes.add(child_node.name)
                self.print_qor_graph(child_node.name, indent+1, known_nodes)

    def analyze_usage(self, node_name, parent, _type, res_breakdown,
                      para_factor):
        """Analyze the resource usage of a node via HLS report"""
        if node_name in self.qor_graph:
            return self.qor_graph[node_name]
        if node_name not in self.skeleton:
            MLOGGER.info("WARN: Node %s does not present in the skeleton!",
                         node_name)
            return None

        node = QorGraphNode(node_name, self, _type, parent, para_factor)
        func_name = node_name.strip().split('@')[0]
        # Analyze the top level
        if _type == 'L':
            pass
        else:
            if func_name in self.fix_res:
                for res in MODEL_RES_LIST:
                    node.fix_struct[res] = self.fix_res[func_name][res]

            if func_name in res_breakdown:
                for res in MODEL_RES_LIST:
                    node.overall[res] = res_breakdown[func_name][res]
                    node.iteration[res] = res_breakdown[func_name][res]
            else:
                node.inline = True
                for res in MODEL_RES_LIST:
                    node.singleton[res] = 0

        # Analyze the sub nodes
        sub_skeletons = self.skeleton[node_name]
        for sub_name in sub_skeletons:
            sub_para_factor = sub_skeletons[sub_name]['unroll-factor']
            child_node_type = sub_name.strip().split('@')[-2]
            sub_func_name = sub_name.strip().split('@')[0]
            if child_node_type == 'F':
                child_node = self.analyze_usage(
                    sub_func_name, node, child_node_type, res_breakdown, 1)
            else:
                child_node = self.analyze_usage(
                    sub_name, node, child_node_type,
                    res_breakdown, sub_para_factor)
            node.child_nodes.add(child_node)

        child_nodes_usages = {'DSP': 0, 'LUT': 0}
        for child_node in node.child_nodes:
            for res in MODEL_RES_LIST:
                if child_nodes_usages[res] < 0:
                    pass
                elif child_node.overall[res] < 0:
                    child_nodes_usages[res] = -1
                else:
                    child_nodes_usages[res] += child_node.overall[res]

        # Finally go back to the top level
        if _type == 'L':
            pass
        else:
            for res in MODEL_RES_LIST:
                if node.inline:
                    node.overall[res] = child_nodes_usages[res] + \
                                             node.fix_struct[res]
                elif child_nodes_usages[res] < 0:
                    node.singleton[res] = -1
                else:
                    node.singleton[res] = node.overall[res] - \
                                               child_nodes_usages[res] - \
                                               node.fix_struct[res]
        # Update usage graph and return
        self.qor_graph[node_name] = node
        return node

class DSEResCacheXilinx(DSECacheBase):
    """
    A class for caching QoR results
    """

    def __init__(self, debug):
        super(DSEResCacheXilinx, self).__init__(
            'DSEResCacheXilinx', debug, False)

    @staticmethod
    def gen_cache_data(cfg_id, work_dir):
        """
        Abstract the skeleton from a given HLS kernel code and
        create cache data
        Return an object of CacheData
        """

        # Extract skeleton if needed
        lc_dir = '{0}/{1}/export/lc'.format(work_dir, OCL_DIR)
        if not os.path.exists(lc_dir):
            MLOGGER.info('ERROR: %s not found', lc_dir)
            return None
        if not os.path.exists('{0}_skeleton_ext'.format(lc_dir)):
            run_command('cp -rf {0} {0}_skeleton_ext'.format(lc_dir))
        lc_dir = '{0}_skeleton_ext'.format(lc_dir)

        if not os.path.exists('{0}/skeleton.json'.format(lc_dir)):
            merlin_pass_cmd = gen_merlin_pass_cmd(
                'ds_skeleton_extract', 'sdaccel')
            run_command("cd {0}; rm -rf metadata; {1}".format(
                lc_dir, merlin_pass_cmd), no_print=True)

        # Analyze BRAM and LUT (memory control) util
        merlin_pass_cmd = gen_merlin_pass_cmd(
            'ds_resource_eval', 'sdaccel')
        run_command("cd {0}; rm -rf metadata; rm -f usage.txt; {1}".format(
            lc_dir, merlin_pass_cmd), no_print=True)

        bram_usage = -1
        usage_results_filename = "{0}/usage.txt".format(lc_dir)
        with open(usage_results_filename, 'r') as usage_results_file:
            bram_usage = int(usage_results_file.readline().strip())
        MLOGGER.debug("BRAM usage obtained: %s", bram_usage)

        lut_memctrl = None
        lut_memctrl_filename = "{0}/lut_memctrl.json".format(lc_dir)
        with open(lut_memctrl_filename, 'r') as lut_memctrl_file:
            lut_memctrl = json.load(lut_memctrl_file)
        MLOGGER.debug("Begin printing lut_memctrl:")
        for item in lut_memctrl:
            MLOGGER.debug("   %s: %s", item, lut_memctrl[item])
        MLOGGER.debug("Done.")

        try:
            cache_data = CacheData(
                cfg_id, "{0}/skeleton.json".format(lc_dir),
                bram_usage, lut_memctrl)
        except Exception:
            MLOGGER.info('ERROR: Error in fetching the kernel skeleton')
            MLOGGER.info(traceback.format_exc())
            cache_data = None
        return cache_data

    @staticmethod
    def gen_code_change_list(data, ref_data):
        """
        Generate the code change list between two cache data
        """
        ret = {}
        for elt in ref_data.skeleton:
            for sub in ref_data.skeleton[elt]:
                if ref_data.skeleton[elt][sub] == data.skeleton[elt][sub]:
                    continue
                component = elt + '/' + sub
                ret[component] = {}
                for key, val in ref_data.skeleton[elt][sub].items():
                    if data.skeleton[elt][sub][key] == val:
                        continue
                    ret[component][key] = bytes(data.skeleton[elt][sub][key])
        return ret

    def gen_metadata_action(self, result_db, ref_data, data, mode):
        """
        Generate metadata for the estimator
        NOTE: The resource estimation is mainly done here
        """
        metadata = {'est_info': None, 'code_change': None, 'status': 'run'}
        if ref_data is None:
            MLOGGER.info('ref_data is missing, no metadata')
            return metadata

        code_change = self.gen_code_change_list(data, ref_data)

        # Match the change history in this cache data.
        # Treat this point as no change
        if not code_change:
            metadata['status'] = 'duplicate'
            metadata['est_info'] = deepcopy(
                result_db[ref_data.cfg_id].parsed_hls_info)
            return metadata
        if str(code_change) in ref_data.history:
            metadata['status'] = 'duplicate'
            if ref_data.history[str(code_change)] in result_db:
                # It's possbile that the duplicated config is running HLS
                # by another thead. In this case we just run HLS for this
                # config even it is duplicated
                metadata['est_info'] = deepcopy(result_db[
                    ref_data.history[str(code_change)]].parsed_hls_info)
                return metadata
        if mode != 'HLS-n-model':
            return metadata

        ref_data.history[str(code_change)] = data.cfg_id

        # Derive the resource usage from the reference data
        est_util = ResUtilEst()
        est_util.usages[0] = data.est_bram
        DSEXilinxResourceModel.derive_usage(data, ref_data, est_util)
        MLOGGER.debug("DSP usage estimated: %s", est_util.usages[1])
        MLOGGER.debug("LUT usage estimated: %s", est_util.usages[2])
        est_util.usages[3] = 0 # FF, skip

        # Check if we have estimated every kind of resources
        if not all([u != -1 for u in est_util.usages]):
            # Must run HLS so no metadata
            return metadata

        # We can skip HLS for resource
        est_info = deepcopy(
            result_db[ref_data.cfg_id].parsed_hls_info)

        kernel_blk = None
        for topo_id in est_info:
            if est_info[topo_id]['type'] == 'kernel':
                kernel_blk = est_info[topo_id]
                break
        if kernel_blk is None:
            MLOGGER.info('ERROR: Failed to find the top function component')
            metadata['est_info'] = None
            return metadata

        metadata['code_change'] = code_change

        # Update the total-/util-res of the top function
        # (skip FF now)
        # FIXME: util is now estimated by assuming the reference
        # design uses at least 1%
        for idx, res in enumerate(['BRAM', 'DSP', 'LUT']):
            old_usage = try_to_int(kernel_blk['total-{0}'.format(res)])
            old_util = try_to_int(kernel_blk['util-{0}'.format(res)])
            old_util = (0.01 if old_util is None or old_util == 0
                        else old_util / 100.0)

            new_usage = 0
            if est_util.usages[idx] != -1:
                new_usage = est_util.usages[idx]
            if old_usage > 0:
                new_util = '{:.0f}'.format(
                    100.0 * new_usage / (old_usage / old_util))
            else:
                new_util = 1
            kernel_blk['total-{0}'.format(res)] = bytes(new_usage)
            kernel_blk['util-{0}'.format(res)] = bytes(new_util)
        metadata['est_info'] = est_info
        return metadata

    @staticmethod
    def postproc_action(ref_data, data, metadata, result):
        """Update QoR graph in the cache data"""
        if result.error_code == HLSRetCode.DUPLICATE:
            return
        MLOGGER.info('Create QoR graph for cfg %s', data.cfg_id)
        data.create_qor_graph(result)
        if ref_data is not None:
            MLOGGER.info('Update QoR graph for cfg %s at cache %s',
                         ref_data.cfg_id, ref_data.data_id)
            ref_data.update_qor_graph(data)
        return

class DSEXilinxResourceModel(object):
    """
    A static class for running Xilinx resource estimation model
    """

    @staticmethod
    def cal_delta_usage(this, other, top_func, para_factor, res):
        """Calculate the delta usage for a specific resource type"""
        # for function:
        #   delta = delta(fix) + sum(delta(sub))
        # for loop:
        #   if same factor:
        #     delta = factor * sum(delta(sub))
        #   elif sum(delta(sub)) == 0:
        #     delta = delta(factor) * iter
        #   else:
        #     delta = delta(iter) * self_factor + \
        #         (iter + delta(iter)) * delta(factor)
        self_node = this.qor_graph[top_func]
        if self_node.overall[res] == 0:
            MLOGGER.debug("   %s: delta = 0", top_func)
            return 0
        other_skeleton = other.skeleton[top_func]
        delta_usage = 0
        for ele in self_node.child_nodes:
            if ele.type == 'L':
                sub_para = other_skeleton[ele.name]['unroll-factor']
            else:
                sub_para = 1
            sub_delta = DSEXilinxResourceModel.cal_delta_usage(
                this, other, ele.name, sub_para, res)
            if sub_delta < 0:
                MLOGGER.debug("   %s: delta = -1", top_func)
                return -1
            else: # pylint: disable=no-else-return
                delta_usage += sub_delta
        if delta_usage < 0:
            MLOGGER.info("WARN: Negative delta usage found!")
            return -1
        # delta_usage is now sum(delta(sub))
        if self_node.type == 'F':
            if self_node.name in other.fix_res:
                delta_usage += (other.fix_res[self_node.name][res] - \
                                self_node.fix_struct[res])
            MLOGGER.debug("   %s: delta = %d", top_func, delta_usage)
            return delta_usage
        if self_node.para == para_factor:
            delta_usage *= para_factor
            MLOGGER.debug("   %s: delta = %d", top_func, delta_usage)
            return delta_usage
        if self_node.iteration[res] < 0:
            MLOGGER.debug("   %s: delta = -1", top_func)
            return -1
        delta_usage = delta_usage * self_node.para + \
                      (self_node.iteration[res] + delta_usage) * \
                      (para_factor - self_node.para)
        MLOGGER.debug("   %s: delta = %d", top_func, delta_usage)
        return delta_usage

    @staticmethod
    def cal_usage(this, other, top_func, para_factor, res):
        """Calculate the usage for a specific resource type"""
        self_node = this.qor_graph[top_func]
        if self_node.overall[res] == 0:
            MLOGGER.debug("   %s: 0, False", top_func)
            return 0, False
        other_skeleton = other.skeleton[top_func]
        res_usage = 0
        changed = False
        for ele in self_node.child_nodes:
            if ele.type == 'L':
                sub_para = other_skeleton[ele.name]['unroll-factor']
            else:
                sub_para = 1
            sub_usage, sub_changed = DSEXilinxResourceModel.cal_usage(
                this, other, ele.name, sub_para, res)
            if sub_usage < 0 and sub_changed:
                MLOGGER.debug('  %s: -1, True', top_func)
                return -1, True
            if res_usage < 0 or sub_usage < 0:
                res_usage = -1
            else:
                res_usage += sub_usage
            if sub_changed:
                changed = True
        if res_usage >= 0:
            if self_node.name in other.fix_res:
                res_usage += other.fix_res[self_node.name][res]
            if self_node.singleton[res] >= 0:
                res_usage += self_node.singleton[res]
            elif not changed and self_node.iteration[res] >= 0:
                res_usage = self_node.iteration[res]
            else:
                res_usage = -1
        if self_node.type == 'L' and para_factor != self_node.para:
            changed = True
        if res_usage >= 0:
            overall_usage = res_usage * para_factor
        elif not changed and self_node.overall[res] >= 0:
            overall_usage = self_node.overall[res]
        else:
            overall_usage = -1
        MLOGGER.debug("   %s: %d, %d", top_func, overall_usage, changed)
        return overall_usage, changed

    @staticmethod
    def derive_usage(data, ref_data, est_util):
        """Obtain usage from reference data"""
        MLOGGER.debug("Begin printing usages for DSP:")
        est_util.usages[1], _ = DSEXilinxResourceModel.cal_usage(
            ref_data, data, ref_data.top_func, 1, 'DSP')
        if est_util.usages[1] < 0:
            est_util.usages[1] = DSEXilinxResourceModel.cal_delta_usage(
                ref_data, data, ref_data.top_func, 1, 'DSP')
            if est_util.usages[1] >= 0:
                est_util.usages[1] += \
                    ref_data.qor_graph[ref_data.top_func].overall['DSP']
        MLOGGER.debug("Begin printing usages for LUT:")
        est_util.usages[2], _ = DSEXilinxResourceModel.cal_usage(
            ref_data, data, ref_data.top_func, 1, 'LUT')
        if est_util.usages[2] < 0:
            est_util.usages[2] = DSEXilinxResourceModel.cal_delta_usage(
                ref_data, data, ref_data.top_func, 1, 'LUT')
            if est_util.usages[2] >= 0:
                est_util.usages[2] += \
                    ref_data.qor_graph[ref_data.top_func].overall['LUT']
        MLOGGER.debug("Done.")
