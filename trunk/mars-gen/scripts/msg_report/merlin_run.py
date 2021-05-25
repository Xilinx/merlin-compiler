#!/bin/python
# -*- coding:utf-8 -*-
from __future__ import print_function
import os
import sys
import shutil
import subprocess
import logging
import re
import json
import xml.etree.ElementTree

def Error(msg):
    logging.error('[RPT ERROR]{0}'.format(msg))

class MerlinRun(object):
    def __init__(self, path, options):
        self.run_dir = path
        assert os.path.isdir(self.run_dir), self.run_dir

        self.spec_file = options.get('xml', '')
        assert os.path.isfile(self.spec_file), self.spec_file
        self.spec = xml.etree.ElementTree.parse(self.spec_file)

        self.src_dir = options.get('src', '')
        self.metadata_dir = options.get('metadata', '')
        assert os.path.isdir(self.metadata_dir), self.metadata_dir

        self.is_fast_dse_mode = False
        if options.get('report', '') != 'fast_dse':
            self.report_dir = options.get('report', '')
        else:
            self.report_dir = ''
            self.is_fast_dse_mode = True

        self.last_c_dir = options.get('dst', '')
        self.msg_report_dir = self.run_dir
        self.gen_token_report    = os.path.join(self.msg_report_dir, "gen_token.json")
        self.perf_est_report     = os.path.join(self.msg_report_dir, "perf_est.json")
        self.ref_induct_report   = os.path.join(self.msg_report_dir, "refer_induct.json")
        self.hierarchy_report    = os.path.join(self.msg_report_dir, "hierarchy.json")
        self.final_report        = os.path.join(self.msg_report_dir, "final.rpt")
        self.gen_token_dir       = os.path.join(self.msg_report_dir, "gen_token")
        self.perf_est_dir        = os.path.join(self.msg_report_dir, "perf_est")
        self.ref_induct_dir      = os.path.join(self.msg_report_dir, "ref_induct")
        self.final_report_dir    = os.path.join(self.msg_report_dir, "final_report")
        self.gen_token_lc_dir    = os.path.join(self.gen_token_dir, 'lc')
        self.perf_est_lc_dir     = os.path.join(self.perf_est_dir, 'lc')
        self.final_info          = os.path.join(self.final_report_dir, 'final_info.json')
        self.rpt_path = os.path.join(self.msg_report_dir, 'merlin.rpt')
        self.fast_dse_perf_dir   = os.path.join(self.msg_report_dir, 'fast_dse/perf')
        self.hls_report_dir      = self.msg_report_dir

    def get_src_dir(self):
        return self.src_dir

    def get_impl_tool(self):
        impl_tool = self.spec.getroot().find('implementation_tool').text.strip()
        return impl_tool

    def get_impl_tool_version(self):
        impl_tool = self.get_impl_tool()
        version = ''
        if impl_tool == 'sdaccel':
            p = subprocess.Popen(['xocc', '--version'], stdout=subprocess.PIPE)
            out, _ = p.communicate()
            m = re.search(r'\* SDx v?(\S*)(_SDX)?', out, re.M)
            if m:
                version = m.group(1)
            if version.endswith('_SDX'):
                version = version[:-len('_SDX')]
        return version

    def get_platform(self):
        tool_to_platform = {
                'sdaccel' : 'xilinx'}
        return tool_to_platform.get(self.get_impl_tool(), '')

    def get_is_debug(self):
        ihatebug = self.spec.getroot().find('ihatebug').text.strip()
        return ihatebug == 'debug'

    def get_include(self):
        default_include = '-I' + os.path.join(os.environ.get('MERLIN_COMPILER_HOME', '.'), 'mars-gen/lib/merlin')
        include_path = self.spec.getroot().find('include_path')
        if include_path is None or include_path.text is None:
            return default_include
        include_str = include_path.text.strip()
        return include_str + ' ' + default_include

    def get_report_dir(self, report_type):
        if report_type == 'hls':
            return os.path.join(self.report_dir, 'hls')

    def clean_msg_report_dir(self):
        shutil.rmtree(self.msg_report_dir)
        os.mkdir(self.msg_report_dir)

    def is_fast_dse(self):
        return self.is_fast_dse_mode

    def run_rose_pass(self, pass_name, work_dir, srcs, *args):
        if os.path.isdir(work_dir):
            # clear_dir(work_dir)
            pass
        elif os.path.exists(work_dir):
            Error('should not exist path: %s\n' % work_dir)
            return False
        else:
            os.mkdir(work_dir)
        macro_define = self.spec.getroot().find('macro_define')
        macro_flags = []
        if macro_define is not None and macro_define.text is not None:
            macro_flags = ['-a', 'cflags="{0}"'.format(macro_define.text.strip())]
        # the order of source file will affact the order of node in AST 
        # should keep all source file in same order
        srcs.sort()
        impl_tool = "impl_tool=" + self.get_impl_tool()
        cmd = ['mars_opt', '-e', 'c', '-p', pass_name, "-a", impl_tool]
        cmd += srcs
        cmd += macro_flags
        debug_cmd = ['gdb', '--args', 'mars_opt_org', '-e', 'c', '-p',
        pass_name, "-a", impl_tool]
        debug_cmd += srcs
        debug_cmd += macro_flags
        for arg in args:
            if arg:
                cmd.append(arg)
                debug_cmd.append(arg)
        includes = set([os.path.dirname(s) for s in srcs])
        cmd += ['-I', '.']
        for i in includes:
            if not i:
                continue
            cmd.append('-I')
            cmd.append(i)
            debug_cmd.append('-I')
            debug_cmd.append(i)
        encrypt = self.spec.getroot().find('encrypt')
        if encrypt is None or encrypt.text != 'on' or pass_name == 'gen_hierarchy':
#            encrypt_cmd = cmd
            encrypt_cmd = ['merlin_safe_exec', ' '.join(cmd), '::', ' '.join(srcs), '::', '0']
        else:
            encrypt_cmd = ['merlin_safe_exec', ' '.join(cmd), '::', ' '.join(srcs), '::', '60']
        with open(os.path.join(work_dir, pass_name + '.msg_report.log'), 'w+') as f:
            p = subprocess.Popen(encrypt_cmd,
                    stdout = f,
                    stderr = f,
                    cwd = work_dir)
            p.communicate()
            f.seek(0)
            log_content = f.read()
            logging.debug(log_content)
            logging.debug('cmd:\n{0}\nworkdir:\n{1}'.format(' '.join(cmd), work_dir))
        critical_msg = os.path.join(work_dir, 'critical_message.rpt')
        if os.path.isfile(critical_msg):
            with open(critical_msg, 'r') as f:
                for line in f:
                    print(line, file=sys.stderr)
        with open(os.path.join(work_dir, 'run_cmd'), 'w') as f:
            f.write(' '.join(cmd))
        debug_cmd += ['-I', os.path.join(os.environ.get('MERLIN_COMPILER_HOME'), 'mars-gen/lib/merlin')]
        debug_cmd += ['-I', os.path.join(os.environ.get('MERLIN_COMPILER_HOME'), 'source-opt/include/apint_include')]
        with open(os.path.join(work_dir, 'run_gdb'), 'w') as f:
            f.write(' '.join(debug_cmd))
        if p.returncode != 0 or not os.path.exists(os.path.join(work_dir, 'rose_succeed')):
            Error('cmd failed\n{0}'.format(' '.join(cmd)))
            return False
        return True
