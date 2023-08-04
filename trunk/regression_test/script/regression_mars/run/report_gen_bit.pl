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
$type = $ARGV[0];
printf $type;

$work_dir = "work";

$dir_list = `ls $work_dir`;

@dir_set = split(/\n/, $dir_list);
$max_length = 1;
foreach $one_case (@dir_set) {
    $name_length = length($one_case);
    if($max_length < $name_length) {
        $max_length = $name_length;
    }
}
if($max_length < 13) {
    $max_length = 13;
}
#print " " x 20;
$head1 = "-" x $max_length;
$head2 = " " x $max_length;
$head3 = " " x ($max_length-13);

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
sub str_shift_to_left {
    $str_tmp = $_[0];
    $len_tmp = $_[1];
    if (@_ > 2)  {
        $max_len= $_[2];
    }

    my $i;

    $str_len = length($str_tmp);

    if ($str_len < $len_tmp) {
        for ($i = $str_len; $i < $len_tmp; $i+=1) {
            $str_tmp .= " ";
        }
    }
    if (@_ > 2 and $str_len > $len_tmp) {
        $str_tmp = substr($str_tmp, 0, $len_tmp); 
    }
    return $str_tmp;
}

# get enviroment
printf "\n";
$count_env = 0;
foreach $one_case (@dir_set) {
    if($count_env == 0) {
        $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt";
        if(-e $sum_file) {
            $count_env++;
            $sum_info = `cat $sum_file 2> /dev/null`;
            if ($sum_info =~ /Mars Compiler - FPGA System Synthesis.*\n.*\n\s*(.*): (.*)\n/) {
                $mars_version = $2;
            }
            if ($sum_info =~ /Execution and Testing.*\n.*\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n/) {
                printf "Test Environment\n";
                printf "Mars Build Rev :$mars_version\n";
                printf "$1\n";
                printf "$2\n";
                printf "$3\n";
                printf "$4\n";
                printf "$5\n";
                printf "$6\n";
                printf "$7\n";
            }
        }
    }
}
printf "\n";
# print Header
$one_line  = "All Part Utilization\n";
print $one_line;
$one_line  = "+$head1+------+------+------+------+------+------+------+------+------+------+------+------+------+\n";
print $one_line;
$one_line  = "|       DESIGN$head3|        BRAM        |         DSP        |         FF         |         LUT        |SLACK |\n";
print $one_line;
$one_line  = "+$head2+------+------+------+------+------+------+------+------+------+------+------+------+      +\n";
print $one_line;
$one_line  = "+$head2| Used | Avail| Ratio| Used | Avail| Ratio| Used | Avail| Ratio| Used | Avail| Ratio|      |\n"; 
print $one_line;
$one_line  = "+$head1+------+------+------+------+------+------+------+------+------+------+------+------+------+\n";
print $one_line;

foreach $one_case (@dir_set) {
    $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt" ;
    $sum_dir  = "$work_dir/$one_case/run" ;
    if (-e "$sum_dir") {
        my @used=("   NG","    NG","    NG","    NG");   # [LUT, FF, BRAM, DSP]
        my @avai=("   NG","    NG","    NG","    NG");
        my @rati=("   NG","    NG","    NG","    NG");
        my $timing_slack = "    NG";

        $sum_info = `cat $sum_file 2> /dev/null`;
        if ($sum_info =~ /\n\|all (.*)\n(.*)\n(.*)\n/) {
            $timing_line = $3;
            if ($timing_line =~ /\|Used.*\|\s*(\S*)\|$/) {
                $timing_slack = $1;
            }
        }
        if ($sum_info =~ /\n\|all .*\n(.*)\n\|Used [^\|]*\|(.*)\n\|Available [^\|]*\|(.*)\n\|Utilization [^\|]*\|(.*)/) {
            $used_line = $2;
            $avai_line = $3;
            $rati_line = $4;
            if ($used_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
                $used[0] = $1;
                $used[1] = $2;
                $used[2] = $3;
                $used[3] = $4;
            }
            if ($avai_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
                $avai[0] = $1;
                $avai[1] = $2;
                $avai[2] = $3;
                $avai[3] = $4;
            }
            if ($rati_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
                $rati[0] = $1;
                $rati[1] = $2;
                $rati[2] = $3;
                $rati[3] = $4;
            }
        }

        $one_case = str_shift_to_left($one_case, $max_length, $max_length);
        for ($i = 0; $i < 4; $i++) {
            $used[$i] = str_shift_to_right($used[$i], 6);
        }
        for ($i = 0; $i < 4; $i++) {
            $avai[$i] = str_shift_to_right($avai[$i], 6);
        }
        for ($i = 0; $i < 4; $i++) {
            $rati[$i] = str_shift_to_right($rati[$i], 6);
        }
        $timing_slack= str_shift_to_right($timing_slack, 6);

        $one_line  = "|$one_case|$used[0]|$avai[0]|$rati[0]|$used[1]|$avai[1]|$rati[1]|$used[2]|$avai[2]|$rati[2]|$used[3]|$avai[3]|$rati[3]|$timing_slack|\n";
        print $one_line;
    }
}

$one_line  = "+$head1+------+------+------+------+------+------+------+------+------+------+------+------+------+\n";
print $one_line;

if ($type eq "kernel") {
$dir_list = `ls $work_dir`;
@dir_set = split(/\n/, $dir_list);

# print Header
$one_line  = "Kernel Part Utilization\n";
print $one_line;
$one_line  = "+$head1+------+------+------+------+------+------+------+------+------+------+------+------+------+\n";
print $one_line;
$one_line  = "|       DESIGN$head3|        BRAM        |         DSP        |         FF         |         LUT        |SLACK |\n";
print $one_line;
$one_line  = "+$head2+------+------+------+------+------+------+------+------+------+------+------+------+      +\n";
print $one_line;
$one_line  = "+$head2| Used | Avail| Ratio| Used | Avail| Ratio| Used | Avail| Ratio| Used | Avail| Ratio|      |\n"; 
print $one_line;
$one_line  = "+$head1+------+------+------+------+------+------+------+------+------+------+------+------+------+\n";
print $one_line;

foreach $one_case (@dir_set) {
    $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt" ;
    if (-e "$sum_file") {

        my @used=("   NG","    NG","    NG","    NG");   # [LUT, FF, BRAM, DSP]
        my @avai=("   NG","    NG","    NG","    NG");
        my @rati=("   NG","    NG","    NG","    NG");
        my $timing_slack = "    NG";
#        for ($i = 0; $i < 4; $i++) {
#            $util[$i] = "";
#        }
#        $timing_slack = "";
#        for ($i = 0; $i < 3; $i++) {
#            $pass[$i] = "";
#        }
#        for ($i = 0; $i < 6; $i++) {
#            $perf[$i] = "";
#        }

        $sum_info = `cat $sum_file`;
        if ($sum_info =~ /\n\|all (.*)\n(.*)\n(.*)\n/) {
            $timing_line = $3;
            if ($timing_line =~ /\|Used.*\|\s*(\S*)\|$/) {
                $timing_slack = $1;
            }
        }
        if ($sum_info =~ /\n\|kernel_part .*\n(.*)\n(.*)\n\|Used [^\|]*\|(.*)\n\|Available [^\|]*\|(.*)\n\|Utilization [^\|]*\|(.*)/) {
            $used_line = $3;
            $avai_line = $4;
            $rati_line = $5;
            if ($used_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
                $used[0] = $1;
                $used[1] = $2;
                $used[2] = $3;
                $used[3] = $4;
            }
            if ($avai_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
                $avai[0] = $1;
                $avai[1] = $2;
                $avai[2] = $3;
                $avai[3] = $4;
            }
            if ($rati_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
                $rati[0] = $1;
                $rati[1] = $2;
                $rati[2] = $3;
                $rati[3] = $4;
            }
        }

        $one_case = str_shift_to_left($one_case, $max_length, $max_length);
        for ($i = 0; $i < 4; $i++) {
            $used[$i] = str_shift_to_right($used[$i], 6);
        }
        for ($i = 0; $i < 4; $i++) {
            $avai[$i] = str_shift_to_right($avai[$i], 6);
        }
        for ($i = 0; $i < 4; $i++) {
            $rati[$i] = str_shift_to_right($rati[$i], 6);
        }
        $timing_slack= str_shift_to_right($timing_slack, 6);

        $one_line  = "|$one_case|$used[0]|$avai[0]|$rati[0]|$used[1]|$avai[1]|$rati[1]|$used[2]|$avai[2]|$rati[2]|$used[3]|$avai[3]|$rati[3]|$timing_slack|\n";
        print $one_line;
    }
}

$one_line  = "+$head1+------+------+------+------+------+------+------+------+------+------+------+------+------+\n";
print $one_line;
}


$work_dir = "work";
$dir_list = `ls $work_dir`;
@dir_set = split(/\n/, $dir_list);

# print Header
$one_line  = "+$head1+----+----+----+-----------+-----------+-----------+-----------+-------+------------+\n";
print $one_line;
$one_line  = "|       DESIGN$head3| C  |OCL |FPGA|FPGA COMPUT|FPGA TRNSFR| FPGATOTAL | CPU TOTAL |SPEEDUP|COMPILE TIME|\n";
print $one_line;
$one_line  = "+$head1+----+----+----+-----------+-----------+-----------+-----------+-------+------------+\n";
print $one_line;

foreach $one_case (@dir_set) {
    $sum_file_2 = "$work_dir/$one_case/run/implement/merlin_summary.rpt" ;
    if (-e "$sum_file_2") {
        for ($i = 0; $i < 3; $i++) {
            $pass[$i] = "";
        }
        for ($i = 0; $i < 6; $i++) {
            $perf[$i] = "";
        }

        $sum_info = `cat $sum_file_2`;
        if ($sum_info =~ /\nCorrectness :\s*\n(.*)\n(.*)\n(.*)\n/) {
            $line_1 = $1;
            $line_2 = $2;
            $line_3 = $3;

            if ($line_1 =~ /\s*[^:]*:\s*(\S*)$/) {
                $pass[0]= $1;
            }
            if ($line_2 =~ /\s*[^:]*:\s*(\S*)$/) {
                $pass[1]= $1;
            }
            if ($line_3 =~ /\s*[^:]*:\s*(\S*)$/) {
                $pass[2]= $1;
            }
#            printf $pass[0]."\n";
#            printf $pass[1]."\n";
#            printf $pass[2]."\n";
        }
        if ($sum_info =~ /\nPerformance :\s*\n.*\n.*\n.*\n(.*)\n(.*)\n(.*)\n.*\n(.*)\n.*\n(.*)/) {
            $line_1 = $1;
            $line_2 = $2;
            $line_3 = $3;
            $line_4 = $4;
            $line_5 = $5;
            if ($line_1 =~ /\s*\|[^\|]*\|\s*(\S*)\s*\|\s*$/) {
                $perf[1]= $1;
            }
            if ($line_2 =~ /\s*\|[^\|]*\|\s*(\S*)\s*\|\s*$/) {
                $perf[2]= $1;
            }
            if ($line_3 =~ /\s*\|[^\|]*\|\s*(\S*)\s*\|\s*$/) {
                $perf[3]= $1;
            }
            if ($line_4 =~ /\s*\|[^\|]*\|\s*(\S*)\s*\|\s*$/) {
                $perf[4]= $1;
            }
            if ($line_5 =~ /\s*\|[^\|]*\|\s*(\S*)\s*\|\s*$/) {
                $perf[5]= $1;
            }
            
#            printf $perf[1]."\n";
#            printf $perf[2]."\n";
#            printf $perf[3]."\n";
#            printf $perf[4]."\n";
#            printf $perf[5]."\n";
        }
        $compile_time = "   NG";
        if ($sum_info =~ /\nCompile Time   : (\S*) minutes \n/) {
            $compile_time = $1;
#            printf $compile_time."\n\n\n";
        }
        $compile_time = str_shift_to_right($compile_time, 12);

        $one_case = str_shift_to_left($one_case, $max_length, $max_length);
        $timing_slack= str_shift_to_right($timing_slack, 6);
        for ($i = 0; $i < 3; $i++) {
            $pass[$i] = str_shift_to_right($pass[$i], 4);
        }
        for ($i = 0; $i < 5; $i++) {
            $perf[$i] = str_shift_to_right($perf[$i], 11);
        }
        $perf[5] = str_shift_to_right($perf[5], 7);

        $one_line  = "|$one_case|$pass[0]|$pass[1]|$pass[2]|";
        $one_line .= "$perf[1]|$perf[2]|$perf[3]|$perf[4]|$perf[5]|$compile_time|\n";
        print $one_line;
    } else {
        if (-d "$work_dir/$one_case") {
            $one_case = str_shift_to_left($one_case, $max_length, $max_length);
            $one_line  = "|$one_case|  NG|  NG|  NG|";
            $one_line .= "         NG|         NG|         NG|         NG|     NG|          NG|\n";
            print $one_line;
        }
    }
}

$one_line  = "+$head1+----+----+----+-----------+-----------+-----------+-----------+-------+------------+\n";
print $one_line;


