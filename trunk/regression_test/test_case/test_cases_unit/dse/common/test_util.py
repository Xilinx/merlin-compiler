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
from __future__ import print_function
import os
import sys
import time

def exit_test(code):
    sys.exit(code)

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def test(msg, expect, actual, not_equal=False, exit_when_failed=True):
    if not_equal:
        if expect == actual:
            eprint('check failed: "{0}" Cannot be {1}'
                    .format(msg, expect))
            sys.stdout.flush()
            if exit_when_failed:
                exit_test(1)
            return False
    else:
        if expect != actual:
            eprint('check failed: "{0}" Expected {1} but {2}'
                    .format(msg, expect, actual))
            sys.stdout.flush()
            if exit_when_failed:
                exit_test(1)
            return False
    print('{0} check OK'.format(msg))
    sys.stdout.flush()
    return True
