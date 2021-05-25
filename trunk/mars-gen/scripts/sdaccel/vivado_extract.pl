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



my $vivado_report = $ARGV[0];
my $vivado_info       = `cat $vivado_report`;
my @vivado_line_set   = split(/\n/, $vivado_info);
printf "\n";
my $error_flag = 0;
foreach my $vivado_line (@vivado_line_set) {
    if($vivado_line =~ /(^.*Error:.*)$/) {
        $error_flag = 1;
    }
    if($vivado_line =~ /(^.*ERROR:.*)$/) {
        $error_flag = 1;
    }
    if($vivado_line =~ /(^.*error:.*)$/) {
        $error_flag = 1;
    }
    if($vivado_report =~ /vivado_hls.log/) {
        if($error_flag eq 1) {
            if($vivado_line =~ /^Pragma processor failed:/) {
            } elsif($vivado_line =~ /^INFO:.*$/) {
            } elsif($vivado_line =~ /while executing/) {
            } elsif($vivado_line =~ /source\s*\[.*\]/) {
            } elsif($vivado_line =~ /uplevel.*body line/) {
            } elsif($vivado_line =~ /invoked from within/) {
            } elsif($vivado_line =~ /uplevel.*\{.*\}/) {
            } elsif($vivado_line =~ /^Compilation of the preprocessed source.*$/) {
            } elsif($vivado_line =~ /^.*(__merlinkernel_.*: error: .*)$/) {
                printf "$1\n";
            } elsif($vivado_line =~ /^.*(__merlinkernel_.*: note: .*)$/) {
                printf "$1\n";
            } elsif($vivado_line =~ /^ERROR: \[.*\] \/.*(__merlinkernel_.*: .*)$/) {
                printf "$1\n";
            } else {
                printf "$vivado_line\n";
            }
        }
    }
    if($vivado_report =~ /vivado.log/) {
        if($vivado_line =~ /(^\s*ERROR: \[.*\].*)\[.*\]$/) {
            printf "$1\n";
        } elsif($vivado_line =~ /(^\s*Error: \[.*\].*)\[.*\]$/) {
            printf "$1\n";
        } elsif($vivado_line =~ /(^\s*error: \[.*\].*)\[.*\]$/) {
            printf "$1\n";
        }elsif($vivado_line =~ /(^\s*Error:.*)$/) {
            printf "$1\n";
        } elsif($vivado_line =~ /(^\s*ERROR:.*)$/) {
            printf "$1\n";
        } elsif($vivado_line =~ /(^\s*error:.*)$/) {
            printf "$1\n";
        } elsif($vivado_line =~ /(^\s*CRITICAL WARNING: \[.*\].*)\[.*\]$/) {
            printf "$1\n";
        } elsif($vivado_line =~ /(^\s*CRITICAL WARNING:.*)$/) {
            printf "$1\n";
        }
    }
}
