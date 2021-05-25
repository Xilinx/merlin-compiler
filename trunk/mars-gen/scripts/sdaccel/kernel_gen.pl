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

my $c2cl_cmd        = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/export_c2cl.pl";

my $export_dir = ".";
my $host_dir   = "$export_dir/host";
my $kernel_dir = "$export_dir/kernel";
my $lib_dir    = "$export_dir/lib_gen";
my $lc_dir     = "$export_dir/lc";
if(not -e $export_dir) {
    printf "Error: Directory $export_dir not exist.\n";
    exit;
}
if(not -e $kernel_dir) {
    printf "Error: Directory $kernel_dir not exist.\n";
    exit;
}

#---------------------------------------------------------------------------------------------#
# generate lc directory and files
#---------------------------------------------------------------------------------------------#

# do something to match line number between lc code and opencl code
if(not -e "$export_dir/$global_kernel_file_json") {
    printf "Error: File $export_dir/$global_kernel_file_json not exist.\n";
    exit;
}
my @file_list = get_file_list("$export_dir/$global_kernel_file_json");
foreach my $file_name (@file_list) {
    if(-e "$kernel_dir/$file_name") {
        run_command "cd $kernel_dir; perl -pi -e 'print \"#define __global\n\" if \$\. == 1' $file_name";
        run_command "cd $kernel_dir; perl -pi -e 'print \"#define __kernel\n\" if \$\. == 1' $file_name";
        run_command "cd $kernel_dir; perl -pi -e 'print \"#define __constant\n\" if \$\. == 1' $file_name";
    }
}

exit;
