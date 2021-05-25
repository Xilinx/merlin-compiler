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



$MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};

$directive = $ARGV[0];
@argv_set = split(/ /, $ARGV[1]);
$curr_dir = $ARGV[2];
$src_dir = $ARGV[3];

sub check_params {
    my ($flag) = @_;
    for($i=0; $i<@argv_set; $i++) {
        if($argv_set[$i] =~ /^$flag/) {
            return 1;
        } 
    }
    return 0;
}
sub get_params {
    my ($flag) = @_;
    my $params = " ";
    my @count = 0;
    for($i=0; $i<@argv_set; $i++) {
        $temp = "";
        if($argv_set[$i] =~ /^${flag}$/) {
            $basic = $argv_set[$i+1];
            if($basic =~ /^(\/.*)/) {
                $temp = "$1";
            } else {
                $temp = "${curr_dir}\/$basic";
            }
        } elsif($argv_set[$i] =~ /^${flag}(.*)/) {
            $basic = $1;
            if($basic =~ /^(\/.*)/) {
                $temp = "$1";
            } else {
                $temp = "${curr_dir}\/${basic}";
            }
        }
        if($temp ne "") {
            if(!-e "$temp") {
                printf "ERROR : $temp not exist\n";  
                exit;
            }
            $params .= "${flag}${temp} ";
        }
    }
    return $params;
}
sub check_param_equal {
    my ($flag) = @_;
    for($i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] =~ /(\S+)=\S+/) {
            if($1 eq "$flag") {
                return 1;
            }
        } 
    }
    return 0;
}
sub get_param_equal {
    my ($flag) = @_;
    for($i=0; $i<@ARGV; $i++) {
        if($ARGV[$i]  =~ /(\S+)=(\S+)/) {
            if($1 eq $flag) {
                if($2 eq "") {
                    printf "ERROR : Please specify parameter for $flag.\n";
                    exit;
                } else {
                    return $2;
                }
            }
        } 
    }
    return " ";
}

#check -I include files
my $include_directory = "";
$include_directory .= "-I${curr_dir}\/${src_dir} ";
if( check_params("-I") ) {
    $include_directory .= get_params("-I");
}
$include_directory .= " -I $MERLIN_COMPILER_HOME/mars-gen/lib/merlin";
$include_directory =~ s/\//\\\//g;
system "perl -pi -e 's/<directives>/<directives>\n    <include_path> $include_directory<\\\/include_path>/g' $directive";

#check keep
my $keep_platform_flag = 0;
if( check_params("-keep_platform") ) {
    $keep_platform_flag = 1;
}

#check merlincc
my $merlincc_flag = 0;
if( check_params("-merlincc") ) {
    $merlincc_flag = 1;
}

#check merlincc
my $no_check = 0;
if( check_params("-no_check") ) {
    $no_check = 1;
}

#check platform
my $merlin_device = "";
my $merlin_platform = "";
if( $keep_platform_flag eq 0 and check_param_equal("--platform") ) {
    $merlin_platform = get_param_equal("--platform");
    if($merlin_platform =~ /(.*)::(.*)$/) {
        $tool = $1;
        $platform = $2;
        if($tool ne "sdaccel") {
            printf "ERROR : Merlin only support sdaccel, not support \"$tool\" as implementation tool\n";
            exit;
        }
        system "perl -pi -e 's/<\\\s*implementation_tool\\\s*>.*<\\\/\\\s*implementation_tool\\\s*>/<implementation_tool>$tool<\\\/implementation_tool>/g' $directive";
        system "perl -pi -e 's/<\\\s*platform_name\\\s*>.*<\\\/\\\s*platform_name\\\s*>/<platform_name>$platform<\\\/platform_name>/g' $directive";
    } else {
        print "WARNING : --platform not set correct, Merlin will use default platform, please set as formet:\n ";
        print "--platform=[implement_tool]::[platform_name]\n ";
        exit;
    }
}

