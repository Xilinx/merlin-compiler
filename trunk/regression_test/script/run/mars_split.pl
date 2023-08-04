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

#choose test cases directory
$xml_org = $ARGV[0];

use File::Basename;
use XML::Simple;
use Data::Dumper;

#change run flow in xml to specify flow
sub split_xml{
    $org_file = $_[0];
    $new_flow = $_[1];

    system "cp -r $org_file ${new_flow}_$org_file";	
    $config_file = "${new_flow}_$org_file";
    $config_list = XML::Simple->new(ForceArray => 1);
    $config_all = $config_list->XMLin($config_file);
    my @config_set = @{$config_all->{"config"}};
    foreach my $config_one (@config_set) {
       $config_one->{"flow"}[0] = "$new_flow";
       $old_name = $config_one->{"name"}[0];
       $config_one->{"name"}[0] = "${new_flow}_$old_name";
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

#generate sim xml file
split_xml($xml_org,"no_bit_test");
change_root("no_bit_test_$xml_org");
#generate hls xml file
split_xml($xml_org,"hls");
change_root("hls_$xml_org");

