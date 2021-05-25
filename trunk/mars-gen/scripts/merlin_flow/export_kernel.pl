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
my $kernel_dir = "$export_dir/kernel";
my $lc_dir     = "$export_dir/lc";
if(not -e $export_dir) {
    printf "Error: Directory $export_dir not exist.\n";
    exit;
}

#---------------------------------------------------------------------------------------------#
# generate lc directory and files
#---------------------------------------------------------------------------------------------#
run_command "rm -rf $kernel_dir; mkdir $kernel_dir;";
run_command "rm -rf $lc_dir; mkdir $lc_dir;";
run_command "cp $export_dir/*.h $export_dir/*.hpp $export_dir/*.json $export_dir/*.cl $lc_dir 2> /dev/null";

if(not -e "$export_dir/$global_kernel_file_json") {
    printf "Error: File $export_dir/$global_kernel_file_json not exist.\n";
    exit;
}
my @file_list = get_file_list("$export_dir/$global_kernel_file_json");
# copy kernel file to lc directory
foreach my $file_name (@file_list) {
    if(-f "$export_dir/$file_name") {
        run_command "cp $export_dir/$file_name $lc_dir;";
    }
}

# copy lc code to kernel directory
run_command "cd $lc_dir; rm -rf cmost.h 2> /dev/null"; 
run_command("cp -r $lc_dir/* $kernel_dir");

# copy kernel file to lc directory
foreach my $file_name (@file_list) {
    if(-f "$export_dir/$file_name") {
        run_command("cd $kernel_dir; perl -pi -e 's/#include \"cmost\.h\"//g'   $file_name");  #youxiang 20161027 remove dummy statement
        run_command("cd $kernel_dir; perl -pi -e 's/(.*)__merlin_dummy_(.*)//g' $file_name");  #youxiang 20161027 remove dummy statement
    }
}

# do something to match line number between lc code and opencl code
# each file only do this for one time
foreach my $file_name (@file_list) {
    if(-f "$lc_dir/$file_name") {
        run_command "cd $lc_dir; perl -pi -e 's/#include.*\"cmost\.h\"//g' $file_name";
        run_command "cd $lc_dir; perl -pi -e 's/#include <merlin_stream\.h>/#include \"merlin_stream\.h\"/g' $file_name";
        run_command "cd $lc_dir; perl -pi -e 'print \"#define __global\n\" if \$\. == 1' $file_name";
        run_command "cd $lc_dir; perl -pi -e 'print \"#define __kernel\n\" if \$\. == 1' $file_name";
        run_command "cd $lc_dir; perl -pi -e 'print \"#define __constant\n\" if \$\. == 1' $file_name";
    }
}

#---------------------------------------------------------------------------------------------#
# transfer c code to cl code
#---------------------------------------------------------------------------------------------#
run_command "cd $export_dir; make -s kernel_gen";

# delete unnecessary files
run_command "cd $kernel_dir; rm -rf __merlin_opencl_if.h __merlin_opencl_kernel_buffer.h  2> /dev/null";
run_command "cd $kernel_dir; rm -rf __merlin_parameter.h __merlinhead_kernel_top.h __merlinheadwrapper_idct_kernel.h  2> /dev/null";
run_command "cd $kernel_dir; rm -rf __merlintask_top.h kernel_die.h cmost*.h 2> /dev/null";
run_command "cd $lc_dir; rm -rf __merlin_opencl_if.h __merlin_opencl_kernel_buffer.h  2> /dev/null";
run_command "cd $lc_dir; rm -rf __merlin_parameter.h __merlinhead_kernel_top.h __merlinheadwrapper_idct_kernel.h  2> /dev/null";
run_command "cd $lc_dir; rm -rf __merlintask_top.h kernel_die.h cmost*.h 2> /dev/null";

exit;
