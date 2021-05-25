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

my $export_dir = $prj_export;
my $host_dir   = "$export_dir/host";
my $lib_dir    = "$export_dir/lib_gen";
if(not -e $export_dir) {
    printf "Error: Directory $export_dir not exist.\n";
    exit;
}
if(not -e $lib_dir) {
    printf "Error: Directory $lib_dir not exist.\n";
    exit;
}

#---------------------------------------------------------------------------------------------#
# copy releted to host files, then delete library and kernel related files, no related to user 
#---------------------------------------------------------------------------------------------#
$xml_include_path =~ s/-I/ -I/g;
$xml_include_path =~ s/\//\\\//g;
run_command "rm -rf $host_dir; mkdir $host_dir;";
run_command "cp $export_dir/*.c $export_dir/*.cpp $export_dir/*.h $export_dir/*.tcl $export_dir/Makefile $host_dir 2>/dev/null;";
if(-e "$lib_dir/bin") {
    run_command "cp $lib_dir/bin/* $host_dir 2> /dev/null";
}
run_command "cd $host_dir; mars_perl $dir_scripts/merlin_flow/add_extern.pl";
run_command "cd $host_dir; perl -pi -e 's/#include \"__merlin_opencl_if\.h\"//g' cmost.h 2> /dev/null"; 
run_command "cd $host_dir; perl -pi -e 's/#include \"__merlin_opencl_kernel_buffer\.h\"//g' cmost.h 2> /dev/null"; 
run_command "cd $host_dir; perl -pi -e 's/\\\$\\\(MERLIN_INCLUDE\\\)/$xml_include_path/g' Makefile 2> /dev/null"; 
run_command "cd $host_dir; perl -pi -e 's/INC_DIRS \:=/INC_DIRS \:= $xml_include_path/g' Makefile";
run_command "cd $host_dir; rm -rf __merlin_opencl_if.* __merlin_opencl_kernel_buffer.* __merlin_timer.c __merlinwrapper_* 2> /dev/null";
run_command "cd $host_dir; rm -rf channel_def.h merlin_include.h __merlinheadwrapper* __merlinkernel_* __merlintask_*.c* 2>/dev/null";
if(not -e "$export_dir/$global_kernel_file_json") {
    printf "Error: File $export_dir/$global_kernel_file_json not exist.\n";
    exit;
}
my @file_list = get_file_list("$export_dir/$global_kernel_file_json");
# copy kernel file to lc directory
# delete kernel file fomr host
foreach my $file_name (@file_list) {
    run_command "cd $host_dir; rm -rf $file_name;";
}


#delete unnecessary files
#run_command "rm -rf __merlinwrapper* __merlin_task* 2> /dev/null";
