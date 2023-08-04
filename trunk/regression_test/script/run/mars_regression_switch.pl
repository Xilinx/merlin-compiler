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
use File::Basename;
use XML::Simple;
use Data::Dumper;

$xml_file = $ARGV[0];

$directive_file = $xml_file;
$directive_list = XML::Simple->new(ForceArray => 1);
$directive_all = $directive_list->XMLin($directive_file);

$config_count = 0;
my @directive_set = @{$directive_all->{"config"}};
foreach my $directive_one (@directive_set) {
#    printf Dumper($directive_one);
    $regression = $directive_one->{"regression_test"}[0];
    $config_name = $directive_one->{"name"}[0];
    if($regression eq "off") {
        printf "$config_name regression_test off\n";
        delete $directive_all->{"config"}[$config_count];
    } else {
        printf "$config_name regression_test on\n";
    }
    $config_count = $config_count + 1;
}

XMLout($directive_all,
       KeepRoot => 1,
       NoAttr => 1,
       Outputfile => $directive_file,
       );

$xml_file_info = `cat $xml_file`;
$xml_file_info =~ s/<config><\/config>//g;
$xml_file_info =~ s/<config>([\s\S]*)<\/config>/<configs>\n\n<config>$1<\/config>\n\n<\/configs>/g;

$file_replace = "tmp.xml";
open(my $fh, '>', $file_replace);
print $fh "$xml_file_info\n";
close $fh;

system "mv tmp.xml $xml_file";
