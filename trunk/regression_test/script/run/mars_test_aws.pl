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
# Description: Entry to excution of checkin test on AWS+US2 hybrid cloud
# Owners: 
#       Peng Zhang (pengzhang@falcon-computing.com) 2017091
################################################################
#
#

use feature qw(say);

sub show_usage {
    print "Usage: mars_test -aws checkin|bugzilla|unit_test|frontend\n";
}

my $arg_num = @ARGV;
my $is_create_inst = 0;
my $is_create_in_advance= 0;
my $input_cmd = "mars_test checkin -force";
my $exec_dir = ".";
if ($arg_num >= 1)
{
    if ($ARGV[0] eq "help" or $ARGV[0] eq "-help") {
        show_usage();
        exit();
    }
    elsif ($ARGV[0] eq "create_inst") {
        $is_create_inst = 1;
        $is_create_in_advance= 0;
    }
    elsif ($ARGV[0] eq "create_inst_in_advance") {
        $is_create_inst = 1;
        $is_create_in_advance= 1;
    }
    elsif ($ARGV[0] eq "checkin") {
        $input_cmd = "mars_test checkin -force";
        $exec_dir = "merlin_checkin";

        if ($arg_num >=2 and $ARGV[1] eq "-full") {
            $input_cmd = "mars_test checkin -full -force";
        }
    }
    elsif ($ARGV[0] eq "frontend") {
        $input_cmd = "mars_test package -front -force";
        $exec_dir = "package_test";
    }
    elsif ($ARGV[0] eq "bugzilla") {
        $exec_dir = "bugzilla";
        system "rm -rf $exec_dir.bak; mv $exec_dir $exec_dir.bak; mkdir $exec_dir";

        $input_cmd = "cd $exec_dir; mars_test bugzilla; mars_test bugzilla bugzilla.list -cloud -force -nosleep";
    }
    elsif ($ARGV[0] eq "unit_test") {
        $exec_dir = "unit_test";
        system "rm -rf $exec_dir.bak; mv $exec_dir $exec_dir.bak; mkdir $exec_dir";
        $input_cmd = "cd $exec_dir; mars_test unit_test; mars_test unit_test unit_test.list -cloud -force -nosleep";
    }
    elsif ($ARGV[0] eq "create_inst_in_advance") {
        $input_cmd = "$ARGV[1]";
    }
    else {
        show_usage();
        exit();
    }
}
else
{
    show_usage();
    exit();       
}

# initialization of the timer 
my $display_interval= 5; #in seconds
my $timeout_minutes = 60;
my $timeout = $timeout_minutes * 60.0 / $display_interval;
my $start_date = `date "+%m/%d/%y %H:%M:%S"`;
chomp($start_date);
my $sec1= `date +%s`;
chomp($sec1);

my $forks = 0;

if ($is_create_in_advance == 0) {
############################################################
    my $submit_pid = fork();
    if (!$submit_pid) {  # process in the sub-process
#system "mars_test checkin -force";
        system "$input_cmd |& tee mars_test_aws.log";
        exit;
    }
    else {  # main thread
        print "-- Sub-process started for task submitting... PID=$submit_pid\n";
        $forks ++;
    }
}

############################################################
if ($is_create_inst == 1) {
    my $instance_pid = fork(); 
    if (!$instance_pid) {  # process in the sub-process
        my $cmd_arg = "c4.2xlarge 5 4 1 1000";
        system "perl $ENV{MARS_TEST_HOME_GIT}/script/run/create_aws_instance.pl $cmd_arg ";
        exit;
    }
    else { # main thread
        print "-- Sub-process started for AWS instance creating ... PID=$instance_pid\n";
        $forks ++;
    }
}

############################################################
for (1 .. $forks) {
    my $curr_date = `date`; chomp($curr_date);
    print "-- Waiting for a running sub-process ... $curr_date\n";
    my $finish_pid = wait();
    if ($finish_pid == $submit_pid) {
        print "-- Sub-process finished for task submitting... PID=$finish_pid\n";
    }
    elsif ($finish_pid == $instance_pid) {
        print "-- Sub-process finished for AWS instance creating... PID=$finish_pid\n";
    }
    else {
    }
}

if ($is_create_in_advance == 1) {
#system "mars_test checkin -force";
    system "$input_cmd";
}

system "sleep 2";  # wait for .condor_submitted file to be created

my $total_submit = `cd $exec_dir; find . -name "*.condor_submitted" | wc -l`;
chomp($total_submit);

my $i = 0;
for ($i = 0; $i < $timeout ; $i++) {

    my $completed = `cd $exec_dir; find . -name "mars_cloud_done.o" | wc -l`;
    chomp($completed);

    my $curr_date = `date "+%m/%d/%y %H:%M:%S"`;
    chomp($curr_date);

    my $sec2= `date +%s`;
    chomp($sec2);
    my $sec = $sec2 - $sec1;
    my $min = $sec / 60.0;
    my $case_rate = $completed / $min;

    my $min_display = sprintf("%.02f minutes", $min);

    if ($min < 1.0) {
        $min_display = sprintf("%d seconds", $sec);
    }
    $case_rate = sprintf("%.01f", $case_rate);
    $min_display .= " \@ $case_rate Case/Min";

    print " Progress: $completed/$total_submit. \t( $min_display ) \n";

    if ($completed == $total_submit) 
    {
        last;
    }

    system "sleep $display_interval";
}

if ($i >= $timeout) {
    print "ERROR: Checkin test timeout in $timeout_minutes minutes! \n";
}

#system "sleep 10";
#system "mars_test checkin -report"; #prefer to call it outside

#system "perl $ENV{MARS_TEST_HOME_GIT}/script/run/create_aws_instance.pl shutdown";


say "Merlin test is completed in directory $exec_dir/";
say "Please run the corresponding report command to view the testing results";


