# (C) Copyright 2016-2021 Xilinx, Inc.
# All Rights Reserved.
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License. 

"""
Python Script in Merlin Compiler

Filename    : optimizers/autodse/scripts/dse.py
Description : This is the central script for the DSE that accepts
              the code from Merlin frontend, performs DSE and outputs
              the code with optimal transformations.
Usage       : Called directly in the dse directory of a Merlin project.
              Look at "Argument parsing" section in this script for the
              detail configuration.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 12-24-2017 First pull request (Cody Hao Yu)
"""
from typing import Optional, List, Tuple, Dict # pylint: disable=unused-import
from multiprocessing.sharedctypes import ( # pylint: disable=unused-import
    Synchronized)
from multiprocessing import Process, Value
import subprocess
import signal
import os
import re
import sys
import time
import argparse
import tempfile
import operator
import glob
import merlin_logger
from util import get_cmd_output, get_src_files, has_merlin_error, must_use_pyc
from util import raise_merlin_error, run_command, try_to_int
from dse_util import MERLIN_COMPILER_HOME, APPLIER_PY, draw_pareto_curve

# Default and minimum time in SECONDs
MIN_DSE_TIME_SDX = 240 # 4 mins
MIN_DSE_TIME_AOCL = 180 # 3 mins
DEF_DSE_TIME = 14400  # 4 hours
DEF_HLS_TIME = 900    # 15 mins
DEF_THREAD = 8

##############################################################
# Argument parsing
##############################################################

PARSER = argparse.ArgumentParser(description='Design Space Explorer')
PARSER.add_argument('--working-dir', action='store',
                    default='/tmp',
                    help='source file directory')
PARSER.add_argument('--max-partition', action='store',
                    default=-1,
                    help='maximum partition number')
PARSER.add_argument('--test-limit', action='store',
                    default=-1,
                    help='maximum points to be explored (used for unit test)')
PARSER.add_argument('src', action='store',
                    help='source file directory')

ARGS = PARSER.parse_args()

PLATFORM_NAME = get_cmd_output('set_cfg -i get directive.xml platform_name')

TIME_LIMIT_CFG = get_cmd_output('set_cfg -i get directive.xml dse_time')
HLS_TIME_LIMIT_CFG = get_cmd_output('set_cfg -i get directive.xml dse_hls_time')
MAX_THREAD_CFG = get_cmd_output('set_cfg -i get directive.xml dse_thread')
RANDOM_SEED = get_cmd_output('set_cfg -i get directive.xml dse_random_seed')
DSE_ALGO = get_cmd_output('set_cfg -i get directive.xml dse_algo')
DSE_ENGINE = get_cmd_output('set_cfg -i get directive.xml dse_engine')
DSE_DEBUG = get_cmd_output('set_cfg -i get directive.xml dse_debug')
IMPL_TOOL = get_cmd_output('set_cfg -i get directive.xml implementation_tool')

MODE_CFG = get_cmd_output('set_cfg -i get directive.xml auto_dse')
CUSTOM_MODE = bool(MODE_CFG == 'custom')

ENCRYPT_CFG = get_cmd_output('set_cfg -i get directive.xml encrypt')
IS_ENCRYPT = bool(ENCRYPT_CFG == 'on')

MLOGGER = merlin_logger.get_logger('DSE')

if TIME_LIMIT_CFG:
    try:
        TIME_LIMIT = int(float(TIME_LIMIT_CFG)) * 60
        MIN_DSE_TIME = (MIN_DSE_TIME_SDX if IMPL_TOOL == 'sdaccel'
                        else MIN_DSE_TIME_AOCL)
        if TIME_LIMIT < MIN_DSE_TIME:
            MLOGGER.warning(0)
            TIME_LIMIT = MIN_DSE_TIME
    except ValueError:
        MLOGGER.error(0, 'DSE', TIME_LIMIT)
        exit_dse(1)
else:
    TIME_LIMIT = DEF_DSE_TIME

MLOGGER.info(0, TIME_LIMIT / 60)

if HLS_TIME_LIMIT_CFG:
    try:
        HLS_TIME_LIMIT = int(float(HLS_TIME_LIMIT_CFG)) * 60
    except ValueError:
        MLOGGER.error(0, 'HLS', TIME_LIMIT)
        exit_dse(1)
else:
    HLS_TIME_LIMIT = DEF_HLS_TIME

if HLS_TIME_LIMIT > TIME_LIMIT - 120:
    HLS_TIME_LIMIT = TIME_LIMIT - 120
    MLOGGER.warning(1)
MLOGGER.info(1, HLS_TIME_LIMIT / 60)

if MAX_THREAD_CFG:
    try:
        MAX_THREAD = int(float(MAX_THREAD_CFG))
    except ValueError:
        MLOGGER.error(1, MAX_THREAD_CFG)
        exit_dse(1)
else: # Default 8 threads
    MAX_THREAD = DEF_THREAD

##############################################################
# Environment setting
##############################################################

# Force terminate (unsigned char, shared by all threads)
FORCE_TERMINATE = Value('B', 0)

# Use full design space without partition
SINGLE_PARTITION = False

# Max partition number (default is the half of thread number)
MAX_PARTITION_NUM = ARGS.max_partition
if MAX_PARTITION_NUM == -1:
    MAX_PARTITION_NUM = MAX_THREAD / 2
    if MAX_PARTITION_NUM <= 0:
        MAX_PARTITION_NUM = 1

# Use default working directory
DSE_WORK_DIR = ARGS.working_dir
try:
    DSE_WORK_DIR = os.environ['MERLIN_DSE_WORKING_DIR']
except KeyError:
    pass
if not os.path.exists(DSE_WORK_DIR):
    run_command('mkdir {0}'.format(DSE_WORK_DIR))

# User specific work space prefix
WORK_PREFIX = 'merlindse_{0}_'.format(get_cmd_output('whoami'))

FILE_LIST = get_src_files('.')

##############################################################
# Function declaration
##############################################################

def exit_dse(ret_code, tmp_dir_list=None):
    # type: (int, Optional[List[str]]) -> None
    """
    Clean the temp files and exit
    """
    if tmp_dir_list is not None:
        for path in tmp_dir_list:
            run_command('rm -rf {0}'.format(path))
    sys.exit(ret_code)

def run_dse_cmd(dse_cmd, terminate):
    # type: (str, Synchronized) -> int
    """
    Run DSE with time limit
    """
    proc = subprocess.Popen(dse_cmd, preexec_fn=os.setsid, shell=True)
    try: # pylint: disable=R1702
        while proc.returncode is None:
            if terminate.value == 0:
                time.sleep(3)
                proc.poll()
            else:
                try:
                    if hasattr(os, 'killpg'):
                        os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
                    else:
                        os.kill(os.getpgid(proc.pid), signal.SIGKILL)
                except OSError:
                    MLOGGER.warning(2)
                break
    except Exception: # pylint: disable=W0703
        if proc.returncode is None:
            raise
    return proc.returncode

def run_dse_task(work_dir, idx, dse_cmd, time_limit, force_terminate):
    # type: (str, int, str, int, Synchronized) -> None
    """
    The DSE execution funtion called by thread pool
    """
    collect_result = True

    # Argument setting: TIME_LIMIT
    dse_cmd = dse_cmd.replace('TIME_LIMIT', str(time_limit))

    # Argument setting: design space partition
    dse_cmd += ('--assign-design-space ds_part{0}.json'.format(idx))

    try:
        sys.stdout.flush()
        MLOGGER.info(2, idx, int(time_limit / 60.0))
        MLOGGER.fcs_lv2('Partition {0} DSE is being performed at {1}'
                        .format(idx, work_dir))
        local_start_time = time.time()

        # Execute DSE
        code = run_dse_cmd('cd {0}; {1} . 2> engine.log'
                           .format(work_dir, dse_cmd), force_terminate)
        if code == 78:
            raise ImportError('fail to import required Python packages')
        elif code != 0:
            raise Exception('errors (code {0})'.format(code))

        sys.stdout.flush()
        MLOGGER.info(3, idx, int((time.time() - local_start_time) / 60.0))
    except KeyboardInterrupt:
        sys.stdout.flush()
        MLOGGER.info(4, idx)
        force_terminate.value = 2
    except ImportError as err:
        sys.stdout.flush()
        MLOGGER.error(2, idx, err)
        force_terminate.value = 1
        collect_result = False
    except Exception as err: # pylint: disable=W0703
        sys.stdout.flush()
        MLOGGER.error(2, idx, err)
        collect_result = True
    finally:
        if collect_result:
            # Report
            run_command('cp -rf {0}/work/report ./dse_report/report{1}'
                        .format(work_dir, idx) + ' 2> /dev/null')
            run_command('cp -rf {0}/tech*.log ./dse_report/report{1}'
                        .format(work_dir, idx) + ' 2> /dev/null')
            run_command('cp -rf {0}/engine.log ./dse_report/report{1}'
                        .format(work_dir, idx) + ' 2> /dev/null')
            run_command('cp -rf {0}/*.pkl ./dse_report/report{1}'
                        .format(work_dir, idx) + ' 2> /dev/null')

            # The best result
            run_command('cp -rf {0}/best_cfg.json'.format(work_dir) +
                        ' ./dse_report/report{0}/ 2>/dev/null'.format(idx))

    # Mark as done
    with open('.dse{0}_finish'.format(idx), 'w') as filep:
        filep.write('')

def fetch_best_latency(path):
    # type: (str) -> Optional[float]
    """
    Fetch so far best latency from log
    """
    if os.path.exists(path):
        filep = open(path, 'r')
        lines = filep.readlines()
        filep.close()
        if lines:
            _, _, best_latency = lines[-1].replace('\n', '').split('\t')
            return float(best_latency)
    return None

def summarize(partition_num):
    # type: (int) -> None
    """
    Display the summary of the DSE process
    """
    total = 0
    timeout = 0
    hls_err = 0
    merlin_err = 0
    lat_area_pts = []
    best_latency = float('inf')

    for idx in range(partition_num):
        latency = fetch_best_latency('./dse_report/report{0}/best.log'
                                     .format(idx))
        if not latency:
            continue
        best_latency = min(latency, best_latency)

    for idx in range(partition_num):
        if not os.path.exists('./dse_report/report{0}/history.log'.format(idx)):
            continue

        summary = None
        filep = open('./dse_report/report{0}/history.log'.format(idx), 'r')
        for line in filep:
            if line.startswith('DSE Summary'):
                summary = line
                continue

            # Collect results for plotting
            words = line.split('\t')
            offset = -1
            for num, word in enumerate(words):
                if word.startswith('tmp'):
                    offset = num
                    break
            if offset == -1 or not try_to_int(words[offset + 1]):
                continue
            lat_area_pts.append((int(words[offset + 1]),
                                 sum([float(u) for u in words[offset + 2:-2]])))
        filep.close()
        if summary is None:
            continue

        tokens = re.search(r'DSE Summary: Explored (\d+) designs with ' +
                           r'(\d+) HLS errors, (\d+) Merlin errors, and ' +
                           r'(\d+) timeout', summary)
        if tokens is not None:
            total += int(tokens.group(1))
            hls_err += int(tokens.group(2))
            merlin_err += int(tokens.group(3))
            timeout += int(tokens.group(4))
        else:
            MLOGGER.warning(3, idx)
    if total == 0:
        MLOGGER.warning(4)
        return

    # Show result distribution
    txtfile, pdffile = draw_pareto_curve(lat_area_pts)
    if txtfile and pdffile:
        with open(txtfile, 'r') as plotfile:
            MLOGGER.info(plotfile.read())
        MLOGGER.fcs_lv2('PDF plot: %s', pdffile)

    valid_num = total - (hls_err + merlin_err + timeout)
    timeout_rate = float(timeout) / total
    hls_err_rate = float(hls_err) / total
    merlin_err_rate = float(merlin_err) / total

    # Show user hint message
    MLOGGER.info(5, total, timeout_rate * 100, hls_err_rate * 100)
    if timeout_rate >= 0.5:
        MLOGGER.warning(5)
    elif hls_err_rate >= 0.5:
        MLOGGER.warning(6)
    elif merlin_err_rate >= 0.5:
        MLOGGER.warning(7, merlin_err_rate * 100)

    if valid_num == 0:
        MLOGGER.warning(8)
    else:
        MLOGGER.info(6, best_latency)
        run_command('echo "Final\t{0}" >> timeline.log'.format(best_latency))
    return

##############################################################
# Main process functions
##############################################################

def dse_build_design_space(max_partition_num):
    # type: (int) -> int
    """
    Build and partition the design space
    Return the partition number
    """
    if SINGLE_PARTITION:
        max_partition_num = 1

    builder_py = must_use_pyc('{0}/optimizers/'.format(MERLIN_COMPILER_HOME) +
                              'autodse/scripts/ds_proc_builder.py')
    ds_builder_cmd = ('python {0} --max-partition={1} '
                      .format(builder_py, max_partition_num) +
                      '--hls-time-limit={0} '
                      .format(HLS_TIME_LIMIT) +
                      '--impl-tool={0} --max-thread={1} '
                      .format(IMPL_TOOL, MAX_THREAD) +
                      '--profile-dir={0} '.format(DSE_WORK_DIR))
    if CUSTOM_MODE:
        ds_builder_cmd += '--type=custom '
    if IS_ENCRYPT:
        ds_builder_cmd += '--encrypt '
    ds_builder_cmd += '.'

    try:
        ret = run_command(ds_builder_cmd)
        if ret != 0:
            raise RuntimeError('Return code {0}'.format(ret))
    except Exception as err: # pylint: disable=W0703
        MLOGGER.error(3, err)
        raise_merlin_error()
        exit_dse(1)

    partition_num = len(glob.glob('ds_part*.json'))
    MLOGGER.info(7, partition_num)
    return partition_num

def dse_setup(partition_num):
    # type: (int) -> Tuple[str, int]
    """
    Setup the DSE engine
    Return (DSE engine command, initial thread number for each partition)
    """
    engine_py = must_use_pyc('{0}/optimizers/autodse/scripts/dse_engine.py'
                             .format(MERLIN_COMPILER_HOME))

    # Argument setting: engine
    dse_engine = 'HLS-only' if not DSE_ENGINE else DSE_ENGINE
    MLOGGER.fcs_lv2('DSE engine is set to {0}'.format(dse_engine))

    run_dse_engine = ('python {0} --no-dups --stop-after=TIME_LIMIT \
                      --engine={1} --impl-tool={2} '
                      .format(engine_py, dse_engine, IMPL_TOOL))
    # --enable-fast-eval: Turn off first
    if IS_ENCRYPT:
        run_dse_engine += '--encrypt '

    # Argument setting: search algorithm
    if DSE_ALGO == "single":
        run_dse_engine += ('--technique=AUCBanditMetaTechniqueSingle ')
    elif DSE_ALGO == "hotspot":
        run_dse_engine += ('--technique=AUCBanditMetaTechniqueHotspotOpt ')
    elif DSE_ALGO == "exhaustive":
        run_dse_engine += ('--technique=ExhaustiveSearch ')
    elif DSE_ALGO.startswith('heuristic'):
        freq = DSE_ALGO[DSE_ALGO.find('_') + 1:]
        if freq not in ['0', '30', '70', '100']:
            freq = '50' # Use default value
        run_dse_engine += ('--technique=AUCBanditMetaTechniqueWithBranchH{0} '
                           .format(freq))
        MLOGGER.fcs_lv2('Set heuristic frequency to %s%%', freq)
    else: # Default heuistic 50
        run_dse_engine += ('--technique=AUCBanditMetaTechniqueWithBranchH50 ')

    # Argument setting: maximum thread per instance
    if partition_num < MAX_THREAD:
        thread_per_partition = int(MAX_THREAD) / partition_num
        MLOGGER.fcs_lv2('Allocating {0} threads to each partition'
                        .format(thread_per_partition))
    else:
        thread_per_partition = 1
    run_dse_engine += ('--parallelism={0} '.format(thread_per_partition))

    # Argument setting: setup HLS time limit
    run_dse_engine += '--hls-time-limit={0} '.format(HLS_TIME_LIMIT)

    # Argument setting: setup random seed (optional)
    if RANDOM_SEED:
        run_dse_engine += '--random-seed={0} '.format(RANDOM_SEED)

    # Argument setting: debug mode
    if DSE_DEBUG in ['on', 'verbose']:
        run_dse_engine += '--debug={0} '.format(DSE_DEBUG)

    if ARGS.test_limit != -1:
        MLOGGER.warning(9, ARGS.test_limit)
        run_dse_engine += '--test-limit={0} '.format(ARGS.test_limit)

    return (run_dse_engine, thread_per_partition)

def dse_scheduling(dse_engine_cmd, partition_num, thread_per_partition):
    # type: (str, int, int) -> Tuple[List[str], int]
    """
    Schedule each partition to run the DSE process
    Return (temp dir list, the partition index with the best result)
    """

    MLOGGER.info(8, partition_num, MAX_THREAD)
    MLOGGER.info(9, PLATFORM_NAME)

    # Waiting tasks: (index, working directory)
    waiting_queue = []

    # Running tasks: (index, working dir, process object, is_finished, thread #)
    running_pool = [] # type: List[Tuple[int, str, Process, bool, int]]

    # Best result
    local_best_latency = {}
    global_best_latency = None

    curr_thread_num = 0
    done_task = 0

    # Record the start time for tracking
    start_time = time.time()

    # Push all partitions to the waiting queue
    work_dir_list = []
    for i in range(partition_num):
        # Generate a unique working space
        work_dir = tempfile.mkdtemp(prefix=WORK_PREFIX, dir='{0}/'
                                    .format(DSE_WORK_DIR))
        waiting_queue.append((i, work_dir))
        work_dir_list.append(work_dir)

    # DSE thread manager
    while waiting_queue or running_pool:
        no_time = False

        # Creating thread for new tasks
        while curr_thread_num < MAX_THREAD and waiting_queue:
            (idx, work_dir) = waiting_queue.pop(0)

            # Check time budget
            rest_time = TIME_LIMIT - int(time.time() - start_time)
            if rest_time < HLS_TIME_LIMIT:
                no_time = True
                # Mark as done
                with open('.dse{0}_finish'.format(idx), 'w') as filep:
                    filep.write('')
            else:
                run_command('rsync -av -progress --exclude="*dse*" ' +
                            '--exclude="profile" . {0} 2>&1 >/dev/null'
                            .format(work_dir))
                run_command('touch {0}/is_dse_project 2>&1 >/dev/null'
                            .format(work_dir))
                proc = Process(target=run_dse_task,
                               args=(work_dir, idx, dse_engine_cmd, rest_time,
                                     FORCE_TERMINATE,))
                proc.daemon = True
                proc.start()
                running_pool.append((idx, work_dir, proc, False,
                                     thread_per_partition))
                curr_thread_num += thread_per_partition

        if no_time:
            MLOGGER.info(10, HLS_TIME_LIMIT / 60)

        # Wait for 2 mins (check status every 10 secs)
        for _ in range(12):
            for i, task in enumerate(running_pool):
                (idx, work_dir, proc, _, thd) = task
                if os.path.exists('.dse{0}_finish'.format(idx)):
                    # Terminate the process
                    proc.terminate()

                    # Clean temporary files
                    run_command('rm -rf {0}'.format(work_dir))

                    # Mark as done
                    running_pool[i] = (idx, work_dir, proc, True, thd)
                    done_task += 1

                    # Record the current running threads
                    curr_thread_num -= thd

            # Remove finished task from the pool
            running_pool = [t for t in running_pool if not t[3]]

            if not running_pool or FORCE_TERMINATE.value != 0:
                break

            # Print waiting for 10 seconds
            time.sleep(10)

        if not FORCE_TERMINATE.value == 0:
            # Terminate all rest running processes and exit
            for task in running_pool:
                (idx, work_dir, proc, _, thd) = task
                if proc.is_alive():
                    MLOGGER.info(11, i)
                    proc.terminate()
            if FORCE_TERMINATE.value == 2:
                MLOGGER.warning(10)
            break

        # Get the current best result of each partition
        for task in running_pool:
            idx, work_dir = task[:2]
            best_latency = fetch_best_latency('{0}/work/report/best.log'
                                              .format(work_dir))
            if not best_latency:
                continue
            local_best_latency[idx] = float(best_latency)

        # Notify partitions about the ranking
        sorted_latency = sorted(local_best_latency.items(),
                                key=operator.itemgetter(1))

        if global_best_latency is None:
            global_best_latency = (0, float('inf'))
            MLOGGER.info(12, done_task, partition_num)

        if not sorted_latency: # Haven't got any result yet
            continue

        ranking = '\n'.join([str(p[1]) for p in sorted_latency])
        for task in running_pool:
            work_dir = task[1]
            with open('{0}/.who_is_the_best'.format(work_dir), 'w') as filep:
                filep.write(ranking)

        # Allocate idle threads to top-3 working partitions
        if (not waiting_queue and running_pool and
                curr_thread_num < MAX_THREAD):
            idle_thread_num = MAX_THREAD - curr_thread_num
            alloc_par = min(3, len(running_pool))
            next_getter_rank = 0
            add_threads = {} # type: Dict[int, int]

            # Sort working partitions by best result
            running = []
            for task in running_pool:
                if not task[0] in local_best_latency:
                    running.append((task[0], float('inf')))
                else:
                    running.append((task[0], local_best_latency[task[0]]))
            sorted_running = sorted(running, key=operator.itemgetter(1))

            while idle_thread_num > 0:
                idx = sorted_running[next_getter_rank][0]
                if not idx in add_threads:
                    add_threads[idx] = 1
                else:
                    add_threads[idx] += 1
                idle_thread_num -= 1
                next_getter_rank = (next_getter_rank + 1) % alloc_par

            for i, task in enumerate(running_pool):
                (idx, work_dir, proc, done, thd) = task
                if not idx in add_threads:
                    continue
                MLOGGER.fcs_lv2('Partition {0} now has {1} threads'
                                .format(idx, thd + add_threads[idx]))
                with open('{0}/.add_threads'.format(work_dir), 'w') as filep:
                    filep.write('{0}'.format(add_threads[idx]))
                running_pool[i] = (idx, work_dir, proc, done,
                                   thd + add_threads[idx])

            curr_thread_num = MAX_THREAD

        if sorted_latency[0][1] < global_best_latency[1]:
            global_best_latency = sorted_latency[0]
            MLOGGER.info(13, done_task, partition_num, global_best_latency[1],
                         global_best_latency[0])
            run_command('echo "{0}\t{1}" >> timeline.log'
                        .format(int(time.time() - start_time) / 60,
                                global_best_latency[1]))

    MLOGGER.info(14, int(time.time() - start_time) / 60)

    if global_best_latency is None:
        global_best_latency = (0, 0)

    return (work_dir_list, global_best_latency[0])

def finalize(work_dir_list, best_result_idx):
    # type: (List[str], int) -> None
    """
    Apply the best config found by the DSE and cleanup
    """

    # Check availability
    if (FORCE_TERMINATE == 1 or
            not os.path.exists('./dse_report/report{0}/best_cfg.json'
                               .format(best_result_idx))):
        MLOGGER.warning(11)
        if has_merlin_error():
            exit_dse(1)
        exit_dse(0)

    # Apply the winner design
    applier_cmd = ('python {0} ./dse_report/report{1}/best_cfg.json .'
                   .format(APPLIER_PY, best_result_idx))
    if IS_ENCRYPT:
        applier_cmd += ' --encrypt'
    run_command(applier_cmd)

    # Cleanup
    run_command('cp -f auto_dse.log ./dse_report')
    exit_dse(0, work_dir_list)

def dse_main():
    # type: () -> None
    """
    Main function of the DSE process
    """
    partition_num = dse_build_design_space(MAX_PARTITION_NUM)

    # Backup source files
    run_command('mkdir -p dse_report/src')
    for file_name in FILE_LIST:
        run_command('cp -rf {0} dse_report/src/'.format(file_name))

    (dse_cmd, thread_per_partition) = dse_setup(partition_num)

    (work_dir_list, best_idx) = dse_scheduling(dse_cmd, partition_num,
                                               thread_per_partition)

    # Summarize DSE
    summarize(partition_num)

    finalize(work_dir_list, best_idx)

if __name__ == '__main__':
    dse_main()
