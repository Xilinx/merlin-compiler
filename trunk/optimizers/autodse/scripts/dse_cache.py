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

Filename    : optimizers/autodse/scripts/dse_cache.py
Description : This is the script for maintaining a cache to
              estimate the HLS result.
Usage       : Internal call
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 10-29-2018 Created (Cody Hao Yu)
"""
from abc import abstractmethod
import threading
from typing import List, Set, Dict, Any # pylint: disable=unused-import
from typing import Optional, Union # pylint: disable=unused-import
from dse_type import MetadataType # pylint: disable=unused-import
import merlin_logger
from dse_util import write_json_to_file
from dse_run_hls import HLSRetCode
from dse_run_hls import HLSResult # pylint: disable=unused-import

MLOGGER = merlin_logger.get_logger('DSE_QoR_Cache')

class CacheDataBase(object):
    """
    The base class of DSE cache data
    """
    def __init__(self, cfg_id, cache_key):
        # type: (int, str) -> None
        self.data_id = -1 # Initialize to an invalid value
        self.cfg_id = cfg_id
        self.hit_cfg_id = set() # type: Set[int]
        self.cache_key = cache_key

    def get_cache_key(self):
        # type: () -> str
        """Return the corresponding cache key"""
        return self.cache_key

    @abstractmethod
    def dump(self):
        # type: () -> Dict[str, Any]
        """Return a dict of this cache data"""
        raise NotImplementedError("Should have implemented this")

class DSECacheBase(object):
    """
    The base class of DSE cache
    """

    def __init__(self, name, debug, is_perf_cache=True):
        # type: (str, str, bool) -> None
        self.name = name
        self.cache = {} # type: Dict[str, CacheDataBase]
        self.cache_type = 'perf' if is_perf_cache else 'res'
        self.call_count = 0
        self.hit_count = 0
        self.skip_hls_count = 0
        self.cache_lock = threading.Lock()
        self.debug_mode = debug

    def has_key(self, key):
        # type: (str) -> bool
        """Check if the given key exisits in the cache already"""
        return key in self.cache

    def get_data(self, key):
        # type: (str) -> Optional[CacheDataBase]
        """
        Return the cache data based on the given key.
        Return None if the key does not exist.
        """
        return self.cache[key] if key in self.cache else None

    def hit(self, data):
        # type: (CacheDataBase) -> Optional[CacheDataBase]
        """
        ** ATOMIC OPERATION **
        Check if the input data matches any existing one
        Return the matched cache data or None
        """
        self.cache_lock.acquire()
        self.call_count += 1
        ref_data = None # type: Optional[CacheDataBase]
        if data is not None:
            ref_data = self.get_data(data.get_cache_key())
            if ref_data is not None:
                ref_data.hit_cfg_id.add(data.cfg_id)
        self.hit_count += 1 if ref_data else 0
        self.cache_lock.release()
        return ref_data

    def add(self, cfg_id, data):
        # type: (int, CacheDataBase) -> bool
        """
        ** ATOMIC OPERATION **
        Add a new data to the cache or update the existing data
        Return True if we add a new data to the cache
        """
        if data is None:
            return False
        if not self.has_key(data.cache_key): # New data, add to cache
            self.cache_lock.acquire()
            data.data_id = len(self.cache) # Assign cache data ID
            self.cache[data.cache_key] = data
            MLOGGER.info('%s adds cfg %d to cache ID %d',
                         self.name, cfg_id, data.data_id)
            self.cache_lock.release()
            return True
        return False

    def gen_metadata(self,
                     result_db, # type: Dict[int, HLSResult]
                     ref_data,  # type: CacheDataBase
                     data,      # type: CacheDataBase
                     mode       # type: str
                    ):
        # type: (...) -> MetadataType
        """
        Generate metadata for cache hit config to run the estimator
        Return: a dict with 3 keys: est_info, code_change, status
        est_info: The reference HLS result
        code_change: The delta for estimator to derive the new values
        status: duplicate, run
        """
        metadata = self.gen_metadata_action(result_db, ref_data, data, mode)
        if 'est_info' not in metadata:
            raise KeyError('est_info is missing in the metadata')
        elif 'code_change' not in metadata:
            raise KeyError('code_change is missing in the metadata')
        elif 'status' not in metadata:
            raise KeyError('status is missing in the metadata')
        return metadata

    def postproc(self,
                 ref_data, # type: Optional[CacheDataBase]
                 data,     # type: CacheDataBase
                 metadata, # type: Optional[MetadataType]
                 result    # type: HLSResult
                ):
        # type: (...) -> None
        """
        Any update to the cache data after getting HLS result
        """
        self.cache_lock.acquire()
        if (result.error_code == HLSRetCode.DUPLICATE or
                result.estimate[self.cache_type]):
            self.skip_hls_count += 1
        self.cache_lock.release()
        self.postproc_action(ref_data, data, metadata, result)

    def get_hit_count(self):
        # type: () -> int
        """
        Return the overall hit count of this cache
        """
        return self.hit_count

    def get_skip_hls_count(self):
        # type: () -> int
        """
        Return the number of HLS runs we saved
        """
        return self.skip_hls_count

    def dump(self, path, result_db):
        # type: (str, Dict[int, HLSResult]) -> None
        """
        Dump ALL cache data to separate json files
        """
        for data in self.cache.values():
            jdata = data.dump()
            jdata['result'] = result_db[data.cfg_id].parsed_hls_info
            write_json_to_file(
                jdata, '{0}/{1}.json'.format(path, data.cfg_id))

    ### To be implemented methods

    @staticmethod
    @abstractmethod
    def gen_cache_data(cfg_id, work_dir):
        # type: (int, str) -> Optional[CacheDataBase]
        """Generate an object of CacheData"""
        raise NotImplementedError("Should have implemented this")

    @abstractmethod
    def gen_metadata_action(self,
                            result_db, # type: Dict[int, HLSResult]
                            ref_data,  # type: CacheDataBase
                            data,      # type: CacheDataBase
                            mode       # type: str
                           ):
        # type: (...) -> MetadataType
        """
        The real method for what to do for generating metadata
        """
        raise NotImplementedError("Should have implemented this")

    @staticmethod
    @abstractmethod
    def postproc_action(ref_data, # type: Optional[CacheDataBase]
                        data,     # type: CacheDataBase
                        metadata, # type: Optional[MetadataType]
                        result    # type: HLSResult
                       ):
        # type: (...) -> None
        """
        The real method for any update to the cache data
        after getting HLS result
        """
        pass # pylint: disable=unnecessary-pass
