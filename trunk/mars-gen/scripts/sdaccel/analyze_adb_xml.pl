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


# suppress smartmatch is experimental
no if $] >= 5.017011, warnings => 'experimental::smartmatch';

my @list = <*.sched.adb.xml>;
if (scalar @list == 0) {
    exit;
}

my $file_list = `ls *.sched.adb.xml`;
my @file_set = split(/\n/, $file_list);

my $output_json = "adb.json";
my $file_name = "";
my $has_data = 0;
my $state_flag = 0;
my $state_id = 0;
my $node_flag = 0;
my $string_json = "";
my $string_json_data = "";
my $string_one_state = "";
my @exist_name = ();
$string_json = "\{\n";
foreach my $one_file (@file_set) {
    #print "one_file = $one_file\n";
    if($one_file =~ /(.*).sched.adb.xml/) {
        $file_name = $1;
    }
    $has_data = 0;
    $string_json_data = "";
    my $file_info = `cat $one_file`;
    my @info_set = split(/\n/, $file_info);
    @exist_name = ();
    foreach my $one_info (@info_set) {
        if($one_info =~ /<state id=\"(.*)\" /) {
            $state_flag = 1;
            $string_one_state = "";
            $state_id = $1;
        }
        if($state_flag eq 1) {
            if($one_info =~ /<Node/) {
                $node_flag = 1;
            }
            if($node_flag eq 1) {
                if($one_info =~ /call fastcc .*\@\"(.*)\"\(/) {
                } elsif($one_info =~ /call fastcc .*\@(.*)\(/) {
                    my $func_name = $1;
                    if($func_name ~~ @exist_name) {
                    } else {
                        push @exist_name, $func_name;
                        $string_one_state .= "$func_name ";
                    }
                    #print "find one line = $1\n";
                }
            }
            if($one_info =~ /<\/Node>/) {
                $node_flag = 0;
            }
        }
        if($one_info =~ /<\/state>/) {
            $state_flag = 0;
            if($string_one_state ne "") {
                $has_data = 1;
                #printf "string_one_state = $string_one_state\n";
                $string_json_data .= "        \"$state_id\":\"$string_one_state\",\n";
            }
        }
    }
    if($has_data eq 1) {
        $string_json .= "    \"$file_name\":\{\n";
        $string_json .= $string_json_data;
        $string_json .= "        \"\":\"\"\n";
        $string_json .= "    },\n";
    }
}
$string_json .= "    \"\":\{\n";
$string_json .= "    }\n";
$string_json .= "}\n";

write_file($output_json, $string_json);

system "perl -i -p0e 's/,\n        \"\":\"\"//g' $output_json";
system "perl -i -p0e 's/,\n    \"\":\\\{\n    \\\}//g' $output_json";

sub read_file {
    my ($filename) = @_;

    open my $in, '<:encoding(UTF-8)', $filename or die "Could not open '$filename' for reading $!";
    local $/ = undef;
    my $all = <$in>;
    close $in;

    return $all;
}

sub write_file {
    my ($filename, $content) = @_;

    open my $out, '>:encoding(UTF-8)', $filename or die "Could not open '$filename' for writing $!";;
    print $out $content;
    close $out;

    return;
}
