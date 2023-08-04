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
    if($first_data =~ /NG/) { $first_data = "NA";}
    if($second_data =~ /NG/) { $second_data = "NA";}
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

sub compare_time{
    $first_data = $_[0];
    $second_data = $_[1];
    $shift_number = $_[2];

    if($first_data eq 0) { $first_data = 0.000000001};
    if($second_data eq 0) { $second_data = 0.000000001};
    if($first_data !~ /\S+/) { $first_data = "NA";}
    if($second_data !~ /\S+/) { $second_data = "NA";}
    if($first_data =~ /NA/) { $first_data = "NA";}
    if($second_data =~ /NA/) { $second_data = "NA";}
    if($first_data =~ /NG/) { $first_data = "NA";}
    if($second_data =~ /NG/) { $second_data = "NA";}
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
    $data_compare = str_shift_to_right($data_compare,$shift_number);
    return $data_compare;
}

sub compare_cycles{
    $first_data = $_[0];
    $second_data = $_[1];
    $shift_number = $_[2];

    if($first_data eq 0) { $first_data = 0.01};
    if($second_data eq 0) { $second_data = 0.01};
    if($first_data =~ /NA/) { $first_data = "NA";}
    if($second_data =~ /NA/) { $second_data = "NA";}
    if($first_data =~ /NG/) { $first_data = "NA";}
    if($second_data =~ /NG/) { $second_data = "NA";}
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
    $data_compare = str_shift_to_right($data_compare,$shift_number);
    return $data_compare;
}

sub compare_result {
    $first_data   = $_[0];
    $second_data  = $_[1];
    $shift_number = $_[2];

    if($first_data =~ /NA/) { $first_data = "N";}
    if($second_data =~ /NA/) { $second_data = "N";}
    if($first_data =~ /NG/) { $first_data = "N";}
    if($second_data =~ /NG/) { $second_data = "N";}
    if($first_data =~ /RUN/) { $first_data = "R";}
    if($second_data =~ /RUN/) { $second_data = "R";}
    if($first_data =~ /Pass/) { $first_data = "P";}
    if($second_data =~ /Pass/) { $second_data = "P";}
    if($first_data =~ /Fail/) { $first_data = "F";}
    if($second_data =~ /Fail/) { $second_data = "F";}
    $data_compare = "$first_data/$second_data";
    $data_compare = str_shift_to_right($data_compare,$shift_number);
    return $data_compare;
}

$compare_result_file = "result.log";
if($output_file =~ /\S+/) {
    $compare_result_file = $output_file;;
}
open(my $compare_report, '>', $compare_result_file);
$first_info  = `cat $first_file`;
$second_info = `cat $second_file`;

my $name1   = "";
my $name2   = "CORRECT_C";
my $name3   = "CORRECT_OCL";
my $name4   = "CORRECT_FPGA";
my $name5   = "TIME_FPGA";
my $name6   = "TIME_TRAN";
my $name7   = "TIME_TOTAL";
my $name8   = "TIME_C";
my $name9   = "SPEEDUP";
my $name10  = "FREQ";
my $name11  = "BRAM";
my $name12  = "DSP";
my $name13  = "FF";
my $name14  = "LUT";
my $length  = 14;
my $length1 = 4;
my $length2 = 10;
my $length3 = 8;
my $length4 = 13;
my %first_data;
my $first_suite;
my $first_kernel;
my $first_correct_c;
my $first_correct_ocl;
my $first_correct_fpga;
my $first_time_fpga;
my $first_time_tran;
my $first_time_total;
my $first_time_c;
my $first_speedup;
my $first_freq;
my $first_ram;
my $first_dsp;
my $first_ff;
my $first_lut;
@first_set=split(/\n/,$first_info);
foreach $one_first (@first_set) {
    if($one_first =~ /Backend report : (.*)/) {
        $first_suite = $1;
    }
    if($one_first =~ /\|(.*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
        $first_kernel       = $1;
        $first_correct_c    = $2;
        $first_correct_ocl  = $3;
        $first_correct_fpga = $4;
        $first_time_fpga    = $5;
        $first_time_tran    = $6;
        $first_time_total   = $7;
        $first_time_c       = $8;
        $first_speedup      = $9;
        $first_freq         = $10;
        $first_ram          = $11;
        $first_dsp          = $12;
        $first_ff           = $13;
        $first_lut          = $14;
        $first_kernel          =~ s/ //g;
        $first_correct_c       =~ s/ //g;
        $first_correct_ocl     =~ s/ //g;
        $first_correct_fpga    =~ s/ //g;
        $first_time_fpga       =~ s/ //g;
        $first_time_tran       =~ s/ //g;
        $first_time_total      =~ s/ //g;
        $first_time_c          =~ s/ //g;
        $first_speedup         =~ s/ //g;
        $first_freq            =~ s/ //g;
        $first_ram             =~ s/ //g;
        $first_dsp             =~ s/ //g;
        $first_ff              =~ s/ //g;
        $first_lut             =~ s/ //g;
        if($first_correct_c !~ /\S+/)               {$first_correct_c = "N";}
        if($first_correct_c =~ /NG/)                {$first_correct_c = "N";}
        if($first_correct_c =~ /NA/)                {$first_correct_c = "N";}
        if($first_correct_c =~ /Pass/)              {$first_correct_c = "P";}
        if($first_correct_c =~ /Fail/)              {$first_correct_c = "F";}
        if($first_correct_ocl !~ /\S+/)             {$first_correct_ocl = "N";}
        if($first_correct_ocl =~ /NG/)              {$first_correct_ocl = "N";}
        if($first_correct_ocl =~ /NA/)              {$first_correct_ocl = "N";}
        if($first_correct_ocl =~ /Pass/)            {$first_correct_ocl = "P";}
        if($first_correct_ocl =~ /Fail/)            {$first_correct_ocl = "F";}
        if($first_correct_fpga !~ /\S+/)            {$first_correct_fpga = "N";}
        if($first_correct_fpga =~ /NG/)             {$first_correct_fpga = "N";}
        if($first_correct_fpga =~ /NA/)             {$first_correct_fpga = "N";}
        if($first_correct_fpga =~ /Pass/)           {$first_correct_fpga = "P";}
        if($first_correct_fpga =~ /Fail/)           {$first_correct_fpga = "F";}
        if($first_time_fpga     =~ /\s*(\S+)/)      { $first_time_fpga  = $1;}
        if($first_time_tran     =~ /\s*(\S+)/)      { $first_time_tran  = $1;}
        if($first_time_total    =~ /\s*(\S+)/)      { $first_time_total = $1;}
        if($first_time_c        =~ /\s*(\S+)/)      { $first_time_c     = $1;}
        if($first_speedup       =~ /\s*(\S+)/)      { $first_speedup    = $1;}
        if($first_freq          =~ /\s*(\S+)/)      { $first_freq       = $1;}
        if($first_ram           =~ /\((.*)%\)/)     { $first_ram        = $1;}
        if($first_dsp           =~ /\((.*)%\)/)     { $first_dsp        = $1;}
        if($first_ff            =~ /\((.*)%\)/)     { $first_ff         = $1;}
        if($first_lut           =~ /\((.*)%\)/)     { $first_lut        = $1;}
        if($first_time_fpga     =~ /N/)             { $first_time_fpga  = "N";}
        if($first_time_tran     =~ /N/)             { $first_time_tran  = "N";}
        if($first_time_total    =~ /N/)             { $first_time_total = "N";}
        if($first_time_c        =~ /N/)             { $first_time_c     = "N";}
        if($first_speedup       =~ /N/)             { $first_speedup    = "N";}
        if($first_freq          =~ /N/)             { $first_freq       = "N";}
        if($first_ram           =~ /N/)             { $first_ram        = "N";}
        if($first_dsp           =~ /N/)             { $first_dsp        = "N";}
        if($first_ff            =~ /N/)             { $first_ff         = "N";}
        if($first_lut           =~ /N/)             { $first_lut        = "N";}
        if($first_time_fpga     !~ /\S+/)           { $first_time_fpga  = "N";}
        if($first_time_tran     !~ /\S+/)           { $first_time_tran  = "N";}
        if($first_time_total    !~ /\S+/)           { $first_time_total = "N";}
        if($first_time_c        !~ /\S+/)           { $first_time_c     = "N";}
        if($first_speedup       !~ /\S+/)           { $first_speedup    = "N";}
        if($first_freq          !~ /\S+/)           { $first_freq       = "N";}
        if($first_ram           !~ /\S+/)           { $first_ram        = "N";}
        if($first_dsp           !~ /\S+/)           { $first_dsp        = "N";}
        if($first_ff            !~ /\S+/)           { $first_ff         = "N";}
        if($first_lut           !~ /\S+/)           { $first_lut        = "N";}
        if($first_freq          =~ /\s*(\S+)\..*/)  { $first_freq       = $1;}
        if($one_first =~ /BRAM.*DSP/) {
        } elsif($one_first =~ /\-\-\-/) {
        } else {
            $first_data{"${first_suite}_${first_kernel}_${name2}"}   = $first_correct_c;
            $first_data{"${first_suite}_${first_kernel}_${name3}"}   = $first_correct_ocl;
            $first_data{"${first_suite}_${first_kernel}_${name4}"}   = $first_correct_fpga;
            $first_data{"${first_suite}_${first_kernel}_${name5}"}   = $first_time_fpga;
            $first_data{"${first_suite}_${first_kernel}_${name6}"}   = $first_time_tran;
            $first_data{"${first_suite}_${first_kernel}_${name7}"}   = $first_time_total;
            $first_data{"${first_suite}_${first_kernel}_${name8}"}   = $first_time_c;
            $first_data{"${first_suite}_${first_kernel}_${name9}"}   = $first_speedup;
            $first_data{"${first_suite}_${first_kernel}_${name10}"}  = $first_freq;
            $first_data{"${first_suite}_${first_kernel}_${name11}"}  = $first_ram;
            $first_data{"${first_suite}_${first_kernel}_${name12}"}  = $first_dsp;
            $first_data{"${first_suite}_${first_kernel}_${name13}"}  = $first_ff;
            $first_data{"${first_suite}_${first_kernel}_${name14}"}  = $first_lut;
        }
    }
}

my $count;
my $compare_correct_c;
my $compare_correct_ocl;
my $compare_correct_fpga;
my $compare_time_fpga;
my $compare_time_tran;
my $compare_time_total;
my $compare_time_c;
my $compare_speedup;
my $compare_freq;
my $compare_ram;
my $compare_dsp;
my $compare_ff;
my $compare_lut;

my $second_correct_c;
my $second_correct_ocl;
my $second_correct_fpga;
my $second_time_fpga;
my $second_time_tran;
my $second_time_total;
my $second_time_c;
my $second_speedup;
my $second_freq;
my $second_ram;
my $second_dsp;
my $second_ff;
my $second_lut;

@second_set=split(/\n/,$second_info);
foreach $one_second (@second_set) {
    if($one_second =~ /Backend report : (.*)/) {
        $second_suite = $1;
        $count = 0;
    }
    if($one_second =~ /\|(.*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
        $second_kernel          = $1;
        $second_kernel_org      = $1;
        $length_name            = length $1;
        $second_correct_c       = $2;
        $second_correct_ocl     = $3;
        $second_correct_fpga    = $4;
        $second_time_fpga       = $5;
        $second_time_tran       = $6;
        $second_time_total      = $7;
        $second_time_c          = $8;
        $second_speedup         = $9;
        $second_freq            = $10;
        $second_ram             = $11;
        $second_dsp             = $12;
        $second_ff              = $13;
        $second_lut             = $14;
        $second_kernel          =~ s/ //g;
        $second_correct_c       =~ s/ //g;
        $second_correct_ocl     =~ s/ //g;
        $second_correct_fpga    =~ s/ //g;
        $second_time_fpga       =~ s/ //g;
        $second_time_tran       =~ s/ //g;
        $second_time_total      =~ s/ //g;
        $second_time_c          =~ s/ //g;
        $second_speedup         =~ s/ //g;
        $second_freq            =~ s/ //g;
        $second_ram             =~ s/ //g;
        $second_dsp             =~ s/ //g;
        $second_ff              =~ s/ //g;
        $second_lut             =~ s/ //g;
        if($second_correct_c !~ /\S+/)              {$second_correct_c = "N";}
        if($second_correct_c =~ /NG/)               {$second_correct_c = "N";}
        if($second_correct_c =~ /NA/)               {$second_correct_c = "N";}
        if($second_correct_c =~ /Pass/)             {$second_correct_c = "P";}
        if($second_correct_c =~ /Fail/)             {$second_correct_c = "F";}
        if($second_correct_ocl !~ /\S+/)            {$second_correct_ocl = "N";}
        if($second_correct_ocl =~ /NG/)             {$second_correct_ocl = "N";}
        if($second_correct_ocl =~ /NA/)             {$second_correct_ocl = "N";}
        if($second_correct_ocl =~ /Pass/)           {$second_correct_ocl = "P";}
        if($second_correct_ocl =~ /Fail/)           {$second_correct_ocl = "F";}
        if($second_correct_fpga !~ /\S+/)           {$second_correct_fpga = "N";}
        if($second_correct_fpga =~ /NG/)            {$second_correct_fpga = "N";}
        if($second_correct_fpga =~ /NA/)            {$second_correct_fpga = "N";}
        if($second_correct_fpga =~ /Pass/)          {$second_correct_fpga = "P";}
        if($second_correct_fpga =~ /Fail/)          {$second_correct_fpga = "F";}
        if($second_time_fpga    =~ /\s*(\S+)/)      { $second_time_fpga  = $1;}
        if($second_time_tran    =~ /\s*(\S+)/)      { $second_time_tran  = $1;}
        if($second_time_total   =~ /\s*(\S+)/)      { $second_time_total = $1;}
        if($second_time_c       =~ /\s*(\S+)/)      { $second_time_c     = $1;}
        if($second_speedup      =~ /\s*(\S+)/)      { $second_speedup    = $1;}
        if($second_freq         =~ /\s*(\S+)/)      { $second_freq       = $1;}
        if($second_ram          =~ /\((.*)%\)/)     { $second_ram        = $1;}
        if($second_dsp          =~ /\((.*)%\)/)     { $second_dsp        = $1;}
        if($second_ff           =~ /\((.*)%\)/)     { $second_ff         = $1;}
        if($second_lut          =~ /\((.*)%\)/)     { $second_lut        = $1;}
        if($second_time_fpga    =~ /N/)             { $second_time_fpga  = "N";}
        if($second_time_tran    =~ /N/)             { $second_time_tran  = "N";}
        if($second_time_total   =~ /N/)             { $second_time_total = "N";}
        if($second_time_c       =~ /N/)             { $second_time_c     = "N";}
        if($second_speedup      =~ /N/)             { $second_speedup    = "N";}
        if($second_freq         =~ /N/)             { $second_freq       = "N";}
        if($second_ram          =~ /N/)             { $second_ram        = "N";}
        if($second_dsp          =~ /N/)             { $second_dsp        = "N";}
        if($second_ff           =~ /N/)             { $second_ff         = "N";}
        if($second_lut          =~ /N/)             { $second_lut        = "N";}
        if($second_time_fpga    !~ /\S+/)           { $second_time_fpga  = "N";}
        if($second_time_tran    !~ /\S+/)           { $second_time_tran  = "N";}
        if($second_time_total   !~ /\S+/)           { $second_time_total = "N";}
        if($second_time_c       !~ /\S+/)           { $second_time_c     = "N";}
        if($second_speedup      !~ /\S+/)           { $second_speedup    = "N";}
        if($second_freq         !~ /\S+/)           { $second_freq       = "N";}
        if($second_ram          !~ /\S+/)           { $second_ram        = "N";}
        if($second_dsp          !~ /\S+/)           { $second_dsp        = "N";}
        if($second_ff           !~ /\S+/)           { $second_ff         = "N";}
        if($second_lut          !~ /\S+/)           { $second_lut        = "N";}
        if($second_freq         =~ /\s*(\S+)\..*/)  { $second_freq       = $1;}
        if($one_second =~ /BRAM.*DSP/) {
            $string_summary .= "$one_second\n";
        } else {
            $second_data{"${second_suite}_${second_kernel}_${name2}"}   = $second_correct_c;
            $second_data{"${second_suite}_${second_kernel}_${name3}"}   = $second_correct_ocl;
            $second_data{"${second_suite}_${second_kernel}_${name4}"}   = $second_correct_fpga;
            $second_data{"${second_suite}_${second_kernel}_${name5}"}   = $second_time_fpga;
            $second_data{"${second_suite}_${second_kernel}_${name6}"}   = $second_time_tran;
            $second_data{"${second_suite}_${second_kernel}_${name7}"}   = $second_time_total;
            $second_data{"${second_suite}_${second_kernel}_${name8}"}   = $second_time_c;
            $second_data{"${second_suite}_${second_kernel}_${name9}"}   = $second_speedup;
            $second_data{"${second_suite}_${second_kernel}_${name10}"}  = $second_freq;
            $second_data{"${second_suite}_${second_kernel}_${name11}"}  = $second_ram;
            $second_data{"${second_suite}_${second_kernel}_${name12}"}  = $second_dsp;
            $second_data{"${second_suite}_${second_kernel}_${name13}"}  = $second_ff;
            $second_data{"${second_suite}_${second_kernel}_${name14}"}  = $second_lut;
            if($first_data{"${second_suite}_${second_kernel}_BRAM"} =~ /\S+/) {
                $compare_correct_c    = compare_result($first_data{"${second_suite}_${second_kernel}_${name2}"},
                        $second_data{"${second_suite}_${second_kernel}_${name2}"}, $length1);
                $compare_correct_ocl  = compare_result($first_data{"${second_suite}_${second_kernel}_${name3}"},
                        $second_data{"${second_suite}_${second_kernel}_${name3}"}, $length1);
                $compare_correct_fpga = compare_result($first_data{"${second_suite}_${second_kernel}_${name4}"},
                        $second_data{"${second_suite}_${second_kernel}_${name4}"}, $length1);
                $compare_time_fpga    = compare_time($first_data{"${second_suite}_${second_kernel}_${name5}"},
                        $second_data{"${second_suite}_${second_kernel}_${name5}"}, $length2);
                $compare_time_tran    = compare_time($first_data{"${second_suite}_${second_kernel}_${name6}"},
                        $second_data{"${second_suite}_${second_kernel}_${name6}"}, $length2);
                $compare_time_total   = compare_time($first_data{"${second_suite}_${second_kernel}_${name7}"},
                        $second_data{"${second_suite}_${second_kernel}_${name7}"}, $length2);
                $compare_time_c       = compare_time($first_data{"${second_suite}_${second_kernel}_${name8}"},
                        $second_data{"${second_suite}_${second_kernel}_${name8}"}, $length2);
                $compare_speedup      = compare_time($first_data{"${second_suite}_${second_kernel}_${name9}"},
                        $second_data{"${second_suite}_${second_kernel}_${name9}"}, $length2);
                $compare_freq         = compare_result($first_data{"${second_suite}_${second_kernel}_${name10}"},
                        $second_data{"${second_suite}_${second_kernel}_${name10}"}, $length3);
                $compare_ram          = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name11}"},
                        $second_data{"${second_suite}_${second_kernel}_${name11}"}, $length4);
                $compare_dsp          = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name12}"},
                        $second_data{"${second_suite}_${second_kernel}_${name12}"}, $length4);
                $compare_ff           = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name13}"},
                        $second_data{"${second_suite}_${second_kernel}_${name13}"}, $length4);
                $compare_lut          = compare_pecent($first_data{"${second_suite}_${second_kernel}_${name14}"},
                        $second_data{"${second_suite}_${second_kernel}_${name14}"}, $length4);
            } else {
                $compare_correct_c    = str_shift_to_right("N/$second_correct_c"      ,$length1);
                $compare_correct_ocl  = str_shift_to_right("N/$second_correct_ocl"    ,$length1);
                $compare_correct_fpga = str_shift_to_right("N/$second_correct_fpga"   ,$length1);
                $compare_time_fpga    = str_shift_to_right("N/$second_time_fpga"      ,$length2);
                $compare_time_tran    = str_shift_to_right("N/$second_time_tran"      ,$length2);
                $compare_time_total   = str_shift_to_right("N/$second_time_total"     ,$length2);
                $compare_time_c       = str_shift_to_right("N/$second_time_c"         ,$length2);
                $compare_speedup      = str_shift_to_right("N/$second_speedup"        ,$length2);
                $compare_freq         = str_shift_to_right("N/$second_freq"           ,$length3);
                $compare_ram          = str_shift_to_right("N/$second_ram%"           ,$length4);
                $compare_dsp          = str_shift_to_right("N/$second_dsp%"           ,$length4);
                $compare_ff           = str_shift_to_right("N/$second_ff%"            ,$length4);
                $compare_lut          = str_shift_to_right("N/$second_lut%"           ,$length4);
            }
            $string_summary .= "|$second_kernel_org|$compare_correct_c|$compare_correct_ocl|$compare_correct_fpga|$compare_time_fpga|$compare_time_tran|$compare_time_total|$compare_time_c|$compare_speedup|$compare_freq|$compare_ram|$compare_dsp|$compare_ff|$compare_lut|\n";
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
                        $data2  = $first_data{"${second_suite}_${name}_${name2}"};
                        $data3  = $first_data{"${second_suite}_${name}_${name3}"};
                        $data4  = $first_data{"${second_suite}_${name}_${name4}"};
                        $data5  = $first_data{"${second_suite}_${name}_${name5}"};
                        $data6  = $first_data{"${second_suite}_${name}_${name6}"};
                        $data7  = $first_data{"${second_suite}_${name}_${name7}"};
                        $data8  = $first_data{"${second_suite}_${name}_${name8}"};
                        $data9  = $first_data{"${second_suite}_${name}_${name9}"};
                        $data10 = $first_data{"${second_suite}_${name}_${name10}"};
                        $data11 = $first_data{"${second_suite}_${name}_${name11}"};
                        $data12 = $first_data{"${second_suite}_${name}_${name12}"};
                        $data13 = $first_data{"${second_suite}_${name}_${name13}"};
                        $data14 = $first_data{"${second_suite}_${name}_${name14}"};
                        $name   = str_shift_to_left($name, $length_name);
                        $compare_correct_c    = str_shift_to_right("$data2/N",$length1);
                        $compare_correct_ocl  = str_shift_to_right("$data3/N",$length1);
                        $compare_correct_fpga = str_shift_to_right("$data4/N",$length1);
                        $compare_time_fpga    = str_shift_to_right("$data5/N",$length2);
                        $compare_time_tran    = str_shift_to_right("$data6/N",$length2);
                        $compare_time_total   = str_shift_to_right("$data7/N",$length2);
                        $compare_time_c       = str_shift_to_right("$data8/N",$length2);
                        $compare_speedup      = str_shift_to_right("$data9/N",$length2);
                        $compare_freq         = str_shift_to_right("$data10/N",$length3);
                        $compare_ram          = str_shift_to_right("$data11%/N",$length4);
                        $compare_dsp          = str_shift_to_right("$data12%/N",$length4);
                        $compare_ff           = str_shift_to_right("$data13%/N",$length4);
                        $compare_lut          = str_shift_to_right("$data14%/N",$length4);
                        $string_summary .= "|$second_kernel_org|$compare_correct_c|$compare_correct_ocl|$compare_correct_fpga|$compare_time_fpga|$compare_time_tran|$compare_time_total|$compare_time_c|$compare_speedup|$compare_freq|$compare_ram|$compare_dsp|$compare_ff|$compare_lut|\n";
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
