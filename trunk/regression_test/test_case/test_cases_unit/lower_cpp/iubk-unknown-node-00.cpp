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
// Ensure that CMarsIrV2::is_used_by_kernel doesn't crash on an unknown node.

namespace x {
void f() {}
} // namespace x

// Some nodes that is_used_by_kernel will choke on include SgAsmStmts,
// SgUsingDeclarationStatements. Note that these need to be in the global
// scope to prevent early detection from kernel_separate's checker.
using x::f;

__asm__("rdtsc");

#pragma ACCEL kernel
void kern() {}
