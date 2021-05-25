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


#---------------------------------------------------------------------------------------------
# excute before everything
#---------------------------------------------------------------------------------------------#
#use strict;
#use warnings;
my $MERLIN_COMPILER_HOME;
my $merlin_library_path;
BEGIN {
    $|=1;   #turn on auto reflush for stdout
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'}; 
    $merlin_library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    $mars_test_home = $ENV{'MARS_TEST_HOME'};
    our $xml_dir = "";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use lib "$merlin_library_path";
use merlin;
use global_var;
use Cwd;
use Cwd 'abs_path';

#---------------------------------------------------------------------------------------------#
# parse directive.xml
#---------------------------------------------------------------------------------------------#
my $ann_rpt_local = "refer_induct.json";
if(-e $ann_rpt_local) {
    $ann_rpt_local = "refer_induct.json";
} else {
    $ann_rpt_local = "gen_token.json";
}
my $perf_rpt_local = "perf_est.json";
for (my $i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] eq "--ann_rpt")  { 
        $ann_rpt = $ARGV[$i+1]; 
        if(-e $ann_rpt and $ann_rpt ne $ann_rpt_local) {
            system "cp $ann_rpt $ann_rpt_local >/dev/null";
        }
    }
    if($ARGV[$i] eq "--perf_rpt")  { 
        $perf_rpt = $ARGV[$i+1]; 
    }
}

sub comment { #can not change linne number
    my $dir = $_[0];
    foreach my $file_name (@kernel_file_array) {
        my $comment_flag = 0;
        my $file_name_out = "${file_name}_bak";
        open(file_in,"< $dir/$file_name");
        open(file_out,"> $dir/$file_name_out");
        while(<file_in>) {
            $one_line_info = $_;
            if($comment_flag eq 1 and $one_line_info !~ /__MERLIN.*END__/) {
                $one_line_info =~s/^/\/\//g;
            }
            if($one_line_info =~ /__MERLIN.*END__/) {
                $comment_flag = 0;
            }
            if($one_line_info =~ /__MERLIN.*BEGIN__/) {
                $comment_flag = 1;
            }
            print file_out $one_line_info;
        }
        close(file_in);
        close(file_out);
        system "mv $dir/$file_name_out $dir/$file_name";
    }
}

sub uncomment {
    my $dir = $_[0];
    foreach my $file_name (@kernel_file_array) {
        my $comment_flag = 0;
        my $file_name_out = "${file_name}_bak";
        open(file_in,"< $dir/$file_name");
        open(file_out,"> $dir/$file_name_out");
        while(<file_in>) {
            $one_line_info = $_;
            if($comment_flag eq 1 and $one_line_info !~ /__MERLIN.*END__/) {
                $one_line_info =~s/^\/\///g;
            }
            if($one_line_info =~ /__MERLIN.*END__/) {
                $comment_flag = 0;
            }
            if($one_line_info =~ /__MERLIN.*BEGIN__/) {
                $comment_flag = 1;
            }
            print file_out $one_line_info;
        }
        close(file_in);
        close(file_out);
        system "mv $dir/$file_name_out $dir/$file_name";
    }
}

#-------------------------------------------------------
# current running directory : report/hls/msg_report
#-------------------------------------------------------
my $exec_dir = "./";
my $sim_dir = "$exec_dir/lc_sim";
my $kernel_list_file = "kernel_list.json"; 

#-------------------------------------------------------
# copy directive.xml to lccal and parse input 
#-------------------------------------------------------
my $directive_file      = "$exec_dir/directive.xml";
my $platform_name       = get_value_from_xml("platform_name", $directive_file);
my $include_path        = get_value_from_xml("include_path" , $directive_file);
my $encrypt             = get_value_from_xml("encrypt"      , $directive_file);
my $tool                = get_value_from_xml("implementation_tool", $directive_file);
$include_path =~ s/-I/ -I/g;
$include_path =~ s/\//\\\//g;
my $enc_time = 0;
if($encrypt eq "on") {
    $enc_time = 60;
}

#-------------------------------------------------------
# get file list and kernel list
#-------------------------------------------------------
my $lc_dir = "$exec_dir/lc";
my $kernel_file = `cat $lc_dir/kernel_file.json`;
my $file_list = "";
my @kernel_file_array;
my @file_set = split(/\n/, $kernel_file);
foreach my $one_line (@file_set) {
	if ($one_line =~ /\"(.*)\": \"(.*)\"/ ) {
        my $one_file_name = $2;
        if(! grep /$one_file_name/, @kernel_file_array) {
            push @kernel_file_array, $one_file_name;
            $file_list .= "$one_file_name ";
        }
    }
}

#-------------------------------------------------------
# get file list, and run rose pass
#-------------------------------------------------------
my $rose_dir = "$exec_dir/lc_rose";
system "rm -rf $rose_dir; mkdir $rose_dir";
my $dse_dir = "$exec_dir/lc_dse";
system "rm -rf $dse_dir; mkdir $dse_dir";

my $step1_json = "step1.json";
my $dram_param_file = "$MERLIN_COMPILER_HOME/source-opt/tools/performance_estimation/configuration/$platform_name.json";

system "cp -rf $lc_dir/* $rose_dir >/dev/null";
system "cp -rf $lc_dir/* $dse_dir >/dev/null";
comment("$rose_dir");
    
my $mode = "";
my $cmd = "";
my $fast_dse = "fast_dse";
#-------------------------------------------------------
# check fast dse first
#-------------------------------------------------------
if(-e $fast_dse) {
    my $change_json = "code_change.json";
    my $org_json = "org.json";
    my $output_json = "fast_dse_output.json";
    $mode = "FASTDSE";
    system "cp $fast_dse/* $dse_dir";
    $cmd = "mars_opt $file_list -e c -p performance_estimation -a $mode -a $change_json -a $org_json -a $output_json -a $tool";
    printf "cmd = $cmd\n";
    my $safe_cmd = "merlin_safe_exec $cmd :: $file_list :: $enc_time";
    system "cd $dse_dir; touch fast_dse_running";
    system "cd $rose_dir; touch fast_dse_running";
    system "cd $dse_dir; $safe_cmd";
    if(-e "$dse_dir/$output_json") {
        system "cp $dse_dir/$output_json $ann_rpt_local";
    } else {
        printf "WARNING : $output_json did not generated, fast DSE failed\n";
    }
}
if (not -e "$ann_rpt_local") {
    printf "ERROR: Did not find $ann_rpt_local.\n";
    exit;
}

#-------------------------------------------------------
#default is static mode, did not touch dynamic mode now
#execute first step to get cycles
#-------------------------------------------------------
system "cp -r $ann_rpt_local $rose_dir ";
#if($encrypt eq "on") {
#    my @file_set = split(/ /, $file_list);
#    foreach my $one_file (@file_set) {
#        if(-e "$rose_dir/$one_file") {
#            system "cd $rose_dir; script_dec $global_passwd $one_file";
#        }
#    }
#}
$mode = "STATIC";
$cmd  = "mars_opt $file_list -e c -p performance_estimation $include_path -a $mode";
$cmd .= " -a $kernel_list_file -a $ann_rpt -a $dram_param_file";
$cmd .= " -a $step1_json -a $tool";
printf "cmd = $cmd\n";
my $safe_cmd = "merlin_safe_exec $cmd :: $file_list :: $enc_time";
system "cd $rose_dir; $safe_cmd";
my $source_file_list = `cd $rose_dir; ls *.c*`;
my @source_file_set = split(/\n/, $source_file_list);
foreach my $one_file (@source_file_set) {
    if ($one_file =~ /rose_(.*.c.*)$/) {
        system "cd $rose_dir; cp $one_file $1;";
    }
}
if($encrypt eq "on") {
    my @file_set = split(/ /, $file_list);
    foreach my $one_file (@file_set) {
        if(-e "$rose_dir/$one_file") {
            system "cd $rose_dir; script_enc $global_passwd $one_file";
        }
    }
}

#-------------------------------------------------------
# merge perf_rpt_temp and ann_rpt into perf_rpt
#-------------------------------------------------------
system "rm -rf $sim_dir; cp -r $rose_dir $sim_dir";
system "cp -r $rose_dir/$ann_rpt_local $sim_dir ";
$cmd  = "mars_opt $file_list -e c -p performance_estimation $include_path -a MERGE";
$cmd .= " -a $step1_json -a $ann_rpt_local -a $perf_rpt_local";
$safe_cmd = "merlin_safe_exec $cmd :: $file_list :: $enc_time";
system "cd $sim_dir; $safe_cmd";
printf "cd $sim_dir; $safe_cmd\n";
if(-e "$sim_dir/$perf_rpt_local") {
    system "cp $sim_dir/$perf_rpt_local $perf_rpt_local >/dev/null";
    system "cd $sim_dir; cp $perf_rpt_local ../../ >/dev/null";
    if($perf_rpt_local ne $perf_rpt) {
        system "cp $perf_rpt_local $perf_rpt";
    }
} else {
    printf "ERROR: Fail to generate performance estimation report.\n";
}

exit;
