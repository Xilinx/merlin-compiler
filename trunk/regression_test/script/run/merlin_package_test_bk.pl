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
$MARS_TEST_HOME= $ENV{'MARS_TEST_HOME'};
$compared_dir = "$MARS_TEST_HOME_GIT/history/package_test/to_be_cmpared";
$last_rele_frontend  = "$compared_dir/last_release_frontend.log";
$last_rele_backend   = "$compared_dir/last_release_backend.log";
$last_week_frontend  = "$compared_dir/last_week_frontend.log";
$last_week_backend   = "$compared_dir/last_week_backend.log";
$last_yest_frontend  = "$compared_dir/last_yest_frontend.log";
$last_yest_backend   = "$compared_dir/last_yest_backend.log";

$user = `whoami`;
chomp($user);
printf "user name = $user\n";

$curr_dir = `pwd`;
chomp($curr_dir);
$on_board_flag = 0;
$report_flag = 0;
$rerun_flag = 0;
$force = 0;
$simple = 0;
$simple_and_board = 0;
$pack_only = 0;
$pack_and_front = 0;
$pack_and_all = 0;
$checkin = 0;
$exec = 0;
$full_flag = 0;
for ($i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] eq "-help") {
        printf "-force : delete original directory and create a new one\n";
        printf "-report : get report\n";
        printf "-front : do frontend test with svn version, not package\n";
        printf "-front_and_back : do frontend test and backend test with svn version, not package\n";
        printf "-pack_only : generate release package only\n";
        printf "-pack_and_front : generate release package and do frontend test on package\n";
        printf "-pack_and_all : generate release package and do all test on package\n";
        printf "-checkin : run checkin test\n";
        printf "-track: keep track on release version\n";
        printf "-exec: execute host on board\n";
        exit;
    }
    if($ARGV[$i] eq "-force") {
        $force = 1;
    }
    if($ARGV[$i] eq "-report") {
        $report_flag = 1;
    }
    if($ARGV[$i] eq "-local_rerun") {
        $rerun_flag = 1;
    }
    if($ARGV[$i] eq "-front") {
        $simple = 1;
        $on_board_flag = 0;
    }
    if($ARGV[$i] eq "-front_and_back") {
        $simple_and_board = 1;
        $on_board_flag = 1;
    }
    if($ARGV[$i] eq "-pack_only") {
        $pack_only = 1;
        $on_board_flag = 0;
    }
    if($ARGV[$i] eq "-pack_and_front") {
        $pack_and_front = 1;
        $on_board_flag = 0;
    }
    if($ARGV[$i] eq "-pack_and_all") {
        $pack_and_all = 1;
    }
    if($ARGV[$i] eq "checkin") {
        $simple = 1;
        #$on_board_flag= 1;
        $checkin = 1;
    }
    if($ARGV[$i] eq "-track") {
        $track = 1;
    }
    if($ARGV[$i] eq "-exec") {
        $exec = 1;
    }
    if($ARGV[$i] eq "-full") {
        $full_flag = 1;
    }
}

$time_update = `date`;
printf "$time_update\n";
if($time_update =~ /(\S*)\s*(\S*)\s*(\S*)\s*(\S*)\s*(\S*)\s*(\S*)/) {
    $month = $2;
    $month_day = $3;
    $year = $6;
}
$time_curr = "${year}${month}${month_day}";

if($checkin eq 1) {
    $work_dir = "merlin_checkin";
} else {
    $work_dir = "package_test";
}
$package_dir = "Merlin_Compiler";
$report_summary_frontend = "$work_dir/report/report_summary_frontend.log";
system "rm -rf $report_summary_frontend";
$string_frontend = "";
$report_summary_backend = "$work_dir/report/report_summary_backend.log";
system "rm -rf $report_summary_backend";
$string_backend = "";
$compare_yest_frontend = "$work_dir/report/compare_yest_frontend.log";
$compare_week_frontend = "$work_dir/report/compare_week_frontend.log";
$compare_rele_frontend = "$work_dir/report/compare_rele_frontend.log";
$compare_best_backend  = "$work_dir/report/compare_best_backend.log";
$compare_week_backend  = "$work_dir/report/compare_week_backend.log";
$compare_rele_backend  = "$work_dir/report/compare_rele_backend.log";
system "rm -rf $work_dir/report/compare_yest_frontend.log";
system "rm -rf $work_dir/report/compare_week_frontend.log";
system "rm -rf $work_dir/report/compare_rele_frontend.log";
system "rm -rf $work_dir/report/compare_best_backend.log";
system "rm -rf $work_dir/report/compare_week_backend.log";
system "rm -rf $work_dir/report/compare_rele_backend.log";
$file_csh     = "/curr/$user/fcs_setting64.csh";
$file_sh      = "/curr/$user/fcs_setting64.sh";

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
#sub function for report
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

$rerun_time_out = 1200;
sub check_failed_to_rerun {
    $dir = $_[0];
    $ls = `ls $dir`;
    @ls_set = split("\n", $ls);
    foreach $one_dir (@ls_set) {
        if($one_dir =~ /run_no_bit_test_(.*)/) {
            $tar = $1;
            $info = `cat $dir/report/report_${tar}.log`;
            @info_set = split("\n", $info);
            foreach $one_line (@info_set) {
                if($one_line =~ /\|(\S+)\s*\|.*\|.*\|.*\|.*\|.*\|.*\|\s*(\S+)\|/) {
                    if($2 eq "Fail") {
                        printf "Rerunning : $dir/run_no_bit_test_${tar}/run/work/no_bit_test_${tar}-$1...\n";
	                    run_time_out($rerun_time_out, "cd $dir/run_no_bit_test_${tar}/run/work/no_bit_test_${tar}-$1; merlin_flow run test -no_bit_test -s");
                    }
                }
            }
        }
    }
}

sub check_report_unit {
    $file = $_[0];
    my $directory = "";
    if($file =~  /(.*)\.log/) {
        $directory = $1;
    }
    if($file =~  /.*\/(.*)\.log/) {
        $name = $1;
    }
    $string_frontend .= "\nReporting $name:\n";
    if(-e $file) {
        $error_count = 0;
        $file_info = `cat $file`;
        @file_lines = split(/\n/,$file_info);
        foreach $one_line (@file_lines) {
            if($one_line =~ /\|(.*)\|\s*(.*)\|/){
                if($2 eq "Pass") {
                } else {
                    if($1 =~ /DESIGN/g) {
                    } elsif($2 =~ /CORRECTNESS/g) {
                    } else {
                        $string_frontend .= "$1 $2\n";
                        $error_count = $error_count + 1;
                    }
                }
            }
        }
        if($error_count == 0) {
            $string_frontend .= "$name passed\n";
        }
    } else {
        $string_frontend .= "$name not exist\n";
    }
}
sub check_report_frontend {
    $file = $_[0];
    $name1 = str_shift_to_right("case_name",15);
    $name2 = str_shift_to_right("result",10);
    $name3 = str_shift_to_right("yesterday",15);
    $name4 = str_shift_to_right("week",15);
    $name5 = str_shift_to_right("release",15);
    $string_frontend .= "\n${name1}${name2}${name3}${name4}${name5}\n";
    system "mars_test -compare -frontend $last_yest_frontend $file $compare_yest_frontend > .log";
    system "mars_test -compare -frontend $last_week_frontend $file $compare_week_frontend > .log";
    system "mars_test -compare -frontend $last_rele_frontend $file $compare_rele_frontend > .log";
    my $name, $case_name, $cycles, $correctness;
    my %case_report;
#    $file_info = `cat $file`;
    $file_info = `cat $compare_yest_frontend`;
    @file_lines = split(/\n/,$file_info);
    foreach $one_line (@file_lines) {
        if($one_line =~ /Frontend report : (.*)/) {
            $name = $1;
        } else {
            if($one_line =~ /\|(.*)\|.*\|.*\|.*\|.*\|.*\|(.*)\|\s*(.*)\|/){
                $case_name = $1;
                $cycles = $2;
                $correctness = $3;
                $case_name =~ s/ //g;
                $cycles =~ s/ //g;
                $cycles =~ s/%//g;
                if ($correctness =~ /(.*)\/(.*)$/) {
                    $correctness = $2;
                }
                if($correctness =~ /Correctness/) {
                } else {
                    if($name =~ /cov_a/) {
                    } else {
                        if($correctness =~ /Pass/) {
                            $case_report{"${name}_${case_name}"} = "$case_name $correctness $cycles";
                            $one_report = $case_report{"${name}_${case_name}"};
                        }
                    }
                }
            }
        }
    }
    $file_info = `cat $compare_week_frontend`;
    @file_lines = split(/\n/,$file_info);
    foreach $one_line (@file_lines) {
        if($one_line =~ /Frontend report : (.*)/) {
            $name = $1;
        } else {
            if($one_line =~ /\|(.*)\|.*\|.*\|.*\|.*\|.*\|(.*)\|(.*)\|/){
                $case_name = $1;
                $cycles = $2;
                $correctness = $3;
                $case_name =~ s/ //g;
                $cycles =~ s/ //g;
                $cycles =~ s/%//g;
                if ($correctness =~ /(.*)\/(.*)$/) {
                    $correctness = $2;
                }
                if($correctness =~ /Correctness/) {
                } else {
                    if($name =~ /cov_a/) {
                    } else {
                        if($correctness =~ /Pass/) {
                            $case_report{"${name}_${case_name}"} .= " $cycles";
                            $one_report = $case_report{"${name}_${case_name}"};
                        }
                    }
                }
            }
        }
    }
    $file_info = `cat $compare_rele_frontend`;
    @file_lines = split(/\n/,$file_info);
    foreach $one_line (@file_lines) {
        if($one_line =~ /Frontend report : (.*)/) {
            $name = $1;
            $string_frontend .= "\nReporting $name:\n";
        } else {
            if($one_line =~ /\|(.*)\|.*\|.*\|.*\|.*\|.*\|(.*)\|(.*)\|/){
                $case_name = $1;
                $cycles = $2;
                $correctness = $3;
                $case_name =~ s/ //g;
                $cycles =~ s/ //g;
                $cycles =~ s/%//g;
                if ($correctness =~ /(.*)\/(.*)$/) {
                    $correctness = $2;
                    $correctness_first = $1;
                }
                if($correctness =~ /Correctness/) {
                } else {
                    if($name =~ /cov_a/) {
                        if($correctness =~ /Pass/) {
                        } else {
                           $case_name   = str_shift_to_right($case_name, 20);
                           $correctness = str_shift_to_right($correctness, 5);
                           $string_frontend .= "$case_name $correctness\n";
                        }
                    } else {
                        #if($correctness_first !~ /NON/) {
                            if($correctness =~ /Pass/) {
                                $case_report{"${name}_${case_name}"} .= " $cycles";
                                $one_report = $case_report{"${name}_${case_name}"};
                                if($one_report =~ /(.*) (.*) (.*) (.*) (.*)/) {
                                    $case_name_report = $1;
                                    $case_name_pass = $2;
                                    $yest_data = $3;
                                    $week_data = $4;
                                    $rele_data = $5;
                                    my $flag = 1;
                                    if($yest_data =~ /\+(\d+)/) { if($1 > 10) { $flag = 0; } }
                                    if($week_data =~ /\+(\d+)/) { if($1 > 10) { $flag = 0; } }
                                    if($rele_data =~ /\+(\d+)/) { if($1 > 10) { $flag = 0; } }
                                    if($yest_data =~ /\-(\d+)/) { if($1 > 10) { $flag = 0; } }
                                    if($week_data =~ /\-(\d+)/) { if($1 > 10) { $flag = 0; } }
                                    if($rele_data =~ /\-(\d+)/) { if($1 > 10) { $flag = 0; } }
                                    if($yest_data =~ /NG/) { $flag = 0; }
                                    if($week_data =~ /NG/) { $flag = 0; }
                                    if($rele_data =~ /NG/) { $flag = 0; }
                                    if($yest_data =~ /e\+(\d+)/) { $flag = 1; }
                                    if($week_data =~ /e\+(\d+)/) { $flag = 1; }
                                    if($rele_data =~ /e\+(\d+)/) { $flag = 1; }
                                    $case_name_report = str_shift_to_right($case_name_report, 20);
                                    $case_name_pass   = str_shift_to_right($case_name_pass, 5);
                                    $yest_data        = str_shift_to_right($yest_data, 15);
                                    $week_data        = str_shift_to_right($week_data, 15);
                                    $rele_data        = str_shift_to_right($rele_data, 15);
                                    if($flag eq 0) {
                                        $string_frontend .= "$case_name_report $case_name_pass $yest_data $week_data $rele_data\n";
                                    }
                                }
                            } elsif($correctness =~ /NON/) {
                            } else {
                                $case_name   = str_shift_to_right($case_name, 20);
                                $correctness = str_shift_to_right($correctness, 5);
                                $string_frontend .= "$case_name $correctness\n";
                            }
                        }
                    #}
                }
            }
        }
    }
}
sub check_report_backend {
    $file = $_[0];
    $name1 = str_shift_to_right("case_name",15);
    $name2 = str_shift_to_right("result",10);
    $name3 = str_shift_to_right("best",15);
    $name4 = str_shift_to_right("week",15);
    $name5 = str_shift_to_right("release",15);
    $string_backend .= "\n${name1}${name2}${name4}${name5}\n";
    system "mars_test -compare -backend $last_week_backend $file $compare_week_backend > .log";
    system "mars_test -compare -backend $last_rele_backend $file $compare_rele_backend > .log";
    my $name, $case_name, $time_fpga, $correctness;
    my %case_report;
    $file_info = `cat $compare_week_backend`;
    @file_lines = split(/\n/,$file_info);
    foreach $one_line (@file_lines) {
        if($one_line =~ /Backend report : (.*)/) {
            $name = $1;
        } else {
            if($one_line =~ /\|(.*)\|.*\|.*\|(.*)\|(.*)\|.*\|.*\|.*\|.*\|.*\|.*\|.*\|.*\|.*\|/){
                $case_name = $1;
                $correctness = $2;
                $time_fpga = $3;
                $case_name =~ s/ //g;
                $time_fpga =~ s/ //g;
                $time_fpga =~ s/%//g;
                if ($correctness =~ /(.*)\/(.*)$/) {
                    $correctness = $2;
                }
                if($correctness =~ /FPGA/) {
                } else {
                    if($name =~ /cov_a/) {
                    } else {
                        if($correctness =~ /P/) {
                            $case_report{"${name}_${case_name}"} = "$case_name Pass $time_fpga";
                            $one_report = $case_report{"${name}_${case_name}"};
                        }
                    }
                }
            }
        }
    }
    $file_info = `cat $compare_rele_backend`;
    @file_lines = split(/\n/,$file_info);
    foreach $one_line (@file_lines) {
        if($one_line =~ /Backend report : (.*)/) {
            $name = $1;
            $string_backend .= "\nReporting $name:\n";
        } else {
            if($one_line =~ /\|(.*)\|.*\|.*\|(.*)\|(.*)\|.*\|.*\|.*\|.*\|.*\|.*\|.*\|.*\|.*\|/){
                $case_name = $1;
                $correctness = $2;
                $time_fpga = $3;
                $case_name =~ s/ //g;
                $time_fpga =~ s/ //g;
                $time_fpga =~ s/%//g;
                if ($correctness =~ /(.*)\/(.*)$/) {
                    $correctness = $2;
                    $correctness_first = $1;
                }
                if($correctness =~ /FPGA/) {
                } else {
                    if($name =~ /cov_a/) {
                        if($correctness =~ /P/) {
                        } else {
                           $case_name   = str_shift_to_right($case_name, 20);
                           $correctness = str_shift_to_right($correctness, 5);
                           $string_backend .= "$case_name $correctness\n";
                        }
                    } else {
                        if($correctness =~ /P/) {
                            $case_report{"${name}_${case_name}"} .= " $time_fpga";
                            $one_report = $case_report{"${name}_${case_name}"};
                            if($one_report =~ /(.*) (.*) (.*) (.*)/) {
                                $case_name_report = $1;
                                $case_name_pass = $2;
                                $week_data = $3;
                                $rele_data = $4;
                                #$best_data = $5;
                                my $flag = 1;
                                if($week_data =~ /\+(\d+)/) { if($1 > 10) { $flag = 0; } }
                                if($rele_data =~ /\+(\d+)/) { if($1 > 10) { $flag = 0; } }
                                #if($best_data =~ /\-(\d+)/) { if($1 > 10) { $flag = 0; } }
                                if($week_data =~ /\-(\d+)/) { if($1 > 10) { $flag = 0; } }
                                if($rele_data =~ /\-(\d+)/) { if($1 > 10) { $flag = 0; } }
                                #if($best_data =~ /NG/) { $flag = 0; }
                                if($week_data =~ /NG/) { $flag = 0; }
                                if($rele_data =~ /NG/) { $flag = 0; }
                                $case_name_report = str_shift_to_right($case_name_report, 20);
                                $case_name_pass   = str_shift_to_right($case_name_pass, 5);
                                $week_data        = str_shift_to_right($week_data, 15);
                                $rele_data        = str_shift_to_right($rele_data, 15);
                                #$best_data        = str_shift_to_right($best_data, 15);
                                if($flag eq 0) {
                                    $string_backend .= "$case_name_report $case_name_pass $best_data $week_data $rele_data\n";
                                }
                            }
                        } else {
                            $case_name   = str_shift_to_right($case_name, 20);
                            $correctness = str_shift_to_right($correctness, 5);
                            $string_backend .= "$case_name $correctness\n";
                        }
                    }
                }
            }
        }
    }
}
#execute
if($report_flag == 0 and $rerun_flag == 0) {
    if(-e $work_dir) {
        if($force == 1) {
            system "rm -rf $work_dir";
        } else {
            printf "\nERROR : $work_dir already exist\n";
            exit;
        }
    }
    system "mkdir $work_dir";
    system "mkdir $work_dir/report";
    system "rm -rf $report_summary; touch $report_summary";
    if($simple == 0 && $simple_and_board == 0) {
        # pack a package
        system "cp $MERLIN_COMPILER_HOME/make_dist.pl $work_dir";
        system "cd $work_dir; perl make_dist.pl 1.1.2";
        # unzip package
        $ls_info = `ls $work_dir`;
        @ls_info_set = split(/\n/, $ls_info);
        foreach $one_file (@ls_info_set) {
            if($one_file =~ /Merlin_Compiler/) {
                system "cd $work_dir; tar zcvf ${one_file}_${time_curr}.tar.gz $one_file";
            }
        }
        # copy package to release directory
        if($user =~ "mars" and $track eq 1) {
            $release_dir  = "/curr/software/Merlin_Compiler/head";
            if(-e $release_dir) {
            } else {
                system "mkdir $release_dir";
            }
            if(-e "$release_dir/${package_dir}_$time_curr") {
                system "rm -rf $release_dir/${package_dir}_$time_curr";
            }
            system "cp -r $work_dir/$package_dir $release_dir/${package_dir}_$time_curr";
            if(-e "$release_dir/${package_dir}_daily_latest") {
                system "rm -rf $release_dir/${package_dir}_daily_latest";
            }
            system "cd $release_dir; ln -fs ${package_dir}_$time_curr $release_dir/${package_dir}_daily_latest";
            $MLH = $ENV{"MERLIN_COMPILER_HOME"};
            system "cd $release_dir/${package_dir}_$time_curr; source $MLH/set_env/env.csh 2> /dev/null; perl install_env.pl all ";
            system "cd $release_dir; cp ${package_dir}_$time_curr/settings64.csh setting64_latest.csh";
            system "cd $release_dir; cp ${package_dir}_$time_curr/settings64.sh  setting64_latest.sh";
            system "ssh us-1.falcon-computing.com 'mkdir -p $release_dir'";
            system "scp -r $release_dir/${package_dir}_$time_curr us-1.falcon-computing.com:$release_dir";
            system "scp -r $release_dir/setting64_latest.csh      us-1.falcon-computing.com:$release_dir";
            system "scp -r $release_dir/setting64_latest.sh       us-1.falcon-computing.com:$release_dir";
            system "ssh us-1.falcon-computing.com 'rm -rf $release_dir/${package_dir}_daily_latest'";
            system "ssh us-1.falcon-computing.com 'cd $release_dir; ln -s ${package_dir}_$time_curr $release_dir/${package_dir}_daily_latest'";
            system "ssh us-2.falcon-computing.com 'mkdir -p $release_dir'";
            system "scp -r $release_dir/${package_dir}_$time_curr us-2.falcon-computing.com:$release_dir";
            system "scp -r $release_dir/setting64_latest.csh      us-2.falcon-computing.com:$release_dir";
            system "scp -r $release_dir/setting64_latest.sh       us-2.falcon-computing.com:$release_dir";
            system "ssh us-2.falcon-computing.com 'rm -rf $release_dir/${package_dir}_daily_latest'";
            system "ssh us-2.falcon-computing.com 'cd $release_dir; ln -s ${package_dir}_$time_curr $release_dir/${package_dir}_daily_latest'";
        }
        if($pack_only == 1) {
            exit;
        }

        #set environment for condor test
        system "cd $work_dir/$package_dir; source $MERLIN_COMPILER_HOME/set_env/env.sh; perl install_env.pl all";
        system "cd $work_dir/$package_dir; cp -r  $MARS_TEST_HOME_GIT/pkg_special_test/directive_mars_basic* mars-gen/default";
        $env_dir = "${curr_dir}\/${work_dir}\/${package_dir}";
        $env_dir =~ s/\//\\\//g;
        system "perl -pi -e 's/setenv\\s*MERLIN_COMPILER_HOME.*/setenv MERLIN_COMPILER_HOME $env_dir/g' $file_csh";
        system "perl -pi -e 's/setenv\\s*MERLIN_COMPILER_HOME.*/setenv MERLIN_COMPILER_HOME $env_dir/g' $file_csh";
        system "perl -pi -e 's/export\\s*MERLIN_COMPILER_HOME\s*=.*/export MERLIN_COMPILER_HOME=$env_dir/g' $file_sh";
        system "perl -pi -e 's/export\\s*MERLIN_COMPILER_HOME\s*=.*/export MERLIN_COMPILER_HOME=$env_dir/g' $file_sh";
    }

    #if($checkin eq 0){
    #message test
    system "mkdir $work_dir/message;";
    system "cd $work_dir/message;";
    system "cd $work_dir/message; mars_test message";
    system "cd $work_dir/message; mars_test message message.list -cloud -force -nosleep";
    #}


    #cmd_test
    system "mkdir $work_dir/cmd_test;";
    system "cd $work_dir/cmd_test;";
    system "cd $work_dir/cmd_test; mars_test cmd_test";
    system "cd $work_dir/cmd_test; mars_test cmd_test cmd_test.list -cloud -force -nosleep";

 if ($checkin ne 1){
   #sda_suite
   #    # system "cp -r $MERLIN_COMPILER_HOME/regression_test/test_case/common $work_dir";
    system "mkdir $work_dir/sda;";
    system "cd $work_dir/sda;";
    system "cd $work_dir/sda; mars_test sda";
    system "cd $work_dir/sda; mars_test sda sda.list -cloud -force -nosleep";

   #qor_suite
   #    # system "cp -r $MERLIN_COMPILER_HOME/regression_test/test_case/common $work_dir";
    system "mkdir $work_dir/qor_suite;";
    system "cd $work_dir/qor_suite;";
    system "cd $work_dir/qor_suite; mars_test qor_suite";
    system "cd $work_dir/qor_suite; mars_test qor_suite qor_suite.list -cloud -force -nosleep";
   
    #mach_set2
    system "mkdir $work_dir/machsuite_set2;";
    system "cd $work_dir/machsuite_set2;";
    system "cd $work_dir/machsuite_set2; mars_test machsuite_set2";
    system "cd $work_dir/machsuite_set2; mars_test machsuite_set2 machsuite_set2.list -cloud -force -nosleep";
   
   }

    #encrypt_test
    system "mkdir $work_dir/encrypt_test;";
    system "cd $work_dir/encrypt_test;";
    system "cd $work_dir/encrypt_test; mars_test encrypt_test";
    system "cd $work_dir/encrypt_test; mars_test encrypt_test encrypt_test.list -cloud -force -nosleep";
        


    #rodinia
    system "mkdir $work_dir/rodinia;";
    system "cd $work_dir/rodinia;";
    if($checkin eq 1) {
      system "cd $work_dir/rodinia; cp -r $MARS_TEST_HOME_GIT/configuration/rodinia_fast.list .";
      system "cd $work_dir/rodinia; mars_test rodinia rodinia_fast.list -cloud -force -nosleep";
    }
    else {
      system "cd $work_dir/rodinia; mars_test rodinia";
      system "cd $work_dir/rodinia; mars_test rodinia rodinia.list -cloud -force -nosleep";
    }
    #unit test
    system "mkdir $work_dir/unit_test;";
    system "cd $work_dir/unit_test;";
    system "cd $work_dir/unit_test; mars_test unit_test";
    system "cd $work_dir/unit_test; mars_test unit_test unit_test.list -cloud -force -nosleep";
    system "cd $work_dir/unit_test; cp $MERLIN_COMPILER_HOME/regression_test/msg_report/Makeupdateref Makefile";
    #msg_report another test
    
    #if($checkin eq 0){
    #
    #}
    #commandline test
    system "mkdir $work_dir/commandline;";
    system "cd $work_dir/commandline;";
    system "cd $work_dir/commandline; mars_test commandline";
    system "cd $work_dir/commandline; mars_test commandline commandline.list -cloud -force -nosleep";
    #merlin_flow
    system "mkdir $work_dir/merlin_flow;";
    system "cd $work_dir/merlin_flow;";
    system "cd $work_dir/merlin_flow; mars_test merlin_flow";
    system "cd $work_dir/merlin_flow; mars_test merlin_flow merlin_flow.list -cloud -force -nosleep";
    #bugizlla
    system "mkdir $work_dir/bugzilla;";
    system "cd $work_dir/bugzilla;";
    if($checkin eq 1 and $full_flag eq 0) {
        system "cd $work_dir/bugzilla; cp -r $MARS_TEST_HOME_GIT/configuration/bugzilla_fast.list .";
        system "cd $work_dir/bugzilla; mars_test bugzilla bugzilla_fast.list -cloud -force -nosleep";
    } else {
        system "cd $work_dir/bugzilla; mars_test bugzilla";
        system "cd $work_dir/bugzilla; mars_test bugzilla bugzilla.list -cloud -force -nosleep";
    }

#one hour  after convert
    system "mkdir $work_dir/onehour_a;";
    system "cd $work_dir/onehour_a;";
    if($checkin eq 1 ) {
        system "cd $work_dir/onehour_a; cp -r $MARS_TEST_HOME_GIT/configuration/onehour_a_fast.list .";
        system "cd $work_dir/onehour_a; mars_test onehour_a onehour_a_fast.list -cloud -force -nosleep";
    } else {
        system "cd $work_dir/onehour_a; mars_test onehour_a";
        system "cd $work_dir/onehour_a; mars_test onehour_a  onehour_a.list -cloud -force -nosleep";
    }

    system "mkdir $work_dir/onehour_x;";
    system "cd $work_dir/onehour_x;";
    if($checkin eq 1 ) {
        system "cd $work_dir/onehour_x; cp -r $MARS_TEST_HOME_GIT/configuration/onehour_x_fast.list .";
        system "cd $work_dir/onehour_x; mars_test onehour_x onehour_x_fast.list -cloud -force -nosleep";
    } else {
        system "cd $work_dir/onehour_x; mars_test onehour_x";
        system "cd $work_dir/onehour_x; mars_test onehour_x  onehour_x.list -cloud -force -nosleep";
    }

    

    
    #one hour test
#    system "mkdir $work_dir/one_hour;";
#    system "cd $work_dir/one_hour;";
#    if($checkin eq 1) {
#        system "cd $work_dir/one_hour; cp -r $MARS_TEST_HOME_GIT/configuration/one_hour_test_fast.xml .";
#        if ($on_board_flag == 1) {
#            system "cd $work_dir/one_hour; mars_test -h one_hour_test_fast.xml -test_after_bit";
#        } else {
#            system "cd $work_dir/one_hour; mars_test -h one_hour_test_fast.xml -cloud -force";
#        }
#    } else {
#	##### choose platform ###
#	my $xtext = `cat  ~/fcs_setting64.sh`;
#	if ($xtext =~ /2017.4/){
#	printf "xocc  2017.4\n";
#        system "cd $work_dir/one_hour; cp -r $MARS_TEST_HOME_GIT/configuration/one_hour_test_pkg_174.xml  one_hour_test_pkg.xml ";
#	} else {
#	printf "xocc  default 2017.1\n";
#        system "cd $work_dir/one_hour; cp -r $MARS_TEST_HOME_GIT/configuration/one_hour_test_pkg.xml .";
#	}
#
#        if ($on_board_flag == 1) {
#            system "cd $work_dir/one_hour; mars_test -h one_hour_test_pkg.xml -test_after_bit";
#        } else {
#            system "cd $work_dir/one_hour; mars_test -h one_hour_test_pkg.xml -cloud -force";
#        }
#    }




    if($pack_and_front == 1) {
        exit;
    }

    if($simple == 0) {
        #on board test
        system "mkdir $work_dir/on_board;";
        system "cd $work_dir/on_board;";
        system "cd $work_dir/on_board; cp -r $MARS_TEST_HOME_GIT/configuration/configuration_pkg.xml .";
        if ($on_board_flag == 1) {
            system "cd $work_dir/on_board; mars_test -x configuration_pkg.xml -test_after_bit";
        } else {
            system "cd $work_dir/on_board; mars_test -x configuration_pkg.xml -cloud -force";
        }
    }

    if($pack_and_all == 1) {
        #special test
        system "rm -rf $work_dir/special";
        system "mkdir $work_dir/special";
        system "cd $work_dir/special; perl $MARS_TEST_HOME_GIT/script/run/merlin_special_test.pl ${curr_dir}\/${work_dir}\/${package_dir}";
    }
} 

if($rerun_flag == 1) {
     system "cd $work_dir/; perl $MARS_TEST_HOME_GIT/script/run/local_rerun.pl $MARS_TEST_HOME_GIT/configuration/checkin_rerun.list ";
}

if($report_flag == 1) {
    $work_dir = "${curr_dir}/${work_dir}";
    #get report
    system "cd $work_dir/unit_test; mars_test unit_test unit_test.list -report > .aaa.log";
#    system "cd $work_dir/msgrpt_sdx2; mars_test msgrpt_sdx2 msg_report_test.list -report > .aaa.log";

    #if($checkin eq 0){
        system "cd $work_dir/message; mars_test message message.list -report > .aaa.log";
    #}
    system "cd $work_dir/commandline; mars_test commandline commandline.list -report > .aaa.log";
       #add rodinia
    if($checkin eq 1) {
        system "cd $work_dir/rodinia; mars_test rodinia rodinia_fast.list -report > .aaa.log";
    } else {
        system "cd $work_dir/rodinia; mars_test rodinia rodinia.list -report > .aaa.log";
    }
    system "cd $work_dir/cmd_test; mars_test cmd_test cmd_test.list -report > .aaa.log";
    system "cd $work_dir/encrypt_test; mars_test encrypt_test encrypt_test.list -report > .aaa.log";
    system "cd $work_dir/machsuite_set2; mars_test machsuite_set2 machsuite_set2.list -report > .aaa.log";
    system "cd $work_dir/sda; mars_test sda sda.list -report > .aaa.log";
    system "cd $work_dir/qor_suite; mars_test qor_suite qor_suite.list -report > .aaa.log";
    system "cd $work_dir/merlin_flow; mars_test merlin_flow merlin_flow.list -report > .aaa.log";
    if($checkin eq 1 and $full_flag eq 0) {
        system "cd $work_dir/bugzilla; mars_test bugzilla bugzilla_fast.list -report >> .aaa.log";
    } else {
        system "cd $work_dir/bugzilla; mars_test bugzilla bugzilla.list -report >> .aaa.log";
    }
    if($checkin eq 1) {
#system "cd $work_dir/one_hour; mars_test -h one_hour_test_fast.xml -report >> .aaa.log";
         system "cd $work_dir/onehour_a; mars_test onehour_a onehour_a_fast.list -report >> .aaa.log";
         system "cd $work_dir/onehour_x; mars_test onehour_x onehour_x_fast.list -report >> .aaa.log";
    } else {
#system "cd $work_dir/one_hour; mars_test -h one_hour_test_pkg.xml -report >> .aaa.log";
         system "cd $work_dir/onehour_a; mars_test onehour_a onehour_a.list -report >> .aaa.log";
         system "cd $work_dir/onehour_x; mars_test onehour_x onehour_x.list -report >> .aaa.log";
        #check_failed_to_rerun("${work_dir}/one_hour");
    }

#######
#LZ 20180411
    system "cd $work_dir/one_hour/report; touch frontend_more.log hls.rpt vendor_msg.rpt;rm frontend_more.log hls.rpt vendor_msg.rpt;";
    system "cd $work_dir/one_hour/report; grep -rh 'HLS rpt not found' *.log >> hls.rpt;grep -rh 'vendor msg found' *.log >> vendor_msg.rpt; cat frontend.log hls.rpt vendor_msg.rpt>>frontend_more.log;";
    system "cd $work_dir/one_hour/report; cp -r frontend.log $work_dir/report; cp -r frontend_more.log $work_dir/report; cp -r hls.rpt $work_dir/report;cp -r vendor_msg.rpt $work_dir/report;";


    #display report
    check_report_unit("${work_dir}/unit_test/unit_test.log");
    #if($checkin eq 0) {
        check_report_unit("${work_dir}/message/message.log");
        #check_report_unit("${work_dir}/msgrpt_sdx1/msgrpt_sdx1.log");
#        check_report_unit("${work_dir}/msgrpt_sdx2/msgrpt_sdx2.log");
    #}
    check_report_unit("${work_dir}/commandline/commandline.log");
  	#add rodinia
    check_report_unit("${work_dir}/rodinia/rodinia.log");
    check_report_unit("${work_dir}/cmd_test/cmd_test.log");
    check_report_unit("${work_dir}/encrypt_test/encrypt_test.log");
    check_report_unit("${work_dir}/machsuite_set2/machsuite_set2.log");
    check_report_unit("${work_dir}/sda/sda.log");
    check_report_unit("${work_dir}/qor_suite/qor_suite.log");
    check_report_unit("${work_dir}/merlin_flow/merlin_flow.log");
    check_report_unit("${work_dir}/bugzilla/bugzilla.log");
    check_report_unit("${work_dir}/onehour_a/onehour_a.log");
    check_report_unit("${work_dir}/onehour_x/onehour_x.log");
#check_report_frontend("$work_dir/one_hour/report/frontend.log");
    printf "$string_frontend\n";
    system "echo '$string_frontend' >> $report_summary_frontend";
    system "cd ${work_dir}/report; cat hls.rpt; ";
    #get on baord report
    if(-e "$work_dir/on_board") {
        # normal on baordi
        if($exec eq 1) {
            $ls = `ls $work_dir/on_board`;
            @ls_set = split("\n", $ls);
            foreach $one_dir (@ls_set) {
                if($one_dir =~ /run_(.*)_a_(.*)/) {
                    system "cd $work_dir/on_board/$one_dir/run; make all_test";
                }
            }
            foreach $one_dir (@ls_set) {
                if($one_dir =~ /run_(.*)_x/) {
                    system "cd $work_dir/on_board/$one_dir/run; make all_test";
                }
            }
        }
        system "cd $work_dir/on_board; mars_test -x configuration_pkg.xml -report >.log";
        system "cd $work_dir/on_board/report; cp -r backend.log $work_dir/report";
        check_report_backend("$work_dir/on_board/report/backend.log");
        printf "$string_backend\n";
        system "echo '$string_backend'  >> $report_summary_backend";
    }
} 

