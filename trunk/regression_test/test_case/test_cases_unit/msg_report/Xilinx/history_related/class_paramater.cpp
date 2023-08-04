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

class Foo
{
public:

    void Process()
    {
        for (int i = 0; i < mLen; ++i) {
            mBuf[i] /= 42 - i;
        }
    }

    void Bigger()
    {
        for (int i = 0; i < mLen; ++i) {
            mBuf[i] *= 3;
        }
    }

    void Smaller()
    {
        for (int i = 0; i < mLen; ++i) {
            mBuf[i] /= 3;
        }
    }

    Foo(int *buf)
        :mBuf(buf)
    {
    }

private:
    int *mBuf;
    static int mLen;
};

int Foo::mLen = BUFSIZE;

void bar(Foo foo, int k) 
{
    if (k % 2 == 0) {
        // foo.Bigger();
    } else {
        // foo.Smaller();
    }
}

#pragma ACCEL kernel
void mykernel(int orig[BUFSIZE], int cpy[BUFSIZE], int j)
{
    Foo foo(orig);
    foo.Process();
    bar(foo, j);
    for (int i = 0; i < BUFSIZE; ++i) {
        cpy[i] = orig[i];
    }
}
