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




############################
# Description: this script waits for the fetching the xilinx license or a timeout
# Owners:
#   Peng Zhang (pengzhang@falcon-computing.com) Sep 12, 2017
############################
#

my $arg_num = @ARGV;
if ($arg_num != 1) {
    print "Usage: \n";
    print "   mars_perl test_xilinx_license.pl platform_name\n";
    exit;
}
my $platform = $ARGV[0];
if ($ARGV[0] eq "default") { 
    $platform = "xilinx:adm-pcie-ku3:2ddr:3.3" ;
}

my $timeout_min = 5;   # 5 minutes
my $interval = 5;      # 5 seconds
my $timeout = $timeout_min * 60 / $interval;

my $sec1= `date +%s`;
chomp($sec1);
for ( 1 .. $timeout)
{
    my $out = `touch __tmp.cpp; xocc -t sw_emu --xdevice $platform -s --kernel a __tmp.cpp 2>&1`;

    my $sec2= `date +%s`;
    chomp($sec2);
    my $sec = $sec2 - $sec1;

    if ($out !~ /LEXnet Licensing checkout error/) {
        print "*** Got Xilinx license ... $sec seconds\n";
        last;
    }

    print "*** Waiting for Xilinx license ... $sec seconds\n";
    system ("sleep $interval");
}


