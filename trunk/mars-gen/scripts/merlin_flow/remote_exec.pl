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


#This file requires password input during remote execution by default
#To run without password, please refere to ssh-kengen
# http://chenlb.iteye.com/blog/211809

my $user_cmd     = "";
my $multiple_cmd = 0;
my $copy_file    = 0;
my $remote_dir = "/tmp/";
my $user_name = `whoami`;
chomp($user_name);

#---------------------------------------------------------------------------------------------#
# parse xml file
#---------------------------------------------------------------------------------------------#
for ($i = 0; $i < @ARGV; $i+=2) {
    if ($ARGV[$i] eq "-server") {
        $remote_host = $ARGV[$i+1];
    }
    elsif ($ARGV[$i] eq "-rdir") {
        $remote_dir = $ARGV[$i+1];
    }
    elsif ($ARGV[$i] eq "-ldir") {
        $local_dir= $ARGV[$i+1];
    }
    elsif ($ARGV[$i] eq "-copy") {
        $copy_file = 1; $i-=1;
    } else {
        if ($user_cmd eq "") {
            $user_cmd = $ARGV[$i]; $i-=1;
        } else {
            $multiple_cmd = 1;
        }
    }
}

#---------------------------------------------------------------------------------------------#
# help information
#---------------------------------------------------------------------------------------------#
if ($user_cmd eq "" or $remote_host eq "" or $multiple_cmd == 1) {
     print "[remote_exec.pl] Argument Error: \n";
     print "Usage : perl \$MARS_BUILD_DIR/scripts/remote_exec.pl 'user cmd' -server host [-rdir remote_dir] [-rdir remote_dir] [-copy] \n";
     print "        remote_dir: default /tmp \n";
     print "        local_dir: default ./ \n";
     print "        -copy: copy file to and from server host\n";
  	 exit;
}

#---------------------------------------------------------------------------------------------#
# copy file in 
#---------------------------------------------------------------------------------------------#
if ($copy_file) {
    # upload file
    system "ssh $remote_host 'cd $remote_dir; rm -rf remote_exec_tmp_${user_name} '" ;
    $remote_dir = $remote_dir."/remote_exec_tmp_${user_name}";
    system "scp -r $local_dir $remote_host:$remote_dir "; 

}

#---------------------------------------------------------------------------------------------#
# execute on remoce server
#---------------------------------------------------------------------------------------------#
system "ssh $remote_host 'cd $remote_dir; $user_cmd '" ;

#---------------------------------------------------------------------------------------------#
# copy file out
#---------------------------------------------------------------------------------------------#
if ($copy_file) {
    # download
    system "ssh $remote_host 'cd $remote_dir; touch dummy.dat; tar czvf remote_exec_data.tgz *.mon *.data *.dat *.o *.rpt *.log *.csv *.html *.output *.csh *.sh fpga_run' >& /dev/null" ;
    system "scp -r $remote_host:$remote_dir/remote_exec_data.tgz $local_dir >& /dev/null"; 
    system "cd $local_dir; tar xzvf remote_exec_data.tgz >& /dev/null"; 
}

