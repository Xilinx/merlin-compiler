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



#---------------------------------------------------------------------------------------------#
# excute before everything
#---------------------------------------------------------------------------------------------#
my $MERLIN_COMPILER_HOME;
my $library_path;
BEGIN {
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'}; 
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use strict;
use warnings;
use lib "$library_path";
use message;
use File::Basename;

my $vendor_log = $ARGV[0];
my $count_line_all = 0;
my $read_line_number = 0;
my $error_flag = 0; 
#my $print_all = 0;  #if license check failed, print all logs

sub output_log {
    my $file_info = `cat $_[0]`;
    if($file_info !~ /\w+/) {
        return;
    }
    my $count_line = 0;
    #printf "$file_info\n\n\n";
    my @file_info_set = split(/\n/, $file_info);
    foreach my $one_line(@file_info_set) {
        my $detail_info;
        if($count_line >= $read_line_number) {
            if($one_line =~ /INFO: \[.*\]\s+(.*)/) {
                $detail_info = $1;
                if ($detail_info =~ /Target device:/ and not -e ".target_device") {
                    if($detail_info =~ /\: (.*)$/) {
                        MSG_I_1017($1);
                        system "touch .target_device";
                    }
                }
                if( $detail_info =~ /Creating kernel/) {
                    if($detail_info =~ /\: \'(.*)\'\s*$/) {
                        MSG_I_1018($1);
                        printf ".";
                    }

                }
                if ($detail_info =~ /Linking for hardware emulation target/) {
                    printf "\n";
                    MSG_I_1019();
                    printf ".";
                    printf "\n";
                    MSG_I_1020();
                    printf ".";
                }
                if ($detail_info =~ /Linking for software emulation target/) {
                    printf "\n";
                    MSG_I_1019();
                    printf ".";
                    printf "\n";
                    MSG_I_1021();
                    printf ".";
                }
                if ($detail_info =~ /Linking for hardware target/) {
                    printf "\n";
                    MSG_I_1019();
                    printf ".";
                    printf "\n";
                    MSG_I_1022();
                    printf ".";
                }
            } else {
                $detail_info = $one_line;
            }
            if($one_line =~ /ERROR: \[.*\]\s+(.*)/) {
                # this may call duplicate error print on screen
                # because we also extract error from vivado_hls.log and vivado.log
                # but this will not miss any error from xocc it self
                printf "$one_line\n";
            }
        }
        $count_line++;
    }
    if($read_line_number eq $count_line and not -e ".finish_cmd") {
        printf ".";
    }
    $read_line_number = $count_line;
#    printf("line number = $read_line_number\n");
}

sleep(3);
while(1) {
    if(-e ".finish_cmd") {
        output_log($vendor_log);
        system "rm .finish_cmd";
        exit; 
    }
    output_log($vendor_log);
    sleep(5);
}

system "rm .finish_cmd";
