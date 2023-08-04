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
use File::Basename;
use XML::Simple;
use Data::Dumper;

$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$report_dir = "${MARS_TEST_HOME_GIT}/script/report";
#$case_dir = "${MARS_TEST_HOME_GIT}/test_case";
$history_dir = "${MARS_TEST_HOME_GIT}/history";

for($i=0;$i<@ARGV;$i++) {
    if($ARGV[$i] eq "-x") {
        $xml_file = $ARGV[$i+1];
    }
}

if(-e "report") {
    printf "report directory already exist.\n";
} else {
    system "mkdir report";
}

sub compare_report {
    $report_file = $_[0];
    if(-e "report/${case_type}_${report_file}") {
        system("cd report; mars_env_loader mars_test -compare report_$name_org.log ${case_type}_${report_file} >& /dev/null"); 
	if($report_file eq "bit_best_ku3.log") {
            system("cd report; cp compare_result.log $history_dir/bit_compare_best_ku3.log"); 
	}
	if($report_file eq "bit_best_bd_sda.log") {
            system("cd report; cp compare_result.log $history_dir/bit_compare_best_baidu.log"); 
	}
        system("cd report; mv compare_result.log compare-$name_org-${case_type}_${report_file}"); 
    } else {
#	printf "WARNING : No $report_file in ${case_dir}/test_cases_${case_type}\n";
    }
}

$string = "";

$config_file = $xml_file;
$config_list = XML::Simple->new(ForceArray => 1);
$config_all = $config_list->XMLin($config_file);
my @config_set = @{$config_all->{"config"}};
foreach my $config_one (@config_set) {
    $case_type =  $config_one->{"case_type"}[0];
    $name_org = $config_one->{"name"}[0];
    if($config_one->{"flow"}[0] eq "hls") {
        system "cd run_$name_org/run; perl $report_dir/report_hls.pl>../../report/report_$name_org.log";
    } else {
        system "cd run_$name_org/run; perl $report_dir/report_bit.pl>../../report/report_$name_org.log";
        $report_info = `cat report/report_$name_org.log`;
        $string .= $report_info;
    }

# compare with old reports
#    compare_report("hls_sim_yesterday.log");
#    compare_report("hls_sim_release.log");
#    compare_report("hls_sim_best.log");
#    compare_report("bit_yesterday_ku3.log");
#    compare_report("bit_release_ku3.log");
#    compare_report("bit_best_ku3.log");
#    compare_report("bit_yesterday_bd_sda.log");
#    compare_report("bit_release_bd_sda.log");
#    compare_report("bit_best_bd_sda.log");
#    compare_report("bit_yesterday_bd_apo.log");
#    compare_report("bit_release_bd_apo.log");
#    compare_report("bit_best_bd_apo.log");
}
printf $string;
$finial_report_name = "report/backend.log";
open(my $new_report, '>', $finial_report_name);
printf $new_report $string;
close $new_report;
