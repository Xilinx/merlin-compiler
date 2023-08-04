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
#!/usr/bin/python

import sys
import os
import fnmatch
import re

orig_log_name = ""
log_name = ""

def BackupOriginalLogFile():
    global orig_log_name, log_name

    for file in os.listdir('.') :
        if fnmatch.fnmatch(file, 'report_summary_*.log.orig') :
            orig_log_name = file
    for file in os.listdir('.') :
        if fnmatch.fnmatch(file, 'report_summary_*.log') :
            log_name = file
    if orig_log_name == "" :
        if log_name != "" :
            orig_log_name = log_name + ".orig"
            os.system("cp " + log_name + " " + orig_log_name)
    else :
        log_name = os.path.splitext(orig_log_name)[0]
        os.system("cp " + orig_log_name + " " + log_name)

def MakeFilter():
    global orig_log_name, log_name

    outputList = []
    filter_on = 0
    with open(orig_log_name) as file :
        lineList = file.readlines()
        for line in lineList:
            if re.search('mco_maintain_test Fail', line) != None :
                print ("-" + line)
            elif re.search('test2 Fail', line) != None :
                print ("-" + line)
            elif filter_on == 1 and re.search('Fail:Fail', line) != None :
                print ("-" + line)
            else :
                if re.search('f_x', line) != None or re.search('m_x', line) != None :
                    filter_on =1
                else :
                    filter_on = 0
                outputList.append(line)
    file = open(log_name, "w")
    file.writelines(outputList)
    file.close()

def main():
    BackupOriginalLogFile()
    MakeFilter()

if __name__ == "__main__" :
    main()

