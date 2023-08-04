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
import os

if not os.path.isdir("run/implement"):
    print "implement not exit"
    exit()

if not os.path.isfile("run/pkg/kernel_top.aocx") and not os.path.isfile("run/pkg/kernel_top.xclbin"):
    print "bitstream not exit"
    exit()

os.system("merlin_flow run host -keep_device; merlin_flow run test -keep_device")

if not os.path.isfile("run/implement/cmost_flow_has_error"):
    os.system("touch pass.o")
