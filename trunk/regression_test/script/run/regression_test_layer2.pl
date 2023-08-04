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
#time out
#increase test
$cmost_top = $ENV{'MERLIN_COMPILER_HOME'};
$merlin_compiler_home = $ENV{'MERLIN_COMPILER_HOME'};

$regression_dir = "${cmost_top}/2015.4/test/regression_mars";
$xml_path = "${cmost_top}/2015.4/mars-gen/default/directive_mars_basic.xml";
$default_name = "config0";

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
    $test_cases_dir = "${cmost_top}/2015.4/test/regression_mars/test_cases_org";
}

#choose run place
$run_place = "";
for($i=0; $i<10; $i++) {
    if($ARGV[$i] eq "-cloud") {
        $run_place = "cloud";
    }
}

#choose case.list and xml.list configure mode
$type = "";
$test_mode = "";
for($i=0; $i<20; $i++) {
    if($ARGV[$i] eq "-a") {
	$type = "all";
	$all_file = $ARGV[$i+1];
    }
    if($ARGV[$i] eq "-s") {
	$type = "seperate";
	$case_file = $ARGV[$i+1];
    }
    if($ARGV[$i] eq "-c") {
	$type = "seperate";
	$configure_file = $ARGV[$i+1];
    }
    if($ARGV[$i] eq "-x") {
	$type = "xml";
    $xml_file = $ARGV[$i+1];
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
}

#choose run_bitstream mode
sub run_command {
    $run_which  = $_[0];
    $run_where  = $_[1]; 
    if($run_place eq "cloud") {
	if($run_which eq "bit") {
            return "make all_bit_cloud";
	} elsif($run_which eq "hls") {
            return "make all_hls_cloud";
	} elsif($run_which eq "front") {
            return "make all_front_cloud";
	} elsif($run_which eq "sim") {
            return "make all_sim_cloud";
	} elsif($run_which eq "host") {
            return "make all_host_cloud";
	} elsif($run_which eq "test") {
            return "make all_test_cloud";
	}
    } else {
	if($run_which eq "bit") {
            return "make all_bit";
	} elsif($run_which eq "hls") {
            return "make all_hls";
	} elsif($run_which eq "front") {
            return "make all_front";
	} elsif($run_which eq "sim") {
            return "make all_sim";
	} elsif($run_which eq "host") {
            return "make all_host";
	} elsif($run_which eq "test") {
            return "make all_test";
	}
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
            print case_out "case_src/$config_number-$_";
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
        if($one_case_tmp =~ /(\S*)/) {
            system "cp -r $test_cases_dir/$1 run_$config_name/run/case_src/$config_name-$1";
	    }
    }
}
                                
#xml file configure
sub xml_configure {
    $argument = $_[0];
    if($argument eq "default_arg") {
	$number		= "config0";
    	$bit	   	= "bit";
    	$tool	   	= "built-in";
        $platform_name  = "baidu_kintex7";
        $clock_period   = "6.6ns";
        $run_bitstream  = "normal";
    } elsif ($argument eq "default_xml") {
	$number		= $config_name;
    	$bit	   	= $run_flow_default;
    	$tool	   	= $run_tool_default;
        $platform_name  = $run_platform_default;
        $clock_period   = $run_clock_default;
        $run_bitstream  = $run_bit_mode_default;
    } else {
        $number	        = $1;
        $bit	        = $2;
        $tool	        = $3;
        $platform_name  = $4;
        $run_bitstream  = $5;
        $clock_period   = $6;
    }
    $directive_in  = "directive.xml";
    $directive_out = "directive_$number.xml";
    open(xml_file_in,"< $directive_in");
    open(xml_file_out,"> $directive_out");
    while(<xml_file_in>) {
            ~s/<clock_period>(.*)<\/clock_period>\n/<clock_period>$clock_period<\/clock_period>\n/g;
        if($run_bitstream eq "kernel") {
            ~s/<kernel>(.*)<\/kernel>\n/<kernel>on<\/kernel>\n/g;
            ~s/<normal>(.*)<\/normal>\n/<normal>off<\/normal>\n/g;
        } elsif($run_bitstream eq "normal") {
            ~s/<kernel>(.*)<\/kernel>\n/<kernel>off<\/kernel>\n/g;
            ~s/<normal>(.*)<\/normal>\n/<normal>on<\/normal>\n/g;
        }
            ~s/<implementation_tool>(.*)<\/implementation_tool>\n/<implementation_tool>$tool<\/implementation_tool>\n/g;

    	if($tool eq "sdaccel") {
    	    if($platform_name eq "alphadata_virtex7") { 
                    ~s/<platform_name>(.*)<\/platform_name>/<platform_name>alphadata_pcie7v3<\/platform_name>/g;
                    ~s/<part_name>(.*)<\/part_name>/<part_name>xc7vx690tffg1761-2<\/part_name>/g;
                    ~s/<arch_name>(.*)<\/arch_name>/<arch_name>virtex7<\/arch_name>/g;
    	    }
    	}
    
    	if($tool eq "built-in") {
    	    if($platform_name eq "baidu_kintex7") { 
                    ~s/<platform_name>(.*)<\/platform_name>/<platform_name>baidu_kintex7<\/platform_name>/g;
                    ~s/<part_name>(.*)<\/part_name>/<part_name>xc7k480tffg1156-1<\/part_name>/g;
                    ~s/<arch_name>(.*)<\/arch_name>/<arch_name>kintex7<\/arch_name>/g;
    	    }
    	    if($platform_name eq "inspur_kintex7") { 
                    ~s/<platform_name>(.*)<\/platform_name>/<platform_name>alphadata_pcie7v3<\/platform_name>/g;
                    ~s/<part_name>(.*)<\/part_name>/<part_name>xc7k480tffg1156-2<\/part_name>/g;
                    ~s/<arch_name>(.*)<\/arch_name>/<arch_name>kintex7<\/arch_name>/g;
    	    }
    	}
	                                
        print xml_file_out;
    }
    close(xml_file_in);
    close(xml_file_out);
}

sub query_exit
{
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

#    print "\n input = +$input+ not_yes=${not_yes} is_yes=${is_yes} \n";


    if ($exit_option eq "yes" and $default eq "yes" and $not_no ) { exit; }
    if ($exit_option eq "yes" and $default eq "no"  and $is_yes ) { exit; }
    if ($exit_option eq "no"  and $default eq "yes" and $is_no  ) { exit; }
    if ($exit_option eq "no"  and $default eq "no"  and $not_yes) { exit; }
}

if(-e "directive_developer.xml") {
    system "cp directive_developer.xml directive.xml";
} else {
    system "cp $xml_path directive.xml";
}

#run case and configure
if($type eq "all") { 							#all mode
    if($test_mode eq "") {
        if (-e "run_$all_file") {
            query_exit("Directory run_$all_file exists. Overwrite?", "no", "no");
        }
        system "rm -rf run_$all_file";	
        system "cp -r $regression_dir run_$all_file";	
        system "mkdir run_$all_file/run/case_src";	
        system "mkdir run_$all_file/run/work";	

        #case configure
        $case_in  = "$all_file";
        $case_out = "case_out.list";
        open(case_in,"< $case_in");
        open(case_out,"> $case_out");
        while(<case_in>) {
           	    if(~/\|(\S*)\s*\|(.*)\|(.*)\|(.*)\|(.*)\|(.*)\|(\S*)\s*\|\n/) {
                    print case_out "case_src/$1-$7\n";
           	    }
        }
        close(case_in);
        close(case_out);
        system "mv case_out.list run_$all_file/run/case.list";

        #configure xml file
        $all_list = `cat $all_file`;
        @all_set=split(/\n/,$all_list);
        foreach $one_set (@all_set) {
            if($one_set =~ /\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|/) {
                xml_configure(normal);
                system "mv $directive_out run_$all_file/run/directive_$1-$7.xml";

                $case_name = "$1-$7";
                system "cp -r $test_cases_dir/$7 run_$all_file/run/case_src/$case_name";
                system "cp run_$all_file/run/mars_replace_xml.pl run_$all_file/run/case_src/$case_name";
	        system "cp -r run_$all_file/run/case_src/$case_name run_$all_file/run/work";	
	        system "cd run_$all_file/run/work/$case_name; mars_create_project run -s src -copy -I${merlin_compiler_home}/mars-gen/lib/merlin";	
	        system "cd run_$all_file/run/work/$case_name/run/spec; cp ../../../../directive_$1-$7.xml directive.xml";	
	        system "cd run_$all_file/run/work/$case_name; mars_flow_cloud run $2 -s -c $case_name";	
            }	
        }

        #run command
#        $run_command_return = run_command($bit,$run_place);	
#        system "cd run_$all_file/run; $run_command_return";	
    } elsif($test_mode eq "test") {
        $all_list = `cat $all_file`;
        @all_set=split(/\n/,$all_list);
        foreach $one_set (@all_set) {
            if($one_set =~ /\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|/) {
        	if($2 eq "bit") {
        	    system "cd run_$all_file/run/work/$1-$7; mars_flow_cloud run test -s -c";	
        	}
	    }
	}
    } 
} elsif($type eq "seperate") {						#seperate mode
    my $platform_name;
    my $clock_period;
    my $run_bitstream;
    if($configure_file eq "") {
	if($test_mode eq "") {
            if (-e "run_$default_name") {
                query_exit("directory run_$default_name exists. overwrite?", "no", "no");
            }
            system "rm -rf run_$default_name";	
            system "cp -r $regression_dir run_$default_name";	
            system "mkdir run_$default_name/run/case_src";
            xml_configure(default_arg);

	    #copy case
            system "cp $case_file case.list.tmp";	
            $case_tmp = "case.list.tmp"; 
            case_configure($case_tmp,$default_name);
            case_copy($default_name);
            system "rm case.list.tmp";	
            system "cp case_out.list run_$default_name/run/case.list";	
            system "cp directive_$default_name.xml run_$default_name/run/directive.xml";	
	    
	    #run command
            $run_command_return = run_command($bit,$run_place);	
            system "cd run_$default_name/run; $run_command_return";	
	} elsif($test_mode eq "test") {
            xml_configure(default_arg);
	    if($bit eq "bit") {
                $run_command_return = run_command($bit,$run_place);	
                system "cd run_$default_name/run; $run_command_return";	
	    }
	}
    } else {
        $configure_info = `cat $configure_file`;
        @configure_set = split(/\n/,$configure_info);
	if($test_mode eq "") {
            foreach $one_configure (@configure_set) {
               if($one_configure =~ /\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|/g) {
     	           if (-e "run_$1") {
     	               query_exit("Directory run_$1 exists. Overwrite?", "no", "no");
     	           }
     	           system "rm -rf run_$1";	
                   system "cp -r $regression_dir run_$1";	
            	   system "mkdir run_$1/run/case_src";
     
     	       	   #configure xml
     	           xml_configure(normal);
     
     	           #copy case
     	           if($case_file eq "") {
     	               if(-e "case.list") {
     	                   system "cp case.list case.list.tmp";	
     	               } else {
     	               	   system "cp $regression_dir/../run_perl/case.list case.list.tmp";
     	               }	
     	           } else { 
     	               system "cp $case_file case.list.tmp";	
     	           }
     	           $case_tmp = "case.list.tmp"; 
                    case_configure($case_tmp,$number);
                   	case_copy($number);
     	           system "rm case.list.tmp";	
     
     	           #run command
                   system "cp case_out.list run_$1/run/case.list";	
                   system "mv directive_$1.xml run_$1/run/directive.xml";	
                   $run_command_return = run_command($bit,$run_place);	
                   system "cd run_$1/run; $run_command_return";	
                }
            }
	} elsif($test_mode eq "test") {
            foreach $one_configure (@configure_set) {
                if($one_configure =~ /\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|(\S*)\s*\|/g) {
     	            xml_configure(normal);
	    	    if($bit eq "bit") {
            	        $run_command_return = run_command("test",$run_place);	
            	        system "cd run_$1/run; $run_command_return";	
	    	    }
		}
	    }
	}
    }
} elsif($type eq "") {								#default mode
    if($test_mode eq "") {
        if (-e "run_$default_name") {
            query_exit("Directory run_$default_name exists. Overwrite?", "no", "no");
        }
        system "rm -rf run_$default_name";	
        system "cp -r $regression_dir run_$default_name";	
        system "mkdir run_$default_name/run/case_src";

        #configure xml file
        xml_configure(default_arg);

        #copy case
        system "cp $regression_dir/../run_perl/case.list case.list.tmp";
        $case_tmp = "case.list.tmp"; 
        case_configure($case_tmp,$default_name);
        case_copy($default_name);
        system "rm case.list.tmp";	

        #run_command
        system "cp case_out.list run_$default_name/run/case.list";	
        system "cp directive_$default_name.xml run_$default_name/run/directive.xml";	
        $run_command_return = run_command($bit,$run_place);	
        system "cd run_$default_name/run; $run_command_return";	
    } elsif($test_mode eq "test") {
	    $dir_list = `ls $work_dir`;
	    @dir_set = split(/\n/, $dir_list);
	    foreach $one_dir (@dir_set) { 
                $run_command_return = run_command("test",$run_place);	
                system "cd $one_dir/run; $run_command_return";	
	    }
    }
} elsif($type eq "xml") {
    use File::Basename;
    use XML::Simple;
    use Data::Dumper;
    
    if($test_mode eq "") {
        my $config_list = XML::Simple->new(ForceArray => 1);
        my $config_all = $config_list->XMLin($xml_file);
    #    print Dumper($config_all); 
        my $xml_file_info = `cat $xml_file`;  
        @xml_info = split(/\n/,$xml_file_info);
        
        my $attribute_count = 0;
        my @config_set = @{$config_all->{"config"}};
        foreach my $config_one (@config_set) {
            $name_one =  $config_one->{"name"}[0];
            if($name_one eq "") {
                printf "Please specify config name!\n";
                exit;
            }
            $case_one =  $config_one->{"case"}[0];
            if($case_one eq "") {
                printf "Please specify case from case pool for configure name $name_one\n";
                exit;
            }
            $flow_one =  $config_one->{"flow"}[0];
            if($flow_one eq "") {
                printf "Please specify run flow for configure name $name_one\n";
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
    #        print Dumper($directive_all); 
            if($xml_file_info =~ /$name_one([\s\S]*?)<\/config>/) {
                $attribute_count = 0;
                @attribute_set = split(/\n/,$1);
                foreach $one_attribute (@attribute_set) {
                    $attribute_count = $attribute_count + 1;;
                    if($attribute_count == 1) {
                        #config case
                        $case_line = $config_one->{"case"}[0];
                        if($case_line eq "all") {
                            system "cp $regression_dir/../run_perl/case.list case.list.tmp";
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
                    }
                    
                    #config xml
                    if($attribute_count>3) {
                        if($one_attribute =~ /<\/(.*)>/) {
                            if($directive_all->{"$1"}[0] eq "") {
                                printf "No Attribute named $1!\n"
                            } else {
                                $directive_all->{"$1"}[0] = $config_one->{"$1"}[0];
                            }
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

            #run command
            $run_command_return = run_command($flow_one,$run_place);	
            system "cd run_$name_one/run; $run_command_return";	
        }
    } elsif ($test_mode eq "test") {
        my $config_list = XML::Simple->new(ForceArray => 1);
        my $config_all = $config_list->XMLin($xml_file);
    #    print Dumper($config_all); 
        my @config_set = @{$config_all->{"config"}};
        foreach my $config_one (@config_set) {
            $name_one =  $config_one->{"name"}[0];
            $flow_one =  $config_one->{"flow"}[0];
            if($flow_one eq "bit"){
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
    while(1) {
        $dir_list = `ls`;
    	@dir_set = split(/\n/, $dir_list);
    	foreach $one_dir (@dir_set) {
            if(-e "$one_dir/run/work") {
    	        $project_list = `ls $one_dir/run/work`;
    	        @project_set = split(/\n/, $project_list);
    	        foreach $one_project (@project_set) {
                    $test_path = "$one_dir/run/work/$one_project";
                    $file_name1 = "$one_dir/run/work/$one_project/run/implement/pkg/design_1_wrapper.bit";
                    $file_name2 = "$one_dir/run/work/$one_project/run/implement/pkg/kernel_pblock_fcs_kernel_0_partial.bit";
                    $tested = "$one_dir/run/work/$one_project/run/implement/pkg/tested";
                    if((-e $file_name1) or (-e $file_name2)) {
                        printf "$one_project\n";
                        if(! -e "$tested") {
                            system "touch $tested";
                            system "cd $test_path; mars_flow_cloud run test";
                            printf "$test_path on board tested!\n";
                        }
                    } 
                }    
            }
    	}
        sleep(300);
    }
}
