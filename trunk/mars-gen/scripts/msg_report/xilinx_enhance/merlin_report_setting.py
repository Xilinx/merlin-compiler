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

'''
This is used for common function
'''

import os
import xml.etree.ElementTree

#pylint: disable=too-few-public-methods
class MerlinReportSetting:
    '''
    this class used to set merlin different type of settings
    '''
    # pylint: disable=too-many-instance-attributes
    # pylint: disable=too-many-statements, too-many-branches
    def __init__(self, xml_spec="directive.xml"):
        # spec attributes
        self.xml_impl_tool = ""
        self.xml_include_path = ""
        self.xml_macro_define = ""
        self.xml_encrypt = ""
        self.xml_platform_name = ""
        self.xml_source_file = ""
        if os.path.isfile(xml_spec):
            self.spec_file = xml_spec
            self.spec_info = xml.etree.ElementTree.parse(self.spec_file)
            node_impl_tool = self.spec_info.getroot().find('implementation_tool')
            if node_impl_tool is not None:
                if node_impl_tool.text is not None:
                    self.xml_impl_tool = node_impl_tool.text.strip()
            node_include_path = self.spec_info.getroot().find('include_path')
            if node_include_path is not None:
                if node_include_path.text is not None:
                    self.xml_include_path = node_include_path.text.strip()
            node_macro_define = self.spec_info.getroot().find('macro_define')
            if node_macro_define is not None:
                if node_macro_define.text is not None:
                    self.xml_macro_define = node_macro_define.text.strip()
            node_encrypt = self.spec_info.getroot().find('encrypt')
            if node_encrypt is not None:
                if node_encrypt.text is not None:
                    self.xml_encrypt = node_encrypt.text.strip()
            node_platform_name = self.spec_info.getroot().find('platform_name')
            if node_platform_name is not None:
                if node_platform_name.text is not None:
                    self.xml_platform_name = node_platform_name.text.strip()
            node_source_file = self.spec_info.getroot().find('source_file')
            if node_source_file is not None:
                if node_source_file.text is not None:
                    self.xml_source_file = node_source_file.text.strip()

        # json file name
        self.json_vivado_hls = "vivado_hls.json"
        self.json_adb = "adb.json"
        self.json_loop_location = "loop_location.json"
        self.json_vendor_summary = "vendor.json"
        self.json_topo_info = "topo_info.json"
        self.json_gen_token = "gen_token.json"
        self.json_perf_est = "perf_est.json"
        self.json_refer_induct = "refer_induct.json"
        self.json_hierarchy = "hierarchy.json"
        self.json_final_info = "final_info.json"
        self.json_fill_source_data = "fill_source_data.json"
        self.json_fill_hotspot_data = "fill_hotspot_data.json"
        self.json_summary = "summary.json"
        self.json_interface = "interface.json"
        self.json_performance = "performance.json"
        self.json_resource = "resource.json"
        self.json_merlin_rpt = "merlin.rpt"
        self.json_kernel_list = "kernel_list.json"

	# json file attributes
        self.attr_comment_interface = "comment_interface"
        self.attr_comment = "comment"
        self.attr_comment_resource = "comment_resource"
        self.attr_begin = "begin"
        self.attr_col = "col"
        self.attr_end = "end"
        self.attr_filename = "filename"
        self.attr_line = "line"
        self.attr_messages = "messages"
        self.attr_name = "name"
        self.attr_label_name = "loop_label"
        self.attr_org_identifier = "org_identifier"
        self.attr_scope_org_identifier = "scope_org_identifier"
        self.attr_topo_id = "topo_id"
        self.attr_type = "type"
        self.attr_unparse_info = "unparse_info"
        self.attr_cycle_burst = "CYCLE_BURST"
        self.attr_cycle_compute = "CYCLE_COMPUTE"
        self.attr_cycle_total = "CYCLE_TOT"
        self.attr_cycle_unit = "CYCLE_UNIT"
        self.attr_glue_latency = "glue-latency"
        self.attr_glue_latency_iteration = "glue-latency-iteration"
        self.attr_logic_latency = "logic-latency"
        self.attr_logic_latency_iteration = "logic-latency-iteration"
        self.attr_serially_execution = "serially_execution"
        self.attr_latency_min = "latency-min"
        self.attr_latency_max = "latency-max"
        self.attr_interval_min = "interval-min"
        self.attr_interval_max = "interval-max"
        self.attr_iteration_latency_min = "iteration-latency-min"
        self.attr_iteration_latency = "iteration-latency"
        self.attr_total_bram = "total-BRAM"
        self.attr_total_dsp = "total-DSP"
        self.attr_total_ff = "total-FF"
        self.attr_total_lut = "total-LUT"
        self.attr_total_uram = "total-URAM"
        self.attr_util_bram = "util-BRAM"
        self.attr_util_dsp = "util-DSP"
        self.attr_util_ff = "util-FF"
        self.attr_util_lut = "util-LUT"
        self.attr_util_uram = "util-URAM"
        self.attr_updated = "updated"
        self.attr_inline = "inline"
        self.attr_instance_name = "instance_name"
        self.attr_module_name = "module_name"
        self.attr_canonical = "canonical"
        self.attr_flatten = "flatten"
        self.attr_flatten_son = "flatten_son"
        self.attr_flatten_id = "flatten-id"
        self.attr_trip_count = "trip-count"
        self.attr_lc_trip_count = "lc-trip-count"
        self.attr_pipelined = "pipelined"
        self.attr_unrolled = "unrolled"
        self.attr_unroll_factor = "unroll-factor"
        self.attr_iteration_interval = "II"
        self.attr_module_bits = "module_bits"
        self.attr_partition_dim = "partition_dim"
        self.attr_cyclic_factor = "cyclic-factor"
        self.attr_false_intra_dependency = "false_intra_dependency"
        self.attr_false_inter_dependency = "false_inter_dependency"
        self.attr_var_type = "var_type"
        self.attr_depth = "depth"
        self.attr_bandwidth = "bandwidth"
        self.attr_burst = "burst"
        self.attr_bitwidth = "bitwidth"
        self.attr_burst_length = "burst_length"
        self.attr_burst_length_max = "burst_length_max"

        self.attr_src_line = "src_line"
        self.attr_src_col = "src_col"
        self.attr_src_filename = "src_filename"
        self.attr_src_func_name = "src_func_name"
        self.attr_src_topo_id = "src_topo_id"
        self.attr_global_variables = "global_variables"
        self.attr_interfaces = "interfaces"
        self.attr_arrays = "arrays"
        self.attr_var_refs = "var_refs"
        self.attr_var_defs = "var_defs"
        self.attr_stmt = "stmts"
        self.attr_childs = "childs"
        self.attr_sub_functions = "sub_functions"
        self.attr_call_functions = "call_functions"
        self.attr_interface_bitwidth = "interface_bitwidth"
        self.attr_coalesce_bitwidth = "coalesce_bitwidth"
        self.attr_coalesce = "coalesce"
        self.attr_burst_read_write = "burst_read_write"
        self.attr_note = "table_note"
        self.attr_parent_loop = "parent_loop"
        self.attr_map_to_id = "map_to_id"
        self.attr_function_id = "function_id"

        self.attr_streaming = "streaming"
        self.attr_memory_coalescing = "memory_coalescing"
        self.attr_memory_coalescing_bitwidth = "memory_coalescing_bitwidth"
        self.attr_critical_warning = "critical_warning"
        self.attr_is_read = "is_read"
        self.attr_is_write = "is_write"
        self.attr_display_name = "display_name"
        self.attr_ii_violation = "II_violation"
        self.attr_ii_reason = "reason"
        self.attr_ii_solution = "solution"
        self.attr_dep_var = "possible_dep_var"
        self.attr_node_priority = "node_priority"

        self.attr_dead_code = "dead_code"

        self.attr_ap_clk = "ap_clk"
        self.attr_perf_estimate = "estimate"
        self.attr_perf_target = "target"
        self.attr_perf_uncertain = "uncertainty"
        self.attr_target_freq = "Target_FREQ"
        self.attr_estimate_freq = "Estimate_FREQ"
        self.attr_coarse_pipeline = "coarse_grained_pipeline"
        self.attr_multiple_instance = "multiple-instance"
        self.attr_multi_inst = "multi_inst"
        self.attr_multi_instance = "multi_instance"
        self.attr_module_order = "order"
        # attribute value
        self.value_interface = "interface"
        self.value_loop = "loop"
        self.value_function = "function"
        self.value_call = "callfunction"
        self.value_burst = "burst"
        self.value_kernel = "kernel"
        self.value_array = "array"
        self.value_scalar = "scalar"
        self.value_stmt = "stmt"
        self.value_is_skipped_node = "is_skipped_node"
        self.value_before = "before"
        self.value_after = "after"
        self.value_burst = "burst"
        self.value_dead_code = "is_dead_code"
        self.value_major = "major"
        self.value_minor = "minor"
        # messages related
        self.msg_tile = "tiling_loop"
        self.msg_tile_factor = "tile_factor"
        self.msg_flattened = "flattened"
        self.msg_merlin_flatten = "merlin_flatten"
        self.msg_inlined = "inlined"
        self.msg_flatten_loop = "flatten_loop"

        # rose pass name
        self.pass_id_update = "id_update"
        self.pass_gen_hierarchy = "gen_hierarchy"
        self.pass_gen_token_id = "gen_token_id"

        # log file
        self.log_hierarchy = "gen_hierarchy.log"
        self.log_message_report = "msg_report.log"
        self.log_id_update = "id_update.log"
        self.log_gen_token = "gen_token.log"
        self.log_vivado_hls = "vivado_hls.log"
        self.log_total_message = "total_message.log"
        # binary name
        self.bin_mars_opt = "mars_opt"
        self.bin_mars_opt_org = "mars_opt_org"
        self.bin_merlin_safe_exec = "merlin_safe_exec"

        # directory name
        self.dir_gen_token = "gen_token"
        self.dir_perf_est = "perf_est"
        self.dir_refer_induct = "refer_induct"
        self.dir_final_report = "final_report"
        self.dir_target_code = "lc"
        self.dir_source_code = "src"
        self.dir_vendor_report = "vendor_report"
        self.dir_metadata = "metadata"
        self.file_directive = "directive.xml"

        # others
        self.run = "run"
        self.rose_succeed = "rose_succeed"
        self.rose_timeout_read_files = "60"
        self.file_bc = "a.o.3.0.bc"
        self.merlin_note = "__merlin_note"

# if __name__ == '__main__':
#     AA = MerlinReportSetting()
#     LIST_OUT = AA.get_attributes_set("loop")
#     print(AA.json_merlin_rpt)
#     print(AA.xml_impl_tool)
