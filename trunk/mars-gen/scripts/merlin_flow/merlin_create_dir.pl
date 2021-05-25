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
use lib "$library_path";
use message;
use merlin;
use global_var;

#---------------------------------------------------------------------------------------------#
# parse command lines and get flags
#---------------------------------------------------------------------------------------------#
my $original_arg   = $ARGV[0];
my $project_file   = $ARGV[1];
my $flow           = $ARGV[2];
my @arg_set = split(/ /, $original_arg);
foreach  my $one_arg (@arg_set) {
}


#---------------------------------------------------------------------------------------------#
# work directories define
#---------------------------------------------------------------------------------------------#
run_command "mkdir  $prj_implement";
if (not -e "$prj_report") {
  run_command "mkdir  $prj_report";
}
run_command "mkdir  $prj_pkg";
run_command "mkdir  $prj_implement/$prj_code_transform";
run_command "mkdir  $prj_implement/$prj_src_org";
run_command "mkdir  $prj_implement/$prj_src_opt";
run_command "mkdir  $prj_implement/$prj_opencl";
run_command "mkdir  $prj_implement/$prj_exec";
run_command "mkdir  $prj_implement/$prj_export";
run_command "mkdir  $prj_implement/$prj_dse";
run_command "mkdir  $prj_implement/$prj_aux";
run_command "mkdir  $prj_implement/$prj_metadata";

# classify data and host code
run_command "python ${MERLIN_COMPILER_HOME}/mars-gen/scripts/project/copy_project_file.py src ${project_file} $prj_implement/$prj_src_org";
run_command "python ${MERLIN_COMPILER_HOME}/mars-gen/scripts/project/copy_project_file.py aux ${project_file} $prj_implement/$prj_aux";

# copy data file to aux
if(-e "$prj_implement/$prj_aux") {
    run_command "cd $prj_implement/$prj_aux; rm -rf Makefile makefile";
    run_command "cp $prj_implement/$prj_aux/* $prj_pkg 2>/dev/null";
    run_command "cp -r $prj_implement/$prj_aux $prj_implement/$prj_code_transform/$prj_aux 2>/dev/null";
}
# copy source code to code transform
if(-e "$prj_implement/$prj_src_org") {
    run_command "cd $prj_implement; cp $prj_src_org/* $prj_code_transform";
    run_command "cd $prj_implement; cp -r $prj_src_org/ $prj_code_transform/src";
}

my $directive_file = get_value_from_xml("spec_file", $project_file);
if (not -e "$directive_file") {
    MSG_E_3012();
    exit;
} else {
    run_command "cp -r $directive_file $prj_implement/$prj_code_transform";
}
exit;
