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

################################################################
# Description: Create instance from AWS and configure them with condor
# Owners: 
#       Peng Zhang (pengzhang@falcon-computing.com) 2017091
################################################################
#
use File::Basename;
use feature qw(say);
use strict;
use warnings;


my $perl_file_name = $0;

sub show_usage {
    my $name = basename($perl_file_name);
    print "ERROR: Command argument incorrect\n";
    print "Usage: \n";
#    print "    perl $name instance_type instance_num slot_num slot_cpu slot_memory\n";
    print "    mars_aws instance_type instance_num slot_num slot_cpu slot_memory\n";
    print "         - instance_type = r4.2xlarge|c4.xlarge|... \n";
    print "\n";
#    print "    perl $name default|list|reset_condor|shutdown|help\n";
    print "    mars_aws create|list|reset_condor|shutdown|help\n";
    print "         - create,default: Create the instances in default settings \n";
    print "         - list: list the IP of all the instances created in the current directory \n";
    print "         - reset_condor: Restart the condor sevices on all the instances created in the current directory \n";
    print "         - shutdown,terminate: Terminate all the instances created in the current directory \n";
    print "         - create_for_build: Create a powerful node for build, and generate the command to ssh to the node \n";
    print "         - build: Create a powerful node, run ~/merlin_build.sh, and terminate the node \n";
    print "         - help: Show this help message \n";
    exit();
}


my $arg_num = @ARGV;
if ($arg_num > 5 or $arg_num < 1) {
    show_usage();
}

my $instance_type;
my $instance_num = "5";
my $slot_num     = "4";
my $slot_cpu     = "1";
my $slot_mem     = "2000";
my $is_generate_ssh = 0;
my $is_reset_condor = 1;
my $is_run_build = 0;
my $is_terminate = 0;
my $add_alarm = "";   # "--no-alarm". By default it has alarm
my $storage = "";     # "-s 200"
my $request_type = "request-spot";     # "request"

if ($arg_num >= 1) {
    $instance_type=$ARGV[0];
}
if ($arg_num >= 2) {
    $instance_num=$ARGV[1];
}
if ($arg_num >= 5) {
    $slot_num=$ARGV[2];
    $slot_cpu=$ARGV[3];
    $slot_mem=$ARGV[4];
}

#system "echo '$instance_type $instance_num $slot_num $slot_cpu $slot_mem' > .instance_setup"

if ($instance_type eq "default" or $instance_type eq "create") {
    $instance_type= "r4.2xlarge";
}
elsif ($instance_type eq "create_for_build") {
#    $instance_type= "r4.16xlarge";
    $instance_type= "r4.8xlarge";
    if ($arg_num >= 2) {
        $instance_type = $ARGV[1];
    }
    $instance_num = "1";
    $is_generate_ssh = 1;
    $is_reset_condor = 0;
    $storage = "-s 200"; # 200GB local disk
#    $request_type = "request";    
    $add_alarm = "--no-alarm";   # ""

}
elsif ($instance_type eq "build") {
   #$instance_type= "r4.16xlarge";
    $instance_type= "c4.8xlarge";
    if ($arg_num >= 2) {
        $instance_type = $ARGV[1];
    }
    $instance_num = "1";
    $is_generate_ssh = 0;
    $is_reset_condor = 0;
    $is_run_build = 1;
    $is_terminate = 1;
}
elsif ($instance_type eq "list") {
    say ("Listing the running aws instances created in this directory...");
    my $inst_list = `grep "^IP=" .aws_inst* /dev/null 2>&1`;
    if (@ARGV > 1 and $ARGV[1] eq "debug") {
        print "$inst_list\n";
    }

    if ($inst_list =~ "No such file or directory") {
        print "No active AWS condor nodes.\n";
        exit;
    }

    my @lines = split('\n', $inst_list);
    my $total_nodes = 0;
    my $total_slots = 0;
    foreach my $line (@lines) {
        if ($line =~ /^\.aws_inst_(\d+)_(\d+)_(\d+)_(\d+)_(\d+)_(\d+)_(\d+)\.(\S+\.\S+large)\.s(\d+)\.c(\d+)\.m(\d+):IP=(\d+\.\d+\.\d+\.\d+)/) {
            my $instance_name = "condor_merlin_$1";
            my $created_date = "$2/$3/$4 $5:$6:$7";
            my $instance_type = $8;
            my $slot_num = $9;
            my $slot_cpu = $10;
            my $slot_mem = $11;
            my $instance_ip = $12;

            $total_nodes += 1;
            $total_slots+= $slot_num;
            print "IP=${instance_ip}  \t type=${instance_type} slots=${slot_num} slot_cpu=${slot_cpu} slot_mem=${slot_mem} create_date=${created_date} \n";
        }
    }
    print "Total $total_nodes nodes, $total_slots slots\n";
    exit;
}
elsif ($instance_type eq "shutdown" or $instance_type eq "terminate") {

    say ("Shutdown all the running aws instances created in this directory.");
    my $deleted_log_dir= ".deleted_aws_inst/";
    if (!-d "$deleted_log_dir") {
        system ("mkdir $deleted_log_dir");
    }

    my $inst_list = `grep "^IP=" .aws_inst* /dev/null 2>&1`;
    if ($inst_list =~ "No such file or directory") {
        print "No active AWS condor nodes.\n";
        exit;
    }

    my @lines = split('\n', $inst_list);
    foreach my $line (@lines) {
        print "++++ $line ++++\n";
        if ($line =~ /^(.+):IP=(\d+\.\d+\.\d+\.\d+)/) {
            my $log_file = $1;
            my $instance_ip = $2;
#            say ("falcon-aws terminate $instance_ip");
#            say ("mv $log_file $deleted_log_dir");
            system ("falcon-aws terminate $instance_ip");
            system ("mv $log_file $deleted_log_dir");

            my $ssh_file = "ssh_to_aws_$instance_ip";
            if (-e "$ssh_file") {
                system ("mv $ssh_file $deleted_log_dir");
            }
        }
    }
    exit();
}
elsif ($instance_type eq "reset_condor") {

    say ("Reset condor service on the running aws instances created in this directory.");
    my $inst_list = `grep "^IP=" .aws_inst* /dev/null 2>&1`;
    if ($inst_list =~ "No such file or directory") {
        print "No active AWS condor nodes.\n";
        exit;
    }

    my @lines = split('\n', $inst_list);
    foreach my $line (@lines) {
        if ($line =~ /^(.+):IP=(\d+\.\d+\.\d+\.\d+)/) {
            my $log_file = $1;
            my $instance_ip = $2;
            system ("sleep 5");
            system ("$ENV{MARS_TEST_HOME_GIT}/script/run/set_condor.sh $instance_ip");
        }
    }
    exit();
}
elsif ($instance_type =~ /[rcm]\d\.\S+large/) {
    # not exist;
}
else {
    show_usage();
    exit();
}

if ($instance_type !~ /[rcm]\d\.\S+large/) {
    show_usage();
    exit();
}



if ($instance_num <=0) { show_usage(); }

for ( 1 .. $instance_num) {
    system("sleep 5"); # try to avoid conflict when multiple requests happen simuteneously
    my $child_pid = fork();
    if (!$child_pid) {  # child thread
        my $file_suffix = `date "+%m_%d_%y_%H_%M_%S"`; chomp($file_suffix);
        my $random = int(rand(10000));
        $file_suffix = "_${random}_".$file_suffix;
        $file_suffix .= ".$instance_type";
        $file_suffix .= ".s$slot_num";
        $file_suffix .= ".c$slot_cpu";
        $file_suffix .= ".m$slot_mem";

        my $log_file = ".aws_inst".$file_suffix;
       # print ("falcon-aws request-spot -i  ami-972dc0ed -d $instance_type -t condor_merlin_$random |& tee $log_file \n");
       #system ("falcon-aws request-spot -i ami-581e0123 -d $instance_type -t condor_merlin_$random --no-alarm |& tee $log_file");
       #system ("falcon-aws request-spot -i ami-972dc0ed -d $instance_type -t condor_merlin_$random --no-alarm |& tee $log_file");
       #system ("falcon-aws request -i ami-972dc0ed -d $instance_type -t condor_merlin_$random -s 200 --no-alarm |& tee $log_file");
        # system ("falcon-aws request -i ami-e7c11b9a -d $instance_type -t ebs_merlin_$random -s 200 --no-alarm |& tee $log_file");
        print ("falcon-aws $request_type -i ami-2ef42e53 -d $instance_type -t merlin_dev_$random $storage $add_alarm |& tee $log_file \n");
        system ("falcon-aws $request_type -i ami-2ef42e53 -d $instance_type -t merlin_dev_$random $storage $add_alarm |& tee $log_file");
        
        my $log_text= `grep "instance with private ip" $log_file 2>&1`;
        my $instance_ip;
        if ($log_text =~ /Obtained a .* instance with private ip: (\d+\.\d+\.\d+\.\d+)/) {
            $instance_ip = $1;
            system "echo 'IP=$instance_ip' >> $log_file";
        }
        else {
            # Please check the version of "falcon-aws", which impact the log file content
            print "ERROR: Can not get the IP for an instance.\n";
            exit;
        }
        if ( $is_reset_condor == 1) {
            say ("source $ENV{MARS_TEST_HOME_GIT}/script/run/set_condor.sh $instance_ip");
            system ("$ENV{MARS_TEST_HOME_GIT}/script/run/set_condor.sh $instance_ip");
        }
        if ( $is_generate_ssh == 1 and $instance_ip ne "") {
            my $ssh_file = "ssh_to_aws_$instance_ip";
            system ("echo '' > $ssh_file ");

            open(my $fh, '>', "$ssh_file");
            print $fh "ssh merlin2 -t 'ssh -o \"StrictHostKeyChecking no\" $instance_ip'\n";
            close $fh;
        }
        if ( $is_run_build == 1 and $instance_ip ne "") {
            system ("ssh merlin2 -t \"ssh '-o StrictHostKeyChecking no' $instance_ip -t \'bash ~/merlin_build.sh\' \" ");
        }
        if ( $is_terminate == 1 and $instance_ip ne "") {
            my $deleted_log_dir= ".deleted_aws_inst/";
            system ("falcon-aws terminate $instance_ip");
            if (-e "$log_file") {
                system ("mv $log_file $deleted_log_dir");
            }

            my $ssh_file = "ssh_to_aws_$instance_ip";
            if (-e "$ssh_file") {
                system ("mv $ssh_file $deleted_log_dir");
            }
        }
        exit();
    }
}

for ( 1 .. $instance_num) {
    my $child_pid = wait();
}

