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
#use lib "$library_path";
#use message;
#use global_var;
#use merlin;
#use strict;
#use warnings;

my $xml_file = $ARGV[0];
my $info = `cat $xml_file`;
my $tool = "";
my $device = "";
if($info =~ /<implementation_tool>(.*)<\/implementation_tool>/) {
    $tool = $1
}

my $xilinx_env = $ENV{'MERLIN_INTERNAL_PLATFORM_XILINX'};
#printf "xilinx_env = $xilinx_env\n";
if($xilinx_env =~ /^sdaccel::/ and $tool eq "sdaccel") {
    if($xilinx_env =~ /sdaccel::(.*)$/) {
        $device = $1;
        system "perl -pi -e 's/<platform_name>.*<\\\/platform_name>/<platform_name>$device<\\\/platform_name>/g' $xml_file";
    }
}
