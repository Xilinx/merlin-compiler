
BEGIN {
    #$|=1;   #turn on auto reflush for stdout
}
#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
my $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
#my $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
#use lib "$library_path";
#use merlin;
package global_var;
use strict;
use warnings;

# get key value from xml file
sub get_value_from_xml {
#$MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    my $bin_dir = "$MERLIN_COMPILER_HOME/build/bin";
    # stuff
    my $key = $_[0];
    my $xml = $_[1];
    #print "$bin_dir/set_cfg -i get $xml $key";
    my $value = `$bin_dir/set_cfg -i get $xml $key`;
    my $exit_code = $?;
    if($exit_code !=0)
    {
        print "Command $bin_dir/set_cfg -i get $xml $key failed with an exit code of $exit_code.\n";
        exit($exit_code >> 8);
    }
    chomp($value);
    #printf "key = $_[0], value = $value, xml = $_[1]\n";
    return $value;
}

#extract attribute from xml
my $directive_dir = $main::xml_dir;
my $directive;
if($directive_dir eq "") {
    $directive = "";
} else {
    $directive = "$directive_dir/directive.xml";
}
if($directive ne "") {
    $directive = $directive;
} elsif(-e "spec/directive.xml") {
    $directive = "spec/directive.xml";
} elsif(-e "directive.xml") {
    $directive = "directive.xml";
} elsif(-e "../directive.xml") {
    $directive = "../directive.xml";
} elsif(-e "../../directive.xml") {
    $directive = "../../directive.xml";
} elsif(-e "../../../directive.xml") {
    $directive = "../../../directive.xml";
} else {
    $directive = "directive.xml";
}
#printf "directive = $directive\n";

use Exporter 'import';
our @ISA = 'Exporter';
our @EXPORT = qw($begin

        $xml_platform_name
        $xml_preprocess
        $xml_memory_burst
        $xml_value_prop
		$xml_stream_prefetch
        $xml_burst_total_size_threshold
        $xml_burst_single_size_threshold
        $xml_burst_lifting_size_threshold
        $xml_cg_memory_burst
        $xml_delinearization
        $xml_memory_coalescing
        $xml_bus_length_threshold
        $xml_lift_fine_grained_cache
        $xml_memory_partition
        $xml_partition_xilinx_threshold
        $xml_partition_heuristic_threshold
        $xml_bitwidth_opt
        $xml_function_inline
        $xml_function_inline_by_pragma_HLS
        $xml_auto_func_inline
        $xml_auto_dim_interchange
        $xml_auto_false_dependence
        $xml_auto_register
        $xml_auto_register_size_threshold
        $xml_auto_reshape_threshold
        $xml_auto_memory_partition
        $xml_reduction_opt
        $xml_reduction_general
        $xml_line_buffer
        $xml_dependency_resolve
        $xml_auto_parallel
        $xml_structural_func_inline
        $xml_coarse_grained_pipeline
        $xml_coarse_grained_parallel
        $xml_fg_pipeline_parallel
        $xml_naive_hls
        $xml_aggressive_opt_for_c
        $xml_aggressive_opt_for_cpp
        $xml_bus_bitwidth
        $xml_multi_layer_reduction
        $xml_loop_flatten
        $xml_dual_port_mem
        $xml_index_transform
        $xml_auto_fg_opt
        $xml_auto_fgpip_opt
        $xml_auto_fgpar_opt
        $xml_auto_fgpar_threshold
        $xml_default_initiation_interval
        $xml_false_dep_removal
        $xml_comm_dead_remove
        $xml_comm_rename
        $xml_comm_sync
        $xml_comm_refine
        $xml_comm_token
        $xml_comm_opt
        $xml_loop_dist
        $xml_loop_tiling
        $xml_loop_tile
        $xml_memory_reduce
        $xml_impl_tool
        $xml_tool_version
        $xml_opt_effort
        $xml_pure_kernel
        $xml_extern_c
        $xml_report_time
        $xml_kernel_separate
        $xml_skip_syntax_check
        $xml_systolic_array
        $xml_array_linearize
        $xml_array_linearize_simple
        $xml_sycl
        $xml_impl_num
        $xml_debug_mode
        $xml_debug_info
        $xml_include_path
        $xml_include_path_tb
        $xml_merlincc
        $xml_merlincc_debug
        $xml_macro_define
        $xml_cstd
        $xml_generate_l2_api
        $xml_enable_cpp
        $xml_libonly
        $xml_auto_dse
        $xml_dse_debug
        $xml_volatile_flag
        $xml_explicit_bundle
        $xml_runtime
        $xml_vendor_options
        $xml_pre_hls_tcl
        $xml_unsafe_math
        $xml_kestrel
        $xml_host_args
        $xml_server
        $xml_api
        $xml_kernel_frequency
        $xml_warning
        $xml_encrypt
        $xml_mem_size_limitation
        $xml_skip_syncheck
        $xml_aligned_struct_decomp
        $xml_hls
        $xml_tb
        $xml_cflags
        $xml_ldflags
        $xml_exec_argv
        $xml_report_type
        $xml_ap_int_max_w
        $xml_pcie_transfer_opt
        $xml_evaluate

        $dir_scripts
        $dir_merlin_flow
        $dir_lib
        $dir_merlincc
        $dir_msg_report
        $dir_others
        $dir_platform
        $dir_drivers
        $dir_cmost_headers
        $dir_code_transform
        $dir_opencl
        $dir_api
        $dir_fcs_runtime
        $dir_template
        $dir_default
        $dir_merlin_lib
        $dir_legacy
        $dir_source_opt

        $prj_export
        $prj_export_host
        $prj_export_lib_gen
        $prj_export_kernel
        $prj_export_lc
        $prj_export_temp
        $prj_spec
        $prj_report
        $prj_pkg
        $prj_implement
        $prj_code_transform
        $prj_aux
        $prj_exec
        $prj_metadata
        $prj_frontend
        $prj_midend
        $prj_backend
        $prj_run_sim
        $prj_run_ref
        $prj_run_opt
        $prj_hls
        $prj_phys
        $prj_host
        $prj_src
        $prj_dse
        $prj_src_org
        $prj_src_opt
        $prj_opencl
        $prj_final_code_gen

        $cmd_frontend
        $cmd_midend
        $cmd_backend
        $cmd_auto_dse
        $cmd_frontend_pass
        $cmd_final_code_gen
        $cmd_cmostcc
        $cmd_report_summary
        $cmd_create_dir
        $cmd_internal_platform
        $cmd_merlin_flow_opt

        $global_vendor_error
        $global_passwd
        $global_kernel_file_json
        $global_task_file
        $global_kernel_list
        $global_vivado_log
        $global_vivado_hls_log
        $global_solution_log
        $global_vitis_hls_log
        $global_kernel_top_log
        $global_kernel_util_routed
        $global_system_xtxt
        $global_platform_routed
        $global_fast_dse
        $global_fast_dse_perf
        $global_fast_dse_res
        $global_gen_token
        $global_change

        run_command

        $end);

our (   $begin,

        $xml_platform_name                  ,
        $xml_preprocess                     ,
        $xml_memory_burst                   ,
        $xml_value_prop                     ,
        $xml_stream_prefetch          ,
        $xml_burst_total_size_threshold     ,
        $xml_burst_single_size_threshold    ,
        $xml_burst_lifting_size_threshold   ,
        $xml_cg_memory_burst                ,
        $xml_delinearization                ,
        $xml_memory_coalescing              ,
        $xml_bus_length_threshold           ,
        $xml_lift_fine_grained_cache        ,
        $xml_memory_partition               ,
        $xml_partition_xilinx_threshold     ,
        $xml_partition_heuristic_threshold  ,
        $xml_bitwidth_opt                   ,
        $xml_function_inline                ,
        $xml_function_inline_by_pragma_HLS  ,
        $xml_auto_func_inline               ,
        $xml_auto_dim_interchange           ,
        $xml_auto_false_dependence          ,
        $xml_auto_register                  ,
        $xml_auto_register_size_threshold   ,
        $xml_auto_reshape_threshold         ,
        $xml_auto_memory_partition          ,
        $xml_reduction_opt                  ,
        $xml_reduction_general              ,
        $xml_line_buffer                    ,
        $xml_dependency_resolve             ,
        $xml_auto_parallel                  ,
        $xml_structural_func_inline         ,
        $xml_coarse_grained_pipeline        ,
        $xml_coarse_grained_parallel        ,
        $xml_fg_pipeline_parallel           ,
        $xml_naive_hls                      ,
        $xml_aggressive_opt_for_c           ,
        $xml_aggressive_opt_for_cpp         ,
        $xml_bus_bitwidth                   ,
        $xml_multi_layer_reduction          ,
        $xml_loop_flatten                   ,
        $xml_dual_port_mem                  ,
        $xml_index_transform                ,
        $xml_auto_fg_opt                    ,
        $xml_auto_fgpip_opt                 ,
        $xml_auto_fgpar_opt                 ,
        $xml_auto_fgpar_threshold           ,
        $xml_false_dep_removal              ,
        $xml_comm_dead_remove               ,
        $xml_comm_rename                    ,
        $xml_comm_sync                      ,
        $xml_comm_refine                    ,
        $xml_comm_token                     ,
        $xml_comm_opt                       ,
        $xml_loop_dist                      ,
        $xml_loop_tiling                    ,
        $xml_loop_tile                      ,
        $xml_memory_reduce                  ,
        $xml_impl_tool                      ,
        $xml_tool_version                   ,
        $xml_opt_effort                     ,
        $xml_pure_kernel                    ,
        $xml_default_initiation_interval    ,
        $xml_extern_c                       ,
        $xml_report_time                    ,
        $xml_kernel_separate                ,
        $xml_skip_syntax_check              ,
        $xml_systolic_array                 ,
        $xml_array_linearize                ,
        $xml_array_linearize_simple         ,
        $xml_sycl                           ,
        $xml_impl_num                       ,
        $xml_debug_mode                     ,
        $xml_debug_info                     ,
        $xml_include_path                   ,
        $xml_include_path_tb                ,
        $xml_merlincc                       ,
        $xml_merlincc_debug                 ,
        $xml_macro_define                   ,
        $xml_cstd                           ,
        $xml_generate_l2_api                ,
        $xml_enable_cpp                     ,
        $xml_libonly                        ,
        $xml_auto_dse                       ,
        $xml_dse_debug                      ,
        $xml_volatile_flag                  ,
        $xml_explicit_bundle                ,
        $xml_runtime                        ,
        $xml_vendor_options                 ,
        $xml_pre_hls_tcl                    ,
        $xml_unsafe_math                    ,
        $xml_kestrel                        ,
        $xml_host_args                      ,
        $xml_server                         ,
        $xml_api                            ,
        $xml_kernel_frequency               ,
        $xml_warning                        , 
        $xml_encrypt                        ,
        $xml_mem_size_limitation            ,
        $xml_skip_syncheck                  ,
        $xml_aligned_struct_decomp          ,
        $xml_hls                            ,
        $xml_tb                             ,
        $xml_cflags                         ,
        $xml_ldflags                        ,
        $xml_exec_argv                      ,
        $xml_report_type                    ,
        $xml_ap_int_max_w                   ,
        $xml_pcie_transfer_opt              ,
        $xml_evaluate                       ,

        $dir_scripts        ,
        $dir_merlin_flow    ,
        $dir_lib            ,
        $dir_merlincc       ,
        $dir_msg_report     ,
        $dir_others         ,
        $dir_platform       ,
        $dir_drivers        ,
        $dir_cmost_headers  ,
        $dir_code_transform ,
        $dir_opencl         ,
        $dir_api            ,
        $dir_fcs_runtime    ,
        $dir_template       ,
        $dir_default        ,
        $dir_merlin_lib     ,
        $dir_legacy         ,
        $dir_source_opt     ,

        $prj_export         ,
        $prj_export_host    ,
        $prj_export_lib_gen ,
        $prj_export_kernel  ,
        $prj_export_lc      ,
        $prj_export_temp    ,
        $prj_spec           ,
        $prj_report         ,
        $prj_pkg            ,
        $prj_implement      ,
        $prj_code_transform ,
        $prj_aux            ,
        $prj_exec           ,
        $prj_metadata       ,
        $prj_frontend       ,
        $prj_midend         ,
        $prj_backend        ,
        $prj_run_sim        ,
        $prj_run_ref        ,
        $prj_run_opt        ,
        $prj_hls            ,
        $prj_phys           ,
        $prj_host           ,
        $prj_src            ,
        $prj_dse            ,
        $prj_src_org        ,
        $prj_src_opt        ,
        $prj_opencl         ,
        $prj_final_code_gen ,

        $cmd_frontend       ,
        $cmd_midend         ,
        $cmd_backend        ,
        $cmd_auto_dse       ,
        $cmd_frontend_pass  ,
        $cmd_final_code_gen ,
        $cmd_cmostcc        ,
        $cmd_report_summary ,
        $cmd_create_dir     ,
        $cmd_internal_platform ,
        $cmd_merlin_flow_opt,

        $global_vendor_error,

        $global_passwd      ,
        $global_kernel_file_json,
        $global_task_file,
        $global_kernel_list,
        $global_vivado_log  ,
        $global_vivado_hls_log,
        $global_solution_log,
        $global_vitis_hls_log,
        $global_kernel_top_log,
        $global_kernel_util_routed,
        $global_system_xtxt,
        $global_platform_routed,
        $global_fast_dse,
        $global_fast_dse_perf,
        $global_fast_dse_res,
        $global_gen_token,
        $global_change,

        $end);

$begin = $MERLIN_COMPILER_HOME;
$end = "end";

$xml_platform_name                  = get_value_from_xml("platform_name",                   "$directive");
$xml_preprocess                     = get_value_from_xml("preprocess",                      "$directive");
$xml_memory_burst                   = get_value_from_xml("memory_burst",                    "$directive");
$xml_value_prop                     = get_value_from_xml("value_prop",                      "$directive");
$xml_stream_prefetch                = get_value_from_xml("stream_prefetch",                 "$directive");
$xml_burst_total_size_threshold     = get_value_from_xml("burst_total_size_threshold",      "$directive");
$xml_burst_single_size_threshold    = get_value_from_xml("burst_single_size_threshold",     "$directive");
$xml_burst_lifting_size_threshold   = get_value_from_xml("burst_lifting_size_threshold",    "$directive");
$xml_cg_memory_burst                = get_value_from_xml("cg_memory_burst",                 "$directive");
$xml_delinearization                = get_value_from_xml("delinearization",                 "$directive");
$xml_memory_coalescing              = get_value_from_xml("memory_coalescing",               "$directive");
$xml_bus_length_threshold           = get_value_from_xml("bus_length_threshold",            "$directive");
$xml_lift_fine_grained_cache        = get_value_from_xml("lift_fine_grained_cache",         "$directive");
$xml_memory_partition               = get_value_from_xml("memory_partition",                "$directive");
$xml_partition_xilinx_threshold     = get_value_from_xml("partition_xilinx_threshold",      "$directive");
$xml_partition_heuristic_threshold  = get_value_from_xml("partition_heuristic_threshold",   "$directive");
$xml_bitwidth_opt                   = get_value_from_xml("biwidth_opt",                     "$directive");
$xml_function_inline                = get_value_from_xml("function_inline",                 "$directive");
$xml_function_inline_by_pragma_HLS  = get_value_from_xml("function_inline_by_pragma_HLS",   "$directive");
$xml_auto_func_inline               = get_value_from_xml("auto_func_inline",                "$directive");
$xml_auto_dim_interchange           = get_value_from_xml("auto_dim_interchange",            "$directive");
$xml_auto_false_dependence          = get_value_from_xml("auto_false_dependence",           "$directive");
$xml_auto_register                  = get_value_from_xml("auto_register",                   "$directive");
$xml_auto_register_size_threshold   = get_value_from_xml("auto_register_size_threshold",    "$directive");
$xml_auto_reshape_threshold         = get_value_from_xml("auto_reshape_threshold",          "$directive");
$xml_auto_memory_partition          = get_value_from_xml("auto_memory_partition",           "$directive");
$xml_reduction_opt                  = get_value_from_xml("reduction_opt",                   "$directive");
$xml_reduction_general              = get_value_from_xml("reduction_general",               "$directive");
$xml_line_buffer                    = get_value_from_xml("line_buffer",                     "$directive");
$xml_dependency_resolve             = get_value_from_xml("dependency_resolve",              "$directive");
$xml_auto_parallel      	    = get_value_from_xml("auto_parallel",                   "$directive");
$xml_structural_func_inline         = get_value_from_xml("structural_func_inline",          "$directive");
$xml_coarse_grained_pipeline        = get_value_from_xml("coarse_grained_pipeline",         "$directive");
$xml_coarse_grained_parallel        = get_value_from_xml("coarse_grained_parallel",         "$directive");
$xml_fg_pipeline_parallel           = get_value_from_xml("fg_pipeline_parallel",            "$directive");
$xml_naive_hls                      = get_value_from_xml("naive_hls",                       "$directive");
$xml_aggressive_opt_for_c           = get_value_from_xml("aggressive_opt_for_c",            "$directive");
$xml_aggressive_opt_for_cpp         = get_value_from_xml("aggressive_opt_for_cpp",          "$directive");
$xml_bus_bitwidth                   = get_value_from_xml("bus_bitwidth",                    "$directive");
$xml_multi_layer_reduction          = get_value_from_xml("multi_layer_reduction",           "$directive");
$xml_loop_flatten                   = get_value_from_xml("loop_flatten",                    "$directive");
$xml_dual_port_mem                  = get_value_from_xml("dual_port_mem",                   "$directive");
$xml_index_transform                = get_value_from_xml("index_transform",                 "$directive");
$xml_auto_fg_opt                    = get_value_from_xml("auto_fg_opt",               	    "$directive");
$xml_auto_fgpip_opt                 = get_value_from_xml("auto_fgpip_opt",                  "$directive");
$xml_auto_fgpar_opt                 = get_value_from_xml("auto_fgpar_opt",                  "$directive");
$xml_auto_fgpar_threshold           = get_value_from_xml("auto_fgpar_threshold",            "$directive");
$xml_false_dep_removal              = get_value_from_xml("false_dep_removal",               "$directive");
$xml_comm_dead_remove               = get_value_from_xml("comm_dead_remove",                "$directive");
$xml_comm_rename                    = get_value_from_xml("comm_rename",                     "$directive");
$xml_comm_sync                      = get_value_from_xml("comm_sync",                       "$directive");
$xml_comm_refine                    = get_value_from_xml("comm_refine",                     "$directive");
$xml_comm_token                     = get_value_from_xml("comm_token",                      "$directive");
$xml_comm_opt                       = get_value_from_xml("comm_opt",                        "$directive");
$xml_loop_dist                      = get_value_from_xml("loop_dist",                       "$directive");
$xml_loop_tiling                    = get_value_from_xml("loop_tiling",                     "$directive");
$xml_loop_tile                      = get_value_from_xml("loop_tile",                       "$directive");
$xml_memory_reduce                  = get_value_from_xml("memory_reduce",                   "$directive");
$xml_impl_tool                      = get_value_from_xml("implementation_tool",             "$directive");
$xml_tool_version                   = get_value_from_xml("tool_version",                    "$directive");
$xml_opt_effort                     = get_value_from_xml("opt_effort",                      "$directive");
$xml_pure_kernel                    = get_value_from_xml("pure_kernel",                     "$directive");
$xml_default_initiation_interval    = get_value_from_xml("default_initiation_interval",     "$directive");
$xml_extern_c                       = get_value_from_xml("extern_c",                        "$directive");
$xml_report_time                    = get_value_from_xml("report_time",                     "$directive");
$xml_kernel_separate                = get_value_from_xml("kernel_separate",                 "$directive");
$xml_skip_syntax_check              = get_value_from_xml("skip_syntax_check",               "$directive");
$xml_systolic_array                 = get_value_from_xml("systolic_array",                  "$directive");
$xml_array_linearize                = get_value_from_xml("array_linearize",                 "$directive");
$xml_array_linearize_simple         = get_value_from_xml("array_linearize_simple",          "$directive");
$xml_sycl                           = get_value_from_xml("sycl",                            "$directive");
$xml_impl_num                       = get_value_from_xml("impl_num",                        "$directive");
$xml_debug_mode                     = get_value_from_xml("ihatebug",                        "$directive");
$xml_debug_info                     = get_value_from_xml("debug_info",                      "$directive");
$xml_include_path                   = get_value_from_xml("include_path",                    "$directive");
$xml_include_path_tb                = get_value_from_xml("include_path_tb",                 "$directive");
$xml_merlincc                       = get_value_from_xml("merlincc",                        "$directive");
$xml_merlincc_debug                 = get_value_from_xml("merlincc_debug",                  "$directive");
$xml_macro_define                   = get_value_from_xml("macro_define",                    "$directive");
$xml_cstd                           = get_value_from_xml("cstd",                            "$directive");
$xml_generate_l2_api                = get_value_from_xml("generate_l2_api",                 "$directive");
$xml_enable_cpp                     = get_value_from_xml("enable_cpp",                      "$directive");
$xml_libonly                        = get_value_from_xml("libonly",                         "$directive");
$xml_auto_dse                       = get_value_from_xml("auto_dse",                        "$directive");
$xml_dse_debug                      = get_value_from_xml("dse_debug",                       "$directive");
$xml_volatile_flag                  = get_value_from_xml("volatile_flag",                   "$directive");
$xml_explicit_bundle                = get_value_from_xml("explicit_bundle",                 "$directive");
$xml_runtime                        = get_value_from_xml("runtime",                         "$directive");
$xml_vendor_options                 = get_value_from_xml("vendor_options",                  "$directive");
$xml_pre_hls_tcl                    = get_value_from_xml("pre_hls_tcl",                     "$directive");
$xml_unsafe_math                    = get_value_from_xml("unsafe_math",                     "$directive");
$xml_kestrel                        = get_value_from_xml("kestrel",                         "$directive");
$xml_host_args                      = get_value_from_xml("host_args",                       "$directive");
$xml_server                         = get_value_from_xml("server",                          "$directive");
$xml_api                            = get_value_from_xml("api",                             "$directive");
$xml_kernel_frequency               = get_value_from_xml("kernel_frequency",                "$directive");
$xml_warning                        = get_value_from_xml("warning",                         "$directive");
$xml_encrypt                        = get_value_from_xml("encrypt",                         "$directive");
$xml_mem_size_limitation            = get_value_from_xml("mem_size_limitation",             "$directive");
$xml_skip_syncheck                  = get_value_from_xml("skip_syncheck",                   "$directive");
$xml_aligned_struct_decomp          = get_value_from_xml("aligned_struct_decomp",           "$directive");
$xml_hls                            = get_value_from_xml("hls",                             "$directive");
$xml_tb                             = get_value_from_xml("tb",                              "$directive");
$xml_cflags                         = get_value_from_xml("cflags",                          "$directive");
$xml_ldflags                        = get_value_from_xml("ldflags",                         "$directive");
$xml_exec_argv                      = get_value_from_xml("exec_argv",                       "$directive");
$xml_report_type                    = get_value_from_xml("report_type",                     "$directive");
$xml_ap_int_max_w                   = get_value_from_xml("ap_int_max_w",                    "$directive");
$xml_pcie_transfer_opt              = get_value_from_xml("pcie_transfer_opt",               "$directive");
$xml_evaluate                       = get_value_from_xml("evaluate",                        "$directive");

# scripts directory
my $platform = $xml_impl_tool;
$dir_scripts        = "$MERLIN_COMPILER_HOME/mars-gen/scripts";
$dir_merlin_flow    = "$MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow";
$dir_lib            = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
$dir_merlincc       = "$MERLIN_COMPILER_HOME/mars-gen/scripts/merlincc";
$dir_msg_report     = "$MERLIN_COMPILER_HOME/mars-gen/scripts/msg_report";
$dir_others         = "$MERLIN_COMPILER_HOME/mars-gen/scripts/others";
$dir_platform       = "$MERLIN_COMPILER_HOME/mars-gen/scripts/$platform";
$dir_drivers        = "$MERLIN_COMPILER_HOME/mars-gen/drivers";
$dir_cmost_headers  = "$MERLIN_COMPILER_HOME/mars-gen/drivers/cmost_headers";
$dir_code_transform = "$MERLIN_COMPILER_HOME/mars-gen/drivers/code_transform";
$dir_opencl         = "$MERLIN_COMPILER_HOME/mars-gen/drivers/opencl";
$dir_api            = "$MERLIN_COMPILER_HOME/mars-gen/drivers/opencl/api";
$dir_fcs_runtime    = "$MERLIN_COMPILER_HOME/mars-gen/drivers/opencl/fcs_runtime";
$dir_template       = "$MERLIN_COMPILER_HOME/mars-gen/drivers/opencl/template";
$dir_default        = "$MERLIN_COMPILER_HOME/mars-gen/default";
$dir_merlin_lib     = "$MERLIN_COMPILER_HOME/mars-gen/lib";
$dir_legacy         = "$MERLIN_COMPILER_HOME/legacy";
$dir_source_opt     = "$MERLIN_COMPILER_HOME/source-opt";

# project directory
$prj_export         = "export";
$prj_export_host    = "host";
$prj_export_lib_gen = "lib_gen";
$prj_export_kernel  = "kernel";
$prj_export_lc      = "lc";
$prj_export_temp    = "temp";
$prj_spec           = "spec";
$prj_report         = "report";
$prj_pkg            = "pkg";
$prj_implement      = "implement";
$prj_code_transform = "code_transform";
$prj_aux            = "aux";
$prj_exec           = "exec";
$prj_metadata       = "metadata";
$prj_frontend       = "frontend";
$prj_midend         = "midend";
$prj_backend        = "backend";
$prj_run_sim        = "run_sim";
$prj_run_ref        = "run_ref";
$prj_run_opt        = "run_opt";
$prj_hls            = "hls";
$prj_phys           = "phys";
$prj_host           = "host";
$prj_src            = "src";
$prj_dse            = "dse";
$prj_src_org        = "src_org";
$prj_src_opt        = "src_opt";
$prj_opencl         = "opencl";
$prj_final_code_gen = "final_code_gen";

#command line
$cmd_frontend       = "merlin_frontend";
$cmd_midend         = "merlin_midend";
$cmd_backend        = "merlin_backend";
$cmd_auto_dse       = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/auto_dse.pl";
$cmd_frontend_pass  = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/frontend_pass.pl";
$cmd_final_code_gen = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/final_code_gen.pl";
$cmd_cmostcc        = "mars_perl $MERLIN_COMPILER_HOME/legacy/drivers/pure_cpu_headers/cmostcc/cmostcc.pl";
$cmd_report_summary = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/mars_summarize.pl";
$cmd_create_dir     = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/merlin_create_dir.pl";
$cmd_internal_platform = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/set_internal_platform.pl";
$cmd_merlin_flow_opt = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/merlin_flow_opt.pl";


$global_vendor_error= ".vendor.error";
$global_passwd      = "falconisgreat";
$global_kernel_file_json= "kernel_file.json";
$global_task_file = "task_kernel.json";
$global_kernel_list = "kernel.list";
$global_vivado_log      = "vivado.log";
$global_vivado_hls_log  = "vivado_hls.log";
$global_solution_log  = "solution.log";
$global_vitis_hls_log  = "vitis_hls.log";
$global_kernel_top_log  = "kernel_top.log";
$global_kernel_util_routed = "kernel_util_routed.rpt";
$global_system_xtxt     = "system_estimate.xtxt";
$global_platform_routed = "platform_utilization_routed.rpt";
$global_fast_dse = "fast_dse";
$global_fast_dse_perf = "perf";
$global_fast_dse_res = "res";
$global_gen_token = "org.json";
$global_change = "code_change.json";

sub run_command {
    if($xml_debug_mode eq "debug-level3") {
        print "[" . [caller(0)]->[1] . ":" . [caller(0)]->[2] . "]";
        print "# $_[0]\n";
    }
    my $exit_code = system("$_[0]");

#    if($exit_code!=0)
#    {
#        if ($xml_debug_mode eq "debug-level2") {
#            print "Command $_[0] failed with an exit code of $exit_code.\n";
#        }
#    }
    return ($exit_code >> 8);
}
