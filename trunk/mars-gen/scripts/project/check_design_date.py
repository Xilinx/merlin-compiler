# (C) Copyright 2016-2021 Xilinx, Inc.
# All Rights Reserved.
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License. 


import xml.etree.ElementTree as ET
import os, sys, re


def print_usage_exit() :
    print "Usage: check_design_date.py project_file ref_dir";
    exit();

if (len(sys.argv) != 3) :
    print_usage_exit();

project_file = sys.argv[1];    
ref_dir      = sys.argv[2];    

if not os.path.isdir(ref_dir) :
    print "[ERROR] reference directory \""+ref_dir+"\" does not exist.";
    exit();


xml_tree = ET.parse(project_file);

xml_root = xml_tree.getroot();


out_of_date = 0;


ref_time = os.path.getmtime(ref_dir); 

xml_file_set = xml_root.find("source_file_set");
for xml_file in xml_file_set.findall("file") :
    file_time = os.path.getmtime(xml_file.text);
    if file_time > ref_time : 
        out_of_date = 1;
        exit(out_of_date);


exit(out_of_date);



