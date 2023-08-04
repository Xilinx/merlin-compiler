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

$user_name = `whoami`;
#printf "$host_name\n";
chomp($user_name);
$cmd = "ps -u $user_name | grep \'mars_\'";
$ps_result = `$cmd`;
printf "$cmd\n";
printf "$ps_result\n";

printf "Killing all the mars_* task...\n";
@ps_set = split(/\n/,$ps_result);
foreach $one_line (@ps_set) {
    if($one_line =~ /^\s*(\S*)/) {
	system "kill -9 $1";
    }
}
