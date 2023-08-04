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




import os, sys;

case = sys.argv[1];

ref_dir = "../../test_ref/";

def run_command ( cmd ): 
    "print and execute the command"
    #print "# "+cmd;
    os.system(cmd);
    return 

ref_case_dir = ref_dir + case;

ret = os.system("diff --exclude=\".svn\" run/implement/export " + ref_case_dir );  
os.system("diff --exclude=\".svn\" run/implement/export " + ref_case_dir + " |& tee -a ../diff.log");  

if (0 == ret) :
    os.system("touch pass.o");



