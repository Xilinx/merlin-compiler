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
#! /usr/bin/python
# -*- coding:utf-8 -*-
import os

def set_environment():
    # os.environ['XILINX_SDACCEL'] = r'/curr/software/Xilinx/SDx/2016.4';
    sdx = os.environ.get('XILINX_SDX')
    aroot = os.environ.get('ALTERAOCLSDKROOT')
    abpr = os.environ.get('AOCL_BOARD_PACKAGE_ROOT')
    aqh =  os.environ.get('ALTERA_QUARTUS_HOME') 
    if  sdx == "":
	os.environ['XILINX_SDX'] = r'/curr/software/Xilinx/SDx/2017.1'
    if  aroot == "":
	os.environ['ALTERAOCLSDKROOT'] = r'/curr/software/Altera/17.0/hld/';
    if  abpr == "":
	os.environ['AOCL_BOARD_PACKAGE_ROOT'] = r'/curr/software/Altera/17.0/hld/board/a10_ref';
    if  aqh == "":
	os.environ['ALTERA_QUARTUS_HOME'] = r'/curr/software/Altera/17.0';

if __name__ == '__main__':
    set_environment()
