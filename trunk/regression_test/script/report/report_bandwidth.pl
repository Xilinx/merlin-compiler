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

#!/usr/bin/perl
use File::Basename;
use XML::Simple;
use Data::Dumper;
use Spreadsheet::WriteExcel;
# Create a new Excel workbook
system "rm -rf result_collect.xls";
my $workbook = Spreadsheet::WriteExcel->new('result_collect.xls');
# Add a worksheet
$worksheet = $workbook->add_worksheet();
#  Add and define a format
$format = $workbook->add_format(); # Add a format
$format->set_bold();
$format->set_color('red');
$format->set_align('center');

$burst_limit = 4096; #4096
$run_times = 1;
$server_ku3 = "root\@10.0.0.6";
$server_baidu = "root\@10.0.0.10";

$MARS_BUILD_DIR = $ENV{'MARS_BUILD_DIR'};
$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$remote_exec = "mars_perl $MARS_BUILD_DIR/scripts/remote_exec.pl";

$xml_file = "ddr_bandwidth.xml";

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

#change run flow in xml to specify flow
sub get_platform{
    my $platform;
    $config_file = "$xml_file";
    $config_list = XML::Simple->new(ForceArray => 1);
    $config_all = $config_list->XMLin($config_file);
    my @config_set = @{$config_all->{"config"}};
    foreach my $config_one (@config_set) {
	$platform = $config_one->{"platform_name"}[0];
#	printf "$platform\n";
    }
    return $platform;
}

$dir_list = `ls`;
@dir_set = split(/\n/, $dir_list);

my @data_set;
for($i=0; $i<20; $i++) {
    $data_set[$i] = "";
}

sub get_report{
    $task = $_[0];

    $platform_name = get_platform();
#    printf "$platform_name\n";
    $worksheet = $workbook->add_worksheet("kernel $task ddr");
    $row=0;
    $col=1;
    $count_dir = 0;
    $head1 = "+------------------+";
    $head2 = "|   Burst Length   |";
    $head3 = "+------------------+";
    foreach($i=1; $i<=$burst_limit; $i=$i*2) {
        $burst = $i * 4 * 256;
        $burst = "${i}KB";
        $burst = str_shift_to_right($burst,8,8);
        $head1 .= "--------+";
        $head2 .= "$burst|";
        $head3 .= "--------+";
        $worksheet->write($row, $col, $burst);
        $col++;
    }
    printf "$head1\n";
    printf "$head2\n";
    printf "$head3\n";
    
    $row=1;
    $col=0;
    foreach $one_dir (@dir_set) {
#        printf "$one_dir\n";
        if ( (-d "$one_dir") && ($one_dir ne "report") ) {
	    if($one_dir =~ /run_(${task}.*)/) {
		if($platform_name eq "baidu_kintex7") {
		    system "cd $one_dir/run/work/$1-$task; merlin_flow run test > log";
#		    system "cd $one_dir/run/work/$1-$task; merlin_flow_cloud run test";
#		    sleep(600);
		}
		$bit_path = "$one_dir/run/work/$1-$task/run/implement/pkg";

                $one_dir = str_shift_to_right($one_dir,18,18);
                $data_set[$count_dir] = "|$one_dir|";
                $col=0;
                $worksheet->write($row, $col, $one_dir);
                $col=1;
                foreach($i=1; $i<=$burst_limit; $i=$i*2) {
                    $burst = $i * 4 * 256;
                    $total_bandwidth = 0;
		    $mean_bandwidth = "";
                    foreach($test_time=0; $test_time<$run_times; $test_time++) {
#                        system "cd $bit_path; rm -rf log; ./project_bit.exe $burst 1000 > log";
			if($platform_name eq "alphadata_pcieku3") {
                            system "cd $bit_path/; rm -rf result.log;";
                            system "cd $bit_path/../; $remote_exec 'source /curr/hanhu/source_sdaccel.sh; ./project_bit.exe $burst 1000 > result.log' -server $server_ku3 -copy -ldir ./pkg > /dev/null";
			} elsif($platform_name eq "baidu_kintex7") {
                            system "cd $bit_path/; rm -rf result.log;";
                            system "cd $bit_path/../; $remote_exec 'source /curr/hanhu/source_baidu.sh; ./project_bit.exe $burst 1000 > result.log' -server $server_baidu -copy -ldir ./pkg > /dev/null";
			} else {
			    printf "ERROR : $platform_name not supported.\n";
			    exit;
			}
                        $report_file = "$bit_path/result.log";
                        $report_info = `cat $report_file`;
                        
                        if($report_info =~ /test burst length = (\S*),/) {
                            $burst_length = $1;
                        }
                        if($report_info =~ /with iterations = (\S*)/) {
                            $iterations = $1;
                        }
                        if($report_info =~ /Total Kernel time: (\S*) /) {
                            $ddr_transfer_time = $1;
                        }
                        $ddr_data_amount = $burst_length * $iterations * 4 / 1000000000;
                        $ddr_bandwidth = $ddr_data_amount / $ddr_transfer_time;
                        $ddr_bandwidth = sprintf("%.2f",$ddr_bandwidth);
                        $total_bandwidth = $total_bandwidth + $ddr_bandwidth;
                    }
                    $mean_bandwidth = $total_bandwidth / $run_times;
                    $mean_bandwidth = sprintf("%.2f",$mean_bandwidth);
#                    printf "dir = $one_dir, memcpy burst = ${burst}B, average bandwidth = ${mean_bandwidth}GB/s\n";
       	            $mean_bandwidth = str_shift_to_right($mean_bandwidth,8,8);
                    $data_set[$count_dir] .= "$mean_bandwidth|";
                    $worksheet->write($row, $col, $mean_bandwidth);
                    $col++;
                }
                printf "$data_set[$count_dir]\n";
                $count_dir = $count_dir + 1;
                $row++;
	    }
        }
    }
    
    $tail1 = "+------------------+";
    foreach($i=1; $i<=$burst_limit; $i=$i*2) {
        $tail1 .= "--------+";
    }
    printf "$tail1\n";
}

#get_report("read");
get_report("write");


