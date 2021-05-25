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


my $MERLIN_COMPILER_HOME;
my $merlin_library_path;
BEGIN {
    $|=1;
    $MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};
    $merlin_library_path = "$MERLIN_COMPILER_HOME/mars-gen/scripts/lib";
    our $xml_dir = "";
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

my $platform_report = $ARGV[0];
my $kernel_report = $ARGV[1];
my $vivado_report = $ARGV[2];
my $vivado_xml = $ARGV[3];
my $version = $ARGV[4];
my $json_string = "";
$json_string .= "{\n";

printf "Resource utilization summary:\n";
if($version eq 0) {
    my $length_name= 10;
    my $length_lut = 20;
    my $length_ff  = 20;
    my $length_bram= 15;
    my $length_dsp = 15;
    my $name_space = "-"x$length_name;
    my $lut_space  = "-"x$length_lut;
    my $ff_space   = "-"x$length_ff;
    my $bram_space = "-"x$length_bram;
    my $dsp_space  = "-"x$length_dsp;
    my $lut_avail, $ff_avail, $bram_avail, $dsp_avail;
    my $lut_total, $ff_total, $bram_total, $dsp_total;
    my $lut_plat, $ff_plat, $bram_plat, $dsp_plat;
    my $lut_kernel, $ff_kernel, $bram_kernel, $dsp_kernel;
    my $lut_plat_p, $ff_plat_p, $bram_plat_p, $dsp_plat_p;
    my $lut_kernel_p, $ff_kernel_p, $bram_kernel_p, $dsp_kernel_p;
    my $lut_total, $ff_total, $bram_total, $dsp_total;
    my $lut_total_p, $ff_total_p, $bram_total_p, $dsp_total_p;
    
    printf "|$name_space|$lut_space|$ff_space|$bram_space|$dsp_space|\n";
    my $platform_name = str_shift_to_right(" ", $length_name);
    my $lut_name      = str_shift_to_right("LUT        ", $length_lut);
    my $ff_name       = str_shift_to_right("FF         ", $length_ff);
    my $bram_name     = str_shift_to_right("BRAM36  ", $length_bram);
    my $dsp_name      = str_shift_to_right("DSP   ", $length_dsp);
    printf "|$platform_name|$lut_name|$ff_name|$bram_name|$dsp_name|\n";
    printf "|$name_space|$lut_space|$ff_space|$bram_space|$dsp_space|\n";
    my $platform_info       = `cat $platform_report`;
    my @platform_line_set   = split(/\n/, $platform_info);
    foreach my $platform_line (@platform_line_set) {
        if($platform_line =~ /\|\s*CLB LUTs\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $lut_avail  = $3;
            $lut_plat   = $1;
            $lut_plat_p = $4;
        }
        if($platform_line =~ /\|\s*CLB Registers\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $ff_avail  = $3;
            $ff_plat   = $1;
            $ff_plat_p = $4;
        }
        if($platform_line =~ /\|\s*Block RAM Tile\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $bram_avail  = $3;
            $bram_plat   = $1;
            $bram_plat_p = $4;
        }
        if($platform_line =~ /\|\s*DSPs\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $dsp_avail  = $3;
            $dsp_plat   = $1;
            $dsp_plat_p = $4;
        }
    }
    $avail_name   = str_shift_to_right("Available", $length_name);
    $lut_avail_f  = str_shift_to_right($lut_avail, $length_lut);
    $ff_avail_f   = str_shift_to_right($ff_avail, $length_ff);
    $bram_avail_f = str_shift_to_right($bram_avail, $length_bram);
    $dsp_avail_f  = str_shift_to_right($dsp_avail, $length_dsp);
    printf "|$avail_name|$lut_avail_f|$ff_avail_f|$bram_avail_f|$dsp_avail_f|\n";
    
    $plat_name = str_shift_to_right("Platform ", $length_name);
    $lut_plat_f  = str_shift_to_right("$lut_plat ($lut_plat_p%)", $length_lut);
    $ff_plat_f   = str_shift_to_right("$ff_plat ($ff_plat_p%)", $length_ff);
    $bram_plat_f = str_shift_to_right("$bram_plat ($bram_plat_p%)", $length_bram);
    $dsp_plat_f  = str_shift_to_right("$dsp_plat ($dsp_plat_p%)", $length_dsp);
    printf "|$plat_name|$lut_plat_f|$ff_plat_f|$bram_plat_f|$dsp_plat_f|\n";
    
    my $kernel_info       = `cat $kernel_report`;
    my @kernel_line_set   = split(/\n/, $kernel_info);
    foreach my $kernel_line (@kernel_line_set) {
        if($kernel_line =~ /\|\s*CLB LUTs\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $lut_kernel   = $1;
            $lut_kernel_p = $4;
        }
        if($kernel_line =~ /\|\s*CLB Registers\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $ff_kernel   = $1;
            $ff_kernel_p = $4;
        }
        if($kernel_line =~ /\|\s*Block RAM Tile\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $bram_kernel   = $1;
            $bram_kernel_p = $4;
        }
        if($kernel_line =~ /\|\s*DSPs\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|\s*(\S*)\s*\|/) {
            $dsp_kernel   = $1;
            $dsp_kernel_p = $4;
        }
    }
    $kernel_name   = str_shift_to_right("Kernel  ", $length_name);
    $lut_kernel_f  = str_shift_to_right("$lut_kernel ($lut_kernel_p%)", $length_lut);
    $ff_kernel_f   = str_shift_to_right("$ff_kernel ($ff_kernel_p%)", $length_ff);
    $bram_kernel_f = str_shift_to_right("$bram_kernel ($bram_kernel_p%)", $length_bram);
    $dsp_kernel_f  = str_shift_to_right("$dsp_kernel ($dsp_kernel_p%)", $length_dsp);
    printf "|$kernel_name|$lut_kernel_f|$ff_kernel_f|$bram_kernel_f|$dsp_kernel_f|\n";
    printf "|$name_space|$lut_space|$ff_space|$bram_space|$dsp_space|\n";
    
    $lut_total  = $lut_plat + $lut_kernel;
    $ff_total   = $ff_plat + $ff_kernel;
    $bram_total = $bram_plat + $bram_kernel;
    $dsp_total  = $dsp_plat + $dsp_kernel;
    $lut_total_p  = $lut_plat_p + $lut_kernel_p;
    $ff_total_p   = $ff_plat_p + $ff_kernel_p;
    $bram_total_p = $bram_plat_p + $bram_kernel_p;
    $dsp_total_p  = $dsp_plat_p + $dsp_kernel_p;
    $total_name   = str_shift_to_right("Total  ", $length_name);
    $lut_total_f  = str_shift_to_right("$lut_total ($lut_total_p%)", $length_lut);
    $ff_total_f   = str_shift_to_right("$ff_total ($ff_total_p%)", $length_ff);
    $bram_total_f = str_shift_to_right("$bram_total ($bram_total_p%)", $length_bram);
    $dsp_total_f  = str_shift_to_right("$dsp_total ($dsp_total_p%)", $length_dsp);
    printf "|$total_name|$lut_total_f|$ff_total_f|$bram_total_f|$dsp_total_f|\n";
    printf "|$name_space|$lut_space|$ff_space|$bram_space|$dsp_space|\n";
} else {
    my $platform_info       = `cat $platform_report`;
    my @platform_line_set   = split(/\n/, $platform_info);
    my $flag_util = 0;
    foreach my $platform_line (@platform_line_set) {
        if($platform_line =~ /SDx System Utilization/) {
            $flag_util = 1;
        } elsif($platform_line =~ /Accelerator Utilization Design Information/) {
            $flag_util = 1;
        } 
        if($flag_util eq 1) {
            if($platform_line =~ /\+.*\+.*\+.*\+.*\+.*\+/) {
                printf "$platform_line\n";
            }
            if($platform_line =~ /\|.*\|.*\|.*\|.*\|.*\|/) {
                printf "$platform_line\n";
            }
        }
    }
    # get json file from vendor report
    my $key1 = "";
    my $key2 = "";
    my $key3 = "";
    my $key4 = "";
    my $key5 = "";
    my $key6 = "";
    foreach my $platform_line (@platform_line_set) {
        if($platform_line =~ /\|\s+Name\s+\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
            $key1 = $1;
            $key2 = $2;
            $key3 = $3;
            $key4 = $4;
            $key5 = $5;
            $key6 = $6;
            $key1 =~ s/ //g;
            $key2 =~ s/ //g;
            $key3 =~ s/ //g;
            $key4 =~ s/ //g;
            $key5 =~ s/ //g;
            $key6 =~ s/ //g;
        } elsif($platform_line =~ /\|\s*Name\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
            $key1 = $1;
            $key2 = $2;
            $key3 = $3;
            $key4 = $4;
            $key5 = $5;
            $key1 =~ s/ //g;
            $key2 =~ s/ //g;
            $key3 =~ s/ //g;
            $key4 =~ s/ //g;
            $key5 =~ s/ //g;
        } elsif($platform_line =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
            my $data0 = $1;
            my $data1 = $2;
            my $data2 = $3;
            my $data3 = $4;
            my $data4 = $5;
            my $data5 = $6;
            my $data6 = $7;
            $data0 =~ s/ //g;
            $data1 =~ s/ //g;
            $data2 =~ s/ //g;
            $data3 =~ s/ //g;
            $data4 =~ s/ //g;
            $data5 =~ s/ //g;
            $data6 =~ s/ //g;
            $json_string .= "\t\"$data0\" : {\n";
            $json_string .= "\t\t\"$key1\" : \"$data1\",\n";
            $json_string .= "\t\t\"$key2\" : \"$data2\",\n";
            $json_string .= "\t\t\"$key3\" : \"$data3\",\n";
            $json_string .= "\t\t\"$key4\" : \"$data4\",\n";
            $json_string .= "\t\t\"$key5\" : \"$data5\",\n";
            $json_string .= "\t\t\"$key6\" : \"$data6\"\n";
            $json_string .= "\t},\n";
        } elsif($platform_line =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|/) {
            my $data0 = $1;
            my $data1 = $2;
            my $data2 = $3;
            my $data3 = $4;
            my $data4 = $5;
            my $data5 = $6;
            $data0 =~ s/ //g;
            $data1 =~ s/ //g;
            $data2 =~ s/ //g;
            $data3 =~ s/ //g;
            $data4 =~ s/ //g;
            $data5 =~ s/ //g;
            $json_string .= "\t\"$data0\" : {\n";
            $json_string .= "\t\t\"$key1\" : \"$data1\",\n";
            $json_string .= "\t\t\"$key2\" : \"$data2\",\n";
            $json_string .= "\t\t\"$key3\" : \"$data3\",\n";
            $json_string .= "\t\t\"$key4\" : \"$data4\",\n";
            $json_string .= "\t\t\"$key5\" : \"$data5\"\n";
            $json_string .= "\t},\n";
        }
    }
#    $json_string =~ s/},\n}/}\n}/g;
}

printf "\n";
$json_string .= "\t\"Timing\" : {\n";
printf "Timing summary:\n";
my $vivado_info       = `cat $vivado_report`;
my @vivado_line_set   = split(/\n/, $vivado_info);
foreach my $vivado_line (@vivado_line_set) {
    if($vivado_line =~ /Estimated Timing Summary \|\s*WNS=(\S*)\s*\|\s*TNS=(\S*)\s*\|\s*WHS=(\S*)\s*\|\s*THS=(\S*)\s*\|/) {
        printf "WNS=$1 TNS=$2 WHS=$3 THS=$4\n";
        $json_string .= "\t\t\"WNS\" : \"$1\",\n";
        $json_string .= "\t\t\"TNS\" : \"$2\",\n";
        $json_string .= "\t\t\"WHS\" : \"$3\",\n";
        $json_string .= "\t\t\"THS\" : \"$4\",\n";
    }
}
my $freq = "Unknown";
my $freq_target = "Unknown";
my $freq_data   = "Unknown";
my $freq_kernel = "UNknown";
my $xml_info = `cat $vivado_xml`;
if($xml_info =~ /clockFreq=\"(.*)MHz\"/) {
    $freq_target = $1;
    $freq_data = $1;
}
#currently, from vivado.log
#Pin xcl_design_i/expanded_region/u_ocl_region/KERNEL_CLK2 has no clock
#so noe KERNE_CLK is useless now, just use DATA_CLK
if($xml_info =~ /DATA_CLK.*frequency=\"(.*)MHz\"/) {
    $freq_data = $1;
}

#foreach my $vivado_line (@vivado_line_set) {
#    if($vivado_line =~ /KERNEL_CLK.*changed to (.*) MHz/) {
#        $freq = "$1 MHz";
#    }
#}

#currently,
#Pin xcl_design_i/expanded_region/u_ocl_region/KERNEL_CLK2 has no clock
#so noe KERNE_CLK is useless now.
#if($vivado_info =~ /KERNEL_CLK.*\n\s*original frequency\s+:\s+(.*)Mhz.*\n\s*scaled frequency\s+:\s+(.*)Mhz/) {
#    $freq_data = $2;
#}
#if($vivado_info =~ /DATA_CLK.*\n\s*original frequency\s+:\s+(.*)Mhz.*\n\s*scaled frequency\s+:\s+(.*)Mhz/) {
#    $freq_target = $1;
#    $freq_data = $2;
#}
printf "Kernel Frequency: $freq_data MHz\n\n";
$json_string .= "\t\t\"Frequency\" : \"$freq_data\"\n";
$json_string .= "\t}\n";
$json_string .= "}\n";
my $filename = 'merlin.json';
system "rm -rf $filename";
open(my $fh, '>', $filename) or die "Could not open file '$filename' $!";
print $fh $json_string;
close $fh;
