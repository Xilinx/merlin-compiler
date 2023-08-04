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
import re
import shutil
import subprocess
import time
import signal
import common
import glob
import sys


class CommandlineTestCase(object):

    def __init__(self):
        common.set_environment()
        self.names = set(os.listdir(os.path.curdir))
        if os.path.exists('err.log'):
            os.remove('err.log')
        self.err_log = open('err.log', 'w')
        self.cmd_code = -1
        self.names.add('err.log')
        self.history = open('command.log', 'w')
        self.file_record = dict()

    def run(self):
        return

    def record_file(self, path):
        assert os.path.isfile(path)
        assert path not in self.file_record
        self.file_record[path] = os.stat(path)
        return

    def check_file_changed(self, path):
        assert os.path.isfile(path)
        assert path in self.file_record
        new_stat = os.stat(path)
        old_stat = self.file_record[path]
        if (new_stat.st_mode != old_stat.st_mode or
                new_stat.st_ino != old_stat.st_ino or
                new_stat.st_dev != old_stat.st_dev or
                new_stat.st_nlink != old_stat.st_nlink or
                new_stat.st_uid != old_stat.st_uid or
                new_stat.st_size != old_stat.st_size or
                new_stat.st_mtime != old_stat.st_mtime or
                new_stat.st_ctime != old_stat.st_ctime):
            print("error : file %s is changed\n" % path)
            print("error : file %s is changed\n" % path, file=self.err_log)
            if info:
                print(info + "\n")
                print(info + "\n")

    def expect_file_not_change(self, path='', info=''):
        if path:
            self.check_file_changed(path)
        else:
            for p in self.file_record.keys():
                self.check_file_changed(p)

    def run_command_period(self, cmd, seconds=3 * 60):
        cmd_str = re.sub(r'\s+', '-', cmd)
        cmd_str = cmd_str.replace('\\', '-')
        cmd_str = cmd_str.replace(r'/', '-')
        self.cmd_out = cmd_str + '_out'
        self.names.add(self.cmd_out)
        self.cmd_err = cmd_str + '_err'
        self.names.add(self.cmd_err)
        with open(self.cmd_out, 'w') as out:
            with open(self.cmd_err, 'w') as err:
                self.cmd = cmd
                print("run command: {0}\n".format(cmd))
                print("run command: {0}\n".format(cmd), file=self.history)
                p = subprocess.Popen(cmd, shell=True, stdout=out, stderr=err)
                time.sleep(seconds)
                if p.poll():
                    self.cmd_code = p.returncode
                else:
                    p.send_signal(signal.SIGINT)
                    if p.returncode:
                        self.cmd_code = p.returncode
                    else:
                        p.kill()
                        print("cmd: {0} was killed\n".format(cmd), file=err)
                        self.cmd_code = -1
        with open(self.cmd_out, 'r') as f:
            print(f.read())
        self.cmd_success = False
        with open(self.cmd_err, 'r') as f:
            s = f.read()
            self.cmd_success = (len(s) == 0) and self.cmd_code == 0
            print(s)

    def run_command(self, cmd):
        cmd_str = re.sub(r'\s+', '-', cmd)
        cmd_str = cmd_str.replace('\\', '-')
        cmd_str = cmd_str.replace(r'/', '-')
        self.cmd_out = cmd_str + '_out'
        self.names.add(self.cmd_out)
        self.cmd_err = cmd_str + '_err'
        self.names.add(self.cmd_err)
        with open(self.cmd_out, 'w') as out:
            with open(self.cmd_err, 'w') as err:
                self.cmd = cmd
                print("run command: {0}\n".format(cmd))
                print("run command: {0}\n".format(cmd), file=self.history)
                self.cmd_code = subprocess.call(cmd, shell=True, stdout=out, stderr=err)
        with open(self.cmd_out, 'r') as f:
            print(f.read())
        self.cmd_success = False
        with open(self.cmd_err, 'r') as f:
            s = f.read()
            self.cmd_success = (len(s) == 0) and self.cmd_code == 0
            print(s)

    def expect_cmd_success(self, *args):
        assert len(args) <= 2, 'wrong call with %s' % args
        if len(args) == 2:
            info = args[-1]
            self.run_command(args[0])
        elif len(args) == 1:
            info = args[0]
        else:
            info = None
        assert self.cmd_code >= 0
        if not self.cmd_success:
            print("error : %s\n" % self.cmd)
            print("error : %s\n" % self.cmd, file=self.err_log)
            if info:
                print(info + "\n")
                print(info + "\n")

    def expect_cmd_failure(self, *args):
        assert len(args) <= 2, 'wrong call with %s' % args
        if len(args) == 2:
            info = args[-1]
            self.run_command(args[0])
        elif len(args) == 1:
            info = args[0]
        else:
            info = None
        assert self.cmd_code >= 0
        if self.cmd_success:
            print("error : %s\n" % self.cmd)
            print("error : %s\n" % self.cmd, file=self.err_log)
            if info:
                print(info + "\n", file=self.err_log)
                print(info + "\n")

    def expect_file_exist(self, path, info=""):
        files = glob.glob(path)
        is_pass = True
        if files:
            for f in files:
                if not os.path.isfile(f):
                    is_pass = False
        else:
            is_pass = False
        if not is_pass:
            print("error : file '%s' not exist\n" % path)
            print("error : file '%s' not exist\n" % path, file=self.err_log)
            if info:
                print(info + "\n", file=self.err_log)
                print(info + "\n")

    def expect_file_not_exist(self, path, info=""):
        files = glob.glob(path)
        is_pass = True
        if files:
            for f in files:
                if os.path.isfile(f):
                    is_pass = False
        if not is_pass:
            print("error : file '%s' exist\n" % path)
            print("error : file '%s' exist\n" % path, file=self.err_log)
            if info:
                print(info + "\n", file=self.err_log)
                print(info + "\n")

    def expect_dir_exist(self, path, info=""):
        if not os.path.exists(path) or not os.path.isdir(path):
            print("error : dir '%s' not exist\n" % path)
            print("error : dir '%s' not exist\n" % path, file=self.err_log)
            if info:
                print(info + "\n", file=self.err_log)
                print(info + "\n")

    def expect_dir_not_empty(self, path, info=""):
        if not os.path.exists(path) or not os.listdir(path):
            print("error : dir '%s' empty\n" % path)
            print("error : dir '%s' empty\n" % path, file=self.err_log)
            if info:
                print(info + "\n", file=self.err_log)
                print(info + "\n")

    def expect_dir_empty(self, path, info=""):
        if not os.path.exists(path) or os.isdir(path):
            print("error : dir '%s' not empty\n" % path)
            print("error : dir '%s' not empty\n" % path, file=self.err_log)
            if info:
                print(info + "\n", file=self.err_log)
                print(info + "\n")

    def expect_dir_not_exist(self, path, info=""):
        if os.path.exists(path) and os.path.isdir(path):
            print("error : file '%s' exist\n" % path)
            print("error : file '%s' exist\n" % path, file=self.err_log)
            if info:
                print(info + "\n", file=self.err_log)
                print(info + "\n")

    def expect_str_in_file(self, path, s, info=""):
        assert os.path.exists(path) and os.path.isfile(path)
        with open(path, 'r') as f:
            for line in f:
                if s in line:
                    return
        print("error : '%s' not exist in %s\n" % (s, path))
        print("error : '%s' not exist in %s\n" % (s, path), file=self.err_log)
        if info:
            print(info + "\n", file=self.err_log)
            print(info + "\n")

    def clear(self):
        self.file_record.clear()
        for name in os.listdir(os.path.curdir):
            if name in self.names:
                continue
            if os.path.isfile(name):
                os.remove(name)
            if os.path.isdir(name):
                shutil.rmtree(name)

    def check_banner(self):
        if not self.cmd_success:
            return
        banner_cont = 0
        with open(self.cmd_out, 'r') as f:
            for line in f:
                if 'Merlin Compiler (TM) Version' in line:
                    banner_cont += 1
        if banner_cont != 1:
            print("error : multi version info in %s\n" % self.cmd)
            print("error : multi version info in %s\n" % self.cmd, file=self.err_log)

    def alarm_failure(self, msg=''):
        print("error : %s\n" % msg)
        print("error : %s\n" % msg, file=self.err_log)

    def run_sim(self, platform='intel'):
        # ignore temparary
        sys.exit(0)
        self.run_command('merlincc -march=sw_emu kernel_top.mco')
        self.expect_cmd_success()
        shutil.copy(os.path.join(mars_lib, 'Makehost'), os.path.curdir)
        host_script = 'cd {0}/set_env; source set_root.sh;' .format(
            os.environ['MERLIN_COMPILER_HOME'])
        if platform == 'intel':
            host_script += 'source set_altera.sh;'.format(os.environ['MERLIN_COMPILER_HOME'])
        else:
            host_script += 'source {0}/set_env/env.sh;'.format(os.environ['MERLIN_COMPILER_HOME'])
        host_script += 'cd {0} make -f Makehost host;'.format(os.path.abspath(os.path.curdir))
        if platform == 'intel':
            host_script += "CL_CONTEXT_EMULATOR_DEVICE_ALTERA='altera'; ./host_top"
        print(host_script)
        subprocess.call(host_script, shell=True)
