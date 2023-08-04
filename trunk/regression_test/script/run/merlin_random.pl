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

$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$ESLOPT_HOME = $ENV{'ESLOPT_HOME'};

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

#analyze the input argument 
$flag    = $ARGV[0];
$project = $ARGV[1];
$number  = $ARGV[2];
if($project =~ /(.*)\/$/) {
    $project = $1;
}
if($number !~ /\d+/) {
    $number = 48;
}
#printf "\n\n\n$run_on_cloud\n\n\n";
$run_directory = "random_test";
$force = 0;
for($i=0; $i<20; $i++) {
    if($ARGV[$i] eq "-report") {
        $mode = "report";
    }
    if($ARGV[$i] eq "-force") {
        $force = 1;
    }
}
if($mode ne "report") {
    $mode = "test";
}
#run test
if($mode eq "test") {
    printf "Generating Makefile...\n";
    if(-e $run_directory) {
        if($force eq 1) {
            system "rm -rf $run_directory";
        } else {
            printf "$run_directory exist\n";
            exit;
        }
    }
    system "mkdir $run_directory";
    $make_arg = "test";

    $top_make = "$run_directory/Makefile";
    open(my $makefile, '>', $top_make);
    $module_name = "";
    for($i=0; $i<$number; $i++) {
        $sub_dir = "${project}_$i";
        system "cd $run_directory; rm -rf $sub_dir";
        system "cp -r ${project} $run_directory/$sub_dir";
        $module_name .= " $sub_dir";
        printf $makefile "\n";
        printf $makefile "$sub_dir : \n";
        $exec_command = "cd ${sub_dir}__002__ touch running__002__ make ${make_arg}__002__ rm -rf running__002__ cd -";
        printf $makefile "\t-mars_cloud_test -input ${sub_dir} -output ${sub_dir} -execute $exec_command -timeout 100 \n";
    }

    printf $makefile "\n";
    printf $makefile "$make_arg : $module_name\n";
    printf $makefile "\n";
    printf $makefile ".PHONY : $make_arg $module_name\n";
    close $makefile;
    system "cd $run_directory; make $make_arg";
}

#generate report
if($mode eq "report") {
    printf "Generating Report...\n";
    $string = "";
    #generate open report file
    $report_file = "random.log";
    open(my $report, '>', $report_file);
    #write herder
    $string .= "+------------------------------+-------------+\n";
    $string .= "|            DESIGN            | CORRECTNESS |\n";
    $string .= "+------------------------------+-------------+\n";
    #write data
    $test_list = `cd $run_directory; ls`;
    @test_set=split(/\n/,$test_list);
    foreach $one_case (@test_set) {
        if(-d "$run_directory/$one_case" and $one_case =~/condor_output/) {
            if ( (!-e "$run_directory/$one_case/err.log") and (!-e "$run_directory/$one_case/running") ) {
                $status = "Not Begin";
            } elsif ( (-e "$run_directory/$one_case/err.log") and -e ("$run_directory/$one_case/running") ) {
                if(-z "$run_directory/$one_case/err.log") {
                    $status = "Running";
                } else {
                    $status = "Fail";
                }
            } elsif ( (-e "$run_directory/$one_case/err.log") and (!-e "$run_directory/$one_case/running") ) {
                if (-z "$run_directory/$one_case/err.log") {
                    $status = "Pass";
                } else {
                    $status = "Fail";
                }
            } else {
                $status = "Error";
            }
            $status = str_shift_to_right($status, 13);
            $name   = str_shift_to_right($one_case, 30);
            $string .= "|$name|$status|\n";
        }
    }
    #write tailer
    $string .= "+------------------------------+-------------+\n";
    printf $string;
    printf $report "$string";
    #close report
    close $report;
}
