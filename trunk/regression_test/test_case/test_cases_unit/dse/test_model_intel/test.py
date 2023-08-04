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
#! /usr/bin/python
# -*- coding:utf-8 -*-

from __future__ import print_function
import os
import sys
import glob

sys.path.append(os.path.join(os.environ['MARS_TEST_HOME_GIT'], 'script/llvm_test/common/'))

from marstest import CommandlineTestCase
import os
import subprocess

class Test(CommandlineTestCase):
    def run(self):

        self.run_command("cd intel_dse && make mcc_acc")
        self.run_command("cp intel_dse/.merlin_prj/run/implement/dse/dse_report/report0/history.log .")
        self.run_command("diff history.log history.log.ref > err.log")
        

if __name__ == '__main__':
    t = Test()
    t.run()
