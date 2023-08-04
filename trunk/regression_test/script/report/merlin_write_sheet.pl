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

$report_frontend = $ARGV[0];
$report_backend = $ARGV[1];
printf "$report_frontend\n";
printf "$report_backend\n";

system "rm -rf result.xls";
# Create a new Excel workbook
my $workbook = Spreadsheet::WriteExcel->new('result.xls');

my $all_info = `cat $report_frontend`;
my $data_flag = 0;
my $count_design = 0;
my $format;
@all_lines_set = split("\n", $all_info);
foreach $one_line (@all_lines_set) {
    if($one_line =~ /One hour test report : hls_(.*)/) {
        $data_flag = 1;
        $sheet_name = $1;
        $worksheet = $workbook->add_worksheet("$sheet_name");
        $format = $workbook->add_format();
        $format->set_bold();
        $format->set_align('center');
        $format->set_color('black');
        $worksheet->write(0, 0, 'DESIGN',   $format);
        $worksheet->write(0, 1, 'BRAM',     $format);
        $worksheet->write(0, 2, 'DSP',      $format);
        $worksheet->write(0, 3, 'FF',       $format);
        $worksheet->write(0, 4, 'LUT',      $format);
        $worksheet->write(0, 5, 'HLS CYCLE',$format);
        $worksheet->write(0, 6, 'SIM RES',  $format);
        $worksheet->write(0, 7, 'KERNEL TIME',$format);
        $worksheet->write(0, 8, 'BIT RES',  $format);
        $count_design = 0;
    }
    if($data_flag == 1 and $one_line =~ /\|(\S+)\s*\|.*\|.*\((.*)\)\s*\|.*\((.*)\)\s*\|.*\((.*)\)\s*\|.*\((.*)\)\s*\|\s*(\S*)\|\s*(\S*)\|/) {
        $name    = $1;
        $bram    = $2;
        $dsp     = $3;
        $ff      = $4;
        $lut     = $5;
        $cycles  = $6;
        $correct = $7;
        if($bram =~ /(.*)%/) { $bram = $1; }
        if($dsp  =~ /(.*)%/) { $dsp  = $1; }
        if($ff   =~ /(.*)%/) { $ff   = $1; }
        if($lut  =~ /(.*)%/) { $lut  = $1; }
#        $format->set_color('black');
#        if($correct =~ "Fail" or $correct =~ "NG" or $correct =~ "RUNNING") {
#            $format->set_color('red');
#        }
        #printf "$name $bram $dsp $ff $lut $cycles $correct\n";
        $count_design = $count_design + 1;
        $row = $count_design;
        $col = 0;           $worksheet->write($row, $col, $name,    $format);
        $col = $col + 1;    $worksheet->write($row, $col, $bram,    $format);
        $col = $col + 1;    $worksheet->write($row, $col, $dsp,     $format);
        $col = $col + 1;    $worksheet->write($row, $col, $ff,      $format);
        $col = $col + 1;    $worksheet->write($row, $col, $lut,     $format);
        $col = $col + 1;    $worksheet->write($row, $col, $cycles,  $format);
        $col = $col + 1;    $worksheet->write($row, $col, $correct, $format);
        
        if($report_backend eq "") {
            exit;
        } elsif(!-e $report_backend) {
            exit;
        } else {
            $match_dir_flag = 0;
            my $bit_info = `cat $report_backend`;
            @bit_lines_set = split("\n", $bit_info);
            foreach $one_bit_line (@bit_lines_set) {
                if($one_bit_line =~ /On board test performance: run_$sheet_name/) {
                    #print "find matched directory name $sheet_name\n";
                    $match_dir_flag = 1;
                } elsif ($one_bit_line =~ /On board test performance:/){
                    $match_dir_flag = 0;
                }
                if($match_dir_flag == 1 and $one_bit_line =~ /\|(\S*)\s*\|.*\|.*\|(.*)\|(.*)\|.*\|.*\|.*\|.*\|.*\|.*\|/) {
                    if($1 eq $name) {
                        #printf "$1, $2, $3\n";
                        $col = $col + 1;    $worksheet->write($row, $col, $3, $format);
                        $col = $col + 1;    $worksheet->write($row, $col, $2, $format);
                    }
                }
            }
        }
    }
    if($one_line =~ /Note:/) {
        $count_design = 0;
        $data_flag = 0;
    }
}

