#(C) Copyright 2016-2021 Xilinx, Inc.
#All Rights Reserved.
#
#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License. (edited)

################################################################
# Xilinx
# run_fpga_pcie.pl
# 
# This script is only used in FCS internally for automated regression test 
# Please make sure to run the this script at
# 1. the machine which has usb cable to configure the FPGA
# 2. the directory contains the files as implement/pkg where bitstream file, host executable and input data file are located.

use XML::Simple;
use Data::Dumper;
system "whoami";
$user = `whoami`;
$pwd = `pwd`;
#$shell_cmd = `echo \$0`;
$shell_cmd = `echo \$SHELL`;
if($shell_cmd =~ /csh/) {
    $source = "source /curr/mars/fcs_setting64.csh"
} elsif($shell_cmd =~ /sh/) {
    $source = "source /curr/mars/fcs_setting64.sh"
}
printf "$shell_cmd\n";
printf "$source\n";

sub run_time_out {
    $time_out_time = $_[0];
    $time_out_exec = $_[1];

    eval {
        local $SIG{ALRM} = sub { die "alarm\n" };
        alarm $time_out_time;
        printf "WARNING : Run $time_out_exec in timeout mode!\n";
        system "$time_out_exec";
        alarm 0;
    };
    if ($@) {
        die unless $@ eq "alarm\n";   # propagate unexpected errors
        # timed out
        printf "WARNING : ${time_out_time}S TIME OUT! Exit command \'$time_out_exec\'.\n"
    } else {
        # didn't
        # printf "no time out\n"
    }
}

my $bit_xml = XMLin("../../spec/directive.xml");
my $platform_name  = $bit_xml -> {platform_name};
my $impl_tool  = $bit_xml -> {implementation_tool};
printf "platform_name       = $platform_name\n";
printf "implementation_tool = $impl_tool\n\n";

if($platform_name =~ "a10gx") {
    $fpga_server = "10.0.0.32";
} elsif($platform_name eq "de5net_a7") {
    $fpga_server = "root\@10.0.0.4";
}

#printf "$fpga_server\n";
#system("ssh -t $fpga_server 'ls'");
#exit;
$MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
$remote_exec = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/remote_exec.pl";

chomp($pwd);
$dir_name = "";
if ($pwd =~ /.*\/(.*)/) 
{
    $dir_name = $1;
}

#system "rm -rf fpga_run";
#system "mkdir  fpga_run";
if(!-e "run_bit/kernel_top.aocx") {
    printf "ERROR : No kernel_top.aocx file exist!\n";
    exit;
}
#system "cp kernel_top.aocx      fpga_run/";
#system "cp host_top             fpga_run/";
#system "cp aux/*                fpga_run/";
#system "cp Makefile             fpga_run/";
#system "cp altera_run_flow.pl   fpga_run/";
#system "pwd > client_pwd.log";
$timeout = 1200;
printf "timeout = $timeout\n";
if($platform_name =~ "a10gx") {
    run_time_out($timeout, "$remote_exec '$source; mars_env_loader make test platform=$platform_name' -server $fpga_server -copy -ldir ./run_bit");
    #system "$remote_exec '$source; mars_env_loader make test platform=a10gx_es3' -server $fpga_server -copy -ldir ./run_bit";
} elsif($platform_name eq "de5net_a7") {
    run_time_out($timeout, "$remote_exec '$source; mars_env_loader make test platform=$platform_name' -server $fpga_server -copy -ldir ./run_bit");
    #system "$remote_exec '$source; mars_env_loader make test platform=de5net_a7' -server $fpga_server -copy -ldir ./run_bit";
}
print "[merlin_flow test] FPGA PCIe execution finished. \n\n";
