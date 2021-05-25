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

Filename    : optimizers/autodse/scripts/dse_proc_eval.py
Description : This is the DSE process of evaluating design points.
Usage       : Internal call
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-10-2018 Created (Cody Hao Yu)
"""
import traceback
from copy import deepcopy

from typing import Tuple, Dict, List # pylint: disable=unused-import
from typing import Union, Optional # pylint: disable=unused-import
from dse_type import ( # pylint: disable=unused-import
    MetadataType, ModelAccuracyType)

from dse_cache import DSECacheBase # pylint: disable=unused-import
from dse_cache import CacheDataBase
from dse_cache_perf_xilinx import DSEPerfCacheXilinx
from dse_cache_res_xilinx import DSEResCacheXilinx
from dse_cache_perf_intel import DSEPerfCacheIntel
from dse_cache_res_intel import DSEResCacheIntel
from dse_run_hls import DSERunHLS, HLSRetCode, HLSResult
from dse_midend_checker import collect_midend_failures
from dse_util import TRANS_DIR
from util import run_command
import merlin_logger

MLOGGER = merlin_logger.get_logger('DSE_QoR_Evaluator')

class DSEQoREvaluator(object):
    """
    A class for evaluating the QoR of design points
    """
    InitHitCacheType = Tuple[Dict[str, Optional[CacheDataBase]],
                             Dict[str, CacheDataBase],
                             Dict[str, Optional[MetadataType]],
                             HLSResult]

    def __init__(self, impl_tool, time_limit, mode, debug):
        # type: (str, int, str, str) -> None
        self.impl_tool = impl_tool
        self.mode = mode
        self.hls_time_limit = time_limit
        self.debug_mode = debug
        self.hls_info_db = {} # type: Dict[int, HLSResult]
        self.skip_hls_count = 0
        self.cache = {} # type: Dict[str, DSECacheBase]
        if self.impl_tool == 'sdaccel':
            self.cache['perf'] = DSEPerfCacheXilinx(self.debug_mode)
            self.cache['res'] = DSEResCacheXilinx(self.debug_mode)
        else:
            self.cache['perf'] = DSEPerfCacheIntel(self.debug_mode)
            self.cache['res'] = DSEResCacheIntel(self.debug_mode)

    def gen_result_log(self, hls, result):
        # type: (DSERunHLS, HLSResult) -> str
        """
        Generate the corresponding log based on HLS result
        """
        log_str = ''
        if result.error_code == HLSRetCode.HLS_TIME_OUT:
            log_str = ('inf (timeout)\t\t\t\t{0}'
                       .format(str(self.hls_time_limit)))
        elif result.error_code == HLSRetCode.HLS_ERROR:
            (is_extempt, hls_error_code) = hls.check_extempt()
            if is_extempt:
                log_str = 'inf (low_QoR)\t\t\t\t{0:.1f}'.format(result.time)
            else:
                log_str = ('inf (hls_error: {0})\t\t\t\t{1:.1f}'
                           .format(hls_error_code, result.time))
        else:
            if result.error_code == HLSRetCode.HLS_RESULT_UNDEF:
                log_str = 'inf (hls_undef)\t'
            elif result.error_code == HLSRetCode.HLS_NO_RESOURCE:
                log_str = '{0} (resource)\t'.format(result.latency)
            elif result.error_code == HLSRetCode.DUPLICATE:
                log_str = '{0} (duplicate)\t'.format(result.latency)
            elif result.source == 'est':
                log_str = '{0} (estimated)\t'.format(result.latency)
            else:
                log_str = '{0}\t'.format(result.latency)
            for res in result.res_detail:
                log_str += ('{0:.2f}\t'
                            .format(result.res_detail[res] * 100.0))
            log_str += '{0:.1f}'.format(result.time)
        return log_str

    @staticmethod
    def compute_model_accuracy(hit_cfg_ids, # type: Dict[str, int]
                               hls # DSERunHLS
                              ):
        # type: (...) -> ModelAccuracyType
        """
        Compute and log the accuracy of the model
        Attach a dict of accuracy to the HLS result
        """
        model_accuracy = {} # type: ModelAccuracyType
        model_accuracy['hit-cfg-ids'] = hit_cfg_ids

        model_accuracy['cycle-hls'] = 'N/A'
        model_accuracy['cycle-err'] = 'N/A'
        model_accuracy['cycle-est'] = 'N/A'
        for res in ['BRAM', 'DSP', 'LUT']:
            model_accuracy['{0}-hls'.format(res)] = 'N/A'
            model_accuracy['{0}-est'.format(res)] = 'N/A'
            model_accuracy['{0}-err'.format(res)] = 'N/A'

        perf_result = hls.parse_merlin_hls_report('report_merlin_perf_bak')
        res_result = hls.parse_merlin_hls_report('report_merlin_res_bak')
        ref_result = hls.parse_merlin_hls_report('report_merlin_ref')

        # Not debug mode so no HLS reference
        if ref_result is None:
            return model_accuracy

        # Check and fill HLS result if success
        hls_success = (ref_result.latency != 0 and
                       ref_result.latency != float('inf'))
        if hls_success:
            model_accuracy['cycle-hls'] = str(ref_result.latency)
            ref = ref_result.res_abs_detail
            for res in ['BRAM', 'DSP', 'LUT']:
                model_accuracy['{0}-hls'.format(res)] = ref[res]
        else:
            model_accuracy['cycle-hls'] = 'Failed'
            return model_accuracy

        # Performance model analysis
        if perf_result is not None:
            model_accuracy['cycle-est'] = str(perf_result.latency)
            if hls_success:
                err = (100 * (ref_result.latency - float(perf_result.latency)) /
                       ref_result.latency)
                model_accuracy['cycle-err'] = '{:f}'.format(err)

        # Resource model analysis
        if res_result is not None:
            ref = ref_result.res_abs_detail
            est = res_result.res_abs_detail
            for res in ['BRAM', 'DSP', 'LUT']:
                model_accuracy['{0}-est'.format(res)] = est[res]
                try:
                    err = ((float(ref[res]) - float(est[res])) /
                           float(ref[res]) * 100.0)
                    model_accuracy['{0}-err'.format(res)] = str(err)
                except ZeroDivisionError:
                    model_accuracy['{0}-err'.format(res)] = 'N/A'
        return model_accuracy

    def hit_cache(self, cfg_id, work_dir):
        # type: (int, str) -> InitHitCacheType
        """
        The cache interface that initializes cache data for each model
        Return: (ref_data, data, metadata, result)
        """
        data = {} # type: Dict[str, CacheDataBase]
        ref_data = {} # type: Dict[str, Optional[CacheDataBase]]
        metadata = {} # type: Dict[str, Optional[MetadataType]]
        result = HLSResult()
        duplicate = 0
        info_log = 'config {0}: '.format(cfg_id)
        for c_type in self.cache:
            cache_data = self.cache[c_type].gen_cache_data(
                cfg_id, work_dir)
            if cache_data is not None:
                data[c_type] = cache_data
            else:
                MLOGGER.info('ERROR: Fail to generate cache data (config %d)',
                             cfg_id)
            sub_ref_data = self.cache[c_type].hit(data[c_type])
            if not sub_ref_data:
                info_log += '{0} cache miss; '.format(c_type)
                ref_data[c_type] = None
                metadata[c_type] = None
                continue

            # Generate metadata for the estimator to derive the QoR w/o
            # actual running HLS
            info_log += ('{0} cache hit ({1}), '.format(
                c_type, sub_ref_data.cfg_id))
            try:
                sub_metadata = self.cache[c_type].gen_metadata(
                    self.hls_info_db, sub_ref_data, data[c_type], self.mode)
            except Exception:
                MLOGGER.info('ERROR: Exception when generating %d metadata',
                             cfg_id)
                MLOGGER.info('Info log: %s', info_log)
                MLOGGER.info(traceback.format_exc())

            assert sub_metadata is not None
            assert isinstance(sub_metadata['status'], str)
            if sub_metadata['status'] == 'duplicate':
                # The config is defined as duplicate based on the cache key
                duplicate += 1
                result = deepcopy(self.hls_info_db[sub_ref_data.cfg_id])
                info_log += 'duplicate'
            else:
                info_log += sub_metadata['status']
            info_log += '; '
            ref_data[c_type] = sub_ref_data
            metadata[c_type] = sub_metadata

        if duplicate == len(self.cache):
            MLOGGER.info('%s -> DUPLICATE', info_log)
            result.error_code = HLSRetCode.DUPLICATE
        else:
            MLOGGER.info('%s -> NEW CONFIG', info_log)
            if self.mode != 'HLS-n-model':
                # Remove metadata so that we will always run the HLS
                metadata = {}
        return ref_data, data, metadata, result

    def evaluate(self, cfg_id, work_dir):
        # type: (int, str) -> Tuple[Optional[HLSResult], Optional[str], str]
        """
        The cache interface that estimates the QoR either using
        cached data or third-party HLS tool.
        Return: (QoR result, Log dir, QoR log)
        """
        # Create HLS object and OpenCL kernel
        trans_dir = '{0}/{1}'.format(work_dir, TRANS_DIR)
        hls = DSERunHLS(trans_dir, work_dir,
                        self.hls_time_limit, self.impl_tool)
        result = HLSResult()
        if not hls.is_setup:
            MLOGGER.info('ERROR: HLS setup failed')
            return (None, None, '')

        # Check if kernel is transformed as expected
        failed_list = collect_midend_failures(trans_dir)
        if failed_list:
            result.add_memo('mid_x', ','.join(failed_list))
            keep_dir = hls.clean(self.debug_mode, True)
            return (result, keep_dir,
                    'inf (trans_failed:{0})\t\t\t\t0.0'
                    .format(','.join(failed_list)))

        # Try to generate final OpenCL
        if not hls.gen_opencl():
            MLOGGER.info('ERROR: Fail to generate OpenCL')
            result.error_code = HLSRetCode.MERLIN_ERROR
            keep_dir = hls.clean(self.debug_mode)
            return (result, keep_dir, 'inf (merlin_error)\t\t\t\t0.0')

        # Evaluation
        try:
            ref_data, data, metadata, result = self.hit_cache(cfg_id, work_dir)
            if result.error_code != HLSRetCode.DUPLICATE:
                MLOGGER.info('Launching the estimator for %d', cfg_id)
                hls.run(metadata)
                result = hls.evaluate()
        except Exception:
            MLOGGER.info('ERROR: Exception when evaluating: %d', cfg_id)
            MLOGGER.info(traceback.format_exc())
            return (None, None, '0.0')

        # Update cache and database
        if (result is not None and
                result.error_code == HLSRetCode.EVAL_PASS or
                result.error_code == HLSRetCode.HLS_NO_RESOURCE or
                result.error_code == HLSRetCode.DUPLICATE):
            if result.error_code != HLSRetCode.DUPLICATE:
                self.hls_info_db[cfg_id] = result
            if (result.source == 'est' or
                    result.error_code == HLSRetCode.DUPLICATE):
                self.skip_hls_count += 1
            try:
                for c_type in self.cache:
                    self.cache[c_type].add(cfg_id, data[c_type])
                    if self.mode == 'HLS-n-model':
                        self.cache[c_type].postproc(
                            ref_data[c_type], data[c_type],
                            metadata[c_type], result)
            except Exception as err_msg:
                MLOGGER.info("WARN: Exception when updating: %s", str(err_msg))
                MLOGGER.info(traceback.format_exc())

        # Model accuracy analysis
        if self.mode == 'HLS-n-model':
            hit_cfg_ids = {t: d.cfg_id if d is not None else -1
                           for t, d in ref_data.items()}
            result.model_accuracy = self.compute_model_accuracy(
                hit_cfg_ids, hls)

        log_str = self.gen_result_log(hls, result)
        keep_dir = hls.clean(self.debug_mode)
        return (result, keep_dir, log_str)

    def dump_cache(self, work_dir):
        # type: (str) -> None
        """Dump cache details for analysis"""
        run_command('mkdir -p {0}/cache'.format(work_dir))
        for c_type in self.cache:
            path = '{0}/cache/{1}'.format(work_dir, c_type)
            run_command('mkdir -p {0}'.format(path))
            self.cache[c_type].dump(path, self.hls_info_db)

    def get_cache_hit_counts(self):
        # type: () -> str
        """
        Return a string of hit count of both caches
        """
        return ('; '.join(
            ['{0}: {1}, {2}'.format(
                t, self.cache[t].get_hit_count(),
                self.cache[t].get_skip_hls_count())
             for t in self.cache]))

    def get_skip_hls_count(self):
        # type: () -> int
        """
        Return the number of HLS runs we saved
        """
        return self.skip_hls_count
