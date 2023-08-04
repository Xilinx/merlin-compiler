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

$curr_dir = `pwd`;
chomp($curr_dir);
printf "$curr_dir\n";
$work_dir = "$curr_dir";

sub check_ii {
   #$key1 = $_[0];

   system "rm -rf info_summary.log";
   system "touch info_summary.log";

   open (FILE,"+>./info_summary.log") or die "can't open file:$!"; 
   print FILE "II info summary:\n";
   print FILE "\n";
   print FILE "\n";

   for(my $i=1;$i<34 + 1;$i++){
       system "cd $curr_dir/test$i";
       $work_dir = "$curr_dir/test$i";
       print FILE "/*Test$i*/\n";
       if(-e "$work_dir/ii_last.log"){
        print FILE "last test:\n";
        open (OLDFILE,"$work_dir/ii_last.log") or die "can't open file:$!";
        while ( my $line_last = <OLDFILE>){
            print FILE $line_last;
        }
        print FILE "\n";
       }
       if(-e "$work_dir/ii.log"){
        print FILE "this test:\n";
        open (NEWFILE,"$work_dir/ii.log") or die "can't open file:$!";
        while ( my $line_new = <NEWFILE>){
            print FILE $line_new;
        }
        print FILE "\n";
       }
        print FILE "\n";
   }

   close NEWFILE;
   close OLDFILE;
   close FILE;
   return 0;
}

check_ii();
