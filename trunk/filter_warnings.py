#!/bin/python
# -*- coding:utf-8 -*-
import os
import re
import sys
import fileinput

def filter_warnings(src):
    # lib_names = (r'lib/boost_1_53_0', r'lib/rose', r'lib/gcc')
    lib_names = (r'source-opt/lib', r'lib/gcc', r'lib/clang', r'/usr/include')
    def contain_lib_name(s):
        for name in lib_names:
            if name in s:
                return True
        return False
    msg_pattern = re.compile(r'^(\S*(/[^/ ]*)*)(/[^/ ]*):\d+:\d+:(.*):(.*)')
    msg_path_idx = 1
    msg_type_idx = 4
    trivial_info_pattern = re.compile(r'(^\d+\s+warning.+generated)|(^In file included from.*)|(^\[\d+/\d+\].*)|(^--.*)')
    is_warning_scope = False
    merlin_warnings = []
    warning = []
    for line in (raw_line.rstrip('\n') for raw_line in src):
        m = msg_pattern.match(line)
        if is_warning_scope:
            if m:
                merlin_warnings.append(warning)
                warning_path = m.group(msg_path_idx)
                if contain_lib_name(warning_path):
                    warning = []
                    is_warning_scpoe = False
                else:
                    warning = [line]
            else:
                n = trivial_info_pattern.match(line)
                if n:
                    merlin_warnings.append(warning)
                    warning = []
                    is_warning_scope = False
                else:
                    warning.append(line)
        else:
            if m:
                warning_path = m.group(msg_path_idx)
                if not contain_lib_name(warning_path):
                    warning = [line]
                    is_warning_scope = True
    return merlin_warnings

if __name__ == '__main__':
    warnings = filter_warnings(fileinput.input())
    for warning in warnings:
        for line in warning:
            print line
        print '\n'
    sys.exit(1 if warnings else 0)
