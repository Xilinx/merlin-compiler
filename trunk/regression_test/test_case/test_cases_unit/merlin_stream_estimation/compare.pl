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

#|vec_add_kernel (vec_add_kernel.cpp:19)                       |       |  1024300|  1024300|  1.10|  0.90|  4.02|  0.00|kernel           |
sub get_data {
    my $file = $_[0];
    if(not -e $file) {
        system("echo \"failed\" > err.log");
        exit;
    }
    my $data = "";    
    my $info = `cat $file`;
    my @set  = split(/\n/, $info);
    foreach $line (@set) {
        if($line =~ /\|vec_add_kernel.*\|.*\|.*\|\s*(.*)\|.*\|.*\|.*\|.*\|.*\|/) {
            $data = $1;
            printf "$data\n";
        }
    }
    return $data;
}

my $file_ref = "merlin.rpt";
my $file_gol = "golden_merlin.rpt";
my $data_ref = get_data($file_ref);
my $data_gol = get_data($file_gol);
if($data_ref eq $data_gol) {
    printf "pass\n";
} else {
    printf "fail\n";
    system("echo \"failed\" > err.log");
}
exit;
