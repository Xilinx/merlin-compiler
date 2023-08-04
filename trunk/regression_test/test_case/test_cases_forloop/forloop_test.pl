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

  system "rm -rf ii_last.log";
  if(-e "ii.log"){
   system "mv ii.log ii_last.log";
  }
  system "touch ii.log";
   open (FILE,"+>./ii.log") or die "can't open file:$!";
   open (TOPFILE,"./kernel_top/kernel_top.log") or die "can't open file:$!";
   open (FLANNFILE,"./kernel_flann/kernel_flann.log") or die "can't open file:$!";
   print FILE "unflatten: II = ";
   while(my $topline = <TOPFILE>){
    $topline =~ /every (\d+) cycles/;
    my $ii = $1;
    if($ii){
        print FILE "$ii,";
    }
    if($topline =~ /every cycle/){ 
        print FILE ("1,");
    }
   }
   print FILE ("\n");
   close TOPFILE;

   print FILE "flatten:   II = ";
   while(my $flannline = <FLANNFILE>){
    $flannline =~ /every (\d+) cycles/;
    my $ii = $1;
    if($ii){
        print FILE "$ii,";
    }
    if($flannline =~ /every cycle/){ 
        print FILE ("1,");
    }
   }
   close FLANNFILE;
   close FILE;

   open (ERRFILE,"+>./err.log") or die "can't open file:$!";
   if(-e "ii_last.log"){
    open (OLDFILE,"./ii_last.log") or die "can't open file:$!";
    open (NEWFILE,"./ii.log") or die "can't open file:$!";
    $line_num = 0;
    while (defined(OLDFILE) and defined(NEWFILE) and
            my $line1 = <OLDFILE> and my $line2 = <NEWFILE>){
                $line_num ++;
                if ($line1 ne $line2){
                    print ERRFILE ("II changed!");
                }
    }
   }

   close NEWFILE;
   close OLDFILE;
   close ERRFILE;
   return 0;
}

check_ii();
