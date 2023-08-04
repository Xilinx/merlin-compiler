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

for($i=0; $i<10; $i=$i+1) {
    if($ARGV[$i] eq "-h") {
        $mode = "one_hour_test";
    }
    if($ARGV[$i] eq "-hls") {
        $mode = "hls";
    }
    if($ARGV[$i] eq "-compare") {
        $first_file = $ARGV[$i+1];
        $second_file = $ARGV[$i+2];
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

sub compare_pecent {
    $first_data_n = $_[0];
    $second_data_n = $_[1];
    $shift_number = $_[2];

    if($first_data_n =~ /.*\((\d*)\%\)/) {
        $first_data = $1;
    } else {
        $first_data = "NA";
    }

    if($second_data_n =~ /.*\((\d*)\%\)/) {
        $second_data = $1;
        if($first_data ne "NA") {
            $data_compare = $first_data - $second_data;
            if($data_compare == 0) {
                $tmp_flag = " ";
            } elsif ($data_compare > 0) {
                $tmp_flag = "+";
            } elsif ($data_compare < 0) {
                $tmp_flag = "-";
            }
            $data_compare = str_shift_to_right(abs($data_compare),2);
            $data_compare = "$tmp_flag     $data_compare%";
        } else {
            $data_compare = "NA1";
        }    
    } else {
        $second_data = "NA";
        if($first_data ne "NA") {
            $data_compare = "NA2";
        } else {
            $data_compare = "NA1/NA2";
        }
    }
    $data_compare = str_shift_to_right($data_compare,$shift_number);
    return $data_compare;
}

sub compare_cycles{
    $first_cycles_n = $_[0];
    $second_cycles_n = $_[1];
    $shift_number = $_[2];
    #printf "$first_cycles_n and $second_cycles_n\n";

    if($first_cycles_n =~ /\s*(.*)/) {
        $first_cycles = $1;
    } else {
        $first_cycles = "NG";
    }
    if($first_cycles eq "") {
        $cycles_compare = str_shift_to_right("?",11);
        return $cycles_compare;
    }

    if($second_cycles_n =~ /\s*(\d.*\d)/) {
        $second_cycles = $1;
	if($second_cycles eq "") {
    	    $cycles_compare = str_shift_to_right("?",11);
    	    return $cycles_compare;
	}
        if($first_cycles ne "NG") {
            $cycles_compare = ($first_cycles - $second_cycles) / $second_cycles * 100;
            $cycles_compare1 = $cycles_compare;
            $cycles_compare = sprintf("%.2f",abs($cycles_compare));
            if($cycles_compare1 == 0) {
                $tmp_flag = " ";
            } elsif ($cycles_compare1 > 0) {
                $tmp_flag = "+";
            } elsif ($cycles_compare1 < 0) {
                $tmp_flag = "-";
            }
            $cycles_compare = str_shift_to_right($cycles_compare,6);
            $cycles_compare = "$tmp_flag   $cycles_compare%";
        } else {
            $cycles_compare = "NA1";
        }    
    } else {
        $second_cycles = "NA";
        if($first_cycles ne "NG") {
            $cycles_compare = "NA2";
        } else {
            $cycles_compare = "NA1/NA2";
        }
    }
    $cycles_compare = str_shift_to_right($cycles_compare,11);
    return $cycles_compare;
}

sub compare_result {
    $first_res_n = $_[0];
    $second_res_n = $_[1];
    $shift_number = $_[2];

    if($first_res_n =~ /\s*(.*)/) {
        $first_res = $1;
    } else {
        $first_res = "NA";
    }

    if($second_res_n =~ /\s*(.*)/) {
        $second_res = $1;
        if($first_res eq "Pass")    { $first_res_s = "Pass";}
        elsif($first_res eq "Fail") { $first_res_s = "Fail";}
        else                        { $first_res_s = " NA1";}

        if($second_res eq "Pass")    { $second_res_s = "Pass";}
        elsif($second_res eq "Fail") { $second_res_s = "Fail";}
        else                         { $second_res_s = " NA2";}

        $res_compare = "$first_res_s/$second_res_s";
    } else {
        $second_res = "NA";
    }
    $res_compare = str_shift_to_right($res_compare,$shift_number);
    return $res_compare;
}

$compare_result_file = "compare_result.log";
open(my $new_report, '>', $compare_result_file);
$first_info = `cat $first_file`;
$second_info = `cat $second_file`;
printf "Compare user  : ";
system "whoami";
printf "Compare time  : ";
system "date";
printf "First report  : $first_file\n";
printf "Second report : $second_file\n";
$tmp = `whoami`;
printf $new_report "Compare user  : $tmp";
$tmp = `date`;
printf $new_report "Compare time  : $tmp";
$tmp = "\n";
printf $tmp;
printf $new_report $tmp;
printf $new_report "First report  : $first_file\n";
$tmp = `cat $first_file`;
printf $tmp;
printf $new_report $tmp;
$tmp = "\n";
printf $tmp;
printf $new_report $tmp;
printf $new_report "Second report : $second_file\n";
$tmp = `cat $second_file`;
printf $tmp;
printf $new_report $tmp;

$tmp = "\n";
printf $tmp;
printf $new_report $tmp;
$tmp = "Compare result :\n";
printf $tmp;
printf $new_report $tmp;
$state_flag = "";
@only_one = "";
$count_only_one = 0;
if($mode eq "one_hour_test") {
    if($first_info =~ /One hour test.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
        printf "$1\n";
        printf "$2\n";
        printf "$3\n";
        printf $new_report "$1\n";
        printf $new_report "$2\n";
        printf $new_report "$3\n";
        @first_set=split(/\n/,$4);
        foreach $one_first (@first_set) {
       	    if($one_first =~ /\|(.*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                $state_flag     = "";
                #$first_name     = $1;
		if($1 eq "") {
                    $first_name = $first_name;
		} else {
                    $first_name = $1;
		}
                $first_kernel_n = $2;
                $first_bram_n   = $3;
                $first_dsp_n    = $4;
                $first_ff_n     = $5;
                $first_lut_n    = $6;
                $first_cycles_n = $7;
                $first_res_n    = $8;
                if($first_kernel_n =~ /\s*(.*)/) {
                    $first_kernel = $1;
                }
                if($second_info =~ /One hour test.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
                    @second_set=split(/\n/,$4);
                    foreach $one_second (@second_set) {
           	            if($one_second =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                            $second_name_n   = $1;
                            $second_kernel_n = $2;
                            $second_bram_n   = $3;
                            $second_dsp_n    = $4;
                            $second_ff_n     = $5;
                            $second_lut_n    = $6;
                            $second_cycles_n = $7;
                            $second_res_n    = $8;
                            if($second_name_n =~ /(.*)\s*/) {
				if($1 eq "") {
                                    $second_name = $second_name;
				} else {
                                    $second_name = $1;
				}
                            }
                            if($second_kernel_n =~ /\s*(.*)/) {
                                $second_kernel = $1;
                            }
                            if($second_kernel eq $first_kernel and $second_name eq $first_name) {
                                $state_flag = "both_have";
				#printf "$second_kernel and $first_kernel and $second_name and $first_name\n";
    			        $bram_compare    = compare_pecent($first_bram_n, $second_bram_n, 13);
    			        $dsp_compare     = compare_pecent($first_dsp_n, $second_dsp_n, 13);
    			        $ff_compare      = compare_pecent($first_ff_n, $second_ff_n, 15);
    			        $lut_compare     = compare_pecent($first_lut_n, $second_lut_n, 15);
    			        $cycles_compare  = compare_cycles($first_cycles_n, $second_cycles_n, 11);
    			        $res_compare     = compare_result($first_res_n, $second_res_n, 11);
    
                                printf "|$second_name_n|$second_kernel_n|$bram_compare|$dsp_compare|$ff_compare|$lut_compare|$cycles_compare|$res_compare|\n";
                                printf $new_report "|$second_name_n|$second_kernel_n|$bram_compare|$dsp_compare|$ff_compare|$lut_compare|$cycles_compare|$res_compare|\n";
                            }
                        }
                    }
                }
                if($state_flag eq "") {
                    $state_flag = "only_1st_have";
                    $only_one[$count_only_one] = "|$first_name|$first_kernel_n|Only the first file has this kernel.                                               |\n";
                    $count_only_one = $count_only_one + 1;
                }
            }
        }
        $last_line = $5;
    }

    $state_flag = "";
    if($second_info =~ /One hour test.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
        @second_set=split(/\n/,$4);
        foreach $one_second (@second_set) {
            if($one_second =~ /\|(.*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                $second_name   = $1;
                $second_kernel_n = $2;
                if($second_kernel_n =~ /\s*(.*)/) {
                    $second_kernel = $1;
                }
                $state_flag = "";
                if($first_info =~ /One hour test.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
                    @first_set=split(/\n/,$4);
                    foreach $one_first (@first_set) {
           	            if($one_first =~ /\|(.*)\s*\|\s*(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                            $first_name = $1;
                            $first_kernel = $2;
                            if($second_kernel eq $first_kernel) {
                                $state_flag = "both_have";
                            }
                        }
                    }
                }
                if($state_flag eq "") {
                    $state_flag = "only_2st_have";
                    printf "|$second_name|$second_kernel_n|Only the second file has this kernel.                                              |\n";
                    printf $new_report "|$second_name|$second_kernel_n|Only the second file has this kernel.                                              |\n";
                }
            }
        }
    }
    for($i=0; $i<$count_only_one; $i++) {
        printf $only_one[$i];
        printf $new_report $only_one[$i];
    }
    printf "$last_line\n";
}
if($mode eq "hls") {
    if($first_info =~ /High Level Synthesis.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
        printf "$1\n";
        printf "$2\n";
        printf "$3\n";
        printf $new_report "$1\n";
        printf $new_report "$2\n";
        printf $new_report "$3\n";
        @first_set=split(/\n/,$4);
        foreach $one_first (@first_set) {
       	    if($one_first =~ /\|(.*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                $state_flag     = "";
                #$first_name     = $1;
		if($1 eq "") {
                    $first_name = $first_name;
		} else {
                    $first_name = $1;
		}
                $first_kernel_n = $2;
                $first_bram_n   = $3;
                $first_dsp_n    = $4;
                $first_ff_n     = $5;
                $first_lut_n    = $6;
                $first_cycles_n = $7;
                if($first_kernel_n =~ /\s*(.*)/) {
                    $first_kernel = $1;
                }
                if($second_info =~ /High Level Synthesis.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
                    @second_set=split(/\n/,$4);
                    foreach $one_second (@second_set) {
           	        if($one_second =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                            $second_name_n   = $1;
                            $second_kernel_n = $2;
                            $second_bram_n   = $3;
                            $second_dsp_n    = $4;
                            $second_ff_n     = $5;
                            $second_lut_n    = $6;
                            $second_cycles_n = $7;
                            if($second_name_n =~ /(.*)\s*/) {
				if($1 eq "") {
                                    $second_name = $second_name;
				} else {
                                    $second_name = $1;
				}
                            }
                            if($second_kernel_n =~ /\s*(.*)/) {
                                $second_kernel = $1;
                            }
                            if($second_kernel eq $first_kernel and $second_name eq $first_name) {
                                $state_flag = "both_have";
    			        $bram_compare    = compare_pecent($first_bram_n, $second_bram_n, 11);
    			        $dsp_compare     = compare_pecent($first_dsp_n, $second_dsp_n, 11);
    			        $ff_compare      = compare_pecent($first_ff_n, $second_ff_n, 13);
    			        $lut_compare     = compare_pecent($first_lut_n, $second_lut_n, 13);
    			        $cycles_compare  = compare_cycles($first_cycles_n, $second_cycles_n, 11);
    
                                printf "|$second_name_n|$second_kernel_n|$bram_compare|$dsp_compare|$ff_compare|$lut_compare|$cycles_compare|\n";
                                printf $new_report "|$second_name_n|$second_kernel_n|$bram_compare|$dsp_compare|$ff_compare|$lut_compare|$cycles_compare|\n";

                            }
                        }
                    }
                }
                if($state_flag eq "") {
                    $state_flag = "only_1st_have";
                    $only_one[$count_only_one] = "|$first_name|$first_kernel_n|Only the first file has this kernel.                            |\n";
                    $count_only_one = $count_only_one + 1;
                }
            }
        }
        $last_line = $5;
    }

    $state_flag = "";
    if($second_info =~ /High Level Synthesis.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
        @second_set=split(/\n/,$4);
        foreach $one_second (@second_set) {
            if($one_second =~ /\|(.*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                $second_name   = $1;
                $second_kernel_n = $2;
                if($second_kernel_n =~ /\s*(.*)/) {
                    $second_kernel = $1;
                }
                $state_flag = "";
                if($first_info =~ /High Level Synthesis.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
                    @first_set=split(/\n/,$4);
                    foreach $one_first (@first_set) {
           	            if($one_first =~ /\|(.*)\s*\|\s*(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                            $first_name = $1;
                            $first_kernel = $2;
                            if($second_kernel eq $first_kernel) {
                                $state_flag = "both_have";
                            }
                        }
                    }
                }
                if($state_flag eq "") {
                    $state_flag = "only_2st_have";
                    printf "|$second_name|$second_kernel_n|Only the second file has this kernel.                          |\n";
                    printf $new_report "|$second_name|$second_kernel_n|Only the second file has this kernel.                          |\n";
                }
            }
        }
    }
    for($i=0; $i<$count_only_one; $i++) {
        printf $only_one[$i];
        printf $new_report $only_one[$i];
    }
    printf "$last_line\n";
}
printf "Note:\n";
printf "     * With '-' before the number, the first report use less resources or time compare with the second report.\n";
printf "     * NA1: the first report didn't generate the output.\n";
printf "     * NA2: the second report didn't generate the output.\n";
printf "     * NA1/NA2: Both the reports didn't generate the output.\n";
printf $new_report "$last_line\n";
printf $new_report "Note:\n";
printf $new_report "     * With '-' before the number, the first report use less resources or time compare with the second report.\n";
printf $new_report "     * NA1: the first report didn't generate the output.\n";
printf $new_report "     * NA2: the second report didn't generate the output.\n";
printf $new_report "     * NA1/NA2: Both the reports didn't generate the output.\n";
close $new_report;

$MARS_BUILD_DIR = $ENV{'MARS_BUILD_DIR'};
$svn_dir = "${MARS_BUILD_DIR}/../../";

if($first_info =~ /Mars Build Rev :(\S*)/) {
    $first_svn_version = $1;
#    printf "$1\n\n";    
}

if($second_info =~ /Mars Build Rev :(\S*)/) {
    $second_svn_version = $1;
#    printf "$1\n\n";    
}

#printf "cd $svn_dir; svn diff --summarize -r $first_svn_version:$second_svn_version > svn_diff_summarize_$first_svn_version:$second_svn_version.log\n";
#system "cd $svn_dir; svn diff --summarize -r $first_svn_version:$second_svn_version > svn_diff_summarize_$first_svn_version:$second_svn_version.log";
#system "mv $svn_dir/svn_diff_summarize_$first_svn_version:$second_svn_version.log .";
#printf "cd $svn_dir; svn diff -r $first_svn_version:$second_svn_version > svn_diff_detail_$first_svn_version:$second_svn_version.log\n";
#system "cd $svn_dir; svn diff -r $first_svn_version:$second_svn_version > svn_diff_detail_$first_svn_version:$second_svn_version.log";
#system "mv $svn_dir/svn_diff_detail_$first_svn_version:$second_svn_version.log .";
