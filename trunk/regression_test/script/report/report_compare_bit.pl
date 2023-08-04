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

for($i=0; $i<@ARGV; $i=$i+1) {
    if($ARGV[$i] eq "-b") {
        $mode = "bit";
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

sub compare_float {
    $first_data  = $_[0];
    $second_data = $_[1];
    if($first_data =~ /\s*(.*)/) {
        $first_data_n = $1;
    }
    if($first_data_n eq "") {
        $data_compare = str_shift_to_right("?",11);
        return $data_compare;
    }
    if($second_data =~ /\s*(\d+.\d+)/) {
        $second_data_n = $1;
	if($second_data_n eq "") {
    	    $data_compare = str_shift_to_right("?",11);
    	    return $data_compare;
	}
        if($first_data_n ne "NG" and $first_data_n ne "") {
            if($second_data_n eq "0.00" or $second_data_n eq "0.000" or $second_data_n eq "0.000000") {
                $devide_data = "0.01";
            } else {
                $devide_data = $second_data_n;
            }
            $data_compare = ($first_data_n - $second_data_n)/$devide_data*100;
            $data_compare = sprintf("%.2f",$data_compare);
            if($data_compare == "0.00") {
                $tmp_flag = " ";
            } elsif ($data_compare =~ /^\-/) {
                $tmp_flag = "-";
            } else {
                $tmp_flag = "+";
            }
            $data_compare = str_shift_to_right(abs($data_compare),7);
            $data_compare = " $tmp_flag $data_compare%";
        } else {
            $data_compare = "NA1";
        }    
    } else {
        $second_data_n = "NA";
        if($first_data_n ne "NG" and $first_data_n ne "") {
            $data_compare = "NA2";
        } else {
            $data_compare = "NA1/NA2";
        }
    }
    $data_compare = str_shift_to_right($data_compare,11);
    return $data_compare;
}

sub compare_int {
    $first_data  = $_[0];
    $second_data = $_[1];
    if($first_data =~ /\s*(.*)/) {
        $first_data_n = $1;
    }
    if($first_data_n eq "") {
        $data_compare = str_shift_to_right("?",11);
        return $data_compare;
    }
    if($second_data =~ /\s*(\d+)/) {
        $second_data_n = $1;
	if($second_data_n eq "") {
    	    $data_compare = str_shift_to_right("?",11);
    	    return $data_compare;
	}
        if($first_data_n ne "NG") {
            if($second_data_n eq "0") {
                $devide_data = "1";
            } else {
                $devide_data = $second_data_n;
            }
            $data_compare = ($first_data_n - $second_data_n)/$devide_data*100;
            $data_compare = sprintf("%.2f",$data_compare);
            if($data_compare == "0.00") {
                $tmp_flag = " ";
            } elsif ($data_compare =~ /^\-/) {
                $tmp_flag = "-";
            } else {
                $tmp_flag = "+";
            }
            $data_compare = str_shift_to_right(abs($data_compare),7);
            $data_compare = " $tmp_flag $data_compare%";
        } else {
            $data_compare = "NA1";
        }    
    } else {
        $second_data_n = "NA";
        if($first_data_n ne "NG") {
            $data_compare = "NA2";
        } else {
            $data_compare = "NA1/NA2";
        }
    }
    $data_compare = str_shift_to_right($data_compare,11);
    return $data_compare;
}

sub compare_corr {
    $first_corr  = $_[0];
    $second_corr = $_[1];
    if($first_corr =~ /\s*(.*)/) {
        $first_corr_n = $1;
    }
    if($second_corr =~ /\s*(.*)/) {
        $second_corr_n = $1;
#        printf "$first_corr_n $second_corr_n\n";
        if($first_corr_n eq "Pass")     { $first_corr_s = "P";}
        elsif($first_corr_n eq "Fail")  { $first_corr_s = "F";}
        else                            { $first_corr_s = "N";}

        if($second_corr_n eq "Pass")    { $second_corr_s = "P";}
        elsif($second_corr_n eq "Fail") { $second_corr_s = "F";}
        else                            { $second_corr_s = "N";}

        if($second_name_n =~ /(\S+)\s*/) {
            $res_compare = " $first_corr_s/$second_corr_s";
        } else {
            $res_compare = " ";
        }
    } else {
        $second_corr_n = "NA";
    }
    $res_compare = str_shift_to_right($res_compare,4);
    return $res_compare;
}

sub compare_slack {
    $first_slack  = $_[0];
    $second_slack = $_[1];
    if($first_slack =~ /\s*(.*)/) {
        $first_slack_n = $1;
    }
    if($second_slack =~ /\s*(.*)/) {
        $second_slack_n = $1;
#        printf "$first_corr_n $second_corr_n\n";
        if($first_slack_n =~ /\-.*/)    { $first_slack_s = "-";}
        else                            { $first_slack_s = "+";}

        if($second_slack_n =~ /\-.*/)   { $second_slack_s = "-";}
        else                            { $second_slack_s = "+";}

        if($second_name_n =~ /(\S+)\s*/) {
            $res_compare = " $first_slack_s / $second_slack_s ";
        } else {
            $res_compare = " ";
        }
    } else {
        $second_slack_n = "NA";
    }
    $res_compare = str_shift_to_right($res_compare,11);
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
if($first_info =~ /On board test performance.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
    printf "$1\n";
    printf "$2\n";
    printf "$3\n";
    printf $new_report "$1\n";
    printf $new_report "$2\n";
    printf $new_report "$3\n";
    @first_set=split(/\n/,$4);
    foreach $one_first (@first_set) {
       	if($one_first =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
            $state_flag     = "";
            $first_name_n   = $1;
            $first_c_res = $2;
            $first_o_res = $3;
            $first_f_res = $4;
            $first_fc_res = $5;
            $first_ft_res = $6;
            $first_fa_res = $7;
            $first_ca_res = $8;
            $first_sp_res = $9;
            $first_ct_res = $10;
            $first_sk_res = $11;
            if($first_name_n =~ /(.*)\s*/) {
                $first_name = $1;
            }
            if($second_info =~ /On board test performance.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
                @second_set=split(/\n/,$4);
                foreach $one_second (@second_set) {
       	            if($one_second =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                        $second_name_n   = $1;
                        $second_c_res = $2;
                        $second_o_res = $3;
                        $second_f_res = $4;
                        $second_fc_res = $5;
                        $second_ft_res = $6;
                        $second_fa_res = $7;
                        $second_ca_res = $8;
                        $second_sp_res = $9;
                        $second_ct_res = $10;
                        $second_sk_res = $11;
                        if($second_name_n =~ /(.*)\s*/) {
                            $second_name = $1;
                        }
                        if($second_name eq $first_name) {
                            $state_flag = "both_have";
                            $compare_c_res = compare_corr($first_c_res,$second_c_res);
                            $compare_o_res = compare_corr($first_o_res,$second_o_res);
                            $compare_f_res = compare_corr($first_f_res,$second_f_res);
                            $compare_fc_res = compare_float($first_fc_res,$second_fc_res);
                            $compare_ft_res = compare_float($first_ft_res,$second_ft_res);
                            $compare_fa_res = compare_float($first_fa_res,$second_fa_res);
                            $compare_ca_res = compare_float($first_ca_res,$second_ca_res);
                            $compare_sp_res = compare_float($first_sp_res,$second_sp_res);
                            $compare_ct_res = compare_int($first_ct_res,$second_ct_res);
                            $compare_sk_res = compare_slack($first_sk_res,$second_sk_res);
                            printf "|$second_name_n|$compare_c_res|$compare_o_res|$compare_f_res|$compare_fc_res|$compare_ft_res|$compare_fa_res|$compare_ca_res|$compare_sp_res|$compare_ct_res|$compare_sk_res|\n";
                            printf $new_report "|$second_name_n|$compare_c_res|$compare_o_res|$compare_f_res|$compare_fc_res|$compare_ft_res|$compare_fa_res|$compare_ca_res|$compare_sp_res|$compare_ct_res|$compare_sk_res|\n";
                        }
                    }
                }
            }
            if($state_flag eq "") {
                $state_flag = "only_1st_have";
		$blank = " " x 64;
                $only_one[$count_only_one] = "|$first_name_n|Only the first file has this case.$blank|\n";
                $count_only_one = $count_only_one + 1;
            }
        }
    }
#    printf "$5\n";
    $last_line = $5;
}

$state_flag = "";
if($second_info =~ /On board test performance.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
    @second_set=split(/\n/,$4);
    foreach $one_second (@second_set) {
       	if($one_second =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
            $second_name_n   = $1;
            if($second_name_n =~ /(\S*)\s*/) {
                $second_name   = $1;
            }
            $state_flag = "";
            if($first_info =~ /On board test performance.*\n(.*)\n(.*)\n(.*)\n([\S\s]*)\n(.*)\nNote:/) {
                @first_set=split(/\n/,$4);
                foreach $one_first (@first_set) {
       	            if($one_first =~ /\|(\S*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
                        $first_name = $1;
                        if($second_name eq $first_name) {
                            $state_flag = "both_have";
                        }
                    }
                }
            }
            if($state_flag eq "") {
                $state_flag = "only_2st_have";
		$blank = " " x 63;
                printf "|$second_name_n|Only the second file has this case.$blank|\n";
                printf $new_report "|$second_name_n|Only the second file has this case.$blank|\n";
            }
        }
    }
}
for($i=0; $i<$count_only_one; $i++) {
    printf $only_one[$i];
    printf $new_report $only_one[$i];
}
printf "$last_line\n";
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

#$MARS_BUILD_DIR = $ENV{'MARS_BUILD_DIR'};
#$svn_dir = "${MARS_BUILD_DIR}/../../";
#
#if($first_info =~ /Mars Build Rev :(\S*)/) {
#    $first_svn_version = $1;
##    printf "$1\n\n";    
#}
#
#if($second_info =~ /Mars Build Rev :(\S*)/) {
#    $second_svn_version = $1;
##    printf "$1\n\n";    
#}
#printf "cd $svn_dir; svn diff --summarize -r $first_svn_version:$second_svn_version > svn_diff_summarize.log\n";
#system "cd $svn_dir; svn diff --summarize -r $first_svn_version:$second_svn_version > svn_diff_summarize.log";
#system "cp $svn_dir/svn_diff_summarize.log .";
#printf "cd $svn_dir; svn diff -r $first_svn_version:$second_svn_version > svn_diff_detail.log\n";
#system "cd $svn_dir; svn diff -r $first_svn_version:$second_svn_version > svn_diff_detail.log";
#system "cp $svn_dir/svn_diff_detail.log .";
