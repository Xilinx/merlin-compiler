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
(#0
"""
F_0 (name:foo)
    X_0_0 (name:bar)
    X_0_1 (name:foobar)
F_1 (name:bar)
    L_1_0 ()
        X_1_0_0 (name:foobar)
    X_1_1 (name:foobar)
F_2 (name:foobar)
""",

"""
{
    "foo":{
        "2":"bar"
    },
    "bar":{
        "10":"foo foobar"
    },
    "foobar":{
    }
}
"""
),

]
case = (
"""
F_0 (name:foo)
    X_0_0 (name:bar)
    X_0_1 (name:foobar)
F_1 (name:bar)
    L_1_0 ()
        X_1_0_0 (name:foobar)
    X_1_1 (name:foobar)
F_2 (name:foobar)
"""
)
def relax(ls):
    return set((tuple(sorted(x)) if isinstance(x, list) else x for x in ls))

def test_method_get_sched_info_01(make_schedule, make_nest_info):
    sched_info = make_schedule(*cases[0])
    nest_info = make_nest_info(case, True),
    res = sched_info.get_sched_info(nest_info, ["X_0_0", "X_1_0_0", "X_1_1"])
    assert relax(res) == relax([['X_0_0'], ['X_1_0_0', 'X_1_1']])

def test_method_get_sched_info_02(make_schedule, make_nest_info):
    sched_info = make_schedule(*cases[0])
    nest_info = make_nest_info(case, True),
    assert sched_info.get_sched_info(nest_info, []) == []

def test_method_get_sched_info_03(make_schedule, make_nest_info):
    sched_info = make_schedule(*cases[0])
    nest_info = make_nest_info(case, True),
    res =  sched_info.get_sched_info(nest_info, ["wrong_id", "X_1_1", "X_1_0_0"])
    assert relax(res) == relax([["wrong_id"], ["X_1_1", "X_1_0_0"]])

def test_method_get_sched_info_04(make_schedule, make_nest_info):
    sched_info = make_schedule(*cases[0])
    nest_info = make_nest_info(case, True),
    res =  sched_info.get_sched_info(nest_info, ["wrong_id", "X_2_0", "X_2_1"])
    assert relax(res) == relax([["wrong_id"], ["X_2_0"], ["X_2_1"]])
