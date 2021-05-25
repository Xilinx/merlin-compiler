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

# hls estimation flow
sub copy_bc_file {
    my $report_dir = $_[0];
    run_command "i=0; for f in \$(find . -name \"a.o.3.bc\"); do cp \$f $report_dir/a.o.3.\$i.bc 2>/dev/null; let i=i+1; done;";
}
sub copy_adb_file {
    my $report_dir = $_[0];
    run_command "find . -name \"*.sched.adb.xml\" -exec cp {} $report_dir 2>/dev/null \\;";
}

#---------------------------------------------------------------------------------------------#
# command line options parser 
#---------------------------------------------------------------------------------------------#
my $flow = $ARGV[0];

my $report_hls_summary  = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/summary_hls.pl";
my $report_bit_summary  = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/summary_bit.pl";
my $adb_report_analyze  = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/analyze_adb_xml.pl";
my $vendor_cycles  = "mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/vecdor_cycles_extract.pl";

#---------------------------------------------------------------------------------------------#
# environment checking
#---------------------------------------------------------------------------------------------#
my $new_driver = 0;
my $xocc = 1;
#my $version = `sdx -version`;
#my @version_info=split(/\n/,$version);
#foreach my $one_line (@version_info) {
#    if($one_line =~ /SDx.*v(\S*)\s*\(/) {
#        $xocc = 1;
#        if($1 =~ /2017/ or $1 =~ /2018/) { $new_driver = 1; }
#    }
#}
my $xocc_version = get_xocc_version;
if($xocc_version =~ /2017/ or $xocc_version =~ /2018/) {
    $new_driver = 1;
}
if($xocc eq 1) {
    if(defined $ENV{'HOME'}) {
    } else {
        MSG_E_3201;
        exit;
    }
}

my $aws_f1 = 0;
if($xml_platform_name =~ /aws-vu9p-f1/) { $aws_f1 = 1; } # F1 board

#---------------------------------------------------------------------------------------------#
# report generation
#---------------------------------------------------------------------------------------------#
my $report_merlin = "report_merlin";
my $report_vendor   = "$report_merlin/vendor_report";
if(not -e "kernel_list.json") {
    printf "Error: File kernel_list.json not exist.\n";
    exit;
}
my @kernel_list = get_kernel_list("kernel_list.json");
if($flow eq "hls") {
    my $pass_flag = 1;
    my $has_report = 0;
    foreach my $kernel_name (@kernel_list) {
        my $one_dir ="$report_merlin/$kernel_name";
        run_command "rm -rf $one_dir; mkdir $one_dir";
        my $path = `find . -name \"${kernel_name}_csynth.rpt\"`;
        if($path =~ /(^.*syn\/report\/)/) {
            $has_report = 1;
            run_command "cp $1/*.rpt $one_dir";
#            run_command "cp $1/*.rpt $report_merlin";
        }
        $path = `find . -name \"${kernel_name}.verbose.rpt.xml\"`;
        if($path =~ /(^.*.autopilot\/db)/) {
            run_command "cp $1/*verbose.rpt.xml $one_dir";
        }
        run_command "cd $one_dir; $vendor_cycles ${kernel_name}_csynth.rpt";
        $path = `find . -name \"$global_vivado_hls_log\"`;
        if($path =~ /(.*impl\/kernels\/${kernel_name}\/)/) {
            run_command "cp $1/$global_vivado_hls_log $one_dir";
        }
        $path = `find . -name \"$global_vitis_hls_log\"`;
        if($path =~ /(.*impl\/kernels\/${kernel_name}\/)/) {
            run_command "cp $1/$global_vitis_hls_log $one_dir/$global_vivado_hls_log";
        }
        $path = `find . -name \"system_estimate_${kernel_name}.xtxt\"`;
        if($path =~ /(^.*\/reports\/${kernel_name})\/system_estimate_${kernel_name}.xtxt/) {
            run_command "cp $1/*.xtxt $one_dir";
        } else {
            $pass_flag = 0;
        }
#        if ($xml_debug_mode ne "") {
#            run_command "cd $report_merlin/$kernel_name; $report_hls_summary ${kernel_name}_csynth.rpt > summary_hls_${kernel_name}.rpt";
#            run_command "cd $report_merlin/$kernel_name; cat summary_hls_${kernel_name}.rpt";
#        }
        copy_bc_file($report_merlin);
        copy_bc_file($report_vendor);
        copy_adb_file($report_vendor);
        run_command "cd $report_vendor; $adb_report_analyze";
        run_command "cp $report_vendor/adb.json $report_merlin 2>/dev/null";
        if(-e "$global_system_xtxt" or $pass_flag eq 1) {
            run_command "touch estimate_pass.o";
        }
        if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"] and $has_report eq 1) {
            run_command "touch estimate_pass.o";
        }
    }
    if(-e "$global_system_xtxt") {
        run_command "cp $global_system_xtxt $report_merlin";
    }
} elsif($flow eq "impl") {
    my $impl_rpt = "summary_impl.rpt";
    my $impl_dir = ".";
    if(not -e "$prj_pkg") {
        run_command "mkdir $prj_pkg";
    }
    # generate bitstream
    if($aws_f1 ne 1) {
        # gen environment package for on board execution
        run_command "cd $impl_dir; xbinst -f $xml_platform_name -d driver > xbinst.log 2>&1";
        if($new_driver eq 1) {
            if(-e "$impl_dir/driver/xbinst/runtime") {
                run_command "rm -rf $prj_pkg/pcie; mkdir $prj_pkg/pcie";
                run_command "cp -r $impl_dir/driver/xbinst/runtime $prj_pkg/pcie";
            }
        } else {
            if(-e "$impl_dir/driver/xbinst/pkg/pcie") {
                run_command "cp -r $impl_dir/driver/xbinst/pkg/pcie $prj_pkg";
            }
        }
    }

    my $xocc_dir;
    my $ls_info = `ls $impl_dir`;
    my @ls_lines = split(/\n/, $ls_info);
    foreach my $one_line (@ls_lines) {
        if ($one_line =~ /(_xocc.*kernel_top\.dir)/) {
            $xocc_dir = $1;
            my $kernel_name = "";
            if ($one_line =~ /_xocc_link_(.*)_kernel_top\.dir/) {
                $kernel_name = $1;
            }
            if(-d "$impl_dir/$xocc_dir/impl/build/system/kernel_top/bitstream/kernel_top_ipi") {
                if(-e "$impl_dir/$xocc_dir/impl") {
                    run_command "cd $impl_dir; find ./$xocc_dir/impl/build/system/kernel_top/bitstream/kernel_top_ipi/ -name \"$global_vivado_log\" -exec cp {} $report_merlin 2>/dev/null \\;";
                    run_command "cd $impl_dir; find ./$xocc_dir/impl/build/system/kernel_top/bitstream -name \"kernel_top.xml\" -exec cp {} $report_merlin 2>/dev/null \\;";
                }
                if(-e "$impl_dir/$xocc_dir/_vpl") {
                    run_command "cd $impl_dir; find ./$xocc_dir/_vpl/ipi/ -name \"$global_vivado_log\" -exec cp {} $report_merlin 2>/dev/null \\;";
                }
                if($new_driver eq 1) {
                    if(-e "$impl_dir/$xocc_dir/impl") {
                        run_command "cd $impl_dir; find ./$xocc_dir/impl/build/system/kernel_top/bitstream/kernel_top_ipi/ -name \"kernel*.rpt\" -exec cp {} $report_merlin 2>/dev/null \\;";
                        run_command "cd $impl_dir; find ./$xocc_dir/impl/build/system/kernel_top/bitstream/kernel_top_ipi/ -name \"top*.rpt\" -exec cp {} $report_merlin 2>/dev/null \\;";
                    }
                    if(-e "$impl_dir/$xocc_dir/_vpl") {
                        run_command "cd $impl_dir; find ./$xocc_dir/_vpl/ipi/imp/imp.runs/impl_1/ -name \"kernel*.rpt\" -exec cp {} $report_merlin 2>/dev/null \\;";
                    }
                    run_command "cd $impl_dir/$report_merlin; $report_bit_summary $global_kernel_util_routed $global_kernel_util_routed $global_vivado_log kernel_top.xml 1 > summary_impl_${kernel_name}.rpt";
                } else {
                    if(-e "$impl_dir/$xocc_dir/impl") {
                        run_command "cd $impl_dir; find ./$xocc_dir/impl/build/system/kernel_top/bitstream/kernel_top_ipi/ -name \"*_utilization_routed.rpt\" -exec cp {} $report_merlin 2>/dev/null \\;";
                    }
                    run_command "cd $impl_dir/$report_merlin; $report_bit_summary $global_platform_routed ${kernel_name}_utilization_routed.rpt $global_vivado_log kernel_top.xml 0 > summary_impl_${kernel_name}.rpt";
                }
                printf "\n";
                run_command "cd $impl_dir/$report_merlin; cat summary_impl_${kernel_name}.rpt";
                run_command "cd $impl_dir/$report_merlin; cat summary_impl_${kernel_name}.rpt >> $impl_rpt";
            }
        }
        if ($one_line =~ /(^_x$)/) {
            $xocc_dir = $1;
            my $has_report = 1;
            if(-e "$impl_dir/$xocc_dir/logs/link/$global_vivado_log") {
                run_command "cd $impl_dir; cp -r $xocc_dir/logs/link/$global_vivado_log $report_merlin";
            } else {
                $has_report = 0;
            }
            if(-e "$impl_dir/$xocc_dir/link/int/kernel_top.xml") {
                run_command "cd $impl_dir; cp -r $xocc_dir/link/int/kernel_top.xml $report_merlin";
            } else {
                $has_report = 0;
            }
            if(-e "$impl_dir/$xocc_dir/reports/link/imp/$global_kernel_util_routed") {
                run_command "cd $impl_dir; cp -r $xocc_dir/reports/link/imp/$global_kernel_util_routed $report_merlin";
            } elsif(-e "$impl_dir/$xocc_dir/link/vivado/vp1/output/output/$global_kernel_util_routed") {
                run_command "cd $impl_dir; cp -r $impl_dir/$xocc_dir/link/vivado/vp1/output/$global_kernel_util_routed $report_merlin";
            } else {
                $has_report = 0;
            }
            if($has_report eq 1) {
                run_command "cd $impl_dir/$report_merlin; $report_bit_summary $global_kernel_util_routed $global_kernel_util_routed $global_vivado_log kernel_top.xml 1 > summary_impl.rpt";
                printf "\n";
                run_command "cd $impl_dir/$report_merlin; cat summary_impl.rpt";
            }
        }
    }
    if(-e "$impl_dir/kernel_top.xclbin") {
        run_command "cp -r $impl_dir/kernel_top.xclbin $prj_pkg";
    }
#    if($aws_f1 ne 1) {
#        run_command "cd $prj_pkg; mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/sdaccel/xilinx_env_gen.pl";
#    }
}

