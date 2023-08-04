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
import stat
import sys
import os
import shutil
import subprocess
import re
import getpass
import time
import datetime
import argparse
import platform
from collections import defaultdict
from common.llvm_test import LLVMTestCase
from common.llvm_test import LLVMTestCaseReport
from common.tools import Counter

class OverrallReport(object):
    def __init__(self, projects, platform):
        self.platform = platform
        self.projects = projects
        self.total_case_count = 0
        self.finish_case_count = 0
        self.kernel_count = 0
        self.pass_count = 0
        self.syntax_fail = 0
        self.crash_steps = []
        self.limitation_errors = []
        self.limit_count = 0
        self.skip_count = 0
        self.file_with_kernel = 0
        for p in self.projects:
            self.total_case_count += 1
            if p.is_skiped():
                self.skip_count += 1
            if p.is_finish():
                self.finish_case_count += 1
            if len(p.merlincc_logs.values()) > 0:
                self.file_with_kernel += 1
            for log in p.merlincc_logs.values():
                self.kernel_count += 1
                if log.is_pass():
                    self.pass_count += 1
                elif log.is_crash:
                    step = log.get_optimize_step()
                    self.crash_steps.append(step)
                elif log.is_syntax_check_fail:
                    self.syntax_fail += 1
                else:
                    self.limit_count += 1
                    self.limitation_errors += log.get_errors()
        self.limit_error_counter = defaultdict(lambda : [0, set()])
        for code, info in self.limitation_errors:
            item = self.limit_error_counter[code]
            item[0] += 1
            item[1].add(info)
        self.limitation_errors = sorted(self.limit_error_counter.iteritems(), key = lambda x : -x[1][0])

    def print_line(self, name, occurance, percentage, target, indent = 0, indent_p = 0):
        if percentage and isinstance(percentage, (int, float)):
            print('{0: <25}{1: ^10}{3}{2:.1%}'.format(' ' * 4 * indent + name, occurance, percentage, indent_p * 4 * ' '), file = target)
        else:
            print('{0: <25}{1: ^10}{3}{2}'.format(' ' * 4 * indent + name, occurance, percentage, indent_p * 4 * ' '), file = target)

    def export(self, target):
        print('', file = target)
        print('=' * 50, file = target)
        print('Platform         :', self.platform, file = target)
        print('=' * 50, file = target)
        print('', file = target)
        print('', file = target)
        print('* Overall Summary', file = target)
        print('=' * 50, file = target)
        self.print_line('', 'Occurance', 'Percentage', target)
        print('=' * 50, file = target)
        self.print_line('Input C/C++ files', self.total_case_count, '', target)
        self.print_line('Skipped files', self.skip_count, '', target, 1)
        self.print_line('Files with kernel', self.file_with_kernel, '', target, 1)
        print('', file = target)
        self.print_line('Kernel cases', self.kernel_count, '', target)
        if self.kernel_count > 0:
            self.print_line('Syntax fail cases', self.syntax_fail, float(self.syntax_fail) / self.kernel_count, target, 1)
            self.print_line('Passed cases', self.pass_count, float(self.pass_count) / self.kernel_count, target, 1)
            self.print_line('Crashed cases', len(self.crash_steps), len(self.crash_steps) / float(self.kernel_count), target, 1)
            crash_count = Counter(self.crash_steps)
            total_count = sum(crash_count.values())
            for step, count in crash_count.most_common():
                self.print_line('Crash at step ' + str(step), count, float(count) / total_count, target, 2, 1)
            self.print_line('Limitation cases', self.limit_count, float(self.limit_count) / self.kernel_count, target, 1)
            print('', file = target)
            self.print_line('Limitation total count', sum([item[1][0] for item in self.limitation_errors]), '', target)
            for code, pair in self.limitation_errors[:10]:
                count, infos = pair
                self.print_line(code, count, '', target, 1)
        print('=' * 50, file = target)
        print('', file = target)
        limit_error_count = len(self.limitation_errors)
        print('* Limitation Summary', file = target)
        print('=' * 50, file = target)
        for code, pair in self.limitation_errors[:10]:
            count, infos = pair
            print('  {0}  ({1}):'.format(code, count), file = target)
            cnt = 0
            for info in infos:
                cnt += 1
                if cnt >= 5:
                    break
                print(' ' * 8, info, file = target)
            print('', file = target)

class LLVMTestReport:
    def __init__(self, src, dst, platform):
        assert os.path.isabs(src), src
        assert os.path.isabs(dst), dst
        assert platform == 'xilinx' or platform == 'intel', platform
        if not os.path.exists(src) or not os.path.isdir(src):
            print('src folder not exist: {0}'.format(src))
        if not os.path.exists(dst) or not os.path.isdir(dst):
            print('dst folder not exist: {0}'.format(dst))
        self.src = src
        self.dst = dst
        self.platform = platform
        self.projects = []
        for p in get_dst_files(src, dst):
            dirpath, filename = os.path.split(p)
            if os.path.isdir(dirpath):
                self.projects.append(LLVMTestCaseReport(dirpath, filename, platform))

    def get_total_case_count(self):
        return len(self.projects)

    def get_finish_case_count(self):
        return len([p for p in self.projects if p.is_finish()])

    def get_total_status(self):
        return (self.get_total_case_count(), self.get_finish_case_count())

    def get_rose_process_status(self):
        rose_pass = 0
        rose_fail = 0
        for p in self.projects:
            if p.is_rose_pass():
                rose_pass += 1
            if p.is_gcc_compile_pass() and not p.is_rose_pass():
                rose_fail += 1
        return (rose_pass, rose_fail)

    def get_overrall_status(self):
        kernel_count = 0
        pass_count = 0
        crash_count = 0
        syntax_fail = 0
        limitation = 0
        crash_step = dict()
        limitation_errors = []
        for p in self.projects:
            for log in p.merlincc_logs.values():
                kernel_count += 1
                if log.is_pass():
                    pass_count += 1
                elif log.is_crash:
                    crash_count += 1
                    step = log.get_optimize_step()
                    if crash_step.has_key(step):
                        crash_step[step] += 1
                    else:
                        crash_step[step] = 1
                elif log.is_syntax_check_fail:
                    syntax_fail += 1
                else:
                    limitation += 1
                    limitation_errors += log.get_errors()
        ret = {'kernel_count': kernel_count,
               'pass_count': pass_count,
               'crash_count': crash_count,
               'syntax_fail': syntax_fail,
               'limitation': limitation,
               'crash_info': crash_step,
               'limitation_info': limitation_errors}
        return ret

    def get_total_execution_time(self):
        exe_times = [p.get_execution_time() for p in self.projects]
        return max(exe_times)

    def get_crash_count(self):
        crash_counts = [p.get_crash_count() for p in self.projects]
        return sum(crash_counts)
    
    def export(self, port = sys.stdout):
        print('Report user     : ', getpass.getuser(), file = port)
        print('Report time     : ', time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime()), file = port)
        # exec_time = (self.get_total_execution_time())
        # print('Execution time  : ', str(datetime.timedelta(hours = exec_time)), file = port)
        print('Run directory   : ', '{machine_name}:{path}'.format(machine_name = platform.node(), path = self.dst), file = port)
        print('Platform        : ', self.platform, file = port)
        print('C file count    : ', len(self.projects), file = port)
        skip_count = sum(map(lambda x : 1 if x.is_skiped() else 0, self.projects))
        print('Skip count      : ', skip_count, file = port)
        finish_case = sum(map(lambda x : 1 if x.is_finish() else 0, self.projects))
        print('Running count   : ', len(self.projects) - finish_case, file = port)
        num_file_have_kernel = sum(map(lambda p : 1 if p.is_gen_kernel_success() else 0, self.projects))
        print('File with kernel: ', num_file_have_kernel, file = port)
        print('=' * 50, file = port)
        
        num_kernel = sum(map(lambda x : len(x.merlincc_logs), self.projects))
        print('Num of kernel: ', num_kernel)

        overall_status = self.get_overrall_status()
        print('Kernel count    : ', overall_status['kernel_count'], file = port)
        print('Pass count      : ', overall_status['pass_count'], file = port)
        print('Crash count     : ', overall_status['crash_count'], file = port)
        print('Syntax fail count: ', overall_status['syntax_fail'], file = port)
        print('Limitation count: ', overall_status['limitation'], file = port)
        print('Crash step      : ', overall_status['crash_info'], file = port)

        print('=' * 50, file = port)
        title_cells = ('case-name', 'func-name', 'state', 'error-code', 'info')
        content = []
        for project in self.projects:
            project.dump()
            content += self.get_content_ClangTestProject(project)
        content_strip = zip(*content)
        cell_widths = [max([len(str(x)) for x in strip]) for strip in content_strip]
        for i in range(len(title_cells)):
            cell_widths[i] = max(cell_widths[i], len(title_cells[i])) + 2
        cell_widths = cell_widths[0:len(title_cells)]
        self.print_frame(cell_widths, port)
        self.print_cell(zip(cell_widths, title_cells), port)
        self.print_frame(cell_widths, port)
        for line in content:
            self.print_cell(zip(cell_widths, line), port, ('^', '^', '^', '<', '<'))
        self.print_frame(cell_widths, port)
        print('\n', file = port)

    def print_frame(self, cell_widths, port):
        for w in cell_widths:
            print('+', end = '', file = port)
            print('-' * w, end = '', file = port)
        print('+', file = port)

    def print_cell(self, cell_pairs, port, aligns = None):
        for n, pair in enumerate(cell_pairs):
            w, s = pair
            print('|', end = '', file = port)
            if not aligns:
                print(('{0:' + '^' + str(w) + '}').format(s), end = '', file = port)
            else:
                print(('{0:' + aligns[n] + str(w) + '}').format(s), end = '', file = port)
        print('|', file = port)

    def get_content_ClangTestProject(self, project):
        ret = []
        if not project.is_finish():
            ret.append((project.filename, '', 'running', '', ''))
            return ret
        info = ''
        if len(project.sub_dir_names) == 0:
            content = (project.filename, '', 'Skip', 'no func', '')
            ret.append(content)
        else:
            for idx, d in enumerate(project.sub_dir_names):
                func_content = ['', '', '', '', '']
                if project.is_merlincc_pass(d):
                    func_content = ['', d, 'Pass', '', '']
                elif project.merlincc_logs.has_key(d):
                    log = project.merlincc_logs[d]
                    error_msg = project.get_merlincc_error(d)
                    if error_msg:
                        func_content = ['', d, log.get_state(), error_msg[1], error_msg[2]]
                        if log.is_crash:
                            func_content[-2] = 'step ' + str(log.get_optimize_step())
                            func_content[-1] = ''
                    else:
                        func_content = ['', d, 'Error', 'get msg failed', '']
                else:
                    func_content = ['', d, 'Error', 'get msg failed', '']
                if idx == 0:
                    func_content[0] = project.filename
                ret.append(func_content)
        return ret


def get_case_list(test_root):
    ret = []
    for dirpath, dirnames, dirfiles in os.walk(test_root):
        source_files = [f for f in dirfiles if f.endswith(('.c', '.cc', '.cpp')) and f not in ignore_cases]
        if not source_files:
            continue
        test_work_root = os.path.abspath(os.path.curdir)
        relative_dir_path = dirpath[len(test_root):]
        relative_dir_path = relative_dir_path.lstrip('/')
        if allow_cases:
            source_files = [f for f in source_files if os.path.join(relative_dir_path, f) in allow_cases]
        ret.append((relative_dir_path, source_files))
    return ret

def get_dst_files(src, dst):
    assert os.path.isabs(src)
    assert os.path.isabs(dst)
    ret = []
    for relative_dir_path, source_files in get_case_list(src):
        ret += [os.path.join(dst, relative_dir_path, f.replace('.', '-'), f) for f in source_files]
    return ret

def file_readable(path):
    m = os.lstat(path).st_mode
    os.chmod(path, m | stat.S_IROTH)

def create_test_work(src, dst, platform, on_condor = False):
    assert os.path.isabs(src)
    assert os.path.isabs(dst)
    assert platform == 'xilinx' or platform == 'intel'
    case_list = get_case_list(src)
    source_files = []
    for relative_dir_path, files in case_list:
        files = [os.path.join(src, relative_dir_path, name) for name in files]
        source_files += files
    destinate_files = get_dst_files(src, dst)
    for dst_file in destinate_files:
        path = os.path.dirname(dst_file)
        if not os.path.exists(path):
            os.makedirs(path)
    for s, d in zip(source_files, destinate_files):
        shutil.copyfile(s, d)
    for dirpath, dirnames, filenames in os.walk(dst):
        file_readable(dirpath)
        for f in filenames:
            file_readable(os.path.join(dirpath, f))
    test_cases = []
    for f in destinate_files:
        dirpath, filename = os.path.split(f)
        p = LLVMTestCase(dirpath, filename, platform, getpass.getuser())
        test_cases.append(p)
    if on_condor:
        for case in test_cases:
            case.run_by_condor()
    else:
        for case in test_cases:
            case.run_by_local()

def gen_exe_script(filepath, cmd):
    with open(filepath, 'w') as f:
        f.write('#! /bin/sh\n')
        f.write(cmd + '\n')
    m = os.lstat(filepath).st_mode
    os.chmod(filepath, m | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

ignore_cases_path = os.path.join(os.environ['MARS_TEST_HOME'], 'test_case/test_case_llvm/ignore_cases')
ignore_cases = set()
if os.path.isfile(ignore_cases_path):
    with open(ignore_cases_path, 'r') as f:
        for line in f:
            ignore_cases.add(line.strip())
    print(ignore_cases)

allow_cases = set()

if __name__ == '__main__':
    os.environ['MARS_TEST_CONDOR_FORCE_TRANSFER'] = '1'

    clang_test_root = os.path.join(os.environ['MARS_TEST_HOME'], 'test_case/test_cases_llvm/test') # os.path.join(os.environ['MARS_TEST_HOME_GIT'], '')
    clang_test_dirs = ('ARCMT', 'ASTMerge', 'Analysis', 'CXX', 'CodeCompletion', 'CodeGen', 
            'CodeGenCUDA', 'CodeGenCXX', 'CodeGenCoroutines', 'CodeGenObjC', 'CodeGenObjCXX'
            'CodeGenOpenCL', 'Coverage', 'CoverageMapping', 'Driver', 'FixIt', 'Format',
            'Frontend', 'Headers', 'Import', 'Index', 'Integration', 'Layout', 'Lexer',
            'Misc', 'Modules', 'OpenMP', 'PCH', 'Parser', 'Preprocessor', 'Profile'
            'Rewriter', 'Sema', 'SemaCUDA', 'SemaCXX', 'SemaObjC', 'SemaObjCXX', 'SemaOpenCL'
            'SemaTemplate', 'TableGen', 'Tooling', 'Unit', 'VFS', 'tmp')

    llvm_test_single = os.path.join(os.environ['MARS_TEST_HOME'], 'test_case/test_cases_llvm/SingleSource')
    llvm_test_multi = os.path.join(os.environ['MARS_TEST_HOME'], 'test_case/test_cases_llvm/MultiSource')

    parser = argparse.ArgumentParser()
    parser.add_argument('--report', action = 'store_true', default = False)
    parser.add_argument('--condor', action = 'store_true', default = False)
    parser.add_argument('--path', default = os.path.realpath('.'))
    parser.add_argument('--small', action = 'store_true', default = False)
    parser.add_argument('--list', action = 'store_true', default = False)
    args = parser.parse_args()

    test_cases = []
    if args.small:
        clang_test_subset = [os.path.join(clang_test_root, d) for d in clang_test_dirs if d == 'Misc']
        test_cases += clang_test_subset
    else:
        test_cases.append(clang_test_root)
        test_cases.append(llvm_test_single)
        test_cases.append(llvm_test_multi)

    platforms = []
    platforms.append('xilinx')
    # platforms.append('intel')

    cases_list_path = os.path.join(os.curdir, 'cases.list')

    if args.list:
        with open(cases_list_path, 'w') as f:
            for p in test_cases:
                case_list = get_case_list(p)
                for relpath, files in case_list:
                    f.writelines((os.path.join(relpath, l) + '\n' for l in files))
        sys.exit()

    if os.path.isfile(cases_list_path):
        with open(cases_list_path, 'r') as f:
            for l in f:
                allow_cases.add(l.strip())

    if args.report:
        total_projects = []
        for p in platforms:
            platform_projects = []
            for case_path in test_cases:
                if not os.path.isdir(os.path.join(args.path, p, os.path.basename(case_path))):
                    continue
                report = LLVMTestReport(os.path.expanduser(case_path), os.path.join(args.path, p, os.path.basename(case_path)),
                        p)
                total_projects += report.projects
                platform_projects += report.projects
                with open('.'.join((os.path.basename(case_path), p, 'log')), 'w') as f:
                    report.export(f)
            platform_status = OverrallReport(platform_projects, p)
            with open('status.%s.log' % p, 'w') as f:
                platform_status.export(f)
            crashed_cases_path = './crash_' + p
            if not os.path.exists(crashed_cases_path):
                for proj in platform_projects:
                    for name, log in proj.merlincc_logs.items():
                        if not log.is_crash:
                            continue
                        step = log.get_optimize_step()
                        src_path = os.path.join(proj.work_dir, name)
                        dst_path = os.path.join(crashed_cases_path, str(step), name)
                        if os.path.exists(dst_path):
                            postfix = 0
                            while os.path.exists(dst_path + str(postfix)):
                                postfix += 1
                            shutil.copytree(src_path, dst_path + str(postfix))
                        else:
                            shutil.copytree(src_path, dst_path)
        all_status = OverrallReport(total_projects, 'intel&xilinx')
        with open('status.all.log', 'w') as f:
            all_status.export(f)
    else:
        version_info = os.path.join(os.environ['MERLIN_COMPILER_HOME'], 'VERSION');
        rerun_cmd =  'python ${MC}/regression_test/script/llvm_test/merlin_llvm_test.py ' + ' '.join(sys.argv[1:]) + ' --path ' + args.path
        gen_exe_script('rerun.sh', rerun_cmd)
        report_cmd = 'python ${MC}/regression_test/script/llvm_test/merlin_llvm_test.py ' + ' '.join([s for s in sys.argv[1:] if s != '--condor']) + ' --report --path ' + args.path
        gen_exe_script('report.sh', report_cmd)
        if os.path.exists(version_info):
            shutil.copy(version_info, args.path);
        for case_path in test_cases:
            for p in platforms:
                create_test_work(os.path.expanduser(case_path),
                        os.path.join(args.path, p, os.path.basename(case_path)), 
                        p,
                        args.condor)
