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

#---------------------------------------------------#
# collect required files from export directory
#---------------------------------------------------#
my $work_dir = ".";
my $export_dir = $prj_export;
if(not -e $export_dir) {
    printf "Error: Directory $export_dir not exist.\n";
    exit;
}
run_command "cp $work_dir/*.json $work_dir/*.c $work_dir/*.h $work_dir/*.hpp $work_dir/*.cpp $work_dir/*.cl $work_dir/*.tcl $work_dir/*.xml $export_dir 2>/dev/null";
if (-e "$work_dir/aux") {
   run_command "cp -r  $work_dir/aux $export_dir 2>/dev/null";
   run_command "cp -r  $work_dir/aux/* $work_dir 2>/dev/null";
}
#---------------------------------------------------#
# copy opencl API files to export
#---------------------------------------------------#
run_command "cp $dir_cmost_headers/*.*  $export_dir 2>/dev/null";
run_command "cp $dir_api/*.*            $export_dir 2>/dev/null";

#---------------------------------------------------------------------------------------------#
# TODO: too complicate about the cpp or c file convert
# do some process on output file
#---------------------------------------------------------------------------------------------#
run_command "touch $export_dir/cmost_access.h";
run_command "cd $export_dir; rm -rf __merlinoldkernel_*";
if (-e "$export_dir/__merlin_api_list.cpp") {
   run_command "cd $export_dir; mv __merlin_api_list.cpp __merlin_api_list.h >& /dev/null ";
}
if (-e "$export_dir/__merlin_opencl_if.cpp") {
   run_command "cd $export_dir; mv __merlin_opencl_if.cpp __merlin_opencl_if.c >& /dev/null";
}
if (-e "$export_dir/__merlin_opencl_kernel_buffer.cpp") {
   run_command "cd $export_dir; mv __merlin_opencl_kernel_buffer.cpp __merlin_opencl_kernel_buffer.c >& /dev/null";
}
run_command "cd $export_dir; ".q(perl -pi -e 's/(.*)__CMOST_COMMENT__(.*)/\/\/\1\2/g'     *.c*);
#add cmost.h in opencl generation file of wrapper file
my $source_list = `cd $export_dir; ls *.c* 2>/dev/null`;
my @source_set = split(/\n/, $source_list);
foreach my $file (@source_set) {
    if ($file =~ '__merlinwrapper_') {
        run_command("cd $export_dir; sed -i '1i\\#include \"cmost.h\"' $file");
    }
}
run_command "cd $export_dir; mv __merlin_parameter_h.cpp __merlin_parameter.h >& /dev/null";
run_command "cd $export_dir; mv __merlin_opencl_kernel_buffer_h.cpp __merlin_opencl_kernel_buffer.h >& /dev/null";
# check if cpp design or c design
if(check_cpp("$export_dir")) {
    run_command "cd $export_dir; mv __merlin_opencl_if.c __merlin_opencl_if.cpp 2>/dev/null";
    run_command "cd $export_dir; mv __merlin_opencl_kernel_buffer.c __merlin_opencl_kernel_buffer.cpp 2>/dev/null";
    run_command "cd $export_dir; mv cmost.c cmost.cpp 2>/dev/null";
    run_command "cd $export_dir; mv __merlin_timer.c __merlin_timer.cpp 2>/dev/null";
}
# do some common replacement 
run_command "cd $export_dir; perl -pi -e 's/::__PRETTY_FUNCTION__/__PRETTY_FUNCTION__/g'    	     *.c *.cpp 2>/dev/null";
run_command "cd $export_dir; perl -pi -e 's/#\\s*include\\s*\"__merlin_math.h\"/#include <math.h>/g' *.c *.cpp 2>/dev/null";
run_command "cd $export_dir; perl -pi -e 's/#\\s*include\\s*<__merlin_math.h>/#include <math.h>/g'   *.c *.cpp 2>/dev/null";
run_command "cd $export_dir; perl -pi -e 's/#include \"cl\.h\"/#include \"cmost\.h\"/g'              *.c *.cpp 2>/dev/null";
run_command "cd $export_dir; perl -pi -e 's/#include <rose_edg_required_macros_and_functions.h>//g'  *.c* 2>/dev/null";
run_command "cd $export_dir; perl -pi -e 's/#include \"cmost_access_c.h\"//g'                        *.c *.cpp 2>/dev/null";
run_command "cd $export_dir; perl -pi -e 's/#pragma ACCEL string //g'                                *.c *.cpp 2> /dev/null"; 
# remove too long name for Xilinx
# temp if repeat ILi three times, we will delete the useless name
#run_command "cd $export_dir; perl -pi -e 's/ILi\\S+ILi\\S+ILi\\S+?\\(/\\(/g'                          *.c *.cpp 2> /dev/null";

# delete time related functions and files
run_command "cd $export_dir; rm -rf __merlin_timer.c* __merlin_timer.h";
run_command "cd $export_dir; perl -pi -e 's/ timer_begin(.*);//g' 	            *.c *.cpp *.h 2> /dev/null";
run_command "cd $export_dir; perl -pi -e 's/ timer_total(.*);//g' 	            *.c *.cpp *.h 2> /dev/null";
run_command "cd $export_dir; perl -pi -e 's/ start_timer(.*);//g' 	            *.c *.cpp *.h 2> /dev/null";
run_command "cd $export_dir; perl -pi -e 's/ acc_comp_timer(.*);//g' 	        *.c *.cpp *.h 2> /dev/null";
run_command "cd $export_dir; perl -pi -e 's/ report_timer(.*)//g'    	        *.c *.cpp *.h 2> /dev/null";
run_command "cd $export_dir; perl -pi -e 's/#include.*\"__merlin_timer\.h\"//g' *.c *.cpp *.h 2> /dev/null";

if($xml_debug_info eq "on") {
    run_command "cd $export_dir; perl -pi -e 's/#pragma ACCEL debug //g'        *.c *.cpp *.h 2> /dev/null"; 
}

# add more API definition task level __merlintask_top.h file
if(-e "$export_dir/__merlintask_top.h") {
    my $file_info_1 = `cat $export_dir/__merlintask_top.h`;
    my $file_info_2 = "";
    if(-e "$export_dir/__merlinhead_kernel_top.h") {
        $file_info_2 = `cat $export_dir/__merlinhead_kernel_top.h`;
    }
    my $tmp_info = $file_info_1.$file_info_2;
    my $tmp_file = "$export_dir/tmp";
    open my $file, ">$tmp_file";
    print $file $tmp_info;
    close $file;
    run_command "cd $export_dir/; mv tmp __merlintask_top.h";
}

# copy vendor tool specific files, and do some preprocess on generated code
if(-e "$dir_scripts/$xml_impl_tool/Makefile") {
    run_command "cp $dir_scripts/$xml_impl_tool/Makefile $export_dir";
}
run_command "cd $export_dir; make -s preprocess";

# delete task kernel and move task wrapper function to real task function
if(-e "$export_dir/$global_task_file") {
    my @task_list = get_task_list("$export_dir/$global_task_file");
    foreach my $one_task (@task_list) {
        my $file_list = `cd $export_dir; ls *.c* 2>/dev/null`;
        my @file_set = split(/\n/, $file_list);
        foreach my $one_file (@file_set) {
            if($one_file =~ /__merlinkernel_$one_task(\.c.*)$/) {
                run_command "cd $export_dir; rm -rf $one_file";
                run_command "cd $export_dir; perl -pi -e 's/^.*__merlinkernel_$one_task\\\.c.*\\n//g' kernel_top.cl 2> /dev/null";
            }
            if($one_file =~ /__merlinwrapper_$one_task(\.c.*)$/) {
                run_command "cd $export_dir; mv $one_file __merlintask_$one_task$1";
            }
        }
    }
}
exit;
