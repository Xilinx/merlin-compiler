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


use Cwd;
#------------------------------------------------------------------------------#
# excute before everything
#------------------------------------------------------------------------------#
#use strict;
#use warnings;
my $MERLIN_COMPILER_HOME;
my $library_path;
BEGIN {
    $|=1;   #turn on auto reflush for stdout
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    our $xml_dir = "";
}

#------------------------------------------------------------------------------#
# include library
#------------------------------------------------------------------------------#
use lib "$library_path";
use global_var;
use merlin;

#------------------------------------------------------------------------------#
# setup working directories and environments
#------------------------------------------------------------------------------#
my $pyEnv = "${MERLIN_COMPILER_HOME}/build/venv";
unless (-e $pyEnv) {
    print "Error: Merlin specific Python environment not found. ".
        "Please run the installation script.\n";
    exit;
}

$ENV{'PYTHONPATH'} = "$MERLIN_COMPILER_HOME/optimizers/autodse/libs/".
    "opentuner:$ENV{'PYTHONPATH'}";

$prj_dir = getcwd();
chdir $ARGV[0]; # Working directory

my $src_opt = "$prj_dir/$ARGV[1]";
system "cp -rf $src_opt/* .";
system "cp -f ./metadata/user_code_file_info.json .";
system "rm history_passes.list";

my $dse_mode = $ARGV[2];

my $directive_file = $prj_dir . "/spec/directive.xml";
system "cp -r $directive_file .";

#------------------------------------------------------------------------------#
# setup attribute from xml file
#------------------------------------------------------------------------------#

my $dse_exe_file = "${MERLIN_COMPILER_HOME}/optimizers/autodse/".
    "scripts/dse.pyc";

unless (-e $dse_exe_file) {
    print "$dse_exe_file not found, stop AutoDSE.\n";
    exit;
}
my $dse_cmd = "python $dse_exe_file";

#------------------------------------------------------------------------------#
# function declarationt
#------------------------------------------------------------------------------#
sub exit_auto_dse {
    copy_src("./", $src_opt);
    exit;
}

#------------------------------------------------------------------------------#
# main preprocess
#------------------------------------------------------------------------------#

if ($xml_auto_dse ne "on" and $xml_auto_dse ne "custom")
{
    exit;
}

print "Starting design space exploration ... \n";

# Launch AutoDSE within the Python virtual environment
system "source $pyEnv/bin/activate;".
    "$dse_cmd $src_opt; deactivate";
exit_auto_dse;
