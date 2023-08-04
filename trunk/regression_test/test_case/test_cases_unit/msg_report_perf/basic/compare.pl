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

my $ref_json = $ARGV[0];
my $new_json = $ARGV[1];
my $ref_log  = $ARGV[2];
my $new_log  = $ARGV[3];

sub get_data_from_json {
    my $json_file = $_[0];
    my $name = "";
    my %module_key_value = ();
    my %key_value = ();
    my @data_set;
    my $find_flag = 0;
    my $file_info = `cat $json_file`;
    my @file_info_set = split(/\n/, $file_info);
    foreach my $one_line(@file_info_set) {
        if($one_line =~ /\"(F_.*)\":/ 
                or $one_line =~ /\"(L_.*)\":/ 
                or $one_line =~ /\"(TOP_.*)\":/ 
                or $one_line =~ /\"(X_.*)\":/) {
            $name = $1;
#            print "name = $name\n";
        }
        if($one_line =~ /\"CYCLE_UNIT\": \"(.*)\"/) {
            my $key = "CYCLE_UNIT";
            my $data = $1;
            $key_value{$name}{$key} = $data;
#            print "$key = $data\n";
        }
        if($one_line =~ /\"CYCLE_TOT\": \"(.*)\"/) {
            my $key = "CYCLE_TOT";
            my $data = $1;
            #push @data_set, $data;
            $key_value{$name}{$key} = $data;
#            print "$key = $data\n";
        }
        if($one_line =~ /\"glue-latency\": \"(.*)\"/) {
            my $key = "glue-latency";
            my $data = $1;
            $key_value{$name}{$key} = $data;
#            print "$key = $data\n";
        }
        if($one_line =~ /\"glue-latency-iteration\": \"(.*)\"/) {
            my $key = "glue-latency-iteration";
            my $data = $1;
            $key_value{$name}{$key} = $data;
#            print "$key = $data\n";
        }
    }
    return %key_value;
}

sub get_data_from_log {
    my $log_file = $_[0];
    my $first_cycle = 0;
    my $second_cycle = 0;
    my $third_cycle = 0;
    my $file_info = `cat $log_file`;
    my @file_info_set = split(/\n/, $file_info);
    foreach my $one_line(@file_info_set) {
        if($one_line =~ /FIRST_CYCLES=(.*)/) {
            $first_cycle = $1;
#            printf "first cycle = $first_cycle\n";
        }
        if($one_line =~ /SECOND_CYCLES=(.*)/) {
            $second_cycle = $1;
#            printf "second cycle = $second_cycle\n";
        }
        if($one_line =~ /THIRD_CYCLES=(.*)/) {
            $third_cycle = $1;
#            printf "third cycle = $third_cycle\n";
        }
    }
    return ($first_cycle, $second_cycle, $third_cycle);
}

sub write_file {
    my ($filename, $content) = @_;

    open my $out, '>:encoding(UTF-8)', $filename or die "Could not open '$filename' for writing $!";;
    print $out $content;
    close $out;

    return;
}

my $output_string = "";
#print "\n\n\n\n";
if(not -e $ref_log) {
    $output_string .= "$ref_log not exist\n";

}
if(not -e $ref_json) {
    $output_string .= "$ref_json not exist\n";

}
if(not -e $new_log) {
    $output_string .= "$new_log not exist\n";

}
if(not -e $new_json) {
    $output_string .= "$new_json not exist\n";

}

my %ref_data = get_data_from_json($ref_json);
my %new_data = get_data_from_json($new_json);

foreach my $name (sort keys %ref_data) {
    foreach my $key (keys %{ $ref_data{$name} }) {
        if($ref_data{$name}{$key} ne $new_data{$name}{$key}) {
            $output_string .= "$name, $key: $ref_data{$name}{$key} $new_data{$name}{$key}\n";
#            print "$name, $key: $ref_data{$name}{$key} $new_data{$name}{$key}\n";
        }
    }
}

(my $ref_first_cycle, $ref_second_cycle, $ref_third_cycle) = get_data_from_log($ref_log);
(my $new_first_cycle, $new_second_cycle, $new_third_cycle) = get_data_from_log($new_log);
if($ref_first_cycle ne $new_first_cycle) {
#    print "ref_first_cycle = $ref_first_cycle\n";
#    print "new_first_cycle = $new_first_cycle\n";
    $output_string .= "ref_first_cycle = $ref_first_cycle, ";
    $output_string .= "new_first_cycle = $new_first_cycle, ";
}
if($ref_second_cycle ne $new_second_cycle) {
#    print "ref_second_cycle = $ref_second_cycle\n";
#    print "new_second_cycle = $new_second_cycle\n";
    $output_string .= "ref_second_cycle = $ref_second_cycle, ";
    $output_string .= "new_second_cycle = $new_second_cycle, ";
}
if($ref_third_cycle ne $new_third_cycle) {
#    print "ref_third_cycle = $ref_third_cycle\n";
#    print "new_third_cycle = $new_third_cycle\n";
    $output_string .= "ref_third_cycle = $ref_third_cycle, ";
    $output_string .= "new_third_cycle = $new_third_cycle, ";
}
write_file("err.log", $output_string);
