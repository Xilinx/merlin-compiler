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
    our $xml_dir = "";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use strict;
use warnings;
use lib "$library_path";
use merlin;
use global_var;
use File::Basename;
$ENV{"PERL5SHELL"} = "tcsh";

my $pure_kernel_flow = "off";
if($xml_sycl eq "on" or $xml_pure_kernel eq "on") {
    $pure_kernel_flow = "on";
}

my $file_list = "";
if ($pure_kernel_flow ne "on") {
   $file_list = `ls __merlinhead_*.h`;
   if ($file_list eq "") {
       MSG_E_3001();
   }
}
add_extern($file_list);

if(-e "__merlintask_top.h") {
    $file_list = `ls __merlintask*.h`;
    add_extern($file_list);
}

sub add_extern {
    my ($file_list) = @_;
    my @file_set = split(/\n/, $file_list);
    foreach my $one_file (@file_set) {
        my $input_file = $one_file;
    
        my $info    = `cat $input_file`;
        my $string  ="";
        $string    .= "#ifdef __cplusplus\n";
        $string    .= "extern \"C\" {\n";
        $string    .= "#endif\n";
        $string    .= $info;
        $string    .= "#ifdef __cplusplus\n";
        $string    .= "}\n";
        $string    .= "#endif\n";
        
        my $temp_file  = ".merlin_temp";
        open(my $file, '>', $temp_file);
        printf $file $string;
        close($file);
        system "mv $temp_file $input_file";
    }
}
