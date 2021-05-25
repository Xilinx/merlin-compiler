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


#---------------------------------------------------------------------------------------------#
# excute before everything
#---------------------------------------------------------------------------------------------#
my $MERLIN_COMPILER_HOME;
my $library_path;
BEGIN {
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'}; 
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    our $xml_dir = "";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use strict;
#use warnings;
use threads;
use lib "$library_path";
use message;
use merlin;
use global_var;
use File::Basename;
$ENV{"PERL5SHELL"} = "tcsh";


my $merlin_sdaccel = "merlin_sdaccel.log";
my $safe_exec = "merlin_safe_exec";
my $encrypt_flag = 0;
if($xml_encrypt eq "on") {
    $encrypt_flag = 1000;
}

sub multi_thread_exec {
    my $cmd = $_[0];
    my $file = $_[1];
    my $th1 = threads->create(sub { 
        run_command "$cmd"; 
#        run_command "touch .finish_cmd"; 
        return 0;
        });
    my $th2 = threads->create(sub { 
        run_command "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/sdaccel/report_screen.pl $file";
#        run_command "rm .finish_cmd";
        return 0;
        });
    $th1->join();
    $th2->join();
}

my $flow        = $ARGV[0];
my $BANK_OPTION = $ARGV[1];
my $DIE_OPTION  = $ARGV[2];

my $reduction_flag = 0;
if(-e "critical_message.rpt") {
    my $out = `grep REDUC-101 critical_message.rpt`;
    if($out ne "") {
        $reduction_flag = 1;
    }
}

my $tcl_name = "my_hls_pre.tcl";
run_command("rm -rf $tcl_name; touch $tcl_name");
if($xml_unsafe_math eq "on" or $reduction_flag eq 1) {
    run_command("echo \"config_compile -unsafe_math_optimizations\" >> $tcl_name");
}
if($xml_pre_hls_tcl ne "") {
    run_command("cat $xml_pre_hls_tcl >> $tcl_name");
}

my $add_tcl = "--xp prop:solution.hls_pre_tcl=my_hls_pre.tcl";


my $makefile = "Makefile";
my $makefile_org = "";
my $makefile_info = "";
my %kernel_list;
if(-e "$global_kernel_file_json") {
    %kernel_list = get_kernel_file_list("$global_kernel_file_json");
} else {
    MSG_E_3002();
    exit;
}

if(($flow eq "sim_config" or $flow eq "sim_config" or $flow eq "exec_sim") and -e "merlin_has_c_wide_bus.h"){
    MSG_E_3200();
    exit;
}

my $freq_num = 300;
my $freq = "";
if($xml_kernel_frequency ne "") {
    $freq = "--kernel_frequency $xml_kernel_frequency";
    $freq_num = $xml_kernel_frequency;
}

my $xocc_version = get_xocc_version();
if ($xocc_version =~ "2017.4" or $xocc_version =~ "2018.1" or $xocc_version =~ "2018.2") {
    if ($xml_platform_name =~ /^xilinx/) {
        if ($xml_platform_name =~ /^xilinx:(.*)\:(\d*ddr\S*)\:(\d+)\.(\d+)$/) {
            $xml_platform_name = "xilinx_$1_$2_$3_$4";
        }
        my $org_dir = "";
        if(-e "$ENV{'XILINX_SDX'}") {
            $org_dir = "$ENV{'XILINX_SDX'}";
        }
        my $compatiable_dir = "/curr/software/Xilinx/SDx/2017.1";
        my $env_comp = $ENV{'XILINX_SDX_COMPATIABLE'};
        if(-e $env_comp) {
            $compatiable_dir = $env_comp;
        }
        if(-e "$org_dir/platforms/$xml_platform_name/$xml_platform_name.xpfm") {
            $xml_platform_name = "$org_dir/platforms/$xml_platform_name/$xml_platform_name.xpfm";
        } elsif(-e "$compatiable_dir/platforms/$xml_platform_name/$xml_platform_name.xpfm") {
            $xml_platform_name = "$compatiable_dir/platforms/$xml_platform_name/$xml_platform_name.xpfm";
        }
    }
}

my $vendor_command = "xocc";
if ($xml_tool_version eq "vitis") {
    $vendor_command = "v++";
}

my $common_arg = "-s --platform $xml_platform_name $xml_vendor_options --include ./ $freq $xml_macro_define $add_tcl  > $merlin_sdaccel 2>&1";

if($flow eq "hls") {
    my $kernel_name_list = "";
    my $xo_name_list = "";
    if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"]) {
        run_command "rm -rf _x; mkdir _x;";
    }
    while (my ($key, $value) = each(%kernel_list)) {
        my $kernel_name = $key;
        my $file_list = $value;
        my $cmd = "";
        if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"]) {
            run_command "mkdir _x/$kernel_name/";
            run_command "mkdir _x/$kernel_name/$kernel_name";
            my $max_w = 4096;
            if($xml_ap_int_max_w ne "") {
                $max_w = $xml_ap_int_max_w;
            }
            my $tcl_file = "$kernel_name.tcl";
            run_command "cp ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/vivado_hls.tcl $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_NAME/$kernel_name/g\"         $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_SRC/$file_list/g\"            $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_FLAGS/$xml_macro_define -DAP_INT_MAX_W=$max_w/g\"   $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_DEVICE/$xml_platform_name/g\" $tcl_file";
            run_command "perl -p -i -e \"s/\#csynth_design/csynth_design/g\"    $tcl_file";
            my $report_type = "";
            if($xml_report_type eq "syn") {
                $report_type = "-flow syn -rtl verilog";
            } elsif($xml_report_type eq "impl") {
                $report_type = "-flow impl -rtl verilog";
            }
            run_command "perl -p -i -e \"s/\#export_design/export_design $report_type/g\"    $tcl_file";
            $cmd = "$xml_tool_version -f $tcl_file > $merlin_sdaccel";
        } else {
            $cmd = "$vendor_command -t hw_emu --report_level estimate -o $kernel_name.xo -c $file_list -k $kernel_name $common_arg";
        }
        my $safe_cmd = "$safe_exec $cmd :: $file_list :: $encrypt_flag; touch .finish_cmd";
        multi_thread_exec($safe_cmd, $merlin_sdaccel);
    }
    run_command "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/sdaccel/report_gen.pl hls";
    run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/copy_vendor_file.pl hls";
    while (my ($key, $value) = each(%kernel_list)) {
        my $kernel_name = $key;
        if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"]) {
            if(-d $kernel_name) {
                run_command "mv $kernel_name _x/$kernel_name/$kernel_name";
            }
        }
    }
}

if($flow eq "sim_config") {
    my $kernel_name_list = "";
    my $xo_name_list = "";
    my $xo_generated = 1;
    while (my ($key, $value) = each(%kernel_list)) {
        my $kernel_name = $key;
        my $file_list = $value;
        my $cmd = "$vendor_command -t sw_emu -o $kernel_name.xo -c $file_list -k $kernel_name $common_arg";
        my $safe_cmd = "$safe_exec $cmd :: $file_list :: $encrypt_flag; touch .finish_cmd";
        multi_thread_exec($safe_cmd, $merlin_sdaccel);
        if(not -e "$kernel_name.xo") {
            $xo_generated = 0;
        }
        $kernel_name_list .= "$kernel_name ";
        $xo_name_list .= "$kernel_name.xo "
    }
    if($xo_generated ne 0) {
        my $cmd = "$vendor_command -t sw_emu -o kernel_top.xclbin --link $xo_name_list $common_arg; touch .finish_cmd";
        multi_thread_exec($cmd, $merlin_sdaccel);
        if(-e "kernel_top.xclbin") {
            run_command "touch sim_pass.o";
            run_command "cp kernel_top.xclbin pkg 2>/dev/null";
            if(-e "emconfig.json") {
                run_command "cp emconfig.json pkg 2>/dev/null";
            }
        }
    }
    run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/copy_vendor_file.pl sim";
}

if($flow eq "hw_sim_config") {
    my $kernel_name_list = "";
    my $xo_name_list = "";
    my $xo_generated = 1;
    while (my ($key, $value) = each(%kernel_list)) {
        my $kernel_name = $key;
        my $file_list = $value;
        my $cmd = "$vendor_command -t hw_emu -o $kernel_name.xo -c $file_list -k $kernel_name $common_arg";
        my $safe_cmd = "$safe_exec $cmd :: $file_list :: $encrypt_flag; touch .finish_cmd";
        multi_thread_exec($safe_cmd, $merlin_sdaccel);
        if(not -e "$kernel_name.xo") {
            $xo_generated = 0;
        }
        $kernel_name_list .= "$kernel_name ";
        $xo_name_list .= "$kernel_name.xo "
    }
    if($xo_generated ne 0) {
        my $cmd = "$vendor_command -t hw_emu -o kernel_top.xclbin --link $xo_name_list $common_arg; touch .finish_cmd";
        multi_thread_exec($cmd, $merlin_sdaccel);
        if(-e "kernel_top.xclbin") {
            run_command "touch hw_sim_pass.o";
            run_command "cp kernel_top.xclbin pkg 2>/dev/null";
            if(-e "emconfig.json") {
                run_command "cp emconfig.json pkg 2>/dev/null";
            }
        }
    }
    run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/copy_vendor_file.pl hw_sim";
}

if($flow ~~ ["csim", "cosim"]) {
    my $kernel_name_list = "";
    my $xo_name_list = "";
    my $xo_generated = 1;
    while (my ($key, $value) = each(%kernel_list)) {
        my $kernel_name = $key;
        my $file_list = $value;
        if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"]) {
            my $max_w = 4096;
            if($xml_ap_int_max_w ne "") {
                $max_w = $xml_ap_int_max_w;
            }
            my $tcl_file = "$kernel_name.tcl";
            run_command "cp ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/vivado_hls.tcl $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_NAME/$kernel_name/g\"         $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_SRC/$file_list/g\"            $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_FLAGS/$xml_macro_define -DAP_INT_MAX_W=$max_w/g\"   $tcl_file";
            run_command "perl -p -i -e \"s/KERNEL_DEVICE/$xml_platform_name/g\" $tcl_file";
            run_command "perl -p -i -e \"s/300MHz/${freq_num}MHz/g\" $tcl_file";
            $xml_exec_argv =~ s/\//\\\//g;
            my $argv = "";
            if ($xml_exec_argv ne "") {
              $argv = "-argv \\\"$xml_exec_argv\\\"";
            }
            $xml_ldflags =~ s/\//\\\//g;
            my $ldflags = "";
            if ($xml_ldflags ne "") {
              $ldflags = "-ldflags \\\"$xml_ldflags\\\"";
            }
            if($flow eq "csim") {
                run_command "perl -p -i -e \"s/\#csim_design/csim_design $argv $ldflags/g\"        $tcl_file";
            } elsif($flow eq "cosim") {
                run_command "perl -p -i -e \"s/\#cosim_design/cosim_design $argv $ldflags/g\"      $tcl_file";
                run_command "perl -p -i -e \"s/\#csynth_design/csynth_design/g\"    $tcl_file";
            }
            $xml_tb =~ s/\//\\\//g;
            $xml_cflags =~ s/\//\\\//g;
            $xml_include_path =~ s/\//\\\//g;
            my $cflags = "-cflags \\\"$xml_include_path $xml_cflags\\\"";
            my $csimflags = "-csimflags \\\"$xml_include_path $xml_cflags\\\"";
            run_command "perl -p -i -e \"s/\#add_files -tb/add_files -tb \\\"$xml_tb\\\" $cflags $csimflags/g\" $tcl_file";
            my $cmd = "$xml_tool_version -f $tcl_file";
            my $safe_cmd = "$safe_exec $cmd :: $file_list :: $encrypt_flag; touch .finish_cmd";
            run_command "$safe_cmd";
        }
    }
    run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/copy_vendor_file.pl $flow";
}

if($flow eq "exec_sim") {
    my $XILINX_SDX = $ENV{'XILINX_SDX'}; 
    run_command "source $XILINX_SDX/settings64.sh; XCL_EMULATION_MODE=true ./host_top $xml_host_args";
}

if($flow eq "impl") {
    my $kernel_name_list = "";
    my $xo_name_list = "";
    my $xo_generated = 1;
    while (my ($key, $value) = each(%kernel_list)) {
        my $kernel_name = $key;
        my $file_list = $value;
        my $cmd = "$vendor_command -t hw -o $kernel_name.xo -c $file_list -k $kernel_name $common_arg";
        my $safe_cmd = "$safe_exec $cmd :: $file_list :: $encrypt_flag; touch .finish_cmd";
        multi_thread_exec($safe_cmd, $merlin_sdaccel);
        if(not -e "$kernel_name.xo") {
            $xo_generated = 0;
        }
        $kernel_name_list .= "$kernel_name ";
        $xo_name_list .= "$kernel_name.xo "
    }
    if($xo_generated ne 0) {
        my $cmd = "$vendor_command -t hw -o kernel_top.xclbin --link $xo_name_list $BANK_OPTION $DIE_OPTION $common_arg; touch .finish_cmd\n\n";
        multi_thread_exec($cmd, $merlin_sdaccel);
        if(-e "kernel_top.xclbin") {
            run_command "touch impl_pass.o";
            run_command "cp kernel_top.xclbin pkg 2>/dev/null";
            run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/report_gen.pl impl";
        }
    }
    run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/copy_vendor_file.pl impl";
}

if(-e "merlin_sdaccel.log") {
    run_command "cp merlin_sdaccel.log report_merlin";
}

exit;

