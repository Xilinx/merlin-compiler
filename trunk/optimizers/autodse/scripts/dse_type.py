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

"""
Python Script in Merlin Compiler

Filename    : optimizers/autodse/scripts/dse_type.py
Description : This is the library for the DSE that defines
              all complex types for type checking.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 02-19-2019 First commit (Cody Hao Yu)
"""
from typing import Dict, Union, Any

CFGType = Dict[str, Union[str, int]]
MetadataType = Dict[str, Union[Dict[str, Any], None, str]]
ModelAccuracyType = Dict[str, Union[Dict[str, int], str]]
