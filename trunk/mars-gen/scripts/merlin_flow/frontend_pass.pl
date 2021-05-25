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


my $library_path = '';
my $prj_dir;
my $curr_dir;
BEGIN {
#    $prj_dir = "$ENV{'MERLIN_CURR_PROJECT'}/implement";
    $prj_dir = "";
    my $curr_dir = "";
    $curr_dir = `pwd`;
    if($curr_dir =~ /(.*)\/implement\/code_transform/) {
        $prj_dir = $1;
    }
    if($curr_dir =~ /(.*)\/implement\/opencl/) {
        $prj_dir = $1;
    }
    $library_path = "$ENV{MERLIN_COMPILER_HOME}/mars-gen/scripts/lib";
    our $xml_dir = "";
}
#use strict;
#use warnings;
use lib "$library_path";
use merlin;
use message;
use global_var;
my $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
my $XILNX_XRT = $ENV{'XILINX_XRT'};

my $extend_macro_define = $xml_macro_define;
$extend_macro_define .= " -D__SYNTHESIS__  -D_MARS_COMPILER_FLOW_ ";
# output mesage to .source_opt directory
sub copy_log_out {
    my $log_file = $_[0];
    my $pass_idx = $_[1];
    if($prj_dir ne "") {
        my $report_dir = "$prj_dir/report/.source_opt";
        if (not -e  "$report_dir") {
            run_command "mkdir -p $report_dir";
        }
        if (-e "$log_file") {
           run_command "cp $log_file $report_dir/compile_step_$pass_idx.log";
        }
    }
}

########################################################################################
### This script call the frontend tool and perform renaming for ROSE output          ###
########################################################################################

########################################################################################
#extract information from XML
########################################################################################
#$xml_include_path =~ s/-I/ -I/g;
$xml_include_path .= " -I $MERLIN_COMPILER_HOME/mars-gen/lib/merlin";
if($xml_impl_tool eq "sdaccel") { $xml_include_path = " -I $dir_source_opt/include/apint_include " . $xml_include_path; }
my $cstd = "c99";
my $cxxstd = "c++11";
if($xml_cstd =~ /\S+/) {
    $cstd = "$xml_cstd";
} 
my $cstd_arg = "-std=" . $cstd;
my $cxxstd_arg = "-std=" . $cxxstd;
my $encrypt_flag = 0;
if($xml_encrypt eq "on") {
    $encrypt_flag = 1;
}

if (0 == @ARGV ) {
	print "Usage : frontend_pass.pl src_file_dir -p pass_name [-a pass_args] [-m metadata dir]\n";
    exit;
}

my $src_file_dir = $ARGV[0];
chomp($src_file_dir);
my $file_name = "*.c *.cpp  *.cxx *.C *.c++ *.cc";
my $pass_name = "";
my $pass_args = "-a cstd=$cstd -a cxxstd=$cxxstd ";
my $input_lib_path = "";
my $given_file_list = "";
my $print_content = 0;
my $metadata_dir = "$src_file_dir/../metadata";
for (my $i = 1; $i < @ARGV; $i+=2) {
	if ($ARGV[$i] eq "-f")              { $file_name = $ARGV[$i+1];
    } elsif ($ARGV[$i] eq "-p")         { $pass_name = $ARGV[$i+1];
    } elsif ($ARGV[$i] eq "-a")         { $pass_args .= $ARGV[$i+1];
    } elsif ($ARGV[$i] eq "-I")         { $input_lib_path = $ARGV[$i+1];
    } elsif ($ARGV[$i] eq "-src_list")  { $given_file_list= $ARGV[$i+1];
    } elsif ($ARGV[$i] eq "-print")     { # -print  $print_content = 1;
    } elsif ($ARGV[$i] eq "-m")         { $metadata_dir = "$src_file_dir/$ARGV[$i+1]/../metadata";
    } else {
        print "[frontend_pass.pl] Argument Error: ".$ARGV[$i]."\n";
        print "Usage : frontend_pass.pl src_file_dir -p pass_name [-a pass_args]\n";
        exit;
    }
}

$metadata_dir = "./metadata";
if(not -e $metadata_dir) {
    run_command "mkdir $metadata_dir";
}

# restore some critical pass name, when execute to this pass,
# printf this information to screen
my %pass_name_to_message_info = (
    'none'                  => "$MSG_I_1033",
    'preprocess'            => "$MSG_I_1034",
    'interface_transform'   => "$MSG_I_1035",
    'memory_burst'          => "$MSG_I_1036",
    'final_code_gen'        => "$MSG_I_1037",
    );

if (exists $pass_name_to_message_info{$pass_name}) {
    print("\n$pass_name_to_message_info{$pass_name}...\n");
}

my $temp_dir = "$prj_dir/implement";
if($prj_dir eq "") {
    $temp_dir = ".";
} else {
    $temp_dir = "$prj_dir/implement";
}
# Cody 20171222: This is a workaround to enforce single flow
# for the DSE process, because the current condition
# (check if proj/implement exists) doesn't work.
my $single_flow = 0;
if(-e $temp_dir and not -e ".enforce_single_flow") {
    $single_flow = 0;
} else {
    $single_flow = 1;
}

my $pass_idx;
if($single_flow eq 0) {
    $file_name =~ s/\"//g;  # remove staring and ending "
    if ($pass_name eq "none") {
        $pass_idx = 0;
    } else {
        if(not -e "$temp_dir/.merlin_pass_idx") {
            $pass_idx = 0;
        } else {
            $pass_idx = `cat $temp_dir/.merlin_pass_idx`;
            chomp($pass_idx);
            $pass_idx = $pass_idx + 1;
        }
    }
    run_command "echo $pass_idx > $temp_dir/.merlin_pass_idx";
    if ($print_content ) {
        print "\n";
    }
} else {
    $pass_idx = 0;
    $pass_name = "final_code_gen";
}

my $args_str = "";
if ($pass_args ne "") {
    $args_str = "args=\"$pass_args\"";
}
my $pass_name_display = "$pass_idx $pass_name";
my $args_str_display = $args_str;
if ($xml_debug_mode eq "debug-level2") {
    print "\nStarting optimization step $pass_name_display $args_str_display ... ";
}

run_command "cd $src_file_dir; mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/memcpy_type_fix.pl on";

my $src_file_list = `cd $src_file_dir; ls $file_name 2>/dev/null`;
my $src_file_array_str = $src_file_list;
$src_file_array_str =~ s/\n/","/g;
$src_file_list =~ s/\n/ /g;
chomp($src_file_list);

#######################################
# ZP: 20151024:
# Create a dummy c file and use it as the first input file for mars_opt
# This is a workaround.
# Then mars_opt will keep the C function in the .c files
# Otherwise it will add :: in front of the function calls to make it global as C++
my $dummy_c_name = "";
my $dummy_c_file = "";
my $str_file_list = $src_file_list . " ";
if ($str_file_list =~ /\.c /) {
    $dummy_c_name = "__dummy__.c";
    $dummy_c_file = "$src_file_dir/$dummy_c_name";
}

if ($str_file_list =~ /\.c /) {
   run_command "touch $dummy_c_file";
   if ($pass_name eq "lower_separate" or $pass_name eq "none") {
   #Youxiang: 20200222 to check whether top kernel function conflicts with system
   #function
       run_command "echo '#include<stdio.h>\n'  >>$dummy_c_file";
       run_command "echo '#include<stdlib.h>\n' >>$dummy_c_file";
       run_command "echo '#include<string.h>\n' >>$dummy_c_file";
   }
}

#######################################

#######################################
# ZP: 20160922
# add file_list
if (-f "${src_file_dir}/${given_file_list}") {
    my $str_list = `cat ${src_file_dir}/${given_file_list}`;
    $src_file_array_str = $str_list;
    $src_file_array_str =~ s/\n/","/g;
    $str_list =~ s/\n/ /g;
    $src_file_list= $str_list;
}
#######################################

my $critical_rpt_bak = "critical_message.rpt.bak";
my $critical_rpt = "critical_message.rpt";
run_command "rm -rf $src_file_dir/$critical_rpt_bak";
if (-e "$src_file_dir/$critical_rpt") {
    run_command "mv $src_file_dir/$critical_rpt $src_file_dir/$critical_rpt_bak";
}

if (not -e "./include") {
    run_command "mkdir ./include";
    if(-e "$dir_legacy/include/simple") {
        run_command "cp -r $dir_legacy/include/simple/* ./include 2>/dev/null";
    }
}
run_command "rm -rf $src_file_dir/rose_succeed";

#######################################
#ShanHe 20171019 apply STL header checker
if ($pass_name eq "none") {
    # check input source code syntax error and report to user
    my $include_special = "$MERLIN_COMPILER_HOME/mars-gen/lib/merlin/__merlin_malloc.h";
    my $source_file_list = `ls *.c* *.C 2>/dev/null`;
    if (-f "${src_file_dir}/${given_file_list}") {
        my $str_list = `cat ${src_file_dir}/${given_file_list}`;
        $source_file_list= $str_list;
    }

    my @source_file_set = split(/\n/, $source_file_list);
    my $gcc_check_cmd = "gcc ";
    $gcc_check_cmd .= " -include CL/opencl.h";
    $gcc_check_cmd .= " -include CL/cl_ext.h";
    if(-f "$XILNX_XRT/include/CL/cl_ext_xilinx.h") {
        $gcc_check_cmd .= " -I $XILNX_XRT/include -include CL/cl_ext_xilinx.h";
    }
    $gcc_check_cmd .= " -I include $xml_include_path";
    $gcc_check_cmd .= " -include stdio.h";
    $gcc_check_cmd .= " -include stdlib.h";
    $gcc_check_cmd .= " -include string.h";
    $gcc_check_cmd .= " $extend_macro_define";
    $gcc_check_cmd .= " $xml_warning";
    $gcc_check_cmd .= " -Wall -Wno-parentheses -Wno-unused-label -Wno-unknown-pragmas";
    $gcc_check_cmd .= " -include $include_special -fsyntax-only -Werror=implicit-function-declaration"; 

    foreach my $one_file (@source_file_set) {
        if ($one_file =~ /(.*)\.c$/) {
            $gcc_check_cmd .= " $cstd_arg ";
        } else {
            $gcc_check_cmd .= " $cxxstd_arg ";
        }
        run_command "$gcc_check_cmd -c $one_file 2>> .gcc_check_log";
        run_command "perl -pi -e 's/^.*code_transform\\\///g' .gcc_check_log";
    }
    if(-e ".gcc_check_log") {
        my $info = `cat .gcc_check_log`;
        if($info =~ " error:") {
            print "$info\n";
            my $flow_name = "Intel";
            if ($xml_impl_tool eq "sdaccel") {
                $flow_name = "Xilinx";
                print_error_msg($MSG_E_3057); 
            } else {
                print_error_msg($MSG_E_3057); 
            }
            exit;
        }
    }
    my $stl_header_check_cmd = "bash -c 'cd $src_file_dir; cd $MERLIN_COMPILER_HOME/set_env/; ";
    $stl_header_check_cmd .= "source env.sh >& /dev/null ; cd - >& /dev/null; mcheck $src_file_list -- ";
    $stl_header_check_cmd .= " -I include $xml_include_path";
    $stl_header_check_cmd .= " --gcc-toolchain=$MERLIN_COMPILER_HOME/source-opt/lib/gcc4.9.4";
    $stl_header_check_cmd .= " -include stdio.h";
    $stl_header_check_cmd .= " -include stdlib.h";
    $stl_header_check_cmd .= " -include string.h";
    $stl_header_check_cmd .= " $extend_macro_define";
    $stl_header_check_cmd .= " $xml_warning";
    $stl_header_check_cmd .= " -Wall -Wno-parentheses -Wno-unused-label -Wno-unknown-pragmas'";
    my $mcheck_ret = run_command "$stl_header_check_cmd 2>> .mcheck_log";
    run_command "perl -pi -e 's/^.*code_transform\\\///g' .mcheck_log";
    my $log_info = `cat .mcheck_log`;
    my @info_set = split(/\n/, $log_info);
    foreach my $one_line (@info_set) {
        if ($one_line =~ /warning generated/) {
        } elsif ($one_line =~ /warnings generated/) {
        } elsif ($one_line =~ /error generated/) {
        } elsif ($one_line =~ /errors generated/) {
        } else {
          print "$one_line\n";
        }
    }
    
    if ($mcheck_ret != 0) {
        # this will exit with a non-zero status.
        MSG_E_3056();
        run_command "touch .merlin_flow_has_error";
        exit;
    }
}

if ($pass_name eq "lower_separate") {
    my $source_file_list = `ls *.c* *.C 2>/dev/null`;
    if (-f "${src_file_dir}/${given_file_list}") {
        my $str_list = `cat ${src_file_dir}/${given_file_list}`;
        $source_file_list= $str_list;
    }

    my @source_file_set = split(/\n/, $source_file_list);
    foreach my $one_file (@source_file_set) {
        if($one_file eq $dummy_c_name) {
            next;
        }
        if($encrypt_flag eq 1) {
            run_command "script_dec $global_passwd $one_file";
        }
        run_command "gcc -E -dM  $extend_macro_define $xml_include_path $one_file >> __merlin_macro_list";
        if($encrypt_flag eq 1) {
            run_command "script_enc $global_passwd $one_file";
        }
    }
}

#######################################



##############################################

if ($xml_debug_mode ne "debug") {
    $pass_args .= " -a debug_mode=debug ";
}

run_command "cd $src_file_dir; echo '\n\n***************************************************' >> frontend.log 2>&1";
run_command "cd $src_file_dir; echo 'Pass $pass_name_display starting ...' >> frontend.log 2>&1";
run_command "cd $src_file_dir; echo 'mars_opt $dummy_c_name $src_file_list -e c -p $pass_name $pass_args $xml_include_path' >> frontend.log 2>&1";
run_command "cd $src_file_dir; echo '***************************************************' >> frontend.log 2>&1";
my $mars_opt_cmd = "mars_opt $dummy_c_name $src_file_list -e c -p $pass_name $pass_args $xml_include_path -a cflags=\"$extend_macro_define\"";
my $mars_opt_debug_cmd = "mars_opt_org $dummy_c_name $src_file_list -e c -p $pass_name $pass_args $xml_include_path -a cflags=\"$extend_macro_define\"";
my $mars_opt_cmd_safe;
my $flag = 0;
if($encrypt_flag eq 1) {
    $flag = 60;
    if($pass_name eq "none") { $flag = 0; }
} else {
    $flag = 0;
}
$mars_opt_cmd_safe = "merlin_safe_exec $mars_opt_cmd :: $src_file_list :: $flag";

#ZP: 20161025
if ($pass_args =~ /granularity=coarse/) {
   $pass_name .= "_cg";
}

if ($encrypt_flag eq 0) {
#print run and gdb before running
    run_command "cd $src_file_dir; echo '$mars_opt_cmd' > run";
    run_command "cd $src_file_dir; echo 'source \$MERLIN_COMPILER_HOME/set_env/env.sh' > gdb";
    run_command "cd $src_file_dir; echo 'gdb --args $mars_opt_debug_cmd' >> gdb";
}

##############################################
# EXECUTION
my $log_file = "run_${pass_name}.log";
my $start_time = time();
if ($print_content == 0) {
    run_command "bash -c 'cd $src_file_dir; cd $MERLIN_COMPILER_HOME/set_env/; source env.sh >& /dev/null ; cd - >& /dev/null; $mars_opt_cmd_safe > $log_file 2>&1 '";
} else {
    run_command "bash -c 'cd $src_file_dir; cd $MERLIN_COMPILER_HOME/set_env/; source env.sh >& /dev/null ; cd - >& /dev/null; $mars_opt_cmd_safe |& tee $log_file '";
}
remove_comment_scope($src_file_dir);

run_command "echo '$mars_opt_cmd' >> $log_file";

my $end_time = time();
my $time_elasped = $end_time - $start_time;
if ($xml_debug_mode eq "debug-level2") {
    print " $time_elasped seconds \n";
}
copy_log_out($log_file, $pass_idx);

##############################################
# handle critical message
if($single_flow eq 0) {
    # handle frontend.log
    run_command "cat $log_file >> $src_file_dir/frontend.log";
    if (-e "$src_file_dir/$critical_rpt") {
        run_command "cat $src_file_dir/$critical_rpt";
    }
}
run_command "touch $src_file_dir/$critical_rpt";
if (-e "$src_file_dir/$critical_rpt_bak") {
    run_command "cp $src_file_dir/$critical_rpt $src_file_dir/${critical_rpt}.tt";
    run_command "cat $src_file_dir/$critical_rpt_bak $src_file_dir/${critical_rpt}.tt > $src_file_dir/$critical_rpt";
}
##############################################

my $pass_name_count = $pass_name;
##############################################
# handle pass directory, generate backup directory to store input and output of each pass
my $back_up_dir = "$src_file_dir/run_$pass_name";
if (-e "$metadata_dir/history_${pass_name_count}.json") {
    $back_up_dir = "$src_file_dir/run_${pass_name}_1";
    $pass_name_count = "${pass_name}_1";
    for (my $i = 2; $i < 10; $i = $i+1) {
        if (-e "$metadata_dir/history_${pass_name_count}.json") {
            $back_up_dir = "$src_file_dir/run_${pass_name}_$i";
            $pass_name_count = "${pass_name}_$i";
        } else {
            last;
        }
    }
}

# generate metadata required files
if(-e "$src_file_dir/refer_report_${pass_name}.json") {
    run_command "cd $src_file_dir; cp input_refer_report_${pass_name}.json $metadata_dir/input_refer_report_${pass_name_count}.json";
    run_command "cd $src_file_dir; cp refer_report_${pass_name}.json $metadata_dir/refer_report_${pass_name_count}.json";
    run_command "cd $src_file_dir; cp refer_report.json $metadata_dir/refer_report.json";
}

if($prj_dir ne "") {
    if($single_flow eq 0) {
        if(-e "$src_file_dir/history_${pass_name}.json") {
            run_command "cd $src_file_dir; cp history_${pass_name}.json $metadata_dir/history_${pass_name_count}.json >& /dev/null";
        }
        run_command "cd $src_file_dir; echo ${pass_name_count} >> $metadata_dir/history_passes.list";
        run_command "cd $src_file_dir; echo ${pass_name_count} >> history_passes.list";
    }
}

    # copy to back up directory
if($encrypt_flag eq 0) {
    run_command "rm -rf $back_up_dir; mkdir $back_up_dir";
    system "cp -r $src_file_dir/kernel_list.json $src_file_dir/*.c $src_file_dir/*.cpp $src_file_dir/*.h $src_file_dir/*.hpp $src_file_dir/*.C $src_file_dir/*.cxx $src_file_dir/*.c++ $src_file_dir/*.cc  $src_file_dir/*.hh $src_file_dir/*.H $src_file_dir/*.hxx $src_file_dir/*.h++ $src_file_dir/*.cl $src_file_dir/*.xml $src_file_dir/*.list $src_file_dir/*.json $src_file_dir/*.tag $src_file_dir/aux history_passes.list $back_up_dir >& /dev/null";
    system "cp -r $src_file_dir/run $src_file_dir/gdb  $back_up_dir >& /dev/null";
    if (-e "$src_file_dir/${critical_rpt}.tt") {
        run_command "cp -r  $src_file_dir/${critical_rpt}.tt $back_up_dir >& /dev/null";
    }
    if (-e "$src_file_dir/__merlin_macro_list") {
        run_command "cp -r $src_file_dir/__merlin_macro_list $back_up_dir >& /dev/null";
    }
    run_command "cp -r  $src_file_dir/run_${pass_name}.log $back_up_dir >& /dev/null";
}
##############################################

##############################################
# check if rose pass succceed
if (not -e "$src_file_dir/rose_succeed") {
    if ($xml_debug_mode eq "debug-level3") {
        my $pwd = `cd $src_file_dir; pwd`;
        print("\n\n");
        if ("none" eq $pass_name) { run_command "cat $log_file"; }
        MSG_E_3061($mars_opt_cmd, $pwd);
        print_error_msg($MSG_E_3058);
    } else {
        if ($pass_name eq "none") {
            run_command "cat $log_file | grep -v 'Warning: Unrecognized attribute name = __leaf__'";
            print_error_msg($MSG_E_3059);
        } else {
            my $info = `cat $src_file_dir/frontend.log`;
            if ($info =~ "Errors in Processing Input File: throwing an instance of \"frontend_failed\"") {
                print_error_msg($MSG_E_3058);
            }
        }
    }
    print_error_msg($MSG_E_3060);
    die("\n");
}

if ($pass_name eq "kernel_wrapper") {
  run_command("cd $src_file_dir; mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/frontend/copy_head.pl");
}

##############################################

##############################################
if ($pass_name eq "lower_separate") {
  my $low_sep_file_list = `cd $src_file_dir/; ls __merlin* 2>/dev/null`;
  my @low_sep_file_set = split(/\n/, $low_sep_file_list);
# check output source code syntax error and report to user
  foreach my $one_file (@low_sep_file_set) {
    if($one_file =~ /^__merlin(.*)\.cpp$/ or $one_file =~ /^__merlin(.*)\.c$/) {
      my $filter_rose_header_cmd = "sed -i ";
      $filter_rose_header_cmd .= "'/rose_edg_required_macros_and_functions.h/d' $one_file";
      run_command "cd $src_file_dir; $filter_rose_header_cmd";
      my $low_sep_syntax_check_cmd = "gcc -c $one_file $xml_include_path ";
      if ($one_file =~ /(.*)\.c$/) {
        $low_sep_syntax_check_cmd .= "$cstd_arg ";
      } else {
        $low_sep_syntax_check_cmd .= "$cxxstd_arg ";
      }
      $low_sep_syntax_check_cmd .= "-fsyntax-only ";
      $low_sep_syntax_check_cmd .= "$extend_macro_define $xml_warning 2>> post_syntax_check_log";
      run_command "cd $src_file_dir; $low_sep_syntax_check_cmd";
    }
  }
} elsif ($pass_name eq "kernel_wrapper") {
  run_command("cd $src_file_dir; mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/frontend/copy_head.pl");
  my $kernel_wrapper_file_list = `cd $src_file_dir/; ls __merlinwrapper* 2>/dev/null`;
  my @kernel_wrapper_file_set = split(/\n/, $kernel_wrapper_file_list);
# check output source code syntax error and report to user
  foreach my $one_file (@kernel_wrapper_file_set) {
    if($one_file =~ /^__merlin(.*)\.cpp$/ or $one_file =~ /^__merlin(.*)\.c$/) {
      my $filter_rose_header_cmd = "sed -i ";
      $filter_rose_header_cmd .= "'/rose_edg_required_macros_and_functions.h/d' $one_file";
      run_command "cd $src_file_dir; $filter_rose_header_cmd";
      my $kernel_wrapper_syntax_check_cmd = "gcc -c $one_file $xml_include_path ";
      if ($one_file =~ /(.*)\.c$/) {
        $kernel_wrapper_syntax_check_cmd .= "$cstd_arg ";
      } else {
        $kernel_wrapper_syntax_check_cmd .= "$cxxstd_arg ";
      }
      $kernel_wrapper_syntax_check_cmd .= "-fsyntax-only ";
      $kernel_wrapper_syntax_check_cmd .= "$extend_macro_define $xml_warning 2>> post_syntax_check_log";
      run_command "cd $src_file_dir; $kernel_wrapper_syntax_check_cmd";
    }
  }
}

if (-e "post_syntax_check_log") {
    my $info = `cat post_syntax_check_log`;
    if ($info =~ " error:") {
        run_command "cd $src_file_dir; cat post_syntax_check_log >> $log_file";
        copy_log_out($log_file, $pass_idx);
        print_error_msg($MSG_E_3403); 
        die("\n");
    }
}

my $file_list = `cd $src_file_dir/; ls rose_* 2>/dev/null`;
my @file_set = split(/\n/, $file_list);
# check output source code syntax error and report to user
foreach my $one_file (@file_set) {
    if($one_file =~ /^rose_(.*)$/) {
        if(-e "$src_file_dir/$1") {
            if ($pass_name ne "none" and $pass_name ne "lower_separate" and $pass_name ne "final_code_gen") {
			    my $filter_rose_header_cmd = "sed -i ";
			    $filter_rose_header_cmd .= "'/rose_edg_required_macros_and_functions.h/d' $one_file";
                run_command "cd $src_file_dir; $filter_rose_header_cmd";
                my $syntax_check_cmd = "gcc -c $one_file $xml_include_path ";
                $syntax_check_cmd .= "-I $src_file_dir/decryped_coalescing ";
                if ($one_file =~ /(.*)\.c$/) {
                    $syntax_check_cmd .= "$cstd_arg ";
                } else {
                    $syntax_check_cmd .= "$cxxstd_arg ";
                }
                $syntax_check_cmd .= "-fsyntax-only ";
                $syntax_check_cmd .= "$extend_macro_define $xml_warning 2>> post_syntax_check_log";
                run_command "cd $src_file_dir; $syntax_check_cmd";
            }
        }
	    if ($pass_name ne "none") {
            run_command "cd $src_file_dir; mv $one_file $1";
	    } else {
            run_command "cd $src_file_dir; rm -f $one_file";
	    }
    }
}

if(-e "post_syntax_check_log") {
    my $info = `cat post_syntax_check_log`;
    if($info =~ " error:") {
        run_command "cd $src_file_dir; cat post_syntax_check_log >> $log_file";
        copy_log_out($log_file, $pass_idx);
        print_error_msg($MSG_E_3403); 
        die("\n");
    }
}
##############################################
if (-e "$dummy_c_file")  { run_command "rm -rf $dummy_c_file"; }
##############################################
#youxiang 20161116 apply preprocessing and restore #include
if ($pass_name eq "none") {
  my $preprocess_cmd = "cd $src_file_dir; python $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/preprocess_remove_directive.py $src_file_list ";
  $preprocess_cmd .= " -I include $xml_include_path";
  $preprocess_cmd .= " $extend_macro_define";
  run_command "$preprocess_cmd";
}
if (-e "cmost.h")     {run_command "rm -rf cmost.h";}
run_command "cd $src_file_dir; mars_perl $MERLIN_COMPILER_HOME/mars-gen/scripts/merlin_flow/memcpy_type_fix.pl off";
##############################################

# in none pass, only encrypt output, the last step will not encrypt code
if($encrypt_flag eq "1" and $pass_name ne "final_code_gen") {
    if($pass_name eq "lower_separate") {
        my $info = `cat $src_file_dir/__merlin_kernel_list.h`;
        my @info_set = split(/\n/, $info);
        foreach my $one_line (@info_set) {
            if($one_line =~ /\/\/(.*)=(.*)$/) {
                run_command "cd $src_file_dir; script_enc $global_passwd $2";
            }
        }
    } elsif($pass_name eq "kernel_wrapper") {
        {
            my $info = `cd $src_file_dir; ls __merlinwrapper_*\.c* 2>/dev/null`;
            my @info_set = split(/\n/, $info);
            foreach my $one_line (@info_set) {
                run_command "cd $src_file_dir; script_enc $global_passwd $one_line";
            }
        }
        if(-e "$src_file_dir/__merlintask_top.h") {
            my $info = `cd $src_file_dir; ls __merlintask_*\.c* 2>/dev/null`;
            my @info_set = split(/\n/, $info);
            foreach my $one_line (@info_set) {
                run_command "cd $src_file_dir; script_enc $global_passwd $one_line";
            }
        }
    }
    # after copy roxe_xxx.c to xxx.c, need encrypt xxx.c
    {
        my @file_set = split(/ /, $src_file_list);
        foreach my $one_file (@file_set) {
            if(-e "$src_file_dir/$one_file") {
                run_command "cd $src_file_dir; script_enc $global_passwd $one_file";
            }
        }
    }
    if($pass_name eq "none") {
        run_command "cd $src_file_dir; rm -rf .finish_read_files .finish_cmd >& /dev/null";       
    }
}

