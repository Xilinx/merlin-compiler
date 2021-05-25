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



use File::Basename;

$XILINX_SDX = $ENV{'XILINX_SDX'};

if(-e "../spec/directive.xml") {
    $xml_file = "../spec/directive.xml";
} elsif(-e ".merlin_prj/run/spec/directive.xml") {
    $xml_file = ".merlin_prj/run/spec/directive.xml";
} elsif(-e "../directive.xml") {
    $xml_file = "../directive.xml";
}

my $info_xml = `cat $xml_file`;
my $platform = "";
#printf "platform = $platform\n";
@file_info_set = split(/\n/, $info_xml);
foreach my $one_line(@file_info_set) {
    if($one_line =~ /<platform_name>\s*(\S+)\s*<\/platform_name>/) {
        $platform = $1;
    }
}

$csh_file_info = "";
$sh_file_info  = "";

$curr_dir = `pwd`;
chomp($curr_dir);
#printf "$curr_dir\n";

#$version = `source $XILINX_SDX/settings64.sh; sdaccel -version`;
#if($version =~ /sdaccel v(\d+\.\d+)/g) {
#    $version = $1;
##    printf "version = $version\n";
#}
#
#if($version eq "2015.3") {
#    $platform .= ".2.0"
#} elsif($version eq "2015.4") {
#    $platform .= ".2.1"
#} elsif($version eq "2016.1") {
#    $platform .= ".3.0"
#}

$platform =~ s/:/_/g;
$platform =~ s/\./_/g;
#printf "platform = $platform\n";

# generate csh and bash used environment setting files
$csh_file_info .= "setenv XILINX_OPENCL pcie\n";
$csh_file_info .= "setenv LD_LIBRARY_PATH \$XILINX_OPENCL/lib/lnx64.o:\$XILINX_OPENCL/runtime/lib/x86_64\n";
$csh_file_info .= "setenv XCL_PLATFORM $platform\n";
$csh_file_info .= "setenv XILINX_SDX $XILINX_SDX\n";
$csh_file_info .= "source $XILINX_SDX/settings64.csh\n";
#printf "$csh_file_info\n";

$sh_file_info .= "export XILINX_OPENCL=pcie\n";
$sh_file_info .= "export LD_LIBRARY_PATH=\$XILINX_OPENCL/lib/lnx64.o:\$XILINX_OPENCL/runtime/lib/x86_64\n";
$sh_file_info .= "export XCL_PLATFORM=$platform\n";
$sh_file_info .= "export XILINX_SDX=$XILINX_SDX\n";
$sh_file_info .= "source $XILINX_SDX/settings64.sh\n";
#printf "$sh_file_info\n";

$csh_file = "source_sdaccel.csh";
$sh_file  = "source_sdaccel.sh";
open(my $fh_csh, '>', $csh_file);
print $fh_csh $csh_file_info;
close $fh_csh;
open(my $fh_sh, '>', $sh_file);
print $fh_sh $sh_file_info;
close $fh_sh;
