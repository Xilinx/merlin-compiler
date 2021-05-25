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


############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2015 Xilinx Inc. All rights reserved.
############################################################

# Reset the project with the -rset option
open_project -reset cnn_prj
set_top cnn_top

set prj_root ../../

add_files cnn_top.cpp -cflags "-I$prj_root/src/host -I$prj_root/inc -I./"
add_files cnn_top.h -cflags "-I$prj_root/src/host -I$prj_root/inc -I./"

add_files -tb ../host/host.cpp -cflags "-DVIVADO_HLS -I./ -I$prj_root/src/host -I$prj_root/inc -I./"
add_files -tb ../host/cnn.cpp -cflags "-I$prj_root/src/host -I$prj_root/inc -I./"
add_files -tb ../host/c_model.cpp -cflags "-I$prj_root/src/host -I$prj_root/inc -I./"
add_files -tb ../host/util.cpp -cflags "-I$prj_root/src/host -I$prj_root/inc -I./"
#add_files -tb $prj_root/inc/systolic_params.h
#add_files -tb ../../src/cnn.h
#add_files -tb ../../src/util.h
#add_files -tb ../../src/config.h

# Reset the project with the -rset option
open_solution -reset "solution1"
#set_part {xcku060-ffva1156-2-e}
#set_part {xcku115-flvb2104-2-e}
set_part {xcvu9p-flgb2104-2-i}

create_clock -period 10 -name default
#create_clock -period 4 -name default

# Leave the previous directives commented out
#source "./fir_prj/solution1/directives.tcl"

csim_design
csynth_design
#cosim_design
#export_design -format ip_catalog

# Exit vivado HLS
exit
