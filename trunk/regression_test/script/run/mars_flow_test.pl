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
use File::Basename;
use XML::Simple;
use Data::Dumper;

$MARS_TEST_HOME_GIT = $ENV{'MARS_TEST_HOME_GIT'};
$MARS_TEST_HOME= $ENV{'MARS_TEST_HOME'};
$config_dir = "${MARS_TEST_HOME_GIT}/configuration";
$MERLIN_COMPILER_HOME = $ENV{'MERLIN_COMPILER_HOME'};

sub read_tool{
    $xml_file = $_[0];

    $tool_name = "";
    $config_file = "$xml_file";
    $config_list = XML::Simple->new(ForceArray => 1);
    $config_all = $config_list->XMLin($config_file);
    my @config_set = @{$config_all->{"config"}};
    foreach my $config_one (@config_set) {
       $tool_name = $config_one->{"implementation_tool"}[0];
    }
    return $tool_name;
}

#############################################
#TODO: ZP: 20170917: to be replaced back to MARS_TEST_HOME_GIT after the test cases move to github
#$case_dir = "${MARS_TEST_HOME_GIT}/test_case";
$case_dir = "${MARS_TEST_HOME}/test_case";
#############################################
#
$config_dir = "${MARS_TEST_HOME_GIT}/configuration";
$script_dir = "${MARS_TEST_HOME_GIT}/script";
$regression_dir = "${MARS_TEST_HOME_GIT}/script/regression_mars";
$xml_path = "${MERLIN_COMPILER_HOME}/mars-gen/default/directive_mars_basic.xml";
$xml_path_altera = "${MERLIN_COMPILER_HOME}/mars-gen/default/directive_mars_basic_altera.xml";
$hls_sim_yesterday_log = "hls_sim_yesterday.log";
$hls_sim_release_log   = "hls_sim_release.log";
$hls_sim_best_log      = "hls_sim_best.log";
$bit_yesterday_log = "bit_yesterday.log";
$bit_release_log   = "bit_release.log";
$bit_best_log      = "bit_best.log";

#choose test cases directory
$test_cases_dir = "";
$test_cases_exist = "";
for($i=0; $i<10; $i++) {
    if($ARGV[$i] eq "-src") {
        $test_cases_dir = $ARGV[$i+1];
        $test_cases_exist = "exist";
    }
}
if($test_case_exist ne "exist") {
    $test_cases_dir = "${case_dir}/test_cases_org";
}

$test_cases_dir_org = $test_cases_dir;

#choose test mode
$force = "";
$run_place = "";
$type = "";
$test_mode = "";
$xml_file = "";
for($i=0; $i<10; $i++) {
    if($ARGV[$i] eq "-x") {
        $type = "xml";
        $xml_file = $ARGV[$i+1];
    }
    if($ARGV[$i] eq "-h") {
        $type = "xml";
        $xml_file = $ARGV[$i+1];
        printf "\nxml_file = $xml_file\n";
    }
    if($ARGV[$i] eq "-test_after_bit") {
        $test_mode = "test";
    }
    if($ARGV[$i] eq "-dir") {
        $type = "dir_test";
        $test_dir = $ARGV[$i+1];
    }
    if($ARGV[$i] eq "-bit_and_test") {
        $one_button_mode = "bit_and_test";
    }
    if($ARGV[$i] eq "-cloud") {
        $run_place = "cloud";
    }
    if($ARGV[$i] eq "-force") {
        $force = "force";
    }
    if($ARGV[$i] eq "-altera") {
        $xml_path = $xml_path_altera;
    }
}

$SIG{'INT'} = \&quit_bit_and_test;

sub quit_bit_and_test {
    print "killed the mars bit_and_test!\n";
    exit;
}
#choose run_bitstream mode
sub run_command {
    $run_which  = $_[0];
    $run_where  = $_[1];
    if($run_place eq "cloud") {
        return "make all_${run_which}_cloud";
    } else {
        return "make all_${run_which}";
    }
}

sub check_report{
    $case_report  = "$_[0]";
    if(-e  "${case_dir}/test_cases_${case_type_one}/$case_report") {
        system "cp ${case_dir}/test_cases_${case_type_one}/$case_report report/${case_type_one}_${case_report}";
    } else {
        printf "WARNING : No $case_report in ${case_dir}/test_cases_${case_type_one}\n";
    }
}

#case name configure
sub case_configure {
    $case_in  = "$_[0]";
    $config_number  = "$_[1]";
    $case_out = "case_out.list";
    open(case_in,"< $case_in");
    open(case_out,"> $case_out");
    while(<case_in>) {
        my $one_case = $_;
        if ($one_case !~ /^\s*\#/) {
            print case_out "case_src/$config_number-$one_case";
        }
    }
    close(case_in);
    close(case_out);
}

#case name configure
sub case_copy {
    $config_name = "$_[0]";
    $case_tmp_info = `cat case.list.tmp`;
    @case_tmp_set = split(/\n/,$case_tmp_info);
    foreach $one_case_tmp (@case_tmp_set) {
        if ($one_case_tmp !~ /^\s*\#/) {
            if($one_case_tmp =~ /(\S*)/) {
                system "cp -r $test_cases_dir/$1 run_$config_name/run/case_src/$config_name-$1";
            }
        }
    }
}

sub query_exit
{
    if($force eq "") {
        my $msg         = $_[0] ;
        my $exit_option = $_[1] ;
        my $default     = $_[2] ;

        if ($default eq "yes") { $query = "[YES|no]"; }
        else                   { $query = "[yes|NO]"; }
        print "\n[mars_flow] $msg $query ";
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

sub run_flow_one
{
    $config_one = $_[0] ;
    $xml_file_info = $_[1];
    $name_one      =  $config_one->{"name"}[0];
    $case_type_one =  $config_one->{"case_type"}[0];
    $case_dir_one  =  $config_one->{"case_directory"}[0];
    if ($case_dir_one ne "") {
        if($case_type_one eq "custom") {
            $test_cases_dir = $case_dir_one;
        }
        else {
            printf "ERROR : case_type must be \"custom\" for specified test_directory: configure $name_one\n";
            exit;
        }
    }
    else {
        if($case_type_one eq "") {
            printf "ERROR : Please specify case type for configure $name_one\n";
            exit;
        }
        if (-e "${case_dir}/test_cases_${case_type_one}") {
            if(! -e "report") {
                system "mkdir report";
            }
            $test_cases_dir = "${case_dir}/test_cases_${case_type_one}";
            check_report($hls_sim_yesterday_log);
            check_report($hls_sim_best_log);
            check_report("bit_best_ku3.log");
            check_report("bit_best_bd_sda.log");
        }
        else {
            print  "case_dir = ${case_dir}/test_cases_${case_type_one} \n";
            printf "ERROR : case_type \"${case_type_one}\" can not be supported: configure $name_one\n";
            exit;
        }
    }
    $case_one =  $config_one->{"case"}[0];
    if($case_one eq "") {
        printf "ERROR : Please specify case from case pool for configure name $name_one\n";
        exit;
    }
    $flow_one =  $config_one->{"flow"}[0];
    if($flow_one eq "") {
        printf "ERROR : Please specify run flow for configure name $name_one\n";
        exit;
    }

    if (-e "run_$name_one") {
        query_exit("Directory run_$name_one exists. Overwrite?", "no", "no");
    }
    system "rm -rf run_$name_one";
    system "cp -r $regression_dir run_$name_one";
    system "mkdir run_$name_one/run/case_src";

    system "cp -r directive.xml directive_$name_one.xml";
    $directive_file = "directive_$name_one.xml";
    $directive_list = XML::Simple->new(ForceArray => 1);
    $directive_all = $directive_list->XMLin($directive_file);
    if($xml_file_info =~ /$name_one([\s\S]*?)<\/config>/) {
        $attribute_count = 0;
        @attribute_set = split(/\n/,$1);
        foreach $one_attribute (@attribute_set) {
            if($one_attribute =~ /<\/(.*)>/) {
                if($1 eq "name") {
                } elsif($1 eq "case") {
                    $case_line = $config_one->{"case"}[0];
                    $case_list = $config_one->{"case_list"}[0];
                    if($case_line eq "all") {
                        if (-f "$case_list") {
                            system "cp $case_list case.list.tmp";
                        }
                        elsif (-f "$test_cases_dir/case.list") {
                            if($flow_one eq "bit" && -e "$test_cases_dir/case_onboard.list") {
                                system "cp $test_cases_dir/case_onboard.list case.list.tmp";
                            } else {
                                system "cp $test_cases_dir/case.list case.list.tmp";
                            }
                        }
                        else {
                            system "cp $config_dir/case.list case.list.tmp";
                        }
                    } else {
                        if (-e "case.list.tmp") {
                            system "rm case.list.tmp";
                        }
                        $case_xml_list = "case.list.tmp";
                        open(case_xml_out,"> $case_xml_list");
                        @case_set=split(/\s/,$case_line);
                        foreach $one_case (@case_set) {
                            printf "$one_case\n";
                            if($one_case =~ /(\S*)/) {
                                print case_xml_out "$1\n";
                            }
                        }
                        close(case_xml_out);
                    }
                    $case_tmp = "case.list.tmp";
                    case_configure($case_tmp,$name_one);
                    case_copy($name_one);
                    system "rm case.list.tmp";
                    system "cp case_out.list run_$name_one/run/case.list";
                } elsif($1 eq "case_type") {
                } elsif($1 eq "flow") {
                } elsif($1 eq "case_directory") {
                } elsif($1 eq "case_list") {
                } elsif($1 eq "regression_daily") {
                } elsif($1 eq "regression_weekly") {
                } elsif($1 eq "regression_release") {
                } elsif($directive_all->{"$1"}[0] eq "") {
                    printf "No Attribute named $1!\n"
                } else {
                    $directive_all->{"$1"}[0] = $config_one->{"$1"}[0];
                }
            }
        }
    }
    XMLout($directive_all,
           KeepRoot => 1,
           NoAttr => 1,
           Outputfile => $directive_file,
           );
    $xml_root_out_file = "directive_root_out.xml";
    open(xml_root_in,"< $directive_file");
    open(xml_root_out,"> $xml_root_out_file");
    while(<xml_root_in>) {
        ~s/<opt>\n/<directives>\n/g;
        ~s/<\/opt>\n/<\/directives>\n/g;
        print xml_root_out;
    }
    close(xml_root_in);
    close(xml_root_out);
    system "mv directive_root_out.xml directive_$name_one.xml";
    system "cp directive_$name_one.xml run_$name_one/run/directive.xml";
    $run_command_return = run_command($flow_one,$run_place);
    printf "$run_command_return\n";
    system "cd run_$name_one/run; $run_command_return";
}

if(-e "directive_developer.xml") {
    system "cp directive_developer.xml directive.xml";
} else {
    system "cp $xml_path directive.xml";
}

$xml_tool = read_tool($xml_file);
if($type eq "xml") {
    use File::Basename;
    use XML::Simple;
    use Data::Dumper;

    if($test_mode eq "") {
        my $config_list = XML::Simple->new(ForceArray => 1);
        my $config_all = $config_list->XMLin($xml_file);
        my $xml_file_info = `cat $xml_file`;
        @xml_info = split(/\n/,$xml_file_info);

        my $attribute_count = 0;
        my @config_set = @{$config_all->{"config"}};
        foreach my $config_one (@config_set) {
            $name_one =  $config_one->{"name"}[0];
            if($name_one eq "") {
                printf "ERROR : Please specify config name!\n";
                exit;
            }
            run_flow_one($config_one, $xml_file_info);
        }
    } elsif ($test_mode eq "test") {
        my $config_list = XML::Simple->new(ForceArray => 1);
        my $config_all = $config_list->XMLin($xml_file);
        my @config_set = @{$config_all->{"config"}};
        foreach my $config_one (@config_set) {
            $name_one =  $config_one->{"name"}[0];
            $flow_one =  $config_one->{"flow"}[0];
            if($flow_one eq "bit"){
                if (! -e "run_$name_one/run") {
                    my $xml_file_info = `cat $xml_file`;
                    run_flow_one($config_one, $xml_file_info);
                }
                $run_command_return = run_command("test",$run_place);
                system "cd run_$name_one/run; $run_command_return";
            } else {
                printf "\n\nrun_$name_one have flow $flow_one, not bit. No on board test. Please generate bitstream first.\n\n";
            }
        }
    }
} elsif($type eq "dir_test") {
    $run_command_return = run_command("test",$run_place);
    system "cd $test_dir/run; $run_command_return";
}

if($one_button_mode eq "bit_and_test") {
    printf "Waiting for test on board...\n";
    $count_test = 0;
    while(1) {
        $dir_list = `ls`;
        @dir_set = split(/\n/, $dir_list);
        foreach $one_dir (@dir_set) {
            if(-e "$one_dir/run/work") {
                $project_list = `ls $one_dir/run/work`;
                @project_set = split(/\n/, $project_list);
                foreach $one_project (@project_set) {
                    $test_path = "$one_dir/run/work/$one_project";
                    $file_name1 = "$one_dir/run/work/$one_project/run/pkg/design_1_wrapper.bit";
                    $file_name2 = "$one_dir/run/work/$one_project/run/pkg/kernel_pblock_fcs_kernel_0_partial.bit";
                    $file_name3 = "$one_dir/run/work/$one_project/run/pkg/kernel_top.xclbin";
                    $file_name4 = "$one_dir/run/work/$one_project/run/pkg/kernel_top.aocx";
                    $tested = "$one_dir/run/work/$one_project/run/pkg/tested";
                    if((-e $file_name1) or (-e $file_name2) or (-e $file_name3) or (-e $file_name4)) {
                        printf "$one_project\n";
                        if(! -e "$tested") {
                            system "touch $tested";
                            system "cd $test_path; merlin_flow_cloud run test; cd -";
                            printf "$test_path on board tested!\n";
                        }
                    }
                }
            }
        }
        sleep(180);
        $count_test++;
        if($count_test == 180) {
            last;
        }
    }
}
