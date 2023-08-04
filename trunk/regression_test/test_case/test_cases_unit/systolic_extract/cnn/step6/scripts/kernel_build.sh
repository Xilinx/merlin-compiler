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
#!/bin/sh

script_name=$(basename "$0")

if [ $# -ne 2 ] && [ $# -ne 3 ] && [ $# -ne 4 ]
then
  echo "Usage: $script_name <cl_file> <board> [flat] [seed#]"
  exit 1
fi

cl_file=$1
board=$2
shift 2

use_flat=0
use_seed=0

filename="${cl_file%.*}"
output_dir=$filename

if [ $# -ge 1 ] && [ $1 == "flat" ]
then
  use_flat=1
  echo "[Info] $script_name: compiling with Quartus flat flow"
  shift

  output_dir=$output_dir.flat
  export ACL_QSH_COMPILE_CMD="quartus_sh --flow compile top -c flat"
else
  echo "[Info] $script_name: compiling with Quartus PR flow (default)"
fi

if [ $# == 1 ]
then
  use_seed=1
  seed=$1
  echo "[Info] $script_name: compiling with seed $seed"

  output_dir=$output_dir.s$seed
fi 

echo "[Info] $script_name: generating $output_dir.aocx"


# -g is not supported for systolic arrays in OpenCL 15.1.1 patch. This will be fixed in 16.0.
flags="-v --fpc --fp-relaxed --opt-arg -nocaching --report --board $board $PARAMS"

if [ $use_seed == 1 ]
then
  cmd="aoc $flags --seed $seed $cl_file -o $output_dir"
else
  cmd="aoc $flags $cl_file -o $output_dir"
fi

echo $cmd
$cmd

