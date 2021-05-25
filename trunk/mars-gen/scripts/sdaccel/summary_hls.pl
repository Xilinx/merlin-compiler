# (C) Copyright 2016-2021 Xilinx, Inc.
# All Rights Reserved.
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License. 


my $report_file = $ARGV[0];

printf "================================================================\n";
printf "== Vivado HLS Report\n";
printf "================================================================\n";
if (not -e $report_file) {
    exit;
}
my $file_info = `cat $report_file`;
my @file_line_set = split(/\n/, $file_info);
foreach my $file_line (@file_line_set) {
    if($file_line =~ /(\* Date\:.*$)/) {
        printf "$1\n";
    }
    if($file_line =~ /(\* Version\:.*$)/) {
        printf "$1\n";
    }
    if($file_line =~ /(\* Project\:.*$)/) {
        printf "$1\n";
    }
    if($file_line =~ /(\* Product family\:.*$)/) {
        printf "$1\n";
    }
    if($file_line =~ /(\* Target device\:.*$)/) {
        printf "$1\n";
    }
}

my $flag_t = 0;
my $flag_l = 0;
printf "\n";
printf "================================================================\n";
printf "== Performance estimate\n";
printf "================================================================\n";
foreach my $file_line (@file_line_set) {
    if($file_line =~ /(^.*Latency \(clock cycles\)\:.*$)/) {
        $flag_t = 0;
        $flag_l = 1;
    }
    if($file_line =~ /(^.*Timing \(ns\)\:.*$)/) {
        $flag_t = 1;
    }
    if($file_line =~ /Detail\:/) {
        $flag_l = 0;
    }
    if($flag_t eq 1 or $flag_l eq 1) {
        printf "$file_line\n";
    }
}

my $flag_r = 0;
printf "\n";
printf "================================================================\n";
printf "== Utilization estimate\n";
printf "================================================================\n";
my $first_line_flag = 0;
my $first_line = "";
foreach my $file_line (@file_line_set) {
    if($file_line =~ /(\|\s*Name\s*\|\s*BRAM_18K\s*\|\s*DSP48E\s*\|\s*FF\s*\|\s*LUT\s*\|.*$)/) {
        $first_line_flag = 1;
    }
    if($first_line_flag eq 1 and $file_line =~ /--------/) {
        $first_line = $file_line;
        printf "$first_line\n";
        last;
    }
}
foreach my $file_line (@file_line_set) {
    if($file_line =~ /(\|\s*Name\s*\|\s*BRAM_18K\s*\|\s*DSP48E\s*\|\s*FF\s*\|\s*LUT\s*\|.*$)/) {
        $flag_r = 1;
        printf "$1\n";
        printf "$first_line\n";
    }
    if($flag_r eq 1 and $file_line =~ /(\|Total.*$)/) {
        printf "$1\n";
    }
    if($flag_r eq 1 and $file_line =~ /(\|Available.*$)/) {
        printf "$1\n";
    }
    if($flag_r eq 1 and $file_line =~ /(\|Utilization.*$)/) {
        printf "$1\n";
    }
    if($file_line =~ /Detail\:/) {
        $flag_r = 0;
    }
}
printf "$first_line\n";
printf "\n";
printf "\n";
