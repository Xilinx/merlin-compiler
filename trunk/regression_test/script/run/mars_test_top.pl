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
$config_dir         = "${MARS_TEST_HOME_GIT}/configuration";

sub read_tool {
    $xml_file = $_[0];

    $tool_name   = "";
    $config_file = "$xml_file";
    $config_list = XML::Simple->new( ForceArray => 1 );
    $config_all  = $config_list->XMLin($config_file);
    my @config_set = @{ $config_all->{"config"} };
    foreach my $config_one (@config_set) {
        $tool_name = $config_one->{"implementation_tool"}[0];
    }
    return $tool_name;
}

$argument            = "";
$user_flag           = 0;
$user_arg            = "";
$altera              = 0;
$checkin             = 0;
$package             = 0;
$min_5               = 0;
$argument_count      = 1;
$regression_xml_file = "";
$mini_xml_file       = "";
$mode                = "";
$report              = "";
$compare             = "";
$tool                = "";
$platform            = "";
for ( $i = 0 ; $i <= @ARGV ; $i = $i + 1 ) {
    $argument .= " $ARGV[$i]";
    if ( $ARGV[0] eq "" ) {
        $argument_count = 0;
    }
    if ( $ARGV[$i] eq "-a" ) {
        $altera = 1;
    }
    if ( $ARGV[$i] eq "checkin" ) {
        $checkin = 1;
        $mode    = "checkin";
    }
    if ( $ARGV[$i] eq "package" ) {
        $package = 1;
        $mode    = "package";
    }
    if ( $ARGV[$i] eq "-min" ) {
        $min_5 = 1;
    }
    if ( $ARGV[$i] eq "-u" ) {
        $user_arg  = $ARGV[ $i + 1 ];
        $user_flag = 1;
    }
    if ( $ARGV[$i] eq "-x" ) {
        $regression_xml_file = $ARGV[ $i + 1 ];
        if ( $regression_xml_file eq "" ) {
            system "cp $config_dir/configuration.xml .";
            exit;
        }
        $mode = "normal";
    }
    if ( $ARGV[$i] eq "-h" ) {
        $mini_xml_file = $ARGV[ $i + 1 ];
        $compare_type  = "one_hour_test";
        if ( $mini_xml_file eq "" ) {
            if ($altera) {
                system
                  "cp $config_dir/one_hour_test_altera.xml one_hour_test.xml";
            }
            elsif ($min_5) {
                system "cp $config_dir/5mins_altera.xml .";
            }
            elsif ($user_flag) {
                if ( -e "$config_dir/$user_arg.xml" ) {
                    system "cp $config_dir/$user_arg.xml .";
                }
                else {
                    printf
"No file named $config_dir/$user_arg.xml in configurations\n";
                    exit;
                }
            }
            else {
                system "cp $config_dir/one_hour_test.xml .";
            }
            exit;
        }
        $mode = "one_hour_test";
    }
    if ( $ARGV[$i] eq "-one" ) {
        $mode = "one";
    }
    if ( $ARGV[$i] eq "-opt_hls" ) {
        $mode = "opt_hls";
    }
    if ( $ARGV[$i] eq "-opt_bit" ) {
        $mode = "opt_bit";
    }
    if ( $ARGV[$i] eq "-hls" ) {
        $compare_type = "hls";
    }
    if ( $ARGV[$i] eq "unit_test" ) {
        $mode = "unit_test";
    }
    if ( $ARGV[$i] eq "-report" ) {
        $report = "report";
    }
    if ( $ARGV[$i] eq "-compare" ) {
        $compare = "compare";
    }
    if ( $ARGV[$i] eq "-regression" ) {
        $mode = "regression_test";
    }
    if ( $ARGV[$i] eq "-ddr_bandwidth" ) {
        $mode = "ddr_bandwidth";
    }
    if ( $ARGV[$i] eq "-platform_test" ) {
        $mode = "platform_test";
    }
    if ( $ARGV[$i] eq "-altera" ) {
        $tool = "altera";
    }
    if ( $ARGV[$i] eq "-xilinx" ) {
        $tool = "xilinx";
    }
    if ( $ARGV[$i] eq "-platform" ) {
        $platform = $ARGV[ $i + 1 ];
    }
    if ( $ARGV[$i] eq "-step" ) {
        $step = $ARGV[ $i + 1 ];
    }
    if ( $ARGV[$i] eq "-frontend" ) {
        $report_mode = "front";
        $first_file  = $ARGV[ $i + 1 ];
        $second_file = $ARGV[ $i + 2 ];
        $output_file = $ARGV[ $i + 3 ];
    }
    if ( $ARGV[$i] eq "-backend" ) {
        $report_mode = "back";
        $first_file  = $ARGV[ $i + 1 ];
        $second_file = $ARGV[ $i + 2 ];
        $output_file = $ARGV[ $i + 3 ];
    }
    if ( $ARGV[$i] eq "-rodinia" ) {
        $mode = "rodinia";
    }
    if ( $ARGV[$i] eq "-random" ) {
        $mode = "random";
    }
}

if ( $argument_count == 0 ) {    #on top level?
    printf "\n";
    printf "unit test commandlines\n";
    printf "\t[options]                   : Create an example directory list\n";
    printf
"\t                            : options = unit_test/bugilla/commandline/...\n";
    printf
      "\t[options] [list]            : Execute options, need directory list\n";
    printf "\t[options] [list] -report    : Get execution report\n";
    printf "\n";
    printf "one hour test commandlines\n";
    printf
"\t-a -h                       : Create an example one hour test list for altera\n";
    printf
"\t-h                          : Create an example one hour test list for xilinx\n";
    printf "\t-h [xml_file]               : Execute one hour test on local\n";
    printf "\t-h [xml_file] -cloud        : Execute one hour test on cloud\n";
    printf "\t-h [xml_file] -report       : Get one hour test report\n";
    printf "\n";
    printf "on board test commandlines\n";
    printf
      "\t-x                          : Create an normal test example list\n";
    printf
"\t-x [xml_file]               : Execute normal test on local, could be hls/sim/bit\n";
    printf
"\t-x [xml_file] -cloud        : Execute normal test on cloud, could be hls/sim/bit\n";
    printf "\t-x [xml_file] -report       : Get normal test report\n";
    printf "\n";
    printf "random test commandlines\n";
    printf
"\t-random [dir] [number]      : run [dir] for random test, [dir] is a directory must\n";
    printf
"\t                              have makefile, the make file can [make test] to run\n";
    printf
"\t                              single test, and generate err.log to show pass or not.\n";
    printf
"\t                              if err.log file empty, passed; else failed.\n";
    printf
"\t                              [number] is the repeat time case run on condor, default 48.\n";
    printf "\t-random -report             : get random test report\n";
    printf "\n";
    printf "package test commandlines\n";
    printf
"\t-front                      : do frontend test with svn version, not package\n";
    printf
"\t-front_and_back i           : do frontend test and backend test with svn version, not package\n";
    printf "\t-pack_only                  : generate release package only\n";
    printf
"\t-pack_and_front             : generate release package and do frontend test on package\n";
    printf
"\t-pack_and_all               : generate release package and do all test on package\n";
    printf
"\t-checkin                    : run checkin test with simple bugzilla list\n";
    printf
"\t-checkin -full              : run checkin test with full bugzilla list\n";
    printf "\t-track                      : keep track on release version\n";
    printf "\t-exec                       : execute host on board\n";
    printf "\n";
    printf "other commandlines\n";
    printf
"\t-force                      : May delete some directory without any hints\n";
    printf
      "\t-regression -d              : run daily regression test as schedule\n";
    printf "\n";
    exit;
}

if ( $mode eq "unit_test" ) {
    system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_unit_test.pl $argument"
    );    #run unit test
}
elsif ( $compare eq "compare" ) {

    if ( $report_mode eq "front" ) {
        system
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/report/compare_frontend.pl $first_file $second_file $output_file";
    }
    elsif ( $report_mode eq "back" ) {
        system
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/report/compare_backend.pl $first_file $second_file $output_file";
    }
}
elsif ( $mode eq "one_hour_test" ) {
    if ( $report eq "" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_split.pl $mini_xml_file"
        );    #spilt config file
        if ( $argument =~ /(.*)$mini_xml_file(.*)/ ) {
            $argument_new1 = "$1no_bit_test_$mini_xml_file$2";
            $argument_new2 = "$1hls_$mini_xml_file$2";
        }
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_flow_test.pl $argument_new1"
        );    #run sim config file
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_flow_test.pl $argument_new2"
        );    #run hls config file
    }
    elsif ( $report eq "report" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/report/report_sim_hls.pl $argument"
        );    #get one_hour_test report
    }
}
elsif ( $mode eq "normal" ) {
    if ( $report eq "" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_flow_test.pl $argument"
        );    #run xml file
    }
    elsif ( $report eq "report" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/report/report_xml.pl $argument"
        );    #get xml file report
    }
}
elsif ( $mode eq "one" ) {
    if ( $report eq "" ) {
        system(
"mars_env_loader cp $MARS_TEST_HOME_GIT/configuration/configuration_one.xml ."
        );    #copy xml file
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_one.pl $argument"
        );    #config xml file
        if ( $argument =~ / hls/ ) {
            system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_flow_test.pl -x configuration_one.xml -cloud -force"
            );    #run xml file
        }
        else {
            system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_flow_test.pl -x configuration_one.xml -cloud -force -bit_and_test"
            );    #run xml file
        }
    }
    elsif ( $report eq "report" ) {
        if ( $argument =~ / hls/ ) {
            system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/report/report_collect.pl one hls"
            );    #config xml file
        }
        else {
            system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/report/report_collect.pl one bit"
            );    #config xml file
        }
    }
}
elsif ( $mode eq "opt_hls" ) {
    if ( $report eq "" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_opt_hls.pl $argument"
        );
    }
    elsif ( $report eq "report" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_opt_hls.pl $argument"
        );
    }
}
elsif ( $mode eq "opt_bit" ) {
    if ( $report eq "" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_opt_bit.pl $argument"
        );
    }
    elsif ( $report eq "report" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_opt_bit.pl $argument"
        );
    }
}
elsif ( $mode eq "regression_test" ) {
    system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_regression.pl $argument"
    );    #run regression test
}
elsif ( $mode eq "ddr_bandwidth" ) {
    if ( $report eq "" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_ddr_bandwidth.pl $argument"
        );
    }
    elsif ( $report eq "report" ) {
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/report/report_bandwidth.pl $argument"
        );
    }
}
elsif ( $mode eq "platform_test" ) {
    if ( $tool eq "altera" ) {
        system("cp $MARS_TEST_HOME_GIT/script/run/write_excel_bandwidth.py .");
        system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/platform_test.pl $platform altera $step"
        );
    }
}
elsif ( $mode eq "checkin" ) {
    system
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/merlin_package_test.pl $argument";
}
elsif ( $mode eq "package" ) {
    system
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/merlin_package_test.pl $argument";
}
elsif ( $mode eq "rodinia" ) {
    system
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/rodinia_test.pl $argument";
}
elsif ( $mode eq "random" ) {
    system
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/merlin_random.pl $argument";
}
else {
    system(
"mars_env_loader perl $MARS_TEST_HOME_GIT/script/run/mars_unit_test.pl $argument"
    );
}
