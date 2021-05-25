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


##############################################################
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

#if ($xml_skip_syntax_check eq "") {
#    if ($xml_impl_tool eq "sdaccel") {
#        $xml_skip_syntax_check = "off";
#    }
#}


if($xml_opt_effort eq "") { $xml_opt_effort = "standard"; }

#---------------------------------------------------------------------------------------------#
# main preprocess
#---------------------------------------------------------------------------------------------#
# none step to check syntax
if ($xml_enable_cpp ne "off") {
  system("cd $dst_dir; $cmd_frontend_pass . -p none -a '-a impl_tool=$xml_impl_tool -a skip_syntax_check=$xml_skip_syntax_check'"); 
} else {
  # delay syntax check
  system("cd $dst_dir; $cmd_frontend_pass . -p none -a '-a impl_tool=$xml_impl_tool -a skip_syntax_check=on'"); 
}
check_error($dst_dir);

my $pure_kernel_flow = "off";
if($xml_sycl eq "on" or $xml_pure_kernel eq "on") {
    $pure_kernel_flow = "on";
}
my $hls_flow = "off";
if ($xml_tool_version eq "vitis_hls" or $xml_tool_version eq "vivado_hls") {
  $hls_flow = "on";
}
my $add_cfg .= " -src_list $global_kernel_list";
my $additional = "";
$additional = " -a impl_tool=$xml_impl_tool -a tool_version=$xml_tool_version -a effort=$xml_opt_effort";
if ( $xml_aggressive_opt_for_c eq "on")     { $additional .= " -a aggressive_opt_for_c"; }
if ( $xml_aggressive_opt_for_cpp eq "on")   { $additional .= " -a aggressive_opt_for_cpp"; }
if ( $xml_report_time eq "on")              { $additional .= " -a report_time=on"; }
if ( $xml_generate_l2_api ne "off")         { $additional .= " -a generate_l2_api=on"; }
if ( $xml_debug_mode eq "on" )              { $additional .= " -a debug=on"; }
if ( $xml_api eq "all" )                    { $additional .= " -a api=all"; }
$additional .= " -a pure_kernel=$pure_kernel_flow";
run_command("cd $dst_dir; $cmd_frontend_pass . -p lower_separate $add_cfg -a ' -a skip_syntax_check=$xml_skip_syntax_check -a enable_cpp=$xml_enable_cpp $additional'");  
check_error($dst_dir);
if ( -e "merlin_type_define.cpp" )  { system ("cd $dst_dir; mv merlin_type_define.cpp merlin_type_define.h >& /dev/null"); }
if ( -e "merlin_type_define.c" )    { system ("cd $dst_dir; mv merlin_type_define.c merlin_type_define.h >& /dev/null"); }
if ( -e "__merlintask_top_h.cpp" )  { system ("cd $dst_dir; mv __merlintask_top_h.cpp __merlintask_top.h >& /dev/null"); }
if ( -e "__merlintask_top_h.c" )    { system ("cd $dst_dir; mv __merlintask_top_h.c __merlintask_top.h >& /dev/null");}

if ($xml_skip_syncheck ne "on") {
run_command("cd $dst_dir; $cmd_frontend_pass . -p synthesis_check $add_cfg -a '$additional'"); 
check_error($dst_dir);
}

run_command("cd $dst_dir; $cmd_frontend_pass . -p preprocess $add_cfg -a '$additional'");  
check_error($dst_dir);

if ($pure_kernel_flow ne "on" and $hls_flow ne "on") {
  run_command("cd $dst_dir; $cmd_frontend_pass . -p kernel_wrapper $add_cfg -a ' $additional'");
  check_error($dst_dir);
}

run_command("cd $dst_dir; $cmd_frontend_pass . -p postwrap_process $add_cfg -a '$additional'");  
check_error($dst_dir);
if ($pure_kernel_flow ne "on") {
  run_command("cd $dst_dir; $cmd_frontend_pass . -p postwrap_process $add_cfg -a '$additional -a global_convert=true'");  
  check_error($dst_dir);
}

my $list = $global_kernel_list;
run_command "cd $dst_dir; rm ${list}.tmp >> ${list}.tmp";
run_command "cd $dst_dir; grep __merlinwrapper ${list} >> ${list}.tmp";
run_command "cd $dst_dir; grep __merlinkernel ${list} >> ${list}.tmp";
run_command "cd $dst_dir; mv ${list}.tmp ${list}";

# split task from read kernels
my @task_list = get_task_list($global_task_file);
if(-e "$global_task_file") {
    @task_list = get_task_list($global_task_file);
    foreach my $one_task (@task_list) {
        run_command "cd $dst_dir; perl -pi -e 's/__merlinwrapper_$one_task\\\.c.*\\n//g' $list";
        run_command "cd $dst_dir; perl -pi -e 's/__merlinkernel_$one_task\\\.c.*\\n//g' $list";
    }
    foreach my $one_task (@task_list) {
        my $file_list = `cd $dst_dir; ls *.c* 2>/dev/null`;
        my @file_set = split(/\n/, $file_list);
        foreach my $one_file (@file_set) {
    #        printf "one file = $one_file\n";
            if($one_file =~ /__merlinwrapper_$one_task(\..*)$/) {
                run_command "cd $dst_dir; perl -pi -e 'print \"$one_file\\n\" if \$\. == 1' $list";
            }
            if($one_file =~ /__merlinkernel_$one_task(\..*)$/) {
                run_command "cd $dst_dir; perl -pi -e 'print \"$one_file\\n\" if \$\. == 1' $list";
            }
        }
    }
}

my $xml_skip_out_of_address_checker = "off";
my $xml_skip_io_analysis = "off";
if ($xml_array_linearize_simple eq "on") {
$xml_skip_out_of_address_checker = "on";
$xml_skip_io_analysis = "on";
}

if ( $xml_line_buffer ne "off" ) {  
  my $linebuf_additional = $additional;  
  if ( $xml_burst_single_size_threshold ne "") {   
    $linebuf_additional .= " -a burst_single_size_threshold=$xml_burst_single_size_threshold"; 
  }    
  $linebuf_additional .= " -a stream_prefetch=$xml_stream_prefetch";
  run_command("cd $dst_dir; $cmd_frontend_pass . -p line_buffer $add_cfg -a '$linebuf_additional'"); 
  check_error($dst_dir);   
}

if($xml_impl_tool ne "sdaccel") {
    $xml_pcie_transfer_opt = "off";
}

run_command("cd $dst_dir; $cmd_frontend_pass . -p interface_transform $add_cfg ".
    "-a '$additional -a systolic_array=$xml_systolic_array ".
    "-a disable_array_linearize=$xml_array_linearize ".
    "-a aligned_struct_decomp=$xml_aligned_struct_decomp ".
    "-a skip_out_of_address_checker=$xml_skip_out_of_address_checker ".
    "-a pcie_transfer_opt=$xml_pcie_transfer_opt ".
    "-a skip_io_analysis=$xml_skip_io_analysis'"); 
check_error($dst_dir);

if(-e "$global_task_file") {
    foreach my $one_task (@task_list) {
        run_command "cd $dst_dir; perl -pi -e 's/__merlinwrapper_$one_task\\\.c.*\\n//g' $list";
        run_command "cd $dst_dir; perl -pi -e 's/__merlinkernel_$one_task\\\.c.*\\n//g' $list";
    }
}

exit;

