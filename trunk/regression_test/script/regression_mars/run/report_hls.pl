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
# this script generate a report for regression test[M#?J[M#?4
# This scripts is called at regression_mars directory

# It scan all the generatd cases and extract all their summmary reports
use File::Basename;
use XML::Simple;
use Data::Dumper;

sub get_kernel_list {
     my $file = $_[0];
     my %kernel_list;
     my $kernel_name, $file_name;
     my $info = `cat $file`;
     my @list_set = split(/\n/, $info);
     foreach $one_line (@list_set) { 
         if ($one_line =~ /\/\/(.*)=(.*)/ ) {
             $kernel_name = $1;
             $file_name = $2;
             $kernel_list{"$kernel_name"} .= " $file_name";
         }
     }
     return %kernel_list;
}

$work_dir = "work";
$dir_list = `ls $work_dir`;
@dir_set = split(/\n/, $dir_list);
#get the max name of the case name
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
} else {
    $max_length = $max_length+1;;
}
#print " " x 20;
$head1 = "-" x $max_length;
$head2 = " " x $max_length;
$head3 = " " x ($max_length-9);
$head_empty = " " x $max_length;

#get the max name of the kernel name
$max_length_k = 1;
foreach $one_case (@dir_set) {
    if(-d "$work_dir/$one_case") {
        if(-e "$work_dir/$one_case/run.condor_output/__merlin_kernel_list.h") {
            %kernel_file_list = get_kernel_list("$work_dir/$one_case/run.condor_output/__merlin_kernel_list.h");
            while (my ($key, $value) = each(%kernel_file_list)) {
                $kernel_name = $key;
                $file_list = $value;
                $name_length_k = length($kernel_name);
                if($max_length_k < $name_length_k) {
                    $max_length_k = $name_length_k;
                }
            }
        }
    }
}

if($max_length_k < 13) {
    $max_length_k = 13;
} else {
    $max_length_k = $max_length_k+1;;
}

$tool_flag = "sdaccel";
$directive_file = "directive.xml";
if(-e $directive_file) {
    $directive_list = XML::Simple->new(ForceArray => 1);
    $directive_all = $directive_list->XMLin($directive_file); 
    if($directive_all->{"implementation_tool"}[0] =~ /sdaccel/) {
        $tool_flag = "sdaccel";
    }elsif($directive_all->{"implementation_tool"}[0] =~ /aocl/) {
        $tool_flag = "aocl";
    } else {
        $tool_flag = "built-in";
    }
}
if($tool_flag eq "aocl") {
    $max_length_k = 20;
}
#printf "$max_length_k\n";
$head1_k = "-" x $max_length_k;
$head2_k = " " x $max_length_k;
$head3_k = " " x ($max_length_k-13);
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


$dir_list = `ls $work_dir`;
@dir_set = split(/\n/, $dir_list);

# get enviroment
printf "\n";
$count_env = 0;
foreach $one_case (@dir_set) {

#############
#LZ 20180411
    $log_file = "$work_dir/$one_case/run.condor_output/merlin_hls.log";
    if (!-e "$log_file"){
        $log_file = "$work_dir/$one_case/run/merlin_hls.log";
        }
    if (-e "$log_file"){
        $grep_line .= `grep -E "XOCC|.merlin_prj|/implement/|^aoc" $log_file |grep -v "/run_ref/" `;
        #print "$grep_line\n";
        if ( $grep_line ne "" ){
        $msg_line .= "$one_case: vendor msg found in $log_file\n";
        #$msg_line .= "$grep_line \n";
        }
        }else{
        $y_line .= "$one_case log not found\n";
        }
######


    if($count_env == 0) {
        $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt";
#
###############
#ZP: 20170910
        if (!-e "$sum_file") {
            $sum_file= "$work_dir/$one_case/run.condor_output/merlin_summary.rpt" ;
        }
###############
#
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
        }else{
	if ($one_case ne "error.log" && $one_case ne "curr_case.log"){
	     $z_line .= "$one_case HLS rpt not found.\n";
	    }
	}
    }
}
printf "\n";

$one_line  = "High Level Synthesis report summary\n";
$one_line .= "+$head1+$head1_k+-------------+-------------+---------------+---------------+-----------+\n";
$one_line .= "|   DESIGN$head3|  KERNEL NAME$head3_k|    BRAM     |     DSP     |      FF       |      LUT      |HLS Cycles |\n";
$one_line .= "+$head1+$head1_k+-------------+-------------+---------------+---------------+-----------+\n";
print $one_line;

foreach $one_case (@dir_set) {
    $sum_file = "$work_dir/$one_case/run/implement/merlin_summary.rpt" ;
    $sum_dir  = "$work_dir/$one_case/run.condor_output" ;
    $kernel_list_file = "$work_dir/$one_case/run/implement/export/__merlin_kernel_list.h";
###############
#ZP: 20170910
    if (!-e "$kernel_list_file") {
        $kernel_list_file= "$work_dir/$one_case/run.condor_output/__merlin_kernel_list.h" ;
    }
    if (!-e "$sum_file") {
        $sum_file= "$work_dir/$one_case/run.condor_output/merlin_summary.rpt" ;
    }
###############
    if($one_case =~ /(.*)-(.*-.*)/) {
        $case_name = $2;
    } elsif($one_case =~ /(.*)-(.*)/) {
        $case_name = $2;
    } else {
        $case_name = $one_case;
    }

    if(-d "$work_dir/$one_case") {
        if(!-e "$sum_dir/.merlin_flow_end.o") {
        	$case_name = str_shift_to_left($case_name, $max_length, $max_length);
        	$kernel_name = str_shift_to_right(" ", $max_length_k, $max_length_k);
        	$one_line  = "|$case_name|$kernel_name|      RUNNING|      RUNNING|        RUNNING|        RUNNING|    RUNNING|\n";
        	print $one_line;
        } elsif (-e "$sum_file") {
    	    for ($i = 0; $i < 4; $i++) {
    	        $used[$i] = "NG";
    	        $avai[$i] = "NG";
    	        $rati[$i] = "NG";
    	    }
            if ($tool_flag eq "aocl") {
    	        $sum_info = `cat $sum_file 2> /dev/null`;
                if ($sum_info =~ /\|Total [^\|]*\|(.*)\n\|Available [^\|]*\|(.*)\n\|Utilization \(%\) [^\|]*\|(.*)\n/) {
                    $used_line = $1;
    	            $avai_line = $2;
    	            $rati_line = $3;
                    if ($used_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|(.*)\|(.*)\|/) {
                        $used[2] = sprintf "%.1f",$3/1000;
                        $used[3] = sprintf "%.1f",$4/1000;
                        $used[0] = $1;
                        $used[1] = $2;
                        $used[2] = "$used[2]k";
                        $used[3] = "$used[3]k";
                        $cycles = $5;
                    }
                    if ($avai_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|(.*)\|(.*)\|/) {
                        $avai[2] = sprintf "%.1f",$3/1000;
                        $avai[3] = sprintf "%.1f",$4/1000;
                        $avai[0] = $1;
                        $avai[1] = $2;
                        $avai[2] = "$avai[2]k";
                        $avai[3] = "$avai[3]k";
                    }
                    if ($rati_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|(.*)\|(.*)\|/) {
                        $rati[0] = sprintf "%.0f",$1;  
                        $rati[1] = sprintf "%.0f",$2;  
                        $rati[2] = sprintf "%.0f",$3;  
                        $rati[3] = sprintf "%.0f",$4;  
                        if ($rati[0] =~ /~0/)  { $rati[0] = "       1"; } 
                        if ($rati[1] =~ /~0/)  { $rati[1] = "       1"; } 
                        if ($rati[2] =~ /~0/)  { $rati[2] = "       1"; } 
                        if ($rati[3] =~ /~0/)  { $rati[3] = "       1"; } 
                        $rati[0] =~ s/\s*//g ;
                        $rati[1] =~ s/\s*//g ;
                        $rati[2] =~ s/\s*//g ;
                        $rati[3] =~ s/\s*//g ;
                        if ( $rati[0] ne "") {$rati[0] = "($rati[0]%)" ; } 
                        if ( $rati[1] ne "") {$rati[1] = "($rati[1]%)" ; }
                        if ( $rati[2] ne "") {$rati[2] = "($rati[2]%)" ; }
                        if ( $rati[3] ne "") {$rati[3] = "($rati[3]%)" ; }
    	            }

    	            $case_name = str_shift_to_left($case_name, $max_length, $max_length);
		            if($kernel_name =~ "__merlinkernel_") {
		            	$kernel_name = " ";
		            }
    	            $kernel_name = str_shift_to_right($kernel_name, $max_length_k, $max_length_k);
    	            $used[0] = str_shift_to_right($used[0], 6);
    	            $used[1] = str_shift_to_right($used[1], 6);
    	            $used[2] = str_shift_to_right($used[2], 8);
    	            $used[3] = str_shift_to_right($used[3], 8);
    	            $rati[0] = str_shift_to_right($rati[0], 6);
    	            $rati[1] = str_shift_to_right($rati[1], 6);
    	            $rati[2] = str_shift_to_right($rati[2], 6);
    	            $rati[3] = str_shift_to_right($rati[3], 6);
    	            $cycles = str_shift_to_right($cycles, 11);
    	            $one_line  = "|$case_name|$kernel_name|$used[0] $rati[0]|$used[1] $rati[1]|$used[2] $rati[2]|$used[3] $rati[3]|$cycles|\n";
    	            print $one_line;
                }
            } elsif (-e "$kernel_list_file") {
                $kernel_index = 0;
                $list_file  = "$work_dir/$one_case/run.condor_output";
                if(-e $kernel_list_file) {
                    %kernel_file_list = get_kernel_list("$kernel_list_file");
                    while (my ($key, $value) = each(%kernel_file_list)) {
                        $kernel_name = $key;
                        $file_list = $value;
    	                for ($i = 0; $i < 4; $i++) {
    	                    $used[$i] = "NG";
    	                    $avai[$i] = "NG";
    	                    $rati[$i] = "NG";
    	                }
    	                $cycles = "NG";
    	                
    	                if ($sum_info =~ /\|$kernel_name.*\n\+\-.*\n\|Total [^\|]*\|(.*)\n\|Available [^\|]*\|(.*)\n\|Utilization [^\|]*\|(.*)/) {
                                $used_line = $1;
    	                    $avai_line = $2;
    	                    $rati_line = $3;
                            if ($used_line =~ /\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|\s*(\S*)\|(.*)\|(.*)\|/) {
                                $used[0] = $1;
                                $used[1] = $2;
                                $used[2] = $3;
                                $used[3] = $4;
                                $cycles = $5;
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
                                if ($rati[0] =~ /~0/)  { $rati[0] = "       1"; } 
                                if ($rati[1] =~ /~0/)  { $rati[1] = "       1"; } 
                                if ($rati[2] =~ /~0/)  { $rati[2] = "       1"; } 
                                if ($rati[3] =~ /~0/)  { $rati[3] = "       1"; } 
    	                        $rati[0] =~ s/\s*//g ;
    	                        $rati[1] =~ s/\s*//g ;
    	                        $rati[2] =~ s/\s*//g ;
    	                        $rati[3] =~ s/\s*//g ;
    	                        if ( $rati[0] ne "") {$rati[0] = "($rati[0]%)" ; } 
    	                        if ( $rati[1] ne "") {$rati[1] = "($rati[1]%)" ; }
    	                        if ( $rati[2] ne "") {$rati[2] = "($rati[2]%)" ; }
    	                        if ( $rati[3] ne "") {$rati[3] = "($rati[3]%)" ; }
    	                    }
    	                }
                            
    	                $case_name = str_shift_to_left($case_name, $max_length, $max_length);
		                if($kernel_name =~ "__merlinkernel_") {
		                	$kernel_name = " ";
		                }
    	                $kernel_name = str_shift_to_right($kernel_name, $max_length_k, $max_length_k);
    	                $used[0] = str_shift_to_right($used[0], 6);
    	                $used[1] = str_shift_to_right($used[1], 6);
    	                $used[2] = str_shift_to_right($used[2], 8);
    	                $used[3] = str_shift_to_right($used[3], 8);
    	                $rati[0] = str_shift_to_right($rati[0], 6);
    	                $rati[1] = str_shift_to_right($rati[1], 6);
    	                $rati[2] = str_shift_to_right($rati[2], 6);
    	                $rati[3] = str_shift_to_right($rati[3], 6);
    	                $cycles = str_shift_to_right($cycles, 11);
                        if($kernel_index == 0) {
    	                    $one_line  = "|$case_name|$kernel_name|$used[0] $rati[0]|$used[1] $rati[1]|$used[2] $rati[2]|$used[3] $rati[3]|$cycles|\n";
    	                    print $one_line;
                        } else {
    	                    $one_line  = "|$head_empty|$kernel_name|$used[0] $rati[0]|$used[1] $rati[1]|$used[2] $rati[2]|$used[3] $rati[3]|$cycles|\n";
    	                    print $one_line;
                        }
                        $kernel_index = $kernel_index + 1;
                    }
                }
	        } 
        } else {
            $case_name = str_shift_to_left($case_name, $max_length, $max_length);
            $kernel_name = str_shift_to_right(" ", $max_length_k, $max_length_k);
            $one_line  = "|$case_name|$kernel_name|           NG|           NG|             NG|             NG|         NG|\n";
            print $one_line;
        }
    }
}

$one_line = "+$head1+$head1_k+-------------+-------------+---------------+---------------+-----------+\n";
print $one_line;
print "Note:\n";
print "Check rpt exist:\n";
print $z_line;
print "Check vendor msg:\n";
print "$msg_line\n";
#print $y_line;

