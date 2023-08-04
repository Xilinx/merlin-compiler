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
####################################################################################
#  (c) Copyright 2014-2015 Xilinx, Inc. All rights reserved.
#
####################################################################################

####################################################################################
#  (c) Copyright 2014-2015 Xilinx, Inc. All rights reserved.
#
####################################################################################

####################################################################################
#  (c) Copyright 2014-2015 Xilinx, Inc. All rights reserved.
#
####################################################################################

####################################################################################
#  (c) Copyright 2014-2015 Xilinx, Inc. All rights reserved.
#
####################################################################################

####################################################################################
#  (c) Copyright 2014-2015 Xilinx, Inc. All rights reserved.
#
####################################################################################


    use File::Basename;
    use XML::Simple;
    use Data::Dumper;

$platform = $ARGV[0];
$merlincc = $ARGV[2];
$library_name = $ARGV[3];
$library_path = $ARGV[4];
my $kernel_name = "";
sub read_kernel_name {
#    $xml_file  = "mhs_cfg.xml";
#    my $config_list = XML::Simple->new();
#    my $config_all = $config_list->XMLin($xml_file);
##    print Dumper($config_all); 
#    $kernel_name = $config_all->{node}->{name};
#    $kernel_name =~ s/_top$/_kernel/;
##    print "kernel_name = $kernel_name\n\n\n"; 
#    $kernel_name = $config_all->{node}->{kernel_name};
    $kernel_name = "kernel/kernel_top";
}

$flag = 1;
$version = `aocl version`;
printf "$version\n";
if($version =~ "aocl 15.0.0") {
    $flag = 0;
} elsif($version =~ "aocl 15.1.0") {
    $flag = 0;
} else {
    $flag = 1;
}

if ($ARGV[1] eq "host") {
	system("rm -rf pkg");
	system("mkdir pkg");
	system("cp bin/* pkg");
}
if ($ARGV[1] eq "sim") {
	#system("cp ../../opencl_gen/opencl/mhs_cfg.xml .");
	read_kernel_name; 
	system("aoc --fp-relaxed -march=emulator -g --board $platform $kernel_name.cl $library_name $library_path");
    system("rm -rf bin ; mkdir bin;            >& /dev/null");
	system("cp *.aocx kernel_top_sim.aocx       >& /dev/null");
	system("cp kernel_top_sim.aocx bin/kernel_top.aocx >& /dev/null");
    if($merlincc eq "merlincc") {
    } else {
        system("cp -r host/bin/* bin             >& /dev/null");
        system("cp -r lib_gen/bin/* bin             >& /dev/null");
        system("cp *.dat bin/                       >& /dev/null");
        system("cp *.data bin/                       >& /dev/null");
	    system("cd bin; env CL_CONTEXT_EMULATOR_DEVICE_ALTERA=$platform ./host_top 20 20");
        system("cp bin/*.dat ./                     >& /dev/null");
        system("cp bin/*.data ./                     >& /dev/null");
	    system("mkdir output");
        system("cp bin/*.dat ./output               >& /dev/null");
        system("cp bin/*.data ./output               >& /dev/null");
    }
}
if ($ARGV[1] eq "sim_gdb") {
	#system("cp ../../opencl_gen/opencl/mhs_cfg.xml .");
	read_kernel_name; 
	system(" aoc --fp-relaxed -march=emulator -g --board $platform $kernel_name.cl");
	system(" cp *.aocx kernel_top_sim.aocx      >& /dev/null");
	system(" cp kernel_top_sim.aocx bin/kernel_top.aocx >& /dev/null");
    if($merlincc eq "merlincc") {
    } else {
        system("cp -r host/bin/* bin             >& /dev/null");
        system("cp -r lib_gen/bin/* bin             >& /dev/null");
        system("cp *.dat bin/                       >& /dev/null");
        system("cp *.data bin/                       >& /dev/null");
	    system(" cd bin; env CL_CONTEXT_EMULATOR_DEVICE_ALTERA=$platform gdb --args ./host_top; ");
        system("cp bin/*.dat ./                     >& /dev/null");
        system("cp bin/*.data ./                     >& /dev/null");
	    system("mkdir output");
        system("cp bin/*.dat ./output               >& /dev/null");
        system("cp bin/*.data ./output               >& /dev/null");
    }
}
if ($ARGV[1] eq "estimate") {
	read_kernel_name; 
	system("aoc --fp-relaxed -g -c $kernel_name.cl --report");
	system("mkdir report");
	if($flag == 1 and -e "kernel_top.aoco") {
	    system("aocl analyze-area kernel_top.aoco");
	    system("cp -r *.html report");
	}
	system("cp -r kernel_top/*.log report       >& /dev/null");
	#system("cp -r kernel_top/*.rpt report");
	#system("cp -r kernel_top/*.attrib report");
	system("cp -r kernel_top/*.txt report       >& /dev/null");
	#system("cp -r kernel_top/*.summary report");
	if($flag == 0) {
	    system("cp -r kernel_top/*.area report  >& /dev/null");
	}
}
if ($ARGV[1] eq "bit") {
	read_kernel_name; 
	system("aoc --fp-relaxed -v -g --board $platform $kernel_name.cl -o kernel_top.aocx");
	system("rm -rf pkg");
	system("mkdir pkg");
	system("cp kernel_top.aocx pkg              >& /dev/null");
	system("cp ../host/pkg/host_top pkg         >& /dev/null");
	system("cp Makefile pkg                     >& /dev/null");
	system("cp altera_run_flow.pl pkg           >& /dev/null");
	system("rm -rf report");
	system("mkdir report");
	system("cp kernel_top/*.log report          >& /dev/null");
	system("cp kernel_top/*.rpt report          >& /dev/null");
	system("cp kernel_top/*.summary report      >& /dev/null");
	system("cp kernel_top/*.txt report          >& /dev/null");
	system("cp kernel_top/*.attrib report       >& /dev/null");
	if($flag == 0) {
	    system("cp kernel_top/*.area report     >& /dev/null");
	}
	if($flag == 1 and -e "kernel_top.aoco") {
	    system("aocl analyze-area kernel_top.aoco");
	    system("cp -r *.html report             >& /dev/null");
	}
}
if ($ARGV[1] eq "profile_bit") {
	read_kernel_name; 
	system("aoc --fp-relaxed -v -g --profile $kernel_name.cl -o kernel_top.aocx");
	system("rm -rf pkg");
	system("mkdir pkg");
	system("cp kernel_top.aocx pkg              >& /dev/null");
	system("cp ../host/pkg/host_top pkg         >& /dev/null");
	system("cp Makefile pkg                     >& /dev/null");
	system("cp altera_run_flow.pl pkg           >& /dev/null");
	system("rm -rf report");
	system("mkdir report");
	system("cp kernel_top/*.log report          >& /dev/null");
	system("cp kernel_top/*.rpt report          >& /dev/null");
	system("cp kernel_top/*.summary report      >& /dev/null");
	system("cp kernel_top/*.txt report          >& /dev/null");
	system("cp kernel_top/*.attrib report       >& /dev/null");
	if($flag == 0) {
	    system("cp kernel_top/*.area report     >& /dev/null");
	}
	if($flag == 1) {
	    system("aocl analyze-area kernel_top.aoco");
	    system("cp -r *.html report             >& /dev/null");
	}
}
if ($ARGV[1] eq "test") {
	system(" mkdir fpga_run;");
	system(" mkdir fpga_run/lib_gen;");
	system(" mkdir fpga_run/lib_gen/bin;");
	system(" cp libkernel.so fpga_run/lib_gen/bin;");
	system(" cp libkernel.so fpga_run;");
	system(" cp *.aocx fpga_run;");
	system(" cp host_top fpga_run;");
	system(" cd aux; touch test_tmp");
	system(" cp aux/* fpga_run;");
	system(" rm aux/test_tmp fpga_run/test_tmp");
#    system(" cd fpga_run; aocl program acl0 kernel_top.aocx; ./host_top");
	system(" cd fpga_run; ./host_top");
}
if ($ARGV[1] eq "profile_test") {
	system(" mkdir fpga_run;");
	system(" cp *.aocx fpga_run;");
	system(" cp host_top fpga_run;");
	system(" cd aux; touch test_tmp");
	system(" cp aux/* fpga_run;");
	system(" rm aux/test_tmp fpga_run/test_tmp");
#    system(" cd fpga_run; aocl program acl0 kernel_top.aocx; ./host_top");
	system(" cd fpga_run; ./host_top");
	system(" cd fpga_run; aocl report kernel_top.aocx profile.mon");
}
