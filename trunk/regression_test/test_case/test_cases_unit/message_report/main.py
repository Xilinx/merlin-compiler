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
#!/usr/bin/env python3

import contextlib
import os
import pytest
import shutil
import sys

result = "__result__"

def test(path=""):
    assert os.getenv("MERLIN_COMPILER_HOME") is not None
    clean()
    os.mkdir(result)
    source = os.path.join(
        os.getenv("MERLIN_COMPILER_HOME"),
        "mars-gen", "scripts", "msg_report", "xilinx_enhance"
    )

    sys.path.insert(0, ".")
    sys.argv.extend(["-o", "console_output_style=count"])
    sys.argv.append("--cov=%s" % source)
    sys.argv.append("--cov-report=annotate:%s" % result)
    sys.argv.append("--cov-report=term")
    sys.argv.append(path)
    with open("%s/result.txt" % result,
              "w") as f, contextlib.redirect_stdout(f):
        pytest.main()

def clean():
    shutil.rmtree(result, ignore_errors=True)
    shutil.rmtree(".pytest_cache", ignore_errors=True)
    with contextlib.suppress(FileNotFoundError):
        os.remove(".coverage")
