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

Filename    : optimizers/autodse/scripts/dse_objective.py
Description : This is the library for the DSE that is called by
              dse_engine_hls.py for the objective functions.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-24-2017 First pull request (Cody Hao Yu)
"""
# pylint: disable=unused-wildcard-import, wildcard-import
from typing import List, Any, Optional # pylint: disable=unused-import
import logging
import opentuner
from opentuner.resultsdb.models import *
from opentuner.search.objective import SearchObjective

LOG = logging.getLogger(__name__)

class ThresholdResourceMinimizeLatency(SearchObjective):
    """
    if used < 1.0:
      minimize latency
    else:
      minimize resource
    """

    def __init__(self):
        # type: () -> None
        self.high_util_limit_multiplier = 10.0
        super(ThresholdResourceMinimizeLatency, self).__init__()

    def result_order_by_terms(self):
        # type: () -> List[float]
        """return database columns required to order by the objective"""
        # Here should be "max(size, 1.0), time" but seems not working.
        # Current workaround requires the minimized size to be 0.99 when
        # storing the value.
        return [opentuner.resultsdb.models.Result.size,
                opentuner.resultsdb.models.Result.time]

    def result_compare(self, result1, result2):
        # type: (Any, Any) -> int
        """cmp() compatible comparison of resultsdb.models.Result"""
        res1 = (max(1.0, result1.size), result1.time)
        res2 = (max(1.0, result2.size), result2.time)
        if res1[0] != res2[0]:
            return -1 if res1[0] < res2[0] else 1
        return -1 if res1[1] < res2[1] else 0 if res1[1] == res2[1] else 1

    def config_compare(self, config1, config2):
        # type: (Any, Any) -> int
        """cmp() compatible comparison of resultsdb.models.Configuration"""
        result1 = self.driver.results_query(
            config=config1, objective_ordered=True)[0]
        result2 = self.driver.results_query(
            config=config2, objective_ordered=True)[0]
        return self.result_compare(result1, result2)

    def limit_from_config(self, config):
        # type: (Any) -> Optional[float]
        """
        a time limit to kill a result after such that
        it can be compared to config
        """
        results = self.driver.results_query(config=config)
        if results.count() == 0:
            return None

        min_size = float('inf')
        max_time = 0
        for result in results:
            if result.size < min_size:
                min_size = result.size
            if result.time > max_time:
                max_time = result.time

        if min_size >= 1.0:
            mul = self.high_util_limit_multiplier
        else:
            mul = 1.0
        return mul * max_time

    def filter_acceptable(self, query):
        # type: (Any) -> Any
        """Return a Result() query that only returns acceptable results"""
        return query.filter(opentuner.resultsdb.models.Result.size < 1.0)

    def is_acceptable(self, result):
        # type: (Any) -> bool
        """Test if a Result() meets thresholds"""
        return result.size < 1.0

    # pylint: disable=unused-argument
    def result_relative(self, result1, result2):
        # type: (Any, Any) -> None
        """return None, or a relative goodness of resultsdb.models.Result"""
        # unimplemented for now
        LOG.warning('result_relative() not yet implemented for %s',
                    self.__class__.__name__)
