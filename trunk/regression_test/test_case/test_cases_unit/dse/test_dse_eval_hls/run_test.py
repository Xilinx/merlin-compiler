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
from dse_util import apply_midend
from dse_run_hls import *
import os
import sys
import time

sys.path.insert(0, '../common/')
from test_util import *

# Current directoty must have directive.xml
prj_dir = '{0}.temp'.format(sys.argv[1])
code_transform_dir = '{0}/code_transform_base'.format(prj_dir)
hls_dir = '{0}/implement/exec/hls'.format(prj_dir)
ocl_dir = '{0}/implement/opencl_gen'.format(prj_dir)
export_dir = '{0}/implement/export'.format(prj_dir)

def exit_test(code):
    import sys
    global runHLS
    try:
        if code != 0:
            runHLS.clean(2) # Keep all files for debugging
        else:
            runHLS.clean(0) # Remove all files
    except:
        pass
    finally:
        sys.exit(code)

impl_tool = sys.argv[1]

##############
# Checkpoint 1: Midend transformation
##############
test('{0} Midend transformation'.format(impl_tool), True, apply_midend(code_transform_dir))

# The HLS of this case usually needs 1 minute only, but it might take longer
# depends on the system loading.
runHLS = DSERunHLS(code_transform_dir, prj_dir, 900, impl_tool)

##############
# Checkpoint 2: OpenCL generation
##############
test('{0} OpenCL generation'.format(impl_tool), True, runHLS.gen_opencl())

##############
# Checkpoint 3: Run HLS w/o error
##############
run_command('echo "" > {0}/.enforce_pref_est'.format(prj_dir))
test('{0} HLS w/o error'.format(impl_tool), 0, runHLS.run())
test('{0} Merlin report generation'.format(impl_tool), True,
     os.path.exists('{0}/report_merlin/final_info.json'.format(hls_dir)))
test('{0} Hierarchy generation'.format(impl_tool), True,
     os.path.exists('{0}/report_merlin/gen_token/topo_info.json'.format(hls_dir)))

##############
# Checkpoint 4: Report parsing
##############
result = runHLS.evaluate()
test('{0} Report parsing'.format(impl_tool), None, result.res_detail,
        not_equal=True)
test('{0} Report parsing FF'.format(impl_tool),
        True, 'FF'   in result.res_detail)
test('{0} Report parsing LUT'.format(impl_tool),
        True, 'LUT'  in result.res_detail)
test('{0} Report parsing BRAM'.format(impl_tool),
        True, 'BRAM' in result.res_detail)
test('{0} Report parsing DSP'.format(impl_tool),
        True, 'DSP'  in result.res_detail)
test('{0} Report parsing hotspot'.format(impl_tool),
        True, result.memo.find('btn:') != -1)

##############
# Checkpoint 5: Run HLS w. timeout
##############
while run_command('rm -r {0}'.format(hls_dir)) != 0:
    time.sleep(0.01)
while run_command('rm -r {0}'.format(export_dir)) != 0:
    time.sleep(0.01)
runHLS.set_time_out(3)
test('{0} HLS timeout return code'.format(impl_tool), -1, runHLS.run())

##############
# Checkpoint 6: Run HLS w. error
##############
runHLS.set_time_out(900)
while run_command('rm -r {0}'.format(hls_dir)) != 0:
    time.sleep(0.01)
while run_command('rm -r {0}'.format(export_dir)) != 0:
    time.sleep(0.01)

if sys.argv[1] == 'sdaccel':
    # Crush HLS C kernel for Xilinx
    run_command('find {0}/export -name "__merlinkernel_aes256_encrypt_ecb_kernel.cpp" | \
            xargs sed -i "s/HLS INTERFACE/dummy/g"'.format(ocl_dir))
else:
    # Crush OpenCL kernel for Intel
    run_command('find {0}/export -name "*.cl" | \
            xargs sed -i "s/void aes256_encrypt_ecb_kernel//g"'.format(ocl_dir))

test('{0} HLS error return code'.format(impl_tool), -2, runHLS.run())

##############
# Checkpoint 7: Recording
##############
runHLS.clean(1)
test('{0} Create the folder for keeping'.format(impl_tool), True,
        os.path.exists('{0}'.format(hls_dir)))
