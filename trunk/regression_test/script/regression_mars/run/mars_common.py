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

#print "here we defines\n";

def create_project_safe(prj, src):
    #print "here we called, prj="+prj+"\n";
    ##########################################
    # Project creating
    if (not os.path.isdir(prj)) :
        os.system("merlin_create_project "+prj+" -s "+src+" -copy -keep_device");
    if os.path.isfile("mars_cloud_replace_xml.pl"):
        os.system("perl mars_cloud_replace_xml.pl");
    if os.path.isfile("../../directive.xml") :
        os.system("cp ../../directive.xml "+prj+"/spec");
    ##########################################


