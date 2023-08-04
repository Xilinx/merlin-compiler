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
# this script generate a report for regression test
# This scripts is called at regression_mars directory

# It scan all the generatd cases and extract all their summmary reports
use File::Basename;
use XML::Simple;
use Data::Dumper;

$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$report_dir = "${MARS_TEST_HOME_GIT}/script/report";
#$case_dir = "${MARS_TEST_HOME_GIT}/test_case";
$history_dir = "${MARS_TEST_HOME_GIT}/history";

$is_brief = 0;
for($i=0; $i<@ARGV; $i=$i+1) {
    if($ARGV[$i] eq "-h") {
        $mini_xml_file = $ARGV[$i+1];
    }
    if($ARGV[$i] eq "-brief") {
        $is_brief = 1;
    }
}

sub str_shift_to_right {
    $str_tmp = $_[0];
    $len_tmp = $_[1];
    if (@_ > 2)  {
        $max_len= $_[2];
    }

    my $i;

    $str_len = length($str_tmp);

    if ($str_len < $len_tmp) {
        for ($i = $str_len; $i < $len_tmp; $i+=1) {
            $str_tmp = " ".$str_tmp;
        }
    }
    if (@_ > 2 and $str_len > $len_tmp) {
        $str_tmp = substr($str_tmp, 0, $len_tmp); 
    }
    return $str_tmp;
}

$flow_name = "no_bit_test_hls";
if(-e "report") {
    printf "report directory already exist.\n";
} else {
    system "mkdir report";
}

open(my $report_summary, '>', "report/frontend.log");
$string_summary = "";

$config_file = "$mini_xml_file";
$config_list = XML::Simple->new(ForceArray => 1);
$config_all = $config_list->XMLin($config_file);
my @config_set = @{$config_all->{"config"}};
foreach my $config_one (@config_set) {
    if($config_one->{"implementation_tool"}[0] =~ /aocl/) {
        $impl_tool = "aocl";
    } else {
        $impl_tool = "sdaccel";
    }
    if($config_one->{"flow"}[0] = $flow_name) {
        $case_type =  $config_one->{"case_type"}[0];
        $name_org = $config_one->{"name"}[0];
        $name_sim = "no_bit_test_$name_org";
        $name_hls = "hls_$name_org";
        system "cd run_$name_sim/run; perl report_bit.pl>report.log";
        system "cd run_$name_sim/run; cp report.log ../../report/$name_sim.log";
        system "cd run_$name_hls/run; perl report_hls.pl>report.log";
        system "cd run_$name_hls/run; cp report.log ../../report/$name_hls.log";
    }
    $finial_report_name = "report/report_$name_org.log";
    open(my $new_report, '>', $finial_report_name);
    $report_file = "report/$name_sim.log" ;
    $report_info = `cat $report_file`;

    my @pass_or_fail;
    my @cpu_time;
    my @sim_case_set;
    my %is_pass_record;
    $i = 0;
    if($report_info =~ /\|\s*DESIGN\s*\| C.*\n(.*)\n([\S\s]*)\n\+.*\+\-\-\-\-\+(.*)\n/) {
        @case_set=split(/\n/,$2);
        foreach $one_case (@case_set) {
#ZP: if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|/) {
           if($one_case =~ /\|(.*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|/) {
                $case_name = $1;
#                if($2 eq "Pass" and $3 eq "Pass") {
                if($3 eq "Pass") {
                    $pass_or_fail[$i] = "Pass";
#                } elsif($2 eq "  NG" and $3 eq "  NG") {
                } elsif($3 eq "NG") {
                    $pass_or_fail[$i] = "  NG";
#                } elsif($2 eq " RUN" and $3 eq " RUN") {
                } elsif($3 eq "RUN") {
                    $pass_or_fail[$i] = " RUNNING";
                #} elsif($2 eq "IPGF" and $3 eq " IPGF") {
                } elsif($2 eq "IPGF") {
                    $pass_or_fail[$i] = "IPGF";
                } elsif($2 eq "BTGF") {
                    $pass_or_fail[$i] = "BTGF";
                } else {
                    #$pass_or_fail[$i] = "Fail";
                    $pass_or_fail[$i] = "Fail:$3";
                }
                $pass_or_fail[$i] = str_shift_to_right($pass_or_fail[$i],12,11);
                $cpu_time[$i] = $8;

                $is_pass_record{$case_name} = $pass_or_fail[$i];
                push (@sim_case_set, $case_name);
                $i++;
            }
        }
    }
    $report_file = "report/$name_hls.log" ;
    $report_info = `cat $report_file`;
    $i = 0;
    $count_head_tail = 0;
    $string_report = "";
    $name_hls =~ s/^hls_//;
    $string_report .= "Frontend report : $name_hls\n";
    @case_set=split(/\n/,$report_info);
    foreach $one_case (@case_set) {
        if($one_case =~ /------/ or $one_case =~ /BRAM.*\|.*DSP/) {
            if($count_head_tail < 4) {
                if($count_head_tail eq 1) {
                    $string_report .= "${one_case}Correctness|\n";
                } else {
                    $string_report .= "$one_case-----------|\n";
                }
            }
            $count_head_tail = $count_head_tail + 1;
        } elsif($one_case =~ /\|(.*)\|(.*\|.*\|.*\|.*\|.*\|.*\|.*)/) {
            $case_name = $1;
            $case_value = $2;
            if($case_name =~ /\S+/) {
                $string_report .= "${one_case}$pass_or_fail[$i]|\n";
                $case_record{$i} = 1;
                $i++;
            } else {
                $string_report .="${one_case}$pass_or_fail[$i-1]|\n";
            }
        }
    }
    print $string_report;
    print $new_report $string_report;
    close $new_report;
    $string_summary .= $string_report;

# compare with old reports
#    if(-e "report/${case_type}_hls_sim_yesterday.log") {
#        system("cd report; mars_env_loader mars_test -h -compare report_$name_org.log ${case_type}_hls_sim_yesterday.log >& /dev/null"); 
#        system("cd report; mv compare_result.log compare-$name_org-${case_type}_hls_sim_yesterday.log"); 
#    } else {
#	    if ($is_brief==0) { printf "WARNING : No hls_sim_yesterday.log in ${case_dir}/test_cases_${case_type}\n";}
#    }
#    if(-e "report/${case_type}_hls_sim_release.log") {
#        system("cd report; mars_env_loader mars_test -h -compare report_$name_org.log ${case_type}_hls_sim_release.log >& /dev/null"); 
#        system("cd report; mv compare_result.log compare-$name_org-${case_type}_hls_sim_release.log"); 
#    } else {
#	    if ($is_brief==0) { printf "WARNING : No hls_sim_release.log in ${case_dir}/test_cases_${case_type}\n"; }
#    }
#    if(-e "report/${case_type}_hls_sim_best.log") {
#	system("cd report; mars_env_loader mars_test -h -compare report_$name_org.log ${case_type}_hls_sim_best.log >& /dev/null"); 
#        if($impl_tool eq "aocl") {
#	    system("cd report; cp compare_result.log $history_dir/one_hour_test_compare_best_altera.log"); 
#        } else {
#	    system("cd report; cp compare_result.log $history_dir/one_hour_test_compare_best_xilinx.log"); 
#        }
#        system("cd report; mv compare_result.log compare-$name_org-${case_type}_hls_sim_best.log"); 
#    } else {
#	    if ($is_brief==0) { printf "WARNING : No hls_sim_best.log in ${case_dir}/test_cases_${case_type}\n"; }
#    }
}
print $report_summary $string_summary;
close $report_summary;

