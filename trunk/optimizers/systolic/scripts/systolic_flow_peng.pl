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



$mars_compiler_home = $ENV{'MERLIN_COMPILER_HOME'};
$src_top = $ENV{'ESLOPT_HOME'};
$mars_top = "$src_top/mars_flow";
$latest_mars_top = $ENV{'MARS_BUILD_DIR'};
$optimizer_top = "$ENV{'OPTIMIZER_HOME'}/systolic";

$cfg_cmd        = "mars_perl ${src_top}/scripts/src_config.pl";
$frontend_cmd   = "mars_perl ${latest_mars_top}/scripts/tldm/frontend_pass.pl";


$impl_num = 1;

for ($i = 0; $i < @ARGV; $i+=1) {
    if ("-impl_num" eq $ARGV[$i])  { $impl_num = $ARGV[$i+1]; }
}

$work_dir = "merlin_tmp";
$opencl_dir= "opencl/";
$output_dir= "opt/";

system("rm -rf $work_dir");
system("mkdir  $work_dir");
system("rm -rf $output_dir");
system("mkdir  $output_dir");
system("rm -rf $opencl_dir");
system("mkdir  $opencl_dir");


############################################################
# 1. Loop transformation for auto mode
############################################################
$opt_dir = "$work_dir/opt";
system("mkdir $opt_dir/");
system("cp -r src/* $opt_dir/");
if (-e "directive.xml")
{
    system("cp directive.xml $opt_dir");
}

system("cd $opt_dir; $frontend_cmd . -p systolic_frontend -print on -a '-a impl_num=$impl_num'");
if ($impl_num == 1)
{
    system ("cp $opt_dir/*.c $opt_dir/*.cpp $opt_dir/*.h $opt_dir/*.hpp $output_dir/ >& /dev/null");    
}
else
{
    system ("cp $opt_dir/*.c $opt_dir/*.cpp $opt_dir/*.h $opt_dir/*.hpp $output_dir/ >& /dev/null");    
}

############################################################
# 2. Systolic informatoin extraction
############################################################
system("cd $opt_dir; $frontend_cmd . -p systolic_extract -print ");

############################################################
# 3. Configure the systolic templates
############################################################
$cfg_dir = "$work_dir/sys_cfg";
system("rm -rf $cfg_dir");
system("mkdir  $cfg_dir");

system("cp -r $optimizer_top/templates/* $cfg_dir");
system("cp -r $opt_dir/systolic_cfg.xml $cfg_dir");
system("cd $cfg_dir; mars_perl $optimizer_top/scripts/systolic_gen.pl");

system ("cp -r $cfg_dir/* $opencl_dir/");    


