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

#---------------------------------------------------------------------------------------------#
# excute before everything
#---------------------------------------------------------------------------------------------#
my $curr_dir;
my $MERLIN_COMPILER_HOME;
my $library_path;
BEGIN {
    $|=1;  #turn on auto reflush for stdout
    $curr_dir = `pwd`;
    chomp($curr_dir);
    $ENV{"MERLIN_CURR_PROJECT"} = $curr_dir;
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use POSIX ":sys_wait_h";
use File::Basename;
use File::Path;
use Sys::Hostname;
use Config;
use Carp qw<longmess>;
use lib $library_path;
use message;
use merlin;
use Time::timegm qw(timegm);
use Cwd 'abs_path';
# suppress smartmatch is experimental
no if $] >= 5.017011, warnings => 'experimental::smartmatch';

#---------------------------------------------------------------------------------------------#
# global variable
#---------------------------------------------------------------------------------------------#
# else run in encyption mode
my $encrypt = "off";
my $evaluate = "off";
my $password = "hanhuisgood";
my $RET_CODE = 0;
# if remained space is limited, give warning or error
my $WARN_SPACE = 5 * 1024 * 1024;
my $EXIT_SPACE = 2 * 1024 * 1024;
# get current working directory
$curr_dir = `pwd`;
chomp($curr_dir);
# some flags
my $debug_info          = 0;
my $debug_level         = 0;
my $keep_prj            = 0;
my $api_level           = 0;
my $log_mode            = 0;
my $is_showed_banner    = 0;  # showed banner only one time when run separate flow or whole flow
my $copy_report         = 0;
# check if directory exist
my $merlincc_vendor_log    = ".Mer";
my $merlincc_prj           = ".merlin_prj";
my $merlincc_src           = ".merlin_prj/src";
my $merlincc_xml           = ".merlin_prj/run/spec/directive.xml";
my $merlincc_export_inside = ".merlin_prj/run/implement/export";
my $merlincc_report        = "merlincc_report";
# check if log and report exist
my $merlin_log             = "merlin.log";
my $merlin_rpt             = "merlin.rpt";
my $impl_rpt               = "summary_impl.rpt";

my $sdaccel = "sdaccel";
my $vitis = "vitis";
my $vivado_hls = "vivado_hls";
my $vitis_hls = "vitis_hls";
my $tool_version_project = "";

# check help and version information
if (@ARGV eq 0) {
    get_help();
    exit;
}
for (my $i = 0; $i < @ARGV; $i++) {
    if($ARGV[$i] eq "-h" or $ARGV[$i] eq "--help" or $ARGV[$i] eq "-help" ) {
        get_help();
        exit;
    }
    if($ARGV[$i] eq "-v" or $ARGV[$i] eq "--version") {
        print get_version_info(0, "not delete prj");
        exit;
    }
    if($ARGV[$i] eq "--debug_report") {
        $copy_report = 1;
    }
}

# run command with special requirement as late as possible -shanh
if (not -w dirname($merlin_log)) {
    printf "Please run merlincc in writeable directory.\n";
    exit 1;
}
system "touch $merlin_log";

# get platform list
if (check_param('--list-platform')) {
    show_platform_info('xilinx');
    exit;
}

# check current directory location
if ($curr_dir =~ /\ /) {
    MSG_E_3071($curr_dir);
    exit;
}

# check required software
check_prerequst_software();
my $env_set_library = check_system();

#check input argument legality
if (is_input_argument_illegal()) {
    $keep_prj = 1;
    get_help();
    exit_on_error();
}
if (is_attribute_illegal()) {
    $keep_prj = 1;
    exit_on_error();
}

# check if need to keep project after execution
if (check_param('--keep_prj')) {
    $keep_prj = 1;
}

# check debuge level
if (check_param('-g'))   { $debug_level  = 1; $debug_info = 1; }
if($encrypt ne "on") {
    if (check_param('-d11')) { $debug_level  = 2; }
    if (check_param('-d22')) { $debug_level  = 3; }
}
#if not use -11 -d22, always use encrypt mode
#only with INT license and -d11, user can use out INT feature
#if($debug_level <= 1) {
#    $encrypt = "on";
#}

# create merlin project execution directory
if (-e "$merlincc_prj" and $keep_prj eq 0) {
    system "rm -rf $merlincc_prj >/dev/null 2>&1";
    if (-e "$merlincc_prj") {
        MSG_E_3039($merlincc_prj);
        exit_on_error();
    }
} elsif (not -e "$merlincc_prj" and $keep_prj eq 1) {
    MSG_E_3040($merlincc_prj);
    exit_on_error();
}

# show banner once before execution
my $banner_flag = "";
my $mco = get_mco_from_args();
if ($mco) {
    if ($keep_prj eq 0) {
        dewrapper_project($mco);
    }
} else {
    $banner_flag = "merlin_synthesis";
}
if (check_param('-syncheck')) {
    show_banner_prof_once($banner_flag);
} else {
    show_banner_once($banner_flag);
}

if (check_param_equal('-march')) {
    my $march_type = get_param_equal('-march');
    if (not $march_type ~~ ['sw_emu', 'hw_emu']) {
        MSG_E_3028();
        exit_on_error();
    }
}

#get tool from commandline, higher priority
#get tool from priject, lower priority
my ($impl_tool, $device_platform);
if ($mco) {
    if(-e "$merlincc_prj/run/spec/directive.xml") {
        ($impl_tool, $device_platform, $tool_version_project) = get_impl_info_from_project($merlincc_prj, "check_env");
        update_impl_info_from_project($merlincc_prj, $impl_tool);
        #($impl_tool, $device_platform, $tool_version_project) = get_impl_info_from_project($merlincc_prj);
    }
} else {
    ($impl_tool, $device_platform) = get_platform_info("first");
}

# get mco file
# if exist, dewrapper, then go on
# if not exist, then do opencl generation
if ($mco) {
#    if ($keep_prj eq 0) {
#        dewrapper_project($mco);
#    }
} elsif (check_param('-c')) {
    if (get_mco_from_args()) {
        MSG_E_3041();
        exit_on_error("init");
    }
    my $need_wrap = 1;
    if (check_param('--lib-only')) {
        $need_wrap = 0;
    }
    my $ouput = "";
    if (check_param('-o')) {
        $output = get_param('-o');
        if (is_illegal_extension($output)) {
            $output = rstrip_illegal_extension($output);
        }
    }

    if (check_param('-syncheck')) {
        do_syncheck();
    } else {
        do_opencl_gen($output);

        if($need_wrap eq 1) {
            if ($output) {
                wrapper_project($merlincc_prj, "$output.mco");
            } else {
                wrapper_project($merlincc_prj, "kernel_top.mco");
            }
        }
        if (not check_param_equal('-march') and not check_param_equal('--report')) {
            exit 0;
        }
    }
} else {
    my $ouput = "";
    if (check_param('-o')) {
        $output = get_param('-o');
        if (is_illegal_extension($output)) {
            $output = rstrip_illegal_extension($output);
        }
    }
    do_opencl_gen($output);
}

# get HLS estimation report
if (check_param_equal('--report')) {
    do_hls();
}
if (check_param('-c') or check_param_equal('--report')) {
    exit 0;
}

# do opencl simulation
if (check_param_equal('-march')) {
    my $march_type = get_param_equal('-march');
    if (not $march_type ~~ ['sw_emu', 'hw_emu']) {
        MSG_E_3028();
        exit_on_error();
    }
    my $output = "";
    if (check_param('-o')) {
        $output = get_param('-o');
        if (is_illegal_extension($output)) {
            $output = rstrip_illegal_extension($output);
        }
    } elsif ($mco and $mco =~ /^(.*)\.mco$/) {
        $output = $1;
    }
    do_sim($output, $march_type, $tool_version_project);
} else {
    my $output = "";
    if (check_param('-o')) {
        $output = get_param('-o');
        if (is_illegal_extension($output)) {
            $output = rstrip_illegal_extension($output);
        }
    }
    do_bit($output, check_param('--profile'));
}

END {
    #after finish all things, release license
    if (-w dirname($merlin_log)) {
        $info = `which rm >/dev/null 2>&1 >.log_tmp`;
        $info = `cat .log_tmp`;
        chomp($info);
        if($info =~ /\S+/) {
            env_clean();
            flex_clean();
            system "rm -rf .log_tmp  >/dev/null";
        }
    }
    exit $RET_CODE;
}
exit 0;

#---------------------------------------------------------------------------------------------#
# copy related file out to .Mer directory
#---------------------------------------------------------------------------------------------#
sub copy_file_out {
    system "rm -rf $merlincc_vendor_log;";
    system "mkdir $merlincc_vendor_log";
#    if($copy_report) {
        if(-e "$merlincc_prj/run/report/.source_opt") {
            system "cp -r $merlincc_prj/run/report/.source_opt $merlincc_vendor_log";
        }
#    }
    if(-e "$merlincc_prj/run/report/kernel_top.log") {
        system "cp -r $merlincc_prj/run/report/kernel_top.log $merlincc_vendor_log";
    }
    if(-e "$merlincc_prj/run/report/quartus_sh_compile.log") {
        system "cp -r $merlincc_prj/run/report/quartus_sh_compile.log $merlincc_vendor_log";
    }
    if(-e "$merlincc_prj/run/report") {
        my $path = `find $merlincc_prj/run/report -name \"vivado_hls.log\"`;
        my @path_lines = split(/\n/, $path);
        foreach my $one_path (@path_lines) {
            if ($one_path =~ /(.*)vivado_hls\.log/) {
                $one_dir = $1;
                system "cp -r $one_dir $merlincc_vendor_log";
            }
        }
        $path = `find $merlincc_prj/run/report -name \"*.zip\"`;
        @path_lines = split(/\n/, $path);
        foreach my $one_path (@path_lines) {
            if(-e $one_path) {
                system "cp -r $one_path .";
            }
        }
    }
    if(-e "$merlincc_prj/run/report/system_estimate.xtxt") {
        system "cp -r $merlincc_prj/run/report/system_estimate.xtxt $merlincc_vendor_log";
    }
    if(-e "$merlincc_prj/run/report/vivado.log") {
        system "cp -r $merlincc_prj/run/report/vivado.log $merlincc_vendor_log";
    }
    if(-e "$merlincc_prj/run/report/merlin_sdaccel.log") {
        system "cp -r $merlincc_prj/run/report/merlin_sdaccel.log $merlincc_vendor_log";
    }
    if(-e "$merlincc_prj/run/report/message_report.log") {
        system "cp -r $merlincc_prj/run/report/message_report.log $merlincc_vendor_log";
    }
    if(-e "$merlincc_prj/run/report/total_message.log") {
        system "cp -r $merlincc_prj/run/report/total_message.log $merlincc_vendor_log";
    }
    system "cp -r $merlincc_prj/run/report/*_cosim.rpt . >/dev/null 2>&1";
    system "cp -r $merlincc_prj/run/report/*_csim.log . >/dev/null 2>&1";
    system "cp -r $merlincc_prj/run/report/*_export.rpt . >/dev/null 2>&1";
}

#---------------------------------------------------------------------------------------------#
# wrapper project to mco file
#---------------------------------------------------------------------------------------------#
sub wrapper_project {
    my ($src, $dst) = @_;
    if(-e $src) {
        if ($debug_level <= 2) {
            system "rm -rf $src/run/implement/exec/*";
        }
        if($encrypt eq "on") {
            #system "tar -zcf - $src | openssl des3 -salt -k $password | dd of=$dst >/dev/null 2>&1"
            system "zip -rP $password $dst.zip $src >/dev/null 2>&1";
            system "mv $dst.zip $dst";
        } else {
            system "tar -zcvf $dst $src >/dev/null 2>&1";
        }
    } else {
        printf "ERROR: Failed to wrapper project.\n";
        exit;
    }
}

#---------------------------------------------------------------------------------------------#
# dewrapper mco file
#---------------------------------------------------------------------------------------------#
sub dewrapper_project {
    my ($src) = @_;
    if(-e $src) {
        system "rm -rf $merlin_prj";
        if($encrypt eq "on") {
            #system "dd if=$src | openssl des3 -d -k $password | tar zxf - >/dev/null 2>&1";
            system "cp $src $src.zip";
            system "unzip -P $password $src.zip >/dev/null 2>&1";
            system "rm -rf $src.zip";
            if(not -e $merlin_prj) {
                system "tar -zxvf $src >/dev/null 2>&1";
            }
        } else {
            system "tar -zxvf $src >/dev/null 2>&1";
            # if can not detect project, try to dewrapper with encrypt mode
            if(not -e $merlin_prj) {
                system "cp $src $src.zip";
                system "unzip -P $password $src.zip >/dev/null 2>&1";
                system "rm -rf $src.zip";
            }
        }
    } 
    if(not -e $src or not -e ".merlin_prj") {
        printf "ERROR: Failed to dewrapper project.\n";
        exit;
    }
}

#---------------------------------------------------------------------------------------------#
# opencl code generation
#---------------------------------------------------------------------------------------------#
sub do_opencl_gen {
    my ($output) = @_;
    if($output =~ /^(.*)\/(.*)$/) {
        $new_h = "$1/__merlin$2.h";
        $new_so = "$1/lib$2.so";
        if(! -e $1) {
            MSG_E_3043($1);
            exit_on_error();
        }
    } elsif($output =~ /\S+/) {
        $new_h = "__merlin$output.h";
        $new_so = "lib$output.so";
    } else {
        $new_h = "__merlinkernel_top.h";
        $new_so = "libkernel.so";
    }
    create_project();
    if(-e ".error") {
        system "rm -rf .error";
        exit_on_error();
    }
    if (is_auto_dse_turn_on()) {
        check_space($WARN_SPACE, $EXIT_SPACE);
    }
    my $need_so = "yes";
    if($impl_tool ~~ [$vivado_hls, $vitis_hls]) {
        $need_so = "no";
    }
    my $tool = $impl_tool;
    if($impl_tool ~~ [$vitis, $sdaccel, $vivado_hls, $vitis_hls]) {
        $tool = $sdaccel
    }
    $ret = system "cd $merlincc_prj; $env_set_library merlin_flow run -keep_platform $tool |& tee -a ../$merlin_log";
    # check stack size error
    my $log_info = `cat $merlin_log`;
    if($log_info =~ /(setrlimit returned result.*)\n/) {   
        print "\nERROR: [MERCC-3062] $1. This may related to the setting of ulimit stack size.\n";
    }
    if(-e "$merlincc_prj/run/implement/export/kernel") {
        system "cp $merlincc_prj/run/pkg/__merlinhead_*.h $new_h 2> /dev/null";
        if(-e "$merlincc_prj/run/pkg/__merlintask_top.h") {
            system "cp $merlincc_prj/run/pkg/__merlintask_top.h $new_h";
        }
        if(-e $new_h and $api_level eq 0) {
            system "perl -pi -e 's/extern\.\*__merlin_wait_\.\*\n//g'            $new_h 2> /dev/null";
            system "perl -pi -e 's/extern\.\*__merlin_init_\.\*\n//g'            $new_h 2> /dev/null";
            system "perl -pi -e 's/extern\.\*__merlin_release_\.\*\n//g'         $new_h 2> /dev/null";
            system "perl -pi -e 's/extern\.\*__merlinwrapper_\.\*\n//g'          $new_h 2> /dev/null";
            system "perl -pi -e 's/extern\.\*__merlin_execute_\.\*\n//g'         $new_h 2> /dev/null";
            system "perl -pi -e 's/extern\.\*__merlin_write_buffer_\.\*\n//g'    $new_h 2> /dev/null";
            system "perl -pi -e 's/extern\.\*__merlin_read_buffer_\.\*\n//g'     $new_h 2> /dev/null";
        }
        if(-e "$merlincc_prj/run/implement/code_transform/.opencl_host_generation_disable_tag") {
            $need_so = "no";
        }
		if (!is_sycl_turn_on() and $need_so eq "yes") {
          if(-e "$merlincc_prj/run/pkg/libkernel.so") {
              system "cp $merlincc_prj/run/pkg/*.so $new_so";
          } else {
              copy_file_out;
              MSG_E_3001($impl_tool);
              exit_on_error();
          }
          prepend_file("$new_h", get_version_info(1));
		}
    } else {
        copy_file_out;
        MSG_E_3044();
        exit_on_error();
    }
}

#---------------------------------------------------------------------------------------------#
# set output file name
#---------------------------------------------------------------------------------------------#
sub get_impl_file_name {
    my($src) = @_;
    my($impl_tool) = "";
    if($simrun ne 1 and $bitrun ne 1) {
        $impl_tool = get_value_from_xml("implementation_tool", "$src/run/spec/directive.xml");
    }
    if($impl_tool ~~ [$vitis, $sdaccel, $vitis_hls, $vivado_hls]) {
        return ("kernel_top.xclbin", "xclbin")
    } else {
        MSG_E_3025($impl_tool);
        exit_on_error();
    }
}

#---------------------------------------------------------------------------------------------#
# check if command line have platform information
#---------------------------------------------------------------------------------------------#
sub is_impl_info_exist_in_cmd {
    my $is_platform = check_param_equal('--platform');
    my $is_p = check_param_equal('-p');
    return $is_platform || $is_p;
}

#---------------------------------------------------------------------------------------------#
# estimation 
#---------------------------------------------------------------------------------------------#
sub do_hls {
    system "cd $merlincc_prj; $env_set_library merlin_flow run hls -keep_platform $impl_tool |& tee -a ../$merlin_log";
    copy_file_out;
    if(-e "$merlincc_prj/run/report/merlin.rpt") {
        system "cp $merlincc_prj/run/report/merlin.rpt $merlin_rpt 2>/dev/null";
        prepend_file($merlin_rpt, get_version_info(0));
    } else {
        MSG_E_3006();
        exit_on_error();
    }
}

#---------------------------------------------------------------------------------------------#
# software simulation(For Xilinx and Intel)
# hardware simulation(Only for Xilinx)
#---------------------------------------------------------------------------------------------#
sub do_sim {
    my ($output_file, $sim_type, $tool_version) = @_;
    my ($impl_file, $suffix) = get_impl_file_name($merlincc_prj);
    my $run_sim = "run_sim";
    if (not $sim_type or $sim_type eq "sw_emu") {
        system "cd $merlincc_prj; $env_set_library merlin_flow run sim_config -merlincc -keep_platform $impl_tool |& tee -a ../$merlin_log";
    } elsif ($sim_type eq "hw_emu") {
        $run_sim = "hw_run_sim";
        system "cd $merlincc_prj; $env_set_library merlin_flow run hw_sim_config -merlincc -keep_platform $impl_tool |& tee -a ../$merlin_log";
    }
    if($tool_version ~~ ["vivado_hls", "vitis_hls"]) {
        copy_file_out;
    } else {
        copy_file_out;
        if(! -e "$merlincc_prj/run/implement/exec/$run_sim/$impl_file") {
            MSG_E_3047();
            exit_on_error();
        } else {
            if($output_file eq "") {
                system "cp -r $merlincc_prj/run/implement/exec/$run_sim/$impl_file .";
            } else {
                system "cp -r $merlincc_prj/run/implement/exec/$run_sim/$impl_file $output_file.$suffix";
            }
            if(-e "$merlincc_prj/run/pkg/emconfig.json") {
                system "cp $merlincc_prj/run/pkg/emconfig.json . 2>/dev/null";
            }
        }
    }
}

#---------------------------------------------------------------------------------------------#
# bitstream generation
#---------------------------------------------------------------------------------------------#
sub do_bit{
    check_space($WARN_SPACE, $EXIT_SPACE);
    my ($output_file, $is_profile) = @_;
    my ($impl_file, $suffix) = get_impl_file_name($merlincc_prj);
    if ($is_profile) {
        system "cd $merlincc_prj; $env_set_library merlin_flow run bit -keep_platform --profile_bit $impl_tool |& tee -a ../$merlin_log";
    } else {
        system "cd $merlincc_prj; $env_set_library merlin_flow run bit -keep_platform $impl_tool |& tee -a ../$merlin_log";
    }
    copy_file_out;
    if (! -e "$merlincc_prj/run/pkg/$impl_file") {
        MSG_E_3048();
        exit_on_error();
    } else {
        my $prefix = "";
        if ($output_file eq "") {
            system "cp -r $merlincc_prj/run/pkg/$impl_file .";
            if($impl_file =~ /(.*)\.aocx/) {
                $prefix = $1;
            } elsif($impl_file =~ /(.*)\.xclbin/) {
                $prefix = $1;
            }
        } else {
            system "cp -r $merlincc_prj/run/pkg/$impl_file $output_file.$suffix";
                $prefix = $output_file;
        }
        #solved for dcp, copy gbs file out  FIXME
        if(-e "$merlincc_prj/run/pkg/afu_fit.gbs") {
            system "cp $merlincc_prj/run/pkg/afu_fit.gbs $prefix.gbs";
        }
#        if(-e "$merlincc_prj/run/report/summary_impl.rpt") {
#            system "cp $merlincc_prj/run/report/summary_impl.rpt $impl_rpt 2>/dev/null";
#        } else {
#            MSG_E_3049();
#            exit_on_error();
#        }
    }
}

#---------------------------------------------------------------------------------------------#
# Check output file name extension name
#---------------------------------------------------------------------------------------------#
sub is_illegal_extension {
    my ($name) = @_;
    if ($name =~ /.*\.mco$/)    { return 1; }
    if ($name =~ /.*\.aocx$/)   { return 1; }
    if ($name =~ /.*\.xclbin$/) { return 1; }
}

sub rstrip_illegal_extension {
    my ($name) = @_;
    my $ret = $name;
    while (is_illegal_extension($ret)) {
        if ($ret =~ /(.*)\.mco$/)       { $ret = $1; }
        if ($ret =~ /(.*)\.aocx$/)      { $ret = $1; }
        if ($ret =~ /(.*)\.xclbin$/)    { $ret = $1; }
    }
    return $ret;
}

#---------------------------------------------------------------------------------------------#
# show banner 
#---------------------------------------------------------------------------------------------#
sub show_banner_once {
    my $syn_flag = 0;
    if($_[0] eq "merlin_synthesis") {
        $syn_flag = 1;
    }
    if (not $is_showed_banner) {
        my $log_warning;
#        if($syn_flag eq 1) {
#            #only when do systhesis, will print this information
#            $log_warning = get_append_log_info();
#        }
        my $version = get_version_info();
        my $misc    = get_misc_info();
        print($version);
        print($misc);
        print($log_warning);
        #only when do systhesis, will rewrite a new log
        if($log_mode eq 1 or $syn_flag eq 0) {
            open(myfile,">>$merlin_log");
        } else {
            open(myfile,">$merlin_log");
        }
        print myfile $log_warning;
        print myfile $version;
        print myfile $misc;
        close(myfile);
        $is_showed_banner = 1;
    }
}

#---------------------------------------------------------------------------------------------#
# show prof banner 
#---------------------------------------------------------------------------------------------#
sub show_banner_prof_once {
    my $syn_flag = 0;
    if($_[0] eq "merlin_synthesis") {
        $syn_flag = 1;
    }
    if (not $is_showed_banner) {
        my $log_warning;
#        if($syn_flag eq 1) {
#            #only when do systhesis, will print this information
#            $log_warning = get_append_log_info();
#        }
        my $version = get_version_prof_info();
        my $misc    = get_misc_info();
        print($version);
        print($misc);
        print($log_warning);
        #only when do systhesis, will rewrite a new log
        if($log_mode eq 1 or $syn_flag eq 0) {
            open(myfile,">>$merlin_log");
        } else {
            open(myfile,">$merlin_log");
        }
        print myfile $log_warning;
        print myfile $version;
        print myfile $misc;
        close(myfile);
        $is_showed_banner = 1;
    }
}

#---------------------------------------------------------------------------------------------#
# get moc files from commandline
#---------------------------------------------------------------------------------------------#
sub get_mco_from_args{
    for (my $i = 0; $i < @ARGV; ++$i) {
        if ($ARGV[$i] =~ /.*\.mco/ and -e $ARGV[$i]) {
            if ($i > 0 and $ARGV[$i - 1] ne '-o') {
                return $ARGV[$i];
            } elsif ($i == 0) {
                return $ARGV[$i];
            }
        }
    }
    return "";
}
        
#---------------------------------------------------------------------------------------------#
# Check if valid xpfm file
#---------------------------------------------------------------------------------------------#
sub check_xpfm {
    my ($device_platform, $platform_support) = @_;
    if (-e $device_platform) {
        if ($device_platform !~ /^\//) {
            $device_platform = "$curr_dir\/$device_platform";
        }
        return $device_platform;
    } else {
        MSG_E_3074($device_platform, $platform_support);
        exit_on_error();
    }
    return $device_platform;
}

#---------------------------------------------------------------------------------------------#
# Check if implementation platform exist
#---------------------------------------------------------------------------------------------#
sub update_platform {
    my ($impl_tool, $device_platform) = @_;
    if(-e "$ENV{'PLATFORM_REPO_PATHS'}") {
        my $platform_all = `ls $ENV{'PLATFORM_REPO_PATHS'}`;
        my @platform_set = split(/\n/, $platform_all);
        foreach my $one_platform (@platform_set) {
            if ($one_platform eq $device_platform) {
                return "$ENV{'PLATFORM_REPO_PATHS'}/$one_platform/$one_platform.xpfm";
            }
        }
    }
    return $device_platform;
}

sub check_impl_tool_exist {
    my ($impl_tool, $device_platform) = @_;
    if ($device_platform =~ /.*\.xpfm/) {
        $device_platform = check_xpfm($device_platform, $platform_support);
        return 1;
    }
    if($impl_tool eq $vitis) {
        my $pass_flag = 0;
        my $platform_support = "";
        my $platform_all = `ls $ENV{'XILINX_VITIS'}/platforms`;
        my @platform_set = split(/\n/, $platform_all);
        my $vitis_version = get_vitis_version();
        foreach my $one_platform (@platform_set) {
            if ($vitis_version =~ 2019.2) {
                if ($one_platform eq $device_platform) {
                    $pass_flag = 1;
                }
            }
            $platform_support .= "$one_platform\n";
        }
        if($pass_flag eq 0) {
            MSG_E_3037($device_platform, $platform_support);
            exit_on_error();
        }
    }
    if($impl_tool eq $sdaccel) {
        my $pass_flag = 0;
        my $platform_support = "";
        my $platform_all = `ls $ENV{'XILINX_SDX'}/platforms`;
        my @platform_set = split(/\n/, $platform_all);
        my $xocc_version = get_xocc_version();
        foreach my $one_platform (@platform_set) {
            if ($xocc_version =~ 2017.1 or $xocc_version =~ 2017.2 or $xocc_version =~ 2017.3) {
                if ($one_platform =~ /^xilinx_(.*)_(\d*ddr\S*)_(\d+)_(\d+)$/) {
                    $one_platform = "xilinx:$1:$2:$3\.$4";
                    if ($one_platform eq $device_platform) {
                        $pass_flag = 1;
                    }
                }
            } else {
                if ($one_platform eq $device_platform) {
                    $pass_flag = 1;
                }
            }
            $platform_support .= "$one_platform\n";
        }
        if($pass_flag eq 0) {
            MSG_E_3037($device_platform, $platform_support);
            exit_on_error();
        }
    }
}

#---------------------------------------------------------------------------------------------#
#get platform information from command line
#---------------------------------------------------------------------------------------------#
sub get_platform_info {
    my $ignore_none = $_[0];
    my ($impl_tool, $device_platform);
    my $used_device = "";
    if (!check_param_equal("--platform") && !check_param_equal("-p")) {
        #if($ignore_none ne "") {
        #    MSG_E_3038();
        #    exit_on_error();
        #}
        # if without platform setting, use xilinx as default platform
        $used_device = "sdaccel::auto";
        if(-e "$ENV{'XILINX_VITIS'}") {
            $used_device = "vitis::auto";
        } elsif(-e "$ENV{'XILINX_SDX'}") {
            $used_device = "sdaccel::auto";
        } else {
            $used_device = "vivado_hls::auto";
        }
    } else {
        if(check_param_equal("--platform")){
            $used_device = get_param_equal("--platform");
        } elsif(check_param_equal("-p")){
            $used_device = get_param_equal("-p");
        }
    }
    if($used_device){
        my @ret = extract_impl_info_from_str($used_device);
        if (@ret) {
            ($impl_tool, $device_platform) = @ret;
            if($ignore_none eq "first") {
                MSG_I_1041($device_platform);
            }
            return ($impl_tool, $device_platform);
        } else {
            MSG_E_3027();
            exit_on_error();
        }
    }
}

#---------------------------------------------------------------------------------------------#
# set implementation platform information from commandlines
#---------------------------------------------------------------------------------------------#
sub extract_impl_info_from_str {
    my ($s) = @_;
    if ($s =~ /(\S+)::(\S+)/) {
        my $impl_tool = $1;
        my $device_platform = $2;
        if($device_platform eq "auto") {
            if($impl_tool ~~ [$vitis, $sdaccel]) {
                my $env_device = $ENV{'MERLIN_AUTO_DEVICE_XILINX'};
                if($env_device ne "") {
                    $device_platform = $env_device;
                    #printf("Use device from MERLIN_AUTO_DEVICE_XILINX environment: $device_platform\n");
                } else {
                    #$device_platform = "xilinx_vcu1525_dynamic_5_1";i
                    $device_platform = "xilinx_u250_xdma_201830_2";
                    $device_platform = "/proj/xbuilds/2020.2_daily_latest/xbb/dsadev/opt/xilinx/platforms/xilinx_u250_xdma_201830_3/xilinx_u250_xdma_201830_3.xpfm";
                    #printf("Use device from auto setting: $device_platform\n");
                }
            } elsif($impl_tool ~~ [$vivado_hls, $vitis_hls]) {
                $device_platform = "xcu250-figd2104-2L-e";
            }
        }

        ($impl_tool, $device_platform) = convert_platform_name_for_2017_4($impl_tool, $device_platform);
        if ($device_platform =~ /.*\.xpfm/) {
            $device_platform = check_xpfm($device_platform, $platform_support);
        }
        return ($impl_tool, $device_platform);
    } elsif ($s =~ /(.*\.xpfm)/) {
        my $impl_tool = $vitis;
        if(-e "$ENV{'XILINX_VITIS'}") {
            $impl_tool = $vitis;
        } elsif(-e "$ENV{'XILINX_SDX'}") {
            $impl_tool = $sdaccel;
        }
        my $device_platform = $1;
        $device_platform = check_xpfm($device_platform, $platform_support);
        return ($impl_tool, $device_platform);
    } elsif ($s =~ /(^\S+$)/) {
        my $impl_tool = $vitis;
        if(-e "$ENV{'XILINX_VITIS'}") {
            $impl_tool = $vitis;
        } elsif(-e "$ENV{'XILINX_SDX'}") {
            $impl_tool = $sdaccel;
        } else {
            $impl_tool = $hls;
        }
        my $device_platform = $1;
        return ($impl_tool, $device_platform);
    }
    return ();
}

#---------------------------------------------------------------------------------------------#
# convert platform name to absolute directory name
#---------------------------------------------------------------------------------------------#
sub convert_platform_name_for_2017_4 {
    my ($impl_tool, $device_platform) = @_;
    my $org_dir = "";
    if(-e "$ENV{'XILINX_VITIS'}") {
        $org_dir = "$ENV{'XILINX_VITIS'}";
    } elsif(-e "$ENV{'XILINX_SDX'}") {
        $org_dir = "$ENV{'XILINX_SDX'}";
    }
    my $compatiable_dir = "";
    if(-e "$ENV{'XILINX_SDX_COMPATIABLE'}") {
        $compatiable_dir = "$ENV{'XILINX_SDX_COMPATIABLE'}";
    }
    my $xocc_version = get_xocc_version();
    if ((($xocc_version eq "2017.4" or $xocc_version eq "2018.1" or $xocc_version eq "2018.2") and
            $impl_tool eq $sdaccel) or ($impl_tool eq $vitis)) {
        if ($device_platform =~ /^(.*)xilinx:(.*)\:(\d*ddr\S*)\:(\d+)\.(\d+)$/) {
            $device_platform = "xilinx_$2_$3_$4_$5";
        }
        if(-e "$org_dir/platforms/$device_platform/$device_platform.xpfm") {
            $device_platform = "$org_dir/platforms/$device_platform/$device_platform.xpfm";
        } elsif(-e "$compatiable_dir/platforms/$device_platform/$device_platform.xpfm") {
            $device_platform = "$compatiable_dir/platforms/$device_platform/$device_platform.xpfm";
        }
    }
    return ($impl_tool, $device_platform);
}

#---------------------------------------------------------------------------------------------#
# show platform information when use --list-platform
#---------------------------------------------------------------------------------------------#
sub show_platform_info {
    $keep_prj = 1;
    my $platform = @_[0];
    if ("xilinx" eq $platform and check_xocc()) {
        my $platform_all = `ls $ENV{'XILINX_SDX'}/platforms`;
        my @platform_set = split(/\n/, $platform_all);
        MSG_I_1038();
        my $xocc_version = get_xocc_version();
        if ($xocc_version eq "2017.4" or $xocc_version eq "2018.1" or $xocc_version eq "2018.2"){
            foreach my $one_platform (@platform_set) {
                printf_and_append($merlin_log, "sdaccel::$one_platform\n");
            }
            my $compatiable_dir = "/curr/software/Xilinx/SDx/2017.1";
            if(-e "$ENV{'XILINX_SDX_COMPATIABLE'}") {
                $compatiable_dir = "$ENV{'XILINX_SDX_COMPATIABLE'}";
            }
            my $platform_all_comp = `ls $compatiable_dir/platforms`;
            my @platform_set_comp = split(/\n/, $platform_all_comp);
            foreach my $one_platform (@platform_set_comp) {
                if($one_platform =~ /^xilinx/) {
                    printf_and_append($merlin_log, "sdaccel::$one_platform\n");
                }
            }
        } else {
            foreach my $one_platform (@platform_set) {
                if ($one_platform =~ /^xilinx_(.*)_(\d*ddr\S*)_(\d+)_(\d+)$/) {
                    printf "sdaccel::xilinx:$1:$2:$3.$4\n";
                    printf_and_append($merlin_log, "sdaccel::xilinx:$1:$2:$3.$4\n");
                }
            }
        }
        printf "\n";
    } elsif ("xilinx" eq $platform and check_vitis()) {
        my $platform_all = `ls $ENV{'XILINX_VITIS'}/platforms`;
        my @platform_set = split(/\n/, $platform_all);
        MSG_I_1038();
        my $vitis_version = get_vitis_version();
        if ($vitis_version eq "2019.2"){
            foreach my $one_platform (@platform_set) {
                printf_and_append($merlin_log, "vitis::$one_platform\n");
            }
        }
        printf "\n";
    }
}

#---------------------------------------------------------------------------------------------#
# check if attribute is legal
#---------------------------------------------------------------------------------------------#
sub is_attribute_illegal {
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] =~ /^(-.*)$/) {
            my $option_single = $1;
            if ($1 eq "--attribute") {
                if($ARGV[$i+1] =~ /(\S+)=(\S+)/) {
                    my $key_attribute = $1;
                    my @key_set = get_key_set();
                    if (! grep { $_ eq $key_attribute } @key_set) {
                        MSG_E_3035($key_attribute);
                        return 1;
                    }
                } elsif($ARGV[$i+1] !~ /(\S+)=(\S+)/) {
                    MSG_E_3076("--attribute $ARGV[$i+1]");
                    return 1;
                }
            }
        }
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# check if input command line argument legal
#---------------------------------------------------------------------------------------------#
sub is_input_argument_illegal {
    my $flag = 0;
    my $is_source_exist;
    my $args_string = "";
    for(my $i=0; $i<@ARGV; $i++) {
        $args_string .= "$ARGV[$i]";
    }
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] =~ /^(-.*)$/) {
            #if start with "-", detect if legal
            my $option_single = $1;
            if($1 =~ /-\d+$/) {
            } elsif($1 =~ / /) {
            } elsif($1 eq "-v") {
            } elsif ($1 eq "--debug_report") {
            } elsif ($1 =~ /--platform=\S+/) {
            } elsif ($1 =~ /-p=\S+/) {
            } elsif ($1 eq "--platform" or $1 eq "-p")  {
                MSG_E_3027();
                exit_on_error();
            } elsif ($1 eq "-c") {
            } elsif ($1 eq '--lib-only') {
            } elsif ($1 eq '--api') {
            } elsif ($1 eq "-o") {
            } elsif ($1 eq "-o") {
            } elsif ($1 eq '-d11') {
            } elsif ($1 eq '-d22') {
            } elsif ($1 eq '-g') {
            } elsif ($1 eq '-w') {
            } elsif ($1 eq "-funsafe-math-optimizations") {
            } elsif ($1 eq "-o1" or
                $1 eq "-o2" or
                $1 eq "-o3" or
                $1 eq "-O1" or
                $1 eq "-O2" or
                $1 eq "-O3" or
                $1 eq '-o0' or
                $1 eq '-O0') {
            } elsif ($1 eq "-I") {
            } elsif ($1 eq "-L") {
            } elsif ($1 =~ /-I\S+/) {
            } elsif ($1 eq "-L\S+") {
            } elsif ($1 eq "--attribute") {
            } elsif ($1 =~ /^-D\S*/) {
            } elsif ($1 =~ /^-march=\S*/) {
            } elsif ($1 eq "-march") {
                MSG_E_3028();
                exit_on_error();
            } elsif ($1 =~ /^--report=\S*/) {
                if (check_param_equal('--report')) {
                    my $report_type = get_param_equal('--report');
                    if (not $report_type) {
                        MSG_E_3029();
                        exit_on_error();
                    } elsif ($report_type ne "estimate" and $report_type ne "syn" and $report_type ne "impl") {
                        MSG_E_3029();
                        exit_on_error();
                    }
                }
            } elsif ($1 eq "--report") {
                MSG_E_3029();
                exit_on_error();
            } elsif ($1 eq "--profile") {
            } elsif ($1 eq "--vendor-options") {
            } elsif ($args_string =~ /--vendor-options\s*$option_single/) {
            } elsif ($1 eq "--tb") {
            } elsif ($1 eq "--keep_prj") {
            } elsif ($1 eq "--kernel_frequency") {
            } elsif ($1 eq "--cflags") {
            } elsif ($1 eq "--ldflags") {
            } elsif ($1 eq "--appendlog") {
            } elsif ($1 eq "--argv") {
                $log_mode = 1;
            } elsif ($1 eq "-syncheck"){ # Min: support for synchk
            } else {
                MSG_E_3030($option_single);
                $flag = 1;
            }
        } else {
            #if the first argument, just check if source code file
            #if not first argument
            #    if have some specific arguments before, do nothing
            #    if not have some specific arguments before, check if source code file
            if ($ARGV[$i] =~ /\ / and $ARGV[$i-1] ne "--cflags" and $ARGV[$i-1] ne "--ldflags" and $ARGV[$i-1] ne "--argv" and $ARGV[$i-1] ne "--tb") {
                MSG_E_3070($ARGV[$i]);
                exit;
            }
            if($i eq 0) {
                if (is_source_file($ARGV[0])) {
                    if(not -e $ARGV[0]) {
                        MSG_E_3031($ARGV[0]);
                        $flag = 1;
                    } elsif (is_source_filename_illegal($ARGV[0])) {
                        MSG_E_3032($ARGV[0]);
                        $flag = 1;
                    }
                } elsif ($ARGV[0] =~ /.*\.mco$/) {
                    if (not -e $ARGV[0]) {
                        MSG_E_3031($ARGV[0]);
                        $flag = 1;
                    }
                } else {
                    MSG_E_3030($ARGV[0]);
                    $flag = 1;
                }
            } else {
                if ($ARGV[$i-1] eq "-I" or
                    $ARGV[$i-1] eq "-L" or
                    $ARGV[$i-1] eq "-o" or
                    $ARGV[$i-1] eq "--kernel_frequency" or
                    $ARGV[$i-1] eq "--cflags" or
                    $ARGV[$i-1] eq "--ldflags" or
                    $ARGV[$i-1] eq "--argv" or
                    $ARGV[$i-1] eq "--attribute" or
                    $ARGV[$i-1] eq "--vendor-options" or
                    $ARGV[$i-1] eq "--tb" or
                    $ARGV[$i-1] eq "-D") {
                } else {
                    if (is_source_file($ARGV[$i])) {
                        if(not -e $ARGV[$i]) {
                            MSG_E_3031($ARGV[$i]);
                            $flag = 1;
                        } elsif (is_source_filename_illegal($ARGV[$i])) {
                            MSG_E_3032($ARGV[$i]);
                            $flag = 1;
                        }
                    } elsif ($ARGV[$i] =~ /.*\.mco$/) {
                        if (not -e $ARGV[$i]) {
                            MSG_E_3031($ARGV[$i]);
                            $flag = 1;
                        }
                    } else {
                        MSG_E_3030($ARGV[$i]);
                        $flag = 1;
                    }
                }
            }
        }
    }
    check_dup_option_illegal("-o");
    check_dup_option_illegal("--report");
    check_dup_equal_option_illegal("--platform", "-p");
    check_dup_equal_option_illegal("-march");
    if (not is_source_file_exist() and not get_mco_from_args()) {
        MSG_E_3033();
        $flag = 1;
    }
    #if (is_source_file_exist() and get_mco_from_args()) {
    #    MSG_E_3034();
    #    $flag = 1;
    #}
    if($flag) {
        MSG_I_1016();
    }
    return $flag;
}

#---------------------------------------------------------------------------------------------#
# check input source file name
#---------------------------------------------------------------------------------------------#
sub is_source_filename_illegal {
    my ($filename) = @_;
    # FIXME kernel is easy to trigger error
    if ($filename =~ /^kernel\.(c|cpp|cxx|cc|C|c\+\+)/) {
        return 1;
    } elsif ($filename =~ /^kernel_top.*\.(c|cpp|cxx|C|cc|c\+\+)/) {
        return 1;
    } elsif ($filename =~ /^rose_.*\.(c|cpp|cxx|C|cc|c\+\+)/) {
        return 1;
    } elsif ($filename =~ /^__merlin.*\.(c|cpp|cxx|C|cc|c\+\+)/) {
        return 1;
    } elsif ($filename =~ /^opencl_if.*\.(c|cpp|cxx|C|cc|c\+\+)/) {
        return 1;
    } elsif ($filename =~ /^opencl_kernel_buffer.*\.(c|cpp|cxx|C|cc|c\+\+)/) {
        return 1;
    } elsif ($filename =~ /^task_top.*\.(c|cpp|cxx|C|cc|c\+\+)/) {
        return 1;
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# check duplicate options for --key=value
#---------------------------------------------------------------------------------------------#
sub check_dup_equal_option_illegal {
    my ($flag) = @_;
    my $value = "";
    for (my $i = 0; $i < @ARGV; ++$i) {
        for my $f (@_) {
            if($ARGV[$i] =~ /^$f=(\S+)/) {
                if (not $1) {
                    MSG_E_3011($flag);
                }
                if (not $value) {
                    $value = $1;
                } else {
                    if ($value ne $1) {
                        MSG_E_3026($flag, $value, $1);
                        exit_on_error();
                    }
                }
            }
        }
    }
}

#---------------------------------------------------------------------------------------------#
# check duplicate options for --key value
#---------------------------------------------------------------------------------------------#
sub check_dup_option_illegal {
    my ($flag) = @_;
    my $value = "";
    for (my $i = 0; $i < @ARGV; ++$i) {
        if ($ARGV[$i] ne $flag) {
            next;
        }
        if($i + 1 >= @ARGV or $ARGV[$i + 1] =~ /^-.*/) {
            MSG_E_3011($flag);
            exit_on_error();
        }
        if (not $value) {
            $value = $ARGV[$i + 1];
        } else {
            if ($value eq $ARGV[$i + 1]) {
                ++$i;
            } else {
                MSG_E_3026($flag, $value, $1);
                exit_on_error();
            }
        }
    }
}

#---------------------------------------------------------------------------------------------#
# check if input argument is source code
#---------------------------------------------------------------------------------------------#
sub is_source_file {
    my ($name) = @_;
    if ($name =~ /.*\.(c|cpp|c\+\+|cc|C|cxx|h|hh|H|hxx|hpp|h\+\+)$/) {
        return 1;
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# check if  source code file exist
#---------------------------------------------------------------------------------------------#
sub is_source_file_exist {
    #my $have_tb = 0;
    #for my $f (@ARGV) {
    #    if ($f =~ /--tb/) {
    #        $have_tb = 1;
    #    }
    #}
    #if($have_tb == 0) {
        for my $f (@ARGV) {
            if ($f =~ /.*\.(c|cpp|c\+\+|cc|C|cl|cxx|h|hh|H|hxx|hpp|h\+\+)$/) {
                return 1;
            }
        }
    #}
    return 0;
}

#---------------------------------------------------------------------------------------------#
# check if -l options legal
#---------------------------------------------------------------------------------------------#
sub check_params_l {
    my ($flag) = @_;
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] =~ /^$flag/) {
            return 1;
        }
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# get -l options value
#---------------------------------------------------------------------------------------------#
sub get_params_l {
    my ($flag) = @_;
    my $params = " ";
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] =~ /^${flag}$/) {
            my $l_name = $ARGV[$i+1];
            $params .= "${flag}${l_name} ";
        } elsif($ARGV[$i] =~ /^${flag}(.*)/) {
            $params .= "${flag}$1 ";
        }
    }
    return $params;
}

#---------------------------------------------------------------------------------------------#
# check "-key value" options value legality
# this key may write multiple times, like -I -L
#---------------------------------------------------------------------------------------------#
sub check_params {
    my ($flag) = @_;
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] =~ /^$flag/) {
            return 1;
        }
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# get "-key value" options value
# this key may write multiple times, like -I -L
#---------------------------------------------------------------------------------------------#
sub get_params {
    my ($flag) = @_;
    my $params = "";
    for(my $i=0; $i<@ARGV; $i++) {
        my $temp = "";
        if($ARGV[$i] =~ /^${flag}$/) {
            my $basic = $ARGV[$i+1];
            if ($basic =~ /^\s*-.*/) {
                next;
            }
            if($basic =~ /^(\/.*)/) {
                $temp = "$1";
            } else {
                $temp = "${curr_dir}\/$basic";
            }
        } elsif($ARGV[$i] =~ /^${flag}(.*)/) {
            my $basic = $1;
            if($basic =~ /^(\/.*)/) {
                $temp = "$1";
            } else {
                $temp = "${curr_dir}\/${basic}";
            }
        }
        if ($temp ne "") {
            $params .= "${flag}${temp} ";
        }
    }
    if ($params =~ /^\s*$/) {
        MSG_E_3011($flag);
        exit_on_error();
    } else {
        return "${flag}${curr_dir} $params";
    }
}

sub get_param_pure {
    my ($flag) = @_;
    for(my $i = 0; $i < @ARGV; $i++) {
        if($ARGV[$i] eq $flag) {
            if($i + 1 >= @ARGV) {
                return "";
            } elsif($ARGV[$i + 1] =~ /^-.*/) {
                if($ARGV[$i + 1] =~ /^-(\d+)$/) {
                    return $ARGV[$i + 1];
                } else {
                    return "";
                }
            } else {
                return $ARGV[$i + 1];
            }
        }
    }
    return "";
}

sub get_param_without_check {
    my ($flag) = @_;
    for(my $i = 0; $i < @ARGV; $i++) {
        if($ARGV[$i] eq $flag) {
            if($i + 1 >= @ARGV) {
                return "";
            } else {
                return $ARGV[$i + 1];
            }
        }
    }
    return "";
}
#---------------------------------------------------------------------------------------------#
# check "-key=value" options value legality
#---------------------------------------------------------------------------------------------#
sub get_param {
    my ($flag) = @_;
    my $param = get_param_pure($flag);
    if ($param =~ /\S+/) {
        return $param;
    }
    MSG_E_3011($flag);
    exit_on_error();
}

#---------------------------------------------------------------------------------------------#
# check "-key=value" options value legality
#---------------------------------------------------------------------------------------------#
sub check_param {
    my ($flag) = @_;
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] eq $flag) {
            return 1;
        }
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# get "-key value" options value
#---------------------------------------------------------------------------------------------#
sub get_param_equal {
    my ($flag) = @_;
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i]  =~ /(\S+)=(\S+)/) {
            if($1 eq $flag) {
                if($2 eq "") {
                    MSG_E_3011($flag);
                    exit_on_error();
                } else {
                    return $2;
                }
            }
        }
    }
    return " ";
}

#---------------------------------------------------------------------------------------------#
# check "-key=value" options value legality
#---------------------------------------------------------------------------------------------#
sub check_param_equal {
    my ($flag) = @_;
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] =~ /$flag=(\S+)/) {
            return 1;
#            printf("flag = $1, $2,, $flag\n");
#            if($1 eq "$flag") {
#                return 1;
#            }
        } elsif($ARGV[$i] =~ /$flag=/) {
            return 1;
        }
    }
    return 0;
}

sub get_file_name_from_path {
    my ($path) = @_;
    my @name = split(/\//, $path);
    my $num = 1;
    my $count = @name;
    my $file_name = $name[$count - $num];
    return $file_name;
}

sub update_to_absolute_path {
    my ($path) = @_;
    my $output_name = "";
    my @path_list = split(/ /, $path);
    foreach my $one_path (@path_list) {
        if(-d $one_path or -e $one_path) {
            my $abs_path = abs_path($one_path);
            $output_name .= " $abs_path";
        } else {
            $output_name .= " $one_path";
        }
    }
    return $output_name;
}

sub update_to_absolute_path_from_string {
    my ($path) = @_;
    my $output_name = "";
    my @path_list = split(/ /, $path);
    foreach my $one_path (@path_list) {
        if(-d $one_path or -e $one_path) {
            my $abs_path = abs_path($one_path);
            $output_name .= " $abs_path";
        }
    }
}

#---------------------------------------------------------------------------------------------#
# create project 
# create an empty xml file and fill it with commandline arguments
#---------------------------------------------------------------------------------------------#
sub create_project {
    my %arguments;
    if($encrypt eq "on") {
        $arguments{"encrypt"} = "on";
    }

    $arguments{"merlincc"} = "on";
    for(my $i=0; $i<@ARGV; $i++) {
        if($ARGV[$i] eq "-a") {
            if($ARGV[$i+1] =~ /(.*)=(.*)/g) {
                $arguments{"$1"} = $2;
            }
        }
    }

    my ($impl_tool, $device_platform) = get_platform_info();
    if(!check_vendor_environment($impl_tool)) {
        exit;
    }

    $device_platform = update_platform($impl_tool, $device_platform);
    check_impl_tool_exist($impl_tool, $device_platform);
    
    $arguments{"implementation_tool"} = $impl_tool;
    if ($impl_tool ~~ [$vitis, $vitis_hls, $vivado_hls]) {
        $arguments{"implementation_tool"} = $sdaccel;
        $arguments{"tool_version"} = $impl_tool;
    }
    $arguments{"platform_name"} = $device_platform;
    $arguments{"evaluate"} = $evaluate;

    if (check_param('--api')) {
        $arguments{"api"} = "all";
        $api_level           = 1;
    }

    if (check_param('-w')) {
        $arguments{"warning"} = "-w";
    }

    if (check_param('-g')) {
        $arguments{"ihatebug"} = "debug";
    }

    if($encrypt ne "on") {
        if (check_param('-d11')) {
            $arguments{"ihatebug"} = "debug-level2";
        }

        if (check_param('-d22')) {
            $arguments{"ihatebug"} = "debug-level3";
        }
    }

    if(check_param_equal("-std")) {
        $arguments{"cstd"} = get_param_equal("-std");
    }

    if($debug_info eq 1) {
        $arguments{"debug_info"} = "on";
    }

    if (check_param_equal('--report')) {
        my $report_type = get_param_equal('--report');
        $arguments{"report_type"} = $report_type;
    }

    my $exist_O1 = "";
    my $exist_O2 = "";
    my $exist_O3 = "";
    my $count_O = 0;
    $arguments{"opt_effort"} = "standard";
    for (my $i=0; $i<@ARGV; $i++) {
        if ($ARGV[$i] eq "-O0" or $ARGV[$i] eq "-o0") {
            my %O0_set = get_O0_set();
            for my $one_key (keys %O0_set) {
                $arguments{$one_key} = $O0_set{$one_key};
            }
#            $arguments{"opt_effort"} = "baseline";
        }
        if ($ARGV[$i] eq "-O1" or $ARGV[$i] eq "-o1") {
            $arguments{"opt_effort"} = "standard";
            $exist_O1 = "-O1";
            $count_O++;
        }
        if ($ARGV[$i] eq "-O2" or $ARGV[$i] eq "-o2") {
            $arguments{"opt_effort"} = "standard";
            $exist_O2 = "-O2";
            $count_O++;
        }
        if ($ARGV[$i] eq "-O3" or $ARGV[$i] eq "-o3") {
            $arguments{"opt_effort"} = "standard";
            $exist_O3 = "-O3";
            $count_O++;
        }
        if ($ARGV[$i] eq "--attribute") {
            if($ARGV[$i+1] =~ /(\S+)=(\S+)/) {
                $arguments{"$1"} = $2;
            }
        }
        if ($ARGV[$i] eq "--vendor-options") {
            $arguments{"vendor_options"} = $ARGV[$i+1];
        }
        if ($ARGV[$i] eq "--tb") {
            $arguments{"tb"} = $ARGV[$i+1];
        }
        if ($ARGV[$i] eq "-funsafe-math-optimizations") {
            $arguments{"unsafe_math"} = "on";
        }
    }

    if ($count_O > 1) {
        MSG_E_3023("$exist_O1 $exist_O2 $exist_O3");
        exit_on_error();
    }

    my $clock_period = "5ns";
    if (check_param("-period")) {
        $clock_period = get_param("-period");
    }
    $arguments{"clock_period"} = $clock_period;

    if (check_param("--kernel_frequency")) {
        my $freq = get_param("--kernel_frequency");
        if($freq !~ /^\d+$/ or $freq <= 0) {
            MSG_E_3024();
            exit_on_error();
        }
        $arguments{"kernel_frequency"} = $freq;
    }

    if(-e $merlincc_prj) {
        system "rm -rf $merlincc_prj";
    }
    if(-e $merlincc_src) {
        system "rm -rf $merlincc_src";
    }
    system "mkdir $merlincc_prj";
    system "mkdir $merlincc_src";
    my %include_directory_mark;
    my $one_inc_dir = " -I$curr_dir";
    my $include_directory = $one_inc_dir;
    $include_directory_mark{$one_inc_dir} = 1;
    my $file_name_string = "";
    for (my $i = 0; $i < @ARGV; $i++) {
        if ($ARGV[$i] =~ /.*\.(c|cpp|c\+\+|cc|C|cxx|h|hh|H|hxx|hpp|h\+\+)$/) {
            if (-e $ARGV[$i] and
                $ARGV[$i - 1] ne "-L" and
                $ARGV[$i - 1] ne "-I" and
                $ARGV[$i - 1] ne "-D") {
                system "cp $ARGV[$i] $merlincc_src";
                my $file_name = get_file_name_from_path($ARGV[$i]);
                if ($file_name =~ /.*\.(c|cpp|c\+\+|cc|C|cxx)$/) {
                    $file_name_string .= " $file_name";
                }
                if ($ARGV[$i] =~ /^(\/.*\/).*$/) {
                    $one_include_dir = " -I$1";
                    if (! $include_directory_mark{$one_include_dir}++) {
                        $include_directory .= $one_include_dir;
                    }
                } elsif($ARGV[$i] =~ /^(.*\/).*$/) {
                    $one_include_dir = " -I${curr_dir}\/$1";
                    if (! $include_directory_mark{$one_include_dir}++) {
                        $include_directory .= $one_include_dir;
                    }
                }
            }
        }
    }
    $arguments{"source_file"} = $file_name_string;
    #check -I include files
    #$include_directory .= "-I${curr_dir} ";
    if (check_params("-I")) {
        for $inc_dir (split / /, get_params("-I")) {
            $one_include_dir = " ".$inc_dir;
            if (! $include_directory_mark{$one_include_dir}++) {
                $include_directory .= $one_include_dir;
            }
        }
    }
    $include_directory .= " -I$MERLIN_COMPILER_HOME/mars-gen/lib/merlin";
    $arguments{"include_path"} = $include_directory;

    if (check_param('--lib-only')) {
        $arguments{"libonly"} = "on";
    }

    my $define_macros = "";
    for (my $i = 0; $i < @ARGV; $i++) {
        my $temp = "";
        if($ARGV[$i] =~ /^-D(.*)/) {
            if ($1) {
                $temp = $1;
            } else {
                if ($i >= @ARGV) {
                    MSG_E_3011("-D");
                }
                $temp = $ARGV[$i + 1];
            }
            $define_macros .= " -D $temp ";
        }
    }
    $arguments{"macro_define"} .= $define_macros;

    if (not $impl_tool ~~ [$sdaccel, $vitis, $vitis_hls, $vivado_hls]) {
        MSG_E_3025($impl_tool);
        exit_on_error();
    }
    my $xilinx_flag = ($impl_tool eq $sdaccel ? '-x' : '-i');
    my $create_cmd = "cd $merlincc_prj; merlin_create_project run -s src $xilinx_flag -keep_platform -no_check";
    # TODO should fix this, it's ugly
    system $create_cmd;

    @arguments_keys = keys(%arguments);
    @arguments_values = values(%arguments);

    if(-e "$MERLIN_COMPILER_HOME/mars-gen/default/directive_empty.xml") {
        system "cp $MERLIN_COMPILER_HOME/mars-gen/default/directive_empty.xml $merlincc_prj/run/spec/directive.xml";
    } else {
        printf "ERROR: Did not find directive_empty.xml in Merlin.\n";
        exit;
    }
    my $xml_file = "$merlincc_prj/run/spec/directive.xml";
    my $data = read_file("$xml_file");
    my $arguments_number = @arguments_keys;
    for(my $i=0; $i<$arguments_number; $i++) {
        $data =~ s/<directives>/<directives>\n<$arguments_keys[$i]>$arguments_values[$i]<\/$arguments_keys[$i]>/g;
    }
    write_file("$xml_file", $data);
}

#---------------------------------------------------------------------------------------------#
# update attributes information by command lines
#---------------------------------------------------------------------------------------------#
sub update_impl_info_from_project {
    # some attributes need to be update when read MCO file
    #my @function_id = ();
    my ($proj_path, $impl_tool) = @_;
    my %arguments;
    for ($i=0; $i<@ARGV; $i++) {
        if ($ARGV[$i] eq "--vendor-options") {
            $arguments{"vendor_options"} = $ARGV[$i+1];
        }
        if ($ARGV[$i] eq "--tb") {
            $arguments{"tb"} = $ARGV[$i+1];
        }
        if ($ARGV[$i] eq "--argv") {
            my $content = $ARGV[$i+1];
            $content = update_to_absolute_path($content);
            $arguments{"exe_args"} = $content;
        }
        if ($ARGV[$i] eq "-funsafe-math-optimizations") {
            $arguments{"unsafe_math"} = "on";
        }
        if ($ARGV[$i] eq "--attribute") {
            if($ARGV[$i+1] =~ /(\S+)=(\S+)/) {
                $arguments{"$1"} = $2;
            }
        }
    }
    my $tb_file_list = "";
# for (my $i = 0; $i < @ARGV; $i++) {
#        if ($ARGV[$i] =~ /.*\.(c|cpp|c\+\+|cc|C|cxx|h|hh|H|hxx|hpp|h\+\+)$/) {
#            if (-e $ARGV[$i] and
#                $ARGV[$i - 1] ne "-L" and
#                $ARGV[$i - 1] ne "-I" and
#                $ARGV[$i - 1] ne "-D") {
#                my $file_name = $ARGV[$i];
#                $file_name = update_to_absolute_path($file_name);
#                $tb_file_list .= "$file_name ";
#            }
#        }
#    }
#    $arguments{"tb"} = $tb_file_list;
   my @tb_set  = split(/\n/, $arguments{"tb"});
    foreach my $one_tb (@tb_set) {
        $one_tb = update_to_absolute_path($one_tb);
        $tb_file_list .= "$one_tb ";
    }
    $arguments{"tb"} = $tb_file_list;
    $arguments{"evaluate"} = $evaluate;

    if (check_param("--kernel_frequency")) {
        my $freq = get_param_pure("--kernel_frequency");
        if($freq !~ /^\d+$/ or $freq <= 0) {
            MSG_E_3024();
            exit_on_error();
        }
        $arguments{"kernel_frequency"} = $freq;
    }
    if (check_param('-d11')) {
        $arguments{"ihatebug"} = "debug-level2";
    }
    if (check_param('-d22')) {
        $arguments{"ihatebug"} = "debug-level3";
    }
    if($device_platform ne "") {
        $arguments{"platform_name"} = $device_platform;
    }

    if (check_param("--cflags")) {
        my $string = get_param_without_check("--cflags");
        $string = update_to_absolute_path($string);
        $arguments{"cflags"} = $string;
    }

    if (check_param("--ldflags")) {
        my $string = get_param_without_check("--ldflags");
        $string = update_to_absolute_path($string);
        $arguments{"ldflags"} = $string;
    }
    
    if (check_param("--argv")) {
        my $string = get_param_without_check("--argv");
        $string = update_to_absolute_path($string);
        $arguments{"exec_argv"} = $string;
    }

    if (check_param_equal('--report')) {
        my $report_type = get_param_equal('--report');
        $arguments{"report_type"} = $report_type;
    }

    @arguments_keys = keys(%arguments);
    @arguments_values = values(%arguments);

    my $xml_file = "$proj_path/run/spec/directive.xml";
    my $data = read_file("$xml_file");
    my $arguments_number = @arguments_keys;
    for($i=0; $i<$arguments_number; $i++) {
        if($data =~ /<$arguments_keys[$i]>.*<\/$arguments_keys[$i]>/) {
            $data =~ s/<$arguments_keys[$i]>.*<\/$arguments_keys[$i]>/<$arguments_keys[$i]>$arguments_values[$i]<\/$arguments_keys[$i]>/g;
        } else {
            $data =~ s/<directives>/<directives>\n<$arguments_keys[$i]>$arguments_values[$i]<\/$arguments_keys[$i]>/g;
        }
    }
    write_file("$xml_file", $data);
    #copy one to implement/export directory
    if(-e $xml_file) {
        system "cp -r $xml_file $proj_path/run/implement/export/directive.xml >/dev/null";
    } else {
        printf "ERROR: $xml_file not generated.\n";
        exit;
    }
}
   
#---------------------------------------------------------------------------------------------#
# print information and append to log
#---------------------------------------------------------------------------------------------#
sub printf_and_append {
    my ($filename, $content) = @_;
    open my $file, '+<:encoding(UTF-8)', $filename or die "Could not open '$filename' for reading $!";
    local $/;
    my $old_content = <$file>;
    seek($file, 0, 0);
    print $file $old_content;
    print $file $content;
    close($file);
    printf $content;
}

#---------------------------------------------------------------------------------------------#
# check xrt environment
#---------------------------------------------------------------------------------------------#
sub check_xrt {
    my $run_time = $ENV{'XILINX_XRT'};
    if(not -d $run_time) {
        MSG_E_3072;
        return 0;
    }
    return 1;
}

#---------------------------------------------------------------------------------------------#
# check vitis environment
#---------------------------------------------------------------------------------------------#
sub check_vitis  {
    printf_and_append($merlin_log, $MSG_I_1042);
    my $path = `which v++ >/dev/null 2>&1 >.log_tmp`;
    $path = `cat .log_tmp`;
    chomp($path);
    if($path =~ /^(.*)\/bin\/v++/) {
        my $vitis_path = $1;
        $ENV{'VITIS_SDX'} = $vitis_path;
    }
    if($path !~ /\S+/) {
        printf_and_append($merlin_log, $MSG_E_3073);
        return 0;
    }
    my $ret = system "ls $ENV{'XILINX_VITIS'}/platforms >/dev/null 2>&1";
    if($path =~ /\S+/ and $ret != 0) {
        printf_and_append($merlin_log, MSG_E_3062("$ENV{'XILINX_VITIS'}/platforms"));
        return 0;
    } else {
        printf_and_append($merlin_log, "${MSG_I_1043}: ${path}\n");
    }
    if (check_xrt eq 0) {
        return 0
    }
    return 1;
}

#---------------------------------------------------------------------------------------------#
# check xocc environment
#---------------------------------------------------------------------------------------------#
sub check_xocc  {
    printf_and_append($merlin_log, $MSG_I_1008);
    my $path = `which xocc >/dev/null 2>&1 >.log_tmp`;
    $path = `cat .log_tmp`;
    chomp($path);
    if($path =~ /^(.*)\/bin\/xocc/) {
        my $sdx_path = $1;
        $ENV{'XILINX_SDX'} = $sdx_path;
    }
    if($path !~ /\S+/) {
        printf_and_append($merlin_log, $MSG_E_3019);
        return 0;
    } 
    my $ret = system "ls $ENV{'XILINX_SDX'}/platforms >/dev/null 2>&1";
    if($path =~ /\S+/ and $ret != 0) {
        printf_and_append($merlin_log, MSG_E_3062("$ENV{'XILINX_SDX'}/platforms"));
        return 0;
    } else {
        printf_and_append($merlin_log, "${MSG_I_1009}: ${path}\n");
    }
    if (check_xrt eq 0) {
        return 0
    }
    return 1;
}

#---------------------------------------------------------------------------------------------#
# check hls
#---------------------------------------------------------------------------------------------#
sub check_vitis_hls  {
    printf_and_append($merlin_log, $MSG_I_1044);
    my $path = `which vitis_hls >/dev/null 2>&1 >.log_tmp`;
    $path = `cat .log_tmp`;
    chomp($path);
    if($path =~ /^(.*)\/bin\/vitis_hls/) {
        printf_and_append($merlin_log, "${MSG_I_1045}: ${path}\n");
        return 1;
    }
    printf_and_append($merlin_log, $MSG_E_3078);
    return 0;
}
sub check_vivado_hls  {
    printf_and_append($merlin_log, $MSG_I_1046);
    my $path = `which vivado_hls >/dev/null 2>&1 >.log_tmp`;
    $path = `cat .log_tmp`;
    chomp($path);
    if($path =~ /^(.*)\/bin\/vivado_hls/) {
        printf_and_append($merlin_log, "${MSG_I_1047}: ${path}\n");
        return 1;
    }
    printf_and_append($merlin_log, $MSG_E_3079);
    return 0;
}

#---------------------------------------------------------------------------------------------#
# get vitis version
#---------------------------------------------------------------------------------------------#
sub get_vitis_version {
    my $version_str = `v++ --version`;
    if($version_str =~ /Vitis v(\S+) /) {
        return $1;
    } elsif($version_str =~ /v\+\+ v(\S+) /) {
        return $1;
    }
    return '';
}

#---------------------------------------------------------------------------------------------#
# get xocc version
#---------------------------------------------------------------------------------------------#
sub get_xocc_version {
    my $version_str = `xocc --version`;
    if($version_str =~ /xocc v(\S+)(_sdx)?/) {
        return $1;
    }
    return '';
}

#---------------------------------------------------------------------------------------------#
# get aoc version
#---------------------------------------------------------------------------------------------#
sub get_aoc_version {
    my $version_str = `aoc --version`;
    if ($version_str =~ /Version (\S+) Build/) {
        return $1;
    }
    return '';
}

#---------------------------------------------------------------------------------------------#
# check aoc environment
#---------------------------------------------------------------------------------------------#
sub check_aoc  {
    my $failed_flag = 0;
    printf_and_append($merlin_log, $MSG_I_1010);
    my $path = `which aoc >/dev/null 2>&1 >.log_tmp`;
    $path = `cat .log_tmp`;
    chomp($path);
    if($path !~ /\S+/) {
        printf_and_append($merlin_log, $MSG_E_3020);
        $failed_flag = 1;
    } else {
        printf_and_append($merlin_log, "${MSG_I_1011}${path}\n");
        if($path =~ /^(.*)\/bin\/aoc/) {
            my $aoc_path = $1;
            $ENV{'INTELFPGAOCLSDKROOT'} = $aoc_path;
        }
    }
    printf_and_append($merlin_log, $MSG_I_1012);
    $path = `which quartus >/dev/null 2>&1 >.log_tmp`;
    $path = `cat .log_tmp`;
    chomp($path);
    if($path !~ /\S+/) {
        printf_and_append($merlin_log, $MSG_E_3021);
        $failed_flag = 1;
    } else {
        printf_and_append($merlin_log, "${MSG_I_1013}${path}\n");
    }
    printf_and_append($merlin_log, $MSG_I_1014);
    $path = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
    if($path !~ /\S+/) {
        printf_and_append($merlin_log, $MSG_E_3022);
        $failed_flag = 1;
    } elsif(not -e $path) {
        printf_and_append($merlin_log, "${MSG_E_3066}${path}\n");
        $failed_flag = 1;
    } elsif(-e $path and not -e "$path/board_env.xml") {
        printf_and_append($merlin_log, "${MSG_E_3066}${path}\n");
        $failed_flag = 1;
    } else {
        printf_and_append($merlin_log, "${MSG_I_1015}${path}\n");
    }
    if($failed_flag eq 1) {
        return 0;
    }
    return 1;
}

#---------------------------------------------------------------------------------------------#
# check vendor environment
#---------------------------------------------------------------------------------------------#
sub check_vendor_environment {
    my $impl_tool = $_[0];
    if($impl_tool eq $sdaccel) {
        return check_xocc();
    } elsif($impl_tool eq $vitis) {
        return check_vitis();
    } elsif($impl_tool eq $vivado_hls) {
        return check_vivado_hls();
    } elsif($impl_tool eq $vitis_hls) {
        return check_vitis_hls();
    } else {
        MSG_E_3025($impl_tool);
        exit_on_error();
        return 1;
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# check if auto dse on
#---------------------------------------------------------------------------------------------#
sub is_auto_dse_turn_on {
    my $ret = 0;
    for (my $i=0; $i<@ARGV; $i++) {
        if ($ARGV[$i] eq "--attribute") {
            if($ARGV[$i+1] =~ /(\S+)=(\S+)/) {
                if ("$1" eq "auto_dse" and "$2" eq "on") {
                    return 1;
                }
            }
        }
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# check if SyCL on
#---------------------------------------------------------------------------------------------#
sub is_sycl_turn_on {
    my $ret = 0;
    for (my $i=0; $i<@ARGV; $i++) {
        if ($ARGV[$i] eq "--attribute") {
            if($ARGV[$i+1] =~ /(\S+)=(\S+)/) {
                if ("$1" eq "sycl" and "$2" eq "on") {
                    return 1;
                }
                if ("$1" eq "pure_kernel" and "$2" eq "on") {
                    return 1;
                }
            }
        }
    }
    return 0;
}

#---------------------------------------------------------------------------------------------#
# get implementaton platformation information from project XML
#---------------------------------------------------------------------------------------------#
sub get_impl_info_from_project {
    my ($proj_path, $check_tool_env) = @_;
    my $xml_file = "$proj_path/run/spec/directive.xml";
    my $impl_tool = get_value_from_xml('implementation_tool', $xml_file);
    my $device_platform = get_value_from_xml('platform_name', $xml_file);
    my $check_tool = $impl_tool;
    my $tool_version = get_value_from_xml('tool_version', $xml_file);
    if($tool_version ~~ [$vitis, $vitis_hls, $vivado_hls]) {
        $impl_tool = $tool_version;
    }
    if(!check_vendor_environment($impl_tool)) {
        exit;
    }
    if (is_impl_info_exist_in_cmd()) {
        my ($cmd_impl_tool, $cmd_device_platform) = get_platform_info("first");
        if ($cmd_impl_tool ne $impl_tool) {
            MSG_E_3045();
            exit_on_error();
        }
    }

    $impl_tool = get_value_from_xml('implementation_tool', $xml_file);
    $device_platform = get_value_from_xml('platform_name', $xml_file);
    ($impl_tool, $device_platform) = convert_platform_name_for_2017_4($impl_tool, $device_platform);
    if(not $tool_version ~~ [$vitis_hls, $vivado_hls]) {
        $device_platform = update_platform($impl_tool, $device_platform);
        check_impl_tool_exist($impl_tool, $device_platform);
    }
    return ($impl_tool, $device_platform, $tool_version);
}

#---------------------------------------------------------------------------------------------#
# prepend string to file
#---------------------------------------------------------------------------------------------#
sub prepend_file {
    my ($filename, $content) = @_;
    open my $file, '+<:encoding(UTF-8)', $filename or die "Could not open '$filename' for reading $!";
    local $/;
    my $old_content = <$file>;
    seek($file, 0, 0);
    print $file $content;
    print $file $old_content;
    close($file);
}

#---------------------------------------------------------------------------------------------#
# read string frome file
#---------------------------------------------------------------------------------------------#
sub read_file {
    my ($filename) = @_;

    open my $in, '<:encoding(UTF-8)', $filename or die "Could not open '$filename' for reading $!";
    local $/ = undef;
    my $all = <$in>;
    close $in;

    return $all;
}

#---------------------------------------------------------------------------------------------#
# write string to file
#---------------------------------------------------------------------------------------------#
sub write_file {
    my ($filename, $content) = @_;

    open my $out, '>:encoding(UTF-8)', $filename or die "Could not open '$filename' for writing $!";;
    print $out $content;
    close $out;

    return;
}

#---------------------------------------------------------------------------------------------#
# get user basic information
#---------------------------------------------------------------------------------------------#
sub get_misc_info {
    my $info;
    my $host = hostname();
    my $time = `date`;
    chomp($time);
    my $pwd = `pwd`;
    chomp($pwd);

    $info .= MSG_I_1096($host, "$Config{'osname'}", "$Config{'archname'}");
    $info .= MSG_I_1097("$ENV{'USER'}");
    $info .= MSG_I_1098("$time");
    $info .= MSG_I_1099("$pwd");
    return $info;
}

#---------------------------------------------------------------------------------------------#
# get append warning append log information
#---------------------------------------------------------------------------------------------#
#sub get_append_log_info {
#    my $info = "";
#    if($log_mode eq 1) {
#        $info = "";
#    } else {
#        if(-e $merlin_log) {
#            $info .= MSG_W_2001($merlin_log);
#        }
#    }
#    return $info;
#}

#---------------------------------------------------------------------------------------------#
# get merlin version information
#---------------------------------------------------------------------------------------------#

sub get_version_v1 {
  my $merlin_version_num = "unknown";
  my $version_file= "$ENV{'MERLIN_COMPILER_HOME'}/build/.rversion";
  if (-f $version_file) {
      $merlin_version_num = read_file($version_file);
      chomp($merlin_version_num);
      return $merlin_version_num;
  }

  $version_file= "$ENV{'MERLIN_COMPILER_HOME'}/build/.version";
  # build version
  my $build_version = "";
  if (open my $in, '<:encoding(UTF-8)', "$version_file") {
    foreach my $line (<$in>) {
      if ($line =~ /^Merlin Compiler version : (\d+\.\d+)/) {
        $merlin_version_num = $1;
      }elsif($line =~ /Build version : (.*)/) {
          $build_version = $1;
          chomp($build_version);
      }
    }
    close($in);
  }
  return $merlin_version_num . ".dev ($build_version)";
}

sub get_version_info {
    my $arg = $_[0];
    my $need_to_keep_prj = $_[1];
    if($need_to_keep_prj eq "not delete prj") {
      $keep_prj = 1;
    }
    my $version_file = "$ENV{MERLIN_COMPILER_HOME}/build/.version";
    if (not -e $version_file) {
        $version_file = "$ENV{MERLIN_COMPILER_HOME}/VERSION";
        if (not -e $version_file) {
            return;
        }
    }
    my $info = `cat $version_file`;
    my $build_date = "";
    if($info =~ /Build date : (.*)/) {
        $build_date = $1;
        chomp($build_date);
    }
    my $version = get_version_v1();
    chomp($version);
    my $banner;

    $banner .= "\n";
    return $banner;
}

sub get_version_prof_info {
    $keep_prj = 1;
    my $arg = $_[0];
    my $version_file = "$ENV{MERLIN_COMPILER_HOME}/build/.version";
    if (not -e $version_file) {
        $version_file = "$ENV{MERLIN_COMPILER_HOME}/VERSION";
        if (not -e $version_file) {
            return;
        }
    }
    my $info = `cat $version_file`;
    my $build_date = "";
    if($info =~ /Build date : (.*)/) {
        $build_date = $1;
        chomp($build_date);
    }
    my $version = get_version_v1();
    chomp($version);
    my $banner;

    $banner .= "\n";
    return $banner;
}

sub check_system{
    my $str = "";
    my $info = "";
    $info = `lsb_release -d >/dev/null 2>&1 >.log_tmp`;
    $info = `cat .log_tmp`;
    chomp($info);

#   disable LD_LIBRAR_PATH reset to fix environment issue in docker 
#    if($info =~ /CentOS.*release 7/) {
#        $str =  "LD_LIBRARY_PATH=";
#    }
#    print "str = $str";
    return $str;
}

#---------------------------------------------------------------------------------------------#
# check prerequist software installation
# tar, zip, unzip
#---------------------------------------------------------------------------------------------#
sub check_prerequst_software{
    my $check_failed = 0;
    # heck zip and unzip
    my $info = "";
#    $ENV{"PATH"} = "";
    $info = `which sh >/dev/null 2>&1 >.log_tmp`;
    $sh_location = `cat .log_tmp`;
    chomp($sh_location);
    $info = `ls -al $sh_location >/dev/null 2>&1 >.log_tmp`;
    $info = `cat .log_tmp`;
    chomp($info);
    if($info =~ /->.*dash/) {
        MSG_E_3077;
        $check_failed = 1;
    }
    $info = `which zip >/dev/null 2>&1 >.log_tmp`;
    $info = `cat .log_tmp`;
    chomp($info);
    if($info !~ /\S+/) {
        MSG_E_3063;
        $check_failed = 1;
    }
    $info = `which unzip >/dev/null 2>&1 >.log_tmp`;
    $info = `cat .log_tmp`;
    chomp($info);
    if($info !~ /\S+/) {
        MSG_E_3064;
        $check_failed = 1;
    }
    $info = `which tar >/dev/null 2>&1 >.log_tmp`;
    $info = `cat .log_tmp`;
    chomp($info);
    if($info !~ /\S+/) {
        MSG_E_3065;
        $check_failed = 1;
    }
    $info = `which lsb_release >/dev/null 2>&1 >.log_tmp`;
    $info = `cat .log_tmp`;
    chomp($info);
    if($info !~ /\S+/) {
        MSG_E_3067;
        $check_failed = 1;
    }
    $info = `which python >/dev/null 2>&1 >.log_tmp`;
    $info = `cat .log_tmp`;
    chomp($info);
    if($info !~ /\S+/) {
        MSG_E_3068;
        $check_failed = 1;
    } else {
        my $python_version = `python -V > .log_tmp 2>&1`;
        $python_version = `cat .log_tmp`;
        chomp($python_version);
        if($python_version !~ /Python 2.7/) {
            MSG_E_3069($python_version);
            $check_failed = 1;
        } 
    }
    if($check_failed eq 1) {
        exit;
    }
}

#---------------------------------------------------------------------------------------------#
# check space for host machine
#---------------------------------------------------------------------------------------------#
sub check_space {
    my ($warn_space, $exit_space) = @_;
    # space in 1024 bytes
    my $cur_space = `df -Pk .|awk '{print \$4}'|sed -n '2p'`;
    chomp($cur_space);
    if ($cur_space < $exit_space) {
        MSG_E_3402($exit_space, $cur_space);
        exit_on_error();
    }
    if ($cur_space < $warn_space) {
        MSG_W_2002($warn_space, $cur_space);
    }
}

#---------------------------------------------------------------------------------------------#
# deleta flex*** file
#---------------------------------------------------------------------------------------------#
sub flex_clean {
    my $file_all = `ls`;
    my @file_set = split(/\n/, $file_all);
    foreach my $one_file(@file_set) {
        if ($one_file =~ /^flex(\d+)\.log$/) {
            system "rm -rf $one_file  >/dev/null 2>&1";
        }
    }
}

#---------------------------------------------------------------------------------------------#
# exit project and clean
#---------------------------------------------------------------------------------------------#
sub exit_on_error {
    my $is_init = $_[0];
    if (-e "$merlincc_prj" and $debug_level <= 1 and $is_init ne "init" and $keep_prj eq 0) {
        system "rm -rf $merlincc_prj >/dev/null 2>&1";
    }
    $RET_CODE = 1;
    flex_clean();
    exit 1;
}

#---------------------------------------------------------------------------------------------#
# clean project
#---------------------------------------------------------------------------------------------#
sub env_clean {
    if (-e $merlincc_prj and $debug_level <= 1 and $keep_prj eq 0) {
        system("rm -rf $merlincc_prj")
    }
    # in encrypt mode, always delete merlin_prj
    if (-e $merlincc_prj and $encrypt eq "on" and $keep_prj eq 0) {
        system("rm -rf $merlincc_prj")
    }
    system "rm -rf .log_tmp  >/dev/null";
}

#---------------------------------------------------------------------------------------------#
# help information
#---------------------------------------------------------------------------------------------#
sub get_help_info {
    my $help_info = "";
    $help_info .= "\n";
    $help_info .= "Usage: merlincc <options> <filename>.< c|cpp|mco >\n";
    $help_info .= "\n";
    $help_info .= "Options:\n";
    $help_info .= "-v, --version\n";
    $help_info .= "    Display compiler version\n";
    $help_info .= "\n";
    $help_info .= "--list-platform\n";
    $help_info .= "    Display supported platforms\n";
    $help_info .= "\n";
    $help_info .= "-c\n";
    $help_info .= "    Compile and generate Merlin Compiler Object file (.mco) only\n";
    $help_info .= "\n";
    $help_info .= "-o <filename>\n";
    $help_info .= "    Specify output file name without file extension. The appropriate extension\n";
    $help_info .= "    will be added automatically per the other options used.\n";
    $help_info .= "    With '-c' option, file extension will be '.mco'\n";
    $help_info .= "    Otherwise, file extension will be '.aocx' (Intel) or '.xclbin' (Xilinx)\n";
    $help_info .= "\n";
    $help_info .= "-march=<sw_emu> \n";
    $help_info .= "    Specify the compilation target architecture.\n";
    $help_info .= "    When this option is not used, compilation is for target FPGA platform.\n";
    $help_info .= "    sw_emu : Create kernel binary that can run software emulation on x86\n";
    $help_info .= "\n";
    $help_info .= "--report=<estimate/syn/impl>\n";
    $help_info .= "    Generate requested report. Reports are in the merlincc_report/ folder.\n";
    $help_info .= "    estimate: Run vendor tools to generate estimated resource and performance \n";
    $help_info .= "              without generating hardware binary. It can be used together with,\n";
    $help_info .= "              '-c' option or with the .mco input but no '-c' option.\n";
    $help_info .= "    syn: run rtl synthesis to generate report, vivado_hls only\n";
    $help_info .= "    impl: run rtl synthesis and implementation to generate report, vivado_hls only\n";
    $help_info .= "\n";
    $help_info .= "--kernel_frequency <arg>\n";
    $help_info .= "    Specify kernel frequency, arg can only be positive integer.\n";
    $help_info .= "    Currently only support in SDx platform.\n";
    $help_info .= "\n";
    $help_info .= "--platform=<implementation_tool>::<platform_name>\n";
    $help_info .= "    Run for a specific acceleration platform.\n";
    $help_info .= "    <implementation_tool> = $sdaccel/$vitis/$vitis_hls/$vivado_hls\n";
    $help_info .= "    <platform_name> = board to execute on \n";
    $help_info .= "                      eg. 'a10gx' or 'xilinx:adm-pcie-ku3:2ddr:3.3'\n";
    $help_info .= "-p=<path>\n";
    $help_info .= "    Run with platform spcieficed in .xpfm file.\n";
    $help_info .= "    <path> = location of .xpfm file, and the extension should end with .xpfm\n";
    $help_info .= "\n";
    $help_info .= "--profile\n";
    $help_info .= "    Enable kernel profiling support in vendor backend tools.\n";
    $help_info .= "    Takes effect when run without '-c' option.\n";
    $help_info .= "\n";
    $help_info .= "-I <directory>\n";
    $help_info .= "    Specify an include file directory.\n";
    $help_info .= "\n";
    $help_info .= "-D <macro>[=macro_value]\n";
    $help_info .= "    Define a macro.\n";
    $help_info .= "\n";
    $help_info .= "-funsafe-math-optimizations\n";
    $help_info .= "    Enable optimizations for floating-point arithmetic that may violate IEEE or ANSI standards.\n";
    $help_info .= "\n";
    #$help_info .= "\n";
    #$help_info .= "--lib-only\n";
    #$help_info .= "    only generate merlincc library with fast mode.\n";
    $help_info .= "\n";
    $help_info .= "-h, --help\n";
    $help_info .= "    Display this help menu.\n";
    $help_info .= "\n";
    $help_info .= "Examples:\n";
    $help_info .= "    # Generate an <file>.mco file\n";
    $help_info .= "    merlincc -c mykernel.cpp -o mykernel --platform=vitis::xxxxx\n";
    $help_info .= "    # Generate kernel simulation binary <file>.< aocx|xclbin >\n";
    $help_info .= "    merlincc mykernel.mco -march=sw_emu -o mykernel_swemu --platform=vitis::xxxxx\n";
    $help_info .= "    # Generate resource estimate\n";
    $help_info .= "    merlincc mykernel.mco --report=estimate --platform=vitis::xxxxx\n";
    $help_info .= "    # Generate kernel hardware binary <file>.< aocx|xclbin >\n";
    $help_info .= "    merlincc mykernel.mco -o mykernel --platform=vitis::xxxxx\n";
    $help_info .= "\n";
    return $help_info;
}

#---------------------------------------------------------------------------------------------#
# get help information
#---------------------------------------------------------------------------------------------#
sub get_help {
    $keep_prj = 1;
    if (check_param('-syncheck')) {
      print get_version_prof_info(0);
    }
    else {
      print get_version_info(0, "not delete prj");
    }
    print get_help_info();
}

#############################
# Min Gao: add do_syncheck

sub do_syncheck{
    create_project();
    if(-e ".error")
    {
        system "rm -rf .error";
        exit_on_error();
    }
    $ret = system "cd $merlincc_prj; $env_set_library merlin_flow run syncheck -keep_platform |& tee -a ../$merlin_log";
    # check stack size error
    my $log_info = `cat $merlin_log`;
    if($log_info =~ /(setrlimit returned result.*)\n/) {   
        print "\nERROR: [MERCC-3062] $1. This may related to the setting of ulimit stack size.\n";
    }
}

#---------------------------------------------------------------------------------------------#
# when -O0 mode, set the default key and value
#---------------------------------------------------------------------------------------------#
sub get_O0_set {
    my %O0_set= (
        "pcie_transfer_opt"             => "off",
        "preprocess"                    => "off",
        "memory_burst"                  => "off",
        "value_prop"                    => "off",
        "burst_total_size_threshold"    => "4000000",
        "burst_single_size_threshold"   => "2000000",
        "burst_lifting_size_threshold"  => "128000",
        "cg_memory_burst"               => "off",
        "delinearization"               => "off",
        "memory_coalescing"             => "off",
        "memory_partition"              => "off",
        "channel_partition"             => "off",
        "partition_xilinx_threshold"    => "1024",
        "partition_heuristic_threshold" => "64",
        "biwidth_opt"                   => "off",
        "function_inline"               => "off",
        "auto_func_inline"              => "off",
        "auto_dim_interchange"          => "off",
        "auto_false_dependence"         => "off",
        "auto_register"                 => "off",
        "auto_register_size_threshold"  => "512",
        "auto_reshape_threshold"        => "4096",
        "auto_memory_partition"         => "off",
        "reduction_opt"                 => "off",
        "reduction_general"             => "off",
        "line_buffer"                   => "off",
        "dependency_resolve"            => "off",
        "auto_parallel"                 => "on",
        "coarse_grained_pipeline"       => "off",
        "coarse_grained_parallel"       => "off",
        "fg_pipeline_parallel"          => "off",
        "aggressive_opt_for_c"          => "off",
        "aggressive_opt_for_cpp"        => "off",
        "bus_bitwidth"                  => "off",
        "multi_layer_reduction"         => "off",
        "loop_flatten"                  => "off",
        "dual_port_mem"                 => "off",
        "index_transform"               => "off",
        "auto_fg_opt"                   => "off",
        "auto_fgpip_opt"                => "off",
        "auto_fgpar_opt"                => "off",
        "auto_fgpar_threshold"          => "off",
        "false_dep_removal"             => "off",
        "comm_dead_remove"              => "off",
        "comm_rename"                   => "off",
        "comm_sync"                     => "off",
        "comm_refine"                   => "off",
        "comm_token"                    => "off",
        "comm_opt"                      => "off",
        "loop_dist"                     => "off",
        "loop_tiling"                   => "off",
        "loop_tile"                     => "off",
        "memory_reduce"                 => "off",
        "structural_func_inline"        => "on",
        "systolic_array"                => "off",
        "array_linearize"               => "on",
        "array_linearize_simple"        => "off",
        "pre_hls_tcl"                   => "",
        "sycl"                          => "off",
        "impl_num"                      => "off",
	    "stream_prefetch"         => "off",
        "auto_dse"                      => "off",
        "ap_int_max_w"                  => "4096"
        );
    return %O0_set;
};

#---------------------------------------------------------------------------------------------#
# all merlin support attribute key
#---------------------------------------------------------------------------------------------#
sub get_key_set {
    my @key_set = ("clock_peroid", "pcie_transfer_opt", "cstd",
     "preprocess",
     "memory_burst",
     "value_prop",
	 "stream_prefetch",
     "burst_total_size_threshold",
     "burst_single_size_threshold",
     "burst_lifting_size_threshold",
     "cg_memory_burst",
     "delinearization",
     "memory_coalescing",
     "memory_partition",
     "channel_partition",
     "partition_xilinx_threshold",
     "partition_heuristic_threshold",
     "biwidth_opt",
     "bus_length_threshold",
     "lift_fine_grained_cache",
     "function_inline",
     "auto_func_inline",
     "auto_dim_interchange",
     "auto_false_dependence",
     "auto_register",
     "auto_register_size_threshold",
     "auto_reshape_threshold",
     "auto_memory_partition",
     "reduction_opt",
     "reduction_general",
     "line_buffer",
     "dependency_resolve",
     "auto_parallel",
     "coarse_grained_pipeline",
     "coarse_grained_parallel",
     "fg_pipeline_parallel",
     "naive_hls",
     "aggressive_opt_for_c",
     "aggressive_opt_for_cpp",
     "bus_bitwidth",
     "multi_layer_reduction",
     "loop_flatten",
     "dual_port_mem",
     "index_transform",
     "auto_fg_opt",
     "auto_fgpip_opt",
     "auto_fgpar_opt",
     "auto_fgpar_threshold",
     "false_dep_removal",
     "comm_dead_remove",
     "comm_rename",
     "comm_sync",
     "comm_refine",
     "comm_token",
     "comm_opt",
     "loop_dist",
     "loop_tiling",
     "loop_tile",
     "memory_reduce",
     "implementation_tool",
     "opt_effort",
     "pure_kernel",
     "default_initiation_interval",
     "extern_c",
     "report_time",
     "kernel_separate",
     "skip_syntax_check",
     "systolic_array",
     "array_linearize",
     "array_linearize_simple",
     "pre_hls_tcl",
     "sycl",
     "impl_num",
     "ihatebug",
     "merlincc_debug",
     "auto_dse",
     "dse_time",
     "dse_hls_time",
     "dse_thread",
     "dse_algo",
     "dse_engine",
     "dse_debug",
     "generate_l2_api",
     "enable_cpp",
     "explicit_bundle",
     "api",
     "mem_size_limitation",
     "structural_func_inline",
     "skip_syncheck",
     "aligned_struct_decomp",
     "ap_int_max_w"
      );
    return @key_set;
}

