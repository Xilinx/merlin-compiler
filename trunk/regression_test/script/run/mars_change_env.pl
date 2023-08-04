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

    $org_file = $ARGV[0];
    $mode     = $ARGV[1];
#    printf "$org_file\n";
    $sh_out_file = "tmp.sh";
    open(sh_in,"< $org_file");
    open(sh_out,"> $sh_out_file");
    while(<sh_in>) {
	if($mode eq "0") {
            ~s/2015.3/2015.1.5/g;
	} elsif($mode eq "1") {
            ~s/2015.1.5/2015.3/g;
	}
        print sh_out;
    }
    close(sh_in);
    close(sh_out);
    system "mv $sh_out_file $org_file";
