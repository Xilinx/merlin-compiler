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
# Usage       : merlin_flow design flow
# Owner(s)    : Han
# version     : 10-18-2016 Create file and basic function
##############################################################

#use strict;
#use warnings;

#---------------------------------------------------------------------------------------------#
# excute before everything
#---------------------------------------------------------------------------------------------#
my $curr_dir;
my $MERLIN_COMPILER_HOME;
my $library_path;
BEGIN {
    $|=1;   #turn on auto reflush for stdout
    $curr_dir = `pwd`;
    chomp($curr_dir);
    $ENV{"MERLIN_CURR_PROJECT"} = $curr_dir;
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    our $xml_dir = "";
}
#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use lib "$library_path";
use message;
use global_var;
use merlin;

#---------------------------------------------------------------------------------------------#
# get spec file
#---------------------------------------------------------------------------------------------#
my $project_file    = $ARGV[0]; #"${project_name}.prj";
my $flow_type       = $ARGV[1]; #syncheck;
my $original_arg    = $ARGV[2];

# check if directive.xml exist
my $directive_file = "$prj_spec/directive.xml";
if (not -e "$directive_file") {
    MSG_E_3012();
    exit;
}

#---------------------------------------------------------------------------------------------#
# parse command line options
#---------------------------------------------------------------------------------------------#
# copy optimization code to code opt
sub exit_code_opt {
    run_command "rm -rf $prj_implement/$prj_src_opt/* ";
    copy_src ("$prj_implement/$prj_code_transform", "$prj_implement/$prj_src_opt");
#    exit;
}

# if not exist export ditectory, create project and run optimization flow
if (not -e "$prj_implement/$prj_export") {
    #---------------------------------------------------------------------------------------------#
    # project directory structure created
    #---------------------------------------------------------------------------------------------#
    run_command "$cmd_create_dir \"$original_arg\" $project_file";

    #---------------------------------------------------------------------------------------------#
    # frontend generation
    # if library only mode or baseline mode, do not run midend pass
    #---------------------------------------------------------------------------------------------#
    run_command "cd $prj_implement/$prj_code_transform; $cmd_frontend -src . -dst .";
    check_error("$prj_implement/$prj_code_transform");

    if ($flow_type eq "syncheck") {
      exit_code_opt;  
      exit;
    }

    if($xml_libonly eq "on")            { exit_code_opt; }
    if($xml_opt_effort eq "baseline")   { exit_code_opt; }

    if($xml_auto_dse eq "on" or $xml_auto_dse eq "custom") {
        #-----------------------------------------------------------------------------------------#
        # design space exploration
        #-----------------------------------------------------------------------------------------#
        # In DSE mode, we do not need to check license again
        # because we check it before
        $ENV{"MERLIN_DSE"} = "ON";
        #TODO environment may not stable if multiple dse running at the same time
        run_command "touch $prj_implement/$prj_code_transform/is_dse_project";
        run_command "$cmd_auto_dse $prj_implement/$prj_dse $prj_implement/$prj_code_transform $xml_auto_dse";
        check_error("$prj_implement/$prj_dse");
        run_command "rm -rf $prj_implement/$prj_code_transform/is_dse_project";
        $ENV{"MERLIN_DSE"} = "OFF";
    }

    #---------------------------------------------------------------------------------------------#
    # midend generation
    #---------------------------------------------------------------------------------------------#
    run_command "cd $prj_implement/$prj_code_transform; $cmd_midend -src . -dst .";
    check_error("$prj_implement/$prj_code_transform");
    exit_code_opt;

    #---------------------------------------------------------------------------------------------#
    # backend flow(kernel generation)
    #---------------------------------------------------------------------------------------------#
    if($xml_systolic_array ne "on") {
		my $final_code_dir = "$prj_implement/$prj_opencl/$prj_final_code_gen";
		run_command "mkdir $final_code_dir";
		run_command "$cmd_backend -flow c2cl -src \"$prj_implement/$prj_src_opt\" -dst $final_code_dir";
		check_error("$final_code_dir");
#		run_command "rm -rf $prj_implement/$prj_export";
#		if (-e "$final_code_dir/$prj_export") {
#			run_command "cp -r  $final_code_dir/$prj_export $prj_implement/$prj_export";
#		}
    }
}

exit;
