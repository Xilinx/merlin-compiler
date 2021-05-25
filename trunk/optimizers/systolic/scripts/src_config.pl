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


#!/usr/bin/perl
#This file is to convert the user template file into a configured instantiation file

$src_top = $ENV{'MERLIN_COMPILER_HOME'};
$script_top = $ENV{'ESLOPT_HOME'};
my $file_in  = $ARGV[0];
#print "[src_config.pl $file_in] ESLOPT_HOME=$src_top \n";
#youxiang split very long lines into multiple lines with white space as
#delimiter
#system "mars_perl ${script_top}/scripts/split_process.pl $file_in";
system "mars_perl ${src_top}/optimizers/systolic/scripts/split_process.pl $file_in";

if (not -e "${src_top}/build/bin/cfg_gen") {
    die "[src_config.pl] ERROR: ${src_top}/build/bin/cfg_gen does not exist!\n";
}

if (0 == @ARGV || 8 < @ARGV) {
    my $num = @ARGV;
    print 'Number of argument ='.$num."\n";
    print "Usage : *.pl src_in [-o src_out] [-x cfg.xml] [-q default|simple] [debug]\n";
    exit;
}

my $src_out = "";
my $file_out = "";
my $xml_in = "";
my $quote_style = "";
my $debug = 0;


for ($i = 1; $i < @ARGV; $i+=2) {
    if ($ARGV[$i] eq "-o") {
        $src_out = "-o ".$ARGV[$i+1];
        $file_out = $ARGV[$i+1];
    }
    elsif ($ARGV[$i] eq "-x") {
        $xml_in = $ARGV[$i+1];
    }
    elsif ($ARGV[$i] eq "-q") {
        $quote_style = "-q ".$ARGV[$i+1];
    }
    elsif ($ARGV[$i] eq "debug") {
        $debug = 1;
    }
    else {
        print "[src_config.pl] Argument Error: ".$ARGV[$i]."\n";
        print "Usage : src_config.pl src_in [-o src_out] [-x cfg.xml] [-q default|simple] [debug]\n";
        exit;
    }
}


if ($file_out eq "" and substr($file_in, length($file_in) - 2) eq "_p") {
	$file_out = substr($file_in, 0, length($file_in) - 2);
}

if ($xml_in eq "") {
if ($debug) { print "[Configuring $file_in with default...]\n" ; }
system "${src_top}/build/bin/cfg_gen -f ${file_in} -o ${file_in}.cfg.xml";
$xml_file = "${file_in}.cfg.xml";
}
else {
if ($debug) { print "[Configuring $file_in with $xml_in ...]\n" ; }
$xml_file = ${xml_in};
}
system "${src_top}/build/bin/cfg2def ${xml_file}";

#####################
open(DEFINE_FILE, ">>defines.pl");
print DEFINE_FILE "\n\nsub argument_define {\n";
print DEFINE_FILE "  return (\"$file_in\", \"$xml_file\", \"$file_out\");\n";
print DEFINE_FILE "}\n";
close(DEFINE_FILE);
#####################


system "${src_top}/build/bin/src_cfg ${file_in} ${src_out} ${quote_style}";

if (not -e "${file_out}" or not -e "src_cfg_succeed") {
	print "[src_config.pl] ERROR: input=$file_in cfg=${xml_in} output=${file_out}\n>";
	$input = <STDIN>;
	system "touch .cmost_flow_has_error";
	die "\n\n";
}

if (0 == $debug)
{
#system "rm -rf ${file_in}.cfg.xml";
system "rm -rf temp.pl";
system "rm -rf tmp";
system "rm -rf defines.pl";
system "rm -rf ${file_in}_int";
}
