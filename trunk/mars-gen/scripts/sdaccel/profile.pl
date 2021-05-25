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


# this script generate a report for xilinx on board test
my $file = $ARGV[0];
my $max_length = 20;

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

for(my $i=0; $i<20; $i++) {
    $count{$i} = 0;
}

# get enviroment
printf "\n";
my $flag = 0;
my $bound = "+";
my $count_global = 0;
my $file_info = `cat $file 2> /dev/null`;
my @file_set = split(/\n/, $file_info);
foreach my $one_line (@file_set) {
    if($flag eq 1) {
        if($one_line =~ /\S+/) {
            my @value_set = split(/,/,$one_line);
            my $line = "|";
            my $i = 0;
            foreach my $one_value (@value_set) {
                if($count{$i} ne 0) {
                    $max_length = $count{$i};
                } else {
                    $max_length = length($one_value);
                    $count{$i} = $max_length; 
                    $bound_unit = "-" x $max_length;
                    $bound .= "$bound_unit+";
                }
                $one_value = str_shift_to_right($one_value, $max_length, $max_length);
                $line .= "$one_value|";
                $i++;
            }
            if($count_global eq 0) {
                printf "$bound\n";
            }
            $count_global++;
            printf "$line\n";
            printf "$bound\n";
        }
    }
    if( $one_line =~ /^Kernel Execution/ or
        $one_line =~ /^Compute Unit Utilization/ or
        $one_line =~ /^Data Transfer: Host and Global Memory/ or
        $one_line =~ /^Data Transfer: Kernels and Global Memory/ or
        $one_line =~ /^Top Data Transfer: Kernels and Global Memory/ or
        $one_line =~ /^Top Kernel Execution/ or
        $one_line =~ /^Top Buffer Writes/ or
        $one_line =~ /^Top Buffer Reads/
            ) {
        $flag = 1;
        $bound = "+";
        $count_global = 0;
        printf "\n$one_line\n";
    }
    if($one_line !~ /\S+/) {
        $flag = 0;
        for(my $i=0; $i<20; $i++) {
            $count{$i} = 0;
        }
    }
}
printf "\n";
