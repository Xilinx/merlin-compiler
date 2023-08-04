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
#source /curr/pengzh/fcs_setting64.csh

export LM_LICENSE_FILE=1900@10.0.0.4

export ALTERA_AOCL_HOME=/curr/software/Altera/16.0_pro
export ALTERA_QUARTUS_HOME=/curr/software/Altera/16.0_pro
export ALTERAOCLSDKROOT=/curr/software/Altera/16.0_pro/hld/
export AOCL_BOARD_PACKAGE_ROOT=/curr/software/Altera/16.0_pro/hld/board/a10_ref_16.0_b211
#export AOCL_BOARD_PACKAGE_ROOT=$ALTERA_QUARTUS_HOME/hld/board/terasic/de5net


export ALTERA_VERSION_PATH=$ALTERA_QUARTUS_HOME
export QUARTUS_ROOTDIR=$ALTERA_VERSION_PATH/quartus
#export LD_LIBRARY_PATH=$QUARTUS_ROOTDIR/linux64:${LD_LIBRARY_PATH}
#export PATH=$ALTERA_VERSION_PATH/quartus/bin/:$PATH
export PATH=$ALTERAOCLSDKROOT/bin/:$PATH
export PATH=$ALTERAOCLSDKROOT/linux64/bin/:$PATH
export LD_LIBRARY_PATH=$ALTERAOCLSDKROOT/linux64/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=$ALTERAOCLSDKROOT/host/lin64/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=$ALTERAOCLSDKROOT/host/linux64/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=$AOCL_BOARD_PACKAGE_ROOT/linux64/lib:${LD_LIBRARY_PATH}
export ACL_PROFILE_TIMER=1

