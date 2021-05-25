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


my $analysis_profile = "";
if (-e "./analyze_profile.py") {
  $analysis_profile  = " echo '\n========== Profiling Data ==========';";
  $analysis_profile .= " python analyze_profile.py sdaccel_profile_summary.csv | grep 'Compute Unit Utilization';";
  $analysis_profile .= " python analyze_profile.py sdaccel_profile_summary.csv | grep 'Data Transfer: Host';";
  $analysis_profile .= " python analyze_profile.py sdaccel_profile_summary.csv | grep 'Data Transfer: Kernels';";
  $analysis_profile .= " echo   '====================================';";
}

my $host_args = "";
for(my $i=0; $i<$ARGC; $i++) {
    $host_args .= " $ARGV[$i]"
}
my $shell = `echo \$0`;
my $user = `whoami`;
chomp($user);
printf "user  = $user\n";
printf "shell = $shell\n";
printf "host_args = $host_args\n";
if($shell =~ /csh/) {
    $source = "source /curr/$user/fcs_setting64.csh";
	system("$source; ./host_top $host_args; $analysis_profile");
} else {
    $source = "source /curr/$user/fcs_setting64.sh";
	system("$source; ./host_top $host_args; $analysis_profile");
}
