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


#############################################################
# Backend flow script
#
# Filename    : merlin_flow/scripts/frontend/merlin_frontend_top.pl
# Description : This is the top level sripts for the frontend design flow
# Usage       :
# Owner(s)    : Han
# version     : 01-11-2018 Create file and basic function
##############################################################

#---------------------------------------------------------------------------------------------#
# 0. environment and scripts preparation
#---------------------------------------------------------------------------------------------#

my $MERLIN_COMPILER_HOME;
my $library_path;
my $src_dir;
my $dst_dir;
BEGIN {
    #---------------------------------------------------------------------------------------------#
    # sub function
    #---------------------------------------------------------------------------------------------#
    sub exit_printf {
        printf "Correct Usage  : \n";
        printf "    -src : source code directory, must contain user source code and xml file\n";
        printf "    -dst : output target directory\n";
        printf "\n";
        exit;
    }

    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    for (my $i = 0; $i < @ARGV; $i+=1) {
        if($ARGV[$i] eq "-src") {
            if($ARGV[$i+1] !~ /^-.*/) {
                $src_dir = $ARGV[$i+1];
            }
        }
    }

    our $xml_dir = "";
    if($src_dir eq "") {
        $xml_dir = "./";
    } else {
        $xml_dir = $src_dir;
    }
    if(not -e "$xml_dir/directive.xml") {
        print "ERROR: [MERCC-3012] Can not find directive.xml.\n";
        exit_printf();
    }
}

use strict;
use warnings;
use lib $library_path;
use merlin;
use message;
use global_var;
#---------------------------------------------------------------------------------------------#
# check input argument
#---------------------------------------------------------------------------------------------#
for (my $i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] =~ /^-.*/) {
        if($ARGV[$i] eq "-src") {
        } elsif($ARGV[$i] eq "-dst") {
        } else {
            MSG_E_3015($ARGV[$i]);
            exit_printf();
        }
    }
}
my $count_src  = 0;
my $count_dst  = 0;
for (my $i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] eq "-src") {
        if($ARGV[$i+1] !~ /^-.*/) {
            $src_dir = $ARGV[$i+1];
            $i=$i+1;
            if($count_src > 0) {
                MSG_E_3008("-src");
                exit_printf();
            }
            $count_src++;
        }
    } elsif($ARGV[$i] eq "-dst") {
        if($ARGV[$i+1] !~ /^-.*/) {
            $dst_dir = $ARGV[$i+1];
            $i=$i+1;
            if($count_dst > 0) {
                MSG_E_3008("-dst");
                exit_printf();
            }
            $count_dst++;
        }
    }
}

if($src_dir eq "") {
    MSG_E_3011("-src");
    exit_printf();
}
if($dst_dir eq "") {
    MSG_E_3011("-dst");
    exit_printf();
}
if(not -e $src_dir) {
    MSG_E_3010($src_dir);
    exit_printf();
}
if(not -e "$src_dir/directive.xml") {
    MSG_E_3012();
    exit_printf();
}


my $curr_pwd = `pwd`;
my $dst_pwd = "";
my $src_pwd = "";
if(-e $src_dir) {
    $src_pwd = `cd $src_dir; pwd;`;
} else {
    exit;
}
if(-e $dst_dir) {
    $dst_pwd = `cd $dst_dir; pwd;`;
} else {
    exit;
}
if($src_pwd eq $dst_pwd) {
} elsif($curr_pwd eq $dst_pwd) {
    run_command "cd $dst_dir; rm -rf * .* >& /dev/null";
} else {
    run_command "rm -rf $dst_dir; mkdir $dst_dir";
}

system "cp -r $src_dir/* $dst_dir >& /dev/null";

run_command "rm -rf .merlin_flow_has_error .cmost_flow_has_error";

my $naive_hls = "";
if($xml_opt_effort eq "baseline")   { $naive_hls = "on"; }
if($xml_opt_effort eq "")           { $xml_opt_effort = "standard"; }

my $bitwidth_opt = "";
if ($xml_memory_coalescing ne "") { $bitwidth_opt = $xml_memory_coalescing; }

#---------------------------------------------------------------------------------------------#
# main preprocess
#---------------------------------------------------------------------------------------------#
# systolic_array related
# FIXME hot to test systolic array
# TODO need more testing
my $systolic_home = "${MERLIN_COMPILER_HOME}/optimizers/systolic/";
#my $systolic_cmd  = "mars_perl ${MERLIN_COMPILER_HOME}/optimizers/systolic//scripts/systolic_flow.pl";
my $systolic_cmd  = "mars_perl ${MERLIN_COMPILER_HOME}/optimizers/systolic/scripts/systolic_flow.pl templates_v0_general mm";
#if ($xml_systolic_array eq "on") {
#    system "echo 0 > $ENV{'MERLIN_CURR_PROJECT'}/implement/.merlin_pass_idx";
#    system "mkdir src/";
#    system "cp -r ../$prj_src_org/* src";
#
#    my $additional ="";
#    $additional = "-impl_num $xml_impl_num";
#    system "$systolic_cmd $additional";
#
#    system "rm -rf ../../export/";
#    system "mkdir  ../../export/";
#    system "cp -r opencl  ../../export/";
#    system "cp -r opt ../../export/";
#    exit;
#}
#
if ($xml_systolic_array eq "on") {
    my $additional ="";
	$additional = "-impl_num $xml_impl_num";
	system "$systolic_cmd $additional";
    system "cp -r run_systolic_extract/sys_cfg/* ../export/";
	system "make -C ../export/lib_gen lib_gen";
	system "cp ../export/lib_gen/__merlinhead_kernel_top.h ../../pkg/";
	system "cp ../export/lib_gen/bin/libkernel.so ../../pkg/";
	exit;
}


my $add_cfg = "";
my $additional = "";
my $bb_additional = "";
my $curr_additional = "";
my $fg_additional = "";
my $pp_additional = "";
my $rd_additional = "";
my $shared_additional = "";
my $burst_additional = "";
my $fg_burst_additional = "";
my $cg_burst_additional = "";
$additional = " -a impl_tool=$xml_impl_tool -a tool_version=$xml_tool_version -a effort=$xml_opt_effort";
if ( $xml_aggressive_opt_for_c eq "on")     { $additional .= " -a aggressive_opt_for_c"; }
if ( $xml_aggressive_opt_for_cpp eq "on")   { $additional .= " -a aggressive_opt_for_cpp"; }
if ( $xml_report_time eq "on")              { $additional .= " -a report_time=on"; }
if ( $xml_generate_l2_api ne "off")         { $additional .= " -a generate_l2_api=on"; }
if ( $xml_default_initiation_interval ne "") { $additional .= " -a default_initiation_interval=$xml_default_initiation_interval"; }
if ( $xml_kernel_separate ne "off")         { $add_cfg .= " -src_list $global_kernel_list"; }
my $additional_with_naive = $additional;
if ( $naive_hls eq "on") {   $additional_with_naive .= " -a naive_hls"; }
if ($xml_memory_coalescing ne "") {
    $bitwidth_opt = $xml_memory_coalescing;
} else {
    $bitwidth_opt = $xml_bitwidth_opt;
}
#if ( $debug eq "on" ) {
#   $additional .= " -a debug=on";
#}

if ($xml_impl_tool eq "sdaccel") {

    $burst_additional = $additional;
    if ( $xml_burst_total_size_threshold ne "")  { $burst_additional .= " -a burst_total_size_threshold=$xml_burst_total_size_threshold"; }
    if ( $xml_burst_single_size_threshold ne "") { $burst_additional .= " -a burst_single_size_threshold=$xml_burst_single_size_threshold"; }
    if ( $xml_burst_lifting_size_threshold ne ""){ $burst_additional .= " -a burst_lifting_size_threshold=$xml_burst_lifting_size_threshold"; }
    $burst_additional .= " -a bus_length_threshold=$xml_bus_length_threshold";

    my $midend_pp_additional = $additional;
    if ($xml_auto_fg_opt eq "on" )      { $midend_pp_additional = "-a auto_fg_opt $midend_pp_additional"; }
    if ($xml_auto_fgpip_opt ne "off" )  { $midend_pp_additional = "-a auto_fgpip_opt $midend_pp_additional"; }
    if ($xml_auto_fgpar_opt ne "off" )  { $midend_pp_additional = "-a auto_fgpar_opt $midend_pp_additional"; }
 	  if ($xml_auto_fgpar_threshold ne ""){ $midend_pp_additional .= " -a auto_fgpar_threshold=$xml_auto_fgpar_threshold $midend_pp_additional"; }

    run_command("cd $dst_dir; $cmd_frontend_pass . -p midend_preprocess $add_cfg -a '$midend_pp_additional'");
    check_error($dst_dir);
 
    my $auto_parallel_additional = $additional;
    if ( $xml_auto_parallel ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p auto_parallel $add_cfg -a '$auto_parallel_additional'");
        check_error($dst_dir);
    }

    my $stream_prefetch_additional = "$additional -a bus_bitwidth=$xml_bus_bitwidth";
    $stream_prefetch_additional .= " -a burst_single_size_threshold=$xml_burst_single_size_threshold";
    if( $xml_stream_prefetch ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p stream_prefetch $add_cfg -a '$stream_prefetch_additional'");
        check_error($dst_dir);
    }
    
    if ( $xml_memory_burst ne "off" ) {  
      run_command("cd $dst_dir; $cmd_frontend_pass . -p memory_burst $add_cfg -a '$burst_additional'");  
      check_error($dst_dir); 
    }
   
    $shared_additional = "-a '$additional'";
    if ( $xml_delinearization ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p delinearization $add_cfg $shared_additional");
        check_error($dst_dir);
    }

    if ( $xml_coarse_grained_parallel ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p coarse_parallel $add_cfg $shared_additional");
        check_error($dst_dir);
    }

    $bb_additional = "$additional";
    if ($xml_bus_bitwidth ne "") {
      $bb_additional .=  " -a bus_bitwidth=$xml_bus_bitwidth";
    } else {
      $xml_bus_bitwidth = "512";
    }
    $bb_additional .= " -a bus_length_threshold=$xml_bus_length_threshold";
    $bb_additional .= " -a lift_fine_grained_cache=$xml_lift_fine_grained_cache";
    if ($bitwidth_opt ne "off") {
        # ZP: 20180126
        # Copy and decryption for memory coalescing libraries
        run_command("rm -rf $dst_dir/tmp_coalescing $dst_dir/decryped_coalescing");
        run_command("mkdir $dst_dir/tmp_coalescing $dst_dir/decryped_coalescing");
        if (-e "$MERLIN_COMPILER_HOME/mars-gen/drivers/code_transform/coalescing/") {
            run_command("cp -r $MERLIN_COMPILER_HOME/mars-gen/drivers/code_transform/coalescing/* $dst_dir/tmp_coalescing");
        }
        run_command "cd $dst_dir/tmp_coalescing; find . -path '*' -name \"*.*\" -exec mars_copy '{}' ../decryped_coalescing/{} \\; >& /dev/null";
        run_command("cd $dst_dir; $cmd_frontend_pass . -p bitwidth_opt $add_cfg -a '$bb_additional'");
        if (-e "$MERLIN_COMPILER_HOME/mars-gen/drivers/code_transform/coalescing/" and 
           -e "$dst_dir/merlin_coalescing_header_file.list") {
            use File::Copy qw(copy);
            my $dst_dir = ".";
            my $header_list_name = "$dst_dir/merlin_coalescing_header_file.list";
            if (open(my $header_list, '<', $header_list_name)) {
                while (my $header = <$header_list>) {
                    chomp $header;
                    copy "$dst_dir/decryped_coalescing/$header", "$dst_dir/$header";
                }
            } else {
                print "cannot open file $header_list_name";
            }
        }
#run_command("rm -rf $dst_dir/tmp_coalescing $dst_dir/decryped_coalescing");
    }
    check_error($dst_dir);

    $rd_additional = "$shared_additional";
    if ( $xml_multi_layer_reduction eq "on") {
       $rd_additional = "-a '-a scheme=1 $additional'";
    }
    if ( $xml_reduction_opt eq "on" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p reduction $add_cfg $rd_additional");
    }
    check_error($dst_dir);

    if ( $xml_reduction_general ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p reduction_general $add_cfg $rd_additional");
    }
    
    check_error($dst_dir);

    if ( $xml_coarse_grained_pipeline ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p coarse_pipeline $add_cfg $shared_additional");
    }
    check_error($dst_dir);

    $fg_additional = ""; # "$shared_additional";
    if ($xml_loop_flatten eq "on") {
       $fg_additional = "-a loop_flatten=on";
    }
    if ($xml_dual_port_mem eq "off" ) {
       $fg_additional = "-a dual_port_mem=off $fg_additional";
    }
 
    if ( $xml_index_transform ne "") {
       $fg_additional = " -a index_transform=$xml_index_transform $fg_additional";
    }
    if ( $xml_dependency_resolve ne "") {
       $fg_additional = " -a dependency_resolve=$xml_dependency_resolve $fg_additional";
    }
    if ( $xml_partition_xilinx_threshold  ne "") {
       $fg_additional = " -a partition_xilinx_threshold=$xml_partition_xilinx_threshold $fg_additional";
    }
    if ( $xml_partition_heuristic_threshold  ne "") {
       $fg_additional = " -a partition_heuristic_threshold=$xml_partition_heuristic_threshold $fg_additional";
    }
    $fg_additional = "-a '$fg_additional $additional'";
    if ( $xml_fg_pipeline_parallel ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p loop_parallel $add_cfg $fg_additional");
    }
    check_error($dst_dir);

    if ( $xml_auto_func_inline ne "off" ) {
        my $auto_func_inline_additional = "-a '-a auto_func_inline=$xml_auto_func_inline $additional'";
        run_command("cd $dst_dir; $cmd_frontend_pass . -p auto_func_inline $add_cfg $auto_func_inline_additional");
    }
    check_error($dst_dir);

    if ( $xml_structural_func_inline ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p structural_func_inline $add_cfg $fg_additional");
    }
    check_error($dst_dir);

    if ( $xml_function_inline_by_pragma_HLS ne "off" ) {
        run_command("cd $dst_dir; $cmd_frontend_pass . -p function_inline_by_pragma_HLS $add_cfg $fg_additional");
    }
    check_error($dst_dir);
}





