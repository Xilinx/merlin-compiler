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


#!/usr/bin/perl
my $file_in = $ARGV[0];
my $file_in_tmp = ${file_in}."_tmp";
open(my $read_file, "<", $file_in) or die "Cannot open file $file_in";
open(my $write_file, ">", $file_in_tmp) or die "Cannot open file $file_in_tmp";
while (my $line = <$read_file>) {
  my $len = length $line;
  my $cur_pos = 0;
  while ($cur_pos + 1000 < $len) {
    my $loc = index $line, ' ', ($cur_pos + 1000);
    if ($loc == -1) {
      last;
    }
    my $new_line = substr $line, $cur_pos, $loc - $cur_pos + 1;
    print $write_file $new_line."\\\n";
    $cur_pos = $loc + 1;
  }
  my $new_line = substr $line, $cur_pos;
  print $write_file $new_line;
}
close $write_file;
close $read_file;
system "mv  $file_in_tmp  $file_in";
