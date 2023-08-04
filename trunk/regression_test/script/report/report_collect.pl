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
use Spreadsheet::WriteExcel;

# Create a new Excel workbook
my $workbook = Spreadsheet::WriteExcel->new('result_collect.xls');

# Add a worksheet
$worksheet = $workbook->add_worksheet();

#  Add and define a format
$format = $workbook->add_format(); # Add a format
$format->set_bold();
$format->set_color('red');
$format->set_align('center');

# Write a formatted and unformatted string, row and column notation.
#$col = $row = 0;
#$worksheet->write($row, $col, 'Hi Excel!', $format);
#$worksheet->write(1,    $col, 'Hi Excel!');
#
## Write a number and a formula using A1 notation
#$worksheet->write('A3', 1.2345);
#$worksheet->write('A4', '=SIN(PI()/4)');
#
#
#$count = 10;
@line_index = <A B C D E F G H I J K L M N O P Q R S T U V W X Y Z>;
#printf "$a[13]\n";
#for($i=0; $i<3; $i++) {
#    for($j=0; $j<$count; $j++) {
#        printf "$index[$j]$i ";
#    }
#}
#printf "\n";
foreach($i=0;$i<@ARGV;$i++) {
    if($ARGV[$i] eq "one") {
	$mode = "one";
    }
    if($ARGV[$i] eq "bit") {
	$flow = "bit";
    }
    if($ARGV[$i] eq "hls") {
	$flow = "hls";
    }
}

system "rm -rf report_collect";
system "mkdir report_collect";

$dir_list = `ls`;
@dir_set = split(/\n/, $dir_list);
foreach $one_dir (@dir_set) {
#    printf "$one_dir\n";
    if ( (-d "$one_dir") && ($one_dir ne "report")  && ($one_dir ne "report_collect")) {
        $one_report .= "\n";
        $one_report .= "\n";
        $one_report .= "$one_dir\n";
        $one_dir_simple = substr($one_dir,4);
	if($flow eq "hls") {
            system "cd $one_dir/run; perl report_hls.pl>report.log";
	} else {
            system "cd $one_dir/run; perl report_bit.pl>report.log";
	}
	
        system "cd $one_dir/run; cp report.log ../../report_collect/$one_dir_simple.log";
        $report_config = `cat $one_dir/run/report.log`;
        $one_report .= $report_config;
    }
}

# printf original report
open(my $fh, '>', 'report_all.log');
print $fh "$one_report";
close $fh;

# this script generate a report for regression test
# This scripts is called at regression_mars directory

if($flow eq "hls") {
    $cycles_compare 		= "on";
    $bram_compare 		= "off";
    $dsp_compare 		= "off";
    $ff_compare 		= "off";
    $lut_compare 		= "off";
    $slack_compare 		= "off";
    $fpga_compute_time_compare  = "off";
    $pcie_transfer_time_compare = "off";
    $fpga_total_time_compare    = "off";
    $cpu_total_time_compare     = "off";
    $speedup_compare 		= "off";
    $pass_or_fail 		= "off";
} else {
    $cycles_compare 		= "off";
    $bram_compare 		= "off";
    $dsp_compare 		= "off";
    $ff_compare 		= "off";
    $lut_compare 		= "off";
    $slack_compare 		= "on";
    $fpga_compute_time_compare 	= "on";
    $pcie_transfer_time_compare = "off";
    $fpga_total_time_compare    = "off";
    $cpu_total_time_compare     = "off";
    $speedup_compare 		= "off";
    $pass_or_fail 		= "on";
}

#printf "$report_dir\n";
$dir_list = `ls report_collect`;
printf "report_list:\n";
printf "$dir_list\n";
@dir_set=split(/\n/,$dir_list);

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

sub print_head {
    $head0 = "+-------------+";
    $head1 = "|    DESIGN   |";
    $head2 = "+-------------+";
    $worksheet->write(0, 0, 'DESIGN');
    $col = 1;
    $row = 0;
    foreach $one_config (@dir_set) {
        $one_config_name = substr($one_config,0,-4);
        $worksheet->write($row, $col, $one_config_name);
    	$one_config_name = str_shift_to_right($one_config_name,11,11);
        $head0 .= "-----------+";
        $head1 .= "$one_config_name|";
        $head2 .= "-----------+";
	    if($one_config_name =~ /.*full.*/) {
	        $count_full = $col;
	    }
#        printf "$one_config_name\n";
        $col = $col + 1;
    }
    printf "$head0\n";
    printf "$head1\n";
    printf "$head2\n";
}

sub print_head_hls {
    $head0 = "+-------------+-------------+";
    $head1 = "|    DESIGN   |    KERNEL   |";
    $head2 = "+-------------+-------------+";
    $worksheet->write(0, 0, 'DESIGN');
    $worksheet->write(0, 1, 'DESIGN');
    $col = 2;
    $row = 0;
    foreach $one_config (@dir_set) {
        $one_config_name = substr($one_config,0,-4);
        $worksheet->write($row, $col, $one_config_name);
    	$one_config_name = str_shift_to_right($one_config_name,11,11);
        $head0 .= "-----------+";
        $head1 .= "$one_config_name|";
        $head2 .= "-----------+";
	    if($one_config_name =~ /.*full.*/) {
	        $count_full = $col;
	    }
#        printf "$one_config_name\n";
        $col = $col + 1;
    }
    printf "$head0\n";
    printf "$head1\n";
    printf "$head2\n";
}

sub print_data1 {
    $data_choice = $_[0];
    #get case data
    $count = 1;
    $count_config = 0;
    $col = 1;
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $count = 1;
        $row = 1;
        $row_name = 1;
        if($report_info =~ /All Part .*\n.*\n.*\n.*\n.*\n(.*)\n([\S\s]*)\n\+\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\+\-\-\-\-\-\-\+/) {
            @case_set=split(/\n/,$2);
            foreach $one_case (@case_set) {
            	if($one_case =~ /\|(.*)\|.*\|.*\|(.*)\|.*\|.*\|(.*)\|.*\|.*\|(.*)\|.*\|.*\|(.*)\|(.*)\|/) {
                    if($count_config == 0) {  
    		            $name_array[$count] = $1;
    		            $name_array_simple[$count] = substr($name_array[$count],8);
                        $worksheet->write($row_name, 0, $name_array_simple[$count]);
    		            $name_array_simple[$count] = str_shift_to_right($name_array_simple[$count],13);
    		            $result_data[$count] = "$name_array_simple[$count]";
#    		            printf "$result_data[$count]\n";
                        $row_name = $row_name + 1;
                    }  
    		        if ($data_choice eq "bram") {
                        $data_get = str_shift_to_right($2,10);
                    } elsif ($data_choice eq "dsp") {
                        $data_get = str_shift_to_right($3,10);
                    } elsif ($data_choice eq "ff") {
                        $data_get = str_shift_to_right($4,10);
                    } elsif ($data_choice eq "lut") {
                        $data_get = str_shift_to_right($5,10);
                    } elsif ($data_choice eq "slack") {
                        $data_get = str_shift_to_right($6,10);
                    }
                    $data_align = str_shift_to_right($data_get,10);
                    $worksheet->write($row, $col, $data_align);
    		        $result_data[$count] .= "|$data_align";
    		        $count = $count + 1;
    		    } 
                $row = $row + 1;
    	    }
        }
        $col = $col + 1;
        $count_config = $count_config + 1;
    }
    $case_number = $count-1;
    for($i=1; $i<=$case_number; $i=$i+1) {
        printf "|$result_data[$i]|\n";
    }
}

sub print_data2 {
    $data_choice = $_[0];
    #get case data
    $count = 1;
    $count_config = 0;
    $col = 1;
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $row = 1;
        $row_name = 1;
        $count = 1;
        if($report_info =~ /\|.*DESIGN.*\| C.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
            @case_set=split(/\n/,$2);
            foreach $one_case (@case_set) {
            	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|/) {
                    if($count_config == 0) { 
    		        $name_array[$count] = $1;
    		        #$name_array_simple[$count] = substr($name_array[$count],8);
    		        $name_array_simple[$count] = $name_array[$count];
                        $worksheet->write($row_name, 0, $name_array_simple[$count]);
    		        $name_array_simple[$count] = str_shift_to_right($name_array_simple[$count],13,13);
    		        $result_data[$count] = "$name_array_simple[$count]";
#    		        printf "$result_data[$count]\n";
                        $row_name = $row_name + 1;
                    }  
    		        if ($data_choice eq "fpga_compute_time") {
                        $data_get = str_shift_to_right($5,11,11);
                    } elsif ($data_choice eq "pcie_transfer_time") {
                        $data_get = str_shift_to_right($6,11,11);
                    } elsif ($data_choice eq "fpga_total_time") {
                        $data_get = str_shift_to_right($7,11,11);
                    } elsif ($data_choice eq "cpu_total_time") {
                        $data_get = str_shift_to_right($8,11,11);
                    } elsif ($data_choice eq "speedup") {
                        $data_get = str_shift_to_right($9,11,11);
                    } elsif ($data_choice eq "pass_or_fail") {
                        $data_get = str_shift_to_right($4,11,11);
                    } elsif ($data_choice eq "slack") {
                        $data_get = str_shift_to_right($11,11,11);
                    }
                    $data_align = str_shift_to_right($data_get,11,11);
                    $worksheet->write($row, $col, $data_align);
    		        $result_data[$count] .= "|$data_align";
    		        $count = $count + 1;
		            if($col == $count_full) {
			        $data_full = $data_align;
#		            	printf "$col $data_full\n";
		            }
    		    }
                $row = $row + 1;
    	    }
        }
        $col = $col + 1;
        $count_config = $count_config + 1;
    }
    $case_number = $count-1;
    for($i=1; $i<=$case_number; $i=$i+1) {
        printf "|$result_data[$i]|\n";
    }
}

sub print_data_compare {
    $data_choice = $_[0];
    #get case data
    $count = 1;
    $count_config = 0;
    $col = 1;
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $row = 1;
        $row_name = 1;
        $count = 1;
        if($report_info =~ /\|.*DESIGN.*\| C.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
            @case_set=split(/\n/,$2);
            foreach $one_case (@case_set) {
            	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|\s*(.*)\|/) {
                    if($count_config == 0) { 
    		        $name_array[$count] = $1;
    		        #$name_array_simple[$count] = substr($name_array[$count],8);
    		        $name_array_simple[$count] = $name_array[$count];
                        $worksheet->write($row_name, 0, $name_array_simple[$count]);
    		        $name_array_simple[$count] = str_shift_to_right($name_array_simple[$count],13,13);
    		        $result_data[$count] = "$name_array_simple[$count]";
#    		        printf "$result_data[$count]\n";
                        $row_name = $row_name + 1;
                    } 

    		    if ($data_choice eq "fpga_compute_time") {
                        $data_get = str_shift_to_right($5,11,11);
                    } elsif ($data_choice eq "pcie_transfer_time") {
                        $data_get = str_shift_to_right($6,11,11);
                    } elsif ($data_choice eq "fpga_total_time") {
                        $data_get = str_shift_to_right($7,11,11);
                    } elsif ($data_choice eq "cpu_total_time") {
                        $data_get = str_shift_to_right($8,11,11);
                    } elsif ($data_choice eq "speedup") {
                        $data_get = str_shift_to_right($9,11,11);
                    }
                    $data_align = str_shift_to_right($data_get,11,11);
		    
		    if($data_align =~ /\s*(\d+.\d+)/) {
		        $data_1 = $1;
		    } else {
		        $data_1 = "NG";
		    }	
		    if($data_full =~ /\s*(\d+.\d+)/) {
		        $data_2 = $1;
		    } else {
		        $data_2 = "NG";
		    }
		    if($data_1 eq "NG" or $data_2 eq "NG"){
                        $data_compare = str_shift_to_right("NG",11,11);
		    } else {
		        if(($data_1 eq "0.0") or ($data_1 eq "") or ($data_1 eq "0.00") or ($data_1 eq "0.000000") ) {
		            $data_1 = 0.000001;
		        }	
		        if(($data_2 eq "0.0") or ($data_2 eq "") or ($data_2 eq "0.00") or ($data_2 eq "0.000000") ) {
		            $data_2 = 0.000001;
		        }
		        $data_compare = ($data_1 - $data_2) / $data_2 * 100;
            	        $data_compare = sprintf("%.2f",$data_compare);
            	        if($data_compare == "0.00") {
            	            $tmp_flag = " ";
            	        } elsif ($data_compare =~ /^\-/) {
            	            $tmp_flag = "-";
            	        } else {
            	            $tmp_flag = "+";
            	        }
            	        $data_compare = str_shift_to_right(abs($data_compare),7);
            	        $data_compare = " $tmp_flag $data_compare%";
                        $data_compare = str_shift_to_right($data_compare,11,11);
		    }

                    $worksheet->write($row, $col, $data_compare);
    		        $result_data[$count] .= "|$data_compare";
    		        $count = $count + 1;
    		    }
                $row = $row + 1;
    	    }
        }
        $col = $col + 1;
        $count_config = $count_config + 1;
    }
    $case_number = $count-1;
    for($i=1; $i<=$case_number; $i=$i+1) {
        printf "|$result_data[$i]|\n";
    }
}

sub case_kernel_number {
    #get case data
    my $max_kernel = 0;
    my @config_index = 0;
    $count_case_name = 0;
    $count_kernel_name = 0;
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $row = 1;
        $count = 1;
        $count_case_name = 0;
        if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
            @case_set=split(/\n/,$2);
            foreach $one_case (@case_set) {
            	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|/) {
                    $case_name = $1;
                    $kernel_name = $2;
                    if($case_name =~ /\s*\S+/) {
                        $count_case_name++;
                        $count_kernel_name = 1;
                        $max_kernel = $count_kernel_name; 
                    } else {
                        $count_kernel_name++;
                        if($max_kernel < $count_kernel_name) {
                            $max_kernel = $count_kernel_name; 
                        }
                    }
                }
            }
        }
    }
    return $max_kernel;
}

sub is_full_index {
    $kernel_number = $_[0];
    my $is_full_flag = "off";
    $count_config = 0;
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $count_kernel = 0;
        if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
            @case_set=split(/\n/,$2);
            foreach $one_case (@case_set) {
            	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|/) {
                    $count_kernel++;
                }
            }
        }
        if($count_kernel = $hls_kernel_number) {
            $is_full_flag = "on";
            last;
        }
        $count_config ++;
    }
    return $count_config;
}

sub is_full {
    $report_info = $_[0];
    $kernel_number = $_[1];
    $is_full_flag = "on";
    $count_kernel = 0;
    if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
        @case_set=split(/\n/,$2);
        foreach $one_case (@case_set) {
        	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|/) {
                $count_kernel++;
            }
        }
    }
    if($count_kernel < $kernel_number) {
        $is_full_flag = "off";
    }
    return $is_full_flag;
}

sub get_name {
    $count_name = $_[0];
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $row_name = 1;
        $count = 1;
        if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
            @case_set=split(/\n/,$2);
            foreach $one_case (@case_set) {
            	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|/) {
                    if($count_config == $count_name) { 
        	            $name_array[$count] = $1;
        	            $kernel_array[$count] = $2;
        	            $name_array_simple[$count] = $name_array[$count];
        	            $kernel_array_simple[$count] = $kernel_array[$count];
                        $worksheet->write($row_name, 0, $name_array_simple[$count]);
                        $worksheet->write($row_name, 1, $kernel_array_simple[$count]);
        	            $name_array_simple[$count] = str_shift_to_right($name_array_simple[$count],13,13);
        	            $kernel_array_simple[$count] = str_shift_to_right($kernel_array_simple[$count],13,13);
        	            $result_data[$count] = "$name_array_simple[$count]|$kernel_array_simple[$count]";
       	                #printf "$result_data[$count]\n";
                        $row_name = $row_name + 1;
                    }
        		    $count = $count + 1;
                }
            }
            $count_config = $count_config + 1;
        }
    }
}

$kernel_number = 0;
sub print_data_cycles {
    $data_choice = $_[0];
    #get case data
    my @data_full;
    $count = 1;
    $col = 2;
    $index_full = is_full_index($hls_kernel_number);
#    printf "$index_full\n";
    get_name($index_full);
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $row = 1;
        $count = 1;
        $full_flag = is_full($report_info,$hls_kernel_number);
        $kernel_number = 0;
        if($full_flag eq "on") {
            if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
                @case_set=split(/\n/,$2);
                foreach $one_case (@case_set) {
                	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|/) {
        		        if ($data_choice eq "cycles_compare") {
                            $data_get = str_shift_to_right($7,11,11);
                        }
                        $data_align = str_shift_to_right($data_get,11,11);
                        $worksheet->write($row, $col, $data_align);
        		        $result_data[$count] .= "|$data_align";
    		            if($col == $count_full) {
    			            $data_full[$kernel_number] = $data_align;
#                            printf "$data_full[$kernel_number]\n";
    		            }
        		        $count = $count + 1;
        		    }
                    $kernel_number = $kernel_number + 1;
                    $row = $row + 1;
        	    }
            }
            $col = $col + 1;
        } else {
            if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
                @case_set=split(/\n/,$2);
                foreach $one_case (@case_set) {
                	for($NA_number=0; $NA_number < $hls_kernel_number; $NA_number++) {
                        $data_align = str_shift_to_right("NG",11,11);
                        $worksheet->write($row, $col, $data_align);
        		        $result_data[$count] .= "|$data_align";
        		        $count = $count + 1;
        		    }
                    $row = $row + 1;
        	    }
            }
            $col = $col + 1;
        }
    }
    $kernel_number = $count-1;
    for($i=1; $i<=$kernel_number; $i=$i+1) {
        printf "|$result_data[$i]|\n";
    }
    return @data_full;
}

sub print_cycles_compare {
#    $data_choice = $_[0];
    #get case data
    $count = 1;
    $col = 2;
    $kernel_number = 0;
    @data_full = @data_ref;
    $index_full = is_full_index($hls_kernel_number);
    get_name($index_full);
    foreach $one_config (@dir_set) {
        $report_file = "report_collect/$one_config" ;
        $report_info = `cat $report_file`;
        $row = 1;
        $row_name = 1;
        $count = 1;
        $full_flag = is_full($report_info,$hls_kernel_number);
        $kernel_number = 0;
        if($full_flag eq "on") {
            if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
                @case_set=split(/\n/,$2);
                foreach $one_case (@case_set) {
                	if($one_case =~ /\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|\s*(.*)\|/) {
                        $data_cycles = $7;
                        if($data_cycles eq "" or $data_cycles eq "NG") {
                            $data_compare = str_shift_to_right("NG",11,11);
                        } elsif($data_cycles eq "RUNNING") {
                            $data_compare = str_shift_to_right("RUNNING",11,11);
                        } elsif($data_full[$kernel_number] !~ /\s*\d+.+\d+/) {
                            $data_compare = str_shift_to_right("NG",11,11);
                        } elsif($data_full[$kernel_number] =~ /\s*RUNNING/) {
                            $data_compare = str_shift_to_right("RUNNING",11,11);
                        } else {
    	    	            if ($data_choice eq "cycles_compare") {
                                $data_get = str_shift_to_right($data_cycles,11,11);
                            }
                            $data_align = str_shift_to_right($data_get,11,11);
		      
		                    if($data_align =~ /\s*(.*)/) {
		    	                $data_1 = $1;
		                    }	
		                    if($data_full[$kernel_number] =~ /\s*(.*)/) {
		    	                $data_2 = $1;
		                    }
		                    if(($data_1 eq "0") or ($data_1 eq "")) {
		                        $data_1 = 1;
		                    }	
		                    if(($data_2 eq "0") or ($data_2 eq "")) {
		                        $data_2 = 1;
		                    }
		                    $data_compare = ($data_1 - $data_2) / $data_2 * 100;
                	        $data_compare = sprintf("%.2f",$data_compare);
                	        if($data_compare == "0.00") {
                	            $tmp_flag = " ";
                	        } elsif ($data_compare =~ /^\-/) {
                	            $tmp_flag = "-";
                	        } else {
                	            $tmp_flag = "+";
                	        }
                	        $data_compare = str_shift_to_right(abs($data_compare),7);
                	        $data_compare = " $tmp_flag $data_compare%";
                            $data_compare = str_shift_to_right($data_compare,11,11);
                        }
                        $worksheet->write($row, $col, $data_compare);
    	    	        $result_data[$count] .= "|$data_compare";
    	    	        $count = $count + 1;
                        $kernel_number = $kernel_number + 1;
    	    	    }
                    $row = $row + 1;
    	        }
                $col = $col + 1;
            }
        } else {
            if($report_info =~ /\|.*DESIGN.*\|.*\n(.*)\n([\S\s]*)\n\+\-(.*)\n/) {
                @case_set=split(/\n/,$2);
                foreach $one_case (@case_set) {
                	for($NA_number=0; $NA_number < $hls_kernel_number; $NA_number++) {
                        $data_align = str_shift_to_right("NG",11,11);
                        $worksheet->write($row, $col, $data_align);
        		        $result_data[$count] .= "|$data_align";
        		        $count = $count + 1;
        		    }
                    $row = $row + 1;
        	    }
            }
            $col = $col + 1;
        }
    }
    $case_number = $count-1;
    for($i=1; $i<=$case_number; $i=$i+1) {
        printf "|$result_data[$i]|\n";
    }
}

sub print_end {
    $end = "+-------------+";
    for($i=0; $i<$count_config; $i=$i+1) {
        $end .= "-----------+";
    }
    printf "$end\n";
}

sub print_end_hls {
    $end = "+-------------+-------------+";
    for($i=0; $i<$count_config; $i=$i+1) {
        $end .= "-----------+";
    }
    printf "$end\n";
}

#caculate case count
my @name_array;
$i = 0;
$count = 1;
foreach $one_config (@dir_set) {
	$count = $count + 1;
}
$count_config = $count - 1;
#printf "count_config = $count_config\n";

# bram compare
if($bram_compare eq "on") {
    printf "bram_compare\n";
    $worksheet = $workbook->add_worksheet("bram");
    print_head;
    print_data1("bram");
    print_end;
}

# dsp compare
if($dsp_compare eq "on") {
    printf "dsp_compare\n";
    $worksheet = $workbook->add_worksheet("dsp");
    print_head;
    print_data1("dsp");
    print_end;
}

# ff compare
if($ff_compare eq "on") {
    printf "ff_compare\n";
    $worksheet = $workbook->add_worksheet("ff");
    print_head;
    print_data1("ff");
    print_end;
}

# lut compare
if($lut_compare eq "on") {
    printf "lut_compare\n";
    $worksheet = $workbook->add_worksheet("lut");
    print_head;
    print_data1("lut");
    print_end;
}

# slack compare
if($slack_compare eq "on") {
    printf "slack_compare\n";
    $worksheet = $workbook->add_worksheet("slack");
    print_head;
#    print_data1("slack");
    print_data2("slack");
    print_end;
}

# fpga compute time compare
if($fpga_compute_time_compare eq "on") {
    printf "fpga compute time compare\n";
    $worksheet = $workbook->add_worksheet("fpga_compute_time");
    print_head;
    print_data2("fpga_compute_time");
    if($mode eq "one") {
        print_data_compare("fpga_compute_time");
    }
    print_end;
}

# pcie transfer time compare
if($pcie_transfer_time_compare eq "on") {
    printf "pcie transfer time compare\n";
    $worksheet = $workbook->add_worksheet("pcie_transfer_time");
    print_head;
    print_data2("pcie_transfer_time");
    if($mode eq "one") {
        print_data_compare("pcie_transfer_time");
    }
    print_end;
}

# fpga total time compare
if($fpga_total_time_compare eq "on") {
    printf "fpga total time compare\n";
    $worksheet = $workbook->add_worksheet("fpga_total_time");
    print_head;
    print_data2("fpga_total_time");
    if($mode eq "one") {
        print_data_compare("fpga_total_time");
    }
    print_end;
}

# fpga total time compare
if($cpu_total_time_compare eq "on") {
    printf "cpu total time compare\n";
    $worksheet = $workbook->add_worksheet("cpu_total_time");
    print_head;
    print_data2("cpu_total_time");
    if($mode eq "one") {
        print_data_compare("cpu_total_time");
    }
    print_end;
}

# speed up compare
if($speedup_compare eq "on") {
    printf "speed up compare\n";
    $worksheet = $workbook->add_worksheet("speedup");
    print_head;
    print_data2("speedup");
    if($mode eq "one") {
        print_data_compare("speedup");
    }
    print_end;
}

# fpga result compare
if($pass_or_fail eq "on") {
    printf "fpga result compare\n";
    $worksheet = $workbook->add_worksheet("pass_or_fail");
    print_head;
    print_data2("pass_or_fail");
    print_end;
}

# hls cycles compare
if($cycles_compare eq "on") {
    printf "hls cycles compare\n";
    $worksheet = $workbook->add_worksheet("cycles_compare");
    $hls_kernel_number = case_kernel_number;
    print_head_hls;
    @data_ref = print_data_cycles("cycles_compare");
    if($mode eq "one") {
        print_cycles_compare("cycles_compare, \@data_ref");
    }
    print_end_hls;
}

if($mode eq "one") {
    printf "Note : all the compare data are compared with 'full'\n";
}
