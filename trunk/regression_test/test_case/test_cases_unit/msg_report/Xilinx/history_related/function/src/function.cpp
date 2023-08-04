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
#define BUFSIZE 2048

struct Slice
{
    int *Buf;
    int Len;
};

void multiple(Slice &s, int factor)
{
    static int k = 0;
    for (int i = 0; i < s.Len; ++i) {
        s.Buf[i] *= factor + k++;
    }
}

void multiple(int *buf, int len, int factor)
{
    static int k = 0;
    for (int i = 0; i < len; ++i) {
        buf[i] *= factor + k++;
    }
}

#pragma ACCEL kernel
void mykernel(int orig[BUFSIZE], int cpy[BUFSIZE])
{
    Slice s;
    s.Buf = orig;
    s.Len = BUFSIZE;

    multiple(s, 2);
    multiple(orig, BUFSIZE, 3);
    for (int i = 0; i < BUFSIZE; ++i) {
        cpy[i] = orig[i];
    }
}
