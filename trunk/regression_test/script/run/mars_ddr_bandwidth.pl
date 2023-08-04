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

$MARS_BUILD_DIR = $ENV{'MARS_BUILD_DIR'};
$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};

$config_dir = "${MARS_TEST_HOME_GIT}/configuration";

system "cp -r $config_dir/ddr_bandwidth.xml .";
$xml_file = "ddr_bandwidth.xml";

#choose test cases directory
$platform = $ARGV[1];

use File::Basename;
use XML::Simple;
use Data::Dumper;

#change run flow in xml to specify flow
sub change_xml{
    $platform = $_[0];

    $config_file = "$xml_file";
    $config_list = XML::Simple->new(ForceArray => 1);
    $config_all = $config_list->XMLin($config_file);
    my @config_set = @{$config_all->{"config"}};
    foreach my $config_one (@config_set) {
       if($platform eq "ku3") {
           $config_one->{"platform_name"}[0] = "alphadata_pcieku3";
           $config_one->{"part_name"}[0] = "xcku060-ffva1156-2-e";
           $config_one->{"arch_name"}[0] = "ultra_kintex7";
       } elsif($platform eq "7v3") {
           $config_one->{"platform_name"}[0] = "alphadata_pcie7v3";
           $config_one->{"part_name"}[0] = "xc7vx690tffg1761-2";
           $config_one->{"arch_name"}[0] = "virtex7";
       } elsif($platform eq "baidu") {
           $config_one->{"platform_name"}[0] = "baidu_kintex7";
           $config_one->{"part_name"}[0] = "xc7k480tiffg1156-2L";
           $config_one->{"arch_name"}[0] = "kintex7";
       } elsif($platform eq "baidu_built-in") {
           $config_one->{"platform_name"}[0] = "baidu_kintex7";
           $config_one->{"part_name"}[0] = "xc7k480tiffg1156-1";
           $config_one->{"arch_name"}[0] = "kintex7";
       } else {
           $config_one->{"platform_name"}[0] = "alphadata_pcieku3";
           $config_one->{"part_name"}[0] = "xcku060-ffva1156-2-e";
           $config_one->{"arch_name"}[0] = "ultra_kintex7";
       }
    }
    XMLout($config_all,
           KeepRoot => 0,
           NoAttr => 0,
           Outputfile => $config_file,
           );
}

#change xml root from opt to configs
sub change_root {
    $org_file = $_[0];
#    printf "$org_file\n";
    $xml_root_out_file = "tmp_root_out.xml";
    open(xml_root_in,"< $org_file");
    open(xml_root_out,"> $xml_root_out_file");
    while(<xml_root_in>) {
        ~s/<opt>\n/<configs>\n/g;
        ~s/<\/opt>\n/<\/configs>\n/g;
        print xml_root_out;
    }
    close(xml_root_in);
    close(xml_root_out);
    system "mv $xml_root_out_file $org_file";
}

change_xml($platform);
change_root($xml_file);

system "mars_test -x $xml_file -cloud -force";
