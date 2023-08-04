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

$package_dir = $ARGV[0];
$MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$case_dir = "$MARS_TEST_HOME_GIT/pkg_special_test";
system "rm -rf err.log; touch err.log;";

$env_file1 = "$package_dir/settings64.csh";
$env_file1 =~ s/\//\\\//g;
$env_file2 = "$package_dir/settings64.sh";
$env_file2 =~ s/\//\\\//g;

# run a case with de5net_a7 with aocl 16.0
printf "\nRun de5net on AOCL 16.0\n";
system "perl -pi -e 's/16.1_pro/16.0/g' $env_file1";
system "perl -pi -e 's/a10_ref/terasic\\\/de5net/g' $env_file1";
system "perl -pi -e 's/16.1_pro/16.0/g' $env_file2";
system "perl -pi -e 's/a10_ref/terasic\\\/de5net/g' $env_file2";

system "rm -rf vec_add_de5net_a7";
system "cp -r $case_dir/vec_add_de5net_a7 .";
system "source $package_dir/settings64.sh; cd vec_add_de5net_a7; merlin_create_project run -s src";
system "source $package_dir/settings64.sh; cd vec_add_de5net_a7; merlin_flow run";
if(-e "vec_add_de5net_a7/run/pkg/libkernel.so") {
} else {
    system "echo fail de5net_a7 16.0 run >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_de5net_a7; merlin_flow run host";
if(-e "vec_add_de5net_a7/run/pkg/host_top") {
} else {
    system "echo fail de5net_a7 16.0 run host >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_de5net_a7; merlin_flow run sim";
if(-e "vec_add_de5net_a7/run/implement/exec/run_sim/bin/pass.o") {
} else {
    system "echo fail de5net_a7 16.0 run sim >> err.log";
}
#system "source $package_dir/settings64.sh; cd vec_add_de5net_a7; merlin_flow run bit";
if(-e "vec_add_de5net_a7/run/pkg/kernel_top.aocx") {
} else {
    system "echo fail de5net_a7 16.0 run bit >> err.log";
}

#run sdaccel 2016.2
printf "\nRun on SDAccel 2016.2\n";
system "perl -pi -e 's/SDAccel\\\/201.*/SDAccel\\\/2016.2/g' $env_file1";
system "perl -pi -e 's/SDAccel\\\/201.*/SDAccel\\\/2016.2/g' $env_file2";
system "perl -pi -e 's/SDx\\\/201.*/SDAccel\\\/2016.2/g' $env_file1";
system "perl -pi -e 's/SDx\\\/201.*/SDAccel\\\/2016.2/g' $env_file2";

system "rm -rf vec_add_sda_2016.2";
system "cp -r $case_dir/vec_add_sda_2016.2 .";
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.2; merlin_create_project run -s src -xilinx";
system "perl -pi -e 's/:3.2/:3.1/g' vec_add_sda_2016.2/run/spec/directive.xml";
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.2; merlin_flow run";
if(-e "vec_add_2016.2/run/pkg/libkernel.so") {
} else {
    system "echo fail sdaccel 2016.2 run >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.2; merlin_flow run host";
if(-e "vec_add_2016.2/run/pkg/host_top") {
} else {
    system "echo fail sdaccel 2016.2 host >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.2; merlin_flow run sim";
if(-e "vec_add_2016.2/run/implement/exec/run_sim/bin/pass.o") {
} else {
    system "echo fail sdaccel 2016.2 sim >> err.log";
}

# run ku115
printf "\nRun ku115\n";
system "rm -rf vec_add_ku115";
system "cp -r $case_dir/vec_add_ku115 .";
system "source $package_dir/settings64.sh; cd vec_add_ku115; merlin_create_project run -s src -xilinx";
system "perl -pi -e 's/xilinx:adm-pcie-ku3:2ddr/xilinx:tul-pcie3-ku115:2ddr/g' vec_add_ku115/run/spec/directive.xml";
system "perl -pi -e 's/:3.2/:3.1/g' vec_add_ku115/run/spec/directive.xml";
system "source $package_dir/settings64.sh; cd vec_add_ku115; merlin_flow run";
if(-e "vec_add_ku115/run/pkg/libkernel.so") {
} else {
    system "echo fail ku115 run >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_ku115; merlin_flow run host";
if(-e "vec_add_ku115/run/pkg/host_top") {
} else {
    system "echo fail ku115 run host >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_ku115; merlin_flow run sim";
if(-e "vec_add_ku115/run/implement/exec/run_sim/bin/pass.o") {
} else {
    system "echo fail ku115 run sim >> err.log";
}
#system "source $package_dir/settings64.sh; cd vec_add_ku115; merlin_flow run bit";
if(-e "vec_add_ku115/run/pkg/kernel_top.xclbin") {
} else {
    system "echo fail ku115 run bit >> err.log";
}

#run sdaccel 2016.1
printf "\nRun on SDAccel 2016.1\n";
system "perl -pi -e 's/SDAccel\\\/201.*/SDAccel\\\/2016.1/g' $env_file1";
system "perl -pi -e 's/SDAccel\\\/201.*/SDAccel\\\/2016.1/g' $env_file2";
system "perl -pi -e 's/SDx\\\/201.*/SDAccel\\\/2016.1/g' $env_file1";
system "perl -pi -e 's/SDx\\\/201.*/SDAccel\\\/2016.1/g' $env_file2";

system "rm -rf vec_add_sda_2016.1";
system "cp -r $case_dir/vec_add_sda_2016.1 .";
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.1; merlin_create_project run -s src -xilinx";
system "perl -pi -e 's/:3.2/:3.0/g' vec_add_sda_2016.1/run/spec/directive.xml";
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.1; merlin_flow run";
if(-e "vec_add_2016.1/run/pkg/libkernel.so") {
} else {
    system "echo fail sdaccel 2016.1 run >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.1; merlin_flow run host";
if(-e "vec_add_2016.1/run/pkg/host_top") {
} else {
    system "echo fail sdaccel 2016.1 host >> err.log";
}
system "source $package_dir/settings64.sh; cd vec_add_sda_2016.1; merlin_flow run sim";
if(-e "vec_add_2016.1/run/implement/exec/run_sim/bin/pass.o") {
} else {
    system "echo fail sdaccel 2016.1 sim >> err.log";
}
