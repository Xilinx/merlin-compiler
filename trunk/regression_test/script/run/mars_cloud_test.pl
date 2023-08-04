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
$MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};

sub replace_symbol {
    $argument = $_[0];
    $argument =~ s/__001__/\"/;
    $argument =~ s/__002__/;/;
    $argument =~ s/__003__/\'/;
    return $argument;
}

if(@ARGV eq 0) {
    printf "\n";
    printf "mars_cloud_test\n";
    printf "-input   : input files or directories, maximum 100\n";
    printf "-output  : output files or directories, maximum 100\n";
    printf "-timeout : cloud test timeout, unit is minute, default value 5\n";
    printf "-execute : execute command\n";
    printf "-env : environment settings\n";
    exit;
}


$envfile = "fcs_setting64.sh";
$username = `whoami`;
chomp($username);
my @file_input;
my @file_output;
$time_out = 5;
$exec_command = "";
for($i=0; $i<@ARGV; $i++) {
    if($ARGV[$i] eq "-input") {
        for($j=0;$j<100;$j++) {
            if($ARGV[$i+1+$j] =~ /^-output/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-timeout/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-env/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-execute/) {
                last;
            } elsif($ARGV[$i+1+$j] eq "") {
                last;
            } else {
                $file_input[$j] = $ARGV[$i+1+$j];
                printf "input file  = $file_input[$j]\n";
            }
        }
    }
    if($ARGV[$i] eq "-output") {
        for($j=0;$j<100;$j++) {
            if($ARGV[$i+1+$j] =~ /^-timeout/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-input/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-execute/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-env/) {
                last;
            } elsif($ARGV[$i+1+$j] eq "") {
                last;
            } else {
                $file_output[$j] = $ARGV[$i+1+$j];
                printf "output file = $file_output[$j]\n";
            }
        }
    }
    if($ARGV[$i] eq "-env") {
        $envfile = $ARGV[$i+1];
        printf "envfile = $envfile\n";
    }
    if($ARGV[$i] eq "-timeout") {
        $time_out = $ARGV[$i+1];
    }
    if($ARGV[$i] eq "-execute") {
        for($j=0;$j<100;$j++) {
            if($ARGV[$i+1+$j] =~ /^-output/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-input/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-timeout/) {
                last;
            } elsif($ARGV[$i+1+$j] =~ /^-env/) {
                last;
            } else {
                $exec_command .= "$ARGV[$i+1+$j] ";
                $exec_command = replace_symbol($exec_command);
            }
        }
    }
}
printf "time out    = $time_out mins\n";
$time_out = $time_out * 60;
printf "command     = $exec_command\n";

$mars_cloud_command_string = <<EOF;
#!/bin/bash -l
date
whoami
hostname
pwd
date >$file_output[0].condor_output/mars_cloud_start.o
export XILINX_LOCAL_USER_DATA=no
source /curr/$username/$envfile;
$exec_command

cp $file_output[0]/err.log $file_output[0].condor_output/
cat $file_output[0].condor_output/mars_cloud_start.o >$file_output[0].condor_output/mars_cloud_done.o
date >>$file_output[0].condor_output/mars_cloud_done.o
EOF

$command_file = "$file_output[0]_condor";
system "rm -rf $command_file";
open(my $fh, '>', $command_file);
print $fh $mars_cloud_command_string;
close $fh;

$mars_condor_job_string  = "";
$mars_condor_job_string .= "executable=$command_file\n";
$mars_condor_job_string .= "\n";
$name = "";
if(@file_input ne 0) {
    $mars_condor_job_string .= "transfer_input_files=";
    for($i=0; $i<@file_input; $i++) {
        $mars_condor_job_string .= "$file_input[$i],";
        $name .= "$file_input[$i]";
    }
    $mars_condor_job_string .= ",$file_output[0].condor_output";
    $mars_condor_job_string .= "\n";
}
if(@file_input ne 0) {
    $mars_condor_job_string .= "transfer_output_files=";
    for($i=0; $i<@file_output; $i++) {
        my $force_transfer = $ENV{'MARS_TEST_CONDOR_FORCE_TRANSFER'};
        if ($i > 0 or $force_transfer eq "1" or $force_transfer eq "true" or $force_transfer eq "TRUE")  {
            $mars_condor_job_string .= "$file_output[$i],";
        }
    }
    $mars_condor_job_string .= ",$file_output[0].condor_output";
    $mars_condor_job_string .= "\n";
}
$mars_condor_job_string .= "\n";
$mars_condor_job_string .= "universe=vanilla\n";
$mars_condor_job_string .= "output=mars.output.$name\n";
$mars_condor_job_string .= "error=mars.error.$name\n";
$mars_condor_job_string .= "log=mars.log.$name\n";
$mars_condor_job_string .= "notification=never\n";
$mars_condor_job_string .= "priority=0\n";
$mars_condor_job_string .= "periodic_hold = ((JobStatus == 2) && (time() - EnteredCurrentStatus) >  $time_out)\n";
$mars_condor_job_string .= "should_transfer_files=YES\n";
$mars_condor_job_string .= "when_to_transfer_output=ON_EXIT_OR_EVICT\n";
$mars_condor_job_string .= "request_cpus=1\n";
$mars_condor_job_string .= "request_memory=200\n";
$mars_condor_job_string .= "request_disk=200\n";
$mars_condor_job_string .= "run_as_owner=true\n";
$mars_condor_job_string .= "queue\n";
$mars_condor_job_string .= "\n";

$condor_file = "mars_job_$file_output[0].condor";
system "rm -rf $condor_file";
open(my $fh, '>', $condor_file);
print $fh $mars_condor_job_string;
close $fh;

system "rm -rf $file_output[0].condor_output; mkdir $file_output[0].condor_output";
system "date > $file_output[0].condor_output/$file_output[0].condor_submitted";
system "condor_submit $condor_file";
