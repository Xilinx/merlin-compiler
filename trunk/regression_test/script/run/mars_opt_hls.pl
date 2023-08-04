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

$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$config_dir = "${MARS_TEST_HOME_GIT}/configuration";

$report_mode = "";
$list_file = $ARGV[1];
for($i=0; $i<@ARGV; $i++) {
    if($ARGV[$i] eq "-report") {
    	$report_mode = "on";
    }
}

sub split_report {
    $report = $_[0];
    $report_info = `cat $report`;
    if($report_info =~ /hls cycles number\s*\n(.*\n.*\n.*\n)([\S\s]*)\n(\+\-.*\n)/) {
        $head = $1;
        $real_data = $2;
        $tail = $3;
    }	
    @data_set=split(/\n/,$real_data);
    $case_number = 0;
    my @common_line = "";
    foreach $one_line (@data_set) {
        if($one_line =~ /^\|\S+\s*(\|?)/) {
            $case_number++;
            $common_line[$case_number] = "$one_line\n";
        } else {
            $common_line[$case_number] .= "$one_line\n";
        }
    }
    $data_line = "";
    $percent_line = "";
    for($i=1; $i<=$case_number; $i=$i+2) {
        $data_line .= $common_line[$i];
    }
    for($i=2; $i<=$case_number; $i=$i+2) {
        $percent_line .= $common_line[$i];
    }

    printf "HLS cycles data\n";
    printf "$head";
    printf "$data_line";
    printf "$tail";
    printf "\n";
    printf "HLS cycles data comparison\n";
    printf "$head";
    printf "$percent_line";
    printf "$tail";
}

if($report_mode eq "") {
    if($list_file eq "") {
        system "rm -rf opt_hls.list";
        system "cp -r $config_dir/opt_hls.list .";
    } else {
        $test_list = `cat $list_file`;
        @test_set=split(/\n/,$test_list);
        foreach $one_case (@test_set) {
            if($one_case =~ /^\s*#(.*)/) {
                #do nothing
            } elsif($one_case =~ /\s*(\S+)\s+(\S+)\s*/) {
                $case_name = $1;
                $case_dir  = $2;
                system "rm -rf $case_dir-$case_name";
                system "mkdir $case_dir-$case_name";
                system "cd $case_dir-$case_name;mars_test -one $case_name $case_dir hls";
            } else {
                #do nothing
            }	
        }
    }
} elsif($report_mode eq "on") {
    system "rm -rf report; mkdir report;";
    $test_list = `cat $list_file`;
    @test_set=split(/\n/,$test_list);
    foreach $one_case (@test_set) {
        if($one_case =~ /^\s*#(.*)/) {
            #do nothing
        } elsif($one_case =~ /\s*(\S+)\s+(\S+)\s*/) {
            $case_name = $1;
            $case_dir  = $2;
#            printf "$1 $2\n";
            system "cd $case_dir-$case_name;mars_test -one $case_name $case_dir hls -report > ../report/$case_dir-$case_name.log";
        } else {
            #do nothing
        }	
    }

    $report_collect = "report/report_collect.log";
    open(my $report_collect_info, '>', $report_collect);
    $count_report = 0;
    $report_list = `ls report`;
    @report_set = split(/\n/, $report_list);
    foreach $one_report (@report_set) {
        $report_info = `cat report/$one_report`;
        if($count_report eq "0") {
            if($report_info =~ /hls cycles compare.*\n(.*\n.*\n.*\n)([\s\S]*)\n(\+\-.*\n)/) {
                #printf "hls cycles number\n";
                #printf "$1";
                printf $report_collect_info "hls cycles number\n";
                printf $report_collect_info "$1";
                $tail = $3;
            }
        }
        if($report_info =~ /hls cycles compare.*\n(.*\n.*\n.*\n)([\s\S]*)\n\+\-/) {
            #printf "$2\n";
            printf $report_collect_info "$2\n";
        }

        $count_report = $count_report + 1;
    }
    #printf "$tail";
    printf $report_collect_info "$tail";
    close $report_collect_info;
    #printf "\n";

    split_report($report_collect);

} else {
    printf "ERROR : Invalid command\n";
}
