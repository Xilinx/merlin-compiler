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
#!/bin/python
# -*- coding:utf-8 -*-

from __future__ import print_function
import os
import subprocess
import multiprocessing
import argparse
import re
import shutil
from common.llvm_test import MerlinccReport
from common.tools import Counter

class MerlinInnerReport(object):
    def __init__(self, filepath):
        self.filepath = filepath
        self.errors = []
        self.is_crash = os.path.exists(filepath)
        self.backtrace = []
        if os.path.exists(filepath):
          with open(filepath, 'r') as merlin_log:
              lines = merlin_log.readlines()
              for idx, line in enumerate(lines):
                  if re.match(r'".*", line (\d+): error:', line):
                      self.errors.append(lines[idx + 1].strip())
                      self.is_crash = False
                  if 'backtrace' in line:
                      self.backtrace = [l.strip() for l in lines[idx + 1:-3]]

optStep2Pass = { -1: 'mcheck',
                0:'none',
                1:'lower_separate',
                2:'preprocess',
                3:'kernel_wrapper',
                4:'postwrap_process',
                5:'postwrap_process',
                6:'interface_transform',
                7:'midend_preprocess',
                8:'memory_burst',
                9:'delinearization',
                10:'coarse_parallel',
                11:'bitwidth_opt',
                12:'reduction_general',
                13:'coarse_pipeline',
                14:'loop_parallel',
                15:'final_code_gen'}

pass2optStep = {}
for key, value in optStep2Pass.iteritems():
    pass2optStep[value] = key

class CrashCase(object):
    def __init__(self, path):
        assert os.path.isdir(path)
        self.path = path
        files = [f for f in os.listdir(self.path) if f.endswith(('.c', '.cpp'))]
        assert len(files) == 1
        self.src_name = files[0]
        log_path = os.path.join(self.path, '.merlin_prj/run/report/merlin.log')
        if self.isHaveProject():
            self.report = MerlinccReport(log_path)

    def isHaveProject(self):
        return os.path.exists(os.path.join(self.path, '.merlin_prj'))

    def doClean(self):
        prj = os.path.join(self.path, '.merlin_prj')
        print(prj)
        if os.path.isdir(prj):
            shutil.rmtree(prj)
        rpt = os.path.join(self.path, 'merlincc_report')
        print(rpt)
        if os.path.isdir(rpt):
            shutil.rmtree(rpt)

    def getRunningIns(self):
        return subprocess.Popen(['merlincc', self.src_name,
                                 '-c', '--platform=sdaccel::xilinx:adm-pcie-ku3:2ddr:3.3',
                                 '-g', '-d11'], cwd = self.path)

    def getCrashStep(self):
        return self.report.get_optimize_step()

    def isPassed(self):
        return self.report.is_pass()

    def isCrash(self):
        return self.report.is_crash

    def analysisCrashType(self):
        step = self.getCrashStep()
        passName = optStep2Pass[step]
        if step == 15:
            passLogPath = os.path.join(self.path, '.merlin_prj/run/implement/opencl_gen/final_code_gen/run_' + passName + '.log')
        else:
            passLogPath = os.path.join(self.path, '.merlin_prj/run/implement/code_transform/run_' + passName + '.log')
        self.passLog = MerlinInnerReport(passLogPath)
        self.crashModule = passName
        if not self.passLog.is_crash:
            self.crashModule = optStep2Pass[step - 1]

    def isTrueCrash(self):
        return not self.isPassed() and self.passLog.is_crash

def getCrashCases(topPath):
    ret = []
    for path, dirnames, filenames in os.walk(topPath):
        files = [f for f in filenames if f.endswith(('.c', '.cpp'))]
        if len(files) == 1 and '.merlin_prj' not in path:
            p = CrashCase(path)
            ret.append(p)
    return ret

def worker(p):
    if not p.isHaveProject():
        with open(os.path.join(p.path, 'Makefile'), 'w') as f:
            f.write('merlincc {0} -c -p --platform=sdaccel::xilinx:adm-pcie-ku3:2ddr:3.3 -d11 -g'.format(p.src_name))
        ins = p.getRunningIns()
        ins.wait()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--run', action = 'store_true', default = False)
    parser.add_argument('--analysis', action = 'store_true', default = False)
    parser.add_argument('--clean', action = 'store_true', default = False)
    parser.add_argument('--path', default = os.path.realpath('.'))
    # parser.add_argument('--report', action = 'store_true', default = False)
    # parser.add_argument('--condor', action = 'store_true', default = False)
    # parser.add_argument('--path', default = os.path.realpath('.'))
    # parser.add_argument('--small', action = 'store_true', default = False)
    # parser.add_argument('--list', action = 'store_true', default = False)
    args = parser.parse_args()
    if args.run:
        cases = getCrashCases(args.path)
        pool = multiprocessing.Pool()
        pool.map(worker, cases)
        pool.close()
        pool.join()

    if args.clean:
        cases = getCrashCases(args.path)
        for c in cases:
            if os.path.isfile(os.path.join(c.path, 'Makefile')):
                os.remove(os.path.join(c.path, 'Makefile'))
            c.doClean()

    if args.analysis:
        modules = []
        cases = getCrashCases(args.path)
        print_cases = []
        lower_separate_cases = []
        for c in cases:
            if not c.isHaveProject():
                continue
            if not c.isCrash():
                print('{0} is not crash.'.format(c.path))
                continue
            c.analysisCrashType()
            modules.append(c.crashModule)
            print_cases.append(c)
        count = Counter(modules)
        totalCount = sum(count.values())
        for module, count in count.most_common():
            print('Crash at module {0}({1}), {2}, {3:.1%}'.format(module, pass2optStep[module], count, float(count) / totalCount))
        print('Total Crash: {0}'.format(totalCount))
        print('')
        sorted(print_cases, key=lambda c: c.crashModule)
        for c in print_cases:
            print('{0} crash at module {1}'.format(c.path, c.crashModule))
            if not os.path.isdir(c.crashModule):
                os.mkdir(c.crashModule)
            shutil.copyfile(os.path.join(c.path, c.src_name),
                os.path.join(c.crashModule, c.src_name))
            if c.isTrueCrash():
                for line in c.passLog.backtrace:
                    print(line)
            else:
                print(c.passLog.errors)
            print('')

