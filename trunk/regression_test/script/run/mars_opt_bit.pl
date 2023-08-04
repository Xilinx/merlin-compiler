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

if($report_mode eq "") {
    if($list_file eq "") {
        system "rm -rf opt_bit.list";
        system "cp -r $config_dir/opt_bit.list .";
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
                system "cd $case_dir-$case_name;mars_test -one $case_name $case_dir bit &";
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
            system "cd $case_dir-$case_name;mars_test -one $case_name $case_dir bit -report > ../report/$case_dir-$case_name.log";
        } else {
            #do nothing
        }	
    }

#    $report_collect = "report/report_collect.log";
#    $report_compare = "report/report_compare.log";
#    open(my $report_collect_info, '>', $report_collect);
#    open(my $report_compare_info, '>', $report_compare);
    $count_report = 0;
    $report_list = `ls report`;
    @report_set = split(/\n/, $report_list);
    foreach $one_report (@report_set) {
        $report_info = `cat report/$one_report`;
        if($count_report eq "0") {
            if($report_info =~ /fpga compute time compare.*\n(.*\n.*\n.*\n)([\s\S]*)\n(\+\-.*\n?)/) {
                $head = $1;
                $tail = $3;
            }
        }
	if($report_info =~ /slack_compare/) 	 { $slack_flag = 1; 		}
	if($report_info =~ /fpga compute time/)  { $fpga_compute_flag = 1; 	}
	if($report_info =~ /pcie transfer time/) { $pcie_transfer_flag = 1; 	}
	if($report_info =~ /fpga total time/)    { $fpga_total_flag = 1; 	}
	if($report_info =~ /cpu total time/)     { $cpu_total_flag = 1; 	}
	if($report_info =~ /speed up/) 		 { $speed_up_flag = 1; 		}
	if($report_info =~ /fpga result/) 	 { $fpga_result_flag = 1; 	}

        $count_report = $count_report + 1;
    }
#slack compare
    if($slack_flag eq "1") {
        printf "slack compare\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /slack_compare.*\n(.*\n.*\n.*\n)(.*)\n/) {
                printf "$2\n";
            }
        }
        printf "$tail";
    }
#fpga compute time
    if($fpga_compute_flag eq "1") {
        printf "fpga compute time data\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /fpga compute time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$2\n";
            }
        }
        printf "$tail";
        printf "fpga compute time compare\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /fpga compute time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$3\n";
            }
        }
        printf "$tail";
    }
#pcie transfer time
    if($pcie_transfer_flag eq "1") {
        printf "pcie transfer time data\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /pcie transfer time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$2\n";
            }
        }
        printf "$tail";
        printf "pcie transfer time compare\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /pcie transfer time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$3\n";
            }
        }
        printf "$tail";
    }
#fpga total time
    if($fpga_total_flag eq "1") {
        printf "fpga total time data\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /fpga total time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$2\n";
            }
        }
        printf "$tail";
        printf "fpga total time compare\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /fpga total time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$3\n";
            }
        }
        printf "$tail";
    }
#cpu total time
    if($cpu_total_flag eq "1") {
        printf "cpu total time data\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /cpu total time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$2\n";
            }
        }
        printf "$tail";
        printf "cpu total time compare\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /cpu total time.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$3\n";
            }
        }
        printf "$tail";
    }
#speed up
    if($speed_up_flag eq "1") {
        printf "speed up data\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /speed up compare.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$2\n";
            }
        }
        printf "$tail";
        printf "speed up compare\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /speed up compare.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$3\n";
            }
        }
        printf "$tail";
    }
#fpga result
    if($fpga_result_flag eq "1") {
        printf "fpga result correctness\n";
        printf "$head";
        foreach $one_report (@report_set) {
            $report_info = `cat report/$one_report`;
            if($report_info =~ /fpga result compare.*\n(.*\n.*\n.*\n)(.*)\n(.*)\n/) {
                printf "$2\n";
            }
        }
        printf "$tail";
    }

#    close $report_collect_info;
#    close $report_compare_info;
} else {
    printf "ERROR : Invalid command\n";
}
