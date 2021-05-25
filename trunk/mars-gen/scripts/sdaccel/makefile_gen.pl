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

sub get_file_list {
    my $dir = $_[0];
    my %kernel_list;
    my $kernel_name; 
    my $file_name;
    my $info = `cat $dir/__merlin_kernel_list.h`;
    my @list_set = split(/\n/, $info);
    foreach my $one_line (@list_set) {
		if ($one_line =~ /\/\/(.*)=(.*)/ ) {
            $kernel_name = $1;
            $file_name = $2;
            $kernel_list{"$kernel_name"} .= " $file_name";
        }
    }
    return %kernel_list;
}

my $makefile = "Makefile";
my $makefile_org = "";
my $makefile_info = "";
my %kernel_list;
if(-e "__merlin_kernel_list.h") {
    %kernel_list = get_file_list(".");
} else {
    MSG_E_3002();
    exit;
}
if(-e "Makefile") {
    $makefile_org = `cat Makefile`;
} else {
    MSG_E_3002();
    exit;
}

# generate multi-kernel multi-die multi-ddr commandlines, and pend after original makefile
my $kernel_name_list = "";
my $xo_name_list = "";
while (my ($key, $value) = each(%kernel_list)) {
    my $kernel_name = $key;
    my $file_list = $value;
    $makefile_info .= "$kernel_name:\n";
    $makefile_info .= "\t\${CLCC} \${CLCC_OPT} \${INCLUDE_PATH} \${VENDOR_OPTIONS} -o $kernel_name.xo -c $file_list -k $kernel_name --include ./\n\n";
    $kernel_name_list .= "$kernel_name ";
    $xo_name_list .= "$kernel_name.xo "
}
$makefile_info .= "xbin: $kernel_name_list\n";
$makefile_info .= "\t\${CLCC} \${CLCC_OPT} \${INCLUDE_PATH} -o kernel_top.xclbin --link $xo_name_list \${VENDOR_OPTIONS} \${BANK_OPTION} \${DIE_OPTION} --include ./\n\n";
$makefile_info .= "\t\@cp kernel_top.xclbin pkg\n\n";
$makefile_info .= "hls: $kernel_name_list\n";
$makefile_info .= "\t\@mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/sdaccel/report_gen.pl hls\n";
#$makefile_info .= "\t\${CLCC} -c -s -t hw_emu -o kernel_top.xo --xdevice \${DEVICE} --report estimate --link $xo_name_list \${INCLUDE_PATH} \${VENDOR_OPTIONS}\n";

my $makefile_new = "${makefile_org}\n${makefile_info}\n";
#printf "$makefile_new\n";

my $filename = "Makefile_config";
system "rm -rf $filename";
open my $file, ">$filename";
print $file $makefile_new;
close $file;
