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
$platform = $ARGV[0];
$vendor = $ARGV[1];
$step = $ARGV[2];
$mars_test_home = $ENV{'MARS_TEST_HOME_GIT'};

$length = 1024*1024*256;
$burst_length_max = $length;
$burst_number_max = 1;
printf "burst_length_max=$burst_length_max\n";
printf "burst_number_max=$burst_number_max\n";
system "rm -rf log; touch log";
open(my $fh, '>', "log");

sub get_exec_time {
    $file = $_[0];

    $count_wr = 0;
    $wr_start = 0;
    $wr_finish = 0;
    $wr_time = 0;
    $wr_time_pure = 0;
    $count_rd = 0;
    $rd_start = 0;
    $rd_finish = 0;
    $rd_time = 0;
    $rd_time_pure = 0;
    $exec_time = 0;
    $pcie_wr_flag = 0;
    $pcie_rd_flag = 0;
    $kernel_exec_flag = 0;

    $info = `cat $file`;
    @info_line_set = split(/\n/, $info);
    foreach $one_line (@info_line_set) {
        if($pcie_wr_flag eq 1) {
            if($one_line =~ /\S*,\S*,.mem_transfer,(\S*),(\S*)/) {
                $wr_time_pure += $2 - $1;
                if($count_wr eq 0) {
                    $wr_start = $1;
                }
                $wr_finish = $2;
                $count_wr++;
            } else {
                $pcie_wr_flag = 0;
                $pcie_rd_flag = 1;
                $kernel_exec_flag = 1;
            }
        }
        if($pcie_rd_flag eq 1) {
            if($one_line =~ /\S*,\S*,.mem_transfer,(\S*),(\S*)/) {
                $rd_time_pure += $2 - $1;
                if($count_rd eq 0) {
                    $rd_start = $1;
                }
                $rd_finish = $2;
                $count_rd++;
            } else {
                $pcie_wr_flag = 0;
                $pcie_rd_flag = 1;
                $kernel_exec_flag = 1;
            }
        }
        if($kernel_exec_flag eq 1) {
            if($one_line =~ /\S*,\S*,\S*,(\S*),(\S*),(\S*)/) {
                $exec_time += $2 - $1;
            }
        }
        if($one_line =~ /reprogram/) {
            $pcie_wr_flag = 1;
            $pcie_rd_flag = 0;
            $kernel_exec_flag = 0;
        }
    }
    $wr_time = $wr_time_pure;
    $rd_time = $rd_time_pure;
    return ($wr_time, $rd_time, $exec_time);
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

sub extract_time {
    $time_mode =  $_[0];
    $NA         = "NA";
    $flag_seq   = "----------";
    $blank      = str_shift_to_right("bandwidth", 10);
    $blank1     = str_shift_to_right("1", 10);
    $blank2     = str_shift_to_right("2", 10);
    $blank4     = str_shift_to_right("4", 10);
    $blank8     = str_shift_to_right("8", 10);
    $blank16    = str_shift_to_right("16", 10);
    $blank32    = str_shift_to_right("32", 10);
    $blank64    = str_shift_to_right("64", 10);
    $blank128   = str_shift_to_right("128", 10);
    $blank256   = str_shift_to_right("256", 10);
    $blank512   = str_shift_to_right("512", 10);
    $blank1024  = str_shift_to_right("1024", 10);
    #execute comand with length and number, then collect data
    my @bandwidth_array;
    for($i=1; $i<=$burst_number_max; $i=$i*2) {
        for($j=1; $j<=$burst_length_max; $j=$j*4) {
            system "cd $time_mode; ./host_top $j $i";
            $profile_file = "$time_mode/profile.mon";
            ($pcie_wr, $pcie_rd, $kernel_exec) = get_exec_time($profile_file);
    	    if($time_mode eq "pcie_wr") {
                $time = $pcie_wr;
            } elsif($time_mode eq "pcie_rd") {
                $time = $pcie_rd;
            } elsif($time_mode eq "ddr_wr" or $time_mode eq "ddr_rd") {
                $time = $kernel_exec;
            } else {
                $time = $NA;
            }
            printf "mode = $time_mode, time = $time\n";
            $time= $time/1000;
#            if($time =~ /(\S+)\.(\S+)/) {$time = $1}
            $bandwidth = (4 * $i * $j) / $time;
            if($time_mode eq "ddr_wr" or $time_mode eq "ddr_rd") {
                $bandwidth = $bandwidth * 2;
            }
            $bandwidth = sprintf("%.2f", $bandwidth);
            $bandwidth_array[$j][$i] = str_shift_to_right($bandwidth, 10);
        }
    }
    #print data
    print $fh "$time_mode bandwidth test result : \n";
    print $fh "+$flag_seq+";
    for($i=1; $i<=$burst_length_max; $i=$i*4) {
        print $fh "$flag_seq+";
    }
    print $fh "\n";
    print $fh "|$blank|";
    for($i=1; $i<=$burst_length_max; $i=$i*4) {
        if($time_mode eq "ddr_wr" or $time_mode eq "ddr_rd") {
            $title_length = $i * 4 * 2;
        } else {
            $title_length = $i * 4;
        }
        $title_length = str_shift_to_right($title_length, 10);
        print $fh "$title_length|";
    }
    print $fh "\n";
    print $fh "+$flag_seq+";
    for($i=1; $i<=$burst_length_max; $i=$i*4) {
        print $fh "$flag_seq+";
    }
    print $fh "\n";
    for($i=1; $i<=$burst_number_max; $i=$i*2) {
        $number_index = str_shift_to_right($i, 10);
        print $fh "|$number_index";
        for($j=1; $j<=$burst_length_max; $j=$j*4) {
            $data = $bandwidth_array[$j][$i];
            print $fh "|$data";
        }
        printf $fh "|\n";
    }
    print $fh "+$flag_seq+";
    for($i=1; $i<=$burst_length_max; $i=$i*4) {
        print $fh "$flag_seq+";
    }
    print $fh "\n";
}

#---------------------------------------------------------------#
# main process                                                  #
#---------------------------------------------------------------#
if($step eq "bitgen" or $step eq "all") {
    # copy project and change board name in spec
    system "rm -rf bitstream; mkdir bitstream; cp -r /$mars_test_home/test_case/test_cases_platform/* bitstream";
    my $project_list = `cd bitstream; ls`;
    my @project_set = split(/\n/, $project_list);
    foreach my $one_project (@project_set) {
        system "cd bitstream/$one_project; perl -pi -e 's/\<platform_name\>.*?\</\<platform_name\>$platform\</g' run/spec/directive.xml";
        system "cd bitstream/$one_project; merlin_flow run bit";
    }

    # collect output
    system "rm -rf pcie_wr";
    system "cp -r bitstream/pcie_wr/run/pkg pcie_wr";
    system "rm -rf pcie_rd";
    system "cp -r bitstream/pcie_rd/run/pkg pcie_rd";
    system "rm -rf ddr_wr";
    system "cp -r bitstream/ddr_wr/run/pkg  ddr_wr";
    system "rm -rf ddr_rd";
    system "cp -r bitstream/ddr_rd/run/pkg  ddr_rd";
}
if($step eq "collect" or $step eq "all") {
    if(-e "ddr_wr") {
        extract_time("ddr_wr");
        printf "\n";
    }
    if(-e "ddr_rd") {
        extract_time("ddr_rd");
        printf "\n";
    }
    close $fh;
    system "python write_excel_bandwidth.py log";
}
