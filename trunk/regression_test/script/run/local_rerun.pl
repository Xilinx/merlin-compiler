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


my $config_file = $ARGV[0];

print "Rerun Candidate List: $config_file \n" ;

if (not -f "$config_file")  {
    printf "input list file not exist.\n";
    exit; 
}

system "Reading rerun list $config_file ... \n";
#print "pwd=".`pwd`."\n";
open(INFO, $config_file) or die("Could not open file config file.");


system "rm -rf checkin_rerun.log";

$count = 0; 
my $curr_pwd = `pwd`;
chomp($curr_pwd);
foreach $line (<INFO>)  {   
#print "L$count: ".$line;    
    $count ++;

    $one_case = $line; 
    chomp($one_case);
#    $one_case =~ s/^\s+|\s+$//g; # remove leading and trailing spaces

    my $one_case_out = "$one_case.condor_output";

    if (not -d $one_case or not -d "$one_case_out") {
        next; 
    }

    my $err_log_file = "$one_case_out/err.log";
    if (not -f "$err_log_file") { 
        printf "Skip running case ... $one_case\n";
        next; # do not touch unfinished cases
    }  

    my $err_content = `cat $err_log_file`;
#    print "++$err_content++";

    if ($err_content eq "") {
        next ; # the case is correct
    }
    else {
        print "Correcting condor failure ... $one_case\n";
        system "echo 'Correcting condor failure ... $one_case\n' >>  checkin_rerun.log 2>&1";
#        print "cd $one_case; make ; cd - \n";
#        system "which gcc";
#        system "echo \$LD_LIBRARY_PATH";
#        system "export LD_LIBRARY_PATH=''";
#        system "echo LD_LIBRARY_PATH = \$LD_LIBRARY_PATH";

        if (not -e "$one_case/Makefile" and not -e "$one_case/makefile") {
            print "ERROR: Makefile not found ... $one_case\n";
            next;
        }

        system "cd $one_case; export LD_LIBRARY_PATH=''; source ~/fcs_setting64.sh >> $curr_pwd/checkin_rerun.log ; make >> $curr_pwd/checkin_rerun.log 2>&1; cd -";
        system "cp $one_case/err.log $err_log_file";
    }
}
close(INFO);


