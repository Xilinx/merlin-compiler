#(C) Copyright 2016-2021 Xilinx, Inc.
#All Rights Reserved.
#
#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License. (edited)
from common.generate_hierarchy import parse_from_text
import json
import os
import pytest
import sys
from collections import OrderedDict

def dump_to_json(json_file, content, mode="default"):
    '''
    dump json file
    '''
    with open(json_file, 'w') as fileh:
        if mode == "default":
            json.dump(content, fileh, sort_keys=True, indent=4)
        elif mode == "nosort":
            json.dump(content, fileh, sort_keys=False, indent=4)

def load_json(json_file, mode="default"): 
    data_set = {}
    with open(json_file, 'r') as fileh:
        if mode == "default":
            data_set = json.load(fileh, object_pairs_hook=OrderedDict)
        return data_set
    return data_set

assert os.getenv("MERLIN_COMPILER_HOME") is not None
path = os.path.join(
    os.getenv("MERLIN_COMPILER_HOME"),
    "mars-gen", "scripts", "msg_report", "xilinx_enhance"
)
sys.path.insert(0, path)

@pytest.fixture(scope="session")
def logger():
    from merlin_log import MerlinLog
    log = MerlinLog()
    log.set_merlin_log()
    return log

@pytest.fixture(scope="session")
def settings():
    from merlin_report_setting import MerlinReportSetting
    return MerlinReportSetting()

@pytest.fixture(scope="session")
def make_topo_info_data():
    def make(case):
        return parse_from_text(case, flatten=False, cpp_type="dst")
    return make

@pytest.fixture(scope="session")
def make_vendor_data():
    def make(case):
        return parse_from_text(case, flatten=True, cpp_type="dst")
    return make

@pytest.fixture(scope="session")
def make_hierarchy_data():
    def make(case):
        return parse_from_text(case, flatten=False, cpp_type="src")
    return make

@pytest.fixture(scope="session")
def make_nest_info(settings, logger, make_topo_info_data, make_vendor_data):
    from json_parser import Topo
    from json_parser import NestedLevel
    def make(case, inlining):
        topo_info = Topo(settings, logger, make_vendor_data(case), make_vendor_data(case))
        function_for_name = topo_info.index_function(settings.attr_name)
        return NestedLevel(
            settings, logger, make_topo_info_data(case), function_for_name, make_vendor_data(case), inlining
        )
    return make

@pytest.fixture(scope="session")
def make_schedule(settings, logger, make_nest_info, make_vendor_data):
    from json_parser import Schedule
    def make(case, case_adb):
        return Schedule(
            settings,
            logger,
            json.loads(case_adb),
            make_nest_info(case, True),
            make_vendor_data(case)
        )
    return make

@pytest.fixture(scope="session")
def make_memory_burst(settings, logger, make_topo_info_data,
                      make_vendor_data, make_hierarchy_data):
    from json_parser import HierTopo
    from json_parser import MemoryBurst
    from json_parser import NestedLevel
    from json_parser import Topo
    def make(case, case_hier):
        topo_info = Topo(settings, logger, make_vendor_data(case), make_hierarchy_data(case_hier))
        hier_topo_info = HierTopo(settings, logger,
                                  make_hierarchy_data(case_hier))
        function_for_name = topo_info.index_function(settings.attr_name)
        nest_info = NestedLevel(
            settings, logger, make_topo_info_data(case), function_for_name, True
        )
        return MemoryBurst(logger, settings, topo_info,
                           hier_topo_info, nest_info)
    return make

@pytest.fixture(scope="session")
def make_memory_burst_from_json(settings, logger):
    from json_parser import HierTopo
    from json_parser import MemoryBurst
    from json_parser import NestedLevel
    from json_parser import Topo
    def make(vendor_data, hier_topo_data, topo_info_data):
        topo_info = Topo(settings, logger, vendor_data, hier_topo_data)
        hier_topo_info = HierTopo(settings, logger, hier_topo_data)
        function_for_name = topo_info.index_function(settings.attr_name)
        nest_info = NestedLevel(settings, logger, topo_info_data, function_for_name, True)
        return MemoryBurst(logger, settings, topo_info, hier_topo_info, nest_info)
    return make

@pytest.fixture(scope="session")
def make_topo(settings, logger, make_vendor_data, make_hierarchy_data):
    from json_parser import Topo
    def make(case1, case2):
        return Topo(settings, logger, make_vendor_data(case1), make_hierarchy_data(case2))
    return make

@pytest.fixture(scope="session")
def make_topo_from_json(settings, logger):
    from json_parser import Topo
    def make(json_vendor_data, json_hier_data):
        return Topo(settings, logger, json_vendor_data, json_hier_data)
    return make

@pytest.fixture(scope="session")
def make_hiertopo_from_json(settings, logger):
    from json_parser import HierTopo
    def make(json_hier_data):
        return HierTopo(settings, logger, json_hier_data)
    return make

@pytest.fixture(scope="session")
def make_separate_json(logger, settings):
    from separate_json_gen_top import OutputJsonGeneration
    def make():
        return OutputJsonGeneration(logger, settings)
    return make

@pytest.fixture(scope="session")
def make_merlin_report_top():
    from merlin_report_top import merlin_report_top
    def make(src, dst, xml, metadata, report):
        return merlin_report_top(src, dst, xml, metadata, report)
    return make

@pytest.fixture(scope="session")
def make_data_extraction():
    import data_extraction
    def make():
        return data_extraction.main()
#        return DataExtraction(src, dst, xml, metadata, report)
    return make

