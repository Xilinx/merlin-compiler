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
use warnings;
use lib "$library_path";
use merlin;
use global_var;
use File::Basename;
$ENV{"PERL5SHELL"} = "tcsh";

#---------------------------------------------------------------------------------------------#
# generate Makefile for xilinx multi kernel flow(2017/09/11)
#---------------------------------------------------------------------------------------------#
my $export_dir = "./";
if(not -e $export_dir) {
    printf "Error: Directory $export_dir not exist.\n";
    exit;
}
#run_command "cd $export_dir; cp Makefile Makefile_config";
#run_command "cd $export_dir; mars_perl $dir_scripts/$xml_impl_tool/makefile_gen.pl";
#run_command "cd $export_dir; mv Makefile_config Makefile";
my $xocc_version = get_xocc_version();

#---------------------------------------------------------------------------------------------#
# generate multi-bank and multi-die required files
#---------------------------------------------------------------------------------------------#
# add multi-die related information to Makefile
#$bank_info = "--max_memory_ports all ";
my $die_tcl = "DIE_OPTION=";
if(-e "$export_dir/kernel_die.h") {
    my $die_info = "";
    my $count_die = 0;
    my $kernel_die_info = `cat $export_dir/kernel_die.h`;
    my @kernel_die_info_set = split(/\n/, $kernel_die_info);
    if ($xocc_version =~ "2016.4" or $xocc_version =~ "2017.1") {
        foreach my $one_kernel_die (@kernel_die_info_set) {
            if($one_kernel_die =~ /\/\/(.*)=(.*)$/) {
                my $one_kernel_name = $1;
                my $one_die_name = $2;
                if($one_die_name ne "NULL") {
                    if($xml_platform_name =~ /ku115/) {
                        $die_info .= "add_cells_to_pblock [get_pblocks pblock_$one_die_name] [get_cells [list xcl_design_i/expanded_region/u_ocl_region/dr_i/${one_kernel_name}_1]]\n";
                    } elsif($xml_platform_name =~ /aws/) {
                        $die_info .= "add_cells_to_pblock [get_pblocks pblock_CL_$one_die_name] [get_cells [list CL/xcl_design_i/expanded_region/u_ocl_region/dr_i/${one_kernel_name}_1]]\n";
                    } else {
                        $die_info .= "add_cells_to_pblock [get_pblocks pblock_CL_$one_die_name] [get_cells [list CL/xcl_design_i/expanded_region/u_ocl_region/dr_i/${one_kernel_name}_1]]\n";
                    }
                    $count_die = $count_die + 1;
                }
            }

        }
        if($count_die > 0) {
            my $tcl_filename = "$export_dir/constraints-pblock.tcl";
            my $absolute_path = `pwd`;
            chomp($absolute_path);
            $die_tcl .= "--xp vivado_prop:run.impl_1.STEPS.OPT_DESIGN.TCL.PRE=__MERLIN_PATH__/constraints-pblock.tcl";
            run_command "rm -rf $tcl_filename";
            open my $file, ">$tcl_filename";
            print $file $die_info;
            close $file;
        }
    } else {
        foreach my $one_kernel_die (@kernel_die_info_set) {
            if($one_kernel_die =~ /\/\/(.*)=(.*)$/) {
                my $one_kernel_name = $1;
                my $one_die_name = $2;
                if($one_die_name ne "NULL") {
                    if($xml_platform_name =~ /aws/) {
                        $die_info .= "set_property CONFIG.SLR_ASSIGNMENTS $one_die_name [get_bd_cells ${one_kernel_name}_1]\n";
                    } else {
                        $die_info .= "set_property CONFIG.SLR_ASSIGNMENTS $one_die_name [get_bd_cells ${one_kernel_name}_1]\n";
                    }
                    $count_die = $count_die + 1;
                }
            }

        }
        if($count_die > 0) {
            my $tcl_filename = "$export_dir/constraints-pblock.tcl";
            my $absolute_path = `pwd`;
            chomp($absolute_path);
            $die_tcl .= "--xp param:compiler.userPostSysLinkTcl=__MERLIN_PATH__/constraints-pblock.tcl";
            run_command "rm -rf $tcl_filename";
            open my $file, ">$tcl_filename";
            print $file $die_info;
            close $file;
        }
    }
}

# add multi-bank related information to Makefile and c code
my $bank_info = "BANK_OPTION=";
my $kernel_buffer_file = "__merlin_opencl_kernel_buffer.c";
if(-e "$export_dir/__merlin_opencl_kernel_buffer.cpp") {
    $kernel_buffer_file = "__merlin_opencl_kernel_buffer.cpp";
}

if(-e "$export_dir/$kernel_buffer_file") {
    #if ($xocc_version =~ "2016.4" or $xocc_version =~ "2017.1") {
    if ($xocc_version =~ "2016.4") {
        my $file_info = `grep -R \"\\\-\\\-xp\" $export_dir/$kernel_buffer_file`;
        my @file_info_set = split(/\n/, $file_info);
        foreach my $one_info (@file_info_set) {
            if($one_info =~ /\/\/(.*)$/) {
                $bank_info .= $1;
            }
        }
    } else {
        my @exist_bank;
        my $file_info = `grep -R \"\\\-\\\-sp\" $export_dir/$kernel_buffer_file`;
        my @file_info_set = split(/\n/, $file_info);
        foreach my $one_info (@file_info_set) {
            if($one_info =~ /\/\/(.*)$/) {
                my $single_bank = $1;
                if(! grep( /^$single_bank$/, @exist_bank)) {
                    $bank_info .= $single_bank;
                    push @exist_bank, $single_bank;
                }
            }
        }
    }
}

my $makefile_org = "";
if(-e "$export_dir/Makefile") {
    $makefile_org = `cat $export_dir/Makefile`;
}
my $makefile_new = "${die_tcl}\n${bank_info}\n${makefile_org}\n";
my $filename = "$export_dir/Makefile_config";
run_command "rm -rf $filename";
open my $file, ">$filename";
print $file $makefile_new;
close $file;
run_command "cd $export_dir; mv Makefile_config Makefile";

# if report flag off, delete timer related infomation
run_command "cd $export_dir; perl -pi -e 's/#define __MERLIN_VENDOR/#define __MERLIN_SDACCEL/g' __merlin_opencl_if.h 2>/dev/null";
run_command "cd $export_dir; perl -pi -e 's/#include \\\"__merlin_timer.h\\\"/\\\/\\\/#include \\\"__merlin_timer.h\\\"/g' cmost.h 2> /dev/null";
run_command "cd $export_dir; rm -rf __merlin_timer.c* __merlin_timer.h";
