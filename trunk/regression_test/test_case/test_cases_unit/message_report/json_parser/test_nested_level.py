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
"""#0
F_0 (name:foo)
    X_0_0 (name:bar)
F_1 (name:bar)
    L_1_0 ()
        X_1_0_0 (name:foo)
    X_1_1 (name:foobar)
F_2 (name:foobar)
""",

"""#1
F_0 (name:foo)
    X_0_0 (name:bar)
F_1 (name:bar, inline:yes)
    L_1_0 ()
""",

"""#2
F_0 (name:foo)
    X_0_0 (name:bar)
    X_0_1 (name:foobar)
F_1 (name:bar, inline:no)
is_skipped_node ()
""",

"""#3
F_0 (name:foo)
    X_0_0 (name:bar)
F_1 (name:bar)
    L_1_0 ()
        L_1_0_0 ()
            L_1_0_0_0 ()
                X_1_0_0_0_0 (name:foo)
    X_1_1 (name:foobar)
F_2 (name:foobar)
""",

"""#4
F_0 (name:foo)
    X_0_0 (name:bar)
    L_0_0 (dead_code:yes)
F_1 (name:bar, inline:yes)
    L_1_0 ()
""",
"""#5
F_0 (name:foo)
    X_0_0 (name:bar, dead_code:yes)
    L_0_0 
F_1 (name:bar, inline:yes)
    L_1_0 ()
""",
]

def test_method_find_all_functions_01(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    assert sorted(nest_info.find_all_functions()) == ['F_0', 'F_1', 'F_2']

def test_method_find_all_functions_02(make_nest_info):
    nest_info = make_nest_info(cases[2], inlining=True)
    assert sorted(nest_info.find_all_functions()) == ['F_0', 'F_1']

def test_method_find_childs_01(make_nest_info):
    nest_info = make_nest_info(cases[1], inlining=True)
    assert nest_info.find_childs("is_skipped_node", 1) == []

def test_method_find_childs_02(make_nest_info):
    nest_info = make_nest_info(cases[1], inlining=True)
    assert nest_info.find_childs("F_0", 1) == [['L_1_0', []]]

def test_method_find_childs_03(make_nest_info):
    nest_info = make_nest_info(cases[1], inlining=False)
    assert nest_info.find_childs("F_0", 1) == ['X_0_0']

def test_method_find_childs_04(make_nest_info):
    nest_info = make_nest_info(cases[4], inlining=False)
    res = nest_info.find_childs("F_0", 0)
    assert res == [[None, []]]

def test_method_find_childs_05(make_nest_info):
    nest_info = make_nest_info(cases[5], inlining=False)
    res = nest_info.find_childs("F_0", 0)
    assert res == [[['L_0_0', []]]]

def test_method_find_child_ids_01(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    assert sorted(nest_info.find_child_ids('F_1')) == ['L_1_0', 'X_1_1']

def test_method_find_child_ids_02(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    assert nest_info.find_child_ids('is_skipped_node') == []

def test_method_find_child_ids_03(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    nest_info._map = {'F_2': [['3'], []]}
    assert nest_info.find_child_ids('F_2') == ['3']
        

def test_method_get_scope_id_01(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    assert nest_info.get_scope_id('L_1_0') == 'F_1'

def test_method_get_scope_id_02(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    assert nest_info.get_scope_id('X_1_0_0') == 'L_1_0'

def test_method_get_scope_id_03(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    assert nest_info.get_scope_id('is_skipped_node') is None

def test_method_get_scope_id_04(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    nest_info._map = {'F_2': [['3'], []]}
    assert nest_info.get_scope_id('F_2') is None

def test_method_filter_outermost_01(make_nest_info):
    nest_info = make_nest_info(cases[0], inlining=False)
    assert sorted(nest_info.filter_outermost(
                    ['X_1_1', 'X_1_0_0', 'L_1_0']
                 )) == ['L_1_0', 'X_1_1']

"""#3
F_0 (name:foo)
    X_0_0 (name:bar)
F_1 (name:bar)
    L_1_0 ()
        L_1_0_0 ()
            L_1_0_0_0 ()
                X_1_0_0_0_0 (name:foo)
    X_1_1 (name:foobar)
F_2 (name:foobar)
""",
def test_method_filter_outermost_02(make_nest_info):
    nest_info = make_nest_info(cases[3], inlining=False)
    assert sorted(nest_info.filter_outermost(
                    ['X_1_1', 'X_1_0_0_0_0', 'L_1_0', 'L_1_0_0']
                 )) == ['L_1_0', 'X_1_1']

def test_method_filter_outermost_03(make_nest_info):
    nest_info = make_nest_info(cases[3], inlining=False)
    nest_info._map = {'X_1_1': [['3'], []]}
    assert sorted(nest_info.filter_outermost(
                    ['X_1_1', 'X_1_0_0', 'L_1_0']
                 )) == ['L_1_0', 'X_1_0_0', 'X_1_1']

def test_method_get_top_scope_id_01(make_nest_info):
    nest_info = make_nest_info(cases[3], inlining=False)
    assert nest_info.get_top_scope_id('X_1_0_0_0_0') == 'F_1'

def test_method_get_top_scope_id_02(make_nest_info):
    nest_info = make_nest_info(cases[3], inlining=False)
    assert nest_info.get_top_scope_id('is_skipped_node') == None

def test_method_get_top_scope_id_03(make_nest_info):
    nest_info = make_nest_info(cases[3], inlining=False)
    assert nest_info.get_top_scope_id('F_0') == None
