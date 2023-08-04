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
from common.fixtures import *

cases = [
#0
(
"""
X_0 (name:a, src_line:1, src_filename:x, interface_bitwidth:8, coalesce_bitwidth:128, burst_read_write:W)
X_1 (name:c, src_line:1, interface_bitwidth:32, table_note:123)
X_1 (name:m, src_line:1, interface_bitwidth:0, table_note:456)
F_0 (name:foo, src_line:1, src_filename:x)
    L_0_0 (name:loop1, table_note:234, CYCLE_UNIT:333)
        X_0_0_0 (name:merlin_memcpy, table_note:345, comment:__merlin_note)
F_1 (name:vvv, src_line:1, src_filename:x, CYCLE_UNIT:123, total-BRAM: 42)
    L_0_0 (name:loop1, table_note:234, CYCLE_UNIT:333)
"""
),
]
def test_separate_json(make_hierarchy_data, make_separate_json):
    json_data = make_hierarchy_data(cases[0])
    separate_json = make_separate_json()
    separate_json.get_summary_json(json_data)
    ref = load_json("report_separate_json_gen/ref_summary.json")
    new = load_json("summary.json")
    assert ref["Summary"]["table"] == new["Summary"]["table"]
    separate_json.get_interface_json(json_data)
    ref = load_json("report_separate_json_gen/ref_interface.json")
    new = load_json("interface.json")
    assert ref == new
    separate_json.get_performance_json(json_data)
    ref = load_json("report_separate_json_gen/ref_performance.json")
    new = load_json("performance.json")
    assert ref == new
    separate_json.get_resource_json(json_data)
    ref = load_json("report_separate_json_gen/ref_resource.json")
    new = load_json("resource.json")
    assert ref == new
