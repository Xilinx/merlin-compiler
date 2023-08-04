//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
// Ensure that non-function-scope occurrences of __PRETTY_FUNCTION__ are
// rejected.

const char *s = __PRETTY_FUNCTION__;

namespace nested {
const char *s = __PRETTY_FUNCTION__;
}

template <typename T> void ff() { const char *s = __PRETTY_FUNCTION__; }

struct S {
    virtual void f() { const char *s = __PRETTY_FUNCTION__; }
};
