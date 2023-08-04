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
// Reduced test case for bug 1690. In essence, we want to trigger `preprocess`
// to call `func_typedef_spread` on a function whose body contains a nested fn
// decl.

typedef unsigned T;

// Contains a typedef in the type signature, which will cause
// func_typedef_spread to clone this entire definition.
T f() {
    // When f is cloned, so will this nested fn decl. The resulting clone will
    // cause a crash in ROSE's unparser.
    void __merlin_dummy_SgVariableDeclaration_struct_Lit_();
}

// Need pragmas, lest preprocess no-ops.
#pragma ACCEL kernel name = CompressFrangment_kernel
void top(const char *input, unsigned input_size, char *op, unsigned *table,
            const int table_size) {
#pragma ACCEL interface variable = input depth = 134217728
#pragma ACCEL interface variable = op depth = 134217728
#pragma ACCEL interface variable = table depth = 256
}
