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


############################################################
# 1. Loop transformation for auto mode
############################################################

############################################################
# 2. Systolic informatoin extraction
############################################################

############################################################
# 3. Configure the systolic templates
############################################################
$work_dir = "sys_cfg";

system("rm -rf $work_dir");
system("mkdir  $work_dir");

#system("cp -r $optimizer_top/templates_debug_perf/* $work_dir");
system("cp -r $optimizer_top/$ARGV[0]/* $work_dir");
system("cp -r ./systolic_cfg.xml $work_dir");
system("cd $work_dir; mars_perl $optimizer_top/scripts/systolic_gen.pl");


