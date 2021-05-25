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


##############################################
# This perl script summarize the report of mars project
# it is executed at project directory
##############################################

$mars_compiler_home = $ENV{'MERLIN_COMPILER_HOME'};
my $mars_flow_cmd = "mars_python ${mars_compiler_home}/mars-gen/scripts/merlin_flow/merlin_flow_start_new.py";

$work_dir       = "implement";
$opencl_gen_dir = "$work_dir/opencl_gen";
$hls_dir        = "$work_dir/exec/hls";
$phys_dir       = "$work_dir/phys";         # perform physical implementation
$test_dir       = "$work_dir/exec";         # perform physical implementation

use File::Basename;
$directive_file = "spec/directive.xml";
if(-e $directive_file) {
    my $info = `cat $directive_file`;
    if($info =~ /<implementation_tool>(.*)<\/implementation_tool>/) {
        if($1 =~ /sdaccel/) {
            $tool_flag = "sdaccel";
        }
    }
}

$summary_file= "$work_dir/merlin_summary.rpt";
system "rm -rf $summary_file";

sub append_string_to_file {
    my ($lines,$filename) = @_;
    open (FILE, ">>$filename");
    print FILE "$lines";
    close (FILE);
}
sub str_shift_to_left {
    $str_tmp = $_[0];
    $len_tmp = $_[1];
    if (@_ > 2)  {
        $max_len= $_[2];
    }
    my $i;
    $str_len = length($str_tmp);
    if ($str_len < $len_tmp) {
        for ($i = $str_len; $i < $len_tmp; $i+=1) {
            $str_tmp .= " ";
        }
    }
    if (@_ > 2 and $str_len > $len_tmp) {
        $str_tmp = substr($str_tmp, 0, $len_tmp); 
    }
    return $str_tmp;
}
sub str_shift_to_right {
    $str_tmp = $_[0];
    $len_tmp = $_[1];
    if (@_ > 2)  {
        $max_len= $_[2];
    }
    my $i;
    $str_len = length($str_tmp);
    if ($str_len < $len_tmp) {
        for ($i = $str_len; $i < $len_tmp; $i+=1) {
            $str_tmp = " ".$str_tmp;
        }
    }
    if (@_ > 2 and $str_len > $len_tmp) {
        $str_tmp = substr($str_tmp, 0, $len_tmp); 
    }
    return $str_tmp;
}
sub str_shift_to_write {
    $str_tmp = $_[0];
    $len_tmp = $_[1];
    my $i;
    $str_len = length($str_tmp);
    if ($str_len < $len_tmp) {
        for ($i = $str_len; $i < $len_tmp; $i+=1) {
            $str_tmp = " ".$str_tmp;
        }
    }
    return $str_tmp;
}

sub get_hls_summary{
    my ($filename,$skip_header,@other) = @_;
    my $file_info= `cat $filename`;

    $kernel_name = "";
    $lat_min     = "";
    $lat_max     = "";
    $clk_period  = "";

    my $str = "\n";
    if (0 == $skip_header) {
        if ($file_info =~ /\n\* (Version.*)\n\* Project:\s*(\S*)\n.*\n\* (Product family.*)\n\* (Target device.*)\n/) {
            $str .= "$1\n";
            $kernel_name = $2;
            $str .= "$3\n";
            $str .= "$4\n";
        }

        if ($file_info =~ /\n\+ Timing.*\n.*\n.*\n.*\n.*\n(.*)\n/) {
            $line = $1;
            if ($line =~ /\s*\|default\s*\|\s*(\d*\.\d*)\|\s*(\d*\.\d*)\|\s*(\d*\.\d*)/) {
                $str .= "Target clock   : $1 ns\n";
                $str .= "Uncertainty    : $3 ns\n";
            }
        }
    }
    if ($file_info =~ /\n\* (Version.*)\n\* Project:\s*(\S*)\n.*\n\* (Product family.*)\n\* (Target device.*)\n/) {
        $kernel_name = $2;
    }
    if ($file_info =~ /\n\+ Timing.*\n.*\n.*\n.*\n.*\n(.*)\n/) {
        $line = $1;
        if ($line =~ /\s*\|default\s*\|\s*(\d*\.\d*)\|\s*(\d*\.\d*)\|\s*(\d*\.\d*)/) {
            $clk_target = $1;
            $clk_period = $2;
            $clk_uncertain = $3;
        }
    }
    if ($file_info =~ /\n\+ Latency.*\n.*\n.*\n.*\n.*\n.*\n(.*)\n/) {
        $line = $1;
        if ($line =~ /\s*\|\s*(\d*)\|\s*(\d*)\|/) {
            $lat_min = $1; #"Latency min    : $1 cycles\n";
            $lat_max = $2; #"Latency max    : $2 cycles\n";
        }
    }

    $one_marker = "";
    if ($file_info =~ /\n== Utilization.*\n.*\n.*\n(.*\n.*\n.*\n)/) {
        $line = $1;
        
        $str_len = length($kernel_name);
        $kernel_name_str = $kernel_name;
        if ($str_len < 17) {
            for ($i = length($kernel_name); $i < 17; $i+=1) {
                $kernel_name_str .= " ";
            }
        }
        else {
          $kernel_name_str = substr($kernel_name, 0, $str_len);
        }

        my @lines_set = split(/\n/, $line);
        $lines_set[1] =~ s/\s*Name\s*/$kernel_name_str/g;

        $lines_set[0] .= "-----------+--------+";
        $lines_set[1] .= "Latency max|CK slack|";
        $lines_set[2] .= "-----------+--------+";

        $line = join("\n", @lines_set);
        $line .= "\n";
        $one_marker = $lines_set[0];
        
        $str .= $line ;
    }

    if ($file_info =~ /\n(\|Total .*\n.*\n.*\n.*\n.*\n.*\n.*\n)/) {
        $line = $1;
        $line =~ s/\+\-*.*\n//g;
        my @lines_set = split(/\n/, $line);

        $clk_constrain = $clk_target - $clk_uncertain;
        $clk_slack = $clk_constrain - $clk_period;
        $str_slack = sprintf "%.2lf\n", $clk_slack;
        chomp($str_slack);
        
        if (length($lat_max) > 8) {
            $lat_max = sprintf("%1.4e", $lat_max);
        }
         
        $lat_min   = str_shift_to_write($lat_min, 11);
        $lat_max   = str_shift_to_write($lat_max, 11); 
        $str_slack = str_shift_to_write($str_slack, 8); 

        $lines_set[0] .= $lat_max."|".$str_slack."|";
        $lines_set[1] .= "     cycles|      ns|";
        $lines_set[2] .= "           |        |";

        $line = join("\n", @lines_set);
        $line .= "\n".$one_marker."\n"; 

        $str .= $line;
    }

    return $str;
}

sub get_freq_from_period {
    my $input_clk_period = $_[0];
    if ($input_clk_period == 0)  {return "";}
    my $freq_clk_tmp = sprintf ("%.0lf", (1000 / $input_clk_period));
    return $freq_clk_tmp;
}

my $LUT_all = 0;
my $FF_all = 0;
my $BRAM18_all = 0;
my $DSP_all = 0;
sub get_all_summary {
    my ($phys_dir,@other) = @_;
    my $str = "";
    if (-e "$phys_dir/design_1_wrapper_utilization_placed.rpt") {
       $place_info = `cat $phys_dir/design_1_wrapper_utilization_placed.rpt`;
       $route_info = `cat $phys_dir/design_1_wrapper_timing_summary_routed.rpt`;
    } elsif (-e "$phys_dir/report_merge_utilization.rpt") {
        $place_info = `cat $phys_dir/report_merge_utilization.rpt`;
        $route_info = `cat $phys_dir/report_timing_summary.rpt`;
    }

    my $device;
    my $place_status;
    if ($place_info =~ /\n\|\s*Device\s*:\s*(\S*)\n\|\s*Design State :\s*(.*)\n/) {
        $device = $1;
        $place_status = $2;
    }
    my $device_detail="";
    if ($route_info =~ /\n\|\s*Device\s*:\s*(\S*)\n\|\s*Speed File\s*:\s*(\S*)\s*/) {
        $device_detail = $1;
        $speed_grade = $2;
        $device_detail =~ s/\-//g;
        $device_detail = "xc$device_detail$speed_grade";
    }

    my @used_res=("       NG","     NG","      NG","      NG");   # [LUT, FF, BRAM, DSP]
    my @totl_res=("       NG","     NG","      NG","      NG");
    my @util_res=("       NG","     NG","      NG","      NG");
    my $clk_slack = "      NG";

    if ($place_info =~ /\n\|\s*Slice LUTs\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
        $used_res[0] = str_shift_to_write($1, 8);
        $totl_res[0] = str_shift_to_write($3, 8);
        $util_res[0] = str_shift_to_write($4, 8);
        $LUT_all = $totl_res[0];
    }
    if ($place_info =~ /\n\|\s*Slice Registers\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
        $used_res[1] = str_shift_to_write($1, 8);
        $totl_res[1] = str_shift_to_write($3, 8);
        $util_res[1] = str_shift_to_write($4, 8);
        $FF_all = $totl_res[1];
    }
    if ($place_info =~ /\n\|\s*Block RAM Tile\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
        $used_res[2] = str_shift_to_write($1*2, 9);
        $totl_res[2] = str_shift_to_write($3*2, 9);
        $util_res[2] = str_shift_to_write($4, 9);
        $BRAM18_all = $totl_res[2];
    }
    if ($place_info =~ /\n\|\s*DSPs\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
        $used_res[3] = str_shift_to_write($1, 7);
        $totl_res[3] = str_shift_to_write($3, 7);
        $util_res[3] = str_shift_to_write($4, 7);
        $DSP_all = $totl_res[3];
    }

    my $user_clk_period;
    if ($route_info =~ /\n\s*clk_out1_design_1_clk_wiz_0_0\s*\{\S*\s*\S*\}\s*(\S*)\s*/) {
        $user_clk_period= $1;
        $user_clk_freq = get_freq_from_period($user_clk_period);
    }
    my $axi_clk_period;
    if ($route_info =~ /\n\s*mmcm_clkout0\s*\{\S*\s*\S*\}\s*(\S*)\s*/) {
        $axi_clk_period= $1;
        $axi_clk_freq = get_freq_from_period($axi_clk_period);
    }
    my $pcie_clk_period;
    if ($route_info =~ /\n\s*clk_125mhz\s*\{\S*\s*\S*\}\s*(\S*)\s*/) {
        $pcie_clk_period= $1;
        $pcie_clk_freq = get_freq_from_period($pcie_clk_period);
    }

    if($tool_flag eq "sdaccel") {
        $clk_slack = 0;      
    }
    if ($clk_slack =~ /^\-/) {
        $route_status = "Timing Violated";
    }
    else {
        $route_status = "Timing Met";
    }
    $clk_slack = str_shift_to_write($clk_slack, 8);

    if (-e "$work_dir/../merlin.log") {
        $timer_info = `cat $work_dir/../merlin.log`;
        if ($timer_info =~ /merlin_flow used time = (\S*) minutes\n/) {
            $fpga_compile_time = $1;
        }
    }
    $fpga_compile_time_p = sprintf "%.0f",$fpga_compile_time;

    $str .= "Compile Time   : $fpga_compile_time_p minutes \n";
    $str .= "Device         : $device_detail\n";
    $str .= "Place State    : $place_status\n";
    $str .= "Route State    : $route_status\n";
    $str .= "user_clk       : $user_clk_freq Mhz ($user_clk_period ns)\n";
    $str .= "axi_clk        : $axi_clk_freq Mhz ($axi_clk_period ns)\n";
    $str .= "pcie_clk       : $pcie_clk_freq Mhz ($pcie_clk_period ns)\n";
    $str .= "               utlization of compelete project\n";    
    $str .= "+-----------------+---------+-------+--------+--------+--------+\n";    
    $str .= "|all              | BRAM_18K| DSP48E|   FF   |   LUT  |CK slack|\n";
    $str .= "+-----------------+---------+-------+--------+--------+--------+\n";    
    $str .= "|Used             |$used_res[2]|$used_res[3]|$used_res[1]|$used_res[0]|$clk_slack|\n";
    $str .= "|Available        |$totl_res[2]|$totl_res[3]|$totl_res[1]|$totl_res[0]|      ns|\n";
    $str .= "|Utilization (%)  |$util_res[2]|$util_res[3]|$util_res[1]|$util_res[0]|        |\n";
    $str .= "+-----------------+---------+-------+--------+--------+--------+\n";    

    return $str;
}

########################################################################
# Start the process
########################################################################
if (1 < @ARGV) {
	print "Usage : mars_summarize.pl [flow]\n";
    exit;
}

$flow = "full";
if (0 < @ARGV) {
    $flow = $ARGV[0];
}

########################################################################
#1. General Information
#   1.1 mars_version
########################################################################
$curr_dir = $opencl_gen_dir;
$curr_rpt = "$curr_dir/summary.rpt";
if ("opencl_gen" eq $flow) {
    system "rm -rf $curr_rpt";
    $lines = `$mars_flow_cmd -v`;
    $lines =~ s/\nFeature.*//g;
    if (-e "$curr_dir/time.o") {
        $lines .= `cat $curr_dir/time.o`;
    }
    append_string_to_file($lines, $curr_rpt);
    exit;
} elsif (-e $curr_rpt) {
    $lines = `cat $curr_rpt`; 
    append_string_to_file($lines, $summary_file);
}

########################################################################
#2. HLS
########################################################################
$curr_dir = $hls_dir;
$curr_rpt = "$curr_dir/summary.rpt";
if ("hls" eq $flow) {
    system "rm -rf $curr_rpt";
    $lines = "\n";
    $lines .= "===================================================\n";
    $lines .= "  High Level Synthesis                              \n";
    $lines .= "===================================================\n";
    {
        $kernel_list = "$work_dir/export/kernel/__merlin_kernel_list.h";
        $kernel_list_info= `cat $kernel_list`;
        if (-e "$kernel_list") {
            $idx = 0;
            @kernel_set=split(/\n/,$kernel_list_info);
            foreach $one_kernel (@kernel_set) {
                if($one_kernel =~ /\/\/(.*)=(.*)$/) {
                    $kernel_name = $1;
                    $hls_lines = get_hls_summary("$hls_dir/report/${kernel_name}/${kernel_name}_csynth.rpt", $idx!=0);
                    $lines .= $hls_lines;
                    $idx += 1;
                }
            }
        }
    }
    
    if (-e "$curr_dir/time.o") {
        $lines .= `cat $curr_dir/time.o`;
    }

    append_string_to_file($lines, $curr_rpt);
    exit;
} elsif (-e $curr_rpt)  {
    $lines = `cat $curr_rpt`; 
    append_string_to_file($lines, $summary_file);
}

########################################################################
#3. Logis Synthesis and Bit Generation
########################################################################
$curr_dir = $phys_dir;
$curr_rpt = "$curr_dir/summary.rpt";
if ("bit" eq $flow) {
    system "rm -rf $curr_rpt";
    $lines = "\n";
    $lines .= "===================================================\n";
    $lines .= "  Logic Synthesis, Place & Route                   \n";
    $lines .= "===================================================\n";
    if($tool_flag eq "sdaccel") {
        $vivado_version= `vivado -version | grep Vivado`;
        $lines .= $vivado_version;
        $lines .= `which vivado`;
    }

    {
        $phys_lines = get_all_summary("$phys_dir/report"); $lines .= $phys_lines;
    }
    
    if (-e "$curr_dir/time.o") {
        $lines .= `cat $curr_dir/time.o`;
    }
    append_string_to_file($lines, $curr_rpt);
    exit;
} elsif (-e $curr_rpt) {
    $lines = `cat $curr_rpt`; 
    append_string_to_file($lines, $summary_file);
}

########################################################################
#4. Testing
########################################################################
$curr_dir = $test_dir;
$curr_rpt = "$curr_dir/summary.rpt";
if ("test" eq $flow) {
    system "rm -rf $curr_rpt";
    $lines = "\n";
    $lines .= "===================================================\n";
    $lines .= "  Execution and Testing                            \n";
    $lines .= "===================================================\n";
    $lines .= "Host      :".`hostname`;
    $lines .= "gcc       :".`gcc -v |& grep version`;
    $lines .= "Date      :".`date`;
    $lines .= "Vivado    :".`which vivado 2> /dev/null`;
    $lines .= "Vivado_HLS:".`which vivado_hls 2> /dev/null`;
    $lines .= "===================================================\n";

    if (-e "$curr_dir/run_sim/sdaccel.log") 
    {
        $sdaccel_sim_err = `grep ERROR $curr_dir/run_sim/sdaccel.log`;
        if ($sdaccel_sim_err =~ /^ERROR/)
        {
            system "rm -rf $curr_dir/compare_alg_pass.o";
            system "touch  $curr_dir/compare_alg_fail.o";
        }
    }

    my $has_bitstream = 1;
    {
        if(!-e "pkg/kernel_top.xclbin" and ! -e "pkg/system.bit") {
            $has_bitstream = 0;
        }
    }
    $opt_status = "Fail";
    if (-e "$curr_dir/compare_opt_pass.o") { $opt_status = "Pass"; }
    if (-e "$curr_dir/compare_opt_fail.o") { $opt_status = "Fail"; }
    if (-e "$curr_dir/run_opt/pass.o") { $opt_status = "Pass"; }
    if (-e "$curr_dir/run_opt/fail.o") { $opt_status = "Fail"; }
    $alg_status = "Fail";
    if (-e "$curr_dir/compare_alg_pass.o") { $alg_status = "Pass"; }
    if (-e "$curr_dir/compare_alg_fail.o") { $alg_status = "Fail"; }
    if (-e "$curr_dir/run_sim/bin/pass.o") { $alg_status = "Pass"; }
    if (-e "$curr_dir/run_sim/bin/fail.o") { $alg_status = "Fail"; }
    if (-e "$curr_dir/run_sim/pass.o") { $alg_status = "Pass"; }
    if (-e "$curr_dir/run_sim/fail.o") { $alg_status = "Fail"; }
    $bit_status = "Fail";
    if (-e "$curr_dir/compare_bit_pass.o" and $has_bitstream eq "1") { $bit_status = "Pass"; }
    if (-e "$curr_dir/compare_bit_fail.o" or  $has_bitstream eq "0") { $bit_status = "Fail"; }
    if (-e "$curr_dir/run_bit/fpga_run/pass.o") { $bit_status = "Pass"; }
    if (-e "$curr_dir/run_bit/fpga_run/fail.o") { $bit_status = "Fail"; }


    if (-e "$curr_dir/run_bit/fpga_run/timer.rpt") {
        $timer_info = `cat $curr_dir/run_bit/fpga_run/timer.rpt`;
        if ($timer_info =~ /Total Kernel   time : (\S*) \S*\nTotal Transfer time : (\S*) /) {
            $fpga_compt_t = $1;
            $fpga_trnsf_t = $2;
            $fpga_total_t = $fpga_compt_t + $fpga_trnsf_t;
            $fpga_total_t_real = $fpga_total_t;
        }
    }
    
    if (-e "$curr_dir/../../merlin_test.log") {
        $timer_info = `cat $curr_dir/../../merlin_test.log`;
        if ($timer_info =~ /Running FPGA on-board[\S\s]*kernel compute time: (\S*) second[\S\s]*PCIe transfer  time: (\S*) second/) {
            $fpga_compt_t = $1;
            $fpga_trnsf_t = $2;
            $fpga_total_t = $fpga_compt_t + $fpga_trnsf_t;
            $fpga_total_t_real = $fpga_total_t;
        }
    }

    $fpga_compt_t = str_shift_to_write($fpga_compt_t, 13); 
    $fpga_trnsf_t = str_shift_to_write($fpga_trnsf_t, 13);
    $fpga_total_t = str_shift_to_write($fpga_total_t, 13);
    
    if (-e "$curr_dir/run_ref/timer.rpt") {
        $timer_info = `cat $curr_dir/run_ref/timer.rpt`;
        if ($timer_info =~ /Total Kernel   time : (\S*) \S*\nTotal Transfer time : (\S*) /) {
            $cpu_total__t = $1;
            $cpu_total_t_real = $cpu_total__t;
        }
    }
    $cpu_total__t = str_shift_to_write($cpu_total__t, 13); 
    
    if ("" ne $fpga_total_t_real and "" ne $cpu_total_t_real) {
        $speed_____up = sprintf "%.2lf", $cpu_total_t_real / $fpga_total_t_real;
    }
    $speed_____up = str_shift_to_write($speed_____up, 13); 

    $lines .= "Correctness : \n";
    $lines .= "    Optimized C code     : $opt_status\n";
    $lines .= "    OpenCL Sim on CPU    : $alg_status\n";
    $lines .= "    HW execution on FPGA : $bit_status\n";
    $lines .= "Performance : \n";
    $lines .= "    +=================+==============+\n";    
    $lines .= "    |                 | Runtime (s)  |\n";
    $lines .= "    +-----------------+--------------+\n";    
    $lines .= "    |FPGA compute     |$fpga_compt_t |\n";
    $lines .= "    |PCIe transfer    |$fpga_trnsf_t |\n";
    $lines .= "    |FPGA kernel total|$fpga_total_t |\n";
    $lines .= "    +-----------------+--------------+\n";    
    $lines .= "    |CPU  kernel total|$cpu_total__t |\n";
    $lines .= "    +-----------------+--------------+\n";    
    $lines .= "    |Speed up         |$speed_____up |\n";
    $lines .= "    +=================+==============+\n";    

    if (-e "$curr_dir/time.o") {
        $lines .= `cat $curr_dir/time.o`;
    }

    append_string_to_file($lines, $curr_rpt);
    exit;
} elsif (-e $curr_rpt) {
    $lines = `cat $curr_rpt`; 
    append_string_to_file($lines, $summary_file);
}

exit;

