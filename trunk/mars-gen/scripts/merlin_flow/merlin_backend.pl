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
my $MERLIN_COMPILER_HOME;
my $library_path;
my $flow   ;
my $src_dir;
my $dst_dir;
BEGIN {
    $|=1;   #turn on auto reflush for stdout
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    $mars_test_home = $ENV{'MARS_TEST_HOME'};
    #---------------------------------------------------------------------------------------------#
    # parse argument
    #---------------------------------------------------------------------------------------------#
    $flow    = $ARGV[0];
    $src_dir = $ARGV[1];
    $dst_dir = $ARGV[2];

    # before call library, set directive directory
    our $xml_dir = "";
    if($src_dir eq "") {
        $xml_dir = "./";
    } else {
        $xml_dir = $src_dir;
    }
#    printf "xml_dir = $xml_dir\n";
}

#---------------------------------------------------------------------------------------------#
# include library
#---------------------------------------------------------------------------------------------#
use strict;
#use warnings;
use lib "$library_path";
use message;
use merlin;
use global_var;
use File::Basename;

my $encrypt_flag = 0;
if($xml_encrypt eq "on") {
    $encrypt_flag = 1;
}
# function to encrypt and delete files
sub dec_files {
    my $file_dir = $_[0];
    my $list = `cd $file_dir; ls *.c* 2>/dev/null`;
    my @info_set = split(/\n/, $list);
    foreach my $one_line (@info_set) {
        if($one_line !~ "merlin_stream\.c" and $one_line !~ "cmost\.c") {
            run_command "cd $file_dir; script_dec $global_passwd $one_line";
        }
    }
}
sub delete_files {
    my $file_dir = $_[0];
    my $list = `cd $file_dir; ls *.c* 2>/dev/null`;
    my @info_set = split(/\n/, $list);
    foreach my $one_line (@info_set) {
        run_command "cd $file_dir; rm -rf $one_line";
    }
}

#---------------------------------------------------------------------------------------------#
# generate package and merlin report directory
#---------------------------------------------------------------------------------------------#
my $pkg = "$dst_dir/pkg";
my $report_merlin   = "$dst_dir/report_merlin";
my $report_vendor   = "$report_merlin/vendor_report";
my $report_merlin_fast_dse   = "$dst_dir/report_merlin_fast_dse";
my $report_merlin_perf_bak   = "$dst_dir/report_merlin_perf_bak";
my $report_merlin_res_bak   = "$dst_dir/report_merlin_res_bak";
my $report_merlin_ref   = "$dst_dir/report_merlin_ref";
if(not -e $pkg) {
    run_command "mkdir $pkg >& /dev/null";
}
if(not -e $report_merlin) {
    run_command "mkdir $report_merlin >& /dev/null";
}

#---------------------------------------------------------------------------------------------#
# generate execution flag
#---------------------------------------------------------------------------------------------#
my $metadata = "metadata";
my $lib_gen_flag    = 0;
my $host_flag       = 0;
my $estimate_flag   = 0;
my $sim_flag        = 0;
my $execute_sim_flag= 0;
my $hw_sim_flag     = 0;
my $impl_flag       = 0;
my $org_flag        = 0;
my $opt_flag        = 0;
my $on_board_flag   = 0;
if($flow eq "lib_gen")                  { $lib_gen_flag = 1; }
if($flow eq "host")                     { $lib_gen_flag = 1; $host_flag = 1; }
if($flow eq "hls")                      { $estimate_flag = 1; }
if($flow eq "sim")                      { $lib_gen_flag = 1; $host_flag = 1; $sim_flag = 1; $execute_sim_flag = 1;}
if($flow eq "sim_config")               { $sim_flag = 1; }
if($flow eq "hw_sim")                   { $lib_gen_flag = 1; $host_flag = 1; $hw_sim_flag = 1; $execute_sim_flag = 1;}
if($flow eq "hw_sim_config")            { $hw_sim_flag = 1; }
if($flow eq "impl" or $flow eq "bit")   { $impl_flag = 1; }
if($flow eq "exec")                     { $on_board_flag = 1; }
if($flow eq "test") {
    $org_flag = 1;
    $opt_flag = 1;
    $lib_gen_flag = 1;
    $host_flag = 1;
    $sim_flag = 1;
    $execute_sim_flag = 1;
    $on_board_flag = 1;
}

#---------------------------------------------------------------------------------------------#
# optional argument parser
#---------------------------------------------------------------------------------------------#
# espically fpr testing related arguments
my $original_arg = " ";
for (my $i = 3; $i < @ARGV; $i+=1) {
    $original_arg .= "$ARGV[$i] ";
}
my @arg_set = split(/ /, $original_arg);
foreach my $one_arg (@arg_set) {
    if ($one_arg eq "-no_sim_test")     { $lib_gen_flag = 0; $host_flag = 0; $sim_flag    = 0; $execute_sim_flag = 0; }
    if ($one_arg eq "-no_hw_sim_test")  { $lib_gen_flag = 0; $host_flag = 0; $hw_sim_flag = 0; $execute_sim_flag = 0; }
    if ($one_arg eq "-no_bit_test")     { $impl_flag = 0; $on_board_flag = 0;}
    if ($one_arg eq "-no_opt_test")     { $opt_flag = 0; }
    if ($one_arg eq "-no_org_test")     { $org_flag = 0; }
}

my $pure_kernel_flow = "off";
if($xml_sycl eq "on" or $xml_pure_kernel eq "on") {
    $pure_kernel_flow = "on";
}
if ($pure_kernel_flow eq "on") {
  $lib_gen_flag = 0;
}

if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"]) {
  $lib_gen_flag = 0;
}

$xml_include_path =~ s/-I/ -I/g;
$xml_vendor_options =~ s/\s*--xp\s*/ --xp /g;

# if call binary separately, set as single flow
my $is_in_project = 0;
if(-e "implement" and -e "spec/directive.xml") { $is_in_project = 1; }

#---------------------------------------------------------------------------------------------#
# main flow execution part
#---------------------------------------------------------------------------------------------#
sub fast_dse_preproc {
    # check fast dse input to determine to run HLS
    # 0 : need to run hls to get vendor report
    # 1 : no need to run hls, generate report directly

    my $est_type = $_[0]; # either "perf" or "res"
    my $fast_dse_dir = "$global_fast_dse/$est_type";

    # check if the metadata is generated by DSE
    if (not -e "$dst_dir/$fast_dse_dir") {
        return 0;
    }

    if (not -e "$dst_dir/$fast_dse_dir/$global_gen_token" or
        -z "$dst_dir/$fast_dse_dir/$global_gen_token") {
        # check if the reference report is available
        return 0;
    }
    elsif (not -e "$dst_dir/$fast_dse_dir/$global_change" or
           -z "$dst_dir/$fast_dse_dir/$global_change") {
        # check if the reference code change is available
        return 0;
    }

    # reorg the metadata and run the estimator
    if(not -e "$report_merlin/$global_fast_dse") {
        run_command "mkdir $report_merlin/$global_fast_dse";
    }
    run_command "cp -r $dst_dir/directive.xml $report_merlin";
    run_command "cp -r $dst_dir/$fast_dse_dir $report_merlin/$global_fast_dse/";
    run_command "cp -r $dst_dir/lc $report_merlin/$fast_dse_dir";
    return 1;
}

sub copy_files_to_report_merlin {
    my $src_dir = $_[0];
    my $report_merlin = $_[1];
    if(-e "$src_dir/directive.xml") {
        run_command "cp $src_dir/directive.xml $report_merlin";
    } elsif(-e "spec/directive.xml") {
        run_command "cp spec/directive.xml $report_merlin";
    }
    run_command "cp -r $src_dir/lc $report_merlin";
    if(-e "$src_dir/$metadata") {
        run_command "cp -r $src_dir/$metadata $report_merlin";
    }
    my $pass = "midend_preprocess";
    if(-e "$src_dir/is_dse_project") {
        run_command "cp -r $src_dir/run_$pass $report_merlin/src";
        run_command "perl -pi -e \'\$_=\"\" unless /$pass/ .. 1' $report_merlin/$metadata/history_passes.list";
        run_command "cd $report_merlin/src; rm -rf history* rose* run gdb host.list kernel.list aux *.xml *.tt *.log";
    } else {
        run_command "cp -r $src_dir/src $report_merlin/src";
    }
}

sub gen_merlin_rpt {
    my $fast_dse = $_[0];
    my $return_value = 0;
    # FIXME, if not in project, will not generate merlin report in tempary
    # because of missing metadata files
    if ($is_in_project eq 1){
        MSG_I_1024();

        run_command "$cmd_report_summary hls  2> .log";
        #TODO it is better to optimize the message report interface
        my $msg_report_args = "--src src --dst lc --metadata $metadata --xml directive.xml ";
        if ($fast_dse eq "fast_dse") {
            $msg_report_args .= "--report fast_dse";
        } else {
            $msg_report_args .= "--report vendor_report";
        }
        #run_command "cd $report_merlin; msg_report $msg_report_args > performace_estimate.log 2>&1";
        run_command "echo $xml_include_path > $report_merlin/.mount_include";
        run_command "echo $msg_report_args > $report_merlin/.merlin_core_args";
        if($xml_evaluate eq "") {$xml_evaluate = "off";}
        run_command "cd $report_merlin; merlin_license $xml_impl_tool $xml_evaluate \"--flow report --args .merlin_core_args\" > performace_estimate.log 2>&1";
        if(-e "$report_merlin/.license_check_pass") {
            # license check pass
            run_command "rm -rf $report_merlin/.license_check_pass";
        } else {
            printf($MSG_E_3080);
            exit;
        }
        #my $ret = run_command "mars_python -O ${MERLIN_COMPILER_HOME}/mars-gen/scripts/msg_report/msg_report.py " . $msg_report_args;
        # collect all message together 
        my $msg_report_log = "$report_merlin/message_report.log";
        run_command "touch $msg_report_log";
        if(-e "$report_merlin/gen_token/msg_report.log") {
            run_command "echo \"\n\n\n######gen_token#####\n\n\n\" >> $msg_report_log";
            run_command "cat $report_merlin/gen_token/msg_report.log >> $msg_report_log";
        }
        if(-e "$report_merlin/perf_est/perf_est.log") {
            run_command "echo \"\n\n\n######perf_est#####\n\n\n\" >> $msg_report_log";
            run_command "cat $report_merlin/perf_est/perf_est.log >> $msg_report_log";
        }
        if(-e "$report_merlin/ref_induct/refer_induct.log") {
            run_command "echo \"\n\n\n######ref_induct#####\n\n\n\" >> $msg_report_log";
            run_command "cat $report_merlin/ref_induct/refer_induct.log >> $msg_report_log";
        }
        if(-e "$report_merlin/final_report/msg_report.log") {
            run_command "echo \"\n\n\n######final_report#####\n\n\n\" >> $msg_report_log";
            run_command "cat $report_merlin/final_report/msg_report.log >> $msg_report_log";
        }
        if($fast_dse eq "fast_dse") {
            if(-e "$report_merlin/perf_est.json") {
                run_command "touch $dst_dir/estimate_perf_pass.o";
                $return_value = 1;
            } else {
                MSG_W_2003();
            }
        } else {
            if(-e "$report_merlin/merlin.rpt") {
                run_command "touch $dst_dir/estimate_perf_pass.o";
                $return_value = 1;
            } else {
                MSG_E_3006();
            }
        }
    } else {
        $return_value = 1;
    }
    return $return_value;
}

#---------------------------------------------------------------------------------------------#
# test mode (run original cpu reference and optimization code cpu reference)
#---------------------------------------------------------------------------------------------#
my $curr_dir    = $dst_dir;
my $run_ref     = "$dst_dir/run_ref";
my $run_opt     = "$dst_dir/run_opt";
if ("test" eq $flow and $is_in_project eq 1) {
    my $src_org     = "$prj_implement/$prj_src_org";
    my $src_opt     = "$prj_implement/$prj_src_opt";
    if ($org_flag eq 1) {
        print "\n  Running CPU Original C Code ...\n\n";
        # create run directory
        if(-e $src_org) {
            run_command "rm -rf $run_ref; cp -r $src_org $run_ref";
            if(-e "$src_dir/aux") {
                run_command "cp $src_dir/aux/* $run_ref >& /dev/null";
            }
            # generate reference host program
            run_command "cd $run_ref; $cmd_frontend_pass . -p cpu_ref_add_timer";
            if(-e "$MERLIN_COMPILER_HOME/mars-gen//drivers/cmost_headers") {
                run_command "cp $MERLIN_COMPILER_HOME/mars-gen//drivers/cmost_headers/__merlin_timer.* $run_ref";
            }
            run_command "cd $run_ref; $cmd_cmostcc -g *.c *.cpp -O3 -lstdc++ -lm -o run_ref -DCPU_REF_TIMER_REPORT -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin";
            # execute
            run_command "cd $run_ref; ./run_ref $xml_host_args";
        }
    }

    if ($opt_flag eq 1) {
        print "\n  Running CPU Optimized C Code ...\n\n";
        # create run directory
        run_command "rm -rf $run_opt";
        if(-e $src_opt) {
            run_command "cp -r $src_opt $run_opt";
            if(-e "$src_dir/aux") {
                run_command "cp $src_dir/aux/* $run_opt >& /dev/null";
            }
            # generate reference host program
            run_command "cd $run_opt; $cmd_cmostcc *.c *.cpp -O3 -lstdc++ -lm -o run_opt";
            # execute
            run_command "cd $run_opt; ./run_opt $xml_host_args";
        }
    }

    if ($sim_flag eq 1) {
        print "\n  Running CPU OpenCL Code ...\n\n";
    }
}

if($flow eq "test") { $dst_dir = "$curr_dir/run_sim"; }
#---------------------------------------------------------------------------------------------#
# library generation flow
#---------------------------------------------------------------------------------------------#
if ($lib_gen_flag eq 1) {
    if(not -e $src_dir or not -e "$src_dir/lib_gen") {
        print "\nERROR: Did not find library generation source code directory ...\n\n";
        exit;
    }
    # copy source code
    if(not -e $dst_dir) {
        run_command "mkdir $dst_dir";
    }
    run_command "cp -r $src_dir/* $dst_dir";
    run_command "cp -r $src_dir/aux/* $dst_dir 2>/dev/null";

    # generate library
    if($encrypt_flag eq 1) { dec_files("$dst_dir/lib_gen"); }
    run_command "cd $dst_dir/lib_gen; make -s clean; make -s lib_gen >& lib_gen.log;";
    run_command "cd $dst_dir/lib_gen; cp __merlinhead_*.h bin 2>/dev/null";
    run_command "cd $dst_dir/lib_gen; cp __merlintask_*.h bin 2>/dev/null";
    if($encrypt_flag eq 1) { delete_files("$dst_dir/lib_gen"); }
    # copy file to package
    if(not -e "$dst_dir/$prj_pkg") {
        run_command "mkdir $dst_dir/$prj_pkg";
    }
    if(-e "$dst_dir/lib_gen/bin/libkernel.so") {
        MSG_I_1003();
        run_command "cp -r $dst_dir/lib_gen/bin/* $dst_dir/$prj_pkg";
    } else {
        if($xml_debug_mode eq "debug-level2" or $xml_debug_mode eq "debug-level3") {
            run_command "cat $dst_dir/lib_gen/lib_gen.log";
        }
        MSG_E_3003();
        exit;
    }
}

#---------------------------------------------------------------------------------------------#
# host binary generation flow
#---------------------------------------------------------------------------------------------#
if ($host_flag eq 1) {
    if(not -e $src_dir or not -e "$src_dir/host") {
        print "\nERROR: Did not find host generation source code directory ...\n\n";
        exit;
    }
    # copy source code
    if(not -e $dst_dir) {
        run_command "mkdir $dst_dir";
    }
    run_command "cp -r $src_dir/* $dst_dir";
    run_command "cp -r $src_dir/aux/* $dst_dir 2>/dev/null";
    # generate host program
    if($encrypt_flag eq 1) { dec_files("$dst_dir/host"); }
    run_command "cd $dst_dir/host; make -s clean; make -s host CFLAGS=\"$xml_cflags\"";
#    if($encrypt_flag eq 1) { delete_files("$dst_dir/host"); }
    # copy file to package
    if(not -e "$dst_dir/$prj_pkg") {
        run_command "mkdir $dst_dir/$prj_pkg";
    }
    if(-e "$dst_dir/host/host_top") {
        MSG_I_1004();
        run_command "cp -r $dst_dir/host/host_top $dst_dir/$prj_pkg";
    } else {
        MSG_E_3004();
        exit;
    }
}

#---------------------------------------------------------------------------------------------#
# hls estimation flow
#---------------------------------------------------------------------------------------------#
if ($estimate_flag eq 1) {
#    printf "*** Estimation report generation ...\n";
    MSG_I_1025();
    if(not -e $src_dir or not -e "$src_dir/kernel") {
        print "\nERROR: Did not find estimate kernel source code directory ...\n\n";
        exit;
    }

    # copy source code
    if(not -e $dst_dir) {
        run_command "mkdir $dst_dir";
    }
    run_command "cp -r $src_dir/* $dst_dir";
    run_command "cp -r $src_dir/aux/* $dst_dir 2>/dev/null";
    # prepare execution files
    run_command "cp $dst_dir/kernel/*.c* $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/kernel/*.h  $dst_dir 2>/dev/null";
    if (-e "$dst_dir/host/__merlin_api_list.c") {
        run_command "cp $dst_dir/host/__merlin_api_list.c $dst_dir 2>/dev/null";
    }

    run_command "rm -rf $report_merlin >& /dev/null";

    # === Resource estimation ===
    # Output: report_merlin_res_bak
    #         estimate_res_pass.o
    my $est_res = 0;
    run_command "mkdir $report_merlin >& /dev/null";
    run_command "mkdir $report_vendor >& /dev/null";
    if (fast_dse_preproc("res") eq 1) {
        run_command "touch $dst_dir/estimate_res_pass.o";
        $est_res = 1;
        # FIXME: Nothing to do for resource model here yet
        run_command("cp $report_merlin/$global_fast_dse/res/org.json " .
                    "$report_merlin/perf_est.json");
    }
    run_command "rm -rf $report_merlin_res_bak";
    run_command "mv $report_merlin $report_merlin_res_bak";

    # === Performance estimation ===
    # Input : The result of resource model (if available)
    #         or metadata
    # Output: report_merlin_perf_bak
    #         estimate_perf_pass.o
    my $est_perf = 0;
    run_command "mkdir $report_merlin >& /dev/null";
    copy_files_to_report_merlin($src_dir, $report_merlin);
    if (fast_dse_preproc("perf") eq 1) {
        if (-f "$dst_dir/estimate_res_pass.o") {
            # Base on the resource model result
            run_command("cp $report_merlin_res_bak/perf_est.json " .
                        "$report_merlin/$global_fast_dse/perf/org.json");
        }
        $est_perf = gen_merlin_rpt("fast_dse");
    }
    run_command "rm -rf $report_merlin_perf_bak";
    run_command "mv $report_merlin $report_merlin_perf_bak";

    my $check_fast_dse = 0;
    if ($est_perf eq 1 and $est_res eq 1) {
        $check_fast_dse = 1;
    }
    run_command "mkdir $report_merlin >& /dev/null";
    run_command "mkdir $report_vendor >& /dev/null";

    # === Run vendor HLS ===
    # Output: report_merlin_ref
    #         estimate_pass.o 

    # msg_report will use these file
    copy_files_to_report_merlin($src_dir, $report_merlin);
    my $merlin_report_generated = 1;       
    if ($check_fast_dse eq 0 or $xml_dse_debug ne "") {
        my $ret = run_command "cd $dst_dir; make -s hls";
        if (-f "$dst_dir/estimate_pass.o") {
            $merlin_report_generated = gen_merlin_rpt();
        }
        run_command "rm -rf $report_merlin_ref";
        run_command "mv $report_merlin $report_merlin_ref";
    }

    # finalize the official report
    if ($check_fast_dse) {
        run_command "cp -rf $report_merlin_perf_bak $report_merlin";
        run_command "touch $dst_dir/result_from_est.o";
    }
    else { # fail to estimate due to disabled model or incomplete metadata
        run_command "cp -rf $report_merlin_ref $report_merlin";
    }

    if (-e "merlin_hls.log") {
        run_command "cp merlin_hls.log $report_merlin 2>/dev/null";
    }

    # check if successful (either HLS or estimation)
    if ($check_fast_dse or (-e "$dst_dir/estimate_pass.o" and $merlin_report_generated)) {
        MSG_I_1026();
    } else {
        MSG_E_3051();
    }
}

#---------------------------------------------------------------------------------------------#
# software simulation flow
#---------------------------------------------------------------------------------------------#
if ($sim_flag eq 1) {
    #printf "*** Software emulation bitstream generation ...\n";
    MSG_I_1027();
    if(not -e $src_dir or not -e "$src_dir/kernel") {
        print "\nERROR: Did not find software simulation source code directory ...\n\n";
        exit;
    }
    # copy source code
    if(not -e $dst_dir) {
        run_command "mkdir $dst_dir";
    }
    run_command "cp -r $src_dir/* $dst_dir";
    run_command "cp -r $src_dir/aux/* $dst_dir 2>/dev/null";
    # collect all required files
    run_command "cp $dst_dir/kernel/*.c*    $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/kernel/*.h*    $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/host/*.h*      $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/host/*.c*      $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/lib_gen/*.h*   $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/lib_gen/*.c*   $dst_dir 2>/dev/null";

    # copy file to package
    if (not -e "$dst_dir/$prj_pkg") {
        run_command "mkdir $dst_dir/$prj_pkg";
    }
    run_command "cp -r $dst_dir/$prj_pkg/* $dst_dir 2>/dev/null;";
    if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"]) {
        run_command "cd $dst_dir; make -s csim DEVICE=$xml_platform_name";
    } else {
        run_command "cd $dst_dir; make -s sim_config DEVICE=$xml_platform_name";

        # report collection
        if(not -e $report_merlin) {
            run_command "mkdir $report_merlin >& /dev/null";
        }
        if(-e "merlin_sim.log") {run_command "cp merlin_sim.log $report_merlin";}

        # check if successful
        if(-e "$dst_dir/sim_pass.o") {
            MSG_I_1028();
#            printf "\n*** Software emulation configuration file generated successfully\n";
        } else {
            MSG_E_3052();
#            printf "\n*** Software emulation configuration file generation failed\n";
        }
    }
    
}

#---------------------------------------------------------------------------------------------#
# software simulation flow
#---------------------------------------------------------------------------------------------#
if ($hw_sim_flag eq 1) {
#    printf "*** Hardware emulation bitstream generation ...\n";
    MSG_I_1029();
    if(not -e $src_dir or not -e "$src_dir/kernel") {
        print "\nERROR: Did not find hardware simulation source code directory ...\n\n";
        exit;
    }
    # copy source code
    if(not -e $dst_dir) {
        run_command "mkdir $dst_dir";
    }
    run_command "cp -r $src_dir/* $dst_dir";
    run_command "cp -r $src_dir/aux/* $dst_dir 2>/dev/null";

    # collect all required files
    run_command "cp $dst_dir/kernel/*.c*    $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/kernel/*.h*    $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/host/*.h*      $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/host/*.c*      $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/lib_gen/*.h*   $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/lib_gen/*.c*   $dst_dir 2>/dev/null";

    # copy file to package
    if(not -e "$dst_dir/$prj_pkg") {
        run_command "mkdir $dst_dir/$prj_pkg";
    }
    if($xml_tool_version ~~ ["vitis_hls", "vivado_hls"]) {
        run_command "cd $dst_dir; make -s cosim DEVICE=$xml_platform_name";
    } else {
        run_command "cp -r $dst_dir/$prj_pkg/* $dst_dir >& /dev/null;";
        run_command "cd $dst_dir; make -s hw_sim_config DEVICE=$xml_platform_name";

        # report collection
        if(not -e $report_merlin) {
            run_command "mkdir $report_merlin >& /dev/null";
        }
        if(-e "merlin_sim.log") {run_command "cp merlin_sim.log $report_merlin";}
        
        # check if successful
        if(-e "$dst_dir/hw_sim_pass.o") {
            MSG_I_1030();
#            printf "*** Hardware emulation configuration file generated successfully\n";
        } else {
            MSG_E_3053();
#            printf "*** Hardware emulation configuration file generation failed\n";
        }
    } 
}

# execute simulation
if ($execute_sim_flag eq 1) {
    run_command "cd $dst_dir; make -s exec_sim";
    if(not -e $report_merlin) {
        run_command "mkdir $report_merlin >& /dev/null";
    }
    if(-e "merlin_sim.log") {run_command "cp merlin_sim.log $report_merlin";}
}

#---------------------------------------------------------------------------------------------#
# generate bitstream
#---------------------------------------------------------------------------------------------#
if ($impl_flag eq 1) {
#    printf "*** Hardware bitstream generation ...\n";
    MSG_I_1031();
    if(not -e $src_dir or not -e "$src_dir/kernel" or not -e "$src_dir/host" or not -e "$src_dir/lib_gen") {
        print "\nERROR: Did not find implementation source code directory ...\n\n";
        exit;
    }
    # copy source code
    if(not -e $dst_dir) {
        run_command "mkdir $dst_dir";
    }
    
    run_command "cp -r $src_dir/* $dst_dir";
    run_command "cp -r $src_dir/aux/* $dst_dir 2>/dev/null";
    # generate execute directory
    run_command "cp $dst_dir/kernel/*.c*    $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/kernel/*.h*    $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/host/*.h*      $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/host/*.c*      $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/lib_gen/*.h*   $dst_dir 2>/dev/null";
    run_command "cp $dst_dir/lib_gen/*.c*   $dst_dir 2>/dev/null";

    # replace __MERLIN_PATH__ in Makefile with absolute path for tcl file
    my $absolute_path = `cd $dst_dir; pwd; cd -`;
    if($absolute_path =~ /(.*)\n(.*)\n/) {
        $absolute_path = $1;
    } elsif($absolute_path =~ /(.*)\n/) {
        $absolute_path = $1;
    }
    $absolute_path =~ s/\//\\\//g;
    run_command "cd $dst_dir; perl -pi -e 's/__MERLIN_PATH__/$absolute_path/g' Makefile 2> /dev/null";
    run_command "cd $dst_dir; make -s impl";

    # copy file to package
    if(not -e "$dst_dir/$prj_pkg") {
        run_command "mkdir $dst_dir/$prj_pkg";
    }

    # report collection
    if(not -e $report_merlin) {
        run_command "mkdir $report_merlin >& /dev/null";
    }
    if(-e "merlin_bit.log") {run_command "cp merlin_bit.log $report_merlin";}
    if ($is_in_project eq 1){
        printf "\n";
        run_command "$cmd_report_summary bit  2> .log";
    }
    
    # check if successful
    if(-e "$dst_dir/impl_pass.o") {
        MSG_I_1032();
#        printf "*** Hardware configuration file generated successfully\n";
    } else {
        MSG_E_3054();
#        printf "*** Hardware configuration file generation failed\n";
    }
}

#---------------------------------------------------------------------------------------------#
# execution on board, only work if current machine have board
#---------------------------------------------------------------------------------------------#
if($flow eq "exec" or $flow eq "test") { $dst_dir = "$curr_dir/run_bit"; }
if ($on_board_flag eq 1 and $is_in_project eq 1) {
    # copy source code
    if(not -e $dst_dir) {
        run_command "mkdir $dst_dir";
    }
    run_command "cp -r $src_dir/Makefile $dst_dir";
    run_command "mkdir $dst_dir/fpga_run";
    run_command "cp -r $prj_pkg/* $dst_dir/fpga_run";
    run_command "cp -r $src_dir/aux/* $dst_dir/fpga_run 2>/dev/null";

    if($xml_server eq "") {
        MSG_E_3007();
    }

    # execute on board
    run_command "cd $dst_dir; make -s exec HOST_ARGS=\"$xml_host_args\" SERVER=$xml_server";
}

#---------------------------------------------------------------------------------------------#
# FIXME generate report for testing environment purpose
#---------------------------------------------------------------------------------------------#
if($flow eq "exec" or $flow eq "test") {
    run_command "$cmd_report_summary test  2> .log";
}

exit;
