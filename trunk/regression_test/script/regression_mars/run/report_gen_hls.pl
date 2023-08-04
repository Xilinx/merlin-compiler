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
$short_mode = 1;
foreach $arg ( @ARGV ) {
    if ($arg eq "-l" or $arg eq "-long") { $short_mode = 0;}
    if ($arg eq "-f" or $arg eq "-full") { $short_mode = 0;}
}


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

$type = "hls"; 
if ($type eq "hls") {
$dir_list = `ls $work_dir`;
@dir_set = split(/\n/, $dir_list);

# get enviroment
printf "\n";
$count_env = 0;
foreach $one_case (@dir_set) {
    if($count_env == 0) {
        $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt";
        if(-e $sum_file) {
            $count_env++;
            $sum_info = `cat $sum_file 2> /dev/null`;
            printf "Test Environment\n";
            if ($sum_info =~ /Mars Compiler - FPGA System Synthesis.*\n.*\n\s*(.*): (.*)\n/) {
                $mars_version = $2;
                printf "Mars Build Rev :$2\n";
            }
            if ($sum_info =~ /High Level Synthesis.*\n.*\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n(.*)\n/) {
                printf "Vivado HLS     :$2\n";
            }
        }
    }
}
printf "\n";

if ($short_mode) {
$one_line  = "High Level Synthesis report summary\n";
$one_line .= "+$head1+-----------+-----------+-------------+-------------+-----------+\n";
$one_line .= "|       DESIGN$head3|   BRAM    |    DSP    |     FF      |     LUT     |HLS Cycles |\n";
$one_line .= "+$head1+-----------+-----------+-------------+-------------+-----------+\n";
print $one_line;
}
else {
# print Header
$one_line  = "High Level Synthesis report summary\n";
print $one_line;
$one_line  = "+$head1+-----------------------+---------------------+----------------------+----------------------+-----------+\n";
print $one_line;                                                                                                                   
$one_line  = "|       DESIGN$head3|          BRAM         |          DSP        |          FF          |          LUT         |  Cycles   |\n";
print $one_line;                                                                                                                   
$one_line  = "|$head2+------+------+---------+------+------+-------+------+------+--------+------+------+--------+           |\n";
print $one_line;                                                                                                                   
$one_line  = "|$head2| Used | Avail|  Ratio  | Used | Avail| Ratio | Used | Avail|  Ratio | Used | Avail|  Ratio |           |\n";
print $one_line;                                                                                                                   
$one_line  = "+$head1+------+------+---------+------+------+-------+------+------+--------+------+------+--------+-----------+\n";
print $one_line;
}

foreach $one_case (@dir_set) {
    $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt" ;
    $sum_dir  = "$work_dir/$one_case/run" ;
    if (-e "$sum_dir") {

	for ($i = 0; $i < 4; $i++) {
	    $used[$i] = "NG";
	}
	$cycles = "NG";
	for ($i = 0; $i < 4; $i++) {
	    $avai[$i] = "NG";
	}
	for ($i = 0; $i < 4; $i++) {
	    $rati[$i] = "NG";
	}
	$sum_info = `cat $sum_file 2> /dev/null`;
#	if ($sum_info =~ /\n  High Level Synthesis .*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*\n\|Total [^\|]*\|(.*)\n\|Available [^\|]*\|(.*)\n\|Utilization [^\|]*\|(.*)/) {
	if ($sum_info =~ /\n  High Level Synthesis .*\n[\s\S]*\|Total [^\|]*\|(.*)\n\|Available [^\|]*\|(.*)\n\|Utilization [^\|]*\|(.*)/) {
#	    printf "$1\n";
#	    printf "$2\n";
#	    printf "$3\n";
	    $used_line = $1;
	    $avai_line = $2;
	    $rati_line = $3;
	    if ($used_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|(.*)\|(.*)\|/) {
		$used[0] = $1;
		$used[1] = $2;
		$used[2] = $3;
		$used[3] = $4;
		$cycles = $5;
#	    	printf "$5\n";
	    }
	    if ($avai_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|/) {
		$avai[0] = $1;
		$avai[1] = $2;
		$avai[2] = $3;
		$avai[3] = $4;
	    }
	    if ($rati_line =~ /(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
		$rati[0] = $1;  
		$rati[1] = $2;  
		$rati[2] = $3;  
		$rati[3] = $4;  

        if ($rati[0] =~ /~0/)  { $rati[0] = "        1"; } 
        if ($rati[1] =~ /~0/)  { $rati[1] = "      1"; } 
        if ($rati[2] =~ /~0/)  { $rati[2] = "       1"; } 
        if ($rati[3] =~ /~0/)  { $rati[3] = "       1"; } 


#    		printf "$1\n";
#	    	printf "$2\n";
#	    	printf "$3\n";
#	    	printf "$4\n";
	    }
	}
    
    if($one_case =~ /(.*)-(.*)/) {
        $case_name = $2;
    }
	$case_name = str_shift_to_left($case_name, $max_length, $max_length);
#	$one_case = str_shift_to_left($one_case, 13, 13);
	$cycles = str_shift_to_right($cycles, 11);

if ($short_mode) {
	$used[0] = str_shift_to_right($used[0], 5);
	$used[1] = str_shift_to_right($used[1], 5);
	$used[2] = str_shift_to_right($used[2], 7);
	$used[3] = str_shift_to_right($used[3], 7);

	$rati[0] =~ s/\s*//g ;
	$rati[1] =~ s/\s*//g ;
	$rati[2] =~ s/\s*//g ;
	$rati[3] =~ s/\s*//g ;
    
	if ( $rati[0] ne "") {$rati[0] = "($rati[0]%)" ; } 
	if ( $rati[1] ne "") {$rati[1] = "($rati[1]%)" ; }
	if ( $rati[2] ne "") {$rati[2] = "($rati[2]%)" ; }
	if ( $rati[3] ne "") {$rati[3] = "($rati[3]%)" ; }

	$rati[0] = str_shift_to_right($rati[0], 5);
	$rati[1] = str_shift_to_right($rati[1], 5);
	$rati[2] = str_shift_to_right($rati[2], 5);
	$rati[3] = str_shift_to_right($rati[3], 5);
	$one_line  = "|$case_name|$used[0] $rati[0]|$used[1] $rati[1]|$used[2] $rati[2]|$used[3] $rati[3]|$cycles|\n";
	print $one_line;
}
else 
{
    if($one_case =~ /(.*)-(.*)/) {
        $case_name = $2;
    }
	$case_name = str_shift_to_left($case_name, $max_length, $max_length);
	for ($i = 0; $i < 4; $i++) {
	    $used[$i] = str_shift_to_right($used[$i], 6);
	}
	for ($i = 0; $i < 4; $i++) {
	    $avai[$i] = str_shift_to_right($avai[$i], 6);
	}
	$rati[0] = str_shift_to_right($rati[0], 9);
	$rati[1] = str_shift_to_right($rati[1], 7);
	$rati[2] = str_shift_to_right($rati[2], 8);
	$rati[3] = str_shift_to_right($rati[3], 8);
	$one_line  = "|$case_name|$used[0]|$avai[0]|$rati[0]|$used[1]|$avai[1]|$rati[1]|$used[2]|$avai[2]|$rati[2]|$used[3]|$avai[3]|$rati[3]|$cycles|\n";
	print $one_line;
}
    }
}

if ($short_mode) {
    $one_line = "+$head1+-----------+-----------+-------------+-------------+-----------+\n";
    print $one_line;
}
else {
    $one_line  = "+$head1+------+------+---------+------+------+-------+------+------+--------+------+------+--------+-----------+\n";
    print $one_line;
}
    print "Have a nice day!\n";
}
