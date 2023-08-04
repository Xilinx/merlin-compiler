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
$work_dir = "work";
$dir_list = `ls $work_dir`;
@dir_set = split(/\n/, $dir_list);
$max_length = 1;
foreach $one_case (@dir_set) {
    if($one_case =~ /(.*?)\-(.*)/) {
        $case_name = $2;
    } else {
        $case_name = $one_case;
    }
    $name_length = length($case_name);
    if($max_length < $name_length) {
        $max_length = $name_length;
    }
}
if($max_length < 13) {
    $max_length = 13;
}
$head1 = "-" x $max_length;
$head2 = " " x $max_length;
$head3 = " " x ($max_length-13);
$blank = " " x 30;

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

#get implement tool
use File::Basename;
use XML::Simple;
use Data::Dumper;
$count_xml = 0;
foreach $one_case (@dir_set) {
    $directive_file = "work/$one_case/run/spec/directive.xml";

    if(-e $directive_file and $count_xml eq "0") {
        $directive_list = XML::Simple->new(ForceArray => 1);
        $directive_all = $directive_list->XMLin($directive_file); 
        if($directive_all->{"implementation_tool"}[0] =~ /sdaccel/) {
            $tool_flag = "sdaccel";
        } else {
            $tool_flag = "built-in";
        }
        $count_xml = 1;
        $platform_name = $directive_all->{"platform_name"}[0];
        printf "$tool_flag\n";
    }
}

# get enviroment
printf "\n";
$count_env = 0;
foreach $one_case (@dir_set) {
    if($count_env == 0) {
        $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt";
###############
#ZP: 20170910
        if (!-e "$sum_file") {
            $sum_file= "$work_dir/$one_case/run.condor_output/merlin_summary.rpt" ;
        }
###############
        if(-e $sum_file) {
            $sum_info = `cat $sum_file 2> /dev/null`;
            if ($sum_info =~ /Mars Compiler - FPGA System Synthesis.*\n.*\n\s*(.*): (.*)\n/) {
                $mars_version = $2;
            }
            if ($sum_info =~ /Execution and Testing.*\n.*\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n/) {
                $count_env++;
		        $info1 = $1; 
		        $info2 = $2; 
		        $info3 = $3; 
		        $info4 = $4; 
		        $info5 = $5; 
		        $info6 = $6; 
		        $info7 = $7; 
            }
	        if($count_env eq "1") {
                printf "Test Environment\n";
                printf "Mars Build Rev :$mars_version\n";
                printf "$info1\n";
                printf "$info2\n";
                printf "$info3\n";
                printf "$info4\n";
                printf "$info5\n";
                printf "$info6\n";
                printf "$info7\n";
                printf "Platform  :$platform_name\n";
	        }
        }
    }
}
printf "\n";

$work_dir = "work";
$dir_list = `ls $work_dir`;
@dir_set = split(/\n/, $dir_list);

$one_line  = "On board test performance:\n";
print $one_line;
# print Header
$one_line  = "+$head1+----+----+----+-----------+-----------+-----------+-----------+-----------+-----------+-----------+\n";
print $one_line;
$one_line  = "|       DESIGN$head3| C  |OCL |FPGA|FPGA COMPUT|FPGA TRNSFR| FPGATOTAL | CPU TOTAL |  SPEEDUP  |COMPILETIME|   SLACK   |\n";
print $one_line;
$one_line  = "+$head1+----+----+----+-----------+-----------+-----------+-----------+-----------+-----------+-----------+\n";
print $one_line;
    
foreach $one_case (@dir_set) {
    if(-d "$work_dir/$one_case") {
        $sum_file_2 = "$work_dir/$one_case/run/implement/merlin_summary.rpt" ;
###############
#ZP: 20170910
        if (!-e "$sum_file_2") {
            $sum_file_2= "$work_dir/$one_case/run.condor_output/merlin_summary.rpt" ;
        }
###############
        if($one_case =~ /(.*)-(.*-.*)/) {
            $case_name = $2;
        } elsif($one_case =~ /(.*)-(.*)/) {
            $case_name = $2;
        } else {
            $case_name = $one_case;
	    }
        $case_name = str_shift_to_left($case_name, $max_length, $max_length);

        if(!-e "$work_dir/$one_case/run.condor_output/.merlin_flow_end.o") {
            $one_line  = "|$case_name| RUN| RUN| RUN|";
            $one_line .= "    RUNNING|    RUNNING|    RUNNING|    RUNNING|    RUNNING|    RUNNING|    RUNNING|\n";
            print $one_line;
        } elsif (-e "$sum_file_2") {
            for ($i = 0; $i < 3; $i++) {
                $pass[$i] = "NG";
            }
            for ($i = 0; $i < 6; $i++) {
                $perf[$i] = "NG";
            }
            $compile_time = "NG";
            $timing_slack = "NG";
            $sum_info = `cat $sum_file_2`;
            if ($sum_info =~ /\n\|all (.*)\n(.*)\n(.*)\n/) {
                $timing_line = $3;
                if ($timing_line =~ /\|Used.*\|\s*(\S*)\|$/) {
                    $timing_slack = $1;
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
            }
            if ($sum_info =~ /\nCompile Time   : (\S*) minutes \n/) {
                $compile_time = $1;
            }
            $compile_time = str_shift_to_right($compile_time, 11);
    
            $timing_slack= str_shift_to_right($timing_slack, 11);
            for ($i = 0; $i < 3; $i++) {
                $pass[$i] = str_shift_to_right($pass[$i], 4);
            }
            for ($i = 0; $i < 5; $i++) {
                $perf[$i] = str_shift_to_right($perf[$i], 11);
            }
            $perf[5] = str_shift_to_right($perf[5], 11);
    
            $one_line  = "|$case_name|$pass[0]|$pass[1]|$pass[2]|";
            $one_line .= "$perf[1]|$perf[2]|$perf[3]|$perf[4]|$perf[5]|$compile_time|$timing_slack|\n";
            print $one_line;
        } else {
            if (-d "$work_dir/$one_case") {
                $one_line  = "|$case_name|  NG|  NG|  NG|";
                $one_line .= "         NG|         NG|         NG|         NG|         NG|         NG|         NG|\n";
                print $one_line;
            }
        }
    }
}
$one_line  = "+$head1+----+----+----+-----------+-----------+-----------+-----------+-----------+-----------+-----------+\n";
print $one_line;
print "Note:\n";
print "     *NG: this case didn't generate the output.\n";
print "     The comparison with \"best\"  can be found in report/compare_result.log\n";
print "     The comparison of svn repos can be found in report/svn_diff.log\n";
print "\n";

