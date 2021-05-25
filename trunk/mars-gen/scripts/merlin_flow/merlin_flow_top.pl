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

use strict;
use warnings;

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
# help information
if (@ARGV < 1 ) {
    print "Usage: merlin_flow project [hls|sim|impl|host|sim_config|hw_sim|hw_sim_config] [-s]\n";
    print "       (no flag): basic C-to-OpenCL flow\n";
    print "       hls           : run HLS on the generated OpenCL design\n";
    print "       sim_config    : only generate software simulation configuration file\n";
    print "       hw_sim_config : only generate hardware simulation configuration file\n";
    print "       sim           : run CPU simulation on the generated OpenCL design\n";
    print "       hw_sim        : run hardware simulation on the generated OpenCL design\n";
    print "       impl          : run implementation on the generated OpenCL design\n";
    print "       bit           : same as impl\n";
    print "       host          : create host executable from generated OpenCL design\n";
    print "       lib_gen       : generate library for host program\n";
    print "       -s            : skip user interaction\n";
    exit;
}

my $project_file = $ARGV[0]; #"${project_name}.prj";

# output version information
if ($project_file eq "-v") {
    my $info = `cat $MERLIN_COMPILER_HOME/.version`;
    if($info =~ /Merlin Compiler version : (.*)/) {
        printf "Merlin Compiler Version : $1\n";
    }
    if($info =~ /commit (.*)/) {
        printf "Merlin Compiler Commit Version : $1\n";
    }
    if($info =~ /Build date.*\n(.*)/) {
        printf "Merlin Compiler Build date : $1\n";
    }
    exit;
}

# check if project file exist
if (not -e "${project_file}") {
    MSG_E_3016(${project_file});
    exit;
}

# check if directive.xml exist
my $directive_file = "$prj_spec/directive.xml";
if (not -e "$directive_file") {
    MSG_E_3012();
    exit;
}
# set default platform if environment setted
run_command "$cmd_internal_platform $directive_file";

#---------------------------------------------------------------------------------------------#
# parse command line options
#---------------------------------------------------------------------------------------------#
my $flow = "opt";
my $flow_type = "opt";  # optimization flow
my $original_arg = " ";
my $is_silent    = 0;
for (my $i = 1; $i < @ARGV; $i+=1) {
    if($ARGV[$i] eq "lib_gen" or $ARGV[$i] eq "hls" or $ARGV[$i] eq "host" or $ARGV[$i] eq "sim" or $ARGV[$i] eq "hw_sim"
            or $ARGV[$i] eq "sim_config" or $ARGV[$i] eq "hw_sim_config"
            or $ARGV[$i] eq "bit" or $ARGV[$i] eq "impl" or $ARGV[$i] eq "test" or $ARGV[$i] eq "exec")  {
        $flow = $ARGV[$i];
        if($flow eq "bit") { $flow = "impl"; }
        $flow_type = "exec";    # vendor execution flow
    }
    #################
    # Min : support for syncheck
    #
    elsif ($ARGV[$i] eq "syncheck") {
        $flow_type = "syncheck";
    }

    if ($ARGV[$i] =~ /-silence/ or $ARGV[$i] =~ /-s/ or $ARGV[$i] =~ /-f/) {
        $is_silent = 1;
    }
    $original_arg .= "$ARGV[$i] ";
}

# copy optimization code to code opt
sub exit_code_opt {
    run_command "rm -rf $prj_implement/$prj_src_opt/* ";
    copy_src ("$prj_implement/$prj_code_transform", "$prj_implement/$prj_src_opt");
#    exit;
}

#---------------------------------------------------------------------------------------------#
# check if old project exist
#---------------------------------------------------------------------------------------------#
if($flow_type eq "opt" or $flow_type eq "syncheck") { # Min: support for syncheck
    if (-e "$prj_implement/$prj_export" and not $is_silent) {
        query_exit("Previous design exists. Overwrite?", "no", "no");
    }
    run_command "rm -rf $prj_report $prj_pkg $prj_implement .merlin_flow_has_error .cmost_flow_has_error 2>/dev/null";
}

# if not exist export ditectory, create project and run optimization flow
if (not -e "$prj_implement/$prj_export") {
    # this script contains all frontend, midend and backend optimization steps
    #run_command "$cmd_merlin_flow_opt $project_file $flow_type \"$original_arg\"";
    run_command "echo $xml_include_path > .mount_include";
    run_command "echo $project_file $flow_type \'$original_arg\' > .merlin_core_args";
    if($xml_evaluate eq "") {$xml_evaluate = "off";}
    run_command "merlin_license $xml_impl_tool $xml_evaluate \"--flow opt --args .merlin_core_args\"";
    if(-e ".license_check_pass") {
        # license check pass
        run_command "rm -rf .license_check_pass";
    } else {
        printf($MSG_E_3080);
        exit;
    }
    check_error("$prj_implement/$prj_code_transform");
    my $final_code_dir = "$prj_implement/$prj_opencl/$prj_final_code_gen";
    check_error("$final_code_dir");
    if($xml_systolic_array ne "on") {
		run_command "$cmd_backend -flow post -src \"$prj_implement/$prj_src_opt\" -dst $final_code_dir";
		check_error("$final_code_dir");
		run_command "rm -rf $prj_implement/$prj_export";
		if (-e "$final_code_dir/$prj_export") {
			run_command "cp -r  $final_code_dir/$prj_export $prj_implement/$prj_export";
		}
    }
    if (-e "$prj_implement/$prj_code_transform/critical_message.rpt") {
		run_command "cp -r $prj_implement/$prj_code_transform/critical_message.rpt $prj_implement/$prj_export";
    }
}

#---------------------------------------------------------------------------------------------#
# backend flow
#---------------------------------------------------------------------------------------------#
if($flow eq "lib_gen" or $flow eq "host" or $flow eq "hls" or $flow eq "sim" or $flow eq "hw_sim" or $flow eq "impl"
        or $flow eq "sim_config" or $flow eq "hw_sim_config"
        or $flow eq "test" or $flow eq "exec") {
    # FIXME
    # in order to match testing environment, need to use specific execution directory
    my $src_dir = "implement/export";
    my $dst_dir = "implement/exec";
    if($flow eq "host") {
        $dst_dir = "implement/exec/host";
    } elsif($flow eq "lib_gen") {
        $dst_dir = "implement/exec/lib_gen";
    } elsif($flow eq "hls") {
        $dst_dir = "implement/exec/hls";
    } elsif($flow eq "sim" or $flow eq "sim_config") {
        $dst_dir = "implement/exec/run_sim";
    } elsif($flow eq "hw_sim" or $flow eq "hw_sim_config") {
        $dst_dir = "implement/exec/hw_run_sim";
    } elsif($flow eq "impl") {
        $dst_dir = "implement/exec/phys";
    } elsif($flow eq "test" or $flow eq "exec") {
        $dst_dir = "implement/exec";
    }
    run_command "$cmd_backend -flow $flow -src $src_dir -dst $dst_dir $original_arg";
    # pkg files collection
    if(-e "$dst_dir/pkg" and -e "pkg") {
        system "cp -r $dst_dir/pkg/* pkg  2>/dev/null";
    }
    if($flow eq "test") { system "cp -r $dst_dir/run_sim/pkg/* pkg  2>/dev/null"; }

    # report file collection
    my $report_dir = "report";
    if(not -e $report_dir) { run_command "mkdir $report_dir"; }
    system "cp -r $dst_dir/report_merlin/* $report_dir 2>/dev/null";
}

#---------------------------------------------------------------------------------------------#
# report collection
#---------------------------------------------------------------------------------------------#
#FIXME
run_command "$cmd_report_summary";

exit;

#---------------------------------------------------------------------------------------------#
# useless
#---------------------------------------------------------------------------------------------#
if(-e "error_platform") { exit; }
if(-e ".exit") {run_command "rm -rf .exit"; exit;}

