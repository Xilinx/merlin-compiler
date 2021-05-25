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
#use strict;
#use warnings;
my $MERLIN_COMPILER_HOME;
my $library_path;
BEGIN {
    $|=1;   #turn on auto reflush for stdout
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'}; 
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    $mars_test_home = $ENV{'MARS_TEST_HOME'};
    our $xml_dir = "";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use strict;
use warnings;
use lib "$library_path";
use merlin;
use message;
use global_var;
use File::Basename;

#---------------------------------------------------------------------------------------------#
# command line options parser 
#---------------------------------------------------------------------------------------------#
my $flow = $ARGV[0];

#---------------------------------------------------------------------------------------------#
# copy report
#---------------------------------------------------------------------------------------------#
if(not -e "kernel_list.json") {
    printf "Error: File kernel_list.json not exist.\n";
    exit;
}
my @kernel_list = get_kernel_list("kernel_list.json");
my $report_merlin = "report_merlin";
my $report_vendor = "$report_merlin/vendor_report";
if($flow eq "hls" or $flow eq "sim" or $flow eq "hw_sim" or $flow eq "impl") {
    my $xo_name_list = "";
    my $totol_vivado_log = "";
    foreach my $kernel_name (@kernel_list) {
        my $one_dir ="$report_merlin/$kernel_name";
        if(not -d $one_dir) {
            run_command "mkdir $one_dir";
        }
        my $path = `find . -name \"$global_vivado_hls_log\"`;
        if($path =~ /(.*impl\/kernels\/${kernel_name}\/)/ or $path =~ /(.*\/${kernel_name}\/${kernel_name}\/)/) {
            run_command "cp $1/$global_vivado_hls_log $one_dir";
            run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/vivado_extract.pl \"$one_dir/$global_vivado_hls_log\"";
            my $info = `cat $one_dir/$global_vivado_hls_log`;
            $totol_vivado_log .= $info;
        } else {
            my $new_path = `find . -name \"$global_solution_log\"`;
            if($new_path =~ /(.*$kernel_name\/solution\/)/) {
                run_command "cp $1/$global_solution_log $one_dir/$global_vivado_hls_log";
                my $info = `cat $one_dir/$global_vivado_hls_log`;
                $totol_vivado_log .= $info;
            }
        }
        $path = `find . -name \"$global_vitis_hls_log\"`;
        if($path =~ /(.*impl\/kernels\/${kernel_name}\/)/ or $path =~ /(.*\/${kernel_name}\/${kernel_name}\/)/) {
            run_command "cp $1/$global_vitis_hls_log $one_dir/$global_vivado_hls_log";
            run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/vivado_extract.pl \"$one_dir/$global_vivado_hls_log\"";
        }
        run_command "cp $one_dir/* $report_vendor 2>/dev/null";
        $path = `find . -name \"xilinx_com*${kernel_name}*zip\"`;
        chomp($path);
        if($path =~ /(.*solution\/impl\/ip\/.*${kernel_name})/) {
            run_command "cp $path $report_vendor";
        }
    }
    if(-e "$report_vendor/$global_vivado_hls_log") {
        write_file("$report_vendor/$global_vivado_hls_log", $totol_vivado_log);
    }
    if(-e "system_estimate.xtxt") {
        run_command "cp system_estimate.xtxt $report_vendor";
    }
    run_command "cd .; find . -name \"*_export.rpt\" -exec cp {} $report_merlin 2>/dev/null \\;";
} 
if($flow eq "impl" or $flow eq "hw_sim" or $flow eq "cosim" or $flow eq "csim") {
    my $xocc_dir;
    my $impl_dir = ".";
    my $ls_info = `ls $impl_dir`;
    my @ls_lines = split(/\n/, $ls_info);
    foreach my $one_line (@ls_lines) {
        if ($one_line =~ /(_xocc.*kernel_top\.dir)/) {
            $xocc_dir = $1;
            my $kernel_name = "";
            if ($one_line =~ /_xocc_link_(.*)_kernel_top\.dir/) {
                $kernel_name = $1;
            }
            if(-e "$impl_dir/$xocc_dir/impl") {
                run_command "cd $impl_dir; find ./$xocc_dir/impl/build/system/kernel_top/bitstream/kernel_top_ipi/ -name \"$global_vivado_log\" -exec cp {} $report_merlin 2>/dev/null \\;";
            }
#            if(-e "$impl_dir/$xocc_dir/_vpl") {
#                run_command "cd $impl_dir; find ./$xocc_dir/_vpl/ipi/ -name \"$global_vivado_log\" -exec cp {} $report_merlin 2>/dev/null \\;";
#            }
            if(-e "$impl_dir/$xocc_dir/_vpl/ipi/$global_vivado_log") {
                run_command "cd $impl_dir; cp ./$xocc_dir/_vpl/ipi/$global_vivado_log $report_merlin 2>/dev/null;";
            }
            if(-e "$report_merlin/$global_vivado_log") {
                run_command "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/vivado_extract.pl \"$report_merlin/$global_vivado_log\"";
            }
        }
    }
    run_command "cd $impl_dir; find . -name \"*_cosim.rpt\" -exec cp {} $report_merlin 2>/dev/null \\;";
    run_command "cd $impl_dir; find . -name \"*_csim.log\" -exec cp {} $report_merlin 2>/dev/null \\;";
}

