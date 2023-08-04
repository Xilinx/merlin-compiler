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

#analyze the input argument
$type = $ARGV[0];
$list_or_run = $ARGV[1];
$run_on_cloud = $ARGV[2];
$force = 0;
$env = 0;

for($i=0; $i<20; $i++) {
    if($ARGV[$i] eq "-report") {
        $mode = "report";
    }
    if($ARGV[$i] eq "-force") {
        $force = 1;
    }
    if($ARGV[$i] eq "-nosleep") {
        $nosleep = 1;
    }
    if($ARGV[$i] eq "-env") {
        $env = 1;
	$envfile = $ARGV[$i + 1];
    }
}

#set make argument
if($type eq "unit_test") {
    $make_arg = "unit_test";
} else {
    $make_arg = "test";
}

#set run flow
if($list_or_run eq "") {
    system "cp $MARS_TEST_HOME_GIT/configuration/$type.list .";
} elsif($list_or_run eq "run") {
    if(-e "$MARS_TEST_HOME_GIT/configuration/$type.list") {
        system "cp $MARS_TEST_HOME_GIT/configuration/$type.list .";
    } else {
        printf "ERROR : No list named $type.list in directory $MARS_TEST_HOME_GIT/configuration.\n";
        exit;
    }
    $list_file = "$type.list";
    $mode = "test";
} else {
    $list_file = $list_or_run;
    $mode = "test";
}

for($i=0; $i<20; $i++) {
    if($ARGV[$i] eq "-report") {
        $mode = "report";
    }
}

sub query_exit
{
    if($force eq "0") {
        my $msg         = $_[0] ;
        my $exit_option = $_[1] ;
        my $default     = $_[2] ;

        if ($default eq "yes") { $query = "[YES|no]"; }
        else                   { $query = "[yes|NO]"; }
        print "\n[mars_test] $msg $query ";
        $input = <STDIN>;
        chomp($input);
        $input = lc($input);

        $not_yes = ("yes" ne $input and "y" ne $input);
        $not_no  = ("no"  ne $input and "n" ne $input);
        $is_yes  = ("yes" eq $input or "y" eq $input);
        $is_no   = ("no"  eq $input or "n" eq $input);

        if ($exit_option eq "yes" and $default eq "yes" and $not_no ) { exit; }
        if ($exit_option eq "yes" and $default eq "no"  and $is_yes ) { exit; }
        if ($exit_option eq "no"  and $default eq "yes" and $is_no  ) { exit; }
        if ($exit_option eq "no"  and $default eq "no"  and $not_yes) { exit; }
    }
}

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

#run test
if($mode eq "test") {
    printf "Generating Makefile...\n";
    open(my $makefile, '>', 'Makefile');
    $test_list = `cat $list_file`;
    @test_set=split(/\n/,$test_list);
    $module_name = "";
    foreach $one_directory (@test_set) {
        if($one_directory =~ /(\S+)\s*(\S*)/) {
            #if has #, ignore it
            if($1 =~ /#.*/) {
            } else {
                #query if directory exits
                $source_name = $1;
                if (-e "$1") {
                    query_exit("Directory $1 exists. Overwrite?", "no", "no");
                }
                system "rm -rf $source_name";
		        $dir = `echo $2`;
		        chomp ($dir);
		        printf "$dir\n\n";
                $source_dir = $dir;

                printf "cp -r $source_dir $source_name\n";
                system "cp -r $source_dir $source_name";
                $module_name .= " $source_name";
                printf $makefile "\n";
                printf $makefile "$source_name : \n";
                if($run_on_cloud eq "-cloud") {
                    $exec_command = "cd ${source_name}__002__ touch running__002__ make ${make_arg}__002__ rm -rf running__002__ cd -";
		    if ($env eq 1){
                        printf $makefile "\t-mars_cloud_test -input $source_name -output $source_name -execute $exec_command -timeout 100 -env $envfile \n";
		    } else {
                        printf $makefile "\t-mars_cloud_test -input $source_name -output $source_name -execute $exec_command -timeout 100 \n";
		    }
                } else {
                    printf $makefile "\t-cd $source_name; touch running__002__ cd -\n";
                    printf $makefile "\t-make $make_arg -C $source_name \n";
                    printf $makefile "\t-cd $source_name; rm -rf running__002__ cd - \n";
                }
            }
        }
    }

    printf $makefile "\n";
    printf $makefile "$make_arg : $module_name\n";
    printf $makefile "\n";
    printf $makefile ".PHONY : $make_arg $module_name\n";
    close $makefile;

    if($run_on_cloud eq "-cloud") {
        system "make $make_arg";
        if($nosleep eq 1) {
        } else {
            system "sleep 6000";
        }
    } else {
        system "make $make_arg";
    }
}

#generate report
if( (($mode eq "test") or ($mode eq "report"))  and  ($nosleep ne "1")) {
    printf "Generating Unit_test Report...\n";
    #generate open report file
    $report_file = "$type.log";
    open(my $report, '>', $report_file);
    #write herder
    $one_line  = "+------------------------------+-------------+\n";
    print $one_line;
    printf $report "$one_line";
    $one_line  = "|            DESIGN            | CORRECTNESS |\n";
    print $one_line;
    printf $report "$one_line";
    $one_line  = "+------------------------------+-------------+\n";
    print $one_line;
    printf $report "$one_line";
    #write data
    $test_list = `cat $list_file`;
    @test_set=split(/\n/,$test_list);
    foreach $one_directory (@test_set) {
        if($one_directory =~ /(\S+)\s*(\S*)/) {
            if($1 =~ /#.*/) {
            } else {
                my $case = $1;
                my $out_dir = $case.".condor_output";
                if ( (!-e "$out_dir/err.log") and (!-e "$out_dir/running") ) {
                    $status = "Not Begin";
                } elsif ( (-e "$out_dir/err.log") and -e ("$out_dir/running") ) {
                    if(-z "$out_dir/err.log") {
                        $status = "Running";
                    } else {
                        $status = "Fail";
                    }
                } elsif ( (-e "$out_dir/err.log") and (!-e "$out_dir/running") ) {
                    if (-z "$out_dir/err.log") {
                        $status = "Pass";
                    } else {
                        $status = "Fail";
                    }
                } else {
                    $status = "Error";
                }
                $status = str_shift_to_right($status, 13);
                $name   = str_shift_to_right($case, 30);
                $one_line  = "|$name|$status|\n";
                print $one_line;
                printf $report "$one_line";
            }
        }
    }
    #write tailer
    $one_line  = "+------------------------------+-------------+\n";
    print $one_line;
    printf $report "$one_line";
    $one_line  = "* Check err.log in the $type directory for detail information.\n";
    print $one_line;
    printf $report "$one_line";
    #close report
    close $report;
}
