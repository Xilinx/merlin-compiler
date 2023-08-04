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
$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};

$report_flag = 0;
$force = 0;
for ($i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] eq "-force") {
        $force = 1;
    }
    if($ARGV[$i] eq "-report") {
        $report_flag = 1;
    }
    if($ARGV[$i] eq "-full") {
        $full_flag = 1;
    }
}

sub check_report_unit {
    $file = $_[0];
    if(-e "$work_dir/${file}.log") {
        $error_count = 0;
        $test_info = `cat $work_dir/${file}.log`;
        @test_lines = split(/\n/,$test_info);
        foreach $one_line (@test_lines) {
            if($one_line =~ /\|(.*)\|\s*(.*)\|/){
                if($2 eq "Pass") {
                } else {
                    if($2 =~ /CORRECT/) {
                    } else {
                        printf "$1 $2\n";
                        $error_count = $error_count + 1;
                    }
                }
            }
        }
        if($error_count == 0) {
            printf "$file passed\n";
        }
    } else {
        printf "${file}.log not exist\n";
    }
}
sub check_report {
    $file = $_[0];
    $error_count = 0;
    $file_info = `cat $file`;
    @file_lines = split(/\n/,$file_info);
    foreach $one_line (@file_lines) {
        if($one_line =~ /\|(.*)\|.*\|.*\|.*\|.*\|.*\|.*\|\s*(.*)\|/){
            if($2 eq "Pass") {
            } else {
                if($2 =~ /Correctness/) {
                } else {
                    printf "$1 $2\n";
                    $error_count = $error_count + 1;
                }
            }
        }
    }
    if($error_count == 0) {
        printf "passed\n";
    }
}

$curr_dir = `pwd`;
chomp($curr_dir);
$work_dir = "$curr_dir/merlin_checkin";

if($report_flag == 0) {
    if(-e $work_dir) {
        if($force eq 1) {
            system "rm -rf $work_dir";
        } else {
            printf "$work_dir exist\n";
            exit;
        }
    }
    system "mkdir $work_dir";
    #one hour test
    system "mkdir $work_dir/one_hour;";
    system "cd $work_dir/one_hour;";
    system "cd $work_dir/one_hour; cp -r $MARS_TEST_HOME_GIT/configuration/one_hour_test_fast.xml .";
    system "cd $work_dir/one_hour; mars_test -h one_hour_test_fast.xml -cloud -force";
    #unit test
    system "mkdir $work_dir/unit_test;";
    system "cd $work_dir/unit_test;";
    system "cd $work_dir/unit_test; mars_test unit_test";
    system "cd $work_dir/unit_test; mars_test unit_test unit_test.list -cloud -force -nosleep";
    #bugizlla
    system "mkdir $work_dir/bugzilla;";
    system "cd $work_dir/bugzilla;";
    if($full_flag eq 1) {
        system "cd $work_dir/bugzilla; cp -r $MARS_TEST_HOME_GIT/configuration/bugzilla.list .";
        system "cd $work_dir/bugzilla; mars_test bugzilla bugzilla.list -cloud -force -nosleep";
    } else {
        system "cd $work_dir/bugzilla; cp -r $MARS_TEST_HOME_GIT/configuration/bugzilla_fast.list .";
        system "cd $work_dir/bugzilla; mars_test bugzilla bugzilla_fast.list -cloud -force -nosleep";
    }
    #commandline
    system "mkdir $work_dir/commandline;";
    system "cd $work_dir/commandline; cp -r $MARS_TEST_HOME_GIT/configuration/commandline_fast.list .";
    system "cd $work_dir/commandline; mars_test commandline commandline_fast.list -cloud -force -nosleep";

} elsif($report_flag == 1) {
    printf "$work_dir\n";
    system "cd $work_dir/one_hour/report; cp -r frontend.log $work_dir";

    printf "\n\n\n\n\n\n";
    #display report
    printf "\nReporting unit test...\n";
    check_report_unit("unit_test");
    printf "\nReporting bugzilla...\n";
    check_report_unit("bugzilla");
    printf "\nReiporting commandline...\n";
    check_report_unit("commandline");
    check_report_frontend("frontend.log");
}
