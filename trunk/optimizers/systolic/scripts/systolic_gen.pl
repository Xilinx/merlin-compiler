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


#$cfg_cmd        = "mars_perl ${src_top}/scripts/src_config.pl";
$cfg_cmd        = "mars_perl ${mars_compiler_home}/optimizers/systolic/scripts/src_config.pl";
#$cfg_cmd        = "echo ";
$frontend_cmd   = "mars_perl ${latest_mars_top}/scripts/tldm/frontend_pass.pl";


$work_dir = ".";
$cfg_xml = "systolic_cfg.xml";


sub print_error_msg {
  my ($msg,@addition) = @_;
  system "touch .cmost_flow_has_error";
	print "$msg"; die("\n");
}

sub run_command
{
#    print "# $_[0]\n";
	system("$_[0]");
}

#print `pwd`;
#print "$cfg_cmd";

&run_command("cd $work_dir; find . -name \"*.cl\"  -exec $cfg_cmd {} -o {} -x $cfg_xml \\;");
&run_command("cd $work_dir; find . -name \"*.c\"   -exec $cfg_cmd {} -o {} -x $cfg_xml \\;");
&run_command("cd $work_dir; find . -name \"*.cpp\" -exec $cfg_cmd {} -o {} -x $cfg_xml \\;");
&run_command("cd $work_dir; find . -name \"*.h\"   -exec $cfg_cmd {} -o {} -x $cfg_xml \\;");
&run_command("cd $work_dir; find . -name \"*.hpp\" -exec $cfg_cmd {} -o {} -x $cfg_xml \\;");
#&run_command("cd $work_dir; find . -name \"*.sh\"  -exec $cfg_cmd {} -o {} -x $cfg_xml \\;");
&run_command("cd $work_dir; find . -name \"*.csh\" -exec $cfg_cmd {} -o {} -x $cfg_xml \\;");

