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

$user = `whoami`;
chomp($user);
printf "user name = $user\n";

$curr_dir = `pwd`;
chomp($curr_dir);
for ($i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] eq "-help") {
        exit;
    }
    if($ARGV[$i] eq "-exec") {
        $exec = 1;
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

$work_dir = "rodinia";

$package_dir = "Merlin_Compiler_2017.2";
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
#execute
if($report_flag == 0) {
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
        system "cd $work_dir; perl make_dist.pl";
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
            $release_dir  = "/home/tools/Merlin_Compiler";
            if(-e $release_dir) {
            } else {
                system "mkdir $release_dir";
            }
            system "cp -r $work_dir/*.tar.gz $release_dir";
            if(-e "$release_dir/${package_dir}_$time_curr") {
                system "rm -rf $release_dir/${package_dir}_$time_curr";
            }
            system "cp -r $work_dir/$package_dir $release_dir/${package_dir}_$time_curr";
            if(-e "$release_dir/${package_dir}_daily_latest") {
                system "rm -rf $release_dir/${package_dir}_daily_latest";
            }
            system "cd $release_dir; ln -fs ${package_dir}_$time_curr $release_dir/${package_dir}_daily_latest";
            system "cd $release_dir/${package_dir}_$time_curr; source $MERLIN_COMPILER_HOME/set_env/env.sh; perl install_env.pl all";
            system "cd $release_dir/${package_dir}_$time_curr; perl -pi -e 's/home\\\/tools/curr\\\/software/g' settings64.csh";
            system "cd $release_dir/${package_dir}_$time_curr; perl -pi -e 's/home\\\/tools/curr\\\/software/g' settings64.sh";
            system "cd $release_dir; cp ${package_dir}_$time_curr/settings64.csh setting64_latest.csh";
            system "cd $release_dir; cp ${package_dir}_$time_curr/settings64.sh  setting64_latest.sh";
            system "scp -r $release_dir/${package_dir}_$time_curr fcs:$release_dir";
            system "scp -r $release_dir/setting64_latest.csh      fcs:$release_dir";
            system "scp -r $release_dir/setting64_latest.sh       fcs:$release_dir";
            system "ssh fcs 'rm -rf $release_dir/${package_dir}_daily_latest'";
            system "ssh fcs 'cd $release_dir; ln -s ${package_dir}_$time_curr $release_dir/${package_dir}_daily_latest'";
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

    #rodinia test
    system "mkdir $work_dir/rodinia;";
    system "cd $work_dir/rodinia;"
    system "make TEST_OPT=2 all";
    system "make TEST_OPT=3 all";
}
