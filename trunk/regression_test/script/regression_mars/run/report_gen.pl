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


$work_dir = "work/";

$dir_list = `ls $work_dir`;

@dir_set = split(/\n/, $dir_list);

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

# print Header
$one_line  = "+----------+-----+-----+-----+-----+------+";
$one_line .= "----+----+----+";
$one_line .= "-----------+-----------+-----------+-----------+-------+\n";
print $one_line;
$one_line  = "|  DESIGN  |BRAM | DSP | LUT | FF  |SLACK |";
$one_line .= " C  |OCL |FPGA|";
$one_line .= "FPGA COMPUT|FPGA TRNSFR| FPGATOTAL | CPU TOTAL |SPEEDUP|\n";
print $one_line;
$one_line  = "+----------+-----+-----+-----+-----+------+";
$one_line .= "----+----+----+";
$one_line .= "-----------+-----------+-----------+-----------+-------+\n";
print $one_line;


foreach $one_case (@dir_set) {
    $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt" ;
    $sum_dir  = "$work_dir/$one_case/run" ;
    if (-e "$sum_dir") {

        for ($i = 0; $i < 4; $i++) {
            $util[$i] = "";
        }
        $timing_slack = "";
        for ($i = 0; $i < 3; $i++) {
            $pass[$i] = "";
        }
        for ($i = 0; $i < 6; $i++) {
            $perf[$i] = "";
        }

        $sum_info = `cat $sum_file 2> /dev/null`;
        if ($sum_info =~ /\n\|all.*\n.*\n(.*)\n.*\n\|Utilization [^\|]*\|(.*)/) {
            $timing_line = $1;
            $util_line = $2;
            if ($timing_line =~ /\|Total.*\|\s*(\S*)\|$/) {
                $timing_slack = $1;
            }
            if ($util_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
                $util[0] = $1;
                $util[1] = $2;
                $util[2] = $3;
                $util[3] = $4;
            }
        }
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

        $one_case = str_shift_to_left($one_case, 10, 10);
        for ($i = 0; $i < 4; $i++) {
            $util[$i] = str_shift_to_right($util[$i], 5);
        }
        $timing_slack= str_shift_to_right($timing_slack, 6);
        for ($i = 0; $i < 3; $i++) {
            $pass[$i] = str_shift_to_right($pass[$i], 4);
        }
        for ($i = 0; $i < 5; $i++) {
            $perf[$i] = str_shift_to_right($perf[$i], 11);
        }
        $perf[5] = str_shift_to_right($perf[5], 7);

        $one_line  = "|$one_case|$util[0]|$util[1]|$util[2]|$util[3]|$timing_slack|";
        $one_line .= "$pass[0]|$pass[1]|$pass[2]|";
        $one_line .= "$perf[1]|$perf[2]|$perf[3]|$perf[4]|$perf[5]|\n";
        print $one_line;
    }
}

$one_line  = "+----------+-----+-----+-----+-----+------+";
$one_line .= "----+----+----+";
$one_line .= "-----------+-----------+-----------+-----------+-------+\n";
print $one_line;




