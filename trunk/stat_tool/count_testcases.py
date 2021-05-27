#!/usr/bin/env python3

import os

groups_with_pytest = [
  'message_report',
]

groups_with_dot_expected = [
  'lower_cpp',
  'mcheck',
  'mrefactor',
  'none',
]

groups_with_dot_ref = [
  'add_pragma_kernel',
  'array_delinearize',
  'array_linearize',
  'auto_parallel_insertion',
  'bitwidth_opt',
  'coarse_parallel',
  'coarse_pipeline',
  'comm_opt',
  'const_propagate',
  'dependency_resolve',
  'detect_library_calls',
  'detect_non_static_pointer',
  'dse',
  'function_inline',
  'input_checker',
  'ir',
  'kernel_separate',
  'kernel_separate_task',
  'line_buffer',
  'loop_parallel',
  'loop_tiling',
  'lower_cpp',
  'lower_separate',
  'lower_static_cast',
  'mars_ir',
  'memory_burst',
  'memory_reduce',
  'midend_preprocess',
  'msg_report',
  'pointer_comparison',
  'pointer_type_cast',
  'postwrap_process',
  'preprocess',
  'program_analysis',
  'reduction',
  'reduction_general',
  'simple_dce',
  'stream_model',
  'stream_prefetch',
  'struct_decompose',
  'structural_inline',
]

groups_with_run_file = [
  'bool2char',
  'comm_opt',
  'const_propagate',
  'dse',
  'fast_dse',
  'final_code_gen',
  'interface',
  'interface_port_renaming',
  'ir',
  'kernel_pragma_gen',
  'kernel_separate',
  'kernel_separate_task',
  'kernel_wrapper',
  'lower_separate',
  'merlin_stream_unit_test_small',
  'msg_report_perf',
  'reduction_general',
  'simple_dce',
  'simple_pointer_alias_removal',
  'stream_prefetch',
]


def count_by_file_name_filter(group, filter):
    num = 0
    for rt, ds, fs in os.walk(group):
        for f in fs:
            if filter(f):
                num += 1
    return {group: num}

def count_by_kernel_pragma(group):
    num = 0
    for rt, ds, fs in os.walk(group):
        for f in filter(lambda x: x.endswith('.c') or
                                  x.endswith('.cpp'), fs):
            with open(os.path.join(rt, f)) as input:
                for line in input:
                    if line.startswith('#pragma ACCEL kernel'):
                        num += 1
    return {group: num}

def display(result):
    for group in sorted(result):
        print("%-35s : %8d" % (group, result[group]))
    print("\nTotal : %d" % sum(result.values()))

def count_all_unit_testcases(dir):
    oldcwd = os.getcwd()
    os.chdir(dir)

    res = {}
    all_groups = filter(os.path.isdir, os.listdir(os.getcwd()))

    for group in groups_with_pytest:
        if group == 'message_report':
            res.update({group: 202})

    for group in groups_with_run_file:
        res.update(
            count_by_file_name_filter(
                group,
                lambda x: x in ('run', 'run.sh')
            )
        )

    for group in groups_with_dot_expected:
        if group not in groups_with_run_file:
            res.update(
                count_by_file_name_filter(
                    group,
                    lambda x: x.endswith('.expected')
                )
            )

    for group in groups_with_dot_ref:
        if group not in groups_with_run_file:
            res.update(
                count_by_file_name_filter(
                    group,
                    lambda x: x.endswith('.ref')
                )
            )

    rest_groups = (
      set(all_groups)
      -set(groups_with_pytest)
      -set(groups_with_run_file)
      -set(groups_with_dot_expected)
      -set(groups_with_dot_ref)
    )

    for group in rest_groups:
        res.update(count_by_kernel_pragma(group))

    display(res)
    os.chdir(oldcwd)


if __name__ == '__main__':
    assert os.getenv("MERLIN_COMPILER_HOME") is not None
    dir_unit = os.path.join(
      *[os.getenv("MERLIN_COMPILER_HOME"),
        'regression_test', 'test_case', 'test_cases_unit'])
    count_all_unit_testcases(dir_unit)
