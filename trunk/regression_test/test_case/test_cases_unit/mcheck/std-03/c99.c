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
// In this file, we include a C99-specific variable length array to ensure that
// MCheck does not parse this as C++ (which in turn checks that MCheck isn't
// invoked with -std=c++03 on this file). Additionally, we include a bogus
// bitwidth to trigger a warning to ensure that this file is in fact processed.

void f(unsigned m, unsigned arr[m]) __attribute__((bitwidth(256))) {}
