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

import re
import getpass
import os
import os.path


usr = getpass.getuser()
print usr
setpath = "/curr/" + usr

oldpath =  os.path.abspath(os.curdir)
os.chdir(setpath)

w = open("fcs_setting64.sh", 'r')
out = open("fcs_setting64.sh_another_sdx", 'w')
lines = w.read()
w.close()
#print lines
if re.search("2017.1", lines):
    print "sdx 2017.1"
    lines1 = lines.replace("2017.1","2016.4")
elif re.search("2016.4", lines):
    print "sdx 2016.4"
    lines1 = lines.replace("2016.4", "2017.1")

out.write(lines1)
out.close()

