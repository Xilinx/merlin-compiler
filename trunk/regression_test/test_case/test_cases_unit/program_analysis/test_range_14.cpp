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



void func(int M)
{
    int a[100];
    int i;
    unsigned j;
    for (i = 0; i < 100; ++i)
    {
        unsigned index = 100 + 4294967295u *i;
        a[index] = i;
    }
    for (i = 1; i < 100; ++i)
    {
        unsigned index = i + 4294967295u;
        a[index] = i;
    }
    for (j = 100; (int)j >= 0; j+=4294967295u)
    {
        a[j] = j;
    }
    for (j = 100; j < 4294967295u; ++j)
    {
        a[j - 1] = j;
    }
}
