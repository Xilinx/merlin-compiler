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
class A {
public:
    int f() { return 123; }
};

A g() {
    return A();
}

int m = g().f();

#pragma ACCEL kernel
void k()
{
    if (g().f()) {
        m;
        g().f();
    }
    else if (g().f()) {
        ;
        g().f();
    }

    switch (1) {
    case 1:
    case 2:
        ;
        g().f();
    default:
        g().f();
    }

    for (int x = g().f(); g().f(); g().f()) {

        for (int x = g().f(); ; ) {
            for (; g().f(); ) {
                for (; ; g().f()) {
                    ;
                    g().f();
                }
            }
        }
    }

    while (g().f()) {
        g().f();
    }

    do {
        g().f();
    } while(g().f());

    {
        g().f();
    }
}
