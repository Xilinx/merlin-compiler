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
import os;
import re;
import xlwt;
import sys;
file_name = sys.argv[1];
print "analize file name " + file_name;
workbook = xlwt.Workbook(encoding = 'ascii');
xls_name = "result.xls";
os.system("rm -rf " + xls_name);
data = "";
count_line = 0;
count_data = 0;
fp = open(file_name)
for line in fp:
    title = re.search(r'(\S+ \S+ \S+) result', line)
    if title:   
        title_name = title.group(1)
        print "generating table for " + title.group(1)
        worksheet = workbook.add_sheet(title_name);
        count_line = 0
    data_array = line.split("|")
    other = re.search(r'---+---',line)
    if not other:
        for data in data_array:
            data_re = re.search(r'\s+(\S+)', data);
            if data_re :
                worksheet.write(count_line, count_data, data_re.group(1));
                count_data=count_data+1
        count_data = 0;
        count_line=count_line+1
        
workbook.save(xls_name);
