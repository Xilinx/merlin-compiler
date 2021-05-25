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
# Perl Script in CMOST Mars
#
# Filename    : merlin_flow/scripts/merlin_flow_top.pl
# Description : This is the top level sripts for the CMOST Mars design flow 
# Usage       : final output code generation code
# Owner(s)    : Han 
# version     : 10-31-2017 Create file and basic function
##############################################################

#---------------------------------------------------------------------------------------------#
# excute before everything
#---------------------------------------------------------------------------------------------#
#use strict;
#use warnings;
my $MERLIN_COMPILER_HOME;
my $library_path;
my $src_dir;
my $dst_dir;
BEGIN {
    $|=1;   #turn on auto reflush for stdout
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'}; 
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    $src_dir = $ARGV[0];
    $dst_dir = $ARGV[1];
    $flow = $ARGV[2]; # c2cl, generate opencl code with ROSE; post, do post process
    our $xml_dir = "";
    if($src_dir eq "") {
        $xml_dir = "./";
    } else {
        $xml_dir = $src_dir;
    }
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use lib "$library_path";
use merlin;
use message;
use global_var;
#encrypt files if required
my $encrypt_flag = 0;
if($xml_encrypt eq "on") {
    $encrypt_flag = 1;
}
sub decrypt_some_files {
    my $file_dir = $_[0];
    if(-e "$file_dir/__merlintask_top.h") {
        my $info = `cd $file_dir; ls __merlintask_*.c*`;
        my @info_set = split(/\n/, $info);
        foreach my $one_line (@info_set) {
            run_command "cd $file_dir; script_dec $global_passwd $one_line";
        }
    }
}

my $pure_kernel_flow = "off";
if($xml_sycl eq "on" or $xml_pure_kernel eq "on") {
    $pure_kernel_flow = "on";
}

#---------------------------------------------------------------------------------------------#
# prepare input files
#---------------------------------------------------------------------------------------------#
my $default_src = 0;
my $default_dir = 0;
if($src_dir eq "" or $src_dir eq ".") {
    $src_dir = ".";
    $default_src = 1;
}
if($dst_dir eq "" or $dst_dir eq ".") {
    $dst_dir = ".";
    $default_dir = 1;
}
if(not -e $src_dir) {
    exit;
}
if(not -e $dst_dir) {
    exit;
}
if($default_src eq 1 and $default_dir eq 1) {
    #
} else {
    #my $directive = $ARGV[1];
    if ($flow eq "c2cl") {
        run_command "cp -r $src_dir/* $dst_dir/";
    }
}

#run_command "cp -r $directive .";
if(-e "merlin_typedef.h") {
    run_command "cp merlin_typedef.h merlin_typedef.cl";
}

#---------------------------------------------------------------------------------------------#
# main function
#---------------------------------------------------------------------------------------------#
if($flow eq "c2cl") {
    my $work_dir = "";
    my $add_cfg = " -src_list $global_kernel_list";
    # add task back to command line
    if(-e "$dst_dir/$global_task_file") {
        my $list_info = "";
        my @task_list = get_task_list("$dst_dir/$global_task_file");
        foreach my $one_task (@task_list) {
            my $file_list = `cd $dst_dir; ls *.c* 2>/dev/null`;
            my @file_set = split(/\n/, $file_list);
            foreach my $one_file (@file_set) {
                if($one_file =~ /__merlinkernel_$one_task(\.c.*)$/) {
                    #run_command "cd $dst_dir; perl -pi -e 'print \"$one_file\n\" if \$\. == 1' $global_kernel_list";
                    $list_info .= "$one_file\n";
                }
                if($one_file =~ /__merlinwrapper_$one_task(\.c.*)$/) {
                    #run_command "cd $dst_dir; perl -pi -e 'print \"$one_file\n\" if \$\. == 1' $global_kernel_list";
                    $list_info .= "$one_file\n";
                }
            }
        }
        open(my $fh, '>>', "$dst_dir/$global_kernel_list") or die "Could not open file '$dsr_dir/$global_kernel_list' $!";
        print $fh $list_info;
        close $fh;
    }
    my $mode = "opencl";
    if($xml_impl_tool ne "sdaccel") {
        $xml_pcie_transfer_opt = "off";
    }
    run_command "cd $dst_dir; $cmd_frontend_pass . -p final_code_gen $add_cfg ".
                  "-a \"-a impl_tool=$xml_impl_tool -a mode=$mode ".
                  "-a naive=$xml_naive_hls -a effort=$xml_opt_effort ".
                  "-a volatile_flag=$xml_volatile_flag ".
                  "-a tool_version=$xml_tool_version ".
                  "-a explicit_bundle=$xml_explicit_bundle ".
                  "-a extern_c=$xml_extern_c ".
                  "-a pcie_transfer_opt=$xml_pcie_transfer_opt ".
                  "-a pure_kernel=$pure_kernel_flow\" -m ../";
    check_error($dst_dir);
}

if($flow eq "post") {
    #---------------------------------------------------------------------------------------------#
    # output files process
    #---------------------------------------------------------------------------------------------#
    my $export_dir  = "$dst_dir/export";
    my $host_dir    = "$dst_dir/export/host";
    my $lib_gen_dir = "$dst_dir/export/lib_gen";
    my $kernel_dir  = "$dst_dir/export/kernel";
    my $lc_dir      = "$dst_dir/export/lc";
    
    # 1.4 export to package
    run_command "rm -rf $export_dir; mkdir $export_dir";
    my $export_preprocess_cmd   = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/export_preprocess.pl";
    my $export_lib_gen_cmd      = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/export_lib_gen.pl";
    my $export_host_cmd         = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/export_host.pl";
    my $export_kernel_cmd       = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/export_kernel.pl";
    
    if($encrypt_flag eq 1) {
        decrypt_some_files($dst_dir);
    }
    
    # copy some files for report analisis if exist in project
    my $meta_data_dir1 = "implement/metadata";
    my $meta_data_dir2 = "$dst_dir/metadata";
    if(-e $meta_data_dir2) {
        run_command "cp -r $meta_data_dir2 $export_dir";
    } elsif(-e $meta_data_dir1) {
        run_command "cp -r $meta_data_dir1 $export_dir";
    }
    my $org_src = "$dst_dir/src";
    if(-e $org_src) {
        run_command "cp -r $org_src $export_dir"
    }
    my $mid_src = "$dst_dir/run_midend_preprocess";
    if(-e $mid_src) {
        run_command "cp -r $mid_src $export_dir";
    }
    
    #--------------------------------------------------------
    # preprocess for somefile
    #--------------------------------------------------------
    run_command "cd $dst_dir; $export_preprocess_cmd";
    my $disable_flag = 0;
    if (-e "$prj_implement/$prj_code_transform/\.opencl_host_generation_disable_tag") {
        $disable_flag = 1;
    }
    if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"] or $disable_flag eq 1) {
    } else {
        #--------------------------------------------------------
        # copy all library files into lib_gen directory
        #--------------------------------------------------------
        run_command "cd $dst_dir; $export_lib_gen_cmd";
        if($encrypt_flag eq 1) {
            encrypt_dir($lib_gen_dir);
        }
        #--------------------------------------------------------
        # copy all host files into host directory
        #--------------------------------------------------------
        run_command "cd $dst_dir; $export_host_cmd";
        #if($encrypt_flag eq 1) {
        #    encrypt_dir($host_dir);
        #}
    }
    #--------------------------------------------------------
    # copy all kernel files into kernel directory
    #-------------------------------------------------------n
    run_command "cd $dst_dir; $export_kernel_cmd";
    if($encrypt_flag eq 1) {
        encrypt_dir($kernel_dir);
        encrypt_dir($lc_dir);
    }
    
    # remove source code files in export
    run_command "cd $export_dir; rm -rf *.h *.c* *.h_c";
    if ($pure_kernel_flow ne "on" and $xml_tool_version ne "vitis_hls" and $xml_tool_version ne "vivado_hls" and $disable_flag ne 1) {
        if(-e "$export_dir/lib_gen/bin/libkernel.so" and -e "$export_dir/kernel") {
            MSG_I_1040();
        #    printf "*** Compilation finished successfully\n";
        } else {
            MSG_E_3055();
        #    printf "*** Compilation failed\n";
        }
    } else {
       if(-e "$export_dir/kernel") {
           MSG_I_1040();
       #    printf "*** Compilation finished successfully\n";
       } else {
           MSG_E_3055();
       #    printf "*** Compilation failed\n";
       }
    }
}
exit;
