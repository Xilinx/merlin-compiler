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

Filename    : optimizers/autodse/scripts/dse_cache_perf_xilinx.py
Description : This is the script for maintaining a cache to
              estimate the performance of Xilinx HLS.
Usage       : Internal call
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-11-2018 Created (Cody Hao Yu)
"""
from copy import deepcopy
import json
import os
from random import randint
import traceback

from typing import Dict, Set, Optional, Any # pylint: disable=unused-import
from dse_type import MetadataType # pylint: disable=unused-import
from dse_run_hls import HLSResult # pylint: disable=unused-import

from dse_cache import CacheDataBase, DSECacheBase
from dse_run_hls import HLSRetCode
from dse_util import gen_merlin_pass_cmd, OCL_DIR
from util import run_command
import merlin_logger

MLOGGER = merlin_logger.get_logger('DSE_Cache_Perf_Xilinx')

class CacheData(CacheDataBase):
    """
    Cache data for config ID with topo ID as the key
    """
    def __init__(self, cfg_id, filename):
        # type: (int, str) -> None
        with open(filename, 'r') as skeleton_file:
            jdata = json.load(skeleton_file)
            self.skeleton = jdata[u'skeleton']
            self.topo_id_map = jdata[u'topo_id_map']
        self.history = {} # type: Dict[str, int]
        cache_key = self.gen_cache_key(self.skeleton, self.topo_id_map)
        if cache_key is None:
            MLOGGER.info('ERROR: Fail to generate cahe key for config %d',
                         cfg_id)
            MLOGGER.info(traceback.format_exc())
        else:
            super(CacheData, self).__init__(cfg_id, cache_key)

    @staticmethod
    def gen_cache_key(skeleton, topo_id_map):
        # type: (Dict[str, Dict[str, int]], Dict[str, str]) -> Optional[str]
        """
        Generate the cache key using topo ID
        Key: We sort all topo IDs we care and form a string.
        """
        if not topo_id_map:
            return None

        key1 = str({k: set(v.keys()) if v else set()
                    for k, v in skeleton.items()})
        key2 = ';'.join(sorted(topo_id_map.values()))
        return key1 + '!' + key2

    def dump(self):
        # type: () -> Dict[str, Any]
        """Return a dict of this cache data"""
        jdata = {}
        jdata['topo_id_map'] = self.topo_id_map
        jdata['skeleton'] = self.skeleton
        jdata['change_history'] = {}
        for key, value in self.history.items():
            jdata['change_history'][value] = key
        return jdata

class DSEPerfCacheXilinx(DSECacheBase):
    """
    A class for caching QoR results
    """

    def __init__(self, debug):
        # type: (str) -> None
        super(DSEPerfCacheXilinx, self).__init__('DSEPerfCacheXilinx', debug)

    @staticmethod
    def gen_cache_data(cfg_id, work_dir):
        # type: (int, str) -> Optional[CacheDataBase]
        """
        Create cache data from HLS C kernel
        Return an object of CacheData
        """

        # Extract skeleton and topo ID if needed
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
            run_command("cd {0}; {1}".format(
                lc_dir, merlin_pass_cmd), no_print=True)

        cache_data = None # type: Optional[CacheData]
        try:
            cache_data = CacheData(
                cfg_id, "{0}/skeleton.json".format(lc_dir))
        except Exception:
            MLOGGER.info('ERROR: Error in creating cache data')
            MLOGGER.info(traceback.format_exc())
        return cache_data

    @staticmethod
    def gen_code_change_list(data, ref_data):
        # type: (CacheData, CacheData) -> Dict[str, Dict[str, str]]
        """
        Generate the code change list between two cache data
        """
        ret = {} # type: Dict[str, Dict[str, str]]
        for elt in ref_data.skeleton:
            for sub in ref_data.skeleton[elt]:
                if ref_data.skeleton[elt][sub] == data.skeleton[elt][sub]:
                    continue
                topo_id = data.topo_id_map[sub]
                ret[topo_id] = {}
                for key, val in ref_data.skeleton[elt][sub].items():
                    if data.skeleton[elt][sub][key] == val:
                        continue
                    ret[topo_id][key] = bytes(data.skeleton[elt][sub][key])
        return ret

    def gen_metadata_action(self,
                            result_db, # type: Dict[int, HLSResult]
                            ref_data,  # type: CacheDataBase
                            data,      # type: CacheDataBase
                            mode       # type: str
                           ):
        # type: (...) -> MetadataType
        """Generate metadata for the estimator"""
        assert ref_data is None or isinstance(ref_data, CacheData)
        assert isinstance(data, CacheData)
        metadata = {'est_info': None,
                    'code_change': None,
                    'status': 'run'} # type: MetadataType
        if ref_data is None:
            MLOGGER.info('ref_data is missing, no metadata')
            return metadata

        try:
            change_list = self.gen_code_change_list(data, ref_data)
        except Exception:
            MLOGGER.info(traceback.format_exc())
            return metadata

        # Match the change history in this cache data.
        # Treat this point as no change
        if not change_list:
            metadata['status'] = 'duplicate'
            metadata['est_info'] = deepcopy(
                result_db[ref_data.cfg_id].parsed_hls_info)
        elif str(change_list) in ref_data.history:
            metadata['status'] = 'duplicate'
            metadata['est_info'] = deepcopy(
                result_db[ref_data.history[str(change_list)]].parsed_hls_info)
        else:
            metadata['est_info'] = deepcopy(
                result_db[ref_data.cfg_id].parsed_hls_info)
            metadata['code_change'] = change_list
            MLOGGER.info('Code change: %s', str(change_list))
        return metadata

    @staticmethod
    def postproc_action(ref_data, # type: Optional[CacheDataBase]
                        data,     # type: CacheDataBase
                        metadata, # type: Optional[MetadataType]
                        result    # type: HLSResult
                       ):
        # type: (...) -> None
        """Update code change"""
        assert ref_data is None or isinstance(ref_data, CacheData)
        assert isinstance(data, CacheData)

        if result.error_code == HLSRetCode.DUPLICATE or ref_data is None:
            return

        code_change = metadata['code_change'] if metadata else None

        if code_change is None:
            # Duplicate the cache miss point
            code_change = 'baseline-dup{0}'.format(randint(0, 1000))
        elif str(code_change) in ref_data.history:
            # Duplicate code change due to multi-thread weak consistency
            code_change = '{0}-dup{1}'.format(
                str(code_change), randint(0, 1000))
        MLOGGER.info('Add cfg %s to the history of cfg %s at cache %s',
                     data.cfg_id, ref_data.cfg_id, ref_data.data_id)
        ref_data.history[str(code_change)] = data.cfg_id
        return
