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
import subprocess
import re
import getpass
import stat
import time
import shutil
import multiprocessing
import time

class RoseErrorLog(object):
    def __init__(self, filepath):
        pass

    def get_errors(self):
        pass

class MerlinccReport(object):
    def __init__(self, filepath):
        self.filepath = filepath
        self.fatal_errors = []
        self.is_syntax_check_fail = False
        self.optimization_step = -1
        self.errors = []
        self.is_crash = False
        with open(filepath, 'r') as merlincc_log:
            lines = merlincc_log.readlines()
            # for line in lines:
            #     m = re.match(r'ERROR:\s*step=(\d+)\s+\S+\s+failed.', line)
            #     if m:
            #         self.is_crash = True
            #         self.optimization_step = int(m.group(1))
            for line in lines:
                fatal_error_tag = 'fatal error:'
                if fatal_error_tag in line:
                    info = line[line.find(fatal_error_tag) + len(fatal_error_tag) + 1:]
                    info = info.strip()
                    self.fatal_errors.append(info)
            for line in lines:
                if 'ERROR: Source code fails syntax check.' in line:
                    self.is_syntax_check_fail = True
            optimization_step = -1
            end_of_log = False
            for line in lines:
                if line.startswith('Starting optimization step'):
                    optimization_step += 1
                # add reg-exp match step 0 error info
                m = re.search(r'.*:\d+: error: (.*)$', line)
                if m:
                    self.errors.append(('syntax error', m.group(1)))
                m = re.match(r'%E\s+(\[.*?\])\s*(.*)', line)
                if m:
                    self.errors.append((m.group(1).strip(), m.group(2).strip()))
                if not end_of_log:
                    end_of_log = 'ERROR: Merlin flow stopped with error(s).' in line
            self.optimization_step = optimization_step
            if end_of_log and not self.errors:
                self.is_crash = True

    def get_optimize_step(self):
        return self.optimization_step

    def get_fatal_errors(self):
        return self.fatal_errors

    def get_errors(self):
        return self.errors

    def dump(self):
        print('path: ', self.filepath)
        if self.is_pass():
            print('pass')
        else:
            if self.is_crash:
                print('crash')
            elif self.get_fatal_errors():
                print('fatal error')
            elif self.get_errors():
                print('error')
            elif self.is_syntax_check_fail:
                print('syntx error')
            else:
                print('unknow error')

    def get_state(self):
        if self.is_pass():
            return 'Pass'
        elif self.is_crash:
            return 'Crash'
        elif self.is_syntax_check_fail:
            return 'Syntax error'
        else:
            return 'Limitation'

    def is_pass(self):
        return (not self.is_crash and 
                len(self.get_fatal_errors()) == 0 and 
                len(self.get_errors()) == 0 and 
                not self.is_syntax_check_fail)

class LLVMTestBase(object):
    def __init__(self, dst_root, filename, platform = 'xilinx'):
        assert os.path.isabs(dst_root) and os.path.isdir(dst_root), dst_root
        self.dst_root = dst_root
        self.filename = filename
        self.platform = platform
        self.project_dir = dst_root
        self.work_dir = os.path.join(self.project_dir, 'run')
        self.gcc_log = 'run/gcc_error.log'
        self.rose_log = 'run/rose_error.log'
        self.condor_dir = os.path.join(self.project_dir, 'run.condor_output')

class LLVMTestCaseReport(LLVMTestBase):
    def __init__(self, dst_root, filename, platform = 'xilinx'):
        super(LLVMTestCaseReport, self).__init__(dst_root, filename, platform)
        self.sub_dir_names = []
        if os.path.exists(self.work_dir) and os.path.isdir(self.work_dir):
            self.sub_dir_names = [d for d in os.listdir(self.work_dir) 
                    if os.path.isdir(os.path.join(self.work_dir, d))]
        self.merlincc_logs = {}
        for name in self.sub_dir_names:
            merlincc_log_path = os.path.join(self.work_dir, name, 'merlin.log')
            if not os.path.isfile(merlincc_log_path):
                continue
            self.merlincc_logs[name] = MerlinccReport(merlincc_log_path)
        self._cache = {}

    def is_gen_kernel_success(self):
        return len(self.sub_dir_names) > 0

    def get_crash_count(self):
        crash_count = 0
        for p in self.merlincc_logs.values():
            if p.is_crash:
                crash_count += 1
        return crash_count

    def is_start(self):
        return (os.path.exists(os.path.join(self.condor_dir, 'mars_cloud_start.o')) or
                os.path.exists(self.work_dir))

    def is_finish(self):
        finish_tag_file = os.path.join(self.condor_dir, 'mars_cloud_done.o')
        if os.path.isfile(finish_tag_file):
            with open(finish_tag_file, 'r') as f:
                lines = f.readlines()
                return len(lines) >= 2
        # error process
        if not os.path.isdir(self.work_dir):
            return False
        for d in os.listdir(self.work_dir):
            if not os.path.isdir(os.path.join(self.work_dir, d)):
                continue
            if not os.path.isfile(os.path.join(self.work_dir, d, 'merlincc_error.log')):
                return False
        return True

    def get_execution_time(self):
        if not self.is_finish():
            return 0
        start_time = os.path.getctime(os.path.join(self.condor_dir, 'mars_cloud_start.o'))
        finish_time = os.path.getctime(os.path.join(self.condor_dir, 'mars_cloud_done.o'))
        return finish_time - start_time

    def is_skiped(self):
        return not self.is_rose_pass() or len(self.merlincc_logs) == 0

    def is_pass(self):
        return self.is_rose_pass() and all([self.is_merlincc_pass(d) for d in self.sub_dir_names])

    def is_rose_pass(self):
        if self._cache.has_key('rose_pass'):
            return self._cache['rose_pass']
        if not os.path.isfile(os.path.join(self.project_dir, self.rose_log)):
            return False
        if os.path.exists(os.path.join(self.project_dir, self.rose_log)):
            with open(os.path.join(self.project_dir, self.rose_log), 'r') as f:
                for line in f:
                    if re.match(r'Errors in Processing Input File:.+syntax errors.+', line):
                        self._cache['rose_pass'] = False
                        return False
            self._cache['rose_pass'] = True
            return True
        else:
            return False

    def is_merlincc_pass(self, sub_dir_name):
        p = os.path.join(self.work_dir, sub_dir_name, '__merlinkernel_top.h')
        return os.path.exists(p)

    def is_gcc_compile_pass(self):
        if not os.path.isfile(os.path.join(self.project_dir, self.gcc_log)):
            return False
        with open(os.path.join(self.project_dir, self.gcc_log), 'r') as err_log:
            for line in err_log:
                if line.find(self.filename) == -1:
                    continue
                tail = line[line.find(self.filename) + len(self.filename):]
                m = re.search(r':(.*):(.*)', tail)
                if m and 'error' in m.group(1).lower():
                    return False
        return True

    def get_rose_error(self):
        err_log_path = os.path.join(self.project_dir, self.rose_log)
        if not os.path.exists(err_log_path):
            return 'no rose_error.log'
        with open(err_log_path, 'r') as err_log:
            for line in err_log:
                if re.match(r'Errors in Processing Input File:.+syntax errors.+', line):
                    return 'syntax error'

    def dump(self):
        for p in self.merlincc_logs.values():
            p.dump()

    def get_merlincc_error(self, sub_dir_name):
        if not self.merlincc_logs.has_key(sub_dir_name):
            return None
        log = self.merlincc_logs[sub_dir_name]
        if log.is_crash:
            opt_step = log.get_optimize_step()
            return (opt_step, 'crash', 'optimization step ' + str(opt_step))
        elif log.is_syntax_check_fail:
            return ('', 'ERROR: syntax check fail', '')
        elif log.get_errors():
            error = log.get_errors()[0]
            return (log.get_optimize_step(), error[0], error[1])
        elif log.get_fatal_errors():
            fatal_error = log.get_fatal_errors()[0]
            return (log.get_optimize_step(), fatal_error[0], fatal_error[1])
        else:
            return ('test error', 'test error', 'test error', 'test error')

def worker(filename):
    src_dir, src_file_name = os.path.split(filename)
    if os.path.exists(os.path.join(src_dir, 'merlincc_report')):
        return
    print('melincc process: ', src_file_name)
    platform_args = 'sdaccel::xilinx:adm-pcie-ku3:2ddr:3.3'
    with open(os.path.join(src_dir, 'merlincc_error.log'), 'w') as error_log:
        subprocess.call(['merlincc', '-c', src_file_name, '--platform={arg}'.format(arg = platform_args), '-d11'], 
                stdout = error_log,
                stderr = error_log,
                cwd = src_dir)

class LLVMTestCase(LLVMTestBase):
    @staticmethod
    def file_readable(path):
        try:
            m = os.lstat(path).st_mode
            os.chmod(path, m | stat.S_IROTH)
        except:
            pass

    @staticmethod
    def extract_func_name(line):
        m = re.search(r'name="(.+?)"', line)
        if not m:
            return ''
        name = m.group(1)
        print('function name: ', name)
        return name

    @staticmethod
    def transform_limit_name(name):
        if 'rose_' in name:
            name = name.replace('rose_', '')
        if '__merlin_' in name:
            name = name.replace('__merlin_', 'merlin_')
        if name.count('.') > 1:
            name = name.replace('.', '-', name.count('.') - 1)
        return name

    def project_readable(self):
        for dirpath, dirnames, filenames in os.walk(self.project_dir):
            LLVMTestCase.file_readable(dirpath)
            for f in filenames:
                LLVMTestCase.file_readable(os.path.join(dirpath, f))


    def __init__(self, dst_root, filename, platform, user = ''):
        super(LLVMTestCase, self).__init__(dst_root, filename, platform)
        self.user = user
        self.rose_file = None
        self.splited_files = None

    def add_pragma_kernel(self):
        src_file = os.path.join(self.project_dir, self.filename)
        cmd_arg = ['mars_opt', '-e', 'c', '-p', 'add_pragma_kernel', src_file]
        if os.path.exists(os.path.join(self.project_dir, self.rose_log)):
            rose_succeed = os.path.exists(os.path.join(self.project_dir, 'rose_succeed'))
            if rose_succeed:
                rose_file = 'rose_' + self.filename
                self.rose_file = os.path.join(self.project_dir, rose_file)
            return
        with open(os.path.join(self.project_dir, self.rose_log), 'w') as err_log:
            ret_code = subprocess.call(cmd_arg, cwd = self.project_dir, stdout = err_log, stderr = err_log)
            print('add_pragma_kernel: ', src_file)
            rose_succeed = os.path.exists(os.path.join(self.project_dir, 'rose_succeed'))
            rose_file = 'rose_' + self.filename
            if rose_succeed:
                self.rose_file = os.path.join(self.project_dir, rose_file)

    def gcc_process(self):
        if os.path.exists(os.path.join(self.project_dir, self.gcc_log)):
            return
        src_file = os.path.join(self.project_dir, self.filename)
        output_name = 'output'
        cmd_arg = ['gcc', '-S', '-Wall', src_file, '-o', output_name]
        with open(os.path.join(self.project_dir, self.gcc_log), 'w') as err_log:
            ret_code = subprocess.call(cmd_arg, cwd = self.project_dir, stdout = err_log, stderr = err_log)
            return ret_code

    def split_file(self):
        if not self.rose_file:
            return
        splited_files = []
        with open(self.rose_file, 'r') as f:
            lines = f.readlines()
            pragma_pos = [(number, LLVMTestCase.extract_func_name(line))
                    for (number, line) in enumerate(lines) 
                    if line.startswith('#pragma ACCEL kernel')]
            src_dir, src_file_name = os.path.split(self.rose_file)
            src_file_prefix, extension = os.path.splitext(src_file_name)
            extension = extension[1:]
            for number, func_name in pragma_pos:
                if not func_name:
                    continue
                if not os.path.exists(os.path.join(self.work_dir, func_name)):
                    os.makedirs(os.path.join(self.work_dir, func_name))
                src_name = '.'.join([src_file_prefix, func_name, extension])
                dst_file = os.path.join(self.work_dir, func_name, LLVMTestCase.transform_limit_name(src_name))
                splited_files.append(dst_file)
                with open(dst_file, 'w') as df:
                    ignore_line = set([n for n, _ in pragma_pos if n != number])
                    df.writelines([line for (n, line) in enumerate(lines) if n not in ignore_line])
        print("split file", splited_files)
        self.splited_files = splited_files


    def melincc_process(self):
        if not self.splited_files:
            return None
        max_process = min(len(self.splited_files), 3)
        pool = multiprocessing.Pool(max_process)
        pool.map(worker, self.splited_files)
        pool.close()

    def run_by_local(self):
        # the folder in condor tranpherout should be created
        shutil.rmtree(self.work_dir, True)
        os.mkdir(self.work_dir)
        self.project_readable()
        self.add_pragma_kernel()
        self.project_readable()
        self.gcc_process()
        self.project_readable()
        self.split_file()
        self.project_readable()
        self.melincc_process()
        if not self.splited_files:
            return
        for f in self.splited_files:
            src_dir, src_file_name = os.path.split(f)
            if os.path.exists(os.path.join(src_dir, 'kernel_top.mco')):
                os.remove(os.path.join(src_dir, 'kernel_top.mco'))
            if os.path.isdir(os.path.join(src_dir, '.merlin_prj')):
                shutil.rmtree(os.path.join(src_dir, '.merlin_prj'))

    def run_by_condor(self):
        shutil.copyfile(os.path.join(os.environ['MARS_TEST_HOME_GIT'], 'script/llvm_test/common/llvm_test.py'),
                        os.path.join(self.project_dir, 'llvm_test.py'))
        shutil.rmtree(self.work_dir, True)
        os.mkdir(self.work_dir)
        script_content = '''#! /bin/bash
source /curr/{user}/fcs_setting64.sh
python {prog} {args}
        '''.format(user = self.user,
                   prog = './llvm_test.py',
                   args = "%s %s" % (self.filename, self.user))
        script_name = 'run.sh'
        script_file = os.path.join(self.project_dir, script_name)
        if os.path.exists(script_file):
            return
        with open(script_file, 'w') as f:
            f.write(script_content)
        st = os.stat(script_file)
        os.chmod(script_file, st.st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
        input_files = os.path.join(self.project_dir, self.filename) + ',' + script_file + ',' + os.path.join(self.project_dir, 'llvm_test.py')
        # output_files = os.path.relpath(self.work_dir, self.project_dir) + ',' + self.gcc_log + ',' + self.rose_log
        output_files = os.path.relpath(self.work_dir, self.project_dir)
        print('input files:', input_files)
        print('output files:', output_files)
        job_title = '%s.%s.sh' % (self.platform[:3], self.filename)
        print(' '.join(['mars_exec_cloud', '-i', input_files, '-o', output_files, '-t', job_title, './' + script_name]))
        subprocess.call(['mars_exec_cloud', '-i', input_files, '-o', output_files, '-t', job_title, './' + script_name], cwd = self.project_dir)


if __name__ == '__main__':
    path = os.path.realpath(os.path.curdir)

    user = getpass.getuser()
    if len(sys.argv) >= 3:
        user = sys.argv[2]

    if len(sys.argv) >= 2:
        filename = sys.argv[1]

    condor = (sys.argv[-1] == '--condor')
    if condor:
        path = os.path.realpath()

#   hack to run with condor
    p = LLVMTestCase(path, filename, user)
    shutil.rmtree(p.work_dir, True)
    os.mkdir(p.work_dir)
    if condor:
        p.run_by_condor()
    else:
        p.run_by_local()
