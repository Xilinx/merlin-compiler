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
import dse_midend_checker
from dse_util import apply_midend
from util import run_command
import itertools
import os
import sys
import json

sys.path.insert(0, '../common/')
from test_util import *

src_dir = sys.argv[1]
log_dir = sys.argv[2]
impl_tool = sys.argv[3]
mode = sys.argv[4]
curr_dir = 'curr'

def prepare_src(work_dir):
    run_command('rm -rf {0}'.format(curr_dir))
    run_command('cp -rf {0}/{1} {2}'.format(src_dir, work_dir, curr_dir))
    if os.path.exists('{0}/directive.xml'.format(curr_dir)):
        run_command("sed -i 's/IMPL_TOOL/{0}/g' {1}/directive.xml"
                    .format(impl_tool, curr_dir))
    return

def save_log(work_dir):
    run_command('mv curr {0}/{1}'.format(log_dir, work_dir))
    return

def run_pass_and_load_history(pass_name, src_file):
    run_command('cd {0}; echo "mars_opt {1} -e c -p {2} -a impl_tool={3}" >> run_{2}.log; cd ..'
                .format(curr_dir, src_file, pass_name, impl_tool))
    run_command('cd {0}; mars_opt {1} -e c -p {2} -a impl_tool={3} &>> run_{2}.log; cd ..'
                .format(curr_dir, src_file, pass_name, impl_tool))
    if not os.path.exists('{0}/history_{1}.json'.format(curr_dir, pass_name)):
        return []
    with open('{0}/history_{1}.json'.format(curr_dir, pass_name), 'r') as f:
        data = json.load(f)
    msgs = dse_midend_checker.fetch_messages(data)
    return msgs

# Fetch checker list
checker_list = [func for func in dir(dse_midend_checker)
                if func.startswith('check_')]

# Unit test for each checker
for checker_name in checker_list:
    pass_name = checker_name[6:]
    func = getattr(dse_midend_checker, checker_name)
    prepare_src(pass_name)
    msgs = run_pass_and_load_history(pass_name, 'passed.cpp')
    run_command('mv -f {0}/history_{1}.json {0}/history_passed.json &>/dev/null'.format(curr_dir, pass_name))
    test('{0} {1} success'.format(impl_tool, pass_name), True,  func(msgs), exit_when_failed=False)
    msgs = run_pass_and_load_history(pass_name, 'failed.cpp')
    run_command('mv -f {0}/history_{1}.json {0}/history_failed.json &>/dev/null'.format(curr_dir, pass_name))
    if ((pass_name == 'bitwidth_opt') or
        (pass_name == 'memory_burst' and impl_tool == 'aocl') or
        (pass_name == 'loop_parallel' and impl_tool == 'aocl')): # Never fail now
        test('{0} {1} failed'.format(impl_tool, pass_name), True, func(msgs), exit_when_failed=False)
    else:
        test('{0} {1} failed'.format(impl_tool, pass_name), False, func(msgs), exit_when_failed=False)
    save_log(pass_name)

# Unit test for the integrated checker
prepare_src('all')
apply_midend(curr_dir)
test('{0} midend transformation'.format(impl_tool), False,
     os.path.exists('{0}/.*_flow_has_error'.format(curr_dir)))
result = dse_midend_checker.collect_midend_failures(curr_dir)
save_log('all')

if mode == 'update':
    ref_result = result
    with open('ref/ref_{0}.txt'.format(impl_tool), 'w') as f:
        f.write('\n'.join([msg for msg in result]))
else:
    ref_result = []
    with open('ref/ref_{0}.txt'.format(impl_tool), 'r') as f:
        for msg in f:
            ref_result.append(msg.replace('\n', ''))

if not test('{0} overall result'.format(impl_tool), True,
            all([p[0] == p[1] for p in itertools.izip_longest(ref_result, result)]),
            exit_when_failed=False):
    eprint('actual\n\t{0}'.format(result))
    eprint('expected\n\t{0}'.format(ref_result))
