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

Filename    : optimizers/autodse/scripts/ds_proc_builder.py
Description : This is the script for the DSE that building the design space.
Usage       : Look at "Argument parsing" section in this script for the
              detail configuration.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 03-15-2018 First created (Cody Hao Yu)
"""
# Filter out warnings from scipy
# pylint: disable=wrong-import-position
import warnings
warnings.filterwarnings("ignore", message="numpy.dtype size changed")
warnings.filterwarnings("ignore", message="numpy.ufunc size changed")

import argparse
import ast
import os
import sys
import json
import re
from copy import deepcopy
from collections import deque
from sklearn.cluster import KMeans

from opentuner.search.manipulator import safe_eval, SAFE_LIST
from ds_proc_profiler import profile_design_space_list
from util import run_command, has_merlin_error, try_to_int, try_to_float
from dse_util import write_json_to_file, get_ds_id_from_auto
from dse_util import MERLIN_COMPILER_HOME, gen_merlin_pass_cmd
import merlin_logger

MLOGGER = merlin_logger.get_logger('DS_Proc_Builder')

CFG_PATH = '{0}/optimizers/autodse/configs'.format(MERLIN_COMPILER_HOME)

PARSER = argparse.ArgumentParser(description='DS Builder')
PARSER.add_argument('--list', action='store',
                    default='{0}/design_space_list.json'.format(CFG_PATH),
                    help='passes for inserting design space pragmas')
PARSER.add_argument('--max-partition', action='store',
                    default=16,
                    help='maximum partition number')
PARSER.add_argument('--type', action='store',
                    default='full',
                    help='design space type (full, custom)')
PARSER.add_argument('path', action='store',
                    help='input kernel path')

# Arguments for enabling profiling
PARSER.add_argument('--no-profiling', action='store_true',
                    default=False,
                    help='Skip the profiling process (used by unit test only)')
PARSER.add_argument('--hls-time-limit', action='store',
                    default=900,
                    help='HLS time limit (default 15 minutes)')
PARSER.add_argument('--impl-tool', action='store',
                    default='sdaccel',
                    help='Vendor tool (default sdaccel')
PARSER.add_argument('--encrypt', action='store_true',
                    default=False,
                    help='source code encryption')
PARSER.add_argument('--max-thread', action='store',
                    default=8,
                    help='Max available thread for profiling')
PARSER.add_argument('--profile-dir', action='store',
                    default='/tmp',
                    help='The temporary place for profiling intermediate files')
ARGS = PARSER.parse_args()

##############################################################
# Environment setting
##############################################################

FULL_DESIGN_SPACE_FILE = 'ds_all.json'

def exit_ds_proc_builder(exit_code):
    """Exit the DSE builder with assigned code"""
    sys.exit(exit_code)

#####################################
# Functions for Building Design Space
#####################################

def parse_ds_from_string(attr_str):
    """
    Return a pair that contains ID and attributes of the design space
    """
    # Build a of for design space attributes
    # Syntax: key1: value1; key2: value2; ...
    data = {a[:a.find(':')].strip().lower(): a[a.find(':') + 1:].strip()
            for a in attr_str.split(';')}

    attrs = {}

    # [TASK] ID/Option syntax checking
    #        Sync-parameter checking
    # Ex: auto{options: A=[...]}
    #     auto{options: A} <- sync parameter
    if 'options' not in data:
        MLOGGER.error(8, attr_str)
        return (None, None, None)
    if data['options'].find('=') == -1:
        ds_id = get_ds_id_from_auto(attr_str)
        MLOGGER.fcs_lv3('Ignore sync-parameter %s', ds_id)
        return (ds_id, None, None)
    try:
        if re.match(r'.+ *= *\[.+\]', data['options']):
            tokens = re.search(r'(.+) *= *(\[.+\])', data['options'])
        elif re.match(r'.+ *=.+', data['options']):
            tokens = re.search(r'(.+) *= *(.+)', data['options'])
        else:
            raise AttributeError()
        attrs['id'], attrs['options'] = tokens.groups()
        attrs['id'] = attrs['id'].strip()
        attrs['options'] = attrs['options'].strip()
    except (AttributeError, IndexError):
        MLOGGER.error(9, data['options'], attr_str)
        return (None, None, None)

    # Legalization checking for options
    # 1. Check if the expression uses forbidded builtins
    # 2. Create a dependency list
    try:
        st = ast.parse(attrs['options'])
    except SyntaxError as err:
        MLOGGER.error(10, data['options'], attr_str)
        return (None, None, None)

    # Traverse AST of the expression for all variables
    names = set()
    iter_val = None
    for node in ast.walk(st):
        if isinstance(node, ast.ListComp):
            funcs = [n.func.id for n in ast.walk(node.elt)
                     if isinstance(n, ast.Call)]
            elt_vals = [n.id for n in ast.walk(node.elt)
                        if isinstance(n, ast.Name) and n.id not in funcs
                        and n.id != '_']
            if len(elt_vals) == 1:
                iter_val = elt_vals[0]
            elif len(elt_vals) > 1:
                MLOGGER.error(34, attrs['options'])
                return (None, None, None)
        elif isinstance(node, ast.Name):
            names.add(node.id)

    # Ignore the list comprehension iterator
    if iter_val:
        names.remove(iter_val)

    # Ignore legal builtin functions
    for _, funcs in SAFE_LIST.items():
        for func in funcs:
            if func in names:
                names.remove(func)

    # Ignore builtin primitive type casting
    for ptype in ['int', 'str', 'float']:
        if ptype in names:
            names.remove(ptype)
    attrs['deps'] = {name: 1 for name in names}

    # [TASK] Partition rule syntax checking
    if 'order' in data:
        local = {}
        while True:
            try:
                safe_eval(data['order'], local)
                attrs['order'] = data['order']
                attrs['order_var'] = local.keys()[0]
                MLOGGER.fcs_lv2('%s has a valid partition rule: %s',
                                attrs['id'], data['order'])
            except SyntaxError:
                MLOGGER.error(12, attr_str)
            except NameError as err:
                name = re.search(r"'(.+)' is not defined", str(err)).group(1)
                if not local:
                    local[name] = 0
                    continue
                else:
                    var = local.keys()[0]
                    MLOGGER.error(13, var, name, attr_str)
            break

    # Compute maximal size by ignoring conditions
    try:
        if attrs['options'].find('if') != -1:
            cond_start = attrs['options'].find('if')
            options = safe_eval('{0}]'.format(attrs['options'][:cond_start]))
        else:
            options = safe_eval(attrs['options'])
    except NameError as err:
        name = re.search(r"'(.+)' is not defined", str(err)).group(1)
        MLOGGER.error(32, name)
        return (None, None, None)

    max_size = len(options)

    def check_list_type(attrs, lst):
        """
        Check if all elements in the list has the same type as the default
        """
        target = type(attrs['default'])
        if not all([isinstance(v, target) for v in lst]):
            MLOGGER.error(14, attrs['id'], target.__name__, str(lst))
            return False
        return True

    # [TASK] Default option checking
    if 'default' not in data:
        MLOGGER.error(15, attr_str)
        return (None, None, None)
    elif data['default'].find('"') != -1:
        attrs['default'] = str(data['default'])
        if not check_list_type(attrs, options):
            return (None, None, None)
    else:
        if try_to_int(data['default']):
            attrs['default'] = int(data['default'])
            # Check int type for all options
            if not check_list_type(attrs, options):
                return (None, None, None)
        elif try_to_float(data['default']):
            attrs['default'] = float(data['default'])
            # Check float type for all options
            if not check_list_type(attrs, options):
                return (None, None, None)
        else:
            MLOGGER.error(16, data['default'])
            return (None, None, None)
    return (attrs['id'], attrs, max_size)

def load_ds_pass(json_file):
    """Build the pass list that constructs the design space"""
    from collections import OrderedDict

    if not os.path.exists(json_file):
        MLOGGER.error(17)
        exit_ds_proc_builder(1)

    filep = open(json_file, 'r')
    data = json.loads(filep.read(), object_pairs_hook=OrderedDict)
    filep.close()

    pass_list = []
    for pass_name in data:
        status = data[pass_name]
        if status == 'on':
            pass_list.append(pass_name)

    MLOGGER.fcs_lv3('Loaded %s design space passes: %s', len(pass_list),
                    ', '.join(pass_list))
    return pass_list

def insert_design_space_pragmas(pass_list, path, impl_tool):
    """
    Execute all design space passes for creating the design space
    """
    for pass_name in load_ds_pass(pass_list):
        cmd = gen_merlin_pass_cmd(pass_name, impl_tool)
        run_command('cd {0}; {1}'.format(path, cmd), no_print=True)

    # Check error
    if has_merlin_error():
        MLOGGER.error(18)
        exit_ds_proc_builder(1)

def parse_pragmas(path):
    """
    Parse design space pragmas from kernel files
    """
    cmd = gen_merlin_pass_cmd('ds_scope')
    run_command('cd {0}; {1}'.format(path, cmd), no_print=True)
    if not os.path.exists('{0}/auto_pragmas.json'.format(path)):
        MLOGGER.error(19)
        exit_ds_proc_builder(1)
    return '{0}/auto_pragmas.json'.format(path)

def build_design_space(pragma_file):
    """
    Extract the design space from pragmas
    """
    total_size = 1
    params = {}
    sync_params = {}

    filep = open(pragma_file, 'r')
    pragma_to_topo = json.loads(filep.read())
    filep.close()

    # Fetch and parse pragmas
    for line, topo_id in pragma_to_topo.items():
        pragma = line.replace('\n', '')

        attr_strs = re.findall(r'auto{(.*?)}', pragma, re.IGNORECASE)
        if not attr_strs:
            MLOGGER.error(20, pragma)
            return (None, 0)
        for attr_str in attr_strs:
            pid, attr, max_size = parse_ds_from_string(attr_str)
            if pid is None:
                return (None, 0)
            if attr is None: # Sync-parameter does not have attributes
                if pid not in sync_params:
                    sync_params[pid] = set()
                sync_params[pid].add(topo_id)
                continue
            if pid in params:
                MLOGGER.error(31, pid, pragma)
                return (None, 0)
            attr['scope'] = [topo_id]
            params[pid] = attr
            total_size *= max_size

    # Sync-parameter checking
    for pid, topo_id in sync_params.items():
        if pid not in params:
            MLOGGER.error(33, pid)
            return (None, 0)
        params[pid]['scope'] += list(topo_id)

    MLOGGER.info(15, len(params))
    return (params, total_size)

def compile_design_space(full_design_space_file, args):
    """
    The overall process of creating the design space
    """
    # Insert design space pragmas
    if args.type == 'full':
        insert_design_space_pragmas(args.list, args.path, args.impl_tool)

    # Parse design space pragmas from kernel files
    pragma_file = parse_pragmas(args.path)

    # Build design space
    design_space, ds_size = build_design_space(pragma_file)
    MLOGGER.info(16, ds_size)
    if ds_size <= 1:
        MLOGGER.warning(13)
        return False

    if not write_json_to_file([design_space], full_design_space_file):
        MLOGGER.error(21)
        return False
    return True

#########################################
# Functions for Partitioning Design Space
#########################################

def gen_seed(params):
    """
    Generate a seed config of a design space by simply taking
    default values
    """
    seed = {}
    for param in params.values():
        seed[param['id']] = param['default']
    return seed

def topo_sort_params(params):
    """
    Sort parameters according to their dependency topologically
    and return a list of IDs
    """
    def topo_sort_helper(curr_id, visited, stack):
        """
        The helper function for topological sort
        """
        visited.add(curr_id)
        if curr_id not in params:
            MLOGGER.error(32, curr_id)
            return False
        for dep in params[curr_id]['deps']:
            if dep not in visited:
                if not topo_sort_helper(dep, visited, stack):
                    return False
        stack.append(curr_id)
        return True

    visited = set()
    stack = []
    for param_id in params.keys():
        if param_id not in visited:
            if not topo_sort_helper(param_id, visited, stack):
                return None
    return stack

def create_partitions(params, sorted_ids, max_num):
    """
    Partition the design space using level-order tree traversal
    The reason of using level order is that we want to partition high
    impact parameters only if the given maximum partition number
    is too small to partition all candidates
    """
    queue = deque([deepcopy(params)])
    ptr = 0
    while len(queue) < max_num and ptr < len(params):
        next_queue = deque()
        while queue:
            curr_ds = queue.pop()
            param_id = sorted_ids[ptr]
            param = curr_ds[param_id]

            # Update dependent parameter value to its default value
            for dep in param['deps']:
                param['deps'][dep] = (
                    curr_ds[dep]['default'].replace('"', '')
                    if isinstance(curr_ds[dep]['default'], str)
                    else curr_ds[dep]['default'])
                MLOGGER.fcs_lv3('Set %s to %s', dep, param['deps'][dep])

            # Evaluate the available options and partition them
            parts = None
            if 'order' in param:
                for option in safe_eval(param['options'], param['deps']):
                    # Use user-given part rule to classify options
                    part_idx = safe_eval(param['order'],
                                         {param['order_var']: option})
                    MLOGGER.fcs_lv3('%s option %s is part %d',
                                    param_id, option, part_idx)
                    if parts is None:
                        parts = {}
                    if part_idx not in parts:
                        parts[part_idx] = []
                    parts[part_idx].append(option)

            accum_part = len(queue) + len(next_queue)
            if parts and len(parts) == 1:
                # Do not partition this parameter because it is
                # fully shadowed in this partition
                copied_ds = deepcopy(curr_ds)
                default = copied_ds[param_id]['default']
                copied_ds[param_id]['options'] = '[{0}]'.format(default)
                next_queue.append(copied_ds)
                MLOGGER.fcs_lv2('{0}: stop partitioning {1} due to shadow'
                                .format(ptr, param_id))
            elif not parts or accum_part + len(parts) > max_num:
                # Do not partition this parameter because it is either
                # 1) not a partitionable parameter, or
                # 2) the accumulated partition number reaches to the limit
                copied_ds = deepcopy(curr_ds)
                next_queue.append(copied_ds)
                MLOGGER.fcs_lv2('{0}: stop partitioning {1} due to no rule '
                                .format(ptr, param_id) +
                                'or too many ({0})'.format(max_num))
            else:
                # Partition this parameter
                for part in parts.values():
                    copied_ds = deepcopy(curr_ds)
                    copied_ds[param_id]['options'] = str(part)
                    copied_ds[param_id]['default'] = part[0] # Reset default
                    next_queue.append(copied_ds)
                MLOGGER.fcs_lv2('{0}: partition {1} to {2} parts, '
                                .format(ptr, param_id, len(parts)) +
                                'so far {0} parts'
                                .format(len(queue) + len(next_queue)))
        queue = next_queue
        ptr += 1
    return [part for part in reversed(queue)]

def partition_design_space(full_design_space_file, max_num):
    """
    Partition the design space according to the given partition rule
    NOTE: We now assume the parameter with a partition rule must have
    a static option list, meaning that the option range does not depend
    on any other parameters
    """
    filep = open(full_design_space_file, 'r')
    params = json.loads(filep.read())[0]
    filep.close()

    # Sort parameters topologically (break cycles)
    sorted_param_ids = topo_sort_params(params)
    if sorted_param_ids is None:
        return None

    # Partition design space
    parts = create_partitions(params, sorted_param_ids, max_num)
    parts = [params] if parts is None else parts

    MLOGGER.fcs_lv2('Partition design space to %s partitions', len(parts))
    cnt = 0
    part_ds_files = []
    for part in parts:
        seed = gen_seed(part)
        if not seed:
            return None
        data = [part] + [seed]

        # Generate partition file name and write to file
        part_ds_file = 'ds_part{0}.json'.format(cnt)
        if not write_json_to_file(data, part_ds_file):
            MLOGGER.error(22)
            return None
        part_ds_files.append(part_ds_file)
        cnt += 1
    return part_ds_files

def prune_design_space_partition(part_ds_files, max_num):
    """
    Filter design space partitions that
    1. cannot pass HLS in time, or
    2. is similar to the other partition
    """
    MLOGGER.fcs_lv2('Pruning partitions')
    part_idx_map = []
    part_profile = []

    # Fetch profiling info and filter out failed partitions
    for idx, part in enumerate(part_ds_files):
        with open(part, 'r') as filep:
            data = json.loads(filep.read())
            if not 'metadata_prev-eval-cfg' in data[0]:
                continue
            part_idx_map.append(idx)
            profile = ([float(data[0]['metadata_prev-eval-cfg']['latency'])] +
                       [float(val) for _, val in
                        data[0]['metadata_prev-eval-cfg']['resource'].items()])
            part_profile.append(profile)

    # Too many partitions, clustering and keep the best one per cluster
    if len(part_idx_map) > max_num:
        kmeans = KMeans(n_clusters=max_num).fit(part_profile)
        clusters = [(-1, float('inf')) for _ in range(max_num)]
        for (idx, cluster) in enumerate(kmeans.labels_):
            if part_profile[idx][0] < clusters[cluster][1]:
                clusters[cluster] = (part_idx_map[idx], part_profile[idx][0])
        part_idx_map = sorted([idx for idx, _ in clusters if idx != -1])

    # Rename partition files
    for part in part_ds_files:
        run_command('mv {0} old_{0}'.format(part))

    # Reindex partitions
    for new_idx, old_idx in enumerate(part_idx_map):
        assert new_idx <= old_idx, 'new idx {0} > old idx {1}'.format(
            new_idx, old_idx)
        run_command('mv old_{0} {1}'.format(part_ds_files[old_idx],
                                            part_ds_files[new_idx]))

##############################################################
# Main process
##############################################################

if __name__ == "__main__":
    # Build design space
    if not compile_design_space(FULL_DESIGN_SPACE_FILE, ARGS):
        exit_ds_proc_builder(1)

    # Partition design space
    PART_DS_FILES = partition_design_space(FULL_DESIGN_SPACE_FILE, 8)
    if PART_DS_FILES is None:
        exit_ds_proc_builder(1)

    if ARGS.no_profiling:
        exit_ds_proc_builder(0)

    # Profile and prune partitions using HLS
    MLOGGER.info(17, len(PART_DS_FILES))
    run_command('mkdir {0}/profile'.format(ARGS.path))
    profile_design_space_list(PART_DS_FILES, ARGS)
    prune_design_space_partition(PART_DS_FILES, int(ARGS.max_partition))
    exit_ds_proc_builder(0)
