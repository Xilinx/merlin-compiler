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
mars_opt  __merlinkernel_fpga_q6.cpp  -e c -p final_code_gen -a impl_tool=sdaccel -a mode=opencl -a naive= -a effort=standard -a volatile_flag= -a explicit_bundle= -a sycl=on -a debug_mode=debug  -I $MERLIN_COMPILER_HOME/mars-gen/lib/merlin 
