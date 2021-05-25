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


##############################################################
# Backend flow script
#
# Filename    : merlin_flow/scripts/merlin_backend_top.pl
# Description : This is the top level sripts for the backend design flow 
# Usage       : 
# Owner(s)    : Han 
# version     : 12-31-2017 Create file and basic function
##############################################################

#---------------------------------------------------------------------------------------------#
# 0. environment and scripts preparation
#---------------------------------------------------------------------------------------------#

#---------------------------------------------------------------------------------------------#
# excute before everything
#---------------------------------------------------------------------------------------------#
#use strict;
#use warnings;
my $MERLIN_COMPILER_HOME;
my $library_path;
my $flow;
my $src_dir;
my $dst_dir;
my $original_arg = ''; 
BEGIN {
    #---------------------------------------------------------------------------------------------#
    # prepare input
    #---------------------------------------------------------------------------------------------#
    sub exit_printf {
        printf "Correct Usage  : \n";
        printf "    -flow       : c2cl/lib_gen/host/hls/sim/hw_sim/sim_config/hw_sim_config/impl/test/exec\n";
        printf "    -src        : source code directory\n";
        printf "                : if need to run c2cl, source code should be c code\n";
        printf "                : if need directly use cl code as input, source code should be export directory\n";
        printf "    -dst        : output target directory\n";
        printf "    --attribute : --attribute key=value\n";
        printf "                : merlinc will add key value pair to xml\n";
        printf "                : --attribute key+=value\n";
        printf "                : merlinc will extend key value pair to xml\n";
        exit;
    }

    $|=1;   #turn on auto reflush for stdout
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'}; 
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";

    for (my $i = 0; $i < @ARGV; $i+=1) {
        if($ARGV[$i] eq "-src") {
            if($ARGV[$i+1] !~ /^-.*/) {
                $src_dir = $ARGV[$i+1];
            }
        }
    }
    our $xml_dir = "";
    if($src_dir eq "") {
        $xml_dir = "./";
    } else {
        $xml_dir = $src_dir;
    }
    if(not -e "$xml_dir/directive.xml") {
        exit;
#        print "ERROR: [MERCC-3012] Can not find directive.xml.\n";
#        exit_printf();
    }
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use strict;
use warnings;
use lib "$library_path";
use message;
use merlin;
use global_var;
#suppress smartmatch is experimental
no if $] >= 5.017011, warnings => 'experimental::smartmatch';

#---------------------------------------------------------------------------------------------#
# parse input argument 
#---------------------------------------------------------------------------------------------#
for (my $i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] =~ /^-.*/) {
        if($ARGV[$i] eq "-src") {
        } elsif($ARGV[$i] eq "-dst") {
        } elsif($ARGV[$i] eq "-flow") {
        } elsif($ARGV[$i] eq "--attribute") {
        } elsif($ARGV[$i] eq "-s") {
        } elsif($ARGV[$i] eq "-c") {
        } elsif($ARGV[$i] eq "-keep_platform") {
        } elsif($ARGV[$i] eq "-keep_device") {
        } elsif($ARGV[$i] eq "-no_bit_test") {
        } elsif($ARGV[$i] eq "-no_hw_sim_test") {
        } elsif($ARGV[$i] eq "-no_sim_test") {
        } elsif($ARGV[$i] eq "-no_opt_test") {
        } elsif($ARGV[$i] eq "-no_org_test") {
        } elsif($ARGV[$i] eq "-merlincc") {
        } elsif($ARGV[$i] eq "--profile_bit") {
        } elsif($ARGV[$i] eq "--profile") {
        } else {
            MSG_E_3015($ARGV[$i]);
            exit_printf();
        } 
    }
}
my $count_flow = 0;
my $count_src  = 0;
my $count_dst  = 0;
for (my $i = 0; $i < @ARGV; $i+=1) {
    if($ARGV[$i] eq "-flow") {
        if($ARGV[$i+1] !~ /^-.*/) {
            $flow = $ARGV[$i+1];
            $i=$i+1;
            if($count_flow > 0) {
                MSG_E_3008($flow);
                exit_printf();
            }
            $count_flow++;
        }
    } elsif($ARGV[$i] eq "-src") {
        if($ARGV[$i+1] !~ /^-.*/) {
            $src_dir = $ARGV[$i+1];
            $i=$i+1;
            if($count_src > 0) {
                MSG_E_3008("-src");
                exit_printf();
            }
            $count_src++;
        }
    } elsif($ARGV[$i] eq "-dst") {
        if($ARGV[$i+1] !~ /^-.*/) {
            $dst_dir = $ARGV[$i+1];
            $i=$i+1;
            if($count_dst > 0) {
                MSG_E_3008("-dst");
                exit_printf();
            }
            $count_dst++;
        }
    } else {
        $original_arg .= "$ARGV[$i] "; 
    }
}

# cgeck input option illegality
if($flow ~~ ["c2cl", "post", "host", "lib_gen", "hls", "sim", "sim_config", "hw_sim", "hw_sim_config", "impl", "exec", "test", "bit"]) {
} else {
    MSG_E_3009($flow);
    exit_printf();
}
if($src_dir eq "") {
    MSG_E_3011("-src");
    exit_printf();
}
if($dst_dir eq "") {
    MSG_E_3011("-dst");
    exit_printf();
}
if(not -e $src_dir) {
    MSG_E_3010($src_dir);
    exit_printf();
}
if(not -e "$src_dir/directive.xml") {
    MSG_E_3012();
    exit_printf();
}

#---------------------------------------------------------------------------------------------#
# script call
#---------------------------------------------------------------------------------------------#
my $merlin_backend  = "mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/merlin_backend.pl";
#---------------------------------------------------------------------------------------------#
# update diretive.xml ny --attributes
#---------------------------------------------------------------------------------------------#
my $curr_pwd = `pwd`;
my $dst_pwd = "";
my $src_pwd = "";
if(-e $src_dir) {
    $src_pwd = `cd $src_dir; pwd;`;
}
if(-e $dst_dir) {
    $dst_pwd = `cd $dst_dir; pwd;`;
}
if($src_pwd eq $dst_pwd) {
} elsif($curr_pwd eq $dst_pwd) {
    run_command "cd $dst_dir; rm -rf * .* >& /dev/null";
} else {
    if($flow ne "post") {
        run_command "rm -rf $dst_dir; mkdir $dst_dir";
    }
}

# replace attribute of xml with input argument
if(-e "$src_dir/directive.xml") {
    for (my $i = 0; $i < @ARGV; $i+=1) {
        if($ARGV[$i] eq "--attribute") {
            my $temp = $ARGV[$i+1];
            if($ARGV[$i+1] =~ /(.*)\+=(.*)/) {
                my $key = $1;
                my $value = $2;
                replace_attribute("$src_dir/directive.xml", $key, $value, 1);
                if($1 eq "" or $2 eq "") {
                    MSG_E_3013();
                    exit 1;
                }
            } elsif($ARGV[$i+1] =~ /(.*)=(.*)/) {
                my $key = $1;
                my $value = $2;
                replace_attribute("$src_dir/directive.xml", $key, $value, 0);
                if($1 eq "" or $2 eq "") {
                    MSG_E_3013();
                    exit 1;
                }
            } else {
                MSG_E_3013();
                exit 1;
            }
        }
    }
}

my $directive = "$src_dir/directive.xml";
if(not -e $directive) {
    MSG_E_3012();
    exit;
}

#---------------------------------------------------------------------------------------------#
# 1. run c2cl flow to generate opencl code
#---------------------------------------------------------------------------------------------#
if($flow eq "c2cl") {
    run_command "$cmd_final_code_gen $src_dir $dst_dir c2cl";
} elsif($flow eq "post") {
    run_command "$cmd_final_code_gen $src_dir $dst_dir post";
} else {
    # check directory legaliity
    if(not -e "$src_dir/kernel" or not -e "$src_dir/lc") {
        MSG_E_3014();
        exit;
    }
#---------------------------------------------------------------------------------------------#
# 2. run vendor tool
#---------------------------------------------------------------------------------------------#
    # set default platform if environment setted
    run_command "$cmd_internal_platform $src_dir/directive.xml";
    run_command "$merlin_backend $flow $src_dir $dst_dir \"$original_arg\"";
}

exit;
