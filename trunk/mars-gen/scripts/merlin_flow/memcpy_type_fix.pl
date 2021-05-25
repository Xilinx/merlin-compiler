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


# 1. this script is called at the same directory with the source file
# 2. This file is called in frontend_pass.pl, do not suggest to use directly.

# when flag is on, string.h is replaced with my_string.h, which is used for tldm_task_extract
# when flag is off, my_string.h is replaced back to string.h for hls and sw compilation.
my $flag = $ARGV[0];
$src_top = $ENV{'ESLOPT_HOME'};
$mars_build_dir = $ENV{'MARS_BUILD_DIR'};

if ($flag eq "on") { 
    $work_dir = '.';
    $source_list = `cd $work_dir; ls *.c *.cpp *.C *.cxx *.c++ *.cc 2>/dev/null`;
    chomp($source_list);
    @source_set = split(/\n/, $source_list);
    foreach $file (@source_set) {
    #print "file $file \n";
        $grep_out  = `cd $work_dir; grep cmost_malloc $file`;
        $grep_out .= `cd $work_dir; grep cmost_load  $file`;
        $grep_out .= `cd $work_dir; grep cmost_write $file`;
        $grep_out .= `cd $work_dir; grep cmost_dump  $file`;
        $grep_out .= `cd $work_dir; grep cmost_free  $file`;
        if ($grep_out =~ 'cmost_') {
            $grep_out = `cd $work_dir; grep 'cmost.h' $file`;
            if ($grep_out !~ 'cmost\.h') {
                system("cd $work_dir; sed -i '1i\\#include \"cmost.h\"' $file");
            }
        }
    }

    if (-e "cmost.h") { system "mv cmost.h cmost_bak.h"; }
    if (-e "$mars_build_dir/scripts/merlin_flow/cmost.h") {
        system "cp $mars_build_dir/scripts/merlin_flow/cmost.h . 2>/dev/null"; 
    }
} elsif ($flag eq "off") { 
    if (-e "cmost_bak.h") { system "mv cmost_bak.h cmost.h"; _}
}

