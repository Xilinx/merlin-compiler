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
$first_file = $ARGV[0];
$second_file = $ARGV[1];
$output_file = $ARGV[2];
$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};

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

sub compare_pecent {
    $first_data = $_[0];
    $second_data = $_[1];
    $shift_number = $_[2];

    $first_data =~ s/ //g;
    $second_data =~ s/ //g;
    if($first_data eq 0) { $first_data = 0.0001};
    if($second_data eq 0) { $second_data = 0.0001};
    if($first_data =~ /NA/) { $first_data = "NA";}
    if($second_data =~ /NA/) { $second_data = "NA";}
    if($first_data =~ /NG/) { $first_data = "NG";}
    if($second_data =~ /NG/) { $second_data = "NG";}
    if($first_data =~ /RUN/) { $first_data = "RUN";}
    if($second_data =~ /RUN/) { $second_data = "RUN";}
    if($second_data =~ /\d+/) {
        if($first_data =~ /\d+/) {
            $data_compare = $second_data - $first_data;
            if($data_compare == 0) {
                $sign = " ";
            } elsif ($data_compare > 0) {
                $sign = "+";
            } elsif ($data_compare < 0) {
                $sign = "-";
            }
            $data_compare = str_shift_to_right(abs($data_compare),4);
            $data_compare = "$sign    $data_compare%";
        } else {
            $data_compare = "$first_data/$second_data";
        }    
    } else {
        $data_compare = "$first_data/$second_data";
    }
    $data_compare = str_shift_to_right($data_compare,$shift_number);
    return $data_compare;
}

sub compare_cycles{
    $first_data = $_[0];
    $second_data = $_[1];
    $shift_number = $_[2];

    if($first_data eq 0 and $second_data ne 0) {
        $data_compare = " + 100.00%";
    } elsif($first_data ne 0 and $second_data eq 0) {
        $data_compare = " - 100.00%";
    } else {
        if($first_data eq 0) { $first_data = 0.01};
        if($second_data eq 0) { $second_data = 0.01};
        if($first_data =~ /NA/) { $first_data = "NA";}
        if($second_data =~ /NA/) { $second_data = "NA";}
        if($first_data =~ /NG/) { $first_data = "NG";}
        if($second_data =~ /NG/) { $second_data = "NG";}
        if($first_data =~ /RUN/) { $first_data = "RUN";}
        if($second_data =~ /RUN/) { $second_data = "RUN";}
        if($second_data =~ /\d+/) {
            if($first_data =~ /\d+/) {
                $data_compare = ($second_data - $first_data) / $first_data * 100;
                if($data_compare == 0) {
                    $sign = " ";
                } elsif ($data_compare > 0) {
                    $sign = "+";
                } elsif ($data_compare < 0) {
                    $sign = "-";
                }
                $data_compare = sprintf("%.2f",abs($data_compare));
                $data_compare = str_shift_to_right($data_compare,7);
                $data_compare = "$sign $data_compare%";
            } else {
                $data_compare = "$first_data/$second_data";
            }    
        } else {
            $data_compare = "$first_data/$second_data";
        }
    }
    $data_compare = str_shift_to_right($data_compare,$shift_number);
    return $data_compare;
}

sub compare_result {
    $first_data   = $_[0];
    $second_data  = $_[1];
    $shift_number = $_[2];

    if($first_data =~ /NA/) { $first_data = "NA";}
    if($second_data =~ /NA/) { $second_data = "NA";}
    if($first_data =~ /NG/) { $first_data = "NG";}
    if($second_data =~ /NG/) { $second_data = "NG";}
    if($first_data =~ /RUN/) { $first_data = "RUN";}
    if($second_data =~ /RUN/) { $second_data = "RUN";}
    $data_compare = "$first_data/$second_data";
    $data_compare = str_shift_to_right($data_compare,$shift_number);
    return $data_compare;
}

$string_summary = "";
$compare_result_file = "result.log";
if($output_file =~ /\S+/) {
    $compare_result_file = $output_file;;
}
open(my $compare_report, '>', $compare_result_file);
$first_info  = `cat $first_file`;
$second_info = `cat $second_file`;

$name3 = "BRAM";
$name4 = "DSP";
$name5 = "FF";
$name6 = "LUT";
$name7 = "CYCLES";
$name8 = "CORRECT";
my $length_name     = 14;
my $length_ram      = 13;
my $length_dsp      = 13;
my $length_ff       = 15;
my $length_lut      = 15;
my $length_cycles   = 11;
my $length_correct  = 11;
my %first_data;
my $first_suite;
my $first_kernel;
my $first_ram;
my $first_dsp;
my $first_ff;
my $first_lut;
my $first_cycles;
my $first_correct;
@first_set=split(/\n/,$first_info);
foreach $one_first (@first_set) {
    if($one_first =~ /Frontend report : (.*)/) {
        $first_suite = $1;
    }
    if($one_first =~ /\|(.*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
        $first_kernel    = $1;
        $first_ram      = $3;
        $first_dsp      = $4;
        $first_ff       = $5;
        $first_lut      = $6;
        $first_cycles   = $7;
        $first_correct  = $8;
        if($first_ram     =~ /\((.*)%\)/) { $first_ram     = $1;}
        if($first_dsp     =~ /\((.*)%\)/) { $first_dsp     = $1;}
        if($first_ff      =~ /\((.*)%\)/) { $first_ff      = $1;}
        if($first_lut     =~ /\((.*)%\)/) { $first_lut     = $1;}
        if($first_cycles  =~ /\s*(\S*)\s*/) { $first_cycles  = $1;}
        if($first_correct =~ /(\S+)/) { $first_correct = $1;}
        $first_kernel  =~ s/ //g;
        $first_ram     =~ s/ //g;
        $first_dsp     =~ s/ //g;
        $first_ff      =~ s/ //g;
        $first_lut     =~ s/ //g;
        $first_cycles  =~ s/ //g;
        $first_correct =~ s/ //g;
        if($one_first =~ /BRAM.*DSP/) {
        } elsif($one_first =~ /\-\-\-/) {
        } else {
            $first_data{"${first_suite}_${first_kernel}_${name3}"} = $first_ram;
            $first_data{"${first_suite}_${first_kernel}_${name4}"} = $first_dsp;
            $first_data{"${first_suite}_${first_kernel}_${name5}"} = $first_ff;
            $first_data{"${first_suite}_${first_kernel}_${name6}"} = $first_lut;
            $first_data{"${first_suite}_${first_kernel}_${name7}"} = $first_cycles;
            $first_data{"${first_suite}_${first_kernel}_${name8}"} = $first_correct;
            $data = $first_data{"${first_suite}_${first_kernel}_BRAM"};
        }
    }
}

my $count;
my %compare_data;
my $compare_suite;
my $compare_kernel;
my $compare_ram;
my $compare_dsp;
my $compare_ff;
my $compare_lut;
my $compare_cycles;
my $compare_correct;
my %second_data;
my $second_suite;
my $second_kernel;
my $second_ram;
my $second_dsp;
my $second_ff;
my $second_lut;
my $second_cycles;
my $second_correct;
@second_set=split(/\n/,$second_info);
foreach $one_second (@second_set) {
    if($one_second =~ /Frontend report : (.*)/) {
        $second_suite = $1;
        $count = 0;
    }
    if($one_second =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
        $second_kernel     = $1;
        $second_kernel_org = $1;
        $length_name       = length $1;
        $blank             = " " x length $2;
        $second_ram        = $3;
        $second_dsp        = $4;
        $second_ff         = $5;
        $second_lut        = $6;
        $second_cycles     = $7;
        $second_correct    = $8;
        $second_kernel     =~ s/ //g;
        if($second_ram     =~ /\((.*)%\)/) { $second_ram     = $1;}
        if($second_dsp     =~ /\((.*)%\)/) { $second_dsp     = $1;}
        if($second_ff      =~ /\((.*)%\)/) { $second_ff      = $1;}
        if($second_lut     =~ /\((.*)%\)/) { $second_lut     = $1;}
        if($second_cycles  =~ /\s*(\S*)\s*/) { $second_cycles  = $1;}
        if($second_correct =~ /(\S+)/) { $second_correct = $1;}
        $second_ram     =~ s/ //g;
        $second_dsp     =~ s/ //g;
        $second_ff      =~ s/ //g;
        $second_lut     =~ s/ //g;
        $second_cycles  =~ s/ //g;
        $second_correct =~ s/ //g;
        if($one_second =~ /BRAM.*DSP/) {
            $string_summary .= "$one_second\n";
        } else {
            $second_data{"${second_suite}_${second_kernel}_${name3}"} = $second_ram;
            $second_data{"${second_suite}_${second_kernel}_${name4}"} = $second_dsp;
            $second_data{"${second_suite}_${second_kernel}_${name5}"} = $second_ff;
            $second_data{"${second_suite}_${second_kernel}_${name6}"} = $second_lut;
            $second_data{"${second_suite}_${second_kernel}_${name7}"} = $second_cycles;
            $second_data{"${second_suite}_${second_kernel}_${name8}"} = $second_correct;
            if($first_data{"${second_suite}_${second_kernel}_BRAM"} =~ /\S+/) {
                $compare_ram    = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name3}"},
                        $second_data{"${second_suite}_${second_kernel}_${name3}"}, $length_ram);
                $compare_dsp    = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name4}"},
                        $second_data{"${second_suite}_${second_kernel}_${name4}"}, $length_dsp);
                $compare_ff     = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name5}"},
                        $second_data{"${second_suite}_${second_kernel}_${name5}"}, $length_ff);
                $compare_lut    = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name6}"},
                        $second_data{"${second_suite}_${second_kernel}_${name6}"}, $length_lut);
                $compare_cycle  = compare_cycles($first_data{"${second_suite}_${second_kernel}_${name7}"},
                        $second_data{"${second_suite}_${second_kernel}_${name7}"}, $length_cycles);
                $compare_correct= compare_result($first_data{"${second_suite}_${second_kernel}_${name8}"},
                        $second_data{"${second_suite}_${second_kernel}_${name8}"}, $length_correct);
            } else {
                $compare_ram    = str_shift_to_right("NON/$second_ram",$length_ram);
                $compare_dsp    = str_shift_to_right("NON/$second_dsp",$length_dsp);
                $compare_ff     = str_shift_to_right("NON/$second_ff",$length_ff);
                $compare_lut    = str_shift_to_right("NON/$second_lut",$length_lut);
                $compare_cycle  = str_shift_to_right("NON/$second_cycles",$length_cycles);
                $compare_correct= str_shift_to_right("NON/$second_correct",$length_correct);
            }
            $string_summary .= "|$second_kernel_org|$blank|$compare_ram|$compare_dsp|$compare_ff|$compare_lut|$compare_cycle|$compare_correct|\n";
        }
    } else {
        $count = $count + 1;
        if($count eq 4) {
            @number = keys(%first_data);
            for($i=0; $i<@number; $i=$i+1) {
                if($number[$i] =~ /${second_suite}_(.*)_BRAM$/) {
                    $name        = $1;
                    $data_first  = $first_data{$number[$i]};
                    $data_second = $second_data{$number[$i]};
                    if($data_second  =~ /\S+/) {
                    } else {
                        $data1 = $first_data{"${second_suite}_${name}_${name3}"};
                        $data2 = $first_data{"${second_suite}_${name}_${name4}"};
                        $data3 = $first_data{"${second_suite}_${name}_${name5}"};
                        $data4 = $first_data{"${second_suite}_${name}_${name6}"};
                        $data5 = $first_data{"${second_suite}_${name}_${name7}"};
                        $data6 = $first_data{"${second_suite}_${name}_${name8}"};
                        $name  = str_shift_to_left($name, $length_name);
                        $compare_ram    = str_shift_to_right("$data1/NON",$length_ram);
                        $compare_dsp    = str_shift_to_right("$data2/NON",$length_dsp);
                        $compare_ff     = str_shift_to_right("$data3/NON",$length_ff);
                        $compare_lut    = str_shift_to_right("$data4/NON",$length_lut);
                        $compare_cycle  = str_shift_to_right("$data5/NON",$length_cycles);
                        $compare_correct= str_shift_to_right("$data6/NON",$length_correct);
                        $string_summary .= "|$name|$blank|$compare_ram|$compare_dsp|$compare_ff|$compare_lut|$compare_cycle|$compare_correct|\n";
                    }
                }
            } 
        }
        $string_summary .= "$one_second\n";
    }
}
printf $string_summary;
printf $compare_report $string_summary;

#write to excel
$input_file_name = $compare_result_file;
system "perl $MARS_TEST_HOME_GIT/script/report/write_to_excel.pl $input_file_name";

exit;
