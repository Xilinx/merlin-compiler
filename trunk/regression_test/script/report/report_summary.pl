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
$date = $ARGV[0];

$MARS_BUILD_DIR = $ENV{'MARS_BUILD_DIR'};
$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$history_dir    = "$MARS_TEST_HOME_GIT/history";
#$case_dir       = "$MARS_TEST_HOME_GIT/test_case";
$config_dir     = "$MARS_TEST_HOME_GIT/configuration";
$script_dir     = "$MARS_TEST_HOME_GIT/script";

open(my $report_file, '>', "summary.log");
printf $report_file "\n\nDaily Regression Summary:\n\n";

sub report_simple{
    $report_type = $_[0];
    $report_log = $_[1];
#    $report_log = "$report_type.log";
    $report_info = `cat $report_log`;
    $error_count = 0;
    if($report_info =~ /\|\s*DESIGN.*\n(.*)\n([\S\s]*)\n\+\-.*\n/) {
        @report_line=split(/\n/,$2);
        foreach $one_case (@report_line) {
	    if($report_type eq "one_hour_test") {
                if($one_case =~ /\|(.*)\|.*\|.*\|.*\|.*\|.*\|.*\|\s*(.*)\|/){
        	    if($2 eq "Pass") {
#	                printf "$1 $2\n";
        	    } else {
        	        printf "$1 $2\n";
        	        printf $report_file "$1 $2\n";
        	        $error_count = $error_count + 1;
        	    }
                }
	    } elsif ($report_type eq "on_board_test") {
                if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|.*\|.*\|.*\|.*\|.*\|.*\|.*\|/){
		    if($2 eq "Pass" and $3 eq "Pass" and $4 eq "Pass" ) {
		    #} else {
        	    #    printf "$1 $2-$3-$4\n";
        	    #    printf $report_file "$1 $2-$3-$4\n";
        	    #    $error_count = $error_count + 1;
		    }
		    if($4 eq "Pass" ) {
		    } else {
        	        printf "$1 $4\n";
        	        printf $report_file "$1 $4\n";
        	        $error_count = $error_count + 1;
		    }
		}
	    } else {
                if($one_case =~ /\|(.*)\|\s*(.*)\|/){
        	    if($2 eq "Pass") {
#	                printf "$1 $2\n";
        	    } else {
        	        printf "$1 $2\n";
        	        printf $report_file "$1 $2\n";
        	        $error_count = $error_count + 1;
        	    }
                }
	    }
        }
        if($error_count > 0) {
    	    printf "$report_type $report_log failed!\n";
    	    printf $report_file "$report_type $report_log failed!\n";
        } else {
    	    printf "$report_type $report_log all passed!\n";
    	    printf $report_file "$report_type $report_log all passed!\n";
        }
    	printf "\n";
    	printf $report_file "\n";
    }
}

$unit_test_log = "$history_dir/unit_test/$date.log";
if(-e "$unit_test_log") {
    printf "Unit_test summary\n";
    printf $report_file "Unit_test summary\n";
    report_simple("unit_test","$unit_test_log");
}

$bugzilla_log = "$history_dir/bugzilla/$date.log";
if(-e "$bugzilla_log") {
    printf "Bugzilla summary\n";
    printf $report_file "Bugzilla summary\n";
    report_simple("bugzilla","$history_dir/bugzilla/$date.log");
}

$commandline_log = "$history_dir/commandline/$date.log";
if(-e "$commandline_log") {
    printf "Commandline summary\n";
    printf $report_file "Commandline summary\n";
    report_simple("commandline","$history_dir/commandline/$date.log");
}

$qor_log = "$history_dir/qor/$date.log";
if(-e "$qor_log") {
    printf "QOR summary\n";
    printf $report_file "qor summary\n";
    report_simple("qor","$history_dir/qor/$date.log");
}

$one_hour_test_xml = "$history_dir/one_hour_test/$date/one_hour_test.xml";
if(-e "$one_hour_test_xml") {
    printf "One hour test summary\n";
    printf $report_file "One hour test xilinx summary\n";
    my $config_list = XML::Simple->new(ForceArray => 1);
    my $config_all = $config_list->XMLin($one_hour_test_xml);
    my @config_set = @{$config_all->{"config"}};
    foreach my $config_one (@config_set) {
        $name_one =  $config_one->{"name"}[0];
        $case_type_one =  $config_one->{"case_type"}[0];
        $log_name = "$history_dir/one_hour_test/$date/report_$name_one.log";
        report_simple("one_hour_test",$log_name);
    }
}

$one_hour_test_xml = "$history_dir/one_hour_test_altera/$date/one_hour_test.xml";
if(-e "$one_hour_test_xml") {
    printf $report_file "One hour test altera summary\n";
    my $config_list = XML::Simple->new(ForceArray => 1);
    my $config_all = $config_list->XMLin($one_hour_test_xml);
    my @config_set = @{$config_all->{"config"}};
    foreach my $config_one (@config_set) {
        $name_one =  $config_one->{"name"}[0];
        $case_type_one =  $config_one->{"case_type"}[0];
        $log_name = "$history_dir/one_hour_test_altera/$date/report_$name_one.log";
        report_simple("one_hour_test",$log_name);
    }
}

$on_board_test_xml = "$history_dir/on_board_test/$date/configuration.xml";
if(-e "$history_dir/on_board_test/$date/configuration.xml") {
    printf "On board test summary\n";
    printf $report_file "On board test summary\n";
    my $config_list = XML::Simple->new(ForceArray => 1);
    my $config_all = $config_list->XMLin($on_board_test_xml);
    my @config_set = @{$config_all->{"config"}};
    foreach my $config_one (@config_set) {
        $name_one =  $config_one->{"name"}[0];
        $case_type_one =  $config_one->{"case_type"}[0];
        $log_name = "$history_dir/on_board_test/$date/report_$name_one.log";
	if($name_one =~ /baidu/) {
            report_simple("on_board_test",$log_name);
	}
    }

}
