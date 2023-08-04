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

use Spreadsheet::WriteExcel;

$input_file = $ARGV[0];
# Create a new Excel workbook
$output_file_name = "result";
if($input_file =~ /(.*)\.log/) {
    $output_file_name = $1;
} elsif($input_file =~ /(.*)\.rpt/) {
    $output_file_name = $1;
}
$output_file_name .= ".xls";
system "rm -rf $output_file_name";
my $workbook = Spreadsheet::WriteExcel->new($output_file_name);
# Add a worksheet
$worksheet = $workbook->add_worksheet();
# Add and define a format
$format = $workbook->add_format(); # Add a format
$format->set_bold();
$format->set_color('red');
$format->set_align('center');

$row = 0;
$col = 0;
$report_info = `cat $input_file`;
@line_set = split(/\n/, $report_info);
foreach $one_line (@line_set) {
    if($one_line =~ /.*report : (.*)/) {
        $row = 0;
        $worksheet = $workbook->add_worksheet($1);
    } elsif($one_line =~ /--------/) {
    } elsif($one_line =~ /\|/) {
        $row++;
        $col = 0;
        @unit_set = split(/\|/, $one_line);
        foreach $one_unit (@unit_set) {
            $one_unit =~ s/ //g;
            $col++;
            $worksheet->write($row, $col, $one_unit);
        }
    }
}

