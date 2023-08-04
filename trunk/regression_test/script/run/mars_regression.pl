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
my @user;

#$user[0] = "hanhu\@falcon-computing.com";
#$user[1] = "junyao\@falcon-computing.com";
#$user[1] = "pengzhang\@falcon-computing.com";
#$user[2] = "yuxinwang\@falcon-computing.com";
#$user[3] = "youxiangchen\@falcon-computing.com";
#$user[4] = "peichen\@falcon-computing.com";
#$user[5] = "jimw\@falcon-computing.com";
#$user[6] = "chaoyang\@falcon-computing.com";

$MARS_BUILD_DIR     = $ENV{'MARS_BUILD_DIR'};
$MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$MARS_TEST_HOME     = $ENV{'MARS_TEST_HOME'};
$history_dir        = "$MARS_TEST_HOME_GIT/history";
#############################################
#TODO: ZP: 20170917: to be replaced back to MARS_TEST_HOME_GIT after the test cases move to github
$case_dir = "${MARS_TEST_HOME}/test_case";
#############################################
$config_dir = "$MARS_TEST_HOME_GIT/configuration";
$script_dir = "$MARS_TEST_HOME_GIT/script";

use File::Basename;
use XML::Simple;
use Data::Dumper;

$debug_mode     = 0;
$time_trigger   = "1230";
$svnupdate_flag = 1;
$report_summary = 1;

$daily_run_time_out = "21600";
$rerun_time_out     = 1800;
if ( $debug_mode == 1 ) {
    $daily_run_time_out = "7200";
}

$run_dir = ".";
if ( !-e $run_dir ) {
    system "mkdir $run_dir";
}
for ( $i = 0 ; $i < 50 ; $i++ ) {
    if ( $ARGV[$i] eq "-regression" ) {
        if ( $ARGV[ $i + 1 ] eq "-d" ) {

            #daily
            $regression_mode = "daily";
        }
        elsif ( $ARGV[ $i + 1 ] eq "-w" ) {

            #weekly
        }
        elsif ( $ARGV[ $i + 1 ] eq "-r" ) {

            #release
        }
        if ( $ARGV[ $i + 2 ] eq "-immediately" ) {
            $run_time = "immediately";
        }
    }
}

$SIG{'INT'} = \&quit_regression_test;

sub quit_regression_test {
    print "killed the mars regression test!\n";
    exit;
}

sub check_failed_to_rerun {
    $dir      = $_[0];
    $tar      = $_[1];
    $info     = `cat $dir/report/report_${tar}.log`;
    @info_set = split( "\n", $info );
    foreach $one_line (@info_set) {
        if ( $one_line =~ /\|(\S+)\s*\|.*\|.*\|.*\|.*\|.*\|.*\|\s*(\S+)\|/ ) {
            if ( $2 eq "Fail" or $2 =~ /NG/ ) {
                printf
"Rerunning : $dir/run_no_bit_test_${tar}/run/work/no_bit_test_${tar}-$1...\n";
                run_time_out( $rerun_time_out,
"cd $dir/run_no_bit_test_${tar}/run/work/no_bit_test_${tar}-$1; rm -rf run/implement; merlin_flow run; merlin_flow run test -no_bit_test"
                );
            }
        }
    }
}

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
        die unless $@ eq "alarm\n";    # propagate unexpected errors
                                       # timed out
        printf
"WARNING : ${time_out_time}S TIME OUT! Exit command \'$time_out_exec\'.\n";
    }
    else {
        # didn't
        # printf "no time out\n"
    }
}

while (1) {
    $time_update = `date`;
    $frontend    = 0;
    $backend     = 0;
    printf "$time_update";
    if ( $time_update =~ /(\S*)\s*(\S*)\s*(\S*)\s*(\S*)\s*(\S*)\s*(\S*)/ ) {
        $weekly_day  = $1;
        $month       = $2;
        $month_day   = $3;
        $time_second = $4;
        $year        = $6;
        if    ( $weekly_day eq "Mon" ) { $frontend = 1; }
        elsif ( $weekly_day eq "Tue" ) { $frontend = 1; }
        elsif ( $weekly_day eq "Wed" ) { $frontend = 1; }
        elsif ( $weekly_day eq "Thu" ) { $frontend = 1; }
        elsif ( $weekly_day eq "Fri" ) { $frontend = 1; }
        elsif ( $weekly_day eq "Sat" ) { $frontend = 1; $backend = 1; }
        elsif ( $weekly_day eq "Sun" ) { }

        if ( $time_second =~ /(.*):(.*):(.*)/ ) { $time_min = "$1$2"; }

        if    ( $month =~ "Jan" ) { $month = "01"; }
        elsif ( $month =~ "Feb" ) { $month = "02"; }
        elsif ( $month =~ "Mar" ) { $month = "03"; }
        elsif ( $month =~ "Apr" ) { $month = "04"; }
        elsif ( $month =~ "May" ) { $month = "05"; }
        elsif ( $month =~ "Jun" ) { $month = "06"; }
        elsif ( $month =~ "Jul" ) { $month = "07"; }
        elsif ( $month =~ "Aug" ) { $month = "08"; }
        elsif ( $month =~ "Sep" ) { $month = "09"; }
        elsif ( $month =~ "Oct" ) { $month = "10"; }
        elsif ( $month =~ "Nov" ) { $month = "11"; }
        elsif ( $month =~ "Dec" ) { $month = "12"; }

    }

    $date = "${year}_${month}_${month_day}";

    if ( $time_min eq $time_trigger ) {
        $time_match = 1;
    }
    else {
        $time_match = 0;
    }
    if ( $debug_mode == 1 ) {
        $time_match     = 1;
        $svnupdate_flag = 1;
    }

    if ( $regression_mode eq "daily" and $time_match == 1 ) {
        if ( $svnupdate_flag == 1 ) {

            #kill running mars task
            system "perl $script_dir/run/mars_kill.pl";
            system "cd $MERLIN_COMPILER_HOME; git pull origin 2017.2";
            system "cd $MERLIN_COMPILER_HOME; make clean";
            system "cd $MERLIN_COMPILER_HOME; make all";
            system "cd $MARS_TEST_HOME_GIT; svn update";
        }
        if ( $backend == 1 ) {
            $daily_run_time_out = "129600";    # 36 hours
            printf
              "cd $run_dir; mars_test package -pack_and_all -force -track\n";
            system "cd $run_dir; mars_test package -pack_and_all -force -track";
        }
        elsif ( $frontend == 1 ) {
            $daily_run_time_out = "21600";     # 36 hours
            printf "cd $run_dir; mars_test package -front -force\n";
            system "cd $run_dir; mars_test package -front -force";
        }
        sleep($daily_run_time_out);

        $package_test = "package_test";
        $compared_dir =
          "$MARS_TEST_HOME_GIT/history/$package_test/to_be_cmpared";
        $last_rele_frontend = "$compared_dir/last_release_frontend.log";
        $last_rele_backend  = "$compared_dir/last_release_backend.log";
        $last_week_frontend = "$compared_dir/last_week_frontend.log";
        $last_week_backend  = "$compared_dir/last_week_backend.log";
        $last_yest_frontend = "$compared_dir/last_yest_frontend.log";
        $last_yest_backend  = "$compared_dir/last_yest_backend.log";

        #commit report to svn
        system "mkdir $history_dir/$package_test/$date";

    #FIXME report comparison, normal compare xxx, on board compare performance
    #compare with yesterday, how and what to compare, only on frontend
    #compare    with last week, how and waht to compare, on frontend and bacnend
        system "cd $run_dir; mars_test package -report";
        if ( $backend == 1 ) {

#            system "cp $package_test/report/report_summary_frontend.log $history_dir/$package_test/$date";
#            system "cp $package_test/report/frontend.log $history_dir/$package_test/$date";
#            system "cp $package_test/report/backend.log $history_dir/$package_test/$date";
#            system "cp $package_test/report/frontend.log $last_yest_frontend ";
#            system "cp $package_test/report/backend.log $last_week_backend";
#            system "cp $package_test/report/frontend.log $last_week_frontend ";
        }
        elsif ( $frontend == 1 ) {
            system
"cd $run_dir; cp $package_test/report/report_summary_frontend.log $history_dir/$package_test/$date";
            system
"cd $run_dir; cp $package_test/report/frontend.log $history_dir/$package_test/$date";
            system
"cd $run_dir; cp $package_test/report/frontend.log $last_yest_frontend ";
        }

        if ( $report_summary == 1 ) {
            printf "Send email\n";

            #get summary report
            $email_title = "daily regression report $date ";
            $email_write =
              `cat $run_dir/$package_test/report/report_summary_frontend.log`;

            $user_number = scalar(@user);
            if ( $debug_mode == 1 ) {
                $user_number = 1;
            }
            $file1 = "$run_dir/$package_test/report/frontend.log";
            for ( $i = 0 ; $i < $user_number ; $i = $i + 1 ) {
                if ( $user[$i] ne "" ) {
                    system
"echo \"$email_write \" | mail -s \"$email_title\" -a $file1 $user[$i]";
                }
            }
        }

        system "cd $run_dir; mv $package_test ${package_test}_$date";

    }
    if ( $debug_mode == 1 ) {
        exit;
    }
    if ( -e "/curr/mars/kill" ) {
        exit;
    }
    sleep(50);
}
exit;
