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
mars_opt  __merlinwrapper_test_kernel.c vec_add.c vec_add_kernel.c  -e c -p interface_transform  -a impl_tool=aocl -a effort=standard -a debug_mode=debug     -I/curr/hanhu/test/variable_depth_2/src   -I/curr/hanhu/WORK/Merlin_Compiler/trunk/mars-gen/lib/merlin  -I /curr/hanhu/WORK/Merlin_Compiler/trunk/set_env/../mars-gen/lib/merlin
