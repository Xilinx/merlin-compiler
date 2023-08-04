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
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:int, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
"""
),

#1
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_memcpy, org_identifier:X_0_0)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
"""
),

#2
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_memcpy, org_identifier:X_0_0)
    L_0_1 (org_identifier:L_0_0)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
"""
),

#3
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
    X_0_1 (name:merlin_memcpy, org_identifier:X_0_0)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
"""
),

#4
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
    X_0_1 (name:merlin_memcpy, org_identifier:X_0_0)
    X_0_2 (name:__assert_fail, org_identifier:X_INVALID_ID)
    L_0_3 (org_identifier:L_0_1)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
F_2 (name:__assert_fail)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
    L_0_1 (name:null, src_filename:null, src_line:null)
"""
),

#5
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
        X_0_0_0 (name:merlin_memcpy, org_identifier:X_0_0)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
"""
),

#6
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
    L_0_1 (org_identifier:L_0_1)
        X_0_1_0 (name:merlin_memcpy, org_identifier:X_0_0)
    L_0_2 (org_identifier:L_0_2)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
    L_0_1 (name:null, src_filename:null, src_line:null)
    L_0_2 (name:null, src_filename:null, src_line:null)
"""
),

#7
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:X_0_0)
        X_0_0_0 (name:merlin_memcpy, org_identifier:X_0_0)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
"""
),

#8
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
    L_0_1 (org_identifier:X_0_0)
        X_0_1_0 (name:merlin_memcpy, org_identifier:X_0_0)
    L_0_2 (org_identifier:L_0_1)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
    L_0_1 (name:null, src_filename:null, src_line:null)
"""
),

#9
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_func, org_identifier:X_0_0, function_id:F_1)
F_1 (name:merlin_func, org_identifier:X_0_0, inline:no)
    X_1_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_2)
F_2 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
"""
),

#10
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
    X_0_1 (name:merlin_func, org_identifier:X_0_0, function_id:F_1)
    L_0_2 (org_identifier:L_0_1)
F_1 (name:merlin_func, org_identifier:X_0_0, inline:no)
    X_1_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_2)
F_2 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
    L_0_1 (name:null, src_filename:null, src_line:null)
"""
),

#11
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
    X_0_1 (name:merlin_memcpy_0, org_identifier:X_0_0)
    X_0_2 (name:merlin_memcpy_1, org_identifier:X_0_1)
    L_0_3 (org_identifier:L_0_1)
F_1 (name:merlin_memcpy_0, org_identifier:X_0_0, inline:no)
F_2 (name:merlin_memcpy_1, org_identifier:X_0_1, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data0, var_type:array, src_filename:null, src_line:null)
X_0_1 (name:data1, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
    L_0_1 (name:null, src_filename:null, src_line:null)
"""
),

#12
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
    X_0_1 (name:merlin_memcpy_0, org_identifier:X_0_0)
    L_0_2 (org_identifier:L_0_1)
        X_0_2_0 (name:merlin_memcpy_1, org_identifier:X_0_1)
F_1 (name:merlin_memcpy_0, org_identifier:X_0_0, inline:no)
F_2 (name:merlin_memcpy_1, org_identifier:X_0_1, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data0, var_type:array, src_filename:null, src_line:null)
X_0_1 (name:data1, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
    L_0_1 (name:null, src_filename:null, src_line:null)
"""
),

#13
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
        X_0_0_0 (name:merlin_memcpy, org_identifier:X_0_0)
        L_0_0_1 (org_identifier:L_0_0)
            L_0_0_1_0 (org_identifier:L_0_0_1)
        X_0_0_2 (name:bar, org_identifier:X_0_0_0)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
F_2 (name:bar, org_identifier:F_1)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
        X_0_0_0 (name:bar, src_filename:null, src_line:null)
        L_0_0_1 (name:null, src_filename:null, src_line:null)
F_1 (name:bar, src_filename:null, src_line:null)
"""
),

#14
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_func, org_identifier:X_0_0, function_id:F_1)
    X_0_1 (name:merlin_func, org_identifier:X_0_0, function_id:F_1)
F_1 (name:merlin_func, org_identifier:X_0_0, inline:no)
    X_1_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_2)
F_2 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
"""
),

#15
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_func, org_identifier:X_0_0, function_id:F_1)
    X_0_1 (name:merlin_func, org_identifier:X_0_0, function_id:F_1)
F_1 (name:merlin_func, org_identifier:X_0_0, inline:no)
    L_1 (name:merlin_loop, org_identifier:X_0_0)
        X_1_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_2)
F_2 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
"""
),

#16
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
    L_0_1 (name:merlin_loop, org_identifier:L_0_0)
        X_0_1_1 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
        L_0_1_2 (name:merlin_loop, org_identifier:L_0_0)
            X_0_1_2_4 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
            L_0_1_2_5 (name:merlin_loop, org_identifier:L_0_0)
                X_0_1_2_5_6 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
                L_0_1_2_5_7 (name:merlin_loop, org_identifier:L_0_0_1)
                    X_0_1_2_5_7_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
                X_0_1_2_5_8 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
            X_0_1_2_6 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
        X_0_1_3 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
    X_0_2 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_1)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
        L_0_0_1 (name:null, src_filename:null, src_line:null)
"""
),

#17
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    X_0_0 (name:merlin_func, org_identifier:X_0_0, function_id:F_1)
F_1 (name:merlin_func, org_identifier:L_0_0, inline:no)
    X_1_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_2)
F_2 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
"""
),

#18
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
F_1 (name:merlin_func, org_identifier:X_0_0, inline:no)
    X_1_0 (name:merlin_memcpy, org_identifier:X_0_0, function_id:F_2)
F_2 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
"""
),
#19
(
# merlin output code
"""
F_0 (name:foo, org_identifier:F_0)
    L_0_0 (org_identifier:L_0_0)
        X_0_0_0 (name:merlin_memcpy, org_identifier:X_0_0)
        L_0_0_2 (org_identifier:L_0_0_2, flatten_loop:on)
F_1 (name:merlin_memcpy, org_identifier:X_0_0, inline:no)
F_2 (name:bar, org_identifier:F_1)
""",

# merlin input code
"""
X_0_0 (name:data, var_type:array, src_filename:null, src_line:null)
F_0 (name:foo, src_filename:null, src_line:null)
    L_0_0 (name:null, src_filename:null, src_line:null)
        L_0_0_1 (name:null, src_filename:null, src_line:null, parent_loop:L_0_0)
            L_0_0_2 (name:null, src_filename:null, src_line:null, parent_loop:L_0_0_1)
F_1 (name:bar, src_filename:null, src_line:null)
"""
),

]

def test_method_find_merlin_memcpy_calls_01(make_memory_burst):
    memb = make_memory_burst(*cases[0])
    interface = {"src_topo_id":"X_0_0", "name": "data", "var_type":"int"}
    res = memb.find_merlin_memcpy_calls(interface)
    assert res == []

def test_method_find_merlin_memcpy_calls_02(make_memory_burst):
    memb = make_memory_burst(*cases[1])
    interface = {"src_topo_id":"X_0_0", "name": "data", "var_type":"array"}
    res = memb.find_merlin_memcpy_calls(interface)
    assert res == [{'type': 'callfunction', 'org_identifier': 'X_0_0',
                    'topo_id': 'X_0_0', 'name': 'merlin_memcpy'}]

def test_method_locate_merlin_memcpy_call_site_03(make_memory_burst):
    memb = make_memory_burst(*cases[1])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_0"] == (None, None)

def test_method_locate_merlin_memcpy_call_site_04(make_memory_burst):
    memb = make_memory_burst(*cases[2])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_0"] == (None, 'L_0_0')

def test_method_locate_merlin_memcpy_call_site_05(make_memory_burst):
    memb = make_memory_burst(*cases[3])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_1"] == ('L_0_0', None)

def test_method_locate_merlin_memcpy_call_site_06(make_memory_burst):
    memb = make_memory_burst(*cases[4])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_1"] == ('L_0_0', 'L_0_1')

def test_method_locate_merlin_memcpy_call_site_07(make_memory_burst):
    memb = make_memory_burst(*cases[5])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_0_0"] == (None, None)

def test_method_locate_merlin_memcpy_call_site_08(make_memory_burst):
    memb = make_memory_burst(*cases[6])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_1_0"] == ('L_0_0', 'L_0_2')

def test_method_locate_merlin_memcpy_call_site_09(make_memory_burst):
    memb = make_memory_burst(*cases[7])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_0_0"] == (None, None)

def test_method_locate_merlin_memcpy_call_site_10(make_memory_burst):
    memb = make_memory_burst(*cases[8])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_1_0"] == ('L_0_0', 'L_0_1')

def test_method_locate_merlin_memcpy_call_site_11(make_memory_burst):
    memb = make_memory_burst(*cases[9])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_1_0"] == (None, None)

def test_method_locate_merlin_memcpy_call_site_12(make_memory_burst):
    memb = make_memory_burst(*cases[10])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_1_0"] == ('L_0_0', 'L_0_1')

def test_method_locate_merlin_memcpy_call_site_13(make_memory_burst):
    memb = make_memory_burst(*cases[11])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_1"] == ('L_0_0', 'L_0_1')

    call = {'type': 'callfunction', 'org_identifier': 'X_0_1',
            'topo_id': 'X_0_2'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_2"] == ('L_0_0', 'L_0_1')

def test_method_locate_merlin_memcpy_call_site_14(make_memory_burst):
    memb = make_memory_burst(*cases[12])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_1"] == ('L_0_0', 'L_0_1')

    call = {'type': 'callfunction', 'org_identifier': 'X_0_1',
            'topo_id': 'X_0_2_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_2_0"] == ('L_0_0', None)

def test_method_locate_merlin_memcpy_call_site_15(make_memory_burst):
    memb = make_memory_burst(*cases[13])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_0_0"] == (None, 'L_0_0_1')

def test_method_locate_merlin_memcpy_call_site_16(make_memory_burst):
    memb = make_memory_burst(*cases[13])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy == {}

def test_method_locate_merlin_memcpy_call_site_17(make_memory_burst):
    memb = make_memory_burst(*cases[14])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_1_0'] == (None, None)
    assert memb.parent_merlin_memcpy['X_1_0'] == "F_0"

def test_method_locate_merlin_memcpy_call_site_18(make_memory_burst):
    memb = make_memory_burst(*cases[15])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_1_0'] == (None, None)
    assert memb.parent_merlin_memcpy['X_1_0'] == 'F_0'

def test_method_locate_merlin_memcpy_call_site_19(make_memory_burst):
    memb = make_memory_burst(*cases[17])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_1_0'] == (None, None)
    assert memb.parent_merlin_memcpy['X_1_0'] == "F_0"

def test_method_locate_merlin_memcpy_call_site_20(make_memory_burst):
    memb = make_memory_burst(*cases[18])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_1_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy == {}
    assert memb.parent_merlin_memcpy == {}

def test_method_locate_merlin_memcpy_call_site_21(make_memory_burst):
    memb = make_memory_burst(*cases[19])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy["X_0_0_0"] == (None, 'L_0_0_1')

def test_get_interface_info(make_memory_burst_from_json):
    vendor_data = load_json("input_set/test_get_interface_info_vendor_data.json")
    hier_topo_data = load_json("input_set/test_get_interface_info_hier_topo.json")
    topo_info_data = load_json("input_set/test_get_interface_info_topo_info.json")
    memb = make_memory_burst_from_json(vendor_data, hier_topo_data, topo_info_data)
    interface = {"src_topo_id": "abc"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst"] == "-"
    assert output_info["burst_read_write"] == "-"
    assert output_info["coalesce"] == "-"
    assert output_info["coalesce_bitwidth"] == "-"
    assert output_info["comment_interface"] == "-"
    assert output_info["table_note"] == "-"
    interface = {"src_topo_id": "X_0_0_0_47_0_1"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst"] == "on"
    assert output_info["burst_read_write"] == "R/W"
    assert output_info["coalesce"] == "on"
    assert output_info["coalesce_bitwidth"] == "128"
    assert output_info["comment_interface"] == "cached & coalesced"
    assert output_info["table_note"] == ""
    interface = {"src_topo_id": "X_0_0_0_47_0_0"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst"] == "on"
    assert output_info["burst_read_write"] == "RO"
    assert output_info["coalesce"] == "off"
    assert output_info["coalesce_bitwidth"] == "-"
    assert output_info["comment_interface"] == "cached__merlin_note"
    #assert "Memory coalescing NOT inferred:" in output_info["table_note"]
    interface = {"src_topo_id": "X_0_0_0_47_0_2"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst"] == "on"
    assert output_info["burst_read_write"] == "WO"
    assert output_info["coalesce"] == "on"
    assert output_info["coalesce_bitwidth"] == "128"
    assert output_info["comment_interface"] == "cached & coalesced"
    assert output_info["table_note"] == ""
    interface = {"src_topo_id": "X_0_0_0_47_0_3"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst"] == "-"
    assert output_info["burst_read_write"] == "WO"
    assert output_info["coalesce"] == "on"
    assert output_info["coalesce_bitwidth"] == "128"
    assert output_info["comment_interface"] == "coalesced"
    assert output_info["table_note"] == ""
    interface = {"src_topo_id": "X_0_0_0_47_0_4"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst"] == "-"
    assert output_info["burst_read_write"] == "WO"
    assert output_info["coalesce"] == "-"
    assert output_info["coalesce_bitwidth"] == "-"
    assert output_info["comment_interface"] == "scalar"
    assert output_info["table_note"] == ""
    interface = {"src_topo_id": "X_0_0_0_47_0_5"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst"] == "-"
    assert output_info["burst_read_write"] == "WO"
    assert output_info["coalesce"] == "off"
    assert output_info["coalesce_bitwidth"] == "-"
    assert output_info["comment_interface"] == "-"
    assert output_info["table_note"] == ""
    interface = {"src_topo_id": "X_0_0_0_47_0_6"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst_read_write"] == "RO"
    assert output_info["comment_interface"] == "streamed"
    interface = {"src_topo_id": "X_0_0_0_47_0_7"}
    output_info = memb.get_interface_info(interface)
    assert output_info["burst_read_write"] == "WO"
    assert output_info["comment_interface"] == "streamed & coalesced"

def test_interchange_memcpy(make_memory_burst):
    memb = make_memory_burst(*cases[13])
    memb.location_merlin_memcpy = {}
    memb.interchange_memcpy()
    assert memb.location_merlin_memcpy_src == {}
    memb.location_merlin_memcpy = {"X_0": ("L_1", "X_2")}
    memb.interchange_memcpy()
    assert memb.location_merlin_memcpy_src == {'L_1': {'after': ['X_0']},
                                               'X_2': {'before': ['X_0']}}
    memb.location_merlin_memcpy = {"X_0": ("L_1", "X_2"),
                                   "X_3": ("X_2", "L_1"),
                                   "X_4": (None, None),
                                   "X_5": ("L_2", None),
                                   "X_6": (None, "L_3"),
                                   "X_8": ("X_2", "L_3"),
                                   "X_7": ("L_2", None)}
    memb.interchange_memcpy()
    assert memb.location_merlin_memcpy_src == {'L_1': {'after': ['X_0'], 'before': ['X_3']},
                                               'X_2': {'after': ['X_3', 'X_8'], 'before': ['X_0']},
                                               'L_2': {'after': ['X_5', 'X_7']},
                                               'L_3': {'before': ['X_6', 'X_8']}}

def test_interchange_memcpy_1(make_memory_burst):
    memb = make_memory_burst(*cases[15])
    memb.location_merlin_memcpy = {'X_1_0': (None, None)}
    memb.parent_merlin_memcpy = {'X_1_0':'F_0'}
    memb.interchange_memcpy()
    assert memb.location_merlin_memcpy_src == {}
    assert memb.parent_merlin_memcpy_src == {'F_0': ['X_1_0']}

def test_method_locate_merlin_memcpy_call_site_and_interchange_memcpy(make_memory_burst):
    memb = make_memory_burst(*cases[16])
    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy == {'X_0_0': (None, 'L_0_0')}
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_2'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_2'] == ('L_0_0', None)
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_1'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_1_1'] == (None, 'L_0_0_1')
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_3'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_1_3'] == ('L_0_0_1', None)
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_2_4'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_1_2_4'] == (None, 'L_0_0_1')
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_2_6'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_1_2_6'] == ('L_0_0_1', None)
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_2_5_6'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_1_2_5_6'] == (None, 'L_0_0_1')
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_2_5_8'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_1_2_5_8'] == ('L_0_0_1', None)
    assert memb.parent_merlin_memcpy == {}

    call = {'type': 'callfunction', 'org_identifier': 'X_0_0',
            'topo_id': 'X_0_1_2_5_7_0'}
    memb.locate_merlin_memcpy_call_site(call)
    assert memb.location_merlin_memcpy['X_0_1_2_5_7_0'] == (None, None)
    assert memb.parent_merlin_memcpy['X_0_1_2_5_7_0'] == 'L_0_0_1'

    memb.interchange_memcpy()
    assert memb.location_merlin_memcpy_src ==  {'L_0_0': {'after': ['X_0_2'], 'before': ['X_0_0']},
             'L_0_0_1': {'after': ['X_0_1_3', 'X_0_1_2_6', 'X_0_1_2_5_8'],
                         'before': ['X_0_1_1', 'X_0_1_2_4', 'X_0_1_2_5_6']}}
    assert memb.parent_merlin_memcpy_src == {'L_0_0_1': ['X_0_1_2_5_7_0']}

def test_get_before_and_aftera_list(make_memory_burst):
    memb = make_memory_burst(*cases[13])
    memb.location_merlin_memcpy_src = {'L_1': {'after': ['X_0'], 'before': ['X_3']},
                                       'X_2': {'after': ['X_3', 'X_8'], 'before': ['X_0']},
                                       'L_2': {'after': ['X_5', 'X_7']},
                                       'L_3': {'before': ['X_6', 'X_8']}}
    print(memb.location_merlin_memcpy_src)
    (before_list, after_list) = memb.get_before_and_after_list("L_1")
    assert before_list == ["X_3"]
    assert after_list == ["X_0"]
    (before_list, after_list) = memb.get_before_and_after_list("L_2")
    assert before_list == []
    assert after_list == ["X_5", "X_7"]
    (before_list, after_list) = memb.get_before_and_after_list("L_3")
    assert before_list == ["X_6", "X_8"]
    assert after_list == []
    (before_list, after_list) = memb.get_before_and_after_list("L_10")
    assert before_list == []
    assert after_list == []
    memb.inserted = ["X_3", "X_0"]
    (before_list, after_list) = memb.get_before_and_after_list("L_1")
    assert before_list == []
    assert after_list == []
