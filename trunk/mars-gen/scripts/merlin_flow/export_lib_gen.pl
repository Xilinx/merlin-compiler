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
use message;
use global_var;
use File::Basename;
$ENV{"PERL5SHELL"} = "tcsh";

#---------------------------------------------------------------------------------------------#
# parse xml file
#---------------------------------------------------------------------------------------------#
my $pure_kernel_flow = "off";
if($xml_sycl eq "on" or $xml_pure_kernel eq "on") {
    $pure_kernel_flow = "on";
}
my $debug = "";
if($xml_merlincc_debug eq "on") {
    $debug = " DEBUG=1"
}

my $export_dir = "$prj_export";
my $lib_dir    = "$export_dir/lib_gen";
my $pkg_dir    = "../../../pkg";
if(not -e $export_dir) {
    printf "Error: Directory $export_dir not exist.\n";
    exit;
}

run_command "rm -rf $lib_dir; mkdir $lib_dir";
if ($pure_kernel_flow eq "on") {
    exit;
}
run_command "cp $export_dir/*.h $export_dir/*.hpp $lib_dir 2> /dev/null";

#---------------------------------------------------------------------------------------------#
# copy library generation releated files, and delete unnecessary files
#---------------------------------------------------------------------------------------------#
run_command "cp $export_dir/__merlinwrapper_*                $lib_dir 2> /dev/null";
run_command "cp $export_dir/__merlintask_*                   $lib_dir 2> /dev/null";
run_command "cp $export_dir/__merlin_opencl_if*              $lib_dir 2> /dev/null";
run_command "cp $export_dir/__merlin_opencl_kernel_buffer*   $lib_dir 2> /dev/null";

if ($xml_sycl ne "on") {
#---------------------------------------------------------------------------------------------#
# if have posix_memalign in wrapper, add #define _POSIX_C_SOURCE 200809L to avoid warning
#---------------------------------------------------------------------------------------------#
  my $source_list = `cd $lib_dir; ls __merlinwrapper_*`;
  my @source_set = split(/\n/, $source_list);
  foreach my $file (@source_set) {
    my $info = `cat $file`;
    if ($info =~ 'posix_memalign') {
      run_command("cd $lib_dir; sed -i '1i#define _POSIX_C_SOURCE 200809L' $file");
    }
  }
}


run_command "cd $lib_dir; mars_perl $dir_scripts/merlin_flow/add_extern.pl";

#---------------------------------------------------------------------------------------------#
# get mem size from attribute
#---------------------------------------------------------------------------------------------#
#print "mem_size_limitation = $xml_mem_size_limitation\n";
my $size = 4;
if($xml_mem_size_limitation =~ /(.*)GB/) {
    $size = $1;
    if($size <= 1) {
        $size = 1;
    } elsif($size > 1 and $size <= 2) {
        $size = 2;
    } elsif($size > 2 and $size <= 4) {
        $size = 4;
    } elsif($size > 4 and $size <= 8) {
        $size = 8;
    } elsif($size > 5 and $size <= 16) {
        $size = 16;
    } elsif($size > 16 and $size <= 32) {
        $size = 32;
    } else {
        $size = 4;
    }
}
#my $number = log($size) / log(2) + 1;
run_command "cd $lib_dir; perl -pi -e 's/MERLIN_MEM_SIZE_LIMITATION_NUMBER/${size}UL << 30/g' *.c* 2> /dev/null"; 
run_command "cd $lib_dir; perl -pi -e 's/MERLIN_MEM_SIZE_LIMITATION_STRING/${size}GB/g' *.c* 2> /dev/null"; 
if ($xml_sycl ne "on") {
   #---------------------------------------------------------------------------------------------#
   # generate library with Makefile
   #---------------------------------------------------------------------------------------------#
   $xml_include_path =~ s/\//\\\//g;
   $xml_include_path =~ s/-I/ -I/g;
   run_command "cp $export_dir/Makefile $lib_dir/Makefile";
   run_command "cd $lib_dir; perl -pi -e 's/\\\$\\\(MERLIN_INCLUDE\\\)/$xml_include_path/g' Makefile 2> /dev/null"; 
   run_command "cd $lib_dir; make -s clean; make -s lib_gen $debug >& lib_gen.log";
   if($xml_debug_mode eq "debug-level2" or $xml_debug_mode eq "debug-level3") {
       if(not -e "$lib_dir/bin/libkernel.so") {
           run_command "cat $lib_dir/lib_gen.log";
           MSG_E_3003();
           exit;
       }
   }
   
   run_command "cd $lib_dir; cp __merlinhead_*.h bin 2> /dev/null";
   run_command "cd $lib_dir; cp __merlintask_*.h bin 2> /dev/null";
   run_command "cp $lib_dir/bin/* $pkg_dir 2> /dev/null";
}
# delete unnecessary files
run_command "cd $lib_dir; rm -rf kernel_die.h cmostmini.h merlin_stream.h 2> /dev/null";

